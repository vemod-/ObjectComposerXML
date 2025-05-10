#include "cscorewizard.h"
#include "ui_cscorewizard.h"
#include <QFileDialog>

CScoreWizard::CScoreWizard(QWidget *parent) :
        QDialog(parent,Qt::Sheet),
        ui(new Ui::CScoreWizard)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    auto bb=findChild<QDialogButtonBox*>();
    QPushButton* pb = bb->button(QDialogButtonBox::Ok);
    pb->setText("Create Score");
    ui->Key->PutKey(0);
    ui->Time->PutTime(0,4,4);
    ui->Accidentals->PutAccidentals(Keys.data());
}

CScoreWizard::~CScoreWizard()
{
    delete ui;
}

void CScoreWizard::AppendFile()
{
    QFileDialog d(this,Qt::Sheet);
    d.setWindowTitle("Open Score");
    d.setNameFilter(tr("XML Score Files (*.mus *.xml)"));
    if (d.exec()==QDialog::Accepted) for (const QString& s : (const QStringList)d.selectedFiles()) Open(s);
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
    for (int i=0;i<XMLScore.NumOfStaffs();i++)
    {
        auto item=new QListWidgetItem();
        item->setText(XMLScore.StaffName(i));
        item->setData(wLWName,XMLScore.StaffName(i));
        item->setData(wLWAbbreviation,XMLScore.StaffAbbreviation(i));
        item->setData(LWNumOfVoices,XMLScore.NumOfVoices(i));
        int p=0;
        while (p < XMLScore.Voice(i,0).symbolCount())
        {
            const XMLSimpleSymbolWrapper& Symbol=XMLScore.Voice(i,0).XMLSimpleSymbol(p);
            if (Symbol.IsRestOrValuedNote())
            {
                break;
            }
            /*
            else if (Symbol.IsEndOfVoice())
            {
                break;
            }
            */
            if (Symbol.Compare("Transpose"))
            {
                item->setData(LWTranspose,Symbol.getIntVal("Transpose"));
            }
            else if (Symbol.Compare("Clef"))
            {
                item->setData(LWClef,Symbol.getIntVal("Clef"));
            }
            else if (Symbol.Compare("Controller"))
            {
                if (Symbol.getIntVal("Controller")==10) item->setData(LWPan,Symbol.getIntVal("Value"));
            }
            p++;
        }
        ui->StaffListLeft->addItem(item);
    }
    ValidateStaffs();
}

QDomLiteDocument* CScoreWizard::CreateXML()
{
    int KeyNum;
    int TimeType;
    int Upper;
    int Lower;
    ui->Key->GetKey(KeyNum);
    ui->Time->GetTime(TimeType,Upper,Lower);
    ui->Accidentals->GetAccidentals(Keys.data());

    XMLScoreWrapper XMLScore;
    XMLScore.newScore();
    for (int i=0;i<ui->StaffListRight->count();i++)
    {
        QListWidgetItem* item=ui->StaffListRight->item(i);
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
            XMLVoiceWrapper Voice=XMLScore.Voice(i,j);
            Voice.addChild(createSymbol("Clef","Clef",item->data(LWClef).toInt()));
            if (item->data(LWTranspose).toInt())
            {
                 Voice.addChild(createSymbol("Transpose","Transpose",item->data(LWTranspose).toInt()));
            }
            if (item->data(LWClef).toInt()<4)
            {
                Voice.addChild(createSymbol("Key","Key",KeyNum+6));
            }
            Voice.addChild(createSymbol("Time",{"Upper","Lower","TimeType"},{Upper,Lower,TimeType}));
            XMLSimpleSymbolWrapper Scale=createSymbol("Scale");
            for (uint i1=0;i1<12;i1++)
            {
                Scale.setAttribute("Step"+QString::number(i1+1),Keys[i1]);
            }
            Voice.addChild(Scale);
        }
    }
    return XMLScore.getXML()->clone();
}

void CScoreWizard::MoveRight()
{
    int i=0;
    while (i<ui->StaffListLeft->count())
    {
        if (ui->StaffListLeft->item(i)->isSelected())
        {
            ui->StaffListRight->addItem(ui->StaffListLeft->takeItem(i));
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
    int i=0;
    while (i<ui->StaffListRight->count())
    {
        if (ui->StaffListRight->item(i)->isSelected())
        {
            ui->StaffListLeft->addItem(ui->StaffListRight->takeItem(i));
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
    while (ui->StaffListLeft->count())
    {
        ui->StaffListRight->addItem(ui->StaffListLeft->takeItem(0));
    }
    ValidateStaffs();
}

void CScoreWizard::MoveAllLeft()
{
    while (ui->StaffListRight->count())
    {
        ui->StaffListLeft->addItem(ui->StaffListRight->takeItem(0));
    }
    ValidateStaffs();
}

void CScoreWizard::ValidateStaffs()
{
    auto bb=findChild<QDialogButtonBox*>();
    QPushButton* pb = bb->button(QDialogButtonBox::Ok);
    pb->setEnabled(ui->StaffListRight->count()>0);
}
