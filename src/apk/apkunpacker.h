#ifndef APKUNPACKER_H
#define APKUNPACKER_H

#include "apkfile.h"

namespace Apk {

    class Unpacker : public QObject
    {
        Q_OBJECT

    public:
        explicit Unpacker(QObject *parent = 0) : QObject(parent) {}

        bool unpack(QString filepath, QString destination, QString frameworks, bool apktool, bool smali);

        QString getManifest(QString filename) const;
        QString getVarAppTitle(QString contents) const;

        QString getPackageName(QString manifest) const;      ///< Returns APK package name.
        QString getVersionCode(QString manifest) const;      ///< Returns APK version code.
        QString getVersionName(QString manifest) const;      ///< Returns APK display version.
        QString getApplicationLabel(QString manifest) const; ///< Returns APK display name.
        QString getMinimumSdk(QString manifest) const;       ///< Returns APK minimum SDK version.
        QString getTargetSdk(QString manifest) const;        ///< Returns APK target SDK version.

        QList<QSharedPointer<Icon> > getIcons(QString manifest, QString contents);
        QList<Apk::String> getTranslations(QString contents, QString key);

        bool unzip(QString filename, QString destination);
        bool unzip(QString filename, QString destination, QString frameworks, bool smali) const;

    public slots:

    signals:
        void unpacked(Apk::File *apk) const;
        void loading(short percentage, QString text = QString()) const;
        void error(QString message) const;

    private:
        QString parse(QString regexp, QString str) const;
    };
}

#endif // APKUNPACKER_H
