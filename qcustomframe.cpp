#include "qcustomframe.h"
#include <QPainter>

QCustomFrame::QCustomFrame(QWidget *parent) : QFrame(parent)
{
    this->setWindowFlags((Qt::WindowFlags)(this->windowFlags() | Qt::WindowStaysOnTopHint));
    this->setWindowModality(Qt::WindowModal);
}

void QCustomFrame::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setWorldMatrixEnabled(false);
    p.setViewTransformEnabled(false);
    QLinearGradient lg(0,0,0,height());
    lg.setColorAt(0,QColor(0,0,0,30));
    lg.setColorAt(0.5,QColor(0,0,0,10));
    p.setRenderHint(QPainter::Antialiasing,true);
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(lg));
    p.drawRect(rect());
}
