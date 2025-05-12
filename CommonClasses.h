#ifndef COMMONCLASSES_H
#define COMMONCLASSES_H

#define expressiondefault 116
#define BarLeftMargin 204
#define BarRightMargin 48
#define ScoreLeftMargin (18*12)
#define ScoreStaffHeight 1200
#define ScoreStaffLinesHeight 384
#define ScoreTopSymbolY ScoreStaffHeight+(20*12)
#define ScoreBottomSymbolY (42*12)
#define ScoreTempoY (44 * 12) + ScoreStaffHeight
#define maxticks 255
#define vorschlagLength 30

#define midifl "/Users/Shared/oc.mid"
#define settingsfile "../OCstuff.xml"
#define OCTTFname "Object Composer"
#define WingDingsName "Wingdings 2"
#define BeamThickness 48
#define BeamSpace 24
#define HelpLineHalfWidth 84
#define LineHalfThickNess 4
#define AccidentalSpace 96

#define inactivestaffcolor Qt::darkGray
#define unselectablecolor QColor(0,0,0,180)
#define selectedcolor Qt::red
#define markedcolor Qt::blue
#define activestaffcolor Qt::black

#define renderinghints QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing

//#include <QVariant>
//#include <qmath.h>
//#include <QGraphicsScene>
#include <QGraphicsItem>
#include "qmacrubberband.h"
//#include <QIcon>
//#include <QAbstractTableModel>
#include <QModelIndex>
#include "ocxmlwrappers.h"
//#include "array"
#include <QGraphicsView>

template <typename T>

class QAverage
{
public:
    inline QAverage() { clear(); }
    inline QAverage(const T value) {
        clear();
        append(value);
    }
    inline QAverage(const QVector<T>& value) {
        clear();
        for (const T v : value) append(v);
    }
    inline void append(const T value) {
        m_Size++;
        m_Value+=value;
    }
    inline int size() { return m_Size; }
    inline bool isEmpty() { return (m_Size==0); }
    void clear() {
        m_Size=0;
        m_Value=0;
    }
    inline T average() {
        return (m_Size==0) ? 0 : T(m_Value/double(m_Size));
    }
private:
    int m_Size;
    T m_Value;
};

typedef QVector<int> OCIntList;

inline const QString MakeUnicode(QString txt)
{
    for (QChar& c : txt) if (c.unicode() <= 0xF000) c = QChar(c.unicode()+0xF000);
    return txt;
}
#ifndef OCPIANO_H
inline int IntDiv(const int a, const int b)
{
    return a/b;
}
#endif
inline float FloatDiv(const float a, const float b)
{
    return a/b;
}

inline double DoubleDiv(const double a, const double b)
{
    return a/b;
}

inline double SizeFactor(const int SymbolSize)
{
    return (SymbolSize == 0) ? 1 : DoubleDiv(10-SymbolSize,10);
}

template <typename T>

inline const T Sgn(const T Num)
{
    return (Num>0) ? 1 : (Num<0) ? -1 : 0;
}

template <typename T>

inline const T loBound(const T lo, const T num)
{
    return qMax<T>(lo,num);
}

template <typename T>

inline const T hiBound(const T num, const T hi)
{
    return qMin<T>(num,hi);
}

template <typename T>

inline const T boundStep(const T Low, T num, const T High, T Step=1)
{
    const T Differ = qMax<T>(1,Step);
    while (num > High) num -= Differ;
    while (num < Low) num += Differ;
    return num;
}

template <typename T>

inline const T boundRoll(const T Low, const T num, const T High)
{
    return boundStep<T>(Low, num, High, (High - Low) + 1);
}


#pragma pack(push,1)

class OCPageBar
{
public:
    inline OCPageBar(const int start) {
        startBar=start;
    }
    int startBar;
    int currentBar = 0;
    int actualBar = 0;
    inline int barNumber() const { return startBar+actualBar; }
    inline void increment() {
        currentBar++;
        actualBar++;
    }
    inline void incActual() { actualBar++; }
};

class OCSignType
{
public:
    inline OCSignType() {
        XMLSymbol.shadowXML(nullptr);
    }
    inline OCSignType(const XMLSimpleSymbolWrapper& Symbol,const int v = 0) {
        set(Symbol,v);
    }
    inline void set(const XMLSimpleSymbolWrapper& Symbol, const int v = 0) {
        XMLSymbol.shadowXML(Symbol.xml());
        val = v;
    }
    inline void unset() {
        XMLSymbol.shadowXML(nullptr);
        val = 0;
    }
    inline bool isSet() const {
        return (XMLSymbol.xml() != nullptr);
    }
    int val = 0;
    XMLSimpleSymbolWrapper XMLSymbol;
};

class OCDurSignType : public OCSignType
{
public:
    inline OCDurSignType() : OCSignType() {}
    inline OCDurSignType(const XMLSimpleSymbolWrapper& Symbol) : OCSignType(Symbol) { set(Symbol); }
    inline void set(const XMLSimpleSymbolWrapper& Symbol, const int v = 0) {
        OCSignType::set(Symbol,v);
        Counter=0;
        Ticks=Symbol.xml()->attributeValueInt("Ticks")+1;
        RemainingTicks=Ticks;
    }
    inline void Decrem(const double Value) {
        Counter++;
        RemainingTicks = loBound<double>(0,RemainingTicks - Value);
    }
    inline bool remains() const { return (RemainingTicks > 0); }
    double RemainingTicks = 0;
    int Counter = 0;
private:
    int Ticks = 0;
};

enum StemDirection
{
    StemDown=-1,
    StemAuto=0,
    StemUp=1
};

class OCVoiceSign
{
public:
    inline OCVoiceSign(){}
    OCSignType CurrentSign;
    OCDurSignType DuratedSign;
    OCSignType x1Sign;
    bool x1Set = false;
    inline void setCurrent(const XMLSimpleSymbolWrapper& Symbol, const int Val=0) {
        CurrentSign.set(Symbol,Val);
    }
    inline void setDurated(const XMLSimpleSymbolWrapper& Symbol, const int Val=0) {
        DuratedSign.set(Symbol,Val);
    }
    inline void setX1(const XMLSimpleSymbolWrapper& Symbol, const int Val=0) {
        x1Sign.set(Symbol,Val);
        x1Set=true;
    }
    inline void decrem(const double Ticks) {
        DuratedSign.Decrem(Ticks);
        x1Sign.unset();
        x1Set=false;
    }
    inline OCSignType& getSign() {
        if (x1Set) {
            x1Set=false;
            return x1Sign;
        }
        return (DuratedSign.remains()) ? DuratedSign : CurrentSign;
    }
    inline int getValue(int val) {
        const int v = getSign().val;
        return (v) ? v : val;
    }
};

typedef QVector<OCDurSignType> OCDurSignVector;

class OCDurSignList : public OCDurSignVector
{
public:
    bool contains(const QString& name) const { return (indexOf(name) > -1); }
    void decrement(const double ticks) {
        for (int i = size() - 1; i >= 0; i--) {
            (*this)[i].Decrem(ticks);
            if (!(*this)[i].remains()) removeAt(i);
        }
    }
    const OCDurSignType& sign(const int index) const { return (*this)[index]; }
    const OCDurSignVector signs(const QString& name) const {
        OCDurSignVector l;
        for (OCDurSignType s : (*this)) if (s.XMLSymbol.Compare(name)) l.append(s);
        return l;
    }
    int indexOf(const QString& name) const {
        for (int i = 0; i < size(); i++) if ((*this)[i].XMLSymbol.Compare(name)) return i;
        return -1;
    }
    void append(const XMLSimpleSymbolWrapper& symbol) { OCDurSignVector::append(OCDurSignType(symbol)); }
};

class OCMIDIVars
{
public:
    OCMIDIVars() {}
    int Channel = 0;
    int Patch = 0;
    int Transpose = 0;
    int Octave = 0;
    inline int pitch(const int p) const { return boundStep<int>(1, p + (Octave * 12) + Transpose, 127, 12); }
};

struct OCPlayBackVarsType
{
    int Pointer = 0;
    int CurrentDelta = 0;
    int VoicedTime = 0;
    int Currentdynam = 70;
    float crescendo = 0;
    OCMIDIVars MIDI;
    bool ExpressionOn = true;
    int express = 0;
    int changeexp = 0;
    int exprbegin = expressiondefault;
    int currentcresc = 0;
    bool PortamentoOn = true;
    int PortamentoTime = 0x10;
    int PlayMeter = 96;
    int Playtempo = 120;
    int HoldTempo = 120;
    int Accel = 0;
    int AccelCounter = 0;
    int currentlen = 80;
    bool Fine = false;
    OCIntList Repeat = {-1};
    int PlayRepeat = 0;
    int Volta = 0;
    OCIntList PedalNotes;
    OCIntList VorschlagNotes;
};

typedef QVector<OCPlayBackVarsType> OCPlayVarsArray;

enum NoteTypeCode
{
    tsnote = 0,
    tstiednote = 1,
    tspolynote = 2,
    tstiedpolynote = 3,
    tsgracenote = 4,
    tstiedgracenote
};

enum OCRefreshMode
{
    tsReformat = 0,
    tsRedrawActiveStave = 1,
    tsNavigate = 2,
    tsVoiceIndexChanged = 3
};

//#define __Lelandfont
enum WingDings {
    WDDot = 0xF098,
    WDX = 0xF0CF,
    WDDiamond = 0xF0AF,
    WDCircle = 0xF09A
};

enum OCTTF
{
    OCTTFSopranoClef = 0xf021,
    OCTTFBassClef = 0xf022,
    OCTTFAltoClef = 0xf023,
    OCTTFSharp = 0xf024,
    OCTTFPercussionClef = 0xf025,
    OCTTFPedalUp = 0xf02B,
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
    OCTTFPedalDown = 0xf0A1,
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

#ifdef __Lelandfont

#define LelandDefaultSize 272

enum Leland {
    LelandSopranoClef = 0xe050,
    LelandBassClef = 0xe062,
    LelandAltoClef = 0xe05c,
    LelandSharp = 0xe262,
    LelandPercussionClef = 0xe069,
    LelandPedalUp = 0xe655,
    LelandStopped = 0xe5e5,
    LelandNoteWhole = 0xe0a2,
    LelandNoteHalf = 0xe0a3,
    LelandNoteQuarter = 0xe0a4,
    LelandAccent = 0xe4a0,
    LelandSegno = 0xe047,
    LelandCoda = 0xe048,
    LelandAllaBreve = 0xe088,
    Lelandmf = 0xe52d,
    Lelandmp = 0xe52c,
    LelandFermata = 0xe4c0,
    //LelandTrillb = 89+0xf000,
    LelandTrill = 0xe566,
    //LelandTrillSharp = 97+0xf000,
    LelandFlat = 0xe260,
    LelandFourFour = 0xe08a,
    Lelandf = 0xe522,
    LelandOpl = 0xe261,
    Lelandp = 0xe520,
    LelandPraltrill = 0xe56d,
    LelandMordent = 0xe56c,
    LelandDobbelUp = 0xe567,
    LelandDobbelDown = 0xe568,
    LelandFinger0 = 0xed10,
    LelandPedalDown = 0xe650,
    LelandRestTrirtytwo = 0xe4e8,
    LelandBowing0 = 177+0xf000,
    LelandDoubleFlat = 187+0xf000,
    LelandRestSixteen = 0xe4e7,
    LelandTremolo0 = 201+0xf000,
    LelandRestQuarter = 0xe4e5,
    LelandDoubleSharp = 220+0xf000,
    LelandRestEight = 0xe4e6,
    LelandRestSixtyfour = 0xe4e9,
    LelandBartok = 0xe630
};
#endif

//--------------------------------------------------------------------------

class OCToolButtonProps
{
public:
    OCToolButtonProps(const QString& ClassName,const int Button)
    {
        classname=ClassName;
        buttonindex=Button;
    }
    QString classname;
    QString category;
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
    bool ismodifier = false;
    bool ishidden = false;
};

//--------------------------------------------------------------------------

class OCCursorRow
{
public:
    inline OCCursorRow() {}
    inline OCCursorRow(const OCSymbolLocation& l) : VoiceLocation(l) { AddSel(l.Pointer); }
    OCVoiceLocation VoiceLocation;
    inline int SelEnd() const { return m_Selected.last(); }
    inline int SelStart() const { return m_Selected.first(); }
    inline OCSymbolRange Range() const { return OCSymbolRange(SelStart(),SelEnd()); }
    inline void AddSel(const int SymbolsIndex) {
        if (m_Selected.contains(SymbolsIndex)) return;
        if (m_Selected.empty()) {
            m_Selected.append(SymbolsIndex);
            return;
        }
        if (SymbolsIndex > m_Selected.last()) {
            m_Selected.append(SymbolsIndex);
            return;
        }
        for (int i = 0; i < m_Selected.size(); i++) {
            if (SymbolsIndex < m_Selected[i]) {
                m_Selected.insert(i,SymbolsIndex);
                return;
            }
        }
    }
    inline void ExtendSel(const int SymbolsIndex) {
        if (m_Selected.isEmpty()) {
            AddSel(SymbolsIndex);
            return;
        }
        while (SymbolsIndex > m_Selected.last()) m_Selected.append(m_Selected.last()+1);
        while (SymbolsIndex < m_Selected.first()) m_Selected.prepend(m_Selected.first()-1);
    }
    inline void ExtendSel(const OCSymbolRange& r) {
        ExtendSel(r.Start);
        ExtendSel(r.End);
    }
    inline void AddSel(const OCSymbolRange& r) {
        for (int i = r.Start; i <= r.End; i++) AddSel(i);
    }
    inline int SelCount() const { return m_Selected.size(); }
    inline bool IsSelected(const int SymbolsIndex) const { return m_Selected.contains(SymbolsIndex); }
    inline void SetPos(const int NewPos) {
        m_Selected.clear();
        AddSel(NewPos);
    }
    inline void SetRange(const int s, const int e) {
        SetPos(s);
        ExtendSel(e);
    }
    inline void SetRange(const OCSymbolRange& r) {
        SetRange(r.Start,r.End);
    }
    inline void MaxSel(const int MaxPointer)
    {
        while ((!m_Selected.empty()) && (m_Selected.last() >= MaxPointer)) m_Selected.removeLast();
    }
    inline OCPointerList SelectedPointers() const { return m_Selected; }
    void serialize(QDomLiteElement* xml) const {
        QStringList l;
        for (const int& i : m_Selected) l.append(QString::number(i));
        VoiceLocation.serialize(xml->elementByTagCreate("Location"));
        xml->setAttribute("Pointers",l.join(','));
    }
    void unserialize(const QDomLiteElement* xml) {
        if (QDomLiteElement* l = xml->elementByTag("Location")) VoiceLocation.unserialize(l);
        m_Selected.clear();
        const QStringList l = xml->attribute("Pointers").split(',');
        for (const QString& p : l) m_Selected.append(p.toInt());
    }
private:
    OCPointerList m_Selected;
};

class OCCursorGrid
{
public:
    inline int indexOf(const OCVoiceLocation& l) const {
        for (int i=0;i<m_Selected.size();i++) if (m_Selected[i].VoiceLocation.matches(l)) return i;
        return -1;
    }
    inline int SelEnd(const OCVoiceLocation& l) const {
        const int i = indexOf(l);
        return (i > -1) ? m_Selected[i].SelEnd() : -1;
    }
    inline int SelStart(const OCVoiceLocation& l) const {
        const int i = indexOf(l);
        return (i > -1) ? m_Selected[i].SelStart() : -1;
    }
    inline OCSymbolRange Range(const OCVoiceLocation& l) const {
        const int i = indexOf(l);
        return (i == -1) ? OCSymbolRange() : OCSymbolRange(m_Selected[i].SelStart(),m_Selected[i].SelEnd());
    }
    inline void AddSel(const OCSymbolLocation& l) {
        const int i = indexOf(l);
        if (i == -1){
            AddRow(l);
            return;
        }
        m_Selected[i].AddSel(l.Pointer);
    }
    inline void ExtendSel(const OCSymbolLocation& l) {
        const int i = indexOf(l);
        if (i == -1) {
            AddRow(l);
            return;
        }
        m_Selected[i].ExtendSel(l.Pointer);
    }
    inline void ExtendSel(const OCSymbolRange& r, const OCVoiceLocation& l) {
        const int i = indexOf(l);
        if (i == -1) {
            AddRow(OCSymbolLocation(l,r.Start));
            m_Selected[0].ExtendSel(r);
            return;
        }
        m_Selected[i].ExtendSel(r);
    }
    inline void AddSel(const OCSymbolRange& r, const OCVoiceLocation& l) {
        const int i = indexOf(l);
        if (i == -1) {
            AddRow(OCSymbolLocation(l,r.Start));
            m_Selected[0].AddSel(r);
            return;
        }
        m_Selected[i].AddSel(r);
    }
    inline int SelCount(const OCVoiceLocation& l) const {
        const int i = indexOf(l);
        return (i == -1) ? 0 : m_Selected[i].SelCount();
    }
    inline int RowCount() const { return m_Selected.size(); }
    inline bool IsSelected(const OCSymbolLocation& l) const {
        const int i = indexOf(l);
        return (i == -1) ? false : m_Selected[i].IsSelected(l.Pointer);
    }
    inline void SetPos(const OCSymbolLocation& l) {
        m_Selected.clear();
        AddSel(l);
    }
    inline void SetRange(const OCSymbolRange& r, const OCVoiceLocation& l) {
        SetPos(OCSymbolLocation(l,r.Start));
        ExtendSel(r,l);
    }
    inline void ClearSel() { m_Selected.clear(); }
    inline void MaxSel(const OCSymbolLocation& l) {
        const int i = indexOf(l);
        if (i == -1) return;
        m_Selected[i].MaxSel(l.Pointer);
        if (m_Selected[i].SelCount() == 0) m_Selected.remove(i);
    }
    inline const OCPointerList SelectedPointers(const OCVoiceLocation& l) const {
        const int i = indexOf(l);
        return (i == -1) ? OCPointerList() : m_Selected[i].SelectedPointers();
    }
    void serialize(QDomLiteElement* xml) const {
        for (const OCCursorRow& r : m_Selected) r.serialize(xml->elementByTagCreate("Row"));
    }
    void unserialize(const QDomLiteElement* xml) {
        const QDomLiteElementList rows = xml->elementsByTag("Row");
        if (!rows.empty()) {
            m_Selected.clear();
            for (const QDomLiteElement* r : rows) {
                OCCursorRow c;
                c.unserialize(r);
                m_Selected.append(c);
            }
        }
    }
private:
    QVector<OCCursorRow> m_Selected;
    inline void AddRow(const OCSymbolLocation& l) {
        m_Selected.append(OCCursorRow(l));
    }
};

class OCCursor
{
public:
    inline OCCursor() : m_Location(0,0,0) {}
    inline int SelEnd() const
    {
        return (m_Selected.SelCount(m_Location)) ? m_Selected.SelEnd(m_Location) : m_Location.Pointer;
    }
    inline int SelStart() const
    {
        return (m_Selected.SelCount(m_Location)) ? m_Selected.SelStart(m_Location) : m_Location.Pointer;
    }
    inline OCSymbolRange Range() const { return m_Selected.Range(m_Location); }
    inline void AddSel(const int SymbolsIndex)
    {
        m_Location.Pointer = SymbolsIndex;
        m_Selected.AddSel(OCSymbolLocation(m_Location,SymbolsIndex));
    }
    inline void ExtendSel(const int SymbolsIndex, const int max = -1)
    {
        m_Selected.ExtendSel(OCSymbolLocation(m_Location,SymbolsIndex));
        if (max > -1) MaxSel(max);
    }
    inline void ExtendSel(const OCSymbolRange& r, const int max = -1)
    {
        m_Selected.ExtendSel(r,m_Location);
        if (max > -1) MaxSel(max);
    }
    inline void ExtendSel(const OCPointerList& l, const int max = -1) {
        ExtendSel(OCSymbolRange(l));
        if (max > -1) MaxSel(max);
    }
    //inline void ExtendSel(const OCLocationList& l) {
    //    ExtendSel(l.pointers());
    //}
    inline void AddSel(const OCSymbolRange& r, const int max = -1)
    {
        m_Selected.AddSel(r,m_Location);
        if (max > -1) MaxSel(max);
    }
    inline void AddSel(const OCPointerList& l, const int max = -1) {
        if (l.empty()) return;
        for (int i : l) AddSel(i);
        if (max > -1) MaxSel(max);
    }
    inline void SetSel(const OCSymbolRange& r, const int max = -1) {
        SetPos(r.Start);
        ExtendSel(r.End,max);
    }
    inline void SetSel(const OCPointerList& l, const int max = -1) {
        if (!l.empty()) SetPos(l.first());
        AddSel(l,max);
    }
    inline int SelCount() const { return m_Selected.SelCount(m_Location); }
    inline bool IsSelected(const int SymbolsIndex) const { return m_Selected.IsSelected(OCSymbolLocation(m_Location,SymbolsIndex)); }
    inline bool IsMarked(const int SymbolsIndex) const {
        return (m_Location.Pointer==SymbolsIndex) ? true : m_Selected.IsSelected(OCSymbolLocation(m_Location,SymbolsIndex));
    }
    inline void SetPos(const int NewPos, const int max = -1) {
        m_Location.Pointer = NewPos;
        m_Selected.SetPos(OCSymbolLocation(m_Location,NewPos));
        if (max > -1) MaxSel(max);
    }
    inline void setLocation(const OCSymbolLocation& l) {
        m_Location = l;
        SetPos(l.Pointer);
    }
    inline void SetZero(const OCSymbolLocation& l) {
        m_Location = l;
        m_Selected.ClearSel();
    }
    inline void SetRange(const int s, const int e, const int max = -1) {
        SetRange(OCSymbolRange(s,e),max);
    }
    inline void SetRange(const OCSymbolRange& r, const int max = -1) {
        m_Location.Pointer=r.Start;
        m_Selected.ClearSel();
        m_Selected.SetRange(r,m_Location);
        if (max > -1) MaxSel(max);
    }
    inline void SetZero(const int EOV) {
        m_Selected.ClearSel();
        m_Location.Pointer = EOV;
    }
    inline int currentPointer() const { return m_Location.Pointer; }
    inline const OCSymbolLocation location() const { return m_Location; }
    inline void MaxSel(const int MaxPointer) {
        const OCSymbolLocation l(m_Location, MaxPointer);
        m_Selected.MaxSel(l);
        if (m_Selected.SelCount(m_Location)) {
            if (m_Location.Pointer >= MaxPointer) m_Location.Pointer = m_Selected.SelEnd(m_Location);
        }
        else {
            if (m_Location.Pointer > MaxPointer) {
                m_Location.Pointer = MaxPointer;
            }
        }
    }
    inline const OCPointerList SelectedPointers() const { return m_Selected.SelectedPointers(m_Location); }
    void serialize(QDomLiteElement* xml) const {
        QDomLiteElement* c = xml->elementByTagCreate("Cursor");
        m_Selected.serialize(c->elementByTagCreate("Grid"));
        m_Location.serialize(c->elementByTagCreate("Location"));
    }
    void unserialize(const QDomLiteElement* xml) {
        if (const QDomLiteElement* c = xml->elementByTag("Cursor")) {
            if (const QDomLiteElement* g = c->elementByTag("Grid")) {
                m_Selected.unserialize(g);
            }
            if (const QDomLiteElement* l = c->elementByTag("Location")) m_Location.unserialize(l);
        }
    }
    void backup() {
        serialize(&m_Backup);
    }
    void restore() {
        unserialize(&m_Backup);
    }
private:
    OCSymbolLocation m_Location;
    OCCursorGrid m_Selected;
    QDomLiteElement m_Backup;
};

//--------------------------------------------------------------------------

typedef QList<QGraphicsItem*> OCGraphicsList;

class OCDraw
{
public:
    inline OCDraw() : ScreenSize(12), col(activestaffcolor), ColorOn(true), UseList(false) {}
    inline OCDraw(QGraphicsScene* scene, const double size) : ScreenSize(size), col(activestaffcolor), ColorOn(false), Scene(scene), UseList(false) {}
    double ScreenSize;
    double XFactor;
    QColor col;
    bool ColorOn;
    OCCursor* Cursor;
    QGraphicsScene* Scene;
    inline void setSpaceX(const int staffSize) { XFactor = SizeFactor(staffSize); }
    inline double spaceX(const double x) const { return x * XFactor; }
    inline void initCurrent() { ZeroPoint = MovingPoint*XFactor; }
    inline void moveTo(const QPointF& Pos) { moveTo(Pos.x(),Pos.y()); }
    inline void moveTo(const double xs, const double ys) { MovingPoint=ZeroPoint+QPointF(xs,-ys); }
    inline void moveTo(const double xs, const double ys, const XMLSymbolWrapper& Symbol)
    {
        moveTo(xs+Symbol.left(), ys+Symbol.top());
    }
    inline void move(const double xs, const double ys, const int Size = 0)
    {
        MovingPoint += (Size != 0) ? QPointF(xs,-ys)/SizeFactor(Size) : QPointF(xs,-ys);
    }
    inline const OCGraphicsList line(const QPointF& Pos, const double a, const double b, const double bold = 1) {
        moveTo(Pos);
        return DJ(a,b,bold);
    }
    inline const OCGraphicsList line(const QPointF& Pos1, const QPointF& Pos2, const double bold = 1) {
        moveTo(Pos1);
        return DJ(Pos2.x()-Pos1.x(),Pos2.y()-Pos1.y(),bold);
    }
    inline const OCGraphicsList line(const double a, const double b, const double bold = 1) { return DJ(a,b,bold); }
    inline const OCGraphicsList line(const double x, const double y, const double a, const double b, const double bold = 1)
    {
        moveTo(x,y);
        return DJ(a,b,bold);
    }
    inline const OCGraphicsList plDot(const int SignSize, const double offsetX = 0,const double offsetY = 0, const int Size = 0)
    {
        OCGraphicsList l;
        //move(offsetX-36,offsetY,Size);
        //l.append(plLet(".",Size,"Courier new",false,false,100,Qt::AlignCenter));
        move(offsetX,offsetY,Size);
        l.append(plChar(WDDot,SignSize,24,WingDingsName));
        move(-offsetX,-offsetY,Size);
        //move(-(offsetX-36),-(offsetY),Size);
        return l;
    }
    inline const QPainterPath TextPath(const QString& Letter, const int SignSize, const QString& Name, const bool Bold, const bool Italic, const double FontSize)
    {
        if (Letter.isEmpty()) return QPainterPath();
        QFont F(Name);
        F.setBold(Bold);
        F.setItalic(Italic);
        F.setPointSizeF(DoubleDiv(FontSize,SizeFactor(SignSize))/ScreenSize);
//#ifdef __MACOSX_CORE__
        const QPointF Pos=(MovingPoint+QPointF(0,DoubleDiv(FontSize,8)))/ScreenSize;
        F.setPointSizeF(F.pointSizeF()*1.30);
//#else
//        int x1=FloatDiv(MovingPoint.x() - 0.1, sizx);
//        int y1=FloatDiv(MovingPoint.y() - (Size + FloatDiv(Size ,6)), sizy);
//#endif
        QPainterPath path;
        path.addText(Pos,F,Letter);
        path.setFillRule(Qt::WindingFill);
        return path;
    }
    inline const OCGraphicsList plLet(const OCTTF letter, const int SignSize, const double FontSize=1200, const Qt::Alignment Align=Qt::AlignLeft)
    {
        return plLet(QChar(uint(letter)),SignSize,OCTTFname,false,false,FontSize,Align);
    }
    inline const OCGraphicsList plChar(const uint ch, const int SignSize, const double FontSize = 1200, const QString FontName = OCTTFname, const bool FontBold = false, const Qt::Alignment Align = Qt::AlignCenter)
    {
        return plLet(QChar(ch),SignSize,FontName,FontBold,false,FontSize,Align);
    }
#ifdef __Lelandfont
    inline const OCGraphicsList plLet(const Leland letter, const int SignSize, const double FontSize=LelandDefaultSize, const Qt::Alignment Align=Qt::AlignLeft)
    {
        return plLet(QChar(letter),SignSize,"Leland",false,false,FontSize,Align);
    }
#endif
    inline const OCGraphicsList plLet(const QString& Letter, const int SignSize, const QFont& Font, const Qt::Alignment Align=Qt::AlignLeft)
    {
        return plLet(Letter, SignSize, Font.family(), Font.bold(), Font.italic(), Font.pointSizeF(), Align);
    }
    inline const OCGraphicsList plLet(const QString& Letter, const int SignSize, const QString& Name, const bool Bold, const bool Italic, const double FontSize, const Qt::Alignment Align=Qt::AlignLeft)
    {
        if (Letter.isEmpty()) return OCGraphicsList();
        QPainterPath p=TextPath(Letter,SignSize,Name,Bold,Italic,FontSize);
        if (Align & Qt::AlignBottom) p.translate(0,-DoubleDiv(FontSize,8)/ScreenSize);
        if (Align & Qt::AlignRight) p.translate(-p.boundingRect().width(),0);
        if (Align & Qt::AlignHCenter) p.translate(-p.boundingRect().width()/2,0);
        if (Align & Qt::AlignVCenter) p.translate(0,(DoubleDiv(MovingPoint.y(),ScreenSize)-p.boundingRect().top())-p.boundingRect().height()/2);
        return plTextPath(p);
    }
    inline const OCGraphicsList plTextPath(const QPainterPath& path, const bool UsePen=false, const double PenWidth = -1, const bool Fill = false)
    {
        OCGraphicsList l;
        QBrush b=QBrush(col);
        QPen p(Qt::NoPen);
        if (UsePen)
        {
            if (PenWidth > -1)
            {
                b = (Fill == true) ? QBrush(col) : QBrush(Qt::NoBrush);
                p = QPen(QBrush(col),PenWidth);
                p.setCapStyle(Qt::RoundCap);
            }
            else
            {
                p = QPen(QBrush(col),1);
            }
        }
        else
        {
            if (canColor())
            {
                if ((col == selectedcolor) || (col == markedcolor)) p = QPen(col);
            }
        }
        QGraphicsItem* i = Scene->addPath(path,p,b);
        l.append(i);
        AppendToList(i);
        return l;
    }
    inline void translatePath(QPainterPath &p) { p.translate(MovingPoint / ScreenSize); }
    inline const OCGraphicsList PlRect(const double width, const double height, const int Size=0, const bool fill=true, const bool LineThickness=false)
    {
        OCGraphicsList l;
        const double factor=SizeFactor(Size);
        QBrush b(col);
        if (!fill) b=QBrush(Qt::NoBrush);
        QPointF ep(MovingPoint+(QPointF(width,-height)/factor));
        QRectF r=QRectF(MovingPoint/ScreenSize,ep/ScreenSize).normalized();

        QPen p(col);
        if (LineThickness) p.setWidth(int(DoubleDiv(LineHalfThickNess*4,ScreenSize)/factor));
        QGraphicsItem* i=Scene->addRect(r,p,b);
        AppendToList(i);
        l.append(i);
        return l;
    }
    inline const OCGraphicsList PlIcon(const QIcon& icon, const double width, const double height, const int Size=0)
    {
        OCGraphicsList l;
        const double factor=SizeFactor(Size);
        const QPointF ep(MovingPoint+(QPointF(width,-height)/factor));
        const QRectF r=QRectF(MovingPoint/ScreenSize,ep/ScreenSize).normalized();
        QGraphicsItem* i = Scene->addPixmap(icon.pixmap(r.size().toSize()));
        i->moveBy(r.left(),r.top());
        AppendToList(i);
        l.append(i);
        return l;
    }
    void PlSquare1(const double h, const double ah, const double t)
    {
        const double height=DoubleDiv(h,ScreenSize);
        const double archeight=DoubleDiv(ah,ScreenSize);
        const double thickness=DoubleDiv(t,ScreenSize);
        const double arcfragment=archeight/10.0;
        if (isZero(ah))
        {
            QPainterPath s(QPointF(0,0));
            s.lineTo(0,height);
            s.lineTo(thickness,height);
            s.lineTo(thickness,0);
            s.lineTo(0,0);
            AppendToList(Scene->addPath(s.translated(MovingPoint/ScreenSize),QPen(col),QBrush(col)));
        }
        else
        {
            QPainterPath s(QPointF(archeight,-arcfragment*6.0));
            s.cubicTo(QPointF(8.0*arcfragment,-4.0*arcfragment),QPointF(6.0*arcfragment,-2.0*arcfragment),QPointF(0,0));
            s.lineTo(0,height);
            s.lineTo(thickness,height);
            s.lineTo(thickness,0);
            s.cubicTo(QPointF(thickness+(3.0*arcfragment),-2.0*arcfragment),QPointF(archeight,-4.0*arcfragment),QPointF(archeight,-arcfragment*6.0));
            AppendToList(Scene->addPath(s.translated(MovingPoint/ScreenSize),QPen(col),QBrush(col)));
        }
    }
    void PlSquare2(const double h, const double ah, const double t)
    {
        const double height=DoubleDiv(h,ScreenSize);
        const double archeight=DoubleDiv(ah,ScreenSize);
        const double thickness=DoubleDiv(t,ScreenSize);
        const double arcfragment=archeight/10.0;
        QPainterPath s(QPointF(0,0));
        s.lineTo(0,height);
        s.cubicTo(QPointF(6.0*arcfragment,height+(2.0*arcfragment)),QPointF(8.0*arcfragment,height+(4.0*arcfragment)),QPointF(archeight,height+(6.0*arcfragment)));
        s.cubicTo(QPointF(archeight,height+(4.0*arcfragment)),QPointF(thickness+(3.0*arcfragment),height+(2.0*arcfragment)),QPointF(thickness,height));
        s.lineTo(thickness,0);
        s.lineTo(0,0);
        AppendToList(Scene->addPath(s.translated(MovingPoint/ScreenSize),QPen(col),QBrush(col)));
    }
    void PlCurly(const double h, const double w, const double t)
    {
        const double height=DoubleDiv(h,ScreenSize);
        const double width=DoubleDiv(w,ScreenSize);
        const double thickness=DoubleDiv(t,ScreenSize);
        const double ydist=height/10.0;
        const double xdist=width/2.0;
        QPainterPath b(QPointF(0,0));
        b.cubicTo(QPointF(-xdist*4.0,ydist),QPointF(xdist,height/2.0),QPointF(-xdist*2,height/2.0));
        b.cubicTo(QPointF(xdist,height/2.0),QPointF(-xdist*4.0,height-ydist),QPointF(0,height));
        b.cubicTo(QPointF(-(xdist*4.0)+thickness,height-(ydist/2.0)),QPointF(xdist+thickness,(height/2.0)+(ydist/2.0)),QPointF(-xdist*2.0,height/2.0));
        b.cubicTo(QPointF(xdist+thickness,(height/2.0)-(ydist/2.0)),QPointF(-(xdist*4.0)+thickness,ydist/2.0),QPointF(0,0));
        AppendToList(Scene->addPath(b.translated(MovingPoint/ScreenSize),QPen(col),QBrush(col)));
    }
    inline void init(const double X, const double Y) { ZeroPoint=QPointF(X,Y); }
    inline void setcol(const int Pointer)
    {
        if (canColor())
        {
            lastCol=col;
            if (Cursor->IsSelected(Pointer))
            {
                col=selectedcolor;
            }
            else if (Cursor->currentPointer()==Pointer)
            {
                col=markedcolor;
            }
        }
    }
    inline void setcol(const QColor& color)
    {
        if (canColor())
        {
            lastCol=col;
            col=color;
        }
    }
    inline void resetcol() { if (canColor()) col=lastCol; }
    inline bool canColor() const
    {
        return (ColorOn & (col != inactivestaffcolor));
    }
    inline bool IsSelected(const int Pointer) const
    {
        return (canColor()) ? Cursor->IsSelected(Pointer) : false;
    }
    inline bool IsMarked(const int Pointer) const
    {
        return (canColor()) ? Cursor->IsMarked(Pointer) : false;
    }
    void PrintFontElement(const double X, const double Y, const QString& Text, const XMLFontWrapper& FontElem, const double scalesize)
    {
        QGraphicsSimpleTextItem* item =Scene->addSimpleText(Text);
        QFont F(FontElem.printerFont());
        F.setPointSizeF(F.pointSizeF()/scalesize);
        item->setFont(F);
        item->setPos(X,Y);
        item->setPen(Qt::NoPen);
        item->setBrush(Qt::black);
        AppendToList(item);
    }
    void PrintTextElement(const double X, const double Y, const XMLTextElementWrapper& TextElem, const double scalesize)
    {
        PrintFontElement(X,Y,TextElem.text(), TextElem, scalesize);
    }
    inline QGraphicsItemGroup* MakeGroup(const OCGraphicsList &l)
    {
        if (l.isEmpty()) return nullptr;
        if (!ColorOn) return nullptr;
        return Scene->createItemGroup(l);
    }
    inline const QRectF boundingRect(const OCGraphicsList &l)
    {
        /*
        QGraphicsItemGroup* g=Scene->createItemGroup(l);
        return (g) ? g->boundingRect() : QRectF();
        */
        QRectF r;
        for (QGraphicsItem* i : l) r = r.united(i->boundingRect());
        return r;
    }
    inline void StartList()
    {
        UseList=true;
        items.clear();
    }
    inline const OCGraphicsList& EndList()
    {
        UseList=false;
        return items;
    }
    QRectF lastSlur;
private:
    bool UseList;
    QColor lastCol;
    QPointF ZeroPoint;
    QPointF MovingPoint;
    inline const OCGraphicsList DJ(const double xs, const double ys,const double bold = 1)
    {
        QPointF temp=MovingPoint/ScreenSize;
        MovingPoint+=QPointF(xs,-ys);
        return MyLine(temp, MovingPoint / ScreenSize, col, false, bold);
    }
    inline const OCGraphicsList MyLine(const QPointF p1, const QPointF p2, const QColor& color, const bool BF, const double bold = 1)
    {
        return MyLine(p1.x(), p1.y(), p2.x(), p2.y(), color, BF, bold);
    }
    inline const OCGraphicsList MyLine(const double x1, const double y1, const double x2, const double y2, const QColor& color, const bool BF=false, const double bold = 1)
    {
        OCGraphicsList l;
        if (BF)
        {
            QRectF r=QRectF(x1,y1,x2,y2).normalized();
            QGraphicsItem* i=Scene->addRect(r,QPen(color),QBrush(color));
            AppendToList(i);
            l.append(i);
            return l;
        }
        QPen p(color);
        //if (!isOne(bold))
        //{
            p.setWidth(qRound(DoubleDiv(LineHalfThickNess * 3.0 * bold , ScreenSize)));
            p.setCapStyle(Qt::RoundCap);
        //}
        QGraphicsItem* i=Scene->addLine(x1,y1,x2,y2,p);
        AppendToList(i);
        l.append(i);
        return l;
    }
    OCGraphicsList items;
    inline void AppendToList(QGraphicsItem* item) { if (UseList) items.append(item); }
    inline void AppendToList(const OCGraphicsList& list) { if (UseList) items.append(list); }
};

//--------------------------------------------------------------------------

class OCFrameProperties
{
public:
    inline OCFrameProperties(const OCSymbolLocation& l) : Location(l) {}
    inline OCFrameProperties(const QRectF& r, const OCSymbolLocation& l) : BoundingRect(r), Location(l) {}
    inline OCFrameProperties(const QRectF& r, const QRectF& ra,  const QRectF& rt, const OCSymbolLocation& l) : BoundingRect(r),AccidentalRect(ra),TieRect(rt), Location(l) {}
    inline const QRectF TranslateBounding(const QPointF& Offset = QPoint()) const
    {
        return translateRect(BoundingRect, Offset);
    }
    inline const QRectF TranslateAccidental(const QPointF& Offset = QPointF()) const
    {
        return translateRect(AccidentalRect, Offset);
    }
    inline const QRectF TranslateTie(const QPointF& Offset = QPointF()) const
    {
        return translateRect(TieRect, Offset);
    }
    inline const QRectF translateRect(const QRectF& r, const QPointF& Offset = QPointF()) const
    {
        return r.adjusted(-4,-4,4,4).translated(Offset);
    }
public:
    QRectF BoundingRect;
    QRectF AccidentalRect;
    QRectF TieRect;
    OCSymbolLocation Location;
};

//--------------------------------------------------------------------------

class OCCursorFrame : public QMacRubberband
{
public:
    OCCursorFrame(QWidget* parent) : QMacRubberband(QRubberBand::Rectangle, QMacRubberband::MacRubberbandYellow, parent) {}
    ~OCCursorFrame() {}
    void showAnimated(const QRectF& g)
    {
        QGraphicsView* v = reinterpret_cast<QGraphicsView*>(parent());
        QRectF gg(v->mapFromScene(g.topLeft().toPoint()),v->mapFromScene(g.bottomRight().toPoint()));
        if (!isVisible())
        {
            if (gg.toRect() == geometry())
            {
                show();
            }
            else
            {
                setGeometry(gg.toRect());
                QMacRubberband::showAnimated();
            }
        }
        else
        {
            setGeometry(gg.toRect());
        }
    }
    void showAnimated(const QPointF& Offset, const bool fortegns, const OCFrameProperties& FrameProps)
    {
        if (fortegns)
        {
            if (qAbs<qreal>(Offset.x()) >= qAbs<qreal>(Offset.y()))
            {
                showAnimated(FrameProps.TranslateAccidental());
            }
            else
            {
                showAnimated(FrameProps.TranslateTie());
            }
        }
        else
        {
            showAnimated(FrameProps.TranslateBounding());
        }
    }
};

//--------------------------------------------------------------------------
static OCFrameProperties nullFrame = OCFrameProperties(OCSymbolLocation());

class OCFrameArray : public QList<OCFrameProperties>
{
private:
    inline QRectF getBoundingRect(QGraphicsItemGroup* g)
    {
        if (g == nullptr) return QRectF();
        return widenRect(g->boundingRect().normalized());
    }
    inline QRectF widenRect(QRectF r)
    {
        if (r.width() < 4) r.adjust(-2, 0, 2, 0);
        if (r.height() < 4) r.adjust(0, -2, 0, 2);
        return r;
    }
public:
    OCFrameArray(){}
    void AppendGroup(QGraphicsItemGroup* g, const OCSymbolLocation& l, const QRectF& bounding = QRectF())
    {
        if (g==nullptr) return;
        if (g->childItems().empty()) return;
        const QRectF r = (bounding.isNull()) ? getBoundingRect(g) : widenRect(bounding.normalized());
        OCFrameProperties F = OCFrameProperties(r,l);
        append(F);
    }
    void AppendAccidentalGroup(QGraphicsItemGroup* g, QGraphicsItemGroup* a, QGraphicsItemGroup* t, const OCSymbolLocation& l)
    {
        if (g==nullptr) return;
        if (g->childItems().empty()) return;
        OCFrameProperties F=OCFrameProperties(getBoundingRect(g),getBoundingRect(a),getBoundingRect(t),l);
        append(F);
    }
    inline const OCFrameProperties& RetrieveFromPointer(const OCSymbolLocation& l) const
    {
        for (const OCFrameProperties& Frame : *this) if (Frame.Location.matches(l)) return Frame;
        return nullFrame;
    }
    inline int Nearest(const double y) const
    {
        for (const OCFrameProperties& Frame : *this)
        {
            if (Frame.BoundingRect.left() > y) return Frame.Location.Pointer;
        }
        return -1;
    }
    inline const OCSymbolLocation Inside(const QPointF& m) const
    {
        for (int i = size() - 1; i >= 0; i--)
        {
            const OCFrameProperties& Frame = (*this)[i];
            if (Frame.BoundingRect.contains(m)) return Frame.Location;
        }
        return OCSymbolLocation();
    }
    inline const OCLocationList locationsInside(const QRectF& r) const
    {
        OCLocationList ptrs;
        for (const OCFrameProperties& Frame : *this)
        {
            if (r.contains(Frame.BoundingRect)) ptrs.push_back(Frame.Location);
        }
        return ptrs;
    }
};

//--------------------------------------------------------------------------

enum OCNoteAccidentalTypes
{
    noteAccDirty=-3,
    noteAccDoubleFlat=-2,
    noteAccFlat=-1,
    noteAccNone=0,
    noteAccSharp=1,
    noteAccDoubleSharp=2
};

typedef std::array<int,12> OCScaleArray;

class acctype
{
public:
    inline acctype() {}
    inline void reset() {
        isSet = false;
        current = noteAccNone;
    }
    bool isSet = false;
    OCNoteAccidentalTypes current = noteAccNone;
};

//--------------------------------------------------------------------------
enum OCAccidentalSymbols
{
    accNone=0,
    accFlat,
    accSharp,
    accDoubleFlat,
    accDoubleSharp,
    accNatural
};

enum OCKeyAccidental
{
    keyAccSharps=-1,
    keyAccNone=0,
    keyAccFlats=1
};

class OCNoteAccidental
{
public:
    inline OCNoteAccidental(const int line=0,const int noteNum=0)
    {
        AccSymbol=accNone;
        LineNum=uint(line);
        NoteNum=noteNum;
    }
    OCAccidentalSymbols AccSymbol;
    uint LineNum;
    int NoteNum;
    inline int OneOctaveNoteNum() const { return NoteNum % 12; }
    OCNoteAccidentalTypes getAccSign(const OCScaleArray& Scale) const
    {
        switch (OneOctaveNoteNum())
        {
        case 0:
            return Switch(Scale[0],noteAccSharp,noteAccDoubleFlat,noteAccNone);
        case 1:
            return Switch(Scale[1],noteAccDoubleSharp,noteAccFlat,noteAccSharp);
        case noteAccDoubleSharp:
            return Switch(Scale[2],noteAccDoubleSharp,noteAccDoubleFlat,noteAccNone);
        case 3:
            return Switch(Scale[3],noteAccSharp,noteAccDoubleFlat,noteAccFlat);
        case 4:
            return Switch(Scale[4],noteAccDoubleSharp,noteAccFlat,noteAccNone);
        case 5:
            return Switch(Scale[5],noteAccSharp,noteAccDoubleFlat,noteAccNone);
        case 6:
            return Switch(Scale[6],noteAccDoubleSharp,noteAccFlat,noteAccSharp);
        case 7:
            return Switch(Scale[7],noteAccDoubleSharp,noteAccDoubleFlat,noteAccNone);
        case 8:
            return Switch(Scale[8],noteAccSharp,noteAccFlat,noteAccFlat);
        case 9:
            return Switch(Scale[9],noteAccDoubleSharp,noteAccDoubleFlat,noteAccNone);
        case 10:
            return Switch(Scale[10],noteAccSharp,noteAccDoubleFlat,noteAccFlat);
        case 11:
            return Switch(Scale[11],noteAccDoubleSharp,noteAccFlat,noteAccNone);
        }
        return noteAccNone;
    }
    inline void setSymbol(const OCNoteAccidentalTypes AccSign) { AccSymbol = Sign2Symbol(AccSign); }
private:
    inline OCAccidentalSymbols Sign2Symbol(const OCNoteAccidentalTypes AccSign) const
    {
        switch (AccSign)
        {
        case noteAccNone: return accNatural;
        case noteAccFlat: return accFlat;
        case noteAccSharp: return accSharp;
        case noteAccDoubleFlat: return accDoubleFlat;
        case noteAccDoubleSharp: return accDoubleSharp;
        default: return accNone;
        }
    }
    inline OCNoteAccidentalTypes Switch(const int In, const OCNoteAccidentalTypes Out1, const OCNoteAccidentalTypes Out2, const OCNoteAccidentalTypes OutElse) const
    {
        switch (In)
        {
        case 1:
            return Out1;
        case 2:
            return Out2;
        default:
            return OutElse;
        }
    }
};

class PrintSignProps : public OCSignType
{
public:
    PrintSignProps() {}
    void fill(const XMLSimpleSymbolWrapper& Symbol, const OCSymbolLocation& location, const QColor& color)
    {
        set(Symbol);
        Location=location;
        Color=color;
    }
    inline void setPosition(const QPointF& NoteCenter, const double balkheight, const double tielen)
    {
        HeightOnBalk = balkheight + XMLSymbol.pos().y();
        Pos = XMLSymbol.pos() + NoteCenter;
        TieLen = tielen;
    }
    OCSymbolLocation Location;
    QPointF Pos;
    QColor Color;
    double HeightOnBalk;
    double TieLen;
    inline int size() const { return XMLSymbol.size(); }
    inline void moveTo(OCDraw& ScreenObj)
    {
        ScreenObj.moveTo(Pos);
    }
    inline void moveTo(const double OffsetX, const double OffsetY, OCDraw& ScreenObj)
    {
        ScreenObj.moveTo(Pos.x() + OffsetX, Pos.y() + OffsetY);
    }
    inline void moveToVertical(const StemDirection UpDown, const double OffsetY, OCDraw& ScreenObj)
    {
        (UpDown == StemDown) ? ScreenObj.moveTo(Pos.x(), HeightOnBalk + OffsetY) : ScreenObj.moveTo(Pos.x(), Pos.y() + OffsetY);
    }
    inline void moveToBelow(const StemDirection UpDown, const double OffsetY, OCDraw& ScreenObj)
    {
        (UpDown == StemUp) ? ScreenObj.moveTo(Pos.x(), HeightOnBalk + OffsetY) : ScreenObj.moveTo(Pos.x(), Pos.y() + OffsetY);
    }

};

class PlaySignProps
{
public:
    PlaySignProps() {}
    void fill(const int duration, const int modulate)
    {
        Duration=duration;
        Modulate=modulate;
    }
    int Duration;
    int Modulate;
};

//--------------------------------------------------------------------------

class OCTieWrap
{
private:
    QList<char> TieWrap;
public:
    OCTieWrap() : EraseTies(false) {}
    bool EraseTies;
    void clear() { TieWrap.clear(); }
    inline void append(const int Value) { TieWrap.append(char(Value)); }
    void plotWrappedTie(const int NoteX, const int NoteY, const int Pitch, const int TieDirection, const int UpDown, OCDraw& ScreenObj)
    {
        if (TieWrap.contains(char(Pitch)))
        {
            ScreenObj.moveTo(NoteX, NoteY);
            ScreenObj.move(-240, 0);
            plotSlur(QPointF(-20 * 12, 0), (UpDown * TieDirection), 0, ScreenObj);
        }
    }
    OCGraphicsList plotTie(const bool IsWrap, const int Count, const int UpDown, const int TieDirection, const int TieLen, const int CenterY, int NextCenterY, OCDraw& ScreenObj, double bold = 1)
    {
        if (IsWrap | (Count != 1))
        {
            ScreenObj.move(-84, 0);
            return plotSlur(QPointF(TieLen + 168, 0), (UpDown * TieDirection), 0, ScreenObj, bold);
        }
        else
        {
            qDebug() << CenterY << NextCenterY;
            //if (NextCenterY <= 0) NextCenterY = CenterY;
            if (NextCenterY != CenterY)
            {
                ScreenObj.move(-84, 0);// ' UpDown * 24
                return plotSlur(QPointF(TieLen + 168, CenterY - NextCenterY), UpDown, 0, ScreenObj, bold);
            }
            else
            {
                ScreenObj.move(-84, 0);// ' UpDown * 24
                return plotSlur(QPointF(TieLen + 168, 0), UpDown, 0, ScreenObj, bold);
            }
        }
    }
    void eraseTie()
    {
        if (EraseTies)
        {
            clear();
            EraseTies = false;
        }
    }
    const static OCGraphicsList plotSlur(QPointF endpoint, const int updown, const double curve, OCDraw& ScreenObj, double bold = 1.0)
    {
        OCGraphicsList l;
        int moveleft = 0;
        if (qAbs(endpoint.x()) < 9 * 12) {
            moveleft = (7 * 12) - qAbs(endpoint.x());
            endpoint.setX(9 * 12 * Sgn(endpoint.x()));
        }
        const double direction = -updown;
        const QPointF start(0, 0);
        const QPointF end = endpoint / ScreenObj.ScreenSize;
        const double thickness = (LineHalfThickNess * 4.0 / ScreenObj.ScreenSize) * bold;
        const QVector2D dir = QVector2D(end).normalized();
        const QVector2D normal = QVector2D(-dir.y(),dir.x()) * direction; // vinkelrätt på bågen

        double xaddp1 = normal.x() * (end.x() / 2.0);// * direction;
        double xaddp2 = xaddp1 / 12.0;
        if (dir.y() * direction > 0) qSwap(xaddp1, xaddp2);

        const double curveFactor = 1.0 + (curve / 50.0);
        const double tiltFactor = 1.0 + qAbs(normal.x() / 4.0);
        const double curvature = ((72.0 / ScreenObj.ScreenSize) + (qAbs(end.x()) / 12.0)) * tiltFactor * curveFactor * direction;

        const QPointF p1((end.x() * 0.1) + xaddp1, curvature);
        const QPointF p2((end.x() * 0.9) + xaddp2, end.y() + curvature);
        const QPointF thick(normal.x() * thickness, thickness * direction);
        const QPointF thin = (thick * 2) / LineHalfThickNess;

        QPainterPath b(start);
        b.cubicTo(p1, p2, end);
        b.lineTo(end - thin);
        b.cubicTo(p2 - thick, p1 - thick, start - thin);
        b.lineTo(start);
        b.translate(QPointF((8 * 12) - moveleft, (-updown * 5 * 12)) / ScreenObj.ScreenSize);
        ScreenObj.translatePath(b);
        ScreenObj.lastSlur=b.boundingRect();
        l.append(ScreenObj.plTextPath(b,true,bold,true));
        return l;
    }
};

class OCPrintVarsType //'track
{
public:
    OCPrintVarsType() {}
    inline void Decrement(const double c)
    {
        DurSigns.decrement(c);
        UpDown.decrem(c);
        SlantFlag.decrem(c);
        Articulation.decrem(c);
        KeyChange=false;
        ClefChange=false;
        MasterStuff=false;
    }
    inline void setKey(const XMLSimpleSymbolWrapper& Symbol)
    {
        CurrentKey.set(Symbol);
        KeyChange=true;
    }
    inline int key() const
    {
        return (!CurrentKey.isSet()) ? 0 : CurrentKey.XMLSymbol.getIntVal("Key")-6;
    }
    inline void setClef(const XMLSimpleSymbolWrapper& Symbol)
    {
        CurrentClef.set(Symbol);
        ClefChange=true;
    }
    inline int clef() const
    {
        return (!CurrentClef.isSet()) ? 0 : CurrentClef.XMLSymbol.getIntVal("Clef")+1;
    }
    int FilePointer = 0;
    int Meter = 96;
    QString MeterText = "4/4";
    int BalkLimit = 24;
    OCScaleArray Scale = {0};
    int cueletter = 0;
    OCMIDIVars MIDI;
    bool FlipTie = false;
    bool ClefChange = false;
    bool KeyChange = false;
    bool MasterStuff = false;

    OCTieWrap TieWrap;
    OCIntList Ties;

    OCSignType CurrentClef;
    OCSignType CurrentKey;
    OCVoiceSign Articulation;
    OCVoiceSign SlantFlag;
    OCVoiceSign UpDown;

    OCDurSignList DurSigns;
};

typedef QVector<OCPrintVarsType> OCPrintVarsArray;

class DomAttributesModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    QDomLiteElement* m_Element;
    QMap<int,QString> m_Attributes;
public:
    DomAttributesModel(QObject */*parent*/) : m_Element(nullptr) {}
    int rowCount(const QModelIndex &/*parent*/ = QModelIndex()) const { return 1; }
    int columnCount(const QModelIndex &/*parent*/ = QModelIndex()) const
    {
        return (m_Element==nullptr) ? 0 : m_Attributes.count();
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if ((role == Qt::EditRole) | (role == Qt::DisplayRole))
        {
            return m_Element->attribute(m_Attributes[index.column()]);
        }
        return QVariant();
    }
    bool setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (role == Qt::EditRole)
        {
            m_Element->setAttribute(m_Attributes[index.column()], value);
            emit dataChanged(index,index);
        }
        return true;
    }
    void addColumn(int column, QString attributeName) { m_Attributes.insert(column,attributeName); }
    void setXML(QDomLiteElement* XML) { m_Element=XML; }
};

#pragma pack(pop)

#endif // COMMONCLASSES_H
