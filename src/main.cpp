#include "main.h"
#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(APP);
    a.setApplicationVersion(VER);
    a.setApplicationDisplayName(QString("%1 v%2").arg(APP, VER));

    QFontDatabase::addApplicationFont(":/fonts/OpenSans-Light.ttf");

    MainWindow w(argc, argv);
    w.show();

    return a.exec();
}
