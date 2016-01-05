///
/// \file
/// This file declares global constants.
///

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QApplication>
#include <QStandardPaths>

namespace Gui {

    namespace Screen {
        int dpi();              ///< Returns the current screen DPI value.
    }

    namespace Window {
        const int WIDTH  = 800; ///< Initial main window width.
        const int HEIGHT = 540; ///< Initial main window height.
        int scaledWidth();      ///< Returns the main window width suitable for the current DPI.
        int scaledHeight();     ///< Returns the main window height suitable for the current DPI.
    }
}

namespace Path {

    namespace App {
        QString dir();          ///< Returns the directory containing the application executable.
        QString file();         ///< Returns the file path of the application executable.
    }

    namespace Log {
        /// Full path to the log directory.
        const QString DIR = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/apk-icon-editor/";

        /// Full path to the log file.
        const QString FILE = DIR + "apk-icon-editor.log";
    }
}

namespace Url {
    const QString WEBSITE   = "http://kefir500.github.io/apk-icon-editor/";
    const QString VERSION   = WEBSITE + "VERSION-2";
    const QString UPDATE    = WEBSITE + "download/#update";
    const QString CONTACT   = WEBSITE + "#contact";
    const QString TRANSLATE = "https://www.transifex.com/projects/p/apk-icon-editor/";
    const QString JRE       = "https://www.java.com/en/download/manual.jsp";
    const QString JDK       = "http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html";
}

#endif // GLOBALS_H
