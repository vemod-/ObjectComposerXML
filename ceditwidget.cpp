#include "ceditwidget.h"
#include "ui_ceditwidget.h"
#include "CommonClasses.h"
#include "ocsymbolscollection.h"
#include <QTextStream>

CEditWidget::CEditWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CEditWidget)
{
    ui->setupUi(this);
}

CEditWidget::~CEditWidget()
{
    delete ui;
}

void CEditWidget::NoteButtonSelected(int Button)
{
    if (Button==6)
    {
        ui->DotTripletButtons->setSelected("Triplet",true);
        ui->DotTripletButtons->setEnabled("Triplet",false);
        ui->DotTripletButtons->setSelected("Dotted",false);
        ui->DotTripletButtons->setEnabled("Dotted",false);
    }
    else if (Button==5)
    {
        ui->DotTripletButtons->setSelected("Dotted",false);
        ui->DotTripletButtons->setEnabled("Dotted",false);
        ui->DotTripletButtons->setEnabled("Triplet",true);
    }
    else
    {
        ui->DotTripletButtons->setEnabled("Dotted",true);
        ui->DotTripletButtons->setEnabled("Triplet",true);
    }
}

void CEditWidget::PutNoteval(int Noteval, bool Dotted, bool Triplet)
{
    ui->NoteButtons->setSelectMode(QMacButtons::SelectOne);
    ui->NoteButtons->addButton("Whole","Whole Note",QIcon(":/Notes/Notes/0.png"));
    ui->NoteButtons->addButton("Half","Half Note",QIcon(":/Notes/Notes/1.png"));
    ui->NoteButtons->addButton("Quarter","Quarter Note",QIcon(":/Notes/Notes/2.png"));
    ui->NoteButtons->addButton("th8","8th Note",QIcon(":/Notes/Notes/3.png"));
    ui->NoteButtons->addButton("th16","16th Note",QIcon(":/Notes/Notes/4.png"));
    ui->NoteButtons->addButton("th32","32th Note",QIcon(":/Notes/Notes/5.png"));
    ui->NoteButtons->addButton("th64","64th Note",QIcon(":/Notes/Notes/6.png"));

    ui->DotTripletButtons->setSelectMode(QMacButtons::SelectOneOrNone);
    ui->DotTripletButtons->addButton("Dotted","Dotted",".");
    ui->DotTripletButtons->addButton("Triplet","Triplet","3");
    connect(ui->NoteButtons,SIGNAL(selected(int)),this,SLOT(NoteButtonSelected(int)));
    ui->NoteButtons->setSelected(Noteval,true);
    ui->DotTripletButtons->setSelected("Triplet",Triplet);
    ui->DotTripletButtons->setSelected("Dotted",Dotted);
    SetVisible(ui->Notevalue);
}


void CEditWidget::GetNoteval(int &Noteval, bool& Dotted, bool& Triplet)
{
    Noteval=ui->NoteButtons->value();
    Dotted=ui->DotTripletButtons->isSelected("Dotted");
    Triplet=ui->DotTripletButtons->isSelected("Triplet");
}

void CEditWidget::PutKey(int Key)
{
    PutCombo("Key:",Key+6, CKey::KeyList);
}

void CEditWidget::GetKey(int &Key)
{
    Key=GetCombo()-6;
}

void CEditWidget::PutSpin(QString Caption, int Value, int Min, int Max)
{
    ui->SpinLabel->setText(Caption);
    ui->Spin->setMinimum(Min);
    ui->Spin->setMaximum(Max);
    SetVisible(ui->SpingroupBox);
    ui->Spin->setValue(Value);
}

int CEditWidget::GetSpin()
{
    return ui->Spin->value();
}

void CEditWidget::PutCombo(QString Caption, int Value, QStringList List)
{
    ui->ComboLabel->setText(Caption);
    SetVisible(ui->CombogroupBox);
    ui->Combo->clear();
    ui->Combo->addItems(List);
    ui->Combo->setCurrentIndex(Value);
}

int CEditWidget::GetCombo()
{
    return ui->Combo->currentIndex();
}

void CEditWidget::PutChannel(int Channel)
{
    PutSpin("Channel:",Channel,1,16);
}

void CEditWidget::GetChannel(int &Channel)
{
    Channel=GetSpin();
}

void CEditWidget::PutAccidentals(int *Acc)
{
    SetVisible(ui->Pianoframe);
    for (int i=0; i<12; i++)
    {
        ui->Accidentals->Keys[i]=Acc[i];
    }
    ui->Accidentals->Paint();
}

void CEditWidget::GetAccidentals(int *Acc)
{
    for (int i=0; i<12; i++)
    {
        Acc[i]=ui->Accidentals->Keys[i];
    }
}

void CEditWidget::PutTime(int Type, int Upper, int Lower)
{
    SetVisible(ui->Time);
    ui->TimeUpper->setMinimum(1);
    ui->TimeUpper->setMaximum(99);
    ui->TimeUpper->setValue(Upper);
    ui->TimeType->addItems(CTime::TimeList);
    connect(ui->TimeType,SIGNAL(currentIndexChanged(int)),this,SLOT(SetTimeElements(int)));
    ui->TimeType->setCurrentIndex(Type);
    QStringList lw;
    lw << "1" << "2" << "4" << "8" << "16" << "32" << "64";
    ui->TimeLower->addItems(lw);
    ui->TimeLower->setCurrentIndex(lw.indexOf(QString::number(Lower)));
}

void CEditWidget::GetTime(int &Type, int &Upper, int &Lower)
{
    Upper=ui->TimeUpper->value();
    Lower=ui->TimeLower->currentText().toInt();
    Type=ui->TimeType->currentIndex();
}

void CEditWidget::SetTimeElements(int Value)
{
    ui->TimeUpper->setEnabled(Value==0);
    ui->TimeLower->setEnabled(Value==0);
}

void CEditWidget::PutTempo(int Tempo, int Noteval, bool Dotted)
{
    ui->TempoButtons->setSelectMode(QMacButtons::SelectOne);
    ui->TempoButtons->addButton("Whole","Whole Note",QIcon(":/Notes/Notes/0.png"));
    ui->TempoButtons->addButton("Half","Half Note",QIcon(":/Notes/Notes/1.png"));
    ui->TempoButtons->addButton("Quarter","Quarter Note",QIcon(":/Notes/Notes/2.png"));
    ui->TempoButtons->addButton("th8","8th Note",QIcon(":/Notes/Notes/3.png"));
    ui->TempoButtons->addButton("th16","16th Note",QIcon(":/Notes/Notes/4.png"));

    ui->TempoDotted->setSelectMode(QMacButtons::SelectOneOrNone);
    ui->TempoDotted->addButton("Dotted","Dotted",".");
    ui->TempoButtons->setSelected(Noteval,true);
    ui->TempoDotted->setSelected("Dotted",Dotted);
    SetVisible(ui->Tempo);
    ui->TempoSpin->setMinimum(10);
    ui->TempoSpin->setMaximum(1000);
    ui->TempoSpin->setValue(Tempo);
}

void CEditWidget::GetTempo(int &Tempo, int& Noteval, bool& Dotted)
{
    Noteval=ui->TempoButtons->value();
    Dotted=ui->TempoDotted->isSelected("Dotted");
    Tempo=ui->TempoSpin->value();
}

void CEditWidget::PutRepeats(int Repeats)
{
    PutSpin("Repeats:",Repeats,1,99);
}

void CEditWidget::GetRepeats(int &Repeats)
{
    Repeats=GetSpin();
}

void CEditWidget::PutStemDirection(int StemDirection)
{
    QStringList ListArr;
    ListArr << "Up" << "Auto" << "Down";
    PutCombo("Stem Direction:",StemDirection+1,ListArr);
}

void CEditWidget::GetStemDirection(int &StemDirection)
{
    StemDirection=GetCombo()-1;
}

void CEditWidget::PutSlanting(int Slanting)
{
    QStringList ListArr;
    ListArr << "Off" << "On";
    PutCombo("Beam Slanting:",Slanting,ListArr);
}

void CEditWidget::GetSlanting(int &Slanting)
{
    Slanting=GetCombo();
}

void CEditWidget::PutSysEx(QString Sysex)
{
    ui->EditLabel->setText("SysEx string (Including F0 and F7:");
    SetVisible(ui->EditgroupBox);
    ui->Edit->setText(Sysex);
}

void CEditWidget::PutSysEx(QDomLiteElement* data)
{
    ui->Edit->setText(data->attribute("SysExString"));
}

void CEditWidget::GetSysEx(QString &Sysex)
{
    Sysex=ui->Edit->text();
}

void CEditWidget::GetSysEx(QDomLiteElement* data)
{
    QString syx;
    GetSysEx(syx);
    data->setAttribute("SysExString",syx);
}

void CEditWidget::PutFont(QFont Font)
{
    ui->FontBox->Fill(Font,"Sample Text",true);
    SetVisible(ui->TextFrame);
}

void CEditWidget::GetFont(QFont &Font)
{
    Font=ui->FontBox->Font();
}

void CEditWidget::PutText(QString Text, QFont Font)
{
    ui->FontBox->Fill(Font,Text);
    SetVisible(ui->TextFrame);
}

void CEditWidget::PutText(QDomLiteElement* data)
{
    XMLTextWrapper XMLText(data);
    ui->FontBox->Fill(XMLText.getFont(),XMLText.getText());
    //SetVisible(ui->TextFrame);
}

void CEditWidget::GetText(QString &Text, QFont &Font)
{
    Text=ui->FontBox->Text();
    Font=ui->FontBox->Font();
}

void CEditWidget::GetText(QDomLiteElement* data)
{
    XMLTextWrapper XMLText(data);
    XMLText.setText(ui->FontBox->Text());
    XMLText.setFont(ui->FontBox->Font());
}

void CEditWidget::PutTranspose(int Transpose)
{
    PutSpin("Transpose:",Transpose,-48,48);
}

void CEditWidget::GetTranspose(int &Transpose)
{
    Transpose=GetSpin();
}

void CEditWidget::PutController(int Controller, int Value)
{
    SetVisible(ui->Controller);
    ui->ControllerCombo->clear();
    ui->ControllerCombo->addItems(CController::ControllerList);
    ui->ControllerCombo->setCurrentIndex(Controller);
    ui->ValueSpin->setMaximum(127);
    ui->ValueSpin->setMinimum(0);
    ui->ValueSpin->setValue(Value);
}

void CEditWidget::PutController(QDomLiteElement* data)
{
    ui->ControllerCombo->setCurrentIndex(data->attributeValue("Controller"));
    ui->ValueSpin->setValue(data->attributeValue("Value"));
}

void CEditWidget::GetController(int &Controller, int &Value)
{
    Controller=ui->ControllerCombo->currentIndex();
    Value=ui->ValueSpin->value();
}

void CEditWidget::GetController(QDomLiteElement* data)
{
    int Controller;
    int Value;
    GetController(Controller,Value);
    data->setAttribute("Controller",Controller);
    data->setAttribute("Value",Value);
}

void CEditWidget::SetVisible(QWidget *w)
{
    QList<QWidget*> wl;
    wl.append(ui->SpingroupBox);
    wl.append(ui->CombogroupBox);
    wl.append(ui->EditgroupBox);
    wl.append(ui->Pianoframe);
    wl.append(ui->Notevalue);
    wl.append(ui->Time);
    wl.append(ui->Tempo);
    wl.append(ui->Controller);
    wl.append(ui->TextFrame);

    for (int i=0;i<wl.count();i++)
    {
        if (wl[i] != w) wl[i]->setVisible(false);
    }
    w->setVisible(true);

    this->setMinimumSize(w->width(),w->height());
    //this->setMaximumSize(w->width(),w->height());
    //this->setGeometry(QRect(0,0,w->width(),w->height()));
    w->move(0,0);
    //w->setGeometry(QRect(0,0,w->width(),w->height()));
}
