#include "cpropertywindow.h"
#include "ui_cpropertywindow.h"
//#include <QStyledItemDelegate>

CCustomButton::CCustomButton(QString Caption, QWidget *parent) : QWidget(parent)
{
    tb=new QToolButton(this);
    tb->setText(Caption);
    tb->setFixedWidth(tablerowheight);
    QGridLayout* layout=new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(tb,0,0,Qt::AlignRight);
    setLayout(layout);
}

CCustomCheck::CCustomCheck(QWidget *parent) : QWidget(parent)
{
    cb=new QCheckBox(this);
    cb->setGeometry(4,0,tablerowheight,tablerowheight);
}

CPropertyWindow::CPropertyWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPropertyWindow)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    LastSelected.clear();
    editRow=0;
    table = new QMacTreeWidget(this);
    //table->setWindowFlags((Qt::WindowFlags)(table->windowFlags()|Qt::WA_OpaquePaintEvent|Qt::WA_PaintOnScreen));
    //table->setAutoFillBackground(true);
    table->setColumnCount(2);
    table->setEditTriggers(0);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    //table->setIndentation(12);
    //table->setUniformRowHeights(true);
    //table->setAnimated(false);
    //table->setAttribute(Qt::WA_MacShowFocusRect, 0);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setIconMode(false);
    //table->header()->setFixedHeight(tablerowheight);
    connect(table,SIGNAL(itemSelectionChanged()),this,SLOT(Edit()));
    QGridLayout* layout=new QGridLayout;
    layout->setMargin(0);
    layout->setVerticalSpacing(0);
    layout->addWidget(table,0,0);
    setLayout(layout);
}

void CPropertyWindow::BoolChanged(int Value)
{
    OCProperty* p=m_PropColl->GetItem(editRow->text(0));
    p->SetValue((bool)Value);
    editRow->setText(1,p->TextValue());
    HideItems();
    emit Changed(p->Name,m_PropColl);
}

void CPropertyWindow::BoolChanged(bool Value)
{
    BoolChanged((int)Value);
}

void CPropertyWindow::ListChanged(int Value)
{
    OCProperty* p=m_PropColl->GetItem(editRow->text(0));
    p->SetValue(Value+p->ListOffset);
    editRow->setText(1,p->TextValue());
    HideItems();
    emit Changed(p->Name,m_PropColl);
}

void CPropertyWindow::NumberChanged(int Value)
{
    OCProperty* p=m_PropColl->GetItem(editRow->text(0));
    p->SetValue(Value);
    editRow->setText(1,p->TextValue());
    HideItems();
    emit Changed(p->Name,m_PropColl);
}

void CPropertyWindow::NumberChanged()
{
    QSpinBox* sb=(QSpinBox*)table->itemWidget(editRow,1);
    NumberChanged(sb->value());
}

void CPropertyWindow::SliderChanged()
{
    QSlider* sl=(QSlider*)table->itemWidget(editRow,1);
    NumberChanged(sl->value());
}

void CPropertyWindow::TextChanged(QString Value)
{
    OCProperty* p=m_PropColl->GetItem(editRow->text(0));
    p->SetValue(Value);
    editRow->setText(1,p->TextValue());
    HideItems();
    emit Changed(p->Name,m_PropColl);
}

void CPropertyWindow::TextChanged()
{
    QLineEdit* le=(QLineEdit*)table->itemWidget(editRow,1);
    TextChanged(le->text());
}

void CPropertyWindow::CustomChanged()
{
    OCProperty* p=m_PropColl->GetItem(editRow->text(0));
    HideItems();
    emit Changed(p->Name,m_PropColl);
}

void CPropertyWindow::leaveEvent ( QEvent * event )
{
    Q_UNUSED(event);
    if (editRow != 0)
    {
        OCProperty* p=m_PropColl->GetItem(editRow->text(0));
        if (p->PropertyType==pwList)
        {
            QComboBox* item=findChild<QComboBox*>();
            QAbstractItemView* popup=item->findChild<QAbstractItemView*>();
            if (popup->isVisible()) return;
        }
        table->removeItemWidget(editRow,1);
        edit->deleteLater();
        edit=0;
        editRow=0;
    }
}

void CPropertyWindow::enterEvent( QEvent* event)
{
    Q_UNUSED(event);
    if ((editRow==0) && (edit==0))
    {
        if (table->selectedItems().count())
        {
            if (table->indexOfTopLevelItem(table->currentItem()) == -1)
            {
                Edit();
            }
        }
    }
}

void CPropertyWindow::Edit()
{
    if (table->indexOfTopLevelItem(table->currentItem()) != -1)
    {
        if (editRow !=0)
        {
            table->setCurrentItem(editRow);
        }
        return;
    }
    LastSelected=table->currentItem()->text(0);
    QLineEdit* te;
    QComboBox* cb;
    CCustomCheck* ch;
    CCustomButton* tb;
    QSpinBox* sb;
    QSlider* sl;
    //QStyledItemDelegate* d;
    OCProperty* p=m_PropColl->GetItem(LastSelected);
    if (editRow!=0)
    {
        table->removeItemWidget(editRow,1);
        delete edit;
        edit=0;
        editRow=0;
    }
    switch (p->PropertyType)
    {
    case pwNumber:
        sb=new QSpinBox(this);
        if (p->Min!="") sb->setMinimum(p->Min.toInt());
        if (p->Max!="") sb->setMaximum(p->Max.toInt());
        sb->setValue(p->GetValue().toInt());
        sb->selectAll();
        sb->setFrame(false);
        connect(sb,SIGNAL(valueChanged(int)),this,SLOT(NumberChanged()));
        edit=sb;
        edit->setFocus();
        break;
    case pwSlider:
        sl=new QSlider(Qt::Horizontal,this);
        if (p->Min!="") sl->setMinimum(p->Min.toInt());
        if (p->Max!="") sl->setMaximum(p->Max.toInt());
        sl->setValue(p->GetValue().toInt());
        connect(sl,SIGNAL(valueChanged(int)),this,SLOT(SliderChanged()));
        edit=sl;
        edit->setFocus();
        break;
    case pwList:
        cb=new QComboBox(this);
        cb->setFrame(false);
        cb->addItems(p->List());
        cb->setCurrentIndex(p->GetValue().toInt()-p->ListOffset);
        //d=new QStyledItemDelegate(this);
        //cb->setItemDelegate(d);
        connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(ListChanged(int)));
        edit=cb;
        edit->setFocus();
        break;
    case pwBoolean:
        ch=new CCustomCheck(this);
        ch->cb->setChecked(p->GetValue().toBool());
        connect(ch->cb,SIGNAL(toggled(bool)),this,SLOT(BoolChanged(bool)));
        edit=ch;
        ch->cb->setFocus();
        break;
    case pwCustom:
        tb=new CCustomButton("...",this);
        edit=tb;
        connect(tb->tb,SIGNAL(clicked()),this,SLOT(CustomChanged()));
        edit->setFocus();
        break;
    case pwText:
        te=new QLineEdit(p->GetValue().toString(),this);
        te->setFrame(false);
        te->selectAll();
        connect(te,SIGNAL(editingFinished()),this,SLOT(TextChanged()));
        edit=te;
        edit->setFocus();
        break;
    }
    if (edit != 0)
    {
        edit->setFixedHeight(tablerowheight);
        table->setItemWidget(table->currentItem(),1,edit);
        editRow=table->currentItem();
        edit->setToolTip(editRow->toolTip(0));
    }
}

CPropertyWindow::~CPropertyWindow()
{
    disconnect(table,SIGNAL(itemSelectionChanged()),this,SLOT(Edit()));
    if (editRow!=0) delete edit;
    table->clear();
    delete ui;
}

void CPropertyWindow::HideItems()
{
    OCSymbolsCollection::ModifyProperties(m_PropColl);
    for (int tl=0;tl<table->topLevelItemCount();tl++)
    {
        QTreeWidgetItem* tli=table->topLevelItem(tl);
        for (int sl=0;sl<tli->childCount();sl++)
        {
            QTreeWidgetItem* i=tli->child(sl);
            i->setHidden(m_PropColl->GetItem(i->text(0))->Hidden);
        }
    }
    for (int tl=0;tl<table->topLevelItemCount();tl++)
    {
        QTreeWidgetItem* tli=table->topLevelItem(tl);
        int ChildCount=0;
        for (int sl=0;sl<tli->childCount();sl++)
        {
            QTreeWidgetItem* i=tli->child(sl);
            if (!i->isHidden()) ChildCount++;
        }
        tli->setHidden(ChildCount==0);
    }
}

void CPropertyWindow::Fill(OCProperties *Properties)
{
    table->blockSignals(true);
    table->setUpdatesEnabled(false);
    if (editRow!=0)
    {
        delete edit;
    }
    editRow=0;
    m_PropColl=Properties;
    table->hide();
    table->clear();
    table->setHeaderLabels(QStringList() << tr("Property") << tr("Value"));
    int foundSelected=-1;
    QTreeWidgetItem* SelectedItem;
    if (Properties != 0)
    {
        for (int i = 0; i < Properties->Count(); ++i) {
            OCProperty* p=Properties->GetItem(i);
            QTreeWidgetItem* CategoryItem;
            bool Match=false;
            for (int j=0; j<table->topLevelItemCount(); j++)
            {
                if (table->topLevelItem(j)->text(0)==p->Category.toUpper())
                {
                    CategoryItem=table->topLevelItem(j);
                    Match=true;
                    break;
                }
            }
            if (!Match)
            {
                CategoryItem=table->createTopLevelItem(p->Category);
                table->addTopLevelItem(CategoryItem);
            }

            QTreeWidgetItem *nameItem = table->createStandardItem(p->Name);
            nameItem->setText(1,p->TextValue());
            if ((p->Name==LastSelected) && (!p->Hidden))
            {
                SelectedItem=nameItem;
                foundSelected=i;
            }
            nameItem->setToolTip(0,"<b>"+p->Name+"</b><br>"+p->Description);
            nameItem->setToolTip(1,"<b>"+p->Name+"</b><br>"+p->Description);
            nameItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            CategoryItem->addChild(nameItem);
            CategoryItem->setExpanded(true);
            nameItem->setSizeHint(0,QSize(70,tablerowheight));
            nameItem->setSizeHint(1,QSize(-1,tablerowheight));
            if (p->PropertyType==pwBoolean)
            {
                nameItem->setFont(1,QFont("Wingdings",18));
            }
        }
    }
    HideItems();
    //table->resizeColumnToContents(0);
    //table->header()->setStretchLastSection(true);
    table->show();
    table->adjust();
    if (foundSelected>-1)
    {
        table->setCurrentItem(SelectedItem,1);
        table->scrollToItem(SelectedItem);
    }
    table->blockSignals(false);
    table->setUpdatesEnabled(true);
}

