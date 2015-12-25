///
/// \file
/// This file contains the recent files handler.
///

#ifndef RECENT_H
#define RECENT_H

#include <QStringList>
#include <QPixmap>

///
/// Recent file.
/// This class describes a single recently opened file.
///

struct RecentFile
{
    /// Constructs a recent file entry.
    /// \param filename Path to the recent file.
    /// \param icon     Path to the recent file thumbnail icon.
    RecentFile(QString filename, QString icon);

    QString filename; ///< Recent file name.
    QPixmap icon;     ///< Recent file icon.
};

///
/// Recent list.
/// This class handles the list of recently opened files.
///

class Recent {

public:
    Recent(QStringList files);                   ///< Constructs a recent list from the incoming \c files.

    bool add(QString filename, QPixmap icon);    ///< Adds a recent file entry with the specified \c filename and \c icon to the recent list.
    bool remove(int id);                         ///< Removes a recent file entry at the specified \c id index.
    bool remove(QString filename);               ///< Removes a recent file entry with the specified \c filename.
    void clear();                                ///< Clears the recent list.
    bool empty() const {return recent.empty(); } ///< Returns \c true if the recent list is empty.
    int size() const {return recent.size(); }    ///< Returns the number of the files in the recent list.

    void setLimit(int limit);                    ///< Sets the maximum number of recent files to the specified \c limit.

    RecentFile at(int id) const;                 ///< Returns the recent file entry at the specified \c id index.
    QList<RecentFile> all() const;               ///< Returns the list of the recent files including filenames and icons.
    QStringList files() const;                   ///< Returns the list of the recent filenames.
    QList<QPixmap> icons() const;                ///< Returns the list of the recent file icons.

private:
    QList<RecentFile> recent;                    ///< Recent files list.
    QString cached(QString filename) const;      ///< Returns a full path to a cached icon for the specified \c filename.
    QString hash(QString filename) const;        ///< Returns an MD5 hash for the specified \c filename. Used for generating unique filenames for cached icons.
    int limit;                                   ///< Maximum number of recent files.
};

#endif // RECENT_H
