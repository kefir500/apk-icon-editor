#ifndef ICON_H
#define ICON_H

#include <QGraphicsEffect>

class Icon : public QObject
{
    Q_OBJECT

public:
    enum Type {
        Unknown,
        Ldpi,
        Mdpi,
        Hdpi,
        Xhdpi,
        Xxhdpi,
        Xxxhdpi,
        TvBanner,
        TypeCount
    };

    explicit Icon(QString filename, Type type = Unknown);
    bool load(QString filename);
    bool save(QString filename = QString());
    bool replace(QPixmap pixmap);
    bool resize(QSize size);
    bool resize(int w, int h);

    QString getTitle() const;                            ///< Returns the user-friendly icon title.
    QPixmap getPixmap();                                 ///< Returns the icon with the applied visual effects.
    QString getFilename() const;                         ///< Returns the icon filename.
    Type getType() const;

    bool revert();                                       ///< Reverts the original icon (loaded from the original filename).
    bool isNull() const { return pixmap.isNull(); }      ///< Checks if the icon is \c NULL.
    int width() const { return pixmap.width(); }         ///< Returns the icon width.
    int height() const { return pixmap.height(); }       ///< Returns the icon height.
    const QStringList &getQualifiers() const;

    bool getColorEnabled() { return isColorize; }        ///< Returns \c true if the "Colorize" effect is enabled.
    int getAngle() { return angle; }                     ///< Returns the rotation angle (in degrees).
    bool getFlipX() { return isFlipX; }                  ///< Returns \c true if the pixmap is flipped horizontally.
    bool getFlipY() { return isFlipY; }                  ///< Returns \c true if the pixmap is flipped vertically.
    QColor getColor() { return color; }                  ///< Returns the "Colorize" effect color.
    qreal getDepth() { return depth; }                   ///< Returns the "Colorize" effect depth.
    qreal getBlur() { return blur; }                     ///< Returns the "Blur" effect \c radius.
    qreal getCorners() { return corners; }               ///< Returns the radius of the rounded corners.

public slots:
    void setPixmap(const QPixmap &pixmap);
    void setAngle(int value);                       ///< Sets the rotation angle to the specified \c value (in degrees).
    void setColorize(bool enable);                  ///< Turns the "Colorize" effect on/off.
    void setFlipX(bool value);                      ///< Turns the horizontal flipping on/off.
    void setFlipY(bool value);                      ///< Turns the vertical flipping on/off.
    void setColor(QColor value);                    ///< Sets the "Colorize" effect color to the specified \c value.
    void setDepth(qreal value);                     ///< Sets the "Colorize" effect depth to the specified \c value.
    void setBlur(qreal radius);                     ///< Sets the "Blur" effect radius to the specified \c radius.
    void setCorners(qreal radius);                  ///< Sets the radius of the rounded corners to the specified \c radius.

signals:
    void updated() const;

private:
    QPixmap pixmap;   ///< Stores the pixmap itself.
    QString filePath; ///< Stores the pixmap original filename. Used to revert the original pixmap.
    QStringList qualifiers;
    Type type;

    bool isColorize;  ///< Stores the "Colorize" effect state.
    bool isFlipX;     ///< Stores the horizontal flipping state.
    bool isFlipY;     ///< Stores the vertical flipping state.
    short angle;      ///< Stores the rotation angle (in degrees).
    QColor color;     ///< Stores the "Colorize" effect color.
    qreal depth;      ///< Stores the "Colorize" effect depth.
    qreal blur;       ///< Stores the "Blur" effect radius.
    qreal corners;    ///< Stores the rounded corners radius.
};

#endif // ICON_H
