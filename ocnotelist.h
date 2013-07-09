#ifndef OCNOTELIST_H
#define OCNOTELIST_H

#include "CommonClasses.h"
#include "ocsignlist.h"

class OCStaffAccidentals
{
private:
    acctype lfortegn[128];
    bool Dirty[7];
    int OneOct[7];
    QList<OCStaffAccidentalItem> NoteItems;
    QList<int> Ignore;
    const int FnKB(int* iJ, const int NoteNum) const;
    const int FnHasFortegn(const int KB) const;
    void update(bool* WasHere, const int LineNum, const int KB, const int iTemp);
    void calc(bool* WasHere, int* iJ);
    const int Switch(const int In, const int Out1, const int Out2, const int OutElse) const;
public:
    void Reset1(const int Pitch);
    void RdAcc(const int Key);
    void CheckFortegn(int* iJ);
    const QList<int> GetHasFortegnArray(const int Antal);
    void PutNoteArrays(const QList<OCStaffAccidentalItem>& Items);
    void ResetCluster();
    void AddIgnore(const int Pitch);
    void ClearIgnore();
};

class OCRhythmObject
{
public:
    bool Invisible;
    int Size;
    int MoveX;
    int BalkBeginY;
    int val;
    int balkx;
    int UpDown;
    int AverageY;
    int BalkEndY;
    int Rounded;
    int Beat;
    int Bar;
    int Meter;
    int NumOfBeams;
    int NumOfEvInBeam;
    bool IsPause;
    int ForceUpDown;
    int ForceBeam;
    int NumOfForcedBeams;
    int ForceSlant;
    int HighestY;
    int LowestY;
    float SlantFactor;
    int BeamLimit;
    int CenterX;
    bool IsSingleNoteWithTie;
    bool FlipTie;
    int PerformanceSign;
    QPointF PerformanceSignPos;
    int PerformanceSignSize;
    OCRhythmObject();
    void PlotStem(OCDraw& ScreenObj);
    const bool BeamAble();
    const int FNclcfanr(const int c) const;
};

class CNoteHead
{
public:
    bool Invisible;
    int Pointer;
    int OrigPlace;
    int Voice;
    NoteTypeCode NoteType;
    int PitchCode;
    int NoteVal;
    int Left;
    int AccidentalLeft;
    int Size;
    int Marked;
    int CenterX;
    int CenterY;
    int FortegnAddX;
    int Pitch;
    int NoteNum;
    int HasFortegn;
    CNoteHead();
    void MoveTo(OCDraw& ScreenObj);
    //void MoveTo0(OCDraw& ScreenObj);
    void plot(const int ClusterSize, const int TriolVal, const QColor TrackColor, const int UpDown, OCSymbolArray& SymbolList, OCDraw& ScreenObj);
};

class IOCRhythmObject
{
protected:
    const int stemX(const int UpDown, const int X);
public:
    OCRhythmObject Props;
    IOCRhythmObject();
     const int Count();
     virtual void Fill(int& Py, QDomLiteElement* XMLVoice, int TrackNum , OCPrintVarsType& dCurrent, float XFysic, OCCounter& CountIt, OCStaffAccidentals& lfortegn, XMLSymbolWrapper& Symbol, OCBarList& BarList, OCDraw& ScreenObj);
     virtual void CalcUpDownProps();
     virtual void plot(int& NextHeight, int NextX, int BarsToPrint, OCTieWrap& TieWrap, int TriolVal, QColor TrackColor, int& Py, OCSymbolArray& SymbolList, OCDraw& ScreenObj);
    //virtual int FNTieLen(float FactorX, OCBarList& BarList, int Flyt, int Meter);
     virtual int FNBalkHeight() const;
     virtual void MoveTo(OCDraw& ScreenObj);
     virtual void SetHasFortegn(OCStaffAccidentals& lfortegn);
     void FillLineNumsArray(const QList<int>& LineNums);
     virtual void MoveChord(int factor);
     const int Moveaccidental(const int Notes, const int Accidentals) const;
     const int Pitch(const int NoteNum) const;
     //const int CenterY(const int NoteNum);
     const NoteTypeCode NoteType(const int NoteNum) const;
     const int LineNum(const int Index) const;
     //const int NoteNum(const int Index);
    //virtual int FNSlurY(int iTemp, int UpDown);
};

class CPausesToPrint :public IOCRhythmObject
{
private:
    int mCenterX;
    int mCenterY;
    int NumOfCompressed;
    int PointerAfterCompress;
    const QList<QGraphicsItem*> plPsSgn(const int c, OCDraw& ScreenObj);
    void plNum(const int b, const int siz, OCDraw& ScreenObj);
    void PlMorePauses(const int NumOfBars, OCDraw& ScreenObj);
public:
    CPausesToPrint();
    //int Count();
    void Fill(int& Py, QDomLiteElement* XMLVoice, int TrackNum , OCPrintVarsType& dCurrent, float XFysic, OCCounter& CountIt, OCStaffAccidentals& lfortegn, XMLSymbolWrapper& Symbol, OCBarList& BarList, OCDraw& ScreenObj);
    void CalcUpDownProps();
    void plot(int& NextHeight, int NextX, int BarsToPrint, OCTieWrap& TieWrap, int TriolVal, QColor TrackColor, int& Py, OCSymbolArray& SymbolList, OCDraw& ScreenObj);
    //int FNTieLen(float FactorX, OCBarList& BarList, int Flyt, int Meter);
    int FNBalkHeight() const;
    //void MoveTo(OCDraw& ScreenObj);
    //void SetHasFortegn(OCStaffAccidentals& lfortegn);
    virtual void FillLineNumsArray(QList<int>& LineNums);
    void MoveChord(int factor);
    //int Moveaccidental(int Notes, int Accidentals);
    //int Pitch(int NoteNum);
    //const int CenterY(const int NoteNum);
    //const QString SymbolName(const int NoteNum);
    //int LineNum(int Index);
    //int NoteNum(int Index);
    //int FNSlurY(int iTemp, int UpDown);
};

class CNotesToPrint : public IOCRhythmObject
{
private:
    QList<CNoteHead*> NoteHeadList;
    CNoteHead* CurrentNote;
    int OldNote;
    int NextNote;
    int CAvoid;
    int hi;
    int lo;
    void Append(const int Voice, const int Pointer, const XMLSymbolWrapper& Symbol, const bool Marked);
    void clear();
    void sort();
    void CalcCenters(OCPrintVarsType& dCurrent, const float XFysic);
    //void Retrieve(int RecNum );
    const int GetAlti(const int RecNum) const;
    void PlBilinier(const int CurrNote , OCDraw& ScreenObj);
    void Calcprops(const int UpDown);
    const int fnAcalc(const int a, OCPrintVarsType& b) const;
    void CalcAverage();
    void CalcFortegns(OCStaffAccidentals& lfortegn, OCPrintVarsType& dCurrent);
    void AddToArrays(OCStaffAccidentals& lfortegn);
    //void FillNotesPrinted(OCSymbolArray& SymbolList, int Py);
    void PlFane(OCDraw& ScreenObj);
    const int fnAandClefCalc(const int a, OCPrintVarsType& b) const;
    void plNote(int& TieDirection, int iTemp, int& NextHeight, int NextX, int BarsToPrint,int TriolVal, QColor TrackColor, OCTieWrap& TieWrap, OCSymbolArray& SymbolList, OCDraw& ScreenObj);
public:
    CNotesToPrint();
    ~CNotesToPrint();
    const int Count() const;
    void Fill(int& Py, QDomLiteElement* XMLVoice, int TrackNum , OCPrintVarsType& dCurrent, float XFysic, OCCounter& CountIt, OCStaffAccidentals& lfortegn, XMLSymbolWrapper& Symbol, OCBarList& BarList, OCDraw& ScreenObj);
    void CalcUpDownProps();
    void plot(int& NextHeight, int NextX, int BarsToPrint, OCTieWrap& TieWrap, int TriolVal, QColor TrackColor, int& Py, OCSymbolArray& SymbolList, OCDraw& ScreenObj);
    //int FNTieLen(float FactorX, OCBarList& BarList, int Flyt, int Meter);
    int FNBalkHeight() const;
    void MoveTo(OCDraw& ScreenObj);
    void SetHasFortegn(OCStaffAccidentals& lfortegn);
    void FillLineNumsArray(QList<int>& LineNums);
    void MoveChord(const int factor);
    const int Moveaccidental(const int Notes, const int Accidentals) const;
    const int Pitch(const int NoteNum) const;
    const int CenterY(const int NoteNum) const;
    const NoteTypeCode NoteType(const int NoteNum) const;
    const int LineNum(const int Index) const;
    const int NoteNum(const int Index) const;
    const int HasAccidental(const int NoteNum) const;
};

class OCNoteList
{
private:
    //QMap<QString,int> args;
    int BalkOverRideCount;
    QList<IOCRhythmObject*> RhythmObjectList;
    void BeamUpDown();
    void SetUpDown(OCRhythmObject* Props);
    void CalcBalk(const int First, const int Last);
    void plleftsubbalk(const int num, const int commonbalk, const int Length, const int prevnum, const int UpDown, const float factor, int cbld, OCDraw& ScreenObj);
    void plrightsubbalk(const int num, const int commonbalk, const int Length, const int nextnum, const int UpDown, const float factor, int cbld, OCDraw& ScreenObj);
    const int FNSlurY(const int iTemp, const int UpDown) const;
public:
    OCNoteList();
    ~OCNoteList();
    void Append(const float XFysic, XMLSymbolWrapper& Symbol, int& Py, QDomLiteElement* XMLVoice, const int TrackNum, OCPrintVarsType& dCurrent, OCCounter& CountIt, OCStaffAccidentals& lfortegn, OCBarList& BarList, QList<int> &LastTiedNotes, OCDraw& ScreenObj);
    void clear();
    void SearchForBeams();
    void plot(const int iTemp, int& CurrentMeter, int& Py, OCBarList& BarList, OCTieWrap& TieWrap, const int TriolVal, const QColor TrackColor, OCSymbolArray& SymbolList, OCDraw& ScreenObj);
    void PlotBeams(const QColor TrackColor, OCDraw& ScreenObj);
    void PlotTuplet(const int iTemp, const int Beats, const int TupletNumber, const QPointF& Pos, const int Size, OCSymbolArray& SymbolList, const int Pointer, OCDraw& ScreenObj);
    const QList<QGraphicsItem*> PlotTuplet(const int iTemp, const int Beats, const int TupletNumber, const QPointF& Pos, const int Size, OCDraw& ScreenObj);
    void PlotSlur(const int iTemp, const int Beats, const int UpDown, const int Angle, const QPointF& Pos, const int Size, OCSymbolArray& SymbolList, const bool IsAWrap, const int Pointer, OCDraw& ScreenObj);
    const QList<QGraphicsItem*> PlotSlur(const int iTemp, const int Beats, const int UpDown, const int Angle, const QPointF& Pos, const int Size, const bool IsAWrap, OCDraw& ScreenObj);
    void PlotHairPin(const int iTemp, const int Beats, const int WhatType, const QPointF& Pos, const int Size, OCSymbolArray& SymbolList, const bool IsAWrap, const int Pointer, OCDraw& ScreenObj);
    const QList<QGraphicsItem*> PlotHairPin(const int iTemp, const int Beats, const int WhatType, const QPointF& Pos, const int Size, const bool IsAWrap, OCDraw& ScreenObj);
    const static QList<QGraphicsItem*> PlPunkt(const int Value, const int TriolVal, const int Size, OCDraw& ScreenObj);
    const static QList<QGraphicsItem*> PlotLengths(const int Sign, const QPointF& Pos, const int UpDown, const int Size, OCDraw& ScreenObj);
    const static QList<QGraphicsItem*> plFan(const float height, const int updown, const int repeat, const int Size, OCDraw& ScreenObj);
    const static QPainterPath FanPath(const float height, const int updown, const int repeat);
    void plBeam(const float x1, const float y1, const float x2, const float y2, const int UpDown, const int NumOfBeams, OCDraw& ScreenObj);
    void plBalk(const float Length, const float factor, const int UpDown, OCDraw& ScreenObj);
    const QList<QGraphicsItem*> PlotHairPin(const int Length, const int gap, const bool IsDim, OCDraw& ScreenObj);
    void plotsigns(const int iTemp, OCSignList& SignsToPrint, OCSymbolArray& SymbolList, OCDraw& ScreenObj);
    void SetHasFortegn(OCStaffAccidentals& lfortegn);
    void SetVoiceUpDown(const int Voice);
    void FillLineNumsArray(QList<int>& LineNums);
    void MoveChord(const int factor);
    const int Moveaccidental(const int Notes, const int Accidentals) const;
};

#endif // OCNOTELIST_H
