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

    // Parse application banner attribute (android:banner):

    QString bannerAttribute = manifest->getApplicationBanner();
    QString bannerFilename = bannerAttribute.split('/').value(1);

    // Parse application label attribute (android:label):

    QString labelAttribute = manifest->getApplicationLabel();
    QString labelKey = labelAttribute.startsWith("@string/") ? labelAttribute.mid(QString("@string/").length()) : QString();

    // Parse resource directories:

    QDirIterator resourceDirectories(contentsPath + "/res/", QDir::Dirs | QDir::NoDotAndDotDot);
    while (resourceDirectories.hasNext()) {

        const QString resourceDirectory = QFileInfo(resourceDirectories.next()).fileName();
        const QString categoryTitle = resourceDirectory.split('-').first(); // E.g., "drawable", "values"...

        if (categoryTitle == iconCategory) {

            // Parse resource files:

            QDirIterator resourceFiles(contentsPath + "/res/" + resourceDirectory, QDir::Files);
            while (resourceFiles.hasNext()) {

                const QString resourceFile = QFileInfo(resourceFiles.next()).fileName();

                // Parse application icons:

                if (categoryTitle == iconCategory) {
                    const QStringList possibleIcons = QStringList() << (iconFilename + ".png") << (iconFilename + ".jpg") << (iconFilename + ".gif");
                    if (possibleIcons.contains(resourceFile)) {
                        const QString icon = QString("%1/res/%2/%3").arg(contentsPath, resourceDirectory, resourceFile);
                        thumbnail.addFile(icon);
                        iconsModel.add(icon);
                    }
                }

                if (categoryTitle == "drawable") {

                    // Parse banner icons:

                    if (!bannerFilename.isEmpty() && bannerFilename != iconFilename) {
                        const QStringList possibleBanners = QStringList() << (bannerFilename + ".png") << (bannerFilename + ".jpg") << (bannerFilename + ".gif");
                        if (possibleBanners.contains(resourceFile)) {
                            const QString banner = QString("%1/res/%2/%3").arg(contentsPath, resourceDirectory, resourceFile);
                            iconsModel.add(banner, Icon::TvBanner);
                        }
                    }
                }
            }
        } else if (categoryTitle == "values") {

            // Parse titles:

            if (!labelKey.isEmpty()) {
                QDirIterator resourceFiles(contentsPath + "/res/" + resourceDirectory, QDir::Files);
                while (resourceFiles.hasNext()) {
                    const QString resourceFile = QFileInfo(resourceFiles.next()).filePath();
                    titlesModel.add(resourceFile, labelKey);
                }
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

bool Apk::File::addIcon(Icon::Type type)
{
    // TODO Mark APK as modified
    if (!iconsModel.hasIcon(type)) {
        if (type != Icon::TvBanner) {
            QString filePath = getIconPath(type);
            if (!filePath.isEmpty()) {
                filePath.append(".png");
                QString directory = QFileInfo(filePath).path();
                QDir().mkdir(directory);
                Icon *icon = iconsModel.getLargestIcon();
                if (icon && icon->getPixmap().save(filePath)) {
                    iconsModel.add(filePath);
                    return true;
                }
            }
        } else {
            if (!manifest->addApplicationBanner()) {
                return false;
            }
            const QString filePath = contentsPath + "/res/drawable-xhdpi/banner_custom.png";
            QString directory = QFileInfo(filePath).path();
            QDir().mkdir(directory);
            QPixmap pixmap(":/gfx/blank/tv.png");
            if (pixmap.save(filePath)) {
                iconsModel.add(filePath, Icon::TvBanner);
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

// Getters:

QString Apk::File::getFilePath() const { return filePath; }
QString Apk::File::getContentsPath() const { return contentsPath; }
QIcon Apk::File::getThumbnail() const { return thumbnail; }

bool Apk::File::getApksigner() const { return isApksigner; }
bool Apk::File::getSmali() const { return isSmali; }
bool Apk::File::getSign() const { return isSign; }
bool Apk::File::getZipalign() const { return isZipalign; }
bool Apk::File::getKeystore() const { return isKeystore; }

QString Apk::File::getApktool() const { return apktool; }
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

void Apk::File::setApktool(QString path) { apktool = path; }
void Apk::File::setFilePemPk8(QString pem, QString pk8) { filePem = pem; filePk8 = pk8; }
void Apk::File::setFileKeystore(QString filename, QString alias, QString passKeystore, QString passAlias)
{
    fileKeystore = filename;
    this->alias = alias;
    this->passKeystore = passKeystore;
    this->passAlias = passAlias;
}

QString Apk::File::getIconPath(Icon::Type type)
{
    const QString iconAttribute = manifest->getApplicationIcon();
    const QString iconCategory = iconAttribute.split('/').value(0).mid(1);
    const QString iconFilename = iconAttribute.split('/').value(1);

    QString qualifier;
    switch (type) {
        case Icon::Ldpi: qualifier = "ldpi"; break;
        case Icon::Mdpi: qualifier = "mdpi"; break;
        case Icon::Hdpi: qualifier = "hdpi"; break;
        case Icon::Xhdpi: qualifier = "xhdpi"; break;
        case Icon::Xxhdpi: qualifier = "xxhdpi"; break;
        case Icon::Xxxhdpi: qualifier = "xxxhdpi"; break;
        case Icon::TvBanner: qualifier = "xhdpi"; break;
        default: return QString();
    }

    if (!iconFilename.isEmpty() && !qualifier.isEmpty()) {
        return QString("%1/res/%2-%3/%4").arg(contentsPath, iconCategory, qualifier, iconFilename);
    }
    return QString();
}
