#include "qmacrubberband.h"
#include <QPainter>
#include <QApplication>

QMacRubberband::QMacRubberband(QRubberBand::Shape shape, MacRubberbandTypes Type, QWidget *parent) : QRubberBand(shape,parent)
{
    m_type=Type;
    animation=new QPropertyAnimation(this,"geometry",this);
    animation->setEasingCurve(QEasingCurve::SineCurve);
    animation->setDuration(150);
}

void QMacRubberband::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setWorldMatrixEnabled(false);
    p.setViewTransformEnabled(false);

    QRect r(event->rect());
    p.setBrush(QColor(100,100,0,15));
    if (m_type==QMacRubberband::MacRubberbandYellow)
    {
        QColor c("#d8f100");
        c.setAlpha(220);
        //QColor c(200,200,0,200);
        p.setPen(QPen(c,5,Qt::SolidLine,Qt::SquareCap,Qt::RoundJoin));
        p.setBrush(QColor(0,0,0,30));
        p.setPen(Qt::yellow);
        p.drawRoundedRect(r.adjusted(0,0,-1,-1),5,5);
    }
    else
    {
        p.setPen(QColor(200,200,0,200));
        p.drawRoundedRect(r.adjusted(0,0,-1,-1),2,2);
    }
}

void QMacRubberband::showAnimated()
{
    show();
    QRect r=geometry();
    animation->setStartValue(r);
    animation->setEndValue(r.adjusted(-10,-10,10,10));
    animation->start();
}

void QMacRubberband::hideAnimated()
{
    while (animation->state()==QPropertyAnimation::Running)
    {
        QApplication::processEvents();
    }
    hide();
}

QHoverRubberband::QHoverRubberband(QRubberBand::Shape shape, QWidget *parent) : QRubberBand(shape,parent)
{
    animation=new QTimeLine(400,this);
    //animation->setEasingCurve(QEasingCurve::OutBack);
    connect(animation,&QTimeLine::frameChanged,this,&QHoverRubberband::TimeStep);
    connect(animation,&QTimeLine::finished,this,&QHoverRubberband::TimeFinished);
    m_Opacity=0;
}

void QHoverRubberband::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setWorldMatrixEnabled(false);
    p.setViewTransformEnabled(false);

    QRect r(event->rect());
    p.setBrush(QColor(0,0,0,m_DefaultOpacity));
    p.setPen(Qt::NoPen);
    p.setOpacity(m_Opacity);
    p.drawRoundedRect(r,5,5);
}

void QHoverRubberband::TimeStep(int Step)
{
    m_Opacity=Step*0.01;
    repaint();
}

void QHoverRubberband::TimeFinished()
{
    if (m_Hiding)
    {
        setVisible(false);
    }
    else
    {
        m_Opacity=1;
        repaint();
    }
}

void QHoverRubberband::show(int Opacity)
{
    if (!isVisible())
    {
        if (animation->state()==QTimeLine::Running) animation->stop();
        m_DefaultOpacity=Opacity;
        m_Opacity=0;
        animation->setDuration(400);
        animation->setFrameRange(0,100);
        m_Hiding=false;
        animation->start();
        setVisible(true);
    }
}
/*
void QHoverRubberband::show()
{
    m_DefaultOpacity=40;
    m_Opacity=1;
    setVisible(true);
}
*/
void QHoverRubberband::hide()
{
    if (isVisible())
    {
        setVisible(false);
        /*
        if (animation->state()==QTimeLine::Running) animation->stop();
        //m_Opacity=0;
        animation->setDuration(100);
        animation->setFrameRange(m_Opacity*100,0);
        m_Hiding=true;
        animation->start();
        */
    }
}
