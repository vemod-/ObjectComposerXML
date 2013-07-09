#include "ocnotetoolbox.h"
#include "ui_ocnotetoolbox.h"
#include "cpatternlist.h"

OCNoteToolbox::OCNoteToolbox(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::OCNoteToolbox)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    DotTimes=0;
    TripletTimes=0;
    RecordCount=0;
    isRecording=false;
    this->setFixedHeight(36);
    this->hide();
    ui->NoteButtons->setSelectMode(QMacButtons::SelectOne);
    ui->NoteButtons->setFixedWidth(36*7);
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/0.png"),"Whole Note",this),QKeySequence(tr("Alt+1")));
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/1.png"),"Half Note",this),QKeySequence(tr("Alt+2")));
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/2.png"),"Quarter Note",this),QKeySequence(tr("Alt+3")));
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/3.png"),"8th Note",this),QKeySequence(tr("Alt+4")));
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/4.png"),"16th Note",this),QKeySequence(tr("Alt+5")));
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/5.png"),"32th Note",this),QKeySequence(tr("Alt+6")));
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/6.png"),"64th Note",this),QKeySequence(tr("Alt+7")));
    connect(ui->NoteButtons,SIGNAL(selected(int)),this,SLOT(SelectNote(int)));
    ui->NoteButtons->setSelected(2,true);

    ui->DotButtons->setFixedWidth(36*3);
    ui->DotButtons->setSelectMode(QMacButtons::SelectOneOrNone);
    ui->DotButtons->addButton(new QAction(QIcon(":/Notes/Notes/dotx.png"),"Dot following Notes",this),QKeySequence(tr("Alt+8")));
    ui->DotButtons->addButton("Dot1","Dot the following Note",QIcon(":/Notes/Notes/dot1.png"));//8
    ui->DotButtons->addButton("Dot2","Dot the 2 following Notes",QIcon(":/Notes/Notes/dot2.png"));//9
    connect(ui->DotButtons,SIGNAL(selected(int)),this,SLOT(SelectDotButton(int)));

    ui->TripletButtons->setFixedWidth(36*3);
    ui->TripletButtons->setSelectMode(QMacButtons::SelectOneOrNone);
    ui->TripletButtons->addButton(new QAction(QIcon(":/Notes/Notes/triplet.png"),"Triplet following Notes",this),QKeySequence(tr("Alt+9")));
    ui->TripletButtons->addButton("Triplet2","Triplet the 2 following Notes",QIcon(":/Notes/Notes/triplet2.png"));//11
    ui->TripletButtons->addButton("Triplet3","Triplet the 3 following Notes",QIcon(":/Notes/Notes/triplet3.png"));//12
    connect(ui->TripletButtons,SIGNAL(selected(int)),this,SLOT(SelectTripletButton(int)));

    ui->PauseButtons->setFixedWidth(36*2);
    ui->PauseButtons->setSelectMode(QMacButtons::SelectNone);
    ui->PauseButtons->addButton(new QAction(QIcon(":/Notes/Notes/quarterrest.png"),"Rest",this),QKeySequence(tr("Alt+0")));
    ui->PauseButtons->addButton(new QAction(QIcon(":/Notes/Notes/wholerest.png"),"Whole Bar Rest",this),QKeySequence(tr("Alt++")));
    connect(ui->PauseButtons,SIGNAL(buttonClicked(int)),this,SLOT(PauseButtonClicked(int)));

    ui->PatternButtons->setFixedWidth(36*3);
    ui->PatternButtons->setSelectMode(QMacButtons::SelectOne);
    ui->PatternButtons->addButton("Record","Start Recording Rhythm Pattern",QIcon(":/24/24/record.png"));//15
    ui->PatternButtons->addButton("Play","Start Applying Rhythm Pattern",QIcon(":/24/24/play.png"));//16
    ui->PatternButtons->addButton("Stop","Stop Recording/Applying Rhythm Pattern",QIcon(":/24/24/stop.png"));//17
    ui->PatternButtons->setMonochrome(false);
    connect(ui->PatternButtons,SIGNAL(selected(int)),this,SLOT(SelectPatternButton(int)));
    ui->PatternButtons->setSelected("Stop",true);
    ui->PatternButtons->setEnabled("Play",false);
    //ui->PatternButtons->setEnabled(2,false);

    ui->SelectPatternButtons->setFixedWidth(36);
    ui->SelectPatternButtons->setSelectMode(QMacButtons::SelectNone);
    ui->SelectPatternButtons->addButton("Patterns","Select Saved Rhythm Pattern",QIcon(":/24/24/preferences.png")); //18
    ui->SelectPatternButtons->setMonochrome(false);
    connect(ui->SelectPatternButtons,SIGNAL(buttonClicked(int)),this,SLOT(PatternSelectClicked(int)));
    this->show();
}

OCNoteToolbox::~OCNoteToolbox()
{
    delete ui;
}

void OCNoteToolbox::SelectNote(int value)
{
    if (value==6)
    {
        ui->TripletButtons->setSelected(0,true);
        ui->TripletButtons->setEnabled(false);
        ui->DotButtons->setSelected(false);
        ui->DotButtons->setEnabled(false);
    }
    else if (value==5)
    {
        ui->DotButtons->setSelected(false);
        ui->DotButtons->setEnabled(false);
        ui->TripletButtons->setEnabled(true);
    }
    else
    {
        ui->DotButtons->setEnabled(true);
        ui->TripletButtons->setEnabled(true);
    }

}

void OCNoteToolbox::SelectDotButton(int value)
{
    ui->TripletButtons->setSelected(false);
    DotTimes=value;
}

void OCNoteToolbox::SelectTripletButton(int value)
{
    ui->DotButtons->setSelected(false);
    TripletTimes=value;
    if (TripletTimes>0) TripletTimes++;
}

void OCNoteToolbox::SelectPatternButton(int value)
{
    if (value==0)
    {
        ui->PatternButtons->setEnabled("Play",false);
        RecordList.clear();
        //ui->PatternButtons->setEnabled("Stop",true);
        isRecording=true;
    }
    if (value==1)
    {
        if (RecordList.count())
        {
            if (isRecording)
            {
                CPatternList p(this);
                p.AppendPattern(RecordList);
                isRecording=false;
            }
            //ui->PatternButtons->setEnabled("Stop",true);
            RecordCount=0;
            DecrementTimes();
        }
    }
    if (value==2)
    {
        if (RecordList.count())
        {
            if (isRecording)
            {
                CPatternList p(this);
                p.AppendPattern(RecordList);
                isRecording=false;
            }
        }
        //ui->PatternButtons->setEnabled("Stop",false);
    }
}

void OCNoteToolbox::PauseButtonClicked(int value)
{
    if (value==0)
    {
        XMLSimpleSymbolWrapper p=OCSymbolsCollection::GetSymbol("Rest");
        p.setAttribute("NoteValue",ui->NoteButtons->value());
        p.setAttribute("Triplet",ui->TripletButtons->isSelected());
        p.setAttribute("Dotted",ui->DotButtons->isSelected());
        emit PasteXML(p,"Rest",true);
        DecrementTimes();
    }
    if (value==1)
    {
        XMLSimpleSymbolWrapper p=OCSymbolsCollection::GetSymbol("Rest");
        p.setAttribute("NoteValue",7);
        emit PasteXML(p,"Rest",true);
        DecrementTimes();
    }
}

void OCNoteToolbox::PatternSelectClicked(int value)
{
    Q_UNUSED(value);
    CPatternList* p=new CPatternList(this);
    if (RecordList.count())
    {
        if (isRecording)
        {
            p->AppendPattern(RecordList);
            isRecording=false;
        }
    }
    ui->PatternButtons->setSelected("Stop",true);
    //ui->PatternButtons->setEnabled("Stop",false);
    if (RecordList.count())
    {
        if (p->SelectPattern(RecordList))
        {
            ui->PatternButtons->setSelected("Play",true);
            //ui->PatternButtons->setEnabled("Stop",true);
            RecordCount=0;
            DecrementTimes();
        }
    }
    delete p;
    //ui->SelectPatternButtons->setDown(false);
}

void OCNoteToolbox::TriggerNotes(QList<QPair<int, int> > &Notes)
{
    QString UndoText=(Notes.count()==0) ? "Note":"Notes";
    for (int i=0;i<Notes.count();i++)
    {
        if (i==1) UndoText.clear();
        XMLSimpleSymbolWrapper p=OCSymbolsCollection::GetSymbol("Note");
        p.setAttribute("NoteType",Notes.at(i).second);
        p.setAttribute("Pitch",Notes.at(i).first);
        p.setAttribute("NoteValue",ui->NoteButtons->value());
        p.setAttribute("Dotted",ui->DotButtons->isSelected());
        p.setAttribute("Triplet",ui->TripletButtons->isSelected());
        emit PasteXML(p,UndoText,i==Notes.count()-1);
    }
    DecrementTimes();
}

void OCNoteToolbox::DecrementTimes()
{
    if (ui->PatternButtons->isSelected("Record"))
    {
        if (RecordList.count()>=100)
        {
            ui->PatternButtons->setSelected("Stop",true);
            //ui->PatternButtons->setEnabled("Stop",false);
        }
        int TripletDotFlag=(2*(int)ui->DotButtons->isSelected())+(int)ui->TripletButtons->isSelected();
        RecordList.append(qMakePair(ui->NoteButtons->value(),TripletDotFlag));
        ui->PatternButtons->setEnabled("Play",true);
    }
    if (ui->PatternButtons->isSelected("Play"))
    {
        DotTimes=0;
        TripletTimes=0;
        ui->NoteButtons->setSelected(RecordList.at(RecordCount).first,true);
        ui->DotButtons->setSelected(0,RecordList.at(RecordCount).second==2);
        ui->TripletButtons->setSelected(0,RecordList.at(RecordCount).second==1);
        RecordCount++;
        if (RecordCount>=RecordList.count()) RecordCount=0;
        return;

    }
    if (DotTimes>0)
    {
        DotTimes--;
        if (DotTimes==0)
        {
            ui->DotButtons->setSelected(false);
        }
    }
    if (TripletTimes>0)
    {
        TripletTimes--;
        if (TripletTimes==0)
        {
            ui->TripletButtons->setSelected(false);
        }
    }
}
