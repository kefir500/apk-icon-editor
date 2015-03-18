#include "main.h"
#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QFontDatabase>
#include <QStandardPaths>

QFile logfile(LOG_FILE);

void msgHandler(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
    QTextStream ts(&logfile);
    const QString TIME = QTime::currentTime().toString();
    QString line;

    switch (type) {
    case QtDebugMsg:
        line = QString("[%1] %2\n").arg(TIME, msg);
        ts << line;
        fprintf(stderr, qPrintable(line));
        break;
    case QtWarningMsg:
        line = QString("[%1] Warning: %2\n").arg(TIME, msg);
        ts << line;
        fprintf(stderr, qPrintable(line));
        break;
    case QtCriticalMsg:
        line = QString("[%1] Critical: %2\n").arg(TIME).arg(msg);
        ts << line;
        fprintf(stderr, qPrintable(line));
        break;
    case QtFatalMsg:
        line = QString("[%1] Fatal: %2\n").arg(TIME, msg);
        ts << line;
        fprintf(stderr, qPrintable(line));
        abort();
    }
    fflush(stderr);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(APP);
    a.setApplicationVersion(VER);
    a.setApplicationDisplayName(QString("%1 v%2").arg(APP, VER));

    // Creating log:
    QDir dir;
    dir.mkpath(LOG_PATH);
    logfile.open(QIODevice::WriteOnly | QIODevice::Text);
    qInstallMessageHandler(msgHandler);

    qDebug() << qPrintable(QString("%1 v%2").arg(APP, VER));
    qDebug() << "Initializing...";

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QFontDatabase::addApplicationFont(":/fonts/OpenSans-Light.ttf");

    MainWindow w;
    w.show();

    qDebug() << "Starting...\n";

    return a.exec();
}
