#include "recent.h"
#include "globals.h"
#include <QDir>
#include <QPixmap>
#include <QCryptographicHash>

// Recent

Recent::Recent(QStringList files)
{
    QDir().mkpath(Path::Data::recent());
    recent.clear();
    limit = 10;

    for (int i = 0; i < files.size(); ++i) {
        const QString FILENAME = files[i];
        const QString ICON = cached(FILENAME);
        recent.append(RecentFile(FILENAME, ICON));
    }
}

bool Recent::add(QString filename, QPixmap icon)
{
    if (filename.isEmpty()) {
        return false;
    }

    // Create cached icon:

    const QString FILENAME = QDir::toNativeSeparators(filename);
    const QString ICON = cached(FILENAME);
    icon.scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).save(ICON);

    // Remove duplicates:

    for (int i = 0; i < recent.size(); ++i) {
        if (recent[i].filename == FILENAME) {
            recent.removeAt(i);
        }
    }

    // Create recent entry:

    recent.prepend(RecentFile(FILENAME, ICON));
    if (recent.size() > limit) {
        this->remove(limit);
    }

    return true;
}

bool Recent::remove(int id)
{
    if (recent.size() > id) {
        const QString FILENAME = recent[id].filename;
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
        if (recent[i].filename == filename) {
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

RecentFile Recent::at(int id) const
{
    return recent.at(id);
}

QList<RecentFile> Recent::all() const
{
    return recent;
}

QStringList Recent::files() const
{
    QStringList result;
    for (int i = 0; i < recent.size(); ++i) {
        result << recent[i].filename;
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

void Recent::setLimit(int limit)
{
    this->limit = limit;
}

QString Recent::hash(QString filename) const
{
    return QCryptographicHash::hash(filename.toUtf8(), QCryptographicHash::Md5).toHex();
}

QString Recent::cached(QString filename) const
{
    return QString("%1/%2.png").arg(Path::Data::recent(), hash(filename));
}

// RecentFile

RecentFile::RecentFile(QString filename, QString icon)
{
    this->filename = filename;
    this->icon = QPixmap(icon);
}
