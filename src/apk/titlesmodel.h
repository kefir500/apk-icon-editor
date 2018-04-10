#ifndef TITLESMODEL_H
#define TITLESMODEL_H

#include "titlenode.h"
#include <QAbstractTableModel>

class TitlesModel : public QAbstractTableModel {

public:
    enum TitleColumn {
        Value,
        Path,
        ColumnCount
    };

    explicit TitlesModel(QObject *parent = 0) : QAbstractTableModel(parent) {}
    ~TitlesModel();

    void add(const QString &filepath, const QString &key);
    bool save() const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    QList<String *> titles;
};

#endif // TITLESMODEL_H
