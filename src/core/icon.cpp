#include "icon.h"
#include <QLabel>
#include <QPainter>

Icon::Icon(QString filename)
{
    load(filename);
}

bool Icon::load(QString filename)
{
    original = filename;
    return revert();
}

bool Icon::save(QString filename)
{
    if (pixmap.isNull()) {
        // Don't save an empty icon (but don't throw error).
        return true;
    }
    if (filename.isEmpty()) {
        filename = original;
    }
    return getPixmap().save(filename, NULL, 100);
}

bool Icon::replace(QPixmap pixmap)
{
    if (!pixmap.isNull()) {
        this->pixmap = pixmap;
        return true;
    }
    else {
        return false;
    }
}

bool Icon::resize(QSize size)
{
    // TODO Keep aspect ratio?
    return !(pixmap = pixmap.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)).isNull();
}

bool Icon::resize(int width, int height)
{
    return resize(QSize(width, height));
}

bool Icon::revert()
{
    angle    = 0;
    colorize = false;
    flipX    = false;
    flipY    = false;
    color    = Qt::black;
    depth    = 1.0;
    blur     = 1.0;
    corners  = 0;
    return !(pixmap = QPixmap(original)).isNull();
}

QPixmap Icon::getPixmap()
{
    QPixmap gfx = pixmap;

    // Apply color overlay:

    if (colorize && !qFuzzyIsNull(depth)) {
        QLabel canvas;
        QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect();
        effect->setColor(color);
        effect->setStrength(depth);
        canvas.setPixmap(gfx);
        canvas.setGraphicsEffect(effect);
        gfx = canvas.grab();
    }

    // Apply rounded corners:

    if (!qFuzzyIsNull(corners)) {
        QImage canvas(gfx.size(), QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&canvas);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(gfx.rect(), Qt::transparent);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(gfx));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawRoundedRect(gfx.rect(), corners, corners);
        painter.end();
        gfx = QPixmap::fromImage(canvas);
    }

    // Apply blur:

    if (blur > 1.0) {
        QLabel canvas;
        QGraphicsBlurEffect *effect = new QGraphicsBlurEffect();
        effect->setBlurRadius(blur);
        effect->setBlurHints(QGraphicsBlurEffect::QualityHint);
        canvas.setPixmap(gfx);
        canvas.setGraphicsEffect(effect);
        gfx = canvas.grab();
    }

    // Apply flipping and rotation:

    if (flipX) { gfx = gfx.transformed(QTransform().scale(-1, 1)); }
    if (flipY) { gfx = gfx.transformed(QTransform().scale(1, -1)); }
    if (angle) { gfx = gfx.transformed(QTransform().rotate(angle)); }

    return gfx;
}
