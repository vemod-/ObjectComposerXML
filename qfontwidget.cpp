#include "qfontwidget.h"
#include "ui_qfontwidget.h"

QFontWidget::QFontWidget(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::QFontWidget)
{
    ui->setupUi(this);
    ui->lineEdit->setText("Sample Text");
    ui->lineEdit->setReadOnly(true);
    ui->lineEdit->setAlignment(Qt::AlignCenter);
    UpdateLineEdit();
    connect(ui->fontComboBox,SIGNAL(currentFontChanged(QFont)),this,SLOT(UpdateLineEdit()));
    connect(ui->doubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(UpdateLineEdit()));
    connect(ui->toolButton,SIGNAL(toggled(bool)),this,SLOT(UpdateLineEdit()));
    connect(ui->toolButton_2,SIGNAL(toggled(bool)),this,SLOT(UpdateLineEdit()));
}

QFontWidget::~QFontWidget()
{
    delete ui;
}

void QFontWidget::Fill(const QFont& Font, const QString& Text, const bool Locked, const Qt::Alignment Align, const float Scale)
{
    ui->lineEdit->setText(Text);
    ui->lineEdit->setReadOnly(Locked);
    ui->lineEdit->setAlignment(Align);
    ui->fontComboBox->blockSignals(true);
    ui->fontComboBox->setCurrentFont(Font);
    ui->fontComboBox->blockSignals(false);
    ui->doubleSpinBox->blockSignals(true);
    ui->doubleSpinBox->setValue(Font.pointSizeF()*Scale);
    m_Scale=Scale;
    ui->doubleSpinBox->blockSignals(false);
    ui->toolButton->blockSignals(true);
    ui->toolButton->setChecked(Font.bold());
    ui->toolButton->blockSignals(false);
    ui->toolButton_2->blockSignals(true);
    ui->toolButton_2->setChecked(Font.italic());
    ui->toolButton_2->blockSignals(false);
    UpdateLineEdit();
}

void QFontWidget::UpdateLineEdit()
{
    QFont f(ui->fontComboBox->currentFont());
    f.setPointSizeF(ui->doubleSpinBox->value());
    f.setBold(ui->toolButton->isChecked());
    f.setItalic(ui->toolButton_2->isChecked());
    ui->lineEdit->setFont(f);
}

const QFont QFontWidget::Font()
{
    QFont f=ui->lineEdit->font();
    f.setPointSizeF(f.pointSizeF()/m_Scale);
    return f;
}

const QString QFontWidget::Text()
{
    return ui->lineEdit->text();
}
