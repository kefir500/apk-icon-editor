#ifndef FILEBOX_H
#define FILEBOX_H

#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

class LineEditDrop : public QLineEdit {
    Q_OBJECT

public:
    explicit LineEditDrop(QWidget *parent = 0) : QLineEdit(parent) { }

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);

signals:
    void fileDropped(QString filename);
};

class FileBox : public QWidget {
    Q_OBJECT

public:
    explicit FileBox(QWidget *parent = 0);

    QString value() const { return edit->text(); }

    void setValue(QString value);
    void setTitle(QString title);
    void setTitleWidth(int width);
    void setFormats(QString _formats);

private:
    QLabel *label;
    LineEditDrop *edit;

    QString formats;

signals:
    void opened(QString);

private slots:
    void openFile(QString filename = NULL);
    void isFileExist(QString filename);

};

#endif // FILEBOX_H
