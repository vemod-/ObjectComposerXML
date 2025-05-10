#ifndef QHOVERBUTTON_H
#define QHOVERBUTTON_H

#include <QToolButton>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QMouseEvent>

class QHoverButton : public QToolButton
{
    Q_OBJECT
public:
    enum AnimationDirection
    {
        NoAnimation,
        Left,
        LeftTop,
        Top,
        TopRight,
        Right,
        RightBottom,
        Bottom,
        BottomLeft
    };
    QHoverButton(QWidget* parent, const QIcon& icon, const QSize& size, AnimationDirection ShowAnimation=NoAnimation, AnimationDirection HideAnimation=NoAnimation);
    void Activate(QPointF pos, QPointF mousePos, QRect area);
public slots:
    void MouseLeave(QEvent* event);
protected:
    void mouseMoveEvent(QMouseEvent* event);
private:
    void init();
    AnimationDirection ShowAnimation;
    AnimationDirection HideAnimation;
    void Show();
    void Hide();
    bool Showing=false;
    bool Hiding=false;
    QRect maxRect;
    QRect minRect;
    QPropertyAnimation* a1=nullptr;
    QPropertyAnimation* a2=nullptr;
    QPropertyAnimation* a3=nullptr;
    QParallelAnimationGroup* animation=nullptr;
private slots:
    void timeFinish();
};


#endif // QHOVERBUTTON_H
