#ifndef TOOLDIALOG_H
#define TOOLDIALOG_H

#include "filebox.h"
#include <QDialog>
#include <QBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>

class ToolDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolDialog(QWidget *parent = 0);
    QString hint_quazip() { return tr(STR_QUAZIP); }
    QString hint_apktool() { return tr(STR_APKTOOL); }
    void retranslate();

private:
    static const char *STR_QUAZIP;
    static const char *STR_APKTOOL;
    QRadioButton *radioZip;
    QRadioButton *radioApktool;
    QRadioButton *radioApkSigner;
    QRadioButton *radioJarSigner;
    QLabel *labelComp;
    QLabel *labelMin;
    QLabel *labelMax;
    QSlider *slideRatio;
    QCheckBox *checkSmali;
    QCheckBox *checkOptimize;
    QGroupBox *groupZip;
    QGroupBox *groupApktool;
    QGroupBox *groupSign;
    FileBox *tempDir;

signals:
    void apktool_checked(bool);
    void tool_changed();

public slots:
    void switch_mode();
    void reset();

protected slots:
    virtual void accept();
    virtual void reject();

private slots:
    void setModeZip();
    void setModeApktool();
};

#endif // TOOLDIALOG_H
