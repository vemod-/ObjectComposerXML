#include "qmactreewidget.h"
//#include "CommonClasses.h"
#include "mouseevents.h"
#include <QHeaderView>
#include <QScrollBar>

QMacTreeWidget::QMacTreeWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    setWindowFlags(Qt::WindowFlags(windowFlags()|Qt::WA_OpaquePaintEvent|Qt::WA_PaintOnScreen));
    setAutoFillBackground(true);
    header()->setFixedHeight(tablerowheight);
    setIndentation(12);
    setUniformRowHeights(true);
    setAnimated(false);
    setAttribute(Qt::WA_MacShowFocusRect, false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto ev=new MouseEvents;
    viewport()->installEventFilter(ev);
    connect(ev,&MouseEvents::MouseReleased,this,&QMacTreeWidget::MouseRelease);
    connect(ev,&MouseEvents::MouseMoved,this,&QMacTreeWidget::MouseMove);
    connect(ev,&MouseEvents::MouseEntered,this,&QMacTreeWidget::MouseEnter);
    connect(ev,&MouseEvents::MouseLeft,this,&QMacTreeWidget::MouseLeave);
    m_iconmode=true;
    MouseLeave(nullptr);
}

void QMacTreeWidget::setIconMode(const bool icons)
{
    m_iconmode=icons;
}

bool QMacTreeWidget::iconMode() const
{
    return m_iconmode;
}

void QMacTreeWidget::MouseRelease(QMouseEvent* event)
{
    if (event->button() & Qt::RightButton)
    {
        emit Popup(QCursor::pos());
    }
}

void QMacTreeWidget::MouseEnter(QEvent *event)
{
    Q_UNUSED(event);
    if (m_iconmode) setMouseTracking(true);
}

void QMacTreeWidget::MouseLeave(QEvent *event)
{
    Q_UNUSED(event);
    if (m_iconmode)
    {
        QTreeWidgetItemIterator it(this);
        while (*it)
        {
            QTreeWidgetItem* item=(*it);
            item->setFirstColumnSpanned(true);
            ++it;
        }
        setMouseTracking(false);
    }
}

void QMacTreeWidget::MouseMove(QMouseEvent *event)
{
    if (m_iconmode)
    {
        setMouseTracking(true);
        QTreeWidgetItemIterator it(this);
        QTreeWidgetItem* i=itemAt(event->pos());
        while (*it)
        {
            QTreeWidgetItem* item=(*it);
            if (i==item)
            {
                for (int j=1;j<columnCount();j++)
                {
                    if (!item->icon(j).isNull())
                    {
                        item->setFirstColumnSpanned(false);
                        break;
                    }
                }
            }
            else
            {
                item->setFirstColumnSpanned(true);
            }
            ++it;
        }
    }
}

void QMacTreeWidget::resizeEvent(QResizeEvent *event)
{
    QTreeWidget::resizeEvent(event);
    if (m_iconmode)
    {
        for (int i = columnCount()-1; i > 0; i--)
        {
            setColumnWidth(i,22);
            //resizeColumnToContents(i);
        }
        int iconWidth=(22*(columnCount()-1))+2;
        if (verticalScrollBar()->isVisible()) iconWidth+=verticalScrollBar()->width();
        setColumnWidth(0,width()-iconWidth);
        //header()->adjustSize();
    }
    else
    {
        resizeColumnToContents(0);
        header()->setStretchLastSection(true);
    }
}

void QMacTreeWidget::dropEvent(QDropEvent *event)
{
    // get the list of the items that are about to be dragged
    QList<QTreeWidgetItem*> dragItems = selectedItems();
    QTreeWidget::dropEvent(event);
    emit itemsMoved(dragItems);
}

void QMacTreeWidget::dragMoveEvent(QDragMoveEvent *event) {
    QTreeWidgetItem* targetItem = itemAt(event->position().toPoint());
    if (!targetItem) {
        event->ignore(); // Blockera drop på root-nivå
    } else {
        QTreeWidget::dragMoveEvent(event); // Annars, tillåt standardbeteendet
    }
}

void QMacTreeWidget::adjust()
{
    //QTreeWidget::adjustSize();
    resizeEvent(nullptr);
    MouseLeave(nullptr);
}

int QMacTreeWidget::visibleRowCount()
{
    int count = 0;
    QTreeWidgetItemIterator iterator(this);
    while (*iterator)
    {
        if ((*iterator)->parent())
        {
            if ((*iterator)->parent()->isExpanded()) count++;
        }
        else
        {
            //root item
            count++;
        }
        ++iterator;
    }
    return count;
}

QSize QMacTreeWidget::contentSize()
{
    int h = visibleRowCount()*tablerowheight;
    int w = 0;
    for (int i = 0; i < columnCount(); i++) w += columnWidth(i);
    return QSize(w + verticalScrollBar()->width(),h + horizontalScrollBar()->width());
}

QTreeWidgetItem* QMacTreeWidget::createTopLevelItem(const QString& text, const QString& iconPath)
{
    auto i=new QTreeWidgetItem;
    i->setText(0,text.toUpper());
    i->setIcon(0,QIcon(iconPath));
    i->setForeground(0,QColor("#666"));
    i->setSizeHint(0,QSize(-1,tablerowheight));
    i->setFlags(Qt::ItemIsEnabled);
    i->setFirstColumnSpanned(true);
    return i;
}

QTreeWidgetItem* QMacTreeWidget::createStandardItem(const QString& text, const QString& iconPath, const int CloseIcon, const int SaveIcon)
{
    auto i=new QTreeWidgetItem;
    i->setText(0,text);
    if (!iconPath.isEmpty()) i->setIcon(0,QIcon(iconPath));
    i->setForeground(0,QColor("#222"));
    if (CloseIcon)
    {
        i->setIcon(CloseIcon,QIcon(":/fileclose.png"));
        i->setSizeHint(CloseIcon,QSize(22,tablerowheight));
    }
    if (SaveIcon)
    {
        i->setIcon(SaveIcon,QIcon(":/filesave.png"));
        i->setSizeHint(SaveIcon,QSize(22,tablerowheight));
    }
    i->setSizeHint(0,QSize(-1,tablerowheight));
    return i;
}
