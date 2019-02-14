#include "device.h"

Device::Device(QString title, QIcon thumb, short ldpi, short mdpi, short hdpi, short xhdpi, short xxhdpi, short xxxhdpi, QSize banner)
{
    this->title = title;
    this->thumbnail = thumb;
    sizes[Icon::Ldpi] = StandardSize(QSize(ldpi, ldpi));
    sizes[Icon::Mdpi] = StandardSize(QSize(mdpi, mdpi));
    sizes[Icon::Hdpi] = StandardSize(QSize(hdpi, hdpi));
    sizes[Icon::Xhdpi] = StandardSize(QSize(xhdpi, xhdpi));
    sizes[Icon::Xxhdpi] = StandardSize(QSize(xxhdpi, xxhdpi));
    sizes[Icon::Xxxhdpi] = StandardSize(QSize(xxxhdpi, xxxhdpi));
    sizes[Icon::TvBanner] = StandardSize(banner);
}

QString Device::getTitle() const
{
    return title;
}

QIcon Device::getThumbnail() const
{
    return thumbnail;
}

QString Device::getIconTitle(const Icon &icon) const
{
    const QString iconTitle = icon.getTitle();
    const StandardSize iconSize = getIconSize(icon.getType());

    if (icon.getScope() != Icon::ScopeApplication) {
        return iconTitle;
    }

    if (iconSize.size.isValid()) {
        return (iconSize.info.isEmpty())
            ? QString("%1 - (%2x%3)").arg(iconTitle).arg(iconSize.size.width()).arg(iconSize.size.height())
            : QString("%1 - (%2x%3 - %4)").arg(iconTitle).arg(iconSize.size.width()).arg(iconSize.size.height()).arg(iconSize.info);
    } else {
        return iconSize.info.isEmpty()
            ? QString("%1").arg(iconTitle)
            : QString("%1 - %2)").arg(iconTitle, iconSize.info);
    }

    return QString();
}

Device::StandardSize Device::getIconSize(Icon::Type type) const
{
    return sizes[type];
}

void Device::setHint(Icon::Type type, const QString &hint)
{
    sizes[type].info = hint;
}
