#ifndef QTOOLBUTTONGRID_H
#define QTOOLBUTTONGRID_H

#include "mainwindow.h"
#include <QWidget>
#include <QFrame>
#include <QGridLayout>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "CommonClasses.h"

#define ButtonGridSize 32

class QToolButtonGrid : public QWidget
{
    Q_OBJECT
public:
    explicit QToolButtonGrid(QWidget *parent = 0);
    ~QToolButtonGrid();
    const bool IsSelected(const QString& Name);
    const bool IsSelected(const int Index);
    void SetSelected(const QString& Name, const bool Selected);
    void SetSelected(const int Index, const bool Selected);
    void AddToTree(const QString& Name, QMacTreeWidget* tw);
signals:
    void ButtonClicked(int Index);
    void ButtonClicked(QString Name);
public slots:
    QToolButton* AddButton(const QString& Name, const QString& Tooltip, const bool Selectable);
    void AddButton(const QString& Name, const QString& Tooltip, const QIcon& Icon, const bool Selectable=false);
    void AddButton(const QString& Name, const QString& Tooltip, const QString& Text, const QFont& Font, const bool Selectable=false);
    void ShowButtons();
protected:
    virtual void resizeEvent(QResizeEvent* event);
    QList<QToolButton*> Buttons;
    QStringList Names;
    QFrame* frame;
    QGridLayout* layout;
private:
    QTreeWidgetItem* TreeItem;
private slots:
    void wasClicked();
};

enum TripletDotFlag
{
    TDFNone = 0,
    TDFTriplet = 1,
    TDFDot = 2
};

enum RecordFlag
{
    RFStop=0,
    RFPlay=1,
    RFRecord=2
};
/*
class CSymbolToolGrid : public QToolButtonGrid
{
    Q_OBJECT
public:
    CSymbolToolGrid(QWidget* parent=0);
public slots:
    //virtual void SelectionChanged(int Ticks);
signals:
    //void PasteProperties(OCProperties *Properties, QString UndoText, bool Finished);
    //void OverwriteProperty(QString Name,QVariant Value, QString UndoText, bool Finished);
protected:
    //void PasteProperties(OCProperties* p);
private slots:
    //virtual void wasClicked1(int Button);
};
*/
class CPropertiesToolGrid : public QToolButtonGrid
{
    Q_OBJECT
public:
    CPropertiesToolGrid(QWidget* parent=0);
    ~CPropertiesToolGrid();
    //void AddButton(QString Name, QString Tooltip, QIcon Icon, QString ClassName, OCRefreshMode RefreshMode, bool CustomDialog=false, QString PropertyName="", QVariant PropertyValue="",QString PropertyName1="", QString PresetName1="",QString PropertyName2="", QString PresetName2="", QString ModifierName="");
    //void AddButton(QString Name, QString Tooltip, QString Text, QFont Font, QString ClassName, OCRefreshMode RefreshMode, bool CustomDialog=false, QString PropertyName="", QVariant PropertyValue="",QString PropertyName1="", QString PresetName1="",QString PropertyName2="", QString PresetName2="", QString ModifierName="");
    void AddButton(const QString& SymbolName);
    void AddModifierButton(const QString& Name, const QString& Tooltip, const QIcon& Icon);
    void AddModifierButton(const QString& Name, const QString& Tooltip, const QString& Text, const QFont& Font);
signals:
    void PasteXML(XMLSimpleSymbolWrapper& Symbol, QString UndoText, OCRefreshMode RefreshMode);
private:
    QList<OCToolButtonProps*> ButtonProps;
private slots:
    void wasClicked1(int Button);
};

#endif // QTOOLBUTTONGRID_H
