#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QLabel>

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = 0);

public slots:
    void setVersions(QString jre, QString jdk, QString apktool);
    void showAuthors() const;

private:
    QLabel *textApp;
    QLabel *textLinks;
    QLabel *textReqs;
};

#endif // ABOUT_H
