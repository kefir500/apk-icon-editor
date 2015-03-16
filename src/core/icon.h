/// \file icon.h
/// \brief This file contains #Icon class declaration.

#ifndef ICON_H
#define ICON_H

#include <QGraphicsEffect>

/// \brief QPixmap wrapper.
///
/// Wrapper for QPixmap used to handle actions with APK icons.

class Icon : public QObject {
    Q_OBJECT

private:
    QPixmap pixmap;                     ///< Stores pixmap itself.
    bool isColor;                       ///< If \c TRUE, renders colorize effect.
    bool flipX;                         ///< Stores horizontal flipping state.
    bool flipY;                         ///< Stores vertical flipping state.
    short angle;                        ///< Stores angle of rotation (in degrees).
    QColor color;                       ///< Stores "Colorize" effect color.
    qreal depth;                        ///< Stores "Colorize" effect depth.
    qreal blur;                         ///< Stores "Blur" effect radius.
    qreal radius;                       ///< Stores rounded corners radius.
    const QString filename_original;    ///< Stores pixmap filename specified in constructor. Used to #revert the original one.

public:
    /// \brief Deprecated #Icon constructor creating dummies. Used by shared pointers.
    explicit Icon() { }

    /// \brief Normal #Icon constructor. The only way to load pixmap in it.
    /// \param[in] filename Used to load #pixmap. This parameter is also saved in #filename_original property to #revert original pixmap.
    explicit Icon(QString filename);

    /// \brief Save pixmap to given filename.
    /// \param[in] filename File to save pixmap to. If NULL, #filename_original is used.
    /// \retval \c TRUE on success.
    /// \retval \c FALSE on error.
    /// \todo Empty #pixmap is currently handled by a dirty workaround.
    bool save(QString filename = NULL);

    /// \brief Replace #pixmap with the specified one.
    /// \param[in] pixmap Pixmap to replace with.
    /// \retval \c TRUE on success.
    /// \retval \c FALSE on error.
    bool replace(QPixmap _pixmap);

    /// \brief Resize pixmap to square with the specified side.
    /// \param[in] side Side in pixels.
    /// \retval \c TRUE on success.
    /// \retval \c FALSE on error.
    bool resize(int side);

    bool revert();                                          ///< Revert the original #pixmap (loaded fron #filename_original).
    bool isNull() const { return pixmap.isNull(); }         ///< Check if #pixmap is null.
    int width() const { return pixmap.width(); }            ///< Get #pixmap width.
    int height() const { return pixmap.height(); }          ///< Get #pixmap height.
    QPixmap getPixmap() { return applyEffects(); }          ///< Get #pixmap.

    QPixmap applyEffects() const;                           ///< Apply graphic effects to #pixmap.
    bool getColorEnabled() { return isColor; }              ///< Returns \c TRUE if "Colorize" effect is enabled.
    int getAngle() { return angle; }                        ///< Returns rotation #angle (in degrees).
    bool getFlipX() { return flipX; }                       ///< Returns \c TRUE if #pixmap is flipped horizontally.
    bool getFlipY() { return flipY; }                       ///< Returns \c TRUE if #pixmap is flipped vertically.
    QColor getColor() { return color; }                     ///< Returns "Colorize" effect #color.
    qreal getDepth() { return depth; }                      ///< Returns "Colorize" effect #depth.
    qreal getBlur() { return blur; }                        ///< Returns "Blur" effect \c radius.
    qreal getCorners() { return radius; }                   ///< Returns the #radius of rounded corners.

public slots:
    void setColorEnabled(bool enable) { isColor = enable; } ///< Enable or disable "Colorize" effect.
    void setAngle(int _angle) { angle = _angle; }           ///< Set rotation #angle (in degrees).
    void setFlipX(bool value) { flipX = value; }            ///< Set horizontal flipping.
    void setFlipY(bool value) { flipY = value; }            ///< Set vertical flipping
    void setColor(QColor _color) { color = _color; }        ///< Set "Colorize" effect #color.
    void setDepth(qreal _depth) { depth = _depth; }         ///< Set "Colorize" effect color #depth.
    void setBlur(qreal radius) { blur = radius; }           ///< Set "Blur" effect \c radius.
    void setCorners(qreal _radius) { radius = _radius; }    ///< Set the #radius of rounded corners.
};

/// \brief Filter for open/save dialogs.
/// \see EXT_GFX

const QString FILTER_GFX =
    "PNG (*.png)"
    ";;ICO (*.ico)"
    ";;GIF (*.gif)"
    ";;JPEG (*.jpg *.jpeg)"
    ";;BMP (*.bmp)"
;

/// \brief List of supported image formats.
/// \see FILTER_GFX

const QStringList EXT_GFX = QStringList()
    << "png" << "ico" << "gif"
    << "jpg" << "jpeg" << "bmp";

#endif // ICON_H
