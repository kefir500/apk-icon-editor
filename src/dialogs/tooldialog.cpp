#include "tooldialog.h"
#include "settings.h"
#include <QDialogButtonBox>

const char *ToolDialog::STR_QUAZIP = QT_TR_NOOP(
    "Use default QuaZIP repacking.\n"
    "This is the fastest method.\n"
    "Provides different compression ratios.");

const char *ToolDialog::STR_APKTOOL = QT_TR_NOOP(
    "Use Apktool repacking. This method is slower,\n"
    "but it provides more reverse-engineering options\n"
    "like editing application name and version\n"
    "as well as decompiling binary resources.");

ToolDialog::ToolDialog(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowIcon(QIcon(":/gfx/actions/box.png"));
    resize(440, 0);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *layoutZip = new QHBoxLayout();
    QHBoxLayout *layoutApktool = new QHBoxLayout();

    radioZip = new QRadioButton(this);
    radioApktool = new QRadioButton(this);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    groupZip = new QGroupBox(this);
    groupZip->setLayout(layoutZip);
    groupApktool = new QGroupBox(this);
    groupApktool->setLayout(layoutApktool);

    labelComp = new QLabel(this);
    labelMin = new QLabel(this);
    labelMax = new QLabel(this);
    slideRatio = new QSlider(Qt::Horizontal, this);
    slideRatio->setRange(0, 9);
    slideRatio->setTickPosition(QSlider::TicksBelow);
    checkSmali = new QCheckBox(this);

    checkSign = new QCheckBox(this);
    checkOptimize = new QCheckBox(this);
    tempDir = new FileBox(this, true);

    layout->addWidget(radioZip);
    layout->addWidget(radioApktool);
    layout->addWidget(groupZip);
    layout->addWidget(groupApktool);
    layout->addWidget(checkSign);
    layout->addWidget(checkOptimize);
    layout->addWidget(tempDir);
    layout->addWidget(buttons);
    layoutZip->addWidget(labelComp);
    layoutZip->addWidget(labelMin);
    layoutZip->addWidget(slideRatio);
    layoutZip->addWidget(labelMax);
    layoutApktool->addWidget(checkSmali);

    connect(radioZip, SIGNAL(clicked()), this, SLOT(setModeZip()));
    connect(radioApktool, SIGNAL(clicked()), this, SLOT(setModeApktool()));
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    radioZip->click();
}

void ToolDialog::accept()
{
    const bool APKTOOL = radioApktool->isChecked();
    const bool SMALI = checkSmali->isChecked();
    const bool SIGN = checkSign->isChecked();
    const bool ZIPALIGN = checkOptimize->isChecked();
    const int COMPRESSION = slideRatio->value();
    const QString TEMP = tempDir->value();

    if (APKTOOL != Settings::get_use_apktool()) {
        Settings::set_use_apktool(APKTOOL);
        emit tool_changed();
    }
    else {
        Settings::set_use_apktool(APKTOOL);
    }

    Settings::set_smali(SMALI);
    Settings::set_compression(COMPRESSION);
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
    slideRatio->setValue(Settings::get_compression());
    checkSmali->setChecked(Settings::get_smali());
    checkSign->setChecked(Settings::get_sign());
    checkOptimize->setChecked(Settings::get_zipalign());
    tempDir->setValue(Settings::get_temp(false));
    Settings::get_use_apktool() ? setModeApktool() : setModeZip();
}

void ToolDialog::switch_mode()
{
    Settings::set_use_apktool(!Settings::get_use_apktool());
    emit apktool_checked(Settings::get_use_apktool());
    reset();
}

void ToolDialog::setModeZip()
{
    radioZip->setChecked(true);
    groupZip->show();
    groupApktool->hide();
    emit apktool_checked(false);
}

void ToolDialog::setModeApktool()
{
    radioApktool->setChecked(true);
    groupApktool->show();
    groupZip->hide();
    emit apktool_checked(true);
}

void ToolDialog::retranslate()
{
    setWindowTitle(tr("Repacking"));
    radioZip->setText(tr(STR_QUAZIP));
    radioApktool->setText(tr(STR_APKTOOL));
    labelComp->setText(tr("Compression:"));
    labelMin->setText(tr("MIN"));
    labelMax->setText(tr("MAX"));
    checkSmali->setText(tr("Decompile %1").arg("CLASSES.DEX"));
    checkSign->setText(tr("Sign APK"));
    checkOptimize->setText(tr("Optimize APK"));
    tempDir->setTitle(tr("Temporary Directory") + ":");
}
