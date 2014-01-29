#include "icon.h"
#include <QLabel>

Icon::Icon(QString filename) : filename_original(filename)
{
    revert();
}

bool Icon::save(QString filename)
{
    if (pixmap.isNull()) {
        // Don't save an empty icon (but don't throw error).
        return true;
    }
    if (filename.isNull()) {
        filename = filename_original;
    }
    return getPixmap().save(filename, NULL, 100);
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
    isColor = false;
    isBlur = false;
    color = Qt::black;
    blur = 0;
    return !(pixmap = QPixmap(filename_original)).isNull();
}

QPixmap Icon::applyEffects()
{
    QPixmap gfx = pixmap;

    if (isColor) {
        QLabel w;
        QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect();
        effect->setColor(color);
        w.setPixmap(gfx);
        w.setGraphicsEffect(effect);
        gfx = w.grab();
    }

    if (isBlur && blur != 0) {
        QLabel w;
        QGraphicsBlurEffect *effect = new QGraphicsBlurEffect();
        effect->setBlurRadius(blur);
        w.setPixmap(gfx);
        w.setGraphicsEffect(effect);
        gfx = w.grab();
    }

    return gfx;
}
