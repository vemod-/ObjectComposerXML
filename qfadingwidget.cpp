#include "qfadingwidget.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QApplication>

QFadingWidget::QFadingWidget(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_StaticContents);
    //setAttribute(Qt::WA_NoSystemBackground);
    transitionType=QFadingWidget::Fade;
    QHBoxLayout* l=new QHBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);
    this->setLayout(l);
    Widget=0;
    oldImage=0;
    newImage=0;
    smoke=0;
    fadefactor=0;
    progressCount=0;
    timeline=new QTimeLine(400,this);
    timeline->setDuration(400);
    timeline->setEasingCurve(QEasingCurve::OutCurve);
    timeline->setUpdateInterval(4);
    connect(timeline,SIGNAL(frameChanged(int)),this,SLOT(progress(int)));
    connect(timeline,SIGNAL(finished()),this,SLOT(finish()));
}

void QFadingWidget::setWidget(QWidget *w)
{
    if (Widget != 0)
    {
        this->layout()->removeWidget(Widget);
    }
    Widget=w;
    this->layout()->addWidget(w);
}

void QFadingWidget::setTransitionType(TransistionType t)
{
    transitionType=t;
}

void QFadingWidget::setEasingCurve(QEasingCurve e)
{
    timeline->setEasingCurve(e);
}

void QFadingWidget::setDuration(int d)
{
    timeline->setDuration(d);
}

void QFadingWidget::progress(int count)
{
    progressCount=count;
    float f=(float)count/(float)timeline->duration();
    float w=WRect.width();
    oldRect=WRect;
    newRect=WRect;
    switch (transitionType)
    {
    case QFadingWidget::Fade:
    case QFadingWidget::DarkFade:
        fadefactor=f;
        break;
    case QFadingWidget::SlideRightToLeft:
        newRect=WRect.translated(w*(1.0-f),0);
        break;
    case QFadingWidget::SlideLeftToRight:
        newRect=WRect.translated(-w*(1.0-f),0);
        break;
    case QFadingWidget::PagingForward:
        oldRect=WRect.translated(-w*f,0);
        break;
    case QFadingWidget::PagingBack:
        newRect=WRect.translated(-w*(1.0-f),0);
        break;
    case QFadingWidget::FadingForward:
        oldRect=WRect.translated(-w*f,0);
        smoke=(1.0-f)/8;
        break;
    case QFadingWidget::FadingBack:
        newRect=WRect.translated(-w*(1.0-f),0);
        smoke=f/8;
        break;
    }
    repaint();
}

void QFadingWidget::finish()
{
    if (transitionType==QFadingWidget::DarkFade)
    {
        delete progressWheel;
    }
    Widget->show();
    timeline->stop();
    Widget->setUpdatesEnabled(true);
    delete oldImage;
    delete newImage;
    oldImage=0;
    newImage=0;
    smoke=0;
    fadefactor=0;
}

void QFadingWidget::prepare()
{
    progressCount=0;
    if (timeline->state()==QTimeLine::Running)
    {
        finish();
    }
    if (!this->isVisible()) return;
    if (Widget == 0) return;
    oldImage=new QCanvasLayer;
    if (transitionType==QFadingWidget::DarkFade)
    {
        oldImage->grabWidget(Widget,0.4);
    }
    else
    {
        oldImage->grabWidget(Widget);
    }
    Widget->hide();
    if (transitionType==QFadingWidget::DarkFade)
    {
        progressWheel= new QMacProgressIndicator(this,":/aqua_icon_gray.png",QSize(150,150));
    }
    smoke=0;
    fadefactor=0;
}

void QFadingWidget::fade()
{
    progressCount=0;
    if (Widget == 0) return;
    if (oldImage == 0) return;
    newImage=new QCanvasLayer;
    Widget->show();
    newImage->grabWidget(Widget);
    Widget->hide();
    WRect=QRect(0,0,Widget->width(),Widget->height());
    if (transitionType==QFadingWidget::DarkFade)
    {
        progressWheel->raise();
    }
    timeline->setFrameRange(0,timeline->duration());
    timeline->start();
}

void QFadingWidget::paintEvent(QPaintEvent *event)
{
    if (oldImage==0) return;
    QPainter P(this);
    //P.fillRect(WRect,Qt::lightGray);
    P.setRenderHint(QPainter::SmoothPixmapTransform);
    P.setWorldMatrixEnabled(false);
    P.setViewTransformEnabled(false);
    if (progressCount==0)
    {
        QRect r=event->rect();
        P.drawImage(r,oldImage->BackImage,r);
        return;
    }
    QRectF oldIntersect=oldRect.intersected(WRect);
    QRectF newIntersect=newRect.intersected(WRect);
    QRectF oldInterTranslated=oldIntersect.translated(newRect.right(),0);
    QRectF newInterTranslated=newIntersect.translated(oldRect.right(),0);
    switch (transitionType)
    {
    case QFadingWidget::SlideRightToLeft:
        P.drawImage(oldIntersect,oldImage->BackImage,oldIntersect.translated(-oldRect.left(),0));
        P.drawImage(newInterTranslated,newImage->BackImage,newIntersect);
        break;
    case QFadingWidget::SlideLeftToRight:
        P.drawImage(oldInterTranslated,oldImage->BackImage,oldIntersect);
        P.drawImage(newIntersect,newImage->BackImage,newIntersect.translated(-newRect.left(),0));
        break;
    case QFadingWidget::PagingBack:
        P.drawImage(oldInterTranslated,oldImage->BackImage,oldInterTranslated);
        P.drawImage(newIntersect,newImage->BackImage,newIntersect.translated(-newRect.left(),0));
        break;
    case QFadingWidget::PagingForward:
        P.drawImage(newInterTranslated,newImage->BackImage,newInterTranslated);
        P.drawImage(oldIntersect,oldImage->BackImage,oldIntersect.translated(-oldRect.left(),0));
        break;
    case QFadingWidget::FadingForward:
        P.drawImage(newInterTranslated,newImage->BackImage,newInterTranslated);
        if (smoke)
        {
            P.setOpacity(smoke);
            P.fillRect(newInterTranslated,Qt::black);
            P.setOpacity(1);
        }
        P.drawImage(oldRect.intersected(WRect),oldImage->BackImage,WRect);
        break;
    case QFadingWidget::FadingBack:
        P.drawImage(oldInterTranslated,oldImage->BackImage,oldInterTranslated);
        if (smoke)
        {
            P.setOpacity(smoke);
            P.fillRect(oldInterTranslated,Qt::black);
            P.setOpacity(1);
        }
        P.drawImage(newRect.intersected(WRect),newImage->BackImage,WRect);
        break;
    default:
        QRect r=event->rect();
        P.drawImage(r,oldImage->BackImage,r);
        if (fadefactor)
        {
            P.setOpacity(fadefactor);
            P.drawImage(r,newImage->BackImage,r);
        }
        break;
    }
}
