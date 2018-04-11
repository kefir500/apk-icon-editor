#include "apkfile.h"
#include <QDirIterator>
#include <QTextStream>
#include <QDebug>

Apk::File::File(const QString &contentsPath)
{
    // Be careful with the "contentsPath" variable: this directory is recursively removed in the destructor.

    this->contentsPath = contentsPath;

    // Parse application manifest:

    manifest = new Manifest(contentsPath + "/AndroidManifest.xml", contentsPath + "/apktool.yml");
    manifestModel.initialize(manifest);

    // Parse application icon attribute (android:icon):

    QString iconAttribute = manifest->getApplicationIcon();
    QString iconCategory = iconAttribute.split('/').value(0).mid(1);
    QString iconFilename = iconAttribute.split('/').value(1);

    // Parse application label attribute (android:label):

    QString labelAttribute = manifest->getApplicationLabel();
    if (!labelAttribute.startsWith("@string/")) {
        return; // TODO
    }
    QString labelKey = labelAttribute.mid(QString("@string/").length());

    // Parse resource directories:

    QDirIterator resourceDirectories(contentsPath + "/res/", QDir::Dirs | QDir::NoDotAndDotDot);
    while (resourceDirectories.hasNext()) {

        const QString resourceDirectory = QFileInfo(resourceDirectories.next()).fileName();
        const QString categoryTitle = resourceDirectory.split('-').first(); // E.g., "drawable", "values"...

        if (iconCategory == categoryTitle) {

            // Parse resource files:

            QDirIterator resourceFiles(contentsPath + "/res/" + resourceDirectory, QDir::Files);
            while (resourceFiles.hasNext()) {

                const QString resourceFile = QFileInfo(resourceFiles.next()).fileName();

                // Parse application icons:

                const QStringList possibleIcons = QStringList() << (iconFilename + ".png") << (iconFilename + ".jpg") << (iconFilename + ".gif");
                if (categoryTitle == iconCategory && possibleIcons.contains(resourceFile)) {
                    const QString icon = QString("%1/res/%2/%3").arg(contentsPath, resourceDirectory, resourceFile);
                    thumbnail.addFile(icon);
                    iconsModel.add(icon);
                }
            }
        } else if (categoryTitle == "values") {

            // Parse titles:

            QDirIterator resourceFiles(contentsPath + "/res/" + resourceDirectory, QDir::Files);
            while (resourceFiles.hasNext()) {
                const QString resourceFile = QFileInfo(resourceFiles.next()).filePath();
                titlesModel.add(resourceFile, labelKey);
            }
        }
    }
}

Apk::File::~File()
{
    delete manifest;
}

void Apk::File::saveIcons()
{
    iconsModel.save();
}

void Apk::File::saveTitles()
{
    titlesModel.save();
}

void Apk::File::removeFiles()
{
    QFileInfo fi(contentsPath);
    if (!contentsPath.isEmpty() && fi.exists() && !fi.isRoot()) {
        QDir(contentsPath).removeRecursively();
    }
}

bool Apk::File::addIcon(Icon::Dpi dpi)
{
    if (!iconsModel.hasDpi(dpi)) {
        const QString filePath = getIconPath(dpi) + ".png";
        if (!filePath.isEmpty()) {
            QString directory = QFileInfo(filePath).path();
            QDir().mkdir(directory);
            QPixmap pixmap = iconsModel.last()->getPixmap();
            if (pixmap.save(filePath)) {
                iconsModel.add(filePath);
                return true;
            }
        }
    }
    return false;
}

void Apk::File::removeIcon(Icon *icon)
{
    QFileInfo fi(icon->getFilename());
    QString filePath = fi.path() + "/" + fi.baseName();
    QStringList icons;
    icons << filePath + ".png" << filePath + ".jpg" << filePath + ".gif";
    foreach (const QString &icon, icons) {
        QFile::remove(icon);
    }
    iconsModel.remove(icon);
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
    QFile f(contentsPath + "/AndroidManifest.xml");
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

QString Apk::File::getFilePath() const { return filePath; }
QString Apk::File::getContentsPath() const { return contentsPath; }
QIcon Apk::File::getThumbnail() const { return thumbnail; }

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

void Apk::File::setFilePath(QString filepath) { this->filePath = filepath; }
void Apk::File::setApksigner(bool value) { isApksigner = value; }
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

QString Apk::File::getIconPath(Icon::Dpi dpi)
{
    const QString iconAttribute = manifest->getApplicationIcon();
    const QString iconCategory = iconAttribute.split('/').value(0).mid(1);
    const QString iconFilename = iconAttribute.split('/').value(1);

    QString qualifier;
    switch (dpi) {
        case Icon::Ldpi: qualifier = "ldpi"; break;
        case Icon::Mdpi: qualifier = "mdpi"; break;
        case Icon::Hdpi: qualifier = "hdpi"; break;
        case Icon::Xhdpi: qualifier = "xhdpi"; break;
        case Icon::Xxhdpi: qualifier = "xxhdpi"; break;
        case Icon::Xxxhdpi: qualifier = "xxxhdpi"; break;
        default: return false;
    }

    if (!iconFilename.isEmpty() && !qualifier.isEmpty()) {
        return QString("%1/res/%2-%3/%4").arg(contentsPath, iconCategory, qualifier, iconFilename);
    }
    return QString();
}
