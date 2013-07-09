#include "ocscore.h"

void CTrack::plMTr(OCBarList& BarList, QDomLiteElement* XMLVoice, QDomLiteElement* XMLTemplate, const QColor TC, OCSymbolArray& MTObj, OCDraw& ScreenObj, XMLScoreWrapper& XMLScore)
{
    OCCounter CountIt;
    OCPrintVarsType dcurrent = fibset;
    float BarX = BarList.BarX()*ScreenObj.XFactor;
    float XFysic = BarX;
    int py = fibset.FilePointer;
    forever
    {
        ScreenObj.col = TC;
        XFysic = BarX;
        CountIt.reset();
        forever
        {
            XMLSymbolWrapper Symbol(XMLVoice, py, BarList.GetMeter(CountIt.BarCounter));
            ScreenObj.setcol(py);
            if (Symbol.IsEndOfVoice()) break;
            else if (Symbol.IsRestOrValuedNote())
            {
                int Ticks=Symbol.ticks();
                CountIt.Flip(Ticks);
                XFysic = (BarList.CalcX(CountIt.BarCounter, CountIt.Counter)*ScreenObj.XFactor) + BarX;
                if (XFysic > (BarList.SystemLength()*ScreenObj.XFactor)) break;
                CountIt.Flip1(Ticks);
            }
            else if (Symbol.IsCompoundNote())
            {
            }
            else if (Symbol.Compare("Tuplet"))
            {
                CountIt.Tuplets(py, XMLVoice);
            }
            else
            {
                OCSymbolsCollection::PlotMTrack(XFysic, Symbol, XMLScoreWrapper::TemplateStaff(XMLTemplate, StaveNum)->attributeValue("Height"), dcurrent, TrackNum, MTObj, py, XMLScore, ScreenObj);
            }
            py++;
            ScreenObj.col = TC;
            if (CountIt.NewBar(BarList.GetMeter(CountIt.BarCounter) * BarList.GetMultiPause(CountIt.BarCounter))) break;
        }
        if (CountIt.BarCounter >= BarList.BarsToPrint()-1) break;
        CountIt.BarFlip();
        BarX = XFysic + ((begofbar+endofbar)*ScreenObj.XFactor);
        if (XFysic > (BarList.SystemLength()*ScreenObj.XFactor)) break;
        BarX += BarList.BegSpace(CountIt.BarCounter, true, true, true)*ScreenObj.XFactor;
    }
}

void CTrack::PlBrLine(const int xs, const int Bracket, const int Tuborg, const int stavedistance, OCDraw& ScreenObj)
{
    ScreenObj.DM(xs, ScoreStaffHeight);
    if ((Bracket == 1) || (Tuborg > 0))
    {
        ScreenObj.DL(0, -(ScoreStaffHeight+(12*stavedistance))*ScreenObj.XFactor);
    }
    else
    {
        ScreenObj.DL(0, -ScoreStaffLinesHeight);
    }
}

void CTrack::getsetting(OCPrintVarsType &tempsetting)
{
    tempsetting = fibset;
}

void CTrack::putsetting(OCPrintVarsType &tempsetting)
{
    fibset = tempsetting;
}

void CTrack::PlfirstClef(OCBarList& BarList, OCDraw& ScreenObj)
{
    //if (ChangeClef) return;
    if (BarList.BarMap.ClefChange(BarList.StartBar(), StaveNum, TrackNum)) return;
    ScreenObj.DM(24*ScreenObj.XFactor, 888);
    CClef::PlClef(fibset.CurrentClef.val,0,ScreenObj);
}

void CTrack::PlfirstAcc(OCBarList& BarList, OCDraw& ScreenObj)
{
    //if (changefortegn) return;
    if (BarList.BarMap.KeyChange(BarList.StartBar(), StaveNum, TrackNum)) return;
    float BarX = begofbar + BarList.BegSpace(0, false, true, false);
    CKey::plotKey(fibset.CurrentKey.val,QPointF((BarX-216)*ScreenObj.XFactor,0),fibset.CurrentClef.val,ScreenObj);
}

void CTrack::FormatBar(const int CurrentBar, const int ActualBar, OCBarList& BarList, XMLScoreWrapper& XMLScore, QDomLiteElement *XMLTemplate)
{
    int Pnt=BarList.BarMap.GetPointer(ActualBar+BarList.StartBar(),StaveNum,TrackNum);//FiFcPnt;
    OCPrintVarsType dummy(fibset);
    QDomLiteElement* XMLVoice = XMLScore.Voice(StaveNum, TrackNum);
    /*
    if (CNoteCompare(XMLVoice->childElement(Pnt)).IsEndOfVoice())
    {
        if (CurrentBar == 0)
        {
            if (BarList.GetKeyInBegOfBar(0) < CKey::NumOfAccidentals(dummy.CurrentKey.val))
            {
                //if (!BarList.BarMap.KeyChange(BarList.StartBar(), StaveNum, TrackNum)) BarList.SetKeyInBegOfBar(0, BarList.GetKeyInBegOfBar(0) + CKey::NumOfAccidentals(dummy.CurrentKey.val));
            }
        }
        //return;
    }
    */
    OCCounter CountIt;
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, Pnt, BarList.GetMeter(CurrentBar));
        if (Symbol.IsRestOrValuedNote())
        {
            int Ticks=Symbol.ticks();
            CountIt.Flip(Ticks);
            BarList.SetMinimum(CurrentBar, CountIt.Rounded, CountIt.Counter - CountIt.Rounded);
            dummy.Decrement(Ticks);
            CountIt.Flip1(Ticks);
        }
        else if (Symbol.IsEndOfVoice())
        {
            break;
        }
        else
        {
            OCSymbolsCollection::DrawFactor(Symbol, &CountIt, XMLTemplate, BarList, StaveNum, TrackNum, CurrentBar, Pnt, XMLScore);
        }
        Pnt++;
        if (CountIt.NewBar(BarList.GetMeter(CurrentBar)))
        {
            break;
        }
    }
    if (CurrentBar == 0)
    {
        if (BarList.GetKeyInBegOfBar(0) < CKey::NumOfAccidentals(dummy.CurrentKey.val))
        {
            if (!BarList.BarMap.KeyChange(BarList.StartBar(), StaveNum, TrackNum)) BarList.SetKeyInBegOfBar(0, BarList.GetKeyInBegOfBar(0) + CKey::NumOfAccidentals(dummy.CurrentKey.val));
        }
    }
}

CTrack::CTrack()
{
    TrackNum=0;
    StaveNum=0;
}

void CTrack::PlTrack(OCBarList &BarList, XMLScoreWrapper &XMLScore, OCSymbolArray &SymbolList, OCNoteList &NoteList, OCDraw& ScreenObj, OCPrintStaffVarsType &sCurrent, const QColor TrackColor)
{
    float XFysic=0;
    float BarX=0;
    int NoteCount=0;
    QColor SignCol(activestaffcolor);
    QDomLiteElement* XMLVoice = XMLScore.Voice(StaveNum, TrackNum);
    int TriolStart=0;
    ScreenObj.col = TrackColor;
    OCCounter CountIt;
    OCSignList SignsToPrint;
    OCPrintVarsType dcurrent = fibset;
    dcurrent.FilePointer = fibset.FilePointer;
    int StartPointer = dcurrent.FilePointer;
    if (dcurrent.FilePointer == 0) dcurrent.Meter = BarList.BarMap.GetMeter(0,StaveNum,TrackNum);//BarList.GetMeter(-1);
    BarX = BarList.BarX()*ScreenObj.XFactor;
    forever
    {
        CountIt.reset();
        XFysic = BarX;
        int TriolVal = 0;
        ScreenObj.setcol(QColor(unselectablecolor));
        if (CountIt.BarCounter == 0)
        {
            if (dcurrent.SlurUp.RemainingTicks) NoteList.PlotSlur(0, dcurrent.SlurUp.RemainingTicks, 1, dcurrent.SlurUp.val, dcurrent.SlurUp.Pos, dcurrent.SlurUp.Size, true, ScreenObj);
            if (dcurrent.SlurDown.RemainingTicks) NoteList.PlotSlur(0, dcurrent.SlurDown.RemainingTicks, -1, dcurrent.SlurDown.val, dcurrent.SlurDown.Pos, dcurrent.SlurDown.Size, true, ScreenObj);
            if (dcurrent.crescendo.RemainingTicks) NoteList.PlotHairPin(0, dcurrent.crescendo.RemainingTicks, 0, dcurrent.crescendo.Pos, dcurrent.crescendo.Size, true, ScreenObj);
            if (dcurrent.diminuendo.RemainingTicks) NoteList.PlotHairPin(0, dcurrent.diminuendo.RemainingTicks, 1, dcurrent.diminuendo.Pos, dcurrent.diminuendo.Size, true, ScreenObj);
        }
        ScreenObj.col = TrackColor;
        forever
        {
            XMLSymbolWrapper Symbol(XMLVoice, dcurrent.FilePointer, dcurrent.Meter);
            ScreenObj.setcol(dcurrent.FilePointer);
            SignCol = ScreenObj.col;
            if (Symbol.IsRestOrValuedNote())
            {
                int Ticks=Symbol.ticks();
                ScreenObj.col = TrackColor;
                CountIt.Flip(Ticks);
                NoteList.plot(NoteCount, dcurrent.Meter, dcurrent.FilePointer, BarList, dcurrent.TieWrap, TriolVal, TrackColor, SymbolList, ScreenObj);
                if (Symbol.getVal("Triplet"))
                {
                    if (TriolVal == 0) TriolStart = NoteCount;
                    TriolVal += Ticks;
                }
                else
                {
                    if (TriolVal > 0) TriolVal += Ticks;
                }
                switch (TriolVal)
                {
                case 3:
                case 6:
                case 12:
                case 24:
                case 48:
                case 96:
                    ScreenObj.setcol(QColor(unselectablecolor));
                    NoteList.PlotTuplet(TriolStart, TriolVal - Ticks, 3, QPointF(0,0), 0, ScreenObj);
                    ScreenObj.col=TrackColor;
                    TriolVal = 0;
                }
                NoteList.plotsigns(NoteCount, SignsToPrint, SymbolList, ScreenObj);
                NoteCount ++;
                CountIt.Flip1(Ticks);
            }
            else if (Symbol.IsCompoundNote())
            {}
            else if (Symbol.IsEndOfVoice())
            {
                ScreenObj.DM(XFysic, ScoreStaffHeight);
                ScreenObj.DJ(0, -96 * 4);
                ScreenObj.DM(XFysic + ScreenObj.ScreenSize * 2, ScoreStaffHeight);
                ScreenObj.DJ(0, -96 * 4);
                return;
            }
            else
            {
                OCSymbolsCollection::plot(Symbol, XFysic, BarList, CountIt, SignsToPrint, SignCol, XMLScore, StartPointer, SymbolList, StaveNum, TrackNum, NoteList, NoteCount, dcurrent, sCurrent, dcurrent.FilePointer, ScreenObj);
            }
            ScreenObj.col = TrackColor;
            SignCol = TrackColor;
            dcurrent.FilePointer++;
            XFysic = (BarList.CalcX(CountIt.BarCounter, CountIt.Counter)*ScreenObj.XFactor) + BarX;
            if (CountIt.NewBar(dcurrent.Meter)) break;
        }
        //'If BarList.GetMultiPause(CountIt.BarCounter) > 0 Then dcurrent.FilePointer = dcurrent.FilePointer + BarList.GetMultiPauseNumOfEvents(CountIt.BarCounter) - 1
        if (CountIt.BarCounter >= BarList.BarsToPrint()-1) break;
        CountIt.BarFlip();
        BarX = XFysic + ((begofbar+endofbar)*ScreenObj.XFactor);
        if (XFysic > BarList.SystemLength()*ScreenObj.XFactor) break;
        PlBrLine(BarX - ((begofbar + 48)*ScreenObj.XFactor), sCurrent.Square, sCurrent.Curly, sCurrent.Height, ScreenObj);
        BarX += (BarList.BegSpace(CountIt.BarCounter, true, true, true)*ScreenObj.XFactor);
    }
    PlBrLine(BarList.SystemLength()*ScreenObj.XFactor, sCurrent.Square, sCurrent.Curly, sCurrent.Height, ScreenObj);
    int py = dcurrent.FilePointer;
    ScreenObj.setcol(QColor(unselectablecolor));
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, py, dcurrent.Meter);
        if (Symbol.Compare("Time"))
        {
            OCSymbolsCollection::fib(Symbol,TrackNum,dcurrent);
            CTime::PlTime(Symbol, BarList.EndSpace(false,false,true)*ScreenObj.XFactor,ScreenObj,Qt::AlignLeft); //BarX-108
        }
        else if (Symbol.Compare("Clef"))
        {
            OCSymbolsCollection::fib(Symbol,TrackNum,dcurrent);
            ScreenObj.DM(BarList.EndSpace(true,true,true)*ScreenObj.XFactor, 888, Symbol);
            CClef::PlClef(Symbol.getVal("Clef") + 1, 0, ScreenObj);
        }
        else if (Symbol.Compare("Key"))
        {
            OCSymbolsCollection::fib(Symbol,TrackNum,dcurrent);
            CKey::plotKey(dcurrent.CurrentKey.val, Symbol.move(BarList.EndSpace(true, false, true)*ScreenObj.XFactor,0),dcurrent.CurrentClef.val,ScreenObj);
        }
        else if (Symbol.Compare("Repeat"))
        {
            if (Symbol.getVal("RepeatType") == 0)
            {
                OCSymbolsCollection::fib(Symbol,TrackNum,dcurrent);
                OCSymbolsCollection::plot(Symbol, (BarList.SystemLength() + (15 * 12))*ScreenObj.XFactor, BarList, CountIt, SignsToPrint, unselectablecolor, XMLScore, StartPointer, SymbolList, StaveNum, TrackNum, NoteList, NoteCount, dcurrent, sCurrent, 0, ScreenObj);
            }
        }
        else if (Symbol.IsEndOfVoice())
        {
            PlBrLine((BarList.SystemLength()-(LineHalfThickNess*6))*ScreenObj.XFactor, sCurrent.Square, sCurrent.Curly, sCurrent.Height, ScreenObj);
            break;
        }
        else if (Symbol.IsRestOrValuedNote())
        {
            break;
        }
        py++;
    }
    ScreenObj.col = TrackColor;
}

CTrack::~CTrack()
{
}

const int CTrack::FillChunk(int& py, OCCounter& CountIt, OCNoteList& NoteList, OCPrintVarsArray dcurrent, const int NumOfTracks, QDomLiteElement* XMLVoice, OCStaffAccidentals& lfortegn, OCBarList& BarList, const int XFysic, QList<int> &LastTiedNotes, OCDraw& ScreenObj)
{
    CountIt.killed=false;
    int currentlen = 0;
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, py, dcurrent[TrackNum].Meter);
        if (Symbol.IsRestOrValuedNote())
        {
            int Ticks=Symbol.ticks();
            CountIt.Flip(Ticks);
            NoteList.Append(XFysic, Symbol, py, XMLVoice, TrackNum, dcurrent[TrackNum], CountIt, lfortegn, BarList, LastTiedNotes, ScreenObj);
            CountIt.Flip1(Ticks);
            currentlen = CountIt.Rounded;
            py++;
            break;
        }
        else if (Symbol.IsEndOfVoice())
        {
            CountIt.killed = true;
            break;
        }
        else
        {
            FillBetweenNotes(py, XMLVoice, NumOfTracks, CountIt, dcurrent, lfortegn);
        }
    }
    return currentlen;
}

void CTrack::FillBetweenNotes(int &py, QDomLiteElement* XMLVoice, const int NumOfTracks, OCCounter& CountIt, OCPrintVarsArray dcurrent, OCStaffAccidentals& lfortegn)
{
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, py, dcurrent[TrackNum].Meter);
        if (Symbol.IsRestOrValuedNote())
        {
            return;
        }
        else if (Symbol.IsCompoundNote())
        {}
        else if (Symbol.IsEndOfVoice())
        {
            return;
        }
        else if (Symbol.Compare("Tuplet"))
        {
            CountIt.Tuplets(py, XMLVoice);
        }
        else
        {
            OCSymbolsCollection::fib(Symbol,TrackNum,dcurrent[TrackNum]);
            if (TrackNum == 0)
            {
                if (OCSymbolsCollection::IsCommon(Symbol))
                {
                    for (int iTemp = 1; iTemp < NumOfTracks; iTemp++)
                    {
                        OCSymbolsCollection::fib(Symbol,iTemp,dcurrent[iTemp]);
                        if (Symbol.Compare("Key")) lfortegn.RdAcc(dcurrent[iTemp].CurrentKey.val);
                    }
                }
            }
        }
        py++;
    }
}

const int CTrack::FiBChunk(int& Pnt, QDomLiteElement* XMLVoice, OCCounter& CountIt, OCPrintVarsArray fibset, const int NumOfTracks, QList<int> &LastTiedNotes)
{
    CountIt.killed=false;
    int currentlen = 0;
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, Pnt, fibset[TrackNum].Meter);
        if (Symbol.IsRestOrValuedNote())
        {
            if (Symbol.IsTiedNote())
            {
                LastTiedNotes.append(Symbol.getVal("Pitch"));
            }
            fibset[TrackNum].TieWrap.EraseTie();
            int Ticks=Symbol.ticks();
            CountIt.Flip(Ticks);
            //TupletFactor = CountIt.factor;
            CountIt.Flip1(Ticks);
            currentlen = CountIt.Rounded;
            Pnt++;
            if (Symbol.IsTiedNote())
            {
                fibset[TrackNum].TieWrap.Add(Symbol.getVal("Pitch"));
            }
            fibset[TrackNum].TieWrap.EraseTies = true;
            break;
        }
        if (Symbol.IsEndOfVoice())
        {
            CountIt.killed = true;
            break;
        }
        else
        {
            FiBBetweenNotes(Pnt, XMLVoice, NumOfTracks, CountIt, fibset, LastTiedNotes);
        }
    }
    return currentlen;
}

void CTrack::FiBBetweenNotes(int &py, QDomLiteElement* XMLVoice, const int NumOfTracks, OCCounter& CountIt, OCPrintVarsArray fibset, QList<int> &LastTiedNotes)
{
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, py, fibset[TrackNum].Meter);
        if (Symbol.IsRestOrValuedNote())
        {
            return;
        }
        if (Symbol.IsCompoundNote())
        {
            if (Symbol.IsTiedNote())
            {
                LastTiedNotes.append(Symbol.getVal("Pitch"));
            }
            fibset[TrackNum].TieWrap.EraseTie();
            if (Symbol.IsTiedNote())
            {
                fibset[TrackNum].TieWrap.Add(Symbol.getVal("Pitch"));
            }
        }
        if (Symbol.IsEndOfVoice())
        {
            return;
        }
        if (Symbol.Compare("Tuplet"))
        {
            CountIt.Tuplets(py, XMLVoice);
        }
        else
        {
            OCSymbolsCollection::fib(Symbol,TrackNum,fibset[TrackNum]);
            if (TrackNum == 0)
            {
                if (OCSymbolsCollection::IsCommon(Symbol))
                {
                    for (int iTemp = 1; iTemp < NumOfTracks; iTemp++)
                    {
                        OCSymbolsCollection::fib(Symbol,iTemp,fibset[iTemp]);
                    }
                }
            }
        }
        py++;
    }
}

const QList<SymbolSearchLocation> CTrack::SearchBetweenNotes(int &py, QDomLiteElement* XMLVoice, const int NumOfTracks, OCCounter& CountIt, OCPrintVarsArray fibset, QString SearchTerm)
{
    QList<SymbolSearchLocation> l;
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, py, fibset[TrackNum].Meter);
        if (Symbol.IsRestOrValuedNote())
        {
            return l;
        }
        if (Symbol.IsEndOfVoice())
        {
            return l;
        }
        if (Symbol.Compare(SearchTerm))
        {
            SymbolSearchLocation s;
            s.Bar=CountIt.BarCounter;
            s.Staff=StaveNum;
            s.Voice=TrackNum;
            s.Pointer=py;
            l.append(s);
        }
        if (Symbol.IsCompoundNote())
        {
        }
        if (Symbol.Compare("Tuplet"))
        {
            CountIt.Tuplets(py, XMLVoice);
        }
        else
        {
            OCSymbolsCollection::fib(Symbol,TrackNum,fibset[TrackNum]);
            if (TrackNum == 0)
            {
                if (OCSymbolsCollection::IsCommon(Symbol))
                {
                    for (int iTemp = 1; iTemp < NumOfTracks; iTemp++)
                    {
                        OCSymbolsCollection::fib(Symbol,iTemp,fibset[iTemp]);
                    }
                }
            }
        }
        py++;
    }
    return l;
}

void CTrack::FakePlotBetweenNotes(int &py, QDomLiteElement* XMLVoice, const int NumOfTracks, OCCounter& CountIt, OCPrintVarsArray fibset)
{
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, py, fibset[TrackNum].Meter);
        if (Symbol.IsRestOrValuedNote())
        {
            return;
        }
        if (Symbol.IsCompoundNote())
        {
        }
        if (Symbol.IsEndOfVoice())
        {
            return;
        }
        if (Symbol.Compare("Tuplet"))
        {
            CountIt.Tuplets(py, XMLVoice);
        }
        else
        {
            OCSymbolsCollection::fib(Symbol,TrackNum,fibset[TrackNum]);
            if (TrackNum == 0)
            {
                if (OCSymbolsCollection::IsCommon(Symbol))
                {
                    for (int iTemp = 1; iTemp < NumOfTracks; iTemp++)
                    {
                        OCSymbolsCollection::fib(Symbol,iTemp,fibset[iTemp]);
                    }
                }
            }
        }
        py++;
    }
}

const int CTrack::FiBPlayChunk(int &Pnt, QDomLiteElement* XMLStaff, OCCounter& CountIt, OCPlayVarsArray pcurrent, const int NumOfTracks, OCSignList& SignsToPlay, int& AccelCounter, OCMIDIFile& MFile, const int TrackOffset)
{
    QDomLiteElement* XMLVoice = XMLScoreWrapper::Voice(XMLStaff, TrackNum);
    CountIt.killed=false;
    int currentlen = 0;
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, Pnt, pcurrent[TrackNum].PlayMeter);
        if (Symbol.IsRestOrValuedNote())
        {
            int Ticks=Symbol.ticks();
            if (Symbol.getVal("Inaudible"))
            {
                CountIt.PlayFlip(Ticks);
                CountIt.Flip1(Ticks);
            }
            else
            {
                CountIt.PlayFlip(Ticks);
                SignsToPlay.PlayAfterNote(Symbol, pcurrent[TrackNum]);
                SignsToPlay.Decrement(Ticks);
                if (pcurrent[TrackNum].Accel != 0)
                {
                    AccelCounter += Ticks;
                }
                else
                {
                    AccelCounter = 0;
                }
                CountIt.Flip1(Ticks);
            }
            currentlen = CountIt.Rounded;
            Pnt++;
            break;
        }
        else if (Symbol.IsEndOfVoice())
        {
            CountIt.killed = true;
            break;
        }
        else
        {
            FiBPlayBetweenNotes(Pnt, XMLStaff, NumOfTracks, CountIt, pcurrent, MFile, SignsToPlay, TrackOffset);
        }
    }
    return currentlen;
}

void CTrack::FiBPlayBetweenNotes(int &py, QDomLiteElement* XMLStaff, const int NumOfTracks, OCCounter& CountIt, OCPlayVarsArray pcurrent, OCMIDIFile& MFile, OCSignList& SignsToPlay, const int TrackOffset)
{
    QDomLiteElement* XMLVoice = XMLScoreWrapper::Voice(XMLStaff, TrackNum);
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, py, pcurrent[TrackNum].PlayMeter);
        if (Symbol.IsRestOrValuedNote())
        {
            return;
        }
        else if (Symbol.IsCompoundNote())
        {}
        else if (Symbol.IsEndOfVoice())
        {
            break;
        }
        else if (Symbol.Compare("Tuplet"))
        {
            if (!Symbol.getVal("Inaudible"))
            {
                CountIt.Tuplets(py, XMLVoice);
            }
        }
        else
        {
            if (!Symbol.getVal("Inaudible")) OCSymbolsCollection::fibPlay(Symbol,MFile,CountIt,py,XMLVoice,SignsToPlay,pcurrent[TrackNum]);
            if (TrackNum == 0)
            {
                if (NumOfTracks > 1)
                {
                    if (OCSymbolsCollection::IsCommon(Symbol))
                    {
                        for (int iTemp = 1; iTemp < NumOfTracks; iTemp++)
                        {
                            MFile.SetTrackNumber(TrackOffset + iTemp);
                            MFile.SetTime(pcurrent[iTemp].Currenttime);
                            QDomLiteElement* XMLVoice1=XMLScoreWrapper::Voice(XMLStaff, iTemp);
                            if (!Symbol.getVal("Inaudible")) OCSymbolsCollection::fibPlay(Symbol,MFile,CountIt,py,XMLVoice1,SignsToPlay,pcurrent[iTemp]);
                        }
                        MFile.SetTrackNumber(TrackOffset);
                    }
                }
            }
        }
        py++;
    }
}

void CTrack::PlayBetweenNotes(int &py, QDomLiteElement* XMLStaff, const int NumOfTracks, OCCounter& CountIt, OCPlayVarsArray pcurrent, OCMIDIFile& MFile, OCSignList& SignsToPlay, const int MIDITrackNumber)
{
    QDomLiteElement* XMLVoice = XMLScoreWrapper::Voice(XMLStaff, TrackNum);

    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, py, pcurrent[TrackNum].PlayMeter);
        if (Symbol.IsRestOrValuedNote())
        {
            return;
        }
        if (Symbol.IsCompoundNote())
        {
            return;
        }
        if (Symbol.IsEndOfVoice())
        {
            return;
        }
        if (Symbol.Compare("Tuplet"))
        {
            if (!Symbol.getVal("Inaudible"))
            {
                CountIt.Tuplets(py, XMLVoice);
            }
        }
        else
        {
            if (!Symbol.getVal("Inaudible"))
            {
                OCSymbolsCollection::Play(Symbol,MFile,CountIt,py,XMLVoice,SignsToPlay,pcurrent[TrackNum]);
            }
            if (TrackNum == 0)
            {
                if (NumOfTracks > 1)
                {
                    if (OCSymbolsCollection::IsCommon(Symbol))
                    {
                        for (int iTemp = 1; iTemp < NumOfTracks; iTemp++)
                        {
                            if (!Symbol.getVal("Inaudible"))
                            {
                                MFile.SetTrackNumber(MIDITrackNumber + iTemp);
                                MFile.SetTime(pcurrent[iTemp].Currenttime);
                                OCSymbolsCollection::Play(Symbol,MFile,CountIt,py,XMLScoreWrapper::Voice(XMLStaff, iTemp),SignsToPlay,pcurrent[iTemp]);
                            }
                        }
                        MFile.SetTrackNumber(MIDITrackNumber);
                        MFile.SetTime(pcurrent[0].Currenttime);
                    }
                }
            }
        }
        py++;
    }
}

const int CTrack::CalcNoteTime(XMLSymbolWrapper& Symbol, const int Rounded, OCPlayBackVarsType &pcurrent, OCSignList& SignsToPlay) const
{
    int RetVal;
    if (Symbol.IsNoteType(false,true))
    {
        RetVal = Rounded;
    }
    else if (SignsToPlay.Exist("Length"))
    {
        RetVal = (Rounded * SignsToPlay.Value("Length", "Legato").toInt()) / 100;
    }
    else
    {
        QVariant v=SignsToPlay.Value("Legato");
        if (v != false)
        {
            RetVal=(Rounded * v.toInt()) / 100;
        }
        else
        {
            RetVal = (Rounded * pcurrent.currentlen) / 100;
        }
    }
    if (SignsToPlay.Exist("Slur"))
    {
        RetVal = Rounded;
        if (SignsToPlay.Exist("Length"))
        {
            if (SignsToPlay.Value("PerformanceType").toInt()>2)
            {
                RetVal = (Rounded * SignsToPlay.Value("Length", "Legato").toInt()) / 100;
            }
        }
        //'End If
    }
//'    If SignsToPlay.exist("Fermata") Then
//'        CalcNoteTime = CalcNoteTime + SignsToPlay.Value("Fermata", "Duration")
//'    End If
    return RetVal;
}

const int CTrack::CalcVoicedTime(const int NoteTime, const int Rounded, OCPlayBackVarsType &pcurrent, OCSignList& SignsToPlay) const
{
    int RetVal = pcurrent.Currenttime + NoteTime;
    //'pcurrent.ArtX1 = 0
    pcurrent.Currenttime += Rounded; //'+ pcurrent.Fermata
    if (SignsToPlay.Exist("Fermata"))
    {
        pcurrent.Currenttime += SignsToPlay.Value("Fermata", "Duration").toInt();
    }
    //'pcurrent.Fermata = 0
    //'If pcurrent.BueCount% Then CalcVoicedTime = pcurrent.Currenttime&
    if (SignsToPlay.Exist("Slur"))
    {
        //'If SignsToPlay.Value("Slur", "Ticks") > 0 Then
        //RetVal = pcurrent.Currenttime;
        //'End If
    }
    return RetVal;
}

void CTrack::PlayNotesOff(CNotesToPlay& NotesToPlay, const int VoicedTime, OCMIDIFile& MFile, OCPlayBackVarsType &pcurrent, bool& EndTimeSet)
{
    for (int iTemp = 0; iTemp < NotesToPlay.Count(); iTemp++)
    {
        switch (NotesToPlay.State(iTemp))
        {
        case tSPlayEnd:
        case tSPlayStartEnd:
            if (!EndTimeSet)
            {
                MFile.SetTime(VoicedTime);
                pcurrent.Currenttime = pcurrent.Currenttime - VoicedTime;
            }
            int Data2 = 0;
            if (!NotesToPlay.Inaudible(iTemp)) Data2 = pcurrent.Currentdynam;
            MFile.Append(0x80 + pcurrent.MIDI.Channel, NotesToPlay.Pitch(iTemp), Data2);
            EndTimeSet = true;
            NotesToPlay.ChangeState(iTemp, 0);// 'PlayOn%(ThisVelocity) = 0
        }
    }
    NotesToPlay.CleanUp();
    if (EndTimeSet)
    {
        MFile.SetTime(pcurrent.Currenttime);
        EndTimeSet = false;
    }
}

void CTrack::PlayPortamentoNotes(const int ThisPitch, CNotesToPlay& NotesToPlay, OCPlayBackVarsType &pcurrent, OCMIDIFile& MFile)
{
    if (NotesToPlay.Count() == 1)
    {
        if (NotesToPlay.State(0) == tSPlayPortamento)
        {
            if (NotesToPlay.Pitch(0) != ThisPitch)
            {
                if (!pcurrent.PortamentoOn)
                {
                    int Data2=0;
                    if (!NotesToPlay.Inaudible(0)) Data2 = pcurrent.Currentdynam;
                    MFile.Append(0x80 + pcurrent.MIDI.Channel, NotesToPlay.Pitch(0), Data2);
                    MFile.SetTime(0);
                    pcurrent.Currenttime = 0;
                    NotesToPlay.ChangeState(0, 0);// 'PlayOn%(0) = 0
                }
                else
                {
                    MFile.PlayController(0x54, NotesToPlay.Pitch(0), pcurrent.MIDI.Channel);
                    MFile.SetTime(0);
                    pcurrent.Currenttime = 0;
                    NotesToPlay.ChangeState(0, 0);//'PlayOn%(ThisVelocity) = 0
                    NotesToPlay.PortIt = NotesToPlay.Pitch(0);
                }
            }
            else
            {
                NotesToPlay.ChangeState(0, tSPlayStart);
            }
        }
        NotesToPlay.CleanUp();
    }
}

void CTrack::PlayPortamentoCleanUp(CNotesToPlay& NotesToPlay, OCMIDIFile& MFile, OCPlayBackVarsType &pcurrent)
{
    if (NotesToPlay.PortIt)
    {
        int Data2=0;
        if (!NotesToPlay.Inaudible(0)) Data2 = pcurrent.Currentdynam;
        MFile.Append(0x80 + pcurrent.MIDI.Channel, NotesToPlay.PortIt, Data2);
        NotesToPlay.PortIt = 0;
    }
}
/*
'Private Sub PlayGlissCleanUp(pcurrent As OCPlayBackVarsType, MFile As OCMIDIFile, GlissWas As Boolean)
'    With MFile
'        If pcurrent.PortamentoOn = True And pcurrent.Gliss = 0 And GlissWas = True Then
'            MFile.PlayController &H5, &H10, pcurrent.Channel
'            MFile.Time = 0
'            pcurrent.Currenttime& = 0
'            GlissWas = False
'        End If
'    End With
'End Sub
*/
void CTrack::PlayExprClear(OCPlayBackVarsType &pcurrent, OCMIDIFile& MFile, OCSignList& SignsToPlay)
{
    if (pcurrent.changeexp) // 'Or (pcurrent.ExpressionOn And pcurrent.cresc > 0) Then
    {
        pcurrent.changeexp--;
        if (!pcurrent.changeexp)
        {
            MFile.PlayExpression(pcurrent.exprbegin, pcurrent.MIDI.Channel);
            MFile.SetTime(0);
        }
    }
    if (pcurrent.ExpressionOn)
    {
        if (SignsToPlay.Exist("Hairpin") || SignsToPlay.Exist("DynamicChange"))
        {
            MFile.PlayExpression(pcurrent.exprbegin, pcurrent.MIDI.Channel);
            MFile.SetTime(0);
            pcurrent.changeexp = 0;
        }
    }
}

const int CTrack::PlayChunk(int &Pnt, QDomLiteElement* XMLStaff, OCCounter& CountIt, OCPlayVarsArray pcurrent, const int NumOfTracks, OCSignList& SignsToPlay, CNotesToPlay& NotesToPlay, int& VoicedTime, OCMIDIFile& MFile, int MIDITrackNumber)
{
    bool EndTimeSet=false;
    QDomLiteElement* XMLVoice = XMLScoreWrapper::Voice(XMLStaff, TrackNum);

    //MFile.SetTime(TrackTime);
    MFile.SetTime(pcurrent[TrackNum].Currenttime);

    CountIt.killed=false;
    int currentlen = 0;
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, Pnt, pcurrent[TrackNum].PlayMeter);
        if (Symbol.IsRestOrValuedNote())
        {
            int Ticks=Symbol.ticks();
            if (Symbol.IsAnyNote())
            {
                int ThisPitch = Inside(Symbol.getVal("Pitch") + pcurrent[TrackNum].MIDI.Octave + pcurrent[TrackNum].MIDI.Transpose, 1, 127, 12);
                PlayPortamentoNotes(ThisPitch, NotesToPlay, pcurrent[TrackNum], MFile);
                if (!NotesToPlay.PitchExists(ThisPitch))
                {
                    NotesToPlay.Append(ThisPitch, Symbol.getVal("Inaudible"));
                    if (Symbol.IsNoteType(false,true))
                    {
                        NotesToPlay.ChangeState(NotesToPlay.Count()-1, tSPlayStart);
                    }
                }
                else
                {
                    if (Symbol.IsNoteType(true))
                    {
                        NotesToPlay.ChangeState(NotesToPlay.FindIndex(ThisPitch), tSPlayEnd);
                    }
                    else if (Symbol.IsNoteType(false,true))
                    {
                        NotesToPlay.ChangeState(NotesToPlay.FindIndex(ThisPitch), tSPlayNone);
                    }
                }
                NotesToPlay.Sort();
                ThisPitch = NotesToPlay.Pitch(NotesToPlay.Count()-1);
                int NoteOffPitch = ThisPitch;
                int NoteOnPitch = NoteOffPitch;
                int ThisVelocity = pcurrent[TrackNum].Currentdynam + (int)(pcurrent[TrackNum].crescendo) + (5 - (int)(((CountIt.Counter / 10) * 8) / pcurrent[TrackNum].PlayMeter));
                PlayExprClear(pcurrent[TrackNum], MFile, SignsToPlay);
                SignsToPlay.PlayBeforeNote(Symbol, ThisVelocity, NoteOnPitch, NoteOffPitch, MFile, pcurrent[TrackNum]);
                ThisVelocity = Inside(ThisVelocity, 0, 127, 1);

                PlayNotesOn(NotesToPlay, MFile, pcurrent[TrackNum], ThisVelocity, VoicedTime, NoteOnPitch);

                if (NotesToPlay.Count() ==1)
                {
                    if ((pcurrent[TrackNum].PortamentoOn) && (SignsToPlay.Exist("Trill"))) // 'pcurrent(TrackNum).Trill <> 0 Then
                    {
                        MFile.PlayController(0x54, NoteOnPitch, pcurrent[TrackNum].MIDI.Channel);
                    }
                    PlayPortamentoCleanUp(NotesToPlay, MFile, pcurrent[TrackNum]);
                }

                CountIt.PlayFlip(Ticks);
                int NoteTime = CalcNoteTime(Symbol, CountIt.Rounded, pcurrent[TrackNum], SignsToPlay);
                VoicedTime = CalcVoicedTime(NoteTime, CountIt.Rounded, pcurrent[TrackNum], SignsToPlay);

                PlayDuringNote(SignsToPlay, Symbol, VoicedTime, NoteOnPitch, EndTimeSet, MFile, pcurrent[TrackNum], NoteTime);

                if ((NotesToPlay.Count() == 1) && (NotesToPlay.State(0) != tSPlayStartEnd) && (NotesToPlay.State(0) != tSPlayEnd))
                {
                    NotesToPlay.ChangeState(0, tSPlayPortamento);
                }
                if (pcurrent[TrackNum].changeexp)
                {
                    if (Symbol.IsNoteType(false,true))
                    {
                        pcurrent[TrackNum].changeexp++;
                    }
                }

                PlayNotesOff(NotesToPlay, VoicedTime, MFile, pcurrent[TrackNum], EndTimeSet);

                //SignsToPlay.PlayCheckTempoChange(MFile, NoteOnPitch, express, changeexp, VoicedTime, pcurrent[TrackNum], MFile.GetTime());
                SignsToPlay.PlayCheckTempoChange(MFile, NoteOnPitch, VoicedTime, pcurrent[TrackNum], pcurrent[TrackNum].Currenttime);

                SignsToPlay.Decrement(Ticks);
            }
            else
            {
                //'It's a Pause
                CountIt.PlayFlip(Ticks);
                int NoteTime = CountIt.Rounded; // '+ pcurrent(TrackNum).Fermata
                if (SignsToPlay.Exist("Fermata")) NoteTime += SignsToPlay.Value("Fermata", "Duration").toInt();
                //VoicedTime = pcurrent[TrackNum].Currenttime + NoteTime;
                pcurrent[TrackNum].Currenttime += NoteTime;
                //                                      NoteOnPitch
                //SignsToPlay.PlayCheckTempoChange(MFile, 0, express, changeexp, VoicedTime, pcurrent[TrackNum], VoicedTime);
                SignsToPlay.PlayCheckTempoChange(MFile, 0, VoicedTime, pcurrent[TrackNum], pcurrent[TrackNum].Currenttime);
                //pcurrent[TrackNum].Currenttime = VoicedTime;
                SignsToPlay.Decrement(Ticks);
            }
            CountIt.Flip1(Ticks);
            currentlen = CountIt.Rounded;
            Pnt++;
            //TrackTime = pcurrent[TrackNum].Currenttime;
            return currentlen;
        }
        else if (Symbol.IsCompoundNote())
        {
            int ThisPitch = Inside(Symbol.getVal("Pitch") + pcurrent[TrackNum].MIDI.Octave + pcurrent[TrackNum].MIDI.Transpose, 1, 127, 12);
            if (!NotesToPlay.PitchExists(ThisPitch))
            {
                NotesToPlay.Append(ThisPitch, Symbol.getVal("Inaudible"));
                if (Symbol.IsNoteType(false,false,false,true))
                {
                    NotesToPlay.ChangeState(NotesToPlay.Count()-1, tSPlayStart);
                }
            }
            else
            {
                if (Symbol.IsNoteType(false,false,true))
                {
                    NotesToPlay.ChangeState(NotesToPlay.FindIndex(ThisPitch), tSPlayEnd);
                }
                else if (Symbol.IsNoteType(false,false,false,true))
                {
                    NotesToPlay.ChangeState(NotesToPlay.FindIndex(ThisPitch), tSPlayNone);
                }
            }
            Pnt++;
        }
        else if (Symbol.IsEndOfVoice())
        {
            CountIt.killed = true;
            return currentlen;
        }
        else
        {
            PlayBetweenNotes(Pnt, XMLStaff, NumOfTracks, CountIt, pcurrent, MFile, SignsToPlay, MIDITrackNumber);
        }
    }
    //TrackTime = pcurrent[TrackNum].Currenttime;
    return currentlen;
}

void CTrack::PlayNotesOn(CNotesToPlay& NotesToPlay, OCMIDIFile& MFile, OCPlayBackVarsType &pcurrent, const int ThisVelocity, const int VoicedTime, const int NoteOnPitch)
{
    int Data2=ThisVelocity;
    if (VoicedTime != 0)
    {
        Data2 = Inside(ThisVelocity - IntDiv(10 * Sgn(NotesToPlay.PortIt), VoicedTime), 0, 127, 1); //Used to be 1000???? Data 2 always 0
    }
    for (int iTemp=0;iTemp<NotesToPlay.Count();iTemp++)
    {
        int Pitch = NotesToPlay.Pitch(iTemp);
        if (iTemp==NotesToPlay.Count()-1) Pitch=NoteOnPitch; ////????
        switch (NotesToPlay.State(iTemp))
        {
        case tSPlayStart:
        case tSPlayStartEnd:
            int Data=Data2;
            if (NotesToPlay.Inaudible(iTemp)) Data = 0;
            MFile.Append(0x90 + pcurrent.MIDI.Channel, Pitch, Data);
            MFile.SetTime(0);
            //VoicedTime = 0;
            pcurrent.Currenttime = 0;
        }
    }
    /*
    Dim iTemp As Integer
    Dim Pitch As Integer
    Dim Data2 As Integer
    If NotesToPlay.Antal > 0 Then
        For iTemp = 0 To NotesToPlay.Antal - 1
            Pitch = NotesToPlay.Pitch(iTemp)
            GoSub NoteOn
        Next
    End If
    iTemp = NotesToPlay.Antal
    Pitch = NoteOnPitch
    GoSub NoteOn
Exit Sub

NoteOn:
    With MFile
        Select Case NotesToPlay.State(iTemp)
            Case tSPlayStart, tSPlayStartEnd
                '.Message = &H90 + pcurrent.Channel
                '.Data1 = pitch
                If VoicedTime = 0 Then
                    Data2 = ThisVelocity
                Else
                    Data2 = Inside(ThisVelocity - ((1000 * Sgn(NotesToPlay.PortIt%)) \ VoicedTime), 0, 127, 1)
                End If
                If NotesToPlay.Inaudible(iTemp) = True Then
                    Data2 = 0
                End If
                '.Action = MIDIFILE_INSERT_MESSAGE
                .Append &H90 + pcurrent.Channel, Pitch, Data2
                .Time = 0
                VoicedTime = 0
                pcurrent.Currenttime = 0
        End Select
    End With
    Return
    */
}

void CTrack::PlayDuringNote(OCSignList& SignsToPlay, XMLSymbolWrapper& XMLNote, const int VoicedTime, int& NoteOnPitch, bool &EndTimeSet, OCMIDIFile& MFile, OCPlayBackVarsType &pcurrent, const int NoteTime)
{
    int UnvoicedTime = MFile.GetTime();// ' - NoteTime
    pcurrent.currentcresc = pcurrent.exprbegin;
    pcurrent.express = pcurrent.exprbegin;
    if (SignsToPlay.Count())
    {
        SignsToPlay.PlayDuringNote(MFile, NoteOnPitch, UnvoicedTime, NoteTime, pcurrent);

        SignsToPlay.PlayAfterNote(XMLNote, pcurrent);
        pcurrent.Currenttime = pcurrent.Currenttime - VoicedTime;
        MFile.SetTime(UnvoicedTime);
        if (XMLNote.IsNoteType(false,true))
        {
            pcurrent.Currenttime += UnvoicedTime;
        }
        EndTimeSet = true;
    }
}

const int CTrack::SearchChunk(int &Pnt, QDomLiteElement* XMLVoice, QList<SymbolSearchLocation>& l, OCCounter& CountIt, OCPrintVarsArray fibset, const int NumOfTracks, const QString& SearchTerm)
{
    CountIt.killed=false;
    int currentlen = 0;
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, Pnt, fibset[TrackNum].Meter);
        if (Symbol.IsRestOrValuedNote())
        {
            int Ticks=Symbol.ticks();
            CountIt.Flip(Ticks);
            //TupletFactor = CountIt.factor;
            CountIt.Flip1(Ticks);
            currentlen = CountIt.Rounded;
            if (Symbol.Compare(SearchTerm))
            {
                SymbolSearchLocation s;
                s.Bar=CountIt.BarCounter;
                s.Voice=TrackNum;
                s.Staff=StaveNum;
                s.Pointer=Pnt;
                l.append(s);
            }
            Pnt++;
            return currentlen;
        }
        else if (Symbol.IsEndOfVoice())
        {
            //IsEnded = true;
            CountIt.killed = true;
            return currentlen;
        }
        else
        {
            l.append (SearchBetweenNotes(Pnt, XMLVoice, NumOfTracks, CountIt, fibset, SearchTerm));
        }
    }
    return currentlen;
}

const int CTrack::FakePlotChunk(int &Pnt, QDomLiteElement* XMLVoice, OCCounter& CountIt, OCPrintVarsArray fibset, const int NumOfTracks)
{
    CountIt.killed=false;
    int currentlen = 0;
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, Pnt, fibset[TrackNum].Meter);
        if (Symbol.IsRestOrValuedNote())
        {
            int Ticks=Symbol.ticks();
            CountIt.Flip(Ticks);
            //TupletFactor = CountIt.factor;
            CountIt.Flip1(Ticks);
            currentlen = CountIt.Rounded;
            Pnt++;
            return currentlen;
        }
        else if (Symbol.IsEndOfVoice())
        {
            //IsEnded = true;
            CountIt.killed = true;
            return currentlen;
        }
        else
        {
            FakePlotBetweenNotes(Pnt, XMLVoice, NumOfTracks, CountIt, fibset);
        }
    }
    return currentlen;
}

const int CTrack::GetFirstChannel(QDomLiteElement* XMLVoice)
{
    int py=0;
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice, py, 0);
        if (Symbol.IsRestOrValuedNote())
        {
            return 0;
        }
        if (Symbol.IsEndOfVoice())
        {
            return 0;
        }
        else if (Symbol.Compare("Channel"))
        {
            return Symbol.getVal("Channel") - 1;
        }
        py++;
    }
}

CStaff::CStaff()
{
    CTrack* T=new CTrack;
    T->TrackNum=0;
    T->StaveNum=0;
    Track.append(T);
    Solo=false;
    Mute=false;
    StaveNum=0;
    //CurrentMeter = 96;
}

CStaff::~CStaff()
{
    qDeleteAll(Track);
    Track.clear();
}

const int CStaff::NumOfTracks() const
{
    return Track.count();
}
/*
const int CStaff::BeginMeter(const int mTrack) const
{
    return Track[mTrack]->fibset.Meter;
}
*/
void CStaff::AddTrack()
{
    CTrack* T=new CTrack;
    T->TrackNum=Track.count();
    T->StaveNum=this->StaveNum;
    Track.append(T);
}

void CStaff::FormatBar(const int CurrentBar, const int ActualBar, OCBarList& BarList, XMLScoreWrapper& XMLScore, QDomLiteElement *XMLTemplate)
{
    for (int iTemp=0; iTemp<NumOfTracks(); iTemp++)
    {
        Track[iTemp]->FormatBar(CurrentBar, ActualBar, BarList, XMLScore, XMLTemplate);
    }
}

void CStaff::plMTr(OCBarList& BarList, QDomLiteElement* XMLStaff, QDomLiteElement* XMLTemplate, const QColor TC, OCSymbolArray& MTObj, OCDraw& ScreenObj, XMLScoreWrapper& XMLScore)
{
    Track[0]->plMTr(BarList, XMLScoreWrapper::Voice(XMLStaff, 0), XMLTemplate, TC, MTObj, ScreenObj, XMLScore);
}

void CStaff::DeleteTrack(const int TrackNumber)
{
    delete Track[TrackNumber];
    Track.removeAt(TrackNumber);
}

void CStaff::PlStaff(OCBarList& BarList, XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate, OCSymbolArray& SymbolList, const QColor color, QList<CStaff*> &Staffs, const int ActiveStave, const int ActiveTrack, const int StavePlacement, OCDraw& ScreenObj)
{
    int MS = XMLScore.getVal("MasterStaff");
    SymbolList.clear();
    OCNoteListArray NoteLists=new OCNoteList[Track.count()];
    QDomLiteElement* TemplateStaff = XMLScore.TemplateStaff(XMLTemplate, StavePlacement);
    OCPrintStaffVarsType sCurrent(TemplateStaff);
    ScreenObj.col = color;
    ScreenObj.XFactor=SizeFactor(TemplateStaff->attributeValue("Size"));
    ScreenObj.ScreenSize=XMLScore.getVal("Size")*ScreenObj.XFactor;
    PlLines(BarList.SystemLength(), XMLScore, XMLTemplate, BarList.StartBar(), StavePlacement, ScreenObj);
    ScreenObj.setcol(QColor(unselectablecolor));
    Track[0]->PlfirstAcc(BarList, ScreenObj);
    Track[0]->PlfirstClef(BarList, ScreenObj);
    ScreenObj.col=color;
    if (StavePlacement == 0)
    {
        QColor MTColor = color;
        if ((StaveNum != MS) && ScreenObj.ColorOn) MTColor = inactivestaffcolor;
        Staffs[MS]->plMTr(BarList, XMLScore.Staff(MS), XMLTemplate, MTColor, SymbolList, ScreenObj, XMLScore);
    }
    FillNoteLists(NoteLists, BarList, XMLScore.Staff(StaveNum), ScreenObj);
    for (int iTemp = 0; iTemp<Track.count(); iTemp++)
    {
        if ((StaveNum == ActiveStave) && (iTemp == ActiveTrack))
        {}
        else
        {
            QColor col = color;
            if (ScreenObj.ColorOn) col = inactivestaffcolor;
            NoteLists[iTemp].SearchForBeams();
            OCSymbolArray FakeList;
            Track[iTemp]->PlTrack(BarList, XMLScore, FakeList, NoteLists[iTemp], ScreenObj, sCurrent, col);
            NoteLists[iTemp].PlotBeams(col, ScreenObj);
        }
    }
    if (StaveNum == ActiveStave)
    {
        QColor col = color;
        NoteLists[ActiveTrack].SearchForBeams();
        Track[ActiveTrack]->PlTrack(BarList, XMLScore, SymbolList, NoteLists[ActiveTrack], ScreenObj, sCurrent, col);
        NoteLists[ActiveTrack].PlotBeams(col, ScreenObj);
    }
    delete [] NoteLists;
}

void CStaff::PlLines(const int SystemLength, XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate, const int StartBar, const int StavePlacement, OCDraw& ScreenObj)
{
    QDomLiteElement* TemplateStaff = XMLScore.TemplateStaff(XMLTemplate, StavePlacement);
    ScreenObj.DM(0,-ScoreStaffHeight/ScreenObj.XFactor);
    ScreenObj.DI();
    int syslen=SystemLength*ScreenObj.XFactor;
    ScreenObj.DM(syslen, ScoreStaffHeight);
    ScreenObj.DL(-syslen, 0);
    ScreenObj.DR(0, -96);
    ScreenObj.DL(syslen, 0);
    ScreenObj.DR(0, -96);
    ScreenObj.DL(-syslen, 0);
    ScreenObj.DR(0, -96);
    ScreenObj.DL(syslen, 0);
    ScreenObj.DR(0, -96);
    ScreenObj.DL(-syslen, 0);

    int Height = TemplateStaff->attributeValue("Height");
    int iTemp1 = (ScoreStaffHeight + (Height*12))*ScreenObj.XFactor;
    if (StavePlacement == XMLTemplate->childCount()-1) iTemp1 = ScoreStaffLinesHeight;
    ScreenObj.DM(0, ScoreStaffHeight);
    ScreenObj.DL(0, -iTemp1);
    ScreenObj.DR(-24*ScreenObj.XFactor, 0);
    ScreenObj.PlRect(-24*ScreenObj.XFactor,iTemp1);

    if ((StavePlacement == 0) && (StartBar > 0))
    {
        if (!XMLScore.getVal("DontShowBN"))
        {
            ScreenObj.DM(0, ScoreStaffHeight + (216*ScreenObj.XFactor));
            ScreenObj.setcol(QColor(unselectablecolor));
            ScreenObj.plLet(QString::number(1 + StartBar + (int)XMLScore.getVal("BarNrOffset")).trimmed(), 0, "Times New Roman", false, false, 140*ScreenObj.XFactor);
            ScreenObj.resetcol();
        }
    }

    if (TemplateStaff->attributeValue("SquareBracket") == SBEnd)
    {
        ScreenObj.DM(-108*ScreenObj.XFactor, ScoreStaffHeight);
        ScreenObj.PlSquare2(ScoreStaffLinesHeight,60*ScreenObj.XFactor,24*ScreenObj.XFactor);
    }
    else if (TemplateStaff->attributeValue("SquareBracket") == SBBegin)
    {
        ScreenObj.DM(-108*ScreenObj.XFactor,ScoreStaffHeight);
        if (StavePlacement > 0)
        {
            if (XMLScore.TemplateStaff(XMLTemplate, StavePlacement - 1)->attributeValue("SquareBracket") != SBBegin)
            {
                ScreenObj.PlSquare1(iTemp1,60*ScreenObj.XFactor,24*ScreenObj.XFactor);
            }
            else
            {
                ScreenObj.PlSquare1(iTemp1,0,24*ScreenObj.XFactor);
            }
        }
        else
        {
            ScreenObj.PlSquare1(iTemp1,60*ScreenObj.XFactor,24*ScreenObj.XFactor);
        }
    }

    if (TemplateStaff->attributeValue("CurlyBracket")>CBNone)
    {
        ScreenObj.DM(-108*ScreenObj.XFactor, ScoreStaffHeight);
        if (TemplateStaff->attributeValue("SquareBracket")==SBNone) ScreenObj.DR(48*ScreenObj.XFactor,0);
        ScreenObj.PlCurly(iTemp1+(ScoreStaffLinesHeight*ScreenObj.XFactor),12*8*ScreenObj.XFactor,24*ScreenObj.XFactor);
    }
}

void CStaff::FillNoteLists(OCNoteListArray NoteLists, OCBarList& BarList, QDomLiteElement* XMLStaff, OCDraw& ScreenObj)
{
    OCPointerArray py=new int[NumOfTracks()];
    OCPrintVarsArray dcurrent=new OCPrintVarsType[NumOfTracks()];
    OCStaffAccidentals lfortegn;
    CStaffCounter StaffCount(NumOfTracks());
    for (int iTemp = 0; iTemp<NumOfTracks(); iTemp++)
    {
        Track[iTemp]->getsetting(dcurrent[iTemp]);
        py[iTemp] = dcurrent[iTemp].FilePointer;
    }
    lfortegn.RdAcc(dcurrent[StaffCount.FirstValidVoice()].CurrentKey.val);
    float BarX = BarList.BarX()*ScreenObj.XFactor;
    float XFysic = BarX;
    forever
    {
        lfortegn.ResetCluster();
        lfortegn.ClearIgnore();
        for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
        {
            if (StaffCount.Ready(iTemp))
            {
                StaffCount.SetCurrentVoice(iTemp);
                int Rounded=Track[iTemp]->FillChunk(py[iTemp], StaffCount.VoiceCounter(), NoteLists[iTemp], dcurrent, NumOfTracks(), XMLScoreWrapper::Voice(XMLStaff, iTemp), lfortegn, BarList, XFysic, LastTiedNotes, ScreenObj);
                if (NumOfTracks() > 1) NoteLists[iTemp].SetVoiceUpDown(iTemp);
                StaffCount.SetNewLen(Rounded, iTemp);
            }
        }
        if (StaffCount.Killed()) break; //if (Ended == NumOfTracks()) break;
        lfortegn.CheckFortegn(dcurrent[0].J);
        for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
        {
            if (StaffCount.WasReady(iTemp))
            {
                if (!StaffCount.Killed(iTemp)) NoteLists[iTemp].SetHasFortegn(lfortegn);
            }
        }
        CheckChordCollision(NoteLists, StaffCount);
        StaffCount.Flip();
        XFysic = (BarList.CalcX(StaffCount.BarCounter, StaffCount.Beat)*ScreenObj.XFactor) + BarX;
        if (StaffCount.NewBar(dcurrent[StaffCount.FirstValidVoice()].Meter))
        {
            lfortegn.RdAcc(dcurrent[StaffCount.FirstValidVoice()].CurrentKey.val);
            StaffCount.BarFlip();
            if (StaffCount.BarCounter > BarList.BarsToPrint() - 1) break;
            BarX = XFysic + ((begofbar+endofbar)*ScreenObj.XFactor);
            if (XFysic > (BarList.SystemLength()*ScreenObj.XFactor)) break;
            BarX += BarList.BegSpace(StaffCount.BarCounter, true, true, true)*ScreenObj.XFactor;
            XFysic = BarX;
        }
    }
    delete [] py;
    delete [] dcurrent;
}

void CStaff::Findall(const int BarToFind, QDomLiteElement* XMLStaff)
{
    OCPointerArray py=new int[NumOfTracks()];
    OCPrintVarsArray dcurrent=new OCPrintVarsType[NumOfTracks()];
    CStaffCounter StaffCount(NumOfTracks());
    for (int iTemp = 0; iTemp<NumOfTracks(); iTemp++)
    {
        py[iTemp]=0;
    }
    while (StaffCount.BarCounter < BarToFind)
    {
        LastTiedNotes.clear();
        for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
        {
            if (StaffCount.Ready(iTemp))
            {
                StaffCount.SetCurrentVoice(iTemp);
                int Rounded=Track[iTemp]->FiBChunk(py[iTemp], XMLScoreWrapper::Voice(XMLStaff, iTemp), StaffCount.VoiceCounter(), dcurrent, NumOfTracks(), LastTiedNotes);
                StaffCount.SetNewLen(Rounded, iTemp);
            }
        }
        if (StaffCount.Killed()) break; //if (Ended == NumOfTracks()) break;
        StaffCount.Flip();
        for (int iTemp = 0 ; iTemp < NumOfTracks(); iTemp++)
        {
            if (!StaffCount.Killed(iTemp)) dcurrent[iTemp].Decrement(StaffCount.Min / StaffCount.TupletFactor(iTemp));//Track[iTemp]->TupletFactor);
        }
        if (StaffCount.NewBar(dcurrent[StaffCount.FirstValidVoice()].Meter))
        {
            if (StaffCount.BarCounter < BarToFind - 1)
            {
                StaffCount.BarFlip();
            }
            else
            {
                break;
            }
        }
    }
    for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
    {
        dcurrent[iTemp].FilePointer = py[iTemp];
        Track[iTemp]->putsetting(dcurrent[iTemp]);
    }
    delete [] py;
    delete [] dcurrent;
}

bool CStaff::ChordCollision(OCNoteList& NoteList1, OCNoteList& NoteList2) const
{
    QList<int> LineNums1;
    QList<int> LineNums2;
    NoteList1.FillLineNumsArray(LineNums1);
    NoteList2.FillLineNumsArray(LineNums2);
    for (int iTemp1 = 0 ; iTemp1< LineNums1.count() ; iTemp1++)
    {
        for (int iTemp2 = 0 ; iTemp2 < LineNums2.count() ; iTemp2++)
        {
            if ((LineNums1[iTemp1] + 1 >= LineNums2[iTemp2]) && (LineNums1[iTemp1] - 1 <= LineNums2[iTemp2])) return true;
        }
    }
    return false;
}

void CStaff::CheckChordCollision(OCNoteListArray NoteLists, CStaffCounter& StaffCount)
{
    int AccMoved=0;
    if (NumOfTracks() < 2) return;
    OCPointerArray times=new int[NumOfTracks()];
    OCPointerArray times1=new int[NumOfTracks()];
    for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
    {
        times[iTemp]=0;
        times1[iTemp]=0;
        if (StaffCount.WasReady(iTemp))
        {
            if (!StaffCount.Killed(iTemp))
            {
                for (int iTemp1 = 1 ; iTemp1 < iTemp; iTemp1++)
                {
                    if (StaffCount.WasReady(iTemp1))
                    {
                        if (!StaffCount.Killed(iTemp1))
                        {
                            if (ChordCollision(NoteLists[iTemp1], NoteLists[iTemp]))
                            {
                                times[iTemp] ++;
                                times1[iTemp1] ++;
                            }
                        }
                    }
                }
            }
        }
    }
    for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
    {
        if (times[iTemp] > 0)
        {
            NoteLists[iTemp].MoveChord(times[iTemp]);
            NoteLists[iTemp].Moveaccidental(times[iTemp], 0);
        }
    }
    for (int iTemp = NumOfTracks() - 1 ; iTemp>=0; iTemp--)
    {
        if (times1[iTemp] > 0) AccMoved += NoteLists[iTemp].Moveaccidental(0, AccMoved + 1);
    }
    delete [] times;
    delete [] times1;
}

void CStaff::fibPlay(const int BarToFind, QDomLiteElement* XMLStaff, OCMIDIFile& MFile, OCPointerArray py, const int TrackOffset, OCPlayVarsArray pcurrent, OCSignListArray SignsToPlay)
{
    OCPointerArray AccelCounter=new int[NumOfTracks()];
    CStaffCounter StaffCount(NumOfTracks());
    for (int iTemp = 0 ; iTemp<NumOfTracks();iTemp++)
    {
        MFile.SetTrackNumber(iTemp + TrackOffset);
        AccelCounter[iTemp]=0;
        py[iTemp] = 0; //'Track(iTemp).tstart
        pcurrent[iTemp].MIDI.Channel = Track[iTemp]->GetFirstChannel(XMLScoreWrapper::Voice(XMLStaff, iTemp));
        MFile.SetTime(0);
    }
    while (StaffCount.BarCounter < BarToFind)
    {
        for (int iTemp = 0 ; iTemp < NumOfTracks(); iTemp++)
        {
            if (StaffCount.Ready(iTemp))
            {
                MFile.SetTrackNumber(iTemp + TrackOffset);
                StaffCount.SetCurrentVoice(iTemp);
                int Rounded=Track[iTemp]->FiBPlayChunk(py[iTemp], XMLStaff, StaffCount.VoiceCounter(), pcurrent, NumOfTracks(), SignsToPlay[iTemp], AccelCounter[iTemp], MFile, TrackOffset);
                StaffCount.SetNewLen(Rounded, iTemp);
            }
        }
        if (StaffCount.Killed()) break;
        StaffCount.Flip();
        if (StaffCount.NewBarPlay(pcurrent[StaffCount.FirstValidVoice()].PlayMeter))
        {
            if (StaffCount.BarCounter < BarToFind)
            {
                StaffCount.BarFlip();
            }
            else
            {
                break;
            }
        }
    }
    for (int iTemp = 0 ; iTemp < NumOfTracks(); iTemp++)
    {
        //if (py[iTemp] > 0)
        //{
            MFile.SetTrackNumber(iTemp + TrackOffset);
            MFile.PlayController(0x79, 0, pcurrent[iTemp].MIDI.Channel);
            MFile.PlayExpression(expressiondefault, pcurrent[iTemp].MIDI.Channel);
            //'MFile.PlayController &H41, 64, pcurrent[iTemp].Channel
            MFile.PlayController(0x5, 0x10, pcurrent[iTemp].MIDI.Channel);
            MFile.PlayController(0x41, 0, pcurrent[iTemp].MIDI.Channel);
            MFile.PlayController(0x7, 0x7F, pcurrent[iTemp].MIDI.Channel);
            MFile.PlayController(0xA, 64, pcurrent[iTemp].MIDI.Channel);
            MFile.PlayPatch(pcurrent[iTemp].MIDI.Patch, pcurrent[iTemp].MIDI.Channel);
            if (AccelCounter[iTemp] > 0)
            {
                if (pcurrent[iTemp].Accel < 0)
                {
                    pcurrent[iTemp].Playtempo = pcurrent[iTemp].Playtempo + ((AccelCounter[iTemp] * 10) / ((pcurrent[iTemp].Playtempo * 8) / -pcurrent[iTemp].Accel));
                }
                else if (pcurrent[iTemp].Accel > 0)
                {
                    pcurrent[iTemp].Playtempo = pcurrent[iTemp].Playtempo - ((AccelCounter[iTemp] * 10) / ((pcurrent[iTemp].Playtempo * 8) / pcurrent[iTemp].Accel));
                }
                MFile.Playtempo(pcurrent[iTemp].Playtempo);
                AccelCounter[iTemp] = 0;
            }
            MFile.SetTime(0);
        //}
    }
    delete [] AccelCounter;
}

void CStaff::Play(OCMIDIFile& MFile, int& MIDITrackNumber, const int StartBar, QDomLiteElement* XMLStaff, const int silence)
{
    OCSignListArray SignsToPlay=new OCSignList[NumOfTracks()];
    OCPlayVarsArray pcurrent=new OCPlayBackVarsType[NumOfTracks()];
    OCPointerArray py=new int[NumOfTracks()];
    OCNotesToPlayArray NotesToPlay=new CNotesToPlay[NumOfTracks()];
    OCPointerArray VoicedTime=new int[NumOfTracks()];

    CStaffCounter StaffCount(NumOfTracks());
    for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
    {
        py[iTemp]=0;
        VoicedTime[iTemp]=0;
        MFile.InsertTrack();
    }
    fibPlay(StartBar, XMLStaff, MFile, py, MIDITrackNumber, pcurrent, SignsToPlay);
    for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
    {
        pcurrent[iTemp].Currenttime = silence;
    }
    StaffCount.reset();
    forever
    {
        for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
        {
            if (StaffCount.Ready(iTemp))
            {
                MFile.SetTrackNumber(iTemp + MIDITrackNumber);
                StaffCount.SetCurrentVoice(iTemp);
                int Rounded=Track[iTemp]->PlayChunk(py[iTemp], XMLStaff, StaffCount.VoiceCounter(), pcurrent, NumOfTracks(), SignsToPlay[iTemp], NotesToPlay[iTemp], VoicedTime[iTemp], MFile, MIDITrackNumber);
                StaffCount.SetNewLen(Rounded, iTemp);
            }
        }
        if (StaffCount.Killed()) break; //if (Ended == NumOfTracks()) break;
        StaffCount.Flip();
        if (StaffCount.NewBarPlay(pcurrent[StaffCount.FirstValidVoice()].PlayMeter))
        {
            StaffCount.BarFlip();
        }
    }
    MIDITrackNumber += NumOfTracks();
    delete [] SignsToPlay;
    delete [] pcurrent;
    delete [] py;
    delete [] NotesToPlay;
    delete [] VoicedTime;
}

const QList<SymbolSearchLocation> CStaff::Search(QDomLiteElement* XMLStaff, const QString& SearchTerm, const int TrackToSearch) const
{
    QList<SymbolSearchLocation> l;
    OCPointerArray py=new int[NumOfTracks()];
    OCPrintVarsType* dcurrent=new OCPrintVarsType[NumOfTracks()];
    CStaffCounter StaffCount(NumOfTracks());
    for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
    {
        py[iTemp] = 0;//PointerBegin(iTemp);//dcurrent[iTemp].FilePointer;
    }
    forever
    {
        for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
        {
            if (StaffCount.Ready(iTemp))
            {
                StaffCount.SetCurrentVoice(iTemp);
                int Rounded=0;
                if ((TrackToSearch==-1) || (iTemp==TrackToSearch))
                {
                    Rounded=Track[iTemp]->SearchChunk(py[iTemp], XMLScoreWrapper::Voice(XMLStaff, iTemp), l, StaffCount.VoiceCounter(), dcurrent, NumOfTracks(), SearchTerm);
                }
                else
                {
                    Rounded=Track[iTemp]->FakePlotChunk(py[iTemp], XMLScoreWrapper::Voice(XMLStaff, iTemp), StaffCount.VoiceCounter(), dcurrent, NumOfTracks());
                }
                StaffCount.SetNewLen(Rounded, iTemp);
            }
        }
        if (StaffCount.Killed()) break; //if (Ended == NumOfTracks()) break;
        StaffCount.Flip();
        for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
        {
            if (!StaffCount.Killed(iTemp)) dcurrent[iTemp].Decrement(StaffCount.Min / StaffCount.TupletFactor(iTemp));//Track[iTemp]->TupletFactor);
        }
        if (StaffCount.NewBar(dcurrent[StaffCount.FirstValidVoice()].Meter)) StaffCount.BarFlip();
    }
    delete [] py;
    delete [] dcurrent;
    return l;
}

void CStaff::FakePlot(const int TrackToSearch, const int PointerGoal, QDomLiteElement* XMLStaff, OCMIDIVars& MIDI)
{
    OCPointerArray py=new int[NumOfTracks()];
    OCPrintVarsType* dcurrent=new OCPrintVarsType[NumOfTracks()];
    CStaffCounter StaffCount(NumOfTracks());
    for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
    {
        Track[iTemp]->getsetting(dcurrent[iTemp]);
        py[iTemp] = dcurrent[iTemp].FilePointer;
    }
    forever
    {
        for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
        {
            if (StaffCount.Ready(iTemp))
            {
                StaffCount.SetCurrentVoice(iTemp);
                int Rounded=Track[iTemp]->FakePlotChunk(py[iTemp], XMLScoreWrapper::Voice(XMLStaff, iTemp), StaffCount.VoiceCounter(), dcurrent, NumOfTracks());
                StaffCount.SetNewLen(Rounded, iTemp);
            }
            if ((iTemp == TrackToSearch) && (py[iTemp] >= PointerGoal))
            {
                StaffCount.Quit();
                break;
            }
        }
        if (StaffCount.Killed()) break; //if (Ended == NumOfTracks()) break;
        StaffCount.Flip();
        for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
        {
            if (!StaffCount.Killed(iTemp)) dcurrent[iTemp].Decrement(StaffCount.Min / StaffCount.TupletFactor(iTemp));//Track[iTemp]->TupletFactor);
        }
        if (StaffCount.NewBar(dcurrent[StaffCount.FirstValidVoice()].Meter)) StaffCount.BarFlip();
    }
    MIDI=dcurrent[TrackToSearch].MIDI;
    delete [] py;
    delete [] dcurrent;
}

void CStaff::SetStavenum(const int Stave)
{
    foreach(CTrack* t,Track) t->StaveNum = Stave;
    StaveNum = Stave;
}

void CStaff::FillBarsArray(QDomLiteElement* XMLStaff, OCBarMap& bars, const int StaffOffset)
{
    OCPointerArray py=new int[NumOfTracks()];
    //OCPointerArray fipmeter=new int[NumOfTracks()];
    OCPrintVarsArray dcurrent=new OCPrintVarsType[NumOfTracks()];
    QList<int> dummy;
    OCBarWindowBar* VoiceBar = new OCBarWindowBar[NumOfTracks()];
    CStaffCounter StaffCount(NumOfTracks());
    for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
    {
        OCBarWindowVoice v;
        v.Voice=iTemp;
        v.Incomplete=false;
        v.EndPointer=0;
        bars.Voices.append(v);
        py[iTemp] = 0; // 'Track(iTemp).tstart
        //fipmeter[iTemp] = 96;
    }
    forever
    {
        for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
        {
            if (StaffCount.Ready(iTemp))
            {
                if (!StaffCount.Killed(iTemp))
                {
                    VoiceBar[iTemp].Density++;
                }
                StaffCount.SetCurrentVoice(iTemp);
                int Rounded=Track[iTemp]->FiBChunk(py[iTemp], XMLScoreWrapper::Voice(XMLStaff, iTemp), StaffCount.VoiceCounter(), dcurrent, NumOfTracks(), dummy);
                if (StaffCount.Beat==0)
                {
                    VoiceBar[iTemp].KeyChangeOnOne=dcurrent[iTemp].KeyChange;
                    VoiceBar[iTemp].ClefChangeOnOne=dcurrent[iTemp].ClefChange;
                }
                VoiceBar[iTemp].MasterStuff=VoiceBar[iTemp].MasterStuff | dcurrent[iTemp].MasterStuff;
                StaffCount.SetNewLen(Rounded, iTemp);
                if (py[iTemp]>0)
                {
                    XMLSymbolWrapper Symbol(XMLScoreWrapper::Voice(XMLStaff, iTemp), py[iTemp]-1, dcurrent[iTemp].Meter);
                    if (Symbol.IsValuedNote())
                    {
                        VoiceBar[iTemp].Notes ++;
                    }
                    if (Symbol.IsRest() && (Symbol.ticks()==dcurrent[iTemp].Meter))
                    {
                        VoiceBar[iTemp].IsFullRest=true;
                        if (VoiceBar[iTemp].Pointer==py[iTemp]-1) VoiceBar[iTemp].IsFullRestOnly=true;
                    }
                }
                dummy.clear();
            }
        }
        if (StaffCount.Killed()) break; //if (Ended == NumOfTracks()) break;
        StaffCount.Flip();
        for (int iTemp = 0 ; iTemp < NumOfTracks(); iTemp++)
        {
            if (!StaffCount.Killed(iTemp)) dcurrent[iTemp].Decrement(StaffCount.Min / StaffCount.TupletFactor(iTemp));//Track[iTemp]->TupletFactor);
        }
        if (StaffCount.NewBar(dcurrent[StaffCount.FirstValidVoice()].Meter))
        {
            StaffCount.BarFlip();
            for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
            {
                if (!StaffCount.Killed(iTemp))
                {
                    VoiceBar[iTemp].Meter=dcurrent[iTemp].Meter;
                    VoiceBar[iTemp].MeterText=dcurrent[iTemp].MeterText;
                    OCBarWindowBar b(VoiceBar[iTemp]);
                    //b.Density=Items[iTemp];
                    //b.Notes=Notes[iTemp];
                    //b.Pointer=BarPointers[iTemp];
                    //b.Meter=fipmeter[iTemp];
                    bars.Voices[StaffOffset+iTemp].Bars.append(b);
                    VoiceBar[iTemp].Pointer=py[iTemp];

                }
                VoiceBar[iTemp].Density = 0;
                VoiceBar[iTemp].Notes = 0;
                VoiceBar[iTemp].IsFullRest = false;
                VoiceBar[iTemp].IsFullRestOnly = false;
                VoiceBar[iTemp].ClefChangeOnOne = false;
                VoiceBar[iTemp].KeyChangeOnOne = false;
                VoiceBar[iTemp].MasterStuff = false;
            }
        }
    }
    for (int iTemp = 0 ; iTemp < NumOfTracks() ; iTemp++)
    {
        if (py[iTemp]>VoiceBar[iTemp].Pointer)
        {
            VoiceBar[iTemp].Meter=dcurrent[iTemp].Meter;
            VoiceBar[iTemp].MeterText=dcurrent[iTemp].MeterText;
            OCBarWindowBar b(VoiceBar[iTemp]);
            //b.Density=Items[iTemp];
            //b.Notes=Notes[iTemp];
            //b.Pointer=BarPointers[iTemp];
            //b.Meter=fipmeter[iTemp];
            bars.Voices[StaffOffset+iTemp].Incomplete=true;
            bars.Voices[StaffOffset+iTemp].Bars.append(b);
        }
        bars.Voices[StaffOffset+iTemp].EndPointer=py[iTemp];
    }
    delete [] py;
    delete [] dcurrent;
    delete [] VoiceBar;
}

OCScore::OCScore()
{
    CStaff* S= new CStaff;
    S->SetStavenum(0);
    Staff.append(S);
}

OCScore::~OCScore()
{
    qDeleteAll(Staff);
    Staff.clear();
}

const int OCScore::NumOfStaves()
{
    return Staff.count();
}

void OCScore::PlSystem(const int q, XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate, const QColor color, const int ActiveStave, const int ActiveTrack, const int StavePlacement, OCDraw& ScreenObj)
{
    Staff[q]->ItemList.clear();
    ScreenObj.StartList();
    Staff[q]->PlStaff(BarList, XMLScore, XMLTemplate, SymbolList, color, Staff, ActiveStave, ActiveTrack, StavePlacement, ScreenObj);
    Staff[q]->ItemList.append(ScreenObj.EndList());
}

void OCScore::PlSystemNoList(const int q, XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate, const QColor color, const int ActiveStave, const int ActiveTrack, const int StavePlacement, OCDraw& ScreenObj)
{
    Staff[q]->PlStaff(BarList, XMLScore, XMLTemplate, SymbolList, color, Staff, ActiveStave, ActiveTrack, StavePlacement, ScreenObj);
}

const int OCScore::ActuallyPrinted() const
{
    return BarList.ActuallyPrinted();
}

const int OCScore::StartBar() const
{
    return BarList.StartBar();
}

const int OCScore::SystemLength() const
{
    return BarList.SystemLength();
}

const bool OCScore::IsEnded(QDomLiteElement* XMLTemplate) const
{
    //return (BarMap().BarCountAll(XMLTemplate) <= StartBar());
    return BarMap().IsEnded(StartBar(),XMLTemplate);
}

void OCScore::EraseSystem(int Stave, QGraphicsScene *Scene)
{
    foreach(QGraphicsItem* item,Staff[Stave]->ItemList)
    {
        Scene->removeItem(item);
        delete item;
    }
    Staff[Stave]->ItemList.clear();
}

void OCScore::EraseAll(QGraphicsScene* Scene)
{
    foreach(CStaff* s,Staff) s->ItemList.clear();
    Scene->clear();
}

void OCScore::PageBackFormat(XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate)
{
    if (StartBar()<=0) return;
    int NewEndbar=StartBar();
    if (IsEnded(XMLTemplate))
    {
        int Start=qMax(0,StartBar()-20);
        FormatPage(XMLScore,XMLTemplate,SystemLength(),Start);
        forever
        {
            if (Start+BarList.ActuallyPrinted() >= NewEndbar) return;
            Start++;
            FormatPage(XMLScore,XMLTemplate,SystemLength(),Start);
        }
    }
    else
    {
        int Start=StartBar()-1;
        FormatPage(XMLScore,XMLTemplate,SystemLength(),Start);
        forever
        {
            if (Start+BarList.ActuallyPrinted() <= NewEndbar) return;
            if (Start<=0) return;
            Start--;
            FormatPage(XMLScore,XMLTemplate,SystemLength(),Start);
        }
    }
}

void OCScore::FormatPage(XMLScoreWrapper& XMLScore, QDomLiteElement *XMLTemplate, const int SysLen, const int Start, const int End)
{
    bool PreviousFullRest=false;
    bool PreviousFullRestOnly=false;

    findall(Start, XMLScore, XMLTemplate);
    BarList.reset(SysLen,Start,XMLScore.AllTemplateIndex(XMLTemplate->firstChild()));
    int BarNr=0;
    int ActualBarNr=0;
    for (BarNr = 0 ; BarNr < 20 ; BarNr++)
    {
        if (ActualBarNr+Start>BarMap().BarCount(BarList.longeststaff,BarList.longestvoice)-1)
        {
            for (int iTemp = 0 ; iTemp < XMLTemplate->childCount() ; iTemp++)
            {
                for (int iTemp1 = 0 ; iTemp1< XMLScore.Staff(XMLScore.AllTemplateIndex(XMLTemplate,iTemp))->childCount() ; iTemp1++)
                {
                    if (BarMap().BarCount(XMLScore.AllTemplateIndex(XMLTemplate,iTemp),iTemp1)-1>=ActualBarNr+Start)
                    {
                        BarList.longeststaff=iTemp;
                        BarList.longestvoice=iTemp1;
                        break;
                    }
                }
            }
        }
        BarList.SetMeter(BarNr,BarMap().GetMeter(Start + ActualBarNr,BarList.longeststaff,BarList.longestvoice));
        bool IsFullRest=false;
        bool IsFullRestOnly=false;
        forever
        {
            for (int iTemp = 0 ; iTemp < XMLTemplate->childCount() ; iTemp++)
            {
                Staff[XMLScore.AllTemplateIndex(XMLTemplate,iTemp)]->FormatBar(BarNr, ActualBarNr, BarList, XMLScore, XMLTemplate);
            }
            IsFullRest=BarMap().IsFullRest(Start+ActualBarNr, XMLTemplate);
            IsFullRestOnly=BarMap().IsFullRestOnly(Start+ActualBarNr, XMLTemplate) & !BarMap().MasterStuff(Start+ActualBarNr,XMLScore.getVal("MasterStaff"));
            if (IsFullRestOnly)
            {
                if ((PreviousFullRest && (BarList.GetMultiPause(BarNr - 1) == 1)) || PreviousFullRestOnly)
                {
                    BarList.SetMultiPause(BarNr - 1, BarList.GetMultiPause(BarNr - 1) + 1);
                    ActualBarNr ++;
                    PreviousFullRest = IsFullRest;
                    PreviousFullRestOnly = IsFullRestOnly;
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        PreviousFullRest = IsFullRest;
        PreviousFullRestOnly = IsFullRestOnly;
        if (BarList.GetMultiPause(BarNr - 1) > 4)
        {
            BarList.SetMinimumAll(BarNr - 1, (BarList.GetMinimumTotal(BarNr - 1) / ((BarList.GetMultiPause(BarNr - 1) - 1) / 4) * 0.4) + 2);
        }
        if (BarList.GetMinimumTotal(BarNr) == maxticks)
        {
            BarList.SetMinimumAll(BarNr, 24);
            break;
        }
        if (End==0)
        {
            if (BarNr > 0)
            {
                if (BarList.XTest(BarNr+1)<(XMLScore.getVal("NoteSpace")+1)*12) break;
            }
        }
        else
        {
            if (ActualBarNr + Start >= End) break;
        }
        ActualBarNr++;
    }
    BarList.CalcFactorX(BarNr, ActualBarNr);
}

void OCScore::ReformatPage(const int SysLen)
{
    BarList.RecalcFactorX(SysLen);
}

const QString OCScore::ToolTipText(const int Pointer,const int Stave, const int TrackNr) const
{
    int BarNumber=BarMap().GetBar(Pointer,Stave,TrackNr);
    int BarPtr=BarMap().GetPointer(BarNumber,Stave,TrackNr);
    return "Bar <b>"+QString::number(BarNumber+1)+"</b> Symbol <b>"+QString::number(Pointer-BarPtr+1)+"</b>";
}

void OCScore::findall(const int BarToFind, XMLScoreWrapper& XMLScore, QDomLiteElement* Template)
{
    for (int i=0;i<Template->childCount();i++)
    {
        Staff[XMLScore.AllTemplateIndex(Template,i)]->Findall(BarToFind, XMLScore.Staff(XMLScore.AllTemplateIndex(Template,i)));
    }
    int MS=XMLScore.getVal("MasterStaff");
    if (!XMLScore.StaffOnTemplate(Template,MS)) Staff[MS]->Findall(BarToFind, XMLScore.Staff(MS));
}

void OCScore::Play(const int StartBr, XMLScoreWrapper& XMLScore, const int silence, OCMIDIFile& MFile, const QString& Path, const int StaveNum)
{
    MFile.OpenIt();
    int TrackNumber = 0;

    if (StaveNum==-1)
    {
        for (int Stave = 0 ; Stave < NumOfStaves() ;Stave++)
        {
            QDomLiteElement* s=XMLScore.Staff(Stave);
            if (!s->attributeValue("Muted"))
            {
                Staff[Stave]->Play(MFile, TrackNumber, StartBr, s, silence);
            }
        }
    }
    else
    {
        Staff[StaveNum]->Play(MFile, TrackNumber, StartBr, XMLScore.Staff(StaveNum), silence);
    }
    MFile.CloseIt(Path);
}

const QByteArray OCScore::MIDIPointer(const int StartBr, XMLScoreWrapper& XMLScore, const int silence, OCMIDIFile& MFile)
{
    MFile.OpenIt();
    int TrackNumber = 0;

    for (int Stave = 0 ; Stave < NumOfStaves() ;Stave++)
    {
        QDomLiteElement* s=XMLScore.Staff(Stave);
        if (!s->attributeValue("Muted"))
        {
            Staff[Stave]->Play(MFile, TrackNumber, StartBr, s, silence);
        }
    }
    return MFile.MIDIPointer();
}

void OCScore::Mute(const int Stave, const bool Muted)
{
    Staff[Stave]->Mute = Muted;
}

void OCScore::Solo(const int Stave, const bool Solo)
{
    Staff[Stave]->Solo = Solo;
}

void OCScore::AddStave(const int NewNumber)
{
    CStaff* S=new CStaff;
    Staff.insert(NewNumber,S);
    for (int i=0;i<Staff.count();i++)
    {
        Staff[i]->SetStavenum(i);
    }
}

void OCScore::AddTrack(const int Stave)
{
     Staff[Stave]->AddTrack();
 }

void OCScore::DeleteTrack(const int Stave, const int Track)
{
    Staff[Stave]->DeleteTrack(Track);
}

void OCScore::DeleteStave(const int Stave)
{
    delete Staff[Stave];
    Staff.removeAt(Stave);
    for (int i=0;i<Staff.count();i++)
    {
        Staff[i]->SetStavenum(i);
    }
}

QList<SymbolSearchLocation> OCScore::Search(XMLScoreWrapper& XMLScore, const QString& SearchTerm, const int Stave, const int Track) const
{
    QList<SymbolSearchLocation> l;
    for (int i=0;i<Staff.count();i++)
    {
        if ((Stave==-1) || (i==Stave)) l.append(Staff[i]->Search(XMLScore.Staff(i), SearchTerm, Track));
    }
    return l;
}

void OCScore::FakePlot(const int Stave, const int Track, const int PointerGoal, XMLScoreWrapper& XMLScore, OCMIDIVars& MIDI)
{
    Staff[Stave]->FakePlot(Track, PointerGoal, XMLScore.Staff(Stave), MIDI);
}

void OCScore::CreateBarMap(XMLScoreWrapper& XMLScore)
{
    BarList.BarMap.Voices.clear();
    int StaffOffset = 0;
    for (int iTemp = 0 ; iTemp < XMLScore.NumOfStaffs() ; iTemp++)
    {
        Staff[iTemp]->FillBarsArray(XMLScore.Staff(iTemp), BarList.BarMap, StaffOffset);
        for (int i=0;i<XMLScore.NumOfVoices(iTemp);i++)
        {
            BarList.BarMap.Voices[StaffOffset+i].Staff=iTemp;
            BarList.BarMap.Voices[StaffOffset+i].NumOfVoices=XMLScore.NumOfVoices(iTemp);
            BarList.BarMap.Voices[StaffOffset+i].Name=XMLScore.StaffName(iTemp);
        }
        StaffOffset += XMLScore.NumOfVoices(iTemp);
    }
}

const OCBarMap& OCScore::BarMap() const
{
    return BarList.BarMap;
}

const bool OCScore::StaffEmpty(const int Staff, XMLScoreWrapper &XMLScore, QDomLiteElement *XMLTemplate)
{
    return (BarMap().NoteCountStaff(XMLScore.AllTemplateIndex(XMLTemplate,Staff),BarList.StartBar(),BarList.StartBar()+BarList.ActuallyPrinted())==0);
}

void OCScore::MakeStaves(XMLScoreWrapper& XMLScore)
{
    qDeleteAll(Staff);
    Staff.clear();
    CStaff* S=new CStaff();
    S->SetStavenum(0);
    Staff.append(S);
    for (int iTemp = 1; iTemp < XMLScore.NumOfVoices(0); iTemp++)// ' Datafile.NumOfTracks(1)
    {
        AddTrack(0);
    }
    for (int iTemp = 1; iTemp < XMLScore.NumOfStaffs(); iTemp++)
    {
        AddStave(iTemp);
        for (int iTemp1 = 1; iTemp1 < XMLScore.NumOfVoices(iTemp); iTemp1++) // 'Datafile.NumOfTracks(iTemp)
        {
            AddTrack(iTemp);
        }
    }
}

OCFrameProperties* OCScore::GetFrame(const int Pointer)
{
    return SymbolList.RetrieveFromPointer(Pointer);
}

int OCScore::InsideFrame(const QPoint& m) const
{
    return SymbolList.Inside(m);
}

const QList<int> OCScore::PointersInside(const QRect& r) const
{
    return SymbolList.PointersInside(r);
}
