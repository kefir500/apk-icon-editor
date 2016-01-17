#include "apk.h"
#include "globals.h"
#include <QProcess>

QString Apk::getApktoolVersion()
{
    QProcess p;
    p.start(QString("java -jar \"%1/apktool.jar\" -version").arg(Path::App::dir()));
    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        return p.readAllStandardOutput().trimmed();
    }
    else {
        return QString();
    }
}

QString Apk::getJavaVersion(Java java)
{
    QProcess p;
    p.start(QString("%1 -version").arg(java == JRE ? "java" : "javac"));
    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        const QString VERSION = p.readAllStandardError().replace("\r\n", "\n").trimmed();
        return VERSION;
    }
    else {
        return QString();
    }
}

QString Apk::getJreVersion() { return getJavaVersion(JRE); }
QString Apk::getJdkVersion() { return getJavaVersion(JDK); }
bool Apk::isJavaInstalled()  { return !(getJreVersion().isNull() && getJdkVersion().isNull()); }
