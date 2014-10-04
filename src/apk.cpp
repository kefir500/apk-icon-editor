#include "apk.h"
#include "main.h"
#include <QApplication>
#include <QDirIterator>
#include <QProcess>
#include <QRegExp>
#include <QDir>
#include <QtConcurrent/QtConcurrentRun>

const QString Apk::STR_ERROR = tr("%1 Error");
const QString Apk::STR_ERRORSTART = tr("Error starting <b>%1</b>");
const QString Apk::LOG_ERRORSTART("%1: could not start");
const QString Apk::LOG_EXITCODE("%1 exit code: %2");
const QString Apk::TEMPDIR_APK = QDir::toNativeSeparators(TEMPDIR + "apk/");

void removeRecursively(QString dir)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QDir meta(dir);
    meta.removeRecursively();
#else
    // TODO: This code needs to be reviewed:
    QDir d(dir);
    if (d.exists(dir)) {
        Q_FOREACH(QFileInfo info, d.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                removeRecursively(info.absoluteFilePath());
            }
            else {
                QFile::remove(info.absoluteFilePath());
            }
        }
        d.rmdir(dir);
    }
#endif
}

QString parse(QString regexp, QString str)
{
    QRegExp rx;
    rx.setMinimal(true);
    rx.setPattern(regexp);
    rx.indexIn(str);
    if (rx.capturedTexts().size() > 1)
        return rx.capturedTexts().at(1);
    else
        return NULL;
}

void Apk::unpack(QString _filename)
{
    qDebug() << "Unpacking" << _filename;
    filename = _filename;
    warnText.clear();
    QtConcurrent::run(this, &Apk::doUnpack);
}

void Apk::pack(QString _filename, short ratio, bool doSign, bool doOptimize)
{
    qDebug() << qPrintable(QString("Packing \"%1\": ratio(%2), sign(%3), zipalign(%4)")
                           .arg(_filename).arg(ratio).arg(doSign).arg(doOptimize));

    filename = _filename;
    QtConcurrent::run(this, &Apk::doPack, ratio, doSign, doOptimize);
}

// --- UNPACKING APK ---

bool Apk::doUnpack()
{
    emit loading(25, tr("Loading APK..."));

    emit loading(50, tr("Reading AndroidManifest.xml..."));
    if (!readManifest()) {
        return false;
    }

    emit loading(75, tr("Unpacking APK..."));
    clearTemp();
    if (!unzip()) {
        return false;
    }
    loadIcons();

    emit loading(100, tr("APK successfully loaded"));
    emit unpacked(filename);
    qDebug() << "Unpacked.\n";

    if (!warnText.isEmpty()) {
        warning(tr("Warning"), warnText);
    }

    return true;
}

bool Apk::readManifest()
{
    const QString AAPT_ERROR(tr(STR_ERROR.toLatin1()).arg("AAPT"));

    QProcess p;
    p.start(QString("aapt dump badging \"%1\"").arg(filename));
    if (!p.waitForStarted(-1)) {
        qDebug() << qPrintable(LOG_ERRORSTART.arg("aapt"));
        return die(AAPT_ERROR, tr(STR_ERRORSTART.toLatin1()).arg("aapt"));
    }
    p.waitForFinished(-1);

    // Check AAPT return code:
    int code = p.exitCode();
    qDebug() << qPrintable(LOG_EXITCODE.arg("aapt").arg(code));
    switch (code) {
    case 1:
        warnText = tr("AndroidManifest.xml contains uncritical errors.");
        // Continue reading AndroidManifest.xml:
    case 0:
        manifest = p.readAllStandardOutput();
        return true;
    default:
        qDebug() << p.readAllStandardError().trimmed();
        return die(AAPT_ERROR, tr("Error reading APK."));
    }
}

bool Apk::unzip() const
{
    QDir dir(TEMPDIR);
    dir.mkpath(".");

    QProcess p;
    p.start(QString("7za x \"%1\" -y -o\"%2apk\"").arg(filename, TEMPDIR));
    if (!p.waitForStarted(-1)) {
        qDebug() << qPrintable(LOG_ERRORSTART.arg("7za"));
        return die(tr(STR_ERROR.toLatin1()).arg("7ZA"), tr(STR_ERRORSTART.toLatin1()).arg("7za"));
    }
    p.waitForFinished(-1);
    return getZipSuccess(p.exitCode());
}

void Apk::loadIcons()
{
    // Load png filenames:
    QStringList pngs;
    pngs.push_back(parse("application-icon-120:'(.+)'", manifest));
    pngs.push_back(parse("application-icon-160:'(.+)'", manifest));
    pngs.push_back(parse("application-icon-240:'(.+)'", manifest));
    pngs.push_back(parse("application-icon-320:'(.+)'", manifest));
    pngs.push_back(parse("application-icon-480:'(.+)'", manifest));
    pngs.push_back(parse("application-icon-640:'(.+)'", manifest));
    qDebug() << "Icons:" << pngs;

    // Load icons themselves:
    icons.clear();
    for (short i = LDPI; i < DPI_COUNT; ++i) {
        QString pngFile(pngs[i]);
        if (pngFile.isEmpty()) {
            icons.push_back(QSharedPointer<Icon>(new Icon)); // Push dummy
            continue;
        }
        int id;
        if (i == 0 || (id = pngs.lastIndexOf(pngFile, i - 1)) == -1) {
            icons.push_back(QSharedPointer<Icon>(new Icon(TEMPDIR_APK + pngFile)));
        }
        else {
            icons.push_back(icons[id]);
        }
    }
}

// --- PACKING APK ---

bool Apk::doPack(short ratio, bool doSign, bool doOptimize)
{
    bool isSigned = false;
    bool isOptimized = false;

    removeRecursively(TEMPDIR_APK + "META-INF");

    emit loading(20, tr("Saving PNG icons..."));
    if (!saveIcons()) {
        return false;
    }

    emit loading(40, tr("Packing APK..."));
    if (!zip(ratio)) {
        return false;
    }

    // Sign:

    if (doSign) {
        qDebug() << "Signing...";
        emit loading(60, tr("Signing APK..."));
        isSigned = sign();
    }
    else {
        isSigned = true;
        QFile::rename(TEMPDIR + "temp-1.apk", TEMPDIR + "temp-2.apk");
    }

    // Optimize:

    if (doOptimize) {
        qDebug() << "Aligning...";
        emit loading(80, tr("Optimizing APK..."));
        isOptimized = optimize();
    }
    else {
        isOptimized = true;
        QFile::rename(TEMPDIR + "temp-2.apk", TEMPDIR + "temp-3.apk");
    }

    // Finalize:

    if (!finalize()) {
        return die(tr("Error"), tr("Could not create output APK file."));
    }

    // Finished!

    emit loading(100, tr("APK successfully packed!"));
    qDebug() << "Packed.\n";

    if (isSigned && isOptimized) {
        emit packed(filename, true, tr("APK successfully packed!"));
    }
    else {
        QString warning(tr("APK packed with following warnings:"));
        if (!isSigned) {
            warning += "<br>&bull; " + tr("APK is <b>not signed</b>;");
        }
        if (!isOptimized) {
            warning += "<br>&bull; " + tr("APK is <b>not optimized</b>;");
        }
        if (!checkJavaInstalled()) {
            warning += "<hr>" +
                    tr("Signing APK requires Java Runtime Environment.") +
                    QString("<br><a href=\"%1\">%2</a> %3.")
                    .arg(URL_JAVA, tr("Download"), tr("(or check PATH variable if already installed)"));
        }
        emit packed(filename, false, warning);
    }
    return true;
}

bool Apk::saveIcons() const
{
    for (int i = 0; i < icons.size(); ++i) {
        if (icons[i]) {
            if (!icons[i]->save()) {
                return die(tr(STR_ERROR.toLatin1()).arg("PNG"), tr("Error saving PNG icon."));
            }
        }
    }
    return true;
}

bool Apk::zip(short ratio) const
{
    QProcess p;
    p.start(QString("7za a -tzip -mx%1 \"%2temp.zip\" \"%3*\"").arg(QString::number(ratio), TEMPDIR, TEMPDIR_APK));
    if (!p.waitForStarted(-1)) {
        qDebug() << qPrintable(LOG_ERRORSTART.arg("7za"));
        return die(tr(STR_ERROR.toLatin1()).arg("7ZA"), tr(STR_ERRORSTART.toLatin1()).arg("7za"));
    }
    p.waitForFinished(-1);
    bool success;
    if (success = getZipSuccess(p.exitCode())) {
        QFile::rename(TEMPDIR + "temp.zip", TEMPDIR + "temp-1.apk");
    }
    return success;
}

bool Apk::checkJavaInstalled() const
{
    QProcess p;
    p.start("java -version");
    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        return true;
    }
    else {
        qDebug() << "Java 32-bit not found!";
        return false;
    }
}

bool Apk::sign() const
{
    const QString APK_SRC(TEMPDIR + "temp-1.apk");
    const QString APK_DST(TEMPDIR + "temp-2.apk");
    const QString SIGNAPK(QApplication::applicationDirPath() + "/signer/");

    QProcess p;
    p.start(QString("java -jar \"%1signapk.jar\" \"%1certificate.pem\" \"%1key.pk8\" \"%2\" \"%3\"")
            .arg(SIGNAPK, APK_SRC, APK_DST));

    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        if (p.exitCode() == 0) {
            QFile::remove(APK_SRC);
            return true;
        }
        else {
            qDebug() << qPrintable(LOG_EXITCODE.arg("Java").arg(p.exitCode()));
            qDebug() << p.readAllStandardError().trimmed();
        }
    }
    else {
        qDebug() << qPrintable(LOG_ERRORSTART.arg("Java"));
    }

    // Something went wrong:
    QFile::rename(APK_SRC, APK_DST);
    return false;
}

bool Apk::optimize() const
{
    const QString APK_SRC(TEMPDIR + "temp-2.apk");
    const QString APK_DST(TEMPDIR + "temp-3.apk");

    QProcess p;
    p.start(QString("zipalign -f 4 \"%1\" \"%2\"").arg(APK_SRC, APK_DST));

    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        if (p.exitCode() == 0) {
            QFile::remove(APK_SRC);
            return true;
        }
        else {
            qDebug() << qPrintable(LOG_EXITCODE.arg("zipalign").arg(p.exitCode()));
            qDebug() << p.readAllStandardError().trimmed();
        }
    }
    else {
        qDebug() << qPrintable(LOG_ERRORSTART.arg("zipalign"));
    }

    // Something went wrong:
    QFile::rename(APK_SRC, APK_DST);
    return false;
}

bool Apk::finalize()
{
    QFileInfo fi(filename);
    QString suffix = fi.suffix();
    if (suffix.toLower() != "apk") {
        filename += ".apk";
    }
    QFile::remove(filename);
    return QFile::rename(TEMPDIR + "temp-3.apk", filename);
}

bool Apk::die(QString title, QString text) const
{
    emit error(title, text);
    return false;
}

bool Apk::getZipSuccess(int code) const
{
    qDebug() << qPrintable(LOG_EXITCODE.arg("7za").arg(code));
    const QString error_7za(tr(STR_ERROR.toLatin1()).arg("7ZA"));

    switch (code) {
    case 0:
        return true;
    case 1:
        return die(error_7za, tr("File is probably being used by another process."));
    case 2:
        return die(error_7za, tr("Fatal 7-Zip error."));
    case 7:
        return die(error_7za, tr("Command line error."));
    case 8:
        return die(error_7za, tr("Not enough memory."));
    default:
        return die(error_7za, tr("7-Zip error code: %1").arg(code));
    }
}

Icon *Apk::getIcon(Dpi id) const
{
    if (icons.size() > id && !icons.at(id).isNull()) {
        return icons.at(id).data();
    }
    else {
        return NULL;
    }
}

QString Apk::getPackageName() const
{
    return parse("package: name='(.+)'", manifest);
}

QString Apk::getVersionCode() const
{
    return parse("versionCode='(.+)'", manifest);
}

QString Apk::getVersionName() const
{
    return parse("versionName='(.+)'", manifest);
}

QString Apk::getApplicationLabel() const
{
    return parse("application-label:'(.+)'", manifest);
}

QString Apk::getMinimumSdk() const
{
    return parse("sdkVersion:'(.+)'", manifest);
}

QString Apk::getTargetSdk() const
{
    return parse("targetSdkVersion:'(.+)'", manifest);
}

void Apk::clearTemp()
{
    removeRecursively(TEMPDIR);
}
