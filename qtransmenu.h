#ifndef QTRANSMENU_H
#define QTRANSMENU_H

#include <QMenu>

class QTransMenu : public QMenu
{
    Q_OBJECT
public:
    QTransMenu(QWidget* parent=0);
protected:
    virtual void paintEvent(QPaintEvent *e);
};

#endif // QTRANSMENU_H
