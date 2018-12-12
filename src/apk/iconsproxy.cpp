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
        Icon *icon = static_cast<Icon *>(sourceIndex.internalPointer());
        if (icon && role == Qt::DisplayRole) {
            Icon::Type type = icon->getType();
            QString caption = sourceModel()->data(index, role).toString();
            Device::StandardSize icon = device->getStandardSize(type);
            if (icon.size.isValid()) {
                return icon.info.isEmpty()
                    ? QString("%1 - (%2x%3)").arg(caption).arg(icon.size.width()).arg(icon.size.height())
                    : QString("%1 - (%2x%3 - %4)").arg(caption).arg(icon.size.width()).arg(icon.size.height()).arg(icon.info);
            } else {
                return icon.info.isEmpty()
                    ? QString("%1").arg(caption)
                    : QString("%1 - %2)").arg(caption, icon.info);
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
