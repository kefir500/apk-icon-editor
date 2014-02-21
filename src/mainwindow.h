/// \file mainwindow.h
/// \brief This file contains #MainWindow class declaration.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QCheckBox>
#include <QProgressDialog>
#include <QSignalMapper>
#include <QActionGroup>
#include <QTranslator>
#include <QSettings>
#include <QCloseEvent>
#include "apk.h"
#include "combolist.h"
#include "drawarea.h"
#include "effects.h"
#include "dialogs.h"
#include "updater.h"
#include "dropbox.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    DrawArea *drawArea;
    ComboList *devices;
    QPushButton *btnPack;
    QCheckBox *checkDropbox;
    EffectsDialog *effects;
    ProgressDialog *loadingDialog;
    ProgressDialog *uploadDialog;
    QSignalMapper *mapLang;
    QSignalMapper *mapRecent;
    QTranslator *translator;
    QTranslator *translatorQt;
    QSettings *settings;

    // Menus:
    QMenu *menuFile;
    QMenu *menuIcon;
    QMenu *menuSett;
    QMenu *menuHelp;

    // Sub-menus, sub-actions:
    QMenu *menuRecent;
    QMenu *menuLang;
    QMenu *menuPack;
    QMenu *menuRatio;
    QActionGroup *groupRatio;
    QAction *actRatio0;
    QAction *actRatio1;
    QAction *actRatio3;
    QAction *actRatio5;
    QAction *actRatio7;
    QAction *actRatio9;
    QAction *actPackSign;
    QAction *actPackOptimize;

    // Actions:
    QAction *actApkOpen;
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
    QAction *actAssoc;
    QAction *actReset;
    QAction *actAutoUpdate;
    QAction *actWebsite;
    QAction *actReport;
    QAction *actUpdate;
    QAction *actAboutQt;
    QAction *actAbout;

    Apk *apk;
    Updater *updater;
    Dropbox *dropbox;
    QString currentLang;
    QString currentPath;
    QStringList recent;

    void setActiveApk(QString filename);    ///< Mark \c filename as currently active.
    void addToRecent(QString filename);     ///< Add \c filename to recent list.
    void refreshRecent();                   ///< Refresh recent list.
    void restoreSettings();                 ///< Restore settings from INI.
    bool confirmExit();                     ///< Ask user to confim exit.
    void invalidDpi();                      ///< Show "Invalid DPI" message.
    void connectRepaintSignals();           ///< Create signal-slot connections to repaint windows.

    void initLanguages();                   ///< Initialize available languages.
    void initProfiles();                    ///< Initialize available profiles.

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:

    void apkLoad(QString filename = NULL);  ///< Open APK from the \c filename. If \c filename is NULL, show "Open" dialog.
    void apkSave();                         ///< Save APK (calls "Save" dialog).

    /// Handle packed APK.
    /// \param[in] filename  Name of the packed APK file.
    /// \param[in] isSuccess \c FALSE if APK is packed with warnings.
    /// \param[in] text      Additional message text.
    void apkPacked(QString filename, bool isSuccess = true, QString text = false);

    void apkUnpacked(QString filename);             ///< Handle APK unpacked from the \c filename.
    void resetSettings();                           ///< Reset settings to default.

//------------------------------------------------------------------------------

    /// Load icon from the file with the given \c filename.
    /// \param[in] filename Name of the file to load image from. If \c NULL, shows "Open" dialog.
    bool iconOpen(QString filename = NULL);

    /// Save icon to the file with the given \c filename.
    /// \param[in] filename Name of the file to save image to. If \c NULL, shows "Save" dialog.
    bool iconSave(QString filename = NULL);

    /// Deprecated.
    void iconScale();

    /// Resize icon to square to the given \c side.
    /// \param[in] side New side value in pixels. If zero, asks user for a value.
    bool iconResize(int side = 0);

    /// Revert the original APK icon.
    bool iconRevert();

//------------------------------------------------------------------------------

    void associate() const;     ///< Associate .apk extension with "APK Icon Editor"
    void browseSite() const;    ///< Open website URL in the default browser.
    void browseBugs() const;    ///< Open bugs webpage in the default browser.
    void about();               ///< Show "About APK Icon Editor" dialog.
    void aboutQt() const;       ///< Show "About Qt" dialog.

    void setLanguage(QString lang); ///< Set GUI language.
    void setCurrentIcon(int id);    ///< Set icon to draw in \c drawArea widget.
    void setPreviewColor();         ///< Show dialog to select background preview color.
    void showEffectsDialog();       ///< Show "Effects" dialog.
    void hideEmptyDpi();            ///< Hide unused (empty) icons from \c profiles widget.
    void clearRecent();             ///< Clear list of recently opened APKs.

//------------------------------------------------------------------------------

    /// Show "New version available" message.
    /// \param[in] version Number representing the new version.
    bool newVersion(QString version);

    void authDropbox();
    void uploaded(bool isSuccess = true);

    QString input(QString title, QString text);

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
    MainWindow(int argc, char *argv[], QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
