///
/// \file
/// This file declares global constants.
///

#ifndef GLOBALS_H
#define GLOBALS_H

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
        QString dir();
        QString file();
    }
    namespace Log {
        const QString DIR = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/apk-icon-editor/";
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
