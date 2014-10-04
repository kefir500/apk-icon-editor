#ifndef TOOLDIALOG_H
#define TOOLDIALOG_H

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
    short getRatio() const { return short(slideRatio->value() * 1.9); }
    bool getUseApktool() const { return radioApktool->isChecked(); }
    bool getSmali() const {return checkSmali->isChecked(); }
    bool getSign() const { return checkSign->isChecked(); }
    bool getOptimize() const { return checkOptimize->isChecked(); }
    QString hint_7zip() { return tr(STR_7ZIP); }
    QString hint_apktool() { return tr(STR_APKTOOL); }
    void retranslate();

private:
    static const char *STR_7ZIP;
    static const char *STR_APKTOOL;
    QRadioButton *radioZip;
    QRadioButton *radioApktool;
    QLabel *labelComp;
    QLabel *labelMin;
    QLabel *labelMax;
    QSlider *slideRatio;
    QCheckBox *checkSmali;
    QCheckBox *checkSign;
    QCheckBox *checkOptimize;
    QGroupBox *groupZip;
    QGroupBox *groupApktool;
    short tempRatio;
    bool tempIsApktool;
    bool tempSmali;
    bool tempSign;
    bool tempOptimize;
    void saveTemp();

signals:
    void apktoolChecked(bool);
    void toolChanged();

public slots:
    void setRatio(short value);
    void setSmali(bool unpack);
    void setUseApktool(bool use);
    void setSign(bool sign);
    void setOptimize(bool optimize);

protected slots:
    virtual void accept();
    virtual void reject();

private slots:
    void setOptionZip();
    void setOptionApktool();
};

#endif // TOOLDIALOG_H
