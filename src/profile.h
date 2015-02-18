/// \file profile.h
/// \brief This file contains #Profile, #IconType and #Dpi declarations.

#ifndef PROFILE_H
#define PROFILE_H

#include <QIcon>

/// \brief This enum represents supported DPI values for icons.

enum Dpi {
    LDPI, MDPI, HDPI, XHDPI, XXHDPI, XXXHDPI, DPI_COUNT
};

/// \brief This class provides profiles with DPI presets for different devices.
/// \todo Custom user profiles.
///
/// Profile class provides \e presets of DPI sizes for different devices.
/// List of available \e #profiles is retrieved using #init static method.
///
/// Concept:
/// \e Preset describes single DPI size.
/// \e Profile stores multiple #presets (for LDPI, MDPI, etc.).
///
/// \image html profile.png

class Profile {
private:

    /// \brief This struct stores description of the profile item (information, standard size, etc.).
    /// \image html profile.png
    struct DpiPreset {
        QString title;      ///< Preset title ("LDPI", "MDPI"...).
        QString info;       ///< Preset additional information.
        short side;         ///< Expected side of square icon.
        DpiPreset(QString _title, short _side) : title(_title), side(_side) { }
    };

    static QList<Profile> profiles; ///< Static list of initialized profiles. Filled in #init method.
    QList<DpiPreset> presets;       ///< List of DPI \e presets.

    QString m_title;    ///< \e Profile displayed title.
    QIcon m_thumb;      ///< \e Profile displayed icon.

public:
    static void init();                                     ///< Initialize #profiles.
    static int count() { return profiles.size(); }          ///< Get number of available #profiles.
    static Profile at(int id) { return profiles.at(id); }   ///< Get \e profile at \c id index.

    QString title() const { return m_title; }               ///< Get \e profile title.
    QIcon thumb() const { return m_thumb; }                 ///< Get \e profile thumbnail icon.

    QString getDpiTitle(Dpi id) const;                      ///< Get \e DPI preset title.
    int getDpiSide(Dpi id) const;                           ///< Get \e DPI preset side.

    // Constructors:
    Profile() { }
    Profile(QString title, QIcon icon,
            short ldpi, short mdpi, short hdpi,
            short xhdpi, short xxhdpi, short xxxhdpi);
};

Q_DECLARE_METATYPE(Profile)

#endif // PROFILE_H
