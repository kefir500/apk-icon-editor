#include "apkunpacker.h"
#include "manifest.h"
#include "globals.h"
#include <QApplication>
#include <QDir>
#include <QDomDocument>
#include <QDebug>

using Apk::Unpacker;

Unpacker::Unpacker(QObject *parent) : QObject(parent)
{
    apktool = new QProcess(this);
    apktool->kill();
}

void Unpacker::unpack(QString filepath, QString destination, QString apktoolPath, QString frameworks, bool smali)
{
    destination = QDir::fromNativeSeparators(destination);
    apktool->disconnect();

    // Clear temporary directory;

//    emit loading(20, tr("Unpacking APK..."));
//    QDir(destination).removeRecursively();

    // Unpack APK:

    emit loading(50, tr("Unpacking APK..."));
    QStringList args;
    args << "-jar";
    args << apktoolPath;
    args << "d" << filepath;
    args << "-f";
    if (!smali) { args << "-s"; }
    args << "-o" << destination;
    args << "-p" << frameworks;

    connect(apktool, static_cast<void(QProcess::*)(int)>(&QProcess::finished), [=](int code) {
        const int QPROCESS_KILL_CODE = 62097;
        switch (code) {
            case 0: {
                emit loading(70, tr("Reading AndroidManifest.xml..."));
                Apk::File *apk = new Apk::File(destination);
                apk->setFilePath(filepath);
                qDebug() << "Done.\n";
                emit loading(100, tr("APK successfully loaded"));
                emit unpacked(apk);
                break;
            }
#ifdef Q_OS_OSX
            case 9:
#endif
            case QPROCESS_KILL_CODE:
                qDebug() << "Unpacking cancelled by user.";
                break;
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

    apktool->start("java", args);
}

void Unpacker::cancel()
{
    apktool->kill();
}
