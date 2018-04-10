#include "device.h"

Device::Device(QString title, QIcon thumb,
               short ldpi, short mdpi, short hdpi,
               short xhdpi, short xxhdpi, short xxxhdpi,
               QSize banner)
{
    this->title = title;
    this->thumbnail = thumb;
    sizes[Icon::Ldpi] = StandardSize(QSize(ldpi, ldpi));
    sizes[Icon::Mdpi] = StandardSize(QSize(mdpi, mdpi));
    sizes[Icon::Hdpi] = StandardSize(QSize(hdpi, hdpi));
    sizes[Icon::Xhdpi] = StandardSize(QSize(xhdpi, xhdpi));
    sizes[Icon::Xxhdpi] = StandardSize(QSize(xxhdpi, xxhdpi));
    sizes[Icon::Xxxhdpi] = StandardSize(QSize(xxxhdpi, xxxhdpi));
    //    sizes[Icon::Ldpi] = Size("TV", banner); // TODO
}

QString Device::getTitle() const
{
    return title;
}

QIcon Device::getThumbnail() const
{
    return thumbnail;
}

Device::StandardSize Device::getStandardSize(Icon::Dpi dpi) const
{
    return sizes[dpi];
}

void Device::setHint(Icon::Dpi dpi, const QString &hint)
{
    sizes[dpi].info = hint;
}
