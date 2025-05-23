#include "cmusictree.h"
#include "ui_cmusictree.h"
//#include "mouseevents.h"
#include "ocsymbolscollection.h"
#include <QAction>
#include <QHeaderView>
#include <QGridLayout>

CMusicTree::CMusicTree(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMusicTree)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    table = new QMacTreeWidget(this);
    table->setColumnCount(2);
    table->setEditTriggers(QFlag(0));
    table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setHeaderHidden(true);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    table->setDragDropMode(QTreeWidget::InternalMove);
    table->setDefaultDropAction(Qt::IgnoreAction);
    auto layout=new QGridLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setVerticalSpacing(0);
    layout->addWidget(table,0,0);
    setLayout(layout);

    connect(table,&QTreeWidget::itemClicked,this,&CMusicTree::ClickItem);
    connect(table,&QTreeWidget::itemDoubleClicked,this,&CMusicTree::DoubleClickItem);
    connect(table,&QWidget::customContextMenuRequested,this,&CMusicTree::showContextMenu);
    connect(table,&QMacTreeWidget::Popup,this,&CMusicTree::Popup);
    connect(table,&QMacTreeWidget::itemsMoved,this,&CMusicTree::ItemsMoved);
}

void CMusicTree::keyReleaseEvent(QKeyEvent *event)
{
    QKeySequence s(event->key(),int(event->modifiers()));
    if (s.matches(QKeySequence::Delete))
    {
        table->blockSignals(true);
        table->setUpdatesEnabled(false);
        emit SendDelete();
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
        emit SendBackspace();
        AdjustSelection();
        table->setFocus();
        table->blockSignals(false);
        table->setUpdatesEnabled(true);
        return;
    }
    if ((event->key() != Qt::Key_Up) && (event->key() != Qt::Key_Down)) return;
    ItemChange();
}

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
            emit Delete(item->data(0,32).toInt());
        }
    }
    else
    {
        ItemChange();
        //if (Cursor->SelCount()) emit Popup(QCursor::pos());
    }
}

void CMusicTree::DoubleClickItem(QTreeWidgetItem* /*item*/, int /*Col*/)
{
    emit Properties(QCursor::pos());
}

void CMusicTree::ItemsMoved(QList<QTreeWidgetItem *>) {
    QTreeWidgetItemIterator it(table);
    QList<int> l;
    while (*it) {
        if ((*it)->data(0,32).isValid()) {
            l.append((*it)->data(0,32).toInt());
        }
        ++it;
    }
    emit Rearranged(l);
}

void CMusicTree::showContextMenu(QPoint /*p*/)
{
    ItemChange();
    if (Cursor->SelCount()) emit Popup(QCursor::pos());
}

QAction* CMusicTree::setAction(QKeySequence keySequence)
{
    auto a=new QAction(this);
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
                item->setForeground(0,selectedcolor);
                item->setSelected(true);
                item->setFont(0,fb);
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled);
                item->setCheckState(0,Qt::Checked);
            }
            else if (Cursor->currentPointer()==item->data(0,32).toInt())
            {
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled);
                item->setCheckState(0,Qt::Unchecked);
                item->setForeground(0,markedcolor);
                item->setSelected(true);
                item->setFont(0,fb);
            }
            else
            {
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled);
                item->setCheckState(0,Qt::Unchecked);
                item->setForeground(0,activestaffcolor);
                item->setSelected(false);
                item->setFont(0,f);
            }
        }
    }
}

void CMusicTree::ItemChange()
{
    if (!table->selectedItems().empty())
    {
        table->blockSignals(true);
        table->setUpdatesEnabled(false);
        Cursor->SetPos(table->selectedItems()[0]->data(0,32).toInt());
        for (QTreeWidgetItem* item : (const QList<QTreeWidgetItem*>)table->selectedItems())
        {
            if (table->indexOfTopLevelItem(item)==-1)
            {
                if ((item->checkState(0)) || (table->selectedItems().size()>1)) Cursor->AddSel(item->data(0,32).toInt());
            }
        }
        emit SelectionChanged();
        AdjustSelection();
        table->blockSignals(false);
        table->setUpdatesEnabled(true);
    }
}

void CMusicTree::Fill(XMLScoreWrapper& XMLScore, OCBarMap& BarMap, int BarCount, const OCBarLocation& BarLocation, OCCursor* C)
{
    int Pointer=BarMap.GetPointer(BarLocation).Pointer;
    int EndPointer=BarMap.GetPointer(OCBarLocation(BarLocation,BarLocation.Bar+BarCount)).Pointer;
    int FirstMeter=BarMap.GetMeter(BarLocation);
    table->setUpdatesEnabled(false);
    table->blockSignals(true);
    Cursor=C;
    table->hide();
    table->clear();
    table->setHeaderLabels(QStringList() << tr("Symbol") << QString());
    QTreeWidgetItem* selItem=nullptr;
    if (Cursor != nullptr)
    {
        QString StaffText="<br>"+XMLScore.StaffName(BarLocation.StaffId)+" Voice "+QString::number(BarLocation.Voice+1);
        QString Info;
        OCCounter CountIt(BarLocation.Bar);
        const XMLVoiceWrapper& XMLVoice = XMLScore.Voice(BarLocation);
        QColor col;
        forever
        {
            //CountIt.reset();
            col=activestaffcolor;
            FirstMeter=BarMap.GetMeter(OCBarLocation(BarLocation,CountIt.barCount()));
            int SymbolCount=0;
            QTreeWidgetItem* BarItem=table->createTopLevelItem(tr("Bar")+" "+QString::number(CountIt.barCount()+1));
            BarItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);
            table->addTopLevelItem(BarItem);
            while (Pointer < XMLVoice.symbolCount())
            {
                const XMLSymbolWrapper& Symbol=XMLVoice.XMLSymbol(Pointer,FirstMeter);
                if (Cursor->IsSelected(Pointer))
                {
                    col=selectedcolor;
                }
                else if (Cursor->currentPointer()==Pointer)
                {
                    col=markedcolor;
                }
                else
                {
                    col=activestaffcolor;
                }
                if (Symbol.IsRestOrValuedNote())
                {
                    CountIt.flipAll(Symbol.ticks());
                }
                else if (Symbol.IsTuplet())
                {
                     CountIt.beginTuplet(Pointer, XMLVoice);
                 }
                Info = Symbol.description();
                QTreeWidgetItem *nameItem = table->createStandardItem(Info,QString(),1);
                nameItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled);
                nameItem->setCheckState(0,Qt::Unchecked);
                if ((col==selectedcolor) || (col==markedcolor))
                {
                    QFont f(nameItem->font(0));
                    f.setBold(true);
                    nameItem->setFont(0,f);
                    selItem=nameItem;
                    if (col==selectedcolor) nameItem->setCheckState(0,Qt::Checked);
                }
                nameItem->setIcon(0,OCSymbolsCollection::SymbolIcon(Symbol));
                nameItem->setForeground(0,col);
                nameItem->setData(0,32,Pointer);
                nameItem->setToolTip(0,"<b>"+Info+"</b><br>Bar "+QString::number(CountIt.barCount()+1)+" Symbol "+QString::number(SymbolCount+1)+StaffText);
                nameItem->setToolTip(1,"Delete "+Info);
                BarItem->addChild(nameItem);
                BarItem->setExpanded(true);
                SymbolCount++;
                Pointer++;
                if (CountIt.newBar(FirstMeter)) break;
            }
            if ((Pointer >= EndPointer) || Pointer >= XMLVoice.symbolCount()) break;
            CountIt.barFlip();
        }
    }
    if (selItem != nullptr) table->scrollToItem(selItem);
    table->show();
    table->adjust();
    table->setColumnWidth(1,100);
    table->blockSignals(false);
    table->setUpdatesEnabled(true);
}

QSize CMusicTree::contentSize()
{
    return table->contentSize();
}
