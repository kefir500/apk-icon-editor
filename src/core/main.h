/// \file main.h
/// \brief This file contains global variables and applicaton description.

/// \mainpage APK Icon Editor
///
/// \section Description
/// Summary: APK Icon Editor is the tool designed to easily edit and replace APK (Android Package) icons.<br>
/// Written in: C++ and Qt framework.<br>
/// License: <a href="http://www.gnu.org/licenses/gpl-2.0.html">GNU General Public License v2.0</a>
///
/// English sentence structure in the current documentation needs lost of corrections.<br>
/// Application design is far from ideal and also needs some refactoring.<br>
/// Note that the given visual schemes are NOT UML standard diagrams.<br>
///
/// \section Structure
/// #MainWindow is the central object. It provides connections between the #Apk module and the #DrawArea widget.<br>
/// #Apk class provides packing and unpacking APK files and loads #Icon list.<br>
/// #DrawArea widget draws the single #Icon currently selected in the #ComboList widget.
///
/// \b Summary: According to the selected #Profile in the #ComboList widget,
/// #MainWindow gets the appropriate #Icon from #Apk and passes it to the #DrawArea widget.
///
/// \todo Review grammar of translatable strings;<br>Review grammar and sentence structure in the current documentation.
///
/// \image html main.png

#ifndef MAIN_H
#define MAIN_H

#include <QDir>
#include <QStandardPaths>

#define APPDIR QApplication::applicationDirPath()

const int WINDOW_WIDTH      = 800;
const int WINDOW_HEIGHT     = 530;

// Application:
const QString URL_WEBSITE   = "http://kefir500.github.io/apk-icon-editor/";
const QString URL_GITHUB    = "https://github.com/kefir500/apk-icon-editor/";
const QString URL_VERSION   = URL_WEBSITE + "VERSION-2";
const QString URL_UPDATE    = URL_WEBSITE + "download/#update";
const QString URL_BUGS      = URL_WEBSITE + "#contact";
const QString URL_JRE       = "https://www.java.com/en/download/manual.jsp";
const QString URL_JDK       = "http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html";
const QString URL_TRANSLATE = "https://www.transifex.com/projects/p/apk-icon-editor/";
const QString LOG_PATH      = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/apk-icon-editor/";
const QString LOG_FILE      = LOG_PATH + "apk-icon-editor.log";

#endif // MAIN_H
