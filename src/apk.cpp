#include "apk.h"
#include "main.h"
#include <QApplication>
#include <QDirIterator>
#include <QProcess>
#include <QRegExp>
#include <QDir>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

static const QString TEMPDIR_APK = QDir::toNativeSeparators(TEMPDIR + "apk/");

Apk::Apk()
{
    thread = new QThread(this);
    moveToThread(thread);
}

void Apk::retranslate()
{
    strError = tr("%1 Error");
    strErrorStart = tr("Error starting <b>%1</b>");
}

void removeRecursively(QString dir)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QDir meta(dir);
    meta.removeRecursively();
#else
    // TODO This code needs to be reviewed:
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

// --- UNPACKING APK ---

void Apk::unpack(QString _filename)
{
    thread->disconnect(this);
    connect(thread, SIGNAL(started()), this, SLOT(doUnpack()));
    filename = _filename;
    thread->start();
}

void Apk::pack(QString _filename)
{
    thread->disconnect(this);
    connect(thread, SIGNAL(started()), this, SLOT(doPack()));
    filename = _filename;
    thread->start();
}

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
    thread->quit();
    return true;
}

bool Apk::readManifest()
{
    const QString aapt_error(strError.arg("AAPT"));

    QProcess p;
    p.start(QString("aapt dump badging \"%1\"").arg(filename));
    if (!p.waitForStarted(-1)) {
        return die(aapt_error, strErrorStart.arg("aapt"));
    }
    p.waitForFinished(-1);

    // Check AAPT return code:
    switch (p.exitCode()) {
    case 0: {
        manifest = p.readAllStandardOutput();
        return true;
    }
    case 1:
        return die(aapt_error, tr("AndroidManifest.xml not found.\nAPK is invalid."));
    default:
        return die(aapt_error, tr("Error reading APK."));
    }
}

bool Apk::unzip() const
{
    QProcess p;
    p.start(QString("7za x \"%1\" -y -o\"%2apk\"").arg(filename, TEMPDIR));
    if (!p.waitForStarted(-1)) {
        return die(strError.arg("7ZA"), strErrorStart.arg("7za"));
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

    // Load icons themselves:
    icons.clear();
    for (short i = 0; i < 5; ++i) {
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

bool Apk::doPack()
{
    bool isSigned = false;
    bool isOptimized = false;

    removeRecursively(TEMPDIR_APK + "META-INF");

    emit loading(20, tr("Saving PNG icons..."));
    if (!saveIcons()) {
        return false;
    }

    emit loading(40, tr("Packing APK..."));
    if (!zip()) {
        return false;
    }

    emit loading(60, tr("Signing APK..."));
    if (checkJavaInstalled()) {
        if (isSigned = sign()) {
            emit loading(80, tr("Optimizing APK..."));
            isOptimized = optimize();
        }
    }

    emit loading(100, tr("APK successfully packed!"));

    if (isSigned && isOptimized) {
        emit success(tr("APK packed"), tr("APK successfully packed!"));
    }
    else {
        QString errtext(tr("APK packed with following warnings:"));
        if (!isSigned) {
            errtext += "<br>&bull; " + tr("APK is <b>not signed</b>;");
        }
        if (!isOptimized) {
            errtext += "<br>&bull; " + tr("APK is <b>not optimized</b>;");
        }
        emit warning(tr("APK packed"), errtext);
    }

    emit packed(filename);
    thread->quit();
    return true;
}

bool Apk::saveIcons() const
{
    for (int i = 0; i < icons.size(); ++i) {
        if (icons[i]) {
            if (!icons[i]->save()) {
                return die(strError.arg("PNG"), tr("Error saving PNG icon."));
            }
        }
    }
    return true;
}

bool Apk::zip() const
{
    QProcess p;
    p.start(QString("7za a -tzip -mx9 \"%1temp.zip\" \"%2*\"").arg(TEMPDIR, TEMPDIR_APK));
    if (!p.waitForStarted(-1)) {
        return die(strError.arg("7ZA"), strErrorStart.arg("7za"));
    }
    p.waitForFinished(-1);
    return getZipSuccess(p.exitCode());
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
        QFile::rename(TEMPDIR + "temp.zip", TEMPDIR + "temp.apk");
        emit warning(strError.arg("Java"),
                     tr("Java Runtime Environment is not installed on this machine.<br>"
                        "APK will be packed but won't be signed.") +
                     QString(" <a href=\"%1\">%2</a>").arg(URL_JAVA, tr("Download JRE")));
        return false;
    }
}

bool Apk::sign() const
{
    const QString TEMPZIP(TEMPDIR + "temp.zip");
    const QString TEMPAPK(TEMPDIR + "temp.apk");
    const QString SIGNAPK(QApplication::applicationDirPath() + "/signer/");

    QProcess p;
    p.start(QString("java -jar \"%1signapk.jar\" \"%1certificate.pem\" \"%1key.pk8\" \"%2\" \"%3\"")
            .arg(SIGNAPK, TEMPZIP, TEMPAPK));
    if (!p.waitForStarted(-1)) {
        return die(strError.arg("Java"), strErrorStart.arg("java"));
    }
    p.waitForFinished(-1);
    if (p.exitCode() == 0) {
        QFile::remove(TEMPZIP);
        return true;
    }
    else {
        QFile::rename(TEMPZIP, filename);
        return false;
    }
}

bool Apk::optimize() const
{
    QString outFile = filename;
    QFileInfo fi(outFile);
    QString suffix = fi.suffix();
    if (suffix.toLower() == "apk") {
        outFile.chop(4);
    }
    const QString APK_SRC(TEMPDIR + "temp.apk");
    const QString APK_DST(outFile + ".apk");

    QProcess p;
    p.start(QString("zipalign -f 4 \"%1\" \"%2\"").arg(APK_SRC, APK_DST));
    if (!p.waitForStarted(-1)) {
        return die(tr("Error"), strErrorStart.arg("<b>zipalign</b>"));
    }
    p.waitForFinished(-1);
    if (p.exitCode() == 0) {
        QFile::remove(APK_SRC);
        return true;
    }
    else {
        QFile::rename(APK_SRC, APK_DST);
        return false;
    }
}

bool Apk::die(QString title, QString text) const
{
    thread->quit();
    emit error(title, text);
    return false;
}

bool Apk::getZipSuccess(int code) const
{
    const QString error_7za(strError.arg("7ZA"));

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

void Apk::setIcon(Dpi id, Icon *icon)
{
    icons[id] = QSharedPointer<Icon>(icon);
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

void Apk::clearTemp()
{
    removeRecursively(TEMPDIR);
}
