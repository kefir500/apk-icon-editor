#include "filebox.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDropEvent>
#include <QMimeData>

FileBox::FileBox(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    label = new QLabel(this);
    edit = new LineEditDrop(this);
    QToolButton *button = new QToolButton(this);
    button->setText("...");

    edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored);

    layout->addWidget(label);
    layout->addWidget(edit);
    layout->addWidget(button);

    connect(button, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(edit, SIGNAL(fileDropped(QString)), this, SLOT(openFile(QString)));
    connect(edit, SIGNAL(textChanged(QString)), this, SLOT(isFileExist(QString)));
}

void FileBox::setValue(QString value)
{
    edit->setText(value);
}

void FileBox::setTitle(QString title)
{
    label->setText(title);
}

void FileBox::setTitleWidth(int width)
{
    label->setMinimumWidth(width);
}

void FileBox::setFormats(QString _formats)
{
    formats = _formats;
}

void FileBox::openFile(QString filename)
{
    if (filename.isNull()) {
        filename = QFileDialog::getOpenFileName(this, NULL, NULL, formats + tr("All Files"));
    }
    if (!filename.isNull()) {
        edit->setText(filename);
        emit opened(filename);
    }
}

void FileBox::isFileExist(QString filename)
{
    if (QFile::exists(filename) || filename.isEmpty()) {
        label->setStyleSheet("color: #000");
    }
    else {
        label->setStyleSheet("color: #d00");
    }
}

void LineEditDrop::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void LineEditDrop::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasText()) {
        QUrl url(mimeData->text());
        QString filename = url.toLocalFile();

        // Handle Qt Unix bug:
        if (filename.right(2) == "\r\n") {
            filename.chop(2);
        }

        emit fileDropped(filename);
    }
}
