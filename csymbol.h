#ifndef CSYMBOL_H
#define CSYMBOL_H

#include "CommonClasses.h"
/*
#include "qttreepropertybrowser.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "QtVariantProperty"
#include "QtVariantEditorFactory"
#include "QtVariantPropertyManager"
*/
#include <QSettings>
#include <QApplication>
#include <QObject>
#include <QToolButton>
#include <QLabel>
#include <QVBoxLayout>

class OCSignList;
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
private:
    QStringList m_sList;
    QVariant m_Value;
public:
    QString Name;
    QVariant Min;
    QVariant Max;
    OCPropertyType PropertyType;
    QString Description;
    QVariant Default;
    bool Hidden;
    QString Category;
    bool PermHidden;
    int ListOffset;

    OCProperty();
    void AddToList(const QString& Text);
    void AddToList(const QStringList& Lst);
    const QString ListItem(int Index) const;
    const int ListCount() const;
    const QStringList& List() const;
    const QVariant GetValue() const;
    void SetValue(const QVariant& vData);
    const QString TextValue() const;
};

//--------------------------------------------------------------------------

class OCProperties
{
public:
    OCProperties();
    ~OCProperties();
    QString Name;
    OCProperty* Add(const QString& Name, const OCPropertyType PropertyType, const QVariant& Min="", const QVariant& Max="", const QString& Description="", const QVariant& List="", const int ListOffset=0, const bool Hidden=false, const QVariant& Value="", const QString& Category="");
    OCProperty* GetItem(const QString& Name);
    OCProperty* GetItem(const int Index);
    const QVariant GetValue(const QString& Name) const;
    void SetValue(const QString& Name,const QVariant& vData);
    const int Count() const;
    void Remove(const QString& Name);
    const bool Exist(const QString& Name) const;
    void Reset();
    void FromXML(QDomLiteElement* XML);
    QDomLiteElement* ToXML() const;
    const QString TextValue(const int Index) const;
protected:
    QMap<QString, OCProperty*> mCol;
};

class CustomEdit : public QWidget
{
    Q_OBJECT
public:
    explicit CustomEdit(QWidget* parent=0);
    QLabel* lbl;
private:
    QToolButton* btn;
signals:
    void clicked();
    void valueChanged(const QString & value);
};
/*
class CustomManager : public QtAbstractPropertyManager
    {
    Q_OBJECT
public:
    explicit CustomManager(QObject* parent=0);
        QString value(const QtProperty *property) const;
    public slots:
        void setValue(QtProperty *property, const QString value);
    signals:
        void valueChanged(QtProperty *property, const QString);
        void clicked(QtProperty* property);
    protected:
        QString valueText(const QtProperty * property) const
        { return ""; }
        void initializeProperty(QtProperty *property)
            { theValues[property] = Data(); }
        void uninitializeProperty(QtProperty *property)
            { theValues.remove(property); }
    private:
        struct Data {
            QString value;
        };
        QMap<const QtProperty *, Data> theValues;
    };

class CustomEditFactory : public QtAbstractEditorFactory<CustomManager>
{
    Q_OBJECT
public:
    explicit CustomEditFactory(QObject* parent=0);
    CustomEdit* createEditor(CustomManager *manager, QtProperty *property, QWidget *parent);
    void connectPropertyManager(CustomManager *manager);
    void disconnectPropertyManager(CustomManager *manager);
private slots:
    void slotPropertyChanged(QtProperty *property,
                             const QString &value);
    void slotSetValue(const QString &value);
    void slotEditorDestroyed(QObject *object);
private:
    QMap<QtProperty *, QList<CustomEdit *> > createdEditors;
    QMap<CustomEdit *, QtProperty *> editorToProperty;
};
*/
/*
class OCPropertiesX : public QObject, public OCProperties
{
    Q_OBJECT
public:
    explicit OCPropertiesX(QObject* parent=0);
    //void fillPropertyBrowser(QtAbstractPropertyBrowser* pb);
private slots:
    //void propertyChanged(QtProperty* p, bool value);
    void propertyChanged(QtProperty* p, QString value);
    void propertyChanged(QtProperty* p, QVariant value);
    void propertyChanged(QtProperty* p, int value);
private:
    QtVariantPropertyManager propertyManager;
    QtVariantEditorFactory editorFactory;
    CustomEditFactory* customFactory;
    QtGroupPropertyManager groupManager;
    QtIntPropertyManager sliderManager;
    CustomManager customManager;
    QtSliderFactory sliderFactory;
signals:
    void dataChanged(QString name);
};
*/
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
    QSettings Settings;
    OCProperties* m_PropColl;
    void SetDefaults();
    QString Tag;
    bool MustSaveProperties;
};

//--------------------------------------------------------------------------

class CSymbol
{
protected:
    QString m_ButtonProperty;
    QList<OCToolButtonProps*> m_ButtonList;
    OCToolButtonProps* CreateButton(const QString& FontName, const float FontSize, const QString& Text, const bool FontBold, const bool FontItalic, const bool CustomDialog=false, const OCRefreshMode RefreshMode=tsRedrawActiveStave, const QString& Tooltip="", const QString& ModifierName="");
    OCToolButtonProps* CreateButton(const QString& IconPath, const bool CustomDialog=false, const OCRefreshMode RefreshMode=tsRedrawActiveStave, const QString& Tooltip="", const QString& ModifierName="");
    OCProperties* m_PropColl;
    QString m_Name;
    bool MTColorCheck(OCDraw& ScreenObj);
public:
    CSymbol(const QString& Name);
    ~CSymbol();
    virtual void plot(XMLSymbolWrapper& Symbol, int XFysic, OCBarList& BarList, OCCounter& CountIt, int BarCounter, OCSignList& SignsToPrint, QColor SignCol, XMLScoreWrapper& Score, int PointerStart, OCSymbolArray& SymbolList, int Stave, int Track, OCNoteList& NoteList, int NoteCount, OCPrintVarsType& dCurrent, OCPrintStaffVarsType& sCurrent, int Pointer, OCDraw& ScreenObj);
    virtual void Edit(XMLSimpleSymbolWrapper& Symbol, OCRefreshMode& RefreshMode, bool& esc, QWidget* parent);
    virtual void fibPlay(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    virtual void fib(XMLSymbolWrapper& Symbol, int TrackNum, OCPrintVarsType& tempsetting);
    virtual void Play(XMLSymbolWrapper& Symbol, OCMIDIFile& MFile, OCCounter& CountIt, int& Py, QDomLiteElement* XMLVoice, OCSignList& SignsToPlay, OCPlayBackVarsType& TemPlay);
    virtual void PlotMTrack(int XFysic, XMLSymbolWrapper& Symbol, int stavedistance, OCPrintVarsType& tempsetting, OCSymbolArray& MTObj, int Pointer, XMLScoreWrapper& Score, OCDraw& ScreenObj);
    virtual void PrintSign(OCSymbolArray& SymbolList, PrintSignProps& SignProps, int UpDown, int &SignsUp, OCDraw& ScreenObj);
    virtual void DuringNote(PlaySignProps& Props, OCMIDIFile& MFile, int Pitch, int& LastTime, int Tick, int PlayTime, OCPlayBackVarsType& TemPlay);
    virtual void BeforeNote(XMLSymbolWrapper& XMLNote, int& PlayDynam, int& Pitch, int& endPitch, OCMIDIFile& MFile, OCPlayBackVarsType& TemPlay);
    virtual void AfterNote(XMLSymbolWrapper& XMLNote, OCPlayBackVarsType& TemPlay);
    virtual void ModifyProperties(OCProperties* p);
    virtual OCProperties* GetProperties();
    virtual OCProperties* GetProperties(int Button);
    const bool PropertyExists(const QString& Name) const;
    const QVariant PropertyValue(const QString& Name) const;
    virtual QList<OCToolButtonProps*> CreateButtons();
    const QString Name() const;
    virtual void InitPlaySymbol(XMLSymbolWrapper& Symbol, OCPlayBackVarsType& TempPlay);
    virtual void InitPrintSymbol(XMLSymbolWrapper& Symbol, OCPrintVarsType& dCurrent);
    virtual void DrawFactor(XMLSymbolWrapper& Symbol, OCCounter* Counter, QDomLiteElement* XMLTemplate, OCBarList& BarList, int Staff, int Voice, int Bar, int Py, XMLScoreWrapper& Score);
};

class CVisibleSymbol :public CSymbol
{
private:
public:
    CVisibleSymbol(const QString& Name, const bool NoInv=false, const bool NoIna=false, const bool NoSize=false);
};

class CDuratedSymbol :public CVisibleSymbol
{
private:
public:
    CDuratedSymbol(const QString& Name, const bool NoInv=false, const bool NoIna=false, const bool NoSize=false);
};

class CGapSymbol :public CDuratedSymbol
{
private:
public:
    CGapSymbol(const QString& Name, const QString& GapName, const QString& Description);
};

class CTextSymbol :public CVisibleSymbol
{
private:
public:
    CTextSymbol(const QString& Name);
};

#endif // CSYMBOL_H
