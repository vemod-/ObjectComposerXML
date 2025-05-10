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
    auto bb=findChild<QDialogButtonBox*>();
    QPushButton* pb = bb->button(QDialogButtonBox::Ok);
    for (int i=0;i<ui->staffList->count();i++)
    {
        QListWidgetItem* im=ui->staffList->item(i);
        if (im->checkState()==Qt::Checked)
        {
            pb->setEnabled(true);
            return;
        }
    }
    pb->setEnabled(false);
}

void CEditSystem::Fill(const XMLTemplateWrapper& LayoutTemplate, const XMLTemplateWrapper& SystemTemplate, const XMLScoreWrapper& XMLScore)
{
    for (int StaffPos=0;StaffPos<LayoutTemplate.staffCount();StaffPos++)
    {
        const int StaffIndex=LayoutTemplate.staffId(StaffPos);
        QListWidgetItem* item=new QListWidgetItem(XMLScore.StaffName(StaffIndex));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        for (int j=0;j<SystemTemplate.staffCount();j++)
        {
            if (SystemTemplate.staffId(j) == StaffIndex) item->setCheckState(Qt::Checked);
        }
        ui->staffList->addItem(item);
        connect(ui->staffList,&QListWidget::itemChanged,this,&CEditSystem::Validate);
    }
    Validate(ui->staffList->item(0));
}

void CEditSystem::GetTemplate(const XMLTemplateWrapper& LayoutTemplate, XMLTemplateWrapper& SystemTemplate)
{
    SystemTemplate.clear();
    for (int i=0;i<ui->staffList->count();i++)
    {
        QListWidgetItem* item=ui->staffList->item(i);
        if (item->checkState()==Qt::Checked)
        {
            SystemTemplate.addChild(LayoutTemplate.staff(i));
        }
    }
    SystemTemplate.validateBrackets();
}
