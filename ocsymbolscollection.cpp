#include "ocsymbolscollection.h"
#include "ceditdialog.h"

OCProperties emptyproperties;

int OCSymbolsCollection::refCount=0;
QMap<QString,CSymbol*> OCSymbolsCollection::Symbols=QMap<QString,CSymbol*>();
QMap<QString, QList<OCToolButtonProps*> > OCSymbolsCollection::Buttons=QMap<QString, QList<OCToolButtonProps*> >();
QMap<QString, QList<QIcon> > OCSymbolsCollection::Icons=QMap<QString, QList<QIcon> >();
QMap<QString, QStringList> OCSymbolsCollection::CategoryMap=QMap<QString, QStringList>();
QStringList OCSymbolsCollection::Cats=QStringList();
QMap<QString, QStringList> OCSymbolsCollection::DuratedCategoryMap=QMap<QString, QStringList>();
QStringList OCSymbolsCollection::DuratedCats=QStringList();

OCSymbolsCollection::OCSymbolsCollection()
{
    if (refCount==0)
    {
        insertSymbol("DynamicChange", new CDynChange, "Dynamics");
        insertSymbol("Dynamic", new CDynamic);
        insertSymbol("Accent", new CAccent);
        insertSymbol("fp", new Cfp);
        insertSymbol("fz", new Cfz);

        insertSymbol("Tempo", new CTempo, "Tempo");
        insertSymbol("TempoChange", new CTempoChange);
        insertSymbol("Fermata", new CFermata);

        insertSymbol("Transpose", new CTranspose, "Pitch");
        insertSymbol("Octave", new COctave);

        insertSymbol("Repeat", new CRepeat, "Repeat");
        insertSymbol("Segno", new CSegno);
        insertSymbol("Coda", new CCoda);
        insertSymbol("DaCapo", new CDaCapo);
        insertSymbol("Fine", new CFine);

        insertSymbol("BeamLimit", new CLimit, "Beams");
        insertSymbol("StemDirection", new CStemDirection);
        insertSymbol("BeamSlant", new CSlant);
        insertSymbol("FlipTie", new CFlipTie);

        insertSymbol("Length", new CLength, "Length");

        insertSymbol("Clef", new CClef, "Staff");
        insertSymbol("Time", new CTime);
        insertSymbol("Key", new CKey);
        insertSymbol("Scale", new CScale);
        insertSymbol("Cue", new CCue);
        insertSymbol("BarWidth", new CBarWidth);
        insertSymbol("Text", new CText);

        insertSymbol("Patch", new CPatch, "MIDI");
        insertSymbol("Channel", new CChannel);
        insertSymbol("SysEx", new CSysEx);
        insertSymbol("Controller", new CController);
        insertSymbol("Expression", new CExpression);
        insertSymbol("Portamento", new CPortamento);

        insertSymbol("Fingering", new CFingering, "Other");
        insertSymbol("Stopped", new CStopped);
        insertSymbol("BartokPizz", new CBartokP);
        insertSymbol("Bowing", new CBowing);
        insertSymbol("Accidental", new CAccidental);
        insertSymbol("Trill", new CTrill);
        insertSymbol("Glissando", new CGliss);
        insertSymbol("Tremolo", new CTremolo);
        insertSymbol("Turn", new CDobbel);
        insertSymbol("Mordent", new CMordent);

        insertDuratedSymbol("Hairpin", new CHairpin, "Durated");
        insertDuratedSymbol("DuratedLength", new CDurLength);
        insertDuratedSymbol("Slur", new CDurSlur);
        insertDuratedSymbol("DuratedSlant", new CDurSlant);
        insertDuratedSymbol("DuratedBeamDirection", new CDurUpDown);
        insertDuratedSymbol("Tuplet", new CTuplet);
        insertDuratedSymbol("Beam", new CBeam);

        Symbols.insert("EndOfVoice", new CSymbol("EndOfVoice"));
        CNote* n = new CNote("Note");
        Symbols.insert("Note",n);
        n = new CNote("Rest");
        Symbols.insert("Rest",n);

        foreach(CSymbol* s,Symbols) Buttons.insert(s->Name(),s->CreateButtons());
        foreach(CSymbol* s,Symbols)
        {
            QList<QIcon> l;
            for (int i=0;i<Buttons[s->Name()].count();i++)
            {
                OCToolButtonProps* tbp=Buttons[s->Name()][i];
                if (!tbp->iconpath.isEmpty())
                {
                    l.append(QIcon(tbp->iconpath));
                }
                else
                {
                    QPixmap pm(24,24);
                    pm.fill(Qt::transparent);
                    QPainter painter(&pm);
                    painter.setRenderHint(QPainter::SmoothPixmapTransform);
                    painter.setWorldMatrixEnabled(false);
                    painter.setViewTransformEnabled(false);
                    //painter.setBackground(Qt::transparent);
                    painter.setPen(Qt::black);
                    painter.setBrush(Qt::black);
                    painter.setFont(QFont(tbp->fontname,tbp->fontsize,tbp->fontbold,tbp->fontitalic));
                    painter.drawText(QRect(0,0,24,24),tbp->buttonText,QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
                    l.append(QIcon(pm));
                }
            }
            Icons.insert(s->Name(),l);
        }
    }
    refCount++;
}

void OCSymbolsCollection::insertSymbol(const QString &name, CSymbol *symbol, const QString &category)
{
    static QString prevCat;
    Symbols.insert(name, symbol);
    if (!category.isEmpty())
    {
        prevCat=category;
        if (!Cats.contains(category))
        {
            Cats.append(category);
            CategoryMap.insert(category,QStringList());
        }
    }
    CategoryMap[prevCat].append(name);
}

void OCSymbolsCollection::insertDuratedSymbol(const QString &name, CSymbol *symbol, const QString &category)
{
    static QString prevCat;
    Symbols.insert(name, symbol);
    if (!category.isEmpty())
    {
        prevCat=category;
        if (!DuratedCats.contains(category))
        {
            DuratedCats.append(category);
            DuratedCategoryMap.insert(category,QStringList());
        }
    }
    DuratedCategoryMap[prevCat].append(name);
}

OCSymbolsCollection::~OCSymbolsCollection()
{
    refCount--;
    if (refCount==0)
    {
        foreach(QList<OCToolButtonProps*> l,Buttons)
        {
            qDeleteAll(l);
            l.clear();
        }
        qDeleteAll(Symbols);
        Symbols.clear();
    }
}

const QStringList OCSymbolsCollection::Classes()
{
    return Symbols.keys();
}

const QStringList OCSymbolsCollection::Categories()
{
    return Cats;
}

const QStringList OCSymbolsCollection::Category(const QString &CategoryName)
{
    return CategoryMap[CategoryName];
}

const QStringList OCSymbolsCollection::DuratedCategories()
{
    return DuratedCats;
}

const QStringList OCSymbolsCollection::DuratedCategory(const QString &CategoryName)
{
    return DuratedCategoryMap[CategoryName];
}

const QStringList OCSymbolsCollection::CommonSymbols()
{
    return QStringList() << "Clef" << "Key" << "Scale" << "Time" << "Channel" << "Patch" << "Dynamic" << "DynamicChange" << "Octave" << "Transpose" << "Fermata";
}

const bool OCSymbolsCollection::IsCommon(const XMLSymbolWrapper &Symbol)
{
    //return Symbol.Compare(CommonSymbols());
    return Symbol.getVal("Common");
}

void OCSymbolsCollection::ParseFileVersion(XMLScoreWrapper& XMLScore)
{
    QDomLiteElementList Symbols=XMLScore.Score()->elementsByTag("Symbol",true);
    foreach(QDomLiteElement* XMLSymbol,Symbols)
    {
        XMLSimpleSymbolWrapper Symbol(XMLSymbol);
        if (Symbol.Compare("SlurUp"))
        {
            Symbol.setAttribute("SymbolName","Slur");
            Symbol.setAttribute("Direction",1);
        }
        else if (Symbol.Compare("SlurDown"))
        {
            Symbol.setAttribute("SymbolName","Slur");
            Symbol.setAttribute("Direction",0);
        }
        else if (Symbol.Compare("HairpinCrescendo"))
        {
            Symbol.setAttribute("SymbolName","Hairpin");
            Symbol.setAttribute("HairpinType",0);
        }
        else if (Symbol.Compare("HairpinDiminuendo"))
        {
            Symbol.setAttribute("SymbolName","Hairpin");
            Symbol.setAttribute("HairpinType",1);
        }
        else if (Symbol.Compare("Fish"))
        {
            Symbol.setAttribute("SymbolName","Hairpin");
            Symbol.setAttribute("HairpinType",Symbol.getVal("FishType")+2);
        }
        else if (Symbol.Compare("DuratedLegato"))
        {
            Symbol.setAttribute("SymbolName","DuratedLength");
            Symbol.setAttribute("PerformanceType",1);
        }
        else if (Symbol.Compare("DuratedStaccato"))
        {
            Symbol.setAttribute("SymbolName","DuratedLength");
            Symbol.setAttribute("PerformanceType",2);
        }
        else if (Symbol.Compare("TurnUD"))
        {
            Symbol.setAttribute("SymbolName","Turn");
            Symbol.setAttribute("Direction",0);
        }
        else if (Symbol.Compare("TurnDU"))
        {
            Symbol.setAttribute("SymbolName","Turn");
            Symbol.setAttribute("Direction",1);
        }
        else if (Symbol.Compare("MordentDown","MordentUp"))
        {
            Symbol.setAttribute("SymbolName","Mordent");
        }
        else if (Symbol.Compare("TrillFlat"))
        {
            Symbol.setAttribute("SymbolName","Trill");
            Symbol.setAttribute("TrillType",1);
        }
        else if (Symbol.Compare("TrillSharp"))
        {
            Symbol.setAttribute("SymbolName","Trill");
            Symbol.setAttribute("TrillType",2);
        }
        else if (Symbol.Compare("TiedNote"))
        {
            Symbol.setAttribute("SymbolName","Note");
            Symbol.setAttribute("NoteType",1);
        }
        else if (Symbol.Compare("CompoundNote"))
        {
            Symbol.setAttribute("SymbolName","Note");
            Symbol.setAttribute("NoteType",2);
        }
        else if (Symbol.Compare("TiedCompoundNote"))
        {
            Symbol.setAttribute("SymbolName","Note");
            Symbol.setAttribute("NoteType",3);
        }
    }
}

const bool OCSymbolsCollection::editevent(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, QWidget* parent)
{
    bool Escape=false;
    /*
    If Extend.GetObj(SymbolName) Then
        Extend.thisobj.Edit XMLScore, RefreshMode, esc, Custom
    Else
    */
        CSymbol* s = Symbols[Symbol.name()];
        if (s != 0) s->Edit(Symbol, RefreshMode, Escape, parent);
        return !Escape;
}

void OCSymbolsCollection::PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType &dCurrent, int TrackNum, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj)
{
    if ((Symbol.IsRestOrAnyNote()) || Symbol.Compare("Tuplet", "EndOfVoice")) return; // 'tsnote To tstiedpolynote, tstuplet, tsend
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.PlotMTrack XFysic, XMLSymbol, stavedistance, dCurrent, ScreenObj, MTObj, Pointer
        Extend.thisobj.UpdatePrintVars XMLSymbol, TrackNum, dCurrent
    Else
    */
        CSymbol* s = Symbols[Symbol.name()];
        if (s != 0)
        {
            if (!Symbol.getVal("Invisible")) s->PlotMTrack(XFysic, Symbol, stavedistance, dCurrent, MTObj, Pointer, Score, ScreenObj);
            s->fib(Symbol, TrackNum, dCurrent);
        }
}

void OCSymbolsCollection::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &fibset)
{
    if (Symbol.IsRestOrAnyNote()) return; // 'tsnote To tstiedpolynote
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.UpdatePrintVars XMLSymbol, TrackNum, fibset
    Else
    */
        CSymbol* s = Symbols[Symbol.name()];
        if (s != 0) s->fib(Symbol, TrackNum, fibset);
}

void OCSymbolsCollection::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &pcurrent)
{
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.Play XMLSymbol, MFIle, CountIt, Py, XMLVoice, SignsToPlay, pcurrent
    Else
    */
        CSymbol* s = Symbols[Symbol.name()];
        if (s != 0) s->Play(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, pcurrent);
}

void OCSymbolsCollection::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType &sCurrent, int Pointer, OCDraw& ScreenObj)
{
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.Draw XMLSymbol, XFysic, BarList, CountIt, CountIt.BarCounter, dCurrent.CurrentClef, SignsToPrint, SignCol, XMLScore, FactorX, BarsToPrint, PointerStart, Objects, Bracket, Tuborg, Stave, Track, NoteList, iiTemp, dCurrent, ScreenObj, Pointer
        Extend.thisobj.UpdatePrintVars XMLSymbol, 0, dCurrent
    Else
    */
        CSymbol* s = Symbols[Symbol.name()];
        if (s != 0)
        {
            if (!Symbol.getVal("Invisible")) s->plot(Symbol, XFysic, BarList, CountIt, CountIt.BarCounter, SignsToPrint, SignCol, Score, PointerStart, SymbolList, Stave, Track, NoteList, NoteCount, dCurrent, sCurrent, Pointer, ScreenObj);
            s->fib(Symbol, 0, dCurrent);
        }
}

void OCSymbolsCollection::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &pcurrent)
{
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.UpdatePlayBackVars XMLSymbol, MFIle, CountIt, Py, XMLVoice, SignsToPlay, pcurrent
    Else
    */
        CSymbol* s = Symbols[Symbol.name()];
        if (s != 0) s->fibPlay(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, pcurrent);
}

void OCSymbolsCollection::DrawFactor(XMLSymbolWrapper& Symbol, OCCounter *Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score)
{
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.DrawFactor XMLSymbol, Counter, XMLTemplate, BarList, AnythingElse, MinimumSet, ChangeKey, ChangeClef, Staff, Voice, LongestStaff, LongestVoice, Bar, Py, XMLScore
    Else
    */
        CSymbol* s = Symbols[Symbol.name()];
        if (s!=0) s->DrawFactor(Symbol, Counter, XMLTemplate, BarList, Staff, Voice, Bar, Py, Score);
}

const QString OCSymbolsCollection::Description(const XMLSimpleSymbolWrapper &Symbol)
{
    if (Symbol.IsValuedNote())
    {
        return "Note (" + QString::number((int)Symbol.getVal("Pitch")) + ")";
    }
    else if (Symbol.IsCompoundNote())
    {
        return "Polyphonic Note (" + QString::number((int)Symbol.getVal("Pitch")) + ")";
    }
    else if (Symbol.IsRest())
    {
        return "Rest";
    }
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Name = Extend.thisobj.Name
    Else
    */
        CSymbol* s = Symbols[Symbol.name()];
        if (s != 0) return s->Name();
        return Symbol.name();
}

OCProperties* OCSymbolsCollection::GetProperties(XMLSimpleSymbolWrapper& Symbol)
{
    OCProperties* Properties;
    /*
    If Extend.GetObj(SN) Then
        Set Properties = Extend.thisobj.Properties(XMLSymbol)
        Properties.Name = Extend.thisobj.Name
    Else
    */
        CSymbol* s = Symbols[Symbol.name()];
        if (s != 0)
        {
            Properties = s->GetProperties();
            Properties->Reset();
            Properties->FromXML(Symbol.getXML());
            s->ModifyProperties(Properties);
            //Properties->Name = s->Name();
            return Properties;
        }
        //Properties=new OCProperties();
        Properties=&emptyproperties;
        Properties->Name=Symbol.name();
        return Properties;
}

void OCSymbolsCollection::ModifyProperties(OCProperties *Properties)
{
    CSymbol* s = Symbols[Properties->Name];
    if (s != 0)
    {
        s->ModifyProperties(Properties);
    }
}

const bool OCSymbolsCollection::PropetyExists(const QString& SymbolName, const QString& PropertyName)
{
    CSymbol* s = Symbols[SymbolName];
    if (s != 0)
    {
        return s->PropertyExists(PropertyName);
    }
    return false;
}

OCProperties* OCSymbolsCollection::GetProperties(const QString& SymbolName)
{
    OCProperties* Properties;
    /*
    If Extend.GetObj(SN) Then
        Set Properties = Extend.thisobj.Properties(XMLSymbol)
        Properties.Name = Extend.thisobj.Name
    Else
    */
        CSymbol* s = Symbols[SymbolName];
        if (s != 0)
        {
            Properties=s->GetProperties();
            //Properties->Name = s->Name();
            Properties->Reset();
            return Properties;
        }
        //Properties=new OCProperties();
        Properties=&emptyproperties;
        Properties->Name=SymbolName;
        return Properties;
}

OCProperties* OCSymbolsCollection::GetProperties(const QString& SymbolName, const int Button)
{
    CSymbol* s = Symbols[SymbolName];
    if (s != 0)
    {
        return s->GetProperties(Button);
    }
    //OCProperties* Properties=new OCProperties();
    OCProperties* Properties=&emptyproperties;
    Properties->Name=SymbolName;
    return Properties;
}

XMLSimpleSymbolWrapper OCSymbolsCollection::GetSymbol(const QString& SymbolName)
{
    return XMLSimpleSymbolWrapper(GetProperties(SymbolName)->ToXML());
}

XMLSimpleSymbolWrapper OCSymbolsCollection::GetSymbol(const QString& SymbolName, const int Button)
{
    return XMLSimpleSymbolWrapper(GetProperties(SymbolName,Button)->ToXML());
}

OCToolButtonProps* OCSymbolsCollection::ButtonProps(const QString& SymbolName, const int Button)
{
    return Buttons[SymbolName][Button];
}

const int OCSymbolsCollection::ButtonCount(const QString& SymbolName)
{
    return Buttons[SymbolName].count();
}

const QIcon OCSymbolsCollection::Icon(const QString& SymbolName, const int Button)
{
    QList<QIcon> &l=Icons[SymbolName];
    if (l.count()) return l[Button];
    return QIcon();
}

CNote::CNote(QString Name):CVisibleSymbol(Name)
{
    m_PropColl->Name=m_Name;
    QStringList ListArr;
    ListArr << "Whole" << "Half" << "Quarter" << "8th" << "16th" << "32th" << "64th";
    if (m_Name=="Rest") ListArr  << "1 Bar";
    m_PropColl->Add("NoteValue", pwList,"" ,"" , "The Time value of a Note or a Pause.", ListArr,0,false ,"" , "Appearance");
    m_PropColl->Add("Dotted", pwBoolean,"" ,"" , "Adds 50% to the Time value of a Note or a Pause.","" ,0,false ,"" , "Appearance");
    m_PropColl->Add("Triplet", pwBoolean,"" ,"" , "Subtracts 1/3 from the Time value of a Note or a Pause.","" ,0,false ,"" , "Appearance");
    m_PropColl->Add("NoteType", pwList,"" ,"" , "Returns/sets the Type of a Note. The Type can be Single Note or Compound Note, and notes can be Tied.", QStringList() << "Note" << "Tied Note" << "Chord Note" << "Tied Chord Note",0,false ,"" , "Appearance");
    m_PropColl->Add("Pitch", pwNumber, 1, 127, "Returns/sets the Pitch of a Note in MIDI key numbers.","" ,0,false ,"" , "Appearance");
    m_PropColl->Add("AccidentalLeft", pwNumber, -32000, 32000, "Returns/sets the distance between an Accidentals default horizontal position and it's current horizontal position.","" ,0,false ,"" , "Position");
}

QList<OCToolButtonProps*> CNote::CreateButtons()
{
    if (m_Name=="Rest")
    {
        CreateButton(":/Notes/Notes/quarterrest.png");
    }
    else
    {
        CreateButton(":/Notes/Notes/3.png");
    }
    m_ButtonList[0]->ishidden=true;
    return m_ButtonList;
}

void CNote::ModifyProperties(OCProperties* p)
{
    if (m_Name == "Rest")
    {
        p->GetItem("Top")->Hidden = false;
        p->GetItem("NoteType")->Hidden = true;
        p->GetItem("Pitch")->Hidden = true;
        p->GetItem("AccidentalLeft")->Hidden = true;
        if (p->GetValue("NoteValue")==7)
        {
            p->GetItem("Dotted")->Hidden=true;
            p->GetItem("Triplet")->Hidden=true;
        }
        else
        {
            p->GetItem("Dotted")->Hidden=false;
            p->GetItem("Triplet")->Hidden=false;
        }
    }
    else
    {
        p->GetItem("NoteType")->Hidden = false;
        p->GetItem("Pitch")->Hidden = false;
        p->GetItem("AccidentalLeft")->Hidden = false;
        p->GetItem("Top")->Hidden = true;
    }
}

OCProperties* CNote::GetProperties()
{
    m_PropColl->Reset();
    //ModifyProperties(m_PropColl);
    return m_PropColl;
}

QStringList CDynamic::DynamicList=QStringList() << "ppp" << "pp" << "p" << "mp" << "mf" << "f" << "ff" << "fff";

CDynamic::CDynamic():CVisibleSymbol("Dynamic")
{
    m_PropColl->Add("Common", pwBoolean, "", "", "Returns/sets symbol in Voice 1 common to all Voices", "", 0, false, true, "Behavior");
    m_PropColl->Add("DynamicSign", pwList, "", "", "Returns/sets the Type of Dynamic Sign.", DynamicList, 0,false, "", "Appearance");
    m_PropColl->Add("Velocity", pwNumber, 1, 127, "Returns/sets the MIDI execution Velocity of the Notes that follows.", "", 0,false, 1, "Behavior");
    m_ButtonProperty="DynamicSign";
}

QList<OCToolButtonProps*> CDynamic::CreateButtons()
{
    for (int i=0;i<8;i++) CreateButton(":/Notes/Notes/"+DynamicList[i]+".png",false,tsRedrawActiveStave,"Add Dynamic "+DynamicList[i]);
    return m_ButtonList;
}

void CDynamic::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& /*BarList*/, OCCounter& /*CountIt*/, int /*BarCounter*/, OCSignList& /*SignsToPrint*/, QColor /*SignCol*/, XMLScoreWrapper& /*Score*/, int /*PointerStart*/, OCSymbolArray& SymbolList, int /*Stave*/, int /*Track*/, OCNoteList& /*NoteList*/, int /*NoteCount*/, OCPrintVarsType& /*dCurrent*/, OCPrintStaffVarsType& /*sCurrent*/, int Pointer, OCDraw& ScreenObj)
{
    QString a("");
    int c=Symbol.getVal("DynamicSign");
    if (c==3)
    {
        a = QChar(OCTTFmp);
    }
    else if (c==4)
    {
        a = QChar(OCTTFmf);
    }
    else if (c<3)
    {
        for (int iTemp1 = 1; iTemp1 <= Abs((int)Symbol.getVal("DynamicSign") - 3); iTemp1++)
        {
            a += QChar(OCTTFp);
        }
    }
    else if (c>4)
    {
        for (int iTemp1 = 1; iTemp1 <= Symbol.getVal("DynamicSign") - 4; iTemp1++)
        {
            a += QChar(OCTTFf);
        }
    }
    ScreenObj.DM(XFysic, ScoreBottomSymbolY, Symbol);
    QList<QGraphicsItem*> l=ScreenObj.plLet(MakeUnicode(a),Symbol.size(),OCTTFname,false,false,1200,Qt::AlignRight | Qt::AlignBottom);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

void CDynamic::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay)
{
    SignsToPlay.KillByName("Hairpin");
    SignsToPlay.KillByName("DynamicChange");
    TemPlay.Currentdynam = Symbol.getVal("Velocity");
    TemPlay.crescendo = 0;
}

void CDynamic::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay)
{
    Play(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
}

OCProperties* CDynamic::GetProperties(int Button)
{
    return OCPresets().SetPropertyValue(CSymbol::GetProperties(Button),"Velocity",DynamicList[Button]+"vel");
}

QStringList CPatch::PatchList=QStringList();

CPatch::CPatch() : CSymbol("Patch")
{
    if (PatchList.isEmpty())
    {
        //PatchList.append("-");
        QFile fileData(":/OCPatches.txt");
        if (fileData.open(QIODevice::ReadOnly))
        {
            QTextStream readData(&fileData);
            while (!readData.atEnd()) PatchList << readData.readLine();
            fileData.close();
        }
        else
        {
            for (int i=0;i<128;i++) PatchList.append(QString::number(i+1));
        }
    }
    m_PropColl->Add("Common", pwBoolean, "", "", "Returns/sets symbol in Voice 1 common to all Voices", "", 0, false, true, "Behavior");
    m_PropColl->Add("Patch",pwList,"","","Sets/returns current MIDI Patch",PatchList,1,false,1,"Behavoir");
}

QList<OCToolButtonProps*> CPatch::CreateButtons()
{
    CreateButton(":/Notes/Notes/patch.png",true);
    return m_ButtonList;
}

void CPatch::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutCombo("Patch",Symbol.getVal("Patch")-1,PatchList);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("Patch",d.EditWidget->GetCombo()+1);
    RefreshMode=tsRedrawActiveStave;
}

void CPatch::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    TemPlay.MIDI.Patch=Symbol.getVal("Patch") - 1;
}

void CPatch::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.MIDI.Patch=Symbol.getVal("Patch") - 1;
}

void CPatch::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    MFile.PlayPatch(Symbol.getVal("Patch")-1,TemPlay.MIDI.Channel);
    MFile.SetTime(0);
    TemPlay.Currenttime=0;
}

QStringList CTime::TimeList=QStringList() << "Time Signature" << "Common Time" << "Cut Time";

CTime::CTime():CVisibleSymbol("Time",false,true)
{
    m_PropColl->Add("Common", pwBoolean, "", "", "Returns/sets symbol in Voice 1 common to all Voices", "", 0, false, true, "Behavior");
    m_PropColl->Add("Time", pwCustom, "", "", "Shows the Time Dialog.", "", 0,false, "", "Appearance");
    m_PropColl->Add("TimeType", pwList, "", "", "Returns/sets the type of the time signature", TimeList, 0,false, 0, "Behavior");
    m_PropColl->Add("Upper", pwNumber, 1, 200, "Returns/sets the upper number in the time signature", "", 0,false, 4, "Behavior");
    m_PropColl->Add("Lower", pwNumber, 1, 200, "Returns/sets the lower number in the time signature", "", 0,false, 4, "Behavior");
}

QList<OCToolButtonProps*> CTime::CreateButtons()
{
    CreateButton(":/Notes/Notes/cuttime.png",true,tsReformat,"Add Time Signature");
    return m_ButtonList;
}

int CTime::GetTicks(XMLSymbolWrapper& Symbol)
{
    switch ((int)Symbol.getVal("TimeType"))
    {
        case 0:
            return 96 * Symbol.getVal("Upper") / Symbol.getVal("Lower");
        case 1:
        case 2:
            return 96;
    }
    return 96;
}

void CTime::DrawFactor(XMLSymbolWrapper& Symbol, OCCounter *Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score)
{
    if (!Symbol.getVal("Invisible"))
    {
        if (Counter->Counter == 0)
        {
            if (Symbol.getVal("TimeType") > 0)
            {
                BarList.SetTimeInBegOfBar(Bar,2);
            }
            else
            {
                float l=QString::number(Symbol.getVal("Upper")).length();
                float ll=QString::number(Symbol.getVal("Lower")).length();
                l = qMax(ll,l);
                if (l>1) l*=0.75;
                BarList.SetTimeInBegOfBar(Bar,l*2);
            }
        }
    }
}

void CTime::ModifyProperties(OCProperties* p)
{
    p->GetItem("Upper")->Hidden = (p->GetValue("TimeType").toInt() > 0);
    p->GetItem("Lower")->Hidden = (p->GetValue("TimeType").toInt() > 0);
}

void CTime::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l=PlTime(Symbol, XFysic - ((BarList.BegSpace(BarCounter, false, false, true) + 192)*ScreenObj.XFactor),ScreenObj, Qt::AlignLeft);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

QList<QGraphicsItem*> CTime::PlTime(XMLSymbolWrapper& Symbol, int X, OCDraw& ScreenObj, Qt::Alignment Align)
{
    QPainterPath p;
    switch (Symbol.attribute("TimeType").toInt())
    {
        case 2:
            ScreenObj.DM(X, ScoreStaffHeight-48, Symbol);
            ScreenObj.DR(0,-86,Symbol.size());
            p=ScreenObj.TextPath(QChar(OCTTFAllaBreve), Symbol.size(), OCTTFname, false, false, 1200);
            p.translate(-p.boundingRect().width()/2,0);
            break;
        case 1:
            ScreenObj.DM(X, ScoreStaffHeight-48, Symbol);
            ScreenObj.DR(0,-86,Symbol.size());
            p=ScreenObj.TextPath(QChar(OCTTFFourFour), Symbol.size(), OCTTFname, false, false, 1200);
            p.translate(-p.boundingRect().width()/2,0);
            break;
        case 0:
            ScreenObj.DM(X, ScoreStaffHeight-40, Symbol);
            p=ScreenObj.TextPath(MakeUnicode(Symbol.attribute("Upper")), Symbol.size(), OCTTFname, false, false, 1200);
            p.translate(-p.boundingRect().width()/2,0);
            ScreenObj.DM(X, ScoreStaffHeight-40, Symbol);
            ScreenObj.DR(0,-192,Symbol.size());
            QPainterPath p1=ScreenObj.TextPath(MakeUnicode(Symbol.attribute("Lower")), Symbol.size(), OCTTFname, false, false, 1200);
            p1.translate(-p1.boundingRect().width()/2,0);
            p.addPath(p1);
    }
    if (Align & Qt::AlignLeft) p.translate(p.boundingRect().width()/2,0);
    if (Align & Qt::AlignRight) p.translate(-p.boundingRect().width()/2,0);
    return ScreenObj.plTextPath(p);
}


void CTime::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent)
{
    int Upper=Symbol.getVal("Upper");
    int Lower=Symbol.getVal("Lower");
    int TimeType=Symbol.getVal("TimeType");
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutTime(TimeType,Upper,Lower);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetTime(TimeType,Upper,Lower);
        Symbol.setAttribute("TimeType",TimeType);
        Symbol.setAttribute("Upper",Upper);
        Symbol.setAttribute("Lower",Lower);
    }
    RefreshMode = tsReformat;
}

void CTime::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
}

void CTime::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.Meter = GetTicks(Symbol);
    switch ((int)Symbol.getVal("TimeType"))
    {
    case 0:
        tempsetting.MeterText=QString::number(Symbol.getVal("Upper"))+"/"+QString::number(Symbol.getVal("Lower"));
        break;
    case 1:
        tempsetting.MeterText="C";
        break;
    case 2:
        tempsetting.MeterText="Alla breve";
        break;
    }
}

void CTime::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    TemPlay.PlayMeter = GetTicks(Symbol);
    switch ((int)Symbol.getVal("TimeType"))
    {
    case 0:
        MFile.PlayTime(Symbol.getVal("Upper"), Symbol.getVal("Lower"));
        break;
    case 1:
        MFile.PlayTime(4,4);
        break;
    case 2:
        MFile.PlayTime(2,2);
        break;
    }
    //CountIt.reset();
    MFile.SetTime(0);
    TemPlay.Currenttime = 0;
}

CTuplet::CTuplet():CDuratedSymbol("Tuplet")
{
    m_PropColl->Add("TupletValue", pwCustom, "", "", "Returns/Sets the Number of Ticks to fit the specified Notes into", "", 0,false, 24, "Appearance");
}

QList<OCToolButtonProps*> CTuplet::CreateButtons()
{
    CreateButton(":/Notes/Notes/tuplet.png",true);
    return m_ButtonList;
}

void CTuplet::DrawFactor(XMLSymbolWrapper& Symbol, OCCounter *Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score)
{
    Counter->Tuplets(Py, Score.Voice(Staff, Voice));
}

void CTuplet::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent)
{
    int Value=Symbol.getVal("TupletValue");
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    int Noteval;
    bool Dotted;
    bool Triplet;
    XMLSimpleSymbolWrapper::SetNoteVal(Noteval,Dotted,Triplet,Value);
    d.EditWidget->PutNoteval(Noteval,Dotted,Triplet);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetNoteval(Noteval,Dotted,Triplet);
        Symbol.setAttribute("TupletValue",XMLSimpleSymbolWrapper::CalcTicks(Noteval,Dotted,Triplet));
    }
    RefreshMode = tsReformat;
}

void CTuplet::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    CountIt.Tuplets((dCurrent.FilePointer), Score.Voice(Stave, Track));
    NoteList.PlotTuplet(NoteCount, CountIt.TupletMax, CountIt.TupletAntal, Symbol.pos(), Symbol.size(), SymbolList, Pointer, ScreenObj);
}

QStringList CClef::ClefList=QStringList() <<"Soprano" << "Bass" << "Alto" << "Tenor" << "Percussion";

CClef::CClef():CVisibleSymbol("Clef",false,true)
{
    m_PropColl->Add("Common", pwBoolean, "", "", "Returns/sets symbol in Voice 1 common to all Voices", "", 0, false, true, "Behavior");
    m_PropColl->Add("Clef", pwList, "", "", "Returns/sets the Type of Clef.", ClefList, 0,false, "", "Appearance");
    m_ButtonProperty="Clef";
}

QList<OCToolButtonProps*> CClef::CreateButtons()
{
    CreateButton(":/Notes/Notes/Gclef.png",false,tsReformat,"Add Soprano Clef");
    CreateButton(":/Notes/Notes/Fclef.png",false,tsReformat,"Add Bass Clef");
    CreateButton(":/Notes/Notes/Cclef.png",false,tsReformat,"Add Alto Clef");
    CreateButton(":/Notes/Notes/Tclef.png",false,tsReformat,"Add Tenor Clef");
    CreateButton(":/Notes/Notes/neutralclef.png",false,tsReformat,"Add Percussion Clef");
    return m_ButtonList;
}

void CClef::DrawFactor(XMLSymbolWrapper& Symbol, OCCounter *Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score)
{
    if (!Symbol.getVal("Invisible"))
    {
        if ((Counter->Counter == 0) & (Bar > 0))
        {
            if (BarList.GetClefInBegOfBar(Bar) == 0) BarList.SetClefInBegOfBar(Bar, 4);
        }
    }
    //if (Bar == 0) ChangeClef = true;
}

void CClef::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    int Left=300;
    if (CountIt.Counter == 0)
    {
        Left = BarList.BegSpace(BarCounter, true, true, true) + 60;
        if (BarCounter > 0) Left += 48;
    }
    ScreenObj.DM(XFysic -((Left + 96)*ScreenObj.XFactor), 888, Symbol);
    QList<QGraphicsItem*> l=PlClef(Symbol.getVal("Clef") + 1, Symbol.size(),ScreenObj);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

QList<QGraphicsItem*> CClef::PlClef(int Clef, int Size, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    switch (Clef)
    {
        case 1:
            ScreenObj.DR(0,96);
            ScreenObj.DR(0,-96,Size);
            l.append(ScreenObj.plLet(QChar(OCTTFSopranoClef), Size, OCTTFname, false, false, 624));
            break;
        case 2:
            ScreenObj.DR(0,288);
            ScreenObj.DR(0,-288,Size);
            l.append(ScreenObj.plLet(QChar(OCTTFBassClef), Size, OCTTFname, false, false, 624));
            break;
        case 3:
            ScreenObj.DR(0,195);
            ScreenObj.DR(0,-192,Size);
            l.append(ScreenObj.plLet(QChar(OCTTFAltoClef), Size, OCTTFname, false, false, 624));
            break;
        case 4:
            ScreenObj.DR(0,96);
            ScreenObj.DR(0,195);
            ScreenObj.DR(0,-192,Size);
            l.append(ScreenObj.plLet(QChar(OCTTFAltoClef), Size, OCTTFname, false, false, 624));
            break;
        case 5:
            ScreenObj.DR(0,192);
            ScreenObj.DR(0,-192,Size);
            l.append(ScreenObj.plLet(QChar(OCTTFPercussionClef), Size, OCTTFname, false, false, 624));
            break;
    }
    return l;
}

void CClef::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent)
{
    esc=false;
    RefreshMode=tsReformat;
}

void CClef::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.CurrentClef=OCSignType(Symbol);
    tempsetting.CurrentClef.val = Symbol.getVal("Clef") + 1;
    tempsetting.ClefChange=true;
}

CTranspose::CTranspose() :CSymbol("Transpose")
{
    m_PropColl->Add("Common", pwBoolean, "", "", "Returns/sets symbol in Voice 1 common to all Voices", "", 0, false, true, "Behavior");
    m_PropColl->Add("Transpose", pwNumber, -48, 48, "Returns/sets the amount of Transposition in half tones.", "", 0,false, "", "Behavior");
}

QList<OCToolButtonProps*> CTranspose::CreateButtons()
{
    CreateButton(":/Notes/Notes/transpose.png",true);
    return m_ButtonList;
}

void CTranspose::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent)
{
    int Value=Symbol.getVal("Transpose");
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutTranspose(Value);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetTranspose(Value);
        Symbol.setAttribute("Transpose",Value);
    }
    RefreshMode = tsRedrawActiveStave;
}

void CTranspose::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
}

void CTranspose::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.MIDI.Transpose = Symbol.getVal("Transpose");
}

void CTranspose::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    TemPlay.MIDI.Transpose = Symbol.getVal("Transpose");
}

QStringList CKey::KeyList=QStringList() << "Gb   eb" << "Db   bb" << "Ab   f" << "Eb   c" << "Bb   g" << "F    d" << "C    a" << "G    e" << "D    b" << "A    f#" << "E    c#" << "B    g#" << "F#   d#";

CKey::CKey() : CVisibleSymbol("Key",false,true,true)
{
    m_PropColl->Add("Common", pwBoolean, "", "", "Returns/sets symbol in Voice 1 common to all Voices", "", 0, false, true, "Behavior");
    m_PropColl->Add("Key", pwList, "", "", "Returns/sets the Key.", KeyList, 0,false, 6, "Appearance");
}

QList<OCToolButtonProps*> CKey::CreateButtons()
{
    CreateButton(":/Notes/Notes/keysigflat.png",true,tsReformat,"Add Key Signature");
    return m_ButtonList;
}

void CKey::DrawFactor(XMLSymbolWrapper& Symbol, OCCounter *Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score)
{
    if (!Symbol.getVal("Invisible"))
    {
        int NumOfSigns = NumOfAccidentals(Symbol.getVal("Key") - 6);
        if ((NumOfSigns > BarList.GetKeyInBegOfBar(Bar)) && (Counter->Counter == 0)) BarList.SetKeyInBegOfBar(Bar, NumOfSigns);
    }
    //if (Bar == 0) ChangeKey = true;
}

int CKey::NumOfAccidentals(int Key)
{
    return Abs(Key);
}

int CKey::AccidentalFlag(int Key)
{
    return -Sgn(Key);
}

void CKey::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    //if (! GetVal(XMLSymbol, "Invisible"))
    //{
        //QList<QGraphicsItem*> l=PlAcc(CurrentClef, XMLSymbol, GetPos((XFysic - BarList.BegSpace(BarCounter, true, false, true)) - 216,0,XMLSymbol), ScreenObj);
        QList<QGraphicsItem*> l=plotKey(Symbol.getVal("Key") - 6,Symbol.move(XFysic - ((BarList.BegSpace(BarCounter, true, false, true) + 216)*ScreenObj.XFactor),0),dCurrent.CurrentClef.val,ScreenObj);
        SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
    //}
}

void CKey::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent)
{
    int Value=Symbol.getVal("Key");
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    //d.EditWidget->PutKey(Value);
    d.EditWidget->PutCombo("Key Signature",Value,KeyList);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        Value=d.EditWidget->GetCombo();
        Symbol.setAttribute("Key",Value);
    }
    RefreshMode = tsReformat;
}

void CKey::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    //MFile.PlayKey(Symbol.getVal("Key") - 6);
    fibPlay(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
    MFile.SetTime(0);
    TemPlay.Currenttime=0;
}

void CKey::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    //tempsetting.KBFlagFaste = -Sgn((int)GetVal(XMLSymbol, "Key") - 6);
    tempsetting.CurrentKey=OCSignType(Symbol);
    tempsetting.CurrentKey.val = Symbol.getVal("Key") - 6;
    tempsetting.KeyChange=true;
}

QList<QGraphicsItem*> CKey::plotKey(int Key, QPointF Pos, int CurrentClef, OCDraw& ScreenObj)
{
    int KBFlagFaste=AccidentalFlag(Key);
    int AntalFasteFortegn=NumOfAccidentals(Key);
    QList<int> Acc;
    QList<QGraphicsItem*> l;
    if (AntalFasteFortegn == 0) return l;
    if (KBFlagFaste == -1)
    {
        switch (CurrentClef)
        {
        case 0:
        case 1:
            Acc << 99 << 87 << 103 << 93 << 79 << 95 << 83;
            break;
        case 2:
            Acc << 91 << 79 << 95 << 83 << 99 << 87 << 103;
            break;
        case 3:
            Acc <<  95 << 83 << 99 << 87 << 103 << 91 << 79;
            break;
        case 4:
            Acc << 103 << 91 << 79 << 95 << 83 << 99 << 87;
            break;
        }
    }
    else
    {
        switch (CurrentClef)
        {
        case 0:
        case 1:
            Acc << 83 << 95 << 79 << 91 << 75 << 87 << 71;
            break;
        case 2:
            Acc << 75 << 87 << 71 << 83 << 67 << 79 << 63;
            break;
        case 3:
            Acc << 79 << 91 << 75 << 87 << 71 << 83 << 67;
            break;
        case 4:
            Acc << 87 << 99 << 83 << 95 << 79 << 91 << 75;
            break;
        }
    }
    for (int iTemp = 0; iTemp < AntalFasteFortegn; iTemp++)
    {
        ScreenObj.DM(Pos.x() + (iTemp * AccidentalSpace) , Pos.y() + (Acc[iTemp] * 12) + 162);
        if (KBFlagFaste == 1)
        {
            l.append(ScreenObj.plLet(QChar(OCTTFFlat), 0, OCTTFname, false, false, 1200));
        }
        else
        {
            l.append(ScreenObj.plLet(QChar(OCTTFSharp), 0, OCTTFname, false, false, 1200));
        }
    }
    return l;
}

void CKey::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    MFile.PlayKey(Symbol.getVal("Key") - 6);
}

CScale::CScale() : CSymbol("Scale")
{
    m_PropColl->Add("Common", pwBoolean, "", "", "Returns/sets symbol in Voice 1 common to all Voices", "", 0, false, true, "Behavior");
    m_PropColl->Add("Accidentals", pwCustom, "", "", "Shows the Scale Dialog.", "", 0,false, "", "Behavior");
    for (int i=0;i<12;i++)
    {
        m_PropColl->Add("Step"+QString::number(i+1), pwNumber, 0, 2, "", "", 0, true);
    }
}

QList<OCToolButtonProps*> CScale::CreateButtons()
{
    CreateButton(":/Notes/Notes/scale.png",true);
    return m_ButtonList;
}

void CScale::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Keys[12];
    for (int i=0;i<12;i++)
    {
        Keys[i] = Symbol.getVal("Step"+QString::number(i+1));
    }
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutAccidentals(&Keys[0]);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetAccidentals(&Keys[0]);
        for (int i=0;i<12;i++)
        {
            Symbol.setAttribute("Step"+QString::number(i+1),Keys[i]);
        }
    }
    RefreshMode = tsRedrawActiveStave;
}

void CScale::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    for (int i=0;i<12;i++)
    {
        tempsetting.J[i] = Symbol.getVal("Step"+QString::number(i+1));
    }
}

QStringList CTempo::TempoList=QStringList() << "Whole" << "Half" << "Quarter" << "8th" << "16th";

CTempo::CTempo():CVisibleSymbol("Tempo")
{
    m_PropColl->Add("Tempo", pwNumber, 20, 300, "Returns/sets the Tempo.", "", 0,false, 120, "Appearance");
    m_PropColl->Add("NoteValue", pwList, "", "", "Returns/sets the Note Value the Tempo relates to.", TempoList, 0,false, 2, "Appearance");
    m_PropColl->Add("Dotted", pwBoolean, "", "", "Adds 50% to the Time of the NoteValue.", "", 0,false, "", "Appearance");
}

QList<OCToolButtonProps*> CTempo::CreateButtons()
{
    CreateButton(":/Notes/Notes/tempo.png",true);
    return m_ButtonList;
}

void CTempo::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value = Symbol.getVal("Tempo");
    int ListValue = Symbol.getVal("NoteValue");
    bool Dotted = Symbol.getVal("Dotted");
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutTempo(Value,ListValue,Dotted);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetTempo(Value,ListValue,Dotted);
        Symbol.setAttribute("Tempo",Value);
        Symbol.setAttribute("NoteValue",ListValue);
        Symbol.setAttribute("Dotted",Dotted);
    }
    RefreshMode = tsRedrawActiveStave;
}

void CTempo::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    SignsToPlay.KillByName("TempoChange");
    if (Symbol.getVal("NoteValue") == 0)
    {
        TemPlay.Playtempo = Symbol.getVal("Tempo");
    }
    else
    {
        TemPlay.Playtempo = (Symbol.getVal("Tempo") * XMLSimpleSymbolWrapper::CalcTicks(Symbol.getVal("NoteValue"), Symbol.getVal("Dotted"),false)) / 24;
    }
    TemPlay.HoldTempo = TemPlay.Playtempo;
    MFile.Playtempo(TemPlay.Playtempo);
    TemPlay.Accel = 0;
}

void CTempo::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    fibPlay(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
    MFile.SetTime(0);
    TemPlay.Currenttime = 0;
}

void CTempo::PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    ScreenObj.DM(XFysic, ScoreTempoY, Symbol);
    l.append(PlTempo(Symbol.getVal("Tempo"),Symbol.getVal("NoteValue"), Symbol.getVal("Dotted"),Score.TempoFont(),Symbol.size(),ScreenObj));
    if (MTColorCheck(ScreenObj)) MTObj.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

void CTempo::fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.MasterStuff=true;
}

QList<QGraphicsItem*> CTempo::PlTempo(int Tempo, int NoteVal, bool Dotted, QFont Font, int Size, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    QPainterPath p(QPointF(0,0));
    switch (NoteVal)
    {
    case 0:
        p.addText(0,0,QFont(OCTTFname,400),QChar(OCTTFNoteWhole));
        break;
    case 1:
        p.addRoundedRect(22,-6,6,-160,3,3);
        p.addText(0,0,QFont(OCTTFname,400),QChar(OCTTFNoteHalf));
        break;
    default:
        p.addRoundedRect(22,-6,6,-160,3,3);
        p.addText(0,0,QFont(OCTTFname,400),QChar(OCTTFNoteQuarter));
    }
    switch (NoteVal)
    {
    case 3:
        p.addPath(OCNoteList::FanPath(168,1,1).translated(28,-160));
        break;
    case 4:
        p.addPath(OCNoteList::FanPath(168,1,2).translated(28,-160));
        break;
    }
    if (Dotted) p.addText(30,18,QFont("Courier",100),".");
    p.addText(56,18,Font,"=" + QString::number(Tempo));
    p.translate(-p.boundingRect().width(),0);
    float f=(1.30/ScreenObj.ScreenSize)/SizeFactor(Size);
    QMatrix m;
    m.scale(f,f);
    p=p*m;
    p.setFillRule(Qt::WindingFill);
    ScreenObj.translatePath(p);
    l.append(ScreenObj.plTextPath(p));
    return l;
}

CSysEx::CSysEx() : CSymbol("SysEx")
{
    m_PropColl->Add("Custom", pwCustom, "", "", "Shows the SysEx Dialog", "", 0,false, "", "Behavior");
    m_PropColl->Add("SysExString", pwText, "", "", "Returns/sets the Hexvalues sent to the synth", "", 0,false, "", "Behavior");
}

QList<OCToolButtonProps*> CSysEx::CreateButtons()
{
    CreateButton(":/Notes/Notes/sysex.png",true);
    return m_ButtonList;
}

void CSysEx::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    QString syx;
    syx=Symbol.attribute("SysExString");
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.ShowList("SysEx");
    d.EditWidget->PutSysEx(syx);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetSysEx(syx);
        Symbol.setAttribute("SysExString",syx);
    }
    RefreshMode = tsRedrawActiveStave;
}

void CSysEx::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    QString syx = Symbol.attribute("SysExString");
    if (syx.length())
    {
        QString Buffer;
        for (int iTemp = 0; iTemp<syx.length(); iTemp+=2)
        {
            Buffer += QChar(QString("0x" + syx.mid(iTemp,2)).toInt());
        }
        MFile.Append(0xF0, 0, 0, -1, -1, Buffer);
    }
}

void CSysEx::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    QString syx = Symbol.attribute("SysExString");
    if (syx.length())
    {
        fibPlay(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
        MFile.SetTime(0);
        TemPlay.Currenttime = 0;
    }
}

QStringList CController::ControllerList=QStringList();

CController::CController() :CSymbol("Controller")
{
    if (ControllerList.isEmpty())
    {
        QFile fileData(":/OCControllers.txt");
        if (fileData.open(QIODevice::ReadOnly))
        {
            QTextStream readData(&fileData);
            while (!readData.atEnd()) ControllerList << readData.readLine();
            fileData.close();
        }
        else
        {
            for (int i=0;i<128;i++) ControllerList.append(QString::number(i));
        }
    }
    m_PropColl->Add("Controller", pwCustom, "", "", "Shows the Controller Dialog.", "", 0,false, "", "Behavior");
    m_PropColl->Add("ControllerNumber", pwList, "", "", "Returns/sets the Controller Number.", ControllerList, 0,false, "", "Behavior");
    m_PropColl->Add("ControllerValue", pwNumber, 0, 127, "Returns/sets the Controller Value.", "", 0,false, "", "Behavior");
}

QList<OCToolButtonProps*> CController::CreateButtons()
{
    CreateButton(":/Notes/Notes/controller.png",true);
    return m_ButtonList;
}

void CController::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value1=Symbol.getVal("ControllerNumber");
    int Value2=Symbol.getVal("ControllerValue");
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.ShowList("Controller");
    d.EditWidget->PutController(Value1,Value2);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetController(Value1,Value2);
        Symbol.setAttribute("ControllerNumber",Value1);
        Symbol.setAttribute("ControllerValue",Value2);
    }
    RefreshMode = tsRedrawActiveStave;
}

void CController::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    MFile.PlayController(Symbol.getVal("ControllerNumber"), Symbol.getVal("ControllerValue"), TemPlay.MIDI.Channel);
}

void CController::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    fibPlay(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
    MFile.SetTime(0);
    TemPlay.Currenttime = 0;
}

CChannel::CChannel() :CSymbol("Channel")
{
    m_PropColl->Add("Common", pwBoolean, "", "", "Returns/sets symbol in Voice 1 common to all Voices", "", 0, false, true, "Behavior");
    m_PropColl->Add("Channel", pwNumber, 1, 16, "Returns/sets the MIDI Channel to send on.", "", 0,false, 1, "Behavior");
}

QList<OCToolButtonProps*> CChannel::CreateButtons()
{
    CreateButton(":/Notes/Notes/channel.png",true);
    return m_ButtonList;
}

void CChannel::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value=Symbol.getVal("Channel");
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutChannel(Value);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetChannel(Value);
        Symbol.setAttribute("Channel",Value);
    }
    RefreshMode = tsRedrawActiveStave;
}

void CChannel::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
}

void CChannel::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.MIDI.Channel = Symbol.getVal("Channel") - 1;
    tempsetting.MIDI.Patch = 0;
}

void CChannel::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    TemPlay.MIDI.Channel = Symbol.getVal("Channel") - 1;
}

CAccent::CAccent():CVisibleSymbol("Accent")
{
    m_PropColl->Add("AddToVelocity", pwNumber, 0, 127, "Returns/sets the amount of Velocity to add to the Note of the Accent.", "", 0,false, 20, "Behavior");
}

QList<OCToolButtonProps*> CAccent::CreateButtons()
{
    CreateButton("Times new Roman",15,">",false,false);
    return m_ButtonList;
}

void CAccent::BeforeNote(XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCMIDIFile& MFile, OCPlayBackVarsType &TemPlay)
{
    PlayDynam += m_PropColl->GetValue("AddToVelocity").toInt();
}

void CAccent::InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType &TempPlay)
{
    m_PropColl->SetValue("AddToVelocity",Symbol.getVal("AddToVelocity"));
}

void CAccent::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    SignsToPrint.Append(OCTTFAccent, false, Symbol, SignCol, Pointer, new CAccent());
}

void CAccent::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    CAccent* S = new CAccent();
    SignsToPlay.AppendPlay(KillInstantly, 0, 0, S);
    S->InitPlaySymbol(Symbol, TemPlay);
}

void CAccent::PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    if (UpDown == 1) SignsUp ++;
    SignProps.DM(0, (12 * 12) + (UpDown * 14 * 12), ScreenObj);
    QList<QGraphicsItem*> l=ScreenObj.plLet(QChar(OCTTFAccent), SignProps.Size, OCTTFname, false, false, 1200, Qt::AlignHCenter);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),SignProps.Pointer);
}

OCProperties* CAccent::GetProperties(int Button)
{
    return OCPresets().SetPropertyValue(CSymbol::GetProperties(Button),"AddToVelocity","AccentAdd");
}

Cfp::Cfp():CVisibleSymbol("fp")
{
    m_PropColl->Add("StartVelocity", pwNumber, 0, 127, "Returns/sets the initial Velocity of the Note of the fp sign.", "", 0,false, "", "Behavior");
    m_PropColl->Add("EndVelocity", pwNumber, 0, 127, "Returns/sets the Velocity of the voice after the fp sign.", "", 0,false, "", "Behavior");
    FPcount=0;
    NewDynam=0;
}

QList<OCToolButtonProps*> Cfp::CreateButtons()
{
    CreateButton(":/Notes/Notes/fp.png");
    return m_ButtonList;
}

void Cfp::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    ScreenObj.DM(XFysic - (16 * 12), ScoreBottomSymbolY,Symbol);
    QList<QGraphicsItem*> l=ScreenObj.plLet(MakeUnicode("fp"), Symbol.size(), OCTTFname, false, false, 1200, Qt::AlignBottom);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

void Cfp::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    TemPlay.Currentdynam = Symbol.getVal("EndVelocity");
}

void Cfp::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Cfp* S = new Cfp;
    SignsToPlay.KillByName("Hairpin");
    SignsToPlay.KillByName("Dynamic Change");
    TemPlay.Currentdynam = Inside(TemPlay.Currentdynam + TemPlay.crescendo, 1, 127, 1);
    TemPlay.crescendo = 0;
    NewDynam = Symbol.getVal("EndVelocity");
    S->NewDynam=NewDynam;
    TemPlay.Currentdynam = Symbol.getVal("StartVelocity");
    int FPgap = TemPlay.Currentdynam - NewDynam;

    int Modulate=TemPlay.Playtempo;
    if (FPgap != 0) Modulate = TemPlay.Playtempo / FPgap;
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.AppendPlay(0, FPgap, Modulate, S);
}

void Cfp::DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int &LastTime, int Tick, int PlayTime, OCPlayBackVarsType &TemPlay)
{
    if (Tick <= TemPlay.Playtempo / 2) return;
    if (Props.Value > FPcount)
    {
        TemPlay.express = Inside(TemPlay.express - 1, 0, 127, 1);
        MFile.Append(0xB0 + TemPlay.MIDI.Channel, 0xB, TemPlay.express, LastTime);
        LastTime = 0;
        TemPlay.changeexp = 1;
        FPcount++;
    }
}

void Cfp::BeforeNote(XMLSymbolWrapper& XMLNote, int &PlayDynam, int &Pitch, int &endPitch, OCMIDIFile& MFile, OCPlayBackVarsType &TemPlay)
{
    FPcount = 0;
    TemPlay.Currentdynam = NewDynam;
}

OCProperties* Cfp::GetProperties(int Button)
{
    CSymbol::GetProperties(Button);
    OCPresets ps;
    m_PropColl->SetValue("StartVelocity",ps.GetValue("fpstartvel"));
    m_PropColl->SetValue("EndVelocity",ps.GetValue("fpendvel"));
    return m_PropColl;
}

Cfz::Cfz():CVisibleSymbol("fz")
{
    m_PropColl->Add("AddToVelocity", pwNumber, 0, 127, "Returns/sets the amount of Velocity to add to the Note of the fz sign.", "", 0,false, "", "Behavior");
    FPcount=0;
    NewDynam=0;
}

QList<OCToolButtonProps*> Cfz::CreateButtons()
{
    CreateButton(":/Notes/Notes/fz.png");
    return m_ButtonList;
}

void Cfz::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    ScreenObj.DM(XFysic - (16 * 12), ScoreBottomSymbolY,Symbol);
    QList<QGraphicsItem*> l=ScreenObj.plLet(MakeUnicode("Z"), Symbol.size(), OCTTFname, false, false, 1200, Qt::AlignBottom);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

void Cfz::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Cfz* S = new Cfz;
    NewDynam = TemPlay.Currentdynam;
    S->NewDynam=NewDynam;
    TemPlay.Currentdynam = Inside(TemPlay.Currentdynam + Symbol.getVal("AddToVelocity"), 1, 127, 1);
    int FPgap = TemPlay.Currentdynam - NewDynam;
    int Modulate=TemPlay.Playtempo;
    if (FPgap != 0) Modulate = TemPlay.Playtempo / FPgap;
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.AppendPlay(0, FPgap, Modulate, S);
}

void Cfz::DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int &LastTime, int Tick, int PlayTime, OCPlayBackVarsType &TemPlay)
{
    if (Tick <= TemPlay.Playtempo / 2) return;
    if (Props.Value > FPcount)
    {
        TemPlay.express = Inside(TemPlay.express - 1, 0, 127, 1);
        MFile.Append(0xB0 + TemPlay.MIDI.Channel, 0xB, TemPlay.express, LastTime);
        LastTime = 0;
        TemPlay.changeexp = 1;
        FPcount++;
    }
}

void Cfz::BeforeNote(XMLSymbolWrapper& XMLNote, int &PlayDynam, int &Pitch, int &endPitch, OCMIDIFile& MFile, OCPlayBackVarsType &TemPlay)
{
    FPcount = 0;
    TemPlay.Currentdynam = NewDynam;
}

OCProperties* Cfz::GetProperties(int Button)
{
    return OCPresets().SetPropertyValue(CSymbol::GetProperties(Button),"AddToVelocity","fzadd");
}

QStringList CAccidental::AccidentalList=QStringList() << "Flat" << "Sharp" << "DblFlat" << "DblSharp" << "Natural";

CAccidental::CAccidental() :CVisibleSymbol("Accidental",false,true)
{
    m_PropColl->Add("AccidentalSign", pwList, "", "", "Returns/sets the Accidentals Type.", AccidentalList, 0,false, "", "Appearance");
    m_PropColl->Add("Parentheses", pwBoolean, "", "", "Returns or sets whether the Accidental has Parantheses.", "", 0,false, "", "Appearance");
    m_ButtonProperty="AccidentalSign";
}

QList<OCToolButtonProps*> CAccidental::CreateButtons()
{
    CreateButton(":/Notes/Notes/flat.png",false,tsRedrawActiveStave,"Add Flat","Parentheses");
    CreateButton(":/Notes/Notes/sharp.png",false,tsRedrawActiveStave,"Add Sharp","Parentheses");
    CreateButton(":/Notes/Notes/doubleflat.png",false,tsRedrawActiveStave,"Add Double Flat","Parentheses");
    CreateButton(":/Notes/Notes/doublesharp.png",false,tsRedrawActiveStave,"Add Double Sharp","Parentheses");
    CreateButton(":/Notes/Notes/natural.png",false,tsRedrawActiveStave,"Add Natural","Parentheses");
    CreateButton("Times new Roman",15,"( )",false,false,false,tsRedrawActiveStave,"Add Parentheses to Accidentals","modifierParentheses");
    return m_ButtonList;
}

void CAccidental::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    SignsToPrint.Append(Symbol.getVal("AccidentalSign"), Symbol.getVal("Parentheses"), Symbol, SignCol, Pointer, new CAccidental);
}

QList<QGraphicsItem*> CAccidental::plLeftParanthesis(OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    ScreenObj.DR(-132,-150);
    l.append(ScreenObj.plLet("(",0,"Arial",false,false,156));
    ScreenObj.DR(72,150);
    return l;
}

void CAccidental::PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignProps.DM(ScreenObj);
    QList<QGraphicsItem*> l;
    switch (SignProps.Sign)
    {
    case 0:
        ScreenObj.DR(-11 * 12, 150);
        if (SignProps.Modifier) l.append(plLeftParanthesis(ScreenObj));
        l.append(ScreenObj.plLet(QChar(OCTTFFlat), SignProps.Size, OCTTFname, false, false, 1200));
        break;
    case 1:
        ScreenObj.DR(-12 * 12, 150);
        if (SignProps.Modifier) l.append(plLeftParanthesis(ScreenObj));
        l.append(ScreenObj.plLet(QChar(OCTTFSharp), SignProps.Size, OCTTFname, false, false, 1200));
        break;
    case 2:
        ScreenObj.DR(-17 * 12, 150);
        if (SignProps.Modifier) l.append(plLeftParanthesis(ScreenObj));
        l.append(ScreenObj.plLet(QChar(OCTTFDoubleFlat), SignProps.Size, OCTTFname, false, false, 1200));
        break;
    case 3:
        ScreenObj.DR(-14 * 12, 150);
        if (SignProps.Modifier) l.append(plLeftParanthesis(ScreenObj));
        l.append(ScreenObj.plLet(QChar(OCTTFDoubleSharp), SignProps.Size, OCTTFname, false, false, 1200));
        break;
    case 4:
        ScreenObj.DR(-12 * 12, 150);
        if (SignProps.Modifier) l.append(plLeftParanthesis(ScreenObj));
        l.append(ScreenObj.plLet(QChar(OCTTFOpl), SignProps.Size, OCTTFname, false, false, 1200));
        break;
    }
    if (SignProps.Modifier)
    {
        SignProps.DM(ScreenObj);
        ScreenObj.DR(-144,0);
        l.append(ScreenObj.plLet(")",0,"Arial",false,false,156));
    }
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),SignProps.Pointer);
}

QStringList CBowing::BowingList=QStringList() << "Up" << "Down";

CBowing::CBowing() : CVisibleSymbol("Bowing",false,true)
{
    m_PropColl->Add("Bowing", pwList, "", "", "Returns/sets the Type of Bowing.", BowingList, 0,false, "", "Appearance");
    m_ButtonProperty="Bowing";
}

QList<OCToolButtonProps*> CBowing::CreateButtons()
{
    CreateButton(":/Notes/Notes/upbow.png",false,tsRedrawActiveStave,"Add Up Bow");
    CreateButton(":/Notes/Notes/downbow.png",false,tsRedrawActiveStave,"Add Down Bow");
    return m_ButtonList;
}

void CBowing::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    SignsToPrint.Append(OCTTFBowing0 + Symbol.getVal("Bowing") + 1,false, Symbol, SignCol, Pointer, new CBowing);
}

void CBowing::PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    SignProps.DMVertical(UpDown, 0, (18 + (SignsUp * 12)) * 12, 0, ScreenObj);
    QList<QGraphicsItem*> l=ScreenObj.plLet(MakeUnicode(QChar(SignProps.Sign)), SignProps.Size, OCTTFname, false, false, 1200, Qt::AlignHCenter);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),SignProps.Pointer);
}

CBartokP::CBartokP() : CVisibleSymbol("BartokPizz",false,true)
{
}

QList<OCToolButtonProps*> CBartokP::CreateButtons()
{
    CreateButton(":/Notes/Notes/bartok.png",false,tsRedrawActiveStave,"Add Bartok Pizz");
    return m_ButtonList;
}

void CBartokP::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    SignsToPrint.Append(OCTTFBartok,false, Symbol, SignCol, Pointer, new CBartokP);
}

void CBartokP::PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    SignProps.DMVertical(UpDown, 0, (18 + (SignsUp * 12)) * 12,0, ScreenObj);
    QList<QGraphicsItem*> l=ScreenObj.plLet(MakeUnicode(QChar(SignProps.Sign)), SignProps.Size, OCTTFname, false, false, 1200, Qt::AlignHCenter);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),SignProps.Pointer);
}

CFingering::CFingering() : CVisibleSymbol("Fingering",false,true)
{
    m_PropColl->Add("Finger", pwNumber, 0, 5, "Returns/sets the Number.", "", 0,false, "", "Appearance");
    m_ButtonProperty="Finger";
}

QList<OCToolButtonProps*> CFingering::CreateButtons()
{
    for (int i=0;i<6;i++) CreateButton("Times new Roman",13,QString::number(i),false,false,false,tsRedrawActiveStave,"Add Fingering "+QString::number(i));
    return m_ButtonList;
}

void CFingering::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    SignsToPrint.Append(OCTTFFinger0 + Symbol.getVal("Finger"),false, Symbol, SignCol, Pointer, new CFingering);
}

void CFingering::PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    SignProps.DMVertical(UpDown,0, (18 + (SignsUp * 12)) * 12, 0, ScreenObj);
    QList<QGraphicsItem*> l=ScreenObj.plLet(MakeUnicode(QChar(SignProps.Sign)), SignProps.Size, OCTTFname, false, false, 1200, Qt::AlignHCenter);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),SignProps.Pointer);
}

CBarWidth::CBarWidth() :CSymbol("BarWidth")
{
    m_PropColl->Add("BarWidthValue", pwCustom, "", "", "Returns/sets the Note Value the Bar is formatted for.", "", 0,false, 24, "Behavior");
}

QList<OCToolButtonProps*> CBarWidth::CreateButtons()
{
    CreateButton(":/Notes/Notes/barwidth.png",true,tsReformat,"Add Fixed Bar Width");
    return m_ButtonList;
}

void CBarWidth::DrawFactor(XMLSymbolWrapper& Symbol, OCCounter *Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score)
{
    BarList.SetMinimumAll(Bar, Symbol.getVal("BarWidthValue"));
    //MinimumSet = true;
}

void CBarWidth::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value=Symbol.getVal("BarWidthValue");
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    int Noteval;
    bool Dotted;
    bool Triplet;
    XMLSimpleSymbolWrapper::SetNoteVal(Noteval,Dotted,Triplet,Value);
    d.EditWidget->PutNoteval(Noteval,Dotted,Triplet);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetNoteval(Noteval,Dotted,Triplet);
        Symbol.setAttribute("BarWidthValue",XMLSimpleSymbolWrapper::CalcTicks(Noteval,Dotted,Triplet));
    }
    RefreshMode = tsReformat;
}

QStringList CBeam::BeamList=QStringList() << "Default" << "1" << "2" << "3" << "4";

CBeam::CBeam():CDuratedSymbol("Beam")
{
    m_PropColl->Add("Beams", pwList, "", "", "Returns/sets the Number of Beams on the Note Group.", BeamList, 0,false, "", "Appearance");
}

QList<OCToolButtonProps*> CBeam::CreateButtons()
{
    CreateButton(":/Notes/Notes/beam.png",true);
    return m_ButtonList;
}

void CBeam::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    if (tempsetting.BalkOverRide.val <= 0)
    {
        tempsetting.BalkOverRide = OCDurSignType(Symbol,true);
        tempsetting.BalkOverRide.val = Symbol.getVal("Beams");
    }
}

CCue::CCue() :CVisibleSymbol("Cue",false,true)
{
}

QList<OCToolButtonProps*> CCue::CreateButtons()
{
    CreateButton("Times new Roman",28,"A",false,false);
    return m_ButtonList;
}

void CCue::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
        tempsetting.cueletter++;
        tempsetting.MasterStuff=true;
}

void CCue::PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType &tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj)
{
    ScreenObj.DM(XFysic - (14 * 12), (60 * 12) + ScoreStaffHeight,Symbol);
    ScreenObj.plLet(QChar(tempsetting.cueletter + 65), Symbol.size(), "times new roman", true, false, 240);
    QFontMetrics m(QFont("times new roman",240));
    ScreenObj.DR(0,-36);
    ScreenObj.DR(-24,-24,Symbol.size());
    QSize s1(m.width(QChar(tempsetting.cueletter+65)),m.height());
    QList<QGraphicsItem*> l=ScreenObj.PlRect(s1.width()+108,s1.height(),Symbol.size(),false,true);
    if (MTColorCheck(ScreenObj)) MTObj.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

CCoda::CCoda() :CVisibleSymbol("Coda")
{
    m_PropColl->Add("CodaType", pwList, "", "", "Returns/sets the Type of Coda Sign.", QStringList() << "Coda" << "To Coda", 0,false, "", "Appearance");
    m_ButtonProperty="CodaType";
}

QList<OCToolButtonProps*> CCoda::CreateButtons()
{
    CreateButton(":/Notes/Notes/coda.png");
    CreateButton("Times new Roman",12,"to\nCoda",false,true,false,tsRedrawActiveStave,"Add To Coda");
    return m_ButtonList;
}

void CCoda::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
}

void CCoda::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    if (TemPlay.Fine)
    {
        if (Symbol.getVal("CodaType") == 1)
        {
            Py=XMLScoreWrapper::FindSymbol(XMLVoice,"Coda",0,"CodaType",0);
            CountIt.reset();
            TemPlay.Fine = false;
        }
    }
}

void CCoda::PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType &tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    if (Symbol.getVal("CodaType") == 0)
    {
        ScreenObj.DM(XFysic, ScoreTopSymbolY + 96, Symbol);
        l.append(ScreenObj.plLet(QChar(OCTTFCoda), Symbol.size(), OCTTFname, false, false, 1200, Qt::AlignHCenter));
    }
    else
    {
        ScreenObj.DM(XFysic, ScoreTopSymbolY, Symbol);
        l.append(ScreenObj.plLet("Dal coda", Symbol.size(), "times new roman", true, true, 156));
    }
    if (MTColorCheck(ScreenObj)) MTObj.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

void CCoda::fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.MasterStuff=true;
}

CDaCapo::CDaCapo() :CVisibleSymbol("DaCapo")
{
}

QList<OCToolButtonProps*> CDaCapo::CreateButtons()
{
    CreateButton("Times new Roman",12,"D.C.",false,true,false,tsRedrawActiveStave,"Add Da Capo");
    return m_ButtonList;
}

void CDaCapo::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
}

void CDaCapo::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    if (!TemPlay.Fine)
    {
        Py = -1;
        CountIt.reset();
        TemPlay.Fine = true;
    }
}

void CDaCapo::PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType &tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj)
{
    ScreenObj.DM(XFysic, ScoreTopSymbolY,Symbol);
    QList<QGraphicsItem*> l=ScreenObj.plLet("Da capo", Symbol.size(), "times new roman", true, true, 156);
    if (MTColorCheck(ScreenObj)) MTObj.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

void CDaCapo::fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.MasterStuff=true;
}

CDobbel::CDobbel() :CVisibleSymbol("Turn")
{
    m_PropColl->Add("Direction", pwList, "", "", "Returns/sets the Shape of the turn", QStringList() << "Up Down" << "Down Up", 0,false, "", "Behavior");
    m_PropColl->Add("Speed", pwNumber, 1, 200, "Returns/sets the MIDI execution Speed of the Ornament.", "", 0,false, 50, "Behavior");
    m_PropColl->Add("Timing", pwList, "", "", "Returns/sets the MIDI execution style.", QStringList() << "Late" << "Early", 0,false, "", "Behavior");
    m_PropColl->Add("RangeDn", pwNumber, 1, 12, "Returns/sets the Downwards interval in half tones.", "", 0,false, 1, "Behavior");
    m_PropColl->Add("RangeUp", pwNumber, 1, 12, "Returns/sets the Upwards interval in half tones.", "", 0,false, 2, "Behavior");
    m_ButtonProperty="Direction";
}

QList<OCToolButtonProps*> CDobbel::CreateButtons()
{
    CreateButton(":/Notes/Notes/turnud.png",false,tsRedrawActiveStave,"Add Turn Up/Down");
    CreateButton(":/Notes/Notes/turndu.png",false,tsRedrawActiveStave,"Add Turn Down/Up");
    return m_ButtonList;
}

void CDobbel::InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType &TempPlay)
{
    trilldynam = 0;
    m_PropColl->SetValue("RangeDn", Symbol.getVal("RangeDn"));
    m_PropColl->SetValue("RangeUp", Symbol.getVal("RangeUp"));
    Currentstep = 0;
    BeginningOfNote = (Symbol.getVal("Timing") == 1);
    m_PropColl->SetValue("Direction",Symbol.getVal("Direction"));
}

void CDobbel::InitPrintSymbol(XMLSymbolWrapper& Symbol, OCPrintVarsType &dCurrent)
{
    m_PropColl->SetValue("Direction",Symbol.getVal("Direction"));
}

void CDobbel::PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    QList<QGraphicsItem*> l;
    //SignProps.DMVertical(UpDown, - (12 * 6), (18 + (SignsUp * 12)) * 12, 0, ScreenObj);
    SignProps.DMVertical(UpDown, 0, (18 + (SignsUp * 12)) * 12, 0, ScreenObj);
    if (m_PropColl->GetValue("Direction").toInt()==1)
    {
        l.append(ScreenObj.plLet(QChar(OCTTFDobbelDown), SignProps.Size, OCTTFname, false, false, 1200, Qt::AlignHCenter));
    }
    else
    {
        l.append(ScreenObj.plLet(QChar(OCTTFDobbelUp), SignProps.Size, OCTTFname, false, false, 1200, Qt::AlignHCenter));
    }
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),SignProps.Pointer);
}

void CDobbel::DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int &LastTime, int Tick, int PlayTime, OCPlayBackVarsType &TemPlay)
{
    if (Currentstep >= 4) return;
    if (!BeginningOfNote)
    {
        if (PlayTime - Tick > Props.Modulate * 4) return;
    }
    Currentstep++;
    switch (Currentstep)
    {
    case 1:
        if (m_PropColl->GetValue("Direction").toInt()==1)
        {
            CurrentPitch = BasePitch - m_PropColl->GetValue("RangeDn").toInt();
        }
        else
        {
            CurrentPitch = BasePitch + m_PropColl->GetValue("RangeUp").toInt();
        }
        break;
    case 2:
        CurrentPitch = BasePitch;
        break;
    case 3:
        if (m_PropColl->GetValue("Direction").toInt()==1)
        {
            CurrentPitch = BasePitch + m_PropColl->GetValue("RangeUp").toInt();
        }
        else
        {
            CurrentPitch = BasePitch - m_PropColl->GetValue("RangeDn").toInt();
        }
        break;
    case 4:
        CurrentPitch = BasePitch;
        break;
    }
    if (TemPlay.PortamentoOn)
    {
        MFile.Append(0x90 + TemPlay.MIDI.Channel, CurrentPitch, TemPlay.Currentdynam - trilldynam, LastTime);
        LastTime = 0;
        MFile.Append(0x80 + TemPlay.MIDI.Channel, oldpitch, TemPlay.Currentdynam - trilldynam, 0);
        MFile.Append(0xB0 + TemPlay.MIDI.Channel, 0x54, CurrentPitch, 0);
        trilldynam++;
    }
    else
    {
        MFile.Append(0x80 + TemPlay.MIDI.Channel, oldpitch, TemPlay.Currentdynam - trilldynam, LastTime);
        MFile.Append(0x90 + TemPlay.MIDI.Channel, CurrentPitch, TemPlay.Currentdynam - trilldynam, 0);
        LastTime = 0;
        trilldynam++;
    }
    oldpitch = CurrentPitch;
}

void CDobbel::BeforeNote(XMLSymbolWrapper& /*XMLNote*/, int & /*PlayDynam*/, int &Pitch, int & /*endPitch*/, OCMIDIFile& /*MFile*/, OCPlayBackVarsType & /*TemPlay*/)
{
    BasePitch = Pitch;
    CurrentPitch = Pitch;
    oldpitch = Pitch;
}

void CDobbel::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int /*Stave*/, int /*Track*/, OCNoteList& /*NoteList*/, int /*NoteCount*/, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & /*sCurrent*/, int Pointer, OCDraw& /*ScreenObj*/)
{
    CDobbel* S=new CDobbel;
    SignsToPrint.Append(96, false, Symbol, SignCol, Pointer, S);
    S->InitPrintSymbol(Symbol,dCurrent);
}

void CDobbel::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int & /*Py*/, QDomLiteElement* /*XMLVoice*/, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    CDobbel* S = new CDobbel;
    int Modulate = IntDiv(TemPlay.Playtempo * 30, Symbol.getVal("Speed"));
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.AppendPlay(0, 0, Modulate, S);
    S->InitPlaySymbol(Symbol, TemPlay);
}

OCProperties* CDobbel::GetProperties(int Button)
{
    return OCPresets().SetPropertyValue(CSymbol::GetProperties(Button),"Speed","trillspeed");
}

QStringList CDurLength::LengthList=QStringList() << "Length" << "Tenuto" << "Staccato";

CDurLength::CDurLength() :CDuratedSymbol("DuratedLength")
{
    m_PropColl->Add("PerformanceType", pwList, "", "", "Returns/sets the Type of sign.", LengthList, 0,false, "", "Appearance");
    m_PropColl->Add("Legato", pwNumber, 1, 100, "Returns/sets the MIDI execution length of the Legato Notes in Percent.", "", 0,false, 80, "Behavior");
    m_ButtonProperty="PerformanceType";
}

QList<OCToolButtonProps*> CDurLength::CreateButtons()
{
    CreateButton("Times new Roman",13,"Len",false,false,true);
    CreateButton(":/Notes/Notes/durlegato.png",false,tsRedrawActiveStave,"Add Legato");
    CreateButton(":/Notes/Notes/durstaccato.png",false,tsRedrawActiveStave,"Add Staccato");
    return m_ButtonList;
}

void CDurLength::plot(XMLSymbolWrapper& Symbol, int /*XFysic*/, OCBarList& /*BarList*/, OCCounter& /*CountIt*/, int /*BarCounter*/, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    CDurLength* S=new CDurLength;
    SignsToPrint.Append(63, false, Symbol, SignCol, Pointer, S);
    S->InitPrintSymbol(Symbol,dCurrent);
}

void CDurLength::InitPrintSymbol(XMLSymbolWrapper& Symbol, OCPrintVarsType & /*dCurrent*/)
{
    m_PropColl->SetValue("PerformanceType",Symbol.getVal("PerformanceType"));
}

void CDurLength::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
}

void CDurLength::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    if (Symbol.getVal("PerformanceType")==1)
    {
        tempsetting.StregOverRide=OCDurSignType(Symbol);
    }
    if (Symbol.getVal("PerformanceType")==2)
    {
        tempsetting.Punktoverride=OCDurSignType(Symbol);
    }
}

void CDurLength::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    CDurLength* S = new CDurLength;
    SignsToPlay.KillByName(m_Name);
    //SignsToPlay.KillByName("Staccato");
    SignsToPlay.AppendPlay(Symbol.ticks()+1, 0, 0, S);
    S->InitPlaySymbol(Symbol, TemPlay);
}

void CDurLength::PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    int SignType=m_PropColl->GetValue("PerformanceType").toInt();
    QList<QGraphicsItem*> l;
    if (SignType==1)
    {
        //legato
        if (ScreenObj.canColor())
        {
            l.append(OCNoteList::PlotLengths(2,SignProps.Pos,UpDown,SignProps.Size,ScreenObj));
        }
    }
    if (SignType==2)
    {
        //dot
        if (ScreenObj.canColor())
        {
            l.append(OCNoteList::PlotLengths(1,SignProps.Pos,UpDown,SignProps.Size,ScreenObj));
        }
    }
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),SignProps.Pointer);
}

void CDurLength::InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType &TempPlay)
{
    m_PropColl->SetValue("Legato", Symbol.getVal("Legato"));
}

void CDurLength::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutSpin("Length of the Voiced part of Notes in %",Symbol.getVal("Legato"),1,100);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("Legato",d.EditWidget->GetSpin());
    RefreshMode = tsRedrawActiveStave;
}

OCProperties* CDurLength::GetProperties(int Button)
{
    CSymbol::GetProperties(Button);
    switch (Button)
    {
    case 1:
        return OCPresets().SetPropertyValue(m_PropColl,"Legato","legatolen");
    case 2:
        return OCPresets().SetPropertyValue(m_PropColl,"Legato","stacclen");
    }
    return m_PropColl;
}

CDurSlant::CDurSlant() :CDuratedSymbol("DuratedSlant",true,true,true)
{
    m_PropColl->Add("Slanting", pwBoolean, "", "", "Returns or sets whether Beam slanting is on.", "", 0,false, "", "Behavior");
}

QList<OCToolButtonProps*> CDurSlant::CreateButtons()
{
    CreateButton(":/Notes/Notes/slanton.png",false,tsRedrawActiveStave,"Beam Slanting On");
    CreateButton(":/Notes/Notes/slantoff.png",false,tsRedrawActiveStave,"Beam Slanting Off");
    return m_ButtonList;
}

void CDurSlant::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.SlantOverRide = OCDurSignType(Symbol);
    tempsetting.SlantOverRide.val = !Symbol.getVal("Slanting");
}

OCProperties* CDurSlant::GetProperties(int Button)
{
    CSymbol::GetProperties(Button);
    m_PropColl->SetValue("Slanting",!(bool)Button);
    return m_PropColl;
}

CDurUpDown::CDurUpDown() :CDuratedSymbol("DuratedBeamDirection",true,true,true)
{
    m_PropColl->Add("Direction", pwList, "", "", "Returns or sets whether the Stem Direction of the Notes in the Group is up or down.", QStringList() << "Up" << "Down", 0,false, "", "Behavior");
    m_ButtonProperty="Direction";
}

QList<OCToolButtonProps*> CDurUpDown::CreateButtons()
{
    CreateButton(":/Notes/Notes/stemsup.png",false,tsRedrawActiveStave,"Add Stems Up");
    CreateButton(":/Notes/Notes/stemsdown.png",false,tsRedrawActiveStave,"Add Stems Down");
    return m_ButtonList;
}

void CDurUpDown::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    int TempDir = 1;
    if (Symbol.getVal("Direction") == 0) TempDir = -1;
    tempsetting.UpDownOverRide = OCDurSignType(Symbol);
    tempsetting.UpDownOverRide.val = TempDir;
}

CDynChange::CDynChange() :CVisibleSymbol("DynamicChange")
{
    m_PropColl->Add("Common", pwBoolean, "", "", "Returns/sets symbol in Voice 1 common to all Voices", "", 0, false, true, "Behavior");
    m_PropColl->Add("DynamicType", pwList, "", "", "Returns/sets the Type of Dynamic Change.", QStringList() << "Diminuendo" << "Crescendo", 0,false, "", "Appearance");
    m_PropColl->Add("Speed", pwNumber, 1, 100, "Returns/sets the MIDI execution speed of the Dynamic Change.", "", 0,false, 50, "Behavior");
    m_ButtonProperty="DynamicType";
}

QList<OCToolButtonProps*> CDynChange::CreateButtons()
{
    CreateButton("Times new Roman",13,"dim",false,true,false,tsRedrawActiveStave,"Add Diminuendo");
    CreateButton("Times new Roman",13,"cresc",false,true,false,tsRedrawActiveStave,"Add Crescendo");
    return m_ButtonList;
}

void CDynChange::InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType &TempPlay)
{
    m_PropColl->SetValue("Speed", Symbol.getVal("Speed"));
    m_PropColl->SetValue("DynamicType", Symbol.getVal("DynamicType"));
}

void CDynChange::AfterNote(XMLSymbolWrapper& XMLNote, OCPlayBackVarsType &TemPlay)
{
    if (m_PropColl->GetValue("DynamicType").toInt() > 0)
    {
        TemPlay.crescendo += FloatDiv(XMLNote.ticks(), 40) * m_PropColl->GetValue("Speed").toInt();
    }
    else if (m_PropColl->GetValue("DynamicType").toInt() == 0)
    {
        TemPlay.crescendo += FloatDiv(XMLNote.ticks(), 60) * -m_PropColl->GetValue("Speed").toInt();
    }
}

void CDynChange::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    //ScreenObj.DM(XFysic, 37 * 12,XMLSymbol);
    ScreenObj.DM(XFysic,ScoreBottomSymbolY,Symbol);
    if (Symbol.getVal("DynamicType") > 0)
    {
        l.append(ScreenObj.plLet("cresc", Symbol.size(), Score.DynamicFont(), Qt::AlignBottom));
    }
    else
    {
        l.append(ScreenObj.plLet("dim", Symbol.size(), Score.DynamicFont(), Qt::AlignBottom));
    }
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

void CDynChange::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
}

void CDynChange::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    CDynChange* S = new CDynChange;
    SignsToPlay.KillByName("Hairpin");
    SignsToPlay.KillByName(m_Name);
    SignsToPlay.AppendPlay(NotDecrementable, 0, 0, S);
    S->InitPlaySymbol(Symbol, TemPlay);
}

OCProperties* CDynChange::GetProperties(int Button)
{
    if (Button==0) return OCPresets().SetPropertyValue(CSymbol::GetProperties(Button),"Speed","dimspeed");
    return OCPresets().SetPropertyValue(CSymbol::GetProperties(Button),"Speed","crescspeed");
}

CExpression::CExpression() :CSymbol("Expression")
{
    m_PropColl->Add("Disabled", pwBoolean, "", "", "Returns or sets whether the use of Expression controllers is on.", "", 0,false, "", "Behavior");
}

QList<OCToolButtonProps*> CExpression::CreateButtons()
{
    CreateButton(":/Notes/Notes/expression.png",true);
    return m_ButtonList;
}

void CExpression::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
}

void CExpression::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    TemPlay.ExpressionOn=!Symbol.getVal("Disabled");
}

void CExpression::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value=(int)Symbol.getVal("Disabled");
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutCombo("Expression controller (controller 11)",Value,QStringList() << "Enabled" << "Disabled");
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("Disabled",(bool)d.EditWidget->GetCombo());
    RefreshMode = tsRedrawActiveStave;
}

CFermata::CFermata() :CVisibleSymbol("Fermata")
{
    m_PropColl->Add("Common", pwBoolean, "", "", "Returns/sets symbol in Voice 1 common to all Voices", "", 0, false, true, "Behavior");
    m_PropColl->Add("Duration", pwNumber, 1, 100, "Returns/sets the MIDI execution time of the Fermata.", "", 0,false, 1, "Behavior");
}

QList<OCToolButtonProps*> CFermata::CreateButtons()
{
    CreateButton(":/Notes/Notes/fermata.png");
    return m_ButtonList;
}

void CFermata::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
}

void CFermata::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    CFermata* S = new CFermata;
    SignsToPlay.AppendPlay(KillInstantly, 0, 0, S);
    S->InitPlaySymbol(Symbol, TemPlay);
}

void CFermata::InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType &TempPlay)
{
    m_PropColl->SetValue("Duration", (Symbol.getVal("Duration") - 1) * 10);
}

void CFermata::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    ScreenObj.DM(XFysic, 1628,Symbol);
    QList<QGraphicsItem*> l=ScreenObj.plLet(QChar(OCTTFFermata), Symbol.size(), OCTTFname, false, false, 1200, Qt::AlignHCenter);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

CFine::CFine() :CVisibleSymbol("Fine")
{
}

QList<OCToolButtonProps*> CFine::CreateButtons()
{
    CreateButton("Times new Roman",12,"Fine",false,true);
    return m_ButtonList;
}

void CFine::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
}

void CFine::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    if (TemPlay.Fine)
    {
        Py=XMLScoreWrapper::FindSymbol(XMLVoice,"",Py);
    }
}

void CFine::PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType &tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj)
{
    ScreenObj.DM(XFysic, ScoreTopSymbolY, Symbol);
    QList<QGraphicsItem*> l=ScreenObj.plLet("Fine", Symbol.size(), "times new roman", true, true, 156);
    if (MTColorCheck(ScreenObj)) MTObj.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

void CFine::fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.MasterStuff=true;
}

CHairpin::CHairpin() :CGapSymbol("Hairpin", "Gap", "Returns/sets the angle of the Hairpin signs.")
{
    m_PropColl->Add("Speed", pwNumber, 0, 127, "Returns/sets the MIDI execution speed of the dynamic change.", "", 0,false, 50, "Behavior");
    m_PropColl->Add("HairpinType", pwList, "", "", "Returns or sets the direction of the dynamic change.", QStringList() << "<" << ">" << "><" << "<>", 0,false, "", "Appearance");
    m_ButtonProperty="HairpinType";
}

QList<OCToolButtonProps*> CHairpin::CreateButtons()
{
    CreateButton(":/Notes/Notes/hpcresc.png",false,tsRedrawActiveStave,"Add Crescendo");
    CreateButton(":/Notes/Notes/hpdim.png",false,tsRedrawActiveStave,"Add Diminuendo");
    CreateButton("Times new Roman",20,"><",true,false,false,tsRedrawActiveStave,"Add Inverted 'Fish'");
    CreateButton("Times new Roman",20,"<>",true,false,false,tsRedrawActiveStave,"Add 'Fish'");
    return m_ButtonList;
}

void CHairpin::InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType &TempPlay)
{
    m_PropColl->SetValue("HairpinType",Symbol.getVal("HairpinType"));
    if (Symbol.getVal("HairpinType") > 1)
    {
        m_PropColl->SetValue("Speed", Symbol.getVal("Speed") / 2);
        FishLen = Symbol.ticks() * 5;
    }
    else
    {
        m_PropColl->SetValue("Speed", Symbol.getVal("Speed"));
    }
}

void CHairpin::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    CHairpin* S = new CHairpin;
    SignsToPlay.KillByName(m_Name);
    SignsToPlay.KillByName("DynamicChange");
    int Modulate = IntDiv(TemPlay.Playtempo * 2, Symbol.getVal("Speed"));
    if (Symbol.getVal("HairpinType")>1)
    {
        Modulate = IntDiv(TemPlay.Playtempo * 2, Symbol.getVal("Speed") / 2);
    }
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.AppendPlay(Symbol.ticks(), 0, Modulate, S);
    S->InitPlaySymbol(Symbol, TemPlay);
}

void CHairpin::DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int &LastTime, int Tick, int PlayTime, OCPlayBackVarsType &TemPlay)
{
    Direction = 1;
    if ((m_PropColl->GetValue("HairpinType").toInt() == 2) || (m_PropColl->GetValue("HairpinType").toInt() == 1))  Direction = -1;
    if (m_PropColl->GetValue("HairpinType").toInt() > 1)
    {
        AfterNoteDuration = Props.Duration;
        if (FishLen <= Tick) Direction = -Direction;
    }
    TemPlay.express = Inside(TemPlay.express + Direction, 0, 127, 1);
    MFile.Append(0xB0 + TemPlay.MIDI.Channel, 0xB, TemPlay.express, LastTime);
    LastTime = 0;
    TemPlay.changeexp = 1;
}

void CHairpin::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    if (Symbol.getVal("HairpinType")==0)
    {
        tempsetting.crescendo=OCDurSignType(Symbol);
        tempsetting.crescendo.Size = Symbol.getVal("Gap");
    }
    if (Symbol.getVal("HairpinType")==1)
    {
        tempsetting.diminuendo=OCDurSignType(Symbol);
        tempsetting.diminuendo.Size = Symbol.getVal("Gap");
    }
}

void CHairpin::BeforeNote(XMLSymbolWrapper& XMLNote, int &PlayDynam, int &Pitch, int &endPitch, OCMIDIFile& MFile, OCPlayBackVarsType &TemPlay)
{
    int crescvar=0;
    if (TemPlay.ExpressionOn)
    {
        crescvar = (63 - (TemPlay.currentcresc / 2) * 1);
        if (crescvar > 127 - PlayDynam) crescvar = 127 - PlayDynam;
        if (crescvar > expressiondefault - 1) crescvar = expressiondefault - 1;
        PlayDynam += crescvar;
    }
    PlayDynam = Inside(PlayDynam, 1, 127, 1);
    TemPlay.exprbegin = expressiondefault - crescvar;
}

void CHairpin::AfterNote(XMLSymbolWrapper& XMLNote, OCPlayBackVarsType &TemPlay)
{
    if (m_PropColl->GetValue("HairpinType").toInt()>1)
    {
        if (FishLen) FishLen = FishLen - (XMLNote.ticks() * 10);
    }
    if (Direction > 0)
    {
        TemPlay.crescendo += FloatDiv(XMLNote.ticks(), 40) * m_PropColl->GetValue("Speed").toInt() * Direction;
    }
    else if (Direction < 0)
    {
        TemPlay.crescendo += FloatDiv(XMLNote.ticks(), 60) * m_PropColl->GetValue("Speed").toInt() * Direction;
    }
    if (m_PropColl->GetValue("HairpinType").toInt()>1)
    {
        if (AfterNoteDuration == XMLNote.ticks()) TemPlay.crescendo = 0;
    }
}

OCProperties* CHairpin::GetProperties(int Button)
{
    CSymbol::GetProperties(Button);
    switch (Button)
    {
    case 0:
    case 3:
        return OCPresets().SetPropertyValue(m_PropColl,"Speed","crescspeed");
    case 1:
    case 2:
        return OCPresets().SetPropertyValue(m_PropColl,"Speed","dimspeed");
    }
    return m_PropColl;
}

void CHairpin::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    NoteList.PlotHairPin(NoteCount, Symbol.ticks(), Symbol.getVal("HairpinType"), Symbol.pos(), Symbol.getVal("Gap"), SymbolList, false, Pointer, ScreenObj);
}

QStringList CFlipTie::FlipList=QStringList() << "Flipped" << "Normal";

CFlipTie::CFlipTie() : CSymbol("FlipTie")
{
    m_PropColl->Add("TieDirection", pwList, "", "", "Returns or sets whether Ties should be shown upside down.", FlipList, 0,false, "", "Behavior");
}

QList<OCToolButtonProps*> CFlipTie::CreateButtons()
{
    CreateButton(":/Notes/Notes/slurdirection.png",true,tsRedrawActiveStave,"Add Flip Ties");
    return m_ButtonList;
}

void CFlipTie::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.FlipTie=!Symbol.getVal("TieDirection");
}

void CFlipTie::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value=(int)Symbol.getVal("TieDirection");
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutCombo("Show Ties Upside Down",Value,FlipList);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("TieDirection",(bool)d.EditWidget->GetCombo());
    RefreshMode = tsRedrawActiveStave;
}

CGliss::CGliss() :CVisibleSymbol("Glissando")
{
    m_PropColl->Add("Range", pwNumber, -48, 48, "Returns/sets the range of the Glissando in half tones.", "", 0,false, "", "Behavior");
    GlissModulate=0;
}

QList<OCToolButtonProps*> CGliss::CreateButtons()
{
    CreateButton(":/Notes/Notes/glissando.png",true);
    return m_ButtonList;
}

void CGliss::InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType &TempPlay)
{
    Range = Symbol.getVal("Range");
}

void CGliss::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    SignsToPrint.Append(201, Symbol.getVal("Range"), Symbol, SignCol, Pointer, new CGliss);
}

void CGliss::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutSpin("Range",Symbol.getVal("Range"),-48,48);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("Range",d.EditWidget->GetSpin());
    RefreshMode = tsRedrawActiveStave;
}

void CGliss::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    CGliss* S = new CGliss;
    SignsToPlay.AppendPlay(0, 0, 1, S);
    S->InitPlaySymbol(Symbol, TemPlay);
}

void CGliss::PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignProps.DM(144, Sgn(SignProps.Modifier) * 24, ScreenObj);
    QList<QGraphicsItem*> l=ScreenObj.DJ(SignProps.TieLen-48, SignProps.Modifier * 24 - Sgn(SignProps.Modifier),true);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),SignProps.Pointer);
}

void CGliss::BeforeNote(XMLSymbolWrapper& XMLNote, int &PlayDynam, int &Pitch, int &endPitch, OCMIDIFile& MFile, OCPlayBackVarsType &TemPlay)
{
    CurrentPitch = Pitch;
    oldpitch = Pitch;
    Direction = Sgn(Range);
    endPitch = Pitch + Range;
}

void CGliss::DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int &LastTime, int Tick, int PlayTime, OCPlayBackVarsType &TemPlay)
{
    if (GlissModulate == 0) GlissModulate = (int)FloatDiv(PlayTime, Abs(Range) + 1);
    if (CurrentPitch != Pitch + Range)
    {
        if (Tick % GlissModulate == 0)
        {
            CurrentPitch += Direction;
            if (TemPlay.PortamentoOn)
            {
                MFile.Append(0x90 + TemPlay.MIDI.Channel, CurrentPitch, TemPlay.Currentdynam, LastTime);
                LastTime = 0;
                MFile.Append(0x80 + TemPlay.MIDI.Channel, oldpitch, TemPlay.Currentdynam, 0);
                MFile.Append(0xB0 + TemPlay.MIDI.Channel, 0x54, CurrentPitch, 0);
            }
            else
            {
                MFile.Append(0x80 + TemPlay.MIDI.Channel, oldpitch, TemPlay.Currentdynam, LastTime);
                MFile.Append(0x90 + TemPlay.MIDI.Channel, CurrentPitch, TemPlay.Currentdynam, 0);
                LastTime = 0;
            }
            oldpitch = CurrentPitch;
        }
    }
}

QStringList CLength::LengthList=QStringList() << "Length" << "Tenuto" << "Staccato" << "Length x1" << "Tenuto x1" << "Staccato x1";

CLength::CLength() : CVisibleSymbol("Length")
{
    m_PropColl->Add("PerformanceType", pwList, "", "", "Returns/sets the Type of sign.", LengthList, 0,false, "", "Appearance");
    m_PropColl->Add("Legato", pwNumber, 1, 100, "Returns/sets the MIDI execution length of the Note(s) in Percent.", "", 0,false, 80, "Behavior");
    m_ButtonProperty="PerformanceType";
}

QList<OCToolButtonProps*> CLength::CreateButtons()
{
    CreateButton("Times new Roman",11,QString("Len\n")+QChar(0x2192),false,false,true,tsRedrawActiveStave,"Add Length");
    CreateButton("Times new Roman",11,QString(QChar(0x25ac))+"\n"+QString(QChar(0x2192)),false,false,false,tsRedrawActiveStave,"Add Tenuto");
    CreateButton("Times new Roman",11,QString(QChar(0x25cf))+"\n"+QString(QChar(0x2192)),false,false,false,tsRedrawActiveStave,"Add Legato");
    CreateButton("Times new Roman",11,"Len\nx1",false,false,true,tsRedrawActiveStave,"Add Length x1");
    CreateButton("Times new Roman",11,QChar(0x25ac)+QString("\nx1"),false,false,false,tsRedrawActiveStave,"Add Tenuto x1");
    CreateButton("Times new Roman",11,QChar(0x25cf)+QString("\nx1"),false,false,false,tsRedrawActiveStave,"Add Legato x1");
    return m_ButtonList;
}

void CLength::InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType &TempPlay)
{
    m_PropColl->SetValue("Legato", Symbol.getVal("Legato"));
    m_PropColl->SetValue("PerformanceType", Symbol.getVal("PerformanceType"));
}

void CLength::ModifyProperties(OCProperties* p)
{
    bool Hide = ((p->GetValue("PerformanceType").toInt() == 0) || (p->GetValue("PerformanceType").toInt() == 3));
    p->GetItem("Left")->Hidden = Hide;
    p->GetItem("Top")->Hidden = Hide;
    p->GetItem("Size")->Hidden = Hide;
    p->GetItem("Invisible")->Hidden = Hide;
}

void CLength::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType &sCurrent, int Pointer, OCDraw& ScreenObj)
{
    SignsToPrint.Append(Symbol.getVal("PerformanceType"), false, Symbol, SignCol, Pointer, new CLength);
}

void CLength::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutSpin("Length of the Voiced part of Notes in %",Symbol.getVal("Legato"),1,100);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("Legato",d.EditWidget->GetSpin());
    RefreshMode = tsRedrawActiveStave;
}

void CLength::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    if (Symbol.getVal("PerformanceType") < 3) TemPlay.currentlen = Symbol.getVal("Legato");
}

void CLength::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    if (Symbol.getVal("PerformanceType") < 3)
    {
        tempsetting.Articulation=OCSignType(Symbol);
        tempsetting.Articulation.val = Symbol.getVal("PerformanceType");
    }
    else
    {
        tempsetting.Articulationx1=OCSignType(Symbol);
        tempsetting.Articulationx1.val = Symbol.getVal("PerformanceType");
    }
}

void CLength::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    CLength* S = new CLength;
    if (Symbol.getVal("PerformanceType") > 2)
    {
        SignsToPlay.AppendPlay(KillInstantly, 0, 0, S);
        S->InitPlaySymbol(Symbol, TemPlay);
    }
    else
    {
        fibPlay(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, TemPlay);
    }
}

void CLength::PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    if ((SignProps.Sign == 1) || (SignProps.Sign == 4))
    {
        //legato
        if (ScreenObj.canColor())
        {
            l.append(OCNoteList::PlotLengths(2,SignProps.Pos,UpDown,SignProps.Size,ScreenObj));
        }
    }
    else if ((SignProps.Sign == 2) || (SignProps.Sign == 5))
    {
        //dot
        if (ScreenObj.canColor())
        {
            l.append(OCNoteList::PlotLengths(1,SignProps.Pos,UpDown,SignProps.Size,ScreenObj));
        }
    }
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),SignProps.Pointer);
}

OCProperties* CLength::GetProperties(int Button)
{
    CSymbol::GetProperties(Button);
    switch (Button)
    {
    case 1:
    case 4:
        return OCPresets().SetPropertyValue(m_PropColl,"Legato","legatolen");
    case 2:
    case 5:
        return OCPresets().SetPropertyValue(m_PropColl,"Legato","stacclen");
    }
    return m_PropColl;
}

CLimit::CLimit() :CSymbol("BeamLimit")
{
    m_PropColl->Add("SixteenthsNotes", pwNumber, 0, 64, "Returns/sets the Max number of sixteenth notes to fit under a Beam.", "", 0,false, 4, "Behavior");
}

QList<OCToolButtonProps*> CLimit::CreateButtons()
{
    CreateButton(":/Notes/Notes/beamlimit.png",true,tsRedrawActiveStave,"Add Beam Limit");
    return m_ButtonList;
}

void CLimit::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.BalkLimit = 6 * Symbol.getVal("SixteenthsNotes");
}

void CLimit::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutSpin("Length in 16ths",Symbol.getVal("SixteenthsNotes"),0,64);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("SixteenthsNotes",d.EditWidget->GetSpin());
    RefreshMode = tsRedrawActiveStave;
}

CMordent::CMordent() :CVisibleSymbol("Mordent")
{
    m_PropColl->Add("Speed", pwNumber, 1, 200, "Returns/sets the MIDI execution Speed of the Ornament.", "", 0,false, 50, "Behavior");
    m_PropColl->Add("Range", pwNumber, -12, 12, "Returns/sets the Downwards interval in half tones.", "", 0,false, -2, "Behavior");
}

QList<OCToolButtonProps*> CMordent::CreateButtons()
{
    CreateButton(":/Notes/Notes/mordent1.png",false,tsRedrawActiveStave,"Add Mordernt Down");
    CreateButton(":/Notes/Notes/mordent.png",false,tsRedrawActiveStave,"Add Mordernt Up");
    return m_ButtonList;
}

void CMordent::InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType &TempPlay)
{
    trilldynam = 0;
    TrillDir = Symbol.getVal("Range");
    Finished = false;
}

void CMordent::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    int SignChar=OCTTFMordent;
    if (Symbol.getVal("Range")>0) SignChar=OCTTFPraltrill;
    SignsToPrint.Append(SignChar, false, Symbol, SignCol, Pointer, new CMordent);
}

void CMordent::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    CMordent* S = new CMordent;
    int Modulate = IntDiv(TemPlay.Playtempo * 30, Symbol.getVal("Speed"));
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.AppendPlay(0, 0, Modulate, S);
    S->InitPlaySymbol(Symbol, TemPlay);
}

void CMordent::PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    SignProps.DMVertical(UpDown, 0, (18 + (SignsUp * 12)) * 12, 0, ScreenObj);
    QList<QGraphicsItem*> l=ScreenObj.plLet(MakeUnicode(QChar(SignProps.Sign)), SignProps.Size, OCTTFname, false, false, 1200, Qt::AlignHCenter);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),SignProps.Pointer);
}

void CMordent::DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int &LastTime, int Tick, int PlayTime, OCPlayBackVarsType &TemPlay)
{
    if (Finished) return;
    if (CurrentPitch == BasePitch)
    {
        CurrentPitch = BasePitch + TrillDir;
    }
    else
    {
        CurrentPitch = BasePitch;
        Finished = true;
    }
    if (TemPlay.PortamentoOn)
    {
        MFile.Append(0x90 + TemPlay.MIDI.Channel, CurrentPitch, TemPlay.Currentdynam - trilldynam, LastTime);
        LastTime = 0;
        MFile.Append(0x80 + TemPlay.MIDI.Channel, oldpitch, TemPlay.Currentdynam - trilldynam, 0);
        MFile.Append(0xB0 + TemPlay.MIDI.Channel, 0x54, CurrentPitch, LastTime);
        trilldynam ++;
    }
    else
    {
        MFile.Append(0x80 + TemPlay.MIDI.Channel, oldpitch, TemPlay.Currentdynam - trilldynam, LastTime);
        MFile.Append(0x90 + TemPlay.MIDI.Channel, CurrentPitch, TemPlay.Currentdynam - trilldynam, 0);
        LastTime = 0;
        trilldynam++;
    }
    oldpitch = CurrentPitch;
}

void CMordent::BeforeNote(XMLSymbolWrapper& XMLNote, int &PlayDynam, int &Pitch, int &endPitch, OCMIDIFile& MFile, OCPlayBackVarsType &TemPlay)
{
    BasePitch = Pitch;
    CurrentPitch = Pitch;
    oldpitch = Pitch;
}

OCProperties* CMordent::GetProperties(int Button)
{
    CSymbol::GetProperties(Button);
    switch (Button)
    {
    case 0:
        m_PropColl->SetValue("Range",-2);
        break;
    case 1:
        m_PropColl->SetValue("Range",2);
        break;
    }
    return OCPresets().SetPropertyValue(m_PropColl,"Speed","trillspeed");
}

QStringList COctave::OctaveList=QStringList() << "15ma Down" << "8va Down" << "Loco" << "8va Up" << "15ma Up";

COctave::COctave() :CVisibleSymbol("Octave")
{
    m_PropColl->Add("Common", pwBoolean, "", "", "Returns/sets symbol in Voice 1 common to all Voices", "", 0, false, true, "Behavior");
    m_PropColl->Add("OctaveType", pwList, "", "", "Returns/sets the Type of Octave change.", OctaveList, 0,false, "", "Appearance");
    m_ButtonProperty="OctaveType";
}

QList<OCToolButtonProps*> COctave::CreateButtons()
{
    CreateButton("Times new Roman",15,QChar(0x2193)+QString("15"),false,true,false,tsRedrawActiveStave,"Add 15ma Down");
    CreateButton("Times new Roman",15,QChar(0x2193)+QString("8"),false,true,false,tsRedrawActiveStave,"Add 8va Down");
    CreateButton("Times new Roman",13,"loco",false,true,false,tsRedrawActiveStave,"Add Loco");
    CreateButton("Times new Roman",15,QChar(0x2191)+QString("8"),false,true,false,tsRedrawActiveStave,"Add 8va Up");
    CreateButton("Times new Roman",15,QChar(0x2191)+QString("15"),false,true,false,tsRedrawActiveStave,"Add 15ma Up");
    return m_ButtonList;
}

void COctave::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol,MFile,CountIt,Py,XMLVoice,SignsToPlay,TemPlay);
}

void COctave::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    if ((Symbol.getVal("OctaveType") - 2) > 0)
    {
        TemPlay.MIDI.Octave = 12 * (Symbol.getVal("OctaveType") - 2);
    }
    else
    {
        TemPlay.MIDI.Octave = (Symbol.getVal("OctaveType") - 2) * 12;
    }
}

void COctave::fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    if ((Symbol.getVal("OctaveType") - 2) > 0)
    {
        tempsetting.MIDI.Octave = 12 * (Symbol.getVal("OctaveType") - 2);
    }
    else
    {
        tempsetting.MIDI.Octave = (Symbol.getVal("OctaveType") - 2) * 12;
    }
}

void COctave::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    int d = 45 * 12;
    if ((Symbol.getVal("OctaveType") - 2) >= 0)
    {
        d = ScoreTopSymbolY;
    }
    ScreenObj.DM(XFysic-60, d, Symbol);
    //if (!GetVal(XMLSymbol, "Invisible"))
    //{
        QList<QGraphicsItem*> l;
        switch ((int)Symbol.getVal("OctaveType") - 2)
        {
        case 1:
            l.append(ScreenObj.DL(0, 60));
            l.append(ScreenObj.DL(60, 0));
            ScreenObj.DR(36, 0);
            l.append(ScreenObj.plLet("8va", Symbol.size(), Score.DynamicFont()));
            break;
        case 2:
            l.append(ScreenObj.DL(0, 60));
            l.append(ScreenObj.DL(60, 0));
            ScreenObj.DR(36, 0);
            l.append(ScreenObj.plLet("15ma", Symbol.size(), Score.DynamicFont()));
            break;
        case -2:
            l.append(ScreenObj.DL(0, -60));
            l.append(ScreenObj.DL(60, 0));
            ScreenObj.DR(36, 0);
            l.append(ScreenObj.plLet("15ma", Symbol.size(), Score.DynamicFont()));
            break;
        case -1:
            l.append(ScreenObj.DL(0, -60));
            l.append(ScreenObj.DL(60, 0));
            ScreenObj.DR(36, 0);
            l.append(ScreenObj.plLet("8va", Symbol.size(), Score.DynamicFont()));
            break;
        case 0:
            l.append(ScreenObj.plLet("loco", Symbol.size(), Score.DynamicFont()));
            break;
        }
        SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
    //}
}

QStringList CPortamento::PortamentoList=QStringList() << "Enabled" << "Disabled";

CPortamento::CPortamento() :CSymbol("Portamento")
{
    m_PropColl->Add("Portamento", pwList, "", "", "Returns or sets whether the use of MIDI Portamento Controllers is on.", PortamentoList, 0,false, "", "Behavior");
}

QList<OCToolButtonProps*> CPortamento::CreateButtons()
{
    CreateButton(":/Notes/Notes/synth.png",true);
    return m_ButtonList;
}

void CPortamento::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol,MFile,CountIt,Py,XMLVoice,SignsToPlay,TemPlay);
}

void CPortamento::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    TemPlay.PortamentoOn = !Symbol.getVal("Portamento");
}

void CPortamento::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value=(int)Symbol.getVal("Portamento");
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutCombo("Portamento (controller 84)",Value,PortamentoList);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("Portamento",(bool)d.EditWidget->GetCombo());
    RefreshMode = tsRedrawActiveStave;
}

QStringList CRepeat::RepeatList=QStringList() << "End" << "Begin" << "Volta";

CRepeat::CRepeat() :CVisibleSymbol("Repeat")
{
    m_PropColl->Add("RepeatType", pwList, "", "", "Returns/sets the Type of Repeat sign.", RepeatList, 0,false, "", "Appearance");
    m_PropColl->Add("Repeats", pwNumber, 1, 127, "Returns/sets the number of Repeats.", "", 0,false, 2, "Appearance");
    m_PropColl->Add("Volta", pwNumber, 1, 127, "Returns/sets the Volta.", "", 0,false, 1, "Appearance");
    PlayRepeat=0;
    Repeat.clear();
    m_ButtonProperty="RepeatType";
}

QList<OCToolButtonProps*> CRepeat::CreateButtons()
{
    CreateButton(":/Notes/Notes/repeatend.png",true,tsRedrawActiveStave,"Add Repeat End");
    CreateButton(":/Notes/Notes/repeatbegin.png",false,tsRedrawActiveStave,"Att Repeat Begin");
    CreateButton(":/Notes/Notes/volte.png",true,tsRedrawActiveStave,"Add Volta");
    return m_ButtonList;
}

void CRepeat::ModifyProperties(OCProperties* p)
{
    p->GetItem("Volta")->Hidden = false;
    p->GetItem("Repeats")->Hidden = false;

    switch (p->GetValue("RepeatType").toInt())
    {
    case 0:
        p->GetItem("Volta")->Hidden=true;
        break;
    case 1:
        p->GetItem("Volta")->Hidden=true;
        p->GetItem("Repeats")->Hidden=true;
        break;
    case 2:
        p->GetItem("Repeats")->Hidden=true;
        break;
    }
}

void CRepeat::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    int XMov=0;
    //if (GetVal(XMLSymbol,"Invisible")) return;
    if (Symbol.getVal("RepeatType")<2)
    {
        if ((Symbol.getVal("RepeatType") == 0) && ((BarCounter == 0) && (CountIt.Counter == 0))) return;
        if (CountIt.Counter == 0)
        {
            if (BarCounter != 0)
            {
                XMov = BarList.BegSpace(BarCounter, true, true, true);
            }
            else
            {
                XMov = BarList.BegSpace(BarCounter, true, 0, true);
            }
        }
        ScreenObj.DM(Symbol.moveX(XFysic - (16 * 12) - XMov), ScoreStaffHeight);
        QList<QGraphicsItem*> l;
        if ((sCurrent.Square == 1) || (sCurrent.Curly == 1))
        {
            l.append(ScreenObj.PlRect(24,-ScoreStaffHeight));
        }
        else
        {
            l.append(ScreenObj.PlRect(24,-ScoreStaffLinesHeight));
        }
        if (Symbol.getVal("RepeatType") == 0)
        {
            ScreenObj.DM(Symbol.moveX(XFysic - (18 * 12) - XMov), ScoreStaffHeight);
            if ((sCurrent.Square == 1) || (sCurrent.Curly == 1))
            {
                l.append(ScreenObj.DL(0, -ScoreStaffHeight));
                ScreenObj.DR(0, ScoreStaffHeight);
            }
            else
            {
                l.append(ScreenObj.DL(0, -ScoreStaffLinesHeight));
                ScreenObj.DR(0, ScoreStaffLinesHeight);
            }
            l.append(ScreenObj.plDot(-36,-156));
            l.append(ScreenObj.plDot(-36,-252));
            if (Symbol.getVal("Repeats") > 2)
            {
                ScreenObj.DM(XFysic - (24 * 12) - XMov, ScoreStaffHeight + 96, Symbol);
                l.append(ScreenObj.plLet(QString::number(Symbol.getVal("Repeats")) + "x", Symbol.size(), "times new roman", true, true, 156));
            }
            if (ScreenObj.canColor()) SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
        }
        else if (Symbol.getVal("RepeatType") == 1)
        {
            ScreenObj.DM(Symbol.moveX(XFysic - (12 * 12) - XMov), ScoreStaffHeight);
            if ((sCurrent.Square == 1) || (sCurrent.Curly == 1))
            {
                l.append(ScreenObj.DL(0, -ScoreStaffHeight));
                ScreenObj.DR(0, ScoreStaffHeight);
            }
            else
            {
                l.append(ScreenObj.DL(0, -ScoreStaffLinesHeight));
                ScreenObj.DR(0, ScoreStaffLinesHeight);
            }
            l.append(ScreenObj.plDot(36,-156));
            l.append(ScreenObj.plDot(36,-252));
            if (ScreenObj.canColor()) SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
        }
    }
    else if (Symbol.getVal("RepeatType") == 2)
    {
        ScreenObj.DM(XFysic - (12 * 12), 1680, Symbol);
        QList<QGraphicsItem*> l=ScreenObj.plLet(QString::number(Symbol.getVal("Volta")) + ".", Symbol.size(), "times new roman", true, true, 156);
        ScreenObj.DM(XFysic - (16 * 12), 1680, Symbol);
        l.append(ScreenObj.DL(0, 180));
        l.append(ScreenObj.DL(480, 0));
        SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
    }
}

void CRepeat::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    if (Symbol.getVal("RepeatType")==0)
    {
        d.EditWidget->PutSpin("Number of Repeats:",Symbol.getVal("Repeats"),1,999);
        esc=(d.exec()!=QDialog::Accepted);
        if (!esc) Symbol.setAttribute("Repeats",d.EditWidget->GetSpin());
    }
    else if (Symbol.getVal("RepeatType")==2)
    {
        d.EditWidget->PutSpin("Volta:",Symbol.getVal("Volta"),1,999);
        esc=(d.exec()!=QDialog::Accepted);
        if (!esc) Symbol.setAttribute("Volta",d.EditWidget->GetSpin());
    }
    RefreshMode = tsRedrawActiveStave;
}

void CRepeat::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    switch ((int)Symbol.getVal("RepeatType"))
    {
    case 0:
        if (Repeat.count() > 0)
        {
            TemPlay.RepeatFromStart = false;
            if (PlayRepeat == 0) PlayRepeat = Symbol.getVal("Repeats") - 1;
            PlayRepeat--;
            Py=Repeat.first();
            if (PlayRepeat<=0)
            {
                PlayRepeat=0;
                Repeat.removeFirst();
            }
        }
        else
        {
            if (TemPlay.RepeatFromStart)
            {
                TemPlay.RepeatFromStart = false;
                if (PlayRepeat == 0) PlayRepeat = Symbol.getVal("Repeats") - 1;
            }
            if (PlayRepeat > 0)
            {
                PlayRepeat--;
                Py = -1;
                CountIt.reset();
            }
        }
        break;
    case 1:
        Repeat.append(Py);
        break;
    case 2:
        TemPlay.Volta++;
        if (TemPlay.Volta > 1)
        {
            if (Symbol.getVal("Volta") == 1)
            {
                if (Repeat.count()==0) Repeat.append(-1);
                Py=XMLScoreWrapper::FindSymbol(XMLVoice,m_Name,Py+1,"RepeatType",2,"Volta",TemPlay.Volta);
                CountIt.reset();
            }
        }
        break;
    }
}

void CRepeat::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol,MFile,CountIt,Py,XMLVoice,SignsToPlay,TemPlay);
}

CSegno::CSegno() :CVisibleSymbol("Segno")
{
    m_PropColl->Add("SegnoType", pwList, "", "", "Returns/sets the Type of Segno Sign.", QStringList() << "dal Segno" << "Segno", 0,false, "", "Appearance");
    m_ButtonProperty="SegnoType";
}

QList<OCToolButtonProps*> CSegno::CreateButtons()
{
    CreateButton("Times new Roman",12,"D.S.",false,true,false,tsRedrawActiveStave,"Add dal Segno");
    CreateButton(":/Notes/Notes/segno.png");
    return m_ButtonList;
}

void CSegno::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol,MFile,CountIt,Py,XMLVoice,SignsToPlay,TemPlay);
}

void CSegno::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    if (!TemPlay.Fine)
    {
        if (Symbol.getVal("SegnoType") == 0)
        {
            Py=XMLScoreWrapper::FindSymbol(XMLVoice,m_Name,Py+1,"SegnoType",1);
            CountIt.reset();
            TemPlay.Fine = true;
        }
    }
}

void CSegno::PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType &tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    if (Symbol.getVal("SegnoType") == 1)
    {
        ScreenObj.DM(XFysic, ScoreTopSymbolY + 144+72, Symbol);
        l.append(ScreenObj.plLet(QChar(OCTTFSegno), Symbol.size(), OCTTFname, false, false, 1200, Qt::AlignHCenter));
    }
    else
    {
        ScreenObj.DM(XFysic, ScoreTopSymbolY + 12, Symbol);
        l.append(ScreenObj.plLet("DS", Symbol.size(), "times new roman", true, true, 156));
    }
    if (MTColorCheck(ScreenObj)) MTObj.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

void CSegno::fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.MasterStuff=true;
}

QStringList CSlant::SlantList=QStringList() << "Slanting" << "Straight";

CSlant::CSlant() :CSymbol("BeamSlant")
{
    m_PropColl->Add("BeamSlanting", pwList, "", "", "Returns or sets whether Slanted Beams are on.", SlantList, 0,false, "", "Behavior");
}

QList<OCToolButtonProps*> CSlant::CreateButtons()
{
    CreateButton(":/Notes/Notes/slant.png",true,tsRedrawActiveStave,"Add Beam Slant");
    return m_ButtonList;
}

void CSlant::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.SlantFlag = Symbol.getVal("BeamSlanting");
}

void CSlant::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value=(int)Symbol.getVal("BeamSlanting");
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutCombo("Beam Slanting",Value,SlantList);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("BeamSlanting",(bool)d.EditWidget->GetCombo());
    RefreshMode = tsRedrawActiveStave;
}

QStringList CDurSlur::SlurList=QStringList() << "Down" << "Up";

CDurSlur::CDurSlur() :CGapSymbol("Slur", "Curve", "Returns/sets the difference between the current Curve and the default Curve.")
{
    m_PropColl->Add("Direction", pwList,"","","Returns/sets the vertical Direction of the Slur",SlurList,0,false,"","Appearance");
    m_PropColl->Add("Angle", pwNumber, -32000, 32000, "Returns/sets the difference between the Endpoints current vertical Position and it's default vertical Position.", "", 0,false, "", "Appearance");
    m_ButtonProperty="Direction";
}

QList<OCToolButtonProps*> CDurSlur::CreateButtons()
{
    CreateButton(":/Notes/Notes/legato.png",false,tsRedrawActiveStave,"Add Slur Down");
    CreateButton(":/Notes/Notes/legatoup.png",false,tsRedrawActiveStave,"Add Slur Up");
    return m_ButtonList;
}

void CDurSlur::InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType &TempPlay)
{
    m_PropColl->SetValue("Ticks", Symbol.ticks());
    m_PropColl->SetValue("Direction", Symbol.getVal("Direction"));
}

void CDurSlur::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    Play(Symbol,MFile,CountIt,Py,XMLVoice,SignsToPlay,TemPlay);
}

void CDurSlur::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    CDurSlur* S = new CDurSlur;
    SignsToPlay.KillByName(m_Name);
    SignsToPlay.AppendPlay(Symbol.ticks(), 0, 0, S);
    S->InitPlaySymbol(Symbol, TemPlay);
}

void CDurSlur::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    if (Symbol.getVal("Direction")==1)
    {
        tempsetting.SlurUp=OCDurSignType(Symbol);
        tempsetting.SlurUp.val = Symbol.getVal("Angle");
        tempsetting.SlurUp.Size = Symbol.getVal("Curve");
    }
    else
    {
        tempsetting.SlurDown=OCDurSignType(Symbol);
        tempsetting.SlurDown.val = Symbol.getVal("Angle");
        tempsetting.SlurDown.Size = Symbol.getVal("Curve");
    }
}

void CDurSlur::AfterNote(XMLSymbolWrapper& XMLNote, OCPlayBackVarsType &TemPlay)
{
    m_PropColl->SetValue("Ticks", m_PropColl->GetValue("Ticks").toInt() - XMLNote.ticks());
}

void CDurSlur::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    int Direction=-1;
    if (Symbol.getVal("Direction")==1) Direction=1;
    NoteList.PlotSlur(NoteCount, Symbol.ticks(), Direction, Symbol.getVal("Angle"), Symbol.pos(), Symbol.getVal("Curve"), SymbolList, false, Pointer, ScreenObj);
}

CStopped::CStopped() : CVisibleSymbol("Stopped",false,true)
{
}

QList<OCToolButtonProps*> CStopped::CreateButtons()
{
    CreateButton("Times new Roman",20,"+",false,false);
    return m_ButtonList;
}

void CStopped::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    SignsToPrint.Append(OCTTFStopped,false, Symbol, SignCol, Pointer, new CStopped);
}

void CStopped::PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    SignProps.DMVertical(UpDown, 0, (18 + (SignsUp * 12)) * 12, 0, ScreenObj);
    QList<QGraphicsItem*> l=ScreenObj.plLet(MakeUnicode(QChar(SignProps.Sign)), SignProps.Size, OCTTFname, false, false, 1200, Qt::AlignHCenter);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),SignProps.Pointer);
}

QStringList CTempoChange::TempoChangeList=QStringList() << "a tempo" << "Ritardando" << "Accelerando";

CTempoChange::CTempoChange() :CVisibleSymbol("TempoChange")
{
    m_PropColl->Add("TempoType", pwList, "", "", "Returns/sets the Type of Tempo change.", TempoChangeList, 0,false, "", "Appearance");
    m_PropColl->Add("Speed", pwNumber, 1, 100, "Returns/sets the MIDI execution speed of the Tempo change.", "", 0,false, 50, "Behavior");
    m_ButtonProperty="TempoType";
}

QList<OCToolButtonProps*> CTempoChange::CreateButtons()
{
    CreateButton("Times new Roman",11,"a\ntempo",true,false,false,tsRedrawActiveStave,"Add a tempo");
    CreateButton("Times new Roman",13,"rit",true,false,false,tsRedrawActiveStave,"Add Ritardando");
    CreateButton("Times new Roman",13,"accel",true,false,false,tsRedrawActiveStave,"Add Accelerando");
    return m_ButtonList;
}

void CTempoChange::ModifyProperties(OCProperties* p)
{
    p->GetItem("Speed")->Hidden = (p->GetValue("TempoType").toInt() == 0);
}

OCProperties* CTempoChange::GetProperties(int Button)
{
    CSymbol::GetProperties(Button);
    switch (Button)
    {
    case 1:
        return OCPresets().SetPropertyValue(m_PropColl,"Speed","ritspeed");
    case 2:
        return OCPresets().SetPropertyValue(m_PropColl,"Speed","accel");
    }
    return m_PropColl;
}

void CTempoChange::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    fibPlay(Symbol,MFile,CountIt,Py,XMLVoice,SignsToPlay,TemPlay);
    MFile.SetTime(0);
    TemPlay.Currenttime = 0;
}

void CTempoChange::fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    CTempoChange* S = new CTempoChange;
    SignsToPlay.KillByName(m_Name);
    TemPlay.Playtempo = TemPlay.HoldTempo;
    MFile.Playtempo(TemPlay.Playtempo);
    if (Symbol.getVal("TempoType") == 0) //'a tempo
    {
        TemPlay.Accel = 0;
    }
    else if (Symbol.getVal("TempoType") == 1) //'rit
    {
        TemPlay.Accel = Symbol.getVal("Speed");
        int Modulate = IntDiv(TemPlay.Playtempo * 8, Symbol.getVal("Speed"));
        if (Modulate < 1) Modulate = 1;
        SignsToPlay.AppendPlay(NotDecrementable, 1, Modulate, S);
    }
    else if (Symbol.getVal("TempoType") == 2) //'accel
    {
        TemPlay.Accel = -Symbol.getVal("Speed");
        int Modulate = IntDiv(TemPlay.Playtempo * 8, Symbol.getVal("Speed"));
        if (Modulate < 1) Modulate = 1;
        SignsToPlay.AppendPlay(NotDecrementable, -1, Modulate, S);
    }
}

void CTempoChange::PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType &tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    ScreenObj.DM(XFysic, ScoreTempoY, Symbol);
    switch ((int)Symbol.getVal("TempoType"))
    {
    case 0:
        l.append(ScreenObj.plLet("a tempo", Symbol.size(), Score.TempoFont()));
        break;
    case 1:
        l.append(ScreenObj.plLet("rit", Symbol.size(), Score.TempoFont()));
        break;
    case 2:
        l.append(ScreenObj.plLet("accel", Symbol.size(), Score.TempoFont()));
        break;
    }
    if (MTColorCheck(ScreenObj)) MTObj.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
}

void CTempoChange::fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    tempsetting.MasterStuff=true;
}

void CTempoChange::DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int &LastTime, int Tick, int PlayTime, OCPlayBackVarsType &TemPlay)
{
    TemPlay.Playtempo = TemPlay.Playtempo - Props.Value;
    if (TemPlay.Playtempo < 20) TemPlay.Playtempo = 20;
    MFile.SetTime(LastTime);
    MFile.Playtempo(TemPlay.Playtempo);
    LastTime = 0;
}

CText::CText() :CTextSymbol("Text")
{
    m_PropColl->Add("Master", pwBoolean, "", "", "Sets/Returns whether the Text should behave as a MasterStaff item", "", 0, false, "", "Behavior");
}

QList<OCToolButtonProps*> CText::CreateButtons()
{
    CreateButton("Times new Roman",13,"Text",false,false,true);
    return m_ButtonList;
}

void CText::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    QString Text=Symbol.attribute("Text");
    QFont Font(Symbol.attribute("FontName"),Symbol.getVal("FontSize"));
    Font.setBold(Symbol.getVal("FontBold"));
    Font.setItalic(Symbol.getVal("FontItalic"));
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.ShowList("Text");
    d.EditWidget->PutText(Text,Font);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetText(Text,Font);
        Symbol.setAttribute("Text",Text);
        Symbol.setAttribute("FontName",Font.family());
        Symbol.setAttribute("FontSize",Font.pointSizeF());
        Symbol.setAttribute("FontBold",Font.bold());
        Symbol.setAttribute("FontItalic",Font.italic());
    }
    RefreshMode = tsRedrawActiveStave;
}

QList<QGraphicsItem*> CText::PlotText(XMLSymbolWrapper& Symbol, int XFysic, int Pointer, OCSymbolArray& SymbolList, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    if (Symbol.attribute("Text").length())
    {
        int Size = Symbol.getVal("FontSize");
        if (Size == 0) Size = 8;
        //ScreenObj.DM(XFysic, (44 * 12) + ScoreStaffHeight, XMLSymbol);
        ScreenObj.DM(XFysic,ScoreTopSymbolY,Symbol);
        l.append(ScreenObj.plLet(Symbol.attribute("Text"), Symbol.size(), Symbol.attribute("FontName"), Symbol.getVal("FontBold"), Symbol.getVal("FontItalic"), Size * 10));
    }
    return l;
}

void CText::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    if (!Symbol.getVal("Master"))
    {
        QList<QGraphicsItem*> l=PlotText(Symbol, XFysic, Pointer, SymbolList, ScreenObj);
        SymbolList.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
    }
}

void CText::PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType &tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj)
{
    if (Symbol.getVal("Master"))
    {
        QList<QGraphicsItem*> l=PlotText(Symbol, XFysic, Pointer, MTObj, ScreenObj);
        if (MTColorCheck(ScreenObj)) MTObj.AppendGroup(ScreenObj.MakeGroup(l),Pointer);
    }
}

void CText::fib(XMLSymbolWrapper &Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    if (Symbol.getVal("Master")) tempsetting.MasterStuff=true;
}

CTremolo::CTremolo() :CVisibleSymbol("Tremolo")
{
    m_PropColl->Add("Speed", pwNumber, 1, 200, "Returns/sets the MIDI execution speed of the Tremolo.", "", 0,false, 50, "Behavior");
    m_PropColl->Add("Beams", pwNumber, 1, 4, "Returns/sets the Tremolo Signs number of Beams.", "", 0,false, 3, "Appearance");
}

QList<OCToolButtonProps*> CTremolo::CreateButtons()
{
    CreateButton(":/Notes/Notes/tremolo.png");
    return m_ButtonList;
}

OCProperties* CTremolo::GetProperties(int Button)
{
    return OCPresets().SetPropertyValue(CSymbol::GetProperties(Button),"Speed","trillspeed");
}

void CTremolo::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    int Modulate = IntDiv(TemPlay.Playtempo * 30, Symbol.getVal("Speed"));
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.AppendPlay(0, 0, Modulate, new CTremolo);
}

void CTremolo::DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int &LastTime, int Tick, int PlayTime, OCPlayBackVarsType &TemPlay)
{
    MFile.Append(0x80 + TemPlay.MIDI.Channel, Pitch, TemPlay.Currentdynam, LastTime);
    MFile.Append(0x90 + TemPlay.MIDI.Channel, Pitch, TemPlay.Currentdynam, 0);
    LastTime = 0;
}

void CTremolo::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    SignsToPrint.Append(OCTTFTremolo0 + Symbol.getVal("Beams"), false, Symbol, SignCol, Pointer, new CTremolo);
}

void CTremolo::PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignProps.DM((12 * -4) * UpDown, 144 + ((12 * -14) * UpDown), ScreenObj);
    QList<QGraphicsItem*> l=ScreenObj.plLet(MakeUnicode(QChar(SignProps.Sign)), SignProps.Size, OCTTFname, false, false, 1200, Qt::AlignHCenter);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),SignProps.Pointer);
}

QStringList CTrill::TrillList=QStringList() << "tr" << "tr b" << "tr #";

CTrill::CTrill() :CVisibleSymbol("Trill")
{
    m_PropColl->Add("Speed", pwNumber, 1, 200, "Returns/sets the MIDI execution speed of the Trill.", "", 0,false, 50, "Behavior");
    m_PropColl->Add("Range", pwNumber, 1, 12, "Returns/sets the Range of the Trill in half tones.", "", 0,false, 2, "Behavior");
    m_PropColl->Add("StartFromAbove", pwBoolean, "", "", "Returns or sets whether the Trill starts from above.", "", 0,false, true, "Behavior");
    m_PropColl->Add("TrillType",pwList,"","","Returns/sets the Trill Sign displayed",TrillList,0,false,"","appearance");
    m_ButtonProperty="TrillType";
}

QList<OCToolButtonProps*> CTrill::CreateButtons()
{
    CreateButton(":/Notes/Notes/trill.png");
    CreateButton(":/Notes/Notes/trillb.png",false,tsRedrawActiveStave,"Add Trill b");
    CreateButton(":/Notes/Notes/trillx.png",false,tsRedrawActiveStave,"Add Trill #");
    return m_ButtonList;
}

void CTrill::InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType &TempPlay)
{
    int Direction = 1;
    if (Symbol.getVal("StartFromAbove")) Direction = -1;
    trilldynam = 0;
    TrillDir = Symbol.getVal("Range") * Direction;
    FinishedPlaying = false;
}

void CTrill::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int &Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay)
{
    CTrill* S = new CTrill;
    SignsToPlay.KillByName(m_Name);
    int Modulate = IntDiv(TemPlay.Playtempo * 30, Symbol.getVal("Speed"));
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.AppendPlay(0, 0, Modulate, S);
    S->InitPlaySymbol(Symbol, TemPlay);
}

void CTrill::plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType &dCurrent, OCPrintStaffVarsType & sCurrent, int Pointer, OCDraw& ScreenObj)
{
    CTrill* s=new CTrill;
    s->height=204;
    int Sign=OCTTFTrill;
    switch ((int)Symbol.getVal("TrillType"))
    {
    case 1:
        Sign=OCTTFTrillb;
        s->height=384;
        break;
    case 2:
        Sign=OCTTFTrillSharp;
        s->height=384;
        break;
    }
    SignsToPrint.Append(Sign, false, Symbol, SignCol, Pointer, s);
}

void CTrill::PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    SignProps.DMVertical(UpDown, 0, (15 + (SignsUp * 12)) * 12, (8 + (SignsUp * 12)) * 12, ScreenObj);
    QList<QGraphicsItem*> l=ScreenObj.plLet(MakeUnicode(QChar(SignProps.Sign)), SignProps.Size, OCTTFname, false, false, 1200);
    SymbolList.AppendGroup(ScreenObj.MakeGroup(l),SignProps.Pointer);
}

void CTrill::DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int &LastTime, int Tick, int PlayTime, OCPlayBackVarsType &TemPlay)
{
    if (CurrentPitch == BasePitch)
    {
        CurrentPitch = BasePitch + Abs(TrillDir);
    }
    else
    {
        CurrentPitch = BasePitch;
    }
    if (!FinishedPlaying)
    {
        if (TemPlay.PortamentoOn)
        {
            if ((PlayTime - Tick >= Props.Modulate) && (PlayTime - Tick <= Props.Modulate * 2) && (CurrentPitch == BasePitch))
            {
                MFile.Append(0x90 + TemPlay.MIDI.Channel, CurrentPitch, TemPlay.Currentdynam - trilldynam, LastTime);
                LastTime = 0;
                FinishedPlaying = true;
                MFile.Append(0x80 + TemPlay.MIDI.Channel, oldpitch, TemPlay.Currentdynam - trilldynam, 0);
            }
            else
            {
                MFile.Append(0x90 + TemPlay.MIDI.Channel, CurrentPitch, TemPlay.Currentdynam - trilldynam, LastTime);
                LastTime = 0;
                MFile.Append(0x80 + TemPlay.MIDI.Channel, oldpitch, TemPlay.Currentdynam - trilldynam, 0);
                if (PlayTime - Tick >= Props.Modulate)
                {
                    MFile.Append(0xB0 + TemPlay.MIDI.Channel, 0x54, CurrentPitch, 0);
                    trilldynam++;
                }
                else
                {
                    FinishedPlaying = true;
                }
            }
        }
        else
        {
            MFile.Append(0x80 + TemPlay.MIDI.Channel, oldpitch, TemPlay.Currentdynam - trilldynam, LastTime);
            MFile.Append(0x90 + TemPlay.MIDI.Channel, CurrentPitch, TemPlay.Currentdynam - trilldynam, 0);
            LastTime = 0;
            trilldynam++;
            if ((PlayTime - Tick < Props.Modulate * 2) && (CurrentPitch == BasePitch)) FinishedPlaying = true;
        }
    }
    oldpitch = CurrentPitch;
}

void CTrill::BeforeNote(XMLSymbolWrapper& XMLNote, int &PlayDynam, int &Pitch, int &endPitch, OCMIDIFile& MFile, OCPlayBackVarsType &TemPlay)
{
    BasePitch = Pitch;
    if (XMLNote.IsValuedNote())
    {
        if (TrillDir < 0) Pitch = Pitch - TrillDir;
    }
    CurrentPitch = Pitch;
    oldpitch = Pitch;
}

OCProperties* CTrill::GetProperties(int Button)
{
    CSymbol::GetProperties(Button);
    if (Button==1) m_PropColl->SetValue("Range",1);
    return OCPresets().SetPropertyValue(m_PropColl,"Speed","trillspeed");
}

QStringList CStemDirection::DirectionList=QStringList() << "Auto" << "Down" << "Up";

CStemDirection::CStemDirection() :CSymbol("StemDirection")
{
    m_PropColl->Add("Direction", pwList, "", "", "Returns/sets the Stem direction.", DirectionList, 0,false, "", "Behavior");
}

QList<OCToolButtonProps*> CStemDirection::CreateButtons()
{
    CreateButton(":/Notes/Notes/stemdirection.png",true,tsRedrawActiveStave,"Add Stem Direction");
    return m_ButtonList;
}

void CStemDirection::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(m_Name);
    d.EditWidget->PutCombo("Stem Direction",Symbol.getVal("Direction"),DirectionList);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("Direction",d.EditWidget->GetCombo());
    RefreshMode = tsRedrawActiveStave;
}

void CStemDirection::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting)
{
    int Direction = Symbol.getVal("Direction");
    if (Direction == 2) Direction = -1;
    tempsetting.UpDown = Direction;
}
