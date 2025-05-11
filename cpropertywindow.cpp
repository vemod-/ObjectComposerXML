#include "cpropertywindow.h"
#include "ui_cpropertywindow.h"
#include <QHeaderView>
#include "ocsymbolscollection.h"
//#include <QStyledItemDelegate>

CCustomButton::CCustomButton(const QString& Caption, QWidget *parent) : QWidget(parent)
{
    tb=new QToolButton(this);
    tb->setText(Caption);
    tb->setFixedWidth(tablerowheight);
    auto layout=new QGridLayout();
    layout->setContentsMargins(0,0,0,0);
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
    editRow=nullptr;
    table = new QMacTreeWidget(this);
    table->setColumnCount(2);
    table->setEditTriggers(QFlag(0));
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setIconMode(false);
    table->header()->setSectionResizeMode(QHeaderView::Stretch);
    table->header()->setStretchLastSection(true);
    connect(table,&QTreeWidget::itemSelectionChanged,this,&CPropertyWindow::Edit);
    auto layout=new QGridLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setVerticalSpacing(0);
    layout->addWidget(table,0,0);
    setLayout(layout);
}

void CPropertyWindow::BoolChanged(int Value)
{
    OCProperty& p=m_PropColl->property(editRow->text(0));
    p.setValue(bool(Value));
    editRow->setText(1,p.textValue());
    HideItems();
    emit Changed(p.Name,p.value(),false);
}

void CPropertyWindow::BoolChanged(bool Value)
{
    BoolChanged(int(Value));
}

void CPropertyWindow::ListChanged(int Value)
{
    OCProperty& p=m_PropColl->property(editRow->text(0));
    p.setListIndex(Value);
    editRow->setText(1,p.textValue());
    HideItems();
    emit Changed(p.Name,p.value(),false);
}

void CPropertyWindow::NumberChanged(int Value)
{
    OCProperty& p=m_PropColl->property(editRow->text(0));
    p.setValue(Value);
    editRow->setText(1,p.textValue());
    HideItems();
    emit Changed(p.Name,p.value(),false);
}

void CPropertyWindow::NumberChanged()
{
    auto sb = dynamic_cast<QSpinBox*>(table->itemWidget(editRow,1));
    if (sb) NumberChanged(sb->value());
}

void CPropertyWindow::SliderChanged()
{
    auto sl = dynamic_cast<QSlider*>(table->itemWidget(editRow,1));
    if (sl) NumberChanged(sl->value());
}

void CPropertyWindow::TextChanged(const QString& Value)
{
    OCProperty& p=m_PropColl->property(editRow->text(0));
    p.setValue(Value);
    editRow->setText(1,p.textValue());
    HideItems();
    emit Changed(p.Name,p.value(),false);
}

void CPropertyWindow::TextChanged()
{
    auto le = dynamic_cast<QLineEdit*>(table->itemWidget(editRow,1));
    if (le) TextChanged(le->text());
}

void CPropertyWindow::CustomChanged()
{
    OCProperty& p=m_PropColl->property(editRow->text(0));
    HideItems();
    emit Changed(p.Name,p.value(),true);
}

void CPropertyWindow::leaveEvent ( QEvent * event )
{
    Q_UNUSED(event);
    if (editRow != nullptr)
    {
        OCProperty& p=m_PropColl->property(editRow->text(0));
        if (p.PropertyType==pwList)
        {
            auto item=findChild<QComboBox*>();
            auto popup=item->findChild<QAbstractItemView*>();
            if (popup->isVisible()) return;
        }
        table->removeItemWidget(editRow,1);
        edit->deleteLater();
        edit=nullptr;
        editRow=nullptr;
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
void CPropertyWindow::enterEvent(QEnterEvent *event)
#else
void CPropertyWindow::enterEvent(QEvent *event)
#endif
{
    Q_UNUSED(event);
    if ((editRow==nullptr) && (edit==nullptr))
    {
        if (!table->selectedItems().empty())
        {
            if (table->indexOfTopLevelItem(table->currentItem()) == -1)
            {
                Edit();
                return;
            }
        }
    }
    if (editRow) table->setCurrentItem(editRow);
}

void CPropertyWindow::Edit() {
    if (table->indexOfTopLevelItem(table->currentItem()) != -1) {
        if (editRow != nullptr) {
            table->setCurrentItem(editRow);
        }
        return;
    }
    LastSelected = table->currentItem()->text(0);
    QLineEdit *te;
    QComboBox *cb;
    CCustomCheck *ch;
    CCustomButton *tb;
    QSpinBox *sb;
    QSlider *sl;
    // QStyledItemDelegate* d;
    OCProperty &p = m_PropColl->property(LastSelected);
    if (editRow != nullptr) {
        table->removeItemWidget(editRow, 1);
        delete edit;
        edit = nullptr;
        editRow = nullptr;
    }
    switch (p.PropertyType) {
    case pwNumber:
        sb = new QSpinBox(this);
        if (p.Min != "")
            sb->setMinimum(p.Min.toInt());
        if (p.Max != "")
            sb->setMaximum(p.Max.toInt());
        sb->setValue(p.value().toInt());
        sb->selectAll();
        sb->setFrame(false);
        connect(sb, qOverload<int>(&QSpinBox::valueChanged), this,
                qOverload<int>(&CPropertyWindow::NumberChanged));
        edit = sb;
        edit->setFocus();
        break;
    case pwSlider:
        sl = new QSlider(Qt::Horizontal, this);
        if (p.Min != "")
            sl->setMinimum(p.Min.toInt());
        if (p.Max != "")
            sl->setMaximum(p.Max.toInt());
        sl->setValue(p.value().toInt());
        connect(sl, &QAbstractSlider::valueChanged, this,
                &CPropertyWindow::SliderChanged);
        edit = sl;
        edit->setFocus();
        break;
    case pwList:
        cb = new QComboBox(this);
        cb->setFrame(false);
        cb->addItems(p.list());
        cb->setCurrentIndex(p.listIndex());
        // d=new QStyledItemDelegate(this);
        // cb->setItemDelegate(d);
        connect(cb, qOverload<int>(&QComboBox::currentIndexChanged), this,
                &CPropertyWindow::ListChanged);
        edit = cb;
        edit->setFocus();
        break;
    case pwBoolean:
        ch = new CCustomCheck(this);
        ch->cb->setChecked(p.value().toBool());
        connect(ch->cb, &QAbstractButton::toggled, this,
                qOverload<bool>(&CPropertyWindow::BoolChanged));
        edit = ch;
        ch->cb->setFocus();
        break;
    case pwCustom:
        tb = new CCustomButton("...", this);
        edit = tb;
        connect(tb->tb, &QAbstractButton::clicked, this,
                &CPropertyWindow::CustomChanged);
        edit->setFocus();
        break;
    case pwText:
        te = new QLineEdit(p.value().toString(), this);
        te->setFrame(false);
        te->selectAll();
        connect(te, &QLineEdit::editingFinished, this,
                qOverload<>(&CPropertyWindow::TextChanged));
        edit = te;
        edit->setFocus();
        break;
    }
    if (edit != nullptr) {
        edit->setFixedHeight(tablerowheight);
        edit->setMinimumWidth(1);
        table->setItemWidget(table->currentItem(), 1, edit);
        editRow = table->currentItem();
        edit->setToolTip(editRow->toolTip(0));
        if (table->currentColumn() == 1) {
            if (p.PropertyType == pwList) ((QComboBox*)edit)->showPopup();
            if (p.PropertyType == pwBoolean) ((CCustomCheck*)edit)->cb->toggle();
            if (p.PropertyType == pwCustom) ((CCustomButton*)edit)->tb->click();
        }
    }
}

CPropertyWindow::~CPropertyWindow()
{
    disconnect(table,&QTreeWidget::itemSelectionChanged,this,&CPropertyWindow::Edit);
    if (editRow!=nullptr) delete edit;
    table->clear();
    delete ui;
}

void CPropertyWindow::HideItems()
{
    OCSymbolsCollection::ModifyProperties(*m_PropColl);
    for (int tl=0;tl<table->topLevelItemCount();tl++)
    {
        QTreeWidgetItem* tli=table->topLevelItem(tl);
        for (int sl=0;sl<tli->childCount();sl++)
        {
            QTreeWidgetItem* i=tli->child(sl);
            i->setHidden(m_PropColl->isHidden(i->text(0)));
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
    if (editRow!=nullptr)
    {
        delete edit;
    }
    editRow=nullptr;
    m_PropColl=Properties;
    table->clear();
    table->setHeaderLabels(QStringList() << tr("Property") << tr("Value"));
    QTreeWidgetItem* SelectedItem=nullptr;
    if (Properties != nullptr)
    {
        //for (int i = 0; i < Properties->count(); ++i) {
        //    OCProperty* p=Properties->item(i);
        for (OCProperty& p : *Properties) {
            QTreeWidgetItem* CategoryItem=nullptr;
            for (int j=0; j<table->topLevelItemCount(); j++)
            {
                if (table->topLevelItem(j)->text(0)==p.Category.toUpper())
                {
                    CategoryItem=table->topLevelItem(j);
                    break;
                }
            }
            if (!CategoryItem)
            {
                CategoryItem=table->createTopLevelItem(p.Category);
                table->addTopLevelItem(CategoryItem);
            }

            QTreeWidgetItem *nameItem = table->createStandardItem(p.Name);
            nameItem->setText(1,p.textValue());
            if ((p.Name==LastSelected) && (!p.Hidden))
            {
                SelectedItem=nameItem;
            }
            nameItem->setToolTip(0,"<b>"+p.Name+"</b><br>"+p.Description);
            nameItem->setToolTip(1,"<b>"+p.Name+"</b><br>"+p.Description);
            nameItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            CategoryItem->addChild(nameItem);
            CategoryItem->setExpanded(true);
            nameItem->setSizeHint(0,QSize(70,tablerowheight));
            nameItem->setSizeHint(1,QSize(-1,tablerowheight));
            if (p.PropertyType==pwBoolean)
            {
                nameItem->setFont(1,QFont("Wingdings",18));
            }
            if (p.value() != p.Default) {
                QFont f= nameItem->font(0);
                f.setBold(true);
                nameItem->setFont(0,f);
            }
        }
    }
    HideItems();
    //table->resizeColumnToContents(0);
    //table->header()->setStretchLastSection(true);
    table->show();
    if (SelectedItem)
    {
        table->setCurrentItem(SelectedItem,1);
        table->scrollToItem(SelectedItem);
    }
    table->adjust();
    table->blockSignals(false);
    table->setUpdatesEnabled(true);
}

void CPropertyWindow::Fill(const XMLSimpleSymbolWrapper &Symbol,const int Voice)
{
    UpdateProperties(Symbol,Voice);
    Fill(m_PropColl);
}

void CPropertyWindow::UpdateProperties(const XMLSimpleSymbolWrapper& Symbol,const int Voice)
{
    m_PropColl = &OCSymbolsCollection::GetProperties(Symbol);
    if (m_PropColl->contains("Common")) m_PropColl->hide("Common",Voice > 0);
}

QSize CPropertyWindow::contentSize()
{
    return table->contentSize();
}
