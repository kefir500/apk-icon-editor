#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include "filebox.h"
#include <QDialog>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QSpinBox>

// LineEditLabel

class LineEditLabel : public QWidget {
    Q_OBJECT

public:
    explicit LineEditLabel(QWidget *parent = 0);
    void setTitle(QString title);
    void setValue(QString value);
    void setPassword(bool value);
    QString getValue() const { return edit->text(); }

private:
    QLabel *label;
    QLineEdit *edit;
};

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
    static void createKey(KeyParams);

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

    QString getFilePem() const { return filePem; }
    QString getFilePk8() const { return filePk8; }
    QString getFileKey() const { return fileKey; }
    bool getIsKeystore() const { return isKeystore; }
    QString getAlias() const { return editAlias->getValue(); }
    QString getPassStore() const { return editStorePass->getValue(); }
    QString getPassAlias() const { return editAliasPass->getValue(); }

    void retranslate();

private:
    bool isKeystore;
    QString filePem;
    QString filePk8;
    QString fileKey;
    QString alias;
    QString passStore;
    QString passAlias;

    KeyCreator *keyCreator;
    FileBox *boxPem;
    FileBox *boxPk8;
    FileBox *boxKey;
    QRadioButton *radioPem;
    QRadioButton *radioKey;
    QGroupBox *groupPem;
    QGroupBox *groupKey;
    QPushButton *btnNew;
    LineEditLabel *editAlias;
    LineEditLabel *editAliasPass;
    LineEditLabel *editStorePass;

signals:
    void success(QString title, QString text);
    void warning(QString title, QString text);
    void error(QString title, QString text);

public slots:
    void setFilePem(QString value);
    void setFilePk8(QString value);
    void setFileKey(QString value);
    void setIsKeyStore(bool value);
    void setAlias(QString value);
    void setPassStore(QString password);
    void setPassAlias(QString password);

protected slots:
    virtual void accept();
    virtual void reject();

private slots:
    void setOptionPem();
    void setOptionKey();
};

#endif // KEYMANAGER_H
