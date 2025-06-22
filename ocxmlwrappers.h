#ifndef OCXMLWRAPPERS_H
#define OCXMLWRAPPERS_H

#include <QStringList>
#include <QDomLite>
//#include <QPointF>
#include <QFontDialog>
#include "softsynthsdefines.h"
#include "cpitchtextconvert.h"

#define defaultscorezoom 0.25
#define defaultlayoutzoom 0.2
#define defaultscreensize 3
#define defaultscoresize 12
#define defaultnotespace 16

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

typedef QVector<int> OCPointerList;

class OCSymbolRange
{
public:
    inline OCSymbolRange(const int s=0, const int e=0)
    {
        Start=s;
        End=e;
        if (End<=Start) End=Start;
    }
    inline OCSymbolRange(const OCPointerList& l) {
        if (l.empty()) return;
        Start = l.first();
        for (const int& i : l) {
            if (i < Start) Start = i;
            if (i > End) End = i;
        }
        if (End <= Start) End = Start;
    }
    int Start = 0;
    int End = 0;
};

typedef QList<OCSymbolRange> OCSelectionList;

class OCPatternNote
{
public:
    inline OCPatternNote(const int b, const int t)
    {
        Button=b;
        TripletDotFlag=t;
    }
    inline OCPatternNote(const int b, const int Dot, const bool Triplet)
    {
        Button=b;
        TripletDotFlag=int(Triplet);
        if (Dot) TripletDotFlag=Dot+1;
    }
    int Button;
    int TripletDotFlag;
    inline bool isDoubleDot() const { return (TripletDotFlag==3); }
    inline bool isDot() const { return (TripletDotFlag==2); }
    inline int dot() const { return qMax<int>(TripletDotFlag-1,0); }
    inline bool isTriplet() const { return (TripletDotFlag==1); }
};

typedef QList<OCPatternNote> OCPatternNoteList;

class OCStaffLocation
{
public:
    inline OCStaffLocation(const int s=0) : StaffId(s) {}
    inline bool matches(const int s) const
    {
        return (StaffId==s);
    }
    inline bool matches(const OCStaffLocation& l) const
    {
        return (StaffId==l.StaffId);
    }
    int StaffId;
    void serialize(QDomLiteElement* xml) const {
        xml->setAttribute("StaffId",StaffId);
    }
    void unserialize(const QDomLiteElement* xml) {
        StaffId = xml->attributeValueInt("StaffId");
    }
};

class OCVoiceLocation : public OCStaffLocation
{
public:
    inline OCVoiceLocation(const OCStaffLocation& s, const int v=0) : OCStaffLocation (s.StaffId),Voice(v) {}
    inline OCVoiceLocation(const int s=0, const int v=0) : OCStaffLocation (s),Voice(v) {}
    inline bool matches(const int s, const int v) const
    {
        return ((StaffId==s) & (Voice==v));
    }
    inline bool matches(const OCVoiceLocation& l) const
    {
        return ((StaffId==l.StaffId) & (Voice==l.Voice));
    }
    int Voice;
    void serialize(QDomLiteElement* xml) const {
        OCStaffLocation::serialize(xml);
        xml->setAttribute("Voice",Voice);
    }
    void unserialize(const QDomLiteElement* xml) {
        OCStaffLocation::unserialize(xml);
        Voice = xml->attributeValueInt("Voice");
    }
};

class OCSymbolLocation : public OCVoiceLocation
{
public:
    inline OCSymbolLocation() : OCVoiceLocation(),Pointer(-1) {}
    inline OCSymbolLocation(const int s, const int v, const int p) : OCVoiceLocation(s,v),Pointer(p) {}
    inline OCSymbolLocation(const OCVoiceLocation& v, const int p) : OCVoiceLocation(v.StaffId,v.Voice),Pointer(p) {}
    inline bool matches(const int s, const int v, const int p) const
    {
        return ((StaffId==s) & (Voice==v) & (Pointer==p));
    }
    inline bool matches(const OCSymbolLocation& l) const
    {
        return ((StaffId==l.StaffId) & (Voice==l.Voice) & (Pointer==l.Pointer));
    }
    int Pointer;
    void serialize(QDomLiteElement* xml) const {
        OCVoiceLocation::serialize(xml);
        xml->setAttribute("Pointer",Pointer);
    }
    void unserialize(const QDomLiteElement* xml) {
        OCVoiceLocation::unserialize(xml);
        Pointer = xml->attributeValueInt("Pointer");
    }
};

class OCBarSymbolLocation;

class OCBarLocation : public OCVoiceLocation
{
public:
    inline OCBarLocation() : OCVoiceLocation(),Bar(0) {}
    inline OCBarLocation(const int s, const int v, const int b) : OCVoiceLocation(s,v),Bar(b) {}
    inline OCBarLocation(const OCVoiceLocation& v, const int b) : OCVoiceLocation(v.StaffId,v.Voice),Bar(b) {}
    OCBarLocation(const OCBarSymbolLocation& b);
    int Bar;
    void serialize(QDomLiteElement* xml) const {
        OCVoiceLocation::serialize(xml);
        xml->setAttribute("Bar",Bar);
    }
    void unserialize(const QDomLiteElement* xml) {
        OCVoiceLocation::unserialize(xml);
        Bar = xml->attributeValueInt("Bar");
    }
};

class OCBarSymbolLocation : public OCSymbolLocation
{
public:
    inline OCBarSymbolLocation() : OCSymbolLocation(),Bar(0) {}
    inline OCBarSymbolLocation(const int b, const int s, const int v, const int p) : OCSymbolLocation(s,v,p),Bar(b) {}
    inline OCBarSymbolLocation(const int b, const OCVoiceLocation& v, const int p) : OCSymbolLocation(v,p),Bar(b) {}
    inline OCBarSymbolLocation(const OCBarLocation& b, const int p) : OCSymbolLocation(b.StaffId,b.Voice,p),Bar(b.Bar) {}
    inline OCBarSymbolLocation(const int b, const OCSymbolLocation& s) : OCSymbolLocation(s.StaffId,s.Voice,s.Pointer),Bar(b) {}
    int Bar;
    void serialize(QDomLiteElement* xml) const {
        OCSymbolLocation::serialize(xml);
        xml->setAttribute("Bar",Bar);
    }
    void unserialize(const QDomLiteElement* xml) {
        OCSymbolLocation::unserialize(xml);
        Bar = xml->attributeValueInt("Bar");
    }
};

class OCLocationList : public QVector<OCSymbolLocation> {
public:
    OCLocationList() : QVector<OCSymbolLocation>() {}
    OCLocationList(const OCVoiceLocation& v, const OCPointerList& pointers) : QVector<OCSymbolLocation>() {
        for (const int i : pointers) append(OCSymbolLocation(v,i));
    }
    const OCPointerList pointers() const {
        OCPointerList l;
        for (int i = 0; i < size(); i++) l.append(at(i).Pointer);
        return l;
    }
    const OCLocationList matchingVoice(const OCVoiceLocation& v) const {
        OCLocationList l;
        for (int i = 0; i < size(); i++) if (v.matches(at(i))) l.append(at(i));
        return l;
    }
};

class OCBarSymbolLocationList : public QList<OCBarSymbolLocation> {
public:
    const OCLocationList matchingVoice(const OCVoiceLocation& v) const {
        OCLocationList l;
        for (int i = 0; i < size(); i++) if (v.matches(at(i))) l.append(at(i));
        return l;
    }
};

class IXMLWrapper
{
public:
    inline IXMLWrapper()
    {
        isShadow=false;
        XMLElement=new QDomLiteElement;
    }
    inline IXMLWrapper(const QString& tag)
    {
        isShadow=false;
        XMLElement=new QDomLiteElement(tag);
    }
    inline IXMLWrapper(QDomLiteElement* e)
    {
        isShadow=true;
        XMLElement=e;
    }
    inline IXMLWrapper(const IXMLWrapper& other)
    {
        isShadow=true;
        XMLElement=other.XMLElement;
        setSubElements();
    }
    inline void operator = (const IXMLWrapper& other)
    {
        shadowXML(other.xml());
    }
    virtual ~IXMLWrapper();
    virtual void shadowXML(QDomLiteElement* e)
    {
        if (!isShadow) delete XMLElement;
        isShadow=true;
        XMLElement=e;
        setSubElements();
    }
    virtual void setXML(QDomLiteElement* e)
    {
        if (!isShadow) delete XMLElement;
        isShadow=false;
        XMLElement=e;
        setSubElements();
    }
    virtual void copy(const QDomLiteElement* xml)
    {
        XMLElement->copy(xml);
        setSubElements();
    }
    virtual void copy(const IXMLWrapper& other)
    {
        copy(other.xml());
    }
    inline QDomLiteElement* xml() const { return XMLElement; }
    void setSubElements() {}
protected:
    bool isShadow;
    QDomLiteElement* XMLElement;
};

class CStringCompare
{
protected:
    QString stringVal;
public:
    inline CStringCompare() {}
    inline CStringCompare(const QString& str) : stringVal(str) {}
    inline CStringCompare(const char* str) : stringVal(str) {}
    inline bool Compare(const QStringList& S) const { return S.contains(stringVal); }
    inline bool Compare(const char* S1) const { return (stringVal==QLatin1String(S1)); }
    inline bool Compare(const char* S1,const char* S2) const { return ((stringVal==QLatin1String(S1)) || (stringVal==QLatin1String(S2))); }
    inline bool Compare(const char* S1,const char* S2,const char* S3) const { return ((stringVal==QLatin1String(S1)) || (stringVal==QLatin1String(S2)) || (stringVal==QLatin1String(S3))); }
    inline bool Compare(const char* S1,const char* S2,const char* S3,const char* S4) const { return ((stringVal==QLatin1String(S1)) || (stringVal==QLatin1String(S2)) || (stringVal==QLatin1String(S3)) || (stringVal==QLatin1String(S4))); }
    inline bool Compare(const char* S1,const char* S2,const char* S3,const char* S4,const char* S5) const { return ((stringVal==QLatin1String(S1)) || (stringVal==QLatin1String(S2)) || (stringVal==QLatin1String(S3)) || (stringVal==QLatin1String(S4)) || (stringVal==QLatin1String(S5))); }
    inline bool Compare(const QString& S1) const { return (stringVal==S1); }
    inline bool Compare(const QString& S1,const QString& S2) const { return ((stringVal==S1) || (stringVal==S2)); }
    inline bool Compare(const QString& S1,const QString& S2,const QString& S3) const { return ((stringVal==S1) || (stringVal==S2) || (stringVal==S3)); }
    inline bool Compare(const QString& S1,const QString& S2,const QString& S3,const QString& S4) const { return ((stringVal==S1) || (stringVal==S2) || (stringVal==S3) || (stringVal==S4)); }
    inline bool Compare(const QString& S1,const QString& S2,const QString& S3,const QString& S4,const QString& S5) const { return ((stringVal==S1) || (stringVal==S2) || (stringVal==S3) || (stringVal==S4) || (stringVal==S5)); }
};

class CTagCompare : public CStringCompare
{
public:
    inline CTagCompare(const QDomLiteElement* e) :
        CStringCompare(e->tag) {}
};

class CNoteCompare : public CStringCompare , public IXMLWrapper
{
public:
    inline CNoteCompare() : CStringCompare(), IXMLWrapper("Symbol") {}
    inline CNoteCompare(const QString& str) : CStringCompare(str), IXMLWrapper("Symbol")
    {
        XMLElement->setAttribute("SymbolName",str);
    }
    inline CNoteCompare(QDomLiteElement* XML) : CStringCompare(XML->attribute("SymbolName")), IXMLWrapper(XML) {}
    void copy(const QDomLiteElement* xml)
    {
        if (xml) stringVal=xml->attribute("SymbolName");
        IXMLWrapper::copy(xml);
    }
    void shadowXML(QDomLiteElement* e);
    void setXML(QDomLiteElement* e)
    {
        IXMLWrapper::setXML(e);
        if (e) stringVal=e->attribute("SymbolName");
    }
    inline bool IsNoteType(const bool Note, const bool TiedNote=false, const bool CompoundNote=false, const bool TiedCompoundNote=false) const
    {
        if (Compare("Note"))
        {
            int NoteType=XMLElement->attributeValueInt("NoteType");
            if (Note) if (NoteType==0) return true;
            if (TiedNote) if (NoteType==1) return true;
            if (CompoundNote) if (NoteType==2) return true;
            if (TiedCompoundNote) if (NoteType==3) return true;
        }
        return false;
    }
    inline bool IsRestOrNoteType(const bool Note, const bool TiedNote=false, const bool CompoundNote=false, const bool TiedCompoundNote=false) const
    {
        return (IsRest()) ? true : IsNoteType(Note,TiedNote,CompoundNote,TiedCompoundNote);
    }
    inline bool IsSingleNote() const { return IsNoteType(true); }
    inline bool IsSingleTiedNote() const { return IsNoteType(false, true); }
    inline bool IsRestOrValuedNote() const { return IsRestOrNoteType(true,true); }
    inline bool IsTiedNote() const { return IsNoteType(false,true,false,true); }
    inline bool IsValuedNote() const { return IsNoteType(true,true); }
    inline bool IsCompoundNote() const { return IsNoteType(false,false,true,true); }
    inline bool IsSingleCompoundNote() const { return IsNoteType(false,false,true,false); }
    inline bool IsTiedCompoundNote() const { return IsNoteType(false,false,false,true); }
    inline bool IsRestOrAnyNote() const { return IsRestOrNoteType(true,true,true,true); }
    inline bool IsPitchedNote() const { return (IsAnyNote() || IsAnyVorschlag()); }
    inline bool IsAnyNote() const { return IsNoteType(true,true,true,true); }
    inline bool IsRest() const { return Compare("Rest"); }
    inline bool IsTuplet() const { return Compare("Tuplet"); }
    inline bool IsTime() const { return Compare("Time"); }
    inline bool IsKey() const { return Compare("Key"); }
    inline bool IsClef() const { return Compare("Clef"); }
    inline bool IsAnyVorschlag() const { return (XMLElement->attributeValueInt("NoteType") >= 4); }
    inline bool IsVorschlag() const { return (XMLElement->attributeValueInt("NoteType") == 4); }
    inline bool IsTiedVorschlag() const { return (XMLElement->attributeValueInt("NoteType") == 5); }
};

#define XMLProperty(GETTER,SETTER,ATTRNAME,DEFVAL,TYPE) inline TYPE GETTER() const { return TYPE(XMLElement->attributeValue(#ATTRNAME,DEFVAL)); } inline void set ## SETTER(const TYPE v) { XMLElement->setAttribute(#ATTRNAME,v,DEFVAL); }
#define XMLIntProperty(GETTER,SETTER,ATTRNAME,DEFVAL) inline int GETTER() const { return XMLElement->attributeValueInt(#ATTRNAME,DEFVAL); } inline void set ## SETTER(const int v) { XMLElement->setAttribute(#ATTRNAME,v,DEFVAL); }
#define XMLBoolProperty(GETTER,SETTER,ATTRNAME,DEFVAL) inline bool GETTER() const { return XMLElement->attributeValueBool(#ATTRNAME,DEFVAL); } inline void set ## SETTER(const bool v) { XMLElement->setAttribute(#ATTRNAME,v,DEFVAL); }
#define XMLStringProperty(GETTER,SETTER,ATTRNAME,DEFVAL) inline const QString GETTER() const { return XMLElement->attribute(#ATTRNAME,DEFVAL); } inline void set ## SETTER(const QString& v) { XMLElement->setAttribute(#ATTRNAME,v,DEFVAL); }

class XMLSimpleSymbolWrapper : public CNoteCompare
{
public:
    inline XMLSimpleSymbolWrapper() : CNoteCompare() {}
    inline XMLSimpleSymbolWrapper(const QString& str) : CNoteCompare(str) {}
    inline XMLSimpleSymbolWrapper(QDomLiteElement* XMLVoice, const int Pointer) : CNoteCompare(XMLVoice->childElement(Pointer)) {}
    inline XMLSimpleSymbolWrapper(QDomLiteElement* XMLSymbol) : CNoteCompare(XMLSymbol) {}
    void shadowXML(QDomLiteElement* e);
    XMLIntProperty(left,Left,Left,0)
    XMLIntProperty(top,Top,Top,0)
    XMLIntProperty(pitch,Pitch,Pitch,0)
    XMLIntProperty(noteValue,NoteValue,NoteValue,0)
    XMLIntProperty(dotted,Dotted,Dotted,0)
    XMLBoolProperty(triplet,Triplet,Triplet,false)
    inline bool compare(const XMLSimpleSymbolWrapper& other) { return XMLElement->compare(other.xml()); }
    inline bool isValid() const { return !stringVal.isEmpty(); }
    inline const QString name() const { return stringVal; }
    inline int size() const { return getIntVal("Size"); }
    inline const QPointF pos() const { return QPointF(left(),top()); }
    inline const QPointF move(const QPointF p) const { return pos()+p; }
    inline const QPointF move(const double x, const double y) const { return pos()+QPointF(x,y); }
    inline double moveX(const double x) const { return pos().x()+x; }
    inline double moveY(const double y) const { return pos().y()+y; }
    inline double getVal(const QString& Tag) const { return XMLElement->attributeValue(Tag); }
    inline int getIntVal(const QString& Tag) const { return XMLElement->attributeValueInt(Tag); }
    inline bool getBoolVal(const QString& Tag) const { return XMLElement->attributeValueBool(Tag); }
    inline int getIntVal(const QString& Tag, const int Default) const { return XMLElement->attributeValueInt(Tag,Default); }
    inline bool getBoolVal(const QString& Tag, const bool Default) const { return XMLElement->attributeValueBool(Tag,Default); }
    inline const QString attribute(const QString& Tag) const { return XMLElement->attribute(Tag); }
    inline void setAttribute(const QString& Tag, const char* Value) { XMLElement->setAttribute(Tag,Value); }
    inline void setAttribute(const QString& Tag, const QString& Value) { XMLElement->setAttribute(Tag,Value); }
    inline void setAttribute(const QString& Tag, const QVariant& Value) { XMLElement->setAttribute(Tag,Value.toString()); }
    inline bool attributeExists(const QString& Tag) const { return XMLElement->attributeExists(Tag); }
    inline void removeZeroAttribute(const QString& Tag) { if (isZero(XMLElement->attributeValue(Tag))) XMLElement->removeAttribute(Tag); }
    inline void removeFalseAttribute(const QString& Tag) { if (!XMLElement->attributeValueBool(Tag)) XMLElement->removeAttribute(Tag); }
    inline QDomLiteElement* getXML() { return XMLElement; }
    inline bool isVisible() const { return !(getBoolVal("Invisible")); }
    inline bool isAudible() const { return !(getBoolVal("Inaudible")); }
    inline void setVisible(const bool v) { XMLElement->setAttribute("Invisible",!v,false); }
    inline void setAudible(const bool v) { XMLElement->setAttribute("Inaudible",!v,false); }
    inline bool isCommon() const { return getBoolVal("Common"); }
    inline bool IsValuedRestOrValuedNote() const { return noteValue() == 7 ? false : IsRestOrValuedNote(); }
    inline void setNoteValFromTicks(const int ticks) {
        int v = 0;
        int d = 0;
        bool t = false;
        ticksToNoteValue(v,d,t,ticks);
        setNoteValue(v);
        setDotted(d);
        setTriplet(t);
    }
    inline static int noteValueToTicks(const int NoteValue, const int Dotted, const bool Triplet) {
        switch (NoteValue)
        {
        case 7:
            return 0;
        case 6:
            return 1;
        case 5:
            if (Triplet) return 2;
            return 3;
        case 4:
            if (Triplet) return 4;
            if (Dotted) return 9;
            return 6;
        case 3:
            if (Triplet) return 8;
            if (Dotted==1) return 18;
            if (Dotted==2) return 21;
            return 12;
        case 2:
            if (Triplet) return 16;
            if (Dotted==1) return 36;
            if (Dotted==2) return 42;
            return 24;
        case 1:
            if (Triplet) return 32;
            if (Dotted==1) return 72;
            if (Dotted==2) return 84;
            return 48;
        case 0:
            if (Triplet) return 64;
            if (Dotted==1) return 144;
            if (Dotted==2) return 168;
            return 96;
        }
        return 0;
    }
    inline static void ticksToNoteValue(int& Notevalue, int& Dotted, bool& Triplet, const int Ticks)
    {
        const int t=Ticks*100;
        Dotted=0;
        Triplet=false;
        switch (t)
        {
        case 16800:
        case 14400:
        case 9600:
        case 6400:
            Notevalue=0;
            break;
        case 8400:
        case 7200:
        case 4800:
        case 3200:
            Notevalue=1;
            break;
        case 4200:
        case 3600:
        case 2400:
        case 1600:
            Notevalue=2;
            break;
        case 2100:
        case 1800:
        case 1200:
        case 800:
            Notevalue=3;
            break;
        case 900:
        case 600:
        case 400:
            Notevalue=4;
            break;
        case 300:
        case 200:
            Notevalue=5;
            break;
        case 100:
            Notevalue=6;
        }
        switch (t)
        {
        case 16800:
        case 8400:
        case 4200:
        case 2100:
            Dotted=2;
        }
        switch (t)
        {
        case 14400:
        case 7200:
        case 3600:
        case 1800:
        case 900:
            Dotted=1;
        }
        switch (t)
        {
        case 6400:
        case 3200:
        case 1600:
        case 800:
        case 400:
        case 200:
        case 100:
            Triplet=true;
        }
    }
    inline static bool isTriplet(const int Ticks) {
        int v = 0;
        int d = 0;
        bool t = false;
        ticksToNoteValue(v,d,t,Ticks);
        return t;
    }
    inline static int isDotted(const int Ticks) {
        int v = 0;
        int d = 0;
        bool t = false;
        ticksToNoteValue(v,d,t,Ticks);
        return d;
    }
    inline static bool isStraight(const int Ticks) {
        int v = 0;
        int d = 0;
        bool t = false;
        ticksToNoteValue(v,d,t,Ticks);
        return !(t || (d > 0));
    }
    inline static int noteType(const int Ticks) {
        int v = 0;
        int d = 0;
        bool t = false;
        ticksToNoteValue(v,d,t,Ticks);
        return v;
    }
    const QString description() const
    {
        if (IsValuedNote())
        {
            return "Note (" + QString::number(pitch()) + ") " + CPitchTextConvert::pitch2Text(pitch());
        }
        else if (IsCompoundNote())
        {
            return "Polyphonic Note (" + QString::number(pitch()) + ") " + CPitchTextConvert::pitch2Text(pitch());
        }
        else if (IsRest())
        {
            return "Rest";
        }
        return name();
    }
    bool isDurated() const
    {
        return (Compare("Tuplet","Beam","DuratedLength","DuratedSlant","DuratedBeamDirection") || Compare("Hairpin","Slur"));
    }
    inline int tickCalc() { return noteValueToTicks(noteValue(),dotted(),triplet()); }
    bool isMaster() const {
        if (Compare("TempoChange","Segno","Fine","Cue") || Compare("Tempo","Coda","DaCapo")) return true;
        return ((Compare("Text")) && (getBoolVal("Master")));
    }
};

class XMLSymbolWrapper : public XMLSimpleSymbolWrapper
{
public:
    inline XMLSymbolWrapper(QDomLiteElement* XMLVoice, const int Pointer, const int CurrentMeter) : XMLSimpleSymbolWrapper(XMLVoice->childElement(Pointer)) { Init(CurrentMeter); }
    inline XMLSymbolWrapper(QDomLiteElement* XMLSymbol, const int CurrentMeter) : XMLSimpleSymbolWrapper(XMLSymbol) { Init(CurrentMeter); }
    void shadowXML(QDomLiteElement* e);
    XMLIntProperty(ticks,Ticks,Ticks,0)
private:
    inline void Init(const int CurrentMeter)
    {
        if (CNoteCompare::IsRest())
        {
            (noteValue() == 7) ? setTicks(CurrentMeter) : setTicks(tickCalc());
        }
        else if (CNoteCompare::IsAnyNote()) setTicks(tickCalc());
    }
};

class XMLFontWrapper : public IXMLWrapper
{
public:
    inline XMLFontWrapper() : IXMLWrapper("Font") {}
    inline XMLFontWrapper(const QString& tag) : IXMLWrapper(tag) {}
    inline XMLFontWrapper(QDomLiteElement* e) : IXMLWrapper(e) {}
    inline XMLFontWrapper(const QString& tag, const QFont& f) : IXMLWrapper(tag) { setFont(f); }
    inline XMLFontWrapper(const IXMLWrapper& w) : IXMLWrapper(w.xml()) {}
    void shadowXML(QDomLiteElement* e);
    inline const QString tag() const {
        return XMLElement->tag;
    }
    inline const QString fontName() const {
        return XMLElement->attribute("FontName", "Times New Roman");
    }
    inline void setFontName(const QString &v) {
        XMLElement->setAttribute("FontName", v, "Times New Roman");
        f.setFamily(v);
        p.setFamily(v);
    }
    inline bool italic() const {
        return XMLElement->attributeValueBool("Italic", false);
    }
    inline void setItalic(const bool v) {
        XMLElement->setAttribute("Italic", v, false);
        f.setItalic(v);
        p.setItalic(v);
    }
    inline bool bold() const {
        return XMLElement->attributeValueBool("Bold", false);
    }
    inline void setBold(const bool v) {
        XMLElement->setAttribute("Bold", v, false);
        f.setBold(v);
        p.setBold(v);
    }
    inline double fontSize() const {
        return double(XMLElement->attributeValue("FontSize", 10));
    }
    inline void setFontSize(const double v) {
        XMLElement->setAttribute("FontSize", v, 10);
        f.setPointSizeF(v);
        p.setPointSizeF(v * 12);
    }
    inline const QFont font() const {
        return f;
    }
    inline const QFont printerFont() const {
        return p;
    }
    inline void setTag(const QString& v) { XMLElement->tag=v; }
    inline void setFont(const QFont& v) {
        setFontName(v.family());
        setItalic(v.italic());
        setBold(v.bold());
        setFontSize(v.pointSizeF());
    }
    inline void showDialog(QWidget* parent) {
        setFont(QFontDialog::getFont(nullptr, font(), parent));
    }
    inline int textWidth(const QString& Text) const {
        QFontMetrics m = QFontMetrics(p);
        return m.boundingRect(Text).width();
    }
    inline int textHeight(const QString& Text) const {
        QFontMetrics m = QFontMetrics(p);
        return m.boundingRect(Text).height();
    }
private:
    QFont f;
    QFont p;
};

class XMLTextElementWrapper : public XMLFontWrapper
{
public:
    inline XMLTextElementWrapper() : XMLFontWrapper("TextElement") {}
    inline XMLTextElementWrapper(QDomLiteElement* e) : XMLFontWrapper(e) {}
    inline XMLTextElementWrapper(const IXMLWrapper& w) : XMLFontWrapper(w.xml()) {}
    void shadowXML(QDomLiteElement* e);
    inline const QString text() const {
        return XMLElement->attribute("Text", "");
    }
    inline void setText(const QString &v) {
        XMLElement->setAttribute("Text", v.trimmed(), "");
    }
    inline void fill(const QFont& v, const QString& t) {
        setFont(v);
        setText(t);
    }
    inline int textWidth() const {
        return XMLFontWrapper::textWidth(text());
    }
    inline int textHeight() const {
        return XMLFontWrapper::textHeight(text());
    }
    inline int textLen() const {
        return text().length();
    }
    inline bool empty() const {
        return text().isEmpty();
    }
};

class XMLScoreOptionsWrapper : public IXMLWrapper
{
public:
    inline XMLScoreOptionsWrapper() : IXMLWrapper("Options") {}
    inline XMLScoreOptionsWrapper(QDomLiteElement* e) : IXMLWrapper(e) {}
    void shadowXML(QDomLiteElement* e);
    XMLProperty(size,Size,Size,defaultscoresize,double)
    XMLIntProperty(view,View,View,0)
    XMLProperty(scoreZoom,ScoreZoom,ScoreZoom,defaultscorezoom,double)
    XMLIntProperty(startBar,StartBar,StartBar,0)
    XMLProperty(layoutZoom,LayoutZoom,LayoutZoom,defaultlayoutzoom,double)
    XMLIntProperty(layoutIndex,LayoutIndex,LayoutIndex,0)
    XMLIntProperty(noteSpace,NoteSpace,NoteSpace,defaultnotespace)
    XMLIntProperty(barNumberOffset,BarNumberOffset,BarNrOffset,0)
    XMLBoolProperty(hideBarNumbers,HideBarNumbers,DontShowBN,false)
    XMLIntProperty(masterStaff,MasterStaff,MasterStave,0)
    XMLIntProperty(followResize,FollowResize,FollowResize,1)
};

class XMLTemplateStaffWrapper : public IXMLWrapper
{
public:
    inline XMLTemplateStaffWrapper() : IXMLWrapper("TemplateStaff") {}
    inline XMLTemplateStaffWrapper(QDomLiteElement* e) : IXMLWrapper(e) {}
    inline XMLTemplateStaffWrapper(QDomLiteElement* e, const int index) : IXMLWrapper(e->childElement(index)) {}
    void shadowXML(QDomLiteElement* e);
    XMLProperty(curlyBracket,CurlyBracket,CurlyBracket,CBNone,CurlyBracketConstants)
    XMLProperty(squareBracket,SquareBracket,SquareBracket,SBNone,SquareBracketConstants)
    XMLIntProperty(height,Height,Height,0)
    XMLIntProperty(size,Size,Size,0)
    XMLIntProperty(id,Id,AllTemplateIndex,0)
    //XMLIntProperty(index,Index,Index,0)
    inline void copyBrackets(const XMLTemplateStaffWrapper& t)
    {
        setCurlyBracket(t.curlyBracket());
        setSquareBracket(t.squareBracket());
    }
};

template <class T>

class IXMLCollectionWrapper : public IXMLWrapper
{
public:
    inline IXMLCollectionWrapper() : IXMLWrapper() {}
    inline IXMLCollectionWrapper(const QString& tag) : IXMLWrapper(tag) {}
    inline IXMLCollectionWrapper(QDomLiteElement* e) : IXMLWrapper(e) {}
    //void copy(const IXMLCollectionWrapper& other)
    //{
    //    IXMLWrapper::copy(other.xml());
    //}
    void clear()
    {
        XMLElement->clearChildren();
    }
    void addChild()
    {
        addChild(T());
    }
    void addChild(const T& e)
    {
        XMLElement->appendClone(e.xml());
    }
    void addChild(const QString& symbolName, const QString& attrName, const QVariant& attrValue)
    {
        XMLSimpleSymbolWrapper s(symbolName);
        s.setAttribute(attrName,attrValue);
        addChild(s);
    }
    void addChild(const QString& symbolName, const QStringList& attrNames, const QVariantList& attrValues)
    {
        XMLSimpleSymbolWrapper s(symbolName);
        for (int i = 0; i < attrNames.size(); i++) s.setAttribute(attrNames[i],attrValues[i]);
        addChild(s);
    }
    void insertChild(const T& e, const int index)
    {
        XMLElement->insertClone(e.xml(),index);
    }
    void insertChild(const QString& symbolName, const QString& attrName, const QVariant& attrValue, const int index)
    {
        XMLSimpleSymbolWrapper s(symbolName);
        s.setAttribute(attrName,attrValue);
        insertChild(s,index);
    }
    void insertChild(const QString& symbolName, const QStringList& attrNames, const QVariantList& attrValues, const int index)
    {
        XMLSimpleSymbolWrapper s(symbolName);
        for (int i = 0; i < attrNames.size(); i++) s.setAttribute(attrNames[i],attrValues[i]);
        insertChild(s,index);
    }
    void deleteChild(const int index)
    {
        XMLElement->removeChild(index);
    }
    T takeChild(const int index)
    {
        T w(XMLElement->childElement(index)->clone());
        XMLElement->removeChild(index);
        return w;
    }
    void replaceChild(const int index, const T& e) { XMLElement->replaceChild(index,e.xml()->clone()); }
    void clearChild(const int index) { replaceChild(index,T()); }
    inline T child(const int index) const { return T(XMLElement->childElement(index)); }
    inline int size() const { return XMLElement->childCount(); }
};

class XMLTemplateWrapper : public IXMLCollectionWrapper<XMLTemplateStaffWrapper>
{
public:
    inline XMLTemplateWrapper() : IXMLCollectionWrapper("Template") {}
    inline XMLTemplateWrapper(QDomLiteElement* e) : IXMLCollectionWrapper(e) {}
    void shadowXML(QDomLiteElement* e);
    inline int staffCount() const { return size(); }
    inline XMLTemplateStaffWrapper staff(const int pos) const { return child(pos); }
    void validateBrackets()
    {
        //updateIndexes();
        for (int i = 1; i<staffCount()-1;i++)
        {
            XMLTemplateStaffWrapper prevStaff(staff(i-1));
            XMLTemplateStaffWrapper thisStaff(staff(i));
            XMLTemplateStaffWrapper nextStaff(staff(i+1));
            if (thisStaff.squareBracket()==SBBegin)
            {
                if (prevStaff.squareBracket()==SBBegin)
                {
                    if (nextStaff.squareBracket()==SBNone) thisStaff.setSquareBracket(SBEnd);
                }
            }
            if (thisStaff.squareBracket()==SBEnd)
            {
                if (prevStaff.squareBracket()==SBEnd) thisStaff.setSquareBracket(SBBegin);
            }
            if (thisStaff.squareBracket()!=SBNone)
            {
                if (nextStaff.squareBracket()==SBNone)
                {
                    if (prevStaff.squareBracket()!=SBBegin) thisStaff.setSquareBracket(SBNone);
                }
            }
            if (thisStaff.curlyBracket()==CBBegin)
            {
                if (nextStaff.id()-thisStaff.id()>1) thisStaff.setCurlyBracket(CBNone);
            }
            if (thisStaff.curlyBracket()==CBBegin)
            {
                if (prevStaff.curlyBracket()==CBBegin) thisStaff.setCurlyBracket(CBNone);
            }
        }
        if (staffCount()>1)
        {
            XMLTemplateStaffWrapper firstStaff(staff(0));
            XMLTemplateStaffWrapper secondStaff(staff(1));
            XMLTemplateStaffWrapper lastStaff(staff(staffCount()-1));
            XMLTemplateStaffWrapper secondlastStaff(staff(staffCount()-2));
            if (firstStaff.squareBracket()==SBBegin)
            {
                if (secondStaff.squareBracket()==SBNone) firstStaff.setSquareBracket(SBNone);
            }
            if (lastStaff.squareBracket()>SBNone)
            {
                if (secondlastStaff.squareBracket()==SBNone) lastStaff.setSquareBracket(SBNone);
            }
            if (lastStaff.squareBracket()==SBBegin)
            {
                if (secondlastStaff.squareBracket()==SBBegin) lastStaff.setSquareBracket(SBEnd);
            }
            if (lastStaff.curlyBracket()==CBBegin)  lastStaff.setCurlyBracket(CBNone);
        }
        if (staffCount()==1)
        {
            XMLTemplateStaffWrapper firstStaff(staff(0));
            firstStaff.setCurlyBracket(CBNone);
            firstStaff.setSquareBracket(SBNone);
        }
    }
    inline int staffTop(const int pos) const {
        int r = 82 * 12;
        for (int i = 0; i < pos; i++) r += ((100 + staff(i).height()) * 12);
        return r;
    }
    inline int staffTopFromId(const int id) const {
        return staffTop(staffPosFromId(id));
    }
    inline int height() const { return staffTop(staffCount()) - 700; }
    inline int staffPosFromId(const int id) const {
        for (int pos=0; pos < staffCount(); pos++) if (child(pos).id()==id) return pos;
        return 0;
    }
    inline int staffId(const int pos) const {
        return child(pos).id();
    }
    inline bool containsStaffId(const int id) const {
        for (int pos=0; pos < staffCount(); pos++) if (child(pos).id()==id) return true;
        return false;
    }
    inline XMLTemplateStaffWrapper staffFromId(const int id) const {
        return staff(staffPosFromId(id));
    }
};

class XMLLayoutFontsWrapper : public IXMLWrapper
{
public:
    inline XMLLayoutFontsWrapper() : IXMLWrapper("Titles")
    {
        setSubElements();
        title.setFont(QFont("Times New Roman",36));
        subtitle.setFont(QFont("Times New Roman",18));
        composer.setFont(QFont("Times New Roman",12));
        names.setFont(QFont("Times New Roman",8));
    }
    inline XMLLayoutFontsWrapper(QDomLiteElement* e) : IXMLWrapper(e)
    {
        setSubElements();
    }
    void shadowXML(QDomLiteElement* e);
    void setSubElements();
    XMLTextElementWrapper title;
    XMLTextElementWrapper subtitle;
    XMLTextElementWrapper composer;
    XMLFontWrapper names;
    inline double height() const
    {
        double retval=0;
        if (!title.empty()) retval += title.textHeight();
        if (!subtitle.empty()) retval += subtitle.textHeight();
        if (!composer.empty()) retval += composer.textHeight();
        return retval;
    }
};

class XMLLayoutOptionsWrapper : public XMLScoreOptionsWrapper
{
public:
    inline XMLLayoutOptionsWrapper() : XMLScoreOptionsWrapper() {}
    inline XMLLayoutOptionsWrapper(QDomLiteElement* e) : XMLScoreOptionsWrapper(e) {}
    void shadowXML(QDomLiteElement* e);
    XMLIntProperty(showNamesSwitch,ShowNamesSwitch,ShowNamesSwitch,2)
    XMLBoolProperty(showAllOnFirstSystem,ShowAllOnFirstSystem,ShowAllOnSys1,true)
    XMLBoolProperty(showLayoutName,ShowLayoutName,ShowLayoutName,false)
    XMLBoolProperty(transposeInstruments,TransposeInstruments,TransposeInstruments,false)
    XMLIntProperty(scoreType,ScoreType,ScoreType,2)
    XMLProperty(scaleSize,ScaleSize,ScaleSize,1,double)
    XMLIntProperty(topMargin,TopMargin,TopMargin,20)
    XMLIntProperty(leftMargin,LeftMargin,LeftMargin,15)
    XMLIntProperty(rightMargin,RightMargin,RightMargin,15)
    XMLIntProperty(bottomMargin,BottomMargin,BottomMargin,25)
    XMLIntProperty(orientation,Orientation,Orientation,0)
    XMLIntProperty(paperSize,PaperSize,PaperSize,0)
    XMLBoolProperty(frontPage,FrontPage,FrontPage,false);
    inline double scale(const double v) const { return v*scaleSize(); }
};

class LayoutLocation
{
public:
    LayoutLocation(int p = -1, int s = -1) : Page(p), System(s) {}
    int Page;
    int System;
    bool isFirstPage() const { return (Page == 0); }
    bool isTopSystem() const { return (System == 0); }
    bool isValid() const {
        return ((Page > -1) && (System >-1));
    }
    bool isFirstSystem() const {
        return ((Page == 0) && (System == 0));
    }
    bool matches(const LayoutLocation& other) const {
        return ((other.Page == Page) && (other.System == System));
    }
};

class XMLLayoutSystemWrapper : public IXMLWrapper
{
public:
    inline XMLLayoutSystemWrapper() : IXMLWrapper("System")
    {
        setSubElements();
    }
    inline XMLLayoutSystemWrapper(QDomLiteElement* e) : IXMLWrapper(e)
    {
        setSubElements();
    }
    void setSubElements();
    XMLTemplateWrapper Template;
    XMLIntProperty(startBar,StartBar,StartBar,0)
    XMLIntProperty(endBar,EndBar,EndBar,0)
    XMLIntProperty(showNames,ShowNames,NamesVisible,0)
    XMLProperty(top,Top,Top,0,double)
    XMLProperty(defaultTop,DefaultTop,DefaultTop,0,double)
    XMLProperty(height,Height,Height,0,double)
    XMLProperty(sysLen,SysLen,SystemLength,0,double)
    bool ContainsBar(const int Bar) const {
        if ((startBar() <= Bar) && (endBar() >= Bar)) return true;
        return false;
    }
};

class XMLLayoutPageWrapper : public IXMLWrapper
{
public:
    inline XMLLayoutPageWrapper() : IXMLWrapper("Page") {}
    inline XMLLayoutPageWrapper(QDomLiteElement* e) : IXMLWrapper(e) {}
    void shadowXML(QDomLiteElement* e);
    inline XMLLayoutSystemWrapper XMLSystem(const int index) const { return XMLLayoutSystemWrapper(XMLElement->childElement(index)); }
    inline int systemCount() const { return XMLElement->childCount(); }
    int SystemOfBar(const int Bar) const {
        for (int i = 0; i < systemCount(); i++) if (XMLSystem(i).ContainsBar(Bar)) return i;
        return -1;
    }
    bool ContainsBar(const int Bar) const {
        return (SystemOfBar(Bar) > -1);
    }
};

class XMLLayoutWrapper : public IXMLWrapper
{
public:
    inline XMLLayoutWrapper() : IXMLWrapper("Layout")
    {
        setSubElements();
    }
    inline XMLLayoutWrapper(QDomLiteElement* e) : IXMLWrapper(e)
    {
        setSubElements();
    }
    void setSubElements();
    XMLLayoutFontsWrapper Fonts;
    XMLLayoutOptionsWrapper Options;
    XMLTemplateWrapper Template;
    XMLStringProperty(name,Name,Name,"")
    XMLBoolProperty(isFormated,IsFormated,IsFormated,false)
    QDomLiteElementList templates() const
    {
        return XMLElement->elementsByTag("Template",true);
    }
    void removeTemplate(QDomLiteElement* t) {
        XMLElement->removeChild(t);
    }
    inline XMLLayoutPageWrapper XMLPage(const int index) const { return XMLLayoutPageWrapper((XMLElement->elementsByTag("Page")).at(index)); }
    inline XMLLayoutSystemWrapper XMLSystem(const LayoutLocation& l) const { return XMLPage(l.Page).XMLSystem(l.System); }
    inline int pageCount() const { return XMLElement->elementsByTag("Page").size(); }
    inline int systemCount(const int page) const { return XMLPage(page).systemCount(); }
    inline bool pageExists(const int page) const { return (page < pageCount()); }
    inline bool systemExists(const LayoutLocation& l) const {
        if (pageExists(l.Page)) return (l.System < systemCount(l.Page));
        return false;
    }
    inline bool isLastPage(const int page) const { return (page == pageCount()-1); }
    inline bool isBottomSystem(const LayoutLocation& l) const { return (l.System == systemCount(l.Page)-1); }
    inline bool isLastSystem(const LayoutLocation& l) const { return (isLastPage(l.Page) && isBottomSystem(l)); }
    inline bool nextSystem(LayoutLocation& l) const {
        if (isLastSystem(l)) return false;
        if (l.System < systemCount(l.Page)-1) {
            l.System++;
        }
        else {
            if (!isLastPage(l.Page)) {
                l.Page++;
                l.System = 0;
            }
        }
        return true;
    }
    inline bool prevSystem(LayoutLocation& l) const {
        if (l.isFirstSystem()) return false;
        if (l.System > 0) {
            l.System--;
        }
        else {
            if (!l.isFirstPage()) {
                l.Page--;
                l.System = systemCount(l.Page) - 1;
            }
        }
        return true;
    }
    inline void lastSystem(LayoutLocation& l) const {
        l.Page = pageCount() - 1;
        l.System = systemCount(l.Page) - 1;
    }
    int PageOfBar(const int Bar) const
    {
        for (int i = 0; i < pageCount(); i++) if (XMLPage(i).ContainsBar(Bar)) return i;
        return -1;
    }
    int SystemOfBar(const int Page, const int Bar) const
    {
        if (Page < 0) return -1;
        return XMLPage(Page).SystemOfBar(Bar);
    }
};

class XMLLayoutCollectionWrapper : public IXMLCollectionWrapper<XMLLayoutWrapper>
{
public:
    inline XMLLayoutCollectionWrapper() : IXMLCollectionWrapper("Layout") {}
    inline XMLLayoutCollectionWrapper(QDomLiteElement* e) : IXMLCollectionWrapper(e) {}
    virtual ~XMLLayoutCollectionWrapper();
    QDomLiteElementList templates() const
    {
        QDomLiteElementList l;
        for (int i = 0; i < layoutCount(); i++)
        {
            const QDomLiteElementList t=XMLLayout(i).templates();
            for (QDomLiteElement* e : t) l.append(e);
        }
        return l;
    }
    void removeTemplate(QDomLiteElement* t) {
        for (int i = 0; i < layoutCount(); i++) XMLLayout(i).removeTemplate(t);
    }
    inline XMLLayoutWrapper XMLLayout(const int index) const { return child(index); }
    inline int layoutCount() const { return size(); }
    inline int layoutExists(const int index) { return ((index > -1) && (index < size())); }
};

class XMLVoiceWrapper : public IXMLCollectionWrapper<XMLSimpleSymbolWrapper>
{
public:
    inline XMLVoiceWrapper() : IXMLCollectionWrapper("Voice") {}
    inline XMLVoiceWrapper(QDomLiteElement* e) : IXMLCollectionWrapper(e) {}
    void shadowXML(QDomLiteElement* e);
    inline XMLSymbolWrapper XMLSymbol(const int index, const int currentMeter) const { return XMLSymbolWrapper(XMLElement,index,currentMeter); }
    inline XMLSimpleSymbolWrapper XMLSimpleSymbol(const int index) const { return child(index); }
    inline int symbolCount() const { return size(); }
    inline bool isFullRest() const {
        for (int i = 0; i < size(); i++) if (child(i).IsValuedNote()) return false;
        return true;
    }
};

class XMLStaffWrapper : public IXMLCollectionWrapper<XMLVoiceWrapper>
{
public:
    inline XMLStaffWrapper() : IXMLCollectionWrapper("Staff") {}
    inline XMLStaffWrapper(QDomLiteElement* e) : IXMLCollectionWrapper(e) {}
    void shadowXML(QDomLiteElement* e);
    XMLStringProperty(name,Name,Name,"")
    XMLStringProperty(abbreviation,Abbreviation,Abbreviation,"")
    inline XMLVoiceWrapper XMLVoice(const int index) const { return child(index); }
    inline int voiceCount() const { return size(); }
};

class XMLStaffCollectionWrapper : public IXMLCollectionWrapper<XMLStaffWrapper>
{
public:
    inline XMLStaffCollectionWrapper() : IXMLCollectionWrapper("Score") {}
    inline XMLStaffCollectionWrapper(QDomLiteElement* e) : IXMLCollectionWrapper(e) {}
    inline XMLStaffWrapper XMLStaff(const int index) const { return child(index); }
    virtual ~XMLStaffCollectionWrapper();
    inline int staffCount() const { return size(); }
};

class XMLScoreWrapper
{
public:
    XMLScoreOptionsWrapper ScoreOptions;
    XMLTemplateWrapper Template;
    XMLFontWrapper TempoFont;
    XMLFontWrapper DynamicFont;
    XMLLayoutCollectionWrapper LayoutCollection;
    XMLStaffCollectionWrapper Score;
    XMLScoreWrapper()
    {
        m_XMLScore=nullptr;
        isShadow=false;
    }
    XMLScoreWrapper(QDomLiteDocument* Doc)
    {
        shadowXML(Doc);
    }
    ~XMLScoreWrapper()
    {
        if (!isShadow)
        {
            if (m_XMLScore != nullptr) delete m_XMLScore;
        }
    }
    void setSubElements()
    {
        Template.shadowXML(m_XMLScore->documentElement->elementByTagCreate("Template"));
        TempoFont.shadowXML(m_XMLScore->documentElement->elementByTagCreate(XMLFontWrapper("TempoFont",QFont("Times New Roman",160,QFont::Bold)).xml()));
        DynamicFont.shadowXML(m_XMLScore->documentElement->elementByTagCreate(XMLFontWrapper("DynamicFont",QFont("Times New Roman",150,QFont::Normal,true)).xml()));
        LayoutCollection.shadowXML(m_XMLScore->documentElement->elementByTagCreate("LayoutCollection"));
        Score.shadowXML(m_XMLScore->documentElement->elementByTagCreate("Score"));
        ScoreOptions.shadowXML(m_XMLScore->documentElement->elementByTagCreate("Options"));
        //setOptions();
    }
    void setXML(QDomLiteDocument* Doc)
    {
        isShadow=false;
        if ((m_XMLScore != nullptr) && (m_XMLScore != Doc)) delete m_XMLScore;
        m_XMLScore=Doc;
        setSubElements();
    }
    void setXML(XMLScoreWrapper& Doc)
    {
        setXML(Doc.getXML());
    }
    void setCopy(QDomLiteDocument* Doc)
    {
        isShadow=false;
        if (m_XMLScore != nullptr) delete m_XMLScore;
        m_XMLScore=Doc->clone();
        setSubElements();
    }
    void setCopy(XMLScoreWrapper& Doc)
    {
        setCopy(Doc.getXML());
    }
    void shadowXML(QDomLiteDocument* Doc)
    {
        isShadow=true;
        m_XMLScore=Doc;
        setSubElements();
    }
    void shadowXML(XMLScoreWrapper& Doc)
    {
        shadowXML(Doc.getXML());
    }
    QDomLiteDocument* getXML()
    {
        return m_XMLScore;
    }
    void swapDocumentElement(QDomLiteElement** e)
    {
        m_XMLScore->swapDoc(e);
        setSubElements();
    }
    QDomLiteElement* documentClone()
    {
        return m_XMLScore->documentElement->clone();
    }
    QDomLiteDocument* getClone()
    {
        return m_XMLScore->clone();
    }
    void newScore()
    {
        newDoc();
        m_XMLScore->documentElement->elementByTagCreate("Score");
        setSubElements();
        AddStaff(0,"New Staff");
    }
    void newDoc()
    {
        isShadow=false;
        if (m_XMLScore != nullptr) delete m_XMLScore;
        m_XMLScore=new QDomLiteDocument("ObjectComposerProject","ObjectComposerScore");
        setSubElements();
    }
    void replaceDocumentElement(QDomLiteElement* XMLDoc)
    {
        m_XMLScore->replaceDoc(XMLDoc);
        setSubElements();
    }
    void replaceScore(const XMLStaffCollectionWrapper& XMLScore)
    {
        m_XMLScore->documentElement->replaceChild(Score.xml(), XMLScore.xml()->clone());
        setSubElements();
    }
    bool Load(const QString& Path)
    {
        newDoc();
        if (!m_XMLScore->load(Path)) return false;
        setSubElements();
        ParseFileVersion();
        setSubElements();
        return true;
    }
    /*
    bool FromByteArray(const QByteArray& b)
    {
        newDoc();
        m_XMLScore->fromByteArray(b);
        setSubElements();
        ParseFileVersion();
        setSubElements();
        return true;
    }
*/
    void ParseFileVersion();
    bool Save(const QString& Path)
    {
        return m_XMLScore->save(Path);
    }
    /*
    QByteArray ToByteArray()
    {
        return m_XMLScore->toByteArray();
    }
*/
    void serialize(QDomLiteElement* xml) const {
        xml->appendChild(m_XMLScore->documentElement->clone());
    }
    void unserialize(const QDomLiteElement* xml) {
        newDoc();
        m_XMLScore->replaceDoc(xml->firstChild()->clone());
        setSubElements();
        ParseFileVersion();
        setSubElements();
    }
    /*
    inline void setOptions(const XMLScoreOptionsWrapper& XMLOptions)
    {
        tempOptions.copy(ScoreOptions);
        ScoreOptions.copy(XMLOptions);
        //ScoreOptions.shadowXML(XMLOptions.xml());
        //setSubElements();
        //ParseFileVersion();
        //setSubElements();
    }
    inline void resetOptions() {
        //ScoreOptions.shadowXML(m_XMLScore->documentElement);
        ScoreOptions.copy(tempOptions);
    }
*/
    void inline setUndoName(const QString& name) { m_XMLScore->documentElement->setAttribute("UndoName",name); }
    void inline setUndoName() { m_XMLScore->documentElement->removeAttribute("UndoName"); }
    inline XMLStaffWrapper Staff(const int Index) const { return Score.XMLStaff(Index); }
    inline XMLVoiceWrapper Voice(const int StaffIndex, const int Index) const
    {
        return Staff(StaffIndex).XMLVoice(Index);
    }
    inline static XMLVoiceWrapper Voice(const XMLStaffWrapper& XMLStaff, const int Index)
    {
        return XMLStaff.XMLVoice(Index);
    }
    inline XMLVoiceWrapper Voice(const OCVoiceLocation& v) const
    {
        return Staff(v.StaffId).XMLVoice(v.Voice);
    }
    inline static XMLVoiceWrapper Voice(const XMLStaffCollectionWrapper& XMLScore, const int StaffIndex, const int Index)
    {
        return XMLVoiceWrapper(XMLScore.XMLStaff(StaffIndex).XMLVoice(Index));
    }
    inline XMLSimpleSymbolWrapper Symbol(const int StaffIndex, const int VoiceIndex, const int Index) const
    {
        return Voice(StaffIndex,VoiceIndex).XMLSimpleSymbol(Index);
    }
    inline XMLSimpleSymbolWrapper Symbol(const OCSymbolLocation& SymbolLocation) const
    {
        return Voice(SymbolLocation).XMLSimpleSymbol(SymbolLocation.Pointer);
    }
    inline static XMLSimpleSymbolWrapper Symbol(const XMLVoiceWrapper& XMLVoice, const int Index)
    {
        return XMLVoice.child(Index);
    }
    static int FindSymbol(const XMLVoiceWrapper& XMLVoice, const QString& Name, const int Ptr=0, const QString& Attr=QString(), const double Val=0, const QString& Attr1=QString(), const double Val1=0);
    inline int NumOfStaffs() const {
        return Score.staffCount();
    }
    inline int NumOfVoices(const int StaffIndex) const {
        return Staff(StaffIndex).voiceCount();
    }
    inline static int NumOfVoices(const XMLStaffWrapper& XMLStaff) {
        return XMLStaff.voiceCount();
    }
    static void Paste1Voice(XMLVoiceWrapper& XMLVoice, const int Pointer, const XMLVoiceWrapper& data) {
        for (int i = 0; i < data.size(); i++) {
            XMLVoice.insertChild(data.XMLSimpleSymbol(i),Pointer+i);
        }
    }
    void Paste1Voice(const OCSymbolLocation& SymbolLocation, const XMLVoiceWrapper& data) {
        XMLVoiceWrapper v = Voice(SymbolLocation);
        if (SymbolLocation.Voice > 0) {
            for (int i = 0; i < data.size(); i++) {
                XMLSimpleSymbolWrapper Symbol = data.XMLSimpleSymbol(i);
                if (Symbol.IsRest() && Symbol.noteValue() == 7) Symbol.setVisible(false);
            }
        }
        Paste1Voice(v, SymbolLocation.Pointer, data);
    }
    static void Clear1Voice(XMLVoiceWrapper& XMLVoice, const int pointer) {
        /*
        if (XMLVoice.XMLSimpleSymbol(pointer).IsValuedNote() && pointer > 0) {
            int i = pointer;
            XMLSimpleSymbolWrapper s = XMLVoice.XMLSimpleSymbol(--i);
            while (!s.IsRestOrValuedNote() && i >= 0) {
                if (s.IsCompoundNote()) {
                    s.xml()->setAttribute("NoteType",s.xml()->attributeValueInt("NoteType") - 2);
                    break;
                }
                s = XMLVoice.XMLSimpleSymbol(--i);
            }
        }
*/      if (pointer >= XMLVoice.size()) return;
        if (XMLVoice.XMLSimpleSymbol(pointer).IsValuedNote()) {
            for (int i = pointer - 1; i >= 0; --i) {
                const XMLSimpleSymbolWrapper s = XMLVoice.XMLSimpleSymbol(i);
                if (s.IsRestOrValuedNote()) break;
                if (s.IsCompoundNote()) {
                    s.xml()->setAttribute("NoteType", s.xml()->attributeValueInt("NoteType") - 2);
                    break;
                }
            }
        }
        XMLVoice.deleteChild(pointer);
    }
    static void Clear1Voice(XMLVoiceWrapper& XMLVoice, const OCSymbolRange& SymbolRange) {
        for (int i = SymbolRange.End; i >= SymbolRange.Start; i--) Clear1Voice(XMLVoice, i);
    }
    static void Clear1Voice(XMLVoiceWrapper& XMLVoice, const QVector<int>& Pointers) {
        for(int i = Pointers.size()-1; i >= 0; i--) Clear1Voice(XMLVoice, Pointers[i]);
    }
    void Clear1Voice(const OCVoiceLocation& VoiceLocation, const OCSymbolRange& SymbolRange) {
        XMLVoiceWrapper v=Voice(VoiceLocation);
        Clear1Voice(v,SymbolRange);
    }
    void Clear1Voice(const OCVoiceLocation& VoiceLocation, const QVector<int>& Pointers) {
        XMLVoiceWrapper v=Voice(VoiceLocation);
        Clear1Voice(v,Pointers);
    }
    XMLVoiceWrapper AddVoice(const int StaffIndex) {
        XMLStaffWrapper s=Staff(StaffIndex);
        return AddVoice(s);
    }
    XMLVoiceWrapper AddVoice(const int StaffIndex, const int NewNumber) {
        XMLStaffWrapper s=Staff(StaffIndex);
        return AddVoice(s,NewNumber);
    }
    static XMLVoiceWrapper AddVoice(XMLStaffWrapper& XMLStaff) {
        XMLStaff.addChild();
        return XMLStaff.XMLVoice(XMLStaff.voiceCount()-1);
    }
    static XMLVoiceWrapper AddVoice(XMLStaffWrapper& XMLStaff, int NewNumber) {
        XMLStaff.insertChild(XMLVoiceWrapper(),NewNumber);
        return XMLStaff.XMLVoice(NewNumber);
    }
    XMLStaffWrapper AddStaff(const int NewNumber, const QString& Name) {
        QString NewName=Name.trimmed();
        if (NewName.length()==0) NewName="Staff "+QString::number(NewNumber+1);
        XMLStaffWrapper NewStaff;
        NewStaff.setName(NewName);
        Score.insertChild(NewStaff,NewNumber);
        NewStaff=Staff(NewNumber);
        AddVoice(NewStaff);
        XMLTemplateStaffWrapper XMLTemplateStaff;
        Template.insertChild(XMLTemplateStaff,NewNumber);
        updateLayoutTemplates();
        return NewStaff;
    }
    static void DeleteVoice(XMLStaffWrapper& XMLStaff, const int VoiceIndex) {
        XMLStaff.deleteChild(VoiceIndex);
    }
    void DeleteVoice(const int StaffIndex, const int VoiceIndex) {
        XMLStaffWrapper s=Staff(StaffIndex);
        DeleteVoice(s,VoiceIndex);
    }
    void DeleteVoice(const OCVoiceLocation& VoiceLocation) {
        DeleteVoice(VoiceLocation.StaffId,VoiceLocation.Voice);
    }
    void DeleteStaff(const int StaffIndex) {
        Score.deleteChild(StaffIndex);
        Template.deleteChild(StaffIndex);
        updateLayoutTemplates();
    }
    void MoveStaff(const int From, const int To) {
        Template.insertChild(Template.takeChild(From),To);
        Score.insertChild(Score.takeChild(From),To);
        updateLayoutTemplates();
    }
    void updateLayoutTemplates() {
        QList<int> l;
        for (int i = 0; i < Template.staffCount(); i++) l.append(Template.staff(i).id());
        for (QDomLiteElement* t : (const QDomLiteElementList)LayoutCollection.templates()) {
            XMLTemplateWrapper tw(t);
            for (int pos = tw.staffCount()-1; pos >= 0; pos--) {
                const int a = l.indexOf(tw.staff(pos).id());
                if (a < 0) {
                    tw.deleteChild(pos);
                }
                else {
                    XMLTemplateStaffWrapper tsw(tw.staff(pos));
                    tsw.setId(a);
                }
            }
            if (tw.staffCount() == 0) LayoutCollection.removeTemplate(t);
        }
        //Template.updateAllTemplateIndexes();
        for (int i = 0; i < Template.staffCount(); i++) {
            XMLTemplateStaffWrapper s(Template.staff(i));
            s.setId(i);
        }
    }
    inline const QString StaffName(const int StaffIndex) const { return Staff(StaffIndex).name(); }
    void setStaffName(const int StaffIndex, const QString& Name) {
        XMLStaffWrapper s(Staff(StaffIndex));
        s.setName(Name);
    }
    inline const QString StaffAbbreviation(const int StaffIndex) const { return Staff(StaffIndex).abbreviation(); }
    void setStaffAbbreviation(const int StaffIndex, const QString& Abbreviation) {
        XMLStaffWrapper s(Staff(StaffIndex));
        s.setAbbreviation(Abbreviation);
    }
    inline int layoutCount() const {
        return LayoutCollection.layoutCount();
    }
    inline XMLLayoutWrapper Layout(const int Index) const {
        return LayoutCollection.XMLLayout(Index);
    }
    inline XMLLayoutWrapper Layout(const QString& Name) const {
        for (int i=0;i<layoutCount();i++) if (LayoutName(i)==Name) return Layout(i);
        return XMLLayoutWrapper();
    }
    inline const QString LayoutName(const int LayoutIndex) const {
        return Layout(LayoutIndex).name();
    }
    inline void setLayoutName(const int LayoutIndex, const QString Name) {
        XMLLayoutWrapper l(Layout(LayoutIndex));
        l.setName(Name);
    }

    inline void setMixer(QDomLiteElement* mixerXML) {
        QDomLiteElement* mixer=new QDomLiteElement("MIDIMixer");
        mixer->appendChild(mixerXML);
        m_XMLScore->documentElement->setChild("MIDIMixer",mixer);
    }
    inline QDomLiteElement* getMixer() {
        const QDomLiteElement* mx=m_XMLScore->documentElement->elementByTag("MIDIMixer");
        return (mx != nullptr) ? mx->firstChild()->clone() : nullptr;
    }
private:
    QDomLiteDocument* m_XMLScore;
    //XMLScoreOptionsWrapper tempOptions;
    bool isShadow;
};

#endif // OCXMLWRAPPERS_H
