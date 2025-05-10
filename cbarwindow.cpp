#include "cbarwindow.h"
#include "ui_cbarwindow.h"
#include "qmactreewidget.h"

CBarWindow::CBarWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CBarWindow)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    table=new QTableWidget(0,0,this);
    table->setAutoFillBackground(true);
    table->setEditTriggers(QFlag(0));
    table->setSelectionMode(QAbstractItemView::ContiguousSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectItems);
    table->setShowGrid(false);
    table->setIconSize(QSize(25,18));
    table->horizontalHeader()->setFixedHeight(tablerowheight);

    auto layout=new QGridLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setVerticalSpacing(0);
    layout->addWidget(table,0,0);
    setLayout(layout);
    connect(table,&QTableWidget::cellDoubleClicked,this,&CBarWindow::Edit);
    connect(table->verticalHeader(),&QHeaderView::sectionDoubleClicked,this,&CBarWindow::SelectVoice);
    connect(table,&QTableWidget::itemSelectionChanged,this,&CBarWindow::Select);
    table->viewport()->installEventFilter(this);
}

CBarWindow::~CBarWindow()
{
    table->clear();
    delete table;
    delete ui;
}

bool CBarWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel)
    {
        table->viewport()->repaint();
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        auto e = static_cast<QMouseEvent*>(event);
        if (e->button() & Qt::RightButton)
        {
            emit Popup(QCursor::pos());
        }
        if (!table->selectedRanges().empty())
        {
            QTableWidgetSelectionRange r=table->selectedRanges().at(0);
            if ((r.columnCount() > 1) || (r.rowCount() > 1)) emit Popup(QCursor::pos());
        }
    }
    return QObject::eventFilter(obj, event);
}

void CBarWindow::Select()
{
    if (!table->selectedRanges().empty())
    {
        QTableWidgetSelectionRange r=table->selectedRanges().at(0);
        QRect s(r.leftColumn(),r.topRow(),r.columnCount(),r.rowCount());
        emit SelectionChanged(s);
    }
}

void CBarWindow::SelectAll()
{
    table->selectAll();
    Select();
}

void CBarWindow::SelectToEnd()
{
    if (!table->selectedRanges().empty())
    {
        QTableWidgetSelectionRange r=table->selectedRanges().at(0);
        QTableWidgetSelectionRange r1(r.topRow(),r.leftColumn(),r.bottomRow(),table->columnCount()-1);
        table->setRangeSelected(r1,true);
        Select();
    }
}

void CBarWindow::SelectFromStart()
{
    if (!table->selectedRanges().empty())
    {
        QTableWidgetSelectionRange r=table->selectedRanges().at(0);
        QTableWidgetSelectionRange r1(r.topRow(),0,r.bottomRow(),r.rightColumn());
        table->setRangeSelected(r1,true);
        Select();
    }
}

void CBarWindow::Edit(int Row, int Col)
{
    emit BarChanged(Col,BarMap.StaffNum(Row),BarMap.VoiceNum(Row));
    table->clearSelection();
    table->item(Row,Col)->setSelected(true);
    static_cast<QWidget*>(parent())->hide();
}

void CBarWindow::SelectVoice(int Row)
{
    emit BarChanged(0,BarMap.StaffNum(Row),BarMap.VoiceNum(Row));
    table->clearSelection();
    table->selectRow(Row);
    static_cast<QWidget*>(parent())->hide();
}

QSize CBarWindow::contentSize()
{
    int w = table->verticalHeader()->width() + (table->frameWidth()*2); // +4 seems to be needed
    for (int i = 0; i < table->columnCount(); i++)
        w += table->columnWidth(i); // seems to include gridline (on my machine)
    int h = table->horizontalHeader()->height() + (table->frameWidth()*2);
    for (int i = 0; i < table->rowCount(); i++)
        h += table->rowHeight(i);
    return QSize(w, h);
}

void CBarWindow::Fill(OCBarMap& barmap, const OCBarLocation& BarVoiceLocation, const int BarCount)
{
    BarMap=barmap;
    table->blockSignals(true);
    table->setUpdatesEnabled(false);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->hide();
    table->clear();
    if (barmap.isEmpty())
    {
        table->setRowCount(0);
        table->setColumnCount(0);
    }
    else
    {
        table->setColumnCount(barmap.BarCountAll()+1);
        table->setRowCount(barmap.voiceCount());
        QStringList hh;
        for (int i=0;i<table->columnCount()-1;i++)
        {
            hh.append(QString::number(i+1));
            table->setColumnWidth(i,30);
        }
        hh.append(QString());
        table->setHorizontalHeaderLabels(hh);
        QStringList vh;
        for (int i=0;i<barmap.voiceCount();i++)
        {
            vh.append(barmap.voiceCaption(i));
            table->setRowHeight(i,tablerowheight);
        }
        table->setVerticalHeaderLabels(vh);
        for (int i=0;i<table->columnCount();i++)
        {
            for (int j=0; j<table->rowCount(); j++)
            {
                auto item=new QTableWidgetItem;
                //const OCBarWindowVoice& bwv=barmap.Voice(j);
                if (i<barmap.barCount(j))
                {
                    item->setToolTip(vh[j]+barmap.tooltipText(j,i));
                    QString IconPath=":/bars/bars/";
                    switch (IntDiv(barmap.density(j,i),4))
                    {
                    case 0:
                        IconPath+="white";
                        break;
                    case 1:
                        IconPath+="lightgray";
                        break;
                    case 2:
                        IconPath+="gray";
                        break;
                    case 3:
                        IconPath+="darkgray";
                        break;
                    default:
                        IconPath+="black";
                    }
                    if ((i==barmap.barCount(j)-1) && (barmap.isIncomplete(j))) IconPath+="i";
                    item->setIcon(QIcon(IconPath+".png"));
                }
                if ((i >= BarVoiceLocation.Bar) && (i < BarVoiceLocation.Bar + BarCount)) item->setBackground(Qt::lightGray);
                table->setItem(j,i,item);
            }
        }
    }
    table->adjustSize();
    table->horizontalHeader()->setStretchLastSection(true);
    int voiceNum=barmap.BarMapIndex(BarVoiceLocation);
    QTableWidgetItem* selItem=nullptr;
    selItem=table->item(voiceNum,BarVoiceLocation.Bar);
    if (selItem != nullptr)
    {
        selItem->setSelected(true);
        table->scrollToItem(selItem,QAbstractItemView::PositionAtCenter);
    }
    table->show();
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    table->setUpdatesEnabled(true);
    table->blockSignals(false);
}

