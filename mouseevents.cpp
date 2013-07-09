#include "mouseevents.h"
#include <QDebug>

bool MouseEvents::eventFilter(QObject* /*obj*/, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        emit MousePress((QMouseEvent*) event);
    }
    else if (event->type() == QEvent::MouseMove)
    {
        emit MouseMove((QMouseEvent*) event);
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        emit MouseRelease((QMouseEvent*) event);
    }
    else if (event->type() == QEvent::Enter)
    {
        emit MouseEnter(event);
    }
    else if (event->type() == QEvent::Leave)
    {
        emit MouseLeave(event);
    }
    else if (event->type() == QEvent::FocusOut)
    {
        emit LostFocus((QFocusEvent*) event);
    }
    return false;
}

bool InternalMoveEvent::eventFilter(QObject* /*obj*/, QEvent *event)
{
    if (event->type() == QEvent::ChildRemoved) emit ItemMoved();
    if (event->type() == QEvent::ChildAdded) emit DragEnter();
    return false;
}

bool KeyEvents::eventFilter(QObject* /*obj*/, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        emit KeyPress((QKeyEvent*) event);
    }
    else if (event->type() == QEvent::KeyRelease)
    {
        emit KeyRelease((QKeyEvent*) event);
    }
    return false;
}

bool ActivateEvent::eventFilter(QObject */*obj*/, QEvent *event)
{
    if (event->type() == QEvent::ApplicationActivate) {
        // The application deactivation can be handled here
        qDebug() << "Activated";
        emit Activated();
        //return true; // The event is handled
    }
    if (event->type() == QEvent::ApplicationDeactivate) {
        // The application activation can be handled here
        qDebug() << "DeActivated";
        emit Deactivated();
        //return true;
    }
    //return QObject::eventFilter(obj, event); // Unhandled events are passed to the base class
    return false;
}
