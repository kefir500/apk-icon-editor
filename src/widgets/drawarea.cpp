#include "drawarea.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QPainter>
#include <QDir>

DrawArea::DrawArea(QWidget *parent) : QLabel(parent)
{
#ifndef Q_OS_OSX
    setFont(QFont("Open Sans Light", 12));
#else
    setFont(QFont("Open Sans", 15, QFont::Light));
#endif
    setAlignment(Qt::AlignCenter);
    setAllowHover(true);

    setMouseTracking(true);
    setAutoFillBackground(true);
    setCursor(Qt::PointingHandCursor);

    welcome = true;
    bounds = QSize(0, 0);
    background = palette().color(QPalette::Background);
}

void DrawArea::setIcon(Icon *icon)
{
    this->icon = icon;
    welcome = false;
    setCursor(Qt::ArrowCursor);
    setAllowHover(false);
    repaint();
}

void DrawArea::mousePressEvent(QMouseEvent *event)
{
    if (welcome && event->button() == Qt::LeftButton) {
        emit clicked();
    }
    event->accept();
}

void DrawArea::paintEvent(QPaintEvent *event)
{
    if (!welcome) {
        QPainter painter(this);

        // Border bounds:
        const int bx = width() / 2 - bounds.width() / 2;
        const int by = height() / 2 - bounds.height() / 2;
        const int bw = bounds.width();
        const int bh = bounds.height();

        painter.fillRect(bx + 1, by + 1, bw - 1, bw - 1, background);
        if (background == palette().color(QPalette::Window)) {
            painter.fillRect(bx + 1, by + 1, bw - 1, bh - 1, Qt::Dense7Pattern);
        }
        painter.drawPixmap(width() / 2 - icon->width() / 2,
                           height() / 2 - icon->height() / 2,
                           icon->getPixmap());
        painter.drawRect(bx, by, bw, bh);
        painter.setPen(Qt::lightGray);
        painter.drawRect(bx - 1, by - 1, bw + 2, bh + 2);
    }
    else {
        QLabel::paintEvent(event);
    }
}

void DrawArea::setAllowHover(bool allow)
{
    QString style =
        "DrawArea { background: url(:/gfx/background.png)"
        "no-repeat bottom left;"
        "border: 1px solid gray; }"
    ;
    if (allow) style += "DrawArea::hover { background-color: white; }";
    setStyleSheet(style);
}
