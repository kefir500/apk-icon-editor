///
/// \file
/// This file contains the main application class.
///

#ifndef APPLICATION_H
#define APPLICATION_H

#include "mainwindow.h"
#include <QApplication>

///
/// Main application class.
/// This class is a \c QApplication wrapper.
/// Used for initializing logging system, loading base resources
/// and handling APK files on OS X.
///

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv);
    ~Application();

protected:
    bool event(QEvent *event);

private:    
    MainWindow *window;     ///< Main application window.
    static QFile *log;      ///< Application logfile.

    void initEnv() const;   ///< Initializes PATH environment variable.
    bool initLog() const;   ///< Initializes logging system.
    void initFonts() const; ///< Initializes additional fonts.

    /// Appends (or prepends) the specified \c path to the PATH environment variable.
    void addToPath(QString path, bool append = true) const;
    static void msgHandler(QtMsgType type, const QMessageLogContext &, const QString &msg);
};

#endif // APPLICATION_H
