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
    setApplicationDisplayName(QString("%1 v%2 Debug").arg(APP, VER));
#elif defined(PORTABLE)
    setApplicationDisplayName(QString("%1 v%2 Portable").arg(APP, VER));
#else
    setApplicationDisplayName(QString("%1 v%2").arg(APP, VER));
#endif

#ifdef Q_OS_LINUX
    QIcon icon;
    icon.addPixmap(QPixmap(":/gfx/icon/16.png"));
    icon.addPixmap(QPixmap(":/gfx/icon/24.png"));
    icon.addPixmap(QPixmap(":/gfx/icon/32.png"));
    setWindowIcon(icon);
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
#ifdef Q_OS_WIN
    const QString FORMAT = append ? "%1;%2" : "%2;%1";
#else
    const QString FORMAT = append ? "%1:%2" : "%2:%1";
#endif
    qputenv("PATH", QString(FORMAT).arg(QString(qgetenv("PATH")), path).toUtf8());
}

void Application::initEnv() const
{
    addToPath(Path::App::dir(), false);
    const QString JAVA_HOME = qgetenv("JAVA_HOME");
    if (!JAVA_HOME.isEmpty()) {
        addToPath(JAVA_HOME + "/bin");
    }
}

bool Application::initLog() const
{
    if (!QDir().mkpath(Path::Log::dir())) {
        return false;
    }

    log = new QFile(Path::Log::file());
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
    case QtInfoMsg:
        line = QString("[%1]  %2\n").arg(TIME).arg(msg);
        ts << line;
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
