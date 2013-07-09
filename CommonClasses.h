#ifndef COMMONCLASSES_H
#define COMMONCLASSES_H

#define expressiondefault 116
#define begofbar 204
#define endofbar 48
#define ScoreLeftMargin 18*12
#define ScoreStaffHeight 1200
#define ScoreStaffLinesHeight 384
#define ScoreTopSymbolY ScoreStaffHeight+(20*12)
#define ScoreBottomSymbolY 42*12
#define ScoreTempoY (44 * 12) + ScoreStaffHeight
#define maxticks 255

#define midifl "/Users/Shared/oc.mid"
#define settingsfile "../OCstuff.xml"
#define OCTTFname "Object Composer"
#define BeamThickness 48
#define BeamSpace 24
#define HelpLineHalfWidth 84
#define LineHalfThickNess 4
#define AccidentalSpace 96

#define tablerowheight 17

#define inactivestaffcolor Qt::darkGray
#define unselectablecolor QColor(0,0,0,180)
#define selectedcolor Qt::red
#define markedcolor Qt::blue
#define activestaffcolor Qt::black

#define renderinghints QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing | QPainter::Antialiasing | QPainter::TextAntialiasing

#include <QString>
#include <QList>
#include <QVariant>
#include <qmath.h>
#include <QFont>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include "qmacrubberband.h"
#include <QDomLite>
#include <QIcon>
#include <QDebug>
#include <QAbstractTableModel>
#include <QModelIndex>

class CStringCompare
{
protected:
    QString Val;
public:
    CStringCompare(const QString& str);
    const bool Compare(const QStringList& S) const;
    const bool Compare(const QString& S1) const;
    const bool Compare(const QString& S1,const QString& S2) const;
    const bool Compare(const QString& S1,const QString& S2,const QString& S3) const;
    const bool Compare(const QString& S1,const QString& S2,const QString& S3,const QString& S4) const;
    const bool Compare(const QString& S1,const QString& S2,const QString& S3,const QString& S4,const QString& S5) const;
};

class CNoteCompare : public CStringCompare
{
protected:
    QDomLiteElement* XMLElement;
public:
    CNoteCompare(QDomLiteElement* XML);
    const bool IsNoteType(const bool Note, const bool TiedNote=false, const bool CompoundNote=false, const bool TiedCompoundNote=false) const;
    const bool IsRestOrNoteType(const bool Note, const bool TiedNote=false, const bool CompoundNote=false, const bool TiedCompoundNote=false) const;
    const bool IsRestOrValuedNote() const;
    const bool IsTiedNote() const;
    const bool IsValuedNote() const;
    const bool IsCompoundNote() const;
    const bool IsRestOrAnyNote() const;
    const bool IsAnyNote() const;
    const bool IsRest() const;
    const bool IsEndOfVoice() const;
};

class XMLSimpleSymbolWrapper : public CNoteCompare
{
public:
    XMLSimpleSymbolWrapper(QDomLiteElement* XMLVoice, const int Pointer);
    XMLSimpleSymbolWrapper(QDomLiteElement* XMLSymbol);
    const QString name() const;
    const int size() const;
    const QPointF pos() const;
    const QPointF move(const QPointF p);
    const QPointF move(const float x, const float y);
    const float moveX(const float x);
    const float moveY(const float y);
    const double getVal(const QString& Tag) const;
    const QString attribute(const QString& Tag) const;
    void setAttribute(const QString& Tag, const QVariant& Value);
    const bool attributeExists(const QString& Tag) const;
    QDomLiteElement* getXML();
    const static float CalcTicks(const int NoteValue, const bool Dotted, const bool Triplet);
    static void SetNoteVal(int& Notevalue, bool& Dotted, bool& Triplet, const float Ticks);
};

class XMLSymbolWrapper : public XMLSimpleSymbolWrapper
{
public:
    XMLSymbolWrapper(QDomLiteElement* XMLVoice, const int Pointer, const int CurrentMeter);
    XMLSymbolWrapper(QDomLiteElement* XMLSymbol, const int CurrentMeter);
    const int ticks() const;
private:
    int m_Ticks;
    void Init(const int CurrentMeter);
    const int Calc();
};

class XMLFontWrapper
{
public:
    XMLFontWrapper(QDomLiteElement* XMLFont);
    XMLFontWrapper(QDomLiteElement *XMLParent, const QString& Name);
    XMLFontWrapper(const QFont& f, const QString& Name);
    const QFont getFont() const;
    void setFont(const QFont& f);
    QDomLiteElement* getXML();
    void showDialog(QWidget* parent=0);
protected:
    QDomLiteElement* XMLElement;
};

class XMLTextWrapper : public XMLFontWrapper
{
public:
    XMLTextWrapper(QDomLiteElement* XMLText);
    XMLTextWrapper(QDomLiteElement *XMLParent, const QString& Name);
    const QString getText() const;
    void setText(const QString& Text);
};

class XMLScoreWrapper
{
public:
    XMLScoreWrapper();
    XMLScoreWrapper(QDomLiteDocument* Doc);
    ~XMLScoreWrapper();
    void setXML(QDomLiteDocument* Doc);
    void setXML(XMLScoreWrapper& Doc);
    void setCopy(QDomLiteDocument* Doc);
    void setCopy(XMLScoreWrapper& Doc);
    void shadowXML(QDomLiteDocument* Doc);
    void shadowXML(XMLScoreWrapper& Doc);
    QDomLiteDocument* getXML();
    QDomLiteElement** documentPointer();
    QDomLiteElement* documentElement();
    QDomLiteElement* documentClone();
    QDomLiteDocument* getClone();
    void newScore();
    void newDoc();
    void replaceDocumentElement(QDomLiteElement* XMLDoc);
    void replaceScore(QDomLiteElement* XMLScore);
    void replaceLayoutCollection(QDomLiteElement* XMLLayoutCollection);
    const bool Load(const QString& Path);
    const bool Save(const QString& Path);
    const QString attribute(const QString& Tag) const;
    const double getVal(const QString& Tag) const;
    void setAttribute(const QString& Tag, const QVariant& Value);
    QDomLiteElement* Score();
    QDomLiteElement* Staff(const int Index);
    QDomLiteElement* Voice(const int StaffIndex, const int Index);
    static QDomLiteElement* Voice(QDomLiteElement* XMLStaff, const int Index);
    static QDomLiteElement* Voice(QDomLiteElement* XMLScore, const int StaffIndex, const int Index);
    QDomLiteElement* Symbol(const int StaffIndex, const int VoiceIndex, const int Index);
    static QDomLiteElement* Symbol(QDomLiteElement* XMLVoice, const int Index);
    QDomLiteElement* SymbolClone(const int StaffIndex, const int VoiceIndex, const int Index);
    QDomLiteElement* SymbolClone(QDomLiteElement* XMLVoice, const int Index);
    const static int FindSymbol(QDomLiteElement* XMLVoice, const QString& Name, const int Ptr=0, const QString& Attr=QString(), const double Val=0, const QString& Attr1=QString(), const double Val1=0);
    QDomLiteElement* Templates();
    QDomLiteElement* Template(const int Index);
    QDomLiteElement* TemplateStaff(const int TemplateIndex, const int Index);
    QDomLiteElement* TemplateOrderStaff(const int TemplateIndex, const int Order);
    void ValidateBrackets(const int TemplateIndex);
    static void ValidateBrackets(QDomLiteElement* Template);
    const int AllTemplateIndex(const int TemplateIndex, const int StaffIndex);
    const static int AllTemplateIndex(QDomLiteElement* XMLTemplate, const int StaffIndex);
    const static int AllTemplateIndex(QDomLiteElement* XMLTemplateStaff);
    const int StaffOrder(const int TemplateIndex, const int StaffIndex);
    const static int StaffOrder(QDomLiteElement* XMLTemplate, const int StaffIndex);
    const static bool StaffOnTemplate(QDomLiteElement* XMLTemplate, const int StaffIndex);
    static QDomLiteElement* TemplateStaff(QDomLiteElement* XMLTemplate, const int Index);
    static QDomLiteElement* TemplateOrderStaff(QDomLiteElement* XMLTemplate, const int Order);
    const QFont TempoFont(const QFont& defaultFont=QFont("Times New Roman",160,QFont::Bold));
    const QFont DynamicFont(const QFont& defaultFont=QFont("Times New Roman",150,QFont::Normal,true));
    void setTempoFont(const QFont& Font);
    void setDynamicFont(const QFont& Font);

    const int NumOfStaffs();
    const int NumOfVoices(const int StaffIndex);
    const static int NumOfVoices(QDomLiteElement* XMLStaff);
    QDomLiteElement* Paste1Voice(QDomLiteElement* XMLVoice, const int Pointer, QDomLiteElement* data);
    QDomLiteElement* Paste1Voice(const int StaffIndex, const int VoiceIndex, const int Pointer, QDomLiteElement* data);
    static void Clear1Voice(QDomLiteElement* XMLVoice, const int StartPointer, const int EndPointer);
    static void Clear1Voice(QDomLiteElement* XMLVoice, const QList<int> Pointers);
    void Clear1Voice(const int StaffIndex, const int VoiceIndex, const int StartPointer, const int EndPointer);
    void Clear1Voice(const int StaffIndex, const int VoiceIndex, const QList<int> Pointers);
    const static int VoiceLength(QDomLiteElement* XMLVoice);
    const int VoiceLength(const int StaffIndex, const int VoiceIndex);
    QDomLiteElement* AddVoice(const int StaffIndex);
    QDomLiteElement* AddVoice(const int StaffIndex, const int NewNumber);
    QDomLiteElement* AddVoice(QDomLiteElement* XMLStaff);
    QDomLiteElement* AddVoice(QDomLiteElement* XMLStaff, int NewNumber);
    void UpdateIndexes(QDomLiteElement* XMLTemplate);
    QDomLiteElement* AddTemplateStaff(const int TemplateIndex, const int NewNumber, const QString& Name);
    QDomLiteElement* AddTemplateStaff(QDomLiteElement* XMLTemplate, const int NewNumber, const QString& Name);
    QDomLiteElement* AddTemplateStaff(const int NewNumber, const QString& Name);
    QDomLiteElement* AddStaff(const int NewNumber, const QString& Name);
    static void InsertSymbol(QDomLiteElement* XMLVoice, QDomLiteElement* XMLSymbol, const int Pointer);
    void InsertSymbol(const int StaffIndex, const int VoiceIndex, QDomLiteElement* XMLSymbol, const int Pointer);
    static void DeleteVoice(QDomLiteElement* XMLStaff, const int VoiceIndex);
    void DeleteVoice(const int StaffIndex, const int VoiceIndex);
    void TemplateDeleteStaff(QDomLiteElement* XMLTemplate, const int StaffIndex);
    void TemplateDeleteStaff(const int TemplateIndex, const int StaffIndex);
    void TemplateDeleteStaff(const int StaffIndex);
    void DeleteStaff(const int StaffIndex);
    void MoveStaff(const int From, const int To);
    const static int StaffPos(QDomLiteElement* XMLTemplate, const int StaffIndex);
    const int StaffPos(const int TemplateIndex, const int StaffIndex);
    const int StaffPos(const int StaffIndex);
    const QString StaffName(const int StaffIndex);
    const static QString StaffName(QDomLiteElement* XMLTemplateStaff);
    const static QString StaffName(QDomLiteElement* XMLTemplate, const int StaffIndex);
    void setStaffName(const int StaffIndex, const QString& Name);
    const QString StaffAbbreviation(const int StaffIndex);
    const static QString StaffAbbreviation(QDomLiteElement* XMLTemplateStaff);
    const static QString StaffAbbreviation(QDomLiteElement* XMLTemplate, const int StaffIndex);
    void setStaffAbbreviation(const int StaffIndex, const QString& Abbreviation);

    const bool LayoutCollectionExists() const;
    const int NumOfLayouts();
    QDomLiteElement* LayoutCollection();
    QDomLiteElement* LayoutTemplate(const int Index);
    QDomLiteElement* Layout(const int Index);
    QDomLiteElement* Layout(const QString& Name);
    const int LayoutNumber(const QString& Name);
    const static QString LayoutName(QDomLiteElement* XMLLayout);
    const QString LayoutName(const int LayoutIndex);
    void setLayoutName(const int LayoutIndex, const QString Name);
private:
    QDomLiteDocument* m_XMLScore;
    bool isShadow;
};

class OCBarWindowBar
{
public:
    OCBarWindowBar();
    int Meter;
    QString MeterText;
    int Pointer;
    int Density;
    int Notes;
    bool IsFullRest;
    bool IsFullRestOnly;
    bool KeyChangeOnOne;
    bool ClefChangeOnOne;
    bool MasterStuff;
};

struct OCBarWindowVoice
{
    QList<OCBarWindowBar> Bars;
    QString Name;
    int Staff;
    int Voice;
    int NumOfVoices;
    int EndPointer;
    bool Incomplete;
};

class OCBarMap
{
public:
    OCBarMap();
    const int BarMapIndex(const int Staff, const int Voice) const;
    const int GetPointer(const int Bar, const int Staff, const int Voice) const;
    const int GetBar(const int Pointer, const int Staff, const int Voice) const;
    const int GetMeter(const int Bar, const int Staff, const int Voice) const;
    const int BarCountAll() const;
    const int BarCount(const int Staff, const int Voice) const;
    const int BarCountAll(QDomLiteElement* XMLTemplate) const;
    const bool IsEnded(const int Bar, QDomLiteElement* XMLTemplate) const;
    const int EndOfVoiceBar(const int Staff, const int Voice) const;
    const int NoteCount(const int Bar, const int Staff, const int Voice) const;
    const int NoteCountStaff(const int Staff, const int StartBar, const int EndBar) const;
    const bool IsFullRest(const int Bar, QDomLiteElement* XMLTemplate) const;
    const bool IsFullRestOnly(const int Bar, QDomLiteElement* XMLTemplate) const;
    const bool ClefChange(const int Bar, const int Staff, const int Voice) const;
    const bool KeyChange(const int Bar, const int Staff, const int Voice) const;
    const bool MasterStuff(const int Bar, const int Staff) const;
    QList<OCBarWindowVoice> Voices;
};

class OCSignType
{
public:
    OCSignType();
    OCSignType(XMLSymbolWrapper& Symbol);
    void Reset();
    int val;
    QPointF Pos;
    int Size;
};

class OCDurSignType : public OCSignType
{
public:
    OCDurSignType();
    OCDurSignType(XMLSymbolWrapper& Symbol, const bool ActualTicks=false);
    void Decrem(int Value);
    void Reset();
    int Ticks;
    int RemainingTicks;
};

class OCPrintStaffVarsType
{
public:
    OCPrintStaffVarsType(QDomLiteElement* XMLTemplateStaff);
    int Curly;
    int Square;
    int Height;
};

class OCMIDIVars
{
public:
    OCMIDIVars();
    void Reset();
    int Channel;
    int Patch;
    int Transpose;
    int Octave;
};

class OCPlayBackVarsType
{
public:
    OCPlayBackVarsType();
    int Currenttime;
    int Currentdynam;
    float crescendo;
    OCMIDIVars MIDI;
    bool ExpressionOn;
    int express;
    int changeexp;
    int exprbegin;
    int currentcresc;
    bool PortamentoOn;
    int PlayMeter;
    int Playtempo;
    int HoldTempo;
    int Accel;
    int currentlen;
    bool Fine;
    bool RepeatFromStart;
    int Volta;
    int CustomFlag0;
    int CustomFlag1;
    int CustomFlag2;
    int CustomFlag3;
    int CustomFlag4;
    int CustomFlag5;
    int CustomFlag6;
    int CustomFlag7;
    int CustomFlag8;
    int CustomFlag9;
};

enum SquareBracketConstants
{
    SBNone = 0,
    SBBegin = 1,
    SBEnd = 2
};

enum CurlyBracketConstants
{
    CBNone = 0,
    CBBegin = 2
};

enum NoteTypeCode
{
    tsnote = 0,
    tstiednote = 1,
    tspolynote = 2,
    tstiedpolynote = 3
};

enum OCRefreshMode
{
    tsReformat = 0,
    tsRedrawActiveStave = 1,
    tsNavigate = 2,
    tsVoiceIndexChanged = 3
};

enum OCTTF
{
    OCTTFSopranoClef = 0xf021,
    OCTTFBassClef = 0xf022,
    OCTTFAltoClef = 0xf023,
    OCTTFSharp = 0xf024,
    OCTTFPercussionClef = 0xf025,
    OCTTFStopped = 0xf02C,
    OCTTFNoteWhole = 0xf02D,
    OCTTFNoteHalf = 0xf02E,
    OCTTFNoteQuarter = 0xf02F,
    OCTTFAccent = 0xf03E,
    OCTTFSegno = 0xf040,
    OCTTFCoda = 0xf042,
    OCTTFAllaBreve = 0xf043,
    OCTTFmf = 0xf046,
    OCTTFmp = 0xf050,
    OCTTFFermata = 85+0xf000,
    OCTTFTrillb = 89+0xf000,
    OCTTFTrill = 96+0xf000,
    OCTTFTrillSharp = 97+0xf000,
    OCTTFFlat = 0xf062,
    OCTTFFourFour = 0xf064,
    OCTTFf = 0xf066,
    OCTTFOpl = 0xf06F,
    OCTTFp = 0xf070,
    OCTTFPraltrill = 123+0xf000,
    OCTTFMordent = 124+0xf000,
    OCTTFDobbelUp = 125+0xf000,
    OCTTFDobbelDown = 126+0xf000,
    OCTTFFinger0 = 132+0xf000,
    OCTTFRestTrirtytwo = 168+0xf000,
    OCTTFBowing0 = 177+0xf000,
    OCTTFDoubleFlat = 187+0xf000,
    OCTTFRestSixteen = 197+0xf000,
    OCTTFTremolo0 = 201+0xf000,
    OCTTFRestQuarter = 0xf0CE,
    OCTTFDoubleSharp = 220+0xf000,
    OCTTFRestEight = 228+0xf000,
    OCTTFRestSixtyfour = 244+0xf000,
    OCTTFBartok = 254+0xf000
};

//--------------------------------------------------------------------------

class OCToolButtonProps
{
public:
    OCToolButtonProps(const QString& ClassName,const int Button);
    QString classname;
    QString tooltip;
    QString iconpath;
    QString fontname;
    float fontsize;
    bool fontbold;
    bool fontitalic;
    QString buttonText;
    QString modifierProperty;
    int buttonindex;
    OCRefreshMode refreshmode;
    bool customdialog;
    bool ismodifier;
    bool ishidden;
};

//--------------------------------------------------------------------------

class OCCursor
{
public:
    OCCursor();
    const int SelEnd() const;
    const int SelStart() const;
    void AddSel(const int SymbolsIndex);
    void ExtendSel(const int SymbolsIndex);
    const int SelCount() const;
    const int Sel(const int Index) const;
    const bool IsSelected(const int SymbolsIndex) const;
    const bool IsMarked(const int SymbolsIndex) const;
    void SetPos(const int NewPos);
    void SetZero();
    const int GetPos() const;
    void MaxSel(const int MaxPointer);
    QList<int> Selected;
    const QList<int> Pointers() const;
private:
    int m_Pos;
};

//--------------------------------------------------------------------------

class CTextElement
{
public:
    QString Text;
    QString FontName;
    bool Italic;
    bool Bold;
    double Size;
    void Save(QDomLiteElement* data);
    void Load(QDomLiteElement* data);
    void Load(XMLTextWrapper& XMLText);
    XMLTextWrapper Save(const QString& Tag);
    void SetFonts(QWidget* parent);
    CTextElement();
    const QFont Font() const;
    void SetFont(const QFont& Font);
};

//--------------------------------------------------------------------------
class OCDraw
{
public:
    float ScreenSize;
    float XFactor;
    QColor col;
    bool ColorOn;
    OCCursor* Cursor;
    QGraphicsScene* Scene;
    void DI();
    const QList<QGraphicsItem*> DJ(const float xs, const float ys,const bool LineThickness=false);
    void DM(const QPointF Pos);
    void DM(const float xs, const float ys);
    void DM(const float xs, const float ys, const XMLSymbolWrapper& Symbol);
    void DR(const float xs, const float ys, const int Size=0);
    const QList<QGraphicsItem*> DL(const float a, const float b);
    const QList<QGraphicsItem*> plDot(const float offsetX=0,const float offsetY=0, const int Size=0);
    const QPainterPath TextPath(const QString& Letter, const int SignSize, const QString& Name, const bool Bold, const bool Italic, const int FontSize);
    const QList<QGraphicsItem*> plLet(const QString& Letter, const int SignSize, const QFont& Font, const Qt::Alignment Align=Qt::AlignLeft);
    const QList<QGraphicsItem*> plLet(const QString& Letter, const int SignSize, const QString& Name, const bool Bold, const bool Italic, const int FontSize, const Qt::Alignment Align=Qt::AlignLeft);
    const QList<QGraphicsItem*> plTextPath(const QPainterPath& path, const bool UsePen=false, const float PenWidth=-1);
    void translatePath(QPainterPath &p);
    const QList<QGraphicsItem*> PlRect(const float width, const float height, const int Size=0, const bool fill=true, const bool LineThickness=false);
    void PlSquare1(const int height, const int archeight, const int thickness);
    void PlSquare2(const int height, const int archeight, const int thickness);
    void PlCurly(const int height, const int width, const int thickness);
    void SetXY(const float X, const float Y);
    void setcol(const int Pointer);
    void setcol(const QColor color);
    void resetcol();
    const bool canColor() const;
    const bool IsSelected(int Pointer) const;
    const bool IsMarked(int Pointer) const;
    const int TextHeight(const CTextElement* TextElem, const int viewsize);
    const int TextWidth(const CTextElement* TextElem, const int viewsize);
    const int TextWidth(const QString& Text, const CTextElement* TextElem, const int viewsize);
    const QFont GetFont(const CTextElement* TextElem, const int viewsize);
    void PrintTextElement(const int X, const int Y, const QString& Text, const CTextElement* TextElem, const int viewsize);
    void PrintTextElement(int X, int Y, const CTextElement* TextElem, int viewsize);
    QGraphicsItemGroup* MakeGroup(const QList<QGraphicsItem*> &l);
    OCDraw();
    void StartList();
    const QList<QGraphicsItem*>& EndList();
    QRectF lastSlur;
private:
    bool UseList;
    QColor lastCol;
    QPointF ZeroPoint;
    QPointF MovingPoint;
    const QList<QGraphicsItem*> MyLine(const QPointF p1, const QPointF p2, const QColor color, const bool BF, const bool LineThickness);
    const QList<QGraphicsItem*> MyLine(const float x1, const float y1, const float x2, const float y2, const QColor color, const bool BF=false, const bool LineThickness=false);
    QList<QGraphicsItem*> items;
    void AppendToList(QGraphicsItem* item);
    void AppendToList(const QList<QGraphicsItem*>& list);
};

//--------------------------------------------------------------------------

class OCFrameProperties
{
public:
    OCFrameProperties(const int p);
    const QRectF TranslateBounding(const QPointF& Offset) const;
    const QRectF TranslateAccidental(const QPointF& Offset) const;
    QGraphicsItemGroup* group;
    QGraphicsItemGroup* accidentalGroup;
    QRectF BoundingRect;
    QRectF AccidentalRect;
    int Pointer;
};

//--------------------------------------------------------------------------

class OCFrame
{
private:
    QMacRubberband* RubberBand;
    void showAnimated(QRectF g);
public:
    OCFrame(QWidget* parent);
    ~OCFrame();
    void EraseFrame();
    void DrawFrame(const XMLSimpleSymbolWrapper& Symbol, QPointF Offset, const QPointF& zero, const bool fortegns, const int shiftit, OCFrameProperties* FrameProps, OCDraw& ScreenObj);
};

//--------------------------------------------------------------------------

class OCSymbolArray
{
private:
    QList<OCFrameProperties*> FrameList;
public:
    OCSymbolArray();
    ~OCSymbolArray();
    void AppendGroup(QGraphicsItemGroup* g, const int Pointer, const QRectF& bounding=QRectF());
    void AppendAccidentalGroup(QGraphicsItemGroup* g, QGraphicsItemGroup* a, const int Pointer);
    OCFrameProperties* RetrieveFromPointer(const int Pointer);
    const int Inside(const QPoint& m) const;
    const QList<int> PointersInside(const QRectF& r) const;
    void clear();
};

//--------------------------------------------------------------------------

struct acctype
{
    int faste;
    int current;
};

//--------------------------------------------------------------------------

struct OCStaffAccidentalItem
{
    int HasFortegn;
    int Pitch;
    int NoteNum;
};

class PrintSignProps : public OCSignType
{
public:
    PrintSignProps();
    void Fill(const int sign, const XMLSymbolWrapper& Symbol, const int pointer, const QColor color, const int modifier);
    void Position(const QPointF& NoteCenter, const float balkheight, const float tielen);
    int Sign;
    float Sizefactor;
    int Pointer;
    QColor Color;
    int Modifier;
    float HeightOnBalk;
    float TieLen;
    void DM(OCDraw& ScreenObj);
    void DM(const float OffsetX, const float OffsetY, OCDraw& ScreenObj);
    void DMVertical(const int UpDown, const float OffsetX, const float OffsetY, const float OffsetYbalk, OCDraw& ScreenObj);
private:
    QPointF InitPos;
};

class PlaySignProps
{
public:
    PlaySignProps();
    void Fill(const int duration, const int value, const int modulate);
    int Duration;
    int Value;
    int Modulate;
};

//--------------------------------------------------------------------------

const QString MakeUnicode(const QString& txt);
const int Sgn(const int Num);
const int Abs(const int Num);
const int Sgn(const float Num);
const float Abs(const float Num);
const double Abs(const double Num);
const int IntDiv(const int a, const int b);
const float FloatDiv(const float a, const float b);
const float FnSize(const float c, const int SymbolSize);
const float SizeFactor(const int SymbolSize);
const int Inside(int num, int Low, int High, int Step);

class OCTieWrap
{
private:
    QList<char> TieWrap;
public:
    OCTieWrap();
    bool EraseTies;
    void clear();
    void Add(const int Value);
    void plot(const int NoteX, const int NoteY, const int Alti, const int TieDirection, const int UpDown, OCDraw& ScreenObj);
    void PlotTie(const bool LastTie, const int Antal, const int UpDown, const int TieDirection, const int TieLen, const int Alti, int NextHeight, OCDraw& ScreenObj);
    void EraseTie();
    const static QList<QGraphicsItem*> PlSlur(int length, const int leftright, const int slant, int updown, const int curve, OCDraw& ScreenObj);
};

class OCPrintVarsType //'track
{
public:
    OCPrintVarsType();
    void Decrement(const int c);
    int FilePointer;
    int Meter;
    QString MeterText;
    int LowerMeter;
    int UpDown;
    int BalkLimit;
    bool SlantFlag;
    int J[12];
    int cueletter;
    OCMIDIVars MIDI;
    bool FlipTie;
    bool ClefChange;
    bool KeyChange;
    bool MasterStuff;

    OCTieWrap TieWrap;

    OCSignType CurrentClef;
    OCSignType CurrentKey;
    OCSignType Articulation;
    OCSignType Articulationx1;

    OCDurSignType SlurDown;
    OCDurSignType SlurUp;
    OCDurSignType crescendo;
    OCDurSignType diminuendo;
    OCDurSignType BalkOverRide;
    OCDurSignType Punktoverride;
    OCDurSignType StregOverRide;
    OCDurSignType UpDownOverRide;
    OCDurSignType SlantOverRide;

    int CustomFlag0;
    int CustomFlag1;
    int CustomFlag2;
    int CustomFlag3;
    int CustomFlag4;
    int CustomFlag5;
    int CustomFlag6;
    int CustomFlag7;
    int CustomFlag8;
    int CustomFlag9;
};

class DomAttributesModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    QDomLiteElement* m_Element;
    QMap<int,QString> m_Attributes;
public:
    DomAttributesModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void addColumn(int column, QString attributeName);
    void setXML(QDomLiteElement* XML);
};


#include "CommonCounters.h"
#include "MIDIFileClasses.h"

#endif // COMMONCLASSES_H
