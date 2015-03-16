#include "tooldialog.h"
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
    setWindowIcon(QIcon(":/gfx/task-pack.png"));
    resize(200, 0);

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

    layout->addWidget(radioZip);
    layout->addWidget(radioApktool);
    layout->addWidget(groupZip);
    layout->addWidget(groupApktool);
    layout->addWidget(checkSign);
    layout->addWidget(checkOptimize);
    layout->addWidget(buttons);
    layoutZip->addWidget(labelComp);
    layoutZip->addWidget(labelMin);
    layoutZip->addWidget(slideRatio);
    layoutZip->addWidget(labelMax);
    layoutApktool->addWidget(checkSmali);

    connect(radioZip, SIGNAL(clicked()), this, SLOT(setOptionZip()));
    connect(radioApktool, SIGNAL(clicked()), this, SLOT(setOptionApktool()));
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    radioZip->click();
}

void ToolDialog::accept()
{
    if (tempIsApktool != getUseApktool()) {
        tempIsApktool = getUseApktool();
        emit toolChanged();
    }
    tempRatio = getRatio();
    tempSmali = getSmali();
    tempSign = getSign();
    tempOptimize = getOptimize();
    QDialog::accept();
}

void ToolDialog::reject()
{
    setUseApktool(tempIsApktool);
    setRatio(tempRatio);
    setSmali(tempSmali);
    setSign(tempSign);
    setOptimize(tempOptimize);
    QDialog::reject();
}

void ToolDialog::setOptionZip()
{
    groupApktool->hide();
    groupZip->show();
    emit apktoolChecked(false);
}

void ToolDialog::setOptionApktool()
{
    groupZip->hide();
    groupApktool->show();
    emit apktoolChecked(true);
}

void ToolDialog::setRatio(short value)
{
    tempRatio = value;
    slideRatio->setValue(value);
}

void ToolDialog::setSmali(bool unpack)
{
    tempSmali = unpack;
    checkSmali->setChecked(unpack);
}

void ToolDialog::setUseApktool(bool use)
{
    tempIsApktool = use;
    if (use) {
        radioApktool->setChecked(true);
        setOptionApktool();
    }
    else {
        radioZip->setChecked(true);
        setOptionZip();
    }
}

void ToolDialog::setSign(bool sign)
{
    tempSign = sign;
    checkSign->setChecked(sign);
}

void ToolDialog::setOptimize(bool optimize)
{
    tempOptimize = optimize;
    checkOptimize->setChecked(optimize);
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
}
