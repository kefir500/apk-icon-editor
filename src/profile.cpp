#include "profile.h"
#include <QVBoxLayout>

QList<Profile> Profile::profiles;

Profile::Profile(QString title, QIcon icon,
                 short ldpi, short mdpi, short hdpi, short xhdpi, short xxhdpi)
{
    m_title = title;
    m_thumb = icon;
    presets.push_back(DpiPreset("LDPI", ldpi));
    presets.push_back(DpiPreset("MDPI", mdpi));
    presets.push_back(DpiPreset("HDPI", hdpi));
    presets.push_back(DpiPreset("XHDPI", xhdpi));
    presets.push_back(DpiPreset("XXHDPI", xxhdpi));
}

void Profile::init()
{
    const QIcon ANDROID(":/gfx/icon-android.png");
    const QIcon KINDLE(":/gfx/icon-kindle.png");

    Profile android("Android Default", ANDROID, 36, 48, 72, 96, 144);
    Profile kindle1("Kindle Fire (1st Gen)", KINDLE, 36, 322, 72, 96, 144);
    Profile kindle2("Kindle Fire (2nd Gen)", KINDLE, 36, 365, 72, 96, 144);
    Profile kindle3("Kindle Fire HD 7\" (2nd Gen)", KINDLE, 36, 48, 425, 96, 144);
    Profile kindle4("Kindle Fire HD 8.9\" (2nd Gen)", KINDLE, 36, 48, 675, 96, 144);
    Profile kindle5("Kindle Fire HD 7\" (3rd Gen)", KINDLE, 36, 48, 375, 96, 144);
    Profile kindle6("Kindle Fire HDX 7\" (3rd Gen)", KINDLE, 36, 48, 72, 562, 144);
    Profile kindle7("Kindle Fire HDX 8.9\" (3rd Gen)", KINDLE, 36, 48, 72, 624, 144);
    Profile kindle8("Kindle Fire - All Models", KINDLE, 36, 365, 675, 624, 144);
    kindle1.presets[MDPI].info = "Carousel";
    kindle2.presets[MDPI].info = "Carousel";
    kindle3.presets[HDPI].info = "Carousel";
    kindle4.presets[HDPI].info = "Carousel";
    kindle5.presets[HDPI].info = "Carousel";
    kindle6.presets[XHDPI].info = "Carousel";
    kindle7.presets[XHDPI].info = "Carousel";
    kindle8.presets[MDPI].info = "Kindle Fire Carousel";
    kindle8.presets[HDPI].info = "Kindle Fire HD Carousel";
    kindle8.presets[XHDPI].info = "Kindle Fire HDX Carousel";

    profiles.push_back(android);
    profiles.push_back(kindle1);
    profiles.push_back(kindle2);
    profiles.push_back(kindle3);
    profiles.push_back(kindle4);
    profiles.push_back(kindle5);
    profiles.push_back(kindle6);
    profiles.push_back(kindle7);
    profiles.push_back(kindle8);
}

int Profile::getDpiSide(Dpi id) const
{
    return presets.at(id).side;
}

QString Profile::getDpiTitle(Dpi id) const
{
    DpiPreset ic = presets.at(id);
    if (ic.info.isNull()) {
        return QString("%1 (%2x%2)").arg(ic.title).arg(ic.side);
    }
    else {
        return QString("%1 (%2x%2) - %3").arg(ic.title).arg(ic.side).arg(ic.info);
    }
}
