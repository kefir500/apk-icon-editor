#include "recent.h"
#include <QDir>
#include <QPixmap>
#include <QCryptographicHash>
#include <QStandardPaths>

const QString CACHE_PATH = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/apk-icon-editor/cache/recent";

// Recent

void Recent::init(QStringList files)
{
    QDir().mkpath(CACHE_PATH);
    recent.clear();

    for (int i = 0; i < files.size(); ++i) {
        const QString FILENAME = files[i];
        const QString ICON = cached(FILENAME);
        recent.append(RecentEntry(FILENAME, ICON));
    }
}

void Recent::add(QString filename, QPixmap icon)
{
    if (!filename.isNull()) {

        // Create cached icon:

        const QString FILENAME = QDir::toNativeSeparators(filename);
        const QString ICON = cached(FILENAME);
        icon.scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).save(ICON);

        // Remove duplicates:

        for (int i = 0; i < recent.size(); ++i) {
            if (recent[i].file == FILENAME) {
                recent.removeAt(i);
            }
        }

        // Create recent entry:

        const short LIMIT = 10;
        recent.prepend(RecentEntry(FILENAME, ICON));
        if (recent.size() > LIMIT) {
            this->remove(LIMIT);
        }
    }
}

bool Recent::remove(int id)
{
    if (recent.size() > id) {
        const QString FILENAME = recent[id].file;
        QFile::remove(cached(FILENAME));
        recent.removeAt(id);
        return true;
    }
    else {
        return false;
    }
}

bool Recent::remove(QString filename)
{
    for (int i = 0; i < recent.size(); ++i) {
        if (recent[i].file == filename) {
            this->remove(i);
            return true;
        }
    }
    return false;
}

void Recent::clear()
{
    while (!recent.isEmpty()) {
        this->remove(0);
    }
}

QString Recent::hash(QString filename) const
{
    return QCryptographicHash::hash(filename.toUtf8(), QCryptographicHash::Md5).toHex();
}

QString Recent::cached(QString filename) const
{
    return QString("%1/%2.png").arg(CACHE_PATH, hash(filename));
}

// RecentEntry

RecentEntry::RecentEntry(QString filename, QString iconfile)
{
    file = filename;
    icon = QPixmap(iconfile);
}

QStringList Recent::files() const
{
    QStringList result;
    for (int i = 0; i < recent.size(); ++i) {
        result << recent[i].file;
    }
    return result;
}

QList<QPixmap> Recent::icons() const
{
    QList<QPixmap> result;
    for (int i = 0; i < recent.size(); ++i) {
        result << recent[i].icon;
    }
    return result;
}
