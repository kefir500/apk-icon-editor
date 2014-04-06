/// \file dialogs.h
/// \brief This file describes additional dialogs - #InputDialog and #ProgressDialog.

#ifndef DIALOGS_H
#define DIALOGS_H

#include "main.h"
#include <QInputDialog>
#include <QLineEdit>
#include <QProgressBar>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>

#ifdef QT5_2_0
    #include <QtWinExtras/QtWinExtras>
#endif

/// \brief This dialog receives user input.
///
/// The InputDialog widget is an alternative for standard \c QInputDialog.
/// It paints additional icon and forbids empty input.

class InputDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel *label;                  ///< Dialog text.
    QLabel *icon;                   ///< Dialog pixmap.
    QLineEdit *input;               ///< Dialog input box.
    QDialogButtonBox *buttons;      ///< Dialog buttons.

public:
    /// Static function returning user string input.
    /// \param[in] title Dialog window title.
    /// \param[in] text Dialog text.
    /// \param[in] _icon Dialog pixmap.
    /// \param[in] parent Parent widget.
    static QString getString(QString title, QString text, QPixmap _icon, QWidget *parent);

    explicit InputDialog(QString title, QString text, QPixmap _icon, QWidget *parent = 0);

private slots:
    /// Paste text from clipboard to #input box.
    void paste();

    /// Check text entered by user.
    /// \param text If is empty, "OK" button is replaced by "Paste" button.
    void checkInput(QString text);

};

class ProgressDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel *label;                  ///< Dialog progress text.
    QLabel *icon;                   ///< Dialog pixmap.
    QProgressBar *progress;         ///< Dialog progress bar.
    QDialogButtonBox *buttons;      ///< Dialog buttons.
#ifdef QT5_2_0
    QWinTaskbarButton *taskbar;     ///< Represents taskbar button.
#endif

public:
    /// Set current progress \c text.
    void setText(QString text) { label->setText(text); }

    /// Set current progress \c pixmap.
    void setIcon(QPixmap pixmap);

    /// If #allow is \c FALSE, forbid cancelling dialog by user.
    void setAllowCancel(bool allow);

    explicit ProgressDialog(QWidget *parent = 0);

private slots:
    /// Closes dialog, resets current progress and emits \c rejected signal.
    void cancel();

public slots:
    /// Show Progress dialog with the given parameters.
    /// \param[in] percentage Loading progress (0-100).
    /// \param[in] text Additional loading text.
    void setProgress(short percentage, QString text = NULL);

    /// Close and reset Progress dialog.
    void finish();

};

#endif // DIALOGS_H
