#ifndef APKPACKER_H
#define APKPACKER_H

#include "apkfile.h"
#include <QProcess>

namespace Apk {

    class Packer : public QObject
    {
        Q_OBJECT

    public:
        explicit Packer(QObject *parent = 0);

        void pack(Apk::File *apk, QString temp);
        void cancel();

        void saveAppTitle(QString contents, QString title) const;
        void saveAppVersion(QString contents, QString code, QString name) const;
        bool saveStrings(QList<Apk::String> strings) const;

        void sign(Apk::File *apk, QString apkPath);
        void zipalign(Apk::File *apk, QString apkPath);

    private:
        void signWithPem(Apk::File *apk, QString apkPath);
        void signWithKeystore(Apk::File *apk, QString apkPath);
        void finalize(Apk::File *apk, QString apkPath) const;

        QProcess *apktool;
        QProcess *zipaligner;
        QProcess *signer;

        QString signError;
        QString alignError;

    signals:
        void packed(Apk::File *apk, bool success, QString message, QString details = QString()) const;
        void loading(short percentage, QString text = QString()) const;
        void error(QString message, QString details = QString()) const;
    };
}

#endif // APKPACKER_H

