#ifndef AUTHORS_H
#define AUTHORS_H

#include <QDialog>
#include <QListWidget>
#include <QTextBrowser>
#include <QMap>

class Authors : public QDialog
{
    Q_OBJECT

public:
    explicit Authors(QWidget *parent = 0);

public slots:

private slots:
    void setCategory(QString text);

private:
    void parse();
    QMap<QString, QString> authors;
    QListWidget *list;
    QTextBrowser *text;
};

#endif // AUTHORS_H
