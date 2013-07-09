#include "ceditdialog.h"
#include "ui_ceditdialog.h"

CEditDialog::CEditDialog(QWidget *parent) :
        QDialog(parent,Qt::Sheet),
    ui(new Ui::CEditDialog)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    EditWidget=ui->EditWidget;
    InitLayout(false);
}

CEditDialog::~CEditDialog()
{
    delete ui;
}

void CEditDialog::InitLayout(bool HasList)
{
    ui->Listframe->setVisible(HasList);
}

void CEditDialog::AddItem()
{
    QString Name=tr("New")+" "+TopNode;
    int c=1;
    QStringList sl;
    for (int i=0;i<theNode->childCount();i++) sl.append(theNode->childElement(i)->attribute("Name"));
    while (sl.contains(Name)) Name=tr("New")+" "+TopNode+" - "+QString::number(c++);
    QDomLiteElement* add=theNode->appendChild(TopNode+"Element");
    add->setAttribute("Name",Name);
    if (TopNode=="SysEx")
    {
        ui->EditWidget->GetSysEx(add);
    }
    else if (TopNode=="Controller")
    {
        ui->EditWidget->GetController(add);
    }
    else if (TopNode=="Text")
    {
        ui->EditWidget->GetText(add);
    }
    QListWidgetItem* item=new QListWidgetItem(Name);
    item->setFlags( item->flags() | Qt::ItemIsEditable);
    ui->listWidget->addItem(item);
    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
    ui->listWidget->editItem(ui->listWidget->currentItem());
    ui->SaveItem->setEnabled(ui->listWidget->currentRow()>-1);
}

void CEditDialog::RemoveItem()
{
    if (ui->listWidget->currentRow()>-1)
    {
        theNode->removeChild(ui->listWidget->currentRow());
        ui->listWidget->takeItem(ui->listWidget->currentRow());
        SetItem(ui->listWidget->currentRow());
    }
    ui->SaveItem->setEnabled(ui->listWidget->currentRow()>-1);
}

void CEditDialog::SetItem(int Index)
{
    if (Index>-1)
    {
        QDomLiteElement* item=theNode->childElement(Index);
        if (TopNode=="SysEx")
        {
            ui->EditWidget->PutSysEx(item);
        }
        else if (TopNode=="Controller")
        {
            ui->EditWidget->PutController(item);
        }
        else if (TopNode=="Text")
        {
            ui->EditWidget->PutText(item);
        }
    }
    ui->SaveItem->setEnabled(Index>-1);
}

void CEditDialog::RenameItem(QListWidgetItem* item)
{
    if (ui->listWidget->currentRow()>-1)
    {
        if (item->text() != theNode->childElement(ui->listWidget->currentRow())->attribute("Name"))
        {
            theNode->childElement(ui->listWidget->currentRow())->setAttribute("Name",item->text());
        }
    }
}

void CEditDialog::SaveItem()
{
    if (ui->listWidget->currentRow()>-1)
    {
        QDomLiteElement* item=theNode->childElement(ui->listWidget->currentRow());
        if (TopNode=="SysEx")
        {
            ui->EditWidget->GetSysEx(item);
        }
        else if (TopNode=="Controller")
        {
            ui->EditWidget->GetController(item);
        }
        else if (TopNode=="Text")
        {
            ui->EditWidget->GetText(item);
        }
    }
    XML.save(settingsfile);
}

void CEditDialog::HideList()
{
    XML.save(settingsfile);
}

void CEditDialog::ShowList(const QString& TopNode)
{
    XML.clear("OCStuff","Stuff");
    XML.load(settingsfile);
    this->TopNode=TopNode;
    InitLayout(true);
    if (XML.documentElement->childCount(TopNode)==0)
    {
        theNode=XML.documentElement->appendChild(TopNode);
        if (TopNode=="Controller")
        {
            QDomLiteElement* ctl=theNode->appendChild(TopNode+"Element");
            ctl->setAttribute("Name","Pan");
            ctl->setAttribute("Controller",10);
            ctl->setAttribute("Value",80);
        }
    }
    else
    {
        theNode=XML.documentElement->elementByTag(TopNode);
    }
    for (int i=0;i<theNode->childCount();i++)
    {
        QListWidgetItem* item=new QListWidgetItem(theNode->childElement(i)->attribute("Name"));
        item->setFlags( item->flags() | Qt::ItemIsEditable);
        ui->listWidget->addItem(item);
    }
    ui->SaveItem->setEnabled(ui->listWidget->currentRow()>-1);
    connect(ui->AddItem,SIGNAL(clicked()),this,SLOT(AddItem()));
    connect(ui->RemoveItem,SIGNAL(clicked()),this,SLOT(RemoveItem()));
    connect(ui->listWidget,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(RenameItem(QListWidgetItem*)));
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(SetItem(int)));
    connect(this,SIGNAL(accepted()),this,SLOT(HideList()));
    connect(ui->SaveItem,SIGNAL(clicked()),this,SLOT(SaveItem()));
}
