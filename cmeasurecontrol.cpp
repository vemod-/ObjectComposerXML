#include "cmeasurecontrol.h"
#include "ui_cmeasurecontrol.h"

CMeasureControl::CMeasureControl(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::CMeasureControl)
{
    ui->setupUi(this);
    UpdateInches();
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(UpdateInches()));
    connect(ui->doubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(UpdateMillimeters()));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SIGNAL(Changed()));
    connect(ui->doubleSpinBox,SIGNAL(valueChanged(double)),this,SIGNAL(Changed()));
}

CMeasureControl::~CMeasureControl()
{
    delete ui;
}

const int CMeasureControl::Millimeters()
{
    return ui->spinBox->value();
}

void CMeasureControl::setMillimeters(const int MM)
{
    ui->spinBox->setValue(MM);
    UpdateInches();
}

void CMeasureControl::UpdateInches()
{
    ui->doubleSpinBox->blockSignals(true);
    ui->doubleSpinBox->setValue(ui->spinBox->value()/25.4);
    ui->doubleSpinBox->blockSignals(false);
}

void CMeasureControl::UpdateMillimeters()
{
    ui->spinBox->blockSignals(true);
    ui->spinBox->setValue((int)(ui->doubleSpinBox->value()*25.4));
    ui->spinBox->blockSignals(false);
}
