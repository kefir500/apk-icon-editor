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
    explicit FileBox(QWidget *parent = 0, bool isDirectory = false);

    QString value() const { return edit->text(); }

private:
    QLabel *label;
    LineEditDrop *edit;

    bool isDir;
    QString formats;

signals:
    void opened(QString);

public slots:
    void setValue(QString value);
    void setTitle(QString title);
    void setTitleWidth(int width);
    void setFormats(QString formats);

private slots:
    void openFile(QString filename = QString());
    void openDir(QString dir = QString());
    void exists(QString path);

};

#endif // FILEBOX_H
