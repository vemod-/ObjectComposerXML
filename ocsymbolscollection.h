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
    static bool editevent(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, QWidget* parent);
    static OCGraphicsList PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& voiceVars, const XMLScoreWrapper& Score, OCDraw& ScreenObj);
    static void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    static void fibCommon(const XMLSymbolWrapper& Symbol, OCStaffCounterPrint& voiceVarsArray, const OCVoiceLocation& VoiceLocation);
    static void Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    static OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, const QColor& SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper &XMLTemplateStaff, OCDraw& ScreenObj);
    static OCGraphicsList plotRemaining(const OCDurSignType& s, OCNoteList& NoteList, OCDraw& ScreenObj);
    static void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, OCPrintVarsType &voiceVars, const OCBarSymbolLocation& Location);
    static OCGraphicsList plotSystemEnd(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, const QColor& SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper &XMLTemplateStaff, OCDraw& ScreenObj);
    static void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    static void DrawFactor(const XMLSymbolWrapper& Symbol, OCCounter& Counter, const XMLTemplateWrapper& XMLTemplate, OCPageBarList& BarList, const XMLScoreWrapper& Score);
    static OCProperties& GetProperties(const XMLSimpleSymbolWrapper& Symbol);
    static void ChangeProperty(XMLSimpleSymbolWrapper& Symbol,const QString& Name,const QVariant& Value);
    static void ChangeProperties(XMLSimpleSymbolWrapper& Symbol,const QStringList& Names,const QVariant& Value);
    static void ModifyProperties(OCProperties& Properties);
    static bool PropetyExists(const QString& SymbolName, const QString& PropertyName);
    static XMLSimpleSymbolWrapper GetDefaultSymbol(const QString& SymbolName);
    static XMLSimpleSymbolWrapper GetDefaultSymbol(const QString& SymbolName, const int Button);
    static OCToolButtonProps* ButtonProps(const QString& SymbolName, const int Button);
    static int ButtonCount(const QString& SymbolName);

    const static QStringList Classes();
    //const QStringList CommonSymbols{"Clef", "Key", "Scale", "Time", "Channel", "Patch", "Dynamic", "DynamicChange","Hairpin", "Octave", "Transpose", "Fermata","Pedal"};
    const static QStringList Categories();
    const static QIcon Icon(const QString& SymbolName, const int Button);
    const static QIcon SymbolIcon(const XMLSimpleSymbolWrapper& Symbol);
private:
    static int refCount;
    static QMap<QString, CSymbol*> Symbols;
    static QMap<QString, QList<OCToolButtonProps*> > Buttons;
    static QMap<QString, QList<QIcon> > Icons;
    static QStringList Cats;
    static OCProperties* GetDefaultProperties(const QString& SymbolName, const int Button);
    static OCProperties* GetDefaultProperties(const QString& SymbolName);
};

class CNote :public CVisibleSymbol
{
public:
    ~CNote();
    inline CNote(const QString& Name) : CVisibleSymbol(Name)
    {
        buildProperties();
    }
    inline CNote(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    /*
    const QIcon symbolIcon() {
        const QString SymbolName = name();
        if (SymbolName == "Rest") {
            return QIcon(":/Notes/Notes/quarterrest.png");
        }
        return QIcon(":/Notes/Notes/"+QString::number(noteValue())+".png");
    }
*/
    QList<OCToolButtonProps*> CreateButtons()
    {
        if (name()=="Rest")
        {
            CreateButton("",":/Notes/Notes/quarterrest.png");
        }
        else
        {
            for (int i = 0; i < 7; i++) CreateButton("",":/Notes/Notes/"+QString::number(i)+".png",false,tsReformat,"Add Note");
        }
        m_ButtonList[0]->ishidden=true;
        return m_ButtonList;
    }
    void ModifyProperties(OCProperties& p)
    {
        if (name() == "Rest")
        {
            p.show("Top");
            p.hide("NoteType");
            p.hide("NoteHeadType");
            p.hide("Pitch");
            p.hide("AccidentalLeft");
            p.hide("AccidentalType");
            p.hide("AccidentalParentheses");
            p.hide("TieTop");
            p.hide("TieCurve");
            p.hide("TieWeight");
            if (p.propertyValue("NoteValue")==7)
            {
                p.hide("Dotted");
                p.hide("Triplet");
            }
            else
            {
                p.show("Dotted");
                p.show("Triplet");
            }
        }
        else
        {
            p.show("NoteType");
            p.show("NoteHeadType");
            p.show("Pitch");
            p.show("AccidentalLeft");
            p.show("AccidentalType");
            p.show("AccidentalParentheses");
            p.show("TieTop");
            p.show("TieCurve");
            p.show("TieWeight");
            p.show("Top");
        }
    }
    OCProperties* GetDefaultProperties()
    {
        m_PropColl.reset(name());
        return &m_PropColl;
    }
protected:
    void buildProperties()
    {
        QStringList ListArr{"Whole", "Half", "Quarter", "8th", "16th", "32th", "64th"};
        if (name()=="Rest") ListArr  << "1 Bar";
        m_PropColl.appendList("NoteValue", "The Time value of a Note or a Pause.", ListArr,0,false ,"" , "Appearance");
        m_PropColl.appendList("Dotted", "Adds 50% or 75% to the Time value of a Note or a Pause.",QStringList{"No dot","Dotted","Double dotted"} ,0,false ,"" , "Appearance");
        m_PropColl.appendBool("Triplet", "Subtracts 1/3 from the Time value of a Note or a Pause.",false ,"" , "Appearance");
        m_PropColl.appendList("NoteType", "Returns/sets the Type of a Note. The Type can be Single Note or Compound Note, and notes can be Tied.", QStringList{"Note", "Tied Note", "Chord Note", "Tied Chord Note","Grace Note","Tied Grace Note"},0,false ,"" , "Appearance");
        m_PropColl.appendList("NoteHeadType","Returns/sets the Shape of the Note head.",{"Normal","X","Diamond"},0,false,"","Appearance");
        m_PropColl.appendNumber("Pitch", 0, 127, "Returns/sets the Pitch of a Note in MIDI key numbers.",false ,"" , "Appearance");
        m_PropColl.appendNumber("AccidentalLeft", -32000, 32000, "Returns/sets the distance between an Accidentals default horizontal position and it's current horizontal position.",false ,"" , "Accidental");
        m_PropColl.appendList("AccidentalType","Returns/sets the type of Accidental.",{"Auto","Hidden","b","#","bb","x","nat"}, 0, false,"", "Accidental");
        m_PropColl.appendBool("AccidentalParentheses", "Returns/sets Parentheses around the Accidentals", false ,"" , "Accidental");
        m_PropColl.appendNumber("TieTop", -32000, 32000, "Returns/sets the distance between a Ties default vertical position and it's current vertical position.",false ,"" , "Tie");
        m_PropColl.appendNumber("TieCurve", -9, 20, "Returns/sets the difference between the current Curve and the default Curve.",false, "", "Tie");
        m_PropColl.appendNumber("TieWeight",-10,10,"Returns/sets the horizontal Weight point",false,0,"Tie");
        m_ButtonProperty = "NoteValue";
    }
};

class CDynamic :public CVisibleSymbol
{
public:
    inline CDynamic() : CVisibleSymbol("Dynamic")
    {
        buildProperties();
    }
    inline CDynamic(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    OCProperties* GetDefaultProperties(int Button);
    static const QStringList DynamicList;
protected:
    void buildProperties()
    {
        appendCommonProperty();
        m_PropColl.appendList("DynamicSign", "Returns/sets the Type of Dynamic Sign.", DynamicList, 0,false, "", "Appearance",true);
        m_PropColl.appendNumber("Velocity", 1, 127, "Returns/sets the MIDI execution Velocity of the Notes that follows.",false, 1, "Behavior");
        m_ButtonProperty="DynamicSign";
    }
};

class CPatch :public CInvisibleSymbol
{
public:
    inline CPatch() : CInvisibleSymbol("Patch")
    {
        buildProperties();
    }
    inline CPatch(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    void Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    static QStringList PatchList[2];
protected:
    void buildProperties()
    {
        if (PatchList[0].isEmpty())
        {
            /*
            //PatchList.append("-");
            QFile fileData(":/OCPatches.txt");
            if (fileData.open(QIODevice::ReadOnly))
            {
                QTextStream readData(&fileData);
                while (!readData.atEnd()) PatchList << readData.readLine();
                fileData.close();
            }
*/
            QDomLiteDocument doc;
            if (doc.load(":/midi_instruments_controllers.xml"))
            {
                const QDomLiteElementList instruments = doc.documentElement->firstChild()->firstChild()->childElements;
                for (const QDomLiteElement* e : instruments) PatchList[0] << e->attribute("name");
                const QDomLiteElementList drumsets = doc.documentElement->firstChild()->lastChild()->childElements;
                for (const QDomLiteElement* e : drumsets) PatchList[1] << e->attribute("name");
            }
            else
            {
                for (int i=0;i<128;i++) {
                    PatchList[0].append(QString::number(i+1));
                    PatchList[1].append(QString::number(i+1));
                }
            }
        }
        appendCommonProperty();
        m_PropColl.appendList("Patch","Sets/returns current MIDI Patch",PatchList[0],1,false,1,"Behavoir");
    }
};

class CTime :public CVisibleSymbol
{
public:
    inline CTime():CVisibleSymbol("Time",false,true)
    {
        buildProperties();
    }
    inline CTime(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol,false,true)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void DrawFactor(const XMLSymbolWrapper& Symbol, OCCounter& Counter, const XMLTemplateWrapper& XMLTemplate, OCPageBarList& BarList, const XMLScoreWrapper& Score);
    void ModifyProperties(OCProperties& p);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    OCGraphicsList plotSystemEnd(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    static const QStringList TimeList;
    static OCGraphicsList PlTime(const XMLSymbolWrapper& Symbol, double X, OCDraw& ScreenObj, Qt::Alignment Align=Qt::AlignCenter);
    //static int GetTicks(const XMLSymbolWrapper& Symbol);
    static int CalcBeamLimit(const XMLSymbolWrapper& Symbol);
protected:
    void buildProperties()
    {
        appendCommonProperty();
        m_PropColl.appendCustom("Time", "Shows the Time Dialog.",false, "", "Appearance");
        m_PropColl.appendList("TimeType", "Returns/sets the type of the time signature", TimeList, 0,false, 0, "Behavior",true);
        m_PropColl.appendNumber("Upper", 1, 200, "Returns/sets the upper number in the time signature",false, 4, "Behavior");
        m_PropColl.appendNumber("Lower", 1, 200, "Returns/sets the lower number in the time signature",false, 4, "Behavior");
    }
};

class CTuplet :public CDuratedSymbol
{
public:
    inline CTuplet() : CDuratedSymbol("Tuplet")
    {
        buildProperties();
    }
    inline CTuplet(const XMLSimpleSymbolWrapper& XMLSymbol) : CDuratedSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
protected:
    void buildProperties()
    {
        m_PropColl.appendCustom("TupletValue", "Returns/Sets the Number of Ticks to fit the specified Notes into",false, 24, "Appearance");
    }
};

class CClef :public CVisibleSymbol
{
public:
    inline CClef() : CVisibleSymbol("Clef",false,true)
    {
        buildProperties();
    }
    inline CClef(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol,false,true)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void DrawFactor(const XMLSymbolWrapper& Symbol, OCCounter& Counter, const XMLTemplateWrapper& XMLTemplate, OCPageBarList& BarList, const XMLScoreWrapper& Score);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    OCGraphicsList plotSystemEnd(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    const QStringList ClefList{"Soprano", "Bass", "Alto", "Tenor", "Percussion"};
    static OCGraphicsList PlClef(int Clef, int Size, OCDraw& ScreenObj);
    static int LineDiff(const int Clef) {
        switch (Clef)
        {
            case 2: return 4;
            case 3: return 28;
            case 4: return 20;
            default: return 52;
        }
    }
protected:
    void buildProperties()
    {
        appendCommonProperty();
        m_PropColl.appendList("Clef", "Returns/sets the Type of Clef.", ClefList, 0,false, "", "Appearance",true);
        m_ButtonProperty="Clef";
    }
};

class CTranspose :public CInvisibleSymbol
{
public:
    inline CTranspose() :CInvisibleSymbol("Transpose")
    {
        buildProperties();
    }
    inline CTranspose(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
protected:
    void buildProperties()
    {
        appendCommonProperty();
        m_PropColl.appendNumber("Transpose", -48, 48, "Returns/sets the amount of Transposition in half tones.",false, "", "Behavior");
    }
};

class CKey :public CVisibleSymbol
{
public:
    inline CKey() : CVisibleSymbol("Key",false,true,true)
    {
        buildProperties();
    }
    inline CKey(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol,false,true,true)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void DrawFactor(const XMLSymbolWrapper& Symbol, OCCounter& Counter, const XMLTemplateWrapper& XMLTemplate, OCPageBarList& BarList, const XMLScoreWrapper& Score);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    OCGraphicsList plotSystemEnd(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    static OCGraphicsList plotKey(int Key, QPointF Pos, int CurrentClef, OCDraw& ScreenObj);
    static const QStringList KeyList;
    static uint NumOfAccidentals(int Key);
    static OCKeyAccidental AccidentalFlag(int Key);
protected:
    void buildProperties()
    {
        appendCommonProperty();
        m_PropColl.appendList("Key", "Returns/sets the Key.", KeyList, 0,false, 6, "Appearance",true);
    }
};

class CScale :public CInvisibleSymbol
{
public:
    inline CScale() : CInvisibleSymbol("Scale")
    {
        buildProperties();
    }
    inline CScale(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
protected:
    void buildProperties()
    {
        appendCommonProperty();
        m_PropColl.appendCustom("Accidentals", "Shows the Scale Dialog.",false, "", "Behavior");
        for (int i=0;i<12;i++)
        {
            m_PropColl.appendNumber("Step"+QString::number(i+1), 0, 2, "", true);
        }
    }
};

class CTempo :public CVisibleSymbol
{
public:
    inline CTempo() : CVisibleSymbol("Tempo")
    {
        buildProperties();
    }
    inline CTempo(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    OCGraphicsList PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& voiceVars, const XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    const QStringList TempoList{"Whole", "Half", "Quarter", "8th", "16th"};
    static OCGraphicsList PlTempo(int Tempo, int NoteVal, bool Dotted, const QFont& Font, int Size, OCDraw& ScreenObj);
protected:
    void buildProperties()
    {
        m_PropColl.appendNumber("Tempo", 20, 300, "Returns/sets the Tempo.",false, 120, "Appearance",true);
        m_PropColl.appendList("NoteValue", "Returns/sets the Note Value the Tempo relates to.", TempoList, 0,false, 2, "Appearance");
        m_PropColl.appendList("Dotted", "Adds 50% or 75% to the Time of the NoteValue.", QStringList{"No dot","Dotted","Double dotted"}, 0,false, "", "Appearance");
    }
};

class CSysEx :public CInvisibleSymbol
{
public:
    CSysEx() : CInvisibleSymbol("SysEx")
    {
        m_PropColl.appendCustom("Custom", "Shows the SysEx Dialog",false, "", "Behavior");
        m_PropColl.appendText("SysExString", "Returns/sets the Hexvalues sent to the synth",false, "", "Behavior");
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
};

class CController :public CInvisibleSymbol
{
public:
    CController() :CInvisibleSymbol("Controller")
    {
        if (ControllerList.isEmpty())
        {
            /*
            QFile fileData(":/OCControllers.txt");
            if (fileData.open(QIODevice::ReadOnly))
            {
                QTextStream readData(&fileData);
                while (!readData.atEnd()) ControllerList << readData.readLine();
                fileData.close();
            }
*/
            QDomLiteDocument doc;
            if (doc.load(":/midi_instruments_controllers.xml"))
            {
                const QDomLiteElementList controllers = doc.documentElement->lastChild()->childElements;
                for (const QDomLiteElement* e : controllers) ControllerList << e->attribute("name");
            }
            else
            {
                for (int i=0;i<128;i++) ControllerList.append(QString::number(i));
            }
        }
        m_PropColl.appendCustom("Controller", "Shows the Controller Dialog.",false, "", "Behavior");
        m_PropColl.appendList("Controller", "Returns/sets the Controller Number.", ControllerList, 0,false, "", "Behavior");
        m_PropColl.appendNumber("Value", 0, 127, "Returns/sets the Controller Value.",false, "", "Behavior");
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    static QStringList ControllerList;
};

class CChannel :public CInvisibleSymbol
{
public:
    CChannel() :CInvisibleSymbol("Channel")
    {
        appendCommonProperty();
        m_PropColl.appendNumber("Channel", 1, 16, "Returns/sets the MIDI Channel to send on.",false, 1, "Behavior");
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
};

class CAccent :public CVisibleSymbol
{
public:
    inline CAccent() : CVisibleSymbol("Accent")
    {
        buildProperties();
    }
    inline CAccent(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void BeforeNote(const XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCPlayBackVarsType& voiceVars);
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    void Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
    OCProperties* GetDefaultProperties(int Button);
protected:
    void buildProperties()
    {
        m_PropColl.appendNumber("AddToVelocity", 0, 127, "Returns/sets the amount of Velocity to add to the Note of the Accent.",false, 20, "Behavior");
    }
};

class Cfp : public CVisibleSymbol
{
public:
    inline Cfp() : CVisibleSymbol("fp")
    {
        buildProperties();
    }
    inline Cfp(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void BeforeNote(const XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCPlayBackVarsType& voiceVars);
    void DuringNote(OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& voiceVars);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    OCProperties* GetDefaultProperties(int Button);
private:
    int FPcount;
protected:
    void buildProperties()
    {
        m_PropColl.appendNumber("StartVelocity", 0, 127, "Returns/sets the initial Velocity of the Note of the fp sign.",false, "", "Behavior");
        m_PropColl.appendNumber("EndVelocity", 0, 127, "Returns/sets the Velocity of the voice after the fp sign.",false, "", "Behavior");
        FPcount=0;
    }
};

class Cfz : public CVisibleSymbol
{
public:
    inline Cfz() : CVisibleSymbol("fz")
    {
        buildProperties();
    }
    inline Cfz(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void BeforeNote(const XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCPlayBackVarsType& voiceVars);
    void DuringNote(OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& voiceVars);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    void Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    OCProperties* GetDefaultProperties(int Button);
private:
    int FPcount;
protected:
    void buildProperties()
    {
        m_PropColl.appendNumber("AddToVelocity", 0, 127, "Returns/sets the amount of Velocity to add to the Note of the fz sign.",false, "", "Behavior");
        FPcount=0;
    }
};
/*
class CAccidental :public CVisibleSymbol
{
public:
    inline CAccidental() :CVisibleSymbol("Accidental",false,true)
    {
        buildProperties();
    }
    inline CAccidental(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol,false,true)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
    const QStringList AccidentalList{"Flat", "Sharp", "DblFlat", "DblSharp", "Natural"};
private:
    OCGraphicsList plLeftParanthesis(OCDraw& ScreenObj);
protected:
    void buildProperties()
    {
        m_PropColl.appendList("AccidentalSign", "Returns/sets the Accidentals Type.", AccidentalList, 0,false, "", "Appearance",true);
        m_PropColl.appendBool("Parentheses", "Returns or sets whether the Accidental has Parantheses.",false, "", "Appearance");
        m_ButtonProperty="AccidentalSign";
    }
};
*/
class CBowing :public CVisibleSymbol
{
public:
    inline CBowing() : CVisibleSymbol("Bowing",false,true)
    {
        buildProperties();
    }
    inline CBowing(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol,false,true)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
    const QStringList BowingList{"Up", "Down"};
protected:
    void buildProperties()
    {
        m_PropColl.appendList("Bowing", "Returns/sets the Type of Bowing.", BowingList, 0,false, "", "Appearance",true);
        m_ButtonProperty="Bowing";
    }
};

class CBartokP :public CVisibleSymbol
{
public:
    inline CBartokP() : CVisibleSymbol("BartokPizz",false,true) {}
    inline CBartokP(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol,false,true) {}
    QList<OCToolButtonProps*> CreateButtons();
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
};

class CFingering : public CVisibleSymbol
{
public:
    inline CFingering() : CVisibleSymbol("Fingering",false,true)
    {
        buildProperties();
    }
    inline CFingering(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol,false,true)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
    static const QStringList FingerList;
protected:
    void buildProperties()
    {
        m_PropColl.appendList("Finger", "Returns/sets the Number.", FingerList, 0,false, "", "Appearance",true);
        m_PropColl.appendBool("LeadingLine", "Returns or sets whether the Fingering has a Leading Line.",false, "", "Appearance");
        m_ButtonProperty="Finger";
    }
};

class CStringNumber : public CVisibleSymbol
{
public:
    inline CStringNumber() : CVisibleSymbol("StringNumber",false,true)
    {
        buildProperties();
    }
    inline CStringNumber(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol,false,true)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
    static const QStringList StringSigns;
protected:
    void buildProperties()
    {
        m_PropColl.appendList("String", "Returns/sets the Number.", StringSigns, 0,false, "", "Appearance",true);
        m_ButtonProperty="String";
    }
};

class CBarWidth : public CInvisibleSymbol
{
public:
    inline CBarWidth() : CInvisibleSymbol("BarWidth")
    {
        buildProperties();
    }
    inline CBarWidth(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void DrawFactor(const XMLSymbolWrapper& Symbol, OCCounter& Counter, const XMLTemplateWrapper& XMLTemplate, OCPageBarList& BarList, const XMLScoreWrapper& Score);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
protected:
    void buildProperties()
    {
        m_PropColl.appendCustom("BarWidthValue", "Returns/sets the Note Value the Bar is formatted for.",false, 24, "Behavior");
    }
};

class CBeam : public CInvisibleDuratedSymbol
{
public:
    inline CBeam() : CInvisibleDuratedSymbol("Beam")
    {
        buildProperties();
    }
    inline CBeam(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleDuratedSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    const QStringList BeamList{"Default", "1", "2", "3", "4"};
protected:
    void buildProperties()
    {
        m_PropColl.appendList("Beams", "Returns/sets the Number of Beams on the Note Group.", BeamList, 0,false, "", "Appearance");
    }
};

class CCue : public CVisibleSymbol
{
public:
    inline CCue() : CVisibleSymbol("Cue",false,true)
    {
        buildProperties();
    }
    inline CCue(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol,false,true)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    OCGraphicsList PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& voiceVars, const XMLScoreWrapper& Score, OCDraw& ScreenObj);
    static const QString cueletter(int i);
protected:
    const QStringList CueTypeList = {"Letters","Numbers"};
    void buildProperties()
    {
        m_PropColl.appendList("Type", "Returns/sets the Type of Cue.", CueTypeList, 0,false, 0, "Appearance",true);
        m_PropColl.appendBool("Reset","Returns/sets whether the cueletter should start from the beginning of the alphabet",false,false,"Appearance");
    }
};

class CCoda : public CVisibleSymbol
{
public:
    inline CCoda() : CVisibleSymbol("Coda")
    {
        buildProperties();
    }
    inline CCoda(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    OCGraphicsList PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& voiceVars, const XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
protected:
    void buildProperties()
    {
        m_PropColl.appendList("CodaType", "Returns/sets the Type of Coda Sign.", QStringList{"Coda", "To Coda"}, 0,false, "", "Appearance",true);
        m_ButtonProperty="CodaType";
    }
};

class CDaCapo :public CVisibleSymbol
{
public:
    inline CDaCapo() :CVisibleSymbol("DaCapo") {}
    inline CDaCapo(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol) {}
    QList<OCToolButtonProps*> CreateButtons();
    OCGraphicsList PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& voiceVars, const XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
protected:
    void buildProperties()
    {
        appendCommonProperty();
    }

};

class CDobbel :public CVisibleSymbol
{
public:
    inline CDobbel() :CVisibleSymbol("Turn")
    {
        buildProperties();
    }
    inline CDobbel(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
    void DuringNote(OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& voiceVars);
    void BeforeNote(const XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCPlayBackVarsType& voiceVars);
    void Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCProperties* GetDefaultProperties(int Button);
protected:
    int trilldynam;
    int BasePitch;
    int CurrentPitch;
    int oldpitch;
    int Currentstep;
    bool BeginningOfNote();
    void buildProperties()
    {
        m_PropColl.appendList("Direction", "Returns/sets the Shape of the turn", QStringList{"Up Down", "Down Up"}, 0,false, "", "Behavior",true);
        m_PropColl.appendNumber("Speed", 1, 200, "Returns/sets the MIDI execution Speed of the Ornament.",false, 50, "Behavior");
        m_PropColl.appendList("Timing", "Returns/sets the MIDI execution style.", QStringList{"Late", "Early"}, 0,false, "", "Behavior");
        m_PropColl.appendNumber("RangeDn", 1, 12, "Returns/sets the Downwards interval in half tones.",false, 1, "Behavior");
        m_PropColl.appendNumber("RangeUp", 1, 12, "Returns/sets the Upwards interval in half tones.",false, 2, "Behavior");
        m_ButtonProperty="Direction";
        trilldynam = 0;
        Currentstep = 0;
    }
};

class CDurLength : public CInvisibleDuratedSymbol
{
public:
    inline CDurLength() : CInvisibleDuratedSymbol("DuratedLength")
    {
        buildProperties();
    }
    inline CDurLength(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleDuratedSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    OCProperties* GetDefaultProperties(int Button);
    const QStringList LengthList{"Length", "Tenuto", "Staccato"};
protected:
    void buildProperties()
    {
        m_PropColl.appendList("PerformanceType", "Returns/sets the Type of sign.", LengthList, 0,false, "", "Appearance");
        m_PropColl.appendNumber("Legato", 1, 100, "Returns/sets the MIDI execution length of the Legato Notes in Percent.",false, 80, "Behavior");
        m_ButtonProperty="PerformanceType";
    }
};

class CDurSlant : public CInvisibleDuratedSymbol
{
public:
    inline CDurSlant() : CInvisibleDuratedSymbol("DuratedSlant")
    {
        buildProperties();
    }
    inline CDurSlant(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleDuratedSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    OCProperties* GetDefaultProperties(int Button);
protected:
    void buildProperties()
    {
        m_PropColl.appendBool("Slanting", "Returns or sets whether Beam slanting is on.",false, "", "Behavior");
    }
};

class CDurUpDown : public CInvisibleDuratedSymbol
{
public:
    inline CDurUpDown() : CInvisibleDuratedSymbol("DuratedBeamDirection")
    {
        buildProperties();
    }
    inline CDurUpDown(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleDuratedSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
protected:
    void buildProperties()
    {
        m_PropColl.appendList("Direction", "Returns or sets whether the Stem Direction of the Notes in the Group is up or down.", QStringList{"Up", "Down"}, 0,false, "", "Behavior");
        m_ButtonProperty="Direction";
    }
};

class CDynChange : public CVisibleSymbol
{
public:
    inline CDynChange() : CVisibleSymbol("DynamicChange")
    {
        buildProperties();
    }
    inline CDynChange(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    void AfterNote(const XMLSymbolWrapper& XMLNote, OCPlayBackVarsType& voiceVars);
    OCProperties* GetDefaultProperties(int Button);
    static const QStringList DynamicChangeList;
protected:
    void buildProperties()
    {
        appendCommonProperty();
        m_PropColl.appendList("DynamicType", "Returns/sets the Type of Dynamic Change.", DynamicChangeList, 0,false, "", "Appearance",true);
        m_PropColl.appendNumber("Speed", 1, 100, "Returns/sets the MIDI execution speed of the Dynamic Change.",false, 50, "Behavior");
        m_ButtonProperty="DynamicType";
    }
};

class CExpression : public CInvisibleSymbol
{
public:
    inline CExpression() : CInvisibleSymbol("Expression")
    {
        buildProperties();
    }
    inline CExpression(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
protected:
    void buildProperties()
    {
        m_PropColl.appendBool("Disabled", "Returns or sets whether the use of Expression controllers is on.",false, "", "Behavior");
    }
};

class CFermata : public CVisibleSymbol
{
public:
    inline CFermata() : CVisibleSymbol("Fermata")
    {
        buildProperties();
    }
    inline CFermata(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
protected:
    void buildProperties()
    {
        appendCommonProperty();
        m_PropColl.appendNumber("Duration", 0, 960, "Returns/sets the MIDI execution time of the Fermata in MIDI Ticks",false, 240, "Behavior");
    }
};

class CFine : public CVisibleSymbol
{
public:
    inline CFine() : CVisibleSymbol("Fine") {}
    inline CFine(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol) {}
    QList<OCToolButtonProps*> CreateButtons();
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    OCGraphicsList PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& voiceVars, const XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
};

class CHairpin : public CGapSymbol
{
public:
    inline CHairpin() :CGapSymbol("Hairpin", "Gap", "Returns/sets the angle of the Hairpin signs.")
    {
        buildProperties();
    }
    inline CHairpin(const XMLSimpleSymbolWrapper& XMLSymbol) : CGapSymbol(XMLSymbol, "Gap", "Returns/sets the angle of the Hairpin signs.")
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    void Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    OCGraphicsList plotRemaining(const OCDurSignType& s, OCNoteList& NoteList, OCDraw& ScreenObj);
    void DuringNote(OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& voiceVars);
    void BeforeNote(const XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCPlayBackVarsType& voiceVars);
    void AfterNote(const XMLSymbolWrapper& XMLNote, OCPlayBackVarsType& voiceVars);
    OCProperties* GetDefaultProperties(int Button);
private:
    int Speed();
    int AfterNoteDuration;
    int Direction;
    int TickCount;
protected:
    void buildProperties()
    {
        appendCommonProperty();
        m_PropColl.appendNumber("Speed", 0, 127, "Returns/sets the MIDI execution speed of the dynamic change.",false, 50, "Behavior");
        m_PropColl.appendList("HairpinType", "Returns or sets the direction of the dynamic change.", QStringList{"<", ">", "><", "<>"}, 0,false, "", "Appearance",true);
        m_ButtonProperty="HairpinType";
        TickCount=0;
    }
};

class CFlipTie : public CInvisibleSymbol
{
public:
    inline CFlipTie() : CInvisibleSymbol("FlipTie")
    {
        buildProperties();
    }
    inline CFlipTie(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    const QStringList FlipList{"Flipped", "Normal"};
protected:
    void buildProperties()
    {
        m_PropColl.appendList("TieDirection", "Returns or sets whether Ties should be shown upside down.", FlipList, 0,false, "", "Behavior");
    }
};

class CGliss :public CVisibleSymbol
{
public:
    inline CGliss() :CVisibleSymbol("Glissando")
    {
        buildProperties();
    }
    inline CGliss(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
    void DuringNote(OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& voiceVars);
    void BeforeNote(const XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCPlayBackVarsType& voiceVars);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
private:
    int CurrentPitch;
    int oldpitch;
    int Direction;
    int GlissModulate;
protected:
    void buildProperties()
    {
        m_PropColl.appendNumber("Range", -48, 48, "Returns/sets the range of the Glissando in half tones.",false, "", "Behavior",true);
        GlissModulate=0;
    }
};

class CLength :public CVisibleSymbol
{
public:
    inline CLength() : CVisibleSymbol("Length")
    {
        buildProperties();
    }
    inline CLength(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void ModifyProperties(OCProperties& p);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    OCProperties* GetDefaultProperties(int Button);
    const QStringList LengthList{"Length", "Tenuto", "Staccato", "Length x1", "Tenuto x1", "Staccato x1"};
protected:
    void buildProperties()
    {
        m_PropColl.appendList("PerformanceType", "Returns/sets the Type of sign.", LengthList, 0,false, "", "Appearance",true);
        m_PropColl.appendNumber("Legato", 1, 100, "Returns/sets the MIDI execution length of the Note(s) in Percent.",false, 80, "Behavior");
        m_ButtonProperty="PerformanceType";
    }
};

class CLimit : public CInvisibleSymbol
{
public:
    inline CLimit() : CInvisibleSymbol("BeamLimit")
    {
        buildProperties();
    }
    inline CLimit(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
protected:
    void buildProperties()
    {
        m_PropColl.appendNumber("SixteenthsNotes", 0, 64, "Returns/sets the Max number of sixteenth notes to fit under a Beam.",false, 4, "Behavior");
    }
};

class CMordent :public CVisibleSymbol
{
public:
    inline CMordent() :CVisibleSymbol("Mordent")
    {
        buildProperties();
    }
    inline CMordent(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
    void DuringNote(OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& voiceVars);
    void BeforeNote(const XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCPlayBackVarsType& voiceVars);
    void Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCProperties* GetDefaultProperties(int Button);
protected:
    int trilldynam;
    int BasePitch;
    int CurrentPitch;
    int oldpitch;
    bool Finished;
    void buildProperties()
    {
        m_PropColl.appendNumber("Speed", 1, 200, "Returns/sets the MIDI execution Speed of the Ornament.",false, 50, "Behavior");
        m_PropColl.appendNumber("Range", -12, 12, "Returns/sets the Downwards interval in half tones.",false, -2, "Behavior");
        trilldynam = 0;
        Finished = false;
    }
};

class COctave :public CVisibleSymbol
{
public:
    inline COctave() :CVisibleSymbol("Octave")
    {
        buildProperties();
    }
    inline COctave(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    const QStringList OctaveList{"15ma Down", "8va Down", "Loco", "8va Up", "15ma Up"};
protected:
    void buildProperties()
    {
        appendCommonProperty();
        m_PropColl.appendList("OctaveType", "Returns/sets the Type of Octave change.", OctaveList, 0,false, "", "Appearance",true);
        m_ButtonProperty="OctaveType";
    }
};

class CPortamento :public CInvisibleSymbol
{
public:
    inline CPortamento() :CInvisibleSymbol("Portamento")
    {
        buildProperties();
    }
    inline CPortamento(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    static QStringList PortamentoList;
protected:
    void buildProperties()
    {
        m_PropColl.appendList("Portamento", "Returns or sets whether the use of MIDI Portamento Controllers is on.", PortamentoList, 0,false, "", "Behavior");
        m_PropColl.appendNumber("Time", 0, 127, "Returns or sets MIDI Portamento Time.",false, 0x10, "Behavior");
    }
};

class CRepeat : public CVisibleSymbol
{
public:
    inline CRepeat() :CVisibleSymbol("Repeat")
    {
        buildProperties();
    }
    inline CRepeat(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void ModifyProperties(OCProperties& p);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    OCGraphicsList plotSystemEnd(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    const QStringList RepeatList{"End", "Begin", "Volta", "Barline"};
protected:
    void buildProperties()
    {
        m_PropColl.appendList("RepeatType", "Returns/sets the Type of Repeat sign.", RepeatList, 0,false, "", "Appearance");
        m_PropColl.appendNumber("Repeats", 1, 127, "Returns/sets the number of Repeats.",false, 2, "Appearance",true);
        m_PropColl.appendNumber("Volta", 1, 127, "Returns/sets the Volta.",false, 1, "Appearance");
        m_ButtonProperty="RepeatType";
    }
};

class CSegno : public CVisibleSymbol
{
public:
    inline CSegno() : CVisibleSymbol("Segno")
    {
        buildProperties();
    }
    inline CSegno(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    OCGraphicsList PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& voiceVars, const XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
protected:
    void buildProperties()
    {
        m_PropColl.appendList("SegnoType", "Returns/sets the Type of Segno Sign.", QStringList{"dal Segno", "Segno"}, 0,false, "", "Appearance",true);
        m_ButtonProperty="SegnoType";
    }
};

class CSlant : public CInvisibleSymbol
{
public:
    inline CSlant() : CInvisibleSymbol("BeamSlant")
    {
        buildProperties();
    }
    inline CSlant(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    const QStringList SlantList{"Slanting", "Straight"};
protected:
    void buildProperties()
    {
        m_PropColl.appendList("BeamSlanting", "Returns or sets whether Slanted Beams are on.", SlantList, 0,false, "", "Behavior");
    }
};

class CDurSlur :public CGapSymbol
{
public:
    inline CDurSlur() :CGapSymbol("Slur", "Curve", "Returns/sets the difference between the current Curve and the default Curve.")
    {
        buildProperties();
    }
    inline CDurSlur(const XMLSimpleSymbolWrapper& XMLSymbol) : CGapSymbol(XMLSymbol, "Curve", "Returns/sets the difference between the current Curve and the default Curve.")
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    //void AfterNote(const XMLSymbolWrapper& XMLNote, OCPlayBackVarsType& voiceVars);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    OCGraphicsList plotRemaining(const OCDurSignType& s, OCNoteList& NoteList, OCDraw& ScreenObj);
    static OCNoteList::SlurTypes SlurDirection(const int d, const OCRhythmObjectList& l)
    {
        if (d==2) return OCNoteList::SlurDown;
        if (d==1) return OCNoteList::SlurUp;
        return (l.stemUp()) ? OCNoteList::SlurUp : OCNoteList::SlurDown;
    }
protected:
    void buildProperties()
    {
        //static QStringList SlurList{"Down", "Up"};
        m_PropColl.appendNumber("Weight",-10,10,"Returns/sets the horizontal Weight point",false,0,"Appearance");
        m_PropColl.appendList("Direction","Returns/sets the vertical Direction of the Slur",QStringList{"Auto", "Down", "Up"},0,false,"","Appearance");
        m_PropColl.appendNumber("Angle", -32000, 32000, "Returns/sets the difference between the Endpoints current vertical Position and it's default vertical Position.",false, "", "Appearance",true);
        m_ButtonProperty="Direction";
    }
};

class CStopped :public CVisibleSymbol
{
public:
    inline CStopped() : CVisibleSymbol("Stopped",false,true) {}
    inline CStopped(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol,false,true) {}
    QList<OCToolButtonProps*> CreateButtons();
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
};

class CHarmonic :public CVisibleSymbol
{
public:
    inline CHarmonic() : CVisibleSymbol("Harmonic",false,true) {}
    inline CHarmonic(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol,false,true) {}
    QList<OCToolButtonProps*> CreateButtons();
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
};

class CComma :public CVisibleSymbol
{
public:
    inline CComma() : CVisibleSymbol("Comma",false,true) {}
    inline CComma(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol,false,true) {}
    QList<OCToolButtonProps*> CreateButtons();
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
};

class CTempoChange : public CVisibleSymbol
{
public:
    inline CTempoChange() : CVisibleSymbol("TempoChange")
    {
        buildProperties();
    }
    inline CTempoChange(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void DuringNote(OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& voiceVars);
    OCGraphicsList PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& voiceVars, const XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void ModifyProperties(OCProperties& p);
    OCProperties* GetDefaultProperties(int Button);
    static const QStringList TempoChangeList;
protected:
    void buildProperties()
    {
        m_PropColl.appendList("TempoType", "Returns/sets the Type of Tempo change.", TempoChangeList, 0,false, "", "Appearance",true);
        m_PropColl.appendNumber("Speed", 1, 100, "Returns/sets the MIDI execution speed of the Tempo change.",false, 50, "Behavior");
        m_ButtonProperty="TempoType";
    }
};

class CText : public CTextSymbol
{
public:
    inline CText() : CTextSymbol("Text")
    {
        buildProperties();
    }
    inline CText(const XMLSimpleSymbolWrapper& XMLSymbol) : CTextSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    OCGraphicsList PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& voiceVars, const XMLScoreWrapper& Score, OCDraw& ScreenObj);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
private:
    OCGraphicsList PlotText(const XMLSymbolWrapper& Symbol, double XFysic, OCDraw& ScreenObj);
protected:
    void buildProperties()
    {
        m_PropColl.appendBool("Master", "Sets/Returns whether the Text should behave as a MasterStaff item", false, "", "Behavior");
    }
};

class CTremolo : public CVisibleSymbol
{
public:
    inline CTremolo() : CVisibleSymbol("Tremolo")
    {
        buildProperties();
    }
    inline CTremolo(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    void DuringNote(OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& voiceVars);
    void Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
    OCProperties* GetDefaultProperties(int Button);
protected:
    void buildProperties()
    {
        m_PropColl.appendNumber("Speed", 1, 200, "Returns/sets the MIDI execution speed of the Tremolo.",false, 50, "Behavior");
        m_PropColl.appendNumber("Beams", 1, 4, "Returns/sets the Tremolo Signs number of Beams.",false, 3, "Appearance",true);
    }
};

class CTrill : public CVisibleSymbol
{
public:
    inline CTrill() : CVisibleSymbol("Trill")
    {
         buildProperties();
    }
    inline CTrill(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    OCGraphicsList PrintSign(StemDirection UpDown, int& SignsUp, OCDraw& ScreenObj);
    void DuringNote(OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& voiceVars);
    void BeforeNote(const XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCPlayBackVarsType& voiceVars);
    void Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location);
    OCProperties* GetDefaultProperties(int Button);
    //const QStringList TrillList{"tr", "tr b", "tr #"};
protected:
    //int height;
    int trilldynam;
    int BasePitch;
    int CurrentPitch;
    int oldpitch;
    int TrillDir();
    bool FinishedPlaying;
    void buildProperties()
    {
        m_PropColl.appendNumber("Speed", 1, 200, "Returns/sets the MIDI execution speed of the Trill.",false, 50, "Behavior");
        m_PropColl.appendNumber("Range", 1, 12, "Returns/sets the Range of the Trill in half tones.",false, 2, "Behavior");
        m_PropColl.appendBool("StartFromAbove", "Returns or sets whether the Trill starts from above.",false, true, "Behavior");
        m_PropColl.appendList("TrillType","Returns/sets the Trill Sign displayed",QStringList{"tr", "tr b", "tr #"},0,false,"","appearance",true);
        m_ButtonProperty="TrillType";
        trilldynam = 0;
        FinishedPlaying = false;
    }
};

class CStemDirection : public CInvisibleSymbol
{
public:
    inline CStemDirection() : CInvisibleSymbol("StemDirection")
    {
        buildProperties();
    }
    inline CStemDirection(const XMLSimpleSymbolWrapper& XMLSymbol) : CInvisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    void fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars);
    const QStringList DirectionList{"Auto", "Down", "Up"};
protected:
    void buildProperties()
    {
        m_PropColl.appendList("Direction", "Returns/sets the Stem direction.", DirectionList, 0,false, "", "Behavior");
    }
};

class CPedal :public CVisibleSymbol
{
public:
    inline CPedal() : CVisibleSymbol("Pedal")
    {
        buildProperties();
    }
    inline CPedal(const XMLSimpleSymbolWrapper& XMLSymbol) : CVisibleSymbol(XMLSymbol)
    {
        buildProperties();
    }
    QList<OCToolButtonProps*> CreateButtons();
    OCGraphicsList plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper& XMLTemplateStaff, OCDraw& ScreenObj);
    void fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars);
    void Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &voiceVars);
    const QStringList PedalList{"Pedal Down", "Pedal Up"};
protected:
    void buildProperties()
    {
        appendCommonProperty();
        m_PropColl.appendList("PedalSign", "Returns/sets the Type of Pedal Sign.", PedalList, 0,false, "", "Appearance",true);
        m_ButtonProperty="PedalSign";
    }
};
#endif // OCSYMBOLSCOLLECTION_H
