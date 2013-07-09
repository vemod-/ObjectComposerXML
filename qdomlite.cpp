#include "qdomlite.h"

const QDomLiteAttribute QDomLiteAttributes::emptyAttribute("","");
const QStringList QDomLiteAttributes::trueList=QStringList() << "true" << "yes" << QVariant(true).toString().toLower();

namespace QDomLite
{
static QRegExp rxTag("\\s*<([^<>/ ]+)([^>]*)>\\s*",Qt::CaseSensitive,QRegExp::RegExp2);
static QRegExp rxRemark("\\s*<!--(.+)-->\\s*",Qt::CaseSensitive,QRegExp::RegExp2);
static QRegExp rxAttributes("\\s*([^=]+=[\"\'][^\"]+[\"\'])\\s*",Qt::CaseSensitive,QRegExp::RegExp2);
static QRegExp rxAttrValue("\\s*([^=]+)=[\"\']([^\"]+)[\"\']\\s*",Qt::CaseSensitive,QRegExp::RegExp2);
static QRegExp rxdocType("\\s*<!doctype(.+)[\[>]\\s*",Qt::CaseInsensitive,QRegExp::RegExp2);
static QRegExp rxXMLAttributes("\\s*<\\?xml[^\\s]*(.+)\\?>\\s*",Qt::CaseInsensitive,QRegExp::RegExp2);
static QRegExp rxCDATA("\\s*<![\[]CDATA[\[](.+)[\\]][\\]]>",Qt::CaseInsensitive,QRegExp::RegExp2);
static QRegExp rxEntityTags("\\s*[\[](.+)[\\]]\\s*>\\s*",Qt::CaseInsensitive,QRegExp::RegExp2);
static QRegExp rxEntity("\\s*<!ENTITY\\s+([^\\s]+).*[\"\'](.+)[\"\']>\\s*",Qt::CaseInsensitive,QRegExp::RegExp2);

const QString decodeEntities( const QString& string )
{
    QString s = string;
    s.replace("&gt;",">" );
    s.replace("&lt;","<" );
    s.replace("&quot;","\"" );
    s.replace("&apos;","\'" );
    s.replace("&amp;","&" );
    return s;
}

const QString encodeEntities( const QString& string )
{
    QString s = string;
    s.replace( "&", "&amp;" );
    s.replace( ">", "&gt;" );
    s.replace( "<", "&lt;" );
    s.replace( "\"", "&quot;" );
    s.replace( "\'", "&apos;" );
    return s;
}

void swapElements(QDomLiteElement **x, QDomLiteElement **y)
{
    QDomLiteElement *t = *x;
    *x = *y;
    *y = t;
}
}

QDomLiteAttribute::QDomLiteAttribute(const QString& name, const QString& value)
{
    this->name=name;
    this->value=QDomLite::encodeEntities(value);
}

QDomLiteAttribute* QDomLiteAttribute::clone()
{
    return new QDomLiteAttribute(name,QDomLite::decodeEntities(value));
}

const int QDomLiteAttributes::attributeCount() const
{
    return attributes.size();
}

const int QDomLiteAttributes::indexOfAttribute(const QString& name) const
{
    for (unsigned int i=0;i<attributes.size();i++) if (attributes[i]->name==name) return i;
    return -1;
}

const bool QDomLiteAttributes::attributeExists(const QString& name) const
{
    return (indexOfAttribute(name)>-1);
}

QDomLiteAttribute* QDomLiteAttributes::item(const QString& name) const
{
    foreach(QDomLiteAttribute* a,attributes) if (a->name==name) return a;
    return (QDomLiteAttribute*)&emptyAttribute;
}

QDomLiteAttribute* QDomLiteAttributes::item(const int index) const
{
    if ((index<0) || ((unsigned int)index>=attributes.size())) return (QDomLiteAttribute*)&emptyAttribute;
    return attributes[index];
}

const QString QDomLiteAttributes::attribute(const QString& name) const
{
    return QDomLite::decodeEntities(item(name)->value);
}

const QString QDomLiteAttributes::attribute(const int index) const
{
    return QDomLite::decodeEntities(item(index)->value);
}

const QString QDomLiteAttributes::attribute(const QString &name, const QString &defaultValue) const
{
    if (!attributeExists(name)) return defaultValue;
    return attribute(name);
}

const QString QDomLiteAttributes::attribute(const int index, const QString &defaultValue) const
{
    if ((index<0) || ((unsigned int)index>=attributes.size())) return defaultValue;
    return attribute(index);
}

const QString QDomLiteAttributes::attributeName(const int index)
{
    return item(index)->name;
}

const double QDomLiteAttributes::attributeValue(const QString& name) const
{
    QString Val=attribute(name);
    if (trueList.contains(Val.toLower())) return boolTrueValue;
    return Val.toDouble();
}

const double QDomLiteAttributes::attributeValue(const int index) const
{
    QString Val=QDomLite::decodeEntities(item(index)->value);
    if (trueList.contains(Val.toLower())) return boolTrueValue;
    return Val.toDouble();
}

const double QDomLiteAttributes::attributeValue(const QString &name, const double defaultValue) const
{
    if (!attributeExists(name)) return defaultValue;
    return attributeValue(name);
}

const double QDomLiteAttributes::attributeValue(const int index, const double defaultValue) const
{
    if ((index<0) || ((unsigned int)index>=attributes.size())) return defaultValue;
    return attributeValue(index);
}

void QDomLiteAttributes::setAttribute(const QString& name, const QVariant& value)
{
    if (!value.toString().length())
    {
        removeAttribute(name);
        return;
    }
    int i=indexOfAttribute(name);
    if (i>-1)
    {
        attributes[i]->value=QDomLite::encodeEntities(value.toString());
    }
    else
    {
        attributes.push_back(new QDomLiteAttribute(name,value.toString()));
    }
}

void QDomLiteAttributes::setAttribute(const int index, const QVariant& value)
{
    if ((index<0) || ((unsigned int)index >= attributes.size())) return;
    if (!value.toString().length())
    {
        removeAttribute(index);
        return;
    }
    attributes[index]->value=QDomLite::encodeEntities(value.toString());
}

void QDomLiteAttributes::appendAttribute(const QString& name, const QVariant& value)
{
    if (!value.toString().length())
    {
        removeAttribute(name);
        return;
    }
    if (indexOfAttribute(name)==-1)
    {
        attributes.push_back(new QDomLiteAttribute(name,value.toString()));
    }
}

const QString QDomLiteAttributes::attributesString() const
{
    QString RetVal;
    foreach(QDomLiteAttribute* a,attributes) RetVal+=" "+a->name+"=\""+a->value+"\"";
    return RetVal;
}

void QDomLiteAttributes::setAttributesString(const QString& attributesString)
{
    attributes.clear();
    appendAttributesString(attributesString);
}

void QDomLiteAttributes::appendAttributesString(const QString &attributesString)
{
    int AttrPos=0;
    while (QDomLite::rxAttributes.indexIn(attributesString,AttrPos)>-1)
    {
        AttrPos+=QDomLite::rxAttributes.matchedLength();
        if (QDomLite::rxAttrValue.indexIn(QDomLite::rxAttributes.cap(1).trimmed())>-1)
        {
            attributes.push_back(new QDomLiteAttribute(QDomLite::rxAttrValue.cap(1),QDomLite::decodeEntities(QDomLite::rxAttrValue.cap(2))));
        }
    }
}

void QDomLiteAttributes::removeAttribute(const QString& name)
{
    removeAttribute(indexOfAttribute(name));
}

void QDomLiteAttributes::removeAttribute(const int index)
{
    if ((index<0) || ((unsigned int)index>=attributes.size())) return;
    delete attributes[index];
    attributes.erase(attributes.begin() + index);
}

void QDomLiteAttributes::clearAttributes()
{
    if (attributes.size()) qDeleteAll(attributes);
    attributes.clear();
}

QDomLiteElement::QDomLiteElement()
{
}

QDomLiteElement::QDomLiteElement(const QString& tag)
{
    this->tag=tag;
}

QDomLiteElement::QDomLiteElement(const QDomLiteElement *element)
{
    copy(element);
}

QDomLiteElement::~QDomLiteElement()
{
    clear();
}

const bool QDomLiteElement::isText() const
{
    return (text.length()>0);
}

const bool QDomLiteElement::isCDATA() const
{
    return (CDATA.length()>0);
}

const bool QDomLiteElement::isParent() const
{
    return (text.length()+CDATA.length()==0);
}

const QDomLiteElement::QDomLiteElementType QDomLiteElement::elementType() const
{
    if (isText()) return QDomLiteElement::TextElement;
    if (isParent()) return QDomLiteElement::ParentElement;
    return QDomLiteElement::UndefinedElement;
}

QDomLiteElementList QDomLiteElement::elementsByTag(const QString& name, const bool deep)
{
    QDomLiteElementList RetVal;
    foreach(QDomLiteElement* e,childElements)
    {
        if (e->tag==name) RetVal.push_back(e);
        if (deep)
        {
            RetVal.append(e->elementsByTag(name,deep));
        }
    }
    return RetVal;
}

QDomLiteElement* QDomLiteElement::elementByTag(const QString& name, const bool deep)
{
    QDomLiteElement* RetVal=0;
    foreach(QDomLiteElement* e,childElements)
    {
        if (e->tag==name) return e;
        if (deep)
        {
            RetVal=e->elementByTag(name,deep);
            if (!RetVal==0) return RetVal;
        }
    }
    return RetVal;
}

QDomLiteElement* QDomLiteElement::replaceChild(QDomLiteElement *destinationElement, QDomLiteElement *sourceElement)
{
    int index=childElements.indexOf(destinationElement);
    if (index>-1)
    {
        delete childElements[index];
        childElements[index]=sourceElement;
    }
    return sourceElement;
}

QDomLiteElement* QDomLiteElement::replaceChild(QDomLiteElement *destinationElement, const QString& name)
{
    return replaceChild(destinationElement, new QDomLiteElement(name));
}

QDomLiteElement* QDomLiteElement::replaceChild(const int index, QDomLiteElement *sourceElement)
{
    if (index>-1)
    {
        delete childElements[index];
        childElements[index]=sourceElement;
    }
    return sourceElement;
}

QDomLiteElement* QDomLiteElement::replaceChild(const int index, const QString& name)
{
    return replaceChild(index, new QDomLiteElement(name));
}

QDomLiteElement* QDomLiteElement::exchangeChild(QDomLiteElement *destinationElement, QDomLiteElement *sourceElement)
{
    int index=childElements.indexOf(destinationElement);
    if (index>-1)
    {
        childElements[index]=sourceElement;
    }
    return destinationElement;
}

QDomLiteElement* QDomLiteElement::exchangeChild(const int index, QDomLiteElement *sourceElement)
{
    QDomLiteElement* destinationElement=0;
    if (index>-1)
    {
        destinationElement=childElements[index];
        childElements[index]=sourceElement;
    }
    return destinationElement;
}

void QDomLiteElement::removeChild(QDomLiteElement *element)
{
    int index=childElements.indexOf(element);
    removeChild(index);
}

void QDomLiteElement::removeChild(const QString& name)
{
    QDomLiteElement* element=elementByTag(name);
    if (element != 0) removeChild(element);
}

void QDomLiteElement::removeChild(const int index)
{
    if ((index<0) || ((unsigned int)index >= childElements.size())) return ;
    delete childElements[index];
    childElements.erase(childElements.begin() + index);
}

void QDomLiteElement::removeFirst()
{
    removeChild(0);
}

void QDomLiteElement::removeLast()
{
    removeChild(childElements.size()-1);
}

QDomLiteElement* QDomLiteElement::takeChild(const int index)
{
    if ((index<0) || ((unsigned int)index >= childElements.size())) return 0;
    return childElements.take(index);
    //QDomLiteElement* element = childElements[index];
    //childElements.erase(childElements.begin() + index);
    //return element;
}

QDomLiteElement* QDomLiteElement::takeChild(QDomLiteElement *element)
{
    int index=childElements.indexOf(element);
    return takeChild(index);
}

QDomLiteElement* QDomLiteElement::takeChild(const QString& name)
{
    QDomLiteElement* element=elementByTag(name);
    if (element != 0) return takeChild(element);
    return 0;
}

QDomLiteElement* QDomLiteElement::takeFirst()
{
    return takeChild(0);
}

QDomLiteElement* QDomLiteElement::takeLast()
{
    return takeChild(childElements.size()-1);
}

QDomLiteElement* QDomLiteElement::appendChild(QDomLiteElement *element)
{
    childElements.push_back(element);
    return element;
}

QDomLiteElement* QDomLiteElement::appendChild(const QString& name)
{
    return appendChild(new QDomLiteElement(name));
}

QDomLiteElement* QDomLiteElement::appendClone(const QDomLiteElement *element)
{
    return appendChild(new QDomLiteElement(element));
}

QDomLiteElement* QDomLiteElement::appendChildFromString(const QString &XML)
{
    QDomLiteElement* e=appendChild(new QDomLiteElement);
    e->fromString(XML);
    return e;
}

QDomLiteElement* QDomLiteElement::prependChild(QDomLiteElement *element)
{
    childElements.push_front(element);
    return element;
}

QDomLiteElement* QDomLiteElement::prependChild(const QString& name)
{
    return prependChild(new QDomLiteElement(name));
}

QDomLiteElement* QDomLiteElement::prependClone(const QDomLiteElement *element)
{
    return prependChild(new QDomLiteElement(element));
}

QDomLiteElement* QDomLiteElement::insertChild(QDomLiteElement *element, int insertBefore)
{
    if ((insertBefore > -1) && ((unsigned int)insertBefore < childElements.size()))
    {
        childElements.insert(childElements.begin() + insertBefore,element);
    }
    else
    {
        childElements.push_back(element);
    }
    return element;
}

QDomLiteElement* QDomLiteElement::insertChild(QDomLiteElement *element, QDomLiteElement *insertBefore)
{
    return insertChild(element,childElements.indexOf(insertBefore));
}

QDomLiteElement* QDomLiteElement::insertChild(const QString& name, QDomLiteElement *insertBefore)
{
    return insertChild(new QDomLiteElement(name),insertBefore);
}

QDomLiteElement* QDomLiteElement::insertChild(const QString& name, const int insertBefore)
{
    return insertChild(new QDomLiteElement(name),insertBefore);
}

QDomLiteElement* QDomLiteElement::insertClone(const QDomLiteElement *element, QDomLiteElement *insertBefore)
{
    return insertChild(new QDomLiteElement(element),insertBefore);
}

QDomLiteElement* QDomLiteElement::insertClone(const QDomLiteElement *element, const int insertBefore)
{
    return insertChild(new QDomLiteElement(element),insertBefore);
}

void QDomLiteElement::swapChild(const int index, QDomLiteElement **element)
{
    if ((index<0) || ((unsigned int)index >= childElements.size())) return;
    QDomLite::swapElements(&childElements[index],element);
}

void QDomLiteElement::swapChild(QDomLiteElement *childElement, QDomLiteElement **element)
{
    int index=childElements.indexOf(childElement);
    swapChild(index,element);
}

void QDomLiteElement::swapChild(const QString& name, QDomLiteElement **element)
{
    QDomLiteElement* elem=elementByTag(name);
    if (elem != 0) swapChild(elem,element);
}

void QDomLiteElement::appendChildren(QDomLiteElementList& elements)
{
    childElements.append(elements);
}

void QDomLiteElement::insertChildren(QDomLiteElementList& elements, QDomLiteElement *insertBefore)
{
    foreach(QDomLiteElement* e,elements) insertChild(e, insertBefore);
}

void QDomLiteElement::insertChildren(QDomLiteElementList& elements, const int insertBefore)
{
    QDomLiteElement* Before=childElements[insertBefore];
    insertChildren(elements,Before);
}

void QDomLiteElement::removeChildren(QDomLiteElementList& elements)
{
    foreach(QDomLiteElement* e,elements) takeChild(e);
    if (elements.size()) qDeleteAll(elements);
    elements.clear();
}

void QDomLiteElement::removeChildren(const QString& name)
{
    QDomLiteElementList elements=elementsByTag(name);
    removeChildren(elements);
}

QDomLiteElementList QDomLiteElement::takeChildren(QDomLiteElementList &elements)
{
    QDomLiteElementList RetVal;
    foreach(QDomLiteElement* e,elements) RetVal.push_back(takeChild(e));
    return RetVal;
}

QDomLiteElementList QDomLiteElement::takeChildren(const QString& name)
{
    QDomLiteElementList elements=elementsByTag(name);
    return takeChildren(elements);
}

const int QDomLiteElement::childCount() const
{
    return childElements.size();
}

const int QDomLiteElement::childCount(const QString& name) const
{
    int count=0;
    foreach(QDomLiteElement* e,childElements) if (e->tag==name) count++;
    return count;
}

QDomLiteElement* QDomLiteElement::childElement(const int index)
{
    if ((index<0) || ((unsigned int)index >= childElements.size())) return 0;
    return childElements[index];
}

QDomLiteElement* QDomLiteElement::firstChild()
{
    return childElement(0);
}

QDomLiteElement* QDomLiteElement::lastChild()
{
    return childElement(childElements.size()-1);
}

const int QDomLiteElement::indexOfChild(QDomLiteElement* element)
{
    return childElements.indexOf(element);
}

QDomLiteElement* QDomLiteElement::clone()
{
    return new QDomLiteElement(this);
}

void QDomLiteElement::copy(const QDomLiteElement *element)
{
    clear();
    tag=element->tag;
    text=element->text;
    CDATA=element->CDATA;
    comments.append(element->comments);
    foreach(QDomLiteAttribute* a,element->attributes) attributes.push_back(a->clone());
    foreach(QDomLiteElement* e,element->childElements) childElements.push_back(e->clone());
}

const QString QDomLiteElement::toString(const int indentLevel) const
{
    QString Indent=QString(indentLevel,QChar(9));
    QString RetVal;
    foreach(QString s,comments) RetVal=Indent+"<!--"+s+"-->\n";
    if (CDATA.length())
    {
        return Indent+"<![CDATA["+CDATA+"]]>\n";
    }
    RetVal+=Indent+"<"+tag+attributesString();
    if (text.length())
    {
        RetVal+=">"+text+"</"+tag+">\n";
    }
    else if (childElements.size())
    {
        int ChildIndent=indentLevel;
        if (ChildIndent>-1) ChildIndent++;
        RetVal+=">\n";
        foreach(QDomLiteElement* e,childElements) RetVal += e->toString(ChildIndent);
        RetVal+=Indent+"</"+tag+">\n";
    }
    else
    {
        RetVal+="/>\n";
    }
    return RetVal;
}

const int QDomLiteElement::fromString(const QString& XML, int start)
{
    QString TestString=XML.mid(start,XMLmaxtaglen);
    clear();
    while (QDomLite::rxRemark.indexIn(TestString)==0) // comment
    {
        start+=QDomLite::rxRemark.matchedLength();
        comments.append(QDomLite::rxRemark.cap(1).trimmed());
        TestString=XML.mid(start,XMLmaxtaglen);
    }
    if (QDomLite::rxCDATA.indexIn(TestString)==0)
    {
        CDATA=QDomLite::rxCDATA.cap(1);
        start+=QDomLite::rxCDATA.matchedLength();
        return start;
    }
    int tagIndex=QDomLite::rxTag.indexIn(TestString);
    if (tagIndex==-1) tagIndex=QDomLite::rxTag.indexIn(XML.mid(start));
    if (tagIndex==0)
    {
        tag=QDomLite::rxTag.cap(1).trimmed();
        QString Attr=QDomLite::rxTag.cap(2).trimmed();
        start+=QDomLite::rxTag.matchedLength();
        if (Attr.endsWith("/")) //doesn't have an end tag
        {
            Attr.chop(1);
        }
        else
        {
            QRegExp rx("\\s*</"+tag+">\\s*"); //end tag
            forever
            {
                QDomLiteElement* e=new QDomLiteElement();
                int i=e->fromString(XML,start);
                if (i==start)
                {
                    delete e;
                    break;
                }
                childElements.push_back(e);
                start=i;
            }
            if (childElements.size()==0) //it's a text node
            {
                int TxtPtr=rx.indexIn(XML.mid(start));
                if (TxtPtr > 0)
                {
                    text=XML.mid(start,TxtPtr).trimmed();
                    start+=TxtPtr;
                }
            }
            if (rx.indexIn(XML.mid(start,XMLendtaglen))==0)
            {
                start+=rx.matchedLength();
            }
        }
        setAttributesString(Attr);
    }
    return start;
}

void QDomLiteElement::clear()
{
    tag.clear();
    text.clear();
    CDATA.clear();
    clearChildren();
    comments.clear();
    clearAttributes();
}

void QDomLiteElement::clear(const QString& tag)
{
    clear();
    this->tag=tag;
}

void QDomLiteElement::clearChildren()
{
    if (childElements.size()) qDeleteAll(childElements);
    childElements.clear();
}

QDomLiteElement::operator QString()
{
    return toString(0);
}

QDomLiteElement& QDomLiteElement::operator <<(QDomLiteElement& element)
{
    appendChild(&element);
    return *this;
}

QDomLiteElement& QDomLiteElement::operator <<(QDomLiteElementList& elements)
{
    appendChildren(elements);
    return *this;
}

QDomLiteElement& QDomLiteElement::operator <<(const char* name)
{
    appendChild(QString(name));
    return *this;
}

QDomLiteDocument::QDomLiteDocument(const QString& docType, const QString& docTag)
{
    init(docType,docTag);
}

QDomLiteDocument::QDomLiteDocument(const QString& path)
{
    init(QString(),QString());
    load(path);
}

QDomLiteDocument::QDomLiteDocument(const QDomLiteDocument *document)
{
    init(QString(),QString());
    copy(document);
}

QDomLiteDocument::QDomLiteDocument()
{
    init(QString(),QString());
}

void QDomLiteDocument::init(const QString& docType, const QString& docTag)
{
    QDomLite::rxRemark.setMinimal(true);
    QDomLite::rxdocType.setMinimal(true);
    QDomLite::rxXMLAttributes.setMinimal(true);
    QDomLite::rxTag.setMinimal(true);
    QDomLite::rxAttributes.setMinimal(true);
    QDomLite::rxAttrValue.setMinimal(true);
    QDomLite::rxCDATA.setMinimal(true);
    QDomLite::rxEntityTags.setMinimal(true);
    QDomLite::rxEntity.setMinimal(true);
    this->docType=docType;
    documentElement=new QDomLiteElement(docTag);
}

QDomLiteDocument::~QDomLiteDocument()
{
    delete documentElement;
}

const bool QDomLiteDocument::load(const QString& path)
{
    QFile fileData(path);
    if (fileData.open(QIODevice::ReadOnly))
    {
        fromString(fileData.readAll());
        fileData.close();
        return true;
    }
    return false;
}

void QDomLiteDocument::clear()
{
    docType.clear();
    comments.clear();
    entities.clear();
    documentElement->clear();
    clearAttributes();
}

void QDomLiteDocument::clear(const QString& docType, const QString& docTag)
{
    this->clear();
    this->docType=docType;
    documentElement->tag=docTag;
}

void QDomLiteDocument::fromString(const QString& XML)
{
    clear();
    int Ptr=0;
    while (appendComments(XML,Ptr)){}
    if (QDomLite::rxdocType.indexIn(XML.mid(Ptr,QDomLiteElement::XMLmaxtaglen))==0)
    {
        docType=QDomLite::rxdocType.cap(1).trimmed();
        Ptr+=QDomLite::rxdocType.matchedLength();
        if (QDomLite::rxEntityTags.indexIn(XML.mid(Ptr-1))==0)
        {
            QString ent=QDomLite::rxEntityTags.cap(1);
            int eptr=0;
            while (appendEntities(ent,eptr)){}
            Ptr+=QDomLite::rxEntityTags.matchedLength();
        }
    }
    while (appendComments(XML,Ptr)){}
    documentElement->fromString(XML,Ptr);
}

const bool QDomLiteDocument::appendEntities(const QString& XML, int& Ptr)
{
    bool retVal=false;
    while (QDomLite::rxRemark.indexIn(XML.mid(Ptr))==0) Ptr+=QDomLite::rxRemark.matchedLength(); //skip!
    while (QDomLite::rxEntity.indexIn(XML.mid(Ptr))==0)
    {
        entities.insert("&"+QDomLite::rxEntity.cap(1)+";",QDomLite::rxEntity.cap(2).trimmed());
        Ptr+=QDomLite::rxEntity.matchedLength();
        retVal=true;
    }
    return retVal;
}

const bool QDomLiteDocument::appendComments(const QString& XML, int& Ptr)
{
    bool retVal=false;
    while (QDomLite::rxXMLAttributes.indexIn(XML.mid(Ptr,QDomLiteElement::XMLmaxtaglen))==0)
    {
        appendAttributesString(QDomLite::rxXMLAttributes.cap(1));
        Ptr+=QDomLite::rxXMLAttributes.matchedLength();
        retVal=true;
    }
    while (QDomLite::rxRemark.indexIn(XML.mid(Ptr,QDomLiteElement::XMLmaxtaglen))==0)
    {
        comments.append(QDomLite::rxRemark.cap(1).trimmed());
        Ptr+=QDomLite::rxRemark.matchedLength();
        retVal=true;
    }
    return retVal;
}

const QString QDomLiteDocument::decodeEntities(QDomLiteElement *textElement)
{
    return decodeEntities(textElement->text);
}

const QString QDomLiteDocument::decodeEntities(const QString &text)
{
    QString retVal(text);
    foreach(QString e,entities.keys()) retVal.replace(e,entities[e]);
    return retVal;
}

void QDomLiteDocument::addEntity(const QString &entity, const QString &value)
{
    QString e=entity;
    if (!e.startsWith("&")) e="&"+e;
    if (!e.endsWith(";")) e+=";";
    entities.insert(e,value);
}

QDomLiteElement* QDomLiteDocument::replaceDoc(QDomLiteElement *element)
{
    delete documentElement;
    documentElement = element;
    return element;
}

QDomLiteElement* QDomLiteDocument::exchangeDoc(QDomLiteElement *element)
{
    QDomLiteElement** t=&documentElement;
    documentElement = element;
    return *t;
}

void QDomLiteDocument::swapDoc(QDomLiteElement **element)
{
    QDomLite::swapElements(&documentElement,element);
}

const bool QDomLiteDocument::save(const QString& path)
{
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)) return false;
    QTextStream ts( &file );
    ts << toString();
    file.close();
    return true;
}

const QString QDomLiteDocument::toString(const bool indent) const
{
    QString RetVal;
    if (attributes.size()) RetVal += "<?xml" + attributesString() + "?>\n";
    if (docType.length())
    {
        RetVal += "<!DOCTYPE "+docType;
        if (entities.count())
        {
            RetVal+=" [\n";
            foreach(QString e,entities.keys())
            {
                RetVal+="<!ENTITY "+e.mid(1,e.length()-2)+" \""+entities[e]+"\">\n";
            }
            RetVal+="] ";
        }
        RetVal+=">\n";
    }
    foreach(QString s,comments) RetVal += "<!-- "+s+"-->\n";
    return RetVal + documentElement->toString(-(!indent));
}

QDomLiteDocument* QDomLiteDocument::clone()
{
    return new QDomLiteDocument(this);
}

void QDomLiteDocument::copy(const QDomLiteDocument *document)
{
    clear();
    docType=document->docType;
    comments=document->comments;
    entities=document->entities;
    foreach(QDomLiteAttribute* a,document->attributes) attributes.push_back(a->clone());
    replaceDoc(document->documentElement->clone());
}

QDomLiteDocument::operator QString()
{
    return toString(true);
}
