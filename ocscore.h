#ifndef OCSCORE_H
#define OCSCORE_H

#include "CommonClasses.h"
#include "ocsymbolscollection.h"

typedef OCPrintVarsType* OCPrintVarsArray;
typedef OCPlayBackVarsType* OCPlayVarsArray;
typedef int* OCPointerArray;
typedef bool* OCBoolArray;
typedef OCSignList* OCSignListArray;
typedef CNotesToPlay* OCNotesToPlayArray;
typedef OCNoteList* OCNoteListArray;

enum TrackPlayTypes
{
    tSPlayStartEnd = 1,
    tSPlayStart = 2,
    tSPlayEnd = 3,
    tSPlayPortamento = 4,
    tSPlayNone = 5
};

struct SymbolSearchLocation
{
    int Bar;
    int Staff;
    int Voice;
    int Pointer;
};

class CTrack
{
private:
    OCPrintVarsType fibset;
    void PlBrLine(const int xs, const int Bracket, const int Tuborg, const int stavedistance, OCDraw& ScreenObj);
    void FillBetweenNotes(int& py, QDomLiteElement* XMLVoice, const int NumOfTracks, OCCounter& CountIt, OCPrintVarsArray dcurrent, OCStaffAccidentals& lfortegn);
    void FiBBetweenNotes(int& py, QDomLiteElement* XMLVoice, const int NumOfTracks, OCCounter& CountIt, OCPrintVarsArray fibset, QList<int> &LastTiedNotes);
    const QList<SymbolSearchLocation> SearchBetweenNotes(int &py, QDomLiteElement* XMLVoice, const int NumOfTracks, OCCounter& CountIt, OCPrintVarsArray fibset, QString SearchTerm);
    void FakePlotBetweenNotes(int& py, QDomLiteElement* XMLVoice, const int NumOfTracks, OCCounter& CountIt, OCPrintVarsArray fibset);
    void FiBPlayBetweenNotes(int &py, QDomLiteElement* XMLStaff, const int NumOfTracks, OCCounter& CountIt, OCPlayVarsArray pcurrent, OCMIDIFile& MFile, OCSignList& SignsToPlay, const int TrackOffset);
    const int CalcNoteTime(XMLSymbolWrapper& Symbol, const int Rounded, OCPlayBackVarsType& pcurrent, OCSignList& SignsToPlay) const;
    const int CalcVoicedTime(const int NoteTime, const int Rounded, OCPlayBackVarsType& pcurrent, OCSignList& SignsToPlay) const;
    void PlayNotesOff(CNotesToPlay& NotesToPlay, const int VoicedTime, OCMIDIFile& MFile, OCPlayBackVarsType& pcurrent, bool& HasBeen);
    void PlayPortamentoNotes(const int ThisPitch, CNotesToPlay& NotesToPlay, OCPlayBackVarsType& pcurrent, OCMIDIFile& MFile);
    void PlayPortamentoCleanUp(CNotesToPlay& NotesToPlay, OCMIDIFile& MFile, OCPlayBackVarsType& pcurrent);
    void PlayExprClear(OCPlayBackVarsType& pcurrent, OCMIDIFile& MFile, OCSignList& SignsToPlay);
    void PlayNotesOn(CNotesToPlay& NotesToPlay, OCMIDIFile& MFile, OCPlayBackVarsType& pcurrent, const int ThisVelocity , const int VoicedTime, const int NoteOnPitch);
    void PlayDuringNote(OCSignList& SignsToPlay, XMLSymbolWrapper& XMLNote, const int VoicedTime, int& NoteOnPitch, bool& HasBeen, OCMIDIFile& MFile, OCPlayBackVarsType& pcurrent, const int NoteTime);

public:
    int TrackNum;
    int StaveNum;
    CTrack();
    ~CTrack();
    void plMTr(OCBarList& BarList,QDomLiteElement* XMLVoice,QDomLiteElement* XMLTemplate, const QColor TC, OCSymbolArray& MTObj, OCDraw& ScreenObj, XMLScoreWrapper& XMLScore);
    void getsetting(OCPrintVarsType& tempsetting);
    void putsetting(OCPrintVarsType& tempsetting);
    void PlfirstClef(OCBarList& BarList, OCDraw& ScreenObj);
    void PlfirstAcc(OCBarList& BarList, OCDraw& ScreenObj);
    void FormatBar(const int CurrentBar, const int ActualBar, OCBarList& BarList, XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate);
    void PlTrack(OCBarList& BarList, XMLScoreWrapper& XMLScore, OCSymbolArray& SymbolList, OCNoteList& NoteList, OCDraw& ScreenObj, OCPrintStaffVarsType &sCurrent, const QColor TrackColor);
    const int FillChunk(int& py, OCCounter& CountIt, OCNoteList& NoteList, OCPrintVarsArray dcurrent, const int NumOfTracks, QDomLiteElement* XMLVoice, OCStaffAccidentals& lfortegn, OCBarList& BarList, const int XFysic, QList<int> &LastTiedNotes, OCDraw& ScreenObj);
    const int FiBChunk(int& Pnt, QDomLiteElement* XMLVoice, OCCounter& CountIt, OCPrintVarsArray fibset, const int NumOfTracks, QList<int> &LastTiedNotes);
    const int FiBPlayChunk(int &Pnt, QDomLiteElement* XMLStaff, OCCounter& CountIt, OCPlayVarsArray pcurrent, const int NumOfTracks, OCSignList& SignsToPlay, int& AccelCounter, OCMIDIFile& MFile, const int TrackOffset);
    const int PlayChunk(int &Pnt, QDomLiteElement* XMLStaff, OCCounter& CountIt, OCPlayVarsArray pcurrent, const int NumOfTracks, OCSignList& SignsToPlay, CNotesToPlay& NotesToPlay, int& VoicedTime, OCMIDIFile& MFile, int MIDITrackNumber);
    void PlayBetweenNotes(int& py, QDomLiteElement* XMLStaff, const int NumOfTracks, OCCounter& CountIt, OCPlayVarsArray pcurrent, OCMIDIFile& MFile, OCSignList& SignsToPlay, const int MIDITrackNumber);
    const int SearchChunk(int &Pnt, QDomLiteElement* XMLVoice, QList<SymbolSearchLocation>& l, OCCounter& CountIt, OCPrintVarsArray fibset, const int NumOfTracks, const QString& SearchTerm);
    const int FakePlotChunk(int& Pnt, QDomLiteElement* XMLVoice, OCCounter& CountIt, OCPrintVarsArray fibset, const int NumOfTracks);
    const int GetFirstChannel(QDomLiteElement* XMLVoice);
};

class CStaff
{
private:
    QList<CTrack*> Track;
    const int NumOfTracks() const;
    QList<int> LastTiedNotes;
    void PlLines(const int SystemLength, XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate, const int StartBar, const int StavePlacement, OCDraw& ScreenObj);
    void FillNoteLists(OCNoteListArray NoteLists, OCBarList& BarList, QDomLiteElement* XMLStaff, OCDraw& ScreenObj);
    bool ChordCollision(OCNoteList& NoteList1, OCNoteList& NoteList2) const;
    void CheckChordCollision(OCNoteListArray NoteLists, CStaffCounter& StaffCount);
public:
    QList<QGraphicsItem*> ItemList;
    bool Solo;
    bool Mute;
    int StaveNum;
    CStaff();
    ~CStaff();
    void AddTrack();
    void FormatBar(const int CurrentBar, const int ActualBar, OCBarList& BarList, XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate);
    void plMTr(OCBarList& BarList, QDomLiteElement* XMLStaff, QDomLiteElement* XMLTemplate, const QColor TC , OCSymbolArray& MTObj, OCDraw& ScreenObj, XMLScoreWrapper& XMLScore);
    void DeleteTrack(const int TrackNumber);
    void PlStaff(OCBarList& BarList, XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate, OCSymbolArray& SymbolList, const QColor color, QList<CStaff*> &Staffs, const int ActiveStave, const int ActiveTrack, const int StavePlacement, OCDraw& ScreenObj);
    void Findall(const int dbr, QDomLiteElement *XMLStaff);
    void fibPlay(const int dbr, QDomLiteElement* XMLStaff, OCMIDIFile& MFile, OCPointerArray py, const int TrackOffset, OCPlayVarsArray pcurrent, OCSignListArray SignsToPlay);
    void Play(OCMIDIFile& MFile, int& MIDITrackNumber, const int StartBar, QDomLiteElement* XMLStaff, const int silence);
    const QList<SymbolSearchLocation> Search(QDomLiteElement* XMLStaff, const QString& SearchTerm, const int TrackToSearch=-1) const;
    void FakePlot(const int TrackToSearch, const int PointerGoal, QDomLiteElement* XMLStaff, OCMIDIVars& MIDI);
    void SetStavenum(const int Stave);
    void FillBarsArray(QDomLiteElement* XMLStaff, OCBarMap& bars, const int StaffOffset);
};

class OCScore
{
private:
    QList<CStaff*> Staff;
    OCBarList BarList;
    OCSymbolArray SymbolList;
    const int NumOfStaves();
    void findall(const int BarToFind, XMLScoreWrapper& XMLScore, QDomLiteElement* Template);
public:
    OCScore();
    ~OCScore();
    const int ActuallyPrinted() const;
    const int StartBar() const;
    const int SystemLength() const;

    const bool IsEnded(QDomLiteElement* XMLTemplate) const;
    void EraseSystem(int Stave, QGraphicsScene* Scene);
    void EraseAll(QGraphicsScene* Scene);
    void PlSystem(const int q, XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate, const QColor color, const int ActiveStave, const int ActiveTrack, const int StavePlacement, OCDraw& ScreenObj);
    void PlSystemNoList(const int q, XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate, const QColor color, const int ActiveStave, const int ActiveTrack, const int StavePlacement, OCDraw& ScreenObj);
    void PageBackFormat(XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate);
    void FormatPage(XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate, const int SysLen, const int Start, const int End=0);
    void ReformatPage(const int SysLen);
    const QString ToolTipText(const int Pointer,const int Stave,const  int TrackNr) const;
    void Play(const int StartBr, XMLScoreWrapper& XMLScore, const int silence, OCMIDIFile& MFile, const QString& Path, const int StaveNum=-1);
    const QByteArray MIDIPointer(const int StartBr, XMLScoreWrapper& XMLScore, const int silence, OCMIDIFile& MFile);
    void Mute(const int Stave, const bool Muted);
    void Solo(const int Stave, const bool Solo);
    void AddStave(const int NewNumber);
    void AddTrack(const int Stave);
    void DeleteTrack(const int Stave, const int Track);
    void DeleteStave(const int Stave);
    QList<SymbolSearchLocation> Search(XMLScoreWrapper& XMLScore, const QString& SearchTerm, const int Stave=-1, const int Track=-1) const;
    void FakePlot(const int Stave, const int Track, const int PointerGoal, XMLScoreWrapper& XMLScore, OCMIDIVars& MIDI);
    void CreateBarMap(XMLScoreWrapper& XMLScore);
    const OCBarMap& BarMap() const;
    const bool StaffEmpty(const int Staff, XMLScoreWrapper& XMLScore, QDomLiteElement* XMLTemplate);
    void MakeStaves(XMLScoreWrapper& XMLScore);
    OCFrameProperties* GetFrame(const int Pointer);
    int InsideFrame(const QPoint& m) const;
    const QList<int> PointersInside(const QRect& r) const;
};

#endif // OCSCORE_H
