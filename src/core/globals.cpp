#include "globals.h"
#include <QDir>
#include <QScreen>
#include <QStandardPaths>

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

// Path::App

QString Path::App::dir()
{
    return QApplication::applicationDirPath() + '/';
}

// Path::Data

QString Path::Data::shared()
{
#ifndef Q_OS_LINUX
    return QApplication::applicationDirPath() + '/';
#else
    return QApplication::applicationDirPath() + "/../share/apk-icon-editor/";
#endif
}

QString Path::Data::recent()
{
#ifndef PORTABLE
    return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/apk-icon-editor/cache/recent";
#else
    return Path::App::dir() + "data/recent/";
#endif
}

QString Path::Data::temp()
{
#ifndef PORTABLE
    return QDir::tempPath();
#else
    return Path::App::dir() + "data/temp/";
#endif
}

// Path::Log

QString Path::Log::dir()
{
#ifndef PORTABLE
    return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/apk-icon-editor/logs/";
#else
    return Path::App::dir() + "data/logs/";
#endif
}

QString Path::Log::file()
{
    return Path::Log::dir() + "apk-icon-editor.log";
}
