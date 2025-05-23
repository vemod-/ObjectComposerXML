#ifndef CLAYOUT_H
#define CLAYOUT_H

#include "ocscore.h"
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QPrinter>
#include <QPageSetupDialog>
#include <QPrintDialog>
#else
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPageSetupDialog>
#include <QtPrintSupport/QPrintDialog>
#endif
//class LayoutViewXML;

enum MarginDirections
{
    MDTop=0,
    MDRight=1,
    MDBottom=2,
    MDLeft=3
};

class CLayoutSystem : public XMLLayoutSystemWrapper
{
public:
    inline CLayoutSystem(QDomLiteElement* e) : XMLLayoutSystemWrapper(e) {}
    ~CLayoutSystem();
    void plot(OCScore* Score, const XMLScoreWrapper& XMLScore, const XMLLayoutFontsWrapper& Fonts, const QRectF& PageRect, const XMLLayoutOptionsWrapper& Options, OCDraw& ScreenObj);
    void Format(OCScore* Score, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& LayoutTemplate, const XMLLayoutOptionsWrapper& Options, const int m_StartBar, const bool Auto, const bool ShowAllStaves, const int ShowNamesOption, const double SystemLength);
    void SetStaveDistance(const double stavedistance);
    void Erase(QGraphicsScene* Scene);
private:
    OCGraphicsList SystemList;
};

class CLayoutPage : public XMLLayoutPageWrapper
{
public:
    CLayoutPage(QDomLiteElement* e);
    ~CLayoutPage();
    void PlotSystem(const int System, OCScore* Score, const XMLScoreWrapper& XMLScore, const XMLLayoutFontsWrapper& Fonts, const QRectF& PageRect, const XMLLayoutOptionsWrapper& Options, OCDraw& ScreenObj);
    void Format(const int System, const double TitleHeight, OCScore* Score, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& LayoutTemplate, const XMLLayoutOptionsWrapper& Options, int& StartBar, const bool Auto, const bool ShowAllStaves, const int ShowNamesOption, const double SystemLength);
    void AddSystem();
    CLayoutSystem* Sys(const int Index) const;
    CLayoutSystem* takeFirstSystem();
    CLayoutSystem* takeLastSystem();
    void PrependSystem(CLayoutSystem* System);
    void AppendSystem(CLayoutSystem* System);
    void RemoveSystem();
    double YSpaceUsed(const int System, const double TitleHeight) const;
    double SysTop(const int System) const;
    double SysHeight(const int System) const;
    double SysLen(const int System) const;
    int StartBar(const int System) const;
    void AddBar(const int System, const bool Top);
    bool RemoveBar(const int System, const bool Top);
    void AdjustSystems(const int PageHeight, const double TitleHeight);
    void FormatTitle(const double TitleHeight);
    void PlotTitle(const int Page, const XMLLayoutFontsWrapper& Fonts, const QRectF& PageRect, const XMLLayoutOptionsWrapper& Options, const QString& LayoutName, OCDraw& ScreenObj);
    void Erase(const int System, QGraphicsScene* Scene);
    void EraseTitle(QGraphicsScene* Scene);
private:
    QList<CLayoutSystem*> Systems;
    OCGraphicsList TitleList;
};


class CLayout : public XMLLayoutWrapper
{
public:
    CLayout(QDomLiteElement* e);
    ~CLayout();
    QPrinter* Printer;
    //double viewSize;
    OCScore* Score;
    CLayoutPage* activePage();
    CLayoutSystem* activeSystem();
    //double PrinterResFactor;
    void AddPage();
    double MMToPixelX(const double MM) const;
    double MMToPixelY(const double MM) const;
    const QRectF PageRect() const;
    const QRectF PageRect(const int page) const;
    const QRectF PaperRect() const;
    const QRectF PaperRect(const int page) const;
    void Plot(const LayoutLocation& l, XMLScoreWrapper& XMLScore, QGraphicsScene *Scene);
    void AutoAll(const LayoutLocation& StartLocation, XMLScoreWrapper& XMLScore, QGraphicsScene *Scene, const bool Auto);
    void RemovePage();
    void RemoveSystem(const int Page);
    int NumOfSystems(const int Page) const;
    void AddSystem(const int Page);
    void assignXML(QDomLiteElement* data);
    void setActiveLocation(const LayoutLocation& l = LayoutLocation(0,0));
    int activeSystemIndex() const;
    int activePageIndex() const;
    double SysTop(const LayoutLocation& l) const;
    double SysHeight(const LayoutLocation& l) const;
    double SysLen(const LayoutLocation& l) const;
    QRectF SysRect(const LayoutLocation& l) const;
    int MoveBar(LayoutLocation& l, const int Direction);
    void MoveSystem(const int Page, const int Direction);
    double TitleHeight(const int Page);
    void AdjustSystems(const int Page);
    void FormatTitle();
    void initPrinter();
    bool ChangePageSetup(QWidget* Owner);
    bool ChangePrinter(QWidget* Owner,QPrinter* Prn);
    void PlotTitle(QGraphicsScene* Scene);
    void PrintIt(const int StartPage, XMLScoreWrapper& XMLScore, QGraphicsScene *Scene);
    void Erase(const LayoutLocation& l, QGraphicsScene* Scene);
    void EraseTitle(QGraphicsScene* Scene);
private:
    QRectF m_PaperRect;
    LayoutLocation m_ActiveLocation;
    QList<CLayoutPage*> Pages;
    void AutoClear(const LayoutLocation& StartLocation);
};

class CLayoutCollection : public XMLLayoutCollectionWrapper
{
public:
    CLayoutCollection();
    ~CLayoutCollection();
    CLayout* activeLayout() const;
    void InitLayout(const int Index, XMLScoreWrapper& Score);
    void setActiveLayout(const QString& Name);
    void setActiveLayout(const int index);
    int activeLayoutIndex() const;
    CLayout* layout(const int index);
    int activePageIndex() const;
    int activeSystemIndex() const;
    LayoutLocation activeLayoutLocation() const;
    void assignXML(const XMLScoreWrapper& score);
    bool ChangePageSetup(QWidget* Owner);
    bool ChangePrinter(QWidget* Owner);
    OCScore Score;
private:
    int m_ActiveLayout;
    QList<CLayout*> Layouts;
};

#endif // CLAYOUT_H
