#include "application.h"
#include "globals.h"
#include <QDir>
#include <QTextCodec>
#include <QFontDatabase>

QFile *Application::log;

Application::Application(int &argc, char **argv) : QApplication(argc, argv)
{
    setApplicationName(APP);
    setApplicationVersion(VER);

#if defined(QT_DEBUG)
    setApplicationDisplayName(QString("%1 v%2 (Debug)").arg(APP, VER));
#elif defined(Q_OS_WIN)
    setApplicationDisplayName(QString("%1 v%2").arg(APP, VER));
#elif defined(Q_OS_LINUX)
    setApplicationDisplayName(QString("%1 v%2 for Linux (Beta)").arg(APP, VER));
#elif defined(Q_OS_OSX)
    setApplicationDisplayName(QString("%1 v%2 for OS X (Beta)").arg(APP, VER));
#else
    setApplicationDisplayName(QString("%1 v%2").arg(APP, VER));
#endif

    initEnv();
    initLog();
    initFonts();

    qDebug() << "Starting...\n";
    window = new MainWindow;
    window->show();

    if (arguments().size() > 1) {
        window->apk_open(arguments()[1]);
    }
}

void Application::addToPath(QString path, bool append) const
{
#if defined(Q_OS_WIN)
    const QString FORMAT = append ? "%1;%2" : "%2;%1";
    qputenv("PATH", QString(FORMAT).arg(QString(qgetenv("PATH")), path).toUtf8());
#else
    const QString FORMAT = append ? "%1:%2" : "%2:%1";
    qputenv("PATH", QString("%1:%2").arg(QString(qgetenv("PATH")), path).toUtf8());
#endif
}

void Application::initEnv() const
{
#ifdef Q_OS_LINUX
    addToPath("/usr/share/apk-icon-editor", false);
#endif
    addToPath(applicationDirPath(), false);
    const QString JAVA_HOME = qgetenv("JAVA_HOME");
    if (!JAVA_HOME.isEmpty()) {
        addToPath(JAVA_HOME + "/bin");
    }
}

bool Application::initLog() const
{
    if (!QDir().mkpath(Path::Log::DIR)) {
        return false;
    }

    log = new QFile(Path::Log::FILE);
    if (log->open(QIODevice::WriteOnly | QIODevice::Text)) {
        qInstallMessageHandler(Application::msgHandler);
        qDebug() << qPrintable(QString("%1 v%2").arg(APP, VER));
        qDebug() << "Initializing...";
        return true;
    }
    else {
        return false;
    }
}

void Application::initFonts() const
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QFontDatabase::addApplicationFont(":/fonts/OpenSans-Light.ttf");
}

void Application::msgHandler(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
    QTextStream stream(log);
    const QString TIME = QTime::currentTime().toString("HH:mm:ss.zzz");
    QString line;

    switch (type) {
    case QtDebugMsg:
        line = QString("[%1] %2\n").arg(TIME, msg);
        stream << line;
        fprintf(stderr, "%s", qPrintable(line));
        break;
    case QtWarningMsg:
        line = QString("[%1] Warning: %2\n").arg(TIME, msg);
        stream << line;
        fprintf(stderr, "%s", qPrintable(line));
        break;
    case QtCriticalMsg:
        line = QString("[%1] Critical: %2\n").arg(TIME).arg(msg);
        stream << line;
        fprintf(stderr, "%s", qPrintable(line));
        break;
    case QtFatalMsg:
        line = QString("[%1] Fatal: %2\n").arg(TIME, msg);
        stream << line;
        fprintf(stderr, "%s", qPrintable(line));
        abort();
    default:
        line = QString("[%1] %2\n").arg(TIME, msg);
        stream << line;
        fprintf(stderr, "%s", qPrintable(line));
        break;
    }
    fflush(stderr);
}

bool Application::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::FileOpen:
        window->apk_open(static_cast<QFileOpenEvent*>(event)->file());
        return true;
    default:
        return QApplication::event(event);
    }
}

Application::~Application()
{
    delete window;
    delete log;
}
