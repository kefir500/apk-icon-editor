#ifndef EFFECTS_H
#define EFFECTS_H

#include <QGroupBox>
#include <QSlider>
#include <QPushButton>
#include <QColorDialog>
#include <QDialogButtonBox>

class EffectsDialog : public QDialog
{
    Q_OBJECT

private:
    QGroupBox *checkColor;
    QGroupBox *checkBlur;
    QPushButton *btnColor;
    QSlider *slideBlur;
    QColorDialog *colorDialog;
    QDialogButtonBox *buttons;

public:
    void retranslate(); ///< Retranslate displayed strings.
    explicit EffectsDialog(QWidget *parent = 0);

signals:
    void colorActivated(bool state);    ///< Switch "Colorize" effect on/off.
    void blurActivated(bool state);     ///< Switch "Blur" effect on/off.
    void colorize(QColor color);        ///< Set "Colorize" effect \c color.
    void blur(qreal radius);            ///< Set "Blur" effect \c radius.

public slots:
    /// Turn "Colorize" effect groupbox on/off.
    void setColorEnabled(bool state) { checkColor->setChecked(state); }

    /// Turn "Blur" effect groupbox on/off.
    void setBlurEnabled(bool state) { checkBlur->setChecked(state); }

    void setColor(QColor color);    ///< Set "Colorize" effect button \c color.
    void setBlur(int value);        ///< Set "Blur" slider \c value.

public slots:

};

#endif // EFFECTS_H
