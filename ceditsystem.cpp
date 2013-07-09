#include "ceditsystem.h"
#include "ui_ceditsystem.h"
#include <QPushButton>

CEditSystem::CEditSystem(QWidget *parent) :
        QDialog(parent,Qt::Sheet),
    ui(new Ui::CEditSystem)
{
    this->setWindowModality(Qt::WindowModal);
    ui->setupUi(this);
}

CEditSystem::~CEditSystem()
{
    delete ui;
}

void CEditSystem::Validate(QListWidgetItem* item)
{
    Q_UNUSED(item);
    QListWidget* lw=findChild<QListWidget*>();
    QDialogButtonBox* bb=findChild<QDialogButtonBox*>();
    QPushButton* pb = bb->button(QDialogButtonBox::Ok);
    for (int i=0;i<lw->count();i++)
    {
        QListWidgetItem* im=lw->item(i);
        if (im->checkState()==Qt::Checked)
        {
            pb->setEnabled(true);
            return;
        }
    }
    pb->setEnabled(false);
}

void CEditSystem::Fill(QDomLiteElement *LayoutTemplate, QDomLiteElement *SystemTemplate)
{
    QListWidget* lw=findChild<QListWidget*>();
    for (int i=0;i<LayoutTemplate->childCount();i++)
    {
        QListWidgetItem* item=new QListWidgetItem(XMLScoreWrapper::StaffName(LayoutTemplate,i));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        int index=XMLScoreWrapper::AllTemplateIndex(LayoutTemplate,i);
        item->setCheckState(Qt::Unchecked);
        for (int j=0;j<SystemTemplate->childCount();j++)
        {
            if (XMLScoreWrapper::AllTemplateIndex(SystemTemplate,j) == index) item->setCheckState(Qt::Checked);
        }
        lw->addItem(item);
        connect(lw,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(Validate(QListWidgetItem*)));
    }
    Validate(lw->item(0));
}

void CEditSystem::GetTemplate(QDomLiteElement *LayoutTemplate, QDomLiteElement *SystemTemplate)
{
    QListWidget* lw=findChild<QListWidget*>();
    SystemTemplate->clearChildren();
    for (int i=0;i<lw->count();i++)
    {
        QListWidgetItem* item=lw->item(i);
        if (item->checkState()==Qt::Checked)
        {
            SystemTemplate->appendClone(LayoutTemplate->childElement(i));
        }
    }
    //UpdateIndexes(SystemTemplate);
    XMLScoreWrapper::ValidateBrackets(SystemTemplate);
}
