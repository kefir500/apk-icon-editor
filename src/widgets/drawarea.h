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

    /// Sets the size of the border which will represent the default icon size.
    /// \param bounds Size of the template border.
    void setBounds(QSize bounds) { this->bounds = bounds; }

    /// Sets the size of the border which will represent the default icon size.
    /// \param w Width of the template border.
    /// \param h Height of the template border.
    void setBounds(int w, int h) { bounds = QSize(w, h); }

    /// Sets the background color to \c color.
    /// If the \c color differs from the default one, grid dots are not painted.
    void setBackground(QColor color) { if (color != QColor::Invalid) background = color; }

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
    QSize bounds;      ///< Size of the template border.

    /// Enables or disables the mouse hover widget styling.
    void setAllowHover(bool allow);
};

#endif // DRAWAREA_H
