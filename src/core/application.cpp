#include "application.h"
#include "main.h"
#include <QTextCodec>
#include <QFontDatabase>

QFile logfile(LOG_FILE);

Application::Application(int &argc, char **argv) : QApplication(argc, argv)
{
    setApplicationName(APP);
    setApplicationVersion(VER);
    setApplicationDisplayName(QString("%1 v%2").arg(APP, VER));

    initLog();

    qDebug() << qPrintable(QString("%1 v%2").arg(APP, VER));
    qDebug() << "Initializing...";

    initFonts();

    qDebug() << "Starting...\n";

    window = new MainWindow;
    window->show();

    if (arguments().size() > 1) {
        window->apkLoad(arguments()[1]);
    }
}

void Application::initLog()
{
    QDir dir;
    dir.mkpath(LOG_PATH);
    logfile.open(QIODevice::WriteOnly | QIODevice::Text);
    qInstallMessageHandler(Application::msgHandler);
}

void Application::initFonts()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QFontDatabase::addApplicationFont(":/fonts/OpenSans-Light.ttf");
}

void Application::msgHandler(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
    QTextStream ts(&logfile);
    const QString TIME = QTime::currentTime().toString();
    QString line;

    switch (type) {
    case QtDebugMsg:
        line = QString("[%1] %2\n").arg(TIME, msg);
        ts << line;
        fprintf(stderr, "%s", qPrintable(line));
        break;
    case QtWarningMsg:
        line = QString("[%1] Warning: %2\n").arg(TIME, msg);
        ts << line;
        fprintf(stderr, "%s", qPrintable(line));
        break;
    case QtCriticalMsg:
        line = QString("[%1] Critical: %2\n").arg(TIME).arg(msg);
        ts << line;
        fprintf(stderr, "%s", qPrintable(line));
        break;
    case QtInfoMsg:
        line = QString("[%1]  %2\n").arg(TIME).arg(msg);
        ts << line;
        fprintf(stderr, "%s", qPrintable(line));
        break;
    case QtFatalMsg:
        line = QString("[%1] Fatal: %2\n").arg(TIME, msg);
        ts << line;
        fprintf(stderr, "%s", qPrintable(line));
        abort();
    }
    fflush(stderr);
}

bool Application::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::FileOpen:
        window->apkLoad(static_cast<QFileOpenEvent *>(event)->file());
        return true;
    default:
        return QApplication::event(event);
    }
}

Application::~Application()
{
    delete window;
}
