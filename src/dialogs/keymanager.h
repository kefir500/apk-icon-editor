#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include "filebox.h"
#include <QDialog>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QSpinBox>

// KeyParams

struct KeyParams {
    QString password;
    QString years;
    QString name;
    QString unit;
    QString organization;
    QString city;
    QString state;
    QString country;
};

// KeyCreator

class KeyCreator : public QDialog {
    Q_OBJECT

public:
    explicit KeyCreator(QWidget *parent = 0);
    void retranslate();

protected slots:
    virtual void accept();
    virtual void reject();

private:
    void clear();
    QLabel *labelStorePass1;
    QLabel *labelStorePass2;
    QLabel *labelAlias;
    QLabel *labelAliasPass1;
    QLabel *labelAliasPass2;
    QLabel *labelYears;
    QLabel *labelName;
    QLabel *labelUnit;
    QLabel *labelOrgan;
    QLabel *labelCity;
    QLabel *labelState;
    QLabel *labelCountry;
    QLineEdit *editStorePass1;
    QLineEdit *editStorePass2;
    QLineEdit *editAlias;
    QLineEdit *editAliasPass1;
    QLineEdit *editAliasPass2;
    QSpinBox *editYears;
    QLineEdit *editName;
    QLineEdit *editUnit;
    QLineEdit *editOrgan;
    QLineEdit *editCity;
    QLineEdit *editState;
    QLineEdit *editCountry;

signals:
    void created(QString filename);
    void success(QString title, QString text);
    void warning(QString title, QString text);
    void error(QString title, QString text);
};

// KeyManager

class KeyManager : public QDialog {
    Q_OBJECT

public:
    explicit KeyManager(QWidget *parent = 0);
    void reset();
    void retranslate();

private:
    KeyCreator *keyCreator;
    FileBox *boxPem;
    FileBox *boxPk8;
    FileBox *boxKey;
    QRadioButton *radioPem;
    QRadioButton *radioKey;
    QGroupBox *groupPem;
    QGroupBox *groupKey;
    QPushButton *btnNew;
    QLabel *labelAlias;
    QLabel *labelAliasPass;
    QLabel *labelStorePass;
    QLineEdit *editAlias;
    QLineEdit *editAliasPass;
    QLineEdit *editStorePass;

signals:
    void success(QString title, QString text);
    void warning(QString title, QString text);
    void error(QString title, QString text);

protected slots:
    virtual void accept();
    virtual void reject();

private slots:
    void setOptionPem();
    void setOptionKey();
};

#endif // KEYMANAGER_H
