#include "dialogs.h"
#include <QBoxLayout>

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
    taskbar = new QWinTaskbarButton(this);
#endif

    connect(buttons, SIGNAL(rejected()), this, SLOT(cancel()));
}

void ProgressDialog::setProgress(short percentage, QString text)
{
    label->setText(text);
    progress->setValue(percentage);

#ifdef WINEXTRAS
    taskbar->setWindow(static_cast<QWidget*>(parent())->windowHandle());
    taskbar->setOverlayIcon(*icon->pixmap());
    QWinTaskbarProgress *taskProgress = taskbar->progress();
    taskProgress->setValue(percentage);
    taskProgress->setVisible(true);
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
    taskbar->clearOverlayIcon();
    QWinTaskbarProgress *taskProgress = taskbar->progress();
    taskProgress->setVisible(false);
    taskProgress->reset();
#endif
}

void ProgressDialog::cancel()
{
    finish();
    emit rejected();
}
