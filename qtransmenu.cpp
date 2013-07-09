#include "qtransmenu.h"

QTransMenu::QTransMenu(QWidget *parent) : QMenu(parent)
{
}

void QTransMenu::paintEvent(QPaintEvent *e)
{
    this->setWindowOpacity(0.7);
    QMenu::paintEvent(e);
}
