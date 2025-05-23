#include "ocxmlwrappers.h"

IXMLWrapper::~IXMLWrapper()
{
    if (!isShadow) delete XMLElement;
}

OCBarLocation::OCBarLocation(const OCBarSymbolLocation& b) : OCVoiceLocation(b)
{
    Bar=b.Bar;
}

void XMLScoreWrapper::ParseFileVersion()
{    
    QDomLiteElementList Elements=m_XMLScore->documentElement->allChildren();
    for (QDomLiteElement* e : std::as_const(Elements))
    {
        if (e->attributeExists("FontName"))
        {
            e->renameAttribute("FontItalic","Italic");
            e->renameAttribute("FontBold","Bold");
            e->renameAttribute("Size","FontSize");
        }
        if (e->attributeValueInt("NoteSpace")==0) e->removeAttribute("NoteSpace");
        if (e->attributeValueInt("NoteSpace")==defaultnotespace) e->removeAttribute("NoteSpace");
    }
    if (m_XMLScore->documentElement->elementByTag("Templates"))
    {
        if (m_XMLScore->documentElement->elementByTag("Templates")->elementByTag("AllTemplate"))
        {
            m_XMLScore->documentElement->removeChildren("Template");
            m_XMLScore->documentElement->appendChild(m_XMLScore->documentElement->elementByTag("Templates")->takeChild("AllTemplate"));
            m_XMLScore->documentElement->elementByTag("AllTemplate")->tag="Template";
            m_XMLScore->documentElement->removeChildren("Templates");
        }
    }
    const QDomLiteElementList l=m_XMLScore->documentElement->elementByTag("Template")->childElements;
    for (int i=0;i<l.size();i++)
    {
        if (l[i]->attributeExists("Name")) setStaffName(i,l[i]->attribute("Name"));
        if (l[i]->attributeExists("Abbreviation")) setStaffAbbreviation(i,l[i]->attribute("Abbreviation"));
    }
    for (QDomLiteElement* s : l)
    {
        s->removeAttribute("Name");
        s->removeAttribute("Abbreviaton");
    }
    QDomLiteElementList t = LayoutCollection.templates();
    for (QDomLiteElement* e : std::as_const(t))
    {
        for (QDomLiteElement* s : std::as_const(e->childElements))
        {
            s->removeAttribute("Name");
            s->removeAttribute("Abbreviaton");
        }
    }
    QDomLiteElementList Voices=Score.xml()->elementsByTag("Voice",true);
    for (QDomLiteElement* XMLVoice : std::as_const(Voices))
    {
        if (XMLVoice->childCount())
        {
            const XMLSimpleSymbolWrapper Symbol(XMLVoice->lastChild());
            if (Symbol.Compare("EndOfVoice")) XMLVoice->removeLast();
        }
    }
    QDomLiteElementList Symbols=Score.xml()->elementsByTag("Symbol",true);
    for (QDomLiteElement* XMLSymbol : std::as_const(Symbols))
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
            Symbol.setAttribute("Direction","0");
        }
        else if (Symbol.Compare("HairpinCrescendo"))
        {
            Symbol.setAttribute("SymbolName","Hairpin");
            Symbol.setAttribute("HairpinType","0");
        }
        else if (Symbol.Compare("HairpinDiminuendo"))
        {
            Symbol.setAttribute("SymbolName","Hairpin");
            Symbol.setAttribute("HairpinType",1);
        }
        else if (Symbol.Compare("Fish"))
        {
            Symbol.setAttribute("SymbolName","Hairpin");
            Symbol.setAttribute("HairpinType",Symbol.getIntVal("FishType")+2);
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
            Symbol.setAttribute("Direction","0");
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

        QDomLiteElement* e=Symbol.getXML();
        e->removeAttribute("Visible");
        e->removeAttribute("Audible");

        if (Symbol.getIntVal("Dotted") < 1)
        {
            Symbol.setAttribute("Dotted",int(Symbol.getBoolVal("Dotted")));
        }

        Symbol.removeZeroAttribute("AccidentalLeft");
        Symbol.removeZeroAttribute("TieTop");
        Symbol.removeZeroAttribute("Left");
        Symbol.removeZeroAttribute("Top");
        Symbol.removeZeroAttribute("Pitch");
        Symbol.removeZeroAttribute("Size");
        Symbol.removeZeroAttribute("NoteType");
        Symbol.removeZeroAttribute("NoteValue");
        Symbol.removeFalseAttribute("Invisible");
        Symbol.removeFalseAttribute("Inaudible");
        Symbol.removeFalseAttribute("Triplet");
        Symbol.removeZeroAttribute("Dotted");
    }
}

int XMLScoreWrapper::FindSymbol(const XMLVoiceWrapper& XMLVoice, const QString& Name, const int Ptr, const QString& Attr, const double Val, const QString& Attr1, const double Val1)
{
    int Py=Ptr;
    forever
    {
        if (Py >= XMLVoice.symbolCount()) return Py-1;
        const XMLSimpleSymbolWrapper XMLSymbol=XMLVoice.XMLSimpleSymbol(Py);
        if (XMLSymbol.Compare(Name))
        {
            if (!Attr.isEmpty())
            {
                if (closeEnough(XMLSymbol.getVal(Attr),Val))
                {
                    if (!Attr1.isEmpty())
                    {
                        if (closeEnough(XMLSymbol.getVal(Attr1),Val1)) return Py;
                    }
                    else
                    {
                        return Py;
                    }
                }
            }
            else
            {
                return Py;
            }
        }
        //if (XMLSymbol.IsEndOfVoice()) return Py-1;
        Py++;
    }
}

XMLStaffCollectionWrapper::~XMLStaffCollectionWrapper() {}

XMLLayoutCollectionWrapper::~XMLLayoutCollectionWrapper(){}

void XMLSimpleSymbolWrapper::shadowXML(QDomLiteElement *e)
{
    CNoteCompare::shadowXML(e);
}

void XMLSymbolWrapper::shadowXML(QDomLiteElement *e)
{
    XMLSimpleSymbolWrapper::shadowXML(e);
}

void XMLFontWrapper::shadowXML(QDomLiteElement *e)
{
    IXMLWrapper::shadowXML(e);
    setFont(QFont(fontName(),fontSize(),bold(),italic()));
}

void XMLTextElementWrapper::shadowXML(QDomLiteElement *e)
{
    XMLFontWrapper::shadowXML(e);
}

void XMLScoreOptionsWrapper::shadowXML(QDomLiteElement *e)
{
    IXMLWrapper::shadowXML(e);
}

void XMLTemplateStaffWrapper::shadowXML(QDomLiteElement *e)
{
    IXMLWrapper::shadowXML(e);
}

void XMLTemplateWrapper::shadowXML(QDomLiteElement *e)
{
    IXMLCollectionWrapper::shadowXML(e);
}

void XMLLayoutFontsWrapper::shadowXML(QDomLiteElement* e) {
    IXMLWrapper::shadowXML(e);
    setSubElements();
}

void XMLLayoutFontsWrapper::setSubElements()
{
    title.shadowXML(XMLElement->elementByTagCreate("Title"));
    subtitle.shadowXML(XMLElement->elementByTagCreate("Subtitle"));
    composer.shadowXML(XMLElement->elementByTagCreate("Composer"));
    names.shadowXML(XMLElement->elementByTagCreate("Names"));
}

void XMLLayoutSystemWrapper::setSubElements()
{
    Template.shadowXML(XMLElement->elementByTagCreate("Template"));
}

void XMLLayoutWrapper::setSubElements()
{
    Template.shadowXML(XMLElement->elementByTagCreate("Template"));
    Fonts.shadowXML(XMLElement->elementByTagCreate("Titles"));
    Options.shadowXML(XMLElement->elementByTagCreate("Options"));
}

void XMLLayoutOptionsWrapper::shadowXML(QDomLiteElement *e)
{
    XMLScoreOptionsWrapper::shadowXML(e);
}

void XMLLayoutPageWrapper::shadowXML(QDomLiteElement *e)
{
    IXMLWrapper::shadowXML(e);
}

void XMLVoiceWrapper::shadowXML(QDomLiteElement *e)
{
    IXMLCollectionWrapper::shadowXML(e);
}

void XMLStaffWrapper::shadowXML(QDomLiteElement *e)
{
    IXMLCollectionWrapper::shadowXML(e);
}

void CNoteCompare::shadowXML(QDomLiteElement *e)
{
    IXMLWrapper::shadowXML(e);
    if (e) stringVal=e->attribute("SymbolName");
}
