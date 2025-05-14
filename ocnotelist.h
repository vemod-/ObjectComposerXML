#ifndef OCNOTELIST_H
#define OCNOTELIST_H

#include "CommonClasses.h"
#include "ocsignlist.h"

class OCStaffAccidentalArray
{
public:
    inline OCStaffAccidentalArray(){}
    inline void setLine(const uint Line) { Lines[Line].isSet=true; }
    inline bool isSet(const uint Line) const { return Lines[Line].isSet; }
    inline void setAllOctaves(const uint Line) { for (unsigned int i = Line; i < 128; i += 7) setLine(i); }
    inline void reset() { for (acctype& a : Lines) a.reset(); }
    inline void setCurrent(const uint Line,const OCNoteAccidentalTypes Sign) { Lines[Line].current=Sign; }
    inline OCNoteAccidentalTypes current(const uint Line) const { return Lines[Line].current; }
private:
    acctype Lines[128];
};

class OCOneOctaveAccidentalArray
{
public:
    inline OCOneOctaveAccidentalArray(){}
    inline bool isSet(const uint Line) const { return Lines[oneOctaveLineNum(Line)].isSet; }
    inline void reset() { for (acctype& a : Lines) a.reset(); }
    inline void setCurrent(const uint Line,const OCNoteAccidentalTypes Sign)
    {
        acctype& item=Lines[oneOctaveLineNum(Line)];
        item.current=Sign;
        item.isSet=true;
    }
    inline OCNoteAccidentalTypes current(const uint Line) const { return Lines[oneOctaveLineNum(Line)].current; }
private:
    inline unsigned int oneOctaveLineNum(const uint Line) const { return Line % 7; }
    acctype Lines[7];
};

class OCOneOctaveFlagsArray
{
public:
    inline OCOneOctaveFlagsArray(){ reset(); }
    void reset() { for (bool& l : Lines) l=false; }
    inline bool isSet(const uint Line) const { return Lines[oneOctaveLineNum(Line)]; }
    inline void set(const  uint Line) { Lines[oneOctaveLineNum(Line)]=true; }
    inline void unset(const uint Line) { Lines[oneOctaveLineNum(Line)]=false; }
private:
    inline uint oneOctaveLineNum(const uint Line) const { return Line % 7; }
    bool Lines[7];
};

class OCStaffAccidentals
{
private:
    OCStaffAccidentalArray StaffAccidentals;
    OCOneOctaveAccidentalArray OneOct;
    QList<OCNoteAccidental> NoteItems;
    OCIntList IgnorePitches;
    void setNoteAccidental(OCOneOctaveFlagsArray& WasHere, OCNoteAccidental& item, const OCNoteAccidentalTypes AccSign);
    void setCurrent(OCNoteAccidental& item, const OCNoteAccidentalTypes AccSign);
    void Process(OCOneOctaveFlagsArray& WasHere, const OCScaleArray& Scale, const OCIntList& IgnoreLines);
public:
    void SetKeyAccidentals(const int Key);
    void ProcessAccidentals(const OCScaleArray& Scale);
    OCAccidentalSymbols PopNoteAccidental();
    void PushNote(const int LineNum, const int NoteNum);
    void AddIgnore(const OCIntList& Pitches);
    void Clear();
};

inline double FnSize(const double c, const int SymbolSize)
{
    if (SymbolSize == 0) return c;
    return DoubleDiv(c ,SizeFactor(SymbolSize));
}

class OCRhythmObject
{
public:
    bool Invisible = false;
    bool InvisibleMeter = false;
    int Size = 0;
    int BalkBeginY = 0;
    int NoteValue = 0;
    int BalkX = 0;
    StemDirection UpDown = StemAuto;
    int AverageY = 0;
    int BalkEndY = 0;
    int Rounded = 0;
    int Beat = 0;
    int Bar = 0;
    int Meter = 0;
    int NumOfBeams = 0;
    int AutoBeamIndex = 0;
    bool IsPause = false;
    StemDirection ForceUpDown = StemAuto;
    int ForceBeamIndex = 0;
    int NumOfForcedBeams = 0;
    int ForceSlant = 0;
    int HighestY = 0;
    int LowestY = 0;
    int BeamLimit = 0;
    int CenterX = 0;
    int CenterNextXAdd = 0;
    bool IsSingleNoteWithTie = false;
    bool FlipTie = false;
    int PerformanceSign = 0;
    QPointF PerformanceSignPos = QPointF(0,0);
    int PerformanceSignSize = 0;
    inline OCRhythmObject() {}
    const QPointF BalkEnd() const {
        return QPointF(BalkX,BalkEndY);
    }
    const QPointF BalkBegin() const {
        return QPointF(BalkX,BalkBeginY);
    }
    const QPointF Center() const {
        return QPointF(CenterX,BalkBeginY);
    }
    void fillProps(const XMLSymbolWrapper& Symbol,const OCPrintCounter& CountIt)
    {
        Size = Symbol.size();
        NoteValue = Symbol.ticks();
        Rounded = CountIt.CurrentTicksRounded;
        Beat = CountIt.TickCounter;
        Bar = CountIt.barCount();
        Meter = CountIt.Meter;
        FlipTie = CountIt.FlipTie;
        NumOfBeams = NumOfFlags(NoteValue);
    }
    void PlotStem(OCDraw& ScreenObj) const
    {
        const int SgnThickness = directed(LineHalfThickNess * 2);
        const int SgnHalfThickness = directed(LineHalfThickNess);
        if ((NoteValue != 144) && (NoteValue != 96) && (NoteValue != 64))
        {
            ScreenObj.line(BalkX, BalkBeginY - SgnThickness, 0,(BalkEndY + SgnHalfThickness) - (BalkBeginY - SgnThickness));
        }
        ScreenObj.moveTo(BalkX, BalkEndY + SgnHalfThickness); //' Size
    }
    inline int directed(const int v) const { return UpDown * v; }
    inline const QPointF directed(const QPointF& p, const int v) {
        return QPointF(p.x(),p.y() - directed(v));
    }
    inline bool isBeamable() const { return (NumOfFlags(NoteValue) > 0); }
    bool shouldEndBeam(const int BalkLimit) const {
        return !isBeamable() ||
               Rounded > BalkLimit ||
               ForceBeamIndex == 1 ||
               (IsPause && ForceBeamIndex == 0);
    }
    bool shouldStartBeam(const OCRhythmObject* next, const int BalkLimit) const {
        if (!isBeamable() || Rounded >= Meter || IsPause || ForceBeamIndex > 0) return false;
        if (!next || !next->isBeamable() || next->IsPause || next->ForceBeamIndex > 0) return false;
        if (next->Bar > Bar) return false;
        if (Rounded + next->Rounded > BalkLimit) return false;
        return true;
    }
    inline int NumOfFlags(const int Ticks) const
    {
        switch (Ticks)
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
        case 21:
            return 1;
        }
        return 0;
    }
    inline void SetUpDownProps()
    {
        if (ForceUpDown) {
            UpDown = ForceUpDown;
        }
        else {
            UpDown = (AverageY > (86 * 12)) ? StemUp : StemDown;
        }
    }
    int SlurY(const int UD) const
    {
        int RetVal;
        if (UD == -1) {
            RetVal = (UpDown == StemUp) ? BalkEndY : BalkBeginY;
        }
        else {
            RetVal = (UpDown == StemUp) ? BalkBeginY : BalkEndY;
        }
        return RetVal + (UD * 24);
    }
    void forceUpDown(const int Voice) {
        StemDirection UpDown = StemAuto;
        UpDown = (Voice % 2 == 1) ? StemUp : StemDown;
        if (ForceUpDown == StemAuto) ForceUpDown = UpDown;
    }
};

class CNoteHead
{
public:
    bool Invisible;
    OCSymbolLocation Location;
    NoteTypeCode NoteType;
    int PitchCode;
    int NoteVal;
    int Left;
    int AccidentalLeft = 0;
    int TieTop;
    int Size;
    int Marked;
    int CenterX = 0;
    int CenterY = 0;
    int FortegnAddX = 0;
    int LineNum = 0;
    int NoteHeadType = 0;
    OCAccidentalSymbols AccidentalSymbol = accNone;
    int AccidentalType = 0;
    bool AccidentalParentheses;
    inline CNoteHead(const XMLSymbolWrapper &Symbol, const OCSymbolLocation& L, const int M)
    {
        Marked=M;
        Location=L;
        Invisible=!Symbol.isVisible();
        NoteType = NoteTypeCode(Symbol.getIntVal("NoteType"));
        PitchCode = Symbol.pitch();
        NoteHeadType = Symbol.getIntVal("NoteHeadType");
        NoteVal = Symbol.ticks();
        Left = int(Symbol.pos().x());
        AccidentalLeft = Symbol.getIntVal("AccidentalLeft");
        AccidentalType = Symbol.getIntVal("AccidentalType");
        AccidentalParentheses = Symbol.getBoolVal("AccidentalParentheses");
        TieTop = Symbol.getIntVal("TieTop");
        Size = Symbol.size();
    }
    inline void moveTo(OCDraw& ScreenObj) const { ScreenObj.moveTo(CenterX, CenterY); }
    void plot(const bool UnderTriplet, const QColor& TrackColor, const int UpDown, OCFrameArray& FrameList, const OCGraphicsList& tie, OCDraw& ScreenObj);
    double sized(const double val) {
        if (Size == 0) return val;
        return DoubleDiv(val ,SizeFactor(Size));
    }
};

class IOCRhythmObject : public OCRhythmObject
{
protected:
    inline int stemX(const int X) const { return X - directed(53); }
public:
    //OCRhythmObject Props;
    inline IOCRhythmObject(){}
    virtual ~IOCRhythmObject();
    virtual int Count() const {return 0;}
    virtual void Fill(const OCVoiceLocation& /*VoiceLocation*/, const XMLVoiceWrapper& /*XMLVoice*/, const double /*XFysic*/, OCPrintCounter& /*CountIt*/, OCStaffAccidentals& /*StaffAccidentals*/, const XMLSymbolWrapper& /*Symbol*/, OCPageBarList& /*BarList*/, OCDraw& /*ScreenObj*/){}
    virtual void CalcUpDownProps(){}
    virtual void plot(OCPrintCounter& /*CountIt*/, const int /*NextHeight*/, const double /*NextX*/, const int /*BarsToPrint*/, const QColor& /*TrackColor*/, const OCVoiceLocation& /*VoiceLocation*/, OCFrameArray& /*FrameList*/, OCDraw& /*ScreenObj*/){}
    virtual int BeamHeight() const {
        int c;
        if (UpDown == StemUp)
        {
            c = LowestY - (28 * 12);
            if (c > (94 * 12)) c = (94 * 12);
        }
        else
        {
            c = HighestY + (28 * 12);
            if (c < (74 * 12)) c = (74 * 12);
        }
        return c;
    }
    virtual void MoveTo(OCDraw& /*ScreenObj*/){}
    virtual void ApplyAccidentals(OCStaffAccidentals& /*StaffAccidentals*/){}
    virtual const OCIntList FillLineNumsArray() const { return OCIntList(); }
    virtual void MoveChord(const int /*factor*/){}
    virtual int Moveaccidental(const int /*Notes*/, const int /*Accidentals*/) {return 0;}
    virtual int Pitch(const int /*NoteNum*/) const{return 0;}
    virtual NoteTypeCode NoteType(const int /*NoteNum*/) const{return tsnote;}
    virtual int LineNum(const int /*Index*/) const{return 0;}
    virtual void SetUpDown(const StemDirection UD = StemAuto) {
        UpDown = UD;
        if (UD == StemAuto) SetUpDownProps();
        BalkEndY = BeamHeight();
        CalcUpDownProps();
    }
    double sized(const double val) {
        if (Size == 0) return val;
        return DoubleDiv(val ,SizeFactor(Size));
    }
};

class OCRhythmObjectList : public QList<IOCRhythmObject*>
{
public:
    inline OCRhythmObjectList() { isWrapRight=false; }
    inline OCRhythmObjectList(QList<IOCRhythmObject*>& RhythmObjectList, const int FirstNote, const int Ticks)
    {
        isWrapRight=false;
        if (FirstNote >= RhythmObjectList.size()) return;
        isWrapRight=true;
        int Add=0;
        for (int i = FirstNote; i < RhythmObjectList.size(); i++)
        {
            append(RhythmObjectList[i]);
            Add += RhythmObjectList[i]->NoteValue;
            //LastNote = i;
            if (Add > Ticks)
            {
                isWrapRight = false;
                break;
            }
        }
    }
    OCRhythmObjectList createList(const int FirstNote, const int Ticks) {
        return OCRhythmObjectList(*this,FirstNote,Ticks);
    }
    int averageY() const {
        QAverage<int> Average;
        for (const IOCRhythmObject* r : *this) Average.append(r->AverageY);
        return Average.average();
    }
    bool stemUp() const {
        return (averageY() > 84 * 12);
    }
    bool isWrapRight;
};

class CPausesToPrint :public IOCRhythmObject
{
private:
    int mCenterX = 0;
    int mCenterY = 0;
    int NumOfCompressed = 0;
    int PointerAfterCompress = 0;
    const OCGraphicsList PrintPauseSign(const int c, OCDraw& ScreenObj);
    void PrintNumber(const int b, const int siz, OCDraw& ScreenObj);
    void PlMorePauses(const int NumOfBars, OCDraw& ScreenObj);
public:
    inline CPausesToPrint() {}
    void SetUpDown(const StemDirection UD=StemAuto) {
        UpDown = UD;
        if (UpDown == StemAuto) SetUpDownProps();
        CalcUpDownProps();
        BalkEndY = BalkBeginY;
    }
    void Fill(const OCVoiceLocation& VoiceLocation, const XMLVoiceWrapper& XMLVoice, const double XFysic, OCPrintCounter& CountIt, OCStaffAccidentals& StaffAccidentals, const XMLSymbolWrapper& Symbol, OCPageBarList& BarList, OCDraw& ScreenObj);
    void CalcUpDownProps();
    void plot(OCPrintCounter& CountIt, const int NextHeight, const double NextX, const int BarsToPrint, const QColor& TrackColor, const OCVoiceLocation& VoiceLocation, OCFrameArray& FrameList, OCDraw& ScreenObj);
    const OCIntList FillLineNumsArray() const;
    void MoveChord(const int factor);
};

class CNotesToPrint : public IOCRhythmObject
{
private:
    QList<CNoteHead> NoteHeadList;
    QList<CNoteHead> VorschlagList;
    void clear();
    void sort();
    void CalcCenters(OCPrintVarsType& voiceVars, const double XFysic);
    void PrintHelpLinesDown(const CNoteHead& CurrentNote, OCDraw& ScreenObj);
    void PrintHelpLinesUp(const CNoteHead& CurrentNote, OCDraw& ScreenObj);
    void Calcprops(CNoteHead& CurrentNote, const StemDirection UpDown, const int OldNoteCenterY, const int NextNoteCenterY, int& CAvoid);
    inline void CalcAverage()
    {
        int lo = 32000;
        int hi = -32000;
        for (const CNoteHead& NoteHead : std::as_const(NoteHeadList))
        {
            hi=loBound<int>(hi,NoteHead.CenterY);
            lo=hiBound<int>(lo,NoteHead.CenterY);
        }
        AverageY = int(DoubleDiv(hi - lo, 2)) + lo;
        HighestY = hi;
        LowestY = lo;
    }
    void PlFane(OCDraw& ScreenObj);
    int LineNum2CenterY(const int LineNum, const int Clef) const;
    void plotNote(CNoteHead& CurrentNote, const int TieDirection, const int NextHeight, const double NextX, const int BarsToPrint,const bool UnderTriplet, const QColor& TrackColor, OCTieWrap& TieWrap, OCFrameArray& FrameList, OCDraw& ScreenObj);
public:
    ~CNotesToPrint();
    static inline int Pitch2LineNum(const int Pitch, const OCScaleArray& Scale)
    {
        int Add=0;
        const uint OneOctavePitch = Pitch % 12;
        if (Scale[OneOctavePitch] == 1)
        {
            Add = -1;
        }
        else if (Scale[OneOctavePitch] == 2)
        {
            Add = 1;
        }
        return int(DoubleDiv(Pitch * 7.0 ,12) + 0.4 + Add);
    }
    int Count() const;
    void Fill(const OCVoiceLocation& VoiceLocation, const XMLVoiceWrapper& XMLVoice, const double XFysic, OCPrintCounter& CountIt, OCStaffAccidentals& StaffAccidentals, const XMLSymbolWrapper& Symbol, OCPageBarList& BarList, OCDraw& ScreenObj);
    void CalcUpDownProps();
    void plot(OCPrintCounter& CountIt, int NextHeight, double NextX, int BarsToPrint, const QColor& TrackColor, const OCVoiceLocation& VoiceLocation, OCFrameArray& FrameList, OCDraw& ScreenObj);
    //int BeamHeight() const;
    void MoveTo(OCDraw& ScreenObj);
    void ApplyAccidentals(OCStaffAccidentals& StaffAccidentals);
    const OCIntList FillLineNumsArray() const;
    void MoveChord(const int factor);
    int Pitch(const int NoteNum) const { return NoteHeadList[NoteNum].PitchCode; }
    NoteTypeCode NoteType(const int NoteNum) const { return NoteHeadList[NoteNum].NoteType; }
    int LineNum(const int Index) const { return NoteHeadList[Index].LineNum; }
    int Moveaccidental(const int Notes, const int Accidentals);
};

class OCBeamList
{
public:
    inline OCBeamList(){}
    OCRhythmObjectList RhythmObjects;
    inline void append(IOCRhythmObject* r);
    inline bool isEmpty() const { return RhythmObjects.isEmpty(); }
    inline void clear() {
        RhythmObjects.clear();
        MinNumOfBeams = 1;
        SlantFactor = 0;
    }
    int MinNumOfBeams = 1;
    double SlantFactor = 0;
    void CalcBalk();
    void plot(OCDraw& ScreenObj)
    {
        plotMainBeam(ScreenObj);
        plotSubBeams(ScreenObj);
    }
    int size() const {
        return RhythmObjects.size();
    }
private:
    void plotSubBeams(OCDraw& ScreenObj);
    void plotMainBeam(OCDraw& ScreenObj);
    void plBalk(const double Length, const double factor, const StemDirection UpDown, OCDraw& ScreenObj);
    void plBeam(const QPointF& p1, const QPointF& p2, const StemDirection UpDown, const int NumOfBeams, OCDraw& ScreenObj);
    void plleftsubbalk(const int num, const int commonbalk, const int Length, const int prevnum, const StemDirection UpDown, const double factor, int cbld, OCDraw& ScreenObj);
    void plrightsubbalk(const int num, const int commonbalk, const int Length, const int nextnum, const StemDirection UpDown, const double factor, int cbld, OCDraw& ScreenObj);
};

class OCNoteList
{
private:
    OCRhythmObjectList RhythmObjectList;
    QList<OCBeamList> BeamLists;
    //void FillBeamLists();
public:
    const OCRhythmObjectList ListFromTo(const int FirstNote, const int LastNote) {
        OCRhythmObjectList l;
        for (int i = FirstNote; i <= LastNote; i++) l.append(RhythmObjectList[i]);
        return l;
    }
    const OCRhythmObjectList CreateList(const int FirstNote, const int Ticks)
    {
        return RhythmObjectList.createList(FirstNote,Ticks);
    }
    enum SlurTypes
    {
        SlurDown=-1,
        SlurUp=1
    };
    enum HairpinTypes
    {
        Cresc,
        Dim,
        Fish,
        InvFish
    };
    ~OCNoteList();
    IOCRhythmObject* Append(const double XFysic, const XMLSymbolWrapper& Symbol, const OCVoiceLocation& VoiceLocation, const XMLVoiceWrapper& XMLVoice, OCPrintCounter& CountIt, OCStaffAccidentals& StaffAccidentals, OCPageBarList& BarList, OCDraw& ScreenObj);
    void clear();
    void CreateBeamLists();
    void plot(const int FirstNote, OCPrintCounter& CountIt, const OCVoiceLocation& VoiceLocation, const OCPageBarList& BarList, const QColor& TrackColor, OCFrameArray& FrameList, OCDraw& ScreenObj);
    void PlotBeams(const QColor& TrackColor, OCDraw& ScreenObj);
    static const OCGraphicsList PlotTuplet(const OCRhythmObjectList& TupletList, const int TupletCaption, const QPointF& Pos, const int Size, OCDraw& ScreenObj, const int YOffset = 0);
    static const OCGraphicsList PlotSlur(const OCRhythmObjectList& SlurList, const XMLSimpleSymbolWrapper& XMLSymbol, const bool IsAWrap, OCDraw& ScreenObj);
    static const OCGraphicsList PlotHairPin(const OCRhythmObjectList& HPList, const XMLSimpleSymbolWrapper& XMLSymbol, const bool IsAWrap, OCDraw& ScreenObj);
    static const OCGraphicsList PlotDot(const int Value, const bool UnderTriplet, const int Size, OCDraw& ScreenObj);
    static const OCGraphicsList PlotLengths(const int Sign, const QPointF& Pos, const int UpDown, const int Size, OCDraw& ScreenObj);
    static const OCGraphicsList PlotLengths(const IOCRhythmObject* Props, OCDraw& ScreenObj);
    static const OCGraphicsList plFan(const double height, const StemDirection updown, const int repeat, const int Size, OCDraw& ScreenObj);
    static const QPainterPath FanPath(const double height, const StemDirection updown, const int repeat);
    static const OCGraphicsList PlotHairPin(const int Length, const int gap, const bool IsDim, OCDraw& ScreenObj);
    void plotsigns(const int FirstNote, OCPrintSignList& SignsToPrint, OCFrameArray& FrameList, OCDraw& ScreenObj);
    void ApplyAccidentals(OCStaffAccidentals& StaffAccidentals);
    inline const OCIntList FillLineNumsArray() const
    {
        OCIntList LineNums;
        if (!RhythmObjectList.isEmpty()) LineNums=RhythmObjectList.last()->FillLineNumsArray();
        return LineNums;
    }
    inline void MoveChord(const int factor)
    {
        if (!RhythmObjectList.isEmpty()) RhythmObjectList.last()->MoveChord(factor);
    }
    inline int Moveaccidental(const int Notes, const int Accidentals) const
    {
        if (!RhythmObjectList.isEmpty()) return RhythmObjectList.last()->Moveaccidental(Notes, Accidentals);
        return 0;
    }
};

typedef QVector<OCNoteList> OCNoteListArray;

#endif // OCNOTELIST_H
