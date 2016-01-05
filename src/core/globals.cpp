#include "globals.h"
#include <QScreen>

int Gui::Screen::dpi()
{
    return QApplication::primaryScreen()->logicalDotsPerInch();
}

int Gui::Window::scaledWidth()
{
    const qreal DPI = Gui::Screen::dpi();
    return WIDTH * (DPI / 100.0);
}

int Gui::Window::scaledHeight()
{
    const qreal DPI = Gui::Screen::dpi();
    return HEIGHT * (DPI / 100.0);
}

QString Path::App::dir() { return QApplication::applicationDirPath(); }
QString Path::App::file() { return QApplication::applicationFilePath(); }
