#ifndef CLAYOUT_H
#define CLAYOUT_H

#include "scoreviewxml.h"
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QPrinter>
#include <QPageSetupDialog>
#include <QPrintDialog>
#else
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPageSetupDialog>
#include <QtPrintSupport/QPrintDialog>
#endif
class LayoutViewXML;

enum MarginDirections
{
    MDTop=0,
    MDRight=1,
    MDBottom=2,
    MDLeft=3
};

class CLayoutFonts
{
public:
    CLayoutFonts();
    CTextElement Title;
    CTextElement SubTitle;
    CTextElement Composer;
    CTextElement Names;
    void Load(QDomLiteElement* data);
    QDomLiteElement* Save(const QString& Tag);
};

class CLayoutOptions
{
public:
    int ShowNamesSwitch;
    bool ShowAllOnSys1;
    bool TransposeInstruments;
    float ScoreType;
    float ScaleSize;
    int NoteSpace;
    bool DontShowBN;
    int BarNrOffset;
    int MasterStave;
    int TopMargin;
    int LeftMargin;
    int RightMargin;
    int BottomMargin;
    int Orientation;
    int PaperSize;
    CLayoutOptions();
    void Load(QDomLiteElement* data);
    QDomLiteElement* Save(const QString& Tag);
};

class CLayoutSystem
{
public:
    QDomLiteElement Template;
    int StartBar;
    int EndBar;
    int PageNr;
    int ShowNames;
    int UpDown;
    int Top;
    int DefaultTop;
    int Distance;
    int Height;
    int Syslen;
    CLayoutSystem();
    ~CLayoutSystem();
    void plot(OCScore& Score, XMLScoreWrapper& XMLScore, CLayoutFonts& Fonts, const int viewsize, const int Left, const int LayTop, CLayoutOptions& Options, OCDraw& ScreenObj, const float PrinterResFactor);
    void Format(OCScore& Score, XMLScoreWrapper& XMLScore, QDomLiteElement* LayoutTemplate, const int m_StartBar, const bool Auto, const bool ShowAllStaves, const int ShowNamesOption, const int SystemLength);
    void Load(QDomLiteElement* data);
    QDomLiteElement* Save(const QString& Tag);
    void SetStaveDistance(const int stavedistance);
    void SetHeight();
    void Erase(QGraphicsScene* Scene);
private:
    QList<QGraphicsItem*> SystemList;
};

class CLayoutPage
{
public:
    CLayoutSystem* ActiveSystem;
    CLayoutPage();
    ~CLayoutPage();
    const int NumOfSystems() const;
    void PlotSystem(const int System, OCScore& Score, XMLScoreWrapper& XMLScore, CLayoutFonts& Fonts, const int viewsize, const int Left, const int Top, CLayoutOptions& Options, OCDraw& ScreenObj, const float PrinterResFactor);
    void Format(const int System, OCScore& Score, XMLScoreWrapper& XMLScore, QDomLiteElement* LayoutTemplate, int& StartBar, const bool Auto, const bool ShowAllStaves, const int ShowNamesOption, const int SystemLength);
    void Load(QDomLiteElement* data);
    QDomLiteElement* Save(const QString& Tag);
    void AddSystem();
    CLayoutSystem* Sys(const int Index);
    CLayoutSystem* GetSystem(const bool Top);
    void InsertSystem(const bool Top, CLayoutSystem* System);
    void RemoveSystem(const bool Top);
    const int YSpaceUsed(const int CurrentSystem) const;
    void ClearAllSystems();
    const int CurrentHeight(const int CurrentSystem) const;
    void setActiveSystem(const int System);
    const int getActiveSystem() const;
    const int SysTop(const int System) const;
    const int SysHeight(const int System) const;
    const int StartBar(const int System) const;
    void AddBar(const int System, const bool Top);
    const bool RemoveBar(const int System, const bool Top);
    void AdjustSystems(const int PageHeight);
    void FormatTitle();
    void PlotTitle(const int Page, CLayoutFonts& Fonts, const int viewsize, const int PageWidth, const int LayTop, CLayoutOptions& Options, const int LayRight, OCDraw& ScreenObj, const float PrinterResFactor);
    void Erase(const int System, QGraphicsScene* Scene);
    void EraseTitle(QGraphicsScene* Scene);
private:
    QList<CLayoutSystem*> Systems;
    QList<QGraphicsItem*> TitleList;
    int TitleHeight;
};


class CLayout
{
public:
    CLayout();
    ~CLayout();
    bool PrinterExist;
    QPrinter* Printer;
    OCScore Score;
    CLayoutPage* ActivePage;
    CLayoutFonts Fonts;
    CLayoutOptions Options;
    QString Name;
    QDomLiteElement Template;
    bool IsFormated;
    float Viewsize;
    float PrinterResFactor;
    void AddPage();
    const int NumOfPages() const;
    const float MMToPixelX(const float MM) const;
    const float MMToPixelY(const float MM) const;
    const int MarginTop() const;
    const int MarginLeft() const;
    const int MarginRight() const;
    const int MarginBottom() const;
    const QRectF PageRect() const;
    const int PageWidth() const;
    const int PageHeight() const;
    const QRectF PaperRect() const;
    const int PaperWidth() const;
    const int PaperHeight() const;
    void Plot(const int Page, const int System, XMLScoreWrapper& XMLScore, QGraphicsScene *Scene, const int PageLeft);
    void AutoAll(const int StartPage, const int StartSystem, XMLScoreWrapper& XMLScore, QGraphicsScene *Scene, const bool Auto, LayoutViewXML* lv);
    void RemovePage();
    void RemoveSystem(const int Page);
    const int NumOfSystems(const int Page) const;
    void AddSystem(const int Page);
    QDomLiteElement* Save(const QString& Tag);
    void Load(QDomLiteElement* data);
    void setActiveObjects(const int Page, const int System);
    const int getActiveSystem() const;
    const int getActivePage() const;
    const int SysTop(const int Page, const int System) const;
    const int SysHeight(const int Page, const int System) const;
    const int MoveBar(int& Page, int& System, const int Direction);
    void MoveSystem(const int Page, const int Direction);
    void AdjustSystems(const int Page);
    void FormatTitle();
    void GetPrinter();
    void SetPrinter();
    const bool ChangePageSetup(QWidget* Owner);
    const bool ChangePrinter(QWidget* Owner,QPrinter* Prn);
    void PlotTitle(QGraphicsScene* Scene);
    void PrintIt(const int StartPage, XMLScoreWrapper& XMLScore, QGraphicsScene *Scene);
    void Erase(const int Page, const int System, QGraphicsScene* Scene);
    void EraseTitle(QGraphicsScene* Scene);
private:
    QList<CLayoutPage*> Pages;
    void AutoClear(const int StartPage, const int StartSystem);
};

class CLayoutCollection
{
public:
    CLayoutCollection();
    ~CLayoutCollection();
    CLayout* CurrentLayout;
    void AddLayout(const QString& Name=QString());
    void RemoveLayout(const int Index);
    const QString Name(const int Index) const;
    void InitLayout(const int Index, XMLScoreWrapper& Score);
    void SetCurrentLayout(const QString& Name);
    void SetCurrentLayout(const int index);
    const int getCurrentLayout() const;
    const int getActivePage() const;
    const int getActiveSystem() const;
    QDomLiteElement* Save();
    void Load(QDomLiteElement* data, XMLScoreWrapper& Score);
    bool ChangePageSetup(QWidget* Owner);
    bool ChangePrinter(QWidget* Owner);
    const int NumOfLayouts() const;
    void Clear();
private:
    QList<CLayout*> Layouts;
};

#endif // CLAYOUT_H
