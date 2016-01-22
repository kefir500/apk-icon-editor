///
/// \file
/// This file describes size presets for different mobile devices.
///

#ifndef DEVICES_H
#define DEVICES_H

#include <QIcon>

///
/// Preset size for a device.
/// This structure describes a single size (DPI) for a device preset.
///

struct Dpi
{
    /// Constructs a size preset with the specified \c title and \c size.
    Dpi(QString title, QSize size) : title(title), size(size) { }

    QString title; ///< Size title ("LDPI", "MDPI", etc.).
    QString info;  ///< Size additional information.
    QSize size;    ///< Expected icon size.

    ///
    /// Supported icon sizes.
    /// This enumeration represents the supported icon sizes.
    ///

    enum Type { LDPI, MDPI, HDPI, XHDPI, XXHDPI, XXXHDPI, BANNER, COUNT };

    /// Casts the integer \c value to a corresponding DPI.
    static Dpi::Type cast(int value) { return static_cast<Dpi::Type>(value); }
};

///
/// Device preset.
/// This class describes icon size presets for different devices.
///

class Device {

public:
    /// Constructs a device preset.
    /// \param title   Device title.
    /// \param thumb   Device thumbmail icon.
    /// \param ldpi    Device LDPI icon size.
    /// \param mdpi    Device MDPI icon size.
    /// \param hdpi    Device HDPI icon size.
    /// \param xhdpi   Device XHDPI icon size.
    /// \param xxhdpi  Device XXHDPI icon size.
    /// \param xxxhdpi Device XXXHDPI icon size.
    /// \param banner  Android TV banner size.
    Device(QString title,
           QIcon thumb,
           short ldpi = 32,
           short mdpi = 48,
           short hdpi = 72,
           short xhdpi = 96,
           short xxhdpi = 144,
           short xxxhdpi = 192,
           QSize banner = QSize(320, 180));

    QString getTitle() const { return title; } ///< Returns the device title.
    QIcon getThumb() const { return thumb; }   ///< Returns the device thumbnail icon.

    QString getDpiTitle(Dpi::Type dpi) const;  ///< Returns the DPI preset title.
    QSize getDpiSize(Dpi::Type dpi) const;     ///< Returns the DPI preset size.

    void setSizeLdpi(QSize size) { sizes[Dpi::LDPI].size = size; }           ///< Sets the LDPI preset size to \c size.
    void setSizeMdpi(QSize size) { sizes[Dpi::MDPI].size = size; }           ///< Sets the MDPI preset size to \c size.
    void setSizeHdpi(QSize size) { sizes[Dpi::HDPI].size = size; }           ///< Sets the HDPI preset size to \c size.
    void setSizeXhdpi(QSize size) { sizes[Dpi::XHDPI].size = size; }         ///< Sets the XHDPI preset size to \c size.
    void setSizeXxhdpi(QSize size) { sizes[Dpi::XXHDPI].size = size; }       ///< Sets the XXHDPI preset size to \c size.
    void setSizeXxxhdpi(QSize size) { sizes[Dpi::XXXHDPI].size = size; }     ///< Sets the XXXHDPI preset size to \c size.
    void setSizeDpi(Dpi::Type dpi, QSize size) { sizes[dpi].size = size; }   ///< Sets the preset size for \c dpi DPI to \c size.

    void setHintLdpi(QString hint) { sizes[Dpi::LDPI].info = hint; }         ///< Sets the additional information for LDPI to \c hint.
    void setHintMdpi(QString hint) { sizes[Dpi::MDPI].info = hint; }         ///< Sets the additional information for MDPI to \c hint.
    void setHintHdpi(QString hint) { sizes[Dpi::HDPI].info = hint; }         ///< Sets the additional information for HDPI to \c hint.
    void setHintXhdpi(QString hint) { sizes[Dpi::XHDPI].info = hint; }       ///< Sets the additional information for XHDPI to \c hint.
    void setHintXxhdpi(QString hint) { sizes[Dpi::XXHDPI].info = hint; }     ///< Sets the additional information for XXHDPI to \c hint.
    void setHintXxxhdpi(QString hint) { sizes[Dpi::XXXHDPI].info = hint; }   ///< Sets the additional information for XXXHDPI to \c hint.
    void setHintDpi(Dpi::Type dpi, QString hint) { sizes[dpi].info = hint; } ///< Sets the additional information for \c dpi DPI to \c hint.

private:
    QString title;    ///< Device title.
    QIcon thumb;      ///< Device thumbnail icon.
    QList<Dpi> sizes; ///< List of sizes for a single device.
};

///
/// Device presets interface.
/// This class provides a static interface for accessing device presets.
///

class Devices {

public:
    static void init();                                 ///< Initializes available device presets.
    static int count() { return devices.size(); }       ///< Returns the number of available device presets.
    static Device at(int id) { return devices.at(id); } ///< Returns the device at the specified \c id index.

private:
    Devices() {}
    static QList<Device> devices; ///< List of initialized device presets.
};

#endif // DEVICES_H
