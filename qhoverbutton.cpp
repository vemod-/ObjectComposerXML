#include "qhoverbutton.h"
#include "mouseevents.h"

void QHoverButton::init()
{
    animation=new QParallelAnimationGroup(this);
    a1=new QPropertyAnimation(this,"minimumSize",this);
    a2=new QPropertyAnimation(this,"maximumSize",this);
    a3=new QPropertyAnimation(this,"pos",this);
    a1->setDuration(500);
    a2->setDuration(500);
    a3->setDuration(500);
    a1->setEasingCurve(QEasingCurve::OutQuint);
    a2->setEasingCurve(QEasingCurve::OutQuint);
    a3->setEasingCurve(QEasingCurve::OutQuint);
    animation->addAnimation(a1);
    animation->addAnimation(a2);
    animation->addAnimation(a3);
    connect(animation,SIGNAL(finished()),this,SLOT(timeFinish()));
    setVisible(false);
    MouseEvents* e=new MouseEvents();
    connect(e,SIGNAL(MouseLeave(QEvent*)),this,SLOT(MouseLeave(QEvent*)));
    this->installEventFilter(e);
    this->setCursor(Qt::PointingHandCursor);
    this->ShowAnimation=NoAnimation;
    this->HideAnimation=NoAnimation;
    Showing=false;
    Hiding=false;
    this->setMouseTracking(true);
}

QHoverButton::QHoverButton(QWidget *parent, QIcon icon, QSize size, AnimationDirection ShowAnimation, AnimationDirection HideAnimation) : QToolButton(parent)
{
    init();
    this->setFixedSize(size);
    maxRect.setSize(size);
    this->setIconSize(size);
    this->setIcon(icon);
    this->ShowAnimation=ShowAnimation;
    this->HideAnimation=HideAnimation;
}

void QHoverButton::Show()
{
    if (Showing) return;
    Hiding=false;
    releaseMouse();
    if (ShowAnimation==NoAnimation)
    {
        move(maxRect.topLeft());
        setVisible(true);
        grabMouse();
        return;
    }
    Showing=true;
    minRect=QRect(maxRect.topLeft(),QSize(0,0));
    switch (ShowAnimation)
    {
    case Left:
        minRect.setHeight(maxRect.height());
        break;
    case LeftTop:
        break;
    case Top:
        minRect.setWidth(maxRect.width());
        break;
    case TopRight:
        minRect.moveLeft(maxRect.x()+maxRect.width());
        break;
    case Right:
        minRect.setHeight(maxRect.height());
        minRect.moveLeft(maxRect.x()+maxRect.width());
        break;
    case RightBottom:
        minRect.moveLeft(maxRect.x()+maxRect.width());
        minRect.moveTop(maxRect.y()+maxRect.height());
        break;
    case Bottom:
        minRect.setWidth(maxRect.width());
        minRect.moveTop(maxRect.y()+maxRect.height());
        break;
    case BottomLeft:
        minRect.moveTop(maxRect.y()+maxRect.height());
        break;
    case NoAnimation:
        break;
    }
    this->move(minRect.topLeft());
    this->setFixedSize(minRect.size());
    this->setVisible(true);
    grabMouse();
    animation->stop();
    a1->setStartValue(minRect.size());
    a1->setEndValue(maxRect.size());
    a2->setStartValue(minRect.size());
    a2->setEndValue(maxRect.size());
    a3->setStartValue(minRect.topLeft());
    a3->setEndValue(maxRect.topLeft());
    animation->start();
}

void QHoverButton::timeFinish()
{
    if (Showing)
    {
        move(maxRect.topLeft());
        setFixedSize(maxRect.size());
        Showing=false;
    }
    if (Hiding)
    {
        releaseMouse();
        setVisible(false);
        move(maxRect.topLeft());
        setFixedSize(maxRect.size());
        Hiding=false;
    }
}

void QHoverButton::Hide()
{
    if (Hiding) return;
    Showing=false;
    if (ShowAnimation==NoAnimation)
    {
        releaseMouse();
        setVisible(false);
        return;
    }
    Hiding=true;
    QRect currentRect=QRect(this->geometry());
    animation->stop();
    a1->setStartValue(currentRect.size());
    a1->setEndValue(minRect.size());
    a2->setStartValue(currentRect.size());
    a2->setEndValue(minRect.size());
    a3->setStartValue(currentRect.topLeft());
    a3->setEndValue(minRect.topLeft());
    animation->start();
}

void QHoverButton::MouseLeave(QEvent *event)
{
    Q_UNUSED(event);
    Hide();
}

void QHoverButton::mouseMoveEvent(QMouseEvent* event)
{
    if (this->rect().contains(event->pos()))
    {
        return;
    }
    if (maxRect.contains(this->mapToParent(event->pos())))
    {
        return;
    }
    Hide();
}

void QHoverButton::Activate(QPointF pos, QPointF mousePos, QRect area)
{
    if ((!isVisible()) && (!Hiding))
    {
        if (area.contains(mousePos.toPoint()))
        {
            maxRect.moveTo(pos.toPoint());
            Show();
        }
    }
}
