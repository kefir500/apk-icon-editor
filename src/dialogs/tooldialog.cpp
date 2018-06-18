#include "tooldialog.h"
#include "settings.h"
#include "globals.h"
#include <QDialogButtonBox>

ToolDialog::ToolDialog(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowIcon(QIcon(":/gfx/actions/box.png"));
    resize(Gui::Screen::scaled(440, 0));

    QVBoxLayout *layout = new QVBoxLayout(this);
    QVBoxLayout *layoutSign = new QVBoxLayout();

    radioApkSigner = new QRadioButton(QString("apksigner (JRE %1 8)").arg(QChar(0x2265)), this);
    radioJarSigner = new QRadioButton(QString("signapk (JRE %1 8) + jarsigner (JDK)").arg(QChar(0x2264)), this);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    groupSign = new QGroupBox(this);
    groupSign->setCheckable(true);
    groupSign->setLayout(layoutSign);

    apktoolPath = new FileBox(this, false);
    checkSmali = new QCheckBox(this);
    checkOptimize = new QCheckBox(this);
    tempDir = new FileBox(this, true);

    layout->addWidget(apktoolPath);
    layout->addWidget(checkSmali);
    layout->addWidget(checkOptimize);
    layout->addWidget(groupSign);
    layout->addWidget(tempDir);
    layout->addWidget(buttons);
    layoutSign->addWidget(radioApkSigner);
    layoutSign->addWidget(radioJarSigner);

    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void ToolDialog::accept()
{
    const QString APKTOOL = apktoolPath->value();
    const bool APKSIGNER = radioApkSigner->isChecked();
    const bool SMALI = checkSmali->isChecked();
    const bool SIGN = groupSign->isChecked();
    const bool ZIPALIGN = checkOptimize->isChecked();
    const QString TEMP = tempDir->value();

    Settings::set_apktool(APKTOOL);
    Settings::set_use_apksigner(APKSIGNER);
    Settings::set_smali(SMALI);
    Settings::set_sign(SIGN);
    Settings::set_zipalign(ZIPALIGN);
    Settings::set_temp(TEMP);

    reset();
    QDialog::accept();
}

void ToolDialog::reject()
{
    reset();
    QDialog::reject();
}

void ToolDialog::reset()
{
    apktoolPath->setValue(Settings::get_apktool(false));
    checkSmali->setChecked(Settings::get_smali());
    groupSign->setChecked(Settings::get_sign());
    checkOptimize->setChecked(Settings::get_zipalign());
    tempDir->setValue(Settings::get_temp(false));
    Settings::get_use_apksigner() ? radioApkSigner->setChecked(true) : radioJarSigner->setChecked(true);
}

void ToolDialog::retranslate()
{
    setWindowTitle(tr("Repacking"));
    apktoolPath->setTitle("Apktool:");
    checkSmali->setText(tr("Decompile %1").arg("CLASSES.DEX"));
    groupSign->setTitle(tr("Sign APK"));
    checkOptimize->setText(tr("Optimize APK"));
    tempDir->setTitle(tr("Temporary Directory") + ":");
}
