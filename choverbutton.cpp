#include "choverbutton.h"

CHoverButton::CHoverButton(QWidget *parent) :
    QToolButton(parent)
{
    connect(this,SIGNAL(clicked()),this,SLOT(wasClicked()));
}

void CHoverButton::setIcon(const QIcon &icon)
{
    holdIcon=icon;
    QToolButton::setIcon(QIcon());
}

void CHoverButton::enterEvent(QEvent * event)
{
    QToolButton::setIcon(holdIcon);
}

void CHoverButton::leaveEvent(QEvent * event)
{
    QToolButton::setIcon(QIcon());
}

void CHoverButton::setID(int ID)
{
    holdID=ID;
}

void CHoverButton::wasClicked()
{
    emit clicked(holdID);
}
