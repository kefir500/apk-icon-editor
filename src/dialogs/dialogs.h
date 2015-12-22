/// \file dialogs.h
/// \brief This file describes additional dialogs - #InputDialog and #ProgressDialog.

#ifndef DIALOGS_H
#define DIALOGS_H

#include <QInputDialog>
#include <QLineEdit>
#include <QProgressBar>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>

#if defined(Q_OS_WIN) && (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
    #define WINEXTRAS
#endif

#ifdef WINEXTRAS
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
    /// \param[in] password Password mode.
    /// \param[in] _icon Dialog pixmap.
    /// \param[in] parent Parent widget.
    static QString getString(QString title, QString text, bool password, QPixmap _icon, QWidget *parent);

    explicit InputDialog(QString title, QString text, bool password, QPixmap _icon, QWidget *parent = 0);

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
    bool isWinExtras;               ///< If \c TRUE, allows using QtWinExtras.
    bool allowCancel;               ///< If \c FALSE, dialog cannot be rejected.
#ifdef WINEXTRAS
    QWinTaskbarButton *taskbar;     ///< Represents taskbar button.
#endif

public:
    explicit ProgressDialog(QWidget *parent = 0);

    void setText(QString text);      ///< Set current progress #text.
    void setIcon(QPixmap pixmap);    ///< Set current progress #pixmap.
    void setAllowCancel(bool allow); ///< If #allow is \c FALSE, forbid cancelling dialog by user.

public slots:
    virtual void accept(); ///< Accept progress dialog.
    virtual void reject(); ///< Reject progress dialog. Does nothing if #allowCancel is \c TRUE).
    void reset();          ///< Reset progress dialog percentage.

    /// Show progress dialog with the given parameters.
    /// \param[in] percentage Loading progress (0-100).
    /// \param[in] text Additional loading text.
    void setProgress(short percentage, QString text = QString());

};

class Donate : public QDialog
{
    Q_OBJECT

public:
    explicit Donate(QWidget *parent = 0);
};

class Wallets : public QWidget
{
    Q_OBJECT

public:
    Wallets(QWidget *parent = 0);
    void add(QString title, QString wallet, QString link = QString());

private slots:
    void copy() const; ///< Copy wallet ID to clipboard.
    void open() const; ///< Open wallet link in browser.

private:
    unsigned int wallets;
};

#endif // DIALOGS_H
