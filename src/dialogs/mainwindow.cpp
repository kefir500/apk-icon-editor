#include "mainwindow.h"
#include "globals.h"
#include "settings.h"
#include "apkfile.h"
#include "decorationdelegate.h"
#include <QHeaderView>
#include <QMenuBar>
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

    apk_close();

    Settings::init();
    settings_load();

    if (actAutoUpdate->isChecked()) {
        // Update check is delayed in order to handle uninitialized GUI occuring with some firewalls.
        QTimer *delay = new QTimer(this);
        connect(delay, SIGNAL(timeout()), updater, SLOT(check()));
        connect(delay, SIGNAL(timeout()), delay, SLOT(deleteLater()));
        delay->setSingleShot(true);
        delay->start(1200);
    }
}

void MainWindow::checkReqs()
{
    QtConcurrent::run([=]() {
        resetApktool();

        QString version_jre;
        QString version_jdk;
        QString version_apktool = Apk::getApktoolVersion();
        const QString JRE = Apk::getJreVersion();
        const QString JDK = Apk::getJdkVersion();

        QRegExp rx;
        QStringList cap;

        rx.setPattern("version \"(.+)\"");
        rx.indexIn(JRE);
        cap = rx.capturedTexts();
        if (cap.size() > 1) {
            version_jre = cap[1];
            Settings::set_java_version(version_jre);
            toolDialog->reset();
        }

        rx.setPattern("javac (.+)");
        rx.indexIn(JDK);
        cap = rx.capturedTexts();
        if (cap.size() > 1) {
            version_jdk = cap[1];
        }

        qDebug() << "JRE version:" << qPrintable(!JRE.isNull() ? version_jre : "---");
        qDebug() << "JDK version:" << qPrintable(!JDK.isNull() ? version_jdk : "---");
        qDebug() << "Apktool version:" << qPrintable(!version_apktool.isNull() ? version_apktool : "---") << '\n';

        if (!JRE.isNull()) qDebug().nospace() << "java -version\n" << qPrintable(JRE) << '\n';
        if (!JDK.isNull()) qDebug().nospace() << "javac -version\n" << qPrintable(JDK) << '\n';

        emit reqsChecked(version_jre, version_jdk, version_apktool);
    });
}

void MainWindow::init_core()
{
    apk = NULL;
    apkManager = new ApkManager(this);
    updater = new Updater(this);
    mapRecent = new QSignalMapper(this);
    recent = NULL;

    dropbox = new Dropbox(this);
    gdrive = new GoogleDrive(this);
    onedrive = new OneDrive(this);
    dropbox->setIcon(QPixmap(":/gfx/clouds/dropbox.png"));
    gdrive->setIcon(QPixmap(":/gfx/clouds/gdrive.png"));
    onedrive->setIcon(QPixmap(":/gfx/clouds/onedrive.png"));
}

void MainWindow::init_gui()
{
    qDebug() << "Screen DPI:" << Gui::Screen::dpi();

    // Dialogs:

    effects = new EffectsDialog(this);
    toolDialog = new ToolDialog(this);
    keyManager = new KeyManager(this);
    about = new About(this);

    // Main Window:

    splitter = new QSplitter(this);
    setCentralWidget(splitter);
    setAcceptDrops(true);

    QMenuBar *menu = new QMenuBar(this);
    setMenuBar(menu);
    menuFile = new QMenu(this);
    menuIcon = new QMenu(this);
    menuSett = new QMenu(this);
    menuHelp = new QMenu(this);
    btnDonate = new QToolButton(this);
    btnDonate->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnDonate->setIcon(QPixmap(":/gfx/actions/donate.png"));
    menu->addMenu(menuFile);
    menu->addMenu(menuIcon);
    menu->addMenu(menuSett);
    menu->addMenu(menuHelp);
    menu->setCornerWidget(btnDonate);
    connect(btnDonate, SIGNAL(clicked()), this, SLOT(donate()));

    actApkOpen = new QAction(this);
    actApkSave = new QAction(this);
    menuRecent = new QMenu(this);
    actApkExplore = new QAction(this);
    actApkClose = new QAction(this);
    actExit = new QAction(this);
    actRecentClear = new QAction(this);
    actNoRecent = new QAction(this);
    iconActions = new QActionGroup(this);
    actIconOpen = new QAction(iconActions);
    actIconSave = new QAction(iconActions);
    menuIconAdd = new QMenu(this);
    actIconRemove = new QAction(iconActions);
    actIconScale = new QAction(iconActions);
    actIconResize = new QAction(iconActions);
    actIconRevert = new QAction(iconActions);
    actIconEffect = new QAction(iconActions);
    actIconClone = new QAction(iconActions);
    actIconBackground = new QAction(this);
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
    menuFile->addAction(actApkClose);
    menuFile->addSeparator();
    menuFile->addAction(actExit);
    menuIcon->addActions(iconActions->actions());
    menuIcon->addSeparator();
    menuIcon->addMenu(menuIconAdd);
    menuIcon->addSeparator();
    menuIcon->addAction(actIconBackground);
    menuSett->addAction(actPacking);
    menuSett->addAction(actKeys);
    menuSett->addSeparator();
    menuSett->addMenu(menuLang);
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
    actNoRecent->setEnabled(false);
    actAutoUpdate->setCheckable(true);

    actApkOpen->setShortcut(QKeySequence::Open);
    actApkExplore->setShortcut(QKeySequence("Ctrl+D"));
    actApkSave->setShortcut(QKeySequence("Ctrl+E"));
    actIconOpen->setShortcut(QKeySequence("Ctrl+R"));
    actIconSave->setShortcut(QKeySequence::Save);
    actIconRemove->setShortcut(QKeySequence::Delete);
    actIconScale->setShortcut(QKeySequence("Ctrl+W"));
    actIconResize->setShortcut(QKeySequence("Ctrl+I"));
    actIconRevert->setShortcut(QKeySequence::Undo);
    actIconEffect->setShortcut(QKeySequence("Ctrl+F"));
    actIconClone->setShortcut(QKeySequence("Ctrl+C"));
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
    actApkClose->setIcon(QIcon(":/gfx/actions/remove.png"));
    actIconOpen->setIcon(QIcon(":/gfx/actions/open-icon.png"));
    actIconSave->setIcon(QIcon(":/gfx/actions/save.png"));
    actIconRemove->setIcon(QIcon(":/gfx/actions/remove.png"));
    actIconScale->setIcon(QIcon(":/gfx/actions/scale.png"));
    actIconResize->setIcon(QIcon(":/gfx/actions/resize.png"));
    actIconRevert->setIcon(QIcon(":/gfx/actions/undo.png"));
    actIconEffect->setIcon(QIcon(":/gfx/actions/effects.png"));
    actIconClone->setIcon(QIcon((":/gfx/actions/copy-icon.png")));
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

    QPushButton *btnStudio = new QPushButton(this);
    btnStudio->setText("APK Editor Studio");
    btnStudio->setIcon(QPixmap(":/gfx/apk-editor-studio.png"));
    btnStudio->setFixedHeight(32);
    btnStudio->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    btnStudio->hide();
    connect(btnStudio, &QPushButton::clicked, []() {
        QDesktopServices::openUrl(QUrl("https://kefir500.github.io/apk-editor-studio"));
    });

    loadingDialog = new ProgressDialog(this);
    loadingDialog->setIcon(QPixmap(":/gfx/actions/box.png"));

    uploadDialog = new ProgressDialog(this);

    tabIcons = new QWidget(this);
    QHBoxLayout *layoutDevices = new QHBoxLayout;
    QVBoxLayout *layoutIcons = new QVBoxLayout(tabIcons);
    devicesLabel = new QLabel(this);
    devices = new QComboBox(this);
    devices->setStyleSheet("QComboBox {padding: 4px}");
    devices->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layoutDevices->addWidget(devicesLabel);
    layoutDevices->addWidget(devices);

    listIcons = new QListView(this);
    listIcons->addActions(menuIcon->actions());
    listIcons->setContextMenuPolicy(Qt::ActionsContextMenu);
    listIcons->setItemDelegate(new DecorationDelegate(QSize(32, 32), this));
    iconsProxy = new IconsProxy(this);
    listIcons->setModel(iconsProxy);

    actAddIconLdpi = new QAction("LDPI", this);
    actAddIconMdpi = new QAction("MDPI", this);
    actAddIconHdpi = new QAction("HDPI", this);
    actAddIconXhdpi = new QAction("XHDPI", this);
    actAddIconXxhdpi = new QAction("XXHDPI", this);
    actAddIconXxxhdpi = new QAction("XXXHDPI", this);
    actAddIconTv = new QAction(this);
    actAddIconTv->setIcon(QIcon(":/gfx/dpi/tv.png"));
    menuIconAdd->setIcon(QIcon(":/gfx/actions/add.png"));
    menuIconAdd->addAction(actAddIconLdpi);
    menuIconAdd->addAction(actAddIconMdpi);
    menuIconAdd->addAction(actAddIconHdpi);
    menuIconAdd->addAction(actAddIconXhdpi);
    menuIconAdd->addAction(actAddIconXxhdpi);
    menuIconAdd->addAction(actAddIconXxxhdpi);
    menuIconAdd->addAction(actAddIconTv);
    btnAddIcon = new QToolButton(this);
    btnAddIcon->setShortcut(QKeySequence("+"));
    btnAddIcon->setIcon(QIcon(":/gfx/actions/add.png"));
    btnAddIcon->setMenu(menuIconAdd);
    btnAddIcon->setPopupMode(QToolButton::InstantPopup);
    btnAddIcon->setStyleSheet("QToolButton::menu-indicator { image: none; width: 0; }");
    btnRemoveIcon = new QToolButton(this);
    btnOpenIcon = new QToolButton(this);
    btnSaveIcon = new QToolButton(this);
    btnScaleIcon = new QToolButton(this);
    btnResizeIcon = new QToolButton(this);
    btnRevertIcon = new QToolButton(this);
    btnEffectIcon = new QToolButton(this);
    btnCloneIcons = new QToolButton(this);
    btnRemoveIcon->setDefaultAction(actIconRemove);
    btnOpenIcon->setDefaultAction(actIconOpen);
    btnSaveIcon->setDefaultAction(actIconSave);
    btnScaleIcon->setDefaultAction(actIconScale);
    btnResizeIcon->setDefaultAction(actIconResize);
    btnRevertIcon->setDefaultAction(actIconRevert);
    btnEffectIcon->setDefaultAction(actIconEffect);
    btnCloneIcons->setDefaultAction(actIconClone);
    btnAddIcon->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    btnRemoveIcon->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    btnOpenIcon->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    btnSaveIcon->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    btnScaleIcon->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    btnResizeIcon->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    btnRevertIcon->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    btnEffectIcon->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    btnCloneIcons->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    QHBoxLayout *layoutIconsButtons = new QHBoxLayout;
    layoutIconsButtons->addWidget(btnAddIcon);
    layoutIconsButtons->addWidget(btnRemoveIcon);
    layoutIconsButtons->addWidget(btnOpenIcon);
    layoutIconsButtons->addWidget(btnSaveIcon);
    layoutIconsButtons->addWidget(btnScaleIcon);
    layoutIconsButtons->addWidget(btnResizeIcon);
    layoutIconsButtons->addWidget(btnRevertIcon);
    layoutIconsButtons->addWidget(btnEffectIcon);
    layoutIconsButtons->addWidget(btnCloneIcons);
    layoutIconsButtons->setSpacing(2);

    layoutIcons->addLayout(layoutDevices);
    layoutIcons->addWidget(listIcons);
    layoutIcons->addLayout(layoutIconsButtons);
    layoutIcons->setMargin(4);
    layoutIcons->setSpacing(6);

    tabTranslations = new QWidget(this);
    QVBoxLayout *layoutTranslations = new QVBoxLayout(tabTranslations);
    tableTitles = new QTableView(this);
    tableTitles->verticalHeader()->setVisible(false);
    tableTitles->setSelectionMode(QTableView::SingleSelection);
    tableTitles->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableTitles->setHorizontalScrollMode(QTableView::ScrollPerPixel);
    tableTitles->setVerticalScrollMode(QTableView::ScrollPerPixel);
    btnApplyAppName = new QPushButton(this);
    layoutTranslations->addWidget(tableTitles);
    layoutTranslations->addWidget(btnApplyAppName);
    layoutTranslations->setMargin(4);

    tabProperties = new QWidget(this);
    QVBoxLayout *layoutProperties = new QVBoxLayout(tabProperties);
    tableManifest = new QTableView(this);
    tableManifest->horizontalHeader()->hide();
    tableManifest->horizontalHeader()->setStretchLastSection(true);
    tableManifest->setSelectionMode(QTableView::SingleSelection);
    tableManifest->setHorizontalScrollMode(QTableView::ScrollPerPixel);
    tableManifest->setVerticalScrollMode(QTableView::ScrollPerPixel);
    layoutProperties->addWidget(tableManifest);
    layoutProperties->setMargin(4);

    tabs = new QTabWidget(this);
    tabs->addTab(tabIcons, NULL);
    tabs->addTab(tabTranslations, NULL);
    tabs->addTab(tabProperties, NULL);

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
    btnPack->setFixedHeight(64);

    QWidget *sidebar = new QWidget(this);
    QVBoxLayout *layoutSide = new QVBoxLayout(sidebar);
    layoutSide->setMargin(0);
    layoutSide->addWidget(tabs);
    layoutSide->addWidget(checkDropbox);
    layoutSide->addWidget(checkGDrive);
    layoutSide->addWidget(checkOneDrive);
    layoutSide->addWidget(checkUpload);
    layoutSide->addWidget(btnStudio);
    layoutSide->addWidget(btnPack);

    splitter->addWidget(drawArea);
    splitter->addWidget(sidebar);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setStyleSheet("QSplitter {padding: 8px;}");

    setInitialSize();
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

    const QDir LANGPATH(Path::Data::shared() + "lang");
    langs << LANGPATH.entryList(QStringList() << "apk-icon-editor.*");

    for (int i = 0; i < langs.size(); ++i) {

        // Get native language title:

        QString locale = langs[i].split('.')[1];
        QString title = QLocale(locale).nativeLanguageName();
        if (title.size() > 1) {
            title[0] = title[0].toUpper();
        }

        // Set up menu action:

        QAction *actLang = new QAction(this);
        actLang->setText(title);
        actLang->setIcon(QIcon(QString("%1/flag.%2.png").arg(LANGPATH.absolutePath(), locale)));
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
    devices->setModel(&deviceModel);
    Device *firstDevice = static_cast<Device *>(deviceModel.index(0, 0).internalPointer());
    iconsProxy->setDevice(firstDevice);
}

void MainWindow::init_slots()
{
    connect(drawArea, SIGNAL(clicked()), this, SLOT(apk_open()));
    connect(checkUpload, SIGNAL(toggled(bool)), this, SLOT(enableUpload(bool)));
    connect(actApkOpen, SIGNAL(triggered()), this, SLOT(apk_open()));
    connect(actApkSave, SIGNAL(triggered()), this, SLOT(apk_save()));
    connect(actApkExplore, SIGNAL(triggered()), this, SLOT(apk_explore()));
    connect(actApkClose, SIGNAL(triggered()), this, SLOT(apk_close()));
    connect(actExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actRecentClear, SIGNAL(triggered()), this, SLOT(recent_clear()));
    connect(actIconOpen, SIGNAL(triggered()), this, SLOT(icon_open()));
    connect(actIconSave, SIGNAL(triggered()), this, SLOT(icon_save()));
    connect(actIconRemove, SIGNAL(triggered()), this, SLOT(removeIcon()));
    connect(actIconScale, SIGNAL(triggered()), this, SLOT(icon_scale()));
    connect(actIconResize, SIGNAL(triggered()), this, SLOT(icon_resize()));
    connect(actIconRevert, SIGNAL(triggered()), this, SLOT(icon_revert()));
    connect(actIconEffect, SIGNAL(triggered()), this, SLOT(showEffectsDialog()));
    connect(actIconClone, SIGNAL(triggered()), this, SLOT(cloneIcons()));
    connect(actIconBackground, SIGNAL(triggered()), this, SLOT(setPreviewColor()));
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
    connect(actAbout, SIGNAL(triggered()), about, SLOT(exec()));
    connect(actAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(btnPack, SIGNAL(clicked()), this, SLOT(apk_save()));
    connect(mapRecent, SIGNAL(mapped(QString)), this, SLOT(apk_open(QString)));
    connect(actAddIconLdpi, &QAction::triggered, [=]() { apk->addIcon(Icon::Ldpi); });
    connect(actAddIconMdpi, &QAction::triggered, [=]() { apk->addIcon(Icon::Mdpi); });
    connect(actAddIconHdpi, &QAction::triggered, [=]() { apk->addIcon(Icon::Hdpi); });
    connect(actAddIconXhdpi, &QAction::triggered, [=]() { apk->addIcon(Icon::Xhdpi); });
    connect(actAddIconXxhdpi, &QAction::triggered, [=]() { apk->addIcon(Icon::Xxhdpi); });
    connect(actAddIconXxxhdpi, &QAction::triggered, [=]() { apk->addIcon(Icon::Xxxhdpi); });
    connect(actAddIconTv, &QAction::triggered, [=]() { apk->addIcon(Icon::TvBanner); });
    void (QComboBox::*devicesIndexChanged)(int row) = &QComboBox::currentIndexChanged;
    connect(devices, devicesIndexChanged, [=](int row) {
        Device *device = static_cast<Device *>(devices->model()->index(row, 0).internalPointer());
        iconsProxy->setDevice(device);
        setCurrentIcon(listIcons->currentIndex().row());
    });
    connect(listIcons->selectionModel(), &QItemSelectionModel::currentChanged, [=](const QModelIndex &index) {
        setCurrentIcon(index.row());
    });
    connect(btnApplyAppName, SIGNAL(clicked()), this, SLOT(applyAppName()));
    connect(apkManager, SIGNAL(loading(short, QString)), loadingDialog, SLOT(setProgress(short, QString)));
    connect(apkManager, SIGNAL(error(QString, QString, QString)), this, SLOT(error(QString, QString, QString)));
    connect(apkManager, SIGNAL(error(QString, QString, QString)), loadingDialog, SLOT(accept()));
    connect(apkManager, SIGNAL(packed(Apk::File*, bool, QString, QString)), this, SLOT(apk_packed(Apk::File*, bool, QString, QString)));
    connect(apkManager, SIGNAL(unpacked(Apk::File*)), this, SLOT(apk_unpacked(Apk::File*)));
    connect(loadingDialog, SIGNAL(rejected()), apkManager, SLOT(cancel()));
    connect(toolDialog, &ToolDialog::accepted, [=]() { checkReqs(); });
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
    connect(this, SIGNAL(reqsChecked(QString, QString, QString)), about, SLOT(setVersions(QString, QString, QString)));
}

void MainWindow::settings_load()
{
    checkReqs();

    // Global:

    restoreGeometry(Settings::get_geometry());
    splitter->restoreState(Settings::get_splitter());
    setLanguage(Settings::get_language());
    currentPath = Settings::get_last_path();
    devices->setCurrentText(Settings::get_device());
    actAutoUpdate->setChecked(Settings::get_update());

    // Recent List:

    if (recent) { delete recent; }
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

bool MainWindow::resetApktool()
{
    const QString FRAMEWORK = Settings::get_temp() + "/apk-icon-editor/framework/1.apk";
    return QFile::remove(FRAMEWORK);
}

void MainWindow::settings_reset()
{
    if (QMessageBox::question(this, tr("Reset?"), tr("Reset settings to default?")) == QMessageBox::Yes) {
        Settings::reset();
        settings_load();
        resetApktool();
        setInitialSize();
    }
}

void MainWindow::setInitialSize()
{
#ifndef Q_OS_OSX
    resize(Gui::Screen::scaled(800, 540));
#else
    resize(Gui::Screen::scaled(880, 540));
#endif
    splitter->setSizes(QList<int>() << Gui::Screen::scaled(492)
                                    << Gui::Screen::scaled(280));
}

void MainWindow::setLanguage(QString lang)
{
    qDebug() << "Language set to" << lang << "\n";
    const QString LANGPATH(Path::Data::shared() + "lang");
    QApplication::removeTranslator(translator);
    QApplication::removeTranslator(translatorQt);
    if (translator->load(QString("apk-icon-editor.%1").arg(lang), LANGPATH)) {
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
    tabs->setTabText(1, tr("Translations"));
    tabs->setTabText(2, tr("Properties")); // tr("Details")
    devicesLabel->setText(tr("Device:"));
    btnApplyAppName->setText(tr("Apply to All"));
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
    actApkClose->setText(tr("&Close APK"));
    actExit->setText(tr("E&xit"));
    actRecentClear->setText(tr("&Clear List"));
    actNoRecent->setText(tr("No Recent Files"));
    actIconOpen->setText(tr("Replace &Icon"));
    actIconSave->setText(tr("&Save Icon"));
    actIconRemove->setText(tr("Remove Icon"));
    actIconScale->setText(tr("Scale to &Fit"));
    actIconResize->setText(tr("&Resize Icon"));
    actIconRevert->setText(tr("Revert &Original"));
    actIconEffect->setText(tr("E&ffects"));
    actIconClone->setText(tr("Apply to All"));
    menuIconAdd->setTitle(tr("&Add Icon"));
    actAddIconTv->setText(QCoreApplication::translate("Icon", "TV Banner"));
    btnAddIcon->setToolTip(tr("&Add Icon").remove('&'));
    actIconBackground->setText(tr("Preview Background &Color"));
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
    btnDonate->setText(tr("Donate"));
    loadingDialog->setWindowTitle(tr("Processing"));
    uploadDialog->setWindowTitle(tr("Uploading"));
    menuBar()->resize(0, 0); // "Repaint" menu bar

    effects->retranslate();
    toolDialog->retranslate();
    keyManager->retranslate();
    about->retranslate();
}

void MainWindow::recent_add(QString filename)
{
    recent->add(filename, apk->getThumbnail().pixmap(32, 32));
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

void MainWindow::cloneIcons()
{
    Icon *newIcon = drawArea->getIcon();
    if (!newIcon->isNull()) {
        if (QMessageBox::question(this, NULL, tr("Apply the current icon to all sizes?")) == QMessageBox::Yes) {
            apk->iconsModel.clone(newIcon);
            setWindowModified(true);
        }
    }
}

void MainWindow::applyAppName()
{
    QModelIndex index = tableTitles->currentIndex();
    if (index.isValid()) {
        if (QMessageBox::question(this, NULL, tr("Apply the current application name to all translations?")) == QMessageBox::Yes) {
            const QString title = index.sibling(index.row(), 0).data().toString();
            static_cast<TitlesModel *>(tableTitles->model())->applyToAll(title);
        }
    }
}

void MainWindow::setCurrentIcon(int index)
{
    if (!apk || index < 0) {
        return;
    }

    Icon *icon = static_cast<Icon *>(apk->iconsModel.index(index, 0).internalPointer());
    const Device *device = static_cast<Device *>(devices->model()->index(devices->currentIndex(), 0).internalPointer());
    const QSize size = device->getStandardSize(icon->getType()).size;
    drawArea->setBounds(size.width(), size.height());

    if (icon) {
        disconnect(effects, 0, 0, 0);
        connect(effects, SIGNAL(colorActivated(bool)), icon,     SLOT(setColorize(bool)), Qt::DirectConnection);
        connect(effects, SIGNAL(rotate(int)),          icon,     SLOT(setAngle(int)),     Qt::DirectConnection);
        connect(effects, SIGNAL(flipX(bool)),          icon,     SLOT(setFlipX(bool)),    Qt::DirectConnection);
        connect(effects, SIGNAL(flipY(bool)),          icon,     SLOT(setFlipY(bool)),    Qt::DirectConnection);
        connect(effects, SIGNAL(colorize(QColor)),     icon,     SLOT(setColor(QColor)),  Qt::DirectConnection);
        connect(effects, SIGNAL(colorDepth(qreal)),    icon,     SLOT(setDepth(qreal)),   Qt::DirectConnection);
        connect(effects, SIGNAL(blur(qreal)),          icon,     SLOT(setBlur(qreal)),    Qt::DirectConnection);
        connect(effects, SIGNAL(round(qreal)),         icon,     SLOT(setCorners(qreal)), Qt::DirectConnection);
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

void MainWindow::apk_packed(Apk::File *apk, bool isSuccess, QString text, QString details)
{
    loadingDialog->accept();
    const QString FILENAME = apk->getFilePath();
    setActiveApk(FILENAME);

    const bool UPLOAD_TO_DROPBOX  = checkDropbox->isChecked();
    const bool UPLOAD_TO_GDRIVE   = checkGDrive->isChecked();
    const bool UPLOAD_TO_ONEDRIVE = checkOneDrive->isChecked();

    if (isSuccess) {
        if (checkUpload->isChecked()) {
            if (UPLOAD_TO_DROPBOX)  upload(dropbox, FILENAME);
            if (UPLOAD_TO_GDRIVE)   upload(gdrive, FILENAME);
            if (UPLOAD_TO_ONEDRIVE) upload(onedrive, FILENAME);
        }
        uploadDialog->accept();
        success(NULL, text, details);
    }
    else {
        warning(NULL, text, details);
        if (checkUpload->isChecked()) {
            if (UPLOAD_TO_DROPBOX)  upload(dropbox, FILENAME);
            if (UPLOAD_TO_GDRIVE)   upload(gdrive, FILENAME);
            if (UPLOAD_TO_ONEDRIVE) upload(onedrive, FILENAME);
        }
        uploadDialog->accept();
    }
}

void MainWindow::apk_unpacked(Apk::File *apk)
{
    this->apk = apk;
    loadingDialog->accept();
    setActiveApk(apk->getFilePath());

    // Set models:

    iconsProxy->setSourceModel(&apk->iconsModel);
    tableManifest->setModel(&apk->manifestModel);
    tableTitles->setModel(&apk->titlesModel);
    tableTitles->resizeColumnsToContents();
    listIcons->setCurrentIndex(listIcons->model()->index(0, 0));
    connect(&apk->manifestModel, &ManifestModel::dataChanged, [=]() { setWindowModified(true); });
    connect(&apk->titlesModel, &TitlesModel::dataChanged, [=]() { setWindowModified(true); });

    // Enable operations with APK and icons:

    tabIcons->setEnabled(true);
    tabTranslations->setEnabled(true);
    tabProperties->setEnabled(true);
    actApkSave->setEnabled(true);
    actApkExplore->setEnabled(true);
    actApkClose->setEnabled(true);
    iconActions->setEnabled(true);
    menuIconAdd->setEnabled(true);
    btnPack->setEnabled(true);

    setWindowModified(false);
}

bool MainWindow::icon_open(QString filename)
{
    Icon *icon = drawArea->getIcon();

    if (icon->isNull()) {
        invalidDpi();
        return false;
    }

    if (filename.isEmpty()) {
        filename = QFileDialog::getOpenFileName(this, tr("Import Icon"), NULL, Image::Formats::openDialogFilter());
        if (filename.isEmpty()) {
            return false;
        }
    }

    QPixmap backup = icon->getPixmap();

    if (icon->replace(QPixmap(filename))) {
        repaint();
        const Device *device = static_cast<Device *>(devices->model()->index(devices->currentIndex(), 0).internalPointer());
        const QSize size = device->getStandardSize(icon->getType()).size;

        if (icon->width() != size.width() || icon->height() != size.height()) {
            int result = QMessageBox::warning(this, tr("Resize?"),
                                              tr("Icon you are trying to load is off-size.\nResize automatically?"),
                                              QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
            switch (result) {
            case QMessageBox::Yes:
                icon->resize(size);
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

bool MainWindow::icon_save(QString filename)
{
    Icon *icon = drawArea->getIcon();

    if (icon->isNull()) {
        invalidDpi();
        return false;
    }

    if (filename.isEmpty()) {
        const Device *device = static_cast<Device *>(devices->model()->index(devices->currentIndex(), 0).internalPointer());
        const QSize size = device->getStandardSize(icon->getType()).size;
        filename = QString("%1-%2x%3").arg(QFileInfo(currentApk).completeBaseName()).arg(size.width()).arg(size.height());
        filename = QFileDialog::getSaveFileName(this, tr("Save Icon"), filename, Image::Formats::saveDialogFilter());
        if (filename.isEmpty()) {
            return false;
        }
    }
    return icon->save(filename);
}

bool MainWindow::icon_scale()
{
    const Device *device = static_cast<Device *>(devices->model()->index(devices->currentIndex(), 0).internalPointer());
    const QSize size = device->getStandardSize(drawArea->getIcon()->getType()).size;
    return icon_resize(size);
}

bool MainWindow::icon_resize(QSize size)
{
    Icon *icon = drawArea->getIcon();

    if (icon->isNull()) {
        invalidDpi();
        return false;
    }

    if (!size.isValid()) {
        const int WIDTH = icon->width();
        const int HEIGHT = icon->height();
        size = Dialogs::getSize(tr("Resize Icon"), WIDTH, HEIGHT, this);
        if (!size.isValid()) {
            return false;
        }
    }

    setWindowModified(true);
    bool result = icon->resize(size);
    drawArea->repaint();
    return result;
}

bool MainWindow::icon_revert()
{
    Icon *icon = drawArea->getIcon();

    if (icon->isNull()) {
        invalidDpi();
        return false;
    }

    bool result = icon->revert();
    drawArea->repaint();
    return result;
}

bool MainWindow::setPreviewColor()
{
    const QColor DEFAULT = drawArea->palette().color(QPalette::Background);
    const QColor COLOR = QColorDialog::getColor(DEFAULT, this);
    if (COLOR.isValid()) {
        drawArea->setBackground(COLOR);
        QPixmap icon(32, 32);
        icon.fill(COLOR);
        actIconBackground->setIcon(QIcon(icon));
        return true;
    }
    else {
        return false;
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
        effects->setColorize(TEMP_IS_COLOR);
        effects->setColor(TEMP_COLOR);
        effects->setColorDepth(TEMP_DEPTH * 100);
        effects->setBlur(TEMP_BLUR * 10);
        effects->setCorners(TEMP_ROUND);
        if (effects->exec() == QDialog::Accepted) {
            setWindowModified(true);
        }
        else {
            icon->setAngle(TEMP_ANGLE);
            icon->setFlipX(TEMP_FLIP_X);
            icon->setFlipY(TEMP_FLIP_Y);
            icon->setColorize(TEMP_IS_COLOR);
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

    loadingDialog->setProgress(20, QApplication::translate("Apk::Unpacker", "Unpacking APK..."));
    QApplication::processEvents();

    const QString DEST = Settings::get_temp() + "/apk-icon-editor/";
    const bool SMALI = Settings::get_smali();
    const QString APKTOOL = Settings::get_apktool();
    apk_close();
    apkManager->unpack(filename, DEST, APKTOOL, SMALI);

    return true;
}

bool MainWindow::apk_save(QString filename)
{
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
            pass_store = Dialogs::getPassword(tr("Enter the KeyStore password:"), "", PIXMAP_KEY, this);
        }
        if (alias.isEmpty()) {
            alias = Dialogs::getString(tr("Enter the alias:"), "", PIXMAP_KEY, this);
        }
        if (pass_alias.isEmpty()) {
            pass_alias = Dialogs::getPassword(tr("Enter the alias password:"), "", PIXMAP_KEY, this);
        }
    }

    // Saving APK:

    currentPath = QFileInfo(filename).absolutePath();
    loadingDialog->setProgress(0);

    apk->setFilePath(filename);
    apk->setApktool(Settings::get_apktool());
    apk->setSmali(Settings::get_smali());
    apk->setSign(Settings::get_sign());
    apk->setZipalign(Settings::get_zipalign());
    apk->setApksigner(Settings::get_use_apksigner());
    apk->setFilePemPk8(Settings::get_pem(), Settings::get_pk8());
    apk->setFileKeystore(Settings::get_keystore(), alias, pass_store, pass_alias);
    apk->setKeystore(USING_KEYSTORE);

    apkManager->pack(apk, Settings::get_temp() + "/apk-icon-editor/");
    return true;
}

void MainWindow::apk_explore()
{
    const QString TEMPDIR = Settings::get_temp() + "/apk-icon-editor/apk";
    QDesktopServices::openUrl(QUrl::fromLocalFile(TEMPDIR));
}

void MainWindow::apk_close()
{
    apkManager->close();

    setWindowTitle(QString());
    setWindowModified(false);

    iconsProxy->setSourceModel(NULL);
    tableManifest->setModel(NULL);
    tableTitles->setModel(NULL);

    tabIcons->setEnabled(false);
    tabTranslations->setEnabled(false);
    tabProperties->setEnabled(false);
    actApkSave->setEnabled(false);
    actApkExplore->setEnabled(false);
    actApkClose->setEnabled(false);
    iconActions->setEnabled(false);
    menuIconAdd->setEnabled(false);
    btnPack->setEnabled(false);

    drawArea->setIcon(NULL);
}

void MainWindow::associate() const
{
#ifdef Q_OS_WIN
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
    QDesktopServices::openUrl(QUrl::fromLocalFile(Path::Data::shared() + "faq.txt"));
}

void MainWindow::openLogFile() const
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(Path::Log::file()));
}

void MainWindow::openLogPath() const
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(Path::Log::dir()));
}

void MainWindow::donate()
{
    QDesktopServices::openUrl(Url::DONATE);
}

void MainWindow::invalidDpi()
{
    warning(tr("Icon Missing"),
            tr("This APK does not support current DPI."));
}

void MainWindow::browseTranslate() const
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
    QString code = Dialogs::getPassword(tr("Allow access to %1 in your browser and paste the provided code here:").arg(cloud->getTitle()),
                                        tr("Authorization"),
                                        cloud->getIcon(), this);
    if (!code.isEmpty()) {
        cloud->login(code);
        uploadDialog->activateWindow();
    }
    else {
        cloud->cancel();
    }
}

void MainWindow::message(QString title, QString text, QString details, QMessageBox::Icon type)
{
    if (type == QMessageBox::Warning) {
        qDebug() << qPrintable(QString("Warning (%1): %2").arg(title).arg(text));
    } else if (type == QMessageBox::Critical) {
        qDebug() << qPrintable(QString("Error (%1): %2").arg(title).arg(text));
    }
    QApplication::alert(this);
    QMessageBox box(this);
    box.setWindowTitle(title);
    box.setText(text);
    box.setIcon(type);
    box.setStandardButtons(QMessageBox::Ok);
    box.setDetailedText(details);
    if (!details.isEmpty()) {
        QSpacerItem *spacer = new QSpacerItem(320, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout *layout = static_cast<QGridLayout *>(box.layout());
        layout->addItem(spacer, layout->rowCount(), 0, 1, layout->columnCount());
    }
    box.exec();
}

void MainWindow::success(QString title, QString text, QString details)
{
    message(title, text, details, QMessageBox::Information);
}

void MainWindow::warning(QString title, QString text, QString details)
{
    message(title, text, details, QMessageBox::Warning);
}

void MainWindow::error(QString title, QString text, QString details)
{
    message(title, text, details, QMessageBox::Critical);
}

void MainWindow::removeIcon()
{
    Icon *icon = drawArea->getIcon();
    if (icon && QMessageBox::question(this, QString(), tr("Are you sure you want to delete this icon?")) == QMessageBox::Yes) {
        apk->removeIcon(icon);
        setWindowModified(true);
    }
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
    if (mimeData->hasUrls()) {
        const QString filename = mimeData->urls().at(0).toLocalFile();
        const QString ext = QFileInfo(filename).suffix().toLower();
        if (Image::Formats::supported().contains(ext)) {
            icon_open(filename);
        }
        else if (ext == "apk") {
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
    Settings::set_device(devices->currentText());
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
    qInstallMessageHandler(0);
}
