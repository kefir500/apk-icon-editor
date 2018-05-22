#include "apkmanager.h"
#include <QFile>
#include <QDebug>

ApkManager::ApkManager(QObject *parent) : QObject(parent)
{
    apk = NULL;

    unpacker = new Apk::Unpacker(this);
    connect(unpacker, SIGNAL(error(QString, QString)), this, SLOT(catchError(QString, QString)));
    connect(unpacker, SIGNAL(loading(short, QString)), this, SIGNAL(loading(short, QString)));
    connect(unpacker, SIGNAL(unpacked(Apk::File*)), this, SIGNAL(unpacked(Apk::File*)));
    connect(unpacker, SIGNAL(unpacked(Apk::File*)), this, SLOT(catchApk(Apk::File*)));

    packer = new Apk::Packer(this);
    connect(packer, SIGNAL(error(QString, QString)), this, SLOT(catchError(QString, QString)));
    connect(packer, SIGNAL(loading(short, QString)), this, SIGNAL(loading(short, QString)));
    connect(packer, SIGNAL(packed(Apk::File*, bool, QString, QString)), this, SIGNAL(packed(Apk::File*, bool, QString, QString)));
}

void ApkManager::unpack(QString filename, QString temp, QString apktool, bool smali)
{
    qDebug() << "Unpacking" << filename;
    qDebug() << "Output directory:" << temp;
    unpacker->unpack(filename,
                     temp + "/apk/",
                     apktool,
                     temp + "/framework",
                     smali);
}

void ApkManager::pack(Apk::File *apk, QString temp)
{
    qDebug() << "\n--- Packing APK ---";
    qDebug() << "Output file:" << apk->getFilePath();
    qDebug() << "Contents directory:" << apk->getContentsPath();

    qDebug() << "Sign:" << apk->getSign();
    qDebug() << "Zipalign:" << apk->getZipalign();
    qDebug() << "Using Apksigner:" << apk->getApksigner();
    qDebug() << "Using KeyStore:" << apk->getKeystore();
    if (!apk->getKeystore()) {
        qDebug() << "PEM" << (QFile::exists(apk->getFilePem()) ? "found;" : "NOT found;");
        qDebug() << "PK8" << (QFile::exists(apk->getFilePk8()) ? "found;" : "NOT found;");
    }
    else {
        qDebug() << "KeyStore" << (QFile::exists(apk->getFileKeystore()) ? "found;" : "NOT found;");
        qDebug() << "KeyStore Password:" << (!apk->getPassKeystore().isEmpty() ? "present;" : "NOT present;");
        qDebug() << "Alias:" << apk->getAlias();
        qDebug() << "Alias Password:" << (!apk->getPassAlias().isEmpty() ? "present;" : "NOT present;");
    }

    packer->pack(apk, temp);
}

void ApkManager::cancel()
{
    unpacker->cancel();
    packer->cancel();
}

void ApkManager::close()
{
    if (apk) {
        delete apk;
        apk = NULL;
    }
}

void ApkManager::catchApk(Apk::File *apk)
{
    close();
    this->apk = apk;
}

void ApkManager::catchError(QString message, QString details)
{
    emit error(QString(), message, details);
}

ApkManager::~ApkManager()
{
    if (apk) {
        apk->removeFiles();
    }
}
