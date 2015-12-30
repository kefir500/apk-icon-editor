/// \file effects.h
/// \brief This file contains #EffectsDialog declaration.

#ifndef EFFECTS_H
#define EFFECTS_H

#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QSignalMapper>

/// \brief Dialog for customizing icon graphic effects.

class EffectsDialog : public QDialog
{
    Q_OBJECT

private:
    QGroupBox *groupRotate;
    QGroupBox *groupColor;
    QPushButton *btnRotate0;
    QPushButton *btnRotate90;
    QPushButton *btnRotate180;
    QPushButton *btnRotate270;
    QPushButton *btnFlipX;
    QPushButton *btnFlipY;
    QPushButton *btnColor;
    QLabel *labelColor;
    QLabel *labelCorners;
    QLabel *labelBlur;
    QSlider *slideColor;
    QSlider *slideBlur;
    QSlider *slideCorners;
    QColorDialog *colorDialog;
    QDialogButtonBox *buttons;
    QSignalMapper *mapRotate;

public:
    void retranslate(); ///< Retranslate displayed strings.
    explicit EffectsDialog(QWidget *parent = 0);

signals:
    void colorActivated(bool state);    ///< Switch "Colorize" effect on/off.
    void blurActivated(bool state);     ///< Switch "Blur" effect on/off.
    void rotate(int rot);               ///< Set current rotation angle (in degrees).
    void flipX(bool flip);              ///< Set current horizontal flip.
    void flipY(bool flip);              ///< Set current vertical flip.
    void colorize(QColor color);        ///< Set "Colorize" effect \c color.
    void colorDepth(qreal depth);       ///< Set "Colorize" effect color \c depth.
    void blur(qreal radius);            ///< Set "Blur" effect \c radius.
    void round(qreal radius);           ///< Set rounded corners \c radius.

public slots:
    /// Turn "Colorize" effect groupbox on/off.
    void setColorize(bool state) { groupColor->setChecked(state); }

    /// Turn "Flip Horizontal" button on/off.
    void setFlipX(bool value) { btnFlipX->setChecked(value); }

    /// Turn "Flip Vertical" button on/off.
    void setFlipY(bool value) { btnFlipY->setChecked(value); }

    void setRotation(int rot);      ///< Set active rotation button.
    void setColor(QColor color);    ///< Set "Colorize" effect button \c color.
    void setColorDepth(int depth);  ///< Set "Colorize" slider \c value.
    void setBlur(int value);        ///< Set "Blur" slider \c value.
    void setCorners(int value);     ///< Set "Rounded Corners" slider \c value.

};

#endif // EFFECTS_H
