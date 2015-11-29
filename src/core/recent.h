#ifndef RECENT_H
#define RECENT_H

#include <QStringList>
#include <QPixmap>

struct RecentEntry {
    RecentEntry(QString filename, QString iconfile);
    QString file;
    QPixmap icon;
};

class Recent {

public:
    void init(QStringList files);
    void add(QString filename, QPixmap icon);
    bool remove(int id);
    bool remove(QString filename);
    bool empty() const {return recent.empty(); }
    int size() const {return recent.size(); }
    void clear();

    QList<RecentEntry> all() const { return recent; }
    QStringList files() const;
    QList<QPixmap> icons() const;

    RecentEntry operator[] (int id) const {
        return recent.at(id);
    }

private:
    QList<RecentEntry> recent;
    QString hash(QString filename) const;
    QString cached(QString filename) const;

};

#endif // RECENT_H
