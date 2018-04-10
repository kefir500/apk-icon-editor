#ifndef APKUNPACKER_H
#define APKUNPACKER_H

#include "apkfile.h"

namespace Apk {

    class Unpacker : public QObject
    {
        Q_OBJECT

    public:
        explicit Unpacker(QObject *parent = 0) : QObject(parent) {}

        bool unpack(QString filepath, QString destination, QString frameworks, bool smali);

    signals:
        void unpacked(Apk::File *apk) const;
        void loading(short percentage, QString text = QString()) const;
        void error(QString message) const;
    };
}

#endif // APKUNPACKER_H
