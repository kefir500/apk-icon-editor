#include "globals.h"
#include <QScreen>

// Path::App

QString Path::App::dir() {
    return QApplication::applicationDirPath();
}
QString Path::App::file() {
    return QApplication::applicationFilePath();
}

// Gui::Screen

int Gui::Screen::dpi() {
    return QApplication::primaryScreen()->logicalDotsPerInch();
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
