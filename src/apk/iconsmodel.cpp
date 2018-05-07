#include "iconsmodel.h"
#include <QDebug>

IconsModel::~IconsModel()
{
    qDeleteAll(icons);
}

void IconsModel::add(const QString &filename, Icon::Type type)
{
    // Add:
    qDebug() << "Added application icon:" << filename;
    beginInsertRows(QModelIndex(), icons.count(), icons.count());
        icons.append(new Icon(filename));
    endInsertRows();

    // Sort:
    std::sort(icons.begin(), icons.end(), [](const Icon *a, const Icon *b) {
        return (a->getType() < b->getType());
    });
    emit dataChanged(index(0, 0), index(icons.count() - 1, 0));
}

bool IconsModel::remove(Icon *icon)
{
    QMutableListIterator<Icon *> it(icons);
    while (it.hasNext()) {
        if (icon == it.next()) {
            int index = icons.indexOf(icon);
            if (index >= 0) {
                beginRemoveRows(QModelIndex(), index, index);
                    it.remove();
                    delete icon;
                endRemoveRows();
                return true;
            }
        }
    }
    return false;
}

void IconsModel::clone(Icon *source)
{
    if (source) {
        foreach (Icon *icon, icons) {
            if (icon->getType() != Icon::Unknown) {
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

bool IconsModel::hasIcon(Icon::Type type) const
{
    QListIterator<Icon *> it(icons);
    while (it.hasNext()) {
        Icon *icon = it.next();
        if (icon->getType() == type) {
            return true;
        }
    }
    return false;
}

Icon *IconsModel::first()
{
    return icons.first();
}

Icon *IconsModel::last()
{
    return icons.last();
}

Icon *IconsModel::getLargestIcon()
{
    for (int i = icons.size() - 1; i >= 0; --i) {
        Icon *icon = icons.at(i);
        if (icon->getType() != Icon::TvBanner) {
            return icon;
        }
    }
    return nullptr;
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
            return icon->getTitle();;
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
