#include "cscorewizard.h"
#include "ui_cscorewizard.h"
#include <QFileDialog>

CScoreWizard::CScoreWizard(QWidget *parent) :
        QDialog(parent,Qt::Sheet),
    ui(new Ui::CScoreWizard)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    QDialogButtonBox* bb=findChild<QDialogButtonBox*>();
    QPushButton* pb = bb->button(QDialogButtonBox::Ok);
    pb->setText("Create Score");
    CEditWidget* Key=findChild<CEditWidget*>("Key");
    Key->PutKey(0);
    CEditWidget* Time=findChild<CEditWidget*>("Time");
    Time->PutTime(0,4,4);
    for (int i=0;i<12;i++)
    {
        Keys[i]=0;
    }
    CEditWidget* Accidentals=findChild<CEditWidget*>("Accidentals");
    Accidentals->PutAccidentals(&Keys[0]);
}

CScoreWizard::~CScoreWizard()
{
    delete ui;
}

void CScoreWizard::AppendFile()
{
    QFileDialog d(this,Qt::Sheet);
    //d.setWindowModality(Qt::WindowModal);
    d.setWindowTitle("Open Score");
    d.setNameFilter(tr("XML Score Files (*.mus *.xml)"));
    if (d.exec()==QDialog::Accepted) foreach(QString s,d.selectedFiles()) Open(s);
    ValidateStaffs();
}

void CScoreWizard::Open(const QString& Path)
{
    XMLScoreWrapper Score;
    Score.Load(Path);
    Fill(Score);
    //delete Score;
}

void CScoreWizard::Fill(XMLScoreWrapper& XMLScore)
{
    QListWidget* Left=findChild<QListWidget*>("StaffListLeft");
    for (int i=0;i<XMLScore.Template(0)->childCount();i++)
    {
        QListWidgetItem* item=new QListWidgetItem();
        item->setText(XMLScore.StaffName(i));
        item->setData(wLWName,XMLScore.StaffName(i));
        item->setData(wLWAbbreviation,XMLScore.StaffAbbreviation(i));
        item->setData(LWNumOfVoices,XMLScore.NumOfVoices(i));
        int p=0;
        forever
        {
            XMLSimpleSymbolWrapper Symbol(XMLScore.Voice(i,0),p);
            if (Symbol.IsRestOrValuedNote())
            {
                break;
            }
            else if (Symbol.IsEndOfVoice())
            {
                break;
            }
            else if (Symbol.Compare("Transpose"))
            {
                item->setData(LWTranspose,Symbol.getVal("Transpose"));
            }
            else if (Symbol.Compare("Clef"))
            {
                item->setData(LWClef,Symbol.getVal("Clef"));
            }
            else if (Symbol.Compare("Controller"))
            {
                if (Symbol.getVal("Controller")==10) item->setData(LWPan,Symbol.getVal("Value"));
            }
            p++;
        }
        Left->addItem(item);
    }
    ValidateStaffs();
}

QDomLiteDocument* CScoreWizard::CreateXML()
{
    int Key;
    int TimeType;
    int Upper;
    int Lower;
    CEditWidget* k=findChild<CEditWidget*>("Key");
    k->GetKey(Key);
    CEditWidget* t=findChild<CEditWidget*>("Time");
    t->GetTime(TimeType,Upper,Lower);
    CEditWidget* a=findChild<CEditWidget*>("Accidentals");
    a->GetAccidentals(&Keys[0]);

    XMLScoreWrapper XMLScore;
    XMLScore.newScore();
    QListWidget* Right=findChild<QListWidget*>("StaffListRight");
    for (int i=0;i<Right->count();i++)
    {
        QListWidgetItem* item=Right->item(i);
        if (i>0)
        {
            XMLScore.AddStaff(i,item->data(wLWName).toString());
        }
        else
        {
            XMLScore.setStaffName(i,item->data(wLWName).toString());
        }
        XMLScore.setStaffAbbreviation(i,item->data(wLWAbbreviation).toString());
        for (int j=0;j<item->data(LWNumOfVoices).toInt();j++)
        {
            if (j>0) XMLScore.AddVoice(i,j);
            QDomLiteElement* Voice=XMLScore.Voice(i,j);
            QDomLiteElement* EOV=Voice->firstChild();
            XMLSimpleSymbolWrapper Symbol=OCSymbolsCollection::GetSymbol("Clef");
            Symbol.setAttribute("Clef",item->data(LWClef).toInt());
            Voice->insertChild(Symbol.getXML(),EOV);
            /*
            if (item->data(LWPan).toInt())
            {
                QDomElement Symbol=OCSymbolsCollection::GetProperties("Controller")->ToXML();
                SetAttribute(Symbol,"Controller",10);
                SetAttribute(Symbol,"Value",item->data(LWPan).toInt());
                Voice.insertBefore(Symbol,EOV);
            }
            */
            if (item->data(LWTranspose).toInt())
            {
                Symbol=OCSymbolsCollection::GetSymbol("Transpose");
                Symbol.setAttribute("Transpose",item->data(LWTranspose).toInt());
                Voice->insertChild(Symbol.getXML(),EOV);
            }
            if (item->data(LWClef).toInt()<4)
            {
                Symbol=OCSymbolsCollection::GetSymbol("Key");
                Symbol.setAttribute("Key",Key+6);
                Voice->insertChild(Symbol.getXML(),EOV);
            }
            Symbol=OCSymbolsCollection::GetSymbol("Time");
            Symbol.setAttribute("Upper",Upper);
            Symbol.setAttribute("Lower",Lower);
            Symbol.setAttribute("TimeType",TimeType);
            Voice->insertChild(Symbol.getXML(),EOV);
            Symbol=OCSymbolsCollection::GetSymbol("Scale");
            for (int i1=0;i1<12;i1++)
            {
                Symbol.setAttribute("Step"+QString::number(i1+1),Keys[i1]);
            }
            Voice->insertChild(Symbol.getXML(),EOV);
        }
    }
    return XMLScore.getXML()->clone();
}

void CScoreWizard::MoveRight()
{
    QListWidget* Left=findChild<QListWidget*>("StaffListLeft");
    QListWidget* Right=findChild<QListWidget*>("StaffListRight");
    int i=0;
    while (i<Left->count())
    {
        if (Left->item(i)->isSelected())
        {
            Right->addItem(Left->takeItem(i));
        }
        else
        {
            i++;
        }
    }
    ValidateStaffs();
}

void CScoreWizard::MoveLeft()
{
    QListWidget* Left=findChild<QListWidget*>("StaffListLeft");
    QListWidget* Right=findChild<QListWidget*>("StaffListRight");
    int i=0;
    while (i<Right->count())
    {
        if (Right->item(i)->isSelected())
        {
            Left->addItem(Right->takeItem(i));
        }
        else
        {
            i++;
        }
    }
    ValidateStaffs();
}

void CScoreWizard::MoveAllRight()
{
    QListWidget* Left=findChild<QListWidget*>("StaffListLeft");
    QListWidget* Right=findChild<QListWidget*>("StaffListRight");
    int i=0;
    while (i<Left->count())
    {
        Right->addItem(Left->takeItem(i));
    }
    ValidateStaffs();
}

void CScoreWizard::MoveAllLeft()
{
    QListWidget* Left=findChild<QListWidget*>("StaffListLeft");
    QListWidget* Right=findChild<QListWidget*>("StaffListRight");
    int i=0;
    while (i<Right->count())
    {
        Left->addItem(Right->takeItem(i));
    }
    ValidateStaffs();
}

void CScoreWizard::ValidateStaffs()
{
    QListWidget* Right=findChild<QListWidget*>("StaffListRight");
    QDialogButtonBox* bb=findChild<QDialogButtonBox*>();
    QPushButton* pb = bb->button(QDialogButtonBox::Ok);
    pb->setEnabled(Right->count()>0);
}
