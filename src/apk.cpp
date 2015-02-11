#include "apk.h"
#include "main.h"
#include <QApplication>
#include <QDirIterator>
#include <QProcess>
#include <QRegExp>
#include <QDir>
#include <QTime>
#include <QtConcurrent/QtConcurrentRun>
#include <QuaZIP/JlCompress.h>

const char *Apk::STR_ERROR = QT_TR_NOOP("%1 Error");
const char *Apk::STR_ERRORSTART = QT_TR_NOOP("Error starting <b>%1</b>");
const char *Apk::STR_CHECKPATH = QT_TR_NOOP("(check the PATH variable if JRE is already installed)");
const QString Apk::LOG_ERRORSTART("%1: could not start");
const QString Apk::LOG_EXITCODE("%1 exit code: %2");
const QString Apk::TEMPDIR_APK = QDir::toNativeSeparators(TEMPDIR + "apk/");

QDebug operator<<(QDebug d, const PackOptions &o) {
    d << "Packing APK...";
    d << "\n\tFilename:" << o.filename;
    d << "\n\tApplication name:" << o.appName;
    d << "\n\tApplication version code:" << o.appVersionCode;
    d << "\n\tApplication version name:" << o.appVersionName;
    d << "\n\tUsing apktool:" << o.isApktool;
    d << "\n\tRatio:" << o.ratio;
    d << "\n\tSmali:" << o.isSmali;
    d << "\n\tSign:" << o.isSign;
    d << "\n\tZipalign:" << o.isOptimize;
    d << "\n\tUsing KeyStore:" << o.isKeystore;
    d << "\n\tPEM" << (QFile::exists(o.filePem) ? "found;" : "NOT found;");
    d << "\n\tPK8" << (QFile::exists(o.filePk8) ? "found;" : "NOT found;");
    d << "\n\tKeyStore" << (QFile::exists(o.fileKey) ? "found;" : "NOT found;");
    d << "\n\tKeyStore Password:" << (!o.passStore.isEmpty() ? "present;" : "NOT present;");
    d << "\n\tAlias:" << o.alias;
    d << "\n\tAlias Password:" << (!o.passAlias.isEmpty() ? "present;" : "NOT present;");
    return d;
}

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

void Apk::unpack(PackOptions options)
{
    qDebug() << "Unpacking" << options.filename;
    filename = options.filename;
    strings.clear();
    var_androidLabel.clear();
    warnText.clear();
    QtConcurrent::run(this, &Apk::doUnpack, options);
}

void Apk::pack(PackOptions options)
{
    qDebug() << options;
    filename = options.filename;
    QtConcurrent::run(this, &Apk::doPack, options);
}

// --- UNPACKING APK ---

bool Apk::doUnpack(PackOptions options)
{
    emit loading(20, tr("Loading APK..."));

    emit loading(40, tr("Reading AndroidManifest.xml..."));
    if (!readManifest()) {
        return false;
    }

    emit loading(60, tr("Unpacking APK..."));
    clearTemp();
    if (!options.isApktool) {
        if (!unzip()) {
            return false;
        }
    }
    else {
        if (!unzip_apktool(options.isSmali)) {
            return false;
        }
        emit loading(80, tr("Parsing string resources..."));
        loadStrings();
    }
    loadIcons();

    emit loading(100, tr("APK successfully loaded"));
    emit unpacked(filename);
    qDebug() << "Done.\n";

    if (!warnText.isEmpty()) {
        warning(tr("Warning"), warnText);
    }

    return true;
}

bool Apk::readManifest()
{
    const QString AAPT_ERROR(tr(STR_ERROR).arg("Aapt"));

    QProcess p;
    p.start(QString("aapt dump badging \"%1\"").arg(filename));
    if (!p.waitForStarted(-1)) {
        qDebug() << qPrintable(LOG_ERRORSTART.arg("aapt"));
        return die(AAPT_ERROR, tr(STR_ERRORSTART).arg("aapt"));
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

#ifndef USE_7ZIP

    QTime sw;
    sw.start();
    QStringList result = JlCompress::extractDir(filename, TEMPDIR + "apk");
    qDebug() << qPrintable(QString("Unpacked in %1s").arg(sw.elapsed() / 1000.0));
    if (!result.isEmpty()) {
        return true;
    }
    else {
        const QString QUAZIP_ERROR = tr(STR_ERROR).arg("QuaZIP");
        return die(QUAZIP_ERROR, QUAZIP_ERROR);
    }

#else

    QProcess p;
    QTime sw;
    sw.start();
    p.start(QString("7za x \"%1\" -y -o\"%2apk\"").arg(filename, TEMPDIR));
    if (!p.waitForStarted(-1)) {
        qDebug() << qPrintable(LOG_ERRORSTART.arg("7za"));
        return die(tr(STR_ERROR).arg("7ZA"), tr(STR_ERRORSTART).arg("7za"));
    }
    p.waitForFinished(-1);
    qDebug() << qPrintable(QString("Unpacked in %1s").arg(sw.elapsed() / 1000.0));
    return getZipSuccess(p.exitCode());

#endif
}

bool Apk::unzip_apktool(bool smali) const
{
    QProcess p;
    QTime sw;
    sw.start();
    p.start(QString("java -jar \"%1/apktool.jar\" d \"%2\" -f %3 -o \"%4apk\"")
            .arg(QApplication::applicationDirPath(), filename, (smali ? "" : "-s"), TEMPDIR));
    if (!p.waitForStarted(-1)) {
        if (isJavaInstalled()) {
            qDebug() << qPrintable(LOG_ERRORSTART.arg("apktool"));
            return die(tr(STR_ERROR).arg("Apktool"), tr(STR_ERRORSTART).arg("apktool"));
        }
        else {
            return die(tr(STR_ERROR).arg("Apktool"),
                       tr("\"Apktool\" requires Java Runtime Environment.") +
                          QString("<br><a href=\"%1\">%2</a> %3.<br>%4").arg(
                                  URL_JAVA, tr("Download"), tr(STR_CHECKPATH),
                                  tr("You may also change repacking method (Settings -> Repacking).")));
        }
    }
    p.waitForFinished(-1);
    qDebug() << qPrintable(QString("Unpacked in %1s").arg(sw.elapsed() / 1000.0));
    if (!p.exitCode()) {
        return true;
    }
    else {
        qDebug() << p.readAllStandardError();
        const QString ERROR_APKTOOL = tr(STR_ERROR).arg("Apktool");
        return die(ERROR_APKTOOL, ERROR_APKTOOL);
    }
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

void Apk::loadStrings()
{
    QString manifest_text;

    // Get application label variable name:
    QFile f(TEMPDIR_APK + "AndroidManifest.xml");
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        manifest_text = f.readAll();
        f.close();
    }
    else {
        return;
    }

    QString label = parse("android:label=\"(.+)\"", manifest_text);
    if (label.startsWith("@string/")) {
        var_androidLabel = label.mid(8);
    }
    else {
        return;
    }

    // Get application label translations:
    QDir dir(TEMPDIR_APK + "res");
    QStringList langs = dir.entryList(QStringList("values*"), QDir::Dirs);
    for (int i = 0; i < langs.size(); ++i) {
        QFile f(dir.absolutePath() + '/' + langs[i] + "/strings.xml");
        if (f.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream stream(&f);
            while (!stream.atEnd()) {
                QString line = stream.readLine();
                QString rx = QString("<string name=\"%1\">(.+)</string>").arg(var_androidLabel);
                QString app_name = parse(rx, line);
                if (!app_name.isEmpty()) {
                    strings << Resource(f.fileName(), app_name);
                }
            }
            f.close();
        }
    }
}

// --- PACKING APK ---

bool Apk::doPack(PackOptions opts)
{
    bool isSigned = false;
    bool isOptimized = false;

    removeRecursively(TEMPDIR_APK + "META-INF");

    emit loading(20, tr("Saving PNG icons..."));
    if (!saveIcons()) {
        return false;
    }

    if (!opts.isApktool) {
        emit loading(40, tr("Packing APK..."));
        if (!zip(opts.ratio)) {
            return false;
        }
    }
    else {
        emit loading(30, tr("Saving string resources..."));
        saveXmlChanges(opts.appName,
                       opts.appVersionName,
                       opts.appVersionCode,
                       opts.resources);
        emit loading(40, tr("Packing APK..."));
        if (!zip_apktool()) {
            return false;
        }
    }

    // Sign:

    if (opts.isSign) {
        qDebug() << "Signing...";
        emit loading(60, tr("Signing APK..."));
        if (opts.isKeystore) {
            isSigned = sign(opts.fileKey, opts.alias, opts.passStore, opts.passAlias);
        }
        else {
            isSigned = sign(opts.filePem, opts.filePk8);
        }
    }
    else {
        isSigned = true;
        QFile::rename(TEMPDIR + "temp-1.apk", TEMPDIR + "temp-2.apk");
    }

    // Optimize:

    if (opts.isOptimize) {
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
    qDebug() << "Done.\n";

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
        if (!isJavaInstalled()) {
            warning += "<hr>" +
                    tr("Signing APK requires Java Runtime Environment.") +
                    QString("<br><a href=\"%1\">%2</a> %3.")
                    .arg(URL_JAVA, tr("Download"), tr(STR_CHECKPATH));
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
                return die(tr(STR_ERROR).arg("PNG"), tr("Error saving PNG icon."));
            }
        }
    }
    return true;
}

void Apk::saveXmlChanges(QString appName, QString versionName, QString versionCode, QList<Resource> resources) const
{
    // Set application name strings:
    if (!var_androidLabel.isEmpty()) {

        resources.push_front(Resource(TEMPDIR_APK + "res/values/strings.xml", appName));

        for (int i = 0; i < resources.size(); ++i) {

            QFileInfo fi(resources[i].getFilename());
            qDebug() << qPrintable(QString("Resource %1/%2: %3 = '%4'")
                                   .arg(fi.dir().dirName(),
                                        fi.fileName(),
                                        var_androidLabel,
                                        resources[i].getValue()));

            QFile f(resources[i].getFilename());
            if (f.open(QFile::ReadWrite | QFile::Text)) {

                const QString LINE = QString("<string name=\"%1\">%2</string>").arg(var_androidLabel);
                QTextStream in(&f);
                QString xml = in.readAll();
                QRegExp rx;
                rx.setMinimal(true);
                rx.setPattern(LINE.arg(".*"));
                xml.replace(rx, QString(LINE).arg(resources[i].getValue()));
                f.resize(0);
                QTextStream out(&f);
                out << xml;
                f.close();
            }
        }
    }
    else {
        QFile f(TEMPDIR_APK + "AndroidManifest.xml");
        if (f.open(QFile::ReadWrite | QFile::Text)) {
            QTextStream in(&f);
            QString xml = in.readAll();
            f.resize(0);
            QRegExp rx("android:label=\"[^\"]+");
            xml.replace(rx, QString("android:label=\"%1").arg(appName));
            QTextStream out(&f);
            out << xml;
            f.close();
        }
    }

    // Set new version code and version name:
    if (versionCode != getVersionCode() || versionName != getVersionName()) {
        qDebug() << "versionCode:" << getVersionCode() << "=>" << versionCode;
        qDebug() << "versionName:" << getVersionName() << "=>" << versionName;
        QFile f(TEMPDIR_APK + "apktool.yml");
        if (f.open(QFile::ReadWrite | QFile::Text)) {
            QString yml = f.readAll();
            QRegExp rxCode, rxName;
            rxCode.setPattern("versionCode: [^\n]+");
            rxName.setPattern("versionName: [^\n]+");
            yml.replace(rxCode, QString("versionCode: '%1'").arg(versionCode));
            yml.replace(rxName, QString("versionName: %1").arg(QString("'%1'").arg(versionName)));
            f.resize(0);
            QTextStream out(&f);
            out << yml;
            f.close();
        }
    }
}

bool Apk::zip(short ratio) const
{
#ifndef USE_7ZIP

    bool result = JlCompress::compressDir(TEMPDIR + "temp.zip", TEMPDIR_APK, ratio);
    if (result) {
        QFile::rename(TEMPDIR + "temp.zip", TEMPDIR + "temp-1.apk");
        return true;
    }
    else {
        const QString QUAZIP_ERROR = tr(STR_ERROR).arg("QuaZIP");
        return die(QUAZIP_ERROR, QUAZIP_ERROR);
    }

#else

    QProcess p;
    p.start(QString("7za a -tzip -mx%1 \"%2temp.zip\" \"%3*\"").arg(QString::number(ratio), TEMPDIR, TEMPDIR_APK));
    if (!p.waitForStarted(-1)) {
        qDebug() << qPrintable(LOG_ERRORSTART.arg("7za"));
        return die(tr(STR_ERROR).arg("7ZA"), tr(STR_ERRORSTART).arg("7za"));
    }
    p.waitForFinished(-1);
    bool success;
    if (success = getZipSuccess(p.exitCode())) {
        QFile::rename(TEMPDIR + "temp.zip", TEMPDIR + "temp-1.apk");
    }
    return success;

#endif
}

bool Apk::zip_apktool() const
{
    QProcess p;
    p.start(QString("java -jar \"%1/apktool.jar\" b \"%2\" -f -o \"%4temp.zip\"")
            .arg(QApplication::applicationDirPath(), TEMPDIR_APK, TEMPDIR));
    if (!p.waitForStarted(-1)) {
        if (isJavaInstalled()) {
            qDebug() << qPrintable(LOG_ERRORSTART.arg("apktool"));
            return die(tr(STR_ERROR).arg("Apktool"), tr(STR_ERRORSTART).arg("apktool"));
        }
        else {
            return die(tr(STR_ERROR).arg("Apktool"),
                       tr("\"Apktool\" requires Java Runtime Environment.") +
                          QString("<br><a href=\"%1\">%2</a> %3.<br>%4").arg(
                                  URL_JAVA, tr("Download"), tr(STR_CHECKPATH),
                                  tr("You may also change repacking method (Settings -> Repacking).")));
        }
    }
    p.waitForFinished(-1);
    if (!p.exitCode()) {
        QFile::rename(TEMPDIR + "temp.zip", TEMPDIR + "temp-1.apk");
        return true;
    }
    else {
        qDebug() << p.readAllStandardError();
        return die(tr(STR_ERROR).arg("Apktool"), tr(STR_ERROR).arg("Apktool"));
    }
}

bool Apk::isJavaInstalled(bool debug)
{
    QProcess p;
    p.start("java -version");
    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        if (debug) {
            qDebug() << p.readAllStandardError().trimmed();
        }
        return true;
    }
    else {
        if (debug) {
            qDebug() << "Java 32-bit not found!";
        }
        return false;
    }
}

bool Apk::sign(const QString PEM, const QString PK8) const
{
    const QString APK_SRC(TEMPDIR + "temp-1.apk");
    const QString APK_DST(TEMPDIR + "temp-2.apk");
    const QString SIGNAPK(QApplication::applicationDirPath() + "/signer/");

    if (!QFile::exists(PEM) || !QFile::exists(PEM)) {
        emit warning(NULL, tr("PEM/PK8 not found."));
        QFile::rename(APK_SRC, APK_DST);
        return false;
    }

    QProcess p;
    p.start(QString("java -jar \"%1signapk.jar\" \"%2\" \"%3\" \"%4\" \"%5\"")
            .arg(SIGNAPK, PEM, PK8, APK_SRC, APK_DST));

    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        if (p.exitCode() == 0) {
            QFile::remove(APK_SRC);
            return true;
        }
        else {
            QString error_text = p.readAllStandardError().trimmed();
            if (error_text.isEmpty()) error_text = p.readAllStandardOutput().trimmed();
            qDebug() << qPrintable(LOG_EXITCODE.arg("Java").arg(p.exitCode()));
            qDebug() << error_text;
        }
    }
    else {
        qDebug() << qPrintable(LOG_ERRORSTART.arg("Java"));
    }

    // Something went wrong:
    QFile::rename(APK_SRC, APK_DST);
    return false;
}

bool Apk::sign(const QString KEY, const QString ALIAS,
               const QString PASS_STORE, const QString PASS_KEY) const
{
    const QString APK_SRC(TEMPDIR + "temp-1.apk");
    const QString APK_DST(TEMPDIR + "temp-2.apk");

    if (!QFile::exists(KEY)) {
        emit warning(NULL, tr("KeyStore not found."));
        QFile::rename(APK_SRC, APK_DST);
        return false;
    }

    QProcess p;
    const QString ENV_PATH = qgetenv("PATH");
    const QString JAVA_HOME = qgetenv("JAVA_HOME");
    qputenv("PATH", QString("%1;%2/bin").arg(ENV_PATH, JAVA_HOME).toStdString().c_str());
    p.start(QString("jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 "
            "-keystore \"%1\" \"%2\" -storepass \"%3\" -keypass \"%4\" \"%5\"")
            .arg(KEY, APK_SRC, PASS_STORE, PASS_KEY, ALIAS));
    qputenv("PATH", ENV_PATH.toStdString().c_str());

    if (p.waitForStarted(-1)) {
        p.waitForFinished(-1);
        if (p.exitCode() == 0) {
            QFile::rename(APK_SRC, APK_DST);
            return true;
        }
        else {
            QString error_text = p.readAllStandardError().trimmed();
            if (error_text.isEmpty()) error_text = p.readAllStandardOutput().trimmed();
            qDebug() << qPrintable(LOG_EXITCODE.arg("Jarsigner").arg(p.exitCode()));
            qDebug() << error_text;
        }
    }
    else {
        qDebug() << qPrintable(LOG_ERRORSTART.arg("Jarsigner"));
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
    const QString error_7za(tr(STR_ERROR).arg("7ZA"));

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

QList<Resource> Apk::getStrings() const
{
    return strings;
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
