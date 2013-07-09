#ifndef OCSYMBOLSCOLLECTION_H
#define OCSYMBOLSCOLLECTION_H

#include "CommonClasses.h"
#include "ocnotelist.h"

class OCSymbolsCollection
{
public:
    OCSymbolsCollection();
    ~OCSymbolsCollection();
    void Connect(void* imglst, void* tlb);
    const static bool editevent(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, QWidget* parent);
    static void PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& dCurrent, int TrackNum, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj);
    static void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& fibset);
    static void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& pcurrent);
    static void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType &sCurrent, int Pointer, OCDraw& ScreenObj);
    static void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& pcurrent);
    static void DrawFactor(XMLSymbolWrapper& Symbol, OCCounter* Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score);
    const static QString Description(const XMLSimpleSymbolWrapper& Symbol);
    static OCProperties* GetProperties(XMLSimpleSymbolWrapper& Symbol);
    static void ModifyProperties(OCProperties* Properties);
    const static bool PropetyExists(const QString& SymbolName, const QString& PropertyName);
    static XMLSimpleSymbolWrapper GetSymbol(const QString& SymbolName);
    static XMLSimpleSymbolWrapper GetSymbol(const QString& SymbolName, const int Button);
    static OCToolButtonProps* ButtonProps(const QString& SymbolName, const int Button);
    const static int ButtonCount(const QString& SymbolName);

    static void ParseFileVersion(XMLScoreWrapper& XMLScore);
    const static QStringList Classes();
    const static QStringList CommonSymbols();
    const static QStringList Categories();
    const static QStringList Category(const QString& CategoryName);
    const static QStringList DuratedCategories();
    const static QStringList DuratedCategory(const QString& CategoryName);
    const static bool IsCommon(const XMLSymbolWrapper& Symbol);
    const static QIcon Icon(const QString& SymbolName, const int Button);
private:
    static int refCount;
    static QMap<QString, CSymbol*> Symbols;
    static QMap<QString, QList<OCToolButtonProps*> > Buttons;
    static QMap<QString, QList<QIcon> > Icons;
    static QMap<QString, QStringList> CategoryMap;
    static QStringList Cats;
    static QMap<QString, QStringList> DuratedCategoryMap;
    static QStringList DuratedCats;
    void insertSymbol(const QString& name, CSymbol* symbol, const QString& Category=QString());
    void insertDuratedSymbol(const QString& name, CSymbol* symbol, const QString& Category=QString());

    static OCProperties* GetProperties(const QString& SymbolName, const int Button);
    static OCProperties* GetProperties(const QString& SymbolName);
};

class CNote :public CVisibleSymbol
{
public:
    CNote(QString Name);
    QList<OCToolButtonProps*> CreateButtons();
    void ModifyProperties(OCProperties* p);
    OCProperties* GetProperties();
};

class CDynamic :public CVisibleSymbol
{
public:
    CDynamic();
    QList<OCToolButtonProps*> CreateButtons();
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    OCProperties* GetProperties(int Button);
    static QStringList DynamicList;
};

class CPatch :public CSymbol
{
public:
    CPatch();
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    static QStringList PatchList;
};

class CTime :public CVisibleSymbol
{
public:
    CTime();
    QList<OCToolButtonProps*> CreateButtons();
    void DrawFactor(XMLSymbolWrapper& Symbol, OCCounter* Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score);
    void ModifyProperties(OCProperties* p);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    static QStringList TimeList;
    static QList<QGraphicsItem*> PlTime(XMLSymbolWrapper& Symbol, int X, OCDraw& ScreenObj, Qt::Alignment Align=Qt::AlignCenter);
    static int GetTicks(XMLSymbolWrapper& Symbol);

};

class CTuplet :public CDuratedSymbol
{
public:
    CTuplet();
    QList<OCToolButtonProps*> CreateButtons();
    void DrawFactor(XMLSymbolWrapper& Symbol, OCCounter* Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
};

class CClef :public CVisibleSymbol
{
public:
    CClef();
    QList<OCToolButtonProps*> CreateButtons();
    void DrawFactor(XMLSymbolWrapper& Symbol, OCCounter* Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    static QStringList ClefList;
    static QList<QGraphicsItem*> PlClef(int Clef, int Size, OCDraw& ScreenObj);
};

class CTranspose :public CSymbol
{
public:
    CTranspose();
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
};

class CKey :public CVisibleSymbol
{
public:
    CKey();
    QList<OCToolButtonProps*> CreateButtons();
    void DrawFactor(XMLSymbolWrapper& Symbol, OCCounter* Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    static QList<QGraphicsItem*> plotKey(int Key, QPointF Pos, int CurrentClef, OCDraw& ScreenObj);
    static QStringList KeyList;
    static int NumOfAccidentals(int Key);
    static int AccidentalFlag(int Key);
};

class CScale :public CSymbol
{
public:
    CScale();
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
};

class CTempo :public CVisibleSymbol
{
public:
    CTempo();
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting);
    static QStringList TempoList;
    static QList<QGraphicsItem*> PlTempo(int Tempo, int NoteVal, bool Dotted, QFont Font, int Size, OCDraw& ScreenObj);

};

class CSysEx :public CSymbol
{
public:
    CSysEx();
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
};

class CController :public CSymbol
{
public:
    CController();
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    static QStringList ControllerList;
};

class CChannel :public CSymbol
{
public:
    CChannel();
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
};

class CAccent :public CVisibleSymbol
{
public:
    CAccent();
    QList<OCToolButtonProps*> CreateButtons();
    void BeforeNote(XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCMIDIFile& MFile, OCPlayBackVarsType& TemPlay);
    void InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType& TempPlay);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int& SignsUp, OCDraw& ScreenObj);
    OCProperties* GetProperties(int Button);
};

class Cfp :public CVisibleSymbol
{
public:
    Cfp();
    QList<OCToolButtonProps*> CreateButtons();
    void BeforeNote(XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCMIDIFile& MFile, OCPlayBackVarsType& TemPlay);
    void DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& TemPlay);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    OCProperties* GetProperties(int Button);
    int NewDynam;
private:
    int FPcount;
};

class Cfz :public CVisibleSymbol
{
public:
    Cfz();
    QList<OCToolButtonProps*> CreateButtons();
    void BeforeNote(XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCMIDIFile& MFile, OCPlayBackVarsType& TemPlay);
    void DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& TemPlay);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    OCProperties* GetProperties(int Button);
    int NewDynam;
private:
    int FPcount;
};

class CAccidental :public CVisibleSymbol
{
public:
    CAccidental();
    QList<OCToolButtonProps*> CreateButtons();
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int& SignsUp, OCDraw& ScreenObj);
    static QStringList AccidentalList;
private:
    QList<QGraphicsItem*> plLeftParanthesis(OCDraw& ScreenObj);
};

class CBowing :public CVisibleSymbol
{
public:
    CBowing();
    QList<OCToolButtonProps*> CreateButtons();
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int& SignsUp, OCDraw& ScreenObj);
    static QStringList BowingList;
};

class CBartokP :public CVisibleSymbol
{
public:
    CBartokP();
    QList<OCToolButtonProps*> CreateButtons();
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int& SignsUp, OCDraw& ScreenObj);
};

class CFingering :public CVisibleSymbol
{
public:
    CFingering();
    QList<OCToolButtonProps*> CreateButtons();
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int& SignsUp, OCDraw& ScreenObj);
};

class CBarWidth :public CSymbol
{
public:
    CBarWidth();
    QList<OCToolButtonProps*> CreateButtons();
    void DrawFactor(XMLSymbolWrapper& Symbol, OCCounter* Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
};

class CBeam :public CDuratedSymbol
{
public:
    CBeam();
    QList<OCToolButtonProps*> CreateButtons();
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    static QStringList BeamList;
};

class CCue :public CVisibleSymbol
{
public:
    CCue();
    QList<OCToolButtonProps*> CreateButtons();
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    void PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj);
};

class CCoda :public CVisibleSymbol
{
public:
    CCoda();
    QList<OCToolButtonProps*> CreateButtons();
    void PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
};

class CDaCapo :public CVisibleSymbol
{
public:
    CDaCapo();
    QList<OCToolButtonProps*> CreateButtons();
    void PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
};

class CDobbel :public CVisibleSymbol
{
public:
    CDobbel();
    QList<OCToolButtonProps*> CreateButtons();
    void InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType& TempPlay);
    void InitPrintSymbol(XMLSymbolWrapper& Symbol, OCPrintVarsType& dCurrent);
    void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int& SignsUp, OCDraw& ScreenObj);
    void DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& TemPlay);
    void BeforeNote(XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCMIDIFile& MFile, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    OCProperties* GetProperties(int Button);
protected:
    int trilldynam;
    int BasePitch;
    int CurrentPitch;
    int oldpitch;
    int Currentstep;
    bool BeginningOfNote;
};

class CDurLength :public CDuratedSymbol
{
public:
    CDurLength();
    QList<OCToolButtonProps*> CreateButtons();
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int& SignsUp, OCDraw& ScreenObj);
    void InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType& TempPlay);
    void InitPrintSymbol(XMLSymbolWrapper& Symbol, OCPrintVarsType& dCurrent);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    OCProperties* GetProperties(int Button);
    static QStringList LengthList;
};

class CDurSlant :public CDuratedSymbol
{
public:
    CDurSlant();
    QList<OCToolButtonProps*> CreateButtons();
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    OCProperties* GetProperties(int Button);
};

class CDurUpDown :public CDuratedSymbol
{
public:
    CDurUpDown();
    QList<OCToolButtonProps*> CreateButtons();
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
};

class CDynChange :public CVisibleSymbol
{
public:
    CDynChange();
    QList<OCToolButtonProps*> CreateButtons();
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType& TempPlay);
    void AfterNote(XMLSymbolWrapper& XMLNote, OCPlayBackVarsType& TemPlay);
    OCProperties* GetProperties(int Button);
};

class CExpression :public CSymbol
{
public:
    CExpression();
    QList<OCToolButtonProps*> CreateButtons();
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
};

class CFermata :public CVisibleSymbol
{
public:
    CFermata();
    QList<OCToolButtonProps*> CreateButtons();
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType& TempPlay);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
};

class CFine :public CVisibleSymbol
{
public:
    CFine();
    QList<OCToolButtonProps*> CreateButtons();
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting);
};

class CHairpin :public CGapSymbol
{
public:
    CHairpin();
    QList<OCToolButtonProps*> CreateButtons();
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    void InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType& TempPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& TemPlay);
    void BeforeNote(XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCMIDIFile& MFile, OCPlayBackVarsType& TemPlay);
    void AfterNote(XMLSymbolWrapper& XMLNote, OCPlayBackVarsType& TemPlay);
    OCProperties* GetProperties(int Button);
private:
    int FishLen;
    int AfterNoteDuration;
    int Direction;
    int TickCount;
};

class CFlipTie :public CSymbol
{
public:
    CFlipTie();
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    static QStringList FlipList;
};

class CGliss :public CVisibleSymbol
{
public:
    CGliss();
    QList<OCToolButtonProps*> CreateButtons();
    void InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType& TempPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int& SignsUp, OCDraw& ScreenObj);
    void DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& TemPlay);
    void BeforeNote(XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCMIDIFile& MFile, OCPlayBackVarsType& TemPlay);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
private:
    int Range;
    int CurrentPitch;
    int oldpitch;
    int Direction;
    int GlissModulate;
};

class CLength :public CVisibleSymbol
{
public:
    CLength();
    QList<OCToolButtonProps*> CreateButtons();
    void ModifyProperties(OCProperties* p);
    void InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType& TempPlay);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int& SignsUp, OCDraw& ScreenObj);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    OCProperties* GetProperties(int Button);
    static QStringList LengthList;
};

class CLimit :public CSymbol
{
public:
    CLimit();
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
};

class CMordent :public CVisibleSymbol
{
public:
    CMordent();
    QList<OCToolButtonProps*> CreateButtons();
    void InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType& TempPlay);
    void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int& SignsUp, OCDraw& ScreenObj);
    void DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& TemPlay);
    void BeforeNote(XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCMIDIFile& MFile, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    OCProperties* GetProperties(int Button);
protected:
    int trilldynam;
    int BasePitch;
    int CurrentPitch;
    int oldpitch;
    int TrillDir;
    bool Finished;
};

class COctave :public CVisibleSymbol
{
public:
    COctave();
    QList<OCToolButtonProps*> CreateButtons();
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    static QStringList OctaveList;
};

class CPortamento :public CSymbol
{
public:
    CPortamento();
    QList<OCToolButtonProps*> CreateButtons();
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    static QStringList PortamentoList;
};

class CRepeat :public CVisibleSymbol
{
public:
    CRepeat();
    QList<OCToolButtonProps*> CreateButtons();
    void ModifyProperties(OCProperties* p);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    static QStringList RepeatList;
private:
    QList<int> Repeat;
    int PlayRepeat;
};

class CSegno :public CVisibleSymbol
{
public:
    CSegno();
    QList<OCToolButtonProps*> CreateButtons();
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
};

class CSlant :public CSymbol
{
public:
    CSlant();
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    static QStringList SlantList;
};

class CDurSlur :public CGapSymbol
{
public:
    CDurSlur();
    QList<OCToolButtonProps*> CreateButtons();
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType& TempPlay);
    void AfterNote(XMLSymbolWrapper& XMLNote, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    static QStringList SlurList;
};

class CStopped :public CVisibleSymbol
{
public:
    CStopped();
    QList<OCToolButtonProps*> CreateButtons();
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int& SignsUp, OCDraw& ScreenObj);
};

class CTempoChange :public CVisibleSymbol
{
public:
    CTempoChange();
    QList<OCToolButtonProps*> CreateButtons();
    void DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting);
    void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void ModifyProperties(OCProperties* p);
    OCProperties* GetProperties(int Button);
    static QStringList TempoChangeList;
};

class CText :public CTextSymbol
{
public:
    CText();
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
private:
    QList<QGraphicsItem*> PlotText(XMLSymbolWrapper& Symbol, int XFysic, int Pointer, OCSymbolArray& SymbolList, OCDraw& ScreenObj);
};

class CTremolo :public CVisibleSymbol
{
public:
    CTremolo();
    QList<OCToolButtonProps*> CreateButtons();
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    void DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int& SignsUp, OCDraw& ScreenObj);
    OCProperties* GetProperties(int Button);
};

class CTrill :public CVisibleSymbol
{
public:
    CTrill();
    QList<OCToolButtonProps*> CreateButtons();
    void InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType& TempPlay);
    void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int& SignsUp, OCDraw& ScreenObj);
    void DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& TemPlay);
    void BeforeNote(XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCMIDIFile& MFile, OCPlayBackVarsType& TemPlay);
    void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    OCProperties* GetProperties(int Button);
    static QStringList TrillList;
protected:
    int height;
    int trilldynam;
    int BasePitch;
    int CurrentPitch;
    int oldpitch;
    int TrillDir;
    bool FinishedPlaying;
};

class CStemDirection :public CSymbol
{
public:
    CStemDirection();
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    static QStringList DirectionList;
};
#endif // OCSYMBOLSCOLLECTION_H
