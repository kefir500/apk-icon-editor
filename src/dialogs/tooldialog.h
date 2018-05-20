#ifndef TOOLDIALOG_H
#define TOOLDIALOG_H

#include "filebox.h"
#include <QDialog>
#include <QBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QCheckBox>

class ToolDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolDialog(QWidget *parent = 0);
    void retranslate();

private:
    FileBox *apktoolPath;
    QRadioButton *radioApkSigner;
    QRadioButton *radioJarSigner;
    QCheckBox *checkSmali;
    QCheckBox *checkOptimize;
    QGroupBox *groupSign;
    FileBox *tempDir;

public slots:
    void reset();

protected slots:
    virtual void accept();
    virtual void reject();
};

#endif // TOOLDIALOG_H
