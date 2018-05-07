#include "icon.h"
#include <QDir>
#include <QFileInfo>
#include <QPainter>
#include <QLabel>

Icon::Icon(QString filename, Type type)
{
    this->type = type;
    if (type == Unknown) {
        qualifiers = QFileInfo(filename).path().split('/').last().split('-').mid(1);
        foreach (const QString &qualifier, qualifiers) {
            if (qualifier == "ldpi") { this->type = Ldpi; break; }
            else if (qualifier == "mdpi") { this->type = Mdpi; break; }
            else if (qualifier == "hdpi") { this->type = Hdpi; break; }
            else if (qualifier == "xhdpi") { this->type = Xhdpi; break; }
            else if (qualifier == "xxhdpi") { this->type = Xxhdpi; break; }
            else if (qualifier == "xxxhdpi") { this->type = Xxxhdpi; break; }
        }
    }
    load(filename);
}

bool Icon::load(QString filename)
{
    filePath = filename;
    return revert();
}

bool Icon::save(QString filename)
{
    if (pixmap.isNull()) {
        // Don't save an empty icon (but don't throw error).
        return true;
    }
    if (filename.isEmpty()) {
        filename = filePath;
    }
    QDir().mkpath(QFileInfo(filename).absolutePath());
    return getPixmap().save(filename, NULL, 100);
}

bool Icon::replace(QPixmap pixmap)
{
    if (pixmap.isNull()) {
        return false;
    }
    setPixmap(pixmap);
    emit updated();
    return true;
}

bool Icon::resize(QSize size)
{
    setPixmap(pixmap.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    emit updated();
    return !pixmap.isNull();
}

bool Icon::resize(int w, int h)
{
    return resize(QSize(w, h));
}

bool Icon::revert()
{
    angle      = 0;
    isColorize = false;
    isFlipX    = false;
    isFlipY    = false;
    color      = Qt::black;
    depth      = 1.0;
    blur       = 1.0;
    corners    = 0;
    setPixmap(QPixmap(filePath));
    emit updated();
    return !pixmap.isNull();
}

QString Icon::getFilename() const
{
    return filePath;
}

Icon::Type Icon::getType() const
{
    return type;
}

QString Icon::getTitle() const
{
    if (type == TvBanner) {
        return tr("TV Banner");
    }
    return getQualifiers().join(" - ").toUpper();
}

QPixmap Icon::getPixmap()
{
    // TODO error on clone
    QPixmap gfx = pixmap;

    // Apply color overlay:

    if (isColorize && !qFuzzyIsNull(depth)) {
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

    if (isFlipX) { gfx = gfx.transformed(QTransform().scale(-1, 1)); }
    if (isFlipY) { gfx = gfx.transformed(QTransform().scale(1, -1)); }
    if (angle) { gfx = gfx.transformed(QTransform().rotate(angle)); }

    return gfx;
}

const QStringList &Icon::getQualifiers() const
{
    return qualifiers;
}

void Icon::setPixmap(const QPixmap &pixmap)
{
    this->pixmap = pixmap;
}
