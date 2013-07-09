#ifndef QDOMLITE_H
#define QDOMLITE_H
#include <QString>
#include <QVariant>
#include <QList>
#include <QStringList>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <deque>

class QDomLiteElement;
class QDomLiteAttribute;

class QDomLiteElementList : public std::deque<QDomLiteElement*>
{
public:
    const int indexOf(QDomLiteElement* element)
    {
        for (unsigned int i=0;i<size();i++)
        {
            if (at(i)==element)
            {
                return i;
            }
        }
        return -1;
    }
    void append(const QDomLiteElementList elements)
    {
        for (unsigned int i=0;i<elements.size();i++)
        {
            push_back(elements[i]);
        }
    }
    QDomLiteElement* take(const unsigned int index)
    {
        QDomLiteElement* e=at(index);
        erase(begin()+index);
        return e;
    }
};

class QDomLiteAttributeList : public std::deque<QDomLiteAttribute*>
{
public:
    const int indexOf(QDomLiteAttribute* attr)
    {
        for (unsigned int i=0;i<size();i++)
        {
            if (at(i)==attr)
            {
                return i;
            }
        }
        return -1;
    }
};

class QDomLiteAttribute
{
public:
    QDomLiteAttribute(const QString&name, const QString&value);
    QString name;
    QString value;
    QDomLiteAttribute* clone();
};

class QDomLiteAttributes
{
public:
    const QString attribute(const QString& name) const;
    const QString attribute(const int index) const;
    const QString attribute(const QString& name, const QString& defaultValue) const;
    const QString attribute(const int index, const QString& defaultValue) const;
    const QString attributeName(const int index);
    const QString attributesString() const;
    const double attributeValue(const QString& name) const;
    const double attributeValue(const int index) const;
    const double attributeValue(const QString& name, const double defaultValue) const;
    const double attributeValue(const int index, const double defaultValue) const;
    void setAttribute(const QString& name, const QVariant& value);
    void setAttribute(const int index, const QVariant& value);
    void appendAttribute(const QString& name, const QVariant& value);
    void setAttributesString(const QString& attributesString);
    void appendAttributesString(const QString& attributesString);
    void removeAttribute(const QString& name);
    void removeAttribute(const int index);
    void clearAttributes();
    const int attributeCount() const;
    const int indexOfAttribute(const QString& name) const;
    const bool attributeExists(const QString& name) const;
protected:
    QDomLiteAttribute* item(const QString& name) const;
    QDomLiteAttribute* item(const int index) const;
    QDomLiteAttributeList attributes;
private:
    static const int boolTrueValue=1;
    static const QDomLiteAttribute emptyAttribute;
    static const QStringList trueList;
};

class QDomLiteElement : public QDomLiteAttributes
{
public:
    enum QDomLiteElementType
    {
        UndefinedElement=0,
        ParentElement=1,
        TextElement=2
    };
    QDomLiteElement(const QString& Tag);
    QDomLiteElement();
    QDomLiteElement(const QDomLiteElement* Element);
    ~QDomLiteElement();
    const bool isText() const;
    const bool isCDATA() const;
    const bool isParent() const;
    const QDomLiteElementType elementType() const;
    QString tag;
    QString text;
    QString CDATA;
    QStringList comments;
    QDomLiteElementList childElements;
    QDomLiteElementList elementsByTag(const QString& name, const bool deep=false);
    QDomLiteElement* elementByTag(const QString& name, const bool deep=false);
    QDomLiteElement* replaceChild(QDomLiteElement* destinationElement, QDomLiteElement* sourceElement);
    QDomLiteElement* replaceChild(QDomLiteElement* destinationElement, const QString& name);
    QDomLiteElement* replaceChild(const int index, QDomLiteElement* sourceElement);
    QDomLiteElement* replaceChild(const int index, const QString& name);
    QDomLiteElement* exchangeChild(QDomLiteElement* destinationElement, QDomLiteElement* sourceElement);
    QDomLiteElement* exchangeChild(const int index, QDomLiteElement* sourceElement);
    void removeChild(QDomLiteElement* element);
    void removeChild(const int index);
    void removeChild(const QString& name);
    void removeFirst();
    void removeLast();
    QDomLiteElement* takeChild(QDomLiteElement* element);
    QDomLiteElement* takeChild(const int index);
    QDomLiteElement* takeChild(const QString& name);
    QDomLiteElement* takeFirst();
    QDomLiteElement* takeLast();
    QDomLiteElement* appendChild(QDomLiteElement* element);
    QDomLiteElement* appendClone(const QDomLiteElement* element);
    QDomLiteElement* appendChildFromString(const QString& XML);
    QDomLiteElement* appendChild(const QString& name);
    QDomLiteElement* prependChild(QDomLiteElement* element);
    QDomLiteElement* prependClone(const QDomLiteElement* element);
    QDomLiteElement* prependChild(const QString& name);
    QDomLiteElement* insertChild(QDomLiteElement* element, const int insertBefore);
    QDomLiteElement* insertChild(QDomLiteElement* element, QDomLiteElement* insertBefore);
    QDomLiteElement* insertChild(const QString& name, const int insertBefore);
    QDomLiteElement* insertChild(const QString& name, QDomLiteElement* insertBefore);
    QDomLiteElement* insertClone(const QDomLiteElement* element, const int insertBefore);
    QDomLiteElement* insertClone(const QDomLiteElement* element, QDomLiteElement* insertBefore);
    void swapChild(const int index, QDomLiteElement** element);
    void swapChild(const QString& name, QDomLiteElement** element);
    void swapChild(QDomLiteElement* childElement, QDomLiteElement** element);
    void appendChildren(QDomLiteElementList& elements);
    void insertChildren(QDomLiteElementList& elements, const int insertBefore);
    void insertChildren(QDomLiteElementList& elements, QDomLiteElement* insertBefore);
    void removeChildren(QDomLiteElementList& elements);
    void removeChildren(const QString& name);
    QDomLiteElementList takeChildren(QDomLiteElementList& elements);
    QDomLiteElementList takeChildren(const QString& name);
    const int childCount() const;
    const int childCount(const QString& name) const;
    QDomLiteElement* childElement(const int index);
    QDomLiteElement* firstChild();
    QDomLiteElement* lastChild();
    const int indexOfChild(QDomLiteElement* element);
    QDomLiteElement* clone();
    void copy(const QDomLiteElement* element);
    const QString toString(const int indentLevel=-1) const;
    const int fromString(const QString& XML, int Start=0);
    void clear();
    void clear(const QString& tag);
    void clearChildren();
    operator QString();
    QDomLiteElement& operator <<(QDomLiteElement& element);
    QDomLiteElement& operator <<(QDomLiteElementList& elements);
    QDomLiteElement& operator <<(const char* name);
    static const int XMLmaxtaglen=800;
    static const int XMLendtaglen=50;
};

class QDomLiteDocument : public QDomLiteAttributes
{
public:
    QDomLiteDocument(const QString& docType, const QString& docTag);
    QDomLiteDocument(const QString& path);
    QDomLiteDocument(const QDomLiteDocument* document);
    QDomLiteDocument();
    ~QDomLiteDocument();
    QDomLiteElement* documentElement;
    QDomLiteElement* replaceDoc(QDomLiteElement* element);
    QDomLiteElement* exchangeDoc(QDomLiteElement* element);
    void swapDoc(QDomLiteElement** element);
    const bool save(const QString& path);
    const bool load(const QString& path);
    void clear();
    void clear(const QString& docType, const QString& docTag);
    void fromString(const QString& XML);
    QDomLiteDocument* clone();
    void copy(const QDomLiteDocument* document);
    const QString toString(const bool indent=false) const;
    const QString decodeEntities(QDomLiteElement* textElement);
    const QString decodeEntities(const QString& text);
    void addEntity(const QString& entity, const QString& value);
    QString docType;
    QStringList comments;
    QMap<QString,QString> entities;
    operator QString();
private:
    void init(const QString& docType, const QString& docTag);
    const bool appendEntities(const QString& XML, int& Ptr);
    const bool appendComments(const QString& XML, int& Ptr);
};

#endif // QDOMLITE_H
