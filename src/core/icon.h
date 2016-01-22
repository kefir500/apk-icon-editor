///
/// \file
/// This file contains the class to handle APK icons.
///

#ifndef ICON_H
#define ICON_H

#include <QGraphicsEffect>

///
/// APK icon.
/// QPixmap wrapper used to handle actions with APK icons.
///

class Icon : public QObject {
    Q_OBJECT

public:
    /// \brief Constructor used by shared pointers to create dummies.
    Icon() { }

    /// \brief Regular constructor for general use.
    /// \param filename Name of the icon file. The argument will be set as the original filename.
    explicit Icon(QString filename);

    /// \brief Loads the icon file from the specified \c filename.
    /// \param filename Name of the icon file. The argument will be set as the original filename.
    bool load(QString filename);

    /// \brief Saves the icon to the file with the given \c filename.
    /// \param filename File to save pixmap to. If empty, the original filename is used.
    /// \retval \c true on success.
    /// \retval \c false on error.
    bool save(QString filename = QString());

    /// \brief Replaces the icon pixmap with the specified one.
    /// \param pixmap Pixmap to replace with.
    /// \retval \c true on success.
    /// \retval \c false on error.
    bool replace(QPixmap pixmap);

    /// \brief Resizes the pixmap to the specified \c size.
    /// \param size Icon size.
    /// \retval \c true on success.
    /// \retval \c false on error.
    bool resize(QSize size);

    /// \brief Resizes the icon to the specified \c width and \c height.
    /// \param width  Icon width in pixels.
    /// \param height Icon height in pixels.
    /// \retval \c true on success.
    /// \retval \c false on error.
    bool resize(int width, int height);

    bool revert();                                       ///< Reverts the original icon (loaded from the original filename).
    bool isNull() const { return pixmap.isNull(); }      ///< Checks if the icon is \c NULL.
    int width() const { return pixmap.width(); }         ///< Returns the icon width.
    int height() const { return pixmap.height(); }       ///< Returns the icon height.
    QPixmap getPixmap();                                 ///< Returns the icon with the applied visual effects.

    bool getColorEnabled() { return colorize; }          ///< Returns \c true if the "Colorize" effect is enabled.
    int getAngle() { return angle; }                     ///< Returns the rotation angle (in degrees).
    bool getFlipX() { return flipX; }                    ///< Returns \c true if the pixmap is flipped horizontally.
    bool getFlipY() { return flipY; }                    ///< Returns \c true if the pixmap is flipped vertically.
    QColor getColor() { return color; }                  ///< Returns the "Colorize" effect color.
    qreal getDepth() { return depth; }                   ///< Returns the "Colorize" effect depth.
    qreal getBlur() { return blur; }                     ///< Returns the "Blur" effect \c radius.
    qreal getCorners() { return corners; }               ///< Returns the radius of the rounded corners.

public slots:
    void setAngle(int value) { angle = value; }          ///< Sets the rotation angle to the specified \c value (in degrees).
    void setColorize(bool enable) { colorize = enable; } ///< Turns the "Colorize" effect on/off.
    void setFlipX(bool value) { flipX = value; }         ///< Turns the horizontal flipping on/off.
    void setFlipY(bool value) { flipY = value; }         ///< Turns the vertical flipping on/off.
    void setColor(QColor value) { color = value; }       ///< Sets the "Colorize" effect color to the specified \c value.
    void setDepth(qreal value) { depth = value; }        ///< Sets the "Colorize" effect depth to the specified \c value.
    void setBlur(qreal radius) { blur = radius; }        ///< Sets the "Blur" effect radius to the specified \c radius.
    void setCorners(qreal radius) { corners = radius; }  ///< Sets the radius of the rounded corners to the specified \c radius.

private:
    QPixmap pixmap;   ///< Stores the pixmap itself.
    bool colorize;    ///< Stores the "Colorize" effect state.
    bool flipX;       ///< Stores the horizontal flipping state.
    bool flipY;       ///< Stores the vertical flipping state.
    short angle;      ///< Stores the rotation angle (in degrees).
    QColor color;     ///< Stores the "Colorize" effect color.
    qreal depth;      ///< Stores the "Colorize" effect depth.
    qreal blur;       ///< Stores the "Blur" effect radius.
    qreal corners;    ///< Stores the rounded corners radius.
    QString original; ///< Stores the pixmap original filename. Used to revert the original pixmap.
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
