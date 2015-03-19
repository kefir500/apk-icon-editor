#ifndef APPLICATION_H
#define APPLICATION_H

#include "mainwindow.h"
#include <QApplication>

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv);
    ~Application();

protected:
    bool event(QEvent *event);

private:
    void initLog();
    void initFonts();
    void loadFile(const QString &fileName);
    static void msgHandler(QtMsgType type, const QMessageLogContext &, const QString &msg);

    MainWindow *window;
};

#endif // APPLICATION_H
