#include "ocscore.h"
#include "ocsymbolscollection.h"

CVoice::~CVoice() {}

void CVoice::plotMasterStuff(OCPageBarList& BarList, const QColor& VoiceColor, OCDraw& ScreenObj, const XMLScoreWrapper& XMLScore)
{
    OCPrintCounter CountIt;
    getPageStartVars(CountIt);
    double BarX = ScreenObj.spaceX(BarList.barX());
    int py = CountIt.FilePointer;
    forever
    {
        ScreenObj.col = VoiceColor;
        double XFysic = BarX;
        //CountIt.reset();
        while (py < symbolCount())
        {
            ScreenObj.setcol(py);
            const XMLSymbolWrapper& Symbol=XMLSymbol(py, BarList.meter(CountIt.barCount()));
            if (Symbol.IsRestOrValuedNote())
            {
                CountIt.flip(Symbol.ticks());
                XFysic = ScreenObj.spaceX(BarList.calcX(CountIt.barCount(), CountIt.TickCounter)) + BarX;
                if (XFysic > ScreenObj.spaceX(BarList.systemLength())) break;
                CountIt.DecrementFlip();
                //CountIt.flip1(Ticks);
            }
            else if (Symbol.IsTuplet())
            {
                CountIt.OCCounter::tuplets(py,*this);
            }
            else
            {
                OCSymbolsCollection::fib(Symbol,CountIt);
                const OCSymbolLocation& l(OCSymbolLocation(VoiceLocation,py));
                FrameList.AppendGroup(ScreenObj.MakeGroup(OCSymbolsCollection::PlotMTrack(XFysic, Symbol, XMLScore.Template.staff(VoiceLocation.StaffId).height(), CountIt, XMLScore, ScreenObj)),l);
            }
            py++;
            ScreenObj.col = VoiceColor;
            if (CountIt.newBar(BarList.multiMeter(CountIt.barCount()))) break;
        }
        if (CountIt.barCount() >= BarList.barsToPrint()-1) break;
        CountIt.barFlip();
        BarX = XFysic + ScreenObj.spaceX((BarLeftMargin+BarRightMargin));
        if (XFysic > ScreenObj.spaceX(BarList.systemLength())) break;
        BarX += ScreenObj.spaceX(BarList.paddingLeft(CountIt.barCount(), true, true, true));
    }
}

void CVoice::getPageStartVars(OCPrintVarsType &voiceVars) { voiceVars = OCPrintVarsType(pageStartVars); }

void CVoice::setPageStartVars(const OCPrintVarsType &voiceVars) { pageStartVars = OCPrintVarsType(voiceVars); }

const QRectF CVoice::plBarLine(const double xs, const XMLTemplateStaffWrapper& templateStaff, OCDraw& ScreenObj)
{
    const QRectF r = ScreenObj.boundingRect(ScreenObj.line(xs, ScoreStaffHeight, 0, -ScoreStaffLinesHeight));
    if ((templateStaff.squareBracket() == SBBegin) || (templateStaff.curlyBracket() > CBNone))
    {
        ScreenObj.line(0, -(ScreenObj.spaceX(ScoreStaffHeight)-ScoreStaffLinesHeight+(ScreenObj.spaceX(12*templateStaff.height()))));
    }
    return r;
}

void CVoice::plfirstClef(OCPageBarList& BarList, OCDraw& ScreenObj)
{
    if (BarList.BarMap.ClefChange(OCBarLocation(VoiceLocation,BarList.startBar()))) return;
    ScreenObj.moveTo(ScreenObj.spaceX(24), 888);
    CClef::PlClef(pageStartVars.clef(),0,ScreenObj);
}

void CVoice::plfirstKey(OCPageBarList& BarList, OCDraw& ScreenObj)
{
    if (BarList.BarMap.KeyChange(OCBarLocation(VoiceLocation,BarList.startBar()))) return;
    const double BarX = BarLeftMargin + BarList.paddingLeft(0, false, true, false);
    CKey::plotKey(pageStartVars.key(),QPointF(ScreenObj.spaceX(BarX-216),0),pageStartVars.clef(),ScreenObj);
}

void CVoice::formatBar(const OCPageBar& b, OCPageBarList& BarList, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate)
{
    int py=BarList.BarMap.GetPointer(OCBarLocation(VoiceLocation,b.barNumber())).Pointer;
    //OCPrintVarsType dummy = OCPrintVarsType(pageStartVars);
    OCPrintCounter CountIt(b.currentBar);
    getPageStartVars(CountIt);
    while (py < symbolCount())
    {
        const XMLSymbolWrapper& Symbol=XMLSymbol(py, BarList.meter(b.currentBar));
        if (Symbol.IsRestOrValuedNote())
        {
            CountIt.flip(Symbol.ticks());
            BarList.setMinimum(b.currentBar, CountIt.CurrentTicksRounded, CountIt.TickCounter - CountIt.CurrentTicksRounded);
            CountIt.DecrementFlip();
            //CountIt.flip1(Ticks);
        }
        else if (Symbol.IsTuplet())
        {
            CountIt.OCCounter::tuplets(py, XMLScore.Voice(VoiceLocation));
        }
        else
        {
            OCSymbolsCollection::DrawFactor(Symbol, CountIt, XMLTemplate, BarList, XMLScore);
        }
        py++;
        if (CountIt.newBar(BarList.meter(b.currentBar))) break;
    }
    if (b.currentBar == 0)
    {
        if (BarList.keyInBegOfBar(0) < int(CKey::NumOfAccidentals(CountIt.key())))
        {
            if (!BarList.BarMap.KeyChange(OCBarLocation(VoiceLocation,BarList.startBar()))) BarList.setKeyInBegOfBar(0, BarList.keyInBegOfBar(0) + int(CKey::NumOfAccidentals(CountIt.key())));
        }
    }
}

void CVoice::plVoice(OCPageBarList &BarList, const XMLScoreWrapper &XMLScore, OCNoteList &NoteList, OCDraw& ScreenObj, const XMLTemplateStaffWrapper &XMLTemplateStaff, const QColor& VoiceColor)
{
    QColor SignCol(activestaffcolor);
    ScreenObj.col = VoiceColor;
    OCPrintCounter CountIt;
    getPageStartVars(CountIt);
    OCPrintSignList SignsToPrint;
    if (CountIt.FilePointer == 0) CountIt.Meter = BarList.BarMap.GetMeter(OCBarLocation(VoiceLocation,0));
    double BarX = ScreenObj.spaceX(BarList.barX());
    ScreenObj.setcol(QColor(unselectablecolor));
    for (const OCDurSignType& s : std::as_const(CountIt.DurSigns)) OCSymbolsCollection::plotRemaining(s, NoteList, ScreenObj);
    forever
    {
        //CountIt.reset();
        double XFysic = BarX;
        ScreenObj.col = VoiceColor;
        forever
        {
            ScreenObj.setcol(CountIt.FilePointer);
            SignCol = ScreenObj.col;
            if (CountIt.FilePointer >= symbolCount())
            {
                const QRectF r=ScreenObj.boundingRect(ScreenObj.line(XFysic, ScoreStaffHeight, 0, -ScoreStaffLinesHeight));
                BarList.setFrame(CountIt.barCount()+1,r);
                ScreenObj.line(XFysic + ScreenObj.ScreenSize * 2, ScoreStaffHeight,0, -ScoreStaffLinesHeight);
                ScreenObj.col = VoiceColor;
                NoteList.PlotBeams(VoiceColor, ScreenObj);
                return;
            }
            const XMLSymbolWrapper& Symbol=XMLSymbol(CountIt.FilePointer, CountIt.Meter);
            if (Symbol.IsRestOrValuedNote())
            {
                ScreenObj.col = VoiceColor;
                CountIt.flip(Symbol.ticks());
                NoteList.plot(CountIt.RhythmObjectIndex, CountIt, VoiceLocation, BarList, VoiceColor, FrameList, ScreenObj);
                bool tuplettriplet = CountIt.TupletTriplet;
                if (CountIt.tripletFlip(Symbol))
                {
                    ScreenObj.setcol(QColor(unselectablecolor));
                    OCRhythmObjectList l;
                    OCNoteList::PlotTuplet(NoteList.ListFromTo(CountIt.TripletStart,CountIt.RhythmObjectIndex), 3, QPointF(0,0), 0, ScreenObj, int(tuplettriplet) * 96);
                    ScreenObj.col = VoiceColor;
                    CountIt.TripletCount = 0;
                }
                NoteList.plotsigns(CountIt.RhythmObjectIndex, SignsToPrint, FrameList, ScreenObj);
                CountIt.DecrementFlip();
                //CountIt.flip1(Ticks);
            }
            else
            {
                if (Symbol.IsTuplet()) CountIt.OCCounter::tuplets(CountIt.FilePointer, XMLScore.Voice(VoiceLocation));
                const OCBarSymbolLocation l(OCBarSymbolLocation(CountIt.barCount(),VoiceLocation,CountIt.FilePointer));
                FrameList.AppendGroup(ScreenObj.MakeGroup(OCSymbolsCollection::plot(Symbol, XFysic, BarList, CountIt, SignsToPrint, SignCol, XMLScore, NoteList, CountIt, XMLTemplateStaff, ScreenObj)),l);
                OCSymbolsCollection::appendSign(Symbol,SignsToPrint,SignCol,CountIt,OCBarSymbolLocation(CountIt.barCount(),VoiceLocation,CountIt.FilePointer));
                OCSymbolsCollection::fib(Symbol,CountIt);
            }
            ScreenObj.col = VoiceColor;
            SignCol = VoiceColor;
            CountIt.FilePointer++;
            XFysic = ScreenObj.spaceX(BarList.calcX(CountIt.barCount(), CountIt.TickCounter)) + BarX;
            if (CountIt.newBar(CountIt.Meter)) break;
        }
        if (CountIt.barCount() >= BarList.barsToPrint() - 1) break;
        CountIt.barFlip();
        BarX = XFysic + ScreenObj.spaceX(BarLeftMargin+BarRightMargin);
        if (XFysic > ScreenObj.spaceX(BarList.systemLength())) break;
        BarList.setFrame(CountIt.barCount(), plBarLine(BarX - ScreenObj.spaceX(BarLeftMargin + 48), XMLTemplateStaff, ScreenObj));
        BarX += ScreenObj.spaceX(BarList.paddingLeft(CountIt.barCount(), true, true, true));
    }
    BarList.setFrame(CountIt.barCount()+1,plBarLine(ScreenObj.spaceX(BarList.systemLength()), XMLTemplateStaff, ScreenObj));
    //int py = voiceVars.FilePointer;
    forever
    {
        if (CountIt.FilePointer >= symbolCount())
        {
            BarList.setFrame(CountIt.barCount()+1,plBarLine(ScreenObj.spaceX(BarList.systemLength()-(LineHalfThickNess*6)), XMLTemplateStaff, ScreenObj));
            break;
        }
        ScreenObj.setcol(CountIt.FilePointer);
        const XMLSymbolWrapper& Symbol = XMLSymbol(CountIt.FilePointer, CountIt.Meter);
        if (Symbol.IsRestOrValuedNote()) break;
        const OCBarSymbolLocation& l(OCBarSymbolLocation(CountIt.barCount(),VoiceLocation,CountIt.FilePointer));
        FrameList.AppendGroup(ScreenObj.MakeGroup(OCSymbolsCollection::plotSystemEnd(Symbol, 0, BarList, CountIt, SignsToPrint, unselectablecolor, XMLScore, NoteList, CountIt, XMLTemplateStaff, ScreenObj)),l);
        OCSymbolsCollection::fib(Symbol,CountIt);
        ScreenObj.col = VoiceColor;
        CountIt.FilePointer++;
    }
    ScreenObj.col = VoiceColor;
    NoteList.PlotBeams(VoiceColor, ScreenObj);
}

int CVoice::fillChunk(OCNoteList& NoteList, OCStaffCounterPrint& voiceVarsArray, OCStaffAccidentals& StaffAccidentals, OCPageBarList& BarList, const double XFysic, OCDraw& ScreenObj)
{
    OCPrintCounter& CountIt = voiceVarsArray[VoiceLocation.Voice];
    CountIt.unfinish();
    int currentlen = 0;
    while (CountIt.valid(symbolCount()))
    {
        const XMLSymbolWrapper& Symbol=XMLSymbol(CountIt.FilePointer, CountIt.Meter);
        if (Symbol.IsRestOrValuedNote())
        {
            CountIt.flip(Symbol.ticks());
            NoteList.Append(XFysic, Symbol, VoiceLocation, *this, CountIt, StaffAccidentals, BarList, ScreenObj);
            //if (voiceVarsArray.size() > 1) s->forceUpDown(VoiceLocation.Voice);
            currentlen = CountIt.DecrementFlip();
            //CountIt.flip1(Ticks);
            //currentlen = CountIt.CurrentTicksRounded;
            CountIt.FilePointer++;
            break;
        }
        fillBetweenNotes(voiceVarsArray, StaffAccidentals);
    }
    return currentlen;
}

void CVoice::fillBetweenNotes(OCStaffCounterPrint& voiceVarsArray, OCStaffAccidentals& StaffAccidentals)
{
    OCPrintCounter& CountIt = voiceVarsArray[VoiceLocation.Voice];
    while (CountIt.FilePointer < symbolCount())
    {
        const XMLSymbolWrapper& Symbol = XMLSymbol(CountIt.FilePointer, CountIt.Meter);
        if (Symbol.IsRestOrValuedNote()) return;
        if (Symbol.IsTuplet())
        {
            CountIt.tuplets(*this);
        }
        else
        {
            OCSymbolsCollection::fibCommon(Symbol,voiceVarsArray,VoiceLocation);
            if (Symbol.Compare("Key")) StaffAccidentals.SetKeyAccidentals(CountIt.key());
        }
        CountIt.FilePointer++;
    }
}

int CVoice::findBarChunk(OCStaffCounterPrint& voiceVarsArray)
{
    OCPrintCounter& CountIt = voiceVarsArray[VoiceLocation.Voice];
    CountIt.unfinish();
    int currentlen = 0;
    while (CountIt.valid(symbolCount()))
    {
        const XMLSymbolWrapper& Symbol=XMLSymbol(CountIt.FilePointer, CountIt.Meter);
        if (Symbol.IsRestOrValuedNote())
        {
            if (Symbol.IsTiedNote()) CountIt.Ties.append(Symbol.pitch());
            CountIt.TieWrap.eraseTie();
            currentlen = CountIt.flipAllDecrement(Symbol.ticks());
            CountIt.FilePointer++;
            if (Symbol.IsTiedNote()) CountIt.TieWrap.append(Symbol.pitch());
            CountIt.TieWrap.EraseTies = true;
            break;
        }
        findBarBetweenNotes(voiceVarsArray);
    }
    return currentlen;
}

void CVoice::findBarBetweenNotes(OCStaffCounterPrint& voiceVarsArray)
{
    OCPrintCounter& CountIt = voiceVarsArray[VoiceLocation.Voice];
    while (CountIt.FilePointer < symbolCount())
    {
        const XMLSymbolWrapper& Symbol = XMLSymbol(CountIt.FilePointer, CountIt.Meter);
        if (Symbol.IsRestOrValuedNote()) return;
        if (Symbol.IsCompoundNote())
        {
            if (Symbol.IsTiedNote()) CountIt.Ties.append(Symbol.pitch());
            CountIt.TieWrap.eraseTie();
            if (Symbol.IsTiedNote()) CountIt.TieWrap.append(Symbol.pitch());
        }
        else if (Symbol.IsTuplet())
        {
            CountIt.tuplets(*this);
        }
        else
        {
            OCSymbolsCollection::fibCommon(Symbol,voiceVarsArray,VoiceLocation);
        }
        CountIt.FilePointer++;
    }
}

int CVoice::searchChunk(OCBarSymbolLocationList& l, OCStaffCounterPrint& voiceVarsArray, const QString& SearchTerm)
{
    OCPrintCounter& CountIt = voiceVarsArray[VoiceLocation.Voice];
    CountIt.unfinish();
    int currentlen = 0;
    while (CountIt.valid(symbolCount()))
    {
        const XMLSymbolWrapper& Symbol=XMLSymbol(CountIt.FilePointer, CountIt.Meter);
        if (Symbol.IsRestOrValuedNote())
        {
            currentlen = CountIt.flipAllDecrement(Symbol.ticks());
            if (Symbol.Compare(SearchTerm)) l.append(OCBarSymbolLocation(CountIt.barCount(),VoiceLocation,CountIt.FilePointer));
            CountIt.FilePointer++;
            break;
        }
        l.append (searchBetweenNotes(voiceVarsArray, SearchTerm));
    }
    return currentlen;
}

const OCBarSymbolLocationList CVoice::searchBetweenNotes(OCStaffCounterPrint& voiceVarsArray, const QString& SearchTerm)
{
    OCBarSymbolLocationList l;
    OCPrintCounter& CountIt=voiceVarsArray[VoiceLocation.Voice];
    forever
    {
        if (CountIt.FilePointer >= symbolCount()) return l;
        const XMLSymbolWrapper& Symbol = XMLSymbol(CountIt.FilePointer, CountIt.Meter);
        if (Symbol.IsRestOrValuedNote()) return l;
        if (Symbol.Compare(SearchTerm)) l.append(OCBarSymbolLocation(CountIt.barCount(),VoiceLocation,CountIt.FilePointer));
        if (Symbol.IsTuplet())
        {
            CountIt.tuplets(*this);
        }
        else
        {
            OCSymbolsCollection::fibCommon(Symbol,voiceVarsArray,VoiceLocation);
        }
        CountIt.FilePointer++;
    }
}

int CVoice::FakePlotChunk(OCStaffCounterPrint& voiceVarsArray)
{
    OCPrintCounter& CountIt = voiceVarsArray[VoiceLocation.Voice];
    CountIt.unfinish();
    int currentlen = 0;
    while (CountIt.valid(symbolCount()))
    {
        const XMLSymbolWrapper& Symbol=XMLSymbol(CountIt.FilePointer, CountIt.Meter);
        if (Symbol.IsRestOrValuedNote())
        {
            currentlen = CountIt.flipAllDecrement(Symbol.ticks());
            CountIt.FilePointer++;
            break;
        }
        fakePlotBetweenNotes(voiceVarsArray);
    }
    return currentlen;
}

void CVoice::fakePlotBetweenNotes(OCStaffCounterPrint& voiceVarsArray)
{
    OCPrintCounter& CountIt = voiceVarsArray[VoiceLocation.Voice];
    while (CountIt.FilePointer < symbolCount())
    {
        const XMLSymbolWrapper& Symbol=XMLSymbol(CountIt.FilePointer, CountIt.Meter);
        if (Symbol.IsRestOrValuedNote()) return;
        if (Symbol.IsTuplet())
        {
            CountIt.tuplets(*this);
        }
        else
        {
            OCSymbolsCollection::fibCommon(Symbol,voiceVarsArray,VoiceLocation);
        }
        CountIt.FilePointer++;
    }
}

int CVoice::findBarPlayChunk(XMLStaffWrapper& XMLStaff, OCStaffCounterPlay& voiceVarsArray, OCPlaySignList& SignsToPlay, OCMIDIFile& MFile, const int TrackOffset)
{
    OCPlayCounter& CountIt = voiceVarsArray[VoiceLocation.Voice];
    CountIt.unfinish();
    int currentlen = 0;
    while (CountIt.valid(symbolCount()))
    {
        const XMLSymbolWrapper& Symbol=XMLSymbol(CountIt.Pointer, CountIt.PlayMeter);
        if (Symbol.IsRestOrValuedNote())
        {
            CountIt.playFlip(Symbol.ticks());
            if (Symbol.isAudible())
            {
                SignsToPlay.PlayAfterNote(Symbol, CountIt);
                SignsToPlay.decrement(CountIt.CurrentTicks);
                if (CountIt.Accel != 0)
                {
                    CountIt.AccelCounter += CountIt.CurrentTicks;
                }
                else
                {
                    CountIt.AccelCounter = 0;
                }
            }
            currentlen = CountIt.flip1();
            CountIt.Pointer++;
            break;
        }
        findBarPlayBetweenNotes(XMLStaff, voiceVarsArray, MFile, SignsToPlay, TrackOffset);
    }
    return currentlen;
}

void CVoice::findBarPlayBetweenNotes(XMLStaffWrapper& XMLStaff, OCStaffCounterPlay& voiceVarsArray, OCMIDIFile& MFile, OCPlaySignList& SignsToPlay, const int TrackOffset)
{
    OCPlayCounter& CountIt = voiceVarsArray[VoiceLocation.Voice];
    while (CountIt.Pointer < symbolCount())
    {
        const XMLSymbolWrapper& Symbol=XMLSymbol(CountIt.Pointer, CountIt.PlayMeter);
        if (Symbol.IsRestOrValuedNote()) return;
        if (Symbol.IsTuplet())
        {
            if (Symbol.isAudible()) CountIt.tuplets(*this);
        }
        else
        {
            if (Symbol.isAudible()) OCSymbolsCollection::fibPlay(Symbol,MFile,CountIt,CountIt.Pointer,*this,SignsToPlay,CountIt);
            if (VoiceLocation.Voice == 0)
            {
                if (voiceVarsArray.size() > 1)
                {
                    if (Symbol.isCommon())
                    {
                        for (int i = 1; i < voiceVarsArray.size(); i++)
                        {
                            if (Symbol.isAudible())
                            {
                                MFile.setTrackNumber(TrackOffset + i,voiceVarsArray[i].CurrentDelta);
                                OCSymbolsCollection::fibPlay(Symbol,MFile,CountIt,CountIt.Pointer,XMLScoreWrapper::Voice(XMLStaff,i),SignsToPlay,voiceVarsArray[i]);
                            }
                        }
                        MFile.setTrackNumber(TrackOffset,voiceVarsArray[TrackOffset].CurrentDelta);
                    }
                }
            }
        }
        CountIt.Pointer++;
    }
}

void CVoice::playBetweenNotes(XMLStaffWrapper& XMLStaff, OCStaffCounterPlay& voiceVarsArray, OCMIDIFile& MFile, OCPlaySignList& SignsToPlay, const int MIDITrackNumber)
{
    OCPlayCounter& CountIt = voiceVarsArray[VoiceLocation.Voice];
    while (CountIt.Pointer < symbolCount())
    {
        const XMLSymbolWrapper& Symbol=XMLSymbol(CountIt.Pointer, CountIt.PlayMeter);
        if (Symbol.IsRestOrAnyNote()) return;
        if (Symbol.IsTuplet())
        {
            if (Symbol.isAudible()) CountIt.tuplets(*this);
        }
        else
        {
            if (Symbol.isAudible()) OCSymbolsCollection::Play(Symbol,MFile,CountIt,CountIt.Pointer,*this,SignsToPlay,CountIt);
            if (VoiceLocation.Voice == 0)
            {
                if (voiceVarsArray.size() > 1)
                {
                    if (Symbol.isCommon())
                    {
                        for (int i = 1; i < voiceVarsArray.size(); i++)
                        {
                            if (Symbol.isAudible())
                            {
                                MFile.setTrackNumber(MIDITrackNumber + i,voiceVarsArray[i].CurrentDelta);
                                OCSymbolsCollection::Play(Symbol,MFile,CountIt,CountIt.Pointer,XMLScoreWrapper::Voice(XMLStaff,i),SignsToPlay,voiceVarsArray[i]);
                            }
                        }
                        MFile.setTrackNumber(MIDITrackNumber,voiceVarsArray[0].CurrentDelta);
                    }
                }
            }
        }
        CountIt.Pointer++;
    }
}

int CVoice::calcNoteTime(const XMLSymbolWrapper& Symbol, const int Rounded, OCPlayBackVarsType &voiceVars, OCPlaySignList& SignsToPlay) const
{
    if (Symbol.IsSingleTiedNote()) return Rounded;
    int RetVal;
    if (SignsToPlay.exist("Length"))
    {
        RetVal = (Rounded * SignsToPlay.value("Length", "Legato")) / 100;
    }
    else
    {
        const int v = SignsToPlay.value("Legato");
        RetVal = (v > 0) ? (Rounded * v) / 100 : (Rounded * voiceVars.currentlen) / 100;
    }
    if (SignsToPlay.exist("Slur"))
    {
        RetVal = Rounded;
        if (SignsToPlay.value("Length","PerformanceType") > 2)
        {
            RetVal = (Rounded * SignsToPlay.value("Length", "Legato")) / 100;
        }
    }
    return RetVal;
}

void CVoice::playNotesOff(CNotesToPlay& NotesToPlay, int DeltaTime, OCMIDIFile& MFile, OCPlayBackVarsType &currentVars, bool Pedal)
{
    if (!Pedal)
    {
        for (int i = 0; i < NotesToPlay.size(); i++)
        {
            if (NotesToPlay.playEnd(i))
            {
                //qDebug() << "Note off 0" << NotesToPlay.pitch(i);
                MFile.appendNoteOffEvent(currentVars.MIDI.Channel, NotesToPlay.pitch(i), NotesToPlay.velocity(i,currentVars.Currentdynam),DeltaTime);
                currentVars.CurrentDelta -= DeltaTime;
                DeltaTime = 0;
                NotesToPlay.invalidate(i);
            }
        }
        NotesToPlay.cleanUp();
    }
    else
    {
        for (int i = 0; i < NotesToPlay.size(); i++)
        {
            if (NotesToPlay.playEnd(i))
            {
                //qDebug() << "Pedal 0" << NotesToPlay.pitch(i);
                currentVars.PedalNotes.append(NotesToPlay.pitch(i));
                NotesToPlay.invalidate(i);
            }
        }
    }
}

void CVoice::playPortamentoNotes(const int ThisPitch, CNotesToPlay& NotesToPlay, OCPlayBackVarsType &currentVars, OCMIDIFile& MFile)
{
    if (NotesToPlay.isMono())
    {
        if (NotesToPlay.playPortamento(0))
        {
            if (NotesToPlay.monoPitch() != ThisPitch)
            {
                if (!currentVars.PortamentoOn)
                {
                    //qDebug() << "Note off 1" << NotesToPlay.monoPitch();
                    MFile.appendNoteOffEvent(currentVars.MIDI.Channel, NotesToPlay.monoPitch(), NotesToPlay.velocity(0,currentVars.Currentdynam));
                    currentVars.CurrentDelta = 0;
                    NotesToPlay.invalidate(0);
                }
                else
                {
                    //qDebug() << "Portamento 1" << NotesToPlay.monoPitch() << ThisPitch;
                    MFile.appendPortamentoEvent(currentVars.MIDI.Channel, NotesToPlay.monoPitch());
                    currentVars.CurrentDelta = 0;
                    NotesToPlay.invalidate(0);
                    NotesToPlay.PortIt = NotesToPlay.monoPitch();
                }
            }
            else
            {
                NotesToPlay.changeState(0, tSPlayStart);
            }
        }
        NotesToPlay.cleanUp();
    }
}

void CVoice::playExprClear(OCPlayBackVarsType &voiceVars, OCMIDIFile& MFile, OCPlaySignList& SignsToPlay)
{
    if (voiceVars.changeexp) // 'Or (currentVars.ExpressionOn And currentVars.cresc > 0) Then
    {
        voiceVars.changeexp--;
        if (!voiceVars.changeexp)
        {
            MFile.appendExpressionEvent(voiceVars.MIDI.Channel, voiceVars.exprbegin);
            voiceVars.CurrentDelta = 0;
        }
    }
    if (voiceVars.ExpressionOn)
    {
        if (SignsToPlay.exist("Hairpin") || SignsToPlay.exist("DynamicChange"))
        {
            MFile.appendExpressionEvent(voiceVars.MIDI.Channel, voiceVars.exprbegin);
            voiceVars.CurrentDelta = 0;
            voiceVars.changeexp = 0;
        }
    }
}

int CVoice::playChunk(XMLStaffWrapper& XMLStaff, OCStaffCounterPlay& voiceVarsArray, OCPlaySignList& SignsToPlay, CNotesToPlay& NotesToPlay, OCMIDIFile& MFile, int MIDITrackNumber)
{
    OCPlayCounter& CountIt = voiceVarsArray[VoiceLocation.Voice];
    CountIt.unfinish();
    int currentlen = 0;
    while (CountIt.valid(symbolCount()))
    {
        const XMLSymbolWrapper& Symbol = XMLSymbol(CountIt.Pointer, CountIt.PlayMeter);
        //qDebug() << py << Symbol.name() << Symbol.ticks();
        if (Symbol.IsRestOrValuedNote())
        {
            if (Symbol.IsAnyNote())
            {
                const int ThisPitch = CountIt.MIDI.pitch(Symbol.pitch());
                playPortamentoNotes(ThisPitch, NotesToPlay, CountIt, MFile);
                if (!NotesToPlay.containsPitch(ThisPitch))
                {
                    NotesToPlay.append(ThisPitch, !Symbol.isAudible());
                    if (Symbol.IsSingleTiedNote()) NotesToPlay.changeState(NotesToPlay.size()-1, tSPlayStart);
                }
                else
                {
                    if (Symbol.IsSingleNote())
                    {
                        NotesToPlay.changeState(NotesToPlay.findIndex(ThisPitch), tSPlayEnd);
                    }
                    else if (Symbol.IsSingleTiedNote())
                    {
                        NotesToPlay.changeState(NotesToPlay.findIndex(ThisPitch), tSPlayNone);
                    }
                }
                NotesToPlay.sort();
                int NoteOnPitch = NotesToPlay.pitch(NotesToPlay.size()-1);
                int NoteOffPitch = NoteOnPitch;
                int ThisVelocity = CountIt.Currentdynam + int(CountIt.crescendo) + (5 - int(((CountIt.TickCounter / 10) * 8) / CountIt.PlayMeter));
                playExprClear(CountIt, MFile, SignsToPlay);
                SignsToPlay.PlayBeforeNote(Symbol, ThisVelocity, NoteOnPitch, NoteOffPitch, CountIt);
                ThisVelocity = qBound<int>(0, ThisVelocity, 127);

                int VorschlagCount = 0;
                for (const int& p : std::as_const(CountIt.VorschlagNotes)) {
                    MFile.appendNoteOnEvent(CountIt.MIDI.Channel, p, ThisVelocity);
                    CountIt.CurrentDelta = 0;
                    MFile.appendNoteOffEvent(CountIt.MIDI.Channel, p, ThisVelocity, vorschlagLength);
                    VorschlagCount += vorschlagLength;
                }

                playNotesOn(NotesToPlay, MFile, CountIt, ThisVelocity, CountIt.VoicedTime, NoteOnPitch);

                if (NotesToPlay.isMono())
                {
                    /*
                    if ((currentVars.PortamentoOn) && (SignsToPlay.exist("Trill")))
                    {
                        qDebug() << "Portamento 2" << NoteOnPitch;
                        MFile.appendPortamentoEvent(currentVars.MIDI.Channel, NoteOnPitch);
                        currentVars.CurrentDelta = 0;
                    }
                    */
                    if (NotesToPlay.PortIt)
                    {
                        //qDebug() << "Note off 2" << NotesToPlay.PortIt;
                        MFile.appendNoteOffEvent(CountIt.MIDI.Channel, NotesToPlay.PortIt, NotesToPlay.velocity(0,CountIt.Currentdynam));
                        CountIt.CurrentDelta = 0;
                        NotesToPlay.PortIt = 0;
                    }
                }

                CountIt.playFlip(Symbol.ticks());
                const int NoteTime = calcNoteTime(Symbol, CountIt.CurrentTicksRounded, CountIt, SignsToPlay) - VorschlagCount;
                CountIt.VoicedTime = CountIt.CurrentDelta + NoteTime;
                CountIt.CurrentDelta += (CountIt.CurrentTicksRounded - VorschlagCount) + SignsToPlay.value("Fermata", "Duration");
                CountIt.VorschlagNotes.clear();

                playDuringNote(SignsToPlay, Symbol, CountIt.VoicedTime, NoteOnPitch, MFile, CountIt, NoteTime);

                if (NotesToPlay.isMono())
                {
                    if (NotesToPlay.monoPitch() != NoteOffPitch) NotesToPlay.setMonoPitch(NoteOffPitch);
                    if (!NotesToPlay.playEnd(0)) NotesToPlay.changeState(0, tSPlayPortamento);
                }

                if (CountIt.changeexp) if (Symbol.IsSingleTiedNote()) CountIt.changeexp++;

                playNotesOff(NotesToPlay, CountIt.VoicedTime, MFile, CountIt, SignsToPlay.exist("Pedal"));
                playDuringPause(SignsToPlay,NoteOnPitch,MFile,CountIt);
                SignsToPlay.decrement(CountIt.CurrentTicks);
            }
            else
            {
                //'It's a Pause
                CountIt.playFlip(Symbol.ticks());
                CountIt.CurrentDelta += CountIt.CurrentTicksRounded + SignsToPlay.value("Fermata", "Duration");
                playDuringPause(SignsToPlay,0,MFile,CountIt);
                SignsToPlay.decrement(CountIt.CurrentTicks);
            }
            currentlen = CountIt.flip1();
            CountIt.Pointer++;
            break;
        }
        if (Symbol.IsCompoundNote())
        {
            const int ThisPitch = CountIt.MIDI.pitch(Symbol.pitch());
            if (!NotesToPlay.containsPitch(ThisPitch))
            {
                NotesToPlay.append(ThisPitch, !Symbol.isAudible());
                if (Symbol.IsTiedCompoundNote())
                {
                    NotesToPlay.changeState(NotesToPlay.size() - 1, tSPlayStart);
                }
            }
            else
            {
                if (Symbol.IsSingleCompoundNote())
                {
                    NotesToPlay.changeState(NotesToPlay.findIndex(ThisPitch), tSPlayEnd);
                }
                else if (Symbol.IsTiedCompoundNote())
                {
                    NotesToPlay.changeState(NotesToPlay.findIndex(ThisPitch), tSPlayNone);
                }
            }
            CountIt.Pointer++;
        }
        else if (Symbol.IsAnyVorschlag()) {
            CountIt.VorschlagNotes.append(Symbol.pitch());
            CountIt.Pointer++;
        }
        else
        {
            playBetweenNotes(XMLStaff, voiceVarsArray, MFile, SignsToPlay, MIDITrackNumber);
        }
    }
    return currentlen;
}

void CVoice::playNotesOn(CNotesToPlay& NotesToPlay, OCMIDIFile& MFile, OCPlayBackVarsType &voiceVars, const int ThisVelocity, const int VoicedTime, const int NoteOnPitch)
{
    const int Data2 = (VoicedTime == 0) ? ThisVelocity : qBound<int>(0, ThisVelocity - IntDiv(10 * Sgn<int>(NotesToPlay.PortIt), VoicedTime), 127); //Used to be 1000???? Data 2 always 0
    for (int i=0;i<NotesToPlay.size();i++)
    {
        const int Pitch = (i == NotesToPlay.size() - 1) ? NoteOnPitch : NotesToPlay.pitch(i);
        if (NotesToPlay.playStart(i))
        {
            //qDebug() << "Note On" << Pitch;
            MFile.appendNoteOnEvent(voiceVars.MIDI.Channel, Pitch, NotesToPlay.velocity(i,Data2));
            voiceVars.CurrentDelta = 0;
        }
    }
}

void CVoice::playDuringNote(OCPlaySignList& SignsToPlay, const XMLSymbolWrapper& XMLNote, int& VoicedTime, int& NoteOnPitch, OCMIDIFile& MFile, OCPlayBackVarsType &voiceVars, const int NoteTime)
{
    voiceVars.currentcresc = voiceVars.exprbegin;
    voiceVars.express = voiceVars.exprbegin;
    if (SignsToPlay.size())
    {
        const int RemainingTime = SignsToPlay.PlayDuringNote(MFile, NoteOnPitch, VoicedTime-NoteTime, NoteTime, voiceVars);
        SignsToPlay.PlayAfterNote(XMLNote, voiceVars);
        voiceVars.CurrentDelta -= VoicedTime-RemainingTime;
        VoicedTime = RemainingTime;
    }
}

void CVoice::playDuringPause(OCPlaySignList& SignsToPlay, int NoteOnPitch, OCMIDIFile& MFile, OCPlayBackVarsType &voiceVars)
{
    if (SignsToPlay.size()) voiceVars.CurrentDelta = SignsToPlay.PlayDuringNote(MFile, NoteOnPitch, 0, voiceVars.CurrentDelta, voiceVars);
}

CVoice::CVoice(QDomLiteElement *e, const int staff, const int index) : XMLVoiceWrapper(e)
{
    VoiceLocation.StaffId=staff;
    VoiceLocation.Voice=index;
}

int CVoice::getFirstChannel()
{
    int py = 0;
    forever
    {
        if (py >= symbolCount()) return 0;
        const XMLSymbolWrapper& Symbol = XMLSymbol(py, 0);
        if (Symbol.IsRestOrValuedNote()) return 0;
        if (Symbol.Compare("Channel")) return Symbol.getIntVal("Channel") - 1;
        py++;
    }
}

CStaff::~CStaff() {}

void CStaff::formatBar(const OCPageBar& b, OCPageBarList& BarList, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate)
{
    for (CVoice& T : Voices) T.formatBar(b, BarList, XMLScore, XMLTemplate);
}

void CStaff::plotMasterStuff(OCPageBarList& BarList, const QColor& VoiceColor, OCDraw& ScreenObj, const XMLScoreWrapper& XMLScore)
{
    Voices.first().plotMasterStuff(BarList, VoiceColor, ScreenObj, XMLScore);
}

void CStaff::plotStaff(OCPageBarList& BarList, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options, const QColor& color, CStaff& MasterStaff, OCDraw& ScreenObj)
{
    for (int i = 0; i < voiceCount(); i++) FrameList(i).clear();
    const int StaffPos=XMLTemplate.staffPosFromId(StaffLocation.StaffId);
    const XMLTemplateStaffWrapper& XMLTemplateStaff(XMLTemplate.staff(StaffPos));
    OCVoiceLocation ActiveVoice;
    if (ScreenObj.ColorOn) ActiveVoice = OCVoiceLocation(ScreenObj.Cursor->location());
    ScreenObj.col = color;
    ScreenObj.setSpaceX(XMLTemplateStaff.size());
    double holdSize = ScreenObj.ScreenSize;
    ScreenObj.ScreenSize = ScreenObj.spaceX(ScreenObj.ScreenSize);
    //qDebug() << Options.xml()->toString();
    //qDebug() << "Templatestaff size" << XMLTemplateStaff.size() << "Screenobj size" << ScreenObj.ScreenSize << "Options size" << Options.size() << "XMLScore size" << XMLScore.Score.size() << "XMLScore template size" << XMLScore.Template.size();
    BarList.setFrame(0, plotLinesAndBrackets(BarList.systemLength(), XMLTemplate, Options, BarList.startBar(), StaffPos, ScreenObj));
    ScreenObj.setcol(QColor(unselectablecolor));
    Voices.first().plfirstKey(BarList, ScreenObj);
    Voices.first().plfirstClef(BarList, ScreenObj);
    ScreenObj.col=color;
    if (StaffPos == 0)
    {
        QColor MTColor = color;
        if ((!StaffLocation.matches(Options.masterStaff())) && ScreenObj.ColorOn) MTColor = inactivestaffcolor;
        MasterStaff.plotMasterStuff(BarList, MTColor, ScreenObj, XMLScore);
    }
    OCNoteListArray NoteLists = fillNoteLists(BarList, ScreenObj);
    for (int i = 0; i < voiceCount(); i++)
    {
        if (!ActiveVoice.matches(StaffLocation.StaffId,i))
        {
            const QColor col = ScreenObj.ColorOn ? inactivestaffcolor : color;
            Voices[i].plVoice(BarList, XMLScore, NoteLists[i], ScreenObj, XMLTemplateStaff, col);
        }
    }
    if (StaffLocation.matches(ActiveVoice.StaffId))
    {
        Voices[ActiveVoice.Voice].plVoice(BarList, XMLScore, NoteLists[ActiveVoice.Voice], ScreenObj, XMLTemplateStaff, color);
    }
    ScreenObj.ScreenSize = holdSize;
}

const QRectF CStaff::plotLinesAndBrackets(const double SystemLength, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options, const int StartBar, const int StaffPos, OCDraw& ScreenObj)
{
    const XMLTemplateStaffWrapper& TemplateStaff(XMLTemplate.staff(StaffPos));
    ScreenObj.moveTo(0,-ScoreStaffHeight/ScreenObj.XFactor);
    ScreenObj.initCurrent();
    const double syslen = ScreenObj.spaceX(SystemLength);
    ScreenObj.moveTo(syslen, ScoreStaffHeight);
    ScreenObj.line(-syslen, 0);
    ScreenObj.move(0, -96);
    ScreenObj.line(syslen, 0);
    ScreenObj.move(0, -96);
    ScreenObj.line(-syslen, 0);
    ScreenObj.move(0, -96);
    ScreenObj.line(syslen, 0);
    ScreenObj.move(0, -96);
    ScreenObj.line(-syslen, 0);

    const double Height = (StaffPos == XMLTemplate.staffCount()-1) ? ScoreStaffLinesHeight : ScreenObj.spaceX(ScoreStaffHeight + (TemplateStaff.height()*12));
    //if (StaffPos == XMLTemplate.staffCount()-1) Height = ScoreStaffLinesHeight;
    const QRectF r=ScreenObj.boundingRect(ScreenObj.line(0, ScoreStaffHeight, 0, -ScoreStaffLinesHeight));
    ScreenObj.line(0, -(Height-ScoreStaffLinesHeight));
    ScreenObj.move(ScreenObj.spaceX(-24), 0);
    ScreenObj.PlRect(ScreenObj.spaceX(-24),Height);

    if ((StaffPos == 0) && (StartBar > 0))
    {
        if (!Options.hideBarNumbers())
        {
            ScreenObj.moveTo(0, ScoreStaffHeight + ScreenObj.spaceX(216));
            ScreenObj.setcol(QColor(unselectablecolor));
            ScreenObj.plLet(QString::number(1 + StartBar + Options.barNumberOffset()).trimmed(), 0, "Times New Roman", false, false, ScreenObj.spaceX(140));
            ScreenObj.resetcol();
        }
    }

    if (TemplateStaff.squareBracket() == SBEnd)
    {
        ScreenObj.moveTo(ScreenObj.spaceX(-108), ScoreStaffHeight);
        ScreenObj.PlSquare2(ScoreStaffLinesHeight,ScreenObj.spaceX(60),ScreenObj.spaceX(24));
    }
    else if (TemplateStaff.squareBracket() == SBBegin)
    {
        ScreenObj.moveTo(ScreenObj.spaceX(-108),ScoreStaffHeight);
        if (StaffPos > 0)
        {
            if (XMLTemplate.staff(StaffPos - 1).squareBracket() != SBBegin)
            {
                ScreenObj.PlSquare1(Height,ScreenObj.spaceX(60),ScreenObj.spaceX(24));
            }
            else
            {
                ScreenObj.PlSquare1(Height,0,ScreenObj.spaceX(24));
            }
        }
        else
        {
            ScreenObj.PlSquare1(Height,ScreenObj.spaceX(60),ScreenObj.spaceX(24));
        }
    }
    if (TemplateStaff.curlyBracket() > CBNone)
    {
        ScreenObj.moveTo(ScreenObj.spaceX(-108), ScoreStaffHeight);
        if (TemplateStaff.squareBracket()==SBNone) ScreenObj.move(ScreenObj.spaceX(48),0);
        ScreenObj.PlCurly(Height+(ScreenObj.spaceX(ScoreStaffLinesHeight)),ScreenObj.spaceX(12*8),ScreenObj.spaceX(48));
    }
    return r;
}

OCNoteListArray CStaff::fillNoteLists(OCPageBarList& BarList, OCDraw& ScreenObj)
{
    OCNoteListArray NoteLists(voiceCount());
    OCStaffAccidentals StaffAccidentals;
    OCStaffCounterPrint PrintCounters(voiceCount());
    for (int i = 0; i < voiceCount(); i++) Voices[i].getPageStartVars(PrintCounters[i]);
    StaffAccidentals.SetKeyAccidentals(PrintCounters.key());
    double BarX = ScreenObj.spaceX(BarList.barX());
    double XFysic = BarX;
    forever
    {
        StaffAccidentals.Clear();
        for (int i = 0 ; i < voiceCount() ; i++) {
            if (PrintCounters[i].isReady()) {
                PrintCounters[i].setLen(Voices[i].fillChunk(NoteLists[i], PrintCounters, StaffAccidentals, BarList, XFysic, ScreenObj));
            }
        }
        if (PrintCounters.isFinished()) break;
        StaffAccidentals.ProcessAccidentals(PrintCounters[0].Scale);
        for (int i = 0 ; i < voiceCount() ; i++)
        {
            if (PrintCounters[i].Ready)
            {
                if (!PrintCounters[i].isFinished()) {
                    NoteLists[i].ApplyAccidentals(StaffAccidentals);
                }
            }
        }
        checkChordCollision(NoteLists, PrintCounters);
        PrintCounters.flip();
        XFysic = ScreenObj.spaceX(BarList.calcX(PrintCounters.BarCounter, PrintCounters.Beat)) + BarX;
        if (PrintCounters.newBar())
        {
            //PrintCounters.barFlip();
            if (PrintCounters.BarCounter > BarList.barsToPrint() - 1) break;
            StaffAccidentals.SetKeyAccidentals(PrintCounters.key());
            BarX = XFysic + (ScreenObj.spaceX(BarLeftMargin+BarRightMargin));
            if (XFysic > ScreenObj.spaceX(BarList.systemLength())) break;
            BarX += ScreenObj.spaceX(BarList.paddingLeft(PrintCounters.BarCounter, true, true, true));
            XFysic = BarX;
        }
    }
    for (OCNoteList& l : NoteLists) l.CreateBeamLists();
    return NoteLists;
}

void CStaff::findall(const int BarToFind)
{
    OCStaffCounterPrint PrintCounters(voiceCount());
    while (PrintCounters.BarCounter < BarToFind)
    {
        for (int i = 0; i < voiceCount(); i++)
        {
            PrintCounters[i].Ties.clear();
            if (PrintCounters[i].isReady())
            {
                PrintCounters[i].setLen(Voices[i].findBarChunk(PrintCounters));
            }
        }
        if (PrintCounters.isFinished()) break;
        //PrintCounters.flip();
        PrintCounters.decrementFlip();
        if (PrintCounters.newBar())
        {
            //PrintCounters.barFlip();
            if (PrintCounters.BarCounter >= BarToFind) break;
        }
    }
    for (int i = 0; i < voiceCount(); i++) Voices[i].setPageStartVars(PrintCounters[i]);
}

bool CStaff::chordCollision(OCNoteList& NoteList1, OCNoteList& NoteList2) const
{
    const OCIntList LineNums1=NoteList1.FillLineNumsArray();
    const OCIntList LineNums2=NoteList2.FillLineNumsArray();
    for (const int& i : LineNums1)
    {
        for (const int& j : LineNums2)
        {
            if ((i + 1 >= j) && (i - 1 <= j)) return true;
        }
    }
    return false;
}

void CStaff::checkChordCollision(OCNoteListArray& NoteLists, OCStaffCounterPrint& StaffCount)
{
    int AccMoved=0;
    if (voiceCount() < 2) return;
    OCIntList times(voiceCount(),0);
    OCIntList times1(voiceCount(),0);
    for (int i = 0 ; i < voiceCount() ; i++)
    {
        if (StaffCount[i].Ready)
        {
            if (!StaffCount[i].isFinished())
            {
                for (int j = i+1 ; j < voiceCount(); j++)
                {
                    if (StaffCount[j].Ready)
                    {
                        if (!StaffCount[j].isFinished())
                        {
                            if (chordCollision(NoteLists[j], NoteLists[i]))
                            {
                                times[i] ++;
                                times1[j] ++;
                            }
                        }
                    }
                }
            }
        }
    }
    for (int i = 0 ; i < voiceCount() ; i++)
    {
        if (times[i] > 0)
        {
            NoteLists[i].MoveChord(times[i]);
            NoteLists[i].Moveaccidental(times[i], 0);
        }
    }
    for (int i = voiceCount() - 1 ; i >= 0; i--)
    {
        if (times1[i] > 0) AccMoved += NoteLists[i].Moveaccidental(0, AccMoved + 1);
    }
}

CStaff::CStaff(QDomLiteElement *e, const int index) : XMLStaffWrapper(e)
{
    StaffLocation.StaffId=index;
    for (int i = 0; i < e->childCount(); i++)
    {
        Voices.append(CVoice(e->childElement(i),index,i));
    }
}

void CStaff::findBarPlay(const int BarToFind, OCMIDIFile& MFile, const int TrackOffset, OCStaffCounterPlay& PlayCounters, OCPlaySignListArray& SignsToPlay)
{
    for (int i = 0 ; i < voiceCount(); i++)
    {
        MFile.setTrackNumber(i + TrackOffset,0);
        PlayCounters[i].MIDI.Channel = Voices[i].getFirstChannel();
    }
    while (PlayCounters.BarCounter < BarToFind)
    {
        for (int i = 0 ; i < voiceCount(); i++)
        {
            if (PlayCounters[i].isReady())
            {
                MFile.setTrackNumber(i + TrackOffset,0);
                PlayCounters[i].setLen(Voices[i].findBarPlayChunk(*this, PlayCounters, SignsToPlay[i], MFile, TrackOffset));
            }
        }
        if (PlayCounters.isFinished()) break;
        PlayCounters.flip();
        //if (PlayCounters.newBarPlay(voiceVarsArray[StaffCount.firstValidVoice()].PlayMeter))
        if (PlayCounters.newBar()) {
            if (PlayCounters.BarCounter < BarToFind)
            {
                PlayCounters.barFlip();
            }
            else
            {
                break;
            }
        }
    }
    for (int i = 0 ; i < voiceCount(); i++)
    {
        OCPlayBackVarsType& voiceVars = PlayCounters[i];
        MFile.setTrackNumber(i + TrackOffset,0);
        const OCMIDIVars& m = voiceVars.MIDI;
        MFile.appendControllerEvent(m.Channel, 0x79, 0);
        MFile.appendExpressionEvent(m.Channel, expressiondefault);
        MFile.appendControllerEvent(m.Channel, 0x5, 0x10);
        MFile.appendControllerEvent(m.Channel, 0x41, 0);
        MFile.appendControllerEvent(m.Channel, 0x7, 0x7F);
        MFile.appendControllerEvent(m.Channel, 0xA, 64);
        MFile.appendPatchChangeEvent(m.Channel, m.Patch);
        if (voiceVars.AccelCounter > 0)
        {
            if (voiceVars.Accel < 0)
            {
                voiceVars.Playtempo = voiceVars.Playtempo + ((voiceVars.AccelCounter * 10) / ((voiceVars.Playtempo * 8) / -voiceVars.Accel));
            }
            else if (voiceVars.Accel > 0)
            {
                voiceVars.Playtempo = voiceVars.Playtempo - ((voiceVars.AccelCounter * 10) / ((voiceVars.Playtempo * 8) / voiceVars.Accel));
            }
            MFile.appendTempoEvent(voiceVars.Playtempo);
            voiceVars.AccelCounter = 0;
        }
    }
}

void CStaff::play(OCMIDIFile& MFile, int& MIDITrackNumber, const int StartBar, const int silence, const OCBarMap& barmap)
{
    OCPlaySignListArray SignsToPlay(voiceCount());
    OCNotesToPlayArray NotesToPlay(voiceCount());
    OCStaffCounterPlay PlayCounters(voiceCount());
    for (int i = 0 ; i < voiceCount() ; i++) MFile.appendTrack("Staff " + QString::number(StaffLocation.StaffId + 1)+" Voice "+QString::number(i+1));
    findBarPlay(StartBar, MFile, MIDITrackNumber, PlayCounters, SignsToPlay);
    for (OCPlayBackVarsType& v : PlayCounters) v.CurrentDelta = silence;
    //PlayCounters.reset();
    forever
    {
        for (int i = 0 ; i < voiceCount() ; i++)
        {
            if (PlayCounters[i].isReady())
            {
                MFile.setTrackNumber(MIDITrackNumber + i, PlayCounters[i].CurrentDelta);
                if ((MIDITrackNumber + i == 0) && (PlayCounters[i].TickCounter == 0))
                {
                    const int b = barmap.GetBar(OCSymbolLocation(StaffLocation.StaffId,i,PlayCounters[i].Pointer)).Bar;
                    if (b < barmap.EndOfVoiceBar(OCVoiceLocation(StaffLocation.StaffId,i))) MFile.appendMetaEvent(0x07, "Bar " + QString::number(b));
                    PlayCounters[i].CurrentDelta = 0;
                }
                PlayCounters[i].setLen(Voices[i].playChunk(*this, PlayCounters, SignsToPlay[i], NotesToPlay[i], MFile, MIDITrackNumber));
            }
        }
        if (PlayCounters.isFinished()) break;
        PlayCounters.flip();
        if (PlayCounters.newBar()) {
            //if (PlayCounters.newBarPlay(voiceVarsArray[StaffCount.firstValidVoice()].PlayMeter)) {
            PlayCounters.barFlip();
        }
    }
    MIDITrackNumber += voiceCount();
}

const OCBarSymbolLocationList CStaff::search(const QString& SearchTerm, const int TrackToSearch)
{
    OCBarSymbolLocationList l;
    OCStaffCounterPrint PrintCounters(voiceCount());
    forever
    {
        for (int i = 0 ; i < voiceCount() ; i++)
        {
            if (PrintCounters[i].isReady())
            {
                if ((TrackToSearch == -1) || (i == TrackToSearch))
                {
                    PrintCounters[i].setLen(Voices[i].searchChunk(l, PrintCounters, SearchTerm));
                }
                else
                {
                    PrintCounters[i].setLen(Voices[i].FakePlotChunk(PrintCounters));
                }
            }
        }
        if (PrintCounters.isFinished()) break;
        //PrintCounters.flip();
        PrintCounters.decrementFlip();
        PrintCounters.newBar();
        //if (PrintCounters.newBar()) PrintCounters.barFlip();
    }
    return l;
}

const OCPrintVarsType CStaff::fakePlot(const int TrackToSearch, const int TargetIndex)
{
    OCStaffCounterPrint PrintCounters(voiceCount());
    for (int i = 0; i < voiceCount(); i++) Voices[i].getPageStartVars(PrintCounters[i]);
    forever
    {
        for (int i = 0; i < voiceCount(); i++)
        {
            if (PrintCounters[i].isReady())
            {
                PrintCounters[i].setLen(Voices[i].FakePlotChunk(PrintCounters));
            }
            if ((i == TrackToSearch) && (PrintCounters[i].FilePointer >= TargetIndex))
            {
                PrintCounters.quit();
                break;
            }
        }
        if (PrintCounters.isFinished()) break;
        //PrintCounters.flip();
        PrintCounters.decrementFlip();
        //if (PrintCounters.newBar()) PrintCounters.barFlip();
        PrintCounters.newBar();
    }
    return PrintCounters[TrackToSearch];
}

void CStaff::fillBarsArray(OCBarMap& barMap, const int StaffOffset)
{
    OCStaffCounterPrint PrintCounters(voiceCount());
    QVector<OCBarWindowBar> VoiceBar(voiceCount());
    for (int i = 0; i < voiceCount(); i++) barMap.appendVoice(i);
    forever
    {
        for (int i = 0; i < voiceCount(); i++)
        {
            if (PrintCounters[i].isReady())
            {
                OCBarWindowBar& VB = VoiceBar[i];
                OCPrintCounter& CountIt = PrintCounters[i];
                if (!CountIt.isFinished()) VB.Density++;
                CountIt.setLen(Voices[i].findBarChunk(PrintCounters));
                if (PrintCounters.Beat == 0)
                {
                    VB.KeyChangeOnOne = CountIt.KeyChange;
                    VB.ClefChangeOnOne = CountIt.ClefChange;
                }
                VB.MasterStuff = VB.MasterStuff | CountIt.MasterStuff;
                if (CountIt.FilePointer > 0)
                {
                    const XMLSymbolWrapper& Symbol = XMLVoice(i).XMLSymbol(CountIt.FilePointer - 1, CountIt.Meter);
                    if (Symbol.IsValuedNote()) VB.Notes++;
                    if (Symbol.IsRest() && (Symbol.ticks() == CountIt.Meter))
                    {
                        VB.IsFullRest = true;
                        if (VB.Pointer == CountIt.FilePointer - 1) VB.IsFullRestOnly = true;
                    }
                }
                CountIt.Ties.clear();
            }
        }
        if (PrintCounters.isFinished()) break; //if (Ended == NumOfTracks()) break;
        //PrintCounters.flip();
        PrintCounters.decrementFlip();
        if (PrintCounters.newBar())
        {
            //PrintCounters.barFlip();
            for (int i = 0; i < voiceCount(); i++)
            {
                OCBarWindowBar& VB = VoiceBar[i];
                OCPrintCounter& CountIt = PrintCounters[i];
                if (!CountIt.isFinished())
                {
                    VB.setMeter(CountIt.Meter,CountIt.MeterText);
                    barMap.appendBar(StaffOffset + i, OCBarWindowBar(VB));
                    VB.Pointer=CountIt.FilePointer;

                }
                VB.reset();
            }
        }
    }
    for (int i = 0 ; i < voiceCount() ; i++)
    {
        OCBarWindowBar& VB = VoiceBar[i];
        OCPrintCounter& CountIt = PrintCounters[i];
        if (CountIt.FilePointer > VB.Pointer)
        {
            VB.setMeter(CountIt.Meter, CountIt.MeterText);
            barMap.setIncomplete(StaffOffset + i);
            barMap.appendBar(StaffOffset + i, OCBarWindowBar(VB));
        }
        barMap.setEndPointer(StaffOffset + i, CountIt.FilePointer);
    }
}

OCScore::~OCScore()
{
    //qDeleteAll(Staff);
    //Staff.clear();
}

int OCScore::barsActuallyPrinted() const { return BarList.actuallyPrinted(); }

int OCScore::startBar() const { return BarList.startBar(); }

double OCScore::systemLength() const { return BarList.systemLength(); }

bool OCScore::isEnded(const XMLTemplateWrapper &XMLTemplate) const { return BarMap().IsEnded(startBar(),XMLTemplate); }

void OCScore::plotStaff(const int StaffIndex, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options, const QColor& color, OCDraw& ScreenObj)
{
    CStaff& s = Staffs[StaffIndex];
    s.ItemList.clear();
    ScreenObj.StartList();
    s.plotStaff(BarList, XMLScore, XMLTemplate, Options, color, Staffs[Options.masterStaff()], ScreenObj);
    s.ItemList.append(ScreenObj.EndList());
}

void OCScore::plotStaffNoList(const int staffIndex, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options, const QColor& color, OCDraw& ScreenObj)
{
    Staffs[staffIndex].plotStaff(BarList, XMLScore, XMLTemplate, Options, color, Staffs[Options.masterStaff()], ScreenObj);
}

void OCScore::eraseSystem(int StaffIndex, QGraphicsScene *Scene)
{
    CStaff& s = Staffs[StaffIndex];
    for(QGraphicsItem* item : std::as_const(s.ItemList))
    {
        Scene->removeItem(item);
        delete item;
    }
    s.ItemList.clear();
}

void OCScore::eraseAll(QGraphicsScene* Scene)
{
    for(CStaff& s : Staffs) s.ItemList.clear();
    Scene->clear();
}

void OCScore::formatPageBack(const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options)
{
    if (startBar() <= 0) return;
    const int NewEndbar = startBar();
    if (isEnded(XMLTemplate))
    {
        int Start = loBound<int>(0,startBar()-20);
        formatPage(XMLScore,XMLTemplate,Options,systemLength(),Start);
        forever
        {
            if (Start+BarList.actuallyPrinted() >= NewEndbar) return;
            Start++;
            formatPage(XMLScore,XMLTemplate,Options,systemLength(),Start);
        }
    }
    else
    {
        int Start = startBar() - 1;
        formatPage(XMLScore,XMLTemplate,Options,systemLength(),Start);
        forever
        {
            if (Start + BarList.actuallyPrinted() <= NewEndbar) return;
            if (Start <= 0) return;
            Start--;
            formatPage(XMLScore,XMLTemplate,Options,systemLength(),Start);
        }
    }
}

void OCScore::formatPage(const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options, const double SysLen, const int Start, const int End)
{
    bool PreviousFullRest=false;
    bool PreviousFullRestOnly=false;

    findall(Start, XMLTemplate, Options);
    BarList.reset(SysLen,Start,XMLTemplate.staffId(0));
    OCPageBar b(Start);
    while((b.currentBar < 20) || (End == -1))
    {
        if (b.barNumber() > BarMap().barCount(BarList.longestVoice) - 1)
        {
            for (int StaffPos = 0; StaffPos < XMLTemplate.staffCount(); StaffPos++)
            {
                const int StaffId = XMLTemplate.staffId(StaffPos);
                for (int voc = 0; voc < XMLScore.Staff(StaffId).voiceCount(); voc++)
                {
                    if (BarMap().barCount(OCVoiceLocation(StaffId,voc)) - 1 >= b.barNumber())
                    {
                        BarList.longestVoice.StaffId=StaffId;
                        BarList.longestVoice.Voice=voc;
                        break;
                    }
                }
            }
        }
        BarList.setMeter(b.currentBar,BarMap().GetMeter(OCBarLocation(BarList.longestVoice,b.barNumber())));
        bool IsFullRest=false;
        bool IsFullRestOnly=false;
        forever
        {
            for (int StaffPos = 0; StaffPos < XMLTemplate.staffCount(); StaffPos++)
            {
                Staffs[XMLTemplate.staffId(StaffPos)].formatBar(b, BarList, XMLScore, XMLTemplate);
            }
            IsFullRest = BarMap().IsFullRest(b.barNumber(), XMLTemplate);
            IsFullRestOnly = BarMap().IsFullRestOnly(b.barNumber(), XMLTemplate) && !BarMap().MasterStuff(b.barNumber(),Options.masterStaff());
            if (IsFullRestOnly)
            {
                if ((PreviousFullRest && (BarList.multiPause(b.currentBar - 1) == 1)) || PreviousFullRestOnly)
                {
                    BarList.setMultiPause(b.currentBar - 1, BarList.multiPause(b.currentBar - 1) + 1);
                    b.incActual();
                    PreviousFullRest = IsFullRest;
                    PreviousFullRestOnly = IsFullRestOnly;
                }
                else
                {
                    break;
                }
            }
            else if (IsFullRest)
            {
                BarList.setMinimumAll(b.currentBar, 24);
                break;
            }
            else
            {
                break;
            }
        }
        PreviousFullRest = IsFullRest;
        PreviousFullRestOnly = IsFullRestOnly;
        //if (BarList.multiPause(b.currentBar - 1) > 4)
        //{
            //BarList.setMinimumAll(b.currentBar - 1, int(BarList.minimumTotal(b.currentBar - 1) / ((BarList.multiPause(b.currentBar - 1) - 1) / 4) * 0.4) + 2);
        //}
        if (BarList.minimumTotal(b.currentBar) == maxticks)
        {
            BarList.setMinimumAll(b.currentBar, 24);
            break;
        }
        if (End==0)
        {
            if (b.currentBar > 0)
            {
                if (BarList.XTest(b.currentBar+1) < (Options.noteSpace() + 1) * 12) break;
            }
        }
        else if (End != -1)
        {
            if (b.barNumber() >= End) break;
        }
        b.increment();
    }
    if (End == -1)
    {
        BarList.TotalLength();
    }
    else
    {
        BarList.calcFactorX(b);
    }
}

void OCScore::reformatPage(const int SysLen)
{
    BarList.recalcFactorX(SysLen);
}

const QString OCScore::toolTipText(const OCSymbolLocation& SymbolLocation) const
{
    const OCBarSymbolLocation Bar = BarMap().GetBar(SymbolLocation);
    return "Bar <b>"+QString::number(Bar.Bar + 1) + "</b> Symbol <b>" + QString::number(SymbolLocation.Pointer - Bar.Pointer + 1) + "</b>";
}

void OCScore::findall(const int BarToFind, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options)
{
    for (int StaffPos=0; StaffPos<XMLTemplate.staffCount(); StaffPos++)
    {
        Staffs[XMLTemplate.staffId(StaffPos)].findall(BarToFind);
    }
    const int MasterStaff=Options.masterStaff();
    if (!XMLTemplate.containsStaffId(MasterStaff)) Staffs[MasterStaff].findall(BarToFind);
}

OCScore::OCScore() : XMLStaffCollectionWrapper() {}

void OCScore::play(const int StartBr, const int silence, const QString& Path)
{
    OCMIDIFile MFile;
    int TrackNumber = 0;
    for (CStaff& s : Staffs) s.play(MFile, TrackNumber, StartBr, silence, BarList.BarMap);
    MFile.Save(Path);
}

const QByteArray OCScore::MIDIPointer(const int StartBr, const int silence)
{
    OCMIDIFile MFile;
    int TrackNumber = 0;
    for (CStaff& s : Staffs) s.play(MFile, TrackNumber, StartBr, silence, BarList.BarMap);
    return MFile.MIDIPointer();
}

OCBarSymbolLocationList OCScore::search(const QString& SearchTerm, const int StaffIndex, const int Voice)
{
    OCBarSymbolLocationList l;
    for (int i = 0; i < Staffs.size(); i++)
    {
        if ((StaffIndex==-1) || (i==StaffIndex)) l.append(Staffs[i].search(SearchTerm, Voice));
    }
    return l;
}

const OCMIDIVars OCScore::fakePlot(const OCSymbolLocation& Target)
{
    return Staffs[Target.StaffId].fakePlot(Target.Voice, Target.Pointer).MIDI;
}

int OCScore::fakePlotClef(const OCSymbolLocation& Target)
{
    return Staffs[Target.StaffId].fakePlot(Target.Voice, Target.Pointer).clef();
}

void OCScore::createBarMap()
{
    BarList.BarMap.clear();
    int StaffOffset = 0;
    for (int StaffIndex = 0; StaffIndex < Staffs.size(); StaffIndex++)
    {
        Staffs[StaffIndex].fillBarsArray(BarList.BarMap, StaffOffset);
        const XMLStaffWrapper& Staff=XMLStaff(StaffIndex);
        for (int v = 0; v < Staff.voiceCount(); v++)
        {
            BarList.BarMap.setStaffParams(StaffOffset++,StaffIndex,Staff.voiceCount(),Staff.name());
        }
    }
}

const OCBarMap &OCScore::BarMap() const { return BarList.BarMap; }

bool OCScore::StaffEmpty(const int StaffPos, const XMLTemplateWrapper &XMLTemplate)
{
    return (BarMap().NoteCountStaff(XMLTemplate.staffId(StaffPos),BarList.startBar(),BarList.startBar()+BarList.actuallyPrinted())==0);
}

void OCScore::assignXML(const XMLScoreWrapper &XMLScore)
{
    //qDeleteAll(Staff);
    Staffs.clear();
    shadowXML(XMLScore.Score.xml());
    for (int i = 0; i < staffCount(); i++)
    {
        Staffs.append(CStaff(XMLStaff(i).xml(),i));
    }
}

const OCFrameProperties &OCScore::getFrame(const OCSymbolLocation &l) { return Staffs[l.StaffId].FrameList(l.Voice).RetrieveFromPointer(l); }

const OCSymbolLocation OCScore::insideFrame(const QPointF &m)
{
    for (int i=0;i<staffCount();i++)
    {
        for (int j=0;j<Staffs[i].voiceCount();j++)
        {
            const OCSymbolLocation& l=Staffs[i].FrameList(j).Inside(m);
            if (l.Pointer > -1) return l;
        }
    }
    return OCSymbolLocation();
}

const OCSymbolLocation OCScore::nearestLocation(const double y, const OCSymbolLocation &currentLocation)
{
    int p = Staffs[currentLocation.StaffId].FrameList(currentLocation.Voice).Nearest(y);
    return OCSymbolLocation(currentLocation,p);
}

const OCLocationList OCScore::locationsInside(const QRectF &r)
{
    OCLocationList v;
    for (int i=0;i<staffCount();i++)
    {
        for (int j=0;j<Staffs[i].voiceCount();j++)
        {
            v.append(Staffs[i].FrameList(j).locationsInside(r));
        }
    }
    return v;
}

const OCPointerList OCScore::pointersInsideVoice(const QRectF &r, const OCVoiceLocation &v)
{
    return Staffs[v.StaffId].FrameList(v.Voice).locationsInside(r).pointers();
}

const QRectF OCScore::getBarlineX(const int BarNum)
{
    return BarList.frame(BarNum - startBar());
}

int OCScore::insideBarline(const QPointF &x)
{
    for (int i=0;i<BarList.barsToPrint();i++)
    {
        if (BarList.frame(i).x()-3 < x.x())
        {
            if (BarList.frame(i).x()+3 > x.x())
            {
                return i + startBar();
            }
        }
    }
    return -1;
}

OCFrameArray &OCScore::FrameList(const int Staff, const int Voice) { return Staffs[Staff].FrameList(Voice); }


