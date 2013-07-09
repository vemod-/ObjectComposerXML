#include "qmactreewidget.h"
#include "CommonClasses.h"
#include "mouseevents.h"
#include <QHeaderView>
#include <QScrollBar>

QMacTreeWidget::QMacTreeWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    setWindowFlags((Qt::WindowFlags)(windowFlags()|Qt::WA_OpaquePaintEvent|Qt::WA_PaintOnScreen));
    setAutoFillBackground(true);
    header()->setFixedHeight(tablerowheight);
    setIndentation(12);
    setUniformRowHeights(true);
    setAnimated(false);
    setAttribute(Qt::WA_MacShowFocusRect, 0);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    MouseEvents* ev=new MouseEvents;
    viewport()->installEventFilter(ev);
    connect(ev,SIGNAL(MouseRelease(QMouseEvent*)),this,SLOT(MouseRelease(QMouseEvent*)));
    connect(ev,SIGNAL(MouseMove(QMouseEvent*)),this,SLOT(MouseMove(QMouseEvent*)));
    connect(ev,SIGNAL(MouseEnter(QEvent*)),this,SLOT(MouseEnter(QEvent*)));
    connect(ev,SIGNAL(MouseLeave(QEvent*)),this,SLOT(MouseLeave(QEvent*)));
    m_iconmode=true;
    MouseLeave(0);
}


void QMacTreeWidget::setIconMode(const bool icons)
{
    m_iconmode=icons;
}

const bool QMacTreeWidget::iconMode() const
{
    return m_iconmode;
}

void QMacTreeWidget::MouseRelease(QMouseEvent* event)
{
    if (event->button() & Qt::RightButton)
    {
        emit Popup(this->cursor().pos());
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
        for (int i=columnCount()-1;i>0;i--)
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

void QMacTreeWidget::adjust()
{
    //QTreeWidget::adjustSize();
    resizeEvent(0);
    MouseLeave(0);
}

QTreeWidgetItem* QMacTreeWidget::createTopLevelItem(const QString& text, const QString& iconPath)
{
    QTreeWidgetItem* i=new QTreeWidgetItem;
    i->setText(0,text.toUpper());
    i->setIcon(0,QIcon(iconPath));
    i->setTextColor(0,QColor("#666"));
    i->setSizeHint(0,QSize(-1,tablerowheight));
    i->setFlags(Qt::ItemIsEnabled);
    i->setFirstColumnSpanned(true);
    return i;
}

QTreeWidgetItem* QMacTreeWidget::createStandardItem(const QString& text, const QString& iconPath, const int CloseIcon, const int SaveIcon)
{
    QTreeWidgetItem* i=new QTreeWidgetItem;
    i->setText(0,text);
    if (!iconPath.isEmpty()) i->setIcon(0,QIcon(iconPath));
    i->setTextColor(0,QColor("#222"));
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
