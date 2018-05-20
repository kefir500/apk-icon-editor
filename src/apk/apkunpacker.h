#ifndef APKUNPACKER_H
#define APKUNPACKER_H

#include "apkfile.h"
#include <QProcess>

namespace Apk {

    class Unpacker : public QObject
    {
        Q_OBJECT

    public:
        explicit Unpacker(QObject *parent = 0);
        void unpack(QString filepath, QString destination, QString apktoolPath, QString frameworks, bool smali);
        void cancel();

    private:
        QProcess *apktool;

    signals:
        void unpacked(Apk::File *apk) const;
        void loading(short percentage, QString text = QString()) const;
        void error(QString message, QString details = QString()) const;
    };
}

#endif // APKUNPACKER_H
