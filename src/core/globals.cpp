#include "globals.h"
#include <QScreen>

int Gui::Window::scaledWidth()
{
    const qreal DPI = QApplication::primaryScreen()->logicalDotsPerInch();
    return WIDTH * (DPI / 100.0);
}

int Gui::Window::scaledHeight()
{
    const qreal DPI = QApplication::primaryScreen()->logicalDotsPerInch();
    return HEIGHT * (DPI / 100.0);
}

QString Path::App::dir() { return QApplication::applicationDirPath(); }
QString Path::App::file() { return QApplication::applicationFilePath(); }
