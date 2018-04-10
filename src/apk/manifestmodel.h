#ifndef MANIFESTMODEL_Hb
#define MANIFESTMODEL_H

#include "manifest.h"
#include <QAbstractItemModel>

class ManifestModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ManifestRow {
        ApplicationLabel,
        VersionCode,
        VersionName,
        MinimumSdk,
        TargetSdk,
        RowCount
    };

    explicit ManifestModel(QObject *parent = 0);

    void initialize(Manifest *manifest);

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    Manifest *manifest;
};

#endif // MANIFESTMODEL_H
