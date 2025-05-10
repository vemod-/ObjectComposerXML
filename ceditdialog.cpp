#include "ceditdialog.h"
#include "csymbol.h"
#include "ui_ceditdialog.h"
//#include <QSettings>

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
    OCSettings s;
    if (XML.documentElement->childCount()) s.setValue("OCStuff",XML.toString()); //XML.save(settingsfile);
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
    for (const QDomLiteElement* e : std::as_const(theNode->childElements)) sl.append(e->attribute("Name"));
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
        XMLTextElementWrapper t(add);
        ui->EditWidget->GetText(t);
    }
    auto item=new QListWidgetItem(Name);
    item->setFlags( item->flags() | Qt::ItemIsEditable);
    ui->listWidget->addItem(item);
    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
    ui->listWidget->editItem(ui->listWidget->currentItem());
    //ui->SaveItem->setEnabled(ui->listWidget->currentRow()>-1);
}

void CEditDialog::RemoveItem()
{
    if (ui->listWidget->currentRow()>-1)
    {
        theNode->removeChild(ui->listWidget->currentRow());
        ui->listWidget->takeItem(ui->listWidget->currentRow());
        SetItem(ui->listWidget->currentRow());
    }
    //ui->SaveItem->setEnabled(ui->listWidget->currentRow()>-1);
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
            item->setAttribute("Text",item->attribute("Name"));
            ui->EditWidget->PutText(XMLTextElementWrapper(item));
        }
    }
    //ui->SaveItem->setEnabled(Index>-1);
}

void CEditDialog::RenameItem(QListWidgetItem* item)
{
    if (ui->listWidget->currentRow()>-1)
    {
        if (item->text() != theNode->childElement(ui->listWidget->currentRow())->attribute("Name"))
        {
            theNode->childElement(ui->listWidget->currentRow())->setAttribute("Name",item->text());
            SetItem(ui->listWidget->currentRow());
        }
    }
}

void CEditDialog::AcceptItem(QListWidgetItem* item)
{
    ui->listWidget->setCurrentItem(item);
    if (ui->listWidget->currentRow()>-1)
    {
        SetItem(ui->listWidget->currentRow());
        accept();
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
            XMLTextElementWrapper t(item);
            ui->EditWidget->GetText(t);
            ui->listWidget->currentItem()->setText(t.text());
        }
    }
    //XML.save(settingsfile);
}
/*
void CEditDialog::HideList()
{
    XML.save(settingsfile);
}
*/
void CEditDialog::ShowList(const QString& TopNode)
{
    XML.clear("OCStuff","Stuff");
    //XML.load(settingsfile);
    OCSettings s;
    XML.fromString(s.value("OCStuff").toString());
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
    for (const QDomLiteElement* e : std::as_const(theNode->childElements))
    {
        QListWidgetItem* item=new QListWidgetItem(e->attribute("Name"));
        item->setFlags( item->flags() | Qt::ItemIsEditable);
        ui->listWidget->addItem(item);
    }
    connect(ui->AddItem,&QAbstractButton::clicked,this,&CEditDialog::AddItem);
    connect(ui->RemoveItem,&QAbstractButton::clicked,this,&CEditDialog::RemoveItem);
    connect(ui->listWidget,&QListWidget::itemChanged,this,&CEditDialog::RenameItem);
    connect(ui->listWidget,&QListWidget::currentRowChanged,this,&CEditDialog::SetItem);
    connect(ui->EditWidget,&CEditWidget::Changed,this,&CEditDialog::SaveItem);
    connect(ui->listWidget,&QListWidget::itemDoubleClicked,this,&CEditDialog::AcceptItem);
}

void CEditDialog::QuickAccept(bool hide)
{
    if (hide) ui->buttonBox->hide();
    connect(ui->EditWidget,&CEditWidget::Changed,this,&QDialog::accept);
}
