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
    updateLineEdit();
    connect(ui->fontComboBox,&QFontComboBox::currentFontChanged,this,&QFontWidget::updateLineEdit);
    connect(ui->doubleSpinBox,qOverload<double>(&QDoubleSpinBox::valueChanged),this,&QFontWidget::updateLineEdit);
    connect(ui->toolButton,&QAbstractButton::toggled,this,&QFontWidget::updateLineEdit);
    connect(ui->toolButton_2,&QAbstractButton::toggled,this,&QFontWidget::updateLineEdit);
    connect(ui->lineEdit,&QLineEdit::textChanged,this,&QFontWidget::Changed);
}

QFontWidget::~QFontWidget()
{
    delete ui;
}

void QFontWidget::fill(const QFont& Font, const QString& Text, const bool Locked, const Qt::Alignment Align, const double Scale)
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
    updateLineEdit();
}

void QFontWidget::updateLineEdit()
{
    QFont f(ui->fontComboBox->currentFont());
    f.setPointSizeF(ui->doubleSpinBox->value());
    f.setBold(ui->toolButton->isChecked());
    f.setItalic(ui->toolButton_2->isChecked());
    ui->lineEdit->setFont(f);
    emit Changed();
}

const QFont QFontWidget::font()
{
    QFont f=ui->lineEdit->font();
    f.setPointSizeF(f.pointSizeF()/m_Scale);
    return f;
}

const QString QFontWidget::text()
{
    return ui->lineEdit->text();
}
