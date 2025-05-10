#include "ocnotetoolbox.h"
#include "ui_ocnotetoolbox.h"
#include "cpatternlist.h"
#include <QAction>
#include "ocsymbolscollection.h"

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
    ui->WriteModeButton->addButton(new QAction(QIcon(":/mini/mini/write.png"),"Write Mode",this),QKeySequence("Alt"));
    ui->WriteModeButton->setFixedWidth(36);
    ui->WriteModeButton->setSelectMode(QMacButtons::SelectOneOrNone);
    connect(ui->WriteModeButton,qOverload<int>(&QMacButtons::buttonClicked),this,&OCNoteToolbox::ToggleWriteModeButton);

    ui->NoteButtons->setSelectMode(QMacButtons::SelectOne);
    ui->NoteButtons->setFixedWidth(36*7);
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/0.png"),"Whole Note",this),QKeySequence(tr("Alt+1")));
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/1.png"),"Half Note",this),QKeySequence(tr("Alt+2")));
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/2.png"),"Quarter Note",this),QKeySequence(tr("Alt+3")));
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/3.png"),"8th Note",this),QKeySequence(tr("Alt+4")));
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/4.png"),"16th Note",this),QKeySequence(tr("Alt+5")));
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/5.png"),"32th Note",this),QKeySequence(tr("Alt+6")));
    ui->NoteButtons->addButton(new QAction(QIcon(":/Notes/Notes/6.png"),"64th Note",this),QKeySequence(tr("Alt+7")));
    connect(ui->NoteButtons,qOverload<int>(&QMacButtons::selected),this,&OCNoteToolbox::SelectNote);
    connect(ui->NoteButtons,qOverload<int>(&QMacButtons::buttonClicked),this,&OCNoteToolbox::NoteChanged);
    ui->NoteButtons->setSelected(2,true);

    ui->DotButtons->setFixedWidth(36*4);
    ui->DotButtons->setSelectMode(QMacButtons::SelectOneOrNone);
    ui->DotButtons->addButton(new QAction(QIcon(":/Notes/Notes/dotx.png"),"Dot following Notes",this),QKeySequence(tr("Alt+8")));
    ui->DotButtons->addButton("Dot1","Dot the following Note",QIcon(":/Notes/Notes/dot1.png"));//8
    ui->DotButtons->addButton("Dot2","Dot the 2 following Notes",QIcon(":/Notes/Notes/dot2.png"));//9
    ui->DotButtons->addButton("DoubleDot","Double dot the following Note",QIcon(":/Notes/Notes/dotdot1.png"));//10;
    connect(ui->DotButtons,qOverload<int>(&QMacButtons::selected),this,&OCNoteToolbox::SelectDotButton);
    connect(ui->DotButtons,qOverload<int>(&QMacButtons::buttonClicked),this,&OCNoteToolbox::NoteChanged);

    ui->TripletButtons->setFixedWidth(36*3);
    ui->TripletButtons->setSelectMode(QMacButtons::SelectOneOrNone);
    ui->TripletButtons->addButton(new QAction(QIcon(":/Notes/Notes/triplet.png"),"Triplet following Notes",this),QKeySequence(tr("Alt+9")));
    ui->TripletButtons->addButton("Triplet2","Triplet the 2 following Notes",QIcon(":/Notes/Notes/triplet2.png"));//12
    ui->TripletButtons->addButton("Triplet3","Triplet the 3 following Notes",QIcon(":/Notes/Notes/triplet3.png"));//13
    connect(ui->TripletButtons,qOverload<int>(&QMacButtons::selected),this,&OCNoteToolbox::SelectTripletButton);
    connect(ui->TripletButtons,qOverload<int>(&QMacButtons::buttonClicked),this,&OCNoteToolbox::NoteChanged);

    ui->VorschlagButton->addButton(new QAction(QIcon(":/Notes/Notes/grace.png"),"Write Mode",this),QKeySequence("Alt+Shift"));
    ui->VorschlagButton->setFixedWidth(36);
    ui->VorschlagButton->setSelectMode(QMacButtons::SelectOneOrNone);
    //connect(ui->WriteModeButton,qOverload<int>(&QMacButtons::buttonClicked),this,&OCNoteToolbox::ToggleWriteModeButton);

    ui->PauseButtons->setFixedWidth(36*2);
    ui->PauseButtons->setSelectMode(QMacButtons::SelectNone);
    ui->PauseButtons->addButton(new QAction(QIcon(":/Notes/Notes/quarterrest.png"),"Rest",this),QKeySequence(tr("Alt+0")));
    ui->PauseButtons->addButton(new QAction(QIcon(":/Notes/Notes/wholerest.png"),"Whole Bar Rest",this),QKeySequence(tr("Alt++")));
    connect(ui->PauseButtons,qOverload<int>(&QMacButtons::buttonClicked),this,&OCNoteToolbox::PauseButtonClicked);

    ui->PatternButtons->setFixedWidth(36*2);
    ui->PatternButtons->setSelectMode(QMacButtons::SelectNone);
    ui->PatternButtons->addButton("RecordPattern","Record rhythm pattern",QIcon(":/24/rhythmpatternrecord24.png"));
    ui->PatternButtons->addButton("Dropdown","Select or manage saved rhythm patterns",QIcon(":/24/rhythmpatternmenu24.png"));
    //ui->PatternButtons->setMonochrome(false);
    connect(ui->PatternButtons,qOverload<int>(&QMacButtons::buttonClicked),this,&OCNoteToolbox::PatternButtonClicked);


    //connect(ui->SelectedPattern,SIGNAL(cellClicked(int,int)),this,SLOT(SelectedPatternClicked(int,int)));

    ui->SelectedPattern->setRowCount(1);
    ui->SelectedPattern->setColumnCount(1);
    ui->SelectedPattern->setRowHeight(0,32);
    ui->SelectedPattern->setIconSize(QSize(20,32));
    ui->SelectedPattern->setSelectionBehavior(QAbstractItemView::SelectItems);
    SetTable(PatternTableIdle);
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
    else if (value==4)
    {
        ui->DotButtons->setEnabled(true);
        ui->DotButtons->setSelected(3,false);
        ui->DotButtons->setEnabled(3,false);
        ui->TripletButtons->setEnabled(true);
    }
    else
    {
        ui->DotButtons->setEnabled(true);
        if (ui->DotButtons->size() > 3) ui->DotButtons->setEnabled(3,true);
        ui->TripletButtons->setEnabled(true);
    }
}

void OCNoteToolbox::SelectDotButton(int value)
{
    ui->TripletButtons->setSelected(false);
    DotTimes=value;
    if (value==3) DotTimes=1;
}

void OCNoteToolbox::SelectTripletButton(int value)
{
    ui->DotButtons->setSelected(false);
    TripletTimes=value;
    if (TripletTimes>0) TripletTimes++;
}

void OCNoteToolbox::PatternButtonClicked(int value)
{
    const auto b = ui->PatternButtons->data(value).toInt();
    if (b==RecordPattern)
    {
        RecordList.clear();
        isRecording=true;
        SetTable(PatternTableRecording);
    }
    else if (b==StopApplyingPattern)
    {
        RecordCount=0;
        DecrementTimes();
        SetTable(PatternTableIdle);
    }
    else if ((b==AbortRecordingPattern) || (b==SavePattern))
    {
        if (!RecordList.empty())
        {
            if (isRecording)
            {
                if (b==SavePattern) CPatternList::AppendPattern(RecordList);
                isRecording=false;
                SetTable(PatternTableApplying);
                RecordCount=0;
                DecrementTimes();
            }
            else
            {
                RecordCount=0;
                DecrementTimes();
                SetTable(PatternTableIdle);
            }
        }
        else
        {
            RecordCount=0;
            DecrementTimes();
            SetTable(PatternTableIdle);
        }
    }
    else if (b==ManagePatterns)
    {
        CPatternList p(this);
        if (p.SelectPattern(RecordList))
        {
            SetTable(PatternTableApplying);
            RecordCount=0;
            DecrementTimes();
        }
    }
}

void OCNoteToolbox::ToggleWriteModeButton(int value) {
    qDebug() << "toggle" << value << ui->WriteModeButton->isSelected(value);
    emit ToggleWriteMode(ui->WriteModeButton->isSelected(value));
}

void OCNoteToolbox::PauseButtonClicked(int value)
{
    if (value==0)
    {
        XMLSimpleSymbolWrapper p=OCSymbolsCollection::GetDefaultSymbol("Rest");
        p.setNoteValue(ui->NoteButtons->value());
        p.setTriplet(ui->TripletButtons->isSelected());
        p.setDotted(getDotted());
        emit PasteXML(p,"Rest",true);
        DecrementTimes();
    }
    if (value==1)
    {
        XMLSimpleSymbolWrapper p=OCSymbolsCollection::GetDefaultSymbol("Rest");
        p.setNoteValue(7);
        emit PasteXML(p,"Rest",true);
        DecrementTimes();
    }
}

void OCNoteToolbox::SetTable(const TableModes value)
{
    TableMode=value;
    if (value==PatternTableApplying)
    {
        ui->PatternButtons->setIcon(0,QIcon(":/24/rhythmpatterncancel24.png"));
        ui->PatternButtons->setData(0,StopApplyingPattern);
        ui->PatternButtons->setTooltip(0,"Stop applying rhythm pattern");
        ui->PatternButtons->setIcon(1,QIcon(":/24/rhythmpatternmenu24.png"));
        ui->PatternButtons->setData(1,ManagePatterns);
        ui->PatternButtons->setTooltip(1,"Select or manage saved rhythm patterns");
        ui->PatternButtons->setEnabled(1,true);
        ui->SelectedPattern->clear();
        CPatternList::createRow(RecordList,ui->SelectedPattern,0);
        for (int i=0;i<ui->SelectedPattern->columnCount();i++) ui->SelectedPattern->setColumnWidth(i,20);
        ui->SelectedPattern->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    else if (value==PatternTableRecording)
    {
        ui->PatternButtons->setIcon(0,QIcon(":/24/rhythmpatterncancel24.png"));
        ui->PatternButtons->setData(0,AbortRecordingPattern);
        ui->PatternButtons->setTooltip(0,"Abort recording rhythm pattern");
        ui->PatternButtons->setIcon(1,QIcon(":/24/rhythmpatternok24.png"));
        ui->PatternButtons->setData(1,SavePattern);
        ui->PatternButtons->setTooltip(1,"Save recorded rhythm pattern");
        ui->PatternButtons->setEnabled(1,!RecordList.isEmpty());
        ui->SelectedPattern->clear();
        CPatternList::createRow(RecordList,ui->SelectedPattern,0);
        for (int i=0;i<ui->SelectedPattern->columnCount();i++) ui->SelectedPattern->setColumnWidth(i,20);
        ui->SelectedPattern->setCurrentItem(nullptr);
        ui->SelectedPattern->setSelectionMode(QAbstractItemView::NoSelection);
    }
    else
    {
        ui->PatternButtons->setIcon(0,QIcon(":/24/rhythmpatternrecord24.png"));
        ui->PatternButtons->setData(0,RecordPattern);
        ui->PatternButtons->setTooltip(0,"Record rhythm pattern");
        ui->PatternButtons->setIcon(1,QIcon(":/24/rhythmpatternmenu24.png"));
        ui->PatternButtons->setData(1,ManagePatterns);
        ui->PatternButtons->setTooltip(1,"Select or manage saved rhythm patterns");
        ui->PatternButtons->setEnabled(1,true);
        ui->SelectedPattern->clear();
        ui->SelectedPattern->setCurrentItem(nullptr);
        ui->SelectedPattern->setSelectionMode(QAbstractItemView::NoSelection);
    }
}

int OCNoteToolbox::getDotted()
{
    return int(ui->DotButtons->isSelected(0) || ui->DotButtons->isSelected(1) || ui->DotButtons->isSelected(2)) + (int(ui->DotButtons->isSelected(3))*2);
}

void OCNoteToolbox::setDotted(const int d)
{
    ui->DotButtons->setSelected(false);
    if (d==2)
    {
        ui->DotButtons->setSelected(3,true);
    }
    else
    {
        ui->DotButtons->setSelected(0,d==1);
    }
}

void OCNoteToolbox::TriggerNotes(OCInputNoteList &Notes)
{
    QString UndoText=(Notes.empty()) ? "Note":"Notes";
    for (int i=0;i<Notes.size();i++)
    {
        if (i==1) UndoText.clear();
        XMLSimpleSymbolWrapper p = OCSymbolsCollection::GetDefaultSymbol("Note");
        p.setAttribute("NoteType",boundStep(0,Notes[i].Type + (4*int(ui->VorschlagButton->isSelected())),5,2));
        p.setPitch(Notes.at(i).Pitch);
        p.setNoteValue(ui->NoteButtons->value());
        p.setDotted(getDotted());
        p.setTriplet(ui->TripletButtons->isSelected());
        emit PasteXML(p,UndoText,i == Notes.size()-1);
    }
    DecrementTimes();
    if (ui->VorschlagButton->isSelected()) ui->VorschlagButton->setSelected(false);
}

void OCNoteToolbox::GetCurrentNote(XMLSimpleSymbolWrapper& note)
{
    //QString UndoText="Note";
    note.copy(OCSymbolsCollection::GetDefaultSymbol("Note").xml());
    note.setAttribute("NoteType",0);
    note.setPitch(60);
    note.setNoteValue(ui->NoteButtons->value());
    note.setDotted(getDotted());
    note.setTriplet(ui->TripletButtons->isSelected());
    note.setAttribute("Ticks",note.tickCalc());
    //emit PasteXML(p,UndoText,true);
    //DecrementTimes();
}

void OCNoteToolbox::DecrementTimes()
{
    if (TableMode == PatternTableRecording)
    {
        if (RecordList.size()>=20)
        {
            CPatternList p(this);
            p.AppendPattern(RecordList);
            isRecording=false;
            SetTable(PatternTableApplying);
            RecordCount=0;
        }
        else
        {
            RecordList.append(OCPatternNote(ui->NoteButtons->value(),getDotted(),ui->TripletButtons->isSelected()));
            SetTable(PatternTableRecording);
        }
    }
    if (TableMode == PatternTableApplying)
    {
        DotTimes=0;
        TripletTimes=0;
        ui->NoteButtons->setSelected(RecordList.at(RecordCount).Button,true);
        setDotted(RecordList.at(RecordCount).dot());
        ui->TripletButtons->setSelected(0,RecordList.at(RecordCount).isTriplet());
        ui->SelectedPattern->setCurrentCell(0,RecordCount);
        RecordCount++;
        if (RecordCount>=RecordList.size()) RecordCount=0;
        return;
    }
    if (DotTimes>0)
    {
        DotTimes--;
        if (DotTimes==0) ui->DotButtons->setSelected(false);
    }
    if (TripletTimes>0)
    {
        TripletTimes--;
        if (TripletTimes==0) ui->TripletButtons->setSelected(false);
    }
}
