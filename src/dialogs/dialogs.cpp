#include "dialogs.h"
#include "main.h"
#include <QBoxLayout>
#include <QToolButton>
#include <QClipboard>
#include <QTextStream>
#include <QApplication>

// Input Dialog

InputDialog::InputDialog(QString title, QString text, bool password, QPixmap _icon, QWidget *parent) : QDialog(parent)
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
        return QString();
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
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    allowCancel = true;

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

    setFixedHeight((sizeHint().height() < 100) ? 100 : sizeHint().height());

#ifdef WINEXTRAS
    isWinExtras = QSysInfo::windowsVersion() >= QSysInfo::WV_VISTA;
    if (isWinExtras) {
        taskbar = new QWinTaskbarButton(this);
    }
#endif

    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void ProgressDialog::setProgress(short percentage, QString text)
{
    label->setText(text);
    progress->setValue(percentage);
#ifdef WINEXTRAS
    if (isWinExtras) {
        taskbar->setWindow(static_cast<QWidget*>(parent())->windowHandle());
        if (icon->pixmap()) { taskbar->setOverlayIcon(*icon->pixmap()); }
        QWinTaskbarProgress *taskProgress = taskbar->progress();
        taskProgress->setValue(percentage);
        taskProgress->setVisible(true);
    }
#endif
    show();
}

void ProgressDialog::setText(QString text)
{
    label->setText(text);
}

void ProgressDialog::setIcon(QPixmap pixmap)
{
    pixmap = pixmap.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    icon->setPixmap(pixmap);
}

void ProgressDialog::setAllowCancel(bool allow)
{
    allowCancel = allow;
    setWindowFlags(windowFlags() & (allow ? Qt::WindowCloseButtonHint : ~Qt::WindowCloseButtonHint));
    buttons->button(QDialogButtonBox::Cancel)->setEnabled(allow);
}

void ProgressDialog::accept()
{
    QDialog::accept();
    reset();
}

void ProgressDialog::reject()
{
    if (allowCancel) {
        QDialog::reject();
        reset();
    }
}

void ProgressDialog::reset()
{
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

// Donate Dialog

Donate::Donate(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(QApplication::translate("MainWindow", "Donate"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(400, 0);

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

    QVBoxLayout *layout = new QVBoxLayout(this);

    Wallets *wallets = new Wallets(this);
    wallets->add("PayPal:", "kefir500@gmail.com", URL_WEBSITE + "donate/#paypal");
    wallets->add("BitCoin:", "1M299bkCjSQL1TDbTzD38a7YyN96NvSo2k", URL_WEBSITE + "donate/#bitcoin");
    wallets->add("Yandex.Money:", "410011762016796", "https://money.yandex.ru/to/410011762016796");

    QLabel *donators = new QLabel(this);
    donators->setAlignment(Qt::AlignCenter);
    donators->setTextFormat(Qt::RichText);
    donators->setOpenExternalLinks(true);
    donators->setText(thanks);

    QPushButton *btnOk = new QPushButton("OK", this);

    layout->addWidget(wallets);
    layout->addWidget(btnOk);
    layout->addWidget(donators);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
}

Wallets::Wallets(QWidget *parent) : QWidget(parent)
{
    new QGridLayout(this);
    wallets = 0;
}

void Wallets::add(QString title, QString wallet, QString link)
{
    QLabel *label_title = new QLabel(title, this);
    label_title->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label_title->setMinimumWidth(80);

    QLineEdit *input_wallet = new QLineEdit(wallet, this);
    input_wallet->setReadOnly(true);
    input_wallet->setCursor(Qt::IBeamCursor);

    QToolButton *btn_copy = new QToolButton(this);
    btn_copy->setProperty("wallet", wallet);
    btn_copy->setIcon(QIcon(":/gfx/actions/copy.png"));
    btn_copy->setToolTip(QApplication::translate("Donate", "Copy to Clipboard"));

    QToolButton *btn_link = new QToolButton(this);
    btn_link->setProperty("link", link);
    btn_link->setIcon(QIcon(":/gfx/actions/coins.png"));
    btn_link->setToolTip(QApplication::translate("MainWindow", "Donate"));
    btn_link->setEnabled(!link.isEmpty());

    QGridLayout *grid = qobject_cast<QGridLayout*>(layout());
    grid->addWidget(label_title,  wallets, 0, Qt::AlignRight);
    grid->addWidget(input_wallet, wallets, 1);
    grid->addWidget(btn_copy,     wallets, 2);
    grid->addWidget(btn_link,     wallets, 3);

    ++wallets;

    connect(btn_copy, SIGNAL(clicked()), this, SLOT(copy()));
    connect(btn_link, SIGNAL(clicked()), this, SLOT(open()));
}

void Wallets::copy() const
{
    QString wallet = sender()->property("id").toString();
    QApplication::clipboard()->setText(wallet);
}

void Wallets::open() const
{
    QString link = sender()->property("wallet").toString();
    QDesktopServices::openUrl(link);
}
