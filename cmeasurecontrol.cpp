#include "cmeasurecontrol.h"
#include "ui_cmeasurecontrol.h"

CMeasureControl::CMeasureControl(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::CMeasureControl)
{
    ui->setupUi(this);
    UpdateInches();
    connect(ui->spinBox,qOverload<int>(&QSpinBox::valueChanged),this,&CMeasureControl::UpdateInches);
    connect(ui->doubleSpinBox,qOverload<double>(&QDoubleSpinBox::valueChanged),this,&CMeasureControl::UpdateMillimeters);
    connect(ui->spinBox,qOverload<int>(&QSpinBox::valueChanged),this,&CMeasureControl::Changed);
    connect(ui->doubleSpinBox,qOverload<double>(&QDoubleSpinBox::valueChanged),this,&CMeasureControl::Changed);
}

CMeasureControl::~CMeasureControl()
{
    delete ui;
}

int CMeasureControl::Millimeters()
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
    ui->spinBox->setValue(int(ui->doubleSpinBox->value()*25.4));
    ui->spinBox->blockSignals(false);
}
