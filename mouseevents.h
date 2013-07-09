#ifndef MOUSEEVENTS_H
#define MOUSEEVENTS_H

#include <QObject>
#include <QEvent>
#include <QMouseEvent>

class MouseEvents : public QObject
{
    Q_OBJECT
signals:
    void MousePress(QMouseEvent* event);
    void MouseMove(QMouseEvent* event);
    void MouseRelease(QMouseEvent* event);
    void MouseEnter(QEvent* event);
    void MouseLeave(QEvent* event);
    void LostFocus(QFocusEvent* event);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
private slots:
};

class InternalMoveEvent : public QObject
{
    Q_OBJECT
signals:
    void ItemMoved();
    void DragEnter();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class KeyEvents : public QObject
{
    Q_OBJECT
signals:
    void KeyPress(QKeyEvent* event);
    void KeyRelease(QKeyEvent* event);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
private slots:
};

class ActivateEvent : public QObject
{
    Q_OBJECT
signals:
    void Activated();
    void Deactivated();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MOUSEEVENTS_H
