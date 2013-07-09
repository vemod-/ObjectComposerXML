#include "qmacsplitter.h"
#include <QPainter>
#include <QSettings>

QMacSplitterHandle::QMacSplitterHandle(Qt::Orientation orientation, QSplitter *parent)
: QSplitterHandle(orientation, parent)
{
    setAutoFillBackground(true);
}

// Paint the horizontal handle as a gradient, paint
// the vertical handle as a line.
void QMacSplitterHandle::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setWorldMatrixEnabled(false);
    painter.setViewTransformEnabled(false);

    QColor topColor(145, 145, 145);
    QColor bottomColor(142, 142, 142);
    QColor gradientStart(252, 252, 252);
    QColor gradientStop(223, 223, 223);

    if (orientation() == Qt::Vertical) {
        painter.setPen(topColor);
        painter.drawLine(0, 0, width(), 0);
        painter.setPen(bottomColor);
        painter.drawLine(0, height() - 1, width(), height() - 1);

        QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, height() -3));
        linearGrad.setColorAt(0, gradientStart);
        linearGrad.setColorAt(1, gradientStop);
        painter.fillRect(QRect(QPoint(0,1), size() - QSize(0, 2)), QBrush(linearGrad));
    } else {
        painter.setPen(topColor);
        painter.drawLine(0, 0, 0, height());
    }
}

QSize QMacSplitterHandle::sizeHint() const
{
    QSize parent = QSplitterHandle::sizeHint();
    if (orientation() == Qt::Vertical) {
        return parent + QSize(0, 3);
    } else {
        return QSize(1, parent.height());
    }
}

QSplitterHandle *QMacSplitter::createHandle()
{
    return new QMacSplitterHandle(orientation(), this);
}

QMacSplitter::QMacSplitter(QWidget* parent) : QSplitter(parent)
{
    init();
}

QMacSplitter::QMacSplitter(Qt::Orientation o,QWidget* parent) : QSplitter(o,parent)
{
    init();
}

void QMacSplitter::init()
{
    setChildrenCollapsible(false);
    setAutoFillBackground(true);
    anim=new QParallelAnimationGroup(this);
    a1=new QPropertyAnimation(this);
    a2=new QPropertyAnimation(this);
    anim->addAnimation(a1);
    anim->addAnimation(a2);
    connect (anim,SIGNAL(finished()),this,SLOT(animationFinished()));
    a1->setDuration(400);
    a1->setEasingCurve(QEasingCurve::Linear);
    a2->setDuration(400);
    a2->setEasingCurve(QEasingCurve::Linear);
    expandedSize=150;
    collapseIndex=-1;
    expandIndex=-1;
}

void QMacSplitter::Load(QString Tag)
{
    QSettings s;
    QByteArray ba=s.value(Tag+"/state").toByteArray();
    if (!ba.isEmpty())
    {
        this->restoreState(ba);
        collapseIndex=s.value(Tag+"/index").toInt();
        expandIndex=s.value(Tag+"/expandindex").toInt();
        expandedMaxSize=s.value(Tag+"/maxsize").toSize();
        expandedMinSize=s.value(Tag+"/minsize").toSize();
        expandedSize=s.value(Tag+"/size").toInt();
        if (collapseIndex>-1) widget(collapseIndex)->setMaximumSize(0,0);
        setChildrenCollapsible(false);
    }
}

void QMacSplitter::Save(QString Tag)
{
    //setChildrenCollapsible(true);
    QSettings s;
    s.setValue(Tag+"/state",this->saveState());
    s.setValue(Tag+"/index",collapseIndex);
    s.setValue(Tag+"/expandindex",expandIndex);
    s.setValue(Tag+"/maxsize",expandedMaxSize);
    s.setValue(Tag+"/minsize",expandedMinSize);
    s.setValue(Tag+"/size",expandedSize);
}

void QMacSplitter::collapse(int index)
{
    if (isCollapsed()) expand();
    collapseIndex=index;
    expandIndex=-1;
    expandedMaxSize=widget(index)->maximumSize();
    expandedMinSize=widget(index)->minimumSize();
    if (!isVisible())
    {
        if (this->handle(0)->orientation()==Qt::Horizontal)
        {
            expandedSize=widget(index)->width();
            widget(index)->setMaximumWidth(0);
            widget(index)->setMinimumWidth(0);
        }
        else
        {
            expandedSize=widget(index)->height();
            widget(index)->setMaximumHeight(0);
            widget(index)->setMinimumHeight(0);
        }
        return;
    }
    a1->setTargetObject(widget(index));
    a2->setTargetObject(widget(index));
    if (this->handle(0)->orientation()==Qt::Horizontal)
    {
        expandedSize=widget(index)->width();
        a1->setPropertyName("maximumWidth");
        a1->setStartValue(widget(index)->width());
        a1->setEndValue(0);
        a2->setPropertyName("minimumWidth");
        a2->setStartValue(widget(index)->width());
        a2->setEndValue(0);
    }
    else
    {
        expandedSize=widget(index)->height();
        a1->setPropertyName("maximumHeight");
        a1->setStartValue(widget(index)->height());
        a1->setEndValue(0);
        a2->setPropertyName("minimumHeight");
        a2->setStartValue(widget(index)->height());
        a2->setEndValue(0);
    }
    anim->start();
}

void QMacSplitter::expand()
{
    if (!isCollapsed()) return;
    expandIndex=collapseIndex;
    collapseIndex=-1;
    if (!isVisible())
    {
        if (this->handle(0)->orientation()==Qt::Horizontal)
        {
            widget(expandIndex)->setMinimumWidth(expandedSize);
            widget(expandIndex)->setMaximumWidth(expandedSize);
        }
        else
        {
            widget(expandIndex)->setMinimumHeight(expandedSize);
            widget(expandIndex)->setMaximumHeight(expandedSize);
        }
        widget(expandIndex)->setMaximumSize(expandedMaxSize);
        widget(expandIndex)->setMinimumSize(expandedMinSize);
        expandIndex=-1;
        return;
    }
    a1->setTargetObject(widget(expandIndex));
    a2->setTargetObject(widget(expandIndex));
    if (this->handle(0)->orientation()==Qt::Horizontal)
    {
        a1->setPropertyName("maximumWidth");
        a1->setStartValue(widget(expandIndex)->width());
        a1->setEndValue(expandedSize);
        a2->setPropertyName("minimumWidth");
        a2->setStartValue(widget(expandIndex)->width());
        a2->setEndValue(expandedSize);
    }
    else
    {
        a1->setPropertyName("maximumHeight");
        a1->setStartValue(widget(expandIndex)->height());
        a1->setEndValue(expandedSize);
        a2->setPropertyName("minimumHeight");
        a2->setStartValue(widget(expandIndex)->height());
        a2->setEndValue(expandedSize);
    }
    anim->start();
}

void QMacSplitter::animationFinished()
{
    if (expandIndex>-1)
    {
        widget(expandIndex)->setMaximumSize(expandedMaxSize);
        widget(expandIndex)->setMinimumSize(expandedMinSize);
        expandIndex=-1;
    }
}

bool QMacSplitter::isCollapsed()
{
    return (collapseIndex>-1);
}
