#include "qfadingframe.h"

QFadingFrame::QFadingFrame(QWidget *parent) :
    QFrame(parent)
{
    timeLine=new QTimeLine(800,this);
    timeLine->setFrameRange(0,100);
    connect(timeLine,&QTimeLine::frameChanged,this,&QFadingFrame::processing);
    connect(timeLine,&QTimeLine::finished,this,&QFadingFrame::finished);
    backPic=nullptr;
    state=FadingFinished;
}

void QFadingFrame::show()
{
    if (timeLine->state()==QTimeLine::Running)
    {
        finished();
    }
    if (isVisible())
    {
        setVisible(true);
        return;
    }
    state=FadingIn;
    backPic=new QPicFrame(this);
    backPic->grabWidget(parentWidget(),this->geometry());
    backPic->setGeometry(this->rect());
    backPic->raise();
    backPic->show();
    this->setVisible(true);
    timeLine->start();
}

void QFadingFrame::hide()
{
    if (timeLine->state()==QTimeLine::Running)
    {
        finished();
    }
    if (!isVisible())
    {
        setVisible(false);
        return;
    }
    state=FadingOut;
    this->setVisible(false);
    backPic=new QPicFrame(this);
    backPic->grabWidget(parentWidget(),this->geometry());
    this->setVisible(true);
    backPic->setGeometry(this->rect());
    backPic->setOpacity(0);
    backPic->raise();
    backPic->show();
    timeLine->start();
}

void QFadingFrame::processing(int count)
{
    this->setVisible(false);
    backPic->grabWidget(parentWidget(),this->geometry());
    this->setVisible(true);
    if (state==FadingOut)
    {
        backPic->setOpacity(count*0.01);
    }
    else
    {
        backPic->setOpacity(1.0-(count*0.01));
    }
    backPic->repaint();
}

void QFadingFrame::finished()
{
    timeLine->stop();
    if (state==FadingOut)
    {
        delete backPic;
        this->setVisible(false);
    }
    else
    {
        delete backPic;
    }
    state=FadingFinished;
}
