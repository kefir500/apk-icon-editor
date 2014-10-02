#include "mainwindow.h"
#include "main.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QColorDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QMimeData>
#include <QDesktopServices>
#include <QApplication>

MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) : QMainWindow(parent)
{
    resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setAcceptDrops(true);

    apk = new Apk(this);
    effects = new EffectsDialog(this);
    updater = new Updater(this);
    translator = new QTranslator(this);
    translatorQt = new QTranslator(this);
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "apk-icon-editor", "config", this);

    dropbox = new Dropbox(this);
    dropbox->setIcon(QPixmap(":/gfx/icon-dropbox.png"));
    gdrive = new GoogleDrive(this);
    gdrive->setIcon(QPixmap(":/gfx/icon-gdrive.png"));
    onedrive = new OneDrive(this);
    onedrive->setIcon(QPixmap(":/gfx/icon-onedrive.png"));

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
    menuLang = new QMenu(this);
    menuPack = new QMenu(this);
    menuRatio = new QMenu(this);
    groupRatio = new QActionGroup(this);
    actRatio0 = new QAction(this);
    actRatio1 = new QAction(this);
    actRatio3 = new QAction(this);
    actRatio5 = new QAction(this);
    actRatio7 = new QAction(this);
    actRatio9 = new QAction(this);
    actPackSign = new QAction(this);
    actPackOptimize = new QAction(this);
    actAutoUpdate = new QAction(this);
    actAssoc = new QAction(this);
    actReset = new QAction(this);
    actWebsite = new QAction(this);
    actReport = new QAction(this);
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
    menuSett->addMenu(menuLang);
    menuSett->addMenu(menuPack);
    menuSett->addAction(actAutoUpdate);
    menuSett->addSeparator();
    menuSett->addAction(actAssoc);
    menuSett->addAction(actReset);
    menuPack->addMenu(menuRatio);
    menuPack->addAction(actPackSign);
    menuPack->addAction(actPackOptimize);
    groupRatio->addAction(actRatio0);
    groupRatio->addAction(actRatio1);
    groupRatio->addAction(actRatio3);
    groupRatio->addAction(actRatio5);
    groupRatio->addAction(actRatio7);
    groupRatio->addAction(actRatio9);
    actRatio0->setData(0);
    actRatio1->setData(1);
    actRatio3->setData(3);
    actRatio5->setData(0);
    actRatio7->setData(7);
    actRatio9->setData(9);
    menuRatio->addActions(groupRatio->actions());
    menuHelp->addAction(actWebsite);
    menuHelp->addAction(actReport);
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
    actIconScale->setVisible(false); // not sure about its necessity
    actIconResize->setEnabled(false);
    actIconRevert->setEnabled(false);
    actIconEffect->setEnabled(false);
    actRatio0->setCheckable(true);
    actRatio1->setCheckable(true);
    actRatio3->setCheckable(true);
    actRatio5->setCheckable(true);
    actRatio7->setCheckable(true);
    actRatio9->setCheckable(true);
    actPackSign->setCheckable(true);
    actPackOptimize->setCheckable(true);
    actAutoUpdate->setCheckable(true);
    actApkSave->setIcon(QIcon(":/gfx/task-pack.png"));
    actApkOpen->setShortcut(QKeySequence("Ctrl+O"));
    actApkSave->setShortcut(QKeySequence("Ctrl+E"));
    actApkExplore->setShortcut(QKeySequence("Ctrl+D"));
    actIconOpen->setShortcut(QKeySequence("Ctrl+R"));
    actIconSave->setShortcut(QKeySequence("Ctrl+S"));
    actIconResize->setShortcut(QKeySequence("Ctrl+I"));
    actIconRevert->setShortcut(QKeySequence("Ctrl+Z"));
    actIconEffect->setShortcut(QKeySequence("F"));
    actIconEffect->setIcon(QIcon(":/gfx/effects.png"));
    actReport->setIcon(QIcon(":/gfx/bug.png"));
    actExit->setShortcut(QKeySequence("Ctrl+Q"));

    drawArea = new DrawArea(this);
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    drawArea->setContextMenuPolicy(Qt::ActionsContextMenu);
    drawArea->addAction(actApkOpen);
    drawArea->addAction(separator);
    drawArea->addActions(menuIcon->actions());
    drawArea->setSizePolicy(QSizePolicy::MinimumExpanding,
                            QSizePolicy::MinimumExpanding);

    loadingDialog = new ProgressDialog(this);
    loadingDialog->setIcon(QPixmap(":/gfx/task-pack.png"));
    loadingDialog->setAllowCancel(false);

    uploadDialog = new ProgressDialog(this);

    devices = new ComboList(this);
    devices->addActions(menuIcon->actions());
    devices->setEnabled(false);
    tabProps = new QTableWidget(this);
    tabProps->setRowCount(6);
    tabProps->setColumnCount(2);
    tabProps->verticalHeader()->setVisible(false);
    tabProps->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabProps->setEditTriggers(QTableWidget::NoEditTriggers);
    tabProps->setSelectionMode(QTableWidget::SingleSelection);
    tabProps->setEnabled(false);

    checkDropbox = new QCheckBox(this);
    checkDropbox->setIcon(dropbox->getIcon());
    checkGDrive = new QCheckBox(this);
    checkGDrive->setIcon(gdrive->getIcon());
    checkOneDrive = new QCheckBox(this);
    checkOneDrive->setIcon(onedrive->getIcon());
    checkUpload = new QCheckBox(this);
    checkUpload->setChecked(true);
    checkUpload->setIcon(QPixmap(":/gfx/icon-upload.png"));
    btnPack = new QPushButton(this);
    btnPack->setEnabled(false);
    btnPack->setFixedHeight(64);

    tabs = new QTabWidget(this);
    tabs->setContentsMargins(5, 5, 5, 5);
    tabs->addTab(devices, NULL);
    tabs->addTab(tabProps, NULL);
    tabs->setStyleSheet("padding: 3px;");
    tabProps->setStyleSheet("padding: 0");

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

    mapRecent = new QSignalMapper(this);

    initLanguages();
    initProfiles();
    restoreSettings();
    hideEmptyDpi();

    connect(drawArea, SIGNAL(clicked()), this, SLOT(apkLoad()));
    connect(devices, SIGNAL(groupChanged(int)), this, SLOT(hideEmptyDpi()));
    connect(devices, SIGNAL(itemChanged(int)), this, SLOT(setCurrentIcon(int)));
    connect(checkUpload, SIGNAL(toggled(bool)), this, SLOT(enableUpload(bool)));
    connect(actApkOpen, SIGNAL(triggered()), this, SLOT(apkLoad()));
    connect(actApkSave, SIGNAL(triggered()), this, SLOT(apkSave()));
    connect(actApkExplore, SIGNAL(triggered()), this, SLOT(apkExplore()));
    connect(actExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actRecentClear, SIGNAL(triggered()), this, SLOT(clearRecent()));
    connect(actIconOpen, SIGNAL(triggered()), this, SLOT(iconOpen()));
    connect(actIconSave, SIGNAL(triggered()), this, SLOT(iconSave()));
    connect(actIconScale, SIGNAL(triggered()), this, SLOT(iconScale()));
    connect(actIconResize, SIGNAL(triggered()), this, SLOT(iconResize()));
    connect(actIconRevert, SIGNAL(triggered()), this, SLOT(iconRevert()));
    connect(actIconEffect, SIGNAL(triggered()), this, SLOT(showEffectsDialog()));
    connect(actIconBack, SIGNAL(triggered()), this, SLOT(setPreviewColor()));
    connect(actAssoc, SIGNAL(triggered()), this, SLOT(associate()));
    connect(actReset, SIGNAL(triggered()), this, SLOT(resetSettings()));
    connect(actWebsite, SIGNAL(triggered()), this, SLOT(browseSite()));
    connect(actReport, SIGNAL(triggered()), this, SLOT(browseBugs()));
    connect(actUpdate, SIGNAL(triggered()), updater, SLOT(check()));
    connect(actAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(btnPack, SIGNAL(clicked()), this, SLOT(apkSave()));
    connect(mapRecent, SIGNAL(mapped(QString)), this, SLOT(apkLoad(QString)));
    connect(apk, SIGNAL(loading(short, QString)), loadingDialog, SLOT(setProgress(short, QString)), Qt::BlockingQueuedConnection);
    connect(apk, SIGNAL(error(QString, QString)), this, SLOT(error(QString, QString)));
    connect(apk, SIGNAL(error(QString, QString)), loadingDialog, SLOT(finish()));
    connect(apk, SIGNAL(warning(QString, QString)), this, SLOT(warning(QString, QString)));
    connect(apk, SIGNAL(packed(QString, bool, QString)), this, SLOT(apkPacked(QString, bool, QString)));
    connect(apk, SIGNAL(unpacked(QString)), this, SLOT(apkUnpacked(QString)));
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

    // Add default English:
    QStringList langs;
    langs << "apk-icon-editor.en";

    // Loading language list:
    const QDir LANGPATH(QApplication::applicationDirPath() + "/lang");
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
}

void MainWindow::initProfiles()
{
    Profile::init();
    for (int i = 0; i < Profile::count(); ++i) {
        Profile profile = Profile::at(i);
        devices->addGroup(profile.title(), profile.thumb());
        for (int j = 0; j <= XXHDPI; ++j) {
            Dpi DPI = static_cast<Dpi>(j);
            devices->addItem(profile.title(), profile.getDpiTitle(DPI));
        }
    }
}

void MainWindow::restoreSettings()
{
    QString locale = QLocale::system().name();

    // Read settings:
    QString sProfile = settings->value("Profile", NULL).toString();
    QString sLanguage = settings->value("Language", locale).toString();
    QString sLastDir = settings->value("Directory", "").toString();
    QByteArray sGeometry = settings->value("Geometry", NULL).toByteArray();
    QByteArray sSplitter = settings->value("Splitter", NULL).toByteArray();
    QStringList sRecent = settings->value("Recent", NULL).toStringList();
    bool sUpdate = settings->value("Update", true).toBool();

    settings->beginGroup("APK");
        short sRatio = settings->value("Compression", 9).toInt();
        bool sSign = settings->value("Sign", true).toBool();
        bool sOptimize = settings->value("Optimize", true).toBool();
        bool sUpload = settings->value("Upload", true).toBool();
    settings->endGroup();

    settings->beginGroup("Dropbox");
        QString sDropbox = settings->value("Token", "").toString();
        bool bDropbox = settings->value("Enable", false).toBool();
    settings->endGroup();

    settings->beginGroup("GDrive");
        QString sGDrive = settings->value("Token", "").toString();
        bool bGDrive = settings->value("Enable", false).toBool();
    settings->endGroup();

    settings->beginGroup("OneDrive");
        QString sOneDrive = settings->value("Token", "").toString();
        bool bOneDrive = settings->value("Enable", false).toBool();
    settings->endGroup();

    // Restore settings:

    devices->setCurrentGroup(sProfile);
    if (!sRecent.isEmpty()) {
        recent = sRecent;
        refreshRecent();
    }
    else {
        clearRecent();
    }

    switch (sRatio) {
    case 0:
        actRatio0->setChecked(true);
        break;
    case 1:
        actRatio1->setChecked(true);
        break;
    case 3:
        actRatio3->setChecked(true);
        break;
    case 5:
        actRatio5->setChecked(true);
        break;
    case 7:
        actRatio7->setChecked(true);
        break;
    case 9:
        actRatio9->setChecked(true);
        break;
    }

    currentPath = sLastDir;
    restoreGeometry(sGeometry);
    splitter->restoreState(sSplitter);
    setLanguage(sLanguage);
    actPackSign->setChecked(sSign);
    actPackOptimize->setChecked(sOptimize);
    actAutoUpdate->setChecked(sUpdate);
    dropbox->setToken(sDropbox);
    gdrive->setToken(sGDrive);
    onedrive->setToken(sOneDrive);
    checkUpload->setChecked(sUpload);
    checkDropbox->setChecked(bDropbox);
    checkGDrive->setChecked(bGDrive);
    checkOneDrive->setChecked(bOneDrive);
}

void MainWindow::resetSettings()
{
    if (QMessageBox::question(this, tr("Reset?"), tr("Reset settings to default?"))
            == QMessageBox::Yes) {
        settings->clear();
        restoreSettings();
        resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    }
}

void MainWindow::setLanguage(QString lang)
{
    qDebug() << "Language set to" << lang;
    QString LANGPATH(QApplication::applicationDirPath() + "/lang");
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
    tabs->setTabText(1, tr("Details"));
    devices->setLabelText(tr("Device:"));
    tabProps->setHorizontalHeaderLabels(QStringList() << tr("Property") << tr("Value"));
    tabProps->setItem(0, 0, new QTableWidgetItem(tr("Application Name")));
    tabProps->setItem(1, 0, new QTableWidgetItem(tr("Package Name")));
    tabProps->setItem(2, 0, new QTableWidgetItem(tr("Version Code")));
    tabProps->setItem(3, 0, new QTableWidgetItem(tr("Version Name")));
    tabProps->setItem(4, 0, new QTableWidgetItem(tr("Minimum SDK")));
    tabProps->setItem(5, 0, new QTableWidgetItem(tr("Target SDK")));
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
    menuLang->setTitle(tr("&Language"));
    menuPack->setTitle(tr("&Packing"));
    menuRatio->setTitle(tr("&Compression Ratio"));
    actRatio0->setText(QString("0 (%1)").arg(tr("No Compression")));
    actRatio1->setText(QString("1 (%1)").arg(tr("Fastest Compression")));
    actRatio3->setText(QString("3 (%1)").arg(tr("Fast Compression")));
    actRatio5->setText(QString("5 (%1)").arg(tr("Normal Compression")));
    actRatio7->setText(QString("7 (%1)").arg(tr("Maximum Compression")));
    actRatio9->setText(QString("9 (%1)").arg(tr("Ultra Compression")));
    actPackSign->setText(QString("%1 (%2)").arg(tr("&Sign APK"), tr("Important")));
    actPackOptimize->setText(tr("&Optimize APK"));
    actAutoUpdate->setText(tr("Auto-check for Updates"));
    actAssoc->setText(tr("Associate .APK"));
    actReset->setText(tr("Reset Settings"));
    actWebsite->setText(tr("Visit Website"));
    actReport->setText(tr("Report a Bug"));
    actUpdate->setText(tr("Check for &Updates"));
    actAboutQt->setText(tr("About Qt"));
    actAbout->setText(tr("About %1").arg(APP));
    loadingDialog->setWindowTitle(tr("Processing"));
    uploadDialog->setWindowTitle(tr("Uploading"));

    effects->retranslate();
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
    if (!recent.isEmpty()) {
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
    else {
        clearRecent();
    }
}

void MainWindow::clearRecent()
{
    recent.clear();
    menuRecent->clear();
    menuRecent->addSeparator();
    menuRecent->addAction(actNoRecent);
}

void MainWindow::hideEmptyDpi()
{
    for (short i = LDPI; i <= XXHDPI; ++i) {
        bool visible = false;
        const Dpi DPI = static_cast<Dpi>(i);
        if (Icon *icon = apk->getIcon(DPI)) {
            visible = !icon->isNull();
        }
        devices->setItemVisible(DPI, visible);
    }
}

void MainWindow::connectRepaintSignals()
{
    connect(effects, SIGNAL(colorActivated(bool)), drawArea, SLOT(repaint()));
    connect(effects, SIGNAL(blurActivated(bool)), drawArea, SLOT(repaint()));
    connect(effects, SIGNAL(rotate(int)), drawArea, SLOT(repaint()));
    connect(effects, SIGNAL(flipX(bool)), drawArea, SLOT(repaint()));
    connect(effects, SIGNAL(flipY(bool)), drawArea, SLOT(repaint()));
    connect(effects, SIGNAL(colorize(QColor)), drawArea, SLOT(repaint()));
    connect(effects, SIGNAL(colorDepth(qreal)), drawArea, SLOT(repaint()));
    connect(effects, SIGNAL(blur(qreal)), drawArea, SLOT(repaint()));
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
    Profile profile = Profile::at(devices->currentGroupIndex());
    int side = profile.getDpiSide(static_cast<Dpi>(id));
    drawArea->setRect(side, side);
    if (Icon *icon = apk->getIcon(static_cast<Dpi>(id))) {
        disconnect(effects, 0, 0, 0);
        connect(effects, SIGNAL(colorActivated(bool)), icon, SLOT(setColorEnabled(bool)), Qt::DirectConnection);
        connect(effects, SIGNAL(blurActivated(bool)), icon, SLOT(setBlurEnabled(bool)), Qt::DirectConnection);
        connect(effects, SIGNAL(rotate(int)), icon, SLOT(setAngle(int)), Qt::DirectConnection);
        connect(effects, SIGNAL(flipX(bool)), icon, SLOT(setFlipX(bool)), Qt::DirectConnection);
        connect(effects, SIGNAL(flipY(bool)), icon, SLOT(setFlipY(bool)), Qt::DirectConnection);
        connect(effects, SIGNAL(colorize(QColor)), icon, SLOT(setColor(QColor)), Qt::DirectConnection);
        connect(effects, SIGNAL(colorDepth(qreal)), icon, SLOT(setDepth(qreal)), Qt::DirectConnection);
        connect(effects, SIGNAL(blur(qreal)), icon, SLOT(setBlur(qreal)), Qt::DirectConnection);
        connectRepaintSignals();
        drawArea->setIcon(icon);
    }
}

void MainWindow::setActiveApk(QString filename)
{
    currentApk = filename;
    setWindowModified(false);
    setWindowTitle(QFileInfo(filename).fileName() + "[*]");
    addToRecent(filename);
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
    loop.exec();
}

void MainWindow::apkPacked(QString filename, bool isSuccess, QString text)
{
    loadingDialog->finish();
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
        uploadDialog->finish();
        success(NULL, text);
    }
    else {
        warning(NULL, text);
        if (checkUpload->isChecked()) {
            if (UPLOAD_TO_DROPBOX)  upload(dropbox, filename);
            if (UPLOAD_TO_GDRIVE)   upload(gdrive, filename);
            if (UPLOAD_TO_ONEDRIVE) upload(onedrive, filename);
        }
        uploadDialog->finish();
    }
}

void MainWindow::apkUnpacked(QString filename)
{
    loadingDialog->finish();
    setActiveApk(filename);

    int id = devices->currentItemIndex();
    if (id == -1) {
        id = 0;
    }
    devices->setCurrentItem(id);
    hideEmptyDpi();

    // Parse APK details:
    tabProps->setItem(0, 1, new QTableWidgetItem(apk->getApplicationLabel()));
    tabProps->setItem(1, 1, new QTableWidgetItem(apk->getPackageName()));
    tabProps->setItem(2, 1, new QTableWidgetItem(apk->getVersionCode()));
    tabProps->setItem(3, 1, new QTableWidgetItem(apk->getVersionName()));
    tabProps->setItem(4, 1, new QTableWidgetItem(apk->getMinimumSdk()));
    tabProps->setItem(5, 1, new QTableWidgetItem(apk->getTargetSdk()));

    // Enable operations with APK and icons:
    actApkSave->setEnabled(true);
    actApkExplore->setEnabled(true);
    actIconOpen->setEnabled(true);
    actIconSave->setEnabled(true);
    actIconRevert->setEnabled(true);
    actIconEffect->setEnabled(true);
    actIconResize->setEnabled(true);
    actIconScale->setEnabled(true);
    devices->setEnabled(true);
    tabProps->setEnabled(true);
    btnPack->setEnabled(true);
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
        Profile profile = Profile::at(devices->currentGroupIndex());
        int side = profile.getDpiSide(static_cast<Dpi>(devices->currentItemIndex()));
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

    if (filename.isEmpty())
    {
        Profile profile = Profile::at(devices->currentGroupIndex());
        int side = profile.getDpiSide(static_cast<Dpi>(devices->currentItemIndex()));
        filename = QString("%1-%2").arg(QFileInfo(currentApk).baseName()).arg(side);
        filename = QFileDialog::getSaveFileName(this, tr("Save Icon"), filename, FILTER_GFX);
        if (filename.isEmpty()) {
            return false;
        }
    }
    return drawArea->getIcon()->save(filename);
}

void MainWindow::iconScale()
{
    Profile profile = Profile::at(devices->currentGroupIndex());
    int side = profile.getDpiSide(static_cast<Dpi>(devices->currentItemIndex()));
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

void MainWindow::showEffectsDialog()
{
    Icon *icon = drawArea->getIcon();
    if (!icon->isNull()) {
        const int TEMP_ANGLE = icon->getAngle();
        const bool TEMP_FLIP_X = icon->getFlipX();
        const bool TEMP_FLIP_Y = icon->getFlipY();
        const bool TEMP_IS_COLOR = icon->getColorEnabled();
        const bool TEMP_IS_BLUR = icon->getBlurEnabled();
        const QColor TEMP_COLOR = icon->getColor();
        const qreal TEMP_DEPTH = icon->getDepth();
        const qreal TEMP_BLUR = icon->getBlur();
        effects->setRotation(TEMP_ANGLE);
        effects->setFlipX(TEMP_FLIP_X);
        effects->setFlipY(TEMP_FLIP_Y);
        effects->setColorEnabled(TEMP_IS_COLOR);
        effects->setColor(TEMP_COLOR);
        effects->setColorDepth(TEMP_DEPTH * 100);
        effects->setBlurEnabled(TEMP_IS_BLUR);
        effects->setBlur(TEMP_BLUR * 10);
        // TODO: Get rid of flicker on exec().
        if (effects->exec() == QDialog::Accepted) {
            setWindowModified(true);
        }
        else {
            icon->setAngle(TEMP_ANGLE);
            icon->setFlipX(TEMP_FLIP_X);
            icon->setFlipY(TEMP_FLIP_Y);
            icon->setColorEnabled(TEMP_IS_COLOR);
            icon->setBlurEnabled(TEMP_IS_BLUR);
            icon->setColor(TEMP_COLOR);
            icon->setDepth(TEMP_DEPTH);
            icon->setBlur(TEMP_BLUR);
        }
    }
    else {
        invalidDpi();
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
        warning(tr("File not found"), tr("Could not find APK:\n%1").arg(filename));
        recent.removeOne(filename);
        refreshRecent();
        return;
    }

    currentPath = QFileInfo(filename).absolutePath();

    // Unpacking:
    loadingDialog->setProgress(0);
    apk->unpack(filename);
}

void MainWindow::apkSave()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Pack APK"),
                                                    currentPath, "APK (*.apk)");
    if (!filename.isEmpty()) {
        currentPath = QFileInfo(filename).absolutePath();
        loadingDialog->setProgress(0);
        bool sign = actPackSign->isChecked();
        bool optimize = actPackOptimize->isChecked();
        short ratio = groupRatio->checkedAction()->data().toInt();
        apk->pack(filename, ratio, sign, optimize);
    }
}

void MainWindow::apkExplore()
{
    QDesktopServices::openUrl(QDir::fromNativeSeparators(TEMPDIR + "apk"));
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
    QDesktopServices::openUrl(URL_WEBSITE);
}

void MainWindow::browseBugs() const
{
    QDesktopServices::openUrl(QUrl(URL_BUGS));
}

void MainWindow::about()
{
    const QString APPDIR(QApplication::applicationDirPath());
    const QString LINK("<a href=\"%1\">%2</a> - %3");

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
            LINK.arg(QString("file:///%1/versions.txt").arg(APPDIR), tr("Version History"), tr("List of changes made to the project.")) +
        "</p>"
    );
    aboutBox.setIconPixmap(QPixmap(":/gfx/logo.png"));
    QPushButton btnAuthors(tr("Authors"));
    connect(&btnAuthors, SIGNAL(clicked()), this, SLOT(aboutAuthors()), Qt::QueuedConnection);
    aboutBox.addButton(QMessageBox::Ok);
    aboutBox.addButton(&btnAuthors, QMessageBox::RejectRole);
    aboutBox.exec();
}

void MainWindow::aboutAuthors()
{
    QMessageBox authors(this);
    authors.setWindowTitle(tr("Authors"));
    authors.setIconPixmap(QPixmap(":/gfx/logo.png"));

    QString strAuthors;
    bool newline = true;
    QFile inputFile(QApplication::applicationDirPath() + "/authors.txt");
    if (inputFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&inputFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line == "Special Thanks:") {
                strAuthors.chop(4); // Chop the last "<br>" tag.
                break;
            }
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

void MainWindow::authCloud()
{
    Cloud *cloud = dynamic_cast<Cloud*>(sender());
    QApplication::alert(this);
    QString code = InputDialog::getString(tr("Authorization"),
                                          tr("Allow access to %1 in your browser and paste the provided code here:").arg(cloud->getTitle()),
                                          cloud->getIcon(), this);
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
    QApplication::alert(this);
    QMessageBox::warning(this, title, text);
}

void MainWindow::error(QString title, QString text)
{
    QApplication::alert(this);
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

        // Handle Qt Unix bug:
        if (filename.right(2) == "\r\n") {
            filename.chop(2);
        }

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
    settings->setValue("Profile", devices->currentGroupText());
    settings->setValue("Language", currentLang);
    settings->setValue("Update", actAutoUpdate->isChecked());
    settings->setValue("Directory", currentPath);
    settings->setValue("Geometry", saveGeometry());
    settings->setValue("Splitter", splitter->saveState());
    settings->setValue("Recent", recent);

    settings->beginGroup("APK");
        settings->setValue("Compression", groupRatio->checkedAction()->data().toInt());
        settings->setValue("Sign", actPackSign->isChecked());
        settings->setValue("Optimize", actPackOptimize->isChecked());
        settings->setValue("Upload", checkUpload->isChecked());
    settings->endGroup();

    settings->beginGroup("Dropbox");
        settings->setValue("Token", dropbox->getToken());
        settings->setValue("Enable", checkDropbox->isChecked());
    settings->endGroup();

    settings->beginGroup("GDrive");
        settings->setValue("Token", gdrive->getToken());
        settings->setValue("Enable", checkGDrive->isChecked());
    settings->endGroup();

    settings->beginGroup("OneDrive");
        settings->setValue("Token", onedrive->getToken());
        settings->setValue("Enable", checkOneDrive->isChecked());
    settings->endGroup();

    // Close window:
    event->accept();
}

MainWindow::~MainWindow()
{
    qDebug() << "Exiting...";
    delete apk;
}
