#include "apkpacker.h"
#include "apkunpacker.h"
#include "globals.h"
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QtXml/QDomDocument>

using Apk::Packer;

Packer::Packer(QObject *parent) : QObject(parent)
{
    apktool = new QProcess(this);
    zipaligner = new QProcess(this);
    signer = new QProcess(this);
}

void Packer::pack(Apk::File *apk, QString temp)
{
    apktool->disconnect();
    zipaligner->disconnect();
    signer->disconnect();

    signError.clear();
    alignError.clear();

    const QString APKTOOL = QDir::fromNativeSeparators(apk->getApktool());
    const QString TEMPAPK = temp + "/packed/temp.zip";
    const QString CONTENTS = QDir::fromNativeSeparators(apk->getContentsPath());
    QDir(CONTENTS + "/META-INF").removeRecursively();

    // Save icons:

    emit loading(20, tr("Saving PNG icons..."));
    apk->saveIcons();

    // Clear temporary intermediate files:

    QFile::remove(temp + "/packed/temp.zip");
    QFile::remove(temp + "/packed/temp.zip.signed");
    QFile::remove(temp + "/packed/temp.zip.aligned");

    // Save application name, version and strings:

    emit loading(30, tr("Saving string resources..."));
    apk->saveTitles();

    // Pack APK (Apktool):

    emit loading(40, tr("Packing APK..."));

    connect(apktool, static_cast<void(QProcess::*)(int)>(&QProcess::finished), [=](int code) {
        const int QPROCESS_KILL_CODE = 62097;
        switch (code) {
            case 0: {
                apk->getApksigner() ? zipalign(apk, TEMPAPK) : sign(apk, TEMPAPK);
                break;
            }
#ifdef Q_OS_OSX
            case 9:
#endif
            case QPROCESS_KILL_CODE:
                qDebug() << "Packing cancelled by user.";
                return;
            default: {
                const QString errorText = apktool->readAllStandardError().replace("\r\n", "\n");
                qDebug() << errorText;
                emit error(Apk::ERROR.arg("Apktool"), errorText);
                break;
            }
        }
    });

    connect(apktool, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error), [=](QProcess::ProcessError processError) {
        if (processError == QProcess::FailedToStart) {
            if (isJavaInstalled()) {
                const QString errorText = apktool->errorString();
                qDebug() << "Error starting Apktool";
                qDebug() << "Error:" << errorText;
                emit error(Apk::ERRORSTART.arg("Apktool"), errorText);
            } else {
                emit error(Apk::NOJAVA + "<br>" + Apk::GETJAVA);
            }
        }
    });

    apktool->start(QString("java -jar \"%1\" b \"%2\" -f -o \"%3\" -p \"%4\"").arg(APKTOOL, CONTENTS, TEMPAPK, temp + "/framework/"));
}

void Packer::cancel()
{
    apktool->kill();
    signer->kill();
    zipaligner->kill();
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

void Packer::sign(Apk::File *apk, QString apkPath)
{
    if (!apk->getSign()) {
        apk->getApksigner() ? finalize(apk, apkPath) : zipalign(apk, apkPath);
        return;
    }

    qDebug() << "Signing...";
    emit loading(apk->getApksigner() ? 80 : 60, tr("Signing APK..."));

    apk->getKeystore()
        ? signWithKeystore(apk, apkPath)
        : signWithPem(apk, apkPath);
}

void Packer::signWithPem(Apk::File *apk, QString apkPath)
{
    const QString pem = apk->getFilePem();
    const QString pk8 = apk->getFilePk8();
    const bool isApksigner = apk->getApksigner();
    const QString apkDest = apkPath + ".signed"; // Separate output file for signapk.jar

    connect(signer, static_cast<void(QProcess::*)(int)>(&QProcess::finished), [=](int code) {
        const int QPROCESS_KILL_CODE = 62097;
        switch (code) {
            case 0: {
                if (!isApksigner) {
                    QFile::remove(apkPath);
                    QFile::rename(apkDest, apkPath);
                }
                break;
            }
#ifdef Q_OS_OSX
            case 9:
#endif
            case QPROCESS_KILL_CODE:
                qDebug() << "Signing cancelled by user.";
                return;
            default: {
                signError = signer->readAllStandardError().replace("\r\n", "\n");
                if (signError.isEmpty()) signError = signer->readAllStandardOutput().replace("\r\n", "\n");
                qDebug() << "Signer exit code:" << signer->exitCode();
                qDebug() << signError;
                break;
            }
        }
        isApksigner ? finalize(apk, apkPath) : zipalign(apk, apkPath);
    });

    connect(signer, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error), [=](QProcess::ProcessError processError) {
        if (processError == QProcess::FailedToStart) {
            signError = signer->errorString();
            qDebug() << "Error starting signer";
            qDebug() << "Error:" << signError;
            QFile::remove(apkDest);
            isApksigner ? finalize(apk, apkPath) : zipalign(apk, apkPath);
        }
    });

    const QString procString = isApksigner
        ? "java -jar \"%1/signer/apksigner.jar\" sign --key \"%3\" --cert \"%2\" \"%4\""
        : "java -jar \"%1/signer/signapk.jar\" \"%2\" \"%3\" \"%4\" \"%5\"";
    signer->start(procString.arg(Path::Data::shared(), pem, pk8, apkPath, apkDest));
}

void Packer::signWithKeystore(Apk::File *apk, QString apkPath)
{
    const QString keystore = apk->getFileKeystore();
    const QString alias = apk->getAlias();
    const QString passKeystore = apk->getPassKeystore();
    const QString passAlias = apk->getPassAlias();
    const bool isApksigner = apk->getApksigner();

    connect(signer, static_cast<void(QProcess::*)(int)>(&QProcess::finished), [=](int code) {
        const int QPROCESS_KILL_CODE = 62097;
        switch (code) {
            case 0: {
                break;
            }
#ifdef Q_OS_OSX
            case 9:
#endif
            case QPROCESS_KILL_CODE:
                qDebug() << "Signing cancelled by user.";
                return;
            default: {
                signError = signer->readAllStandardError().replace("\r\n", "\n");
                if (signError.isEmpty()) signError = signer->readAllStandardOutput().replace("\r\n", "\n");
                qDebug() << "Signer exit code:" << signer->exitCode();
                qDebug() << signError;
                break;
            }
        }
        isApksigner ? finalize(apk, apkPath) : zipalign(apk, apkPath);
    });

    connect(signer, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error), [=](QProcess::ProcessError processError) {
        if (processError == QProcess::FailedToStart) {
            signError = signer->errorString();
            qDebug() << "Error starting signer";
            qDebug() << "Error:" << signError;
            isApksigner ? finalize(apk, apkPath) : zipalign(apk, apkPath);
        }
    });

    qDebug() << "Packing...";
    QString procString = isApksigner
        ? "java -jar \"%6/signer/apksigner.jar\" sign --ks \"%1\" --ks-key-alias \"%2\" --ks-pass pass:\"%3\" --key-pass pass:\"%4\" \"%5\""
        : "jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore \"%1\" \"%5\" -storepass \"%3\" -keypass \"%4\" \"%2\"";
    signer->start(procString.arg(keystore, alias, passKeystore, passAlias, apkPath, Path::Data::shared()));
}

void Packer::zipalign(Apk::File *apk, QString apkPath)
{
    if (!apk->getZipalign()) {
        apk->getApksigner() ? sign(apk, apkPath) : finalize(apk, apkPath);
        return;
    }

    qDebug() << "Optimizing...";
    emit loading(apk->getApksigner() ? 60 : 80, tr("Optimizing APK..."));

    const QString apkDest(apkPath + ".aligned");

    connect(zipaligner, static_cast<void(QProcess::*)(int)>(&QProcess::finished), [=](int code) {
        const int QPROCESS_KILL_CODE = 62097;
        switch (code) {
            case 0: {
                QFile::remove(apkPath);
                QFile::rename(apkDest, apkPath);
                break;
            }
#ifdef Q_OS_OSX
            case 9:
#endif
            case QPROCESS_KILL_CODE:
                qDebug() << "Alignment cancelled by user.";
                QFile::remove(apkDest);
                return;
            default: {
                alignError = zipaligner->readAllStandardError().replace("\r\n", "\n");
                qDebug() << "Zipalign exit code:" << zipaligner->exitCode();
                qDebug() << alignError;
                QFile::remove(apkDest);
                break;
            }
        }
        apk->getApksigner() ? sign(apk, apkPath) : finalize(apk, apkPath);
    });

    connect(zipaligner, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error), [=](QProcess::ProcessError processError) {
        if (processError == QProcess::FailedToStart) {
            alignError = zipaligner->errorString();
            qDebug() << "Error starting Zipalign";
            qDebug() << "Error:" << alignError;
            apk->getApksigner() ? sign(apk, apkPath) : finalize(apk, apkPath);
        }
    });

    zipaligner->start(QString("zipalign -f 4 \"%1\" \"%2\"").arg(apkPath, apkDest));
}

void Packer::finalize(Apk::File *apk, QString apkPath) const
{
    // Move APK to destination:

    QString output = apk->getFilePath();
    QFileInfo fi(apk->getFilePath());
    QString suffix = fi.suffix();
    if (suffix.toLower() != "apk") {
        output += ".apk";
    }
    QFile::remove(output);
    if (!QFile::rename(apkPath, output)) {
        emit error(tr("Could not create output APK file."));
        return;
    }

    // Done:

    emit loading(100, tr("APK successfully packed!"));
    qDebug() << "Done.\n";

    if (signError.isNull() && alignError.isNull()) {
        emit packed(apk, true, tr("APK successfully packed!"));
    } else {
        QString brief(tr("APK packed with following warnings:"));
        QString descriptive;
        if (!signError.isNull()) {
            brief += "<br>&bull; " + tr("APK is <b>not signed</b>;");
            descriptive += "----- APK not signed -----\n";
            descriptive += signError + "\n\n";
        }
        if (!alignError.isNull()) {
            brief += "<br>&bull; " + tr("APK is <b>not optimized</b>;");
            descriptive += "----- APK not optimized -----\n";
            descriptive += alignError + "\n\n";
        }
        if (!isJavaInstalled()) {
            brief += "<hr>" + tr("Signing APK requires Java Runtime Environment.") + "<br>" + Apk::GETJAVA;
        }
        emit packed(apk, false, brief, descriptive);
    }
}
