#ifndef APKMANAGER_H
#define APKMANAGER_H

#include "apkpacker.h"
#include "apkunpacker.h"

class ApkManager : public QObject
{
    Q_OBJECT

public:
    explicit ApkManager(QObject *parent = 0);
    ~ApkManager();

public slots:
    void unpack(QString filename, QString temp, bool apktool, bool smali);
    void pack(Apk::File *apk, QString temp);

signals:
    void unpacked(Apk::File *apk) const;
    void packed(Apk::File *apk, QString message, bool success = true) const;
    void loading(short percentage, QString text = QString()) const;
    void error(QString title, QString text);

private slots:
    void catchApk(Apk::File *apk);
    void catchError(QString message);

private:
    Apk::Packer *packer;
    Apk::Unpacker *unpacker;
    Apk::File *apk;
    QList<Apk::File*> apks; // For future needs
};

#endif // APKMANAGER_H
