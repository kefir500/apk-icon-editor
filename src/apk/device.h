#ifndef DEVICES_H
#define DEVICES_H

#include "icon.h"
#include <QMap>
#include <QIcon>

class Device {

public:

    struct StandardSize {

        StandardSize() {}
        StandardSize(const QSize &size) : size(size) { }

        QSize size;
        QString info; ///< Size additional information.
    };

    Device(QString title,
           QIcon thumbnail,
           short ldpi = 32,
           short mdpi = 48,
           short hdpi = 72,
           short xhdpi = 96,
           short xxhdpi = 144,
           short xxxhdpi = 192,
           QSize banner = QSize(320, 180));

    QString getTitle() const;   ///< Returns the device title.
    QIcon getThumbnail() const; ///< Returns the device thumbnail icon.
    StandardSize getStandardSize(Icon::Type type) const;

    void setHint(Icon::Type type, const QString &hint);

private:
    QString title;                        ///< Device title.
    QIcon thumbnail;                      ///< Device thumbnail icon.
    QMap<Icon::Type, StandardSize> sizes; ///< List of sizes for a single device.
};

#endif // DEVICES_H
