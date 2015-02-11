#include "keymanager.h"
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QProcess>
#include <QDebug>

KeyManager::KeyManager(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowIcon(QIcon(":/gfx/key.png"));
    resize(540, 0);

    keyCreator = new KeyCreator(this);

    radioPem = new QRadioButton(this);
    radioKey = new QRadioButton(this);

    boxPem = new FileBox(this);
    boxPk8 = new FileBox(this);
    boxKey = new FileBox(this);

    boxPem->setTitle("PEM");
    boxPk8->setTitle("PK8");
    boxKey->setTitle("KeyStore");

    boxPem->setTitleWidth(44);
    boxPk8->setTitleWidth(44);
    boxKey->setTitleWidth(44);

    boxPem->setFormats("PEM (*.pem);;");
    boxPk8->setFormats("PK8 (*.pk8);;");
    boxKey->setFormats("KeyStore (*.keystore);;");

    btnNew = new QPushButton(this);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    editAlias = new LineEditLabel(this);
    editAliasPass = new LineEditLabel(this);
    editStorePass = new LineEditLabel(this);

    editAliasPass->setPassword(true);
    editStorePass->setPassword(true);

    groupPem = new QGroupBox(this);
    QVBoxLayout *layoutPem = new QVBoxLayout;
    layoutPem->addWidget(boxPem);
    layoutPem->addWidget(boxPk8);
    groupPem->setLayout(layoutPem);

    groupKey = new QGroupBox(this);
    QGridLayout *layoutKey = new QGridLayout;
    layoutKey->addWidget(boxKey, 0, 0, 1, 0);
    layoutKey->addWidget(editStorePass, 1, 0, 1, 0);
    layoutKey->addWidget(btnNew, 2, 0, 1, 0);
    layoutKey->addWidget(editAlias, 3, 0);
    layoutKey->addWidget(editAliasPass, 3, 1);
    groupKey->setLayout(layoutKey);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(radioPem);
    layout->addWidget(radioKey);
    layout->addWidget(groupPem);
    layout->addWidget(groupKey);
    layout->addWidget(buttons);

    connect(radioPem, SIGNAL(clicked()), this, SLOT(setOptionPem()));
    connect(radioKey, SIGNAL(clicked()), this, SLOT(setOptionKey()));
    connect(btnNew, SIGNAL(clicked()), keyCreator, SLOT(open()));
    connect(keyCreator, SIGNAL(created(QString)), this, SLOT(setFileKey(QString)));
    connect(keyCreator, SIGNAL(success(QString, QString)), this, SIGNAL(success(QString, QString)));
    connect(keyCreator, SIGNAL(warning(QString, QString)), this, SIGNAL(warning(QString, QString)));
    connect(keyCreator, SIGNAL(error(QString, QString)), this, SIGNAL(error(QString, QString)));
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void KeyManager::retranslate()
{
    setWindowTitle(tr("Key Manager"));
    radioPem->setText("PEM/PK8");
    radioKey->setText(QString("KeyStore (%1)").arg(tr("Requires JDK")));
    btnNew->setText(tr("Create &New KeyStore or Alias"));
    editAlias->setTitle(QString("%1:").arg(tr("Alias")));
    editAliasPass->setTitle(QString("%1:").arg(tr("Alias Password")));
    editStorePass->setTitle(QString("%1:").arg(tr("KeyStore Password")));
    keyCreator->retranslate();
}

void KeyManager::setFilePem(QString value)
{
    filePem = value;
    boxPem->setValue(value);
}

void KeyManager::setFilePk8(QString value)
{
    filePk8 = value;
    boxPk8->setValue(value);
}

void KeyManager::setFileKey(QString value)
{
    fileKey = value;
    boxKey->setValue(value);
}

void KeyManager::setIsKeyStore(bool value)
{
    isKeystore = value;
    if (isKeystore) {
        setOptionKey();
        radioKey->setChecked(true);
    }
    else {
        setOptionPem();
        radioPem->setChecked(true);
    }
}

void KeyManager::setAlias(QString value)
{
    alias = value;
    editAlias->setValue(alias);
}

void KeyManager::setPassStore(QString password)
{
    passStore = password;
    editStorePass->setValue(password);
}

void KeyManager::setPassAlias(QString password)
{
    passAlias = password;
    editAliasPass->setValue(password);
}

void KeyManager::setOptionPem()
{
    groupPem->setEnabled(true);
    groupKey->setEnabled(false);
}

void KeyManager::setOptionKey()
{
    groupPem->setEnabled(false);
    groupKey->setEnabled(true);
}

void KeyManager::accept()
{
    setFilePem(boxPem->value());
    setFilePk8(boxPk8->value());
    setFileKey(boxKey->value());
    setIsKeyStore(radioKey->isChecked());
    setAlias(editAlias->getValue());
    setPassStore(editStorePass->getValue());
    setPassAlias(editAliasPass->getValue());
    QDialog::accept();
}

void KeyManager::reject()
{
    setFilePem(filePem);
    setFilePk8(filePk8);
    setFileKey(fileKey);
    setIsKeyStore(isKeystore);
    setAlias(alias);
    setPassStore(passStore);
    setPassAlias(passAlias);
    QDialog::reject();
}

// KeyCreator

KeyCreator::KeyCreator(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(400, 0);

    QGridLayout *layout = new QGridLayout(this);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    labelStorePass1 = new QLabel(this);
    labelStorePass2 = new QLabel(this);
    labelAlias = new QLabel(this);
    labelAliasPass1 = new QLabel(this);
    labelAliasPass2 = new QLabel(this);
    labelYears = new QLabel(this);
    labelName = new QLabel(this);
    labelUnit = new QLabel(this);
    labelOrgan = new QLabel(this);
    labelCity = new QLabel(this);
    labelState = new QLabel(this);
    labelCountry = new QLabel(this);
    editStorePass1 = new QLineEdit(this);
    editStorePass2 = new QLineEdit(this);
    editAlias = new QLineEdit(this);
    editAliasPass1 = new QLineEdit(this);
    editAliasPass2 = new QLineEdit(this);
    editYears = new QSpinBox(this);
    editName = new QLineEdit(this);
    editUnit = new QLineEdit(this);
    editOrgan = new QLineEdit(this);
    editCity = new QLineEdit(this);
    editState = new QLineEdit(this);
    editCountry = new QLineEdit(this);

    editStorePass1->setFocus();
    editStorePass1->setEchoMode(QLineEdit::Password);
    editStorePass2->setEchoMode(QLineEdit::Password);
    editAliasPass1->setEchoMode(QLineEdit::Password);
    editAliasPass2->setEchoMode(QLineEdit::Password);
    editYears->setRange(1, 999999);

    clear();

    layout->addWidget(labelStorePass1, 0, 0);
    layout->addWidget(labelStorePass2, 1, 0);
    layout->addWidget(labelAlias, 2, 0);
    layout->addWidget(labelAliasPass1, 3, 0);
    layout->addWidget(labelAliasPass2, 4, 0);
    layout->addWidget(labelYears, 5, 0);
    layout->addWidget(labelName, 6, 0);
    layout->addWidget(labelUnit, 7, 0);
    layout->addWidget(labelOrgan, 8, 0);
    layout->addWidget(labelCity, 9, 0);
    layout->addWidget(labelState, 10, 0);
    layout->addWidget(labelCountry, 11, 0);
    layout->addWidget(editStorePass1, 0, 1);
    layout->addWidget(editStorePass2, 1, 1);
    layout->addWidget(editAlias, 2, 1);
    layout->addWidget(editAliasPass1, 3, 1);
    layout->addWidget(editAliasPass2, 4, 1);
    layout->addWidget(editYears, 5, 1);
    layout->addWidget(editName, 6, 1);
    layout->addWidget(editUnit, 7, 1);
    layout->addWidget(editOrgan, 8, 1);
    layout->addWidget(editCity, 9, 1);
    layout->addWidget(editState, 10, 1);
    layout->addWidget(editCountry, 11, 1);
    layout->addWidget(buttons, 12, 1);

    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void KeyCreator::retranslate()
{
    labelStorePass1->setText(QString("%1 (%2):").arg(tr("Password"), tr("KeyStore")));
    labelStorePass2->setText(QString("%1 (%2):").arg(tr("Confirm Password"), tr("KeyStore")));
    labelAlias->setText(QString("%1:").arg(tr("Alias")));
    labelAliasPass1->setText(QString("%1 (%2):").arg(tr("Password"), tr("Alias")));
    labelAliasPass2->setText(QString("%1 (%2):").arg(tr("Confirm Password"), tr("Alias")));
    labelYears->setText(QString("%1:").arg(tr("Validity (Years)")));
    labelName->setText(QString("%1:").arg(tr("First and Last Name")));
    labelUnit->setText(QString("%1:").arg(tr("Organizational Unit")));
    labelOrgan->setText(QString("%1:").arg(tr("Organization")));
    labelCity->setText(QString("%1:").arg(tr("City or Locality")));
    labelState->setText(QString("%1:").arg(tr("State or Province")));
    labelCountry->setText(QString("%1:").arg(tr("Country Code")));
}

void KeyCreator::clear()
{
    editStorePass1->clear();
    editStorePass2->clear();
    editAlias->clear();
    editAliasPass1->clear();
    editAliasPass2->clear();
    editYears->setValue(25);
    editName->clear();
    editUnit->clear();
    editOrgan->clear();
    editCity->clear();
    editState->clear();
    editCountry->clear();
}

void KeyCreator::accept()
{
    // Validate data:

    if (editStorePass1->text().length() < 6) {
        emit warning(NULL, tr("Password must be at least 6 characters."));
        editStorePass1->setFocus();
        editStorePass1->selectAll();
        return;
    }

    if (editStorePass1->text() != editStorePass2->text()) {
        emit warning(NULL, tr("Passwords do not match."));
        editStorePass2->setFocus();
        editStorePass2->selectAll();
        return;
    }

    if (editAlias->text().isEmpty()) {
        emit warning(NULL, tr("Enter alias name."));
        editAlias->setFocus();
        editAlias->selectAll();
        return;
    }

    if (editAliasPass1->text() != editAliasPass2->text()) {
        emit warning(NULL, tr("Passwords do not match."));
        editAliasPass2->setFocus();
        editAliasPass2->selectAll();
        return;
    }

    if (editAliasPass1->text().length() < 6) {
        emit warning(NULL, tr("Password must be at least 6 characters."));
        editAliasPass1->setFocus();
        editAliasPass1->selectAll();
        return;
    }

    // Create KeyStore and Alias:

    const QString FILENAME = QFileDialog::getSaveFileName(this, NULL, NULL, "KeyStore (*.keystore)");
    if (FILENAME.isEmpty()) {
        return;
    }
    qDebug() << "Creating KeyStore...";
    const QString ENV_PATH = qgetenv("PATH");
    const QString JAVA_HOME = qgetenv("JAVA_HOME");
    const QString KEYTOOL_CMD =
            QString("keytool -genkeypair -v -keystore \"%1\" -storepass \"%10\""
                    " -alias \"%2\" -keyalg RSA -keysize 2048"
                    " -dname \"CN=%3, OU=%4, O=%5, L=%6, S=%7, C=%8\""
                    " -validity %9 -keypass \"%11\"")
                        .arg(FILENAME)
                        .arg(editAlias->text())
                        .arg(editName->text())
                        .arg(editUnit->text())
                        .arg(editOrgan->text())
                        .arg(editCity->text())
                        .arg(editState->text())
                        .arg(editCountry->text())
                        .arg(editYears->text().toInt() * 365);
    qputenv("PATH", ENV_PATH.toStdString().c_str());
    qputenv("PATH", QString("%1;%2/bin").arg(ENV_PATH, JAVA_HOME).toStdString().c_str());
    qDebug() << qPrintable(KEYTOOL_CMD.arg("*****", "*****"));
    QProcess p;
    p.start(KEYTOOL_CMD.arg(editStorePass1->text(), editAliasPass1->text()));
    qputenv("PATH", ENV_PATH.toStdString().c_str());

    if (p.waitForStarted(-1)) {
        p.waitForFinished(10000);
        if (p.exitCode() == 0) {
            // TODO fire signal FILENAME
        }
        else {
            QString error_text = p.readAllStandardError().trimmed();
            if (error_text.isEmpty()) error_text = p.readAllStandardOutput().trimmed();
            qDebug() << qPrintable(QString("Keytool exit code: %1").arg(p.exitCode()));
            qDebug() << error_text;
            emit warning("Keytool", tr("%1: invalid parameters").arg("Keytool"));
            return;
        }
    }
    else {
        const QString ERROR_TEXT = tr("Error starting %1.\n"
                                      "Check your JDK installation and "
                                      "PATH environment variable.").arg("Keytool");
        emit error("Keytool", ERROR_TEXT);
        return;
    }
    qDebug() << "Done.\n";
    emit success("Keytool", tr("KeyStore successfully created/updated!"));
    emit created(FILENAME);
    clear();
    QDialog::accept();
}

void KeyCreator::reject()
{
    clear();
    QDialog::reject();
}

void KeyCreator::createKey(KeyParams)
{
}

// LineEditLabel

LineEditLabel::LineEditLabel(QWidget *parent) : QWidget(parent)
{
    label = new QLabel(this);
    edit = new QLineEdit(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(label);
    layout->addWidget(edit);
}

void LineEditLabel::setTitle(QString title)
{
    label->setText(title);
}

void LineEditLabel::setValue(QString value)
{
    edit->setText(value);
}

void LineEditLabel::setPassword(bool value)
{
    edit->setEchoMode(value ? QLineEdit::Password : QLineEdit::Normal);
}
