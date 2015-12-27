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

#include <QApplication>
#include <QStandardPaths>

namespace Gui {
    namespace Window {
        const int WIDTH  = 800;
        const int HEIGHT = 530;
    }
}

namespace Path {
    namespace App {
        static QString dir() { return QApplication::applicationDirPath(); }
        static QString file() { return QApplication::applicationFilePath(); }
    }
    namespace Log {
        static const QString DIR = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/apk-icon-editor/";
        static const QString FILE = DIR + "apk-icon-editor.log";
    }
}

namespace Url {
    static const QString WEBSITE   = "http://kefir500.github.io/apk-icon-editor/";
    static const QString VERSION   = WEBSITE + "VERSION-2";
    static const QString UPDATE    = WEBSITE + "download/#update";
    static const QString CONTACT   = WEBSITE + "#contact";
    static const QString TRANSLATE = "https://www.transifex.com/projects/p/apk-icon-editor/";
    static const QString JRE       = "https://www.java.com/en/download/manual.jsp";
    static const QString JDK       = "http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html";
}

#endif // MAIN_H
