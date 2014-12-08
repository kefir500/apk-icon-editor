#include "icon.h"
#include <QLabel>
#include <QPainter>

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
    angle = 0;
    flipX = false;
    flipY = false;
    color = Qt::black;
    depth = 1.0;
    blur = 1.0;
    radius = 0;
    return !(pixmap = QPixmap(filename_original)).isNull();
}

QPixmap Icon::applyEffects() const
{
    QPixmap gfx = pixmap;

    if (isColor && !qFuzzyIsNull(depth)) {
        QLabel w;
        QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect();
        effect->setColor(color);
        effect->setStrength(depth);
        w.setPixmap(gfx);
        w.setGraphicsEffect(effect);
        gfx = w.grab();
    }

    if (!qFuzzyIsNull(radius)) {
        QImage canvas(gfx.size(), QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&canvas);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(gfx.rect(), Qt::transparent);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(gfx));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawRoundedRect(gfx.rect(), radius, radius);
        painter.end();
        gfx = QPixmap::fromImage(canvas);
    }

    if (blur > 1.0) {
        QLabel w;
        QGraphicsBlurEffect *effect = new QGraphicsBlurEffect();
        effect->setBlurRadius(blur);
        effect->setBlurHints(QGraphicsBlurEffect::QualityHint);
        w.setPixmap(gfx);
        w.setGraphicsEffect(effect);
        gfx = w.grab();
    }

    if (flipX) {
        gfx = gfx.transformed(QTransform().scale(-1, 1));
    }

    if (flipY) {
        gfx = gfx.transformed(QTransform().scale(1, -1));
    }

    if (angle != 0) {
        gfx = gfx.transformed(QTransform().rotate(angle));
    }

    return gfx;
}
