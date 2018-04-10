#ifndef APKPACKER_H
#define APKPACKER_H

#include "apkfile.h"

namespace Apk {

    class Packer : public QObject
    {
        Q_OBJECT

    public:
        explicit Packer(QObject *parent = 0) : QObject(parent) {}

        bool pack(Apk::File *apk, QString temp);

        void saveAppTitle(QString contents, QString title) const;
        void saveAppVersion(QString contents, QString code, QString name) const;
        bool saveStrings(QList<Apk::String> strings) const;

        bool zip(QString contents, QString apk, QString frameworks) const;
        bool sign(QString apk, QString pem, QString pk8, bool apksigner) const;
        bool sign(QString apk, QString keystore, QString alias, QString passKeystore, QString passAlias, bool apksigner) const;
        bool zipalign(QString apk) const;

    private:
        bool stepSign(Apk::File *apk, QString tempAPK) const;
        bool stepZipalign(Apk::File *apk, QString tempAPK) const;
        bool stepFinalize(Apk::File *apk, QString tempAPK) const;

    signals:
        void packed(Apk::File *apk, QString message, bool success = true) const;
        void loading(short percentage, QString text = QString()) const;
        void error(QString message) const;
    };
}

#endif // APKPACKER_H

