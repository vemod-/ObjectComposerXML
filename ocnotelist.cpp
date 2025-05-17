#include "ocnotelist.h"
#include "ocsymbolscollection.h"

void OCStaffAccidentals::SetKeyAccidentals(const int Key)
{
    static const uint FlatLines[7] = {6, 2, 5, 1, 4, 0, 3};
    static const uint SharpLines[7] = {3, 0, 4, 1, 5, 2, 6};
    uint Antal=CKey::NumOfAccidentals(Key);
    OCKeyAccidental AccSignFlag=CKey::AccidentalFlag(Key);
    StaffAccidentals.reset();
    OneOct.reset();
    if (Antal > 0)
    {
        for (uint n = 0; n < Antal; n++)
        {
            if (AccSignFlag == keyAccSharps)
            {
                StaffAccidentals.setAllOctaves(SharpLines[n]);
            }
            else if (AccSignFlag == keyAccFlats)
            {
                StaffAccidentals.setAllOctaves(FlatLines[n]);
            }
        }
    }
}

void OCStaffAccidentals::ProcessAccidentals(const OCScaleArray& Scale)
{
    OCOneOctaveFlagsArray WasHere;
    OCIntList IgnoreLines;
    for (const int& i : std::as_const(IgnorePitches)) IgnoreLines.append(CNotesToPrint::Pitch2LineNum(i,Scale));
    Process(WasHere, Scale, IgnoreLines);
    if (NoteItems.size() > 1) Process(WasHere, Scale, IgnoreLines);
}

void OCStaffAccidentals::Process(OCOneOctaveFlagsArray& WasHere, const OCScaleArray& Scale, const OCIntList& IgnoreLines)
{
    for (OCNoteAccidental& item : NoteItems)//(int iTemp = 0;iTemp<NoteItems.size();iTemp++)
    {
        //OCNoteAccidental& item=NoteItems[iTemp];
        OCNoteAccidentalTypes AccSign = item.getAccSign(Scale);
        if ((IgnorePitches.contains(item.NoteNum)) && ((!OneOct.isSet(item.LineNum)) || (!WasHere.isSet(item.LineNum))))
        {
            //Tie, same note, ignore this one
            if (AccSign != noteAccNone)
            {
                setCurrent(item,noteAccDirty);
                WasHere.unset(item.LineNum);
            }
        }
        else if (IgnoreLines.contains(int(item.LineNum)))
        {
            //Tie, different note on same line
            setNoteAccidental(WasHere,item,AccSign);
        }
        else if (!OneOct.isSet(item.LineNum))
        {
            if ((AccSign == noteAccNone) && StaffAccidentals.isSet(item.LineNum))
            {
                //natural
                setNoteAccidental(WasHere,item,AccSign);
            }
            else if ((!StaffAccidentals.isSet(item.LineNum)) && (AccSign != noteAccNone))
            {
                //flat or sharp
                setNoteAccidental(WasHere,item,AccSign);
            }
            else if ((AccSign == noteAccDoubleFlat) || (AccSign == noteAccDoubleSharp)) {
                //flat or sharp
                setNoteAccidental(WasHere,item,AccSign);
            }
        }
        else
        {
            //Octaves
            if ((AccSign != OneOct.current(item.LineNum)) || (OneOct.current(item.LineNum) != StaffAccidentals.current(item.LineNum)))
            {
                setNoteAccidental(WasHere,item,AccSign);
            }
        }
    }
}

void OCStaffAccidentals::setNoteAccidental(OCOneOctaveFlagsArray& WasHere, OCNoteAccidental& item, const OCNoteAccidentalTypes AccSign)
{
    if ((WasHere.isSet(item.LineNum)) && (AccSign != OneOct.current(item.LineNum)) && (OneOct.current(item.LineNum) != noteAccNone))
    {
        item.setSymbol(AccSign);
        setCurrent(item,noteAccDirty);
    }
    else
    {
        item.setSymbol(AccSign);
        setCurrent(item,AccSign);
    }
    WasHere.set(item.LineNum);
}

void OCStaffAccidentals::setCurrent(OCNoteAccidental& item, const OCNoteAccidentalTypes AccSign)
{
    StaffAccidentals.setCurrent(item.LineNum,AccSign);
    OneOct.setCurrent(item.LineNum,AccSign);
}

OCAccidentalSymbols OCStaffAccidentals::PopNoteAccidental()
{
    return (!NoteItems.isEmpty()) ? NoteItems.takeFirst().AccSymbol : accNone;
}

void OCStaffAccidentals::PushNote(const int LineNum, const int NoteNum)
{
    NoteItems.append(OCNoteAccidental(LineNum,NoteNum));
}

void OCStaffAccidentals::Clear()
{
    NoteItems.clear();
    IgnorePitches.clear();
}

void OCStaffAccidentals::AddIgnore(const OCIntList& Pitches)
{
    for (const int& Pitch : Pitches) if (!IgnorePitches.contains(Pitch)) IgnorePitches.append(Pitch);
}

//--------------------------------------------------------------------------

const OCGraphicsList CPausesToPrint::PrintPauseSign(const int Ticks, OCDraw& ScreenObj)
{
    ScreenObj.moveTo(mCenterX, mCenterY);
    switch (Ticks)
    {
    case 168:
    case 144:
    case 96:
    case 64:
        ScreenObj.move(sized(-5 * 12), 8 * 12);
        return ScreenObj.PlRect(120,-48,Size);
    case 48:
    case 32:
    case 72:
    case 84:
        ScreenObj.move(sized(-5 * 12), 0);
        return ScreenObj.PlRect(120,48,Size);
    case 24:
    case 16:
    case 36:
    case 42:
        ScreenObj.moveTo(mCenterX - sized(36), mCenterY - sized(66));
        return ScreenObj.plLet(OCTTFRestQuarter, Size);
    case 12:
    case 8:
    case 18:
    case 21:
        ScreenObj.moveTo(mCenterX - sized(36), mCenterY - sized(66));
        return ScreenObj.plLet(OCTTFRestEight, Size);
    case 6:
    case 4:
    case 9:
        ScreenObj.moveTo(mCenterX - sized(36), mCenterY - sized(66));
        return ScreenObj.plLet(OCTTFRestSixteen, Size);
    case 3:
    case 2:
        ScreenObj.moveTo(mCenterX - sized(36), mCenterY - sized(66));
        return ScreenObj.plLet(OCTTFRestTrirtytwo, Size);
    case 1:
        ScreenObj.moveTo(mCenterX - sized(36), mCenterY - sized(66));
        return ScreenObj.plLet(OCTTFRestSixtyfour, Size);
    }
    return OCGraphicsList();
}

void CPausesToPrint::PrintNumber(const int b, const int siz, OCDraw& ScreenObj)
{
    ScreenObj.plLet(QString::number(b).trimmed(), siz, "times new roman", true, false, 192);
}

void CPausesToPrint::CalcUpDownProps()
{
    BalkBeginY = mCenterY - directed(120 + 96 + (loBound<int>(0,NumOfBeams-1)*96));
    BalkX = stemX(mCenterX);
    CenterX = mCenterX;
}

void CPausesToPrint::Fill(const OCVoiceLocation& /*VoiceLocation*/, const XMLVoiceWrapper& /*XMLVoice*/, const double XFysic, OCPrintCounter& CountIt, OCStaffAccidentals& /*StaffAccidentals*/, const XMLSymbolWrapper& Symbol, OCPageBarList& BarList, OCDraw& /*ScreenObj*/)
{
    fillProps(Symbol,CountIt);
    Invisible = !Symbol.isVisible();
    IsPause = true;
    mCenterX = int(XFysic + Symbol.pos().x());
    AverageY = int(Symbol.moveY(1008));
    HighestY = AverageY;
    LowestY = AverageY;
    InvisibleMeter = BarList.invisibleMeter(Bar);
    mCenterY = AverageY;
    if (BarList.multiPause(Bar) > 1)
    {
        NumOfCompressed = BarList.multiPause(Bar);
        CountIt.FilePointer = CountIt.FilePointer + (BarList.multiPause(Bar) - 1);
        PointerAfterCompress = CountIt.FilePointer;
    }
}

const OCIntList CPausesToPrint::FillLineNumsArray() const
{
    return (Invisible) ? OCIntList() : OCIntList(1,34);
}

void CPausesToPrint::MoveChord(const int factor)
{
    CenterX += 120 * factor;
    BalkX += 120 * factor;
    mCenterX += 120 * factor;
}

void CPausesToPrint::plot(OCPrintCounter& CountIt, const int /*NextHeight*/, const double /*NextX*/, const int /*BarsToPrint*/, const QColor& TrackColor, const OCVoiceLocation& VoiceLocation, OCFrameArray& FrameList, OCDraw& ScreenObj)
{
    int Pnt = CountIt.FilePointer;
    ScreenObj.setcol(CountIt.FilePointer);
    OCGraphicsList l;
    if (NumOfCompressed > 1) {
        CountIt.FilePointer = PointerAfterCompress;
    }
    if (Invisible) return;
    if (NumOfCompressed > 1)
    {
        ScreenObj.moveTo(mCenterX, mCenterY);
        ScreenObj.move(-60, 48);
        PlMorePauses(NumOfCompressed, ScreenObj);
        //'ScreenObj.setsize Size
        Beat = Meter;
        ScreenObj.moveTo(mCenterX - 72, mCenterY + 288);
        ScreenObj.move(IntDiv((NumOfCompressed - 1) ,4) * 72, 0);
        PrintNumber(NumOfCompressed, 0, ScreenObj);
    }
    else
    {
        if ((NoteValue == Meter) && (!InvisibleMeter))
        {
            l.append(PrintPauseSign(96,ScreenObj));
        }
        else
        {
            l.append(PrintPauseSign(NoteValue, ScreenObj));
            ScreenObj.moveTo(mCenterX + sized(12), mCenterY + 48);
            l.append(OCNoteList::PlotDot(NoteValue, CountIt.isNormalTriplet(), 4, ScreenObj));
            if (ForceBeamIndex > 0)
            {
                ScreenObj.moveTo(mCenterX, mCenterY + directed(96));
                PlotStem(ScreenObj);
            }
        }
    }
    ScreenObj.col = TrackColor;
    FrameList.AppendGroup(ScreenObj.MakeGroup(l),OCSymbolLocation(VoiceLocation,Pnt));
}

void CPausesToPrint::PlMorePauses(const int NumOfBars, OCDraw& ScreenObj)
{
    for (int i = 1;i<=IntDiv(NumOfBars ,4)+1;i++)
    {
        if (i == IntDiv(NumOfBars ,4) + 1)
        {
            for (int i1 = 1; i1 <= NumOfBars % 4; i1++)
            {
                ScreenObj.PlRect(120,48);
                ScreenObj.move(0, -48);
            }
        }
        else
        {
            for (int i1 = 1; i1 <= 4; i1++)
            {
                ScreenObj.PlRect(120,48);
                ScreenObj.move(0, -48);
            }
        }
        ScreenObj.move(192, 192);
    }
}

//--------------------------------------------------------------------------

CNotesToPrint::~CNotesToPrint()
{
    clear();
}

void CNotesToPrint::clear()
{
    NoteHeadList.clear();
}

void CNotesToPrint::sort()
{
    for (int i = 0; i < NoteHeadList.size(); i++)
    {
        for (int j = 0; j < NoteHeadList.size(); j++)
        {
            if (NoteHeadList[i].PitchCode < NoteHeadList[j].PitchCode) NoteHeadList.swapItemsAt(j,i);
        }
    }
}

void CNotesToPrint::CalcCenters(OCPrintVarsType &voiceVars, const double XFysic)
{
    for (CNoteHead& n : NoteHeadList)
    {
        n.LineNum = Pitch2LineNum(n.PitchCode, voiceVars.Scale);
        n.CenterY = LineNum2CenterY(n.LineNum, voiceVars.clef());
        n.CenterX = int(XFysic + n.Left)  + (VorschlagList.size() * 66);
        n.FortegnAddX = n.AccidentalLeft;
    }
    for (int i = 0; i < VorschlagList.size(); i++)
    {
        CNoteHead& n = VorschlagList[i];
        n.LineNum = Pitch2LineNum(n.PitchCode, voiceVars.Scale);
        n.CenterY = LineNum2CenterY(n.LineNum, voiceVars.clef());
        n.CenterX = (int(XFysic + n.Left) - ((i * 132) + 156)) + (VorschlagList.size() * 66);
        n.FortegnAddX = n.AccidentalLeft;
    }
}

void CNotesToPrint::PrintHelpLinesDown(const CNoteHead& CurrentNote, OCDraw& ScreenObj)
{
    //if (CurrentNote.PitchCode == NoteHeadList.first().PitchCode)
    //{
    const double bold = (CurrentNote.NoteType < tsgracenote) ? 1 : 0.8;
    const int HalfWidth = HelpLineHalfWidth * bold;
        if (CurrentNote.CenterY <= 720)
        {
            ScreenObj.moveTo(CurrentNote.CenterX - HalfWidth, 720);
            for (int i = 720;i >= CurrentNote.CenterY; i += -96)
            {
                ScreenObj.line(HalfWidth * 2, 0, bold);
                ScreenObj.move(-HalfWidth * 2, -96);
            }
        }
    //}
}

void CNotesToPrint::PrintHelpLinesUp(const CNoteHead& CurrentNote, OCDraw& ScreenObj)
{
    //if (CurrentNote.PitchCode == NoteHeadList.last().PitchCode)
    //{
    const double bold = (CurrentNote.NoteType < tsgracenote) ? 1 : 0.8;
    const int HalfWidth = HelpLineHalfWidth * bold;
    if (CurrentNote.CenterY >= 1296)
    {
        ScreenObj.moveTo(CurrentNote.CenterX - HalfWidth, 1296);
        for (int i = 1296; i <= CurrentNote.CenterY; i += 96)
        {
            ScreenObj.line(HalfWidth * 2, 0, bold);
            ScreenObj.move(-HalfWidth * 2, 96);
        }
    }
    //}
}


void CNotesToPrint::Calcprops(CNoteHead& CurrentNote, const StemDirection UpDown, const int PrevNoteCenterY, const int NextNoteCenterY, int& CAvoid)
{
    int MoveAccidental=0;
    if (UpDown == StemUp)
    {
        if (CAvoid) {
            CAvoid = 0;
        }
        else {
            if (qAbs<int>(CurrentNote.CenterY - PrevNoteCenterY) < 60) {
                CAvoid = -108;
            }
        }
        MoveAccidental = (qAbs<int>(CurrentNote.CenterY - NextNoteCenterY) < 60) ? -180 : 0;
    }
    else
    {
        if (CAvoid) {
            CAvoid = 0;
            MoveAccidental = 0;
        }
        else {
            if (qAbs<int>(CurrentNote.CenterY - PrevNoteCenterY) < 60) {
                CAvoid = 108;
                MoveAccidental = -180;
            }
        }
    }
    CurrentNote.CenterX = CurrentNote.CenterX + CAvoid;
    CurrentNote.FortegnAddX = CurrentNote.FortegnAddX + MoveAccidental;
}

int CNotesToPrint::Count() const
{
    return NoteHeadList.size();
}

void CNotesToPrint::CalcUpDownProps()
{
    int NextNoteCenterY = -2400;
    int PrevNoteCenterY = -2400;
    int CAvoid = 0;
    if (UpDown == StemDown)
    {
        for (int i = 0; i < NoteHeadList.size(); i++)
        {
            NextNoteCenterY = (i < NoteHeadList.size()-1) ? NoteHeadList[i+1].CenterY : -200;
            Calcprops(NoteHeadList[i], UpDown, PrevNoteCenterY, NextNoteCenterY, CAvoid);
            PrevNoteCenterY = NoteHeadList[i].CenterY;
        }
    }
    else
    {
        for (int i = NoteHeadList.size() - 1; i >= 0; i--)
        {
            NextNoteCenterY = (i > 0) ? NoteHeadList[i - 1].CenterY : -200;
            Calcprops(NoteHeadList[i], UpDown, PrevNoteCenterY, NextNoteCenterY, CAvoid);
            PrevNoteCenterY = NoteHeadList[i].CenterY;
        }
    }
    CNoteHead& CurrentNote = (UpDown == StemDown) ? NoteHeadList.first() : NoteHeadList.last();
    BalkBeginY = CurrentNote.CenterY;
    BalkX = stemX(CurrentNote.CenterX);
    CenterX = CurrentNote.CenterX;
}

void CNotesToPrint::Fill(const OCVoiceLocation& VoiceLocation, const XMLVoiceWrapper& XMLVoice, const double XFysic, OCPrintCounter& CountIt, OCStaffAccidentals& StaffAccidentals, const XMLSymbolWrapper& Symbol, OCPageBarList& /*BarList*/, OCDraw& ScreenObj)
{
    fillProps(Symbol,CountIt);
    clear();
    NoteHeadList.append(CNoteHead(Symbol,OCSymbolLocation(VoiceLocation,CountIt.FilePointer),ScreenObj.IsMarked(CountIt.FilePointer)));
    for (int py = CountIt.FilePointer - 1; py >- 1; py--)
    {
        const XMLSymbolWrapper S = XMLVoice.XMLSymbol(py, CountIt.Meter);
        if (S.IsRestOrValuedNote()) break;
        if (S.IsCompoundNote()) NoteHeadList.append(CNoteHead(S,OCSymbolLocation(VoiceLocation,py),ScreenObj.IsMarked(py)));
        if (S.IsAnyVorschlag()) {
            auto n = CNoteHead(S,OCSymbolLocation(VoiceLocation,py),ScreenObj.IsMarked(py));
            n.Size = n.Size - 6;
            n.NoteVal = S.noteValueToTicks(S.noteValue(),S.dotted(),S.triplet());
            VorschlagList.append(n);
        }
    }
    if ((NoteHeadList.size() == 1) && (NoteHeadList[0].NoteType == tstiednote))
    {
        IsSingleNoteWithTie = true;
    }
    CalcCenters(CountIt, XFysic);
    sort();
    for (const CNoteHead& n : std::as_const(NoteHeadList)) if (!n.Invisible) StaffAccidentals.PushNote(n.LineNum,n.PitchCode);
    for (const CNoteHead& n : std::as_const(VorschlagList)) if (!n.Invisible) StaffAccidentals.PushNote(n.LineNum,n.PitchCode);
    CalcAverage();
}

const OCIntList CNotesToPrint::FillLineNumsArray() const
{
    OCIntList LineNums;
    for (const CNoteHead& n : NoteHeadList) if (!n.Invisible) LineNums.append(n.LineNum);
    for (const CNoteHead& n : VorschlagList) if (!n.Invisible) LineNums.append(n.LineNum);
    return LineNums;
}

int CNotesToPrint::Moveaccidental(const int Notes, const int Accidentals)
{
    int RetVal=0;
    for (CNoteHead& n : NoteHeadList)
    {
        if (!n.Invisible)
        {
            if (n.AccidentalSymbol != accNone)
            {
                RetVal ++;
                if (Notes > 0) n.FortegnAddX = n.FortegnAddX - (120 * Notes);
                if (Accidentals > 0) n.FortegnAddX = n.FortegnAddX - (96 * Accidentals);
            }
        }
    }
    return RetVal;
}

void CNotesToPrint::MoveChord(const int factor)
{
    CenterX += 120 * factor;
    BalkX += 120 * factor;
    for (CNoteHead& n : NoteHeadList) if (!n.Invisible) n.CenterX += 120 * factor;
}

void CNotesToPrint::MoveTo(OCDraw& ScreenObj)
{
    CNoteHead& CurrentNote = (UpDown == StemDown) ? NoteHeadList.first() : NoteHeadList.last();
    CurrentNote.moveTo(ScreenObj);
}

void CNotesToPrint::PlFane(OCDraw& ScreenObj)
{
    if (NumOfBeams) OCNoteList::plFan(30*12,UpDown,NumOfBeams,0,ScreenObj);
}

void CNotesToPrint::plot(OCPrintCounter& CountIt, const int NextHeight, const double NextX, const int BarsToPrint, const QColor& TrackColor, const OCVoiceLocation& /*VoiceLocation*/, OCFrameArray& FrameList, OCDraw& ScreenObj)
{
    int TieDirection = 1;
    PrintHelpLinesDown(NoteHeadList.first(), ScreenObj);
    PrintHelpLinesUp(NoteHeadList.last(),ScreenObj);
    for (CNoteHead& n : VorschlagList) {
        PrintHelpLinesDown(n, ScreenObj);
        PrintHelpLinesUp(n,ScreenObj);
    }
    if (UpDown == StemDown)
    {
        for (int i = 0; i < NoteHeadList.size(); i++)
        {
            if (!NoteHeadList[i].Invisible) plotNote(NoteHeadList[i], TieDirection,NextHeight,NextX,BarsToPrint,CountIt.isNormalTriplet(),TrackColor,CountIt.TieWrap,FrameList,ScreenObj); //GoSub 1630
            if ((i+1) * 2 >= NoteHeadList.size()) TieDirection = -1;
        }
        for (int i = 0; i < VorschlagList.size(); i++)
        {
            if (!VorschlagList[i].Invisible) plotNote(VorschlagList[i], TieDirection, NoteHeadList.first().CenterY, NoteHeadList.first().CenterX,BarsToPrint,CountIt.isNormalTriplet(),TrackColor,CountIt.TieWrap,FrameList,ScreenObj); //GoSub 1630
            //if ((i+1) * 2 >= NoteHeadList.size()) TieDirection = -1;
        }
    }
    else
    {
        for (int i = NoteHeadList.size() - 1; i >= 0; i--)
        {
            if ((i+1) * 2 <= NoteHeadList.size()) TieDirection = -1;
            if (!NoteHeadList[i].Invisible) plotNote(NoteHeadList[i], TieDirection,NextHeight,NextX,BarsToPrint,CountIt.isNormalTriplet(),TrackColor,CountIt.TieWrap,FrameList,ScreenObj); //GoSub 1630
        }
        for (int i = VorschlagList.size() - 1; i >= 0; i--)
        {
            //if ((i+1) * 2 <= VorschlagList.size()) TieDirection = -1;
            if (!VorschlagList[i].Invisible) plotNote(VorschlagList[i], TieDirection,NoteHeadList.first().CenterY, NoteHeadList.first().CenterX,BarsToPrint,CountIt.isNormalTriplet(),TrackColor,CountIt.TieWrap,FrameList,ScreenObj); //GoSub 1630
        }
    }
    CNoteHead& CurrentNote = (UpDown == StemDown) ? NoteHeadList.first() : NoteHeadList.last();
    CurrentNote.moveTo(ScreenObj);
    if (!CurrentNote.Invisible) {
        PlotStem(ScreenObj);
        if ((AutoBeamIndex == 0) && (ForceBeamIndex == 0)) PlFane(ScreenObj);
    }
    for (CNoteHead& n : VorschlagList) {
        if (!n.Invisible) {
            n.moveTo(ScreenObj);
            const double bold = 0.8;
            const int beamCount = NumOfFlags(n.NoteVal);
            const double BalkBeginY = n.CenterY - n.sized(directed(120 + 96 + (beamCount * 96)));
            const double BalkX = stemX(n.CenterX);
            if ((n.NoteVal != 144) && (n.NoteVal != 96) && (n.NoteVal != 64))
            {
                ScreenObj.line(BalkX, BalkBeginY, 0, n.CenterY - BalkBeginY, bold);
                ScreenObj.line(BalkX - 72, BalkBeginY + directed(144), 144, directed(-144), bold);
                ScreenObj.moveTo(BalkX,BalkBeginY);
                if (beamCount) OCNoteList::plFan(30*12,UpDown,beamCount,n.Size,ScreenObj);
            }
        }
    }
}

void CNotesToPrint::plotNote(CNoteHead& CurrentNote, const int TieDirection, const int NextCenterY, const double NextX, const int BarsToPrint,const bool UnderTriplet, const QColor& TrackColor, OCTieWrap& TieWrap, OCFrameArray& FrameList, OCDraw& ScreenObj)
{
    CurrentNote.moveTo(ScreenObj);
    OCGraphicsList t;
    //if (NextCenterY <= 0) NextCenterY = CurrentNote.CenterY;
    auto TieLen = int(NextX - CurrentNote.CenterX - 192);
    int TieUpDown = UpDown;
    if (FlipTie) TieUpDown = -TieUpDown;
    if (CurrentNote.NoteType < tsgracenote) {
        if (Bar == 0)
        {
            if (Beat == Rounded)
            {
                TieWrap.plotWrappedTie(CurrentNote.CenterX, CurrentNote.CenterY, CurrentNote.PitchCode, TieDirection, TieUpDown, ScreenObj);
            }
        }
        if ((CurrentNote.NoteType==tstiednote) || (CurrentNote.NoteType==tstiedpolynote))
        {
            CurrentNote.moveTo(ScreenObj);
            ScreenObj.move(0,CurrentNote.TieTop);
            if (TieLen < 336)
            {
                ScreenObj.move(IntDiv(TieLen ,4), 0);
                TieLen = TieLen - IntDiv(TieLen ,2);
            }
            else
            {
                ScreenObj.move(84, 0);
                TieLen = TieLen - 168;
            }
            bool IsWrap=((Beat == Meter) & (Bar == BarsToPrint - 1));
            t.append(TieWrap.plotTie(IsWrap, NoteHeadList.size(), TieUpDown, TieDirection, TieLen, CurrentNote.CenterY, NextCenterY, ScreenObj));
        }
        CurrentNote.plot(UnderTriplet, TrackColor, UpDown, FrameList, t, ScreenObj);
    }
    else {
        if (CurrentNote.NoteType==tstiedgracenote)
        {
            CurrentNote.moveTo(ScreenObj);
            ScreenObj.move(0,CurrentNote.TieTop);
            if (TieLen < 336)
            {
                ScreenObj.move(IntDiv(TieLen ,4), 0);
                TieLen = TieLen - IntDiv(TieLen ,2);
            }
            else
            {
                ScreenObj.move(84, 0);
                TieLen = TieLen - 168;
            }
            t.append(TieWrap.plotTie(false, 1, TieUpDown, TieDirection, TieLen, CurrentNote.CenterY, NextCenterY, ScreenObj, 0.5));
        }
        CurrentNote.plot(UnderTriplet, TrackColor, UpDown, FrameList, t, ScreenObj);
    }
}

void CNotesToPrint::ApplyAccidentals(OCStaffAccidentals& StaffAccidentals)
{
    for (CNoteHead& n : NoteHeadList) n.AccidentalSymbol = StaffAccidentals.PopNoteAccidental();
    for (CNoteHead& n : VorschlagList) n.AccidentalSymbol = StaffAccidentals.PopNoteAccidental();
}

int CNotesToPrint::LineNum2CenterY(const int LineNum, const int Clef) const
{
    return ((LineNum - 7)*4*12)-(CClef::LineDiff(Clef)*12);
}

//--------------------------------------------------------------------------

OCNoteList::~OCNoteList()
{
    clear();
}

IOCRhythmObject* OCNoteList::Append(const double XFysic, const XMLSymbolWrapper& Symbol, const OCVoiceLocation& VoiceLocation, const XMLVoiceWrapper& XMLVoice, OCPrintCounter& CountIt, OCStaffAccidentals& StaffAccidentals, OCPageBarList& BarList, OCDraw& ScreenObj)
{
    IOCRhythmObject* s = (Symbol.IsRest()) ? static_cast<IOCRhythmObject*>(new CPausesToPrint) : new CNotesToPrint;

    s->Fill(VoiceLocation, XMLVoice, XFysic, CountIt, StaffAccidentals, Symbol, BarList, ScreenObj);
    RhythmObjectList.append(s);
    if (CountIt.Counter.TickCounter == CountIt.Counter.CurrentTicksRounded)
    {
        if (Symbol.IsPitchedNote()) StaffAccidentals.AddIgnore(CountIt.Ties);
    }
    CountIt.Ties.clear();
    for (int i = 0; i < s->Count(); i++)
    {
        if ((s->NoteType(i)==tstiednote) || (s->NoteType(i)==tstiedpolynote))
        {
            CountIt.Ties.append(s->Pitch(i));
        }
    }
    OCDurSignVector l = CountIt.DurSigns.signs("Beam");
    if (!l.empty())
    {
        OCDurSignType d = l.first();
        if (d.remains())
        {
            if (RhythmObjectList.size() == 1) d.Counter = 0;
            s->ForceBeamIndex = d.Counter + 1;
            s->NumOfForcedBeams = d.XMLSymbol.getIntVal("Beams");
        }
    }
    s->ForceUpDown = StemDirection(CountIt.UpDown.getValue(s->ForceUpDown));
    s->ForceSlant = CountIt.SlantFlag.getValue(s->ForceSlant);
    s->BeamLimit = CountIt.BalkLimit;
    const OCSignType& art = CountIt.Articulation.getSign();
    if (art.val)
    {
        s->PerformanceSign = art.val % 3;
        s->PerformanceSignPos = art.XMLSymbol.pos();
        s->PerformanceSignSize = art.XMLSymbol.size();
    }
    s->CenterNextXAdd = BarList.calcX(CountIt.barCount(),CountIt.Counter.CurrentTicksRounded);
    return s;
}

void OCNoteList::clear()
{
    qDeleteAll(RhythmObjectList);
    RhythmObjectList.clear();
    //qDeleteAll(BeamLists);
    BeamLists.clear();
}

void OCNoteList::CreateBeamLists()
{
    int AutoBeamLimit = 0;
    int AutoBeamCount = 0;
    OCBeamList TempBeamList;

    for (int i = 0; i < RhythmObjectList.size(); ++i) {
        IOCRhythmObject* s = RhythmObjectList[i];

        // Reset BalkLimit if start of bar
        if (s->Beat == s->Rounded) AutoBeamLimit = 0;
        // Check if current beam should end
        if (AutoBeamCount > 0) if (s->shouldEndBeam(AutoBeamLimit)) AutoBeamCount = 0;
        // Prepare for possible new beam
        bool canStartBeam = false;
        if (AutoBeamCount == 0 && i < RhythmObjectList.size() - 1) {
            AutoBeamLimit = qMin(boundStep<int>(1, AutoBeamLimit, s->BeamLimit, s->BeamLimit), s->Meter);
            canStartBeam = s->shouldStartBeam(RhythmObjectList[i + 1], AutoBeamLimit);
        }
        if (canStartBeam) {
            AutoBeamCount = 1;
            s->AutoBeamIndex = 1;
        }
        else if (AutoBeamCount > 0) {
            s->AutoBeamIndex = ++AutoBeamCount;
            if (s->Rounded >= AutoBeamLimit) AutoBeamCount = 0; //end beam
        } else {
            s->AutoBeamIndex = 0;
        }
        // Update BalkLimit after this symbol
        AutoBeamLimit = boundStep<int>(0, AutoBeamLimit - s->Rounded, s->BeamLimit, s->BeamLimit);

        if (s->ForceBeamIndex + s->AutoBeamIndex < TempBeamList.size()) {
            TempBeamList.CalcBalk();
            BeamLists.append(OCBeamList(TempBeamList));
            TempBeamList.clear();
        }
        if (s->ForceBeamIndex + s->AutoBeamIndex > 0) {
            TempBeamList.append(s);
        }
        else {
            s->SetUpDown();
        }
    }
    if (TempBeamList.size()) {
        TempBeamList.CalcBalk();
        BeamLists.append(OCBeamList(TempBeamList));
    }
}

void OCNoteList::plot(const int FirstNote, OCPrintCounter& CountIt, const OCVoiceLocation& VoiceLocation, const OCPageBarList& BarList, const QColor& TrackColor, OCFrameArray& FrameList, OCDraw& ScreenObj)
{
    if (FirstNote >= RhythmObjectList.size()) return;
    IOCRhythmObject* s = RhythmObjectList[FirstNote];
    int NextHeight = 86 * 12;
    double NextX = BarList.systemLength() + 144;
    if (s != RhythmObjectList.last())
    {
        NextHeight = RhythmObjectList[FirstNote + 1]->AverageY;
        NextX = RhythmObjectList[FirstNote + 1]->CenterX;
    }
    s->plot(CountIt, NextHeight, NextX, BarList.barsToPrint(), TrackColor, VoiceLocation, FrameList, ScreenObj);
    CountIt.Meter = s->Meter;
}
/*
void OCNoteList::FillBeamLists()
{
    for (int i = 0; i < RhythmObjectList.size(); i++)
    {
        IOCRhythmObject* s = RhythmObjectList[i];
        if ((s->AutoBeamIndex == 1) || (s->ForceBeamIndex == 1))
        {
            OCBeamList l;
            l.append(s);
            for (int j = i + 1; j < RhythmObjectList.size(); j++)
            {
                IOCRhythmObject* r = RhythmObjectList[j];
                const int idx = j - i + 1;
                if (s->ForceBeamIndex == 1) {
                    if (r->ForceBeamIndex < idx) break;
                } else {
                    if (r->AutoBeamIndex < idx) break;
                }
                l.append(r);
            }
            l.CalcBalk();
            BeamLists.append(l);
        }
    }
}
*/
void OCNoteList::PlotBeams(const QColor& TrackColor, OCDraw& ScreenObj)
{
    ScreenObj.col = TrackColor;
    for (OCBeamList& l : BeamLists) l.plot(ScreenObj);
}

const OCGraphicsList OCNoteList::PlotTuplet(const OCRhythmObjectList& TupletList, const int TupletCaption, const QPointF& Pos, const int Size, OCDraw& ScreenObj, const int YOffset)
{
    OCGraphicsList l;
    if (TupletList.isEmpty()) return l;
    IOCRhythmObject* NoteL = TupletList.first();
    IOCRhythmObject* NoteR = TupletList.last();
    QPointF lineL(NoteL->directed(NoteL->BalkEnd(),108));
    QPointF lineR(NoteL->directed(NoteR->BalkEnd(),108));
    if (NoteL->UpDown != NoteR->UpDown) {
        lineR.setY(NoteR->BalkBeginY - NoteL->directed(108));
    }
    const int lineUD = NoteL->UpDown;
    lineL += Pos - QPointF(0,lineUD * YOffset);
    lineR += Pos - QPointF(0,lineUD * YOffset);
    const QPointF center = (lineL + lineR) / 2.0;
    QVector2D dir(lineR - lineL);
    dir.normalize();
    l.append(ScreenObj.line(lineL, center - QPointF(dir.x() * 60, dir.y() * 60)));
    l.append(ScreenObj.line(center + QPointF(dir.x() * 60, dir.y() * 60),lineR));
    l.append(ScreenObj.line(lineL, 0, lineUD * 36));
    l.append(ScreenObj.line(lineR, 0, lineUD * 36));
    ScreenObj.moveTo(center);
    l.append(ScreenObj.plLet(QString::number(TupletCaption).trimmed(), Size, "times new roman", true, true, 132,Qt::AlignCenter));
    return l;
}

const OCGraphicsList OCNoteList::PlotSlur(const OCRhythmObjectList& SlurList, const XMLSimpleSymbolWrapper& XMLSymbol, const bool IsAWrap, OCDraw& ScreenObj)
{
    const SlurTypes UpDown = CDurSlur::SlurDirection(XMLSymbol.getIntVal("Direction"),SlurList);
    const int Angle = XMLSymbol.getIntVal("Angle");
    const QPointF& Pos = XMLSymbol.pos();
    const int Size = XMLSymbol.getIntVal("Curve");
    int WrapLeft=0;
    OCGraphicsList l;
    int WrapRight = 360;
    if (IsAWrap) WrapLeft = 360;

    if (SlurList.isEmpty()) return l;
    if (!SlurList.isWrapRight) WrapRight=0;

    //const double lastX = (SlurList.size() < 2) ? SlurList.first()->CenterX + SlurList.first()->CenterNextXAdd : SlurList.last()->CenterX + 24;
    const double lastX = SlurList.last()->CenterX + 24;
    QPointF SlurPos(SlurList.first()->CenterX - 60 - WrapLeft,SlurList.first()->SlurY(UpDown));
    const double Length = (lastX - SlurPos.x()) - 144 + WrapRight;
    const double SlurY1 = SlurList.last()->SlurY(UpDown);
    const double EndHeight = SlurPos.y() - SlurY1 - Angle;
    ScreenObj.moveTo(SlurPos + Pos);
    l.append(OCTieWrap::plotSlur(QPointF(Length,EndHeight),UpDown,Size*4,ScreenObj));
    return l;
}

const OCGraphicsList OCNoteList::PlotHairPin(const OCRhythmObjectList& HPList, const XMLSimpleSymbolWrapper& XMLSymbol, const bool IsAWrap, OCDraw& ScreenObj)
{
    const HairpinTypes WhatType = HairpinTypes(XMLSymbol.getIntVal("HairpinType"));
    const QPointF& Pos = XMLSymbol.pos();
    const int Size = XMLSymbol.getIntVal("Gap");
    OCGraphicsList l;
    int Gap=0;
    int WrapLeft=0;
    int WrapRight = 360;
    if (IsAWrap) WrapLeft = 360;

    if (HPList.isEmpty()) return l;
    if (!HPList.isWrapRight) WrapRight=0;

    const int lastX = (HPList.size() < 2) ? HPList.first()->CenterX + HPList.first()->CenterNextXAdd : HPList[HPList.size()-2]->CenterX + HPList[HPList.size()-2]->CenterNextXAdd;

    QPointF HPPos(HPList.first()->CenterX - 48, ScoreBottomSymbolY+48);
    auto Length = int((lastX - HPPos.x()) - 144 + WrapLeft + WrapRight);
    HPPos+=Pos;
    Gap=Size+10;
    ScreenObj.moveTo(HPPos + QPointF(-WrapLeft,0));
    if (WhatType == HairpinTypes::Cresc)
    {
        l.append(PlotHairPin(Length, Gap * 24, false, ScreenObj));
    }
    else if (WhatType == HairpinTypes::Dim)
    {
        l.append(PlotHairPin(Length, Gap * 24, true, ScreenObj));
    }
    else if (WhatType == HairpinTypes::Fish)
    {
        l.append(PlotHairPin((Length / 2) - 48, Gap * 24, true, ScreenObj));
        ScreenObj.moveTo(HPPos + QPointF((Length / 2.0)+48,0));
        l.append(PlotHairPin((Length / 2) - 48, Gap * 24, false, ScreenObj));
    }
    else if (WhatType == HairpinTypes::InvFish)
    {
        l.append(PlotHairPin((Length / 2) - 48, Gap * 24, false, ScreenObj));
        ScreenObj.moveTo(HPPos + QPointF((Length / 2.0)+48,0));
        l.append(PlotHairPin((Length / 2) - 48, Gap * 24, true, ScreenObj));
    }
    return l;
}

const OCGraphicsList OCNoteList::PlotHairPin(const int Length, const int gap, const bool IsDim, OCDraw& ScreenObj)
{
    OCGraphicsList l;
    QPainterPath p(QPointF(0,0));
    if (IsDim)
    {
        p.moveTo(0,DoubleDiv(-gap,2*ScreenObj.ScreenSize));
        p.lineTo(DoubleDiv(Length,ScreenObj.ScreenSize),0);
        p.lineTo(0,DoubleDiv(gap,2*ScreenObj.ScreenSize));
    }
    else
    {
        p.moveTo(DoubleDiv(Length,ScreenObj.ScreenSize),DoubleDiv(-gap,2*ScreenObj.ScreenSize));
        p.lineTo(0,0);
        p.lineTo(DoubleDiv(Length,ScreenObj.ScreenSize),DoubleDiv(gap,2*ScreenObj.ScreenSize));
    }
    ScreenObj.translatePath(p);
    l.append(ScreenObj.plTextPath(p,true,DoubleDiv(LineHalfThickNess * 3,ScreenObj.ScreenSize)));
    return l;
}

const OCGraphicsList OCNoteList::PlotDot(const int Value, const bool UnderTriplet, const int Size, OCDraw& ScreenObj)
{
    OCGraphicsList l;
    if (OCCounter::isDoubleDotted(Value))
    {
        l.append(ScreenObj.plDot(Size,5*12,0,Size));
        l.append(ScreenObj.plDot(Size,8*12,0,Size));
    }
    if (OCCounter::isDotted(Value))
    {
        if (!UnderTriplet)
        {
            l.append(ScreenObj.plDot(Size,5*12,0,Size));
        }
    }
    if (OCCounter::isStraight(Value))
    {
        if (UnderTriplet)
        {
            l.append(ScreenObj.plDot(Size,5*12,0,Size));
        }
    }
    return l;
}

const OCGraphicsList OCNoteList::PlotLengths(const int Sign, const QPointF& Pos, const int UpDown, const int Size, OCDraw& ScreenObj)
{
    OCGraphicsList l;
    ScreenObj.moveTo(Pos);
    if (Sign == 2)
    {
        ScreenObj.move(0, UpDown * 10 * 12);
        l.append(ScreenObj.plDot(Size + 3,0,0,Size));
    }
    else if (Sign == 1)
    {
        ScreenObj.move(FnSize(-5 * 12,Size), UpDown * 10 * 12);
        l.append(ScreenObj.line(FnSize(120,Size),0,1.4));
    }
    return l;
}

const OCGraphicsList OCNoteList::PlotLengths(const IOCRhythmObject* Props, OCDraw& ScreenObj)
{
    return PlotLengths(Props->PerformanceSign, Props->Center() + Props->PerformanceSignPos, Props->UpDown, Props->PerformanceSignSize,ScreenObj);
}

const OCGraphicsList OCNoteList::plFan(const double height, const StemDirection updown, const int repeat, const int Size, OCDraw& ScreenObj)
{
    QPainterPath b=FanPath((height/ScreenObj.ScreenSize)/SizeFactor(Size),StemDirection(-updown),repeat);
    ScreenObj.translatePath(b);
    return ScreenObj.plTextPath(b,true);
}

const QPainterPath OCNoteList::FanPath(const double height, const StemDirection updown, const int repeat)
{
    QPainterPath b(QPointF(0,0));
    if (repeat < 1) return b;
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
    QTransform m;
    double factor=height/35.0;
    m.scale(factor,factor);
    return b*m;
}

void OCNoteList::plotsigns(const int FirstNote, OCPrintSignList& SignsToPrint, OCFrameArray& FrameList, OCDraw& ScreenObj)
{
    if (FirstNote >= RhythmObjectList.size()) return;
    IOCRhythmObject* r = RhythmObjectList[FirstNote];
    //OCRhythmObject* s=&r;
    if (r->IsPause) return;
    if (!r->IsSingleNoteWithTie)
    {
        ScreenObj.setcol(QColor(unselectablecolor));
        PlotLengths(r,ScreenObj);
        ScreenObj.resetcol();
    }
    int NextNoteX = r->CenterX + 432;
    if (r != RhythmObjectList.last()) NextNoteX = RhythmObjectList[FirstNote + 1]->CenterX;
    int TieLen = loBound<int>(1,NextNoteX - r->CenterX - 192);
    r->MoveTo(ScreenObj);
    SignsToPrint.PrintSigns(TieLen, FrameList, r, ScreenObj);
    r->MoveTo(ScreenObj);
}

void OCNoteList::ApplyAccidentals(OCStaffAccidentals& StaffAccidentals)
{
    if (!RhythmObjectList.isEmpty())
    {
        if (!RhythmObjectList.last()->IsPause)
        {
            RhythmObjectList.last()->ApplyAccidentals(StaffAccidentals);
        }
    }
}

//--------------------------------------------------------------------------

void CNoteHead::plot(const bool UnderTriplet, const QColor& TrackColor, const int UpDown, OCFrameArray &FrameList, const OCGraphicsList& tie, OCDraw &ScreenObj)
{
    if (ScreenObj.canColor())
    {
        if (Marked)
        {
            ScreenObj.col = (ScreenObj.Cursor->SelCount()==0) ? markedcolor : selectedcolor;
        }
    }
    OCGraphicsList a;
    int AccSign = AccidentalSymbol;
    if (AccidentalType > 0) {
        AccSign = AccidentalType - 1;
    }
    if (AccSign != accNone) {
        if (AccidentalParentheses) {
            moveTo(ScreenObj);
            ScreenObj.move(sized(-9 * 12) + FortegnAddX,0);
            ScreenObj.move((-9 * 12), 0, Size);
            if (AccSign == accDoubleFlat) ScreenObj.move(sized(-3 * 12),0);
            if (AccSign == accFlat) ScreenObj.move(sized(12),0);
            a.append(ScreenObj.plLet("(",Size,"Arial",false,false,156,Qt::AlignCenter));
            moveTo(ScreenObj);
            ScreenObj.move(sized(-9 * 12) + FortegnAddX, 0);
            ScreenObj.move(12, 0, Size);
            a.append(ScreenObj.plLet(")",Size,"Arial",false,false,156,Qt::AlignCenter));
            moveTo(ScreenObj);
            ScreenObj.move(sized(-12 * 12) + FortegnAddX,0);
            if (AccSign == accFlat) ScreenObj.move(sized(12),0);
        }
        else {
            moveTo(ScreenObj);
            ScreenObj.move(sized(-9 * 12) + FortegnAddX,0);
        }
    }
    switch (AccSign)
    {
    case accFlat:
        //ScreenObj.move(sized((-11 * 12) + FortegnAddX), 150);
        ScreenObj.move(0,sized(48));
#ifndef __Lelandfont
        //a.append(ScreenObj.plLet(OCTTFFlat, Size));
        a.append(ScreenObj.plChar(OCTTFFlat, Size));
#else
        ScreenObj.move(-12,-118);
        a.append(ScreenObj.plLet(LelandFlat, Size));
#endif
        break;
    case accSharp:
        //ScreenObj.move(sized((-13 * 12) + FortegnAddX), 150);
#ifndef __Lelandfont
        //a.append(ScreenObj.plLet(OCTTFSharp, Size));
        a.append(ScreenObj.plChar(OCTTFSharp, Size));
#else
        ScreenObj.move(-12,-118);
        a.append(ScreenObj.plLet(LelandSharp, Size));
#endif
        break;
    case accDoubleFlat:
        ScreenObj.move(0,sized(48));
        //ScreenObj.move(sized((-17 * 12) + FortegnAddX), 150);
        a.append(ScreenObj.plChar(OCTTFDoubleFlat, Size));
        //a.append(ScreenObj.plLet(OCTTFDoubleFlat, Size));
        break;
    case accDoubleSharp:
        //ScreenObj.move(sized((-14 * 12) + FortegnAddX), 150);
        //a.append(ScreenObj.plLet(OCTTFDoubleSharp, Size));
        a.append(ScreenObj.plChar(OCTTFDoubleSharp, Size));
        break;
    case accNatural:
        //ScreenObj.move(sized(FortegnAddX - 12 * 12), 150);
        //a.append(ScreenObj.plLet(OCTTFOpl, Size));
        a.append(ScreenObj.plChar(OCTTFOpl, Size));
        break;
    case accNone:
        break;
    }
    OCGraphicsList l;
    moveTo(ScreenObj);
    if ((CenterY % 96) != 0) ScreenObj.move(0,48);
    l.append(OCNoteList::PlotDot(NoteVal, UnderTriplet, 4, ScreenObj));
    moveTo(ScreenObj);
    ScreenObj.move((53 + sized(-53)) * -UpDown, 0);
    if (NoteHeadType == 0) {
        //ScreenObj.move(0,77);
    #ifdef __Lelandfont
        ScreenObj.move(-60,-41);
    #endif
        ScreenObj.move(58,0,Size);
        switch (NoteVal)
        {
        case 168:
        case 144:
        case 96:
        case 64:
    #ifndef __Lelandfont
            l.append(ScreenObj.plChar(OCTTFNoteWhole, Size, 624));
    #else
            l.append(ScreenObj.plLet(LelandNoteWhole, ClusterSize));
    #endif
            break;
        case 48:
        case 32:
        case 72:
        case 84:
    #ifndef __Lelandfont
            l.append(ScreenObj.plChar(OCTTFNoteHalf, Size, 624));
    #else
            l.append(ScreenObj.plLet(LelandNoteHalf, ClusterSize));
    #endif
            break;
        default:
    #ifndef __Lelandfont
            l.append(ScreenObj.plChar(OCTTFNoteQuarter, Size, 624));
    #else
            l.append(ScreenObj.plLet(LelandNoteQuarter, ClusterSize));
    #endif
        }
    }
    else if (NoteHeadType == 1) {
        ScreenObj.move(-28,0,Size);
        l.append(ScreenObj.plChar(WDX,Size,138,WingDingsName,true));
    }
    else if (NoteHeadType == 2) {
        ScreenObj.move(-14,-2,Size);
        l.append(ScreenObj.plChar(WDDiamond,Size,108,WingDingsName,true));
        ScreenObj.move(4,4,Size);
        l.append(ScreenObj.plChar(WDDiamond,Size,108,WingDingsName,true));
    }
    FrameList.AppendAccidentalGroup(ScreenObj.MakeGroup(l),ScreenObj.MakeGroup(a),ScreenObj.MakeGroup(tie),Location);
    ScreenObj.col = TrackColor;
}

void OCBeamList::append(IOCRhythmObject *r)
{
    if (isEmpty())
    {
        MinNumOfBeams=r->NumOfBeams;
    }
    else
    {
        if (r->NumOfBeams < MinNumOfBeams) MinNumOfBeams = r->NumOfBeams;
    }
    RhythmObjects.append(r);
}

void OCBeamList::CalcBalk()
{
    if (RhythmObjects.first()->NumOfForcedBeams > MinNumOfBeams) MinNumOfBeams = RhythmObjects.first()->NumOfForcedBeams;
    if ((RhythmObjects.first()->ForceBeamIndex == 1) && (MinNumOfBeams == 0)) MinNumOfBeams = 1;
    StemDirection UpDown = (RhythmObjects.stemUp()) ? StemUp : StemDown;
    for (const IOCRhythmObject* r : std::as_const(RhythmObjects)) {
        if (r->ForceUpDown != 0) {
            UpDown = r->ForceUpDown;
            break;
        }
    }
    for (IOCRhythmObject* r : std::as_const(RhythmObjects)) r->SetUpDown(UpDown);
    QPointF First(RhythmObjects.first()->BalkEnd());
    QPointF Last(RhythmObjects.last()->BalkEnd());
    QVector2D dir = (RhythmObjects.first()->ForceSlant == 0) ? QVector2D(Last - First) : QVector2D(1,0);
    dir.normalize();
    SlantFactor = -dir.y();
    double addit = 0;
    for (IOCRhythmObject* r : std::as_const(RhythmObjects)) {
        const double balkheight = First.y() + (dir.y() * (r->BalkX - First.x()));
        const double add1 = r->BalkEndY - balkheight;
        if (add1 * UpDown < 0) {
            if (add1 * UpDown < addit * UpDown) {
                addit = add1;
            }
        }
    }
    First.setY(First.y() + addit);
    for (IOCRhythmObject* r : std::as_const(RhythmObjects)) r->BalkEndY = First.y() + ((r->BalkX - First.x()) * dir.y());
}

void OCBeamList::plBalk(const double Length, const double factor, const StemDirection UpDown, OCDraw &ScreenObj)
{
    QPainterPath p(QPointF(0,0));
    p.lineTo(QPointF(Length, factor * Length) / ScreenObj.ScreenSize);
    p.lineTo(QPointF(Length, (factor * Length) + (-UpDown * BeamThickness)) / ScreenObj.ScreenSize);
    p.lineTo(QPointF(0, -UpDown*BeamThickness) / ScreenObj.ScreenSize);
    p.lineTo(0,0);
    ScreenObj.translatePath(p);
    ScreenObj.plTextPath(p,true);
    ScreenObj.move(0, (BeamSpace + BeamThickness) * UpDown);
}

void OCBeamList::plBeam(const QPointF& p1, const QPointF& p2, const StemDirection UpDown, const int NumOfBeams, OCDraw &ScreenObj)
{
    QPainterPath b(QPointF(0,0));
    for (int i = 0; i <= NumOfBeams - 1; i++)
    {
        QPainterPath p(QPointF(0,0));
        p.lineTo(QPointF(p2.x() - p1.x(),p1.y() - p2.y()) / ScreenObj.ScreenSize);
        p.lineTo(QPointF(p2.x() - p1.x(),(p1.y() - p2.y()) + (-UpDown * BeamThickness)) / ScreenObj.ScreenSize);
        p.lineTo(QPointF(0, -UpDown * BeamThickness) / ScreenObj.ScreenSize);
        p.lineTo(0,0);
        b.addPath(p.translated(QPointF(0,(BeamThickness + BeamSpace) * i * -UpDown) / ScreenObj.ScreenSize));
    }
    ScreenObj.moveTo(p1);
    ScreenObj.translatePath(b);
    ScreenObj.plTextPath(b,true);
}

void OCBeamList::plleftsubbalk(const int num, const int commonbalk, const int Length, const int prevnum, const StemDirection UpDown, const double factor, int cbld, OCDraw &ScreenObj)
{
    const int len = qMin(96,Length);
    for (int i = 1; i <= num; i++)
    {
        if (i <= commonbalk)
        {
            ScreenObj.move(0, UpDown * (BeamThickness + BeamSpace));
            if (i == prevnum) cbld = -len; // ': Slant = -6
        }
        else
        {
            if (cbld != 0)
            {
                plBalk(cbld, factor, UpDown, ScreenObj);
            }
            else
            {
                ScreenObj.move(0, UpDown * (BeamThickness + BeamSpace));
            }
            if (i == prevnum) cbld = -len;
        }
    }
}

void OCBeamList::plrightsubbalk(const int num, const int commonbalk, const int Length, const int nextnum, const StemDirection UpDown, const double factor, int cbld, OCDraw &ScreenObj)
{
    const int len = qMin(96,Length);
    for (int i = 1; i <= num; i++)
    {
        if (i <= commonbalk)
        {
            ScreenObj.move(0, UpDown * (BeamThickness + BeamSpace));
            if (i == nextnum) cbld = len;
        }
        else
        {
            plBalk(cbld, factor, UpDown, ScreenObj);
            if (i == nextnum) cbld = len;
        }
    }
}

void OCBeamList::plotSubBeams(OCDraw &ScreenObj)
{
    for (int j = 0;j<RhythmObjects.size();j++)
    {
        const IOCRhythmObject* r=RhythmObjects[j];
        int LenLeft = 0;
        int LenRight = 0;
        int NumOfBeamsLeft = 0;
        int NumOfBeamsRight = 0;
        if (r != RhythmObjects.first())
        {
            NumOfBeamsLeft = RhythmObjects[j - 1]->NumOfBeams;
            LenLeft = (r->BalkX - RhythmObjects[j - 1]->BalkX);
        }
        if (r != RhythmObjects.last())
        {
            NumOfBeamsRight = RhythmObjects[j + 1]->NumOfBeams;
            LenRight = (RhythmObjects[j + 1]->BalkX - r->BalkX);
        }
        if ((LenRight == 0) && (r != RhythmObjects.first())) LenRight = LenLeft; // '(LenLeft * objarr[iTemp1]->Rounded) \ objarr[iTemp1 - 1]->Rounded
        int NumOfSubBeamsRight = r->NumOfBeams;
        int NumOfSubBeamsLeft = NumOfBeamsRight;
        if (NumOfBeamsLeft >= r->NumOfBeams)
        {
            if (r->NumOfBeams <= NumOfBeamsRight)
            {
                NumOfSubBeamsRight = r->NumOfBeams;
                NumOfSubBeamsLeft = r->NumOfBeams;
            }
        }
        else
        {
            if (NumOfBeamsLeft < NumOfBeamsRight)
            {
                NumOfSubBeamsRight = NumOfBeamsLeft;
                NumOfSubBeamsLeft = r->NumOfBeams;
            }
        }
        if (NumOfSubBeamsRight)
        {
            ScreenObj.moveTo(r->BalkEnd()); // '- (84 * MinNumOfBeams * s->UpDown)
            plleftsubbalk(NumOfSubBeamsRight, MinNumOfBeams, LenRight / 2 , NumOfBeamsLeft, RhythmObjects.first()->UpDown, SlantFactor, 0, ScreenObj);
        }
        if (NumOfSubBeamsLeft)
        {
            ScreenObj.moveTo(r->BalkEnd()); // '- (84 * MinNumOfBeams * s->UpDown)
            plrightsubbalk(NumOfSubBeamsLeft, MinNumOfBeams, LenRight / 2 , NumOfBeamsRight, RhythmObjects.first()->UpDown, SlantFactor, LenRight, ScreenObj);
        }
    }
}

void OCBeamList::plotMainBeam(OCDraw &ScreenObj)
{
    plBeam(RhythmObjects.first()->directed(RhythmObjects.first()->BalkEnd(),LineHalfThickNess), RhythmObjects.last()->directed(RhythmObjects.last()->BalkEnd(),LineHalfThickNess), RhythmObjects.first()->UpDown, MinNumOfBeams, ScreenObj);
}

IOCRhythmObject::~IOCRhythmObject(){}
