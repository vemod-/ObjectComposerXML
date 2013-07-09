#include "cpresetsdialog.h"
#include "ui_cpresetsdialog.h"

CPresetsDialog::CPresetsDialog(QWidget *parent) :
        QDialog(parent,Qt::Sheet),
    ui(new Ui::CPresetsDialog)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    //editor1 = new QtTreePropertyBrowser();
    //this->layout()->addWidget(editor1);
}

CPresetsDialog::~CPresetsDialog()
{
    delete ui;
}
