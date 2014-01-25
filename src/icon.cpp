#include "icon.h"

bool Icon::save(QString filename) const
{
    if (pixmap.isNull()) {
        // Don't save an empty icon (but don't throw error).
        return true;
    }
    if (filename.isNull()) {
        filename = filename_original;
    }
    return pixmap.save(filename, NULL, 100);
}

bool Icon::replace(QPixmap _pixmap)
{
    if (!_pixmap.isNull()) {
        pixmap = _pixmap;
        return true;
    }
    else {
        return false;
    }
}

bool Icon::resize(int side)
{
    return !(pixmap = pixmap.scaled(side, side, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)).isNull();
}

bool Icon::revert()
{
    return !(pixmap = QPixmap(filename_original)).isNull();
}
