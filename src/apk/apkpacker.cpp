#include "apkpacker.h"
#include "apkunpacker.h"
#include "globals.h"
#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QtXml/QDomDocument>
#include <QuaZIP/JlCompress.h>

using Apk::Packer;

bool Packer::pack(Apk::File *apk, QString temp)
{
    const QString CONTENTS = QDir::fromNativeSeparators(apk->getDirectory());
    QDir(CONTENTS + "/META-INF").removeRecursively();

    // Save icons:

    emit loading(20, tr("Saving PNG icons..."));
    if (!saveIcons(apk->getIcons())) {
        return false;
    }

    // Clear temporary intermediate files:

    QFile::remove(temp + "/packed/temp.zip");
    QFile::remove(temp + "/packed/temp-1.apk");
    QFile::remove(temp + "/packed/temp-2.apk");
    QFile::remove(temp + "/packed/temp-3.apk");

    // Pack APK:

    if (!apk->getApktool()) {

        // Pack APK (ZIP):

        emit loading(40, tr("Packing APK..."));
        if (!zip(CONTENTS, temp, apk->getRatio())) {
            return false;
        }
    }
    else {

        // Save application name, version and strings:

        emit loading(30, tr("Saving string resources..."));

        if (apk->getVarAppTitle().isEmpty()) {
            saveAppTitle(CONTENTS, apk->getAppTitle());
        }
        saveAppVersion(CONTENTS, apk->getVersionCode(), apk->getVersionName());
        saveStrings(apk->getStrings());

        // Pack APK (Apktool);

        emit loading(40, tr("Packing APK..."));
        if (!zip(CONTENTS, temp, temp + "/framework/")) {
            return false;
        }
    }

    // Sign APK:

    bool isSigned = false;
    if (apk->getSign()) {
        qDebug() << "Signing...";
        emit loading(60, tr("Signing APK..."));

        isSigned = apk->getKeystore()
        ? sign(temp, apk->getFileKeystore(), apk->getAlias(), apk->getPassKeystore(), apk->getPassAlias())
        : isSigned = sign(temp, apk->getFilePem(), apk->getFilePk8());
    }
    else {
        isSigned = true;
        QFile::rename(temp + "/packed/temp-1.apk", temp + "/packed/temp-2.apk");
    }

    // Zipalign APK:

    bool isOptimized = false;
    if (apk->getZipalign()) {
        qDebug() << "Optimizing...";
        emit loading(80, tr("Optimizing APK..."));
        isOptimized = zipalign(temp);
    }
    else {
        isOptimized = true;
        QFile::rename(temp + "/packed/temp-2.apk", temp + "/packed/temp-3.apk");
    }

    // Move APK to the destination:

    QString output = apk->getFilePath();
    QFileInfo fi(apk->getFilePath());
    QString suffix = fi.suffix();
    if (suffix.toLower() != "apk") {
        output += ".apk";
    }
    QFile::remove(output);
    if (!QFile::rename(temp + "/packed/temp-3.apk", output)) {
        emit error(tr("Could not create output APK file."));
        return false;
    }

    // Done!

    emit loading(100, tr("APK successfully packed!"));
    qDebug() << "Done.\n";

    if (isSigned && isOptimized) {
        emit packed(apk, tr("APK successfully packed!"));
    }
    else {
        QString warning(tr("APK packed with following warnings:"));
        if (!isSigned) {
            warning += "<br>&bull; " + tr("APK is <b>not signed</b>;");
        }
        if (!isOptimized) {
            warning += "<br>&bull; " + tr("APK is <b>not optimized</b>;");
        }
        if (!isJavaInstalled()) {
            warning += "<hr>" + tr("Signing APK requires Java Runtime Environment.") + "<br>" + Apk::GETJAVA;
        }
        emit packed(apk, warning, false);
    }
    return true;
}

bool Packer::zip(QString contents, QString temp, short ratio) const
{
    bool result = JlCompress::compressDir(temp + "/packed/temp.zip", contents, ratio);
    if (result) {
        QFile::rename(temp + "/packed/temp.zip", temp + "/packed/temp-1.apk");
        return true;
    }
    else {
        emit error(tr("%1 Error").arg("QuaZIP"));
        return false;
    }
}

bool Packer::zip(QString contents, QString temp, QString frameworks) const
{
    QProcess p;
    p.start(QString("java -jar \"%1/apktool.jar\" b \"%2\" -f -o \"%3/packed/temp.zip\" -p \"%4\"")
            .arg(Path::App::dir(),contents, temp, frameworks));
    if (!p.waitForStarted(-1)) {
        if (isJavaInstalled()) {
            qDebug() << "Error starting Apktool";
            qDebug() << "Error:" << p.errorString();
            emit error(tr("Error starting <b>%1</b>").arg("Apktool"));
            return false;
        }
        else {
            emit error(tr("\"Apktool\" requires Java Runtime Environment.") + "<br>" + Apk::GETJAVA);
            return false;
        }
    }
    p.waitForFinished(-1);
    if (!p.exitCode()) {
        QFile::rename(temp + "/packed/temp.zip", temp + "/packed/temp-1.apk");
        return true;
    }
    else {
        qDebug() << p.readAllStandardError().replace("\r\n", "\n");
        emit error(tr("%1 Error").arg("Apktool"));
        return false;
    }
}

void Packer::saveAppTitle(QString contents, QString title) const
{
    QFile f(contents + "/AndroidManifest.xml");
    if (f.open(QFile::ReadWrite | QFile::Text)) {
        QTextStream in(&f);
        QString xml = in.readAll();
        f.resize(0);

        QDomDocument dom;
        dom.setContent(xml);

        QDomNodeList list = dom.elementsByTagName("application");
        if (!list.isEmpty()) {
            QDomNamedNodeMap attr = list.at(0).attributes();
            attr.namedItem("android:label").setNodeValue(title);
        }

        QTextStream out(&f);
        dom.save(out, 4);
        f.close();
    }
}

void Packer::saveAppVersion(QString contents, QString code, QString name) const
{
    QFile f(contents + "/apktool.yml");
    if (f.open(QFile::ReadWrite | QFile::Text)) {
        QString yml = f.readAll();
        QRegExp rxCode, rxName;
        rxCode.setPattern("versionCode: [^\n]+");
        rxName.setPattern("versionName: [^\n]+");
        yml.replace(rxCode, QString("versionCode: '%1'").arg(code));
        yml.replace(rxName, QString("versionName: %1").arg(QString("'%1'").arg(name)));
        f.resize(0);
        QTextStream out(&f);
        out << yml;
        f.close();
    }
}

bool Packer::saveIcons(QList<QSharedPointer<Icon> > icons) const
{
    for (int i = 0; i < icons.size(); ++i) {
        if (!icons[i]->save()) {
            emit error("Error saving PNG icon.");
            return false;
        }
    }
    return true;
}

bool Packer::saveStrings(QList<Apk::String> strings) const
{
    bool result = true;

    for (int i = 0; i < strings.size(); ++i) {

        const QString KEY = strings[i].getKey();
        const QString VALUE = strings[i].getValue();
        const QString FILENAME = strings[i].getFilename();

        QFileInfo fi(FILENAME);
        qDebug() << qPrintable(QString("String in '%1/%2': %3 = '%4'")
                               .arg(fi.dir().dirName(),
                                    fi.fileName(),
                                    strings[i].getKey(),
                                    strings[i].getValue()));

        QFile f(FILENAME);
        if (f.open(QFile::ReadWrite | QFile::Text)) {
            const QString LINE = QString("<string name=\"%1\">%2</string>").arg(KEY);
            QTextStream in(&f);
            QString xml = in.readAll();
            QRegExp rx;
            rx.setMinimal(true);
            rx.setPattern(LINE.arg(".*"));
            xml.replace(rx, QString(LINE).arg(VALUE));
            f.resize(0);
            QTextStream out(&f);
            out << xml;
            f.close();
        }
        else {
            result = false;
        }
    }

    return result;
}

bool Packer::sign(QString temp, QString pem, QString pk8) const
{
    const QString APK_SRC(temp + "/packed/temp-1.apk");
    const QString APK_DST(temp + "/packed/temp-2.apk");
    const QString SIGNAPK(Path::App::dir() + "/signer/");

    if (!QFile::exists(pem) || !QFile::exists(pk8)) {
        qDebug() << "Warning: PEM/PK8 not found.";
        QFile::rename(APK_SRC, APK_DST);
        return false;
    }

    QProcess p;
    p.start(QString("java -jar \"%1signapk.jar\" \"%2\" \"%3\" \"%4\" \"%5\"")
            .arg(SIGNAPK, pem, pk8, APK_SRC, APK_DST));

    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        if (p.exitCode() == 0) {
            QFile::remove(APK_SRC);
            return true;
        }
        else {
            QString error_text = p.readAllStandardError().replace("\r\n", "\n");
            if (error_text.isEmpty()) error_text = p.readAllStandardOutput().replace("\r\n", "\n");
            qDebug() << "Java exit code:" << p.exitCode();
            qDebug() << error_text;
        }
    }
    else {
        qDebug() << "Error starting Java";
        qDebug() << "Error:" << p.errorString();
    }

    // Something went wrong:

    QFile::rename(APK_SRC, APK_DST);
    return false;
}

bool Packer::sign(QString temp, QString keystore, QString alias, QString passKeystore, QString passAlias) const
{
    const QString APK_SRC(temp + "/packed/temp-1.apk");
    const QString APK_DST(temp + "/packed/temp-2.apk");

    if (!QFile::exists(keystore)) {
        qDebug() << "Warning: KeyStore not found.";
        QFile::rename(APK_SRC, APK_DST);
        return false;
    }

    QProcess p;
    const QString ENV_PATH = qgetenv("PATH");
    const QString JAVA_HOME = qgetenv("JAVA_HOME");
    qputenv("PATH", QString("%1;%2/bin").arg(ENV_PATH, JAVA_HOME).toStdString().c_str());
    p.start(QString("jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 "
            "-keystore \"%1\" \"%2\" -storepass \"%3\" -keypass \"%4\" \"%5\"")
            .arg(keystore, APK_SRC, passKeystore, passAlias, alias));
    qputenv("PATH", ENV_PATH.toStdString().c_str());

    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        if (p.exitCode() == 0) {
            QFile::rename(APK_SRC, APK_DST);
            return true;
        }
        else {
            QString error_text = p.readAllStandardError().replace("\r\n", "\n");
            if (error_text.isEmpty()) error_text = p.readAllStandardOutput().replace("\r\n", "\n");
            qDebug() << "Jarsigner exit code:" << p.exitCode();
            qDebug() << error_text;
        }
    }
    else {
        qDebug() << qPrintable("Error starting Jarsigner");
    }

    // Something went wrong:

    QFile::rename(APK_SRC, APK_DST);
    return false;
}

bool Packer::zipalign(QString temp) const
{
    const QString APK_SRC(temp + "/packed/temp-2.apk");
    const QString APK_DST(temp + "/packed/temp-3.apk");

    QProcess p;
    p.start(QString("\"%1/zipalign\" -f 4 \"%2\" \"%3\"").arg(Path::App::dir(), APK_SRC, APK_DST));

    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        if (p.exitCode() == 0) {
            QFile::remove(APK_SRC);
            return true;
        }
        else {
            qDebug() << "Zipalign exit code:" << p.exitCode();
            qDebug() << p.readAllStandardError().replace("\r\n", "\n");
        }
    }
    else {
        qDebug() << "Error starting Zipalign";
        qDebug() << "Error:" << p.errorString();
    }

    // Something went wrong:

    QFile::rename(APK_SRC, APK_DST);
    return false;
}
