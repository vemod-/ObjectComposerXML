#include "QToolButtonGrid.h"

QToolButtonGrid::QToolButtonGrid(QWidget *parent) :
    QWidget(parent)
{
    frame=new QFrame(this);
    layout=new QGridLayout(frame);
    layout->setMargin(2);
    layout->setVerticalSpacing(1);
    layout->setHorizontalSpacing(1);
    TreeItem=0;
}

QToolButtonGrid::~QToolButtonGrid()
{
    qDeleteAll(Buttons);
    Buttons.clear();
    Names.clear();
    delete layout;
    delete frame;
}

QToolButton* QToolButtonGrid::AddButton(const QString& Name, const QString& Tooltip, const bool Selectable)
{
    QToolButton* b=new QToolButton(frame);
    b->setToolTip(Tooltip);
    b->setFixedSize(ButtonGridSize,ButtonGridSize);
    b->setIconSize(QSize(ButtonGridSize-4,ButtonGridSize-4));
    b->setCheckable(Selectable);
    Buttons.append(b);
    Names.append(Name);
    connect(b,SIGNAL(pressed()),this,SLOT(wasClicked()));
    return b;
}

void QToolButtonGrid::AddButton(const QString &Name, const QString& Tooltip, const QIcon& Icon, const bool Selectable)
{
    QToolButton* b=AddButton(Name,Tooltip,Selectable);
    b->setIcon(Icon);
}

void QToolButtonGrid::AddButton(const QString& Name, const QString& Tooltip, const QString& Text, const QFont& Font, const bool Selectable)
{
    QToolButton* b=AddButton(Name,Tooltip,Selectable);
    b->setText(Text);
    b->setFont(Font);
}

void QToolButtonGrid::AddToTree(const QString& Name, QMacTreeWidget* tw)
{
    QTreeWidgetItem* tl=tw->createTopLevelItem(Name);
    tw->addTopLevelItem(tl);
    QTreeWidgetItem* item=new QTreeWidgetItem();
    tl->addChild(item);
    tw->setItemWidget(item,0,this);
    TreeItem=item;
    tl->setExpanded(true);
}

void QToolButtonGrid::ShowButtons()
{
    int Columns=IntDiv(this->width()-4,ButtonGridSize+1);
    if (Columns==0) Columns=1;
    int ColumnCounter=0;
    int RowCounter=0;
    frame->hide();
    layout->setRowMinimumHeight(0,ButtonGridSize);
    foreach(QToolButton* b,Buttons)
    {
        layout->addWidget(b,RowCounter,ColumnCounter);
        if (RowCounter==0) layout->setColumnMinimumWidth(ColumnCounter,ButtonGridSize);

        ColumnCounter++;
        if (ColumnCounter>=Columns)
        {
            ColumnCounter=0;
            RowCounter++;
            layout->setRowMinimumHeight(RowCounter,ButtonGridSize);
        }
    }
    if (ColumnCounter!=0)
    {
        RowCounter++;
    }
    ColumnCounter++;
    frame->move(0,-2);
    frame->setBaseSize(ColumnCounter*(ButtonGridSize+1),RowCounter*(ButtonGridSize+1));
    if (TreeItem != 0) TreeItem->setSizeHint(0,QSize(ColumnCounter*(ButtonGridSize+1),RowCounter*(ButtonGridSize+1)));
    frame->show();
}

void QToolButtonGrid::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    foreach(QToolButton* b,Buttons) layout->removeWidget(b);
    ShowButtons();
}

void QToolButtonGrid::wasClicked()
{
    for (int i=0;i<Buttons.count();i++)
    {
        if (Buttons[i]->isDown())
        {
            emit ButtonClicked(i);
            emit ButtonClicked(Names[i]);
        }
    }
}

const bool QToolButtonGrid::IsSelected(const int Index)
{
    return Buttons[Index]->isChecked();
}

const bool QToolButtonGrid::IsSelected(const QString& Name)
{
    return IsSelected(Names.indexOf(Name));
}

void QToolButtonGrid::SetSelected(const int Index, const bool Selected)
{
    Buttons[Index]->setChecked(Selected);
}

void QToolButtonGrid::SetSelected(const QString& Name, const bool Selected)
{
    SetSelected(Names.indexOf(Name),Selected);
}
/*
CSymbolToolGrid::CSymbolToolGrid(QWidget *parent) : QToolButtonGrid(parent)
{
    //connect(this,SIGNAL(ButtonClicked(int)),this,SLOT(wasClicked1(int)));
}

void CSymbolToolGrid::SelectionChanged(int Ticks)
{

}

void CSymbolToolGrid::wasClicked1(int Button)
{

}
*/
CPropertiesToolGrid::CPropertiesToolGrid(QWidget *parent) : QToolButtonGrid(parent)
{
    connect(this,SIGNAL(ButtonClicked(int)),this,SLOT(wasClicked1(int)));
}

void CPropertiesToolGrid::AddModifierButton(const QString& Name, const QString& Tooltip, const QIcon& Icon)
{
    OCToolButtonProps* p=new OCToolButtonProps(Name,0);
    p->ismodifier=true;
    ButtonProps.append(p);
    QToolButtonGrid::AddButton(Name,Tooltip,Icon);
    Buttons[Buttons.count()-1]->setCheckable(true);
}

void CPropertiesToolGrid::AddModifierButton(const QString& Name, const QString& Tooltip, const QString& Text, const QFont& Font)
{
    OCToolButtonProps* p=new OCToolButtonProps(Name,0);
    p->ismodifier=true;
    ButtonProps.append(p);
    QToolButtonGrid::AddButton(Name,Tooltip,Text,Font);
    Buttons[Buttons.count()-1]->setCheckable(true);
}

void CPropertiesToolGrid::AddButton(const QString& SymbolName)
{
    for (int i=0;i<OCSymbolsCollection::ButtonCount(SymbolName);i++)
    {
        OCToolButtonProps* p=OCSymbolsCollection::ButtonProps(SymbolName,i);
        if (p->ismodifier)
        {
            AddModifierButton(p->modifierProperty,p->tooltip,p->buttonText,QFont(p->fontname,p->fontsize,p->fontbold,p->fontitalic));
        }
        else
        {
            if (!p->ishidden)
            {
                ButtonProps.append(p);
                if (!p->iconpath.isEmpty())
                {
                    QToolButtonGrid::AddButton(p->classname,p->tooltip,QIcon(p->iconpath));
                }
                else
                {
                    QFont f(p->fontname,p->fontsize);
                    f.setBold(p->fontbold);
                    f.setItalic(p->fontitalic);
                    QToolButtonGrid::AddButton(p->classname,p->tooltip,p->buttonText,f);
                }
            }
        }
    }
}

void CPropertiesToolGrid::wasClicked1(int Button)
{
    OCToolButtonProps* b=ButtonProps[Button];
    if (b->ismodifier) return;
    XMLSimpleSymbolWrapper s=OCSymbolsCollection::GetSymbol(b->classname,b->buttonindex);
    if (!b->modifierProperty.isEmpty())
    {
        QToolButton* ModifierButton=0;
        for (int i=0;i<ButtonProps.count();i++)
        {
            if (ButtonProps[i]->ismodifier)
            {
                if (Names[i]==b->modifierProperty)
                {
                    ModifierButton=Buttons[i];
                    break;
                }
            }
        }
        if (ModifierButton != 0)
        {
            if (OCSymbolsCollection::PropetyExists(s.name(), b->modifierProperty)) s.setAttribute(b->modifierProperty,ModifierButton->isChecked());
        }
    }
    if (b->customdialog)
    {
        OCRefreshMode RefreshMode=b->refreshmode;
        if (OCSymbolsCollection::editevent(s,RefreshMode,this))
        {
            emit PasteXML(s,Names[Button],RefreshMode);
        }
        Buttons[Button]->setDown(false);
    }
    else
    {
        emit PasteXML(s,Names[Button],b->refreshmode);
    }
}

CPropertiesToolGrid::~CPropertiesToolGrid()
{
}
