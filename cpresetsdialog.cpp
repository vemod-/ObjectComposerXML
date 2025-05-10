#include "cpresetsdialog.h"
#include "ui_cpresetsdialog.h"

CPresetsDialog::CPresetsDialog(QWidget *parent) :
        QDialog(parent,Qt::Sheet),
    ui(new Ui::CPresetsDialog)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
}

CPresetsDialog::~CPresetsDialog()
{
    delete ui;
}
