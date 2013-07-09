#ifndef QSLIDINGFRAME_H
#define QSLIDINGFRAME_H
#include <QWidget>
#include <QFrame>
#include <QPixmap>
#include <QPropertyAnimation>
#include "qpicframe.h"

class QSlidingFrame : public QWidget
{
    Q_OBJECT
public:
    explicit QSlidingFrame(QWidget* parent=0);
    void AddWidget(QWidget* w);
    QList<QWidget*> Widgets;
    int activeWidget();
    void setOrientation(Qt::Orientation o);
    Qt::Orientation orientation();
public slots:
    void setActiveWidget(int w);
    void nextWidget();
    void previousWidget();
private:
    QWidget* LayoutWidget;
    //QGraphicsView* ImgWidget;
    QPicFrame* ImgWidget;
    Qt::Orientation m_Orientation;
    QPropertyAnimation* animation;
    int m_ActiveWidget;
private slots:
    void animationFinished();
};

#endif // QSLIDINGFRAME_H
