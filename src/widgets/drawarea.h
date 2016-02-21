/// \file drawarea.h
/// \brief This file contains #DrawArea class declaration.

#ifndef DRAWAREA_H
#define DRAWAREA_H

#include "icon.h"
#include <QLabel>

/// \brief This widget previews #icon.
///
/// The DrawArea widget paints current #icon.

class DrawArea : public QLabel {
    Q_OBJECT

public:
    explicit DrawArea(QWidget *parent = 0);

    /// Start drawing given pixmap.
    /// \param[in] icon Pointer to icon for drawing.
    void setIcon(Icon *icon);

    /// Set template rectange which will represent standard icon size (according to selected #Profile).
    /// \param[in] w Width of the template rectange.
    /// \param[in] h Height of the template rectange.
    void setRect(int w, int h) { rect_w = w; rect_h = h; }

    /// Set background color. If it differs from default, grid dots are not painted.
    /// \param[in] color New background color.
    void setBack(QColor color) { if (color != QColor::Invalid) background = color; }

    /// Get current icon.
    /// \return Current icon.
    Icon *getIcon() const { return icon; }

signals:
    /// This signal is emitted when this widget is clicked.
    void clicked() const;

protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    Icon *icon;         ///< Current icon to draw.
    QColor background;  ///< Preview area background color.
    bool welcome;       ///< If \c TRUE, welcome text is rendered and #clicked signal is allowed.
    int rect_w;         ///< Template rectangle width.
    int rect_h;         ///< Template rectangle height.

    void setAllowHover(bool allow);
};

#endif // DRAWAREA_H
