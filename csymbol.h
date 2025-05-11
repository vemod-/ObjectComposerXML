#ifndef CSYMBOL_H
#define CSYMBOL_H

#include "CommonClasses.h"
//#include <QSettings>
//#include <QApplication>
//#include <QObject>
#include <QToolButton>
#include <QLabel>
//#include <QVBoxLayout>
#include "CommonCounters.h"
#include "midifileclasses.h"
;
#pragma pack(push,1)

class OCPrintSignList;
class OCPlaySignList;
class OCNoteList;

enum OCPropertyType
{
    pwNumber = 1,
    pwList = 2,
    pwBoolean = 3,
    pwCustom = 4,
    pwText = 6,
    pwSlider = 7
};

//--------------------------------------------------------------------------

class OCProperty
{
public:
    QString Name;
    QVariant Min = QString();
    QVariant Max = QString();
    OCPropertyType PropertyType = pwText;
    QString Description;
    QVariant Default = QString();
    bool Hidden = false;
    bool Prefered = false;
    QString Category;
    bool PermHidden = false;
    inline OCProperty() {}
    inline OCProperty(const QString& Name, const OCPropertyType PropertyType, const QVariant& Min = QString(), const QVariant& Max = QString(), const QString& Description = QString(), const QStringList& List = QStringList(), const int ListOffset = 0, const bool Hidden = false, const QVariant& Value = QString(), const QString& Category = QString(), const bool Prefered = false)
    {
        this->Name = Name;
        this->PropertyType = PropertyType;
        this->Min = Min;
        this->Max = Max;
        this->Description = Description;
        this->ListOffset = ListOffset;
        addToList(List);
        this->setValue(Value);
        this->Default = Value;
        this->Hidden = Hidden;
        this->Prefered = Prefered;
        this->PermHidden = Hidden;
        this->Category = Category;
    }
    inline void addToList(const QString& Text)
    {
        m_sList.append(Text);
        if (m_sList.size()) {
            Min = 0;
            Max = m_sList.size()-1;
        }
    }
    inline void addToList(const QStringList& Lst)
    {
        if (Lst.size()) {
            m_sList.append(Lst);
            Min = 0;
            Max = m_sList.size()-1;
        }
    }
    inline const QString listItem(int Index) const
    {
        return m_sList[Index];
    }
    inline int listCount() const
    {
        return m_sList.size();
    }
    inline const QStringList& list() const
    {
        return m_sList;
    }
    inline const QVariant value() const
    {
        return m_Value;
    }
    inline void setValue(const QVariant& vData)
    {
        if (PropertyType == pwList)
        {
            if ((listIndex(vData) >= 0) && (listIndex(vData) < m_sList.size()))
            {
                m_Value = vData.toInt();
                return;
            }
            else
            {
                setListIndex(m_sList.indexOf(vData.toString()));
                return;
            }
        }
        m_Value=vData;
    }
    inline void setListIndex(const int index) {
        m_Value = index + ListOffset;
    }
    inline const QString textValue() const
    {
        QString Value=m_Value.toString();
        switch (PropertyType)
        {
        case pwList:
            Value = m_sList[listIndex()];
            break;
        case pwBoolean:
            Value = (m_Value.toBool() ? QChar(0xF0FE):QChar(0xF0A8));
            break;
        case pwNumber:
            Value = QString::number(m_Value.toInt());
            break;
        case pwSlider:
            Value = QString::number(m_Value.toInt());
            break;
        case pwCustom:
            Value = "Custom";
            break;
        case pwText:
            break;
        }
        return Value;
    }
    int listIndex() const {
        return listIndex(m_Value);
    }
private:
    int listIndex(const QVariant v) const {
        return v.toInt() - ListOffset;
    }
    QStringList m_sList;
    QVariant m_Value;
    int ListOffset = 0;
};

//--------------------------------------------------------------------------

class OCProperties : public QMap<QString, OCProperty>
{
public:
    inline OCProperties(){}
    QString Name;
    inline void append(const QString& Name, const OCPropertyType PropertyType, const QVariant& Min = QString(), const QVariant& Max = QString(), const QString& Description = QString(), const QStringList& List = QStringList(), const int ListOffset = 0, const bool Hidden = false, const QVariant& Value = QString(), const QString& Category = QString(), const bool Prefered = false)
    {
        insert(Name,OCProperty(Name,PropertyType,Min,Max,Description,List,ListOffset,Hidden,Value,Category,Prefered));
    }
    inline void appendNumber(const QString& Name, const int Min, const int Max, const QString& Description = QString(), const bool Hidden = false, const QVariant& Value = QString(), const QString& Category = QString(), const bool Prefered = false) {
        append(Name,pwNumber,Min,Max,Description,{},0,Hidden,Value,Category,Prefered);
    }
    inline void appendList(const QString& Name, const QString& Description = QString(), const QStringList& List = QStringList(), const int ListOffset = 0, const bool Hidden = false, const QVariant& Value = QString(), const QString& Category = QString(), const bool Prefered = false) {
        append(Name,pwList,"","",Description,List,ListOffset,Hidden,Value,Category,Prefered);
    }
    inline void appendBool(const QString& Name, const QString& Description = QString(), const bool Hidden = false, const QVariant& Value = QString(), const QString& Category = QString(), const bool Prefered = false) {
        append(Name,pwBoolean,0,1,Description,{},0,Hidden,Value,Category,Prefered);
    }
    inline void appendCustom(const QString& Name, const QString& Description = QString(), const bool Hidden = false, const QVariant& Value = QString(), const QString& Category = QString(), const bool Prefered = false) {
        append(Name,pwCustom,"","",Description,{},0,Hidden,Value,Category,Prefered);
    }
    inline void appendText(const QString& Name, const QString& Description = QString(), const bool Hidden = false, const QVariant& Value = QString(), const QString& Category = QString(), const bool Prefered = false) {
        append(Name,pwText,"","",Description,{},0,Hidden,Value,Category,Prefered);
    }
    inline void appendSlider(const QString& Name, const int Min, const int Max, const QString& Description = QString(), const bool Hidden = false, const QVariant& Value = QString(), const QString& Category = QString(), const bool Prefered = false) {
        append(Name,pwSlider,Min,Max,Description,{},0,Hidden,Value,Category,Prefered);
    }
    inline OCProperty& property(const QString& Name)
    {
        return (*this)[Name];
    }
    inline const QVariant propertyValue(const QString& Name) const
    {
        return (*this)[Name].value();
    }
    inline void setPropertyValue(const QString& Name,const QVariant& vData)
    {
        (*this)[Name].setValue(vData);
    }
    inline void hide(const QString& propertyName, const bool hidden = true) {
        (*this)[propertyName].Hidden = hidden;
    }
    inline void show(const QString& propertyName) {
        (*this)[propertyName].Hidden = false;
    }
    inline bool isHidden(const QString& propertyName) {
        return (*this)[propertyName].Hidden;
    }
    /*
    inline void remove(const QString& Name)
    {
        mCol.remove(Name);
    }
    inline bool contains(const QString& Name) const
    {
        return mCol.contains(Name);
    }
*/
    inline QString preferedCaption() const
    {
        for (const OCProperty& p : (*this)) if (p.Prefered) return p.Name;
        return QString();
    }
    inline void reset(const QString& name)
    {
        Name = name;
        for (OCProperty& p : (*this)) p.setValue(p.Default);
    }
    inline void fromXML(const XMLSimpleSymbolWrapper& XML)
    {
        Name = XML.name();
        for (OCProperty& p : (*this)) p.setValue(XML.xml()->attribute(p.Name,p.Default.toString()));
    }
    inline XMLSimpleSymbolWrapper toXML() const
    {
        XMLSimpleSymbolWrapper s(Name);
        for (const OCProperty& p : (*this)) s.xml()->setAttribute(p.Name,p.value(),0);
        return s;
    }
    /*
    QList<OCProperty> values() const {
        return (*this).values();
    }
protected:
    QMap<QString, OCProperty> mCol;
*/
};

class CustomEdit : public QWidget
{
    Q_OBJECT
public:
    explicit CustomEdit(QWidget* parent=nullptr);
    QLabel* lbl;
private:
    QToolButton* btn;
signals:
    void clicked();
    void valueChanged(const QString & value);
};

class OCSettings : public QSettings
{
public:
    OCSettings() :
#ifdef Q_OS_IOS
        //QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/ObjectComposerXML";
        //Settings = new QSettings(settingsPath, QSettings::IniFormat);
        QSettings(QString("Veinge Musik och Data"),QString("ObjectComposerXML"))
#else
        QSettings(QString("http://www.musiker.nu/objectstudio"),QString("ObjectComposerXML"))
#endif
    {}
};

class OCPresets
{
public:
    OCPresets(const QString& tag="Presets");
    ~OCPresets();
    const QVariant GetValue(const QString& Preset) const;
    const QVariant GetValue(const QString& Preset, const QVariant& Default) const;
    const QString GetString(const QString& Preset) const;
    const QString GetString(const QString& Preset, const QString& Default) const;
    void SetValue(const QString& Preset, const QVariant& Value);
    OCProperties* SetPropertyValue(OCProperties* p, const QString& PropertyName, const QString& Preset);
    OCProperties* Properties();
    void SaveProperties();
private:
    OCSettings* Settings;
    OCProperties* m_PropColl=nullptr;
    void SetDefaults();
    QString Tag;
    bool MustSaveProperties;
};

//--------------------------------------------------------------------------

class CSymbol : public XMLSimpleSymbolWrapper
{
protected:
    QString m_ButtonProperty;
    QList<OCToolButtonProps*> m_ButtonList;
    OCToolButtonProps* CreateButton(const QString& Category, const QString& FontName, const float FontSize, const QString& Text, const bool FontBold, const bool FontItalic, const bool CustomDialog=false, const OCRefreshMode RefreshMode=tsRedrawActiveStave, const QString& Tooltip=QString(), const QString& ModifierName=QString())
    {
        OCToolButtonProps* p = new OCToolButtonProps(name(),m_ButtonList.size());
        p->category=Category;
        p->fontname=FontName;
        p->fontsize=FontSize;
        p->fontbold=FontBold;
        p->fontitalic=FontItalic;
        p->buttonText=Text;
        p->customdialog=CustomDialog;
        p->refreshmode=RefreshMode;
        p->tooltip=Tooltip;
        if (ModifierName.startsWith("modifier"))
        {
            p->ismodifier=true;
            p->modifierProperty=ModifierName.mid(8);
        }
        else
        {
            p->modifierProperty=ModifierName;
        }
        m_ButtonList.append(p);
        if (p->tooltip.isEmpty()) p->tooltip="Add "+name();
        return p;
    }
    OCToolButtonProps* CreateButton(const QString& Category, const QString& IconPath, const bool CustomDialog=false, const OCRefreshMode RefreshMode=tsRedrawActiveStave, const QString& Tooltip=QString(), const QString& ModifierName=QString())
    {
        OCToolButtonProps* p = new OCToolButtonProps(name(),m_ButtonList.size());
        p->category=Category;
        p->iconpath=IconPath;
        p->customdialog=CustomDialog;
        p->refreshmode=RefreshMode;
        p->tooltip=Tooltip;
        p->modifierProperty=ModifierName;
        m_ButtonList.append(p);
        if (p->tooltip.isEmpty()) p->tooltip="Add "+name();
        return p;
    }
    OCProperties m_PropColl;
    void buildProperties() {}
    void appendCommonProperty()
    {
        m_PropColl.appendBool("Common", "Returns/sets symbol in Voice 1 common to all Voices", false, true, "Behavior");
    }
    static OCGraphicsList plotInvisibleIcon(const CSymbol* This, const XMLSimpleSymbolWrapper& Symbol, const double XFysic, OCDraw& ScreenObj) {
        if (ScreenObj.canColor() || ((ScreenObj.col == inactivestaffcolor) && Symbol.isCommon())) {
            const QString buttonProperty = This->buttonProperty();
            int buttonIndex = 0;
            ScreenObj.moveTo(XFysic - (16 * 12), ScoreStaffHeight + (16* 12) + (16 * 12));
            OCGraphicsList l;
            if (!buttonProperty.isEmpty()) buttonIndex = Symbol.getIntVal(buttonProperty);
            OCToolButtonProps* p = This->m_ButtonList[buttonIndex];
            if ((ScreenObj.col == markedcolor) || (ScreenObj.col == selectedcolor)) l.append(ScreenObj.PlRect(16 * 12, 16 * 12, 0, false));
            if (!p->iconpath.isEmpty()) {
                QIcon i = QIcon(p->iconpath);
                l.append(ScreenObj.PlIcon(i, 16 * 12, 16 * 12));
                if ((ScreenObj.col == activestaffcolor) || (ScreenObj.col == inactivestaffcolor)) {
                    QGraphicsPixmapItem* i = (QGraphicsPixmapItem*)l.first();
                    i->setOpacity(0.6);
                }
            }
            else {
                QFont f(p->fontname,int(p->fontsize)*6);
                f.setBold(p->fontbold);
                f.setItalic(p->fontitalic);
                ScreenObj.move(8*12,8*12);
                l.append(ScreenObj.plLet(p->buttonText,0,f, Qt::AlignCenter));
                if ((ScreenObj.col == activestaffcolor) || (ScreenObj.col == inactivestaffcolor)) {
                    QGraphicsPathItem* i = (QGraphicsPathItem*)l.first();
                    i->setOpacity(0.6);
                }
            }
            return l;
        }
        return OCGraphicsList();
    }
public:
    inline CSymbol(const QString& Name) : XMLSimpleSymbolWrapper(Name)
    {
        buildProperties();
    }
    inline CSymbol(const XMLSimpleSymbolWrapper& XMLSymbol) : XMLSimpleSymbolWrapper(XMLSymbol.xml())
    {
        buildProperties();
    }
    inline OCProperties& fromXML(const XMLSimpleSymbolWrapper& XMLSymbol)
    {
        m_PropColl.fromXML(XMLSymbol);
        ModifyProperties(m_PropColl);
        return m_PropColl;
    }
    virtual ~CSymbol();
    virtual OCGraphicsList plot(const XMLSymbolWrapper& /*Symbol*/, double /*XFysic*/, OCPageBarList& /*BarList*/, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType& /*voiceVars*/, const XMLTemplateStaffWrapper& /*sCurrent*/, OCDraw& /*ScreenObj*/) {
        return OCGraphicsList();
    }
    virtual OCGraphicsList plotRemaining(const OCDurSignType& /*s*/, OCNoteList& /*NoteList*/, OCDraw& /*ScreenObj*/)
    {
        return OCGraphicsList();
    }
    virtual void appendSign(const XMLSymbolWrapper& /*Symbol*/, OCPrintSignList& /*SignsToPrint*/, const QColor& /*SignCol*/, const OCBarSymbolLocation& /*Location*/){}
    virtual void Edit(XMLSimpleSymbolWrapper& /*Symbol*/, OCRefreshMode& RefreshMode, bool& esc, QWidget* /*parent*/)
    {
        esc = false;
        RefreshMode = tsReformat;
    }
    virtual void fibPlay(const XMLSymbolWrapper& /*Symbol*/, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &/*TemPlay*/){}
    virtual void fib(const XMLSymbolWrapper& /*Symbol*/,OCPrintVarsType &/*voiceVars*/){}
    virtual void Play(const XMLSymbolWrapper& /*Symbol*/, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &/*TemPlay*/){}
    virtual OCGraphicsList PlotMTrack(double /*XFysic*/, const XMLSymbolWrapper& /*Symbol*/, int /*stavedistance*/, OCPrintVarsType &/*voiceVars*/, const XMLScoreWrapper& /*Score*/, OCDraw& /*ScreenObj*/)
    {
        return OCGraphicsList();
    }
    virtual OCGraphicsList PrintSign(StemDirection /*UpDown*/, int &/*SignsUp*/, OCDraw& /*ScreenObj*/){
        return OCGraphicsList();
    }
    virtual void MoveToSignUp(StemDirection UpDown, int & SignsUp , OCDraw& ScreenObj) {
        SignsUp++;
        PrintProps.moveToVertical(UpDown, (18 + (SignsUp * 12)) * 12, ScreenObj);
    }
    virtual void DuringNote(OCMIDIFile& /*MFile*/, int /*Pitch*/, int& /*LastTime*/, int /*Tick*/, int /*PlayTime*/, OCPlayBackVarsType &/*TemPlay*/){}
    virtual void BeforeNote(const XMLSymbolWrapper& /*XMLNote*/, int& /*PlayDynam*/, int& /*Pitch*/, int& /*endPitch*/, OCPlayBackVarsType &/*TemPlay*/){}
    virtual void AfterNote(const XMLSymbolWrapper& /*XMLNote*/, OCPlayBackVarsType &/*TemPlay*/){}
    virtual OCGraphicsList plotSystemEnd(const XMLSymbolWrapper& /*Symbol*/, double /*XFysic*/, OCPageBarList& /*BarList*/, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType& /*voiceVars*/, const XMLTemplateStaffWrapper& /*sCurrent*/, OCDraw& /*ScreenObj*/)
    {
        return OCGraphicsList();
    }
    virtual void ModifyProperties(OCProperties& /*p*/) {}
    virtual OCProperties* GetDefaultProperties()
    {
        m_PropColl.reset(name());
        return &m_PropColl;
    }
    virtual OCProperties* GetDefaultProperties(int Button)
    {
        m_PropColl.reset(name());
        if (!m_ButtonProperty.isEmpty()) m_PropColl.setPropertyValue(m_ButtonProperty,Button);
        return &m_PropColl;
    }
    bool PropertyExists(const QString &Name) const
    {
        return m_PropColl.contains(Name);
    }
    const QVariant propertyDefaultValue(const QString& Name)
    {
        return m_PropColl.property(Name).Default;
    }
    virtual QList<OCToolButtonProps*> CreateButtons()
    {
        return QList<OCToolButtonProps*>();
    }
    const QIcon symbolIcon() const {
        return QIcon();
    }
    const QString buttonProperty() const {
        return m_ButtonProperty;
    }
    inline double XMLValue(const XMLSimpleSymbolWrapper& XMLSymbol, const QString& name) { return XMLSymbol.xml()->attributeValue(name,m_PropColl.property(name).Default.toDouble()); }
    inline double XMLValue(const QString& name) { return XMLElement->attributeValue(name, m_PropColl.property(name).Default.toDouble()); }
    inline int XMLIntValue(const XMLSimpleSymbolWrapper& XMLSymbol, const QString& name) { return XMLSymbol.xml()->attributeValueInt(name,m_PropColl.property(name).Default.toInt()); }
    inline int XMLIntValue(const QString& name) { return XMLElement->attributeValueInt(name, m_PropColl.property(name).Default.toInt()); }
    inline bool XMLBoolValue(const XMLSimpleSymbolWrapper& XMLSymbol, const QString& name) { return XMLSymbol.xml()->attributeValueBool(name,m_PropColl.property(name).Default.toBool()); }
    inline bool XMLBoolValue(const QString& name) { return XMLElement->attributeValueBool(name, m_PropColl.property(name).Default.toBool()); }
    virtual void DrawFactor(const XMLSymbolWrapper& /*Symbol*/, OCCounter& /*Counter*/, const XMLTemplateWrapper& /*XMLTemplate*/, OCPageBarList& /*BarList*/, const XMLScoreWrapper& /*Score*/){}
    PrintSignProps PrintProps;
    PlaySignProps PlayProps;
};

class CInvisibleSymbol :public CSymbol
{
public:
    inline CInvisibleSymbol(const QString& Name) : CSymbol(Name)
    {
        buildProperties();
    }
    inline CInvisibleSymbol(const XMLSimpleSymbolWrapper& XMLSymbol) : CSymbol(XMLSymbol.xml())
    {
        buildProperties();
    }
    virtual ~CInvisibleSymbol();
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& /*BarList*/, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType& /*voiceVars*/, const XMLTemplateStaffWrapper& /*sCurrent*/, OCDraw& ScreenObj) {
        return CSymbol::plotInvisibleIcon(this,Symbol,XFysic,ScreenObj);
    }
};

class CVisibleSymbol :public CSymbol
{
private:
public:
    inline CVisibleSymbol(const QString& Name, const bool NoInv=false, const bool NoIna=false, const bool NoSize=false) : CSymbol(Name)
    {
        buildProperties(NoInv,NoIna,NoSize);
    }
    inline CVisibleSymbol(const XMLSimpleSymbolWrapper& XMLSymbol, const bool NoInv=false, const bool NoIna=false, const bool NoSize=false) : CSymbol(XMLSymbol)
    {
        buildProperties(NoInv,NoIna,NoSize);
    }
    virtual ~CVisibleSymbol();
protected:
    void buildProperties(const bool NoInv=false, const bool NoIna=false, const bool NoSize=false)
    {
        m_PropColl.appendNumber("Top", -32000, 32000, "Returns/sets the distance between an objects default vertical position and it's current vertical position.",false, "", "Position");
        m_PropColl.appendNumber("Left", -32000, 32000, "Returns/sets the distance between an objects default horizontal position and it's current horizontal position.",false, "", "Position");
        if (!NoSize)  m_PropColl.appendSlider("Size", -9, 9, "Returns/sets the difference between an objects default Size and it's current Size",false, "", "Position");
        if (!NoInv) m_PropColl.appendBool("Invisible", "Returns or sets whether an object is Visible.",false, false, "Behavior");
        if (!NoIna) m_PropColl.appendBool("Inaudible", "Returns or sets whether an object is Audible.",false, false, "Behavior");
    }
};

class CInvisibleDuratedSymbol :public CInvisibleSymbol
{
public:
    inline CInvisibleDuratedSymbol(const QString& Name) : CInvisibleSymbol(Name)
    {
        buildProperties();
    }
    inline CInvisibleDuratedSymbol(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleSymbol(XMLSymbol.xml())
    {
        buildProperties();
    }
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& /*BarList*/, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType& /*voiceVars*/, const XMLTemplateStaffWrapper& /*sCurrent*/, OCDraw& ScreenObj) {
        return CSymbol::plotInvisibleIcon(this,Symbol,XFysic,ScreenObj);
    }
    virtual ~CInvisibleDuratedSymbol();
protected:
    void buildProperties()
    {
        m_PropColl.appendNumber("Ticks", 1, 32000, "Returns/sets the Duration of the Note Group in Ticks. (A Quarter note is 24 Ticks.)",false, "", "Appearance");
    }
};


class CDuratedSymbol : public CVisibleSymbol
{
private:
public:
    inline CDuratedSymbol(const QString& Name, const bool NoInv=false, const bool NoIna=false, const bool NoSize=false) : CVisibleSymbol(Name,NoInv,NoIna,NoSize)
    {
        buildProperties();
    }
    inline CDuratedSymbol(const XMLSimpleSymbolWrapper& XMLSymbol, const bool NoInv=false, const bool NoIna=false, const bool NoSize=false) : CVisibleSymbol(XMLSymbol,NoInv,NoIna,NoSize)
    {
        buildProperties();
    }
    virtual ~CDuratedSymbol();
protected:
    void buildProperties()
    {
        m_PropColl.appendNumber("Ticks", 1, 32000, "Returns/sets the Duration of the Note Group in Ticks. (A Quarter note is 24 Ticks.)",false, "", "Appearance");
    }
};

class CGapSymbol : public CDuratedSymbol
{
private:
public:
    inline CGapSymbol(const QString& Name, const QString& GapName, const QString& Description) : CDuratedSymbol(Name,false,false,true)
    {
        buildProperties(GapName,Description);
    }
    inline CGapSymbol(const XMLSimpleSymbolWrapper& XMLSymbol, const QString& GapName, const QString& Description) : CDuratedSymbol(XMLSymbol,false,false,true)
    {
        buildProperties(GapName,Description);
    }
    virtual ~CGapSymbol();
protected:
    void buildProperties(const QString& GapName, const QString& Description)
    {
        m_PropColl.appendNumber(GapName, -9, 20, Description,false, "", "Appearance");
    }
};

class CTextSymbol : public CVisibleSymbol
{
private:
public:
    inline CTextSymbol(const QString& Name) : CVisibleSymbol(Name,false,true)
    {
        buildProperties();
    }
    inline CTextSymbol(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol,false,true)
    {
        buildProperties();
    }
    virtual ~CTextSymbol();
protected:
    void buildProperties()
    {
        m_PropColl.appendCustom("Custom", "Shows the Text Dialog",false, "", "Appearance");
        m_PropColl.appendText("Text", "Returns/sets the visible text",false, "Text", "Appearance");
        m_PropColl.appendText("FontName", "Returns/sets the name of the font used",false, "Times new Roman", "Appearance");
        m_PropColl.appendBool("Italic", "Returns/sets whether the text is shown as italic",false, false, "Appearance");
        m_PropColl.appendBool("Bold", "Returns/sets whether the text is shown as bold",false, false, "Appearance");
        m_PropColl.appendNumber("FontSize", 8, 300, "Returns/sets the size of the text",false, 8.25, "Appearance");
    }
};

#pragma pack(pop)

#endif // CSYMBOL_H
