#include "iconsmodel.h"
#include <QDebug>

IconsModel::~IconsModel()
{
    qDeleteAll(icons);
}

void IconsModel::add(const QString &filename)
{
    qDebug() << "Added application icon:" << filename;
    beginInsertRows(QModelIndex(), icons.count(), icons.count());
        icons.append(new Icon(filename));
    endInsertRows();
}

bool IconsModel::hasDpi(Icon::Dpi dpi)
{
    QListIterator<Icon *> it(icons);
    while (it.hasNext()) {
        Icon *icon = it.next();
        if (icon->getDpi() == dpi) {
            return true;
        }
    }
    return false;
}

void IconsModel::clone(Icon *source)
{
    if (source) {
        foreach (Icon *icon, icons) {
            if (icon->getDpi() != Icon::None) {
                icon->replace(source->getPixmap());
            }
        }
    }
}

void IconsModel::save()
{
    foreach (Icon *icon, icons) {
        icon->save();
    }
}

Icon *IconsModel::first()
{
    return icons.first();
}

Icon *IconsModel::last()
{
    return icons.last();
}

bool IconsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);
    if (index.isValid()) {
        Icon *icon = icons.at(index.row());
        icon->setPixmap(value.value<QPixmap>());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

QVariant IconsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        Icon *icon = icons.at(index.row());
        if (role == Qt::DisplayRole) {
            return icon->getQualifiers().join(" - ").toUpper();
        } else if (role == Qt::DecorationRole) {
            return icon->getPixmap();
        }
    }
    return QVariant();
}

QModelIndex IconsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent)) {
        return createIndex(row, column, icons.at(row));
    }
    return QModelIndex();
}

int IconsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return icons.count();
}
