#ifndef ICONSPROXY_H
#define ICONSPROXY_H

#include <QSortFilterProxyModel>
#include "device.h"

class IconsProxy : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit IconsProxy(QObject *parent = 0);

    void setDevice(Device *device);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    Device *device;
};

#endif // ICONSPROXY_H
