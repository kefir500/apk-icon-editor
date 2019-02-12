#include "icon.h"
#include <QDir>
#include <QFileInfo>
#include <QPainter>
#include <QLabel>

Icon::Icon(QString filename, Type type, Scope scope)
{
    this->type = type;
    this->scope = scope;

    qualifiers = QFileInfo(filename).path().split('/').last().split('-').mid(1);

    // Guess qualifier from icon type:
    if (qualifiers.isEmpty() && type != Unknown) {
        QString dpi;
        switch (type) {
            case Ldpi: dpi = "ldpi"; break;
            case Mdpi: dpi = "mdpi"; break;
            case Hdpi: dpi = "hdpi"; break;
            case Xhdpi: dpi = "xhdpi"; break;
            case Xxhdpi: dpi = "xxhdpi"; break;
            case Xxxhdpi: dpi = "xxxhdpi"; break;
        }
        if (!dpi.isEmpty()) {
            qualifiers.append(dpi);
        }
    }
    // Guess icon type from qualifiers:
    if (type == Unknown && !qualifiers.isEmpty()) {
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

Icon::Scope Icon::getScope() const
{
    return scope;
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
    return pixmapFx;
}

const QStringList &Icon::getQualifiers() const
{
    return qualifiers;
}

void Icon::setPixmap(const QPixmap &pixmap)
{
    this->pixmap = pixmap;
    this->pixmapFx = pixmap;
    applyEffects();
}

void Icon::setAngle(int value)
{
    angle = value;
    applyEffects();
}

void Icon::setColorize(bool enable)
{
    isColorize = enable;
    applyEffects();
}

void Icon::setFlipX(bool value)
{
    isFlipX = value;
    applyEffects();
}

void Icon::setFlipY(bool value)
{
    isFlipY = value;
    applyEffects();
}

void Icon::setColor(QColor value)
{
    color = value;
    applyEffects();
}

void Icon::setDepth(qreal value)
{
    depth = value;
    applyEffects();
}

void Icon::setBlur(qreal radius)
{
    blur = radius;
    applyEffects();
}

void Icon::setCorners(qreal radius)
{
    corners = radius;
    applyEffects();
}

void Icon::applyEffects()
{
    pixmapFx = pixmap;

    // Apply color overlay:

    if (isColorize && !qFuzzyIsNull(depth)) {
        QLabel canvas;
        QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect();
        effect->setColor(color);
        effect->setStrength(depth);
        canvas.setPixmap(pixmapFx);
        canvas.setGraphicsEffect(effect);
        pixmapFx = canvas.grab();
    }

    // Apply rounded corners:

    if (!qFuzzyIsNull(corners)) {
        QImage canvas(pixmapFx.size(), QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&canvas);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(pixmapFx.rect(), Qt::transparent);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(pixmapFx));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawRoundedRect(pixmapFx.rect(), corners, corners);
        painter.end();
        pixmapFx = QPixmap::fromImage(canvas);
    }

    // Apply blur:

    if (blur > 1.0) {
        QLabel canvas;
        QGraphicsBlurEffect *effect = new QGraphicsBlurEffect();
        effect->setBlurRadius(blur);
        effect->setBlurHints(QGraphicsBlurEffect::QualityHint);
        canvas.setPixmap(pixmapFx);
        canvas.setGraphicsEffect(effect);
        pixmapFx = canvas.grab();
    }

    // Apply flipping and rotation:

    if (isFlipX) { pixmapFx = pixmapFx.transformed(QTransform().scale(-1, 1)); }
    if (isFlipY) { pixmapFx = pixmapFx.transformed(QTransform().scale(1, -1)); }
    if (angle) { pixmapFx = pixmapFx.transformed(QTransform().rotate(angle)); }
}
