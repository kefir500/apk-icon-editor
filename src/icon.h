/// \file icon.h
/// \brief This file contains #Icon class declaration.

#ifndef ICON_H
#define ICON_H

#include <QPixmap>
#include <QDir>

/// \brief QPixmap wrapper.
///
/// Wrapper for QPixmap used to handle actions with APK icons.

class Icon {

private:
    QPixmap pixmap;                   ///< Stores pixmap itself.
    const QString filename_original;  ///< Stores pixmap filename specified in constructor. Used to #revert the original one.

public:
    /// \brief Deprecated #Icon constructor creating dummies. Used by shared pointers.
    Icon() { }

    /// \brief Normal #Icon constructor. The only way to load pixmap in it.
    /// \param[in] filename Used to load #pixmap. This parameter is also saved in #filename_original property to #revert original pixmap.
    Icon(QString filename) :
        filename_original(QDir::toNativeSeparators(filename)),
        pixmap(QPixmap(filename)) { }

    /// \brief Save pixmap to given filename.
    /// \param[in] filename File to save pixmap to. If NULL, #filename_original is used.
    /// \retval TRUE on success.
    /// \retval FALSE on error.
    /// \todo Empty #pixmap is currently handled by a dirty workaround.
    bool save(QString filename = NULL) const;

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

    bool revert();                                  ///< Revert the original #pixmap (loaded fron #filename_original).
    bool isNull() const { return pixmap.isNull(); } ///< Check if #pixmap is null.
    int width() const { return pixmap.width(); }    ///< Get #pixmap width.
    int height() const { return pixmap.height(); }  ///< Get #pixmap height.
    QPixmap getPixmap() const { return pixmap; }    ///< Get #pixmap.
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
