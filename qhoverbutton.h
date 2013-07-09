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
    QHoverButton(QWidget* parent,QIcon icon,QSize size, AnimationDirection ShowAnimation=NoAnimation, AnimationDirection HideAnimation=NoAnimation);
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
    bool Showing;
    bool Hiding;
    QRect maxRect;
    QRect minRect;
    QPropertyAnimation* a1;
    QPropertyAnimation* a2;
    QPropertyAnimation* a3;
    QParallelAnimationGroup* animation;
private slots:
    void timeFinish();
};


#endif // QHOVERBUTTON_H
