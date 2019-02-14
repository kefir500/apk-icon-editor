#include "apkfile.h"
#include <QDirIterator>
#include <QTextStream>

Apk::File::File(const QString &contentsPath)
{
    // Be careful with the "contentsPath" variable: this directory is recursively removed in the destructor.

    this->contentsPath = contentsPath;

    // Parse application manifest:

    manifest = new Manifest(contentsPath + "/AndroidManifest.xml", contentsPath + "/apktool.yml");
    manifestModel.initialize(manifest);
    const QString appIconAttribute = manifest->getApplicationIcon();
    const QString appIconCategory = appIconAttribute.split('/').value(0).mid(1);
    const QString appIconFilename = appIconAttribute.split('/').value(1);
    const QString appBannerAttribute = manifest->getApplicationBanner();
    const QString appBannerCategory = appBannerAttribute.split('/').value(0).mid(1);
    const QString appBannerFilename = appBannerAttribute.split('/').value(1);
    QStringList activityIcons = manifest->getActivityIcons();
    QStringList activityBanners = manifest->getActivityBanners();
    const QString appLabelAttribute = manifest->getApplicationLabel();
    const QString appLabelKey = appLabelAttribute.startsWith("@string/") ? appLabelAttribute.mid(QString("@string/").length()) : QString();

    // Parse resource directories:

    const QStringList drawableExtensions = QStringList() << "png" << "jpg" << "gif";
    QDirIterator categories(contentsPath + "/res/", QDir::Dirs | QDir::NoDotAndDotDot);
    while (categories.hasNext()) {

        const QFileInfo category(categories.next());
        const QString categoryTitle = category.fileName().split('-').first(); // E.g., "drawable", "values"...

        QDirIterator resources(category.filePath(), QDir::Files);
        while (resources.hasNext()) {

            const QFileInfo resource(resources.next());

            if (drawableExtensions.contains(resource.suffix())) {

                // Parse application icons:

                if (categoryTitle == appIconCategory) {
                    if (resource.baseName() == appIconFilename) {
                        const QString icon = resource.filePath();
                        thumbnail.addFile(icon);
                        iconsModel.add(icon, Icon::Unknown, Icon::ScopeApplication);
                    }
                }

                // Parse application banners:

                if (!appBannerFilename.isEmpty()) {
                    if (categoryTitle == appBannerCategory) {
                        if (resource.baseName() == appBannerFilename) {
                            iconsModel.add(resource.filePath(), Icon::TvBanner, Icon::ScopeApplication);
                        }
                    }
                }

                // Parse activity icons:

                foreach (const QString &activityIconAttribute, activityIcons) {
                    const QString activityIconCategory = activityIconAttribute.split('/').value(0).mid(1);
                    if (activityIconCategory == categoryTitle) {
                        const QString activityIconFilename = activityIconAttribute.split('/').value(1);
                        if (resource.baseName() == activityIconFilename) {
                            iconsModel.add(resource.filePath(), Icon::Unknown, Icon::ScopeActivity);
                        }
                    }
                }

                // Parse activity banners:

                foreach (const QString &activityBannerAttribute, activityBanners) {
                    const QString activityBannerCategory = activityBannerAttribute.split('/').value(0).mid(1);
                    if (activityBannerCategory == categoryTitle) {
                        const QString activityBannerFilename = activityBannerAttribute.split('/').value(1);
                        if (resource.baseName() == activityBannerFilename) {
                            iconsModel.add(resource.filePath(), Icon::TvBanner, Icon::ScopeActivity);
                        }
                    }
                }
            }

            // Parse application titles:

            if (!appLabelKey.isEmpty()) {
                if (categoryTitle == "values") {
                    titlesModel.add(resource.filePath(), appLabelKey);
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
            if (manifest->getApplicationIcon().isEmpty()) {
                manifest->addApplicationIcon();
            }
            QString filePath = getIconPath(type);
            if (!filePath.isEmpty()) {
                filePath.append(".png");
                QString directory = QFileInfo(filePath).path();
                QDir().mkdir(directory);
                Icon *icon = iconsModel.getLargestIcon();
                QPixmap pixmap = icon
                        ? icon->getPixmap()
                        : QPixmap(":/gfx/icon/256.png").scaled(
                              Device().getIconSize(type).size,
                              Qt::KeepAspectRatio,
                              Qt::SmoothTransformation
                        );
                if (pixmap.save(filePath)) {
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
