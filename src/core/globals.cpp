#include "globals.h"

QString Path::App::dir() { return QApplication::applicationDirPath(); }
QString Path::App::file() { return QApplication::applicationFilePath(); }
