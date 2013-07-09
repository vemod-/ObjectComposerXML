#include "cmusictree.h"
#include "ui_cmusictree.h"
//#include "mouseevents.h"
#include "ocsymbolscollection.h"

CMusicTree::CMusicTree(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMusicTree)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    table = new QMacTreeWidget(this);
    //table->setWindowFlags((Qt::WindowFlags)(table->windowFlags()|Qt::WA_OpaquePaintEvent|Qt::WA_PaintOnScreen));
    //table->setAutoFillBackground(true);
    table->setColumnCount(2);
    table->setEditTriggers(0);
    table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    //table->header()->setFixedHeight(tablerowheight);
    //table->setIndentation(12);
    //table->setUniformRowHeights(true);
    //table->setAnimated(false);
    //table->setAttribute(Qt::WA_MacShowFocusRect, 0);
    table->setHeaderHidden(true);
    //table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QGridLayout* layout=new QGridLayout;
    layout->setMargin(0);
    layout->setVerticalSpacing(0);
    layout->addWidget(table,0,0);
    setLayout(layout);
    //MouseEvents* ev=new MouseEvents;
    //table->viewport()->installEventFilter(ev);
    //connect(ev,SIGNAL(MouseRelease(QMouseEvent*)),this,SLOT(MouseRelease(QMouseEvent*)));
    //connect(ev,SIGNAL(MouseMove(QMouseEvent*)),this,SLOT(MouseMove(QMouseEvent*)));
    //connect(ev,SIGNAL(MouseEnter(QEvent*)),this,SLOT(MouseEnter(QEvent*)));
    //connect(ev,SIGNAL(MouseLeave(QEvent*)),this,SLOT(MouseLeave(QEvent*)));
    connect(table,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(ClickItem(QTreeWidgetItem*,int)));
    connect(table,SIGNAL(Popup(QPoint)),this,SIGNAL(Popup(QPoint)));
}

void CMusicTree::keyReleaseEvent(QKeyEvent *event)
{
    QKeySequence s(event->key(),event->modifiers());
    if (s.matches(QKeySequence::Delete))
    {
        table->blockSignals(true);
        table->setUpdatesEnabled(false);
        SendDelete();
        AdjustSelection();
        table->setFocus();
        table->blockSignals(false);
        table->setUpdatesEnabled(true);
        return;
    }
    if (s.matches(Qt::Key_Backspace))
    {
        table->blockSignals(true);
        table->setUpdatesEnabled(false);
        SendBackspace();
        AdjustSelection();
        table->setFocus();
        table->blockSignals(false);
        table->setUpdatesEnabled(true);
        return;
    }
    if ((event->key() != Qt::Key_Up) && (event->key() != Qt::Key_Down)) return;
    ItemChange();
}
/*
void CMusicTree::MouseRelease(QMouseEvent* event)
{
    if (event->button() & Qt::RightButton)
    {
        emit Popup(this->cursor().pos());
    }
}

void CMusicTree::MouseEnter(QEvent *event)
{
    Q_UNUSED(event);
    table->setMouseTracking(true);
}

void CMusicTree::MouseLeave(QEvent *event)
{
    Q_UNUSED(event);
    QTreeWidgetItemIterator it(table);
    while (*it)
    {
        QTreeWidgetItem* item=(*it);
        item->setFirstColumnSpanned(true);
        ++it;
    }
    table->setMouseTracking(false);
}

void CMusicTree::MouseMove(QMouseEvent *event)
{
    QTreeWidgetItemIterator it(table);
    QTreeWidgetItem* i=table->itemAt(event->pos());
    while (*it)
    {
        QTreeWidgetItem* item=(*it);
        if (i==item)
        {
            if (!item->icon(1).isNull())
            {
                item->setFirstColumnSpanned(false);
            }
        }
        else
        {
            item->setFirstColumnSpanned(true);
        }
        ++it;
    }
}
*/
/*
void CMusicTree::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (table->verticalScrollBar()->isVisible())
    {
        table->setColumnWidth(0,table->width()-(22+2)-table->verticalScrollBar()->width());
    }
    else
    {
        table->setColumnWidth(0,table->width()-(22+2));
    }
}
*/
CMusicTree::~CMusicTree()
{
    table->clear();
    delete ui;
}

void CMusicTree::ClickItem(QTreeWidgetItem* item, int Col)
{
    if (Col==1)
    {
        if (table->indexOfTopLevelItem(item)==-1)
        {
            if (item->text(0) != "EndOfVoice")
            {
                emit Delete(item->data(0,32).toInt());
            }
        }
    }
    else
    {
        ItemChange();
    }
}

QAction* CMusicTree::setAction(QKeySequence keySequence)
{
    QAction* a=new QAction(this);
    a->setShortcut(keySequence);
    a->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    this->addAction(a);
    return a;
}

void CMusicTree::AdjustSelection()
{
    for (int j=0;j<table->topLevelItemCount();j++)
    {
        QTreeWidgetItem* tl=table->topLevelItem(j);
        tl->setSelected(false);
        for (int i=0;i<tl->childCount();i++)
        {
            QTreeWidgetItem* item=tl->child(i);
            QFont f(item->font(0));
            f.setBold(false);
            QFont fb(item->font(0));
            fb.setBold(true);
            if (Cursor->IsSelected(item->data(0,32).toInt()))
            {
                item->setTextColor(0,selectedcolor);
                item->setSelected(true);
                item->setFont(0,fb);
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
                item->setCheckState(0,Qt::Checked);
            }
            else if (Cursor->GetPos()==item->data(0,32).toInt())
            {
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
                item->setCheckState(0,Qt::Unchecked);
                item->setTextColor(0,markedcolor);
                item->setSelected(true);
                item->setFont(0,fb);
            }
            else
            {
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
                item->setCheckState(0,Qt::Unchecked);
                item->setTextColor(0,activestaffcolor);
                item->setSelected(false);
                item->setFont(0,f);
            }
            if (item->text(0)=="EndOfVoice")
            {
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                item->setCheckState(0,Qt::Unchecked);
            }
        }
    }
}

void CMusicTree::ItemChange()
{
    if (table->selectedItems().count())
    {
        table->blockSignals(true);
        table->setUpdatesEnabled(false);
        Cursor->SetPos(table->selectedItems()[0]->data(0,32).toInt());
        foreach(QTreeWidgetItem* item,table->selectedItems())
        {
            if (table->indexOfTopLevelItem(item)==-1)
            {
                if ((item->checkState(0)) || (table->selectedItems().count()>1)) Cursor->AddSel(item->data(0,32).toInt());
            }
        }
        emit SelectionChanged();
        AdjustSelection();
        table->blockSignals(false);
        table->setUpdatesEnabled(true);
    }
}

void CMusicTree::Fill(XMLScoreWrapper& XMLScore, OCBarMap& BarMap, int StartBar, int BarCount, int Staff, int Voice, OCCursor* C)
{
    int Pointer=BarMap.GetPointer(StartBar, Staff, Voice);
    int EndPointer=BarMap.GetPointer(StartBar+BarCount, Staff, Voice);
    int Bar=StartBar;
    int FirstMeter=BarMap.GetMeter(StartBar, Staff, Voice);
    table->setUpdatesEnabled(false);
    table->blockSignals(true);
    Cursor=C;
    table->hide();
    table->clear();
    table->setHeaderLabels(QStringList() << tr("Symbol") << QString());
    QTreeWidgetItem* selItem=0;
    if (Cursor != 0)
    {
        QString StaffText="<br>"+XMLScore.StaffName(Staff)+" Voice "+QString::number(Voice+1);
        QString Info;
        int BarCount=Bar;
        OCCounter CountIt;
        QDomLiteElement* XMLVoice = XMLScore.Voice(Staff, Voice);
        XMLSymbolWrapper Symbol(XMLVoice,Pointer,FirstMeter);
        QColor col;
        forever
        {
            CountIt.reset();
            col=activestaffcolor;
            FirstMeter=BarMap.GetMeter(BarCount,Staff,Voice);
            BarCount++;
            int SymbolCount=0;
            QTreeWidgetItem* BarItem=table->createTopLevelItem(tr("Bar")+" "+QString::number(BarCount));
            //BarItem->setFlags(Qt::ItemIsEnabled);
            //BarItem->setText(0,"BAR "+QString::number(BarCount));
            //BarItem->setTextColor(0,"#666666");
            //BarItem->setFirstColumnSpanned(true);
            table->addTopLevelItem(BarItem);
            forever
            {
                Symbol=XMLSymbolWrapper(XMLVoice,Pointer,FirstMeter);
                if (Cursor->IsSelected(Pointer))
                {
                    col=selectedcolor;
                }
                else if (Cursor->GetPos()==Pointer)
                {
                    col=markedcolor;
                }
                else
                {
                    col=activestaffcolor;
                }
                if (Symbol.IsRestOrValuedNote())
                {
                    CountIt.Flip(Symbol.ticks());
                    CountIt.Flip1(Symbol.ticks());
                }
                else if (Symbol.IsEndOfVoice())
                {
                    Info = OCSymbolsCollection::Description(Symbol);
                    QTreeWidgetItem *nameItem = table->createStandardItem(Info);
                    nameItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                    nameItem->setCheckState(0,Qt::Unchecked);
                    if ((col==selectedcolor) || (col==markedcolor))
                    {
                        QFont f(nameItem->font(0));
                        f.setBold(true);
                        nameItem->setFont(0,f);
                        selItem=nameItem;
                    }
                    nameItem->setTextColor(0,col);
                    nameItem->setData(0,32,Pointer);
                    nameItem->setSizeHint(0,QSize(-1,tablerowheight));
                    BarItem->addChild(nameItem);
                    BarItem->setExpanded(true);
                    break;
                }
                else if (Symbol.Compare("Tuplet"))
                {
                     CountIt.Tuplets(Pointer, XMLVoice);
                 }
                /*
                else if (Symbol.Compare("Time"))
                {
                     FirstMeter = CTime::GetTicks(Symbol);
                }
                */
                Info = OCSymbolsCollection::Description(Symbol);
                QTreeWidgetItem *nameItem = table->createStandardItem(Info,QString(),1);
                //nameItem->setText(0,Info);
                nameItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
                nameItem->setCheckState(0,Qt::Unchecked);
                if ((col==selectedcolor) || (col==markedcolor))
                {
                    QFont f(nameItem->font(0));
                    f.setBold(true);
                    nameItem->setFont(0,f);
                    selItem=nameItem;
                    if (col==selectedcolor) nameItem->setCheckState(0,Qt::Checked);
                }
                nameItem->setIcon(0,OCSymbolsCollection::Icon(Symbol.name(),0));
                //nameItem->setIcon(1,QIcon(":/fileclose.png"));
                nameItem->setTextColor(0,col);
                nameItem->setData(0,32,Pointer);
                nameItem->setToolTip(0,"<b>"+Info+"</b><br>Bar "+QString::number(BarCount)+" Symbol "+QString::number(SymbolCount+1)+StaffText);
                nameItem->setToolTip(1,"Delete "+Info);
                //nameItem->setSizeHint(0,QSize(-1,tablerowheight));
                //nameItem->setSizeHint(1,QSize(22,tablerowheight));
                BarItem->addChild(nameItem);
                BarItem->setExpanded(true);
                //BarItem->setSizeHint(0,QSize(-1,tablerowheight));
                SymbolCount++;
                Pointer++;
                if (CountIt.NewBar(FirstMeter)) break;
            }
            if ((Pointer >= EndPointer) || Symbol.IsEndOfVoice()) break;
            CountIt.BarFlip();
        }
    }
    //MouseLeave(0);
    //table->setColumnWidth(1,22);
    //table->resizeColumnToContents(1);
    //table->header()->adjustSize();
    if (selItem != 0) table->scrollToItem(selItem);
    table->show();
    table->adjust();
    //resizeEvent(0);
    table->blockSignals(false);
    table->setUpdatesEnabled(true);
}

