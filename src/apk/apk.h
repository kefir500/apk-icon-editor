#ifndef APK_H
#define APK_H

#include "globals.h"
#include <QString>
#include <QList>

enum Java { JRE, JDK };

namespace Apk {

    bool isJavaInstalled();            ///< Returns \c true if Java (JRE or JDK) is installed.
    QString getJavaVersion(Java java); ///< Returns Java version.
    QString getJreVersion();           ///< Returns JRE version.
    QString getJdkVersion();           ///< Returns JDK version.
    QString getApktoolVersion();       ///< Returns Apktool version.

    class String {
    public:
        String(QString key, QString value, QString filename) : key(key), value(value), filename(filename) { }
        QString getKey() const { return key; }
        QString getValue() const { return value; }
        QString getFilename() const { return filename; }
    private:
        QString key;
        QString value;
        QString filename;
    };

    const QString GETJAVA =
          QString("<a href=\"%1\">%2</a> %3.").arg(
                  Url::JRE,
                  QT_TR_NOOP("Download"),
                  QT_TR_NOOP("(check the PATH variable if JRE is already installed)"));
}

#endif // APK_H
