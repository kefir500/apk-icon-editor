#include "manifestmodel.h"
#include <QCoreApplication>

ManifestModel::ManifestModel(QObject *parent) : QAbstractListModel(parent)
{
    manifest = nullptr;
}

void ManifestModel::initialize(Manifest *manifest)
{
    this->manifest = manifest;
    emit dataChanged(index(0, 0), index(RowCount - 1, 0));
}

bool ManifestModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (manifest && index.isValid() && role == Qt::EditRole) {
        const int row = index.row();
        switch (row) {
            case VersionCode:      manifest->setVersionCode(value.toInt()); break;
            case VersionName:      manifest->setVersionName(value.toString()); break;
            case MinimumSdk:       manifest->setMinSdk(value.toInt()); break;
            case TargetSdk:        manifest->setTargetSdk(value.toInt()); break;
            case ApplicationLabel: manifest->setApplicationLabel(value.toString()); break;
        }
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

QVariant ManifestModel::data(const QModelIndex &index, int role) const
{
    if (manifest && index.isValid()) {
        const int row = index.row();
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            switch (row) {
                case VersionCode:      return manifest->getVersionCode();
                case VersionName:      return manifest->getVersionName();
                case MinimumSdk:       return manifest->getMinSdk();
                case TargetSdk:        return manifest->getTargetSdk();
                case ApplicationLabel: return manifest->getApplicationLabel();
            }
        }
    }
    return QVariant();
}

QVariant ManifestModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical) {
        if (role == Qt::DisplayRole) {
            switch (section) {
                case VersionCode:      return QCoreApplication::translate("MainWindow", "Version Code");
                case VersionName:      return QCoreApplication::translate("MainWindow", "Version Name");
                case MinimumSdk:       return tr("Minimum SDK");
                case TargetSdk:        return tr("Target SDK");
                case ApplicationLabel: return QCoreApplication::translate("MainWindow", "Application Name");
            }
        }
    }
    return QVariant();
}

QModelIndex ManifestModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent)) {
        return createIndex(row, column);
    }
    return QModelIndex();
}

int ManifestModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return RowCount;
}

Qt::ItemFlags ManifestModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}
