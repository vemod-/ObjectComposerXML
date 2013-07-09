#include "cocoaprogressbar.h"
#include <Cocoa/Cocoa.h>
#include <QMacCocoaViewContainer>
#include <QApplication>
#include <QDebug>

#define ProgressIndicatorSize 32

QMacProgressIndicator::QMacProgressIndicator(QWidget *parent,QSize Size) : QPicFrame(parent)
{
    if (Size==QSize(0,0)) Size=QSize(ProgressIndicatorSize,ProgressIndicatorSize);
    setFixedSize(Size);
    init();
}

QMacProgressIndicator::QMacProgressIndicator(QWidget *parent, QString path, QSize Size) : QPicFrame(parent)
{
    load(path,Size);
    init();
}

void QMacProgressIndicator::init()
{
    QRect r((parentWidget()->width()-width())/2,(parentWidget()->height()-height())/2,width(),height());
    setGeometry(r.normalized());
    show();
    repaint();
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    QMacCocoaViewContainer* cv=new QMacCocoaViewContainer(0,this);

    cv->setGeometry(QRect((width()-ProgressIndicatorSize)/2,(height()-ProgressIndicatorSize)/2,ProgressIndicatorSize,ProgressIndicatorSize).normalized());
    NSProgressIndicator* pb = [[NSProgressIndicator alloc] initWithFrame : NSMakeRect(0, 0, ProgressIndicatorSize, ProgressIndicatorSize)];
    cv->setCocoaView(pb);
    [pb setUsesThreadedAnimation:YES];
    [pb setStyle:NSProgressIndicatorSpinningStyle];
    [pb setIndeterminate:YES];
    [pb setControlTint:NSClearControlTint];
    [pb setControlSize:NSRegularControlSize];
    [pb startAnimation:0];
}

void QMacProgressIndicator::paintEvent(QPaintEvent *e)
{
    if (pic != 0)
    {
        QPicFrame::paintEvent(e);
    }
    else
    {
        QPainter p(this);
        p.setRenderHint(QPainter::SmoothPixmapTransform);
        p.setWorldMatrixEnabled(false);
        p.setViewTransformEnabled(false);
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::black);
        p.setOpacity(0.6);
        p.setRenderHint(QPainter::Antialiasing,true);
        p.drawRoundedRect(rect(),rect().width()/10,rect().height()/10);
    }
}

