#include "qiphotorubberband.h"
#include <QPainter>
#include <QPainterPath>

QiPhotoRubberband::QiPhotoRubberband(QWidget *parent) :
    QWidget(parent)
{
    this->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    this->hide();
}

void QiPhotoRubberband::setWindowGeometry(QRect g)
{
    wRect=g;
    this->repaint();
}

QRect QiPhotoRubberband::windowGeometry()
{
    return wRect;
}

void QiPhotoRubberband::paintEvent(QPaintEvent* /*e*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setWorldMatrixEnabled(false);
    p.setViewTransformEnabled(false);
    QPainterPath path(QPoint(0,0));
    //QPainterPath p1(QPoint(0,0));
    path.addRect(rect());
    //p1.addRect(wRect);
    //path=path.subtracted(p1);
    if (!wRect.isEmpty()) path.addRect(wRect);
    p.setPen(Qt::NoPen);
    /*
    QLinearGradient lg(0,0,0,height());
    lg.setColorAt(0,QColor(0,0,0,30));
    lg.setColorAt(0.5,QColor(0,0,0,10));
    p.setBrush(lg);
    */
    p.setBrush(QColor(0,0,0,30));
    p.drawPath(path);
    if (!wRect.isEmpty())
    {
        p.setPen(Qt::yellow);
        p.setBrush(Qt::NoBrush);
        p.drawRect(wRect);
    }
}
