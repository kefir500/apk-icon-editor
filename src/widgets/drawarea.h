///
/// \file
/// This file contains the icon preview widget declaration.
///

#ifndef DRAWAREA_H
#define DRAWAREA_H

#include "icon.h"
#include <QLabel>

///
/// Icon preview widget.
/// This widget draws the specified icon in the preview area.
///

class DrawArea : public QLabel {
    Q_OBJECT

public:
    explicit DrawArea(QWidget *parent = 0);

    /// Draws the specified \c icon in the preview area.
    void setIcon(Icon *icon);

    /// Sets the size of the template rectange which will represent the standard icon size.
    /// \param w Width of the template rectange.
    /// \param h Height of the template rectange.
    void setRect(int w, int h) { rect_w = w; rect_h = h; }

    /// Sets the background color to \c color.
    /// If the \c color differs from the default one, grid dots are not painted.
    void setBack(QColor color) { if (color != QColor::Invalid) background = color; }

    /// Returns the current icon.
    Icon *getIcon() const { return icon; }

signals:
    /// This signal is emitted when this widget is clicked.
    void clicked() const;

protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    Icon *icon;        ///< Currently previewed icon.
    QColor background; ///< Preview area background color.
    bool welcome;      ///< If \c true, prints the welcoming text and allows the \c clicked signal.
    int rect_w;        ///< Template rectangle width.
    int rect_h;        ///< Template rectangle height.

    /// Enables or disables the mouse hover widget styling.
    void setAllowHover(bool allow);
};

#endif // DRAWAREA_H
