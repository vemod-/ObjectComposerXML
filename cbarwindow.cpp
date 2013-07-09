#include "cbarwindow.h"
#include "ui_cbarwindow.h"
#include "mouseevents.h"

CBarWindow::CBarWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CBarWindow)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    table=new QTableWidget(0,0,this);
    table->setAutoFillBackground(true);
    table->setEditTriggers(0);
    table->setSelectionMode(QAbstractItemView::ContiguousSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectItems);
    table->setShowGrid(false);
    table->setIconSize(QSize(25,18));
    table->horizontalHeader()->setFixedHeight(tablerowheight);
    //table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    //table->horizontalHeader()->setResizeMode(QHeaderView::Fixed);

    QGridLayout* layout=new QGridLayout;
    layout->setMargin(0);
    layout->setVerticalSpacing(0);
    layout->addWidget(table,0,0);
    setLayout(layout);
    connect(table,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(Edit(int,int)));
    connect(table->verticalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(SelectVoice(int)));
    connect(table,SIGNAL(itemSelectionChanged()),this,SLOT(Select()));
    MouseEvents* ev=new MouseEvents;
    table->viewport()->installEventFilter(ev);
    connect(ev,SIGNAL(MouseRelease(QMouseEvent*)),this,SLOT(MouseRelease(QMouseEvent*)));
}

CBarWindow::~CBarWindow()
{
    table->clear();
    delete table;
    delete ui;
}

void CBarWindow::MouseRelease(QMouseEvent* event)
{
    if (event->button() & Qt::RightButton)
    {
        emit Popup(this->cursor().pos());
    }
}

void CBarWindow::Select()
{
    if (table->selectedRanges().count())
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
    if (table->selectedRanges().count())
    {
        QTableWidgetSelectionRange r=table->selectedRanges().at(0);
        QTableWidgetSelectionRange r1(r.topRow(),r.leftColumn(),r.bottomRow(),table->columnCount()-1);
        table->setRangeSelected(r1,true);
        Select();
    }
}

void CBarWindow::SelectFromStart()
{
    if (table->selectedRanges().count())
    {
        QTableWidgetSelectionRange r=table->selectedRanges().at(0);
        QTableWidgetSelectionRange r1(r.topRow(),0,r.bottomRow(),r.rightColumn());
        table->setRangeSelected(r1,true);
        Select();
    }
}

void CBarWindow::Edit(int Row, int Col)
{
    emit BarChanged(Col,BarMap.Voices.at(Row).Staff,BarMap.Voices.at(Row).Voice);
    table->clearSelection();
    table->item(Row,Col)->setSelected(true);
}

void CBarWindow::SelectVoice(int Row)
{
    emit BarChanged(0,BarMap.Voices.at(Row).Staff,BarMap.Voices.at(Row).Voice);
    table->clearSelection();
    table->selectRow(Row);
}

void CBarWindow::Fill(OCBarMap& barmap, int StartBar, int Staff, int Voice)
{
    BarMap=barmap;
    table->blockSignals(true);
    table->setUpdatesEnabled(false);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->hide();
    table->clear();
    if (barmap.Voices.count()==0)
    {
        table->setRowCount(0);
        table->setColumnCount(0);
    }
    else
    {
        table->setColumnCount(barmap.BarCountAll()+1);
        table->setRowCount(barmap.Voices.count());
        QStringList hh;
        for (int i=0;i<table->columnCount()-1;i++)
        {
            hh.append(QString::number(i+1));
            table->setColumnWidth(i,30);
        }
        hh.append(QString());
        table->setHorizontalHeaderLabels(hh);
        QStringList vh;
        for (int i=0;i<barmap.Voices.count();i++)
        {
            OCBarWindowVoice& bwv=barmap.Voices[i];
            if (bwv.NumOfVoices>1)
            {
                vh.append(bwv.Name+" ("+QString::number(bwv.Voice+1)+")");
            }
            else
            {
                vh.append(bwv.Name);
            }
            table->setRowHeight(i,tablerowheight);
        }
        table->setVerticalHeaderLabels(vh);
        for (int i=0;i<table->columnCount();i++)
        {
            for (int j=0; j<table->rowCount(); j++)
            {
                QTableWidgetItem* item=new QTableWidgetItem;
                OCBarWindowVoice& bwv=barmap.Voices[j];
                if (i<bwv.Bars.count())
                {
                    const OCBarWindowBar& bwb=bwv.Bars[i];
                    item->setToolTip(
                                vh[j]+"\n"+tr("Bar")
                                     +" "+ QString::number(i+1) +"\n"
                                     +tr("Meter")+" "+bwb.MeterText
                                     //+"\nNotes "+QString::number(bwb.Notes)
                                    //+"\nPointer "+QString::number(bwb.Pointer)
                                     + "\n" + (bwb.IsFullRest ? "Rest only":"Notes "+QString::number(bwb.Notes))
                                     //+"\nFullRest"+QString::number(bwb.IsFullRest)
                                     //+"\nFullRestOnly"+QString::number(bwb.IsFullRestOnly)
                                     //+"\nKeyChange"+QString::number(bwb.KeyChangeOnOne)
                                     //+"\nClefChange"+QString::number(bwb.ClefChangeOnOne)
                                     //+"\nMasterStuff"+QString::number(bwb.MasterStuff)
                            );
                    QString IconPath=":/bars/bars/";
                    switch (IntDiv(bwb.Density,4))
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
                    if ((i==bwv.Bars.count()-1) && (bwv.Incomplete)) IconPath+="i";
                    item->setIcon(QIcon(IconPath+".png"));
                }
                table->setItem(j,i,item);
            }
        }
    }
    table->adjustSize();
    table->horizontalHeader()->setStretchLastSection(true);
    int voiceNum=barmap.BarMapIndex(Staff, Voice);
    //VoiceList.indexOf(qMakePair(Staff,Voice));
    QTableWidgetItem* selitem=0;
    selitem=table->item(voiceNum,StartBar);
    if (selitem != 0)
    {
        selitem->setSelected(true);
        table->scrollToItem(selitem,QAbstractItemView::PositionAtCenter);
        /*
        if ((sv->StartBar()*table->columnWidth(0))+table->verticalHeader()->width()>table->width())
        {
            QRect r=table->viewport()->geometry();
            r.moveTo((sv->StartBar()*table->columnWidth(0))+table->verticalHeader()->width(),0);
            table->viewport()->setGeometry(r);
        }
        */
    }
    table->show();
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    table->setUpdatesEnabled(true);
    table->blockSignals(false);
}

