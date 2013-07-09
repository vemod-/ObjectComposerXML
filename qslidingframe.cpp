#include "qslidingframe.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>

QSlidingFrame::QSlidingFrame(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);
    LayoutWidget=new QWidget(this);
    setOrientation(Qt::Horizontal);
    ImgWidget=0;
    animation=new QPropertyAnimation(this);
    animation->setDuration(800);
    animation->setEasingCurve(QEasingCurve::OutQuint);
    animation->setPropertyName("pos");
    connect(animation,SIGNAL(finished()),this,SLOT(animationFinished()));
    m_ActiveWidget=-1;
    QHBoxLayout* l=new QHBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);
    l->addWidget(LayoutWidget);
    this->setLayout(l);
    LayoutWidget->show();
}

void QSlidingFrame::AddWidget(QWidget *w)
{
    Widgets.append(w);
    setOrientation(m_Orientation);
}

int QSlidingFrame::activeWidget()
{
    return m_ActiveWidget;
}

void QSlidingFrame::nextWidget()
{
    int w=m_ActiveWidget;
    w++;
    if (w>=Widgets.count()) w=0;
    setActiveWidget(w);
}

void QSlidingFrame::previousWidget()
{
    int w=m_ActiveWidget;
    w--;
    if (w<0) w=Widgets.count()-1;
    setActiveWidget(w);
}

void QSlidingFrame::setActiveWidget(int w)
{
    animation->stop();
    animationFinished();
    /*
    if (ImgWidget != 0)
    {
        delete ImgWidget;
        ImgWidget=0;
    }
    */
    if (w>=Widgets.count()) w=Widgets.count()-1;
    if (w==-1) return;
    if ((m_ActiveWidget==-1) || (!this->isVisible()))
    {
        m_ActiveWidget=w;
        Widgets[m_ActiveWidget]->show();
        for (int i=0;i<Widgets.count();i++)
        {
            if (i!=m_ActiveWidget) Widgets[i]->hide();
        }
        return;
    }
    int diff=w-m_ActiveWidget;
    int diffSign=1;
    if (diff<0)
    {
        diff=-diff;
        diffSign=-1;
    }
    if (diff==0)
    {
        Widgets[m_ActiveWidget]->show();
        return;
    }
    if (animation->state()==QPropertyAnimation::Running)
    {
        animation->stop();
    }
    this->layout()->removeWidget(LayoutWidget);
    if (m_Orientation==Qt::Horizontal)
    {
        LayoutWidget->setGeometry(0,0,this->width()*(diff+1),this->height());
        if (diffSign==1)
        {
            for (int i=m_ActiveWidget;i<=w;i++)
            {
                Widgets[i]->setVisible(true);
                Widgets[i]->setGeometry(this->width()*i,0,this->width(),this->height());
            }
            QPoint startPoint(LayoutWidget->pos());
            QPoint endPoint(startPoint);
            endPoint.setX(startPoint.x()+(-diff*this->width()));
            animation->setStartValue(startPoint);
            animation->setEndValue(endPoint);
        }
        else
        {
            for (int i=w;i<=m_ActiveWidget;i++)
            {
                Widgets[i]->setVisible(true);
                Widgets[i]->setGeometry(this->width()*i,0,this->width(),this->height());
            }
            QPoint startPoint(LayoutWidget->pos());
            QPoint endPoint(startPoint);
            startPoint.setX(startPoint.x()+(-diff*this->width()));
            animation->setStartValue(startPoint);
            animation->setEndValue(endPoint);
        }
    }
    else
    {
        LayoutWidget->setGeometry(0,0,this->width(),this->height()*(diff+1));
        if (diffSign==1)
        {
            for (int i=m_ActiveWidget;i<=w;i++)
            {
                Widgets[i]->setVisible(true);
                Widgets[i]->setGeometry(0,this->height()*i,this->width(),this->height());
            }
            QPoint startPoint(LayoutWidget->pos());
            QPoint endPoint(startPoint);
            endPoint.setY(startPoint.y()+(-diff*this->height()));
            animation->setStartValue(startPoint);
            animation->setEndValue(endPoint);
        }
        else
        {
            for (int i=w;i<=m_ActiveWidget;i++)
            {
                Widgets[i]->setVisible(true);
                Widgets[i]->setGeometry(0,this->height()*i,this->width(),this->height());
            }
            QPoint startPoint(LayoutWidget->pos());
            QPoint endPoint(startPoint);
            startPoint.setY(startPoint.y()+(-diff*this->height()));
            animation->setStartValue(startPoint);
            animation->setEndValue(endPoint);
        }
    }
    ImgWidget=new QPicFrame(this);
    ImgWidget->setOpacity(1);
    animation->setTargetObject(ImgWidget);
    ImgWidget->setAutoFillBackground(true);
    ImgWidget->setGeometry(LayoutWidget->geometry());
    ImgWidget->show();
    ImgWidget->grabWidget(LayoutWidget);
    LayoutWidget->hide();
    for (int i=0;i<Widgets.count();i++)
    {
        if (i!=w) Widgets[i]->hide();
    }
    this->layout()->addWidget(LayoutWidget);
    m_ActiveWidget=w;
    animation->start();
}

void QSlidingFrame::animationFinished()
{
    LayoutWidget->show();
    if (ImgWidget != 0) delete ImgWidget;
    ImgWidget=0;
}

void QSlidingFrame::setOrientation(Qt::Orientation o)
{
    delete LayoutWidget->layout();
    m_Orientation=o;
    if (o==Qt::Horizontal)
    {
        QHBoxLayout* l=new QHBoxLayout(LayoutWidget);
        l->setMargin(0);
        l->setSpacing(0);
        foreach (QWidget* w,Widgets)
        {
            w->hide();
            l->addWidget(w);
        }
        LayoutWidget->setLayout(l);
    }
    else
    {
        QVBoxLayout* l=new QVBoxLayout(LayoutWidget);
        l->setMargin(0);
        l->setSpacing(0);
        foreach (QWidget* w,Widgets)
        {
            w->hide();
            l->addWidget(w);
        }
        LayoutWidget->setLayout(l);
    }
}
