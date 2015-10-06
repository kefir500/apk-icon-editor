#include "dialogs.h"
#include "main.h"
#include <QBoxLayout>
#include <QToolButton>
#include <QClipboard>
#include <QTextStream>
#include <QApplication>

#define PAYPAL "kefir500@gmail.com"
#define BITCOIN "1M299bkCjSQL1TDbTzD38a7YyN96NvSo2k"

// Input Dialog

InputDialog::InputDialog(QString title, QString text, bool password, QPixmap _icon, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *layout = new QVBoxLayout(this);

    label = new QLabel(this);
    icon = new QLabel(this);
    icon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    input = new QLineEdit(this);
    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    label->setText(text);
    icon->setPixmap(_icon);
    input->setEchoMode(password ? QLineEdit::Password : QLineEdit::Normal);

    QHBoxLayout *row1 = new QHBoxLayout();
    row1->addWidget(icon);
    row1->addWidget(label);

    layout->addLayout(row1);
    layout->addWidget(input);
    layout->addWidget(buttons);

    checkInput("");

    connect(input, SIGNAL(textChanged(QString)), this, SLOT(checkInput(QString)));
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

QString InputDialog::getString(QString title, QString text, bool password, QPixmap _icon, QWidget *parent)
{
    InputDialog dialog(title, text, password, _icon, parent);
    if (dialog.exec() == QDialog::Accepted) {
        return dialog.input->text();
    }
    else {
        return NULL;
    }
}

void InputDialog::paste()
{
    input->paste();
}

void InputDialog::checkInput(QString text)
{
    buttons->clear();
    if (!text.isEmpty()) {
        buttons->addButton(QDialogButtonBox::Ok)->setDefault(true);
    }
    else {
        QPushButton *btnPaste = new QPushButton(tr("Paste"), this);
        buttons->addButton(btnPaste, QDialogButtonBox::ActionRole);
        btnPaste->setDefault(true);
        connect(btnPaste, SIGNAL(clicked()), this, SLOT(paste()));
    }
    buttons->addButton(QDialogButtonBox::Cancel);
}

// Progress Dialog

ProgressDialog::ProgressDialog(QWidget *parent) : QDialog(parent)
{
    resize(220, 100);
    setFixedHeight(100);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *layout = new QVBoxLayout(this);

    label = new QLabel(this);
    icon = new QLabel(this);
    progress = new QProgressBar(this);
    buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);

    icon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    progress->setRange(0, 100);

    QHBoxLayout *row1 = new QHBoxLayout();
    row1->addWidget(icon);
    row1->addWidget(label);

    layout->addLayout(row1);
    layout->addWidget(progress);
    layout->addWidget(buttons);

#ifdef WINEXTRAS
    isWinExtras = QSysInfo::windowsVersion() >= QSysInfo::WV_VISTA;
    if (isWinExtras) {
        taskbar = new QWinTaskbarButton(this);
    }
#endif

    connect(buttons, SIGNAL(rejected()), this, SLOT(cancel()));
}

void ProgressDialog::setProgress(short percentage, QString text)
{
    label->setText(text);
    progress->setValue(percentage);

#ifdef WINEXTRAS
    if (isWinExtras) {
        taskbar->setWindow(static_cast<QWidget*>(parent())->windowHandle());
        taskbar->setOverlayIcon(*icon->pixmap());
        QWinTaskbarProgress *taskProgress = taskbar->progress();
        taskProgress->setValue(percentage);
        taskProgress->setVisible(true);
    }
#endif

    show();
}

void ProgressDialog::setIcon(QPixmap pixmap)
{
    pixmap = pixmap.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    icon->setPixmap(pixmap);
}

void ProgressDialog::setAllowCancel(bool allow)
{
    if (allow) {
        setWindowFlags(windowFlags() & Qt::WindowCloseButtonHint);
    }
    else {
        setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    }
    buttons->button(QDialogButtonBox::Cancel)->setEnabled(allow);
}

void ProgressDialog::finish()
{
    accept();
    progress->setValue(0);
#ifdef WINEXTRAS
    if (isWinExtras) {
        taskbar->clearOverlayIcon();
        QWinTaskbarProgress *taskProgress = taskbar->progress();
        taskProgress->setVisible(false);
        taskProgress->reset();
    }
#endif
}

void ProgressDialog::cancel()
{
    finish();
    emit rejected();
}

// Donate Dialog

Donate::Donate(QWidget *parent) : QDialog(parent)
{
    resize(400, 0);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Initialize "Thanks" section:

    QString thanks = QString("<hr>%1:<br>").arg(tr("Thank you for your support"));

    QRegExp rx("\\((.+)\\)");
    rx.setMinimal(true);

    QFile inputFile(APPDIR + "/authors.txt");
    if (inputFile.open(QIODevice::ReadOnly)) {

        QTextStream stream(&inputFile);

        while (!stream.atEnd()) {

            QString line = stream.readLine();

            if (line == "Supported by:") {
                while (!line.isEmpty() && !stream.atEnd()) {
                    line = stream.readLine();
                    line = line.replace(rx, "(<a href=\"\\1\">\\1</a>)");
                    thanks += line + "<br>";
                }
                thanks.chop(8);
                break;
            }
        }

        inputFile.close();
    }

    // Create GUI:

    QGridLayout *grid = new QGridLayout(this);
    QLabel *donators = new QLabel(this);
    donators->setAlignment(Qt::AlignCenter);
    donators->setTextFormat(Qt::RichText);
    donators->setOpenExternalLinks(true);
    donators->setText(thanks);

    QLabel *titlePayPal = new QLabel("PayPal", this);
    QLabel *titleBitCoin = new QLabel("BitCoin", this);
    QLineEdit *fieldPayPal = new QLineEdit(PAYPAL, this);
    QLineEdit *fieldBitCoin = new QLineEdit(BITCOIN, this);
    fieldPayPal->setReadOnly(true);
    fieldBitCoin->setReadOnly(true);
    fieldPayPal->setCursor(Qt::IBeamCursor);
    fieldBitCoin->setCursor(Qt::IBeamCursor);

    QToolButton *btnPayPal = new QToolButton(this);
    QToolButton *btnBitCoin = new QToolButton(this);
    btnPayPal->setIcon(QIcon(":/gfx/actions/copy.png"));
    btnBitCoin->setIcon(QIcon(":/gfx/actions/copy.png"));
    btnPayPal->setToolTip(tr("Copy to Clipboard"));
    btnBitCoin->setToolTip(tr("Copy to Clipboard"));

    QPushButton *btnOk = new QPushButton("OK", this);

    grid->addWidget(titlePayPal, 0, 0);
    grid->addWidget(fieldPayPal, 0, 1);
    grid->addWidget(btnPayPal, 0, 2);
    grid->addWidget(titleBitCoin, 1, 0);
    grid->addWidget(fieldBitCoin, 1, 1);
    grid->addWidget(btnBitCoin, 1, 2);
    grid->addWidget(btnOk, 2, 1);
    grid->addWidget(donators, 3, 1);

    connect(btnPayPal, SIGNAL(clicked()), this, SLOT(copyPayPal()));
    connect(btnBitCoin, SIGNAL(clicked()), this, SLOT(copyBitCoin()));
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
}

void Donate::copyPayPal() { QApplication::clipboard()->setText(PAYPAL); }
void Donate::copyBitCoin() { QApplication::clipboard()->setText(BITCOIN); }
