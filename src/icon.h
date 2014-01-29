/// \file icon.h
/// \brief This file contains #Icon class declaration.

#ifndef ICON_H
#define ICON_H

#include <QPixmap>
#include <QGraphicsEffect>
#include <QDir>

/// \brief QPixmap wrapper.
///
/// Wrapper for QPixmap used to handle actions with APK icons.

class Icon : public QObject {

private:
    QPixmap pixmap;                     ///< Stores pixmap itself.
    bool isColor;                       ///< If \c TRUE, renders colorize effect.
    bool isBlur;                        ///< If \c TRUE, renders blur effect.
    qreal blur;                         ///< Stores "Blur" effect radius.
    QColor color;                       ///< Stores "Colorize" effect preferences.
    const QString filename_original;    ///< Stores pixmap filename specified in constructor. Used to #revert the original one.

public:
    /// \brief Deprecated #Icon constructor creating dummies. Used by shared pointers.
    explicit Icon() { }

    /// \brief Normal #Icon constructor. The only way to load pixmap in it.
    /// \param[in] filename Used to load #pixmap. This parameter is also saved in #filename_original property to #revert original pixmap.
    explicit Icon(QString filename);

    /// \brief Save pixmap to given filename.
    /// \param[in] filename File to save pixmap to. If NULL, #filename_original is used.
    /// \retval TRUE on success.
    /// \retval FALSE on error.
    /// \todo Empty #pixmap is currently handled by a dirty workaround.
    bool save(QString filename = NULL);

    /// \brief Replace #pixmap with the specified one.
    /// \param[in] pixmap Pixmap to replace with.
    /// \retval TRUE on success.
    /// \retval FALSE on error.
    bool replace(QPixmap _pixmap);

    /// \brief Resize pixmap to square with the specified side.
    /// \param[in] side Side in pixels.
    /// \retval TRUE on success.
    /// \retval FALSE on error.
    bool resize(int side);

    bool revert();                                          ///< Revert the original #pixmap (loaded fron #filename_original).
    bool isNull() const { return pixmap.isNull(); }         ///< Check if #pixmap is null.
    int width() const { return pixmap.width(); }            ///< Get #pixmap width.
    int height() const { return pixmap.height(); }          ///< Get #pixmap height.
    QPixmap getPixmap() { return applyEffects(); }          ///< Get #pixmap.

    QPixmap applyEffects();                                 ///< Apply graphic effects to #pixmap.
    void setColorEnabled(bool enable) { isColor = enable; } ///< Enable or disable "Colorize" effect.
    void setBlurEnabled(bool enable) { isBlur = enable; }   ///< Enable or disable "Blur" effect.
    void setColor(QColor _color) { color = _color; }        ///< Set "Colorize" effect \c color.
    void setBlur(qreal radius = 0) { blur = radius; }       ///< Set "Blur effect \c radius.
    bool getColorEnabled() { return isColor; }              ///< Returns \c TRUE if "Colorize" effect is enabled.
    bool getBlurEnabled() { return isBlur; }                ///< Returns \c TRUE if "Blur effect is enabled.
    QColor getColor() { return color; }                     ///< Get "Colorize" effect \c color.
    qreal getBlur() { return blur; }                        ///< Get "Blur effect \c radius.
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
