#include "apkunpacker.h"
#include "manifest.h"
#include "globals.h"
#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QDomDocument>
#include <QDebug>

using Apk::Unpacker;

bool Unpacker::unpack(QString filepath, QString destination, QString frameworks, bool smali)
{
    destination = QDir::fromNativeSeparators(destination);

    // Clear temporary directory;

    emit loading(20, tr("Unpacking APK..."));
    QDir(destination).removeRecursively();

    // Unpack APK;

    emit loading(50, tr("Unpacking APK..."));
    QProcess apktool;
    QStringList args;
    args << "-jar"
         << QString("%1/apktool.jar").arg(Path::Data::shared())
         << QString("d") << filepath
         << "-f"
         << (smali ? "" : "-s")
         << "-o" << destination
         << "-p" << frameworks;
    apktool.start("java", args);
    if (!apktool.waitForStarted(-1)) {
        if (isJavaInstalled()) {
            qDebug() << "Error starting Apktool";
            qDebug() << "Error:" << apktool.errorString();
            emit error(Apk::ERRORSTART.arg("Apktool"));
            return false;
        } else {
            emit error(Apk::NOJAVA + "<br>" + Apk::GETJAVA);
            return false;
        }
    }
    apktool.waitForFinished(-1);
    if (apktool.exitCode()) {
        qDebug() << apktool.readAllStandardError().replace("\r\n", "\n");
        emit error(Apk::ERROR.arg("Apktool"));
        return false;
    }

    // Read APK manifest:

    emit loading(70, tr("Reading AndroidManifest.xml..."));
    Apk::File *apk = new Apk::File(destination);
    apk->setFilePath(filepath);

    // Done!

    qDebug() << "Done.\n";
    emit loading(100, tr("APK successfully loaded"));
    emit unpacked(apk);
    return true;
}
