#ifndef DEVICES_H
#define DEVICES_H

#include <QIcon>

///
/// Preset size for a device.
/// This structure describes a single size (DPI) for a device preset.
///

struct Dpi
{
    Dpi(QString title, short size) : title(title), size(size) { }
    QString title; ///< Size title ("LDPI", "MDPI", etc.).
    QString info;  ///< Size additional information.
    short size;    ///< Expected size of a square icon (in pixels).

    ///
    /// Supported icon sizes.
    /// This enumeration represents the supported icon sizes.
    ///

    enum Type { LDPI, MDPI, HDPI, XHDPI, XXHDPI, XXXHDPI, COUNT };

    /// Casts integer value to a corresponding DPI.
    static Dpi::Type cast(int value) { return static_cast<Dpi::Type>(value); }
};

///
/// Device preset.
/// This class describes icon size presets for different devices.
///

class Device {

public:
    Device(QString title,
           QIcon thumb,
           short ldpi,
           short mdpi,
           short hdpi,
           short xhdpi,
           short xxhdpi,
           short xxxhdpi);

    QString getTitle() const { return title; } ///< Get device title.
    QIcon getThumb() const { return thumb; }   ///< Get device thumbnail icon.

    QString getDpiTitle(Dpi::Type dpi) const;  ///< Get DPI preset title.
    int getDpiSize(Dpi::Type dpi) const;       ///< Get DPI preset side.

    void setSizeLdpi(short size) { sizes[Dpi::LDPI].size = size; }
    void setSizeMdpi(short size) { sizes[Dpi::MDPI].size = size; }
    void setSizeHdpi(short size) { sizes[Dpi::HDPI].size = size; }
    void setSizeXhdpi(short size) { sizes[Dpi::XHDPI].size = size; }
    void setSizeXxhdpi(short size) { sizes[Dpi::XXHDPI].size = size; }
    void setSizeXxxhdpi(short size) { sizes[Dpi::XXXHDPI].size = size; }
    void setSizeDpi(Dpi::Type dpi, short size) { sizes[dpi].size = size; }

    void setHintLdpi(QString hint) { sizes[Dpi::LDPI].info = hint; }
    void setHintMdpi(QString hint) { sizes[Dpi::MDPI].info = hint; }
    void setHintHdpi(QString hint) { sizes[Dpi::HDPI].info = hint; }
    void setHintXhdpi(QString hint) { sizes[Dpi::XHDPI].info = hint; }
    void setHintXxhdpi(QString hint) { sizes[Dpi::XXHDPI].info = hint; }
    void setHintXxxhdpi(QString hint) { sizes[Dpi::XXXHDPI].info = hint; }
    void setHintDpi(Dpi::Type dpi, QString hint) { sizes[dpi].info = hint; }

private:
    QString title;     ///< Device displayed title.
    QIcon thumb;       ///< Device displayed thumbnail icon.
    QList<Dpi> sizes;  ///< List of sizes for a single device.
};

///
/// Device presets interface.
/// This class provides a static interface for accessing device presets.
///

class Devices {

public:
    static void init();                                 ///< Initialize device presets.
    static int count() { return devices.size(); }       ///< Returns number of available device presets.
    static Device at(int id) { return devices.at(id); } ///< Returns device at the specified index.

private:
    static QList<Device> devices; ///< List of initialized device presets.
};

#endif // DEVICES_H
