#include "ocnotelist.h"
#include "ocsymbolscollection.h"

const float FnSize(const float c, const int SymbolSize)
{
    if (SymbolSize == 0) return c;
    return FloatDiv(c ,SizeFactor(SymbolSize));
}

void OCStaffAccidentals::Reset1(const int Pitch)
{
    lfortegn[Pitch].current = 0;
}

void OCStaffAccidentals::RdAcc(const int Key)
{
    int Antal=CKey::NumOfAccidentals(Key);
    int KBFlag=CKey::AccidentalFlag(Key);
    int fn[7];
    if (KBFlag == -1)
    {
        fn[1] = 3;
        fn[2] = 0;
        fn[3] = 4;
        fn[4] = 1;
        fn[5] = 5;
        fn[6] = 2;
    }
    else
    {
        fn[1] = 6;
        fn[2] = 2;
        fn[3] = 5;
        fn[4] = 1;
        fn[5] = 4;
        fn[6] = 0;
    }
    for (int iTemp=0;iTemp<128;iTemp++)
    {
        lfortegn[iTemp].current=0;
        lfortegn[iTemp].faste=0;
    }
    if (Antal > 0)
    {
        for (int iTemp = 1;iTemp<=Antal;iTemp++)
        {
            int b = fn[iTemp];
            for (int iTemp1 = b;iTemp1 < 128;iTemp1+=7)
            {
                lfortegn[iTemp1].faste = 1;
            }
        }
    }
    for (int iTemp = 0;iTemp<7;iTemp++)
    {
        Dirty[iTemp] = false;
        OneOct[iTemp] = 0;
    }
}

void OCStaffAccidentals::CheckFortegn(int *iJ)
{
    bool WasHere[7];
    calc(WasHere, iJ);
    if (NoteItems.count()) calc(WasHere, iJ);
}

void OCStaffAccidentals::calc(bool* WasHere, int* iJ)
{
    for (int iTemp = 0;iTemp<NoteItems.count();iTemp++)
    {
        OCStaffAccidentalItem& item=NoteItems[iTemp];
        int LineNum = item.Pitch % 7;
        LineNum = Inside(LineNum, 0, 6, 7);
        int KB = FnKB(iJ, item.NoteNum);
        if (Ignore.contains(item.Pitch))
        {
            Dirty[LineNum] = true;
            lfortegn[item.Pitch].current = -3;
            OneOct[LineNum] = -3;
            WasHere[LineNum] = false;
        }
        /*
        for (int iTemp1 = 0; iTemp1<Ignore.count();iTemp1++)
        {

            if (Ignore[iTemp1] == item.Pitch)
            {
                Dirty[LineNum] = true;
                lfortegn[item.Pitch].current = -3;
                OneOct[LineNum] = -3;
                WasHere[LineNum] = false;
                break;
            }
        }
        */
        if (!Dirty[LineNum])
        {
            if ((KB == 0) && (lfortegn[item.Pitch].faste == 1))
            {
                item.HasFortegn=FnHasFortegn(KB);
                update(WasHere,LineNum,KB,iTemp);
            }
            else if ((lfortegn[item.Pitch].faste == 0) && (KB != 0))
            {
                item.HasFortegn=FnHasFortegn(KB);
                update(WasHere,LineNum,KB,iTemp);
            }
        }
        else
        {
            if ((KB != OneOct[LineNum]) || (OneOct[LineNum] != lfortegn[item.Pitch].current))
            {
                item.HasFortegn=FnHasFortegn(KB);
                update(WasHere,LineNum,KB,iTemp);
            }
        }
    }
}

void OCStaffAccidentals::update(bool* WasHere, const int LineNum, const int KB, const int iTemp)
{
    Dirty[LineNum] = true;
    if ((WasHere[LineNum]) && (KB != OneOct[LineNum]) && (OneOct[LineNum] != 0))
    {
        lfortegn[NoteItems[iTemp].Pitch].current = -3;
        OneOct[LineNum] = -3;
    }
    else
    {
        lfortegn[NoteItems[iTemp].Pitch].current = KB;
        OneOct[LineNum] = KB;
    }
    WasHere[LineNum] = true;
}

const int OCStaffAccidentals::Switch(const int In, const int Out1, const int Out2, const int OutElse) const
{
    switch (In)
    {
    case 1:
        return Out1;
    case 2:
        return Out2;
    default:
        return OutElse;
    }
}

const int OCStaffAccidentals::FnKB(int *iJ, const int NoteNum) const
{
    switch (NoteNum)
    {
    case 0:
        return Switch(iJ[0],1,-2,0);
    case 1:
        return Switch(iJ[1],2,-1,1);
    case 2:
        return Switch(iJ[2],2,-2,0);
    case 3:
        return Switch(iJ[3],1,-2,-1);
    case 4:
        return Switch(iJ[4],2,-1,0);
    case 5:
        return Switch(iJ[5],1,-2,0);
    case 6:
        return Switch(iJ[6],2,-1,1);
    case 7:
        return Switch(iJ[7],2,-2,0);
    case 8:
        return Switch(iJ[8],1,-1,-1);
    case 9:
        return Switch(iJ[9],2,-2,0);
    case 10:
        return Switch(iJ[10],1,-2,-1);
    case 11:
        return Switch(iJ[11],2,-1,0);
    }
    return 0;
}

const int OCStaffAccidentals::FnHasFortegn(const int KB) const
{
    switch (KB)
    {
    case 0: return 5;
    case -1: return 1;
    case 1: return 2;
    case -2: return 3;
    case 2: return 4;
    }
    return 0;
}

const QList<int> OCStaffAccidentals::GetHasFortegnArray(const int Antal)
{
    QList<int> HasFortegn;
    for (int i=0;i<Antal;i++)
    {
        if (NoteItems.count())
        {
            HasFortegn.append(NoteItems.takeFirst().HasFortegn);
        }
        else
        {
            HasFortegn.append(0);
        }
    }
    return HasFortegn;
}

void OCStaffAccidentals::PutNoteArrays(const QList<OCStaffAccidentalItem>& Items)
{
    NoteItems.append(Items);
}

void OCStaffAccidentals::ResetCluster()
{
    NoteItems.clear();
}

void OCStaffAccidentals::AddIgnore(const int Pitch)
{
    Ignore.append(Pitch);
}

void OCStaffAccidentals::ClearIgnore()
{
    Ignore.clear();
}

//--------------------------------------------------------------------------

OCRhythmObject::OCRhythmObject()
{
    Size=0;
    MoveX=0;
    BalkBeginY=0;
    val=0;
    balkx=0;
    UpDown=0;
    AverageY=0;
    BalkEndY=0;
    Rounded=0;
    Beat=0;
    Bar=0;
    Meter=0;
    NumOfBeams=0;
    NumOfEvInBeam=0;
    IsPause=false;
    ForceUpDown=0;
    ForceBeam=0;
    NumOfForcedBeams=0;
    ForceSlant=0;
    HighestY=0;
    LowestY=0;
    SlantFactor=0;
    BeamLimit=0;
    CenterX=0;
    IsSingleNoteWithTie=false;
    FlipTie=false;
    PerformanceSign=0;
    PerformanceSignPos=QPointF(0,0);
    PerformanceSignSize=0;
}

void OCRhythmObject::PlotStem(OCDraw& ScreenObj)
{
    int SgnThickness = LineHalfThickNess * 2 * UpDown;
    int SgnHalfThickness = LineHalfThickNess * UpDown;
    if (val != 144)
    {
        if (val != 96)
        {
            if (val != 64)
            {
                ScreenObj.DM(balkx, BalkBeginY - SgnThickness);
                ScreenObj.DL(0,(BalkEndY + SgnHalfThickness) - (BalkBeginY - SgnThickness));
            }
        }
    }
    ScreenObj.DM(balkx, BalkEndY + SgnHalfThickness); //' Size
}

const bool OCRhythmObject::BeamAble()
{
    return ((val <= 12) || (val == 18));
}

const int OCRhythmObject::FNclcfanr(const int c) const
{
    switch (c)
    {
        case 1:
            return 4;
        case 2:
        case 3:
            return 3;
        case 4:
        case 6:
        case 9:
            return 2;
        case 8:
        case 12:
        case 18:
            return 1;
    }
    return 0;
}

//--------------------------------------------------------------------------

const int IOCRhythmObject::stemX(const int UpDown, const int X)
{
    return X - (53 * UpDown);
}

void IOCRhythmObject::plot(int& NextHeight, int NextX, int BarsToPrint, OCTieWrap& TieWrap, int TriolVal, QColor TrackColor, int &Py, OCSymbolArray& SymbolList, OCDraw& ScreenObj){}

IOCRhythmObject::IOCRhythmObject(){}

const int IOCRhythmObject::Pitch(const int NoteNum) const{return 0;}

const int IOCRhythmObject::Count(){return 0;}

void IOCRhythmObject::CalcUpDownProps(){}

void IOCRhythmObject::FillLineNumsArray(const QList<int>& LineNums){}

void IOCRhythmObject::SetHasFortegn(OCStaffAccidentals& lfortegn){}

int IOCRhythmObject::FNBalkHeight() const{return 0;}

const NoteTypeCode IOCRhythmObject::NoteType(const int NoteNum) const{return tsnote;}

void IOCRhythmObject::Fill(int &Py, QDomLiteElement* XMLVoice, int TrackNum, OCPrintVarsType &dCurrent, float XFysic, OCCounter& CountIt, OCStaffAccidentals& lfortegn, XMLSymbolWrapper& Symbol, OCBarList& BarList, OCDraw& ScreenObj){}

const int IOCRhythmObject::LineNum(const int Index) const{return 0;}

void IOCRhythmObject::MoveTo(OCDraw& ScreenObj){}

const int IOCRhythmObject::Moveaccidental(const int Notes, const int Accidentals) const{return 0;}

void IOCRhythmObject::MoveChord(int factor){}

//--------------------------------------------------------------------------

CPausesToPrint::CPausesToPrint()
{
    mCenterX=0;
    mCenterY=0;
    NumOfCompressed=0;
    PointerAfterCompress=0;
}

const QList<QGraphicsItem*> CPausesToPrint::plPsSgn(const int c, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
//'    Dim faner As Integer
    ScreenObj.DM(mCenterX, mCenterY);
    switch (c)
    {
    case 144:
    case 96:
    case 64:
        ScreenObj.DR(FnSize(-5 * 12,Props.Size), 8 * 12);
        l.append(ScreenObj.PlRect(120,-48,Props.Size));
        break;
    case 48:
    case 32:
    case 72:
        ScreenObj.DR(FnSize(-5 * 12,Props.Size), 0);
        l.append(ScreenObj.PlRect(120,48,Props.Size));
        break;
    case 24:
    case 16:
    case 36:
        ScreenObj.DM(mCenterX - FnSize(36,Props.Size), mCenterY - FnSize(66,Props.Size));
        l.append(ScreenObj.plLet(QChar(OCTTFRestQuarter), Props.Size, OCTTFname, false, false, 1200));
        break;
    case 12:
    case 8:
    case 18:
        ScreenObj.DM(mCenterX - FnSize(36,Props.Size), mCenterY - FnSize(66,Props.Size));
        l.append(ScreenObj.plLet(QChar(OCTTFRestEight), Props.Size, OCTTFname, false, false, 1200));
        break;
    case 6:
    case 4:
    case 9:
        ScreenObj.DM(mCenterX - FnSize(36,Props.Size), mCenterY - FnSize(66,Props.Size));
        l.append(ScreenObj.plLet(QChar(OCTTFRestSixteen), Props.Size, OCTTFname, false, false, 1200));
        break;
    case 3:
    case 2:
        ScreenObj.DM(mCenterX - FnSize(36,Props.Size), mCenterY - FnSize(66,Props.Size));
        l.append(ScreenObj.plLet(QChar(OCTTFRestTrirtytwo), Props.Size, OCTTFname, false, false, 1200));
        break;
    case 1:
        ScreenObj.DM(mCenterX - FnSize(36,Props.Size), mCenterY - FnSize(66,Props.Size));
        l.append(ScreenObj.plLet(QChar(OCTTFRestSixtyfour), Props.Size, OCTTFname, false, false, 1200));
        break;
    }
    return l;
}

void CPausesToPrint::plNum(const int b, const int siz, OCDraw& ScreenObj)
{
    ScreenObj.plLet(QString::number(b).trimmed(), siz, "times new roman", true, false, 192);
}

void CPausesToPrint::CalcUpDownProps()
{
    Props.BalkBeginY = mCenterY - (Props.UpDown * 192);
    Props.balkx = stemX(Props.UpDown, mCenterX);
    Props.CenterX = mCenterX;
}

void CPausesToPrint::Fill(int &Py, QDomLiteElement* XMLVoice, int TrackNum, OCPrintVarsType &dCurrent, float XFysic, OCCounter& CountIt, OCStaffAccidentals& lfortegn, XMLSymbolWrapper& Symbol, OCBarList& BarList, OCDraw& ScreenObj)
{
    Props.Invisible = Symbol.getVal("Invisible");
    Props.IsPause = true;
    Props.MoveX = Symbol.pos().x();
    mCenterX = XFysic + Props.MoveX;
    Props.Size = Symbol.size();
    Props.val = Symbol.ticks();
    Props.Rounded = CountIt.Rounded;
    Props.Beat = CountIt.Counter;
    Props.Bar = CountIt.BarCounter;
    Props.Meter = dCurrent.Meter;
    Props.AverageY = Symbol.moveY(1008);
    Props.HighestY = Props.AverageY;
    Props.NumOfBeams = Props.FNclcfanr(Symbol.ticks());
    Props.LowestY = Props.AverageY;
    mCenterY = Props.AverageY;
    if (BarList.GetMultiPause(Props.Bar) > 1)
    {
        NumOfCompressed = BarList.GetMultiPause(Props.Bar);
        //'py = py + (BarList.GetMultiPauseNumOfEvents(Props.Bar + 1) * 6 * (BarList.GetMultiPause(Props.Bar + 1) - 1)) + (6 * (BarList.GetMultiPauseNumOfEvents(Props.Bar + 1) - 1))
        Py = Py + (BarList.GetMultiPause(Props.Bar) - 1);
        PointerAfterCompress = Py;
    }
}

void CPausesToPrint::FillLineNumsArray(QList<int>& LineNums)
{
    LineNums.clear();
    //if (Props.Size > -1)
    LineNums.append(34);
}

int CPausesToPrint::FNBalkHeight() const
{
    int c;
    if (Props.UpDown == 1)
    {
        c = mCenterY - (28 * 12);
        if (c > (94 * 12)) c = (94 * 12);
    }
    else
    {
        c = mCenterY + (28 * 12);
        if (c < (74 * 12)) c = (74 * 12);
    }
    return c;
}

void CPausesToPrint::MoveChord(int factor)
{
    Props.CenterX = Props.CenterX + (120 * factor);
    Props.balkx = Props.balkx + (120 * factor);
    mCenterX = mCenterX + (120 * factor);
}

void CPausesToPrint::plot(int& NextHeight, int NextX, int BarsToPrint, OCTieWrap& TieWrap, int TriolVal, QColor TrackColor, int& Py, OCSymbolArray& SymbolList, OCDraw& ScreenObj)
{
    /*
    int Height=0;
    int Width=0;
    int LeftAdd=0;
    int BottomAdd=0;
    */
    int Pnt = Py;
    ScreenObj.setcol(Py);
    QList<QGraphicsItem*> l;
    if (NumOfCompressed > 1) Py = PointerAfterCompress;
    if (Props.Invisible) return;
    //if (Props.Size > -1)
    //{
        if (NumOfCompressed > 1)
        {
            ScreenObj.DM(mCenterX, mCenterY);
            ScreenObj.DR(-60, 48);
            PlMorePauses(NumOfCompressed, ScreenObj);
            //'ScreenObj.setsize Props.Size
            Props.Beat = Props.Meter;
            ScreenObj.DM(mCenterX - 72, mCenterY + 288);
            ScreenObj.DR(IntDiv((NumOfCompressed - 1) ,4) * 72, 0);
            plNum(NumOfCompressed, 0, ScreenObj);
            /*
            LeftAdd = -132;
            BottomAdd = 36;
            Height = 72;
            Width = 168;
            */
        }
        else
        {
            if (Props.val == Props.Meter)
            {
                //ScreenObj.DMabs(mCenterX, mCenterY, Props.Size);
                //ScreenObj.DR(-60, 48);
                //ScreenObj.PlRect(120,48);
                //ScreenObj.DR(0,48);
                l.append(plPsSgn(96,ScreenObj));
                /*
                LeftAdd = -132;
                BottomAdd = 36;
                Height = 72;
                Width = 168;
                */
            }
            else
            {
                //ScreenObj.DMabs(mCenterX + 24, mCenterY + 12, Props.Size);
                l.append(plPsSgn(Props.val, ScreenObj));
                ScreenObj.DM(mCenterX + FnSize(12,Props.Size), mCenterY + 48);
                l.append(OCNoteList::PlPunkt(Props.val, TriolVal, 0, ScreenObj));
                /*
                switch (Props.val)
                {
                case 144:
                case 96:
                case 64:
                    LeftAdd = -132;
                    BottomAdd = 36;
                    Height = 72;
                    Width = 168;
                    break;
                case 72:
                case 48:
                case 32:
                    LeftAdd = -132;
                    BottomAdd = -12;
                    Height = 72;
                    Width = 168;
                    break;
                default:
                    LeftAdd = -120;
                    BottomAdd = -120;
                    Height = 240;
                    Width = 192;
                }
                */
                if (Props.ForceBeam > 0)
                {
                    ScreenObj.DM(mCenterX, mCenterY + (96*Props.UpDown));
                    Props.PlotStem(ScreenObj);
                }
            }
        }
    //}
    ScreenObj.col = TrackColor;
    //SymbolList.Append(Props.CenterX + 48 + LeftAdd, Props.AverageY + BottomAdd, Height, Width, Pnt, Props.Size);
    //SymbolList.AppendBoundingRect(r,Pnt);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pnt);
}

void CPausesToPrint::PlMorePauses(const int NumOfBars, OCDraw& ScreenObj)
{
    for (int iTemp = 1;iTemp<=IntDiv(NumOfBars ,4)+1;iTemp++)
    {
        if (iTemp == IntDiv(NumOfBars ,4) + 1)
        {
            for (int iTemp1 = 1; iTemp1 <= NumOfBars % 4; iTemp1++)
            {
                ScreenObj.PlRect(120,48);
                ScreenObj.DR(0, -48);
            }
        }
        else
        {
            for (int iTemp1 = 1;iTemp1 <=4; iTemp1++)
            {
                ScreenObj.PlRect(120,48);
                ScreenObj.DR(0, -48);
            }
        }
        ScreenObj.DR(192, 192);
    }
}

//--------------------------------------------------------------------------

CNoteHead::CNoteHead()
{
    OrigPlace=0;
    Voice=0;
    PitchCode=0;
    NoteVal=0;
    Left=0;
    AccidentalLeft=0;
    Size=0;
    Marked=0;
    CenterX=0;
    CenterY=0;
    FortegnAddX=0;
    Pitch=0;
    NoteNum=0;
    HasFortegn=0;
}

void CNoteHead::MoveTo(OCDraw& ScreenObj)
{
    ScreenObj.DM(CenterX, CenterY);
}
/*
void CNoteHead::MoveTo0(OCDraw& ScreenObj)
{
    ScreenObj.DM(CenterX, CenterY);
}
*/
void CNoteHead::plot(const int ClusterSize, const int TriolVal, const QColor TrackColor, const int UpDown, OCSymbolArray& SymbolList, OCDraw& ScreenObj)
{
    if (ScreenObj.canColor())
    {
        if (Marked)
        {
            if (ScreenObj.Cursor->SelCount()==0)
            {
                ScreenObj.col=markedcolor;
            }
            else
            {
                ScreenObj.col=selectedcolor;
            }
        }
    }
    MoveTo(ScreenObj);
    QList<QGraphicsItem*> a;
    switch (HasFortegn)
    {
    case 1:
        ScreenObj.DR(FnSize((-11 * 12) + FortegnAddX,Size), 150);
        a.append(ScreenObj.plLet(QChar(OCTTFFlat), Size, OCTTFname, false, false, 1200));
        break;
    case 2:
        ScreenObj.DR(FnSize((-13 * 12) + FortegnAddX,Size), 150);
        a.append(ScreenObj.plLet(QChar(OCTTFSharp), Size, OCTTFname, false, false, 1200));
        break;
    case 3:
        ScreenObj.DR(FnSize((-17 * 12) + FortegnAddX,Size), 150);
        a.append(ScreenObj.plLet(QChar(OCTTFDoubleFlat), Size, OCTTFname, false, false, 1200));
        break;
    case 4:
        ScreenObj.DR(FnSize((-14 * 12) + FortegnAddX,Size), 150);
        a.append(ScreenObj.plLet(QChar(OCTTFDoubleSharp), Size, OCTTFname, false, false, 1200));
        break;
    case 5:
        ScreenObj.DR(FnSize(FortegnAddX - 12 * 12,Size), 150);
        a.append(ScreenObj.plLet(QChar(OCTTFOpl), Size, OCTTFname, false, false, 1200));
    }
    QList<QGraphicsItem*> l;
    MoveTo(ScreenObj);
    if ((CenterY % 96) != 0) ScreenObj.DR(0,48);
    l.append(OCNoteList::PlPunkt(NoteVal, TriolVal, 0, ScreenObj));
    MoveTo(ScreenObj);
    ScreenObj.DR(0,77);
    ScreenObj.DR((53+FnSize(-53,Size))*-UpDown,0);
    switch (NoteVal)
    {
    case 144:
    case 96:
    case 64:
        l.append(ScreenObj.plLet(QChar(OCTTFNoteWhole), ClusterSize, OCTTFname, false, false, 612));
        break;
    case 48:
    case 32:
    case 72:
        l.append(ScreenObj.plLet (QChar(OCTTFNoteHalf), ClusterSize, OCTTFname, false, false, 612));
        break;
    default:
        l.append(ScreenObj.plLet(QChar(OCTTFNoteQuarter), ClusterSize, OCTTFname, false, false, 612));
    }
    SymbolList.AppendAccidentalGroup(ScreenObj.MakeGroup(l),ScreenObj.Scene->createItemGroup(a),Pointer);
    ScreenObj.col = TrackColor;
}

//--------------------------------------------------------------------------

CNotesToPrint::CNotesToPrint()
{
    OldNote=0;
    NextNote=0;
    CAvoid=0;
    hi=0;
    lo=0;
}

CNotesToPrint::~CNotesToPrint()
{
    clear();
}

void CNotesToPrint::Append(const int Voice, const int Pointer, const XMLSymbolWrapper& Symbol, const bool Marked)
{
    CNoteHead* n=new CNoteHead;
    n->Pointer=Pointer;
    n->OrigPlace = NoteHeadList.count();
    n->Voice = Voice;
    n->Invisible=Symbol.getVal("Invisible");
    n->NoteType = (NoteTypeCode)(Symbol.getVal("NoteType"));
    n->PitchCode = Symbol.getVal("Pitch");
    n->NoteVal = Symbol.ticks();
    n->Left = Symbol.pos().x();
    n->AccidentalLeft = Symbol.getVal("AccidentalLeft");
    n->Size = Symbol.size();
    n->Marked = Marked;
    NoteHeadList.append(n);
}

void CNotesToPrint::clear()
{
    qDeleteAll(NoteHeadList);
    NoteHeadList.clear();
}

void CNotesToPrint::sort()
{
    for (int iTemp1=0;iTemp1<NoteHeadList.count();iTemp1++)
    {
        for (int iTemp=0;iTemp<NoteHeadList.count();iTemp++)
        {
            if (NoteHeadList[iTemp1]->PitchCode < NoteHeadList[iTemp]->PitchCode) NoteHeadList.swap(iTemp,iTemp1);
        }
    }
}

void CNotesToPrint::CalcCenters(OCPrintVarsType &dCurrent, const float XFysic)
{
    //for (int iTemp=0;iTemp<NoteHeadList.count();iTemp++)
    foreach(CNoteHead* n,NoteHeadList)
    {
        n->CenterY = fnAandClefCalc(n->PitchCode, dCurrent);
        n->CenterX = XFysic + n->Left;
        n->FortegnAddX = n->AccidentalLeft;
        n->Pitch = fnAcalc(n->PitchCode, dCurrent);
        n->NoteNum = n->PitchCode % 12;
    }
}
/*
void CNotesToPrint::Retrieve(int RecNum)
{
    CurrentNote = NoteHeadList[RecNum];
}
*/
const int CNotesToPrint::GetAlti(const int RecNum) const
{
    return NoteHeadList[RecNum]->PitchCode;
}

void CNotesToPrint::PlBilinier(const int CurrNote, OCDraw& ScreenObj)
{
    if (CurrNote == 0)
    {
        if (CurrentNote->CenterY <= 720)
        {
            ScreenObj.DM(CurrentNote->CenterX - HelpLineHalfWidth, 720);
            for (int iTemp = 720;iTemp>=CurrentNote->CenterY;iTemp+=-96)
            {
                ScreenObj.DL(HelpLineHalfWidth * 2, 0);
                ScreenObj.DR(-HelpLineHalfWidth * 2, -96);
            }
        }
    }
    if (CurrNote == NoteHeadList.count()-1)
    {
        if (CurrentNote->CenterY >= 1296)
        {
            ScreenObj.DM(CurrentNote->CenterX - HelpLineHalfWidth, 1296);
            for (int iTemp = 1296;iTemp<=CurrentNote->CenterY;iTemp+=96)
            {
                ScreenObj.DL(HelpLineHalfWidth * 2, 0);
                ScreenObj.DR(-HelpLineHalfWidth * 2, 96);
            }
        }
    }
}

void CNotesToPrint::Calcprops(const int UpDown)
{
    int FlytFortegn=0;
    //if (CurrentNote->Size < 0) return;
    if (UpDown == 1)
    {
        if (CAvoid)
        {
            CAvoid = 0;
        }
        else
        {
            if (Abs(CurrentNote->CenterY - OldNote) < 60)
            {
                CAvoid = -108;
            }
        }
        if (Abs(CurrentNote->CenterY - NextNote) < 60)
        {
            FlytFortegn = -180;
        }
        else
        {
            FlytFortegn = 0;
        }
    }
    else
    {
        if (CAvoid)
        {
            CAvoid = 0;
            FlytFortegn = 0;
        }
        else
        {
            if (Abs(CurrentNote->CenterY - OldNote) < 60)
            {
                CAvoid = 108;
                FlytFortegn = -180;
            }
        }
    }
    CurrentNote->CenterX = CurrentNote->CenterX + CAvoid;
    CurrentNote->FortegnAddX = CurrentNote->FortegnAddX + FlytFortegn;
    OldNote = CurrentNote->CenterY;
}

const int CNotesToPrint::fnAcalc(const int a, OCPrintVarsType &b) const
{
    int Add=0;
    //'Pitch = (Fix((a% * 3 / 4) - 0.1) + 14) Mod 12
    int Pitch = a % 12;
    if (b.J[Pitch] == 1)
    {
        Add = -1;
    }
    else if (b.J[Pitch] == 2)
    {
        Add = 1;
    }
    return (int)(FloatDiv((a - 12) * 7.0 ,12) + 0.4 + Add); // used to be fix()
}

void CNotesToPrint::CalcAverage()
{
    lo = 32000;
    hi = 0;
    foreach (CNoteHead* NoteHead, NoteHeadList)
    {
        hi=qMax(hi,NoteHead->CenterY);
        lo=qMin(lo,NoteHead->CenterY);
        //if (NoteHead->CenterY > hi) hi = NoteHead->CenterY;
        //if (NoteHead->CenterY < lo) lo = NoteHead->CenterY;
    }
    Props.AverageY = FloatDiv(hi - lo, 2) + lo;
    Props.HighestY = hi;
    Props.LowestY = lo;
}

void CNotesToPrint::CalcFortegns(OCStaffAccidentals& lfortegn, OCPrintVarsType &dCurrent)
{
    AddToArrays(lfortegn);
    lfortegn.CheckFortegn(dCurrent.J);
    SetHasFortegn(lfortegn);
}

void CNotesToPrint::AddToArrays(OCStaffAccidentals& lfortegn)
{
    QList<OCStaffAccidentalItem> NoteItems;
    foreach (CNoteHead* NoteHead, NoteHeadList)
    {
        OCStaffAccidentalItem i = {0,NoteHead->Pitch,NoteHead->NoteNum};
        //i.Pitch=NoteHead->Pitch;
        //i.NoteNum=NoteHead->NoteNum;
        NoteItems.append(i);
    }
    lfortegn.PutNoteArrays(NoteItems);
}

const int CNotesToPrint::Count() const
{
    return NoteHeadList.count();
}

void CNotesToPrint::CalcUpDownProps()
{
    NextNote = -2400;
    OldNote = -2400;
    CAvoid = 0;
    if (Props.UpDown == -1)
    {
        for (int iTemp = 0; iTemp<NoteHeadList.count();iTemp++)
        {
            CurrentNote=NoteHeadList[iTemp];
            if (iTemp < NoteHeadList.count()-1)
            {
                NextNote = NoteHeadList[iTemp+1]->CenterY;
            }
            else
            {
                NextNote = -200;
            }
            Calcprops(Props.UpDown);
        }
        CurrentNote=NoteHeadList.first();
    }
    else
    {
        for (int iTemp = NoteHeadList.count()-1;iTemp>=0;iTemp--)
        {
            CurrentNote=NoteHeadList[iTemp];
            if (iTemp > 0)
            {
                NextNote = NoteHeadList[iTemp - 1]->CenterY;
            }
            else
            {
                NextNote = -200;
            }
            Calcprops(Props.UpDown);
        }
        CurrentNote=NoteHeadList.last();
    }
    Props.BalkBeginY = CurrentNote->CenterY;
    Props.balkx = stemX(Props.UpDown, CurrentNote->CenterX);
    Props.CenterX = CurrentNote->CenterX;
}

const int CNotesToPrint::CenterY(const int NoteNum) const
{
    return NoteHeadList[NoteNum]->CenterY;
}

const NoteTypeCode CNotesToPrint::NoteType(const int NoteNum) const
{
    return NoteHeadList[NoteNum]->NoteType;
}

void CNotesToPrint::Fill(int &Py, QDomLiteElement* XMLVoice, int TrackNum, OCPrintVarsType &dCurrent, float XFysic, OCCounter& CountIt, OCStaffAccidentals& lfortegn, XMLSymbolWrapper& Symbol, OCBarList& BarList, OCDraw& ScreenObj)
{
    int py1 = Py;
    clear();
    //XMLSymbolWrapper S(XMLVoice, py1, dCurrent.Meter);
    Props.MoveX = Symbol.pos().x();
    Props.Size=Symbol.size();
    Props.val = Symbol.ticks();
    Props.Rounded = CountIt.Rounded;
    Props.Beat = CountIt.Counter;
    Props.Bar = CountIt.BarCounter;
    Props.Meter = dCurrent.Meter;
    Props.FlipTie = dCurrent.FlipTie;
    Props.NumOfBeams = Props.FNclcfanr(Props.val);
    Append(TrackNum,py1, Symbol, ScreenObj.IsMarked(py1));
    forever
    {
        py1--;
        if (py1 < 0) break;
        XMLSymbolWrapper S(XMLVoice, py1, dCurrent.Meter);
        if (S.IsRestOrValuedNote())
        {
            break;
        }
        else if (S.IsCompoundNote())
        {
            Append(TrackNum,py1, S, ScreenObj.IsMarked(py1));
        }
    }
    if ((NoteHeadList.count() == 1) && (NoteHeadList[0]->NoteType == tstiednote))
    {
        Props.IsSingleNoteWithTie = true;
    }
    CalcCenters(dCurrent, XFysic);
    sort();
    AddToArrays(lfortegn);
    CalcAverage();
}

void CNotesToPrint::FillLineNumsArray(QList<int>& LineNums)
{
    LineNums.clear();
    foreach(CNoteHead* n,NoteHeadList)
    {
        LineNums.append(n->Pitch);
    }
}
/*
void CNotesToPrint::FillNotesPrinted(OCSymbolArray& SymbolList, int Py)
{
    foreach(CNoteHead* n,NoteHeadList)
    {
        SymbolList.Append(n->CenterX - (12 * 6), n->CenterY - (12 * 4), 12 * 8, 12 * 12, Py - n->OrigPlace, n->Size);
    }
}
*/
int CNotesToPrint::FNBalkHeight() const
{
    int c=0;
    if (Props.UpDown == 1)
    {
        c = lo - (28 * 12);
        if (c > (94 * 12)) c = (94 * 12);
    }
    else
    {
        c = hi + (28 * 12);
        if (c < (74 * 12)) c = (74 * 12);
    }
    return c;
}

const int CNotesToPrint::HasAccidental(const int NoteNum) const
{
    return NoteHeadList[NoteNum]->HasFortegn;
}

const int CNotesToPrint::LineNum(const int Index) const
{
    return NoteHeadList[Index]->Pitch;
}

const int CNotesToPrint::Moveaccidental(const int Notes, const int Accidentals) const
{
    int RetVal=0;
    foreach(CNoteHead* n,NoteHeadList)
    {
        if (n->HasFortegn)
        {
            RetVal ++;
            if (Notes > 0) n->FortegnAddX = n->FortegnAddX - (120 * Notes);
            if (Accidentals > 0) n->FortegnAddX = n->FortegnAddX - (96 * Accidentals);
        }
    }
    return RetVal;
}

void CNotesToPrint::MoveChord(int factor)
{
    Props.CenterX = Props.CenterX + (120 * factor);
    Props.balkx = Props.balkx + (120 * factor);
    foreach(CNoteHead* n,NoteHeadList)
    {
        n->CenterX = n->CenterX + (120 * factor);
    }
}

void CNotesToPrint::MoveTo(OCDraw& ScreenObj)
{
    CurrentNote->MoveTo(ScreenObj);
}

void CNotesToPrint::PlFane(OCDraw& ScreenObj)
{
    if (Props.NumOfBeams) OCNoteList::plFan(30*12,-Props.UpDown,Props.NumOfBeams,0,ScreenObj);
}

const int CNotesToPrint::NoteNum(const int Index) const
{
    return NoteHeadList[Index]->NoteNum;
}

const int CNotesToPrint::Pitch(const int NoteNum) const
{
    return NoteHeadList[NoteNum]->PitchCode;
}

void CNotesToPrint::plot(int& NextHeight, int NextX, int BarsToPrint, OCTieWrap& TieWrap, int TriolVal, QColor TrackColor, int &Py, OCSymbolArray& SymbolList, OCDraw& ScreenObj)
{
    int TieDirection = 1;
    if (Props.UpDown == -1)
    {
        for (int iTemp = 0; iTemp<NoteHeadList.count(); iTemp++)
        {
            CurrentNote=NoteHeadList[iTemp];
            //if (CurrentNote->Size > -1)
            if (!CurrentNote->Invisible) plNote(TieDirection,iTemp,NextHeight,NextX,BarsToPrint,TriolVal,TrackColor,TieWrap,SymbolList,ScreenObj); //GoSub 1630
            if ((iTemp+1) * 2 >= NoteHeadList.count()) TieDirection = -1;
        }
        CurrentNote=NoteHeadList.first();
    }
    else
    {
        for (int iTemp = NoteHeadList.count()-1; iTemp>=0; iTemp--)
        {
            CurrentNote=NoteHeadList[iTemp];
            if ((iTemp+1) * 2 <= NoteHeadList.count()) TieDirection = -1;
            //if (CurrentNote->Size > -1)
            if (!CurrentNote->Invisible) plNote(TieDirection,iTemp,NextHeight,NextX,BarsToPrint,TriolVal,TrackColor,TieWrap,SymbolList,ScreenObj); //GoSub 1630
        }
        CurrentNote=NoteHeadList.last();
    }
    CurrentNote->MoveTo(ScreenObj);
    if (!CurrentNote->Invisible) Props.PlotStem(ScreenObj);
    if ((Props.NumOfEvInBeam == 0) && (Props.ForceBeam == 0))
    {
        if (!CurrentNote->Invisible) PlFane(ScreenObj);
    }
    //FillNotesPrinted(SymbolList, Py);
}

//1630
void CNotesToPrint::plNote(int& TieDirection, int iTemp, int& NextHeight, int NextX, int BarsToPrint,int TriolVal, QColor TrackColor, OCTieWrap& TieWrap, OCSymbolArray& SymbolList, OCDraw& ScreenObj)
{
    PlBilinier(iTemp, ScreenObj);
    CurrentNote->MoveTo(ScreenObj);
    if (NextHeight <= 0) NextHeight = CurrentNote->CenterY; //'FNclclf%(a%, dcurrent)
    int TieLen = NextX - CurrentNote->CenterX - 192;
    int TieUpDown = Props.UpDown;
    if (Props.FlipTie) TieUpDown = -TieUpDown;
    if (Props.Bar == 0)
    {
        if (Props.Beat == Props.Rounded)
        {
            TieWrap.plot(CurrentNote->CenterX, CurrentNote->CenterY, CurrentNote->PitchCode, TieDirection, TieUpDown, ScreenObj);
        }
    }
    if ((CurrentNote->NoteType==tstiednote) || (CurrentNote->NoteType==tstiedpolynote))
    {
        CurrentNote->MoveTo(ScreenObj);
        if (TieLen < 336)
        {
            ScreenObj.DR(IntDiv(TieLen ,4), 0);
            TieLen = TieLen - IntDiv(TieLen ,2);
        }
        else
        {
            ScreenObj.DR(84, 0);
            TieLen = TieLen - 168;
        }
        bool LastTie=((Props.Beat == Props.Meter) & (Props.Bar == BarsToPrint - 1));
        TieWrap.PlotTie(LastTie, NoteHeadList.count(), TieUpDown, TieDirection, TieLen, CurrentNote->CenterY, NextHeight, ScreenObj);
    }
    CurrentNote->plot(Props.Size, TriolVal, TrackColor, Props.UpDown, SymbolList, ScreenObj);
}

void CNotesToPrint::SetHasFortegn(OCStaffAccidentals& lfortegn)
{
    QList<int> HasFortegn=lfortegn.GetHasFortegnArray(NoteHeadList.count());
    for (int iTemp = 0;iTemp<NoteHeadList.count();iTemp++)
    {
        NoteHeadList[iTemp]->HasFortegn = HasFortegn[iTemp];
    }
}

const int CNotesToPrint::fnAandClefCalc(const int a, OCPrintVarsType& b) const
{
    int Add=0;
    int Pitch = a % 12;
    if (b.J[Pitch] == 1)
    {
        Add = -1;
    }
    else if (b.J[Pitch] == 2)
    {
        Add = 1;
    }
    else
    {
        Add = 0;
    }
    int RetVal = ((int)(((a - 12.0) * 7.0 / 12.0) + 0.4 + Add) * 4 * 12); //used to be fix()
    switch (b.CurrentClef.val)
    {
    case 2:
        RetVal=RetVal - (4*12);
        break;
    case 3:
        RetVal=RetVal - (28*12);
        break;
    case 4:
        RetVal=RetVal - (20*12);
        break;
    default:
        RetVal=RetVal - (52*12);
    }
    return RetVal;
}

//--------------------------------------------------------------------------

OCNoteList::OCNoteList()
{
    BalkOverRideCount=0;
}

OCNoteList::~OCNoteList()
{
    clear();
}

void OCNoteList::Append(const float XFysic, XMLSymbolWrapper& Symbol, int& Py, QDomLiteElement* XMLVoice, const int TrackNum, OCPrintVarsType &dCurrent, OCCounter& CountIt, OCStaffAccidentals& lfortegn, OCBarList& BarList, QList<int> &LastTiedNotes, OCDraw& ScreenObj)
{
    //Static BalkOverRideCount As Integer
    IOCRhythmObject* s;
    int Ticks=Symbol.ticks();
    if (Symbol.IsRest())
    {
        s = new CPausesToPrint;
    }
    else
    {
        s = new CNotesToPrint;
    }
    s->Fill(Py, XMLVoice, TrackNum, dCurrent, XFysic, CountIt, lfortegn, Symbol, BarList, ScreenObj);
    RhythmObjectList.append(s);
    if (CountIt.Counter == CountIt.Rounded)
    {
        if (Symbol.IsAnyNote())
        {
            for (int iTemp=0;iTemp<s->Count();iTemp++) if (LastTiedNotes.contains(s->Pitch(iTemp))) lfortegn.AddIgnore(s->LineNum(iTemp));
        }
    }
    LastTiedNotes.clear();
    for (int iTemp = 0;iTemp<s->Count();iTemp++)
    {
        if ((s->NoteType(iTemp)==tstiednote) || (s->NoteType(iTemp)==tstiedpolynote))
        {
            LastTiedNotes.append(s->Pitch(iTemp));
        }
    }
    if (dCurrent.BalkOverRide.RemainingTicks > 0)
    {
        BalkOverRideCount++;
        s->Props.ForceBeam = BalkOverRideCount;
        s->Props.NumOfForcedBeams = dCurrent.BalkOverRide.val;
        dCurrent.BalkOverRide.Decrem(Ticks);
    }
    else
    {
        if (BalkOverRideCount > 0)
        {
            s->Props.ForceBeam = BalkOverRideCount + 1;
            s->Props.NumOfForcedBeams = dCurrent.BalkOverRide.val;
            BalkOverRideCount = 0;
        }
    }
    if (dCurrent.UpDown) s->Props.ForceUpDown = dCurrent.UpDown;
    if (dCurrent.SlantFlag) s->Props.ForceSlant = dCurrent.SlantFlag;
    if (dCurrent.UpDownOverRide.RemainingTicks > 0)
    {
        s->Props.ForceUpDown = dCurrent.UpDownOverRide.val;
        dCurrent.UpDownOverRide.Decrem(Ticks);
    }
    if (dCurrent.SlantOverRide.RemainingTicks > 0)
    {
        s->Props.ForceSlant = dCurrent.SlantOverRide.val;
        dCurrent.SlantOverRide.Decrem(Ticks);
    }
    s->Props.BeamLimit = dCurrent.BalkLimit;
    if (dCurrent.Articulationx1.val > 0)
    {
        s->Props.PerformanceSignPos = dCurrent.Articulationx1.Pos;
        s->Props.PerformanceSignSize = dCurrent.Articulationx1.Size;
        if (dCurrent.Articulationx1.val == 5)
        {
            s->Props.PerformanceSign = 1;
            dCurrent.Articulationx1.val = 0;
        }
        if (dCurrent.Articulationx1.val == 4)
        {
            s->Props.PerformanceSign = 2;
            dCurrent.Articulationx1.val = 0;
        }
    }
    else if (dCurrent.StregOverRide.RemainingTicks > 0)
    {
        s->Props.PerformanceSignPos = dCurrent.StregOverRide.Pos;
        s->Props.PerformanceSignSize = dCurrent.StregOverRide.Size;
        s->Props.PerformanceSign = 2;
        dCurrent.StregOverRide.Decrem(Ticks);
    }
    else if (dCurrent.Punktoverride.RemainingTicks > 0)
    {
        s->Props.PerformanceSignPos = dCurrent.Punktoverride.Pos;
        s->Props.PerformanceSignSize = dCurrent.Punktoverride.Size;
        s->Props.PerformanceSign = 1;
        dCurrent.Punktoverride.Decrem(Ticks);
    }
    else if (dCurrent.Articulation.val > 0)
    {
        s->Props.PerformanceSignPos = dCurrent.Articulation.Pos;
        s->Props.PerformanceSignSize = dCurrent.Articulation.Size;
        if (dCurrent.Articulation.val == 2) s->Props.PerformanceSign = 1;
        if (dCurrent.Articulation.val == 1) s->Props.PerformanceSign = 2;
    }
}

void OCNoteList::clear()
{
    qDeleteAll(RhythmObjectList);
    RhythmObjectList.clear();
}

void OCNoteList::SearchForBeams()
{
    //Dim iTemp As Integer
    int BalkLimit = 0;
    bool NoGood = false;
    int BeamCount = 0;
    //'If toprec < 1 Then Exit Sub
    for (int iTemp=0;iTemp<RhythmObjectList.count();iTemp++)
    {
        OCRhythmObject* s=&RhythmObjectList[iTemp]->Props;
        if (s->Beat == s->Rounded) BalkLimit = 0;
        NoGood = false;
        if (BeamCount > 0)
        {
            if (!s->BeamAble()) NoGood = true;
            if (s->Rounded > BalkLimit) NoGood = true;
            if (s->ForceBeam == 1) NoGood = true;
            if ((s->IsPause) && (s->ForceBeam == 0)) NoGood = true;
            //if (!NoGood) GoTo putit
        }
        if ((NoGood && (BeamCount > 0)) || (BeamCount==0))
        {
            if (iTemp == RhythmObjectList.count()-1) break;
            BalkLimit = Inside(BalkLimit, 1, s->BeamLimit, s->BeamLimit);
            if (BalkLimit > s->Meter) BalkLimit = s->Meter;
            NoGood = false;
            if (!s->BeamAble()) NoGood = true;
            if (!RhythmObjectList[iTemp + 1]->Props.BeamAble()) NoGood = true;
            if (s->Rounded >= s->Meter) NoGood = true;
            if (s->IsPause) NoGood = true;
            if (RhythmObjectList[iTemp + 1]->Props.IsPause) NoGood = true;
            if (s->Rounded + RhythmObjectList[iTemp + 1]->Props.Rounded > BalkLimit) NoGood = true;
            if (RhythmObjectList[iTemp + 1]->Props.Bar > s->Bar) NoGood = true;
            if (s->ForceBeam > 0) NoGood = true;
            if (RhythmObjectList[iTemp + 1]->Props.ForceBeam > 0) NoGood = true;
        }
//putit:
        if (NoGood)
        {
            BeamCount = 0;
            s->NumOfEvInBeam = 0;
        }
        else
        {
            BeamCount++;
            s->NumOfEvInBeam = BeamCount;
            if (s->Rounded >= BalkLimit) BeamCount = 0;
        }
        BalkLimit = BalkLimit - s->Rounded;
        BalkLimit = Inside(BalkLimit, 0, s->BeamLimit, s->BeamLimit);
    }
    BeamUpDown();
}

void OCNoteList::plot(const int iTemp, int& CurrentMeter, int& Py, OCBarList& BarList, OCTieWrap& TieWrap, const int TriolVal, const QColor TrackColor, OCSymbolArray& SymbolList, OCDraw& ScreenObj)
{
    int NextHeight;
    int NextX;
    if (iTemp >= RhythmObjectList.count()) return;
    if (iTemp < RhythmObjectList.count()-1)
    {
        NextHeight = RhythmObjectList[iTemp + 1]->Props.AverageY;
        NextX = RhythmObjectList[iTemp + 1]->Props.CenterX;
    }
    else
    {
        NextHeight = 86 * 12;
        NextX = BarList.SystemLength() + 144;
    }
    RhythmObjectList[iTemp]->plot(NextHeight, NextX, BarList.BarsToPrint(), TieWrap, TriolVal, TrackColor, Py, SymbolList, ScreenObj);
    CurrentMeter = RhythmObjectList[iTemp]->Props.Meter;
}

void OCNoteList::BeamUpDown()
{
    for (int iTemp=0;iTemp<RhythmObjectList.count();iTemp++)
    {
        IOCRhythmObject* s=RhythmObjectList[iTemp];
        if ((s->Props.NumOfEvInBeam == 1) || (s->Props.ForceBeam == 1))
        {
            bool Forced = false;
            if (s->Props.ForceBeam == 1) Forced = true;
            int iTemp1 = iTemp;
            int Added = s->Props.AverageY;
            forever
            {
                iTemp1 ++;
                if (iTemp1 >= RhythmObjectList.count()) break;
                if (Forced)
                {
                    if (RhythmObjectList[iTemp1]->Props.ForceBeam < iTemp1 - iTemp + 1) break;
                }
                else
                {
                    //'If objarr(iTemp1).Props.IsPause Then Exit Do
                    if (RhythmObjectList[iTemp1]->Props.NumOfEvInBeam < iTemp1 - iTemp + 1) break;
                }
                Added += RhythmObjectList[iTemp1]->Props.AverageY;
            }
            int LastNote = iTemp1 - 1;
            Added = IntDiv(Added, LastNote - iTemp + 1);
            if (Added > (86 * 12))
            {
                Added = 1;
            }
            else
            {
                Added = -1;
            }
            for (iTemp1 = iTemp; iTemp1<=LastNote;iTemp1++)
            {
                if (RhythmObjectList[iTemp1]->Props.ForceUpDown != 0)
                {
                    Added = RhythmObjectList[iTemp1]->Props.ForceUpDown;
                    break;
                }
            }
            for (iTemp1 = iTemp; iTemp1<=LastNote;iTemp1++)
            {
                RhythmObjectList[iTemp1]->Props.UpDown = Added;
                RhythmObjectList[iTemp1]->Props.BalkEndY = RhythmObjectList[iTemp1]->FNBalkHeight();
                RhythmObjectList[iTemp1]->CalcUpDownProps();
            }
            CalcBalk(iTemp, LastNote);
        }
        else if ((s->Props.NumOfEvInBeam == 0) && (s->Props.ForceBeam == 0))
        {
            SetUpDown(&s->Props);// 'dcurrent, isacombine, combinewith
            s->Props.BalkEndY = s->FNBalkHeight();
            s->CalcUpDownProps();
        }
    }
}

void OCNoteList::SetUpDown(OCRhythmObject *Props)
{
    if (Props->ForceUpDown)
    {
        Props->UpDown = Props->ForceUpDown;
    }
    else
    {
        if (Props->AverageY > (86 * 12))
        {
            Props->UpDown = 1;
        }
        else
        {
            Props->UpDown = -1;
        }
    }
}

void OCNoteList::CalcBalk(const int First, const int Last)
{
    float BalkFactor;
    int UpDown = RhythmObjectList[First]->Props.UpDown;
    int BalkTotal = RhythmObjectList[Last]->Props.balkx - RhythmObjectList[First]->Props.balkx;
    if (BalkTotal == 0) BalkTotal = 1;
    int FirstHd = RhythmObjectList[First]->Props.BalkEndY;
    int LastHd = RhythmObjectList[Last]->Props.BalkEndY;
    if (RhythmObjectList[First]->Props.ForceSlant)
    {
        BalkFactor = 0;
    }
    else
    {
        BalkFactor = (float)(FirstHd - LastHd) / (float)BalkTotal;
    }
    if (Abs(BalkFactor) > 0.5) BalkFactor = Sgn(BalkFactor) * 0.5;
    int balkheight = FirstHd;
    int addit = 0;
    for (int iTemp = First + 1;iTemp<=Last;iTemp++)
    {
        balkheight = balkheight - (BalkFactor * (RhythmObjectList[iTemp]->Props.balkx - RhythmObjectList[iTemp - 1]->Props.balkx));
        int add1 = RhythmObjectList[iTemp]->Props.BalkEndY - balkheight;
        if (add1 * UpDown < 0)
        {
            if (add1 * UpDown < addit * UpDown) addit = add1;
        }
    }
    FirstHd += addit;
    RhythmObjectList[First]->Props.BalkEndY = FirstHd;
    RhythmObjectList[First]->Props.SlantFactor = BalkFactor;
    for (int iTemp = First + 1;iTemp<=Last;iTemp++)
    {
        RhythmObjectList[iTemp]->Props.BalkEndY = FirstHd + ((RhythmObjectList[iTemp]->Props.balkx - RhythmObjectList[First]->Props.balkx) * -BalkFactor);
        RhythmObjectList[iTemp]->Props.SlantFactor = BalkFactor;
    }
}

void OCNoteList::PlotBeams(const QColor TrackColor, OCDraw& ScreenObj)
{
    ScreenObj.col = TrackColor;
    for (int iTemp=0;iTemp<RhythmObjectList.count();iTemp++)
    {
        IOCRhythmObject* s=RhythmObjectList[iTemp];
        if ((s->Props.NumOfEvInBeam == 1) || (s->Props.ForceBeam == 1))
        {
            bool Forced = false;
            if (s->Props.ForceBeam == 1) Forced = true;
            int iTemp1 = iTemp;
            int MinNumOfBeams = s->Props.NumOfBeams;
            forever
            {
                iTemp1 ++;
                if (iTemp1 >= RhythmObjectList.count()) break;
                if (RhythmObjectList[iTemp1]->Props.NumOfBeams < MinNumOfBeams) MinNumOfBeams = RhythmObjectList[iTemp1]->Props.NumOfBeams;
                if (Forced)
                {
                    if (RhythmObjectList[iTemp1]->Props.ForceBeam < iTemp1 - iTemp) break;
                }
                else
                {
                    //'If objarr[iTemp1]->Props.IsPause Then Exit Do
                    if (RhythmObjectList[iTemp1]->Props.NumOfEvInBeam < iTemp1 - iTemp + 1) break;
                }
            }
            if (s->Props.NumOfForcedBeams > MinNumOfBeams) MinNumOfBeams = s->Props.NumOfForcedBeams;
            int LastNote = iTemp1 - 1;
            plBeam(s->Props.balkx, s->Props.BalkEndY-(s->Props.UpDown*LineHalfThickNess), RhythmObjectList[LastNote]->Props.balkx, RhythmObjectList[LastNote]->Props.BalkEndY, s->Props.UpDown, MinNumOfBeams, ScreenObj);
            //'SlantFactor = -(objarr[LastNote]->Props.BalkEndY - s->Props.BalkEndY) / (objarr[LastNote]->Props.balkx - s->Props.balkx)
            for (int iTemp1 = iTemp;iTemp1<=LastNote;iTemp1++)
            {
                int LenLeft = 0;
                int LenRight = 0;
                int NumOfBeamsLeft = 0;
                int NumOfBeamsRight = 0;
                if ((iTemp1 > 0) && (iTemp1 > iTemp))
                {
                    NumOfBeamsLeft = RhythmObjectList[iTemp1 - 1]->Props.NumOfBeams;
                    LenLeft = (RhythmObjectList[iTemp1]->Props.balkx - RhythmObjectList[iTemp1 - 1]->Props.balkx);
                }
                if ((iTemp1 < RhythmObjectList.count()-1) && (iTemp1 < LastNote))
                {
                    NumOfBeamsRight = RhythmObjectList[iTemp1 + 1]->Props.NumOfBeams;
                    LenRight = (RhythmObjectList[iTemp1 + 1]->Props.balkx - RhythmObjectList[iTemp1]->Props.balkx);
                }
                if ((LenRight == 0) && (iTemp1 > 0) && (iTemp1 > iTemp)) LenRight = LenLeft; // '(LenLeft * objarr[iTemp1]->Props.Rounded) \ objarr[iTemp1 - 1]->Props.Rounded
                int a = RhythmObjectList[iTemp1]->Props.NumOfBeams;
                int b = NumOfBeamsRight;
                if (NumOfBeamsLeft >= RhythmObjectList[iTemp1]->Props.NumOfBeams)
                {
                    if (RhythmObjectList[iTemp1]->Props.NumOfBeams <= NumOfBeamsRight)
                    {
                        a = RhythmObjectList[iTemp1]->Props.NumOfBeams;
                        b = RhythmObjectList[iTemp1]->Props.NumOfBeams;
                    }
                }
                else
                {
                    if (NumOfBeamsLeft < NumOfBeamsRight)
                    {
                        a = NumOfBeamsLeft;
                        b = RhythmObjectList[iTemp1]->Props.NumOfBeams;
                    }
                }
                if (a)
                {
                    ScreenObj.DM(RhythmObjectList[iTemp1]->Props.balkx, RhythmObjectList[iTemp1]->Props.BalkEndY); // '- (84 * MinNumOfBeams * s->Props.UpDown)
                    int cbld = 0;
                    plleftsubbalk(a, MinNumOfBeams, LenRight / 2, NumOfBeamsLeft, s->Props.UpDown, s->Props.SlantFactor, cbld, ScreenObj);
                }
                if (b)
                {
                    ScreenObj.DM(RhythmObjectList[iTemp1]->Props.balkx, RhythmObjectList[iTemp1]->Props.BalkEndY); // '- (84 * MinNumOfBeams * s->Props.UpDown)
                    int cbld = LenRight;
                    plrightsubbalk(b, MinNumOfBeams, LenRight / 2, NumOfBeamsRight, s->Props.UpDown, RhythmObjectList[iTemp]->Props.SlantFactor, cbld, ScreenObj);
                }
            }
        }
    }
}

void OCNoteList::plleftsubbalk(const int num, const int commonbalk, const int Length, const int prevnum, const int UpDown, const float factor, int cbld, OCDraw& ScreenObj)
{
    for (int iTemp = 1; iTemp<=num;iTemp++)
    {
        if (iTemp <= commonbalk)
        {
            ScreenObj.DR(0, UpDown * (BeamThickness + BeamSpace));
            if (iTemp == prevnum) cbld = -Length; // ': Slant = -6
        }
        else
        {
            if (cbld != 0)
            {
                plBalk(cbld, factor, UpDown, ScreenObj);
            }
            else
            {
                ScreenObj.DR(0, UpDown * (BeamThickness + BeamSpace));
            }
            if (iTemp == prevnum) cbld = -Length; // ': Slant = -6
        }
    }
}

void OCNoteList::plrightsubbalk(const int num, const int commonbalk, const int Length, const int nextnum, const int UpDown, const float factor, int cbld, OCDraw& ScreenObj)
{
    for (int iTemp = 1; iTemp<=num;iTemp++)
    {
        if (iTemp <= commonbalk)
        {
            ScreenObj.DR(0, UpDown * (BeamThickness + BeamSpace));
            if (iTemp == nextnum) cbld = Length; // ': Slant = 6
        }
        else
        {
            plBalk(cbld, factor, UpDown, ScreenObj);
            if (iTemp == nextnum) cbld = Length; // ': Slant = 6
        }
    }
}

const QList<QGraphicsItem*> OCNoteList::PlotTuplet(const int iTemp, const int Beats, const int TupletNumber, const QPointF& Pos, const int Size, OCDraw& ScreenObj)
{
    int LastNote;
    int TupletY=0;
    int NoteLeft=0;
    int NoteRight=0;
    int Add = 0;
    QList<QGraphicsItem*> l;
    if (iTemp >= RhythmObjectList.count()) return l;
    for (int iTemp1 = iTemp; iTemp1<RhythmObjectList.count();iTemp1++)
    {
        Add += RhythmObjectList[iTemp1]->Props.val;
        LastNote = iTemp1;
        if (Add > Beats) break;
    }
    int TupletX = ((RhythmObjectList[LastNote]->Props.balkx - RhythmObjectList[iTemp]->Props.balkx) / 2) + RhythmObjectList[iTemp]->Props.balkx;
    for (int iTemp1 = iTemp; iTemp1<=LastNote; iTemp1++)
    {
        if (RhythmObjectList[iTemp1]->Props.balkx <= TupletX) NoteLeft = iTemp1;
        if (RhythmObjectList[iTemp1]->Props.balkx > TupletX)
        {
            NoteRight = iTemp1;
            break;
        }
    }

    if (RhythmObjectList[NoteLeft]->Props.IsPause)
    {
        if (!RhythmObjectList[NoteRight]->Props.IsPause)
        {
            if (RhythmObjectList[NoteLeft]->Props.UpDown!=RhythmObjectList[NoteRight]->Props.UpDown)
            {
                RhythmObjectList[NoteLeft]->Props.UpDown=RhythmObjectList[NoteRight]->Props.UpDown;
                RhythmObjectList[NoteLeft]->CalcUpDownProps();
                RhythmObjectList[NoteLeft]->Props.BalkEndY=RhythmObjectList[NoteLeft]->Props.BalkBeginY;
            }
        }
    }
    if (RhythmObjectList[NoteRight]->Props.IsPause)
    {
        if (!RhythmObjectList[NoteLeft]->Props.IsPause)
        {
            if (RhythmObjectList[NoteLeft]->Props.UpDown!=RhythmObjectList[NoteRight]->Props.UpDown)
            {
                RhythmObjectList[NoteRight]->Props.UpDown=RhythmObjectList[NoteLeft]->Props.UpDown;
                RhythmObjectList[NoteRight]->CalcUpDownProps();
                RhythmObjectList[NoteRight]->Props.BalkEndY=RhythmObjectList[NoteRight]->Props.BalkBeginY;
            }
        }
    }
    /*
    else
    {
        if (RhythmObjectList[NoteLeft]->Props.UpDown!=RhythmObjectList[NoteRight]->Props.UpDown)
        {
            RhythmObjectList[NoteRight]->Props.UpDown=RhythmObjectList[NoteLeft]->Props.UpDown;
            RhythmObjectList[NoteRight]->CalcUpDownProps();
            RhythmObjectList[NoteRight]->Props.BalkEndY+=RhythmObjectList[NoteRight]->Props.UpDown*192;
        }
    }
    */

    if ((RhythmObjectList[NoteRight]->Props.balkx - RhythmObjectList[NoteLeft]->Props.balkx) == 0) return l;
    float factor = (RhythmObjectList[NoteLeft]->Props.balkx - TupletX) / (RhythmObjectList[NoteRight]->Props.balkx - RhythmObjectList[NoteLeft]->Props.balkx);
    if (RhythmObjectList[NoteLeft]->Props.UpDown == RhythmObjectList[NoteRight]->Props.UpDown)
    {
        TupletY = ((RhythmObjectList[NoteRight]->Props.BalkEndY - RhythmObjectList[NoteLeft]->Props.BalkEndY) * factor) + RhythmObjectList[NoteLeft]->Props.BalkEndY;
        TupletY = TupletY - (RhythmObjectList[NoteLeft]->Props.UpDown * 96);
    }
    else
    {
        /*
        int UD1 = ((RhythmObjectList[NoteLeft]->Props.BalkEndY - RhythmObjectList[NoteRight]->Props.BalkBeginY) * factor) + RhythmObjectList[NoteRight]->Props.BalkBeginY;
        int UD2 = ((RhythmObjectList[NoteRight]->Props.BalkBeginY - RhythmObjectList[NoteLeft]->Props.BalkEndY) * factor) + RhythmObjectList[NoteLeft]->Props.BalkEndY;
        if (Abs(UD1 - (86 * 12)) < Abs(UD2 - (86 * 12)))
        {
            TupletY = UD1 + (192*RhythmObjectList[NoteLeft]->Props.UpDown);//48;
        }
        else
        {
            TupletY = UD2 + (192*RhythmObjectList[NoteRight]->Props.UpDown);//48;
        }
        */
        TupletY = RhythmObjectList[NoteRight]->Props.BalkBeginY + (RhythmObjectList[NoteRight]->Props.UpDown*192);
    }
    //ScreenObj.DM(TupletX,TupletY);
    ScreenObj.DM(Pos+QPointF(TupletX,TupletY));
    l.append(ScreenObj.plLet(QString::number(TupletNumber).trimmed(), Size, "times new roman", true, true, 120,Qt::AlignHCenter | Qt::AlignVCenter));
    return l;
}

void OCNoteList::PlotTuplet(const int iTemp, const int Beats, const int TupletNumber, const QPointF& Pos, const int Size, OCSymbolArray& SymbolList, const int Pointer, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l=PlotTuplet(iTemp,Beats,TupletNumber,Pos,Size,ScreenObj);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

const QList<QGraphicsItem*> OCNoteList::PlotSlur(const int iTemp, const int Beats, const int UpDown, const int Angle, const QPointF& Pos, const int Size, const bool IsAWrap, OCDraw& ScreenObj)
{
    int LastNote=0;
    int WrapLeft=0;
    int MoveBottom = 0;
    QList<QGraphicsItem*> l;
    //if (Size < 0) return l;
    //if (Size > 0) Size = Size - 10;
    if (UpDown == -1) MoveBottom = -192;
    int WrapRight = 360;
    if (IsAWrap) WrapLeft = 360;
    int Add = 0;
    if (iTemp >= RhythmObjectList.count()) return l;
    for (int iTemp1 = iTemp; iTemp1 < RhythmObjectList.count(); iTemp1++)
    {
        Add += RhythmObjectList[iTemp1]->Props.val;
        LastNote = iTemp1;
        if (Add > Beats)
        {
            WrapRight = 0;
            break;
        }
    }
    QPointF SlurPos(RhythmObjectList[iTemp]->Props.CenterX - 48 - WrapLeft,FNSlurY(iTemp, UpDown));
    int Length = (RhythmObjectList[LastNote]->Props.CenterX - SlurPos.x()) - 144 + WrapRight;
    int SlurY1 = FNSlurY(LastNote, UpDown);
    int Slant = SlurPos.y() - SlurY1 - Angle;
    ScreenObj.DM(SlurPos + Pos);
    l.append(OCTieWrap::PlSlur(Length,1,Slant,UpDown,Size*4,ScreenObj));
    return l;
}

void OCNoteList::PlotSlur(const int iTemp, const int Beats, const int UpDown, const int Angle, const QPointF& Pos, const int Size, OCSymbolArray& SymbolList, const bool IsAWrap, const int Pointer, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l=PlotSlur(iTemp,Beats,UpDown,Angle,Pos,Size,IsAWrap,ScreenObj);
    //SymbolList.Append(args["SlurX"] + MoveX, args["SlurY"] + MoveY + args["MoveBottom"], 192, 192, Pointer, Size);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer,ScreenObj.lastSlur);
    //SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

const int OCNoteList::FNSlurY(const int iTemp, const int UpDown) const
{
    int RetVal;
    if (UpDown == -1)
    {
        if (RhythmObjectList[iTemp]->Props.UpDown == 1)
        {
            RetVal = RhythmObjectList[iTemp]->Props.BalkEndY;
        }
        else
        {
            RetVal = RhythmObjectList[iTemp]->Props.BalkBeginY;
        }
    }
    else
    {
        if (RhythmObjectList[iTemp]->Props.UpDown == 1)
        {
            RetVal = RhythmObjectList[iTemp]->Props.BalkBeginY;
        }
        else
        {
            RetVal = RhythmObjectList[iTemp]->Props.BalkEndY;
        }
    }
    return RetVal + (UpDown * 24);
}

const QList<QGraphicsItem*> OCNoteList::PlotHairPin(const int iTemp, const int Beats, const int WhatType, const QPointF& Pos, const int Size, const bool IsAWrap, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    int LastNote=0;
    int Gap=0;
    int WrapLeft=0;
    //if (Size < 0) return l;
    int WrapRight = 360;
    if (IsAWrap) WrapLeft = 360;
    int Add = 0;
    if (iTemp >= RhythmObjectList.count()) return l;
    for (int iTemp1 = iTemp; iTemp1 < RhythmObjectList.count(); iTemp1++)
    {
        Add += RhythmObjectList[iTemp1]->Props.val;
        LastNote = iTemp1;
        if (Add > Beats)
        {
            WrapRight = 0;
            break;
        }
    }
    QPointF HPPos(RhythmObjectList[iTemp]->Props.CenterX - 48, ScoreBottomSymbolY+48);
    int Length = (RhythmObjectList[LastNote]->Props.CenterX - HPPos.x()) - 144 + WrapLeft + WrapRight;
    HPPos+=Pos;
    /*
    if (Size == 0)
    {
        Gap = 10;
    }
    else
    {
        Gap = Size;
    }
    */
    Gap=Size+10;
    ScreenObj.DM(HPPos + QPointF(-WrapLeft,0));
    if (WhatType == 0)
    {
        l.append(PlotHairPin(Length, Gap * 24, false, ScreenObj));
    }
    else if (WhatType == 1)
    {
        l.append(PlotHairPin(Length, Gap * 24, true, ScreenObj));
    }
    else if (WhatType == 2)
    {
        l.append(PlotHairPin((Length / 2) - 48, Gap * 24, true, ScreenObj));
        ScreenObj.DM(HPPos + QPointF((Length / 2)+48,0));
        l.append(PlotHairPin((Length / 2) - 48, Gap * 24, false, ScreenObj));
    }
    else if (WhatType == 3)
    {
        l.append(PlotHairPin((Length / 2) - 48, Gap * 24, false, ScreenObj));
        ScreenObj.DM(HPPos + QPointF((Length / 2)+48,0));
        l.append(PlotHairPin((Length / 2) - 48, Gap * 24, true, ScreenObj));
    }
    return l;
}

void OCNoteList::PlotHairPin(const int iTemp, const int Beats, const int WhatType, const QPointF& Pos, const int Size, OCSymbolArray& SymbolList, const bool IsAWrap, const int Pointer, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l=PlotHairPin(iTemp,Beats,WhatType,Pos,Size,IsAWrap,ScreenObj);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
    //SymbolList.Append(args["HPX"] + MoveX, args["HPY"] + MoveY - (args["Gap"] * 24), args["Gap"] * 24, -48, Pointer, 0);
}

void OCNoteList::plBeam(const float x1, const float y1, const float x2, const float y2, const int UpDown, const int NumOfBeams, OCDraw& ScreenObj)
{
    QPainterPath b(QPointF(0,0));
    for (int iTemp = 0;iTemp<=NumOfBeams-1;iTemp++)
    {
        QPainterPath p(QPointF(0,0));
        p.lineTo(QPointF(x2-x1,y1-y2)/ScreenObj.ScreenSize);
        p.lineTo(QPointF(x2-x1,(y1-y2)+(-UpDown*BeamThickness))/ScreenObj.ScreenSize);
        p.lineTo(QPointF(0,-UpDown*BeamThickness)/ScreenObj.ScreenSize);
        p.lineTo(0,0);
        b.addPath(p.translated(QPointF(0,(BeamThickness + BeamSpace) * iTemp * -UpDown)/ScreenObj.ScreenSize));
    }
    ScreenObj.DM(x1,y1);
    ScreenObj.translatePath(b);
    //b.translate(MovingPoint/ScreenSize);
    ScreenObj.plTextPath(b,true);
    //AppendToList(Scene->addPath(b,QPen(col),QBrush(col)));
}

void OCNoteList::plBalk(const float Length, const float factor, const int UpDown, OCDraw& ScreenObj)
{
    QPainterPath p(QPointF(0,0));
    p.lineTo(QPointF(Length,factor * Length)/ScreenObj.ScreenSize);
    p.lineTo(QPointF(Length,(factor * Length)+(-UpDown*BeamThickness))/ScreenObj.ScreenSize);
    p.lineTo(QPointF(0,-UpDown*BeamThickness)/ScreenObj.ScreenSize);
    p.lineTo(0,0);
    ScreenObj.translatePath(p);
    ScreenObj.plTextPath(p,true);
    //AppendToList(Scene->addPath(p.translated(MovingPoint/ScreenSize),QPen(col),QBrush(col)));
    ScreenObj.DR(0, (BeamSpace+BeamThickness) * UpDown);
}

const QList<QGraphicsItem*> OCNoteList::PlotHairPin(const int Length, const int gap, const bool IsDim, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    QPainterPath p(QPointF(0,0));
    if (IsDim)
    {
        p.moveTo(0,FloatDiv(-gap,2*ScreenObj.ScreenSize));
        p.lineTo(FloatDiv(Length,ScreenObj.ScreenSize),0);
        p.lineTo(0,FloatDiv(gap,2*ScreenObj.ScreenSize));
    }
    else
    {
        p.moveTo(FloatDiv(Length,ScreenObj.ScreenSize),FloatDiv(-gap,2*ScreenObj.ScreenSize));
        p.lineTo(0,0);
        p.lineTo(FloatDiv(Length,ScreenObj.ScreenSize),FloatDiv(gap,2*ScreenObj.ScreenSize));
    }
    //p.translate(MovingPoint/ScreenSize);
    ScreenObj.translatePath(p);
    //QGraphicsItem* i=Scene->addPath(p,QPen(col,FloatDiv(LineHalfThickNess*3,ScreenSize)));
    //AppendToList(i);
    //l.append(i);
    l.append(ScreenObj.plTextPath(p,true,FloatDiv(LineHalfThickNess*3,ScreenObj.ScreenSize)));
    return l;
}

const QList<QGraphicsItem*> OCNoteList::PlPunkt(const int Value, const int TriolVal, const int Size, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    switch (Value)
    {
    case 9:
    case 18:
    case 36:
    case 72:
    case 144:
        if (TriolVal == 0)
        {
            l.append(ScreenObj.plDot(10*12,0,Size));
        }
        break;
    case 3:
        case 6:
        case 12:
        case 24:
        case 48:
        case 96:
        if (TriolVal)
        {
            l.append(ScreenObj.plDot(10*12,0,Size));
        }
        break;
    }
    return l;
}

const QList<QGraphicsItem*> OCNoteList::PlotLengths(const int Sign, const QPointF& Pos, const int UpDown, const int Size, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    ScreenObj.DM(Pos);
    if (Sign == 1)
    {
        ScreenObj.DR(0, UpDown * 10 * 12);
        l.append(ScreenObj.plDot(0,0,Size));
    }
    else if (Sign == 2)
    {
        ScreenObj.DR(FnSize(-5 * 12,Size), UpDown * 10 * 12);
        l.append(ScreenObj.DL(FnSize(120,Size),0));
    }
    return l;
}

const QList<QGraphicsItem*> OCNoteList::plFan(const float height, const int updown, const int repeat, const int Size, OCDraw& ScreenObj)
{
    QPainterPath b=FanPath((height/ScreenObj.ScreenSize)/SizeFactor(Size),updown,repeat);
    ScreenObj.translatePath(b);
    return ScreenObj.plTextPath(b,true);
}

const QPainterPath OCNoteList::FanPath(const float height, const int updown, const int repeat)
{
    QPainterPath b(QPointF(0,0));
    if (repeat<1) return b;
    b.cubicTo(QPointF(0,12.0*updown),QPointF(18.0,17.0*updown),QPointF(6.5,35.0*updown));
    b.cubicTo(QPointF(14.2,20.0*updown),QPointF(6.5,15.0*updown),QPointF(0,10.0*updown));
    b.setFillRule(Qt::WindingFill);
    QPainterPath c(QPointF(0,0));
    c.cubicTo(QPointF(0,12.0*updown),QPointF(12,17.0*updown),QPointF(9.6,20.0*updown));
    c.cubicTo(QPointF(9.6,18.0*updown),QPointF(6.5,15.0*updown),QPointF(0,10.0*updown));
    QPainterPath d(QPointF(0,0));
    QPainterPath e(QPointF(0,0));
    switch (repeat)
    {
    case 2:
        b.addPath(c.translated(0,7.0*updown));
        break;
    case 3:
        d.cubicTo(QPointF(0,12.0*updown),QPointF(10.8,17.0*updown),QPointF(8.4,20.0*updown));
        d.cubicTo(QPointF(8.4,18.0*updown),QPointF(6.5,15.0*updown),QPointF(0,10.0*updown));
        b.addPath(c.translated(0,5.0*updown));
        b.addPath(d.translated(0,10.0*updown));
        break;
    case 4:
        e.cubicTo(QPointF(0,12.0*updown),QPointF(10.8,17.0*updown),QPointF(7.8,20.0*updown));
        e.cubicTo(QPointF(7.8,18.0*updown),QPointF(6.5,15.0*updown),QPointF(0,10.0*updown));
        b.addPath(c.translated(0,4.0*updown));
        b.addPath(c.translated(0,8.0*updown));
        b.addPath(e.translated(0,12.0*updown));
        break;
    }
    QMatrix m;
    float factor=height/35.0;
    m.scale(factor,factor);
    return b*m;
}

void OCNoteList::plotsigns(const int iTemp, OCSignList& SignsToPrint, OCSymbolArray& SymbolList, OCDraw& ScreenObj)
{
    int NextNoteX=0;
    if (iTemp >= RhythmObjectList.count()) return;
    OCRhythmObject* s=&RhythmObjectList[iTemp]->Props;
    if (s->IsPause) return;
    if (!s->IsSingleNoteWithTie)
    {
        ScreenObj.setcol(QColor(unselectablecolor));
        PlotLengths(s->PerformanceSign, QPointF(s->CenterX, s->BalkBeginY) + s->PerformanceSignPos, s->UpDown, s->PerformanceSignSize,ScreenObj);
        ScreenObj.resetcol();
    }
    if (iTemp < RhythmObjectList.count()-1)
    {
        NextNoteX = RhythmObjectList[iTemp + 1]->Props.CenterX;
    }
    else
    {
        NextNoteX = s->CenterX + 432;
    }
    int TieLen = NextNoteX - s->CenterX - 192;// Min(NextNoteX - s->CenterX - 192,1);
    if (TieLen<1) TieLen=1;
    RhythmObjectList[iTemp]->MoveTo(ScreenObj);
    SignsToPrint.PrintSign(TieLen, SymbolList, s->CenterX, s->UpDown, s->BalkEndY, s->BalkBeginY, ScreenObj);
    RhythmObjectList[iTemp]->MoveTo(ScreenObj);
}

void OCNoteList::SetHasFortegn(OCStaffAccidentals& lfortegn)
{
    if (RhythmObjectList.count())
    {
        if (!RhythmObjectList.last()->Props.IsPause)
        {
            RhythmObjectList.last()->SetHasFortegn(lfortegn);
        }
    }
}

void OCNoteList::SetVoiceUpDown(const int Voice)
{
    int UpDown=0;
    if (Voice % 2 == 1)
    {
        UpDown = 1;
    }
    else
    {
        UpDown = -1;
    }
    if (RhythmObjectList.count())
    {
        if (RhythmObjectList.last()->Props.ForceUpDown == 0)
        {
            RhythmObjectList.last()->Props.ForceUpDown = UpDown;
        }
    }
}

void OCNoteList::FillLineNumsArray(QList<int>& LineNums)
{
    if (RhythmObjectList.count())
    {
        RhythmObjectList.last()->FillLineNumsArray(LineNums);
    }
}

void OCNoteList::MoveChord(const int factor)
{
    if (RhythmObjectList.count())
    {
        RhythmObjectList.last()->MoveChord(factor);
    }
}

const int OCNoteList::Moveaccidental(const int Notes, const int Accidentals) const
{
    if (RhythmObjectList.count())
    {
        return RhythmObjectList.last()->Moveaccidental(Notes, Accidentals);
    }
    return 0;
}
