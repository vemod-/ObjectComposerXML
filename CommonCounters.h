#ifndef COMMONCOUNTERS_H
#define COMMONCOUNTERS_H

#include "ocbarmap.h"

class CFraction {
public:
    int num;  // numerator (täljare)
    int den;  // denominator (nämnare)
    CFraction(int numerator = 0, int denominator = 1) {
        if (denominator == 0) throw std::invalid_argument("Nämnare kan inte vara 0!");
        num = numerator;
        den = denominator;
    }
    void reduce() {
        int g = std::gcd(num, den);
        num /= g;
        den /= g;
        if (den < 0) { num = -num; den = -den; } // Håll nämnaren positiv
    }
    CFraction reduced() const {
        CFraction f(num,den);
        f.reduce();
        return f;
    }
    void reduceWith(int factor) {
        if (factor == 0)
            throw std::invalid_argument("Kan inte reducera med 0.");

        num /= factor;
        den /= factor;
    }
    CFraction reducedWith(int factor) const {
        CFraction f(num/factor,den/factor);
        f.reduce();
        return f;
    }
    CFraction operator+(const CFraction& other) const {
        return CFraction(num * other.den + other.num * den, den * other.den).reduced();
    }
    CFraction operator-(const CFraction& other) const {
        return CFraction(num * other.den - other.num * den, den * other.den).reduced();
    }
    CFraction operator*(const CFraction& other) const {
        return CFraction(num * other.num, den * other.den).reduced();
    }
    CFraction operator/(const CFraction& other) const {
        if (other.num == 0) throw std::invalid_argument("Kan inte dividera med 0!");
        return CFraction(num * other.den, den * other.num).reduced();
    }
    CFraction operator+(int value) const {
        return *this + CFraction(value, 1);
    }
    CFraction operator-(int value) const {
        return *this - CFraction(value, 1);
    }
    CFraction operator*(int value) const {
        return CFraction(num * value, den);
    }
    CFraction operator/(int value) const {
        if (value == 0) throw std::invalid_argument("Division med 0 är inte tillåten.");
        return CFraction(num, den * value);
    }
    bool operator==(const CFraction& other) const {
        return num == other.num && den == other.den;
    }
    bool operator!=(const CFraction& other) const {
        return !(*this == other);
    }
    bool operator<(const CFraction& other) const {
        return num * other.den < other.num * den;
    }
    bool operator<=(const CFraction& other) const {
        return *this < other || *this == other;
    }
    bool operator>(const CFraction& other) const {
        return !(*this <= other);
    }
    bool operator>=(const CFraction& other) const {
        return !(*this < other);
    }
    double value() const {
        return static_cast<double>(num) / den;
    }
    double reciprocalValue() const {
        if (num == 0) throw std::domain_error("Reciprokt värde är inte definierat för 0.");
        return static_cast<double>(den) / num;
    }
    friend std::ostream& operator<<(std::ostream& os, const CFraction& f) {
        return os << f.num << "/" << f.den;
    }
};

/*
class IOCCounter {
public:
    virtual void reset() {}
    virtual void flip(int ticks, double factor) {}
    virtual void beginTuplet(int SymbolIndex, const XMLVoiceWrapper& XMLVoice) {}
    virtual void flip1(int ticks) {}
};
*/

class OCTickCounter {
public:
    OCTickCounter(double resolution = 1) : Resolution(resolution) {}
    int TickCounter = 0;
    int CurrentTicks = 0;
    int CurrentTicksRounded = 0;
    void reset() {
        Check = 0;
        CheckInt = 0;
        TickCounter = 0;
    }
    void flip(int ticks, double factor) {
        CurrentTicks = ticks * Resolution;
        if (!isOne(factor)) {
            Check += (CurrentTicks * factor);
            CurrentTicksRounded = (CheckInt + qRound(Check)) - TickCounter;
            TickCounter = CheckInt + qRound(Check);
        }
        else {
            TickCounter += CurrentTicks;
            CurrentTicksRounded = CurrentTicks;
            CheckInt = TickCounter;
            Check = 0;
        }
    }
private:
    int CheckInt = 0;
    double Check = 0;
    double Resolution = 1;
};

class OCTupletCounter
{
public:
    int NumOfNotes = 0;
    bool ContainsTriplet = false;
    double Factor = 1;
    CFraction Fraction = 1;
    void reset() {
        Factor = 1;
        ContainsTriplet = false;
        CountDown = 0;
        Fraction = 1;
    }
    inline void beginTuplet(int SymbolIndex, const XMLVoiceWrapper& XMLVoice) {
        const XMLSymbolWrapper XMLSymbol = XMLVoice.XMLSymbol(SymbolIndex,0);
        int TicksToCount = XMLSymbol.ticks();
        int ActualTicks = XMLSymbol.getIntVal("TupletValue");
        int NoteCount = 0;
        //'Point to next symbol
        SymbolIndex++;
        int ShortestValue = maxticks;
        while (SymbolIndex < XMLVoice.symbolCount()) {
            const XMLSymbolWrapper Symbol = XMLVoice.XMLSymbol(SymbolIndex,0);
            if (Symbol.IsRestOrValuedNote()) {
                int NoteValue = Symbol.ticks();
                if (Symbol.isTriplet(Symbol.ticks())) ContainsTriplet = true;
                CountDown += NoteValue;
                NoteCount++;
                if (ShortestValue > NoteValue) ShortestValue = NoteValue;
            }
            if (CountDown > TicksToCount) break;
            SymbolIndex ++;
        }
        if (ShortestValue == 0) return;
        if (CountDown == 0) return;
        while (CFraction(CountDown, ShortestValue).reduced().den != 1) {
            if ((ShortestValue == 1) || (ShortestValue == 3)) break;
            ShortestValue /= 2;
        }
        const CFraction f{CountDown,ActualTicks};
        Fraction = f.reducedWith(ShortestValue);
        if (ContainsTriplet) Fraction.reduce();
        Factor = f.reciprocalValue();
        ActualTicks = CountDown;
        NumOfNotes = NoteCount;
    }
    bool isStart() {
        return (CountDown == ActualTicks);
    }
    bool isEnd(int CurrentTicks) {
        return (CountDown <= CurrentTicks);
    }
    void flip1(int ticks) {
        if (CountDown > 0) {
            CountDown -= ticks;
            if (CountDown <= 0) {
                CountDown = 0;
                ContainsTriplet = false;
                Factor = 1;
            }
        }
    }
    inline bool isTuplet() { return !isOne(Factor); }
private:
    int CountDown = 0;
    int ActualTicks = 0;
};

class OCTripletCounter
{
public:
    int FirstNoteIndex = 0;
    bool TupletTriplet = false;
    void reset() {
        Count = 0;
        TupletTriplet = false;
    }
    void beginTuplet(const int TupletValue, const int CurrentIndex) {
        if (isTriplet(TupletValue)) {
            if (Count == 0) FirstNoteIndex = CurrentIndex;
            TupletTriplet = true;
        }
    }
    inline void flip(const OCTickCounter& Counter, const int CurrentIndex) {
        if (TupletTriplet) {
            Count += Counter.CurrentTicksRounded;
        }
        else if (isTriplet(Counter.CurrentTicks)) {
            if (Count == 0) FirstNoteIndex = CurrentIndex;
            Count += Counter.CurrentTicks;
        }
        else {
            if (Count > 0) Count += Counter.CurrentTicks;
        }
    }
    inline void flip1(const bool tupletEnd) {
        if (TupletTriplet) {
            if (isStraight(Count) && tupletEnd) {
                TupletTriplet = false;
                Count = 0;
            }
        }
        else {
            if (isStraight(Count)) Count = 0;
        }
    }
    inline bool isNormalTriplet() {return (Count != 0) && (!TupletTriplet); }
    inline bool isTupletTriplet() {return (Count != 0) && (TupletTriplet); }
    inline bool tripletStart(OCTickCounter& Counter) {
        if (TupletTriplet) return (Counter.CurrentTicksRounded == Count);
        return (Counter.CurrentTicks == Count);
    }
    inline bool tripletEnd(const bool tupletEnd) {
        if (TupletTriplet) {
            return (isStraight(Count) && tupletEnd);
        }
        return isStraight(Count);
    }
    static inline bool isStraight(const int val) {
        switch (val)
        {
        case 3:
        case 6:
        case 12:
        case 24:
        case 48:
        case 96:
        case 192:
            return true;
        }
        return false;
    }
    static inline bool isTriplet(const int val) {
        switch (val)
        {
        case 1:
        case 2:
        case 4:
        case 8:
        case 16:
        case 32:
        case 64:
        case 128:
            return true;
        }
        return false;
    }
private:
    int Count = 0;
};

class OCFragmentCounter
{
public:
    inline void finish() {
        m_Finished = true;
    }
    inline void unfinish() {
        m_Finished = false;
    }
    inline bool isFinished() const {
        return m_Finished;
    }
    inline bool isReady() {
        return wasReady = (CurrentLen == 0);
    }
    inline int getLen() const {
        return CurrentLen;
    }
    inline void setLen(const int ticks) {
        CurrentLen = ticks;
    }
    void flip(double Factor) { TupletFactor = Factor; }
    bool wasReady = false;
    bool m_Finished = false;
    double TupletFactor = 1;
    int CurrentLen = 0;
};

class OCCounter
{
public:
    int CurrentIndex = 0;
    OCTickCounter PlayCounter = OCTickCounter(10);
    OCTickCounter Counter;
    OCTupletCounter TupletCounter;
    OCTripletCounter TripletCounter;
    OCFragmentCounter FragmentCounter;
    inline OCCounter(){}
    inline OCCounter(const int StartBar) {
        BarCounter = StartBar;
    }
    inline void flip(const int ticks) {
        PlayCounter.flip(ticks,TupletCounter.Factor);
        Counter.flip(ticks,TupletCounter.Factor);
        TripletCounter.flip(Counter,CurrentIndex);
        FragmentCounter.flip(TupletCounter.Factor);
    }
    inline int flip1() {
        TripletCounter.flip1(tupletEnd());
        TupletCounter.flip1(Counter.CurrentTicks);
        CurrentIndex++;
        return Counter.CurrentTicksRounded;
    }
    inline int flipAll(const int ticks) {
        flip(ticks);
        return flip1();
    }
    inline void beginTuplet(int SymbolIndex, const XMLVoiceWrapper& XMLVoice) {
        //'Seach for a tuplet group
        const XMLSymbolWrapper XMLSymbol = XMLVoice.XMLSymbol(SymbolIndex,0);
        TupletCounter.beginTuplet(SymbolIndex,XMLVoice);
        TripletCounter.beginTuplet(XMLSymbol.getIntVal("TupletValue"),CurrentIndex);
    }
    inline void reset() {
        PlayCounter.reset();
        Counter.reset();
        TupletCounter.reset();
        TripletCounter.reset();
    }
    inline void barFlip() {
        BarCounter++;
        reset();
    }
    inline bool newBar(const int Meter) { return (Counter.TickCounter >= Meter); }
    inline int barCount() const { return BarCounter; }
    inline bool isFirstBeat() { return (Counter.TickCounter == 0); }
    inline bool isFirstBar() { return (BarCounter == 0); }
    inline bool isFirstBeatOfFirstBar() { return (isFirstBeat() && isFirstBar()); }
    inline bool tripletStart() { return TripletCounter.tripletStart(Counter); }
    inline bool tripletEnd() { return TripletCounter.tripletEnd(tupletEnd()); }
    inline bool tupletStart() { return TupletCounter.isStart(); }
    inline bool tupletEnd() { return TupletCounter.isEnd(Counter.CurrentTicks); }
    inline bool isNormalTriplet() {return TripletCounter.isNormalTriplet(); }
    inline bool isTupletTriplet() {return TripletCounter.isTupletTriplet(); }
    inline bool isTuplet() { return TupletCounter.isTuplet(); }
    static inline bool isDotted(const int val) {
        switch (val)
        {
        case 9:
        case 18:
        case 36:
        case 72:
        case 144:
            return true;
        }
        return false;
    }
    static inline bool isDoubleDotted(const int val) {
        switch (val)
        {
        case 21:
        case 42:
        case 84:
        case 168:
            return true;
        }
        return false;
    }
    static inline bool isStraight(const int val) {
        return OCTripletCounter::isStraight(val);
    }
    static inline bool isTriplet(const int val) {
        return OCTripletCounter::isTriplet(val);
    }
    static int calcTime(const XMLSimpleSymbolWrapper& symbol) {
        switch (symbol.getIntVal("TimeType"))
        {
        case 0:
            return 96 * symbol.getIntVal("Upper") / symbol.getIntVal("Lower");
        case 1:
        case 2:
            return 96;
        }
        return 96;
    }
private:
    int BarCounter = 0;
};

class CPageBarProps
{
public:
    QRectF Frame;
    int Pointer = 0;
    int MultiPause = 0;
    int MultiPauseNumOfEvents = 0;
    int KeyinBegOfBar = 0;
    int ClefInBegOfBar = 0;
    int TimeInBegOfBar = 0;
    bool InvisibleMeter = false;
    inline CPageBarProps(const bool Reset = false) {
        if (Reset) reset();
    }
    inline void SetMeter(const int Meter) { Minimums.fill(maxticks,Meter); }
    inline int GetMeter() const { return Minimums.size(); }
    inline void SetMinimum(const int Value, const int Counter)
    {
        for (int i = Counter; i < Counter+Value; i++)
        {
            if (i >= Minimums.size()) break;
            if (Minimums[i] > Value) Minimums.replace(i,Value);
        }
    }
    inline double CalcX(const int Counter) const
    {
        int ThisBeat=0;
        double BeatCount=0;
        int CurrentMin=12;
        int Tick=0;
        int OldMin;
        if (Counter==0) return 0;
        forever
        {
            if (Tick>=Minimums.size()) break;
            OldMin=CurrentMin;
            CurrentMin=Minimums[Tick];
            if ((OldMin != CurrentMin) && (Tick != 0))
            {
                if (ThisBeat != 0)
                {
                    BeatCount++;
                    if (CurrentMin >= Minimums.size()/2) BeatCount++;
                }
                ThisBeat=0;
            }
            ThisBeat++;
            if (ThisBeat==CurrentMin)
            {
                ThisBeat=0;
                BeatCount++;
                if (CurrentMin >= Minimums.size()/2) BeatCount++;
            }
            Tick++;
            if (Tick == Counter) break;
        }
        if (ThisBeat>0)
        {
            int Min=CurrentMin;
            if (Tick<Minimums.size()) Min=Minimums[Tick];
            BeatCount += (Min==CurrentMin) ? DoubleDiv(ThisBeat,CurrentMin) : 1;
        }
        return BeatCount;
    }
    inline int GetMinimumTotal() const
    {
        return QAverage<int>(Minimums).average();
    }
    inline void SetMinimumAll(const int Value) { Minimums.fill(Value); }
    inline void reset()
    {
        MultiPause = 1;
        MultiPauseNumOfEvents = 1;
        SetMinimumAll(maxticks);
        KeyinBegOfBar = 0;
        ClefInBegOfBar = 0;
        TimeInBegOfBar = 0;
        Pointer=0;
        Frame=QRectF();
    }
    inline int BarSpaceX() const { return int(CalcX(Minimums.size())); }
private:
    OCIntList Minimums;
};

class OCPageBarList
{
public:
    inline OCPageBarList() {}
    OCBarMap BarMap;
    inline CPageBarProps& bar(const int BarNum)
    {
        for (int i = bars.size(); i <= BarNum + 1; i++) bars.append(CPageBarProps(true));
        return bars[BarNum + 1];
    }
    inline void setFrame(const int BarNum, const QRectF& f)
    {
        bar(BarNum).Frame = f;
    }
    inline const QRectF frame(const int BarNum)
    {
        return bar(BarNum).Frame;
    }
    inline void setMeter(const int BarNum, const int new_Meter)
    {
        bar(BarNum).SetMeter(new_Meter);
    }
    inline int meter(const int BarNum)
    {
        return bar(BarNum).GetMeter();
    }
    inline void setMinimum(const int BarNum, const int Value, const int Counter)
    {
        if (LockBar != BarNum)
        {
            bar(BarNum).SetMinimum(Value, Counter);
        }
    }
    inline void setMinimumAll(const int BarNum, const int Value)
    {
        LockBar = BarNum;
        bar(BarNum).SetMinimumAll(Value);
    }
    inline double calcX(const int BarNum, const int Counter)
    {
        return bar(BarNum).CalcX(Counter) * m_FactorX;
    }
    inline int minimumTotal(const int BarNum)
    {
        return bar(BarNum).GetMinimumTotal();
    }
    inline void setMultiPause(const int BarNum, const int new_MultiPause)
    {
        bar(BarNum).MultiPause = new_MultiPause;
    }
    inline int multiPause(const int BarNum)
    {
        return bar(BarNum).MultiPause;
    }
    inline int multiMeter(const int BarNum) {
        return meter(BarNum) * multiPause(BarNum);
    }
    inline void setMultiPauseNumOfEvents(const int BarNum, const int new_MultiPauseNumOfEvents)
    {
        bar(BarNum).MultiPauseNumOfEvents = new_MultiPauseNumOfEvents;
    }
    inline int multiPauseNumOfEvents(const int BarNum)
    {
        return bar(BarNum).MultiPauseNumOfEvents;
    }
    inline void setKeyInBegOfBar(const int BarNum, const int new_KeyInBegOfBar)
    {
        bar(BarNum).KeyinBegOfBar = new_KeyInBegOfBar;
    }
    inline int keyInBegOfBar(const int BarNum)
    {
        return bar(BarNum).KeyinBegOfBar;
    }
    inline void setClefInBegOfBar(const int BarNum, const int new_ClefInBegOfBar)
    {
        bar(BarNum).ClefInBegOfBar = new_ClefInBegOfBar;
    }
    inline int clefInBegOfBar(const int BarNum)
    {
        return bar(BarNum).ClefInBegOfBar;
    }
    inline void setTimeInBegOfBar(const int BarNum, const int new_TimeInBegOfBar)
    {
        bar(BarNum).TimeInBegOfBar = new_TimeInBegOfBar;
    }
    inline int timeInBegOfBar(const int BarNum)
    {
        return bar(BarNum).TimeInBegOfBar;
    }
    inline void setInvisibleMeter(const int BarNum, const bool new_InvisibleMeter)
    {
        bar(BarNum).InvisibleMeter = new_InvisibleMeter;
    }
    inline int invisibleMeter(const int BarNum)
    {
        return bar(BarNum).InvisibleMeter;
    }
    double paddingLeft(const int BarNum, const bool Key, const bool clef, const bool Time)
    {
        double RetVal=0;
        const CPageBarProps& p = bar(BarNum);
        if (Key) if (p.KeyinBegOfBar > 0) RetVal += (p.KeyinBegOfBar * AccidentalSpace) + 36;
        if (clef)
        {
            RetVal += (BarNum == 0) ? p.ClefInBegOfBar * 72 : p.ClefInBegOfBar * 60;
        }
        if (Time) RetVal += p.TimeInBegOfBar * 100;
        return RetVal;
    }
    inline double paddingRight(const bool Key, const bool clef, const bool Time)
    {
        return m_SystemLength - paddingLeft(m_BarsToPrint, Key, clef, Time);
    }
    inline double barX(const int BarNum = 0)
    {
        return BarLeftMargin + paddingLeft(BarNum, true, true, true);
    }
    void reset(const double SysLen, const int Start, const int TopStaff)
    {
        LockBar = -1;
        m_StartBar = Start;
        m_SystemLength = SysLen;
        bars = QVector<CPageBarProps>(1);
        bar(0).ClefInBegOfBar = 5;
        longestVoice.StaffId = TopStaff;
        longestVoice.Voice = 0;
    }
    inline int startBar() const { return m_StartBar; }
    inline double systemLength() const { return m_SystemLength; }
    inline int barsToPrint() const { return m_BarsToPrint; }
    inline int actuallyPrinted() const { return m_ActuallyPrinted; }
    inline void calcFactorX(const OCPageBar& b)
    {
        m_FactorX = XTest(loBound<int>(b.currentBar,1));
        m_BarsToPrint = loBound<int>(b.currentBar, 1);
        m_ActuallyPrinted = loBound<int>(b.actualBar, 1);
    }
    inline void recalcFactorX(const int SysLen)
    {
        m_SystemLength=SysLen;
        m_FactorX = XTest(m_BarsToPrint);
    }
    inline double TotalLength()
    {
        int fMinCount = 0;
        int fBeginSpace = 0;
        for (int i = 0 ; i < bars.size() - 1 ; i++)
        {
            fMinCount += BarSpaceX(i);
            fBeginSpace += paddingLeft(i, true, true, true) + BarLeftMargin + BarRightMargin;
            //qDebug() << iTemp << fMinCount;
        }
        fBeginSpace += paddingLeft(bars.size()-1, true, true, true);
        m_SystemLength = (fMinCount == 0) ? 3600 : 3600 + fBeginSpace + (fMinCount * 360);
        m_FactorX = DoubleDiv(m_SystemLength - fBeginSpace, fMinCount);
        m_BarsToPrint = bars.size()-1;
        return m_SystemLength;
    }
    inline double XTest(const int BarCount)
    {
        int fMinCount = 0;
        int fBeginSpace = 0;
        for (int i = 0 ; i < BarCount ; i++)
        {
            fMinCount += BarSpaceX(i);
            fBeginSpace += paddingLeft(i, true, true, true) + BarLeftMargin + BarRightMargin;
        }
        fBeginSpace += paddingLeft(BarCount, true, true, true);
        if (fMinCount == 0) return 120 * 12;
        return DoubleDiv(m_SystemLength - fBeginSpace, fMinCount);
    }
    OCVoiceLocation longestVoice;
private:
    QVector<CPageBarProps> bars = QVector<CPageBarProps>(1);
    int LockBar = -1;
    int m_StartBar = 0;
    double m_SystemLength = 0;
    double m_FactorX;
    int m_BarsToPrint;
    int m_ActuallyPrinted;
    inline int BarSpaceX(const int BarNum) { return bar(BarNum).BarSpaceX(); }
};

template <typename T>

class CStaffCounter : public QList<T>
{
public:
    int Min = 0;
    int BarCounter = 0;
    int Beat = 0;
    inline CStaffCounter(const int NumOfTracks = 0) { QList<T>::resize(NumOfTracks); }
    inline ~CStaffCounter() {}
    inline void flip() {
        Min = 32000;
        for (const T& c : (*this)){
            const int Len = c.FragmentCounter.getLen();
            if ((Len < Min) && (Len > 0)) Min = Len;
        }
        if (Min == 32000) Min = 0;
        for (T& c : (*this)) {
            int Len = c.FragmentCounter.getLen();
            if (Len > 0) Len -= Min;
            c.FragmentCounter.setLen(Len);
        }
        Beat += Min;
    }
    inline bool newBar(const int Meter) { return (*this)[shortestCount()].OCCounter::newBar(Meter); }
    inline void barFlip() {
        for (T& c : (*this)) c.barFlip();
        BarCounter = (*this)[shortestCount()].barCount();
        Beat = 0;
    }
    inline bool isFinished() const {
        if (didQuit) return true;
        for (const T& c : (*this)) {
            if (!c.FragmentCounter.isFinished()) return false;
        }
        return true;
    }
    inline void quit() { didQuit = true; }
    inline int firstValidVoice() const {
        for (int i = 0 ; i < (*this).size() ; i++) {
            if (!(*this)[i].FragmentCounter.isFinished()) return i;
        }
        return -1;
    }
private:
    bool didQuit = false;
    inline int shortestCount() const
    {
        int RetVal = 0;
        int minTicks = 1280000000;
        for (int i = 0; i < this->size(); i++) {
            const T& c = (*this)[i];
            int ticks = c.barCount() * 500 + c.Counter.TickCounter;
            if ((ticks < minTicks) && (!c.FragmentCounter.isFinished())) {
                minTicks = ticks;
                RetVal = i;
            }
        }
        return RetVal;
    }
};

typedef CStaffCounter<OCCounter> OCStaffCounter;

class OCPrintCounter : public OCCounter, public OCPrintVarsType
{
public:
    OCPrintCounter() {}
    OCPrintCounter(const int StartBar) : OCCounter(StartBar) {}
    void tuplets(const XMLVoiceWrapper& v) {
        OCCounter::beginTuplet(FilePointer,v);
    }
    inline int DecrementFlip() {
        OCPrintVarsType::Decrement(Counter.CurrentTicks);
        return flip1();
    }
    inline bool valid(const int p) {
        if (FilePointer >= p) {
            FragmentCounter.finish();
            return false;
        }
        return true;
    }
};

class OCStaffCounterPrint : public CStaffCounter<OCPrintCounter>
{
public:
    inline OCStaffCounterPrint(const int NumOfTracks = 0) : CStaffCounter<OCPrintCounter>(NumOfTracks) {}
    bool newBar() {
        if (CStaffCounter::newBar(firstValidVoice().Meter)) {
            barFlip();
            return true;
        }
        return false;
    }
    void decrementFlip() {
        CStaffCounter::flip();
        for (int i = 0; i < size(); i++) {
            if (!at(i).FragmentCounter.isFinished()) (*this)[i].Decrement(double(Min) / at(i).FragmentCounter.TupletFactor);
        }
        //decrementVars();
    }
    int key() {
        return firstValidVoice().key();
    }
private:
    const OCPrintCounter& firstValidVoice() const {
        return at(CStaffCounter<OCPrintCounter>::firstValidVoice());
    }
    /*
    void decrementVars() {
        for (int i = 0; i < size(); i++) {
            if (!at(i).isFinished()) (*this)[i].Decrement(double(Min) / (*this)[i].prevFactor);
        }
    }
*/
};

class OCPlayCounter : public OCCounter, public OCPlayBackVarsType
{
public:
    OCPlayCounter() {}
    OCPlayCounter(const int StartBar) : OCCounter(StartBar) {}
    void tuplets(const XMLVoiceWrapper& v) {
        OCCounter::beginTuplet(Pointer,v);
    }
    inline bool valid(const int p) {
        if (Pointer >= p) {
            FragmentCounter.finish();
            return false;
        }
        return true;
    }
};

class OCStaffCounterPlay : public CStaffCounter<OCPlayCounter>
{
public:
    inline OCStaffCounterPlay(const int NumOfTracks = 0) : CStaffCounter<OCPlayCounter>(NumOfTracks) {}
    const OCPlayCounter& firstValidVoice() const {
        return at(CStaffCounter<OCPlayCounter>::firstValidVoice());
    }
    bool newBar() {
        return CStaffCounter::newBar(firstValidVoice().PlayMeter);
    }
};

enum TrackPlayTypes
{
    tsPlayInvalid = 0,
    tSPlayStartEnd = 1,
    tSPlayStart = 2,
    tSPlayEnd = 3,
    tSPlayPortamento = 4,
    tSPlayNone = 5
};

class PlayStates
{
public:
    inline PlayStates(int pitch,bool inaudible) {
        Pitch = pitch;
        Inaudible = inaudible;
    }
    int Pitch;
    TrackPlayTypes State = tSPlayStartEnd;
    bool Inaudible;
};

class CNotesToPlay : private QList<PlayStates>
{
public:
    int PortIt = 0;
    CNotesToPlay() {}// : PortIt(0) { clear(); }
    void append(const int Pitch, const bool Inaudible) {
        QList::append(PlayStates(Pitch,Inaudible));
    }
    inline TrackPlayTypes state(const int Index) const { return at(Index).State; }
    inline bool playStart(const int Index) { return ((at(Index).State==tSPlayStart) || ((*this)[Index].State==tSPlayStartEnd)); }
    inline bool hasEnd() {
        for (const PlayStates& P : std::as_const(*this)) if ((P.State==tSPlayEnd) | (P.State==tSPlayStartEnd)) return true;
        return false;
    }
    inline bool playEnd(const int Index) { return ((at(Index).State==tSPlayEnd) || ((*this)[Index].State==tSPlayStartEnd)); }
    inline bool playPortamento(const int Index) { return (at(Index).State==tSPlayPortamento); }
    inline int pitch(const int Index) const { return at(Index).Pitch; }
    inline int monoPitch() const { return at(0).Pitch; }
    inline void setMonoPitch(const int pitch) { (*this)[0].Pitch = pitch; }
    inline bool inaudible(const int Index) const { return at(Index).Inaudible; }
    inline int velocity(const int index, const int data2) const {
        return (inaudible(index)) ? 0 : qBound<int>(0,data2,127);
    }
    inline void changeState(const int Index, const TrackPlayTypes State) { (*this)[Index].State = State; }
    inline void invalidate(const int Index) { (*this)[Index].State=tsPlayInvalid; }
    inline int size() const { return QList::size(); }
    inline bool isMono() const { return (size() == 1); }
    inline void cleanUp() {
        for (int i = size() - 1; i >= 0; i--) if (at(i).State == tsPlayInvalid) removeAt(i);
    }
    inline bool containsPitch(const int Pitch) const {
        for (const PlayStates& P : (*this)) if (P.Pitch==Pitch) return true;
        return false;
    }
    inline int findState(const int Pitch) const {
        for (const PlayStates& P : (*this)) if (P.Pitch==Pitch) return P.State;
        return 0;
    }
    inline int findIndex(const int Pitch) const {
        for (int i = 0; i < size(); i++) if (at(i).Pitch == Pitch) return i;
        return -1;
    }
    inline void sort()
    {
        if (size() < 2) return;
        for (int i1 = 0; i1 < size(); i1++) {
            for (int i2 = 0; i2 < size(); i2++) {
                if (at(i1).Pitch < at(i2).Pitch) swapItemsAt(i2,i1);
            }
        }
    }
};

typedef QVector<CNotesToPlay> OCNotesToPlayArray;

#endif // COMMONCOUNTERS_H
