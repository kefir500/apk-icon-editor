#include "apkpacker.h"
#include "apkunpacker.h"
#include "globals.h"
#include <QApplication>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QtXml/QDomDocument>

using Apk::Packer;

bool Packer::pack(Apk::File *apk, QString temp)
{
    const QString TEMPAPK = temp + "/packed/temp.zip";
    const QString CONTENTS = QDir::fromNativeSeparators(apk->getContentsPath());
    QDir(CONTENTS + "/META-INF").removeRecursively();

    // Save icons:

    emit loading(20, tr("Saving PNG icons..."));
    apk->saveIcons();

    // Clear temporary intermediate files:

    QFile::remove(temp + "/packed/temp.zip");
    QFile::remove(temp + "/packed/temp-signed.apk");
    QFile::remove(temp + "/packed/temp-aligned.apk");

    // Save application name, version and strings:

    emit loading(30, tr("Saving string resources..."));
    apk->saveTitles();

    // Pack APK (Apktool);

    emit loading(40, tr("Packing APK..."));
    if (!zip(CONTENTS, TEMPAPK, temp + "/framework/")) {
        return false;
    }

    // Sing and optimize:

    bool isSigned = false;
    bool isOptimized = false;
    if (apk->getApksigner()) {
        emit loading(60, tr("Optimizing APK..."));
        isOptimized = stepZipalign(apk, TEMPAPK);
        emit loading(80, tr("Signing APK..."));
        isSigned = stepSign(apk, TEMPAPK);
    }
    else {
        emit loading(60, tr("Signing APK..."));
        isSigned = stepSign(apk, TEMPAPK);
        emit loading(80, tr("Optimizing APK..."));
        isOptimized = stepZipalign(apk, TEMPAPK);
    }

    // Move APK to the destination:

    stepFinalize(apk, TEMPAPK);

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

bool Packer::zip(QString contents, QString apk, QString frameworks) const
{
    QProcess p;
    p.start(QString("java -jar \"%1/apktool.jar\" b \"%2\" -f -o \"%3\" -p \"%4\"")
            .arg(Path::Data::shared(),contents, apk, frameworks));
    if (!p.waitForStarted(-1)) {
        if (isJavaInstalled()) {
            qDebug() << "Error starting Apktool";
            qDebug() << "Error:" << p.errorString();
            emit error(Apk::ERRORSTART.arg("Apktool"));
            return false;
        }
        else {
            emit error(Apk::NOJAVA + "<br>" + Apk::GETJAVA);
            return false;
        }
    }
    p.waitForFinished(-1);
    if (p.exitCode()) {
        qDebug() << p.readAllStandardError().replace("\r\n", "\n");
        emit error(Apk::ERROR.arg("Apktool"));
        return false;
    }
    return true;
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
        dom.firstChildElement("manifest")
           .firstChildElement("application")
           .setAttribute("android:label", title);

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

bool Packer::sign(QString apk, QString pem, QString pk8, bool apksigner) const
{
    if (!QFile::exists(pem) || !QFile::exists(pk8)) {
        qDebug() << "Warning: PEM/PK8 not found.";
        return false;
    }

    QProcess p;
    const QString apk_dst = apk + ".signed"; // Separate output file for signapk.jar
    const QString procString = apksigner
        ? "java -jar \"%1/signer/apksigner.jar\" sign --key \"%3\" --cert \"%2\" \"%4\""
        : "java -jar \"%1/signer/signapk.jar\" \"%2\" \"%3\" \"%4\" \"%5\"";
    p.start(procString.arg(Path::Data::shared(), pem, pk8, apk, apk_dst));

    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        if (p.exitCode() == 0) {
            if (!apksigner) {
                QFile::remove(apk);
                QFile::rename(apk_dst, apk);
            }
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

    QFile::remove(apk_dst);
    return false;
}

bool Packer::sign(QString apk, QString keystore, QString alias, QString passKeystore, QString passAlias, bool apksigner) const
{
    if (!QFile::exists(keystore)) {
        qDebug() << "Warning: KeyStore not found.";
        return false;
    }

    QProcess p;
    QString procString = apksigner
        ? "java -jar \"%1/signer/apksigner.jar\" sign --ks \"%2\" --ks-key-alias \"%3\" --ks-pass pass:\"%4\" --key-pass pass:\"%5\" \"%6\""
        : "jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore \"%2\" \"%6\" -storepass \"%4\" -keypass \"%5\" \"%3\"";
    p.start(procString.arg(Path::Data::shared(), keystore, alias, passKeystore, passAlias, apk));

    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        if (p.exitCode() == 0) {
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
        qDebug() << qPrintable("Error starting Java");
        qDebug() << "Error:" << p.errorString();
    }

    // Something went wrong:

    return false;
}

bool Packer::zipalign(QString apk) const
{
    const QString apk_dst(apk + ".aligned");

    QProcess p;
    p.start(QString("zipalign -f 4 \"%1\" \"%2\"").arg(apk, apk_dst));

    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        if (p.exitCode() == 0) {
            QFile::remove(apk);
            QFile::rename(apk_dst, apk);
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

    QFile::remove(apk_dst);
    return false;
}

bool Packer::stepSign(Apk::File *apk, QString tempAPK) const
{
    if (apk->getSign()) {
        qDebug() << "Signing...";
        return apk->getKeystore()
            ? sign(tempAPK, apk->getFileKeystore(), apk->getAlias(), apk->getPassKeystore(), apk->getPassAlias(), apk->getApksigner())
            : sign(tempAPK, apk->getFilePem(), apk->getFilePk8(), apk->getApksigner());
    }
    // No signing needed:
    return true;
}

bool Packer::stepZipalign(Apk::File *apk, QString tempAPK) const
{
    if (apk->getZipalign()) {
        qDebug() << "Optimizing...";
        return zipalign(tempAPK);
    }
    // No alignment needed:
    return true;
}

bool Packer::stepFinalize(Apk::File *apk, QString tempAPK) const
{
    QString output = apk->getFilePath();
    QFileInfo fi(apk->getFilePath());
    QString suffix = fi.suffix();
    if (suffix.toLower() != "apk") {
        output += ".apk";
    }
    QFile::remove(output);
    if (!QFile::rename(tempAPK, output)) {
        emit error(tr("Could not create output APK file."));
        return false;
    }
    return true;
}
