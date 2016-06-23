#include "apk.h"
#include "globals.h"
#include <QProcess>

bool Apk::whichJava(Java java)
{
#ifdef Q_OS_OSX
    QProcess p;
    p.start(QString("which %1").arg(java == JRE ? "java" : "javac"));
    if (p.waitForStarted()) {
        p.waitForFinished();
        return !p.readAllStandardOutput().trimmed().isEmpty();
    }
    else {
        return false;
    }
#else
    Q_UNUSED(java);
    return true;
#endif
}

QString Apk::getApktoolVersion()
{
    if (whichJava(JRE)) {
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
    else {
        return QString();
    }
}

QString Apk::getJavaVersion(Java java)
{
    if (whichJava(java)) {
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
    else {
        return QString();
    }
}

QString Apk::getJreVersion() { return getJavaVersion(JRE); }
QString Apk::getJdkVersion() { return getJavaVersion(JDK); }
bool Apk::isJavaInstalled()  { return !(getJreVersion().isNull() && getJdkVersion().isNull()); }
