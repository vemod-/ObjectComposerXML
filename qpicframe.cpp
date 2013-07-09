#include "qpicframe.h"

QPicFrame::QPicFrame(QWidget *parent) : QWidget(parent)
{
    pic=0;
    opc=1;
}

QPicFrame::~QPicFrame()
{
    if (pic !=0 ) delete pic;
}

void QPicFrame::setOpacity(const qreal o)
{
    opc=o;
    if (pic != 0) repaint();
}

const qreal QPicFrame::opacity()
{
    return opc;
}

void QPicFrame::setPicture(QImage *p)
{
    if (pic !=0 ) delete pic;
    pic=p;
}

const QImage* QPicFrame::picture()
{
    return pic;
}

void QPicFrame::paintEvent(QPaintEvent *event)
{
    if (pic==0) return;
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setWorldMatrixEnabled(false);
    p.setViewTransformEnabled(false);
    p.setOpacity(opc);
    QRect r=event->rect();
    p.drawImage(r,*pic,r);
}

void QPicFrame::fill(const QSize size, const float smoke)
{
    if (pic !=0 ) delete pic;
    pic=new QImage(size,QImage::Format_ARGB32_Premultiplied);
    QPainter p(pic);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setWorldMatrixEnabled(false);
    p.setViewTransformEnabled(false);
    p.setPen(Qt::NoPen);
    p.fillRect(0,0,size.width(),size.height(),QColor(0,0,0,255.0*smoke));
}

void QPicFrame::grabWidget(QWidget *w, const float smoke)
{
    grabWidget(w,w->rect(),smoke);
}

void QPicFrame::grabWidget(QWidget *w, const QRect sourceRect, const float smoke)
{
    if (pic !=0 ) delete pic;
    pic=new QImage(sourceRect.size(),QImage::Format_ARGB32_Premultiplied);
    QPainter p(pic);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setWorldMatrixEnabled(false);
    p.setViewTransformEnabled(false);
    w->render(&p,QPoint(0,0),QRegion(sourceRect));
    if (smoke>0)
    {
        p.setBrush(QColor(0,0,0,255.0*smoke));
        p.setPen(Qt::NoPen);
        p.drawRect(QRect(QPoint(0,0),sourceRect.size()));
    }
}

void QPicFrame::load(const QString& path, const QSize size)
{
    if (pic !=0 ) delete pic;
    pic=new QImage(path);
    if (size != QSize(0,0))
    {
        QImage temp=(*pic);
        delete pic;
        pic=new QImage(temp.scaled(size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    }
    this->setFixedSize(pic->size());
}
