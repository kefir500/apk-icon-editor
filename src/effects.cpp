#include "effects.h"
#include <QVBoxLayout>

EffectsDialog::EffectsDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Effects"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(260, 180);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QVBoxLayout *layoutColor = new QVBoxLayout();
    QVBoxLayout *layoutBlur = new QVBoxLayout();

    checkColor = new QGroupBox(this);
    checkBlur = new QGroupBox(this);
    btnColor = new QPushButton(this);
    slideBlur = new QSlider(Qt::Horizontal, this);
    colorDialog = new QColorDialog(this);
    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    checkColor->setCheckable(true);
    checkBlur->setCheckable(true);
    checkColor->setLayout(layoutColor);
    checkBlur->setLayout(layoutBlur);
    btnColor->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    layout->addWidget(checkColor);
    layout->addWidget(checkBlur);
    layoutColor->addWidget(btnColor);
    layoutBlur->addWidget(slideBlur);
    layout->addWidget(buttons);

    connect(checkColor, SIGNAL(clicked(bool)), this, SIGNAL(colorActivated(bool)));
    connect(checkBlur, SIGNAL(clicked(bool)), this, SIGNAL(blurActivated(bool)));
    connect(btnColor, SIGNAL(clicked()), colorDialog, SLOT(exec()));
    connect(slideBlur, SIGNAL(valueChanged(int)), this, SLOT(setBlur(int)));
    connect(colorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(setColor(QColor)));
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void EffectsDialog::retranslate()
{
    checkColor->setTitle(tr("Color Overlay"));
    checkBlur->setTitle(tr("Blur"));
    btnColor->setText(tr("Select Color"));
}

void EffectsDialog::setColor(QColor color)
{
    QPixmap icon(32, 32);
    icon.fill(color);
    btnColor->setIcon(QIcon(icon));
    colorDialog->setCurrentColor(color);
    emit colorize(color);
}

void EffectsDialog::setBlur(int value)
{
    slideBlur->setSliderPosition(value);
    emit blur(value);
}
