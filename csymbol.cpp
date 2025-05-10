#include "csymbol.h"
#include <QHBoxLayout>

CustomEdit::CustomEdit(QWidget *parent) : QWidget(parent)
{
    auto layout=new QHBoxLayout;
    this->setLayout(layout);
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

OCPresets::OCPresets(const QString& tag)
{
    Settings = new OCSettings();
    MustSaveProperties=false;
    Tag=tag;
    if (Tag == "Presets") SetDefaults();
}

void OCPresets::SaveProperties()
{
    if (MustSaveProperties)
    {
        OCProperties* p=m_PropColl;
        SetValue("crescspeed",p->propertyValue("CrescendoSpeed"));
        SetValue("dimspeed",p->propertyValue("DiminuendoSpeed"));
        SetValue("accelspeed",p->propertyValue("AccelerandoSpeed"));
        SetValue("ritspeed",p->propertyValue("RitardandoSpeed"));
        SetValue("stacclen",p->propertyValue("StaccatoLength"));
        SetValue("legatolen",p->propertyValue("LegatoLength"));
        SetValue("pppvel",p->propertyValue("pppVelocity"));
        SetValue("ppvel",p->propertyValue("ppVelocity"));
        SetValue("pvel",p->propertyValue("pVelocity"));
        SetValue("mpvel",p->propertyValue("mpVelocity"));
        SetValue("mfvel",p->propertyValue("mfVelocity"));
        SetValue("fvel",p->propertyValue("fVelocity"));
        SetValue("ffvel",p->propertyValue("ffVelocity"));
        SetValue("fffvel",p->propertyValue("fffVelocity"));
        SetValue("accentadd",p->propertyValue("AccentAdd"));
        SetValue("fzadd",p->propertyValue("fzAdd"));
        SetValue("fpstartvel",p->propertyValue("fpStartVelocity"));
        SetValue("fpendvel",p->propertyValue("fpEndVelocity"));
        SetValue("trillspeed",p->propertyValue("TrillSpeed"));
    }
}

OCPresets::~OCPresets()
{
    if (MustSaveProperties) delete m_PropColl;
    delete Settings;
}

const QVariant OCPresets::GetValue(const QString& Preset) const
{
    return Settings->value(Tag+"/"+Preset);
}

const QVariant OCPresets::GetValue(const QString& Preset, const QVariant& Default) const
{
    return Settings->value(Tag+"/"+Preset,Default);
}

const QString OCPresets::GetString(const QString& Preset) const
{
    return Settings->value(Tag+"/"+Preset).toString();
}

const QString OCPresets::GetString(const QString& Preset, const QString& Default) const
{
    return Settings->value(Tag+"/"+Preset,Default).toString();
}

void OCPresets::SetValue(const QString& Preset, const QVariant& Value)
{
    Settings->setValue(Tag+"/"+Preset,Value);
}

OCProperties* OCPresets::SetPropertyValue(OCProperties* p, const QString& PropertyName, const QString& Preset)
{
    if (p->contains(PropertyName)) p->setPropertyValue(PropertyName,GetValue(Preset));
    return p;
}

OCProperties* OCPresets::Properties()
{
    MustSaveProperties=true;
    m_PropColl=new OCProperties();
    OCProperties* p=m_PropColl;
    p->Name="Presets";
    p->appendNumber("CrescendoSpeed",1,100,"Sets the Speed of Crescendos relative to the Tempo",false,GetValue("crescspeed",50),"Dynamics");
    p->appendNumber("DiminuendoSpeed",1,100,"Sets the Speed of Diminuendos relative to the Tempo",false,GetValue("dimspeed",50),"Dynamics");
    p->appendNumber("AccelerandoSpeed",1,100,"Sets the Speed of Accelerandos relative to the Tempo",false,GetValue("accelspeed",50),"Tempo");
    p->appendNumber("RitardandoSpeed",1,100,"Sets the Speed of Ritardandos relative to the Tempo",false,GetValue("ritspeed",50),"Tempo");
    p->appendNumber("StaccatoLength",1,100,"Sets the Length of Staccato Notes in Percent",false,GetValue("stacclen",30),"Articulation");
    p->appendNumber("LegatoLength",1,100,"Sets the Length of Legato Notes in Percent",false,GetValue("legatolen",99),"Articulation");
    p->appendNumber("pppVelocity",1,127,"Sets the Velocity of the ppp Sign",false,GetValue("pppvel",6),"Dynamics");
    p->appendNumber("ppVelocity",1,127,"Sets the Velocity of the pp Sign",false,GetValue("ppvel",22),"Dynamics");
    p->appendNumber("pVelocity",1,127,"Sets the Velocity of the p Sign",false,GetValue("pvel",38),"Dynamics");
    p->appendNumber("mpVelocity",1,127,"Sets the Velocity of the mp Sign",false,GetValue("mpvel",54),"Dynamics");
    p->appendNumber("mfVelocity",1,127,"Sets the Velocity of the mf Sign",false,GetValue("mfvel",70),"Dynamics");
    p->appendNumber("fVelocity",1,127,"Sets the Velocity of the f Sign",false,GetValue("fvel",86),"Dynamics");
    p->appendNumber("ffVelocity",1,127,"Sets the Velocity of the ff Sign",false,GetValue("ffvel",102),"Dynamics");
    p->appendNumber("fffVelocity",1,127,"Sets the Velocity of the fff Sign",false,GetValue("fffvel",118),"Dynamics");
    p->appendNumber("AccentAdd",1,127,"Sets the amount ov Velocity an Accent will Add to the Current Velocity",false,GetValue("accentadd",20),"Dynamics");
    p->appendNumber("fzAdd",1,127,"Sets the amount ov Velocity an fz Sign will Add to the Current Velocity",false,GetValue("fzadd",30),"Dynamics");
    p->appendNumber("fpStartVelocity",1,127,"Sets the Velocity at the beginning of a Note with an fp Sign",false,GetValue("fpstartvel",86),"Dynamics");
    p->appendNumber("fpEndVelocity",1,127,"Sets the Velocity at the end of a Note with an fp Sign",false,GetValue("fpendvel",38),"Dynamics");
    p->appendNumber("TrillSpeed",1,300,"Sets the Speed of Trills relative to the Tempo",false,GetValue("trillspeed",90),"Articulation");
    p->appendCustom("test","Test",false,"0","1");
    return m_PropColl;
}

void OCPresets::SetDefaults()
{
    if (Settings->contains("Presets/pvel")) return;
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


CTextSymbol::~CTextSymbol(){}

CGapSymbol::~CGapSymbol(){}

CDuratedSymbol::~CDuratedSymbol(){}

CVisibleSymbol::~CVisibleSymbol(){}

CSymbol::~CSymbol(){}

CInvisibleDuratedSymbol::~CInvisibleDuratedSymbol() {}

CInvisibleSymbol::~CInvisibleSymbol() {}
