#include "mainwindow.h"
#include "main.h"
#include "settings.h"
#include <QHeaderView>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QMimeData>
#include <QTextCodec>
#include <QDesktopServices>
#include <QtConcurrent/QtConcurrentRun>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    init_core();
    init_gui();
    init_languages();
    init_devices();
    init_slots();

    QtConcurrent::run(this, &MainWindow::checkDeps);
    Settings::init();

    settings_load();

    if (actAutoUpdate->isChecked()) {
        updater->check();
    }
}

void MainWindow::checkDeps()
{
    const QString JRE     = Apk::getJreVersion();
    const QString JDK     = Apk::getJdkVersion();
    version_apktool       = Apk::getApktoolVersion();

    QRegExp rx;
    QStringList cap;

    rx.setPattern("java version \"(.+)\"");
    rx.indexIn(JRE);
    cap = rx.capturedTexts();
    if (cap.size() > 1) version_jre = cap[1];

    rx.setPattern("javac version \"(.+)\"");
    rx.indexIn(JDK);
    cap = rx.capturedTexts();
    if (cap.size() > 1) version_jdk = cap[1];

    qDebug() << "JRE (32-bit) version:" << qPrintable(!JRE.isNull() ? version_jre : "---");
    qDebug() << "JDK (32-bit) version:" << qPrintable(!JDK.isNull() ? version_jdk : "---");
    qDebug() << "Apktool version:" << qPrintable(!version_apktool.isNull() ? version_apktool : "---") << '\n';

    if (!JRE.isNull()) qDebug().nospace() << "java -version\n" << qPrintable(JRE) << '\n';
    if (!JDK.isNull()) qDebug().nospace() << "javac -version\n" << qPrintable(JDK) << '\n';
}

void MainWindow::init_core()
{
    apk = new Apk(this);
    updater = new Updater(this);
    mapRecent = new QSignalMapper(this);

    dropbox = new Dropbox(this);
    gdrive = new GoogleDrive(this);
    onedrive = new OneDrive(this);
    dropbox->setIcon(QPixmap(":/gfx/clouds/dropbox.png"));
    gdrive->setIcon(QPixmap(":/gfx/clouds/gdrive.png"));
    onedrive->setIcon(QPixmap(":/gfx/clouds/onedrive.png"));
}

void MainWindow::init_gui()
{
    // Dialogs:

    effects = new EffectsDialog(this);
    toolDialog = new ToolDialog(this);
    keyManager = new KeyManager(this);

    // Main Window:

    setAcceptDrops(true);
    resize(Gui::Window::WIDTH,
           Gui::Window::HEIGHT);

    splitter = new QSplitter(this);
    setCentralWidget(splitter);

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

    labelTool = new QLabel(this);
    btnTool = new QToolButton(this);
    QWidget *menuCorner = new QWidget(this);
    QHBoxLayout *layoutCorner = new QHBoxLayout(menuCorner);
    layoutCorner->addWidget(labelTool);
    layoutCorner->addWidget(btnTool);
    layoutCorner->setMargin(0);
    menu->setCornerWidget(menuCorner);

    actApkOpen = new QAction(this);
    actApkSave = new QAction(this);
    menuRecent = new QMenu(this);
    actApkExplore = new QAction(this);
    actExit = new QAction(this);
    actRecentClear = new QAction(this);
    actNoRecent = new QAction(this);
    actIconOpen = new QAction(this);
    actIconSave = new QAction(this);
    actIconScale = new QAction(this);
    actIconResize = new QAction(this);
    actIconRevert = new QAction(this);
    actIconEffect = new QAction(this);
    actIconBack = new QAction(this);
    actPacking = new QAction(this);
    actKeys = new QAction(this);
    menuLang = new QMenu(this);
    actAutoUpdate = new QAction(this);
    actAssoc = new QAction(this);
    actReset = new QAction(this);
    actFaq = new QAction(this);
    actWebsite = new QAction(this);
    actReport = new QAction(this);
    actDonate = new QAction(this);
    menuLogs = new QMenu(this);
    actLogFile = new QAction(this);
    actLogPath = new QAction(this);
    actUpdate = new QAction(this);
    actAboutQt = new QAction(this);
    actAbout = new QAction(this);

    menuFile->addAction(actApkOpen);
    menuFile->addMenu(menuRecent);
    menuFile->addSeparator();
    menuFile->addAction(actApkExplore);
    menuFile->addSeparator();
    menuFile->addAction(actApkSave);
    menuFile->addSeparator();
    menuFile->addAction(actExit);
    menuIcon->addAction(actIconOpen);
    menuIcon->addAction(actIconSave);
    menuIcon->addAction(actIconScale);
    menuIcon->addAction(actIconResize);
    menuIcon->addAction(actIconRevert);
    menuIcon->addAction(actIconEffect);
    menuIcon->addSeparator();
    menuIcon->addAction(actIconBack);
    menuSett->addAction(actPacking);
    menuSett->addAction(actKeys);
    menuSett->addSeparator();
#ifndef Q_OS_OSX
    menuSett->addMenu(menuLang);
#endif
    menuSett->addAction(actAutoUpdate);
    menuSett->addSeparator();
#ifndef Q_OS_UNIX
    menuSett->addAction(actAssoc);
#endif
    menuSett->addAction(actReset);
    menuHelp->addAction(actFaq);
    menuHelp->addSeparator();
    menuHelp->addAction(actWebsite);
    menuHelp->addAction(actReport);
    menuHelp->addAction(actDonate);
    menuHelp->addSeparator();
    menuHelp->addMenu(menuLogs);
    menuLogs->addAction(actLogFile);
    menuLogs->addAction(actLogPath);
    menuHelp->addSeparator();
    menuHelp->addAction(actUpdate);
    menuHelp->addSeparator();
    menuHelp->addAction(actAboutQt);
    menuHelp->addAction(actAbout);
    actApkExplore->setEnabled(false);
    actApkSave->setEnabled(false);
    actNoRecent->setEnabled(false);
    actIconOpen->setEnabled(false);
    actIconSave->setEnabled(false);
    actIconScale->setEnabled(false);
    actIconResize->setEnabled(false);
    actIconRevert->setEnabled(false);
    actIconEffect->setEnabled(false);
    actAutoUpdate->setCheckable(true);

    actApkOpen->setShortcut(QKeySequence("Ctrl+O"));
    actApkExplore->setShortcut(QKeySequence("Ctrl+D"));
    actApkSave->setShortcut(QKeySequence("Ctrl+E"));
    actIconOpen->setShortcut(QKeySequence("Ctrl+R"));
    actIconSave->setShortcut(QKeySequence("Ctrl+S"));
    actIconScale->setShortcut(QKeySequence("Ctrl+W"));
    actIconResize->setShortcut(QKeySequence("Ctrl+I"));
    actIconRevert->setShortcut(QKeySequence::Undo);
    actIconEffect->setShortcut(QKeySequence("Ctrl+F"));
    actPacking->setShortcut(QKeySequence("Ctrl+P"));
    actKeys->setShortcut(QKeySequence("Ctrl+K"));
    actFaq->setShortcut(QKeySequence::HelpContents);
    actLogPath->setShortcut(QKeySequence("Ctrl+L"));
    actExit->setShortcut(QKeySequence("Ctrl+Q"));

    actApkOpen->setIcon(QIcon(":/gfx/actions/open.png"));
    menuRecent->setIcon(QIcon(":/gfx/actions/open-list.png"));
    actRecentClear->setIcon(QIcon(":/gfx/actions/close.png"));
    actApkExplore->setIcon(QIcon(":/gfx/actions/explore.png"));
    actApkSave->setIcon(QIcon(":/gfx/actions/pack.png"));
    actIconOpen->setIcon(QIcon(":/gfx/actions/open-icon.png"));
    actIconSave->setIcon(QIcon(":/gfx/actions/save.png"));
    actIconScale->setIcon(QIcon(":/gfx/actions/scale.png"));
    actIconResize->setIcon(QIcon(":/gfx/actions/resize.png"));
    actIconRevert->setIcon(QIcon(":/gfx/actions/undo.png"));
    actIconEffect->setIcon(QIcon(":/gfx/actions/effects.png"));
    actPacking->setIcon(QIcon(":/gfx/actions/box.png"));
    actKeys->setIcon(QIcon(":/gfx/actions/key.png"));
    actAutoUpdate->setIcon(QIcon(":/gfx/actions/update.png"));
    actAssoc->setIcon(QIcon(":/gfx/actions/associate.png"));
    actReset->setIcon(QIcon(":/gfx/actions/reset.png"));
    actFaq->setIcon(QIcon(":/gfx/actions/help.png"));
    actWebsite->setIcon(QIcon(":/gfx/actions/world.png"));
    actReport->setIcon(QIcon(":/gfx/actions/bug.png"));
    actDonate->setIcon(QIcon(":/gfx/actions/donate.png"));
    menuLogs->setIcon(QIcon(":/gfx/actions/file.png"));
    actLogFile->setIcon(QIcon(":/gfx/actions/file.png"));
    actLogPath->setIcon(QIcon(":/gfx/actions/open-list.png"));
    actUpdate->setIcon(QIcon(":/gfx/actions/update.png"));
    actAbout->setIcon(QIcon(":/gfx/actions/logo.png"));
    actAboutQt->setIcon(QIcon(":/gfx/actions/qt.png"));
    actExit->setIcon(QIcon(":/gfx/actions/close.png"));

    drawArea = new DrawArea(this);
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    drawArea->setContextMenuPolicy(Qt::ActionsContextMenu);
    drawArea->addAction(actApkOpen);
    drawArea->addAction(separator);
    drawArea->addActions(menuIcon->actions());

    loadingDialog = new ProgressDialog(this);
    loadingDialog->setIcon(QPixmap(":/gfx/actions/box.png"));
    loadingDialog->setAllowCancel(false);

    uploadDialog = new ProgressDialog(this);

    QWidget *tabIcons = new QWidget(this);
    QVBoxLayout *layoutIcons = new QVBoxLayout(tabIcons);
    devices = new ComboList(this);
    devices->addActions(menuIcon->actions());
    devices->setEnabled(false);
    btnApplyIcons = new QPushButton(this);
    btnApplyIcons->setEnabled(false);
    layoutIcons->addWidget(devices);
    layoutIcons->addWidget(btnApplyIcons);
    layoutIcons->setMargin(4);

    QWidget *tabStrings = new QWidget(this);
    QGridLayout *layoutStrings = new QGridLayout(tabStrings);
    labelAppName = new QLabel(this);
    labelVersionName = new QLabel(this);
    labelVersionCode = new QLabel(this);
    editAppName = new QLineEdit(this);
    editAppName->setEnabled(false);
    editVersionName = new QLineEdit(this);
    editVersionName->setEnabled(false);
    editVersionCode = new QSpinBox(this);
    editVersionCode->setEnabled(false);
    editVersionCode->setMinimum(1);
    editVersionCode->setMaximum(std::numeric_limits<int>::max());
    btnApplyAppName = new QPushButton(this);
    btnApplyAppName->setEnabled(false);
    tableStrings = new QTableWidget(this);
    tableStrings->setColumnCount(3);
    tableStrings->verticalHeader()->setVisible(false);
    tableStrings->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    tableStrings->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableStrings->horizontalHeader()->setSectionHidden(2, true);
    tableStrings->setSelectionMode(QTableWidget::SingleSelection);
    labelApktool = new QLabel(this);
    labelApktool->setWordWrap(true);
#ifndef Q_OS_UNIX
    labelApktool->setFont(QFont("Open Sans Light", 10));
#endif
    labelApktool->setAlignment(Qt::AlignCenter);
    labelApktool->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    btnRepacking = new QPushButton(this);
    btnRepacking->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    panelApktool = new QWidget(this);
    panelApktool->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    panelApktool->setObjectName("panelApktool");
    panelApktool->setStyleSheet("QWidget#panelApktool {border: 1px solid gray}");
    QVBoxLayout *layoutApktool = new QVBoxLayout(panelApktool);
    layoutApktool->setAlignment(Qt::AlignCenter);
    layoutApktool->addWidget(labelApktool);
    layoutApktool->addWidget(btnRepacking);

    layoutStrings->addWidget(labelAppName,      0, 0, 1, 1);
    layoutStrings->addWidget(editAppName,       1, 0, 1, 1);
    layoutStrings->addWidget(btnApplyAppName,   1, 1, 1, 1);
    layoutStrings->addWidget(tableStrings,      2, 0, 1, 2);
    layoutStrings->addWidget(panelApktool,      3, 0, 1, 2);
    layoutStrings->addWidget(labelVersionName,  4, 0, 1, 1);
    layoutStrings->addWidget(editVersionName,   4, 1, 1, 1);
    layoutStrings->addWidget(labelVersionCode,  5, 0, 1, 1);
    layoutStrings->addWidget(editVersionCode,   5, 1, 1, 1);
    layoutStrings->setMargin(4);

    tabs = new QTabWidget(this);
    tabs->addTab(tabIcons, NULL);
    tabs->addTab(tabStrings, NULL);

    checkDropbox = new QCheckBox(this);
    checkDropbox->setIcon(dropbox->getIcon());
    checkGDrive = new QCheckBox(this);
    checkGDrive->setIcon(gdrive->getIcon());
    checkOneDrive = new QCheckBox(this);
    checkOneDrive->setIcon(onedrive->getIcon());
    checkUpload = new QCheckBox(this);
    checkUpload->setChecked(true);
    checkUpload->setIcon(QPixmap(":/gfx/clouds/upload.png"));
    btnPack = new QPushButton(this);
    btnPack->setEnabled(false);
    btnPack->setFixedHeight(64);

    QWidget *sidebar = new QWidget(this);
    QVBoxLayout *layoutSide = new QVBoxLayout(sidebar);
    layoutSide->setMargin(0);
    layoutSide->addWidget(tabs);
    layoutSide->addWidget(checkDropbox);
    layoutSide->addWidget(checkGDrive);
    layoutSide->addWidget(checkOneDrive);
    layoutSide->addWidget(checkUpload);
    layoutSide->addWidget(btnPack);

    splitter->addWidget(drawArea);
    splitter->addWidget(sidebar);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);
    splitter->setStyleSheet("QSplitter {padding: 8px;}");
}

void MainWindow::init_languages()
{
    translator = new QTranslator(this);
    translatorQt = new QTranslator(this);
    mapLang = new QSignalMapper(this);
    connect(mapLang, SIGNAL(mapped(QString)), this, SLOT(setLanguage(QString)));

    // Add default English:

    QStringList langs;
    langs << "apk-icon-editor.en";

    // Load language list:

    const QDir LANGPATH(Path::App::dir() + "/lang");
    langs << LANGPATH.entryList(QStringList() << "apk-icon-editor.*");

    for (int i = 0; i < langs.size(); ++i) {

        // Get native language title:

        QString locale = langs[i].split('.')[1];
        QString title = QLocale(locale).nativeLanguageName();
        if (title.size() > 1) {
            title[0] = title[0].toUpper();
        }

        // Set up menu action:

        QAction *actLang = new QAction(QIcon(QString("%1/flag.%2.png").arg(LANGPATH.absolutePath(), locale)), title, this);
        connect(actLang, SIGNAL(triggered()), mapLang, SLOT(map()));
        mapLang->setMapping(actLang, locale);
        menuLang->addAction(actLang);
    }

    // Add "Help Translate" action:

    menuLang->addSeparator();
    actTranslate = new QAction(this);
    actTranslate->setIcon(QPixmap(":/gfx/actions/world.png"));
    connect(actTranslate, SIGNAL(triggered()), this, SLOT(browseTranslate()));
    menuLang->addAction(actTranslate);
}

void MainWindow::init_devices()
{
    Devices::init();
    for (int i = 0; i < Devices::count(); ++i) {
        Device device = Devices::at(i);
        devices->addGroup(device.getTitle(), device.getThumb());
        for (short j = Dpi::LDPI; j < Dpi::COUNT; ++j) {
            const Dpi::Type DPI = Dpi::cast(j);
            devices->addItem(device.getTitle(), device.getDpiTitle(DPI));
        }
    }
}

void MainWindow::init_slots()
{
    connect(drawArea, SIGNAL(clicked()), this, SLOT(apk_open()));
    connect(devices, SIGNAL(groupChanged(int)), this, SLOT(hideEmptyDpi()));
    connect(devices, SIGNAL(itemChanged(int)), this, SLOT(setCurrentIcon(int)));
    connect(checkUpload, SIGNAL(toggled(bool)), this, SLOT(enableUpload(bool)));
    connect(actApkOpen, SIGNAL(triggered()), this, SLOT(apk_open()));
    connect(actApkSave, SIGNAL(triggered()), this, SLOT(apk_save()));
    connect(actApkExplore, SIGNAL(triggered()), this, SLOT(apk_explore()));
    connect(actExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actRecentClear, SIGNAL(triggered()), this, SLOT(recent_clear()));
    connect(actIconOpen, SIGNAL(triggered()), this, SLOT(iconOpen()));
    connect(actIconSave, SIGNAL(triggered()), this, SLOT(iconSave()));
    connect(actIconScale, SIGNAL(triggered()), this, SLOT(iconScale()));
    connect(actIconResize, SIGNAL(triggered()), this, SLOT(iconResize()));
    connect(actIconRevert, SIGNAL(triggered()), this, SLOT(iconRevert()));
    connect(actIconEffect, SIGNAL(triggered()), this, SLOT(showEffectsDialog()));
    connect(actIconBack, SIGNAL(triggered()), this, SLOT(setPreviewColor()));
    connect(actPacking, SIGNAL(triggered()), toolDialog, SLOT(open()));
    connect(actKeys, SIGNAL(triggered()), keyManager, SLOT(open()));
    connect(actAssoc, SIGNAL(triggered()), this, SLOT(associate()));
    connect(actReset, SIGNAL(triggered()), this, SLOT(settings_reset()));
    connect(actWebsite, SIGNAL(triggered()), this, SLOT(browseSite()));
    connect(actReport, SIGNAL(triggered()), this, SLOT(browseBugs()));
    connect(actDonate, SIGNAL(triggered()), this, SLOT(donate()));
    connect(actFaq, SIGNAL(triggered()), this, SLOT(browseFaq()));
    connect(actLogFile, SIGNAL(triggered()), this, SLOT(openLogFile()));
    connect(actLogPath, SIGNAL(triggered()), this, SLOT(openLogPath()));
    connect(actUpdate, SIGNAL(triggered()), updater, SLOT(check()));
    connect(actAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(btnPack, SIGNAL(clicked()), this, SLOT(apk_save()));
    connect(mapRecent, SIGNAL(mapped(QString)), this, SLOT(apk_open(QString)));
    connect(btnApplyIcons, SIGNAL(clicked()), this, SLOT(cloneIcons()));
    connect(tableStrings, SIGNAL(cellChanged(int, int)), this, SLOT(stringChanged(int, int)));
    connect(editAppName, SIGNAL(textEdited(QString)), this, SLOT(setModified()));
    connect(editVersionCode, SIGNAL(valueChanged(int)), this, SLOT(setModified()));
    connect(editVersionName, SIGNAL(textEdited(QString)), this, SLOT(setModified()));
    connect(btnApplyAppName, SIGNAL(clicked()), this, SLOT(applyAppName()));
    connect(btnRepacking, SIGNAL(clicked()), toolDialog, SLOT(open()));
    connect(btnTool, SIGNAL(clicked()), toolDialog, SLOT(switch_mode()));
    connect(btnTool, SIGNAL(clicked()), this, SLOT(askReloadApk()));
    connect(toolDialog, SIGNAL(apktool_checked(bool)), this, SLOT(enableApktool(bool)));
    connect(toolDialog, SIGNAL(tool_changed()), this, SLOT(askReloadApk()));
    connect(apk, SIGNAL(loading(short, QString)), loadingDialog, SLOT(setProgress(short, QString)));
    connect(apk, SIGNAL(error(QString, QString)), this, SLOT(error(QString, QString)));
    connect(apk, SIGNAL(error(QString, QString)), loadingDialog, SLOT(accept()));
    connect(apk, SIGNAL(warning(QString, QString)), this, SLOT(warning(QString, QString)));
    connect(apk, SIGNAL(packed(QString, bool, QString)), this, SLOT(apkPacked(QString, bool, QString)));
    connect(apk, SIGNAL(unpacked(QString)), this, SLOT(apkUnpacked(QString)));
    connect(keyManager, SIGNAL(success(QString, QString)), this, SLOT(success(QString, QString)));
    connect(keyManager, SIGNAL(warning(QString, QString)), this, SLOT(warning(QString, QString)));
    connect(keyManager, SIGNAL(error(QString, QString)), this, SLOT(error(QString, QString)));
    connect(dropbox, SIGNAL(auth_required()), this, SLOT(authCloud()));
    connect(dropbox, SIGNAL(progress(short, QString)), uploadDialog, SLOT(setProgress(short, QString)));
    connect(dropbox, SIGNAL(error(QString, QString)), this, SLOT(error(QString, QString)));
    connect(gdrive, SIGNAL(auth_required()), this, SLOT(authCloud()));
    connect(gdrive, SIGNAL(progress(short, QString)), uploadDialog, SLOT(setProgress(short, QString)));
    connect(gdrive, SIGNAL(error(QString, QString)), this, SLOT(error(QString, QString)));
    connect(onedrive, SIGNAL(auth_required()), this, SLOT(authCloud()));
    connect(onedrive, SIGNAL(progress(short, QString)), uploadDialog, SLOT(setProgress(short, QString)));
    connect(onedrive, SIGNAL(error(QString, QString)), this, SLOT(error(QString, QString)));
    connect(uploadDialog, SIGNAL(rejected()), dropbox, SLOT(cancel()));
    connect(uploadDialog, SIGNAL(rejected()), gdrive, SLOT(cancel()));
    connect(uploadDialog, SIGNAL(rejected()), onedrive, SLOT(cancel()));
    connect(updater, SIGNAL(version(QString)), this, SLOT(newVersion(QString)));
    connect(this, SIGNAL(destroyed()), apk, SLOT(clearTemp()));
}

void MainWindow::settings_load()
{
    // Global:

    if (Settings::get_version() != VER) { resetApktool(); }
    restoreGeometry(Settings::get_geometry());
    splitter->setSizes(QList<int>() << 492 << 280);
    splitter->restoreState(Settings::get_splitter());
    setLanguage(Settings::get_language());
    currentPath = Settings::get_last_path();
    devices->setCurrentGroup(Settings::get_device());
    actAutoUpdate->setChecked(Settings::get_update());

    // Recent List:

    recent = new Recent(Settings::get_recent());
    recent_update();

    // APK:

    toolDialog->reset();

    // Keys:

    keyManager->reset();

    // Cloud Services:

    checkUpload->setChecked(Settings::get_upload());
    checkDropbox->setChecked(Settings::get_dropbox());
    checkGDrive->setChecked(Settings::get_gdrive());
    checkOneDrive->setChecked(Settings::get_onedrive());
    dropbox->setToken(Settings::get_dropbox_token());
    gdrive->setToken(Settings::get_gdrive_token());
    onedrive->setToken(Settings::get_onedrive_token());
}

void MainWindow::resetApktool()
{
    const QString FRAMEWORK = Settings::get_temp() + "/apk-icon-editor/framework/1.apk";
    QFile::remove(FRAMEWORK);
}

void MainWindow::settings_reset()
{
    if (QMessageBox::question(this, tr("Reset?"), tr("Reset settings to default?")) == QMessageBox::Yes) {
        const bool APKTOOL = Settings::get_use_apktool();
        Settings::reset();
        settings_load();
        resetApktool();
        resize(Gui::Window::WIDTH, Gui::Window::HEIGHT);
        if (APKTOOL != Settings::get_use_apktool()) {
            askReloadApk();
        }
    }
}

void MainWindow::setLanguage(QString lang)
{
    qDebug() << "Language set to" << lang << "\n";
    QString LANGPATH(Path::App::dir() + "/lang");
    QApplication::removeTranslator(translator);
    QApplication::removeTranslator(translatorQt);
    if (translator->load(QString("apk-icon-editor.%1").arg(lang), LANGPATH)) {
        // TODO (OS X) Crashes when setting any language different from English
        translatorQt->load(QString("qt.%1").arg(lang), LANGPATH);
        QApplication::installTranslator(translator);
        QApplication::installTranslator(translatorQt);
    }
    else {
        lang = "en";
    }

    currentLang = lang;
    QPixmap flag;
    if (flag.load(QString("%1/flag.%2.png").arg(LANGPATH, lang)) == false) {
        lang = lang.left(2);
        flag = QString("%1/flag.%2.png").arg(LANGPATH, lang);
    }
    menuLang->setIcon(flag);

    // Retranslate strings:

    drawArea->setText(tr("CLICK HERE\n(or drag APK and icons)"));
    tabs->setTabText(0, tr("Icons"));
    tabs->setTabText(1, tr("Properties"));
    tabs->setTabText(2, tr("Details"));
    devices->setLabelText(tr("Device:"));
    btnApplyIcons->setText(tr("Apply to All"));
    labelAppName->setText(tr("Application Name") + ':');
    labelVersionName->setText(tr("Version Name") + ':');
    labelVersionCode->setText(tr("Version Code") + ':');
    btnApplyAppName->setText(tr("Apply to All"));
    labelApktool->setText(tr("To edit application name and version, switch to \"Apktool\" mode."));
    btnRepacking->setText(tr("&Repacking"));
    tableStrings->setHorizontalHeaderLabels(QStringList() << tr("Language") << tr("Application Name"));
    checkDropbox->setText(tr("Upload to %1").arg(dropbox->getTitle()));
    checkGDrive->setText(tr("Upload to %1").arg(gdrive->getTitle()));
    checkOneDrive->setText(tr("Upload to %1").arg(onedrive->getTitle()));
    checkUpload->setText(tr("Enable Upload to Cloud Storages"));
    btnPack->setText(tr("Pack APK"));
    menuFile->setTitle(tr("&File"));
    menuIcon->setTitle(tr("&Icon"));
    menuSett->setTitle(tr("&Settings"));
    menuHelp->setTitle(tr("&Help"));
    actApkOpen->setText(tr("&Open APK"));
    actApkSave->setText(tr("&Export (Pack) APK"));
    menuRecent->setTitle(tr("&Recent APKs"));
    actApkExplore->setText(tr("Explore APK &Contents"));
    actExit->setText(tr("E&xit"));
    actRecentClear->setText(tr("&Clear List"));
    actNoRecent->setText(tr("No Recent Files"));
    actIconOpen->setText(tr("Replace &Icon"));
    actIconSave->setText(tr("&Save Icon"));
    actIconScale->setText(tr("Scale to &Fit"));
    actIconResize->setText(tr("&Resize Icon"));
    actIconRevert->setText(tr("Revert &Original"));
    actIconEffect->setText(tr("E&ffects"));
    actIconBack->setText(tr("Preview Background &Color"));
    actPacking->setText(tr("&Repacking"));
    actKeys->setText(tr("Key Manager"));
    menuLang->setTitle(tr("&Language"));
    actTranslate->setText(tr("Help Translate"));
    actAutoUpdate->setText(tr("Auto-check for Updates"));
    actAssoc->setText(tr("Associate .APK"));
    actReset->setText(tr("Reset Settings"));
    actFaq->setText(tr("FAQ"));
    actWebsite->setText(tr("Visit Website"));
    actReport->setText(tr("Report a Bug"));
    actDonate->setText(tr("Donate"));
    menuLogs->setTitle(tr("Logs"));
    actLogFile->setText(tr("Open Log File"));
    actLogPath->setText(tr("Open Log Directory"));
    actUpdate->setText(tr("Check for &Updates"));
    actAboutQt->setText(tr("About Qt"));
    actAbout->setText(tr("About %1").arg(APP));
    labelTool->setText(tr("Current Mode:"));
    loadingDialog->setWindowTitle(tr("Processing"));
    uploadDialog->setWindowTitle(tr("Uploading"));
    btnTool->setToolTip(Settings::get_use_apktool() ? toolDialog->hint_apktool() : toolDialog->hint_quazip());
    menuBar()->resize(0, 0); // "Repaint" menu bar

    effects->retranslate();
    toolDialog->retranslate();
    keyManager->retranslate();
}

void MainWindow::recent_add(QString filename)
{
    // Get the smallest icon:

    Dpi::Type dpi = Dpi::LDPI;
    for (short i = Dpi::LDPI; i < Dpi::COUNT; ++i) {
        if (Icon *icon = apk->getIcon(Dpi::cast(i))) {
            if (!icon->isNull()) {
                dpi = Dpi::cast(i);
                break;
            }
        }
    }

    // Add to recent and refresh the list:

    recent->add(filename, apk->getIcon(dpi)->getPixmap());
    recent_update();
}

void MainWindow::recent_update()
{
    menuRecent->clear();

    if (!recent->empty()) {
        for (short i = 0; i < recent->size(); ++i) {
            const RecentFile RECENT = recent->at(i);
            QAction *actRecent = new QAction(RECENT.filename, menuRecent);
            actRecent->setIcon(RECENT.icon);
            menuRecent->addAction(actRecent);
            connect(actRecent, SIGNAL(triggered()), mapRecent, SLOT(map()));
            mapRecent->setMapping(actRecent, RECENT.filename);
        }
        menuRecent->addSeparator();
        menuRecent->addAction(actRecentClear);
    }
    else {
        menuRecent->addAction(actNoRecent);
    }
}

void MainWindow::recent_clear()
{
    recent->clear();
    recent_update();
}

void MainWindow::hideEmptyDpi()
{
    for (short i = Dpi::LDPI; i < Dpi::COUNT; ++i) {
        bool visible = false;
        const Dpi::Type DPI = Dpi::cast(i);
        if (Icon *icon = apk->getIcon(DPI)) {
            visible = !icon->isNull();
        }
        devices->setItemVisible(DPI, visible);
    }
}

void MainWindow::cloneIcons()
{
    Icon *newIcon = drawArea->getIcon();
    if (!newIcon->isNull()) {
        if (QMessageBox::question(this, NULL, tr("Apply the current icon to all sizes?"))
         == QMessageBox::Yes) {
            for (short i = Dpi::LDPI; i < Dpi::COUNT; ++i) {
                const Dpi::Type DPI = Dpi::cast(i);
                Icon *oldIcon = apk->getIcon(DPI);
                if (!oldIcon->isNull()) {
                    oldIcon->replace(newIcon->getPixmap());
                }
            }
        }
        setWindowModified(true);
    }
}

void MainWindow::stringChanged(int row, int col)
{
    if (col == 1) {
        // Mark this cell as "edited":
        tableStrings->item(row, col)->setData(Qt::UserRole, true);
        setWindowModified(true);
    }
}

void MainWindow::applyAppName()
{
    QString name = editAppName->text();
    if (!name.isEmpty()) {
        if (QMessageBox::question(this, NULL, tr("Apply the current application name to all translations?"))
         == QMessageBox::Yes) {
            for (int i = 0; i < tableStrings->rowCount(); ++i) {
                tableStrings->item(i, 1)->setText(name);
            }
        }
    }
}

void MainWindow::enableApktool(bool value)
{
    btnTool->setText(value ? "Apktool" : "ZIP");
    btnTool->setToolTip(value ? toolDialog->hint_apktool() : toolDialog->hint_quazip());

    menuBar()->resize(0, 0); // "Repaint" menu bar

    // If any APK is currently open:

    if (!currentApk.isEmpty()) {
        editAppName->setEnabled(value);
        editVersionCode->setEnabled(value);
        editVersionName->setEnabled(value);
        btnApplyAppName->setEnabled(value);
    }

    if (value) {
        tableStrings->setVisible(true);
        panelApktool->setVisible(false);
    }
    else {
        tableStrings->setVisible(false);
        panelApktool->setVisible(true);
    }
}

void MainWindow::askReloadApk()
{
    if (currentApk.isEmpty()) { // Check if any APK is currently loaded
        return;
    }

    int result = QMessageBox::question(this, tr("Repack?"),
                                       tr("Changing tool requires repacking.\nProceed?"),
                                       QMessageBox::Yes, QMessageBox::No);

    if (result != QMessageBox::Yes || !apk_open(currentApk)) {
        toolDialog->switch_mode();
    }
}

void MainWindow::setCurrentIcon(int id)
{
    if (id == -1) return;
    const Device DEVICE = Devices::at(devices->currentGroupIndex());
    const int SIZE = DEVICE.getDpiSize(Dpi::cast(id));
    drawArea->setRect(SIZE, SIZE);
    if (Icon *icon = apk->getIcon(Dpi::cast(id))) {
        disconnect(effects, 0, 0, 0);
        connect(effects, SIGNAL(colorActivated(bool)), icon,     SLOT(setColorEnabled(bool)), Qt::DirectConnection);
        connect(effects, SIGNAL(rotate(int)),          icon,     SLOT(setAngle(int)),         Qt::DirectConnection);
        connect(effects, SIGNAL(flipX(bool)),          icon,     SLOT(setFlipX(bool)),        Qt::DirectConnection);
        connect(effects, SIGNAL(flipY(bool)),          icon,     SLOT(setFlipY(bool)),        Qt::DirectConnection);
        connect(effects, SIGNAL(colorize(QColor)),     icon,     SLOT(setColor(QColor)),      Qt::DirectConnection);
        connect(effects, SIGNAL(colorDepth(qreal)),    icon,     SLOT(setDepth(qreal)),       Qt::DirectConnection);
        connect(effects, SIGNAL(blur(qreal)),          icon,     SLOT(setBlur(qreal)),        Qt::DirectConnection);
        connect(effects, SIGNAL(round(qreal)),         icon,     SLOT(setCorners(qreal)),     Qt::DirectConnection);
        connect(effects, SIGNAL(colorActivated(bool)), drawArea, SLOT(repaint()));
        connect(effects, SIGNAL(blurActivated(bool)),  drawArea, SLOT(repaint()));
        connect(effects, SIGNAL(rotate(int)),          drawArea, SLOT(repaint()));
        connect(effects, SIGNAL(flipX(bool)),          drawArea, SLOT(repaint()));
        connect(effects, SIGNAL(flipY(bool)),          drawArea, SLOT(repaint()));
        connect(effects, SIGNAL(colorize(QColor)),     drawArea, SLOT(repaint()));
        connect(effects, SIGNAL(colorDepth(qreal)),    drawArea, SLOT(repaint()));
        connect(effects, SIGNAL(blur(qreal)),          drawArea, SLOT(repaint()));
        connect(effects, SIGNAL(round(qreal)),         drawArea, SLOT(repaint()));
        drawArea->setIcon(icon);
    }
}

void MainWindow::setActiveApk(QString filename)
{
    currentApk = filename;
    setWindowModified(false);
    setWindowTitle(QFileInfo(filename).fileName() + "[*]");
    recent_add(filename);
}

void MainWindow::enableUpload(bool enable)
{
    checkDropbox->setVisible(enable);
    checkGDrive->setVisible(enable);
    checkOneDrive->setVisible(enable);
}

void MainWindow::upload(Cloud *uploader, QString filename)
{
    QEventLoop loop;
    connect(uploader, SIGNAL(finished(bool)), &loop, SLOT(quit()), Qt::QueuedConnection);
    uploadDialog->setIcon(uploader->getIcon());
    uploader->upload(filename);
    loop.exec(); // Block execution until cloud upload is finished.
}

void MainWindow::apkPacked(QString filename, bool isSuccess, QString text)
{
    loadingDialog->accept();
    setActiveApk(filename);

    const bool UPLOAD_TO_DROPBOX  = checkDropbox->isChecked();
    const bool UPLOAD_TO_GDRIVE   = checkGDrive->isChecked();
    const bool UPLOAD_TO_ONEDRIVE = checkOneDrive->isChecked();

    if (isSuccess) {
        if (checkUpload->isChecked()) {
            if (UPLOAD_TO_DROPBOX)  upload(dropbox, filename);
            if (UPLOAD_TO_GDRIVE)   upload(gdrive, filename);
            if (UPLOAD_TO_ONEDRIVE) upload(onedrive, filename);
        }
        uploadDialog->accept();
        success(NULL, text);
    }
    else {
        warning(NULL, text);
        if (checkUpload->isChecked()) {
            if (UPLOAD_TO_DROPBOX)  upload(dropbox, filename);
            if (UPLOAD_TO_GDRIVE)   upload(gdrive, filename);
            if (UPLOAD_TO_ONEDRIVE) upload(onedrive, filename);
        }
        uploadDialog->accept();
    }
}

void MainWindow::apkUnpacked(QString filename)
{
    loadingDialog->accept();
    setActiveApk(filename);

    // Automatically select DPI from the list:

    const int ID = devices->currentItemIndex();
    if (ID != -1
        && apk->getIcon(Dpi::cast(ID))
        && !apk->getIcon(Dpi::cast(ID))->isNull())
    {
        devices->setCurrentItem(ID);
    }
    else {
        for (short i = Dpi::LDPI; i < Dpi::COUNT; ++i) {
            const Dpi::Type DPI = Dpi::cast(i);
            if (Icon *icon = apk->getIcon(DPI)) {
                if (!icon->isNull()) {
                    devices->setCurrentItem(i);
                    break;
                }
            }
        }
    }

    hideEmptyDpi();

    // Load strings to the table:

    editAppName->clear();
    editVersionCode->clear();
    editVersionName->clear();
    tableStrings->clear();
    tableStrings->setHorizontalHeaderLabels(QStringList() << tr("Language") << tr("Application Name"));
    QList<Resource> res = apk->getStrings();
    tableStrings->setRowCount(res.size());
    for (int i = res.size() - 1; i >= 0; --i) {
        QFileInfo fi(res[i].getFilename());
        QString locale = fi.dir().dirName().mid(7);
        QString native = QLocale(locale).nativeLanguageName();
        if (!native.isEmpty()) {
            native[0] = native[0].toUpper();
            native = QString("%1 (%2)").arg(native, locale);
        }
        else {
            if (locale.isEmpty()) { // This has to be the last iteration
                editAppName->setText(res[i].getValue());
                tableStrings->removeRow(i);
                continue;
            }
            else {
                native = locale;
            }
        }
        QTableWidgetItem *c1 = new QTableWidgetItem(native);
        QTableWidgetItem *c2 = new QTableWidgetItem(res[i].getValue());
        QTableWidgetItem *c3 = new QTableWidgetItem(res[i].getFilename());
        c1->setFlags(c1->flags() & ~Qt::ItemIsEditable);
        tableStrings->blockSignals(true);
        tableStrings->setItem(i, 0, c1);
        tableStrings->setItem(i, 1, c2);
        tableStrings->setItem(i, 2, c3);
        tableStrings->item(i, 1)->setData(Qt::UserRole, false); // Mark as "not edited"
        tableStrings->blockSignals(false);
    }
    if (editAppName->text().isEmpty()) {
        editAppName->setText(apk->getApplicationLabel());
    }
    editVersionCode->setValue(apk->getVersionCode().toInt());
    editVersionName->setText(apk->getVersionName());

    // Enable operations with APK and icons:

    actApkSave->setEnabled(true);
    actApkExplore->setEnabled(true);
    actIconOpen->setEnabled(true);
    actIconSave->setEnabled(true);
    actIconRevert->setEnabled(true);
    actIconEffect->setEnabled(true);
    actIconResize->setEnabled(true);
    actIconScale->setEnabled(true);
    btnApplyIcons->setEnabled(true);
    if (Settings::get_use_apktool()) {
        editAppName->setEnabled(true);
        editVersionCode->setEnabled(true);
        editVersionName->setEnabled(true);
        btnApplyAppName->setEnabled(true);
    }
    devices->setEnabled(true);
    btnPack->setEnabled(true);

    setWindowModified(false);
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
        const Device DEVICE = Devices::at(devices->currentGroupIndex());
        const int SIZE = DEVICE.getDpiSize(Dpi::cast(devices->currentItemIndex()));
        if (icon->width() != SIZE || icon->height() != SIZE) {
            int result = QMessageBox::warning(this, tr("Resize?"),
                                      tr("Icon you are trying to load is off-size.\nResize automatically?"),
                                      QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
            switch (result) {
            case QMessageBox::Yes:
                icon->resize(SIZE);
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
        Device device = Devices::at(devices->currentGroupIndex());
        int size = device.getDpiSize(Dpi::cast(devices->currentItemIndex()));
        filename = QString("%1-%2").arg(QFileInfo(currentApk).baseName()).arg(size);
        filename = QFileDialog::getSaveFileName(this, tr("Save Icon"), filename, FILTER_GFX);
        if (filename.isEmpty()) {
            return false;
        }
    }
    return drawArea->getIcon()->save(filename);
}

void MainWindow::iconScale()
{
    const Device DEVICE = Devices::at(devices->currentGroupIndex());
    const int SIZE = DEVICE.getDpiSize(Dpi::cast(devices->currentItemIndex()));
    iconResize(SIZE);
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

void MainWindow::showEffectsDialog()
{
    Icon *icon = drawArea->getIcon();
    if (!icon->isNull()) {
        const int TEMP_ANGLE = icon->getAngle();
        const bool TEMP_FLIP_X = icon->getFlipX();
        const bool TEMP_FLIP_Y = icon->getFlipY();
        const bool TEMP_IS_COLOR = icon->getColorEnabled();
        const QColor TEMP_COLOR = icon->getColor();
        const qreal TEMP_DEPTH = icon->getDepth();
        const qreal TEMP_BLUR = icon->getBlur();
        const qreal TEMP_ROUND = icon->getCorners();
        effects->setRotation(TEMP_ANGLE);
        effects->setFlipX(TEMP_FLIP_X);
        effects->setFlipY(TEMP_FLIP_Y);
        effects->setColorEnabled(TEMP_IS_COLOR);
        effects->setColor(TEMP_COLOR);
        effects->setColorDepth(TEMP_DEPTH * 100);
        effects->setBlur(TEMP_BLUR * 10);
        effects->setCorners(TEMP_ROUND);
        // TODO: Get rid of flicker on exec().
        if (effects->exec() == QDialog::Accepted) {
            setWindowModified(true);
        }
        else {
            icon->setAngle(TEMP_ANGLE);
            icon->setFlipX(TEMP_FLIP_X);
            icon->setFlipY(TEMP_FLIP_Y);
            icon->setColorEnabled(TEMP_IS_COLOR);
            icon->setColor(TEMP_COLOR);
            icon->setDepth(TEMP_DEPTH);
            icon->setBlur(TEMP_BLUR);
            icon->setCorners(TEMP_ROUND);
        }
    }
    else {
        invalidDpi();
    }
}

bool MainWindow::apk_open(QString filename)
{
    if (confirmExit()) {
        return false;
    }

    // "Open" dialog:

    if (filename.isEmpty()) {
        filename = QFileDialog::getOpenFileName(this, tr("Open APK"), currentPath, "APK (*.apk);;" + tr("All Files"));
        if (filename.isEmpty()) {
            return false;
        }
    }

    // Open APK:

    if (!QFile::exists(filename)) {
        error(tr("File not found"), tr("Could not find APK:\n%1").arg(filename));
        recent->remove(filename);
        recent_update();
        return false;
    }

    currentPath = QFileInfo(filename).absolutePath();

    // Unpacking:

    loadingDialog->setProgress(0);
    PackOptions opts;
    opts.filename = filename;
    opts.temp = Settings::get_temp();
    opts.isApktool = Settings::get_use_apktool();
    opts.isSmali = Settings::get_smali();
    apk->unpack(opts);
    return true;
}

bool MainWindow::apk_save(QString filename)
{
    // Check if required fields are empty:

    if (editAppName->text().isEmpty()) {
        warning(NULL, tr("\"%1\" field cannot be empty.").arg(tr("Application Name")));
        tabs->setCurrentIndex(1);
        editAppName->setFocus();
        return false;
    }
    else if (editVersionName->text().isEmpty()) {
        warning(NULL, tr("\"%1\" field cannot be empty.").arg(tr("Version Name")));
        tabs->setCurrentIndex(1);
        editVersionName->setFocus();
        return false;
    }

    // Fill empty translation values:

    for (int i = 0; i < tableStrings->rowCount(); ++i) {
        if (tableStrings->item(i, 1)->text().isEmpty()) {
            tableStrings->item(i, 1)->setText(editAppName->text());
        }
    }

    // "Save" dialog:

    if (filename.isEmpty()) {
        filename = QFileDialog::getSaveFileName(this, tr("Pack APK"), currentPath, "APK (*.apk)");
        if (filename.isEmpty()) {
            return false;
        }
    }

    // Fetch KeyStore properties:

    QString alias = Settings::get_alias();
    QString pass_store = Settings::get_keystore_pass();
    QString pass_alias = Settings::get_alias_pass();

    const bool USING_KEYSTORE = Settings::get_use_keystore();
    if (USING_KEYSTORE) {
        const QPixmap PIXMAP_KEY(":/gfx/actions/key.png");
        if (pass_store.isEmpty()) {
            pass_store = InputDialog::getString(tr("Enter the KeyStore password:"), "", true, PIXMAP_KEY, this);
        }
        if (alias.isEmpty()) {
            alias = InputDialog::getString(tr("Enter the alias:"), "", false, PIXMAP_KEY, this);
        }
        if (pass_alias.isEmpty()) {
            pass_alias = InputDialog::getString(tr("Enter the alias password:"), "", true, PIXMAP_KEY, this);
        }
    }

    // Saving APK:

    currentPath = QFileInfo(filename).absolutePath();
    loadingDialog->setProgress(0);

    PackOptions opts;
    opts.filename = filename;
    opts.temp = Settings::get_temp();
    opts.isApktool = Settings::get_use_apktool();
    opts.ratio = Settings::get_compression();
    opts.isSmali = Settings::get_smali();
    opts.isSign = Settings::get_sign();
    opts.isOptimize = Settings::get_zipalign();
    opts.filePem = Settings::get_pem();
    opts.filePk8 = Settings::get_pk8();
    opts.fileKey = Settings::get_keystore();
    opts.isKeystore = USING_KEYSTORE;
    opts.alias = alias;
    opts.passStore = pass_store;
    opts.passAlias = pass_alias;
    opts.appName = editAppName->text();
    opts.appVersionName = editVersionName->text();
    opts.appVersionCode = editVersionCode->text();

    QList<Resource> res;
    for (int i = 0; i < tableStrings->rowCount(); ++i) {
        // Save only edited strings:
        if (tableStrings->item(i, 1)->data(Qt::UserRole) == true) {
            res.push_back(Resource(tableStrings->item(i, 2)->text(),
                                   tableStrings->item(i, 1)->text()));
        }
    }

    opts.resources = res;
    apk->pack(opts);
    return true;
}

void MainWindow::apk_explore()
{
    const QString TEMPDIR = Settings::get_temp() + "/apk-icon-editor/apk";
    QDesktopServices::openUrl(QUrl::fromLocalFile(TEMPDIR));
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
    QDesktopServices::openUrl(Url::WEBSITE);
}

void MainWindow::browseBugs() const
{
    QDesktopServices::openUrl(Url::CONTACT);
}

void MainWindow::browseFaq() const
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QString("%1/faq.txt").arg(Path::App::dir())));
}

void MainWindow::openLogFile() const
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(Path::Log::FILE));
}

void MainWindow::openLogPath() const
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(Path::Log::DIR));
}

void MainWindow::about()
{
    const QString TD = "<td style='padding-right: 8px'>%1</td>";
    const QString JRE = !version_jre.isEmpty() ? version_jre : QString("<a href='%1'>%2</a>").arg(Url::JRE, QApplication::translate("Apk", "Download"));
    const QString JDK = !version_jdk.isEmpty() ? version_jdk : QString("<a href='%1'>%2</a>").arg(Url::JDK, QApplication::translate("Apk", "Download"));
    const QString LINK = "<tr><td style='padding-right: 4px' align='right'><a href='%1'>%2</a></td><td>%3</td></tr>";

    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle(tr("About"));
    aboutBox.setStyleSheet("font-weight: normal");
    aboutBox.setText(
        QString("<h3>%1 v%2</h3>").arg(APP, VER) +
        "<p>" +
            tr("Built on: %1 - %2").arg(__DATE__, __TIME__) + "<br>" +
            tr("Author: %1").arg("Alexander Gorishnyak") + "<br>" +
            tr("License") + ": <a href='http://www.gnu.org/licenses/gpl-3.0.html'>GNU GPL v3.0</a>" +
        "</p>"
        "<p><table>" +
            LINK.arg(Url::WEBSITE, tr("Visit Website"), tr("Visit our official website.")) +
            LINK.arg(Url::CONTACT, tr("Report a Bug"), tr("Found a bug? Let us know so we can fix it!")) +
            LINK.arg(Url::TRANSLATE, tr("Help Translate"), tr("Join our translation team on Transifex.")) +
            LINK.arg(QString("file:///%1/versions.txt").arg(Path::App::dir()), tr("Version History"), tr("List of changes made to the project.")) +
        "</table></p>"
        "<hr style='margin-top: 6px'>"
        "<p style='margin-top: 4px'><table width='100%'><tr>" +
            TD.arg("Qt: %1").arg(qVersion()) +
            TD.arg("Apktool: %1").arg(version_apktool) +
            TD.arg("JRE: %1").arg(JRE) +
            TD.arg("JDK: %1").arg(JDK) +
        "</tr></table></p>"
    );
    aboutBox.setIconPixmap(QPixmap(":/gfx/logo-about.png"));
    QPushButton btnAuthors(tr("Authors"));
    connect(&btnAuthors, SIGNAL(clicked()), &aboutBox, SLOT(close()));
    connect(&btnAuthors, SIGNAL(clicked()), this, SLOT(aboutAuthors()));
    aboutBox.addButton(QMessageBox::Ok);
    aboutBox.addButton(&btnAuthors, QMessageBox::ActionRole);
    aboutBox.setEscapeButton(aboutBox.button(QMessageBox::Ok));
    aboutBox.exec();
}

void MainWindow::aboutAuthors()
{
    QMessageBox authors(this);
    authors.setWindowTitle(tr("Authors"));
    authors.setIconPixmap(QPixmap(":/gfx/logo-about.png"));

    QRegExp rx("\\(www.(.+)\\)");
    rx.setMinimal(true);

    QString strAuthors;
    QFile inputFile(Path::App::dir() + "/authors.txt");
    if (inputFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&inputFile);
        in.setCodec(QTextCodec::codecForName("UTF-8"));
        bool newline = true;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line == "Testers:") {
                strAuthors.chop(4); // Chop the last "<br>" tag.
                break;
            }
            line = line.replace(rx, "(<a href=\"http://www.\\1\">www.\\1</a>)");
            strAuthors += !newline ? line + "<br>" : QString("<b>%1</b><br>").arg(line);
            newline = line.isEmpty();
        }
        strAuthors.chop(4); // Chop the last "<br>" tag.
        inputFile.close();
        authors.setText(strAuthors);
        authors.exec();
    }
}

void MainWindow::aboutQt() const
{
    QApplication::aboutQt();
}

void MainWindow::donate()
{
    Donate dialog(this);
    dialog.exec();
}

void MainWindow::invalidDpi()
{
    warning(tr("Icon Missing"),
            tr("This APK does not support current DPI."));
}

void MainWindow::browseTranslate()
{
    QDesktopServices::openUrl(Url::TRANSLATE);
}

bool MainWindow::newVersion(QString version)
{
    QMessageBox msgBox(
                QMessageBox::Information,
                tr("Update"),
                tr("%1 v%2 is available.\nDownload?").arg(APP, version),
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

void MainWindow::authCloud()
{
    Cloud *cloud = dynamic_cast<Cloud*>(sender());
    QApplication::alert(this);
    QString code = InputDialog::getString(tr("Allow access to %1 in your browser and paste the provided code here:").arg(cloud->getTitle()),
                                          tr("Authorization"),
                                          true, cloud->getIcon(), this);
    if (!code.isEmpty()) {
        cloud->login(code);
        uploadDialog->activateWindow();
    }
    else {
        cloud->cancel();
    }
}

void MainWindow::success(QString title, QString text)
{
    QApplication::alert(this);
    QMessageBox::information(this, title, text);
}

void MainWindow::warning(QString title, QString text)
{
    qDebug() << qPrintable(QString("Warning (%1): %2").arg(title).arg(text));
    QApplication::alert(this);
    QMessageBox::warning(this, title, text);
}

void MainWindow::error(QString title, QString text)
{
    qDebug() << qPrintable(QString("Error (%1): %2").arg(title).arg(text));
    QApplication::alert(this);
    QMessageBox::critical(this, title, text);
}

void MainWindow::setModified()
{
    setWindowModified(true);
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

        // Handle Qt Unix bug:
        if (filename.right(2) == "\r\n") {
            filename.chop(2);
        }

        QString ext = QFileInfo(filename).suffix().toLower();
        if (EXT_GFX.contains(ext)) {
            iconOpen(filename);
        }
        else {
            apk_open(filename);
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

    // Save Settings:

    Settings::set_version(VER);
    Settings::set_device(devices->currentGroupText());
    Settings::set_language(currentLang);
    Settings::set_update(actAutoUpdate->isChecked());
    Settings::set_path(currentPath);
    Settings::set_geometry(saveGeometry());
    Settings::set_splitter(splitter->saveState());
    Settings::set_recent(recent->files());

    Settings::set_upload(checkUpload->isChecked());
    Settings::set_dropbox(checkDropbox->isChecked());
    Settings::set_gdrive(checkGDrive->isChecked());
    Settings::set_onedrive(checkOneDrive->isChecked());
    Settings::set_dropbox_token(dropbox->getToken());
    Settings::set_gdrive_token(gdrive->getToken());
    Settings::set_onedrive_token(onedrive->getToken());

    Settings::kill();

    // Close window:

    event->accept();
}

MainWindow::~MainWindow()
{
    qDebug() << "Exiting...";
    delete recent;
}
