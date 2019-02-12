#include "iconsproxy.h"
#include "icon.h"

#ifdef QT_DEBUG
    #include <QDebug>
#endif

IconsProxy::IconsProxy(QObject *parent) : QSortFilterProxyModel(parent)
{
    device = nullptr;
    showActivities = false;
}

void IconsProxy::setDevice(Device *device)
{
    this->device = device;
    emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

void IconsProxy::setShowActivities(bool show)
{
    showActivities = show;
    invalidateFilter();
}

QVariant IconsProxy::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && device) {
        QModelIndex sourceIndex = mapToSource(index);
        if (role == Qt::DisplayRole) {
            Icon *icon = static_cast<Icon *>(sourceIndex.internalPointer());
            if (icon) {
                return device->getIconTitle(*icon);
            }
        }
        return sourceIndex.data(role);
    }
    return QVariant();
}

bool IconsProxy::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)
    QModelIndex sourceIndex = sourceModel()->index(source_row, 0);
    Icon *icon = static_cast<Icon *>(sourceIndex.internalPointer());
    return (icon && (icon->getScope() != Icon::ScopeActivity || showActivities));
}
