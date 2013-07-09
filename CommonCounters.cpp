#include "CommonCounters.h"
#include "CommonClasses.h"

OCCounter::OCCounter()
{
    factor=1;
    Check=0;
    CheckInt=0;
    Counter=0;
    TupletCount=0;
    Rounded=0;
    TupletAntal=0;
    TupletMax=0;
    BarCounter=0;
    CurrentLen=0;
    killed=false;
    Ready=false;
}

void OCCounter::Flip(const int val)
{
    if (factor != 1)
    {
        Check += (val * factor);
        Rounded = (CheckInt + round(Check)) - Counter;
        Counter = CheckInt + round(Check);
    }
    else
    {
        Counter += val;
        Rounded = val;
        CheckInt = Counter;
        Check = 0;
    }
    prevFactor=factor;
}

void OCCounter::Flip1(const int val)
{
    if (TupletCount > 0)
    {
        TupletCount -= val;
        if (TupletCount <0) TupletCount=0;
    }
    else
    {
        factor = 1;
    }
}

void OCCounter::Tuplets(int VoiceIndex, QDomLiteElement* XMLVoice)
{
    //'Seach for a tuplet group
    XMLSymbolWrapper XMLSymbol(XMLVoice->childElement(VoiceIndex),0);
    int TicksToCount=XMLSymbol.ticks();
    int ActualTicks=XMLSymbol.getVal("TupletValue");
    //'Point to next symbol
    VoiceIndex++;
    int Minim = maxticks;
    forever
    {
        XMLSymbolWrapper Symbol(XMLVoice->childElement(VoiceIndex),0);
        if (Symbol.IsEndOfVoice()) break;
        if (Symbol.IsRestOrValuedNote())
        {
            int NoteValue = Symbol.ticks();
            TupletCount += NoteValue;
            if (Minim>NoteValue) Minim=NoteValue;
        }
        if (TupletCount > TicksToCount) break;
        VoiceIndex ++;
    }
    while (FloatDiv(TupletCount ,Minim) != IntDiv(TupletCount ,Minim))
    {
        if ((Minim == 1) || (Minim == 3)) break;
        Minim = FloatDiv(Minim ,2);
    }
    if (TupletCount == 0) return;
    TupletAntal = IntDiv(TupletCount ,Minim);
    factor = FloatDiv(ActualTicks ,TupletCount);
    TupletCount = TicksToCount;
    TupletMax = TupletCount;
}

void OCCounter::reset()
{
    factor = 1;
    Check = 0;
    CheckInt = 0;
    TupletCount = 0;
    Counter = 0;
}

void OCCounter::PlayFlip(const int val)
{
    if (factor != 1)
    {
        Check += (val * factor * 10);
        Rounded = (CheckInt + round(Check)) - Counter;
        Counter = CheckInt + round(Check);
    }
    else
    {
        Counter += (val * 10);
        Rounded = val * 10;
        CheckInt = Counter;
        Check = 0;
    }
}

void OCCounter::BarFlip()
{
    BarCounter++;
    reset();
}

const bool OCCounter::NewBar(const int Meter)
{
    if (Counter >= Meter) return true;
    return false;
}

const bool OCCounter::NewBarPlay(const int Meter)
{
    if (IntDiv(Counter ,10) >= Meter) return true;
    return false;
}

CBarProps::CBarProps()
{
    Meter=0;
    MultiPause=0;
    MultiPauseNumOfEvents=0;
    KeyinBegOfBar=0;
    ClefInBegOfBar=0;
    TimeInBegOfBar=0;
    Pointer=0;
}

void CBarProps::SetMeter(const int new_Meter)
{
    Meter = new_Meter;
    Minimums.clear();
    for (int iTemp = 0;iTemp < Meter;iTemp++) Minimums.append(maxticks);
}

const int CBarProps::GetMeter() const
{
    return Meter;
}

void CBarProps::SetMinimum(const int Value, const int Counter)
{
    for (int iTemp = Counter;iTemp < Counter+Value;iTemp++)
    {
        if (iTemp >= Minimums.count()) break;
        if (Minimums[iTemp] > Value) Minimums.replace(iTemp,Value);
    }
}

const float CBarProps::CalcX(const int Counter) const
{
    //int Total=0;
    int ThisBeat=0;
    float BeatCount=0;
    int CurrentMin=12;
    int Tick=0;
    int OldMin;
    if (Counter==0) return 0;
    forever
    {
        if (Tick>=Minimums.count()) break;
        OldMin=CurrentMin;
        CurrentMin=Minimums[Tick];
        if ((OldMin != CurrentMin) && (Tick != 0))
        {
            if (ThisBeat != 0)
            {
                BeatCount++;
                if (CurrentMin>=Meter/2) BeatCount++;
            }
            ThisBeat=0;
        }
        //Total+=CurrentMin;
        ThisBeat++;
        if (ThisBeat==CurrentMin)
        {
            //Total=0;
            ThisBeat=0;
            BeatCount++;
            if (CurrentMin>=Meter/2) BeatCount++;
        }
        Tick++;
        if (Tick==Counter) break;
    }
    if (ThisBeat>0)
    {
        int Min=CurrentMin;
        if (Tick<Minimums.count())
        {
            Min=Minimums[Tick];
        }
        if (Min==CurrentMin)
        {
            //BeatCount+=FloatDiv(ThisBeat,FloatDiv(Total,ThisBeat));
            BeatCount+=FloatDiv(ThisBeat,CurrentMin);
        }
        else
        {
            BeatCount++;
        }
    }
    return BeatCount;
/*
    int iTemp1=0;
    int lTemp=0;
    float RetVal=0;
    if (Counter == 0) return 0;
    for (int iTemp = 0;iTemp < Counter;iTemp++)
    {
        if (iTemp >= Minimums.count()) break;
        iTemp1++;
        int iTemp2 = 12;
        if (Minimums[iTemp] < maxticks) iTemp2 = Minimums[iTemp];
        lTemp += iTemp2;
        if (iTemp1 >= iTemp2)
        {
            RetVal++;
            iTemp1 = 0;
            lTemp = 0;
        }
    }
    if (lTemp > 0) RetVal += FloatDiv(iTemp1+1 ,FloatDiv(lTemp ,iTemp1));
    return RetVal;
    */
}

const int CBarProps::GetMinimumTotal() const
{
    int lTemp=0;
    for (int iTemp = 0;iTemp<Meter;iTemp++) lTemp += Minimums[iTemp];
    if (Meter == 0) return 0;
    return FloatDiv(lTemp ,Meter);
}

void CBarProps::SetMinimumAll(const int Value)
{
    for (int iTemp = 0;iTemp<Meter;iTemp++)
    {
        if (iTemp >= Minimums.count()) break;
        Minimums.replace(iTemp,Value);
    }
}

void  CBarProps::reset()
{
        MultiPause = 1;
        MultiPauseNumOfEvents = 1;
        SetMinimumAll(maxticks);
        KeyinBegOfBar = 0;
        ClefInBegOfBar = 0;
        TimeInBegOfBar = 0;
        Pointer=0;
}

const int CBarProps::BarSpaceX() const
{
    return CalcX(Meter);
}

OCBarList::OCBarList()
{
    LockBar=-1;
    m_StartBar=0;
    m_SystemLength=0;
}

void OCBarList::SetMeter(const int BarNum, const int new_Meter)
{
    bars[BarNum+1].SetMeter(new_Meter);
}

const int OCBarList::GetMeter(const int BarNum) const
{
    return bars[BarNum+1].GetMeter();
}
/*
const int OCBarList::GetMeter(const int BarNum, const int Staff, const int Voice) const
{
    return BarMap.GetMeter(BarNum+m_StartBar,Staff,Voice);
}
*/
void OCBarList::SetMinimum(const int BarNum, const int Value, const int Counter)
{
    if (LockBar != BarNum) bars[BarNum+1].SetMinimum(Value, Counter);
}

void OCBarList::SetMinimumAll(const int BarNum, const int Value)
{
    LockBar=BarNum;
    bars[BarNum+1].SetMinimumAll(Value);
}

const float OCBarList::CalcX(const int BarNum, const int Counter) const
{
    return bars[BarNum+1].CalcX(Counter) * m_FactorX;
}

const int OCBarList::GetMinimumTotal(const int BarNum) const
{
    return bars[BarNum+1].GetMinimumTotal();
}

void OCBarList::SetMultiPause(const int BarNum, const int new_MultiPause)
{
    bars[BarNum+1].MultiPause = new_MultiPause;
}

const int OCBarList::GetMultiPause(const int BarNum) const
{
    return bars[BarNum+1].MultiPause;
}

void OCBarList::SetMultiPauseNumOfEvents(const int BarNum, const int new_MultiPauseNumOfEvents)
{
    bars[BarNum+1].MultiPauseNumOfEvents = new_MultiPauseNumOfEvents;
}

const int OCBarList::GetMultiPauseNumOfEvents(const int BarNum) const
{
    return bars[BarNum+1].MultiPauseNumOfEvents;
}

void OCBarList::SetKeyInBegOfBar(const int BarNum, const int new_KeyInBegOfBar)
{
    bars[BarNum+1].KeyinBegOfBar = new_KeyInBegOfBar;
}

const int OCBarList::GetKeyInBegOfBar(const int BarNum) const
{
    return bars[BarNum+1].KeyinBegOfBar;
}

void OCBarList::SetClefInBegOfBar(const int BarNum, const int new_ClefInBegOfBar)
{
    bars[BarNum+1].ClefInBegOfBar = new_ClefInBegOfBar;
}

const int OCBarList::GetClefInBegOfBar(const int BarNum) const
{
    return bars[BarNum+1].ClefInBegOfBar;
}

void OCBarList::SetTimeInBegOfBar(const int BarNum, const int new_TimeInBegOfBar)
{
    bars[BarNum+1].TimeInBegOfBar = new_TimeInBegOfBar;
}

const int OCBarList::GetTimeInBegOfBar(const int BarNum) const
{
    return bars[BarNum+1].TimeInBegOfBar;
}

const int OCBarList::BegSpace(const int BarNum, const bool Key, const bool clef, const bool Time) const
{
    int RetVal=0;
    if (Key) RetVal += bars[BarNum+1].KeyinBegOfBar*AccidentalSpace;
    if (clef) RetVal += bars[BarNum+1].ClefInBegOfBar*72;
    if (Time) RetVal += bars[BarNum+1].TimeInBegOfBar*100;
    return RetVal;
}

const int OCBarList::EndSpace(const bool Key, const bool clef, const bool Time) const
{
    return m_SystemLength - BegSpace(m_BarsToPrint, Key, clef, Time);
}

const float OCBarList::BarX(const int BarNum) const
{
    return begofbar + BegSpace(BarNum, true, true, true);
}

const int OCBarList::StartBar() const
{
    return m_StartBar;
}

const int OCBarList::SystemLength() const
{
    return m_SystemLength;
}

const int OCBarList::BarsToPrint() const
{
    return m_BarsToPrint;
}

const int OCBarList::ActuallyPrinted() const
{
    return m_ActuallyPrinted;
}

void OCBarList::CalcFactorX(const int BarCount, const int ActualBars)
{
    m_FactorX = XTest(BarCount);
    m_BarsToPrint = qMax(BarCount, 1);
    m_ActuallyPrinted = ActualBars;
}

void OCBarList::RecalcFactorX(const int SysLen)
{
    m_SystemLength=SysLen;
    m_FactorX = XTest(m_BarsToPrint);
}

const float OCBarList::XTest(const int BarCount) const
{
    float fMinCount = 0;
    float fBeginSpace = 0;
    for (int iTemp = 0 ; iTemp < BarCount ; iTemp++)
    {
        fMinCount += BarSpaceX(iTemp);
        fBeginSpace += BegSpace(iTemp, true, true, true)+begofbar+endofbar;
    }
    fBeginSpace += BegSpace(BarCount, true, true, true);
    if (fMinCount==0) return 120 * 12;
    return (m_SystemLength - fBeginSpace) / fMinCount;
}

void OCBarList::reset(const int SysLen, const int Start, const int TopStaff)
{
    LockBar=-1;
    m_StartBar=Start;
    m_SystemLength=SysLen;
    //SetMeter(-1, BarMap.GetMeter(Start,TopStaff,0));
    for (int iTemp = 1; iTemp < 24; iTemp++)
    {
        //bars[iTemp].SetMeter(bars[iTemp - 1].GetMeter());
        bars[iTemp].reset();
    }
    bars[1].ClefInBegOfBar = 5;// ': If dbr = 1 Then fb(dbr, ClefInBegOfBar) = 5
    longeststaff=TopStaff;
    longestvoice=0;
}

const int OCBarList::BarSpaceX(const int BarNum) const
{
    return bars[BarNum+1].BarSpaceX();
}

void CStaffCounter::InitCounters(const int NumOfTracks)
{
    CountIt.clear();
    for (int i=0;i<NumOfTracks;i++)
    {
        OCCounter C;
        C.reset();
        CountIt.append(C);
    }
}

void CStaffCounter::reset()
{
    for(int i=0;i<CountIt.count();i++) CountIt[i].reset();
}

void CStaffCounter::Flip()
{
    Min = 32000;
    foreach(const OCCounter& c,CountIt)
    {
        if ((c.CurrentLen < Min) && (c.CurrentLen > 0))  Min = c.CurrentLen;
    }
    if (Min == 32000) Min = 0;
    for(int i=0;i<CountIt.count();i++)
    {
        OCCounter& c=CountIt[i];
        if (c.CurrentLen > 0) c.CurrentLen -= Min;
    }
    Beat += Min;
}

const bool CStaffCounter::NewBar(const int Meter)
{
    return CountIt[ShortestCount()].NewBar(Meter);
}

void CStaffCounter::BarFlip()
{
    for(int i=0;i<CountIt.count();i++) CountIt[i].BarFlip();
    BarCounter = CountIt[ShortestCount()].BarCounter;
    Beat = 0;
}

void CStaffCounter::SetCurrentVoice(const int Voice)
{
    currentCounter=Voice;
}

const float CStaffCounter::TupletFactor(const int Voice) const
{
    return CountIt.at(Voice).prevFactor;
}

OCCounter& CStaffCounter::VoiceCounter()
{
    return CountIt[currentCounter];
}

const bool CStaffCounter::Ready(const int Voice)
{
    CountIt[Voice].Ready=(CountIt.at(Voice).CurrentLen == 0);
    return CountIt.at(Voice).Ready;
}

const bool CStaffCounter::WasReady(const int Voice) const
{
    return CountIt.at(Voice).Ready;
}

const bool CStaffCounter::Killed(const int Voice) const
{
    return CountIt.at(Voice).killed;
}

const bool CStaffCounter::Killed() const
{
    if (didQuit) return true;
    for (int iTemp=0;iTemp<CountIt.count();iTemp++)
    {
        if (!CountIt.at(iTemp).killed) return false;
    }
    return true;
}

void CStaffCounter::Quit()
{
    didQuit=true;
}

const int CStaffCounter::FirstValidVoice() const
{
    for (int iTemp = 0 ; iTemp < CountIt.count() ; iTemp++)
    {
        if (!CountIt.at(iTemp).killed) return iTemp;
    }
    return -1;
}

void CStaffCounter::SetNewLen(const int Value, const int Voice)
{
    CountIt[Voice].CurrentLen = Value;
}

const int CStaffCounter::ShortestCount() const
{
    int RetVal = 0;
    int TempCount = 1280000000;
    for (int iTemp=0;iTemp<CountIt.count();iTemp++)
    {
        const OCCounter& c=CountIt[iTemp];
        if (((c.BarCounter * 500) + c.Counter < TempCount) && (!c.killed))
        {
            RetVal = iTemp;
            TempCount = (c.BarCounter * 500) + c.Counter;
        }
    }
    return RetVal;
}

const bool CStaffCounter::NewBarPlay(const int Meter)
{
    return CountIt[ShortestCount()].NewBarPlay(Meter);
}

CStaffCounter::CStaffCounter(const int NumOfTracks)
{
    Min=0;
    BarCounter=0;
    Beat=0;
    didQuit=false;
    InitCounters(NumOfTracks);
}

CStaffCounter::~CStaffCounter()
{
    CountIt.clear();
}

void CNotesToPlay::Append(const int Pitch, const bool Inaudible)
{
    PlayStates P={Pitch,1,Inaudible};
    PlayStateList.append(P);
}

void CNotesToPlay::clear()
{
    PlayStateList.clear();
}

void CNotesToPlay::Delete(const int Index)
{
    PlayStateList.removeAt(Index);
}

const int CNotesToPlay::State(const int Index) const
{
    return PlayStateList.at(Index).State;
}

const int CNotesToPlay::Pitch(const int Index) const
{
    return PlayStateList.at(Index).Pitch;
}

const bool CNotesToPlay::Inaudible(const int Index) const
{
    return PlayStateList.at(Index).Inaudible;
}

void CNotesToPlay::ChangeState(const int Index, const int State)
{
    PlayStateList[Index].State = State;
}

const int CNotesToPlay::Count() const
{
    return PlayStateList.count();
}

void CNotesToPlay::CleanUp()
{
    for (int i=PlayStateList.count()-1;i>=0;i--)
    {
        if (PlayStateList.at(i).State<1) PlayStateList.removeAt(i);
    }
}

const bool CNotesToPlay::PitchExists(const int Pitch) const
{
    foreach(const PlayStates& P,PlayStateList) if (P.Pitch==Pitch) return true;
    return false;
}

const int CNotesToPlay::FindState(const int Pitch) const
{
    foreach(const PlayStates& P,PlayStateList) if (P.Pitch==Pitch) return P.State;
    return 0;
}

CNotesToPlay::CNotesToPlay()
{
    PortIt=0;
    clear();
}

const int CNotesToPlay::FindIndex(const int Pitch) const
{
    for (int i=0;i<PlayStateList.count();i++)
    {
        if (PlayStateList.at(i).Pitch==Pitch) return i;
    }
    return -1;
}

void CNotesToPlay::Sort()
{
    for (int iTemp1=0;iTemp1<PlayStateList.count();iTemp1++)
    {
        for (int iTemp=0;iTemp<PlayStateList.count();iTemp++)
        {
            if (PlayStateList.at(iTemp1).Pitch < PlayStateList.at(iTemp).Pitch) PlayStateList.swap(iTemp,iTemp1);
        }
    }
}
