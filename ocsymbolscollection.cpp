#include "ocsymbolscollection.h"
#include "ceditdialog.h"

static OCProperties emptyproperties;

int OCSymbolsCollection::refCount=0;
QMap<QString,CSymbol*> OCSymbolsCollection::Symbols=QMap<QString,CSymbol*>();
QMap<QString, QList<OCToolButtonProps*> > OCSymbolsCollection::Buttons=QMap<QString, QList<OCToolButtonProps*> >();
QMap<QString, QList<QIcon> > OCSymbolsCollection::Icons=QMap<QString, QList<QIcon> >();
QStringList OCSymbolsCollection::Cats=QStringList();

OCSymbolsCollection::OCSymbolsCollection()
{
    if (refCount++==0)
    {
        Symbols.insert("DynamicChange", new CDynChange);
        Symbols.insert("Dynamic", new CDynamic);
        Symbols.insert("Accent", new CAccent);
        Symbols.insert("fp", new Cfp);
        Symbols.insert("fz", new Cfz);

        Symbols.insert("Tempo", new CTempo);
        Symbols.insert("TempoChange", new CTempoChange);
        Symbols.insert("Fermata", new CFermata);

        Symbols.insert("Transpose", new CTranspose);
        Symbols.insert("Octave", new COctave);

        Symbols.insert("Repeat", new CRepeat);
        Symbols.insert("Segno", new CSegno);
        Symbols.insert("Coda", new CCoda);
        Symbols.insert("DaCapo", new CDaCapo);
        Symbols.insert("Fine", new CFine);

        Symbols.insert("BeamLimit", new CLimit);
        Symbols.insert("StemDirection", new CStemDirection);
        Symbols.insert("BeamSlant", new CSlant);
        Symbols.insert("FlipTie", new CFlipTie);

        Symbols.insert("Length", new CLength);

        Symbols.insert("Clef", new CClef);
        Symbols.insert("Time", new CTime);
        Symbols.insert("Key", new CKey);
        Symbols.insert("Scale", new CScale);
        Symbols.insert("Cue", new CCue);
        Symbols.insert("BarWidth", new CBarWidth);
        Symbols.insert("Text", new CText);
        Symbols.insert("Pedal",new CPedal);

        Symbols.insert("Patch", new CPatch);
        Symbols.insert("Channel", new CChannel);
        Symbols.insert("SysEx", new CSysEx);
        Symbols.insert("Controller", new CController);
        Symbols.insert("Expression", new CExpression);
        Symbols.insert("Portamento", new CPortamento);

        Symbols.insert("Fingering", new CFingering);
        Symbols.insert("StringNumber", new CStringNumber);
        Symbols.insert("Stopped", new CStopped);
        Symbols.insert("Harmonic", new CHarmonic);
        Symbols.insert("Comma", new CComma);
        Symbols.insert("BartokPizz", new CBartokP);
        Symbols.insert("Bowing", new CBowing);
        Symbols.insert("Accidental", new CAccidental);
        Symbols.insert("Trill", new CTrill);
        Symbols.insert("Glissando", new CGliss);
        Symbols.insert("Tremolo", new CTremolo);
        Symbols.insert("Turn", new CDobbel);
        Symbols.insert("Mordent", new CMordent);

        Symbols.insert("Hairpin", new CHairpin);
        Symbols.insert("DuratedLength", new CDurLength);
        Symbols.insert("Slur", new CDurSlur);
        Symbols.insert("DuratedSlant", new CDurSlant);
        Symbols.insert("DuratedBeamDirection", new CDurUpDown);
        Symbols.insert("Tuplet", new CTuplet);
        Symbols.insert("Beam", new CBeam);

        //Symbols.insert("EndOfVoice", new CSymbol("EndOfVoice"));
        CNote* n = new CNote("Note");
        Symbols.insert("Note",n);
        n = new CNote("Rest");
        Symbols.insert("Rest",n);

        for (CSymbol* s : std::as_const(Symbols)) Buttons.insert(s->name(),s->CreateButtons());
        for (const CSymbol* s : std::as_const(Symbols))
        {
            QList<QIcon> l;
            for (int i=0;i<Buttons[s->name()].size();i++)
            {
                OCToolButtonProps* tbp=Buttons[s->name()][i];
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
                    painter.setFont(QFont(tbp->fontname,int(tbp->fontsize),tbp->fontbold,tbp->fontitalic));
                    painter.drawText(QRect(0,0,24,24),tbp->buttonText,QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
                    l.append(QIcon(pm));
                }
                if (!tbp->category.isEmpty())
                {
                    if (!Cats.contains(tbp->category)) Cats.append(tbp->category);
                }
            }
            Icons.insert(s->name(),l);
        }
    }
}

OCSymbolsCollection::~OCSymbolsCollection()
{
    if (--refCount==0)
    {
        for (QList<OCToolButtonProps*>& l : Buttons)
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

bool OCSymbolsCollection::editevent(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, QWidget* parent)
{
    bool Escape=false;
    /*
    If Extend.GetObj(SymbolName) Then
        Extend.thisobj.Edit XMLScore, RefreshMode, esc, Custom
    Else
    */
    CSymbol* s = Symbols[Symbol.name()];
    if (s != nullptr) s->Edit(Symbol, RefreshMode, Escape, parent);
    return !Escape;
}

OCGraphicsList OCSymbolsCollection::PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType &voiceVars, const XMLScoreWrapper& Score, OCDraw& ScreenObj)
{
    if ((Symbol.IsRestOrAnyNote()) || Symbol.IsTuplet()) return OCGraphicsList(); // 'tsnote To tstiedpolynote, tstuplet, tsend
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.PlotMTrack XFysic, XMLSymbol, stavedistance, voiceVars, ScreenObj, MTObj, Pointer
        Extend.thisobj.UpdatePrintVars XMLSymbol, TrackNum, voiceVars
    Else
    */
    CSymbol* s = Symbols[Symbol.name()];
    if (s != nullptr)
    {
        if (Symbol.isVisible()) return s->PlotMTrack(XFysic, Symbol, stavedistance, voiceVars, Score, ScreenObj);
    }
    return OCGraphicsList();
}

void OCSymbolsCollection::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType &voiceVars)
{
    if (Symbol.IsRestOrAnyNote()) return; // 'tsnote To tstiedpolynote
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.UpdatePrintVars XMLSymbol, TrackNum, fibset
    Else
    */
    CSymbol* s = Symbols[Symbol.name()];
    if (s != nullptr) s->fib(Symbol, voiceVars);
}

void OCSymbolsCollection::fibCommon(const XMLSymbolWrapper& Symbol, OCStaffCounterPrint& voiceVarsArray, const OCVoiceLocation& VoiceLocation)
{
    fib(Symbol,voiceVarsArray[VoiceLocation.Voice]);
    if (VoiceLocation.Voice == 0)
    {
        if (Symbol.isCommon())
        {
            for (int i = 1; i < voiceVarsArray.size(); i++)
            {
                fib(Symbol,voiceVarsArray[i]);
            }
        }
    }
}

void OCSymbolsCollection::Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &voiceVars)
{
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.Play XMLSymbol, MFIle, CountIt, Py, XMLVoice, SignsToPlay, voiceVars
    Else
    */
    CSymbol* s = Symbols[Symbol.name()];
    if (s != nullptr)
    {
        s->Play(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, voiceVars);
        s->fibPlay(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, voiceVars);
    }
}

OCGraphicsList OCSymbolsCollection::plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, const QColor& SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType &voiceVars, const XMLTemplateStaffWrapper &XMLTemplateStaff, OCDraw& ScreenObj)
{
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.Draw XMLSymbol, XFysic, BarList, CountIt, CountIt.Location.Bar, voiceVars.CurrentClef, SignsToPrint, SignCol, XMLScore, FactorX, BarsToPrint, PointerStart, Objects, Bracket, Tuborg, Stave, Track, NoteList, iiTemp, voiceVars, ScreenObj, Pointer
        Extend.thisobj.UpdatePrintVars XMLSymbol, 0, voiceVars
    Else
    */
    CSymbol* s = Symbols[Symbol.name()];
    if (s != nullptr)
    {
        if (Symbol.isVisible()) return s->plot(Symbol, XFysic, BarList, CountIt, SignsToPrint, SignCol, Score, NoteList, voiceVars, XMLTemplateStaff, ScreenObj);
    }
    return OCGraphicsList();
}

OCGraphicsList OCSymbolsCollection::plotRemaining(const OCDurSignType& Sign, OCNoteList& NoteList, OCDraw& ScreenObj)
{
    CSymbol* s = Symbols[Sign.XMLSymbol.name()];
    if (s != nullptr)
    {
        if (Sign.XMLSymbol.isVisible() && Sign.remains()) return s->plotRemaining(Sign, NoteList, ScreenObj);
    }
    return OCGraphicsList();
}

void OCSymbolsCollection::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, OCPrintVarsType& /*voiceVars*/, const OCBarSymbolLocation& Location)
{
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.Draw XMLSymbol, XFysic, BarList, CountIt, CountIt.Location.Bar, voiceVars.CurrentClef, SignsToPrint, SignCol, XMLScore, FactorX, BarsToPrint, PointerStart, Objects, Bracket, Tuborg, Stave, Track, NoteList, iiTemp, voiceVars, ScreenObj, Pointer
        Extend.thisobj.UpdatePrintVars XMLSymbol, 0, voiceVars
    Else
    */
    CSymbol* s = Symbols[Symbol.name()];
    if (s != nullptr)
    {
        if (Symbol.isVisible()) s->appendSign(Symbol, SignsToPrint, SignCol,Location);
    }
}

OCGraphicsList OCSymbolsCollection::plotSystemEnd(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, const QColor& SignCol, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType &voiceVars, const XMLTemplateStaffWrapper &XMLTemplateStaff, OCDraw& ScreenObj)
{
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.Draw XMLSymbol, XFysic, BarList, CountIt, CountIt.Location.Bar, voiceVars.CurrentClef, SignsToPrint, SignCol, XMLScore, FactorX, BarsToPrint, PointerStart, Objects, Bracket, Tuborg, Stave, Track, NoteList, iiTemp, voiceVars, ScreenObj, Pointer
        Extend.thisobj.UpdatePrintVars XMLSymbol, 0, voiceVars
    Else
    */
    CSymbol* s = Symbols[Symbol.name()];
    if (s != nullptr)
    {
        if (Symbol.isVisible()) return s->plotSystemEnd(Symbol, XFysic, BarList, CountIt, SignsToPrint, SignCol, Score, NoteList, voiceVars, XMLTemplateStaff, ScreenObj);
    }
    return OCGraphicsList();
}

void OCSymbolsCollection::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &voiceVars)
{
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.UpdatePlayBackVars XMLSymbol, MFIle, CountIt, Py, XMLVoice, SignsToPlay, voiceVars
    Else
    */
    CSymbol* s = Symbols[Symbol.name()];
    if (s != nullptr) s->fibPlay(Symbol, MFile, CountIt, Py, XMLVoice, SignsToPlay, voiceVars);
}

void OCSymbolsCollection::DrawFactor(const XMLSymbolWrapper& Symbol, OCCounter& Counter, const XMLTemplateWrapper& XMLTemplate, OCPageBarList& BarList, const XMLScoreWrapper& Score)
{
    /*
    If Extend.GetObj(GetSymbolName(XMLSymbol)) Then
        Extend.thisobj.DrawFactor XMLSymbol, Counter, XMLTemplate, BarList, AnythingElse, MinimumSet, ChangeKey, ChangeClef, Staff, Voice, LongestStaff, LongestVoice, Bar, Py, XMLScore
    Else
    */
    CSymbol* s = Symbols[Symbol.name()];
    if (s!=nullptr) s->DrawFactor(Symbol, Counter, XMLTemplate, BarList, Score);
}

OCProperties& OCSymbolsCollection::GetProperties(const XMLSimpleSymbolWrapper& Symbol)
{
    //OCProperties* Properties;
    /*
    If Extend.GetObj(SN) Then
        Set Properties = Extend.thisobj.Properties(XMLSymbol)
        Properties.Name = Extend.thisobj.Name
    Else
    */
    CSymbol* s = Symbols[Symbol.name()];
    if (s != nullptr) return s->fromXML(Symbol);
    //Properties=&emptyproperties;
    emptyproperties.Name=Symbol.name();
    return emptyproperties;
}

void OCSymbolsCollection::ChangeProperty(XMLSimpleSymbolWrapper &Symbol,const QString& Name,const QVariant& Value)
{
    if (PropetyExists(Symbol.name(),Name)) Symbol.setAttribute(Name,Value);
}

void OCSymbolsCollection::ChangeProperties(XMLSimpleSymbolWrapper &Symbol, const QStringList &Names, const QVariant &Value)
{
    for (const QString& n : Names) ChangeProperty(Symbol,n,Value);
}

void OCSymbolsCollection::ModifyProperties(OCProperties& Properties)
{
    CSymbol* s = Symbols[Properties.Name];
    if (s != nullptr) s->ModifyProperties(Properties);
}

bool OCSymbolsCollection::PropetyExists(const QString& SymbolName, const QString& PropertyName)
{
    const CSymbol* s = Symbols[SymbolName];
    if (s != nullptr) return s->PropertyExists(PropertyName);
    return false;
}

OCProperties* OCSymbolsCollection::GetDefaultProperties(const QString& SymbolName)
{
    /*
    If Extend.GetObj(SN) Then
        Set Properties = Extend.thisobj.Properties(XMLSymbol)
        Properties.Name = Extend.thisobj.Name
    Else
    */
    CSymbol* s = Symbols[SymbolName];
    if (s != nullptr) return s->GetDefaultProperties();
    OCProperties* Properties=&emptyproperties;
    Properties->Name=SymbolName;
    return Properties;
}

OCProperties* OCSymbolsCollection::GetDefaultProperties(const QString& SymbolName, const int Button)
{
    CSymbol* s = Symbols[SymbolName];
    if (s != nullptr) return s->GetDefaultProperties(Button);
    OCProperties* Properties=&emptyproperties;
    Properties->Name=SymbolName;
    return Properties;
}

XMLSimpleSymbolWrapper OCSymbolsCollection::GetDefaultSymbol(const QString& SymbolName)
{
    return GetDefaultProperties(SymbolName)->toXML();
}

XMLSimpleSymbolWrapper OCSymbolsCollection::GetDefaultSymbol(const QString& SymbolName, const int Button)
{
    return GetDefaultProperties(SymbolName,Button)->toXML();
}

OCToolButtonProps* OCSymbolsCollection::ButtonProps(const QString& SymbolName, const int Button)
{
    return Buttons[SymbolName][Button];
}

int OCSymbolsCollection::ButtonCount(const QString& SymbolName)
{
    return Buttons[SymbolName].size();
}

const QIcon OCSymbolsCollection::Icon(const QString& SymbolName, const int Button)
{
    QList<QIcon> &l=Icons[SymbolName];
    if (!l.empty()) return l[qMin(Button,l.size()-1)];
    return QIcon();
}

const QIcon OCSymbolsCollection::SymbolIcon(const XMLSimpleSymbolWrapper& Symbol) {
    const QString SymbolName = Symbol.name();
    const CSymbol* s = Symbols[SymbolName];
    int buttonIndex = 0;
    if (s) {
        const QString buttonProperty = s->buttonProperty();
        if (!buttonProperty.isEmpty()) buttonIndex = Symbol.getIntVal(buttonProperty);
    }
    return Icon(SymbolName,buttonIndex);
}


const QStringList CDynamic::DynamicList{"ppp", "pp", "p", "mp", "mf", "f", "ff", "fff"};

QList<OCToolButtonProps*> CDynamic::CreateButtons()
{
    for (int i=0;i<8;i++) CreateButton("Dynamics",":/Notes/Notes/"+DynamicList[i]+".png",false,tsRedrawActiveStave,"Add Dynamic "+DynamicList[i]);
    return m_ButtonList;
}

OCGraphicsList CDynamic::plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& /*BarList*/, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType& /*voiceVars*/, const XMLTemplateStaffWrapper& /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    QString a;
    int c=Symbol.getIntVal("DynamicSign");
    if (c==3)
    {
        a = QChar(uint(OCTTFmp));
    }
    else if (c==4)
    {
        a = QChar(uint(OCTTFmf));
    }
    else if (c<3)
    {
        for (int i = 1; i <= qAbs<int>(Symbol.getIntVal("DynamicSign") - 3); i++)
        {
            a += QChar(uint(OCTTFp));
        }
    }
    else if (c>4)
    {
        for (int i = 1; i <= Symbol.getIntVal("DynamicSign") - 4; i++)
        {
            a += QChar(uint(OCTTFf));
        }
    }
    ScreenObj.moveTo(XFysic, ScoreBottomSymbolY, Symbol);
    return ScreenObj.plLet(MakeUnicode(a),Symbol.size(),OCTTFname,false,false,1200,Qt::AlignRight | Qt::AlignBottom);
}

void CDynamic::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& voiceVars)
{
    SignsToPlay.remove("Hairpin");
    SignsToPlay.remove("DynamicChange");
    voiceVars.Currentdynam = Symbol.getIntVal("Velocity");
    voiceVars.crescendo = 0;
}

OCProperties* CDynamic::GetDefaultProperties(int Button)
{
    return OCPresets().SetPropertyValue(CSymbol::GetDefaultProperties(Button),"Velocity",DynamicList[Button]+"vel");
}

QStringList CPatch::PatchList[2]={};

QList<OCToolButtonProps*> CPatch::CreateButtons()
{
    CreateButton("MIDI",":/Notes/Notes/patch.png",true);
    return m_ButtonList;
}

void CPatch::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(name());
    d.EditWidget->PutCombo("Patch",Symbol.getIntVal("Patch")-1,PatchList[0]);
    d.QuickAccept();
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("Patch",d.EditWidget->GetCombo()+1);
    RefreshMode=tsRedrawActiveStave;
}

void CPatch::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    voiceVars.MIDI.Patch = Symbol.getIntVal("Patch") - 1;
}

void CPatch::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    voiceVars.MIDI.Patch = Symbol.getIntVal("Patch") - 1;
}

void CPatch::Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    MFile.appendPatchChangeEvent(voiceVars.MIDI.Channel, Symbol.getIntVal("Patch")-1);
    voiceVars.CurrentDelta=0;
}

const QStringList CTime::TimeList=QStringList{"Time Signature", "Common Time", "Cut Time"};



QList<OCToolButtonProps*> CTime::CreateButtons()
{
    CreateButton("Staff",":/Notes/Notes/cuttime.png",true,tsReformat,"Add Time Signature");
    return m_ButtonList;
}
/*
int CTime::GetTicks(const XMLSymbolWrapper& Symbol)
{
    switch (Symbol.getIntVal("TimeType"))
    {
    case 0:
        return 96 * Symbol.getIntVal("Upper") / Symbol.getIntVal("Lower");
    case 1:
    case 2:
        return 96;
    }
    return 96;
}
*/
int CTime::CalcBeamLimit(const XMLSymbolWrapper& Symbol) {
    int limit = 24;
    switch (Symbol.getIntVal("TimeType")) {
        case 0:
            limit = 4 * 6;
            if (Symbol.getIntVal("Lower") == 8) {
                if ((Symbol.getIntVal("Upper") % 3) == 0) limit = 6 * 6;
            }
            else if (Symbol.getIntVal("Lower") == 16) {
                if ((Symbol.getIntVal("Upper") % 3) == 0) limit = 3 * 6;
            }
            else if (Symbol.getIntVal("Lower") == 2) {
                limit = 8 * 6;
            }
            break;
        case 1:
            //C
            limit = 4 * 6;
            break;
        case 2:
            //Alla breve
            limit = 8 * 6;
            break;
    }
    return limit;
}

void CTime::DrawFactor(const XMLSymbolWrapper& Symbol, OCCounter& Counter, const XMLTemplateWrapper& /*XMLTemplate*/, OCPageBarList& BarList, const XMLScoreWrapper& /*Score*/)
{
    if (Symbol.isVisible())
    {
        if (Counter.isFirstBeat())
        {
            if (Symbol.getIntVal("TimeType") > 0)
            {
                BarList.setTimeInBegOfBar(Counter.barCount(),2);
            }
            else
            {
                float l=QString::number(Symbol.getIntVal("Upper")).length();
                float ll=QString::number(Symbol.getIntVal("Lower")).length();
                l = qMax<float>(ll,l);
                if (l>1) l*=0.75;
                BarList.setTimeInBegOfBar(Counter.barCount(),int(l*2));
            }
        }
    }
    else
    {
        BarList.setInvisibleMeter(Counter.barCount(),true);
    }
}

void CTime::ModifyProperties(OCProperties& p)
{
    p.hide("Upper", p.propertyValue("TimeType").toInt() > 0);
    p.hide("Lower", p.propertyValue("TimeType").toInt() > 0);
}

OCGraphicsList CTime::plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType &/*voiceVars*/, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    return PlTime(Symbol, (XFysic - ScreenObj.spaceX(BarList.paddingLeft(CountIt.barCount(), false, false, true) + 192)),ScreenObj, Qt::AlignLeft);
}

OCGraphicsList CTime::plotSystemEnd(const XMLSymbolWrapper& Symbol, double /*XFysic*/,OCPageBarList& BarList, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType &/*voiceVars*/, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    PlTime(Symbol, ScreenObj.spaceX(BarList.paddingRight(false,false,true))-24,ScreenObj,Qt::AlignLeft); //BarX-108
    return OCGraphicsList();
}

OCGraphicsList CTime::PlTime(const XMLSymbolWrapper& Symbol, double X, OCDraw& ScreenObj, Qt::Alignment Align)
{
    QPainterPath p;
    switch (Symbol.getIntVal("TimeType"))
    {
    case 2:
        ScreenObj.moveTo(X, ScoreStaffHeight-48, Symbol);
        ScreenObj.move(0,-86,Symbol.size());
        p=ScreenObj.TextPath(QChar(uint(OCTTFAllaBreve)), Symbol.size(), OCTTFname, false, false, 1200);
        p.translate(-p.boundingRect().width()/2,0);
        break;
    case 1:
        ScreenObj.moveTo(X, ScoreStaffHeight-48, Symbol);
        ScreenObj.move(0,-86,Symbol.size());
        p=ScreenObj.TextPath(QChar(uint(OCTTFFourFour)), Symbol.size(), OCTTFname, false, false, 1200);
        p.translate(-p.boundingRect().width()/2,0);
        break;
    case 0:
        ScreenObj.moveTo(X, ScoreStaffHeight-40, Symbol);
        p=ScreenObj.TextPath(MakeUnicode(Symbol.attribute("Upper")), Symbol.size(), OCTTFname, false, false, 1200);
        p.translate(-p.boundingRect().width()/2,0);
        ScreenObj.moveTo(X, ScoreStaffHeight-40, Symbol);
        ScreenObj.move(0,-192,Symbol.size());
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
    int Upper=Symbol.getIntVal("Upper");
    int Lower=Symbol.getIntVal("Lower");
    int TimeType=Symbol.getIntVal("TimeType");
    CEditDialog d(parent);
    d.setWindowTitle(name());
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

void CTime::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    voiceVars.Meter = OCCounter::calcTime(Symbol);
    voiceVars.BalkLimit = CalcBeamLimit(Symbol);
    switch (Symbol.getIntVal("TimeType"))
    {
    case 0:
        voiceVars.MeterText=QString::number(Symbol.getIntVal("Upper"))+"/"+QString::number(Symbol.getIntVal("Lower"));
    case 1:
        voiceVars.MeterText="C";
        break;
    case 2:
        voiceVars.MeterText="Alla breve";
        break;
    }
}

void CTime::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    voiceVars.PlayMeter = OCCounter::calcTime(Symbol);
    switch (Symbol.getIntVal("TimeType"))
    {
    case 0:
        MFile.appendTimeEvent(Symbol.getIntVal("Upper"), Symbol.getIntVal("Lower"));
        if (voiceVars.PlayMeter==0) voiceVars.PlayMeter = Symbol.getIntVal("Upper") * (96 / Symbol.getIntVal("Lower"));
        break;
    case 1:
        MFile.appendTimeEvent(4,4);
        break;
    case 2:
        MFile.appendTimeEvent(2,2);
        break;
    }
    if (voiceVars.PlayMeter==0) voiceVars.PlayMeter = 96;
    voiceVars.CurrentDelta = 0;
}



QList<OCToolButtonProps*> CTuplet::CreateButtons()
{
    CreateButton("Durated",":/Notes/Notes/tuplet.png",true);
    return m_ButtonList;
}

void CTuplet::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent)
{
    int Value=Symbol.getIntVal("TupletValue");
    CEditDialog d(parent);
    d.setWindowTitle(name());
    int Noteval = 0;
    int Dotted = 0;
    bool Triplet = false;
    XMLSimpleSymbolWrapper::ticksToNoteValue(Noteval,Dotted,Triplet,Value);
    d.EditWidget->PutNoteval(Noteval,Dotted,Triplet);
    //d.QuickAccept(true);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetNoteval(Noteval,Dotted,Triplet);
        Symbol.setAttribute("TupletValue",XMLSimpleSymbolWrapper::noteValueToTicks(Noteval,Dotted,Triplet));
    }
    RefreshMode = tsReformat;
}

OCGraphicsList CTuplet::plot(const XMLSymbolWrapper& Symbol, double /*XFysic*/, OCPageBarList& /*BarList*/, OCCounter& CountIt, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& NoteList, OCPrintVarsType& /*voiceVars*/, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    return OCNoteList::PlotTuplet(NoteList.CreateList(CountIt.RhythmObjectIndex, CountIt.TupletMax), CountIt.TupletCaption, Symbol.pos(), Symbol.size(), ScreenObj);
}



QList<OCToolButtonProps*> CClef::CreateButtons()
{
    CreateButton("Staff",":/Notes/Notes/Gclef.png",false,tsReformat,"Add Soprano Clef");
    CreateButton("Staff",":/Notes/Notes/Fclef.png",false,tsReformat,"Add Bass Clef");
    CreateButton("Staff",":/Notes/Notes/Cclef.png",false,tsReformat,"Add Alto Clef");
    CreateButton("Staff",":/Notes/Notes/Tclef.png",false,tsReformat,"Add Tenor Clef");
    CreateButton("Staff",":/Notes/Notes/neutralclef.png",false,tsReformat,"Add Percussion Clef");
    return m_ButtonList;
}

void CClef::DrawFactor(const XMLSymbolWrapper& Symbol, OCCounter& Counter, const XMLTemplateWrapper& /*XMLTemplate*/, OCPageBarList& BarList, const XMLScoreWrapper& /*Score*/)
{
    if (Symbol.isVisible())
    {
        if ((Counter.isFirstBeat()) && (!Counter.isFirstBar()))
        {
            if (BarList.clefInBegOfBar(Counter.barCount()) == 0) BarList.setClefInBegOfBar(Counter.barCount(), 4);
        }
    }
}

OCGraphicsList CClef::plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType &/*voiceVars*/, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    if (Symbol.isVisible())
    {
        double Left=300;
        int Sz=Symbol.size();
        if (CountIt.isFirstBeat())
        {
            Left = BarList.paddingLeft(CountIt.barCount(), true, true, true) + 60;
            if (!CountIt.isFirstBar())
            {
                Left += 48;
                if (BarList.keyInBegOfBar(CountIt.barCount())) Left -= 36;
            }
        }
        if (!CountIt.isFirstBeatOfFirstBar())
        {
            Sz-=3;
        }
        ScreenObj.moveTo(XFysic -ScreenObj.spaceX(Left + 96), 888, Symbol);
        return PlClef(Symbol.getIntVal("Clef") + 1, Sz,ScreenObj);
    }
    return OCGraphicsList();
}

OCGraphicsList CClef::plotSystemEnd(const XMLSymbolWrapper& Symbol, double /*XFysic*/,OCPageBarList& BarList, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType &/*voiceVars*/, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    ScreenObj.moveTo(ScreenObj.spaceX(BarList.paddingRight(true,true,true)), 888, Symbol);
    PlClef(Symbol.getIntVal("Clef") + 1, -3, ScreenObj);
    return OCGraphicsList();
}

OCGraphicsList CClef::PlClef(int Clef, int Size, OCDraw& ScreenObj)
{
    OCGraphicsList l;
#ifdef __Lelandfont
    ScreenObj.move(0,-36);
#endif
    switch (Clef)
    {
    case 1:
        ScreenObj.move(0,96);
#ifndef __Lelandfont
        ScreenObj.move(0,-96,Size);
        l.append(ScreenObj.plLet(OCTTFSopranoClef, Size, 624));
#else
        l.append(ScreenObj.plLet(LelandSopranoClef, Size));
#endif
        break;
    case 2:
        ScreenObj.move(0,288);
#ifndef __Lelandfont
        ScreenObj.move(0,-288,Size);
        l.append(ScreenObj.plLet(OCTTFBassClef, Size, 624));
#else
        l.append(ScreenObj.plLet(LelandBassClef, Size));
#endif
        break;
    case 3:
        ScreenObj.move(0,195);
#ifndef __Lelandfont
        ScreenObj.move(0,-192,Size);
        l.append(ScreenObj.plLet(OCTTFAltoClef, Size, 624));
#else
        ScreenObj.move(0,-4);
        l.append(ScreenObj.plLet(LelandAltoClef, Size));
#endif
        break;
    case 4:
        ScreenObj.move(0,96);
        ScreenObj.move(0,195);
#ifndef __Lelandfont
        ScreenObj.move(0,-192,Size);
        l.append(ScreenObj.plLet(OCTTFAltoClef, Size, 624));
#else
        ScreenObj.move(0,-4);
        l.append(ScreenObj.plLet(LelandAltoClef, Size));
#endif
        break;
    case 5:
        ScreenObj.move(0,192);
#ifndef __Lelandfont
        ScreenObj.move(0,-192,Size);
        l.append(ScreenObj.plLet(OCTTFPercussionClef, Size, 624));
#else
        l.append(ScreenObj.plLet(LelandPercussionClef, Size));
#endif
        break;
    }
    return l;
}

void CClef::Edit(XMLSimpleSymbolWrapper& /*Symbol*/, OCRefreshMode& RefreshMode, bool& esc, QWidget* /*parent*/)
{
    esc=false;
    RefreshMode=tsReformat;
}

void CClef::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    voiceVars.setClef(Symbol);
}



QList<OCToolButtonProps*> CTranspose::CreateButtons()
{
    CreateButton("Staff",":/Notes/Notes/transpose.png",true);
    return m_ButtonList;
}

void CTranspose::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent)
{
    int Value=Symbol.getIntVal("Transpose");
    CEditDialog d(parent);
    d.setWindowTitle(name());
    //d.EditWidget->PutSpin("Transpose:",Value,-48,48);
    QStringList l;
    for (int i = -48; i <= 48; i++) l.append(QString::number(i));
    d.EditWidget->PutCombo("Transpose",Value+48,l);
    d.QuickAccept();
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        Value = d.EditWidget->GetCombo();
        Symbol.setAttribute("Transpose",Value-48);
    }
    RefreshMode = tsRedrawActiveStave;
}

void CTranspose::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    voiceVars.MIDI.Transpose = Symbol.getIntVal("Transpose");
}

void CTranspose::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    voiceVars.MIDI.Transpose = Symbol.getIntVal("Transpose");
}

const QStringList CKey::KeyList=QStringList{"Gb   eb", "Db   bb", "Ab   f", "Eb   c", "Bb   g", "F    d", "C    a", "G    e", "D    b", "A    f#", "E    c#", "B    g#", "F#   d#"};



QList<OCToolButtonProps*> CKey::CreateButtons()
{
    CreateButton("Staff",":/Notes/Notes/keysigflat.png",true,tsReformat,"Add Key Signature");
    return m_ButtonList;
}

void CKey::DrawFactor(const XMLSymbolWrapper& Symbol, OCCounter& Counter, const XMLTemplateWrapper& /*XMLTemplate*/, OCPageBarList& BarList, const XMLScoreWrapper& /*Score*/)
{
    if (Symbol.isVisible())
    {
        auto NumOfSigns = int(NumOfAccidentals(Symbol.getIntVal("Key") - 6));
        if ((NumOfSigns > BarList.keyInBegOfBar(Counter.barCount())) && (Counter.isFirstBeat())) BarList.setKeyInBegOfBar(Counter.barCount(), NumOfSigns);
    }
}

uint CKey::NumOfAccidentals(int Key)
{
    return uint(qAbs<int>(Key));
}

OCKeyAccidental CKey::AccidentalFlag(int Key)
{
    return OCKeyAccidental(-Sgn<int>(Key));
}

OCGraphicsList CKey::plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType &voiceVars, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    OCGraphicsList l=plotKey(Symbol.getIntVal("Key")-6,Symbol.move(XFysic - ScreenObj.spaceX(BarList.paddingLeft(CountIt.barCount(), true, false, true) + 180),0),voiceVars.clef(),ScreenObj);
    if ((CountIt.isFirstBeat()) && (!CountIt.isFirstBar()))
    {
        ScreenObj.moveTo(XFysic - ScreenObj.spaceX(BarList.paddingLeft(CountIt.barCount(), true, true, true) + 216), ScoreStaffHeight);
        l.append(ScreenObj.line(0,-ScoreStaffLinesHeight));
    }
    return l;
}

OCGraphicsList CKey::plotSystemEnd(const XMLSymbolWrapper& Symbol, double /*XFysic*/,OCPageBarList& BarList, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType &voiceVars, const XMLTemplateStaffWrapper& /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    plotKey(Symbol.getIntVal("Key")-6, Symbol.move(ScreenObj.spaceX(BarList.paddingRight(true, false, true))-(12*4),0),voiceVars.clef(),ScreenObj);
    ScreenObj.moveTo(ScreenObj.spaceX(BarList.systemLength()-(LineHalfThickNess*6)), ScoreStaffHeight);
    ScreenObj.line(0,-ScoreStaffLinesHeight);
    return OCGraphicsList();
}

void CKey::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent)
{
    int Value=Symbol.getIntVal("Key");
    CEditDialog d(parent);
    d.setWindowTitle(name());
    d.EditWidget->PutCombo("Key Signature",Value,KeyList);
    d.QuickAccept();
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        Value=d.EditWidget->GetCombo();
        Symbol.setAttribute("Key",Value);
    }
    RefreshMode = tsReformat;
}

void CKey::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    voiceVars.setKey(Symbol);
    std::array<int,12> s = {{0}};
    if (voiceVars.key() < 0)
    {
        s = {{0,2,0,0,0,0,2,0,0,0,0,0}};
    }
    if (voiceVars.key() > 0)
    {
        s = {{0,0,0,1,0,0,0,0,1,0,1,0}};
    }
    voiceVars.Scale=s;
}

OCGraphicsList CKey::plotKey(int Key, QPointF Pos, int CurrentClef, OCDraw& ScreenObj)
{
    OCKeyAccidental KBFlagFaste=AccidentalFlag(Key);
    uint AntalFasteFortegn=NumOfAccidentals(Key);
    std::array<int,7> Acc={{99 ,87 ,103 ,91 ,79 ,95 ,83}};
    OCGraphicsList l;
    if (AntalFasteFortegn == 0) return l;
    if (KBFlagFaste == keyAccSharps)
    {
        switch (CurrentClef)
        {
        case 0:
        case 1:
            Acc = {{99 ,87 ,103 ,91 ,79 ,95 ,83}};
            break;
        case 2:
            Acc = {{91 ,79 ,95 ,83 ,99 ,87 ,103}};
            break;
        case 3:
            Acc = {{95 ,83 ,99 ,87 ,103 ,91 ,79}};
            break;
        case 4:
            Acc = {{103 ,91 ,79 ,95 ,83 ,99 ,87}};
            break;
        }
    }
    else if (KBFlagFaste == keyAccFlats)
    {
        switch (CurrentClef)
        {
        case 0:
        case 1:
            Acc = {{83 ,95 ,79 ,91 ,75 ,87 ,71}};
            break;
        case 2:
            Acc = {{75 ,87 ,71 ,83 ,67 ,79 ,63}};
            break;
        case 3:
            Acc = {{79 ,91 ,75 ,87 ,71 ,83 ,67}};
            break;
        case 4:
            Acc = {{87 ,99 ,83 ,95 ,79 ,91 ,75}};
            break;
        }
    }
    for (uint i = 0; i < AntalFasteFortegn; i++)
    {
        ScreenObj.moveTo(Pos.x() + (i * AccidentalSpace) , Pos.y() + (Acc[i] * 12) + 162);
        if (KBFlagFaste == 1)
        {
#ifndef __Lelandfont
            l.append(ScreenObj.plLet(OCTTFFlat, 0));
#else
            ScreenObj.move(-12,-118);
            l.append(ScreenObj.plLet(LelandFlat, 0));
#endif
        }
        else
        {
#ifndef __Lelandfont
            l.append(ScreenObj.plLet(OCTTFSharp, 0));
#else
            ScreenObj.move(-12,-118);
            l.append(ScreenObj.plLet(LelandSharp, 0));
#endif
        }
    }
    return l;
}

void CKey::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    MFile.appendKeyEvent(Symbol.getIntVal("Key") - 6);
    voiceVars.CurrentDelta=0;
}



QList<OCToolButtonProps*> CScale::CreateButtons()
{
    CreateButton("Staff",":/Notes/Notes/scale.png",true);
    return m_ButtonList;
}

void CScale::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Keys[12];
    for (int i=0;i<12;i++)
    {
        Keys[i] = Symbol.getIntVal("Step"+QString::number(i+1));
    }
    CEditDialog d(parent);
    d.setWindowTitle(name());
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

void CScale::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    for (unsigned int i=0;i<voiceVars.Scale.size();i++)
    {
        voiceVars.Scale[i] = OCNoteAccidentalTypes(Symbol.getIntVal("Step"+QString::number(i+1)));
    }
}



QList<OCToolButtonProps*> CTempo::CreateButtons()
{
    CreateButton("Tempo",":/Notes/Notes/tempo.png",true);
    return m_ButtonList;
}

void CTempo::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value = Symbol.getIntVal("Tempo");
    int ListValue = Symbol.noteValue();
    bool Dotted = Symbol.dotted();
    CEditDialog d(parent);
    d.setWindowTitle(name());
    d.EditWidget->PutTempo(Value,ListValue,Dotted);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetTempo(Value,ListValue,Dotted);
        Symbol.setAttribute("Tempo",Value);
        Symbol.setNoteValue(ListValue);
        Symbol.setDotted(Dotted);
    }
    RefreshMode = tsRedrawActiveStave;
}

void CTempo::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &voiceVars)
{
    SignsToPlay.remove("TempoChange");
    if (Symbol.noteValue() == 0)
    {
        voiceVars.Playtempo = Symbol.getIntVal("Tempo");
    }
    else
    {
        voiceVars.Playtempo = int((Symbol.getIntVal("Tempo") * XMLSimpleSymbolWrapper::noteValueToTicks(Symbol.noteValue(), Symbol.dotted(),false)) / 24);
    }
    voiceVars.HoldTempo = voiceVars.Playtempo;
    MFile.appendTempoEvent(voiceVars.Playtempo);
    voiceVars.CurrentDelta = 0;
    voiceVars.Accel = 0;
}

OCGraphicsList CTempo::PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int /*stavedistance*/, OCPrintVarsType& /*voiceVars*/, const XMLScoreWrapper& Score, OCDraw& ScreenObj)
{
    ScreenObj.moveTo(XFysic, ScoreTempoY, Symbol);
    return PlTempo(Symbol.getIntVal("Tempo"),Symbol.noteValue(), Symbol.dotted(),Score.TempoFont.font(),Symbol.size(),ScreenObj);
}

void CTempo::fib(const XMLSymbolWrapper& /*Symbol*/, OCPrintVarsType& voiceVars)
{
    voiceVars.MasterStuff=true;
}

OCGraphicsList CTempo::PlTempo(int Tempo, int NoteVal, bool Dotted, const QFont& Font, int Size, OCDraw& ScreenObj)
{
    OCGraphicsList l;
    QPainterPath p(QPointF(0,0));
    switch (NoteVal)
    {
    case 0:
        p.addText(0,0,QFont(OCTTFname,400),QChar(uint(OCTTFNoteWhole)));
        break;
    case 1:
        p.addRoundedRect(22,-6,6,-160,3,3);
        p.addText(0,0,QFont(OCTTFname,400),QChar(uint(OCTTFNoteHalf)));
        break;
    default:
        p.addRoundedRect(22,-6,6,-160,3,3);
        p.addText(0,0,QFont(OCTTFname,400),QChar(uint(OCTTFNoteQuarter)));
    }
    switch (NoteVal)
    {
    case 3:
        p.addPath(OCNoteList::FanPath(168,StemUp,1).translated(28,-160));
        break;
    case 4:
        p.addPath(OCNoteList::FanPath(168,StemUp,2).translated(28,-160));
        break;
    }
    if (Dotted) p.addText(30,18,QFont("Courier new",100),".");
    p.addText(56,18,Font,"=" + QString::number(Tempo));
    p.translate(-p.boundingRect().width(),0);
    double f=(1.30/ScreenObj.ScreenSize)/SizeFactor(Size);
    QTransform m;
    m.scale(f,f);
    p=p*m;
    p.setFillRule(Qt::WindingFill);
    ScreenObj.translatePath(p);
    l.append(ScreenObj.plTextPath(p));
    return l;
}



QList<OCToolButtonProps*> CSysEx::CreateButtons()
{
    CreateButton("MIDI",":/Notes/Notes/sysex.png",true);
    return m_ButtonList;
}

void CSysEx::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    QString syx;
    syx=Symbol.attribute("SysExString");
    CEditDialog d(parent);
    d.setWindowTitle(name());
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

void CSysEx::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    QString syx = Symbol.attribute("SysExString");
    if (syx.length())
    {
        QByteArray Buffer;
        for (int iTemp = 0; iTemp<syx.length(); iTemp+=2)
        {
            Buffer += char(QString(QStringLiteral("0x") + syx.mid(iTemp,2)).toInt());
        }
        MFile.appendSysExEvent(Buffer);
        voiceVars.CurrentDelta = 0;
    }
}

QStringList CController::ControllerList=QStringList();



QList<OCToolButtonProps*> CController::CreateButtons()
{
    CreateButton("MIDI",":/Notes/Notes/controller.png",true);
    return m_ButtonList;
}

void CController::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value1=Symbol.getIntVal("Controller");
    int Value2=Symbol.getIntVal("Value");
    CEditDialog d(parent);
    d.setWindowTitle(name());
    d.ShowList("Controller");
    d.EditWidget->PutController(Value1,Value2);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetController(Value1,Value2);
        Symbol.setAttribute("Controller",Value1);
        Symbol.setAttribute("Value",Value2);
    }
    RefreshMode = tsRedrawActiveStave;
}

void CController::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    MFile.appendControllerEvent(voiceVars.MIDI.Channel, Symbol.getIntVal("Controller"), Symbol.getIntVal("Value"));
    voiceVars.CurrentDelta = 0;
}


QList<OCToolButtonProps*> CChannel::CreateButtons()
{
    CreateButton("MIDI",":/Notes/Notes/channel.png",true);
    return m_ButtonList;
}

void CChannel::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value=Symbol.getIntVal("Channel");
    CEditDialog d(parent);
    d.setWindowTitle(name());
    QStringList l;
    for (int i = 1; i <= 16; i++) l.append(QString::number(i));
    d.EditWidget->PutCombo("Channel",Value-1,l);
    d.QuickAccept();
    //d.EditWidget->PutSpin("Channel",Value,1,16);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        Value=d.EditWidget->GetCombo();
        Symbol.setAttribute("Channel",Value+1);
    }
    RefreshMode = tsRedrawActiveStave;
}

void CChannel::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    voiceVars.MIDI.Channel = Symbol.getIntVal("Channel") - 1;
    voiceVars.MIDI.Patch = 0;
}

void CChannel::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    voiceVars.MIDI.Channel = Symbol.getIntVal("Channel") - 1;
}



QList<OCToolButtonProps*> CAccent::CreateButtons()
{
    CreateButton("Dynamics","Times new Roman",15,">",false,false);
    return m_ButtonList;
}

void CAccent::BeforeNote(const XMLSymbolWrapper& /*XMLNote*/, int& PlayDynam, int& /*Pitch*/, int& /*endPitch*/, OCPlayBackVarsType &/*voiceVars*/)
{
    PlayDynam += XMLIntValue("AddToVelocity");
}

void CAccent::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CAccent(Symbol));
}

void CAccent::Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &/*voiceVars*/)
{
    SignsToPlay.append(KillInstantly, 0, new CAccent(Symbol));
}

OCGraphicsList CAccent::PrintSign(StemDirection UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    if (UpDown == 1) SignsUp ++;
    PrintProps.moveTo(0, (12 * 12) + (UpDown * 14 * 12), ScreenObj);
    return ScreenObj.plLet(OCTTFAccent, PrintProps.size(), 1200, Qt::AlignHCenter);
}

OCProperties* CAccent::GetDefaultProperties(int Button)
{
    return OCPresets().SetPropertyValue(CSymbol::GetDefaultProperties(Button),"AddToVelocity","AccentAdd");
}



QList<OCToolButtonProps*> Cfp::CreateButtons()
{
    CreateButton("Dynamics",":/Notes/Notes/fp.png");
    return m_ButtonList;
}

OCGraphicsList Cfp::plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& /*BarList*/, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType &/*voiceVars*/, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    ScreenObj.moveTo(XFysic - (16 * 12), ScoreBottomSymbolY,Symbol);
    return ScreenObj.plLet(MakeUnicode("fp"), Symbol.size(), OCTTFname, false, false, 1200, Qt::AlignBottom);
}

void Cfp::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    voiceVars.crescendo = 0;
    voiceVars.Currentdynam = Symbol.getIntVal("EndVelocity");
}

void Cfp::Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &voiceVars)
{
    SignsToPlay.remove("Hairpin");
    SignsToPlay.remove("Dynamic Change");
    voiceVars.crescendo = 0;
    voiceVars.Currentdynam = Symbol.getIntVal("EndVelocity");
    int FPgap = Symbol.getIntVal("StartVelocity") - Symbol.getIntVal("EndVelocity");

    int Modulate=voiceVars.Playtempo;
    if (FPgap != 0) Modulate = voiceVars.Playtempo / FPgap;
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.append(KillInstantly, Modulate, new Cfp(Symbol));
}

void Cfp::DuringNote(OCMIDIFile& MFile, int /*Pitch*/, int &LastTime, int Tick, int /*PlayTime*/, OCPlayBackVarsType &voiceVars)
{
    if (Tick <= voiceVars.Playtempo / 2) return;
    if (XMLIntValue("StartVelocity") - XMLIntValue("EndVelocity") > FPcount)
    {
        voiceVars.express = qBound<int>(0, voiceVars.express - 1, 127);
        MFile.appendExpressionEvent(voiceVars.MIDI.Channel, voiceVars.express, LastTime);
        LastTime = 0;
        voiceVars.changeexp = 1;
        FPcount++;
    }
}

void Cfp::BeforeNote(const XMLSymbolWrapper& /*XMLNote*/, int& PlayDynam, int &/*Pitch*/, int &/*endPitch*/, OCPlayBackVarsType &/*voiceVars*/)
{
    FPcount = 0;
    PlayDynam = XMLIntValue("StartVelocity");
}

OCProperties* Cfp::GetDefaultProperties(int Button)
{
    CSymbol::GetDefaultProperties(Button);
    OCPresets ps;
    m_PropColl.setPropertyValue("StartVelocity",ps.GetValue("fpstartvel"));
    m_PropColl.setPropertyValue("EndVelocity",ps.GetValue("fpendvel"));
    return &m_PropColl;
}



QList<OCToolButtonProps*> Cfz::CreateButtons()
{
    CreateButton("Dynamics",":/Notes/Notes/fz.png");
    return m_ButtonList;
}

OCGraphicsList Cfz::plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& /*BarList*/, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType &/*voiceVars*/, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    ScreenObj.moveTo(XFysic - (16 * 12), ScoreBottomSymbolY,Symbol);
    return ScreenObj.plLet(MakeUnicode("Z"), Symbol.size(), OCTTFname, false, false, 1200, Qt::AlignBottom);
}

void Cfz::Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &voiceVars)
{
    int FPgap = Symbol.getIntVal("AddToVelocity");

    int Modulate=voiceVars.Playtempo;
    if (FPgap != 0) Modulate = voiceVars.Playtempo / FPgap;
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.append(KillInstantly, Modulate, new Cfz(Symbol));
}

void Cfz::DuringNote(OCMIDIFile& MFile, int /*Pitch*/, int &LastTime, int Tick, int /*PlayTime*/, OCPlayBackVarsType &voiceVars)
{
    if (Tick <= voiceVars.Playtempo / 2) return;
    if (XMLIntValue("AddToVelocity") > FPcount)
    {
        voiceVars.express = qBound<int>(0, voiceVars.express - 1, 127);
        MFile.appendExpressionEvent(voiceVars.MIDI.Channel, voiceVars.express, LastTime);
        LastTime = 0;
        voiceVars.changeexp = 1;
        FPcount++;
    }
}

void Cfz::BeforeNote(const XMLSymbolWrapper& /*XMLNote*/, int& PlayDynam, int &/*Pitch*/, int &/*endPitch*/, OCPlayBackVarsType &/*voiceVars*/)
{
    FPcount = 0;
    PlayDynam += XMLIntValue("AddToVelocity");
}

OCProperties* Cfz::GetDefaultProperties(int Button)
{
    return OCPresets().SetPropertyValue(CSymbol::GetDefaultProperties(Button),"AddToVelocity","fzadd");
}



QList<OCToolButtonProps*> CAccidental::CreateButtons()
{
    CreateButton("Note",":/Notes/Notes/flat.png",false,tsRedrawActiveStave,"Add Flat","Parentheses");
    CreateButton("Note",":/Notes/Notes/sharp.png",false,tsRedrawActiveStave,"Add Sharp","Parentheses");
    CreateButton("Note",":/Notes/Notes/doubleflat.png",false,tsRedrawActiveStave,"Add Double Flat","Parentheses");
    CreateButton("Note",":/Notes/Notes/doublesharp.png",false,tsRedrawActiveStave,"Add Double Sharp","Parentheses");
    CreateButton("Note",":/Notes/Notes/natural.png",false,tsRedrawActiveStave,"Add Natural","Parentheses");
    CreateButton("Note","Times new Roman",15,"( )",false,false,false,tsRedrawActiveStave,"Add Parentheses to Accidentals","modifierParentheses");
    return m_ButtonList;
}

void CAccidental::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CAccidental(Symbol));
}

OCGraphicsList CAccidental::plLeftParanthesis(OCDraw& ScreenObj)
{
    OCGraphicsList l;
    ScreenObj.move(-132,-150);
    l.append(ScreenObj.plLet("(",0,"Arial",false,false,156));
    ScreenObj.move(72,150);
    return l;
}

OCGraphicsList CAccidental::PrintSign(StemDirection /*UpDown*/, int &/*SignsUp*/, OCDraw& ScreenObj)
{
    PrintProps.moveTo(ScreenObj);
    OCGraphicsList l;
    const bool Paranthsis = XMLBoolValue("Parentheses");
    switch (XMLIntValue("AccidentalSign"))
    {
    case 0:
        ScreenObj.move(-11 * 12, 150);
        if (Paranthsis) l.append(plLeftParanthesis(ScreenObj));
#ifndef __Lelandfont
        l.append(ScreenObj.plLet(OCTTFFlat, PrintProps.size()));
#else
        ScreenObj.move(-12,-118);
        l.append(ScreenObj.plLet(LelandFlat, PrintProps.size()));
#endif
        break;
    case 1:
        ScreenObj.move(-12 * 12, 150);
        if (Paranthsis) l.append(plLeftParanthesis(ScreenObj));
#ifndef __Lelandfont
        l.append(ScreenObj.plLet(OCTTFSharp, PrintProps.size()));
#else
        ScreenObj.move(-12,-118);
        l.append(ScreenObj.plLet(LelandSharp, PrintProps.size()));
#endif
        break;
    case 2:
        ScreenObj.move(-17 * 12, 150);
        if (Paranthsis) l.append(plLeftParanthesis(ScreenObj));
        l.append(ScreenObj.plLet(OCTTFDoubleFlat, PrintProps.size()));
        break;
    case 3:
        ScreenObj.move(-14 * 12, 150);
        if (Paranthsis) l.append(plLeftParanthesis(ScreenObj));
        l.append(ScreenObj.plLet(OCTTFDoubleSharp, PrintProps.size()));
        break;
    case 4:
        ScreenObj.move(-12 * 12, 150);
        if (Paranthsis) l.append(plLeftParanthesis(ScreenObj));
        l.append(ScreenObj.plLet(OCTTFOpl, PrintProps.size()));
        break;
    }
    if (Paranthsis)
    {
        PrintProps.moveTo(ScreenObj);
        ScreenObj.move(-144,0);
        l.append(ScreenObj.plLet(")",0,"Arial",false,false,156));
    }
    return l;
}



QList<OCToolButtonProps*> CBowing::CreateButtons()
{
    CreateButton("Note",":/Notes/Notes/upbow.png",false,tsRedrawActiveStave,"Add Up Bow");
    CreateButton("Note",":/Notes/Notes/downbow.png",false,tsRedrawActiveStave,"Add Down Bow");
    return m_ButtonList;
}

void CBowing::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CBowing(Symbol));
}

OCGraphicsList CBowing::PrintSign(StemDirection UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    PrintProps.moveToVertical(UpDown, (18 + (SignsUp * 12)) * 12, ScreenObj);
    return ScreenObj.plLet(MakeUnicode(QChar(OCTTFBowing0 + XMLIntValue("Bowing") + 1)), PrintProps.size(), OCTTFname, false, false, 1200, Qt::AlignHCenter);
}



QList<OCToolButtonProps*> CBartokP::CreateButtons()
{
    CreateButton("Note",":/Notes/Notes/bartok.png",false,tsRedrawActiveStave,"Add Bartok Pizz");
    return m_ButtonList;
}

void CBartokP::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CBartokP(Symbol));
}

OCGraphicsList CBartokP::PrintSign(StemDirection UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    PrintProps.moveToVertical(UpDown, (18 + (SignsUp * 12)) * 12, ScreenObj);
    return ScreenObj.plLet(MakeUnicode(QChar(uint(OCTTFBartok))), PrintProps.size(), OCTTFname, false, false, 1200, Qt::AlignHCenter);
}

const QStringList CFingering::FingerList = {"0","1","2","3","4","5","+"};

QList<OCToolButtonProps*> CFingering::CreateButtons()
{
    for (int i=0;i<7;i++) CreateButton("Note","Times new Roman",13,FingerList[i],false,false,false,tsRedrawActiveStave,"Add Fingering "+QString::number(i),"LeadingLine");
    CreateButton("Note","Times new Roman",15,"-",false,false,false,tsRedrawActiveStave,"Add Leading Line to Fingerings","modifierLeadingLine");
    return m_ButtonList;
}

void CFingering::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CFingering(Symbol));
}

OCGraphicsList CFingering::PrintSign(StemDirection UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    PrintProps.moveToVertical(UpDown, (18 + (SignsUp * 12)) * 12, ScreenObj);
    OCGraphicsList l;
#ifndef __Lelandfont
    if (XMLIntValue("Finger")==6)
    {
        l=ScreenObj.plLet(QChar(uint(OCTTFStopped)), PrintProps.size(), OCTTFname, false, false, 1200, Qt::AlignHCenter);
    }
    else
    {
        l=ScreenObj.plLet(QChar(OCTTFFinger0 + XMLIntValue("Finger")), PrintProps.size(), OCTTFname, false, false, 1500, Qt::AlignHCenter);
    }
#else
    ScreenObj.move(-24,-144);
    if (XMLIntValue("Finger")==6)
    {
        l=ScreenObj.plLet(LelandStopped, PrintProps.size());
    }
    else
    {
        l=ScreenObj.plLet(Leland(LelandFinger0 + XMLIntValue("Finger")), PrintProps.size());
    }
    ScreenObj.move(24,144);
#endif
    if (XMLBoolValue("LeadingLine"))
    {
        ScreenObj.move(-144,-180);
        l.append(ScreenObj.plLet("-",0,"Arial",false,false,156));
    }
    return l;
}

const QStringList CStringNumber::StringSigns = {"I","II","III","IV","V","VI"};

QList<OCToolButtonProps*> CStringNumber::CreateButtons()
{
    for (int i=0;i<6;i++) CreateButton("Note","Times new Roman",13,StringSigns[i],false,false,false,tsRedrawActiveStave,"Add String Number "+StringSigns[i]);
    return m_ButtonList;
}

void CStringNumber::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CStringNumber(Symbol));
}

OCGraphicsList CStringNumber::PrintSign(StemDirection UpDown, int& /*SignsUp*/, OCDraw& ScreenObj)
{
    PrintProps.moveToBelow(UpDown, -24*12, ScreenObj);
    return ScreenObj.plLet(StringSigns[XMLIntValue("String")], PrintProps.size(), "Times New Roman", false, false, 156, Qt::AlignHCenter);
}


QList<OCToolButtonProps*> CBarWidth::CreateButtons()
{
    CreateButton("Staff",":/Notes/Notes/barwidth.png",true,tsReformat,"Add Fixed Bar Width");
    return m_ButtonList;
}

void CBarWidth::DrawFactor(const XMLSymbolWrapper& Symbol, OCCounter& Counter, const XMLTemplateWrapper& /*XMLTemplate*/, OCPageBarList& BarList, const XMLScoreWrapper& /*Score*/)
{
    BarList.setMinimumAll(Counter.barCount(), Symbol.getIntVal("BarWidthValue"));
}

void CBarWidth::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value=Symbol.getIntVal("BarWidthValue");
    CEditDialog d(parent);
    d.setWindowTitle(name());
    int Noteval = 0;
    int Dotted = 0;
    bool Triplet = false;
    XMLSimpleSymbolWrapper::ticksToNoteValue(Noteval,Dotted,Triplet,Value);
    d.EditWidget->PutNoteval(Noteval,Dotted,Triplet);
    //d.QuickAccept(true);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetNoteval(Noteval,Dotted,Triplet);
        Symbol.setAttribute("BarWidthValue",XMLSimpleSymbolWrapper::noteValueToTicks(Noteval,Dotted,Triplet));
    }
    RefreshMode = tsReformat;
}



QList<OCToolButtonProps*> CBeam::CreateButtons()
{
    CreateButton("Durated",":/Notes/Notes/beam.png",true);
    return m_ButtonList;
}

void CBeam::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    //if (tempsetting.BalkOverRide.val <= 0)
    //{
    //tempsetting.BalkOverRide.init(Symbol);
    //tempsetting.BalkOverRide.val = Symbol.getVal("Beams");
    //}
    voiceVars.DurSigns.append(Symbol);
}



QList<OCToolButtonProps*> CCue::CreateButtons()
{
    CreateButton("Staff","Times new Roman",28,"A",false,false);
    return m_ButtonList;
}

void CCue::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    if (Symbol.getBoolVal("Reset")) voiceVars.cueletter = 0;
    voiceVars.cueletter++;
    voiceVars.MasterStuff=true;
}

const QString CCue::cueletter(int i)
{
    int A = QChar('A').unicode();
    int Z = QChar('Z').unicode();
    int l = (i-1)%(Z-(A-1));
    int d = (i-1)/(Z-(A-1));
    QString s = QChar(A + l);
    for (int i = 0; i < d; i++) s += QChar(A + l);
    return s;
}

OCGraphicsList CCue::PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int /*stavedistance*/, OCPrintVarsType &voiceVars, const XMLScoreWrapper& /*Score*/, OCDraw& ScreenObj)
{
    ScreenObj.moveTo(XFysic - (14 * 12), (30 * 12) + ScoreStaffHeight,Symbol);
    QString s = cueletter(voiceVars.cueletter); //= QChar(voiceVars.cueletter + QChar('A').unicode() - 1);
    if (Symbol.getIntVal("Type")==1)  s=QString::number(voiceVars.cueletter);
    OCGraphicsList l = ScreenObj.plLet(s, Symbol.size(), "times new roman", true, false, 240);
    QFontMetrics m(QFont("times new roman",240));
    ScreenObj.move(0,-36);
    ScreenObj.move(-24,-24,Symbol.size());
    QSize s1(m.horizontalAdvance(s),m.height());
    l.append(ScreenObj.PlRect(s1.width()+(108*s.length()),s1.height(),Symbol.size(),false,true));
    return l;
}



QList<OCToolButtonProps*> CCoda::CreateButtons()
{
    CreateButton("Repeats",":/Notes/Notes/coda.png");
    CreateButton("Repeats","Times new Roman",12,"to\nCoda",false,true,false,tsRedrawActiveStave,"Add To Coda");
    return m_ButtonList;
}

void CCoda::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    if (voiceVars.Fine)
    {
        if (Symbol.getIntVal("CodaType") == 1)
        {
            Py=XMLScoreWrapper::FindSymbol(XMLVoice,"Coda",0,"CodaType",0);
            CountIt.reset();
            voiceVars.Fine = false;
        }
    }
}

OCGraphicsList CCoda::PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int /*stavedistance*/, OCPrintVarsType &/*voiceVars*/, const XMLScoreWrapper& /*Score*/, OCDraw& ScreenObj)
{
    OCGraphicsList l;
    if (Symbol.getIntVal("CodaType") == 0)
    {
        ScreenObj.moveTo(XFysic, ScoreTopSymbolY + 96, Symbol);
        l.append(ScreenObj.plLet(OCTTFCoda, Symbol.size(), 1200, Qt::AlignHCenter));
    }
    else
    {
        ScreenObj.moveTo(XFysic, ScoreTopSymbolY, Symbol);
        l.append(ScreenObj.plLet("Dal coda", Symbol.size(), "times new roman", true, true, 156));
    }
    return l;
}

void CCoda::fib(const XMLSymbolWrapper& /*Symbol*/, OCPrintVarsType& voiceVars)
{
    voiceVars.MasterStuff=true;
}



QList<OCToolButtonProps*> CDaCapo::CreateButtons()
{
    CreateButton("Repeats","Times new Roman",12,"D.C.",false,true,false,tsRedrawActiveStave,"Add Da Capo");
    return m_ButtonList;
}

void CDaCapo::fibPlay(const XMLSymbolWrapper& /*Symbol*/, OCMIDIFile& /*MFile*/, OCCounter& CountIt, int& Py, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    if (!voiceVars.Fine)
    {
        Py = -1;
        CountIt.reset();
        voiceVars.Fine = true;
    }
}

OCGraphicsList CDaCapo::PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int /*stavedistance*/, OCPrintVarsType &/*voiceVars*/, const XMLScoreWrapper& /*Score*/, OCDraw& ScreenObj)
{
    ScreenObj.moveTo(XFysic, ScoreTopSymbolY,Symbol);
    return ScreenObj.plLet("Da capo", Symbol.size(), "times new roman", true, true, 156);
}

void CDaCapo::fib(const XMLSymbolWrapper& /*Symbol*/, OCPrintVarsType& voiceVars)
{
    voiceVars.MasterStuff=true;
}



QList<OCToolButtonProps*> CDobbel::CreateButtons()
{
    CreateButton("Note",":/Notes/Notes/turnud.png",false,tsRedrawActiveStave,"Add Turn Up/Down");
    CreateButton("Note",":/Notes/Notes/turndu.png",false,tsRedrawActiveStave,"Add Turn Down/Up");
    return m_ButtonList;
}

bool CDobbel::BeginningOfNote() { return (XMLIntValue("Timing")==1); }

OCGraphicsList CDobbel::PrintSign(StemDirection UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    OCGraphicsList l;
    PrintProps.moveToVertical(UpDown, (18 + (SignsUp * 12)) * 12, ScreenObj);
    if (XMLIntValue("Direction")==1)
    {
        l.append(ScreenObj.plLet(OCTTFDobbelDown, PrintProps.size(), 1200, Qt::AlignHCenter));
    }
    else
    {
        l.append(ScreenObj.plLet(OCTTFDobbelUp, PrintProps.size(), 1200, Qt::AlignHCenter));
    }
    return l;
}

void CDobbel::DuringNote(OCMIDIFile& MFile, int /*Pitch*/, int &LastTime, int Tick, int PlayTime, OCPlayBackVarsType &voiceVars)
{
    if (Currentstep >= 4) return;
    if (!BeginningOfNote())
    {
        if (PlayTime - Tick > PlayProps.Modulate * 4) return;
    }
    Currentstep++;
    switch (Currentstep)
    {
    case 1:
        if (XMLIntValue("Direction")==1)
        {
            CurrentPitch = BasePitch - XMLIntValue("RangeDn");
        }
        else
        {
            CurrentPitch = BasePitch + XMLIntValue("RangeUp");
        }
        break;
    case 2:
        CurrentPitch = BasePitch;
        break;
    case 3:
        if (XMLIntValue("Direction")==1)
        {
            CurrentPitch = BasePitch + XMLIntValue("RangeUp");
        }
        else
        {
            CurrentPitch = BasePitch - XMLIntValue("RangeDn");
        }
        break;
    case 4:
        CurrentPitch = BasePitch;
        break;
    }
    if (voiceVars.PortamentoOn)
    {
        MFile.appendPortamentoEvent(voiceVars.MIDI.Channel, oldpitch, LastTime);
        MFile.appendNoteOnEvent(voiceVars.MIDI.Channel, CurrentPitch, voiceVars.Currentdynam - trilldynam);
        MFile.appendNoteOffEvent(voiceVars.MIDI.Channel, oldpitch, voiceVars.Currentdynam - trilldynam);
        LastTime = 0;
        trilldynam++;
    }
    else
    {
        MFile.appendNoteOffEvent(voiceVars.MIDI.Channel, oldpitch, voiceVars.Currentdynam - trilldynam, LastTime);
        MFile.appendNoteOnEvent(voiceVars.MIDI.Channel, CurrentPitch, voiceVars.Currentdynam - trilldynam);
        LastTime = 0;
        trilldynam++;
    }
    oldpitch = CurrentPitch;
}

void CDobbel::BeforeNote(const XMLSymbolWrapper& /*XMLNote*/, int & /*PlayDynam*/, int &Pitch, int & /*endPitch*/, OCPlayBackVarsType & /*voiceVars*/)
{
    BasePitch = Pitch;
    CurrentPitch = Pitch;
    oldpitch = Pitch;
}

void CDobbel::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CDobbel(Symbol));
}

void CDobbel::Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int & /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &voiceVars)
{
    int Modulate = IntDiv(voiceVars.Playtempo * 30, Symbol.getIntVal("Speed"));
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.append(KillInstantly, Modulate, new CDobbel(Symbol));
}

OCProperties* CDobbel::GetDefaultProperties(int Button)
{
    return OCPresets().SetPropertyValue(CSymbol::GetDefaultProperties(Button),"Speed","trillspeed");
}



QList<OCToolButtonProps*> CDurLength::CreateButtons()
{
    CreateButton("Durated","Times new Roman",13,"Len",false,false,true);
    CreateButton("Durated",":/Notes/Notes/durlegato.png",false,tsRedrawActiveStave,"Add Legato");
    CreateButton("Durated",":/Notes/Notes/durstaccato.png",false,tsRedrawActiveStave,"Add Staccato");
    return m_ButtonList;
}

void CDurLength::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CDurLength(Symbol));
}

void CDurLength::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    const int sgn=Symbol.getIntVal("PerformanceType");
    voiceVars.Articulation.setDurated(Symbol,sgn);
    /*
    switch (sgn)
    {
    case 0:
        voiceVars.Articulation.setDurated(Symbol,0);
        break;
    case 1:
        voiceVars.Articulation.setDurated(Symbol,1);
        break;
    case 2:
        voiceVars.Articulation.setDurated(Symbol,2);
        break;
    }
*/
}

void CDurLength::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &/*voiceVars*/)
{
    SignsToPlay.remove(name());
    SignsToPlay.append(Symbol.ticks()+1, 0, new CDurLength(Symbol));
}

OCGraphicsList CDurLength::PrintSign(StemDirection UpDown, int &/*SignsUp*/, OCDraw& ScreenObj)
{
    int SignType=XMLIntValue("PerformanceType") % 3;
    return OCNoteList::PlotLengths(SignType,PrintProps.Pos,UpDown,PrintProps.size(),ScreenObj);
}

void CDurLength::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(name());
    d.EditWidget->PutSpin("Length of the Voiced part of Notes in %",Symbol.getIntVal("Legato"),1,100);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("Legato",d.EditWidget->GetSpin());
    RefreshMode = tsRedrawActiveStave;
}

OCProperties* CDurLength::GetDefaultProperties(int Button)
{
    CSymbol::GetDefaultProperties(Button);
    switch (Button)
    {
    case 1:
        return OCPresets().SetPropertyValue(&m_PropColl,"Legato","legatolen");
    case 2:
        return OCPresets().SetPropertyValue(&m_PropColl,"Legato","stacclen");
    }
    return &m_PropColl;
}



QList<OCToolButtonProps*> CDurSlant::CreateButtons()
{
    CreateButton("Durated",":/Notes/Notes/slanton.png",false,tsRedrawActiveStave,"Beam Slanting On");
    CreateButton("Durated",":/Notes/Notes/slantoff.png",false,tsRedrawActiveStave,"Beam Slanting Off");
    return m_ButtonList;
}

void CDurSlant::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    voiceVars.SlantFlag.setDurated(Symbol,Symbol.getIntVal("Slanting"));
}

OCProperties* CDurSlant::GetDefaultProperties(int Button)
{
    CSymbol::GetDefaultProperties(Button);
    m_PropColl.setPropertyValue("Slanting",Button);
    return &m_PropColl;
}



QList<OCToolButtonProps*> CDurUpDown::CreateButtons()
{
    CreateButton("Durated",":/Notes/Notes/stemsup.png",false,tsRedrawActiveStave,"Add Stems Up");
    CreateButton("Durated",":/Notes/Notes/stemsdown.png",false,tsRedrawActiveStave,"Add Stems Down");
    return m_ButtonList;
}

void CDurUpDown::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    int Direction = 1;
    if (Symbol.getIntVal("Direction") == 0) Direction = -1;
    voiceVars.UpDown.setDurated(Symbol,Direction);
}

const QStringList CDynChange::DynamicChangeList = {"dim","cresc"};

QList<OCToolButtonProps*> CDynChange::CreateButtons()
{
    CreateButton("Dynamics","Times new Roman",13,"dim",false,true,false,tsRedrawActiveStave,"Add Diminuendo");
    CreateButton("Dynamics","Times new Roman",13,"cresc",false,true,false,tsRedrawActiveStave,"Add Crescendo");
    return m_ButtonList;
}

void CDynChange::AfterNote(const XMLSymbolWrapper& XMLNote, OCPlayBackVarsType &voiceVars)
{
    if (XMLIntValue("DynamicType") > 0)
    {
        voiceVars.crescendo += DoubleDiv(XMLNote.ticks(), 40) * XMLIntValue("Speed");
    }
    else if (XMLIntValue("DynamicType") == 0)
    {
        voiceVars.crescendo += DoubleDiv(XMLNote.ticks(), 60) * -XMLIntValue("Speed");
    }
}

OCGraphicsList CDynChange::plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& /*BarList*/, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& Score, OCNoteList& /*NoteList*/, OCPrintVarsType &/*voiceVars*/, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    OCGraphicsList l;
    //ScreenObj.DM(XFysic, 37 * 12,XMLSymbol);
    ScreenObj.moveTo(XFysic,ScoreBottomSymbolY,Symbol);
    l.append(ScreenObj.plLet(DynamicChangeList[Symbol.getIntVal("DynamicType")], Symbol.size(), Score.DynamicFont.font(), Qt::AlignBottom));
    return l;
}

void CDynChange::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &/*voiceVars*/)
{
    SignsToPlay.remove("Hairpin");
    SignsToPlay.remove(name());
    SignsToPlay.append(NotDecrementable, 0, new CDynChange(Symbol));
}

OCProperties* CDynChange::GetDefaultProperties(int Button)
{
    if (Button==0) return OCPresets().SetPropertyValue(CSymbol::GetDefaultProperties(Button),"Speed","dimspeed");
    return OCPresets().SetPropertyValue(CSymbol::GetDefaultProperties(Button),"Speed","crescspeed");
}



QList<OCToolButtonProps*> CExpression::CreateButtons()
{
    CreateButton("MIDI",":/Notes/Notes/expression.png",true);
    return m_ButtonList;
}

void CExpression::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    voiceVars.ExpressionOn=!Symbol.getBoolVal("Disabled");
}

void CExpression::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value=Symbol.getIntVal("Disabled");
    CEditDialog d(parent);
    d.setWindowTitle(name());
    d.EditWidget->PutCombo("Expression controller (controller 11)",Value,QStringList{"Enabled", "Disabled"});
    d.QuickAccept();
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("Disabled",bool(d.EditWidget->GetCombo()));
    RefreshMode = tsRedrawActiveStave;
}



QList<OCToolButtonProps*> CFermata::CreateButtons()
{
    CreateButton("Tempo",":/Notes/Notes/fermata.png");
    return m_ButtonList;
}

void CFermata::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &/*voiceVars*/)
{
    SignsToPlay.append(KillInstantly, 0, new CFermata(Symbol));
}

OCGraphicsList CFermata::plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& /*BarList*/, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType &/*voiceVars*/, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    ScreenObj.moveTo(XFysic, 1628,Symbol);
    return ScreenObj.plLet(OCTTFFermata, Symbol.size(), 1200, Qt::AlignHCenter);
}



QList<OCToolButtonProps*> CFine::CreateButtons()
{
    CreateButton("Repeats","Times new Roman",12,"Fine",false,true);
    return m_ButtonList;
}

void CFine::fibPlay(const XMLSymbolWrapper& /*Symbol*/, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int &Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    if (voiceVars.Fine)
    {
        Py = XMLScoreWrapper::FindSymbol(XMLVoice,"",Py);
    }
}

OCGraphicsList CFine::PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int /*stavedistance*/, OCPrintVarsType &/*voiceVars*/, const XMLScoreWrapper& /*Score*/, OCDraw& ScreenObj)
{
    ScreenObj.moveTo(XFysic, ScoreTopSymbolY, Symbol);
    return ScreenObj.plLet("Fine", Symbol.size(), "times new roman", true, true, 156);
}

void CFine::fib(const XMLSymbolWrapper& /*Symbol*/, OCPrintVarsType& voiceVars)
{
    voiceVars.MasterStuff=true;
}



QList<OCToolButtonProps*> CHairpin::CreateButtons()
{
    CreateButton("Durated",":/Notes/Notes/hpcresc.png",false,tsRedrawActiveStave,"Add Crescendo");
    CreateButton("Durated",":/Notes/Notes/hpdim.png",false,tsRedrawActiveStave,"Add Diminuendo");
    CreateButton("Durated","Times new Roman",20,"><",true,false,false,tsRedrawActiveStave,"Add Inverted 'Fish'");
    CreateButton("Durated","Times new Roman",20,"<>",true,false,false,tsRedrawActiveStave,"Add 'Fish'");
    return m_ButtonList;
}

int CHairpin::Speed()
{
    if (XMLIntValue("HairpinType") > 1)
    {
        return XMLIntValue("Speed") / 2;
    }
    return XMLIntValue("Speed");
}

void CHairpin::Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &voiceVars)
{
    SignsToPlay.remove("Hairpin");
    SignsToPlay.remove("DynamicChange");
    int Modulate = IntDiv(voiceVars.Playtempo * 2, Symbol.getIntVal("Speed"));
    if (Symbol.getIntVal("HairpinType")>1)
    {
        Modulate = IntDiv(voiceVars.Playtempo * 2, Symbol.getIntVal("Speed") / 2);
    }
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.append(Symbol.ticks(), Modulate, new CHairpin(Symbol));
}

void CHairpin::DuringNote(OCMIDIFile& MFile, int /*Pitch*/, int &LastTime, int Tick, int /*PlayTime*/, OCPlayBackVarsType &voiceVars)
{
    Direction = 1;
    if ((XMLIntValue("HairpinType") == 2) || (XMLIntValue("HairpinType") == 1))  Direction = -1;
    if (XMLIntValue("HairpinType") > 1)
    {
        AfterNoteDuration = PlayProps.Duration;
        if (TickCount + Tick > XMLIntValue("Ticks")*5) Direction = -Direction;
    }
    voiceVars.express = qBound<int>(0, voiceVars.express + Direction, 127);
    //qDebug() << voiceVars.exprbegin << voiceVars.express << XMLIntValue("HairpinType") << Direction << voiceVars.currentcresc << voiceVars.Currentdynam;
    MFile.appendExpressionEvent(voiceVars.MIDI.Channel, voiceVars.express, LastTime);
    LastTime = 0;
    voiceVars.changeexp = 1;
}

void CHairpin::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    voiceVars.DurSigns.append(Symbol);
}

void CHairpin::BeforeNote(const XMLSymbolWrapper& /*XMLNote*/, int &PlayDynam, int &/*Pitch*/, int &/*endPitch*/, OCPlayBackVarsType &voiceVars)
{
    int crescvar=0;
    if (voiceVars.ExpressionOn)
    {
        crescvar = (63 - (voiceVars.currentcresc / 2) * 1);
        if (crescvar > 127 - PlayDynam) crescvar = 127 - PlayDynam;
        if (crescvar > expressiondefault - 1) crescvar = expressiondefault - 1;
        PlayDynam += crescvar;
    }
    PlayDynam = qBound<int>(1, PlayDynam, 127);
    voiceVars.exprbegin = expressiondefault - crescvar;
}

void CHairpin::AfterNote(const XMLSymbolWrapper& XMLNote, OCPlayBackVarsType &voiceVars)
{
    if (XMLIntValue("HairpinType") > 1)
    {
        TickCount += XMLNote.ticks()*10;
    }
    if (Direction > 0)
    {
        voiceVars.crescendo += DoubleDiv(XMLNote.ticks(), 40) * XMLIntValue("Speed") * Direction;
    }
    else if (Direction < 0)
    {
        voiceVars.crescendo += DoubleDiv(XMLNote.ticks(), 60) * XMLIntValue("Speed") * Direction;
    }
    if (XMLIntValue("HairpinType") > 1)
    {
        if (AfterNoteDuration == XMLNote.ticks()) voiceVars.crescendo = 0;
    }
    voiceVars.exprbegin = expressiondefault;
}

OCProperties* CHairpin::GetDefaultProperties(int Button)
{
    CSymbol::GetDefaultProperties(Button);
    switch (Button)
    {
    case 0:
    case 3:
        return OCPresets().SetPropertyValue(&m_PropColl,"Speed","crescspeed");
    case 1:
    case 2:
        return OCPresets().SetPropertyValue(&m_PropColl,"Speed","dimspeed");
    }
    return &m_PropColl;
}

OCGraphicsList CHairpin::plot(const XMLSymbolWrapper& Symbol, double /*XFysic*/, OCPageBarList& /*BarList*/, OCCounter& CountIt, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& NoteList, OCPrintVarsType &/*voiceVars*/, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    return OCNoteList::PlotHairPin(NoteList.CreateList(CountIt.RhythmObjectIndex,Symbol.ticks()), Symbol, false, ScreenObj);
}

OCGraphicsList CHairpin::plotRemaining(const OCDurSignType& s, OCNoteList& NoteList, OCDraw& ScreenObj)
{
    const OCRhythmObjectList l = NoteList.CreateList(0,int(s.RemainingTicks));
    return  (l.size() > 1) ? OCNoteList::PlotHairPin(l, s.XMLSymbol, true, ScreenObj) : OCGraphicsList();
}


QList<OCToolButtonProps*> CFlipTie::CreateButtons()
{
    CreateButton("Staff",":/Notes/Notes/slurdirection.png",true,tsRedrawActiveStave,"Add Flip Ties");
    return m_ButtonList;
}

void CFlipTie::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    voiceVars.FlipTie=!Symbol.getIntVal("TieDirection");
}

void CFlipTie::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value=Symbol.getIntVal("TieDirection");
    CEditDialog d(parent);
    d.setWindowTitle(name());
    d.EditWidget->PutCombo("Show Ties Upside Down",Value,FlipList);
    d.QuickAccept();
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("TieDirection",bool(d.EditWidget->GetCombo()));
    RefreshMode = tsRedrawActiveStave;
}



QList<OCToolButtonProps*> CGliss::CreateButtons()
{
    CreateButton("Note",":/Notes/Notes/glissando.png",true);
    return m_ButtonList;
}

void CGliss::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CGliss(Symbol));
}

void CGliss::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(name());
    QStringList l;
    for (int i = -48; i <= 48; i++) l.append(QString::number(i));
    d.EditWidget->PutCombo("Range",Symbol.getIntVal("Range")+48,l);
    d.QuickAccept();
    //d.EditWidget->PutSpin("Range",Symbol.getIntVal("Range"),-48,48);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("Range",d.EditWidget->GetCombo()-48);
    RefreshMode = tsRedrawActiveStave;
}

void CGliss::Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &/*voiceVars*/)
{
    SignsToPlay.append(KillInstantly, 1, new CGliss(Symbol));
}

OCGraphicsList CGliss::PrintSign(StemDirection /*UpDown*/, int &/*SignsUp*/, OCDraw& ScreenObj)
{
    const int Range = XMLIntValue("Range");
    PrintProps.moveTo(144, Sgn<int>(Range) * 24, ScreenObj);
    return ScreenObj.line(double(PrintProps.TieLen-48), Range * 24 - Sgn<int>(Range));
}

void CGliss::BeforeNote(const XMLSymbolWrapper& /*XMLNote*/, int &/*PlayDynam*/, int &Pitch, int &endPitch, OCPlayBackVarsType &/*voiceVars*/)
{
    CurrentPitch = Pitch;
    oldpitch = Pitch;
    Direction = Sgn<int>(XMLIntValue("Range"));
    endPitch = Pitch + XMLIntValue("Range");
    qDebug() << "Gliss before note ************************" << CurrentPitch << oldpitch << Direction << endPitch;
}

void CGliss::DuringNote(OCMIDIFile& MFile, int Pitch, int &LastTime, int Tick, int PlayTime, OCPlayBackVarsType &voiceVars)
{
    if (GlissModulate == 0) GlissModulate = int(DoubleDiv(PlayTime, qAbs<int>(XMLIntValue("Range")) + 1));
    if (CurrentPitch != Pitch + XMLIntValue("Range"))
    {
        if (Tick % GlissModulate == 0)
        {
            CurrentPitch += Direction;
            if (voiceVars.PortamentoOn)
            {
                qDebug() << "Portamento 3" << oldpitch;
                MFile.appendPortamentoEvent(voiceVars.MIDI.Channel, oldpitch,LastTime);
                qDebug() << "Note On 3" << CurrentPitch;
                MFile.appendNoteOnEvent(voiceVars.MIDI.Channel, CurrentPitch, voiceVars.Currentdynam);
                qDebug() << "Note Off 3" << oldpitch;
                MFile.appendNoteOffEvent(voiceVars.MIDI.Channel, oldpitch, voiceVars.Currentdynam);
                LastTime = 0;
            }
            else
            {
                qDebug() << "Note Off 4" << oldpitch;
                MFile.appendNoteOffEvent(voiceVars.MIDI.Channel, oldpitch, voiceVars.Currentdynam, LastTime);
                qDebug() << "Note On 4" << CurrentPitch;
                MFile.appendNoteOnEvent(voiceVars.MIDI.Channel, CurrentPitch, voiceVars.Currentdynam);
                LastTime = 0;
            }
            oldpitch = CurrentPitch;
        }
    }
}

QList<OCToolButtonProps*> CLength::CreateButtons()
{
    CreateButton("Note","Times new Roman",11,QString("Len\n")+QChar(0x2192),false,false,true,tsRedrawActiveStave,"Add Length");
    CreateButton("Note","Times new Roman",11,QString(QChar(0x25ac))+"\n"+QString(QChar(0x2192)),false,false,false,tsRedrawActiveStave,"Add Tenuto");
    CreateButton("Note","Times new Roman",11,QString(QChar(0x25cf))+"\n"+QString(QChar(0x2192)),false,false,false,tsRedrawActiveStave,"Add Legato");
    CreateButton("Note","Times new Roman",11,"Len\nx1",false,false,true,tsRedrawActiveStave,"Add Length x1");
    CreateButton("Note","Times new Roman",11,QChar(0x25ac)+QString("\nx1"),false,false,false,tsRedrawActiveStave,"Add Tenuto x1");
    CreateButton("Note","Times new Roman",11,QChar(0x25cf)+QString("\nx1"),false,false,false,tsRedrawActiveStave,"Add Legato x1");
    return m_ButtonList;
}

void CLength::ModifyProperties(OCProperties& p)
{
    bool Hide = (p.propertyValue("PerformanceType").toInt() % 3 == 0);
    p.hide("Left", Hide);
    p.hide("Top", Hide);
    p.hide("Size", Hide);
    p.hide("Invisible", Hide);
}

void CLength::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CLength(Symbol));
}

void CLength::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(name());
    d.EditWidget->PutSpin("Length of the Voiced part of Notes in %",Symbol.getIntVal("Legato"),1,100);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("Legato",d.EditWidget->GetSpin());
    RefreshMode = tsRedrawActiveStave;
}

void CLength::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    if (Symbol.getIntVal("PerformanceType") < 3) voiceVars.currentlen = Symbol.getIntVal("Legato");
}

void CLength::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    int sgn=Symbol.getIntVal("PerformanceType");
    if (sgn < 3) {
        voiceVars.Articulation.setCurrent(Symbol,sgn);
    }
    else {
        voiceVars.Articulation.setX1(Symbol,sgn % 3);
    }
    /*
    switch (sgn)
    {
    case 0:
        voiceVars.Articulation.setCurrent(Symbol,0);
        break;
    case 1:
        voiceVars.Articulation.setCurrent(Symbol,1);
        break;
    case 2:
        voiceVars.Articulation.setCurrent(Symbol,2);
        break;
    case 3:
        voiceVars.Articulation.setX1(Symbol,0);
        break;
    case 4:
        voiceVars.Articulation.setX1(Symbol,1);
        break;
    case 5:
        voiceVars.Articulation.setX1(Symbol,2);
        break;
    }
*/
}

void CLength::Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& /*voiceVars*/)
{
    if (Symbol.getIntVal("PerformanceType") > 2) SignsToPlay.append(KillInstantly, 0, new CLength(Symbol));
}

OCGraphicsList CLength::plot(const XMLSymbolWrapper& Symbol, double XFysic,OCPageBarList& /*BarList*/, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType& /*voiceVars*/, const XMLTemplateStaffWrapper& /*XMLTemplateStaff*/, OCDraw& ScreenObj) {

    if ((Symbol.getIntVal("PerformanceType") % 3) == 0) return CSymbol::plotInvisibleIcon(this,Symbol,XFysic,ScreenObj);
    return OCGraphicsList();
}

OCGraphicsList CLength::PrintSign(StemDirection UpDown, int &/*SignsUp*/, OCDraw& ScreenObj)
{
    const int Sign = XMLIntValue("PerformanceType") % 3;
    return OCNoteList::PlotLengths(Sign,PrintProps.Pos,UpDown,PrintProps.size(),ScreenObj);
}

OCProperties* CLength::GetDefaultProperties(int Button)
{
    CSymbol::GetDefaultProperties(Button);
    switch (Button)
    {
    case 1:
    case 4:
        return OCPresets().SetPropertyValue(&m_PropColl,"Legato","legatolen");
    case 2:
    case 5:
        return OCPresets().SetPropertyValue(&m_PropColl,"Legato","stacclen");
    }
    return &m_PropColl;
}



QList<OCToolButtonProps*> CLimit::CreateButtons()
{
    CreateButton("Staff",":/Notes/Notes/beamlimit.png",true,tsRedrawActiveStave,"Add Beam Limit");
    return m_ButtonList;
}

void CLimit::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    voiceVars.BalkLimit = 6 * Symbol.getIntVal("SixteenthsNotes");
}

void CLimit::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(name());
    d.EditWidget->PutSpin("Length in 16ths",Symbol.getIntVal("SixteenthsNotes"),0,64);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("SixteenthsNotes",d.EditWidget->GetSpin());
    RefreshMode = tsRedrawActiveStave;
}



QList<OCToolButtonProps*> CMordent::CreateButtons()
{
    CreateButton("Note",":/Notes/Notes/mordent1.png",false,tsRedrawActiveStave,"Add Mordernt Down");
    CreateButton("Note",":/Notes/Notes/mordent.png",false,tsRedrawActiveStave,"Add Mordernt Up");
    return m_ButtonList;
}

void CMordent::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CMordent(Symbol));
}

void CMordent::Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &voiceVars)
{
    int Modulate = IntDiv(voiceVars.Playtempo * 30, Symbol.getIntVal("Speed"));
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.append(KillInstantly, Modulate, new CMordent(Symbol));
}

OCGraphicsList CMordent::PrintSign(StemDirection UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    int SignChar=OCTTFMordent;
    if (XMLIntValue("Range")>0) SignChar=OCTTFPraltrill;
    PrintProps.moveToVertical(UpDown, (18 + (SignsUp * 12)) * 12, ScreenObj);
    return ScreenObj.plLet(MakeUnicode(QChar(SignChar)), PrintProps.size(), OCTTFname, false, false, 1200, Qt::AlignHCenter);
}

void CMordent::DuringNote(OCMIDIFile& MFile, int /*Pitch*/, int &LastTime, int /*Tick*/, int /*PlayTime*/, OCPlayBackVarsType &voiceVars)
{
    if (Finished) return;
    if (CurrentPitch == BasePitch)
    {
        CurrentPitch = BasePitch + XMLIntValue("Range");
    }
    else
    {
        CurrentPitch = BasePitch;
        Finished = true;
    }
    if (voiceVars.PortamentoOn)
    {
        MFile.appendNoteOnEvent(voiceVars.MIDI.Channel, CurrentPitch, voiceVars.Currentdynam - trilldynam, LastTime);
        MFile.appendNoteOffEvent(voiceVars.MIDI.Channel, oldpitch, voiceVars.Currentdynam - trilldynam);
        MFile.appendPortamentoEvent(voiceVars.MIDI.Channel, CurrentPitch);
        LastTime = 0;
        trilldynam ++;
    }
    else
    {
        MFile.appendNoteOffEvent(voiceVars.MIDI.Channel, oldpitch, voiceVars.Currentdynam - trilldynam, LastTime);
        MFile.appendNoteOnEvent(voiceVars.MIDI.Channel, CurrentPitch, voiceVars.Currentdynam - trilldynam);
        LastTime = 0;
        trilldynam++;
    }
    oldpitch = CurrentPitch;
}

void CMordent::BeforeNote(const XMLSymbolWrapper& /*XMLNote*/, int &/*PlayDynam*/, int &Pitch, int &/*endPitch*/, OCPlayBackVarsType &/*voiceVars*/)
{
    BasePitch = Pitch;
    CurrentPitch = Pitch;
    oldpitch = Pitch;
}

OCProperties* CMordent::GetDefaultProperties(int Button)
{
    CSymbol::GetDefaultProperties(Button);
    switch (Button)
    {
    case 0:
        m_PropColl.setPropertyValue("Range",-2);
        break;
    case 1:
        m_PropColl.setPropertyValue("Range",2);
        break;
    }
    return OCPresets().SetPropertyValue(&m_PropColl,"Speed","trillspeed");
}



QList<OCToolButtonProps*> COctave::CreateButtons()
{
    CreateButton("Octave","Times new Roman",15,QChar(0x2193)+QString("15"),false,true,false,tsRedrawActiveStave,"Add 15ma Down");
    CreateButton("Octave","Times new Roman",15,QChar(0x2193)+QString("8"),false,true,false,tsRedrawActiveStave,"Add 8va Down");
    CreateButton("Octave","Times new Roman",13,"loco",false,true,false,tsRedrawActiveStave,"Add Loco");
    CreateButton("Octave","Times new Roman",15,QChar(0x2191)+QString("8"),false,true,false,tsRedrawActiveStave,"Add 8va Up");
    CreateButton("Octave","Times new Roman",15,QChar(0x2191)+QString("15"),false,true,false,tsRedrawActiveStave,"Add 15ma Up");
    return m_ButtonList;
}

void COctave::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    voiceVars.MIDI.Octave = Symbol.getIntVal("OctaveType") - 2;
}

void COctave::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    voiceVars.MIDI.Octave = Symbol.getIntVal("OctaveType") - 2;
}

OCGraphicsList COctave::plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& /*BarList*/, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& Score, OCNoteList& /*NoteList*/, OCPrintVarsType &/*voiceVars*/, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    int d = 45 * 12;
    if ((Symbol.getIntVal("OctaveType") - 2) >= 0) d = ScoreTopSymbolY;
    ScreenObj.moveTo(XFysic-60, d, Symbol);
    OCGraphicsList l;
    switch (Symbol.getIntVal("OctaveType") - 2)
    {
    case 1:
        l.append(ScreenObj.line(0, 60));
        l.append(ScreenObj.line(60, 0));
        ScreenObj.move(36, 0);
        l.append(ScreenObj.plLet("8va", Symbol.size(), Score.DynamicFont.font()));
        break;
    case 2:
        l.append(ScreenObj.line(0, 60));
        l.append(ScreenObj.line(60, 0));
        ScreenObj.move(36, 0);
        l.append(ScreenObj.plLet("15ma", Symbol.size(), Score.DynamicFont.font()));
        break;
    case -2:
        l.append(ScreenObj.line(0, -60));
        l.append(ScreenObj.line(60, 0));
        ScreenObj.move(36, 0);
        l.append(ScreenObj.plLet("15ma", Symbol.size(), Score.DynamicFont.font()));
        break;
    case -1:
        l.append(ScreenObj.line(0, -60));
        l.append(ScreenObj.line(60, 0));
        ScreenObj.move(36, 0);
        l.append(ScreenObj.plLet("8va", Symbol.size(), Score.DynamicFont.font()));
        break;
    case 0:
        l.append(ScreenObj.plLet("loco", Symbol.size(), Score.DynamicFont.font()));
        break;
    }
    return l;
}



QList<OCToolButtonProps*> CPortamento::CreateButtons()
{
    CreateButton("MIDI",":/Notes/Notes/synth.png",true);
    return m_ButtonList;
}

QStringList CPortamento::PortamentoList={"Enabled", "Disabled"};

void CPortamento::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    voiceVars.PortamentoOn = !Symbol.getBoolVal("Portamento");
    voiceVars.PortamentoTime = Symbol.getIntVal("Time");
    MFile.appendControllerEvent(voiceVars.MIDI.Channel,0x41,int(voiceVars.PortamentoOn * 64));
    MFile.appendControllerEvent(voiceVars.MIDI.Channel,0x5,voiceVars.PortamentoTime);
    voiceVars.CurrentDelta = 0;
}

void CPortamento::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value=Symbol.getIntVal("Portamento");
    int Time=Symbol.getIntVal("Time");
    CEditDialog d(parent);
    d.setWindowTitle(name());
    //d.ShowList("Controller");
    d.EditWidget->PutPortamento(Value,Time);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetController(Value,Time);
        Symbol.setAttribute("Portamento",Value);
        Symbol.setAttribute("Time",Time);
    }
    RefreshMode = tsRedrawActiveStave;
}

QList<OCToolButtonProps*> CRepeat::CreateButtons()
{
    CreateButton("Repeats",":/Notes/Notes/repeatend.png",true,tsRedrawActiveStave,"Add Repeat End");
    CreateButton("Repeats",":/Notes/Notes/repeatbegin.png",false,tsRedrawActiveStave,"Att Repeat Begin");
    CreateButton("Repeats",":/Notes/Notes/volte.png",true,tsRedrawActiveStave,"Add Volta");
    CreateButton("Repeats",":/Notes/Notes/barline.png",true,tsRedrawActiveStave,"Add Barline");
    return m_ButtonList;
}

void CRepeat::ModifyProperties(OCProperties& p)
{
    p.show("Volta");
    p.show("Repeats");

    switch (p.propertyValue("RepeatType").toInt())
    {
    case 0:
        p.hide("Volta");
        break;
    case 1:
        p.hide("Volta");
        p.hide("Repeats");
        break;
    case 2:
        p.hide("Repeats");
        break;
    case 3:
        p.hide("Volta");
        p.hide("Repeats");
        break;
    }
}

OCGraphicsList CRepeat::plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType &/*voiceVars*/, const XMLTemplateStaffWrapper & XMLTemplateStaff, OCDraw& ScreenObj)
{
    double XMov=0;
    double Height = ScoreStaffLinesHeight;
    if ((XMLTemplateStaff.squareBracket() == SBBegin) || (XMLTemplateStaff.curlyBracket() == CBBegin))
    {
        Height = ScoreStaffHeight + (XMLTemplateStaff.height() * 12);
    }
    if (Symbol.getIntVal("RepeatType") < 2) // End begin
    {
        if ((Symbol.getIntVal("RepeatType") == 0) && CountIt.isFirstBeatOfFirstBar()) return OCGraphicsList();
        if (CountIt.isFirstBeat()) {
            XMov = BarList.paddingLeft(CountIt.barCount(), false, false, false) + 72;
            if (CountIt.isFirstBeatOfFirstBar()) XMov -= 48;
        }
        ScreenObj.moveTo(Symbol.moveX(XFysic - (16 * 12) - XMov), ScoreStaffHeight);
        OCGraphicsList l;
        l.append(ScreenObj.PlRect(24,-Height));
        if (Symbol.getIntVal("RepeatType") == 0) // End
        {
            ScreenObj.moveTo(Symbol.moveX(XFysic - (18 * 12) - XMov), ScoreStaffHeight);
            l.append(ScreenObj.line(0, -Height));
            ScreenObj.move(0, Height);
            l.append(ScreenObj.plDot(3,-36,-144));
            l.append(ScreenObj.plDot(3,-36,-240));
            if (Symbol.getIntVal("Repeats") > 2)
            {
                ScreenObj.moveTo(XFysic - (24 * 12) - XMov, ScoreStaffHeight + 96, Symbol);
                l.append(ScreenObj.plLet(QString::number(Symbol.getIntVal("Repeats")) + "x", Symbol.size(), "times new roman", true, true, 156));
            }
            if (ScreenObj.canColor()) return l;
        }
        else if (Symbol.getIntVal("RepeatType") == 1) // Begin
        {
            ScreenObj.moveTo(Symbol.moveX(XFysic - (12 * 12) - XMov), ScoreStaffHeight);
            l.append(ScreenObj.line(0, -Height));
            ScreenObj.move(0, Height);
            l.append(ScreenObj.plDot(3,36,-144));
            l.append(ScreenObj.plDot(3,36,-240));
            if (ScreenObj.canColor()) return l;
        }
    }
    else if (Symbol.getIntVal("RepeatType") == 2) // Volta
    {
        ScreenObj.moveTo(XFysic - (12 * 12), 1680, Symbol);
        OCGraphicsList l=ScreenObj.plLet(QString::number(Symbol.getIntVal("Volta")) + ".", Symbol.size(), "times new roman", true, true, 156);
        ScreenObj.moveTo(XFysic - (16 * 12), 1680, Symbol);
        l.append(ScreenObj.line(0, 180));
        l.append(ScreenObj.line(480, 0));
        return l;
    }
    else if (Symbol.getIntVal("RepeatType") == 3) // Barline
    {
        if (CountIt.isFirstBeatOfFirstBar()) return OCGraphicsList();
        if (CountIt.isFirstBeat())
        {
            XMov = BarList.paddingLeft(CountIt.barCount(), true, !CountIt.isFirstBar(), true)+72;
        }
        ScreenObj.moveTo(Symbol.moveX(XFysic - (18 * 12) - XMov), ScoreStaffHeight);
        OCGraphicsList l;
        l.append(ScreenObj.line(0,-Height));
        if (ScreenObj.canColor()) return l;
    }
    return OCGraphicsList();
}

OCGraphicsList CRepeat::plotSystemEnd(const XMLSymbolWrapper& Symbol, double /*XFysic*/,OCPageBarList& BarList, OCCounter& CountIt, OCPrintSignList& SignsToPrint, QColor /*SignCol*/, const XMLScoreWrapper& Score, OCNoteList& NoteList, OCPrintVarsType& voiceVars, const XMLTemplateStaffWrapper & XMLTemplateStaff, OCDraw& ScreenObj)
{
    if ((Symbol.getIntVal("RepeatType") == 0) || (Symbol.getIntVal("RepeatType") == 3))
    {
        return plot(Symbol, ScreenObj.spaceX(BarList.systemLength() + (15 * 12)), BarList, CountIt, SignsToPrint, unselectablecolor, Score, NoteList, voiceVars, XMLTemplateStaff, ScreenObj);
    }
    return OCGraphicsList();
}

void CRepeat::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(name());
    if (Symbol.getIntVal("RepeatType")==0)
    {
        d.EditWidget->PutSpin("Number of Repeats:",Symbol.getIntVal("Repeats"),1,999);
        esc=(d.exec()!=QDialog::Accepted);
        if (!esc) Symbol.setAttribute("Repeats",d.EditWidget->GetSpin());
    }
    else if (Symbol.getIntVal("RepeatType")==2)
    {
        d.EditWidget->PutSpin("Volta:",Symbol.getIntVal("Volta"),1,999);
        esc=(d.exec()!=QDialog::Accepted);
        if (!esc) Symbol.setAttribute("Volta",d.EditWidget->GetSpin());
    }
    RefreshMode = tsRedrawActiveStave;
}

void CRepeat::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& CountIt, int &Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    switch (Symbol.getIntVal("RepeatType"))
    {
    case 0: //Repeat end
        if (!voiceVars.Repeat.empty())
        {
            if (voiceVars.PlayRepeat == 0) voiceVars.PlayRepeat = Symbol.getIntVal("Repeats") - 1;
            voiceVars.PlayRepeat--;
            Py = voiceVars.Repeat.last();
            if (Py==-1) CountIt.reset();
            if (voiceVars.PlayRepeat <= 0)
            {
                voiceVars.PlayRepeat = 0;
                voiceVars.Repeat.removeLast();
            }
        }
        break;
    case 1: //Repeat start
        if (!voiceVars.Repeat.empty())
        {
            if (voiceVars.Repeat.first() == -1) voiceVars.Repeat.removeFirst();
        }
        voiceVars.Repeat.append(Py);
        break;
    case 2: //Volta
        voiceVars.Volta++;
        if (voiceVars.Volta > 1)
        {
            if (Symbol.getIntVal("Volta") == 1)
            {
                if (voiceVars.Repeat.empty()) voiceVars.Repeat.append(-1);
                Py=XMLScoreWrapper::FindSymbol(XMLVoice,name(),Py+1,"RepeatType",2,"Volta",voiceVars.Volta);
                CountIt.reset();
            }
        }
        break;
    }
}



QList<OCToolButtonProps*> CSegno::CreateButtons()
{
    CreateButton("Repeats","Times new Roman",12,"D.S.",false,true,false,tsRedrawActiveStave,"Add dal Segno");
    CreateButton("Repeats",":/Notes/Notes/segno.png");
    return m_ButtonList;
}

void CSegno::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& CountIt, int &Py, const XMLVoiceWrapper& XMLVoice, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType &voiceVars)
{
    if (!voiceVars.Fine)
    {
        if (Symbol.getIntVal("SegnoType") == 0)
        {
            Py = XMLScoreWrapper::FindSymbol(XMLVoice,name(),0,"SegnoType",1);
            CountIt.reset();
            voiceVars.Fine = true;
        }
    }
}

OCGraphicsList CSegno::PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int /*stavedistance*/, OCPrintVarsType &/*voiceVars*/, const XMLScoreWrapper& /*Score*/, OCDraw& ScreenObj)
{
    OCGraphicsList l;
    if (Symbol.getIntVal("SegnoType") == 1)
    {
        ScreenObj.moveTo(XFysic, ScoreTopSymbolY + 144+72, Symbol);
        l.append(ScreenObj.plLet(OCTTFSegno, Symbol.size(), 1200, Qt::AlignHCenter));
    }
    else
    {
        ScreenObj.moveTo(XFysic, ScoreTopSymbolY + 12, Symbol);
        l.append(ScreenObj.plLet("DS", Symbol.size(), "times new roman", true, true, 156));
    }
    return l;
}

void CSegno::fib(const XMLSymbolWrapper& /*Symbol*/, OCPrintVarsType& voiceVars)
{
    voiceVars.MasterStuff=true;
}

QList<OCToolButtonProps*> CSlant::CreateButtons()
{
    CreateButton("Staff",":/Notes/Notes/slant.png",true,tsRedrawActiveStave,"Add Beam Slant");
    return m_ButtonList;
}

void CSlant::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    voiceVars.SlantFlag.setCurrent(Symbol, Symbol.getIntVal("BeamSlanting"));
}

void CSlant::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    int Value=Symbol.getIntVal("BeamSlanting");
    CEditDialog d(parent);
    d.setWindowTitle(name());
    d.EditWidget->PutCombo("Beam Slanting",Value,SlantList);
    d.QuickAccept();
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("BeamSlanting",d.EditWidget->GetCombo());
    RefreshMode = tsRedrawActiveStave;
}



QList<OCToolButtonProps*> CDurSlur::CreateButtons()
{
    CreateButton("Durated",":/Notes/Notes/legato.png",false,tsRedrawActiveStave,"Add Slur");
    CreateButton("Durated",":/Notes/Notes/legatoup.png",false,tsRedrawActiveStave,"Add Slur Up");
    CreateButton("Durated",":/Notes/Notes/legato.png",false,tsRedrawActiveStave,"Add Slur Down");
    return m_ButtonList;
}

void CDurSlur::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &/*voiceVars*/)
{
    SignsToPlay.remove(name());
    SignsToPlay.append(Symbol.ticks(), 0, new CDurSlur(Symbol));
}

void CDurSlur::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    /*
    if (Symbol.getVal("Direction")==1)
    {
        tempsetting.SlurUp.init(Symbol);
        tempsetting.SlurUp.val = Symbol.getVal("Angle");
    }
    else if (Symbol.getVal("Direction")==2)
    {
        //tempsetting.SlurDown.init(Symbol);
        //tempsetting.SlurDown.val = Symbol.getVal("Angle");
        */
    voiceVars.DurSigns.append(Symbol);
    //}
}
/*
void CDurSlur::AfterNote(const XMLSymbolWrapper& XMLNote, OCPlayBackVarsType &voiceVars)
{
    //m_PropColl.setValue("Ticks", m_PropColl.value("Ticks").toInt() - XMLNote.ticks());
    //TickCount += XMLNote.ticks();
}
*/
OCGraphicsList CDurSlur::plot(const XMLSymbolWrapper& Symbol, double /*XFysic*/, OCPageBarList& /*BarList*/, OCCounter& CountIt, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& NoteList, OCPrintVarsType &/*voiceVars*/, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    const OCRhythmObjectList slurlist = NoteList.CreateList(CountIt.RhythmObjectIndex,Symbol.ticks());
    return OCNoteList::PlotSlur(slurlist, Symbol, false, ScreenObj);
}

OCGraphicsList CDurSlur::plotRemaining(const OCDurSignType& s, OCNoteList& NoteList, OCDraw& ScreenObj)
{
    return OCNoteList::PlotSlur(NoteList.CreateList(0,int(s.RemainingTicks)), s.XMLSymbol, true, ScreenObj);
}


QList<OCToolButtonProps*> CStopped::CreateButtons()
{
    CreateButton("Note","Times new Roman",20,"+",false,false);
    return m_ButtonList;
}

void CStopped::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CStopped(Symbol));
}

OCGraphicsList CStopped::PrintSign(StemDirection UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    PrintProps.moveToVertical(UpDown, (18 + (SignsUp * 12)) * 12, ScreenObj);
#ifndef __Lelandfont
    return ScreenObj.plLet(MakeUnicode(QChar(uint(OCTTFStopped))), PrintProps.size(), OCTTFname, false, false, 1200, Qt::AlignHCenter);
#else
    ScreenObj.move(-24,-144);
    return ScreenObj.plLet(LelandStopped, PrintProps.size());
#endif
}


QList<OCToolButtonProps*> CHarmonic::CreateButtons()
{
    CreateButton("Note","Wingdings 2",10,QChar(0XF09A),false,false);
    return m_ButtonList;
}

void CHarmonic::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CHarmonic(Symbol));
}

OCGraphicsList CHarmonic::PrintSign(StemDirection UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    PrintProps.moveToVertical(UpDown, (18 + (SignsUp * 12)) * 12, ScreenObj);
#ifndef __Lelandfont
    ScreenObj.move(0,-16 * 12);
    return ScreenObj.plLet(QChar(0XF09A), PrintProps.size(), "Wingdings 2", false, false, 96, Qt::AlignHCenter);
#else
    ScreenObj.move(-24,-144);
    return ScreenObj.plLet(LelandStopped, PrintProps.size());
#endif
}


QList<OCToolButtonProps*> CComma::CreateButtons()
{
    CreateButton("Note","Times new Roman",24,",",true,false);
    return m_ButtonList;
}

void CComma::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CComma(Symbol));
}

OCGraphicsList CComma::PrintSign(StemDirection /*UpDown*/, int& /*SignsUp*/, OCDraw& ScreenObj)
{
    //PrintProps.moveToVertical(UpDown, (18 + (SignsUp * 12)) * 12, ScreenObj);
    PrintProps.moveTo(ScreenObj);
    ScreenObj.move(-20 * 12, 96);
    return ScreenObj.plLet(",", PrintProps.size(), "Times New Roman", true, false, 360, Qt::AlignHCenter);
}

const QStringList CTempoChange::TempoChangeList{"a tempo", "rit", "accel"};

QList<OCToolButtonProps*> CTempoChange::CreateButtons()
{
    CreateButton("Tempo","Times new Roman",11,"a\ntempo",true,false,false,tsRedrawActiveStave,"Add a tempo");
    CreateButton("Tempo","Times new Roman",13,"rit",true,false,false,tsRedrawActiveStave,"Add Ritardando");
    CreateButton("Tempo","Times new Roman",13,"accel",true,false,false,tsRedrawActiveStave,"Add Accelerando");
    return m_ButtonList;
}

void CTempoChange::ModifyProperties(OCProperties& p)
{
    p.hide("Speed", p.propertyValue("TempoType").toInt() == 0);
}

OCProperties* CTempoChange::GetDefaultProperties(int Button)
{
    CSymbol::GetDefaultProperties(Button);
    switch (Button)
    {
    case 1:
        return OCPresets().SetPropertyValue(&m_PropColl,"Speed","ritspeed");
    case 2:
        return OCPresets().SetPropertyValue(&m_PropColl,"Speed","accelspeed");
    }
    return &m_PropColl;
}

void CTempoChange::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &voiceVars)
{
    SignsToPlay.remove(name());
    voiceVars.Playtempo = voiceVars.HoldTempo;
    MFile.appendTempoEvent(voiceVars.Playtempo);
    voiceVars.CurrentDelta = 0;
    if (Symbol.getIntVal("TempoType") == 0) //'a tempo
    {
        voiceVars.Accel = 0;
        return;
    }
    if (Symbol.getIntVal("TempoType") == 1) //'rit
    {
        voiceVars.Accel = Symbol.getIntVal("Speed");
    }
    else if (Symbol.getIntVal("TempoType") == 2) //'accel
    {
        voiceVars.Accel = -Symbol.getIntVal("Speed");
    }
    int Modulate = IntDiv(voiceVars.Playtempo * 8, Symbol.getIntVal("Speed"));
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.append(NotDecrementable, Modulate, new CTempoChange(Symbol));
}

OCGraphicsList CTempoChange::PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int /*stavedistance*/, OCPrintVarsType &/*voiceVars*/, const XMLScoreWrapper& Score, OCDraw& ScreenObj)
{
    OCGraphicsList l;
    ScreenObj.moveTo(XFysic, ScoreTempoY, Symbol);
    switch (Symbol.getIntVal("TempoType"))
    {
    case 0:
        l.append(ScreenObj.plLet("a tempo", Symbol.size(), Score.TempoFont.font()));
        break;
    case 1:
        l.append(ScreenObj.plLet("rit", Symbol.size(), Score.TempoFont.font()));
        break;
    case 2:
        l.append(ScreenObj.plLet("accel", Symbol.size(), Score.TempoFont.font()));
        break;
    }
    return l;
}

void CTempoChange::fib(const XMLSymbolWrapper& /*Symbol*/, OCPrintVarsType& voiceVars)
{
    voiceVars.MasterStuff=true;
}

void CTempoChange::DuringNote(OCMIDIFile& MFile, int /*Pitch*/, int &LastTime, int /*Tick*/, int /*PlayTime*/, OCPlayBackVarsType &voiceVars)
{
    if (XMLIntValue("TempoType") == 1) //'rit
    {
        voiceVars.Playtempo--;
    }
    else if (XMLIntValue("TempoType") == 2) //'accel
    {
        voiceVars.Playtempo++;
    }
    if (voiceVars.Playtempo < 20) voiceVars.Playtempo = 20;
    MFile.appendTempoEvent(voiceVars.Playtempo,LastTime);
    LastTime = 0;
}



QList<OCToolButtonProps*> CText::CreateButtons()
{
    CreateButton("Staff","Times new Roman",13,"Text",false,false,true);
    return m_ButtonList;
}

void CText::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    XMLTextElementWrapper t(Symbol);
    /*
    QString Text=Symbol.attribute("Text");
    QFont Font(Symbol.attribute("FontName"),Symbol.getVal("FontSize"));
    Font.setBold(Symbol.getVal("FontBold"));
    Font.setItalic(Symbol.getVal("FontItalic"));
    */
    CEditDialog d(parent);
    d.setWindowTitle(name());
    d.ShowList("Text");
    d.EditWidget->PutText(t);
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc)
    {
        d.EditWidget->GetText(t);
        /*
        Symbol.setAttribute("Text",Text);
        Symbol.setAttribute("FontName",Font.family());
        Symbol.setAttribute("FontSize",Font.pointSizeF());
        Symbol.setAttribute("FontBold",Font.bold());
        Symbol.setAttribute("FontItalic",Font.italic());
        */
    }
    RefreshMode = tsRedrawActiveStave;
}

OCGraphicsList CText::PlotText(const XMLSymbolWrapper& Symbol, double XFysic, OCDraw& ScreenObj)
{
    OCGraphicsList l;
    XMLTextElementWrapper t(Symbol);
    if (!t.empty())
    {
        ScreenObj.moveTo(XFysic,ScoreTopSymbolY,Symbol);
        l.append(ScreenObj.plLet(t.text(), Symbol.size(), t.fontName(), t.bold(), t.italic(), t.fontSize() * 10));
    }
    return l;
}

OCGraphicsList CText::plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& /*BarList*/, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType &/*voiceVars*/, const XMLTemplateStaffWrapper & /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
    if (!Symbol.getBoolVal("Master")) return PlotText(Symbol, XFysic, ScreenObj);
    return OCGraphicsList();
}

OCGraphicsList CText::PlotMTrack(double XFysic, const XMLSymbolWrapper& Symbol, int /*stavedistance*/, OCPrintVarsType &/*voiceVars*/, const XMLScoreWrapper& /*Score*/, OCDraw& ScreenObj)
{
    if (Symbol.getBoolVal("Master")) return PlotText(Symbol, XFysic, ScreenObj);
    return OCGraphicsList();
}

void CText::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    if (Symbol.getBoolVal("Master")) voiceVars.MasterStuff=true;
}



QList<OCToolButtonProps*> CTremolo::CreateButtons()
{
    CreateButton("Note",":/Notes/Notes/tremolo.png");
    return m_ButtonList;
}

OCProperties* CTremolo::GetDefaultProperties(int Button)
{
    return OCPresets().SetPropertyValue(CSymbol::GetDefaultProperties(Button),"Speed","trillspeed");
}

void CTremolo::Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &voiceVars)
{
    int Modulate = IntDiv(voiceVars.Playtempo * 30, Symbol.getIntVal("Speed"));
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.append(KillInstantly, Modulate, new CTremolo(Symbol));
}

void CTremolo::DuringNote(OCMIDIFile& MFile, int Pitch, int &LastTime, int /*Tick*/, int /*PlayTime*/, OCPlayBackVarsType &voiceVars)
{
    MFile.appendNoteOffEvent(voiceVars.MIDI.Channel, Pitch, voiceVars.Currentdynam, LastTime);
    MFile.appendNoteOnEvent(voiceVars.MIDI.Channel, Pitch, voiceVars.Currentdynam);
    LastTime = 0;
}

void CTremolo::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CTremolo(Symbol));
}

OCGraphicsList CTremolo::PrintSign(StemDirection UpDown, int &/*SignsUp*/, OCDraw& ScreenObj)
{
    PrintProps.moveTo((12 * -4) * UpDown, 144 + ((12 * -14) * UpDown), ScreenObj);
    return ScreenObj.plLet(MakeUnicode(QChar(OCTTFTremolo0 + XMLIntValue("Beams"))), PrintProps.size(), OCTTFname, false, false, 1200, Qt::AlignHCenter);
}



QList<OCToolButtonProps*> CTrill::CreateButtons()
{
    CreateButton("Note",":/Notes/Notes/trill.png");
    CreateButton("Note",":/Notes/Notes/trillb.png",false,tsRedrawActiveStave,"Add Trill b");
    CreateButton("Note",":/Notes/Notes/trillx.png",false,tsRedrawActiveStave,"Add Trill #");
    return m_ButtonList;
}

int CTrill::TrillDir()
{
    int Direction = 1;
    if (XMLIntValue("StartFromAbove")) Direction = -1;
    return XMLIntValue("Range") * Direction;
}

void CTrill::Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int &/*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType &voiceVars)
{
    SignsToPlay.remove(name());
    int Modulate = IntDiv(voiceVars.Playtempo * 30, Symbol.getIntVal("Speed"));
    if (Modulate < 1) Modulate = 1;
    SignsToPlay.append(KillInstantly, Modulate, new CTrill(Symbol));
}

void CTrill::appendSign(const XMLSymbolWrapper& Symbol, OCPrintSignList& SignsToPrint, const QColor& SignCol, const OCBarSymbolLocation& Location)
{
    SignsToPrint.append(SignCol, Location, new CTrill(Symbol));
}

OCGraphicsList CTrill::PrintSign(StemDirection UpDown, int &SignsUp, OCDraw& ScreenObj)
{
    SignsUp++;
    int Sign=OCTTFTrill;
    switch (XMLIntValue("TrillType"))
    {
    case 1:
        Sign=OCTTFTrillb;
        break;
    case 2:
        Sign=OCTTFTrillSharp;
        break;
    }
    PrintProps.moveToVertical(UpDown, (18 + (SignsUp * 12)) * 12, ScreenObj);
    return ScreenObj.plLet(MakeUnicode(QChar(Sign)), PrintProps.size(), OCTTFname, false, false, 1200);
}

void CTrill::DuringNote(OCMIDIFile& MFile, int /*Pitch*/, int &LastTime, int Tick, int PlayTime, OCPlayBackVarsType &voiceVars)
{
    if (CurrentPitch == BasePitch)
    {
        CurrentPitch = BasePitch + qAbs<int>(TrillDir());
    }
    else
    {
        CurrentPitch = BasePitch;
    }
    if (!FinishedPlaying)
    {
        if (voiceVars.PortamentoOn)
        {
            if ((PlayTime - Tick >= PlayProps.Modulate) && (PlayTime - Tick <= PlayProps.Modulate * 2) && (CurrentPitch == BasePitch))
            {
                MFile.appendNoteOnEvent(voiceVars.MIDI.Channel, CurrentPitch, voiceVars.Currentdynam - trilldynam, LastTime);
                MFile.appendNoteOffEvent(voiceVars.MIDI.Channel, oldpitch, voiceVars.Currentdynam - trilldynam);
                LastTime = 0;
                FinishedPlaying = true;
            }
            else
            {
                MFile.appendNoteOnEvent(voiceVars.MIDI.Channel, CurrentPitch, voiceVars.Currentdynam - trilldynam, LastTime);
                MFile.appendNoteOffEvent(voiceVars.MIDI.Channel, oldpitch, voiceVars.Currentdynam - trilldynam);
                if (PlayTime - Tick >= PlayProps.Modulate)
                {
                    MFile.appendPortamentoEvent(voiceVars.MIDI.Channel, CurrentPitch);
                    trilldynam++;
                }
                else
                {
                    FinishedPlaying = true;
                }
                LastTime = 0;
            }
        }
        else
        {
            MFile.appendNoteOffEvent(voiceVars.MIDI.Channel, oldpitch, voiceVars.Currentdynam - trilldynam, LastTime);
            MFile.appendNoteOnEvent(voiceVars.MIDI.Channel, CurrentPitch, voiceVars.Currentdynam - trilldynam);
            LastTime = 0;
            trilldynam++;
            if ((PlayTime - Tick < PlayProps.Modulate * 2) && (CurrentPitch == BasePitch)) FinishedPlaying = true;
        }
    }
    oldpitch = CurrentPitch;
}

void CTrill::BeforeNote(const XMLSymbolWrapper& XMLNote, int &/*PlayDynam*/, int &Pitch, int &/*endPitch*/, OCPlayBackVarsType &/*voiceVars*/)
{
    BasePitch = Pitch;
    if (XMLNote.IsValuedNote())
    {
        if (TrillDir() < 0) Pitch = Pitch - TrillDir();
    }
    CurrentPitch = Pitch;
    oldpitch = Pitch;
}

OCProperties* CTrill::GetDefaultProperties(int Button)
{
    CSymbol::GetDefaultProperties(Button);
    if (Button==1) m_PropColl.setPropertyValue("Range",1);
    return OCPresets().SetPropertyValue(&m_PropColl,"Speed","trillspeed");
}



QList<OCToolButtonProps*> CStemDirection::CreateButtons()
{
    CreateButton("Staff",":/Notes/Notes/stemdirection.png",true,tsRedrawActiveStave,"Add Stem Direction");
    return m_ButtonList;
}

void CStemDirection::Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode &RefreshMode, bool &esc, QWidget *parent)
{
    CEditDialog d(parent);
    d.setWindowTitle(name());
    d.EditWidget->PutCombo("Stem Direction",Symbol.getIntVal("Direction"),DirectionList);
    d.QuickAccept();
    esc=(d.exec()!=QDialog::Accepted);
    if (!esc) Symbol.setAttribute("Direction",d.EditWidget->GetCombo());
    RefreshMode = tsRedrawActiveStave;
}

void CStemDirection::fib(const XMLSymbolWrapper& Symbol, OCPrintVarsType& voiceVars)
{
    int Direction = Symbol.getIntVal("Direction");
    if (Direction == 2) Direction = -1;
    voiceVars.UpDown.setCurrent(Symbol,Direction);
}

QList<OCToolButtonProps*> CPedal::CreateButtons()
{
    QStringList PedalDirections = {"pedaldown","pedalup"};
    for (int i=0;i<2;i++) CreateButton("Pedal",":/Notes/Notes/"+PedalDirections[i]+".png",false,tsRedrawActiveStave,"Add "+PedalList[i]);
    return m_ButtonList;
}

OCGraphicsList CPedal::plot(const XMLSymbolWrapper& Symbol, double XFysic, OCPageBarList& /*BarList*/, OCCounter& /*CountIt*/, OCPrintSignList& /*SignsToPrint*/, QColor /*SignCol*/, const XMLScoreWrapper& /*Score*/, OCNoteList& /*NoteList*/, OCPrintVarsType& /*voiceVars*/, const XMLTemplateStaffWrapper& /*XMLTemplateStaff*/, OCDraw& ScreenObj)
{
#ifndef __Lelandfont
    QString a = (Symbol.getIntVal("PedalSign")==0) ? QChar(uint(OCTTFPedalDown)) : QChar(uint(OCTTFPedalUp));
    ScreenObj.moveTo(XFysic, ScoreBottomSymbolY, Symbol);
    return ScreenObj.plLet(MakeUnicode(a),Symbol.size(),OCTTFname,false,false,1200,Qt::AlignRight | Qt::AlignBottom);
#else
    Leland l = (Symbol.getIntVal("PedalSign")==0) ? LelandPedalDown : LelandPedalUp;
    ScreenObj.moveTo(XFysic, ScoreBottomSymbolY, Symbol);
    return ScreenObj.plLet(l, Symbol.size(), LelandDefaultSize, Qt::AlignRight | Qt::AlignBottom);
#endif
}

void CPedal::fibPlay(const XMLSymbolWrapper& Symbol, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& SignsToPlay, OCPlayBackVarsType& /*voiceVars*/)
{
    SignsToPlay.remove("Pedal");
    if (Symbol.getIntVal("PedalSign")==0)
    {
        SignsToPlay.append(NotDecrementable, 0, new CPedal(Symbol));
    }
}

void CPedal::Play(const XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& /*CountIt*/, int& /*Py*/, const XMLVoiceWrapper& /*XMLVoice*/, OCPlaySignList& /*SignsToPlay*/, OCPlayBackVarsType & voiceVars)
{
    if (Symbol.getIntVal("PedalSign")==1)
    {
        for (int i = voiceVars.PedalNotes.size()-1; i >= 0; i--)
        {
            qDebug() << "Note off 0" << voiceVars.PedalNotes[i];
            MFile.appendNoteOffEvent(voiceVars.MIDI.Channel, voiceVars.PedalNotes[i], 0,voiceVars.CurrentDelta);
            voiceVars.CurrentDelta=0;
            voiceVars.PedalNotes.removeAt(i);
        }
    }
}


CNote::~CNote() {}
