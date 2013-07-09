#include "csymbol.h"

OCProperty::OCProperty()
{
    Name.clear();
    Min="";
    Max="";
    PropertyType=pwText;
    Description.clear();
    Default="";
    Hidden=false;
    Category.clear();
    PermHidden=false;
    ListOffset=0;
}

void OCProperty::AddToList(const QString& Text)
{
    m_sList.append(Text);
}

void OCProperty::AddToList(const QStringList& Lst)
{
    m_sList.append(Lst);
}

const QString OCProperty::ListItem(const int Index) const
{
    return m_sList[Index];
}

const int OCProperty::ListCount() const
{
    return m_sList.count();
}

const QStringList& OCProperty::List() const
{
    return m_sList;
}

const QVariant OCProperty::GetValue() const
{
    return m_Value;
}

void OCProperty::SetValue(const QVariant& vData)
{
    if (PropertyType==pwList)
    {
        if ((vData.toInt()-ListOffset >= 0) && (vData.toInt()-ListOffset < m_sList.count()))
        {
            m_Value=vData.toInt();
            return;
        }
        else
        {
            m_Value=m_sList.indexOf(vData.toString())+ListOffset;
            return;
        }
    }
    m_Value=vData;
}

const QString OCProperty::TextValue() const
{
    QString Value=m_Value.toString();
    switch (PropertyType)
    {
    case pwList:
        Value=m_sList[m_Value.toInt()-ListOffset];
        break;
    case pwBoolean:
        Value=(m_Value.toBool() ? QChar(0xF0FE):QChar(0xF0A8));
        break;
    case pwNumber:
        Value=QString::number(m_Value.toInt());
        break;
    case pwSlider:
        Value="";
        break;
    case pwCustom:
        Value="Custom";
        break;
    case pwText:
        break;
    }
    return Value;
}

OCProperty* OCProperties::Add(const QString& Name, const OCPropertyType PropertyType, const QVariant& Min, const QVariant& Max, const QString& Description, const QVariant& List, const int ListOffset, const bool Hidden, const QVariant& Value, const QString& Category)
{
    //'create a new object
    OCProperty* objNewMember = new OCProperty;
    //'set the properties passed into the method
    objNewMember->Name = Name;
    objNewMember->PropertyType = PropertyType;
    objNewMember->Min = Min;
    objNewMember->Max = Max;
    objNewMember->Description = Description;
    if (List != "")
    {
        objNewMember->AddToList(List.toStringList());
        objNewMember->ListOffset=ListOffset;
    }
    objNewMember->SetValue(Value);
    objNewMember->Default = Value;
    objNewMember->Hidden = Hidden;
    objNewMember->PermHidden = Hidden;
    objNewMember->Category = Category;
    mCol.insert(Name, objNewMember);
    return objNewMember;
}

OCProperty* OCProperties::GetItem(const QString& Name)
{
    return mCol[Name];
}

OCProperty* OCProperties::GetItem(const int Index)
{
    return mCol.values()[Index];
}

OCProperties::OCProperties()
{
}

OCProperties::~OCProperties()
{
    qDeleteAll(mCol);
    mCol.clear();
}

const QVariant OCProperties::GetValue(const QString& Name) const
{
    return mCol[Name]->GetValue();
}

void OCProperties::SetValue(const QString& Name, const QVariant& vData)
{
    mCol[Name]->SetValue(vData);
}

const int OCProperties::Count() const
{
    return mCol.count();
}

void OCProperties::Remove(const QString& Name)
{
    delete mCol[Name];
    mCol.remove(Name);
}

const bool OCProperties::Exist(const QString& Name) const
{
    return mCol.contains(Name);
}

void OCProperties::Reset()
{
    foreach(OCProperty* p,mCol) p->SetValue(p->Default);
}

void OCProperties::FromXML(QDomLiteElement* XML)
{
    this->Name=XML->attribute("SymbolName");
    foreach(OCProperty* p,mCol)
    {
        p->SetValue(XML->attribute(p->Name));
    }
}

QDomLiteElement* OCProperties::ToXML() const
{
    QDomLiteElement* e=new QDomLiteElement("Symbol");
    e->setAttribute("SymbolName",this->Name);
    foreach(OCProperty* p,mCol)
    {
        e->setAttribute(p->Name,p->GetValue());
    }
    return e;
}

const QString OCProperties::TextValue(const int Index) const
{
    return mCol.values()[Index]->TextValue();
}

CustomEdit::CustomEdit(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout* layout=new QHBoxLayout;
    this->setLayout(layout);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    lbl=new QLabel(this);
    lbl->show();
    layout->addWidget(lbl);
    btn=new QToolButton(this);
    btn->setFixedSize(this->height(),this->height());
    btn->setText("...");
    btn->show();
    layout->addWidget(btn);
}
/*
CustomManager::CustomManager(QObject *parent) : QtAbstractPropertyManager(parent)
{

}

QString CustomManager::value(const QtProperty *property)
const
{
    if (!theValues.contains(property)) return "";
    return theValues[property].value;
}

void CustomManager::setValue(QtProperty *property, const QString val)
{
    if (!theValues.contains(property)) return;
    Data data = theValues[property];
    data.value = val;
    theValues[property] = data;
    emit propertyChanged(property);
    emit valueChanged(property, data.value);
}

CustomEditFactory::CustomEditFactory(QObject *parent) : QtAbstractEditorFactory<CustomManager>(parent)
{

}

CustomEdit* CustomEditFactory::createEditor(CustomManager *manager, QtProperty *property, QWidget *parent)
{
    CustomEdit *editor = new CustomEdit(parent);
    editor->lbl->setText(manager->value(property));
    createdEditors[property].append(editor);
    editorToProperty[editor] = property;
    connect(editor, SIGNAL(valueChanged(const QString &)),
            this, SLOT(slotSetValue(const QString &)));
    connect(editor, SIGNAL(destroyed(QObject *)),
            this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

void CustomEditFactory::connectPropertyManager(CustomManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
            this, SLOT(slotPropertyChanged(QtProperty *, const QString &)));
}

void CustomEditFactory::disconnectPropertyManager(CustomManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
               this, SLOT(slotPropertyChanged(QtProperty *, const QString &)));
}

void CustomEditFactory::slotPropertyChanged(QtProperty *property,
                                            const QString &value)
{
}

void CustomEditFactory::slotSetValue(const QString &value)
{
}

void CustomEditFactory::slotEditorDestroyed(QObject *object)
{
}

void OCPropertiesX::propertyChanged(QtProperty* p, int value)
{
    SetValue(p->propertyName(),value);
    emit dataChanged(p->propertyName());
}

void OCPropertiesX::propertyChanged(QtProperty* p, QString value)
{
    SetValue(p->propertyName(),value);
    emit dataChanged(p->propertyName());
}

void OCPropertiesX::propertyChanged(QtProperty* p, QVariant value)
{
    SetValue(p->propertyName(),value);
    emit dataChanged(p->propertyName());
}

void OCPropertiesX::fillPropertyBrowser(QtAbstractPropertyBrowser *pb)
{
    sliderManager.disconnect(this);
    propertyManager.disconnect(this);
    customManager.disconnect(this);
    pb->clear();
    pb->setFactoryForManager(&propertyManager,&editorFactory);
    pb->setFactoryForManager(&customManager,customFactory);
    pb->setFactoryForManager(&sliderManager, &sliderFactory);
    QMap<QString,QtProperty*> cats;
    foreach(OCProperty* p,mCol)
    {
        if (!p->Hidden)
        {
            if (!cats.contains(p->Category))
            {
                cats[p->Category]=groupManager.addProperty(p->Category);
            }
            QtProperty* item=0;
            QtVariantProperty* qp=0;
            switch (p->PropertyType)
            {
            case pwList:
                qp=propertyManager.addProperty(QtVariantPropertyManager::enumTypeId(),p->Name);
                qp->setAttribute("enumNames", p->List());
                qp->setValue(p->GetValue()); // "Suggestion"
                //item=enumManager.addProperty(p->Name);
                //enumManager.setEnumNames(item,p->List());
                //enumManager.setValue(item,p->GetValue().toInt());
                break;
            case pwBoolean:
                qp=propertyManager.addProperty(QVariant::Bool,p->Name);
                qp->setValue(p->GetValue());
                //item=boolManager.addProperty(p->Name);
                //boolManager.setValue(item,p->GetValue().toBool());
                break;
            case pwNumber:
                qp=propertyManager.addProperty(QVariant::Int,p->Name);
                qp->setAttribute("minimum", p->Min);
                qp->setAttribute("maximum", p->Max);
                qp->setValue(p->GetValue());
                //item=intManager.addProperty(p->Name);
                //intManager.setRange(item,p->Min.toInt(),p->Max.toInt());
                //intManager.setValue(item,p->GetValue().toInt());
                break;
            case pwSlider:
                item=sliderManager.addProperty(p->Name);
                sliderManager.setRange(item,p->Min.toInt(),p->Max.toInt());
                sliderManager.setValue(item,p->GetValue().toInt());
                break;
            case pwCustom:
                item=customManager.addProperty(p->Name);
                customManager.setValue(item,p->TextValue());
                break;
            case pwText:
                qp=propertyManager.addProperty(QVariant::String,p->Name);
                qp->setValue(p->GetValue());
                //item=stringManager.addProperty(p->Name);
                //stringManager.setValue(item,p->GetValue().toString());
                break;
            }
            if(qp) cats[p->Category]->addSubProperty(qp);
            if (item) cats[p->Category]->addSubProperty(item);
        }
    }
    foreach (QtProperty* p,cats)
    {
        pb->addProperty(p);
    }
    connect(&propertyManager,SIGNAL(valueChanged(QtProperty*,QVariant)),this,SLOT(propertyChanged(QtProperty*,QVariant)));
    connect(&sliderManager,SIGNAL(valueChanged(QtProperty*,int)),this,SLOT(propertyChanged(QtProperty*,int)));
    connect(&customManager,SIGNAL(valueChanged(QtProperty*,QString)),this,SLOT(propertyChanged(QtProperty*,QString)));
}

OCPropertiesX::OCPropertiesX(QObject* parent) : QObject(parent)
{
    customFactory=new CustomEditFactory(this);
}
*/
OCPresets::OCPresets(const QString& tag)
{
    MustSaveProperties=false;
    Tag=tag;
    if (Tag=="Presets") SetDefaults();
}

void OCPresets::SaveProperties()
{
    if (MustSaveProperties)
    {
        OCProperties* p=m_PropColl;
        SetValue("crescspeed",p->GetValue("CrescendoSpeed"));
        SetValue("dimspeed",p->GetValue("DiminuendoSpeed"));
        SetValue("accelspeed",p->GetValue("AccelerandoSpeed"));
        SetValue("ritspeed",p->GetValue("RitardandoSpeed"));
        SetValue("stacclen",p->GetValue("StaccatoLength"));
        SetValue("legatolen",p->GetValue("LegatoLength"));
        SetValue("pppvel",p->GetValue("pppVelocity"));
        SetValue("ppvel",p->GetValue("ppVelocity"));
        SetValue("pvel",p->GetValue("pVelocity"));
        SetValue("mpvel",p->GetValue("mpVelocity"));
        SetValue("mfvel",p->GetValue("mfVelocity"));
        SetValue("fvel",p->GetValue("fVelocity"));
        SetValue("ffvel",p->GetValue("ffVelocity"));
        SetValue("fffvel",p->GetValue("fffVelocity"));
        SetValue("accentadd",p->GetValue("AccentAdd"));
        SetValue("fzadd",p->GetValue("fzAdd"));
        SetValue("fpstartvel",p->GetValue("fpStartVelocity"));
        SetValue("fpendvel",p->GetValue("fpEndVelocity"));
        SetValue("trillspeed",p->GetValue("TrillSpeed"));
    }
}

OCPresets::~OCPresets()
{
    if (MustSaveProperties) delete m_PropColl;
}

const QVariant OCPresets::GetValue(const QString& Preset) const
{
    return Settings.value(Tag+"/"+Preset);
}

const QVariant OCPresets::GetValue(const QString& Preset, const QVariant& Default) const
{
    return Settings.value(Tag+"/"+Preset,Default);
}

const QString OCPresets::GetString(const QString& Preset) const
{
    return Settings.value(Tag+"/"+Preset).toString();
}

const QString OCPresets::GetString(const QString& Preset, const QString& Default) const
{
    return Settings.value(Tag+"/"+Preset,Default).toString();
}

void OCPresets::SetValue(const QString& Preset, const QVariant& Value)
{
    Settings.setValue(Tag+"/"+Preset,Value);
}

OCProperties* OCPresets::SetPropertyValue(OCProperties* p, const QString& PropertyName, const QString& Preset)
{
    if (p->Exist(PropertyName)) p->SetValue(PropertyName,GetValue(Preset));
    return p;
}

OCProperties* OCPresets::Properties()
{
    MustSaveProperties=true;
    m_PropColl=new OCProperties();
    OCProperties* p=m_PropColl;
    p->Name="Presets";
    p->Add("CrescendoSpeed",pwNumber,1,100,"Sets the Speed of Crescendos relative to the Tempo","",0,false,GetValue("crescspeed",50),"Dynamics");
    p->Add("DiminuendoSpeed",pwNumber,1,100,"Sets the Speed of Diminuendos relative to the Tempo","",0,false,GetValue("dimspeed",50),"Dynamics");
    p->Add("AccelerandoSpeed",pwNumber,1,100,"Sets the Speed of Accelerandos relative to the Tempo","",0,false,GetValue("accelspeed",50),"Tempo");
    p->Add("RitardandoSpeed",pwNumber,1,100,"Sets the Speed of Ritardandos relative to the Tempo","",0,false,GetValue("ritspeed",50),"Tempo");
    p->Add("StaccatoLength",pwNumber,1,100,"Sets the Length of Staccato Notes in Percent","",0,false,GetValue("stacclen",30),"Articulation");
    p->Add("LegatoLength",pwNumber,1,100,"Sets the Length of Legato Notes in Percent","",0,false,GetValue("legatolen",99),"Articulation");
    p->Add("pppVelocity",pwNumber,1,127,"Sets the Velocity of the ppp Sign","",0,false,GetValue("pppvel",6),"Dynamics");
    p->Add("ppVelocity",pwNumber,1,127,"Sets the Velocity of the pp Sign","",0,false,GetValue("ppvel",22),"Dynamics");
    p->Add("pVelocity",pwNumber,1,127,"Sets the Velocity of the p Sign","",0,false,GetValue("pvel",38),"Dynamics");
    p->Add("mpVelocity",pwNumber,1,127,"Sets the Velocity of the mp Sign","",0,false,GetValue("mpvel",54),"Dynamics");
    p->Add("mfVelocity",pwNumber,1,127,"Sets the Velocity of the mf Sign","",0,false,GetValue("mfvel",70),"Dynamics");
    p->Add("fVelocity",pwNumber,1,127,"Sets the Velocity of the f Sign","",0,false,GetValue("fvel",86),"Dynamics");
    p->Add("ffVelocity",pwNumber,1,127,"Sets the Velocity of the ff Sign","",0,false,GetValue("ffvel",102),"Dynamics");
    p->Add("fffVelocity",pwNumber,1,127,"Sets the Velocity of the fff Sign","",0,false,GetValue("fffvel",118),"Dynamics");
    p->Add("AccentAdd",pwNumber,1,127,"Sets the amount ov Velocity an Accent will Add to the Current Velocity","",0,false,GetValue("accentadd",20),"Dynamics");
    p->Add("fzAdd",pwNumber,1,127,"Sets the amount ov Velocity an fz Sign will Add to the Current Velocity","",0,false,GetValue("fzadd",30),"Dynamics");
    p->Add("fpStartVelocity",pwNumber,1,127,"Sets the Velocity at the beginning of a Note with an fp Sign","",0,false,GetValue("fpstartvel",86),"Dynamics");
    p->Add("fpEndVelocity",pwNumber,1,127,"Sets the Velocity at the end of a Note with an fp Sign","",0,false,GetValue("fpendvel",38),"Dynamics");
    p->Add("TrillSpeed",pwNumber,1,300,"Sets the Speed of Trills relative to the Tempo","",0,false,GetValue("trillspeed",90),"Articulation");
    p->Add("test",pwCustom,0,0,"Test","",0,false,"0","1");
    return m_PropColl;
}

void OCPresets::SetDefaults()
{
    if (Settings.contains("Presets/pvel")) return;
    SetValue("crescspeed",50);
    SetValue("dimspeed",50);
    SetValue("accelspeed",50);
    SetValue("ritspeed",50);
    SetValue("stacclen",30);
    SetValue("legatolen",99);
    SetValue("pppvel",6);
    SetValue("ppvel",22);
    SetValue("pvel",38);
    SetValue("mpvel",54);
    SetValue("mfvel",70);
    SetValue("fvel",86);
    SetValue("ffvel",102);
    SetValue("fffvel",118);
    SetValue("fpstartvel",86);
    SetValue("fpendvel",38);
    SetValue("accentadd",20);
    SetValue("fzadd",30);
    SetValue("trillspeed",90);
}

CSymbol::CSymbol(const QString& Name)
{
    m_Name=Name;
    m_PropColl=new OCProperties;
}

CSymbol::~CSymbol()
{
    delete m_PropColl;
}

bool CSymbol::MTColorCheck(OCDraw& ScreenObj)
{
    return (ScreenObj.col != inactivestaffcolor);
}

void CSymbol::PrintSign(OCSymbolArray& /*SymbolList*/, PrintSignProps& /*SignProps*/, int /*UpDown*/, int &/*SignsUp*/, OCDraw& /*ScreenObj*/){}

void CSymbol::fibPlay(XMLSymbolWrapper& /*Symbol*/, OCMIDIFile& /*MFile*/, OCCounter& /*CountIt*/, int& /*Py*/, QDomLiteElement* /*XMLVoice*/, OCSignList& /*SignsToPlay*/, OCPlayBackVarsType &/*TemPlay*/){}

void CSymbol::BeforeNote(XMLSymbolWrapper& /*XMLNote*/, int& /*PlayDynam*/, int& /*Pitch*/, int& /*endPitch*/, OCMIDIFile& /*MFile*/, OCPlayBackVarsType &/*TemPlay*/){}

void CSymbol::PlotMTrack(int /*XFysic*/, XMLSymbolWrapper& /*Symbol*/, int /*stavedistance*/, OCPrintVarsType &/*tempsetting*/, OCSymbolArray& /*MTObj*/, int /*Pointer*/, XMLScoreWrapper& /*Score*/, OCDraw& /*ScreenObj*/){}

void CSymbol::AfterNote(XMLSymbolWrapper& /*XMLNote*/, OCPlayBackVarsType &/*TemPlay*/){}

void CSymbol::plot(XMLSymbolWrapper& /*Symbol*/, int /*XFysic*/, OCBarList& /*BarList*/, OCCounter& /*CountIt*/, int /*BarCounter*/, OCSignList& /*SignsToPrint*/, QColor /*SignCol*/, XMLScoreWrapper& /*Score*/, int /*PointerStart*/, OCSymbolArray& /*SymbolList*/, int /*Stave*/, int /*Track*/, OCNoteList& /*NoteList*/, int /*NoteCount*/, OCPrintVarsType &/*dCurrent*/, OCPrintStaffVarsType & /*sCurrent*/, int /*Pointer*/, OCDraw& /*ScreenObj*/){}

void CSymbol::DuringNote(PlaySignProps& /*Props*/, OCMIDIFile& /*MFile*/, int /*Pitch*/, int& /*LastTime*/, int /*Tick*/, int /*PlayTime*/, OCPlayBackVarsType &/*TemPlay*/){}

void CSymbol::Edit(XMLSimpleSymbolWrapper& /*Symbol*/, OCRefreshMode& RefreshMode, bool& esc, QWidget* /*parent*/)
{
    esc = false;
    RefreshMode = tsReformat;
}

void CSymbol::DrawFactor(XMLSymbolWrapper& Symbol, OCCounter *Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score){}

void CSymbol::InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType &TempPlay){}

void CSymbol::Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType &TemPlay){}

void CSymbol::fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType &tempsetting){}

void CSymbol::InitPrintSymbol(XMLSymbolWrapper& Symbol, OCPrintVarsType &dCurrent){}

const QString CSymbol::Name() const{return m_Name;}

OCProperties* CSymbol::GetProperties()
{
    //m_PropColl->Reset();
    m_PropColl->Name = m_Name;
    return m_PropColl;
}

const bool CSymbol::PropertyExists(const QString &Name) const
{
    return m_PropColl->Exist(Name);
}

const QVariant CSymbol::PropertyValue(const QString &Name) const
{
    return m_PropColl->GetValue(Name);
}

void CSymbol::ModifyProperties(OCProperties* p)
{
    Q_UNUSED(p);
}
/*
OCProperties* CSymbol::GetProperties(QString SymbolName)
{
    Q_UNUSED(SymbolName);
    m_PropColl->Name = m_Name;
    m_PropColl->Reset();
    return m_PropColl;
}
*/
OCProperties* CSymbol::GetProperties(int Button)
{
    m_PropColl->Reset();
    m_PropColl->Name=m_Name;
    if (!m_ButtonProperty.isEmpty()) m_PropColl->SetValue(m_ButtonProperty,Button);
    return m_PropColl;
}
/*
int CSymbol::ButtonCount()
{
    return m_ButtonList.count();
}

OCToolButtonProps* CSymbol::ButtonProps(int Button)
{
    return m_ButtonList[Button];
}
*/
QList<OCToolButtonProps*> CSymbol::CreateButtons()
{
    return QList<OCToolButtonProps*>();
}

OCToolButtonProps* CSymbol::CreateButton(const QString& FontName, const float FontSize, const QString& Text, const bool FontBold, const bool FontItalic, const bool CustomDialog, const OCRefreshMode RefreshMode, const QString& Tooltip, const QString& ModifierName)
{
    OCToolButtonProps* p = new OCToolButtonProps(m_Name,m_ButtonList.count());
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
    if (p->tooltip.isEmpty()) p->tooltip="Add "+m_Name;
    return p;
}

OCToolButtonProps* CSymbol::CreateButton(const QString& IconPath, const bool CustomDialog, const OCRefreshMode RefreshMode, const QString& Tooltip, const QString& ModifierName)
{
    OCToolButtonProps* p = new OCToolButtonProps(m_Name,m_ButtonList.count());
    p->iconpath=IconPath;
    p->customdialog=CustomDialog;
    p->refreshmode=RefreshMode;
    p->tooltip=Tooltip;
    p->modifierProperty=ModifierName;
    m_ButtonList.append(p);
    if (p->tooltip.isEmpty()) p->tooltip="Add "+m_Name;
    return p;
}

CVisibleSymbol::CVisibleSymbol(const QString& Name, const bool NoInv, const bool NoIna, const bool NoSize):CSymbol(Name)
{
    //QStringList ListArr;
    //ListArr << "Default" << "-9" << "-8" << "-7" << "-6" << "-5" << "-4" << "-3" << "-2" << "-1" << "0 - Default" << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10";
    m_PropColl->Add("Top", pwNumber, -32000, 32000, "Returns/sets the distance between an objects default vertical position and it's current vertical position.", "", 0,false, "", "Position");
    m_PropColl->Add("Left", pwNumber, -32000, 32000, "Returns/sets the distance between an objects default horizontal position and it's current horizontal position.", "", 0,false, "", "Position");
    if (!NoSize)  m_PropColl->Add("Size", pwNumber, -9, 9, "Returns/sets the difference between an objects default Size and it's current Size", "", 0,false, "", "Position");
    if (!NoInv) m_PropColl->Add("Invisible", pwBoolean, "", "", "Returns or sets whether an object is Visible.", "", 0,false, false, "Behavior");
    if (!NoIna) m_PropColl->Add("Inaudible", pwBoolean, "", "", "Returns or sets whether an object is Audible.", "", 0,false, false, "Behavior");
}

CDuratedSymbol::CDuratedSymbol(const QString& Name, const bool NoInv, const bool NoIna, const bool NoSize):CVisibleSymbol(Name,NoInv,NoIna,NoSize)
{
    m_PropColl->Add("Ticks", pwNumber, 1, 32000, "Returns/sets the Duration of the Note Group in Ticks. (A Quarter note is 24 Ticks.)", "", 0,false, "", "Appearance");
}

CGapSymbol::CGapSymbol(const QString& Name, const QString& GapName, const QString& Description):CDuratedSymbol(Name,false,false,true)
{
    //QStringList ListArr;
    //ListArr << "Default" << "-9" << "-8" << "-7" << "-6" << "-5" << "-4" << "-3" << "-2" << "-1" << "0 - Default" << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10";
    m_PropColl->Add(GapName, pwNumber, -9, 9, Description, "", 0,false, "", "Appearance");
}

CTextSymbol::CTextSymbol(const QString& Name):CVisibleSymbol(Name,false,true)
{
    m_PropColl->Add("Custom", pwCustom, "", "", "Shows the Text Dialog", "", 0,false, "", "Appearance");
    m_PropColl->Add("Text", pwText, "", "", "Returns/sets the visible text", "", 0,false, "Text", "Appearance");
    m_PropColl->Add("FontName", pwText, "", "", "Returns/sets the name of the font used", "", 0,false, "Times new Roman", "Appearance");
    m_PropColl->Add("FontItalic", pwBoolean, "", "", "Returns/sets whether the text is shown as italic", "", 0,false, false, "Appearance");
    m_PropColl->Add("FontBold", pwBoolean, "", "", "Returns/sets whether the text is shown as bold", "", 0,false, false, "Appearance");
    m_PropColl->Add("FontSize", pwNumber, 8, 300, "Returns/sets the size of the text", "", 0,false, 8.25, "Appearance");
}

