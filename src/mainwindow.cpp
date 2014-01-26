#include "mainwindow.h"
#include "main.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QColorDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QDate>
#include <QDesktopServices>
#include <QApplication>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) : QMainWindow(parent)
{
    resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setAcceptDrops(true);

    apk = new Apk();
    updater = new Updater(this);
    translator = new QTranslator(this);
    translatorQt = new QTranslator(this);
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "apk-icon-editor", "config", this);

    QWidget *w = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(w);
    setCentralWidget(w);

#ifdef QT5_2_0
    taskbar = new QWinTaskbarButton(this);
#endif

    QMenuBar *menu = new QMenuBar(this);
    setMenuBar(menu);
    menuFile = new QMenu(this);
    menuIcon = new QMenu(this);
    menuSett = new QMenu(this);
    menuHelp = new QMenu(this);
    menu->addMenu(menuFile);
    menu->addMenu(menuIcon);
    menu->addMenu(menuSett);
    menu->addMenu(menuHelp);
    actApkOpen = new QAction(this);
    actApkSave = new QAction(this);
    menuRecent = new QMenu(this);
    actExit = new QAction(this);
    actRecentClear = new QAction(this);
    actNoRecent = new QAction(this);
    actIconOpen = new QAction(this);
    actIconSave = new QAction(this);
    actIconScale = new QAction(this);
    actIconResize = new QAction(this);
    actIconRevert = new QAction(this);
    actIconBack = new QAction(this);
    menuLang = new QMenu(this);
    actAutoUpdate = new QAction(this);
    actAssoc = new QAction(this);
    actWebsite = new QAction(this);
    actReport = new QAction(this);
    actUpdate = new QAction(this);
    actAboutQt = new QAction(this);
    actAbout = new QAction(this);
    menuFile->addAction(actApkOpen);
    menuFile->addMenu(menuRecent);
    menuFile->addSeparator();
    menuFile->addAction(actApkSave);
    menuFile->addSeparator();
    menuFile->addAction(actExit);
    menuIcon->addAction(actIconOpen);
    menuIcon->addAction(actIconSave);
    menuIcon->addAction(actIconScale);
    menuIcon->addAction(actIconResize);
    menuIcon->addAction(actIconRevert);
    menuFile->addSeparator();
    menuIcon->addAction(actIconBack);
    menuSett->addMenu(menuLang);
    menuSett->addAction(actAutoUpdate);
    menuSett->addAction(actAssoc);
    menuHelp->addAction(actWebsite);
    menuHelp->addAction(actReport);
    menuHelp->addSeparator();
    menuHelp->addAction(actUpdate);
    menuHelp->addSeparator();
    menuHelp->addAction(actAboutQt);
    menuHelp->addAction(actAbout);
    actApkSave->setEnabled(false);
    actNoRecent->setEnabled(false);
    actIconOpen->setEnabled(false);
    actIconSave->setEnabled(false);
    actIconScale->setEnabled(false);
    actIconScale->setVisible(false); // not sure about its necessity
    actIconResize->setEnabled(false);
    actIconRevert->setEnabled(false);
    actAutoUpdate->setCheckable(true);
    actApkSave->setIcon(QIcon(":/gfx/task-pack.png"));
    actApkOpen->setShortcut(QKeySequence("Ctrl+O"));
    actApkSave->setShortcut(QKeySequence("Ctrl+E"));
    actIconOpen->setShortcut(QKeySequence("Ctrl+R"));
    actIconSave->setShortcut(QKeySequence("Ctrl+S"));
    actIconResize->setShortcut(QKeySequence("Ctrl+I"));
    actIconRevert->setShortcut(QKeySequence("Ctrl+Z"));
    actReport->setIcon(QIcon(":/gfx/bug.png"));
    actExit->setShortcut(QKeySequence("Ctrl+Q"));

    drawArea = new DrawArea(this);
    QAction *separator1 = new QAction(this);
    QAction *separator2 = new QAction(this);
    separator1->setSeparator(true);
    separator2->setSeparator(true);
    drawArea->setContextMenuPolicy(Qt::ActionsContextMenu);
    drawArea->addAction(actApkOpen);
    drawArea->addAction(separator1);
    drawArea->addAction(actIconOpen);
    drawArea->addAction(actIconSave);
    drawArea->addAction(actIconScale);
    drawArea->addAction(actIconResize);
    drawArea->addAction(actIconRevert);
    drawArea->addAction(separator2);
    drawArea->addAction(actIconBack);
    drawArea->setSizePolicy(QSizePolicy::MinimumExpanding,
                            QSizePolicy::MinimumExpanding);

    progress = new QProgressDialog(this);
    progress->setRange(0, 100);
    progress->setModal(true);
    progress->setFixedSize(200, 62);
    progress->setAutoClose(false);
    progress->setAutoReset(false);
    progress->setCancelButton(NULL);
    progress->setWindowFlags(progress->windowFlags()
                             & ~Qt::WindowContextHelpButtonHint
                             & ~Qt::WindowCloseButtonHint);

    profiles = new ComboList(this);

    btnPack = new QPushButton(this);
    btnPack->setEnabled(false);
    btnPack->setFixedHeight(64);

    QVBoxLayout *layoutProfile = new QVBoxLayout();
    layoutProfile->addWidget(profiles);
    layoutProfile->addWidget(btnPack);

    layout->addWidget(drawArea);
    layout->addLayout(layoutProfile);
    layout->setStretch(0, 2);
    layout->setStretch(1, 1);

    mapRecent = new QSignalMapper(this);

    initLanguages();
    initProfiles();
    restoreSettings();
    hideEmptyDpi();

    connect(drawArea, SIGNAL(clicked()), this, SLOT(apkLoad()));
    connect(profiles, SIGNAL(groupChanged(int)), this, SLOT(hideEmptyDpi()));
    connect(profiles, SIGNAL(itemChanged(int)), this, SLOT(setCurrentIcon(int)));
    connect(actApkOpen, SIGNAL(triggered()), this, SLOT(apkLoad()));
    connect(actApkSave, SIGNAL(triggered()), this, SLOT(apkSave()));
    connect(actExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actRecentClear, SIGNAL(triggered()), this, SLOT(clearRecent()));
    connect(actIconOpen, SIGNAL(triggered()), this, SLOT(iconOpen()));
    connect(actIconSave, SIGNAL(triggered()), this, SLOT(iconSave()));
    connect(actIconScale, SIGNAL(triggered()), this, SLOT(iconScale()));
    connect(actIconResize, SIGNAL(triggered()), this, SLOT(iconResize()));
    connect(actIconRevert, SIGNAL(triggered()), this, SLOT(iconRevert()));
    connect(actIconBack, SIGNAL(triggered()), this, SLOT(setPreviewColor()));
    connect(actAssoc, SIGNAL(triggered()), this, SLOT(associate()));
    connect(actWebsite, SIGNAL(triggered()), this, SLOT(browseSite()));
    connect(actReport, SIGNAL(triggered()), this, SLOT(browseBugs()));
    connect(actUpdate, SIGNAL(triggered()), updater, SLOT(check()));
    connect(actAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(btnPack, SIGNAL(clicked()), this, SLOT(apkSave()));
    connect(mapRecent, SIGNAL(mapped(QString)), this, SLOT(apkLoad(QString)));
    connect(apk, SIGNAL(loading(short, QString)), this, SLOT(loading(short, QString)), Qt::BlockingQueuedConnection);
    connect(apk, SIGNAL(success(QString, QString)), this, SLOT(success(QString, QString)));
    connect(apk, SIGNAL(warning(QString, QString)), this, SLOT(warning(QString, QString)));
    connect(apk, SIGNAL(error(QString, QString)), this, SLOT(error(QString, QString)));
    connect(apk, SIGNAL(error(QString, QString)), this, SLOT(loadingComplete()));
    connect(apk, SIGNAL(packed(QString)), this, SLOT(apkPacked(QString)));
    connect(apk, SIGNAL(unpacked(QString)), this, SLOT(apkUnpacked(QString)));
    connect(updater, SIGNAL(version(QString)), this, SLOT(newVersion(QString)));
    connect(this, SIGNAL(destroyed()), apk, SLOT(clearTemp()));

    if (actAutoUpdate->isChecked()) {
        updater->check();
    }

    if (argc > 1) {
        apkLoad(QString::fromLocal8Bit(argv[1]));
    }
}

void MainWindow::initLanguages()
{
    mapLang = new QSignalMapper(this);
    connect(mapLang, SIGNAL(mapped(QString)), this, SLOT(setLanguage(QString)));

    // Loading language list:
    const QDir LANGPATH(QApplication::applicationDirPath() + "/lang");
    QStringList langs = LANGPATH.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (int i = 0; i < langs.size(); ++i) {

        // Get native language title:
        QString locale = langs[i].left(2);
        QString lang = QLocale(locale).nativeLanguageName();
        if (lang.size() > 1) {
            lang[0] = lang[0].toUpper();
        }

        // Set up menu action:
        QAction *actLang = new QAction(QIcon(QString("%1/%2/flag.png").arg(LANGPATH.absolutePath(), locale)), lang, this);
        connect(actLang, SIGNAL(triggered()), mapLang, SLOT(map()));
        mapLang->setMapping(actLang, locale);
        menuLang->addAction(actLang);
    }
}

void MainWindow::initProfiles()
{
    Profile::init();
    for (int i = 0; i < Profile::count(); ++i) {
        Profile profile = Profile::at(i);
        profiles->addGroup(profile.title(), profile.thumb());
        for (int j = 0; j <= XXHDPI; ++j) {
            Dpi DPI = static_cast<Dpi>(j);
            profiles->addItem(profile.title(), profile.getDpiTitle(DPI));
        }
    }
}

void MainWindow::restoreSettings()
{
    QString locale = QLocale::system().name().left(2);

    // Read settings:
    QString sProfile = settings->value("Profile", NULL).toString();
    QString sLanguage = settings->value("Language", locale).toString();
    QString sLastDir = settings->value("Directory", "").toString();
    QByteArray sGeometry = settings->value("Geometry", NULL).toByteArray();
    QStringList sRecent = settings->value("Recent", NULL).toStringList();
    bool sUpdate = settings->value("Update", true ).toBool();

    // Restore settings:
    profiles->setCurrentGroup(sProfile);
    if (!sRecent.isEmpty()) {
        recent = sRecent;
        refreshRecent();
    }
    else {
        clearRecent();
    }

    currentPath = sLastDir;
    restoreGeometry(sGeometry);
    setLanguage(sLanguage);
    actAutoUpdate->setChecked(sUpdate);
}

void MainWindow::setLanguage(QString lang)
{
    QString LANGPATH(QApplication::applicationDirPath() + "/lang/" + lang);
    QApplication::removeTranslator(translator);
    QApplication::removeTranslator(translatorQt);
    if (translator->load("apk-icon-editor", LANGPATH)) {
        translatorQt->load("qt", LANGPATH);
        QApplication::installTranslator(translator);
        QApplication::installTranslator(translatorQt);
    }
    else {
        lang = "en";
    }

    currentLang = lang;
    menuLang->setIcon(QIcon(QString("%1/flag.png").arg(LANGPATH)));

    // Retranslate strings:
    drawArea->setText(tr("CLICK HERE\n(or drag APK and icons)"));
    profiles->setLabelText(tr("Size Profile:"));
    btnPack->setText(tr("Pack APK"));
    menuFile->setTitle(tr("&File"));
    menuIcon->setTitle(tr("&Icon"));
    menuSett->setTitle(tr("&Settings"));
    menuHelp->setTitle(tr("&Help"));
    actApkOpen->setText(tr("&Open APK"));
    actApkSave->setText(tr("&Export (Pack) APK"));
    menuRecent->setTitle(tr("&Recent APKs"));
    actExit->setText(tr("E&xit"));
    actRecentClear->setText(tr("&Clear List"));
    actNoRecent->setText(tr("No Recent Files"));
    actIconOpen->setText(tr("Replace &Icon"));
    actIconSave->setText(tr("&Save Icon"));
    actIconScale->setText(tr("Scale to &Fit"));
    actIconResize->setText(tr("&Resize Icon"));
    actIconRevert->setText(tr("Revert &Original"));
    actIconBack->setText(tr("Preview Background &Color"));
    menuLang->setTitle(tr("&Language"));
    actAutoUpdate->setText(tr("Auto-check for Updates"));
    actAssoc->setText(tr("Associate .APK"));
    actWebsite->setText(tr("Visit Website"));
    actReport->setText(tr("Report a Bug"));
    actUpdate->setText(tr("Check for &Updates"));
    actAboutQt->setText(tr("About Qt"));
    actAbout->setText(tr("About %1").arg(APP));
    progress->setWindowTitle(tr("Processing"));
    apk->retranslate();
}

void MainWindow::addToRecent(QString filename)
{
    if (!filename.isNull()) {
        filename = QDir::toNativeSeparators(filename);
        recent.push_front(filename);
        recent.removeDuplicates();
        if (recent.size() > 10) {
            recent.pop_back();
        }
    }
    refreshRecent();
}

void MainWindow::refreshRecent()
{
    menuRecent->clear();
    for (short i = 0; i < recent.size(); ++i) {
        QAction *actRecent = new QAction(recent[i], menuRecent);
        menuRecent->addAction(actRecent);
        connect(actRecent, SIGNAL(triggered()), mapRecent, SLOT(map()));
        mapRecent->setMapping(actRecent, recent[i]);
    }
    menuRecent->addSeparator();
    menuRecent->addAction(actRecentClear);
}

void MainWindow::hideEmptyDpi()
{
    for (short i = LDPI; i <= XXHDPI; ++i) {
        bool visible = false;
        const Dpi DPI = static_cast<Dpi>(i);
        if (Icon *icon = apk->getIcon(DPI)) {
            visible = !icon->isNull();
        }
        profiles->setItemVisible(DPI, visible);
    }
}

void MainWindow::setCurrentIcon(int id)
{
    if (id == -1) {
#ifdef QT_DEBUG
        // currentRowChanged() signal is fired twice. Qt bug?
        qDebug() << "BUG #001";
#endif
        return;
    }
    Profile profile = Profile::at(profiles->currentGroupIndex());
    int side = profile.getDpiSide(static_cast<Dpi>(id));
    drawArea->setRect(side, side);
    if (Icon *icon = apk->getIcon(static_cast<Dpi>(id))) {
        drawArea->setIcon(icon);
    }
}

void MainWindow::setActiveApk(QString filename)
{
    setWindowModified(false);
    setWindowTitle(QFileInfo(filename).fileName() + "[*]");
    addToRecent(filename);
}

void MainWindow::loadingComplete(bool success, QString filename)
{
    progress->close();
#ifdef QT5_2_0
    taskbar->clearOverlayIcon();
    QWinTaskbarProgress *taskProgress = taskbar->progress();
    taskProgress->setVisible(false);
    taskProgress->reset();
#endif
    activateWindow();

    if (success) {
        setActiveApk(filename);
    }
}

void MainWindow::apkPacked(QString filename)
{
    loadingComplete(true, filename);
}

void MainWindow::apkUnpacked(QString filename)
{
    loadingComplete(true, filename);
    int id = profiles->currentItemIndex();
    if (id == -1) {
        id = 0;
    }
    profiles->setCurrentItem(id);
    hideEmptyDpi();

    // Enable operations with APK:
    actApkSave->setEnabled(true);
    actIconOpen->setEnabled(true);
    actIconSave->setEnabled(true);
    actIconRevert->setEnabled(true);
    actIconResize->setEnabled(true);
    actIconScale->setEnabled(true);
    btnPack->setEnabled(true);
}

void MainWindow::clearRecent()
{
    recent.clear();
    menuRecent->clear();
    menuRecent->addSeparator();
    menuRecent->addAction(actNoRecent);
}

bool MainWindow::iconOpen(QString filename)
{
    if (drawArea->getIcon()->isNull()) {
        invalidDpi();
        return false;
    }

    if (filename.isEmpty()) {
        const QString SUPPORTED = " (*." + EXT_GFX.join(" *.") + ");;";
        filename = QFileDialog::getOpenFileName(this, tr("Import Icon"), NULL,
                                                tr("Supported Formats") + SUPPORTED
                                                + FILTER_GFX + ";;"
                                                + tr("All Files"));
        if (filename.isEmpty()) {
            return false;
        }
    }

    Icon *icon = drawArea->getIcon();
    QPixmap backup = icon->getPixmap();

    if (icon->replace(QPixmap(filename))) {
        Profile profile = Profile::at(profiles->currentGroupIndex());
        int side = profile.getDpiSide(static_cast<Dpi>(profiles->currentItemIndex()));
        if (icon->width() != side || icon->height() != side) {
            int result = QMessageBox::warning(this, tr("Resize?"),
                                      tr("Icon you are trying to load is off-size.\nResize automatically?"),
                                      QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
            switch (result) {
            case QMessageBox::Yes:
                icon->resize(side);
                break;
            case QMessageBox::No:
                break;
            default:
                // Restore previous icon:
                icon->replace(backup);
                return false;
            }
        }
        setWindowModified(true);
        return true;
    }
    else {
        warning(tr("Can't Load Icon"), tr("You are trying to load invalid or unsupported icon."));
        return false;
    }
}

bool MainWindow::iconSave(QString filename)
{
    if (drawArea->getIcon()->isNull()) {
        invalidDpi();
        return false;
    }

    if (filename.isEmpty()) {
        filename = QFileDialog::getSaveFileName(this, tr("Save Icon"), NULL, FILTER_GFX);
        if (filename.isEmpty()) {
            return false;
        }
    }
    return drawArea->getIcon()->save(filename);
}

void MainWindow::iconScale()
{
    Profile profile = Profile::at(profiles->currentGroupIndex());
    int side = profile.getDpiSide(static_cast<Dpi>(profiles->currentItemIndex()));
    iconResize(side);
}

bool MainWindow::iconResize(int side)
{
    if (drawArea->getIcon()->isNull()) {
        invalidDpi();
        return false;
    }

    if (side == 0) {
        bool ok;
        side = QInputDialog::getInt(this, tr("Resize Icon"), tr("Icon side (in pixels):"), drawArea->getIcon()->width(), 1, 4096, 1, &ok);
        if (!ok) {
            return false;
        }
    }
    setWindowModified(true);
    return drawArea->getIcon()->resize(side);
}

bool MainWindow::iconRevert()
{
    if (drawArea->getIcon()->isNull()) {
        invalidDpi();
        return false;
    }
    bool result = drawArea->getIcon()->revert();
    drawArea->repaint();
    return result;
}

void MainWindow::setPreviewColor()
{
    QColor def = drawArea->palette().color(QPalette::Background);
    QColor color = QColorDialog::getColor(def, this);
    if (color.isValid()) {
        drawArea->setBack(color);
        QPixmap icon(32, 32);
        icon.fill(color);
        actIconBack->setIcon(QIcon(icon));
    }
}

void MainWindow::apkLoad(QString filename)
{
    if (confirmExit()) {
        return;
    }

    // Open dialog:
    if (filename.isEmpty()) {
        if ((filename = QFileDialog::getOpenFileName(this, tr("Open APK"), currentPath, "APK (*.apk);;" + tr("All Files"))).isEmpty()) {
            return;
        }
    }

    // Opening file:
    QFile file(filename);
    if (!file.exists()) {
        error(tr("File not found"), tr("Could not find APK:\n%1").arg(filename));
        recent.removeOne(filename);
        refreshRecent();
        return;
    }

    currentPath = QFileInfo(filename).absolutePath();

    // Unpacking:
    progress->setValue(0);
    apk->unpack(filename);
}

void MainWindow::apkSave()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Pack APK"),
                                                    currentPath, "APK (*.apk)");
    if (!filename.isEmpty()) {
        currentPath = QFileInfo(filename).absolutePath();
        progress->setValue(0);
        apk->pack(filename);
    }
}

void MainWindow::associate() const
{
#ifdef Q_OS_WIN32
    QString exe = QDir::toNativeSeparators(QApplication::applicationFilePath());
    QSettings reg("HKEY_CURRENT_USER\\Software\\Classes", QSettings::NativeFormat);
    reg.setValue("apk-icon-editor.apk/DefaultIcon/Default", exe + ",1");
    reg.setValue("apk-icon-editor.apk/Shell/Open/Command/Default", exe + " \"%1\"");
    reg.setValue(".apk/Default", "apk-icon-editor.apk");
#endif
}

void MainWindow::browseSite() const
{
    QDesktopServices::openUrl(QUrl(URL_WEBSITE));
}

void MainWindow::browseBugs() const
{
    QDesktopServices::openUrl(QUrl(URL_BUGS));
}

void MainWindow::about()
{
    const QString LINK("<a href=\"%1\">%2</a> - %3");
    /*const QString DATE = QLocale(QLocale::C).toDate(QString(__DATE__).simplified(),
                           QLatin1String("MMM d yyyy")).toString(Qt::ISODate);*/

    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle(tr("About"));
    aboutBox.setText(
        QString("<h3>%1 v%2</h3>").arg(APP, VER) +
        "<p>" +
            tr("Built on: %1 - %2").arg(__DATE__, __TIME__) + "<br>" +
            tr("Author: %1").arg("Alexander Gorishnyak") + "<br>" +
            tr("License") + ": <a href=\"http://www.gnu.org/licenses/gpl-2.0.html\">GNU General Public License v2.0</a>" +
        "</p><p>" +
            tr("APK Icon Editor is the tool designed to easily edit and replace<br>APK (Android Package) icons. Written in C++ and Qt framework.") +
        "</p><p>" +
            tr("<b>Usage:</b><br>Open the APK file in which you would like to replace icon(s).<br>"
            "Select profile (according to the final device) and the needed DPI.<br>"
            "If you are in doubt about the DPI, you may replace all of them.<br>"
            "APK Icon Editor will ask to scale icons with the wrong size, but<br>"
            "note that Android devices are usually able to do it automatically.") +
        "</p><p>" +
            LINK.arg(URL_WEBSITE, tr("Visit Website"), tr("Visit our official website.")) + "<br>" +
            LINK.arg(URL_BUGS, tr("Report a Bug"), tr("Found a bug? Let us know so we can fix it!")) + "<br>" +
            LINK.arg(URL_TRANSLATE, tr("Help Translate"), tr("Join our translation team on Transifex.")) + "<br>" +
            LINK.arg(QString("file:///%1/authors.txt")
                .arg(QApplication::applicationDirPath()), tr("List of Authors"), tr("Contributors, translators, testers and helpers.")) +
        "</p>"
    );
    aboutBox.setIconPixmap(QPixmap(":/gfx/logo.png"));
    aboutBox.exec();
}

void MainWindow::aboutQt() const
{
    QApplication::aboutQt();
}

void MainWindow::loading(short percentage, QString text)
{
    progress->setLabelText(text);
    progress->setValue(percentage);
    progress->show();
#ifdef QT5_2_0
    taskbar->setWindow(windowHandle());
    taskbar->setOverlayIcon(QIcon(":/gfx/task-pack.png"));
    QWinTaskbarProgress *taskProgress = taskbar->progress();
    taskProgress->setValue(percentage);
    taskProgress->setVisible(true);
#endif
}

void MainWindow::invalidDpi()
{
    warning(tr("Icon Missing"),
            tr("This APK does not support current DPI."));
}

bool MainWindow::newVersion(QString version)
{
    QMessageBox msgBox(
                QMessageBox::Information,
                tr("Update"),
                tr("%1 v%2 is available.\nDownload?")
                .arg(APP, version),
                QMessageBox::Yes | QMessageBox::No,
                this);

    if (msgBox.exec() == QMessageBox::Yes) {
        updater->download();
        return true;
    }
    else {
        return false;
    }
}

void MainWindow::success(QString title, QString text)
{
    QMessageBox::information(this, title, text);
}

void MainWindow::warning(QString title, QString text)
{
    QMessageBox::warning(this, title, text);
}

void MainWindow::error(QString title, QString text)
{
    QMessageBox::critical(this, title, text);
}

bool MainWindow::confirmExit()
{
    if (isWindowModified()) {
        return (QMessageBox::question(this, tr("Discard Changes?"),
                                      tr("APK has been modified. Discard changes?"),
                                      QMessageBox::Discard, QMessageBox::Cancel)
                == QMessageBox::Cancel);
    }
    else {
        return false;
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasText()) {
        QUrl url(mimeData->text());
        QString filename = url.toLocalFile();
        QString ext = QFileInfo(filename).suffix().toLower();
        if (EXT_GFX.contains(ext)) {
            iconOpen(filename);
        }
        else {
            apkLoad(filename);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Confirm exit:
    if (confirmExit()) {
        event->ignore();
        return;
    }

    // Save settings:
    settings->setValue("Profile", profiles->currentGroupText());
    settings->setValue("Language", currentLang);
    settings->setValue("Update", actAutoUpdate->isChecked());
    settings->setValue("Directory", currentPath);
    settings->setValue("Geometry", saveGeometry());
    settings->setValue("Recent", recent);

    // Close window:
    event->accept();
}

MainWindow::~MainWindow()
{
    delete apk;
}
