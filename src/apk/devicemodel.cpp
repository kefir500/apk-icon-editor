#include "devicemodel.h"

DeviceModel::DeviceModel(QObject *parent) : QAbstractListModel(parent)
{
    const QIcon iconAndroid(":/gfx/devices/android.png");
    const QIcon iconBlackberry(":/gfx/devices/blackberry.png");
    const QIcon iconAmazon(":/gfx/devices/amazon.png");

    const QSize tvBannerSize(320, 180);

    Device *android = new Device("Android Default", iconAndroid, 36, 48, 72, 96, 144, 192, tvBannerSize);
    Device *bb_q10 = new Device("BlackBerry Q10", iconBlackberry, 90, 90, 90, 90, 90, 90, tvBannerSize); // Q10, Q5, Q10
    Device *bb_z10 = new Device("BlackBerry Z10", iconBlackberry, 110, 110, 110, 110, 110, 110, tvBannerSize);
    Device *bb_z30 = new Device("BlackBerry Z30", iconBlackberry, 96, 96, 96, 96, 96, 96, tvBannerSize); // Z30, Z3, Z30
    Device *bb_passport = new Device("BlackBerry Passport", iconBlackberry, 144, 144, 144, 144, 144, 144, tvBannerSize);
    Device *kindle1 = new Device("Kindle Fire (1st Gen)", iconAmazon, 36, 322, 72, 96, 144, 192, tvBannerSize);
    Device *kindle2 = new Device("Kindle Fire (2nd Gen)", iconAmazon, 36, 365, 72, 96, 144, 192, tvBannerSize);
    Device *kindle3 = new Device("Kindle Fire HD 7\" (2nd Gen)", iconAmazon, 36, 48, 425, 96, 144, 192, tvBannerSize);
    Device *kindle4 = new Device("Kindle Fire HD 8.9\" (2nd Gen)", iconAmazon, 36, 48, 675, 96, 144, 192, tvBannerSize);
    Device *kindle5 = new Device("Kindle Fire HD 7\" (3rd Gen)", iconAmazon, 36, 48, 375, 96, 144, 192, tvBannerSize);
    Device *kindle6 = new Device("Kindle Fire HDX 7\" (3rd Gen)", iconAmazon, 36, 48, 72, 562, 144, 192, tvBannerSize);
    Device *kindle7 = new Device("Kindle Fire HDX 8.9\" (3rd Gen)", iconAmazon, 36, 48, 72, 624, 144, 192, tvBannerSize);
    Device *kindle8 = new Device("Kindle Fire - All Models", iconAmazon, 36, 365, 675, 624, 144, 192, tvBannerSize);
    kindle1->setHint(Icon::Mdpi, "Carousel");
    kindle2->setHint(Icon::Mdpi, "Carousel");
    kindle3->setHint(Icon::Hdpi, "Carousel");
    kindle4->setHint(Icon::Hdpi, "Carousel");
    kindle5->setHint(Icon::Hdpi, "Carousel");
    kindle6->setHint(Icon::Xhdpi, "Carousel");
    kindle7->setHint(Icon::Xhdpi, "Carousel");
    kindle8->setHint(Icon::Mdpi, "Kindle Fire Carousel");
    kindle8->setHint(Icon::Hdpi, "Kindle Fire HD Carousel");
    kindle8->setHint(Icon::Xhdpi, "Kindle Fire HDX Carousel");

    add(android);
    add(bb_q10);
    add(bb_z10);
    add(bb_z30);
    add(bb_passport);
    add(kindle1);
    add(kindle2);
    add(kindle3);
    add(kindle4);
    add(kindle5);
    add(kindle6);
    add(kindle7);
    add(kindle8);
}

DeviceModel::~DeviceModel()
{
    qDeleteAll(devices);
}

void DeviceModel::add(Device *device)
{
    beginInsertRows(QModelIndex(), devices.count(), devices.count());
    devices.append(device);
    endInsertRows();
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        Device *device = devices.at(index.row());
        if (role == Qt::DisplayRole) {
            return device->getTitle();
        } else if (role == Qt::DecorationRole) {
            return device->getThumbnail();
        }
    }
    return QVariant();
}

QModelIndex DeviceModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent)) {
        return createIndex(row, column, devices.at(row));
    }
    return QModelIndex();
}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return devices.count();
}
