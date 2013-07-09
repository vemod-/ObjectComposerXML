#ifndef COMMONCOUNTERS_H
#define COMMONCOUNTERS_H

#include "CommonClasses.h"
#include <QDomLite>
#include <QList>

class OCCounter
{
public:
    float factor;
    float prevFactor;
    float Check;
    int CheckInt;
    int Counter;
    int TupletCount;
    int Rounded;
    int TupletAntal;
    int TupletMax;
    int BarCounter;
    int CurrentLen;
    bool killed;
    bool Ready;
    OCCounter();
    void Flip(const int val);
    void Flip1(const int val);
    void Tuplets(int VoiceIndex, QDomLiteElement* XMLVoice);
    void reset();
    void PlayFlip(const int val);
    void BarFlip();
    const bool NewBar(const int Meter);
    const bool NewBarPlay(const int Meter);
};

class CBarProps
{
private:
    QList<int> Minimums;
    int Meter;
public:
    int Pointer;
    //int ActualBarNumber;
    int MultiPause;
    int MultiPauseNumOfEvents;
    int KeyinBegOfBar;
    int ClefInBegOfBar;
    int TimeInBegOfBar;
    CBarProps();
    void SetMeter(const int newMeter);
    const int GetMeter() const;
    void SetMinimum(const int Value, const int Counter);
    const float CalcX(const int Counter) const;
    const int GetMinimumTotal() const;
    void SetMinimumAll(const int Value);
    void reset();
    const int BarSpaceX() const;
};

class OCBarList
{
private:
    CBarProps bars[24];
    int begmeter;
    int LockBar;
    int m_StartBar;
    //int m_EndBar;
    int m_SystemLength;
    float m_FactorX;
    int m_BarsToPrint;
    int m_ActuallyPrinted;
    const int BarSpaceX(const int BarNum) const;
public:
    OCBarList();
    OCBarMap BarMap;
    void SetMeter(const int BarNum, const int new_Meter);
    const int GetMeter(const int BarNum) const;
    //const int GetMeter(const int BarNum, const int Staff, const int Voice) const;
    void SetMinimum(const int BarNum, const int Value, const int Counter);
    void SetMinimumAll(const int BarNum, const int Value);
    const float CalcX(const int BarNum, const int Counter) const;
    const int GetMinimumTotal(const int BarNum) const;
    void SetMultiPause(const int BarNum, const int new_MultiPause);
    const int GetMultiPause(const int BarNum) const;
    void SetMultiPauseNumOfEvents(const int BarNum, const int new_MultiPauseNumOfEvents);
    const int GetMultiPauseNumOfEvents(const int BarNum) const;
    void SetKeyInBegOfBar(const int BarNum, const int new_KeyInBegOfBar);
    const int GetKeyInBegOfBar(const int BarNum) const;
    void SetClefInBegOfBar(const int BarNum, const int new_ClefInBegOfBar);
    const int GetClefInBegOfBar(const int BarNum) const;
    void SetTimeInBegOfBar(const int BarNum, const int new_TimeInBegOfBar);
    const int GetTimeInBegOfBar(const int BarNum) const;
    const int BegSpace(const int BarNum, const bool Key, const bool clef, const bool Time) const;
    const int EndSpace(const bool Key, const bool clef, const bool Time) const;
    const float BarX(const int BarNum=0) const;
    void reset(const int SysLen, const int Start, const int TopStaff);
    const int StartBar() const;
    const int SystemLength() const;
    const int BarsToPrint() const;
    const int ActuallyPrinted() const;
    void CalcFactorX(const int BarCount, const int ActualBars);
    void RecalcFactorX(const int SysLen);
    const float XTest(const int BarCount) const;
    int longeststaff;
    int longestvoice;
};

class CStaffCounter
{
private:
    QList<OCCounter> CountIt;
    const int ShortestCount() const;
    bool didQuit;
    int currentCounter;
    void InitCounters(const int NumOfTracks);
public:
    int Min;
    int BarCounter;
    OCCounter& VoiceCounter();
    int Beat;
    CStaffCounter(const int NumOfTracks);
    ~CStaffCounter();
    void reset();
    void Flip();
    const bool NewBar(const int Meter);
    void BarFlip();
    void SetCurrentVoice(const int Voice);
    const float TupletFactor(const int Voice) const;
    const bool Ready(const int Voice);
    const bool WasReady(const int Voice) const;
    const bool Killed(const int Voice) const;
    const bool Killed() const;
    void Quit();
    const int FirstValidVoice() const;
    void SetNewLen(const int Value, const int Voice);
    const bool NewBarPlay(const int Meter);
};

struct PlayStates
{
    int Pitch;
    int State;
    bool Inaudible;
};

class CNotesToPlay
{
private:
    QList<PlayStates> PlayStateList;
public:
    int PortIt;
    CNotesToPlay();
    void Append(const int Pitch, const bool Inaudible);
    void clear();
    void Delete(const int Index);
    const int State(const int Index) const;
    const int Pitch(const int Index) const;
    const bool Inaudible(const int Index) const;
    void ChangeState(const int Index, const int State);
    const int Count() const;
    void CleanUp();
    const bool PitchExists(const int Pitch) const;
    const int FindState(const int Pitch) const;
    const int FindIndex(const int Pitch) const;
    void Sort();
};

#endif // COMMONCOUNTERS_H
