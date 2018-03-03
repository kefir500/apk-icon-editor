#include "apkfile.h"
#include <QDir>
#include <QTextStream>

Apk::File::~File()
{
    //clear();
}

void Apk::File::clear()
{
    icons.clear();
    strings.clear();

    QFileInfo fi(contents);
    if (!contents.isEmpty() && fi.exists() && !fi.isRoot()) {
        QDir(contents).removeRecursively();
    }
}

QDomElement Apk::File::findIntentByCategory(QDomElement activity, QString category)
{
    QDomElement intent = activity.firstChildElement("intent-filter");
    while (!intent.isNull()) {

        QDomElement cat = intent.firstChildElement("category");
        while (!cat.isNull()) {

            if (cat.attribute("android:name") == "android.intent.category." + category) {
                return intent;
            }
            cat = cat.nextSiblingElement();
        }
        intent = intent.nextSiblingElement();
    }
    return QDomElement();
}

bool Apk::File::addAndroidTV()
{
    QFile f(contents + "/AndroidManifest.xml");
    if (f.open(QFile::ReadWrite | QFile::Text)) {

        QTextStream in(&f);
        QString xml = in.readAll();
        QDomDocument dom;
        dom.setContent(xml);

        QDomElement application = dom.firstChildElement("manifest")
                                     .firstChildElement("application");

        application.setAttribute("android:banner", "@drawable/banner");

        QDomElement intent;

        QDomElement activity = application.firstChildElement("activity");
        while (!activity.isNull()) {
            intent = findIntentByCategory(activity, "LAUNCHER");
            if (!intent.isNull()) { break; }
            activity = activity.nextSiblingElement();
        }

        // If the "activity" tag was not found, try the "activity-alias" tag:
        if (intent.isNull()) {
            QDomElement activity = application.firstChildElement("activity-alias");
            while (!activity.isNull()) {
                intent = findIntentByCategory(activity, "LAUNCHER");
                if (!intent.isNull()) { break; }
                activity = activity.nextSiblingElement();
            }
        }

        if (!intent.isNull()) {
            // Add Android TV launcher:
            QDomElement tvlauncher = dom.createElement("category");
            tvlauncher.setTagName("category");
            tvlauncher.setAttribute("android:name", "android.intent.category.LEANBACK_LAUNCHER");
            intent.appendChild(tvlauncher);

            // Write output XML:
            f.resize(0);
            QTextStream out(&f);
            dom.save(out, 4);
            f.close();
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}

// Getters:

QString Apk::File::getFilePath() const { return filepath; }
QString Apk::File::getDirectory() const { return contents; }
QString Apk::File::getAppTitle() const { return appTitle; }
QString Apk::File::getVarAppTitle() const { return varAppTitle; }
QString Apk::File::getVersionName() const { return versionName; }
QString Apk::File::getVersionCode() const { return versionCode; }

Icon* Apk::File::getIcon(Dpi::Type id) const
{
    return icons.size() > id && !icons.at(id).isNull()
    ? icons.at(id).data()
    : NULL;
}

QList<QSharedPointer<Icon> > Apk::File::getIcons() const { return icons; }
QList<Apk::String> Apk::File::getStrings() const { return strings; }

short Apk::File::getRatio() const { return ratio; }
bool Apk::File::getApktool() const { return isApktool; }
bool Apk::File::getApksigner() const { return isApksigner; }
bool Apk::File::getSmali() const { return isSmali; }
bool Apk::File::getSign() const { return isSign; }
bool Apk::File::getZipalign() const { return isZipalign; }
bool Apk::File::getKeystore() const { return isKeystore; }

QString Apk::File::getFilePem() const { return filePem; }
QString Apk::File::getFilePk8() const { return filePk8; }
QString Apk::File::getFileKeystore() const { return fileKeystore; }
QString Apk::File::getPassKeystore() const { return passKeystore; }
QString Apk::File::getAlias() const { return alias; }
QString Apk::File::getPassAlias() const { return passAlias; }

// Setters:

void Apk::File::setFilePath(QString filepath) { this->filepath = filepath; }
void Apk::File::setDirectory(QString path) { contents = path; }
void Apk::File::setAppTitle(QString title) { appTitle = title; }
void Apk::File::setVarAppTitle(QString variable) { varAppTitle = variable; }
void Apk::File::setVersionName(QString name) { versionName = name; }
void Apk::File::setVersionCode(QString code) { versionCode = code; }
void Apk::File::setIcons(QList<QSharedPointer<Icon> > icons) { this->icons = icons; }
void Apk::File::setStrings(QList<Apk::String> strings) { this->strings = strings; }

void Apk::File::setApktool(bool value) { isApktool = value; }
void Apk::File::setApksigner(bool value) { isApksigner = value; }
void Apk::File::setRatio(short ratio) { this->ratio = ratio; }
void Apk::File::setSmali(bool value) { isSmali = value; }
void Apk::File::setZipalign(bool value) { isZipalign = value; }
void Apk::File::setSign(bool value) { isSign = value; }
void Apk::File::setKeystore(bool value) { isKeystore = value; }

void Apk::File::setFilePemPk8(QString pem, QString pk8) { filePem = pem; filePk8 = pk8; }

void Apk::File::setFileKeystore(QString filename, QString alias, QString passKeystore, QString passAlias)
{
    fileKeystore = filename;
    this->alias = alias;
    this->passKeystore = passKeystore;
    this->passAlias = passAlias;
}
