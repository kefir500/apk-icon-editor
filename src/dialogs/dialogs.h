///
/// \file
/// This file describes additional input dialogs.
///

#ifndef DIALOGS_H
#define DIALOGS_H

#include <QInputDialog>
#include <QLineEdit>
#include <QProgressBar>
#include <QLabel>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QToolButton>

#if defined(Q_OS_WIN) && (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
    #define WINEXTRAS
#endif

#ifdef WINEXTRAS
    #include <QtWinExtras/QtWinExtras>
#endif

///
/// This namespace incorporates the modal calls for input dialogs.
///

namespace Dialogs {
    /// Displays the modal string input dialog.
    /// \param text   Dialog informative text.
    /// \param title  Dialog window title.
    /// \param icon   Dialog icon.
    /// \param parent Dialog parent widget.
    /// \return String entered by user.
    QString getString(QString text,
                      QString title = QString(),
                      QPixmap icon = QPixmap(),
                      QWidget *parent = 0);

    /// Displays the modal password input dialog.
    /// \param text   Dialog informative text.
    /// \param title  Dialog window title.
    /// \param icon   Dialog icon.
    /// \param parent Dialog parent widget.
    /// \return String entered by user.
    QString getPassword(QString text,
                        QString title = QString(),
                        QPixmap icon = QPixmap(),
                        QWidget *parent = 0);

    /// Displays the modal size input dialog.
    /// \param title  Dialog window title.
    /// \param width  Initial width value.
    /// \param height Initial height value.
    /// \param parent Dialog parent widget.
    /// \return Size entered by user.
    QSize getSize(QString title = QString(),
                  int width = 1,
                  int height = 1,
                  QWidget *parent = 0);
}

///
/// String input dialog.
/// This class provides an extended input dialog to get the string entered by user.
///

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    /// Constructs a string input dialog.
    /// \param text     Dialog informative text.
    /// \param title    Dialog window title.
    /// \param password If \c true, the input is masked.
    /// \param icon     Dialog icon.
    /// \param parent   Dialog parent widget.
    InputDialog(QString text,
                QString title = QString(),
                bool password = false,
                QPixmap icon = QPixmap(),
                QWidget *parent = 0);

    /// Returns the string entered by user.
    QString getString() const { return input->text(); }

private slots:
    /// Inserts text from the clipboard to the input box.
    void paste();

    /// Checks the \c text entered by user.
    /// If \c text is empty, "OK" button is replaced by "Paste" button.
    void checkInput(QString text);

private:
    QLabel *label;             ///< Dialog text.
    QLabel *icon;              ///< Dialog pixmap.
    QLineEdit *input;          ///< Dialog input box.
    QDialogButtonBox *buttons; ///< Dialog buttons.
};

///
/// Size input dialog.
/// This class provides an input dialog to get the size entered by user.
///

class ResizeDialog : public QDialog
{
    Q_OBJECT

public:
    /// Constructs a size input dialog.
    /// \param title  Dialog window title.
    /// \param width  Initial width value.
    /// \param height Initial height value
    /// \param parent Dialog parent widget.
    ResizeDialog(QString title = QString(),
                 int width = 1,
                 int height = 1,
                 QWidget *parent = 0);

    int getWidth() const { return wInput->value(); }  ///< Returns the current width input value.
    int getHeight() const { return hInput->value(); } ///< Returns the current height input value.


public slots:
    void setWidth(int width) { wInput->setValue(width); }    ///< Sets the width input value to \c width.
    void setHeight(int height) { hInput->setValue(height); } ///< Sets the height input value to \c height.

private slots:
    void widthChanged(int width) { if (btnLock->isChecked()) hInput->setValue(width); }
    void heightChanged(int height) { if (btnLock->isChecked()) wInput->setValue(height); }

private:
    QSpinBox *wInput;     ///< Width input box.
    QSpinBox *hInput;     ///< Height input box.
    QToolButton *btnLock; ///< Lock button.
};

///
/// Extended progress dialog.
///

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    /// Constructs a progress dialog with the specified \c parent.
    explicit ProgressDialog(QWidget *parent = 0);

    void setText(QString text);      ///< Sets the progress text.
    void setIcon(QPixmap pixmap);    ///< Sets the progress icon.
    void setAllowCancel(bool allow); ///< If \c allow is \c false, the dialog cannot be rejected.

public slots:
    virtual void accept(); ///< Accepts the progress dialog.
    virtual void reject(); ///< Rejects the progress dialog. Does nothing if #allowCancel is \c true.
    void reset();          ///< Resets the progress dialog percentage.

    /// Displays the progress dialog with the given parameters.
    /// \param percentage Loading progress (0-100).
    /// \param text Additional loading text.
    void setProgress(short percentage, QString text = QString());

private:
    QLabel *label;              ///< Dialog progress text.
    QLabel *icon;               ///< Dialog icon.
    QProgressBar *progress;     ///< Dialog progress bar.
    QDialogButtonBox *buttons;  ///< Dialog buttons.
    bool isWinExtras;           ///< If \c true, allows using QtWinExtras.
    bool allowCancel;           ///< If \c false, the dialog cannot be rejected.
#ifdef WINEXTRAS
    QWinTaskbarButton *taskbar; ///< Represents the taskbar button.
#endif
};

#endif // DIALOGS_H
