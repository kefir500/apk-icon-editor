/// \file mainwindow.h
/// \brief This file contains #MainWindow class declaration.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTableWidget>
#include <QSignalMapper>
#include <QTranslator>
#include <QCloseEvent>
#include "apk.h"
#include "combolist.h"
#include "drawarea.h"
#include "effectsdialog.h"
#include "tooldialog.h"
#include "keymanager.h"
#include "dialogs.h"
#include "about.h"
#include "updater.h"
#include "recent.h"
#include "cloud.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    QSplitter *splitter;
    DrawArea *drawArea;
    QTabWidget *tabs;
    ComboList *devices;
    QPushButton *btnApplyIcons;
    QLabel *labelTool;
    QToolButton *btnTool;
    QLabel *labelAppName;
    QLabel *labelVersionName;
    QLabel *labelVersionCode;
    QLineEdit *editAppName;
    QLineEdit *editVersionName;
    QSpinBox *editVersionCode;
    QPushButton *btnApplyAppName;
    QWidget *panelApktool;
    QLabel *labelApktool;
    QPushButton *btnRepacking;
    QTableWidget *tableStrings;
    QCheckBox *checkDropbox;
    QCheckBox *checkGDrive;
    QCheckBox *checkOneDrive;
    QCheckBox *checkUpload;
    QPushButton *btnPack;
    EffectsDialog *effects;
    ToolDialog *toolDialog;
    KeyManager *keyManager;
    ProgressDialog *loadingDialog;
    ProgressDialog *uploadDialog;
    About *about;
    QSignalMapper *mapLang;
    QSignalMapper *mapRecent;
    QTranslator *translator;
    QTranslator *translatorQt;

    // Menus:

    QMenu *menuFile;
    QMenu *menuIcon;
    QMenu *menuSett;
    QMenu *menuHelp;
    QMenu *menuRecent;
    QMenu *menuLang;
    QMenu *menuLogs;

    // Actions:

    QAction *actApkOpen;
    QAction *actApkExplore;
    QAction *actApkSave;
    QAction *actExit;
    QAction *actRecentClear;
    QAction *actNoRecent;
    QAction *actIconOpen;
    QAction *actIconSave;
    QAction *actIconScale;
    QAction *actIconResize;
    QAction *actIconRevert;
    QAction *actIconBack;
    QAction *actIconEffect;
    QAction *actPacking;
    QAction *actKeys;
    QAction *actTranslate;
    QAction *actAssoc;
    QAction *actReset;
    QAction *actAutoUpdate;
    QAction *actFaq;
    QAction *actWebsite;
    QAction *actReport;
    QAction *actDonate;
    QAction *actLogFile;
    QAction *actLogPath;
    QAction *actUpdate;
    QAction *actAboutQt;
    QAction *actAbout;

    Apk *apk;
    Recent *recent;
    Updater *updater;
    Dropbox *dropbox;
    GoogleDrive *gdrive;
    OneDrive *onedrive;
    QString currentApk;
    QString currentLang;
    QString currentPath;

    QString version_apktool;                ///< Holds Apktool version.
    QString version_jre;                    ///< Holds JRE version.
    QString version_jdk;                    ///< Holds JDK version.

    void init_core();                       ///< Initialize core objects.
    void init_gui();                        ///< Initialize dialogs and widgets.
    void init_languages();                  ///< Initialize available languages.
    void init_devices();                    ///< Initialize available device presets.
    void init_slots();                      ///< Initialize signals/slots.

    void checkDeps();                       ///< Check Java and Apktool versions.
    void setActiveApk(QString filename);    ///< Mark \c filename as currently active.
    void resetApktool();                    ///< Remove Apktool "1.apk" framework
    bool confirmExit();                     ///< Ask user to confim exit.
    void invalidDpi();                      ///< Show "Invalid DPI" message.

    void upload(Cloud *uploader, QString filename); ///< Upload file to a cloud service.

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *event);

public slots:
    bool apk_open(QString filename = ""); ///< Open APK (calls "Open" dialog if parameter is empty).
    bool apk_save(QString filename = ""); ///< Save APK (calls "Save" dialog if parameter is empty).
    void apk_explore();                   ///< Open the folder with APK contents.

private slots:
    /// Handle packed APK.
    /// \param[in] filename  Name of the packed APK file.
    /// \param[in] isSuccess \c FALSE if APK is packed with warnings.
    /// \param[in] text      Additional message text.
    void apkPacked(QString filename, bool isSuccess = true, QString text = QString());

    void apkUnpacked(QString filename);             ///< Handle APK unpacked from the \c filename.

    void settings_load();                           ///< Load settings from INI.
    void settings_reset();                          ///< Reset settings to default.

//------------------------------------------------------------------------------

    /// Load icon from the file with the given \c filename.
    /// \param[in] filename Name of the file to load image from. If \c NULL, shows "Open" dialog.
    bool iconOpen(QString filename = QString());

    /// Save icon to the file with the given \c filename.
    /// \param[in] filename Name of the file to save image to. If \c NULL, shows "Save" dialog.
    bool iconSave(QString filename = QString());

    /// Deprecated.
    void iconScale();

    /// Resize icon to square to the given \c side.
    /// \param[in] side New side value in pixels. If zero, asks user for a value.
    bool iconResize(int side = 0);

    /// Revert the original APK icon.
    bool iconRevert();

//------------------------------------------------------------------------------

    void associate() const;            ///< Associate .apk extension with "APK Icon Editor"
    void browseSite() const;           ///< Open website URL in the default browser.
    void browseBugs() const;           ///< Open bugs webpage in the default browser.
    void browseFaq() const;            ///< Open FAQ text document.
    void openLogFile() const;          ///< Open log file.
    void openLogPath() const;          ///< Open log directory.
    void donate();                     ///< Show donation dialog.

    void setLanguage(QString lang);    ///< Set GUI language.
    void setCurrentIcon(int id);       ///< Set icon to draw in \c drawArea widget.
    void setPreviewColor();            ///< Show dialog to select background preview color.
    void showEffectsDialog();          ///< Show "Effects" dialog.
    void hideEmptyDpi();               ///< Hide unused (empty) icons from \c profiles widget.
    void cloneIcons();                 ///< Apply current icon to all DPI sizes.
    void enableApktool(bool value);    ///< Enable or disable application name/version editing.
    void stringChanged(int, int);      ///< Handle translation cell changes.
    void applyAppName();               ///< Apply global application name to all translations.
    void askReloadApk();               ///< Ask to repack APK if the tool ("7za" or "apktool") was changed.
    void enableUpload(bool enable);    ///< Enable or disable upload to cloud services.

    void recent_add(QString filename); ///< Add \c filename to recent list.
    void recent_update();              ///< Refresh recent menu.
    void recent_clear();               ///< Clear list of recently opened APKs.

//------------------------------------------------------------------------------

    /// Show "New version available" message.
    /// \param[in] version Number representing the new version.
    bool newVersion(QString version);

    /// Show "Enter \c cloud code" message.
    void authCloud();

    /// Open Transifex URL in the default browser.
    void browseTranslate();

    /// Calls QDialog::setWindowModified(true).
    void setModified();

    /// Show success message.
    /// \param[in] title Message brief title.
    /// \param[in] text  Message detailed text.
    void success(QString title, QString text);

    /// Show warning message.
    /// \param[in] title Message brief title.
    /// \param[in] text  Message detailed text.
    void warning(QString title, QString text);

    /// Show error message.
    /// \param[in] title Message brief title.
    /// \param[in] text  Message detailed text.
    void error(QString title, QString text);

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
