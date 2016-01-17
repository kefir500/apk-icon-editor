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
        bool saveIcons(QList<QSharedPointer<Icon> > icons) const;
        bool saveStrings(QList<Apk::String> strings) const;

        bool zip(QString contents, QString temp, short ratio) const;
        bool zip(QString contents, QString temp, QString frameworks) const;
        bool sign(QString temp, QString pem, QString pk8) const;
        bool sign(QString temp, QString keystore, QString alias, QString passKeystore, QString passAlias) const;
        bool zipalign(QString temp) const;

    public slots:

    signals:
        void packed(Apk::File *apk, QString message, bool success = true) const;
        void loading(short percentage, QString text = QString()) const;
        void error(QString message) const;
    };
}

#endif // APKPACKER_H

