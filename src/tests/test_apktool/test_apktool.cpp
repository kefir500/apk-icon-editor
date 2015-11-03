#include "test_apktool.h"
#include <QtTest>

// Set these defines to match the corresponding paths:
#define APK_PATH (QCoreApplication::applicationDirPath() + "/apk/")             // Directory containing APK files to test
#define APKTOOL  (QCoreApplication::applicationDirPath() + "/apktool.jar")      // Path to apktool
#define AAPT     (QCoreApplication::applicationDirPath() + "/aapt.exe")         // Path to aapt
#define TEMP     (QCoreApplication::applicationDirPath() + "/apktool-temp/")    // Directory to store temporary apktool files

bool Apktool::unpack(const QString FILENAME)
{
    QString manifest = Apktool::read_manifest(FILENAME);
    if (manifest.isEmpty()) {
        return false;
    }

    QProcess p;
    p.start(QString("java -jar \"%1\" d \"%2\" -f -o \"%3\"").arg(APKTOOL, FILENAME, TEMP));

    if (!p.waitForStarted(-1)) {
        if (is_java_installed()) {
            qCritical() << "Could not start Apktool";
            return false;
        }
        else {
            qCritical() << "Could not start Java";
            return false;
        }
    }

    p.waitForFinished(-1);
    if (p.exitCode() == 0) {
        return get_icons(manifest, false);
    }
    else {
        qCritical() << "Apktool internal error:";
        qCritical() << p.readAllStandardError();
        return false;
    }
}

bool Apktool::get_icons(const QString MANIFEST, bool critical)
{
    // Load png filenames:
    QStringList icons;
    icons.push_back(parse("application-icon-120:'(.+)'", MANIFEST));
    icons.push_back(parse("application-icon-160:'(.+)'", MANIFEST));
    icons.push_back(parse("application-icon-240:'(.+)'", MANIFEST));
    icons.push_back(parse("application-icon-320:'(.+)'", MANIFEST));
    icons.push_back(parse("application-icon-480:'(.+)'", MANIFEST));
    icons.push_back(parse("application-icon-640:'(.+)'", MANIFEST));

    foreach (QString icon, icons) {

        if (icon.isEmpty()) {
            continue;
        }

        if (QFile::exists(TEMP + '/' + icon)) {
            qDebug() << icon << "exists:" << "true";
        }
        else {
            qWarning() << icon << "exists:" << "FALSE";
            if (critical) {
                return false;
            }
        }
    }
    return true;
}

QString Apktool::read_manifest(const QString FILENAME)
{
    QProcess p;
    p.start(QString("\"%1\" dump badging \"%2\"").arg(AAPT, FILENAME));
    if (!p.waitForStarted(-1)) {
        qCritical() << "Could not start AAPT";
        return "";
    }

    p.waitForFinished(-1);

    const int CODE = p.exitCode();

    switch (CODE) {
        case 1:
            qWarning() << "AndroidManifest.xml contains uncritical errors";

        case 0: {
            const QString MANIFEST = p.readAllStandardOutput();
            return MANIFEST;
        }

        default:
            qCritical() << "AAPT exit code:" << CODE;
            qCritical() << p.readAllStandardError();
            return "";
    }
}

bool Apktool::is_java_installed()
{
    QProcess p;
    p.start("java -version");
    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        return true;
    }
    else {
        return false;
    }
}

QString Apktool::parse(const QString REGEXP, const QString STR)
{
    QRegExp rx;
    rx.setMinimal(true);
    rx.setPattern(REGEXP);
    rx.indexIn(STR);
    if (rx.capturedTexts().size() > 1) {
        return rx.capturedTexts().at(1);
    }
    else {
        return NULL;
    }
}

void TestApktool::test()
{
    QVERIFY(QFile::exists(APKTOOL));
    QVERIFY(QFile::exists(AAPT));
    QVERIFY(Apktool::is_java_installed());

    if (QFile::remove(QDir::homePath() + "/apktool/framework/1.apk")) {
        qDebug() << "Apktool \"1.apk\" framework removed";
    }
    else {
        qWarning() << "Apktool \"1.apk\" framework NOT removed";
    }

    const QString DIR(APK_PATH + '/');
    const QStringList APKS = QDir(DIR).entryList(QStringList("*.apk"));
    foreach (const QString APK, APKS) {
        qDebug() << "\n\nExtracting" << APK;
        QVERIFY(Apktool::unpack(DIR + APK));
    }
}

QTEST_MAIN(TestApktool)
