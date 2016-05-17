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

private slots:
    void setCategory(QString text);

private:
    void parse();
    QListWidget *list;
    QTextBrowser *text;
    QMap<QString, QString> authors;
};

#endif // AUTHORS_H
