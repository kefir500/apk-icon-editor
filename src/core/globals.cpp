#include "globals.h"
#include <QScreen>

// Gui::Screen

int Gui::Screen::dpi() {
#ifndef Q_OS_OSX
    return QApplication::primaryScreen()->logicalDotsPerInch();
#else
    return 100;
#endif
}

int Gui::Screen::scaled(int size) {
    return size * Gui::Screen::dpi() / 100.0;
}

QSize Gui::Screen::scaled(int w, int h) {
    return QSize(w, h) * Gui::Screen::dpi() / 100.0;
}

QSize Gui::Screen::scaled(QSize size) {
    return size * Gui::Screen::dpi() / 100.0;
}

// Image::Formats

QStringList Image::Formats::supported()
{
    return QStringList()
        << "png" << "ico" << "gif"
        << "jpg" << "jpeg" << "bmp";
}

QString Image::Formats::openDialogFilter()
{
    return QString(
        "%1 (%2)"
        ";;PNG (*.png)"
        ";;ICO (*.ico)"
        ";;GIF (*.gif)"
        ";;JPEG (*.jpg *.jpeg)"
        ";;BMP (*.bmp)"
        ";;%3"
    )
    .arg(QApplication::translate("MainWindow", "Supported Formats"))
    .arg("*." + Image::Formats::supported().join(" *."))
    .arg(QApplication::translate("MainWindow", "All Files"));
}

QString Image::Formats::saveDialogFilter()
{
    return QString(
        "PNG (*.png)"
        ";;ICO (*.ico)"
        ";;GIF (*.gif)"
        ";;JPEG (*.jpg *.jpeg)"
        ";;BMP (*.bmp)"
    );
}
