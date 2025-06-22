#ifndef OCSCORE_H
#define OCSCORE_H

#include "CommonClasses.h"
#include "ocnotelist.h"

class CVoice : public XMLVoiceWrapper
{
private:
    OCPrintVarsType pageStartVars;
    const QRectF plBarLine(const double xs, const XMLTemplateStaffWrapper& templateStaff, OCDraw& ScreenObj);
    void fillBetweenNotes(OCStaffCounterPrint& voiceVarsArray, OCStaffAccidentals& StaffAccidentals);
    void findBarBetweenNotes(OCStaffCounterPrint& voiceVarsArray);
    const OCBarSymbolLocationList searchBetweenNotes(OCStaffCounterPrint& voiceVarsArray, const QString& SearchTerm);
    void fakePlotBetweenNotes(OCStaffCounterPrint& voiceVarsArray);
    void findBarPlayBetweenNotes(XMLStaffWrapper& XMLStaff, OCStaffCounterPlay& voiceVarsArray, OCMIDIFile& MFile, OCPlaySignList& SignsToPlay, const int TrackOffset);
    int calcNoteTime(const XMLSymbolWrapper& Symbol, const int Rounded, OCPlayBackVarsType& voiceVars, OCPlaySignList& SignsToPlay) const;
    void playNotesOff(CNotesToPlay& NotesToPlay, int DeltaTime, OCMIDIFile& MFile, OCPlayBackVarsType& voiceVars, bool Pedal);
    void playPortamentoNotes(const int ThisPitch, CNotesToPlay& NotesToPlay, OCPlayBackVarsType& voiceVars, OCMIDIFile& MFile);
    void playExprClear(OCPlayBackVarsType& voiceVars, OCMIDIFile& MFile, OCPlaySignList& SignsToPlay);
    void playNotesOn(CNotesToPlay& NotesToPlay, OCMIDIFile& MFile, OCPlayBackVarsType& voiceVars, const int ThisVelocity , const int VoicedTime, const int NoteOnPitch);
    void playDuringNote(OCPlaySignList& SignsToPlay, const XMLSymbolWrapper& XMLNote, int& VoicedTime, int& NoteOnPitch, OCMIDIFile& MFile, OCPlayBackVarsType& voiceVars, const int NoteTime);
    void playDuringPause(OCPlaySignList& SignsToPlay, int NoteOnPitch, OCMIDIFile& MFile, OCPlayBackVarsType& voiceVars);
public:
    OCVoiceLocation VoiceLocation;
    OCFrameArray FrameList;
    CVoice(QDomLiteElement* e, const int staff, const int index);
    ~CVoice();
    void plotMasterStuff(OCPageBarList& BarList, const QColor& TC, OCDraw& ScreenObj, const XMLScoreWrapper& XMLScore);
    void getPageStartVars(OCPrintVarsType& voiceVars);
    void setPageStartVars(const OCPrintVarsType& voiceVars);
    void plfirstClef(OCPageBarList& BarList, OCDraw& ScreenObj);
    void plfirstKey(OCPageBarList& BarList, OCDraw& ScreenObj);
    void formatBar(const OCPageBar& b, OCPageBarList& BarList, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate);
    void plVoice(OCPageBarList& BarList, const XMLScoreWrapper& XMLScore, OCNoteList& NoteList, OCDraw& ScreenObj, const XMLTemplateStaffWrapper& XMLTemplateStaff, const QColor& TrackColor);
    void clefLookAhead(OCPrintCounter& CountIt, const XMLScoreWrapper& XMLScore);
    int fillChunk(OCNoteList& NoteList, OCStaffCounterPrint& voiceVarsArray, OCStaffAccidentals& StaffAccidentals, OCPageBarList& BarList, const double XFysic, OCDraw& ScreenObj);
    int findBarChunk(OCStaffCounterPrint& voiceVarsArray);
    int findBarPlayChunk(XMLStaffWrapper& XMLStaff, OCStaffCounterPlay& voiceVarsArray, OCPlaySignList& SignsToPlay, OCMIDIFile& MFile, const int TrackOffset);
    int playChunk(XMLStaffWrapper& XMLStaff, OCStaffCounterPlay& voiceVarsArray, OCPlaySignList& SignsToPlay, CNotesToPlay& NotesToPlay, OCMIDIFile& MFile, int MIDITrackNumber);
    void playBetweenNotes(XMLStaffWrapper& XMLStaff, OCStaffCounterPlay& voiceVarsArray, OCMIDIFile& MFile, OCPlaySignList& SignsToPlay, const int MIDITrackNumber);
    int searchChunk(OCBarSymbolLocationList& l, OCStaffCounterPrint& CountIt, const QString& SearchTerm);
    int FakePlotChunk(OCStaffCounterPrint& voiceVarsArray);
    int getFirstChannel();
};

class CStaff : public XMLStaffWrapper
{
private:
    QList<CVoice> Voices;
    const QRectF plotLinesAndBrackets(const double SystemLength, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options, const int StartBar, const int StavePlacement, OCDraw& ScreenObj);
    OCNoteListArray fillNoteLists(OCPageBarList& BarList, OCDraw& ScreenObj);
    bool chordCollision(OCNoteList& NoteList1, OCNoteList& NoteList2) const;
    void checkChordCollision(OCNoteListArray& NoteLists, OCStaffCounterPrint& StaffCount);
public:
    OCGraphicsList ItemList;
    OCStaffLocation StaffLocation;
    OCFrameArray& FrameList(const int Voice) { return Voices[Voice].FrameList; }
    CStaff(QDomLiteElement* e, const int index);
    ~CStaff();
    void formatBar(const OCPageBar& b, OCPageBarList& BarList, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate);
    void plotMasterStuff(OCPageBarList& BarList, const QColor& TC , OCDraw& ScreenObj, const XMLScoreWrapper& XMLScore);
    void plotStaff(OCPageBarList& BarList, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options, const QColor& color, CStaff& MasterStaff, OCDraw& ScreenObj);
    void findall(const int dbr);
    void findBarPlay(const int dbr, OCMIDIFile& MFile, const int TrackOffset, OCStaffCounterPlay& voiceVarsArray, OCPlaySignListArray& SignsToPlay);
    void play(OCMIDIFile& MFile, int& MIDITrackNumber, const int StartBar, const int silence, const OCBarMap& barmap);
    const OCBarSymbolLocationList search(const QString& SearchTerm, const int TrackToSearch=-1);
    const OCPrintVarsType fakePlot(const int TrackToSearch, const int TargetIndex);
    void fillBarsArray(OCBarMap& bars, const int StaffOffset);
};

class OCScore : public XMLStaffCollectionWrapper
{
private:
    QList<CStaff> Staffs;
    OCPageBarList BarList;
    void findall(const int BarToFind, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options);
public:
    OCScore();
    ~OCScore();
    int barsActuallyPrinted() const;
    int startBar() const;
    double systemLength() const;
    bool isEnded(const XMLTemplateWrapper& XMLTemplate) const;
    void eraseSystem(int Stave, QGraphicsScene* Scene);
    void eraseAll(QGraphicsScene* Scene);
    void plotStaff(const int q, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options, const QColor& color, OCDraw& ScreenObj);
    void plotStaffNoList(const int q, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options, const QColor& color, OCDraw& ScreenObj);
    void formatPageBack(const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options);
    void formatPage(const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& XMLTemplate, const XMLScoreOptionsWrapper& Options, const double SysLen, const int Start, const int End=0);
    void reformatPage(const int SysLen);
    const QString toolTipText(const OCSymbolLocation& Symbol) const;
    void play(const int StartBr, const int silence, const QString& Path);
    const QByteArray MIDIPointer(const int StartBr, const int silence);
    OCBarSymbolLocationList search(const QString& SearchTerm, const int Stave=-1, const int Track=-1);
    const OCMIDIVars fakePlot(const OCSymbolLocation& Target);
    int fakePlotClef(const OCSymbolLocation& Target);
    void createBarMap();
    const OCBarMap& BarMap() const;
    bool StaffEmpty(const int StaffPos, const XMLTemplateWrapper& XMLTemplate);
    void assignXML(const XMLScoreWrapper& XMLScore);
    const OCFrameProperties& getFrame(const OCSymbolLocation& l);
    const OCSymbolLocation insideFrame(const QPointF& m);
    const OCSymbolLocation nearestLocation(const double y, const OCSymbolLocation& currentLocation);
    const OCLocationList locationsInside(const QRectF& r);
    const OCPointerList pointersInsideVoice(const QRectF& r, const OCVoiceLocation& v);
    const QRectF getBarlineX(const int BarNum);
    int insideBarline(const QPointF& x);
    OCFrameArray& FrameList(const int Staff, const int Voice);
};

#endif // OCSCORE_H
