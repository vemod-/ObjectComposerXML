#include "clayout.h"

CLayoutSystem::~CLayoutSystem() {}

void CLayoutSystem::Erase(QGraphicsScene* Scene)
{
    for (QGraphicsItem* item : std::as_const(SystemList))
    {
        if (Scene->items().contains(item))
        {
            Scene->removeItem(item);
            delete item;
        }
    }
    SystemList.clear();
}

void CLayoutSystem::plot(OCScore* Score, const XMLScoreWrapper& XMLScore, const XMLLayoutFontsWrapper& Fonts, const QRectF& PageRect, const XMLLayoutOptionsWrapper& Options, OCDraw& ScreenObj)
{
    setSysLen(Score->systemLength());
    double NameLen = 0;
    SystemList.clear();
    ScreenObj.StartList();
    if (showNames() > 0)
    {
        for (int StaffPos = 0; StaffPos<Template.staffCount();StaffPos++)
        {
            const int StaffId=Template.staffId(StaffPos);
            QString Text;
            switch (showNames())
            {
            case 1:
                Text = XMLScore.StaffAbbreviation(StaffId).trimmed();
                break;
            case 2:
                Text = XMLScore.StaffName(StaffId).trimmed();
                break;
            }
            const double TopAdjust=(ScoreStaffLinesHeight-Fonts.names.textHeight(Text))/2;
            ScreenObj.PrintFontElement(PageRect.left(),(Template.staffTopFromId(StaffId) + top() + TopAdjust)/Options.scaleSize() + PageRect.top(),Text,Fonts.names, Options.scaleSize());
            NameLen = fmax(Fonts.names.textWidth(Text),NameLen);
        }
        if (NameLen > 0)
        {
            NameLen += 240;
            setSysLen(sysLen() - NameLen);
            Score->reformatPage(int(sysLen()));
        }
    }
    for (int StaffPos = 0; StaffPos<Template.staffCount();StaffPos++)
    {
        const int StaffId=Template.staffId(StaffPos);
        ScreenObj.init(Options.scale(PageRect.left()) + NameLen,Template.staffTopFromId(StaffId) + top() + Options.scale(PageRect.top()));
        Score->plotStaffNoList(StaffId, XMLScore, Template, Options, Qt::black, ScreenObj);
    }
    SystemList.append(ScreenObj.EndList());
    for (QGraphicsItem* item : std::as_const(SystemList)) item->setZValue(2);
}

void CLayoutSystem::Format(OCScore* Score, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& LayoutTemplate, const XMLLayoutOptionsWrapper& Options, const int m_StartBar, const bool Auto, const bool ShowAllStaves, const int ShowNamesOption, const double SystemLength)
{
    if (Auto)
    {
        setEndBar(0);
        setStartBar(m_StartBar);
    }
    Score->formatPage(XMLScore, LayoutTemplate, Options, SystemLength, startBar(), endBar());
    if (Auto)
    {
        Template.clear();
        if (ShowNamesOption == 1) setShowNames(1);
        for (int c = 0; c<LayoutTemplate.staffCount(); c++)
        {
            const bool StaffEmpty=Score->StaffEmpty(c,LayoutTemplate);
            if (StaffEmpty && (ShowNamesOption == 2)) setShowNames(1);
            if (!StaffEmpty || ShowAllStaves) Template.addChild(LayoutTemplate.staff(c));
        }
        if (Template.staffCount()==0) Template.addChild(LayoutTemplate.staff(0));
        Template.validateBrackets();
        setEndBar(startBar() + Score->barsActuallyPrinted());
        setHeight(Template.height());
    }
    else
    {
        for (int StaffPos=0;StaffPos<Template.staffCount();StaffPos++)
        {
            Template.staff(StaffPos).copyBrackets(LayoutTemplate.staffFromId(Template.staffId(StaffPos)));
        }
        Template.validateBrackets();
    }
}

void CLayoutSystem::SetStaveDistance(const double stavedistance) {
    for (int i = 0; i<Template.staffCount();i++) {
        XMLTemplateStaffWrapper s(Template.staff(i));
        s.setHeight(int(stavedistance));
    }
    setHeight(Template.height());
}

CLayoutPage::CLayoutPage(QDomLiteElement *e) : XMLLayoutPageWrapper(e) {
    for (QDomLiteElement* s : (const QDomLiteElementList)e->elementsByTag("System")) {
        Systems.append(new CLayoutSystem(s));
    }
}

CLayoutPage::~CLayoutPage()
{
    qDeleteAll(Systems);
}

void CLayoutPage::PlotSystem(const int System, OCScore* Score, const XMLScoreWrapper& XMLScore, const XMLLayoutFontsWrapper& Fonts, const QRectF& PageRect, const XMLLayoutOptionsWrapper& Options, OCDraw& ScreenObj)
{
    Sys(System)->plot(Score,XMLScore,Fonts,PageRect,Options,ScreenObj);
}

void CLayoutPage::Format(const int System, const double TitleHeight, OCScore* Score, const XMLScoreWrapper& XMLScore, const XMLTemplateWrapper& LayoutTemplate, const XMLLayoutOptionsWrapper& Options, int& StartBar, const bool Auto, const bool ShowAllStaves, const int ShowNamesOption, const double SystemLength)
{
    CLayoutSystem* s=Sys(System);
    s->Format(Score, XMLScore, LayoutTemplate, Options, StartBar, Auto, ShowAllStaves, ShowNamesOption, SystemLength);
    if (Auto)
    {
        StartBar = s->endBar();
        s->setTop(YSpaceUsed(System-1,TitleHeight));
        s->setDefaultTop(s->top());
    }
}

void CLayoutPage::AddSystem() {
    Systems.append(new CLayoutSystem(XMLElement->appendChild("System")));
}

CLayoutSystem *CLayoutPage::Sys(const int Index) const {
    return Systems[Index];
}

CLayoutSystem *CLayoutPage::takeFirstSystem() {
    XMLElement->takeFirst();
    return Systems.takeFirst();
}

CLayoutSystem *CLayoutPage::takeLastSystem() {
    XMLElement->takeLast();
    return Systems.takeLast();
}

void CLayoutPage::PrependSystem(CLayoutSystem *System) {
    Systems.prepend(System);
    XMLElement->prependChild(System->xml());
}

void CLayoutPage::AppendSystem(CLayoutSystem *System) {
    Systems.append(System);
    XMLElement->appendChild(System->xml());
}

void CLayoutPage::RemoveSystem() {
    if (!Systems.isEmpty()) {
        //XMLElement->takeLast();
        //delete Systems.takeLast();
        delete takeLastSystem();
    }
}

double CLayoutPage::YSpaceUsed(const int System, const double TitleHeight) const {
    if (System > -1) return SysTop(System) + SysHeight(System);
    return TitleHeight;
}

double CLayoutPage::SysTop(const int System) const {
    //if (Systems.empty()) return 0;
    if (System >= Systems.size()) return 0;
    return Sys(System)->top();
}

double CLayoutPage::SysHeight(const int System) const {
    //if (Systems.empty()) return 0;
    if (System >= Systems.size()) return 0;
    return Sys(System)->height();
}

double CLayoutPage::SysLen(const int System) const {
    //if (Systems.empty()) return 0;
    if (System >= Systems.size()) return 0;
    return Sys(System)->sysLen();
}

int CLayoutPage::StartBar(const int System) const {
    //if (Systems.empty()) return 0;
    if (System >= Systems.size()) return 0;
    return Sys(System)->startBar();
}

void CLayoutPage::AddBar(const int System, const bool Top)
{
    CLayoutSystem* s=Sys(System);
    (Top) ? s->setStartBar(s->startBar()-1) : s->setEndBar(s->endBar()+1);
}

bool CLayoutPage::RemoveBar(const int System, const bool Top)
{
    CLayoutSystem* s=Sys(System);
    if (s->startBar() + 1 == s->endBar()) return false;
    (Top) ? s->setStartBar(s->startBar()+1) : s->setEndBar(s->endBar()-1);
    return true;
}

void CLayoutPage::AdjustSystems(const int PageHeight, const double TitleHeight)
{
    int NumOfStaffs=0;
    for (int i=0;i<systemCount();i++)
    {
        NumOfStaffs += Sys(i)->Template.staffCount();
    }
    double FirstTop = Sys(0)->top();
    if (FirstTop < TitleHeight) FirstTop = TitleHeight;
    double YSpaceAvailable = PageHeight - FirstTop;
    double stavedistance=DoubleDiv(YSpaceAvailable-((ScoreStaffHeight*NumOfStaffs)+(240*(systemCount()-1))),NumOfStaffs)/12;
    double YSpaceUsed = FirstTop;
    for (int i=0;i<systemCount();i++)
    {
        CLayoutSystem* s=Sys(i);
        s->SetStaveDistance(stavedistance);
        s->setTop(YSpaceUsed);
        s->setDefaultTop(YSpaceUsed);
        YSpaceUsed += s->height();
    }
}

void CLayoutPage::FormatTitle(const double TitleHeight)
{
    double YSpaceUsed = TitleHeight;
    for (int i=0;i<systemCount();i++)
    {
        CLayoutSystem* s=Sys(i);
        s->setTop(YSpaceUsed);
        s->setDefaultTop(YSpaceUsed);
        YSpaceUsed += s->height();
    }
}

void CLayoutPage::Erase(const int System, QGraphicsScene *Scene)
{
    if (System < systemCount()) Sys(System)->Erase(Scene);
}

void CLayoutPage::EraseTitle(QGraphicsScene* Scene)
{
    for (QGraphicsItem* item : std::as_const(TitleList))
    {
        if (Scene->items().contains(item))
        {
            Scene->removeItem(item);
            delete item;
        }
    }
    TitleList.clear();
}

void CLayoutPage::PlotTitle(const int Page, const XMLLayoutFontsWrapper& Fonts, const QRectF& PageRect, const XMLLayoutOptionsWrapper& Options, const QString& LayoutName, OCDraw& ScreenObj)
{
    TitleList.clear();
    ScreenObj.StartList();
    ScreenObj.col = Qt::black;
    double TitleHeight = 0;
    if (Page > 0) return;
    if (Options.showLayoutName()) {
        ScreenObj.PrintFontElement(PageRect.left(), PageRect.top(), LayoutName, Fonts.composer, 1);
    }
    if (!Fonts.title.empty())
    {
        double Halfwidth = Fonts.title.textWidth() / 2.0;  //' Calculate one-half width.
        double TextHeight = Fonts.title.textHeight();
        TitleHeight += TextHeight;
        ScreenObj.PrintTextElement(PageRect.center().x() - Halfwidth, PageRect.top(), Fonts.title, 1);
    }
    if (!Fonts.subtitle.empty())
    {
        double Halfwidth = Fonts.subtitle.textWidth() / 2.0; // ' Calculate one-half width.
        double TextHeight = Fonts.subtitle.textHeight();
        ScreenObj.PrintTextElement(PageRect.center().x() - Halfwidth, PageRect.top() + TitleHeight,Fonts.subtitle, 1);
        TitleHeight += TextHeight;
    }
    if (!Fonts.composer.empty())
    {
        double TextWidth = Fonts.composer.textWidth();
        //double TextHeight = Fonts.composer.textHeight();
        ScreenObj.PrintTextElement(PageRect.right()-TextWidth , PageRect.top() + TitleHeight,Fonts.composer, 1);
        //TitleHeight += TextHeight;
    }
    TitleList.append(ScreenObj.EndList());
    for (QGraphicsItem* g : std::as_const(TitleList)) g->setZValue(2);//int i=0;i<TitleList.size();i++) TitleList[i]->setZValue(2);
}

CLayout::CLayout(QDomLiteElement *e) : XMLLayoutWrapper(e)
{
    Printer=new QPrinter(QPrinter::HighResolution);
    initPrinter();
    m_ActiveLocation.System=0;
    m_ActiveLocation.Page=0;
    for (QDomLiteElement* p : (const QDomLiteElementList)e->elementsByTag("Page")) Pages.append(new CLayoutPage(p));
}

CLayout::~CLayout()
{
    qDeleteAll(Pages);
    delete Printer;
}

CLayoutPage *CLayout::activePage() { return Pages[m_ActiveLocation.Page]; }

CLayoutSystem *CLayout::activeSystem() { return activePage()->Sys(m_ActiveLocation.System); }

void CLayout::AddPage()
{
    Pages.append(new CLayoutPage(XMLElement->appendChild("Page")));
}

double CLayout::MMToPixelX(const double MM) const
{
    static const double PixelPerMM = 720.0 / 25.4;//Printer->logicalDpiX()/25.4;
    return (MM*PixelPerMM);//PrinterResFactor;
}

double CLayout::MMToPixelY(const double MM) const
{
    static const double PixelPerMM = 720.0 / 25.4;//Printer->logicalDpiY()/25.4;
    return (MM*PixelPerMM);//PrinterResFactor;
}

const QRectF CLayout::PageRect() const
{
    return m_PaperRect.adjusted(MMToPixelX(Options.leftMargin()),MMToPixelY(Options.topMargin()),-MMToPixelX(Options.rightMargin()),-MMToPixelY(Options.bottomMargin()));
}

const QRectF CLayout::PageRect(const int page) const
{
    return PageRect().translated(m_PaperRect.width()*page,0);
}

const QRectF CLayout::PaperRect() const
{
    return m_PaperRect;
}

const QRectF CLayout::PaperRect(const int page) const
{
    return m_PaperRect.translated(m_PaperRect.width()*page,0);
}


void CLayout::Plot(const LayoutLocation& l, XMLScoreWrapper& XMLScore, QGraphicsScene *Scene)
{
    OCDraw ScreenObj(Scene,Options.scaleSize());
    int StartBar=0;
    Pages[l.Page]->Format(l.System, TitleHeight(l.Page), Score, XMLScore, Template, Options, StartBar, false, Options.showAllOnFirstSystem(), 0, Options.scale(PageRect().width()));
    Pages[l.Page]->PlotSystem(l.System, Score, XMLScore, Fonts, PageRect(l.Page), Options, ScreenObj);
}

void CLayout::AutoAll(const LayoutLocation& StartLocation, XMLScoreWrapper& XMLScore, QGraphicsScene *Scene, const bool Auto)
{
    bool Finished=false;
    //OCCursor c;
    OCDraw ScreenObj(Scene,Options.scaleSize());
    //ScreenObj.Cursor = &c;
    LayoutLocation l(StartLocation);
    while (l.Page > Pages.size()-1) AddPage();
    Pages[l.Page]->PlotTitle(l.Page, Fonts, PageRect(), Options, name(), ScreenObj);
    int StartBar=0;
    //qDebug() << Options.scaleSize() << Options.size() << Options.scoreType() << Options.layoutZoom();
    forever
    {
        while (l.Page > Pages.size()-1) AddPage();
        forever
        {
            const bool ShowAllStaves = (l.isFirstSystem() && Options.showAllOnFirstSystem());
            while (l.System > Pages[l.Page]->systemCount()-1) Pages[l.Page]->AddSystem();
            if (l.matches(StartLocation))
            {
                if (l.System > 0)
                {
                    StartBar = Pages[l.Page]->Sys(l.System-1)->endBar();
                }
                else if (!l.isFirstPage())
                {
                    StartBar = Pages[l.Page-1]->Sys(Pages[l.Page-1]->systemCount()-1)->endBar();
                }
            }
            int ShowNames = Options.showNamesSwitch();
            if (l.isFirstSystem() && (Options.showNamesSwitch() > 1)) ShowNames = 1;
            Pages[l.Page]->Format(l.System, TitleHeight(l.Page), Score, XMLScore, Template, Options, StartBar, Auto, ShowAllStaves, ShowNames, Options.scale(PageRect().width()));
            if (Score->isEnded(Template))
            {
                if (!l.isFirstSystem())
                {
                    Pages[l.Page]->RemoveSystem();
                    if (l.System == 0) RemovePage();
                    break;
                }
            }
            Pages[l.Page]->PlotSystem(l.System, Score, XMLScore, Fonts, PageRect(l.Page), Options, ScreenObj);
            if (Auto)
            {
                if (Pages[l.Page]->YSpaceUsed(l.System,l.Page) + Pages[l.Page]->SysHeight(l.System) > Options.scale(PageRect().height())) break;
            }
            else
            {
                if (isLastSystem(l))
                {
                    l.System ++;
                    Finished=true;
                    break;
                }
                if (isBottomSystem(l)) break;
            }
            l.System ++;
        }
        if (Score->isEnded(Template) || Finished) break;
        l.System = 0;
        l.Page ++;
    }
    AutoClear(l);
}

void CLayout::RemovePage()
{
    if (Pages.size() > 1)
    {
        while (Pages.last()->systemCount()) Pages.last()->RemoveSystem();
        const QDomLiteElementList l = XMLElement->elementsByTag("Page");
        XMLElement->takeChild(l.last());
        delete Pages.takeLast();
    }
}

void CLayout::RemoveSystem(const int Page)
{
    Pages[Page]->RemoveSystem();
}

int CLayout::NumOfSystems(const int Page) const
{
    if (Page >= Pages.size()) return 0;
    return Pages[Page]->systemCount();
}

void CLayout::AddSystem(const int Page)
{
    Pages[Page]->AddSystem();
}

void CLayout::assignXML(QDomLiteElement *data)
{
    qDeleteAll(Pages);
    Pages.clear();
    shadowXML(data);
    for (QDomLiteElement* p : (const QDomLiteElementList)data->elementsByTag("Page"))
    {
        Pages.append(new CLayoutPage(p));
    }
}

void CLayout::setActiveLocation(const LayoutLocation &l)
{
    int P = 0;
    if (Pages.size() > 0) P=qBound<int>(0,l.Page,Pages.size()-1);
    if (!Pages.empty()) {
        if (Pages[P]->systemCount()==0) P = loBound<int>(0,P-1);
    }
    m_ActiveLocation.Page = P;
    m_ActiveLocation.System=loBound<int>(0,l.System);
}

int CLayout::activeSystemIndex() const
{
    if (Pages.isEmpty()) return 0;
    return m_ActiveLocation.System;
}

int CLayout::activePageIndex() const
{
    if (Pages.isEmpty()) return 0;
    return m_ActiveLocation.Page;
}

double CLayout::SysTop(const LayoutLocation &l) const
{
    if (l.Page >= Pages.size()) return 0;
    return (Pages[l.Page]->SysTop(l.System) / Options.scaleSize()) + PageRect().top();
}

double CLayout::SysHeight(const LayoutLocation &l) const
{
    if (l.Page >= Pages.size()) return 0;
    return Pages[l.Page]->SysHeight(l.System) / Options.scaleSize();
}

double CLayout::SysLen(const LayoutLocation &l) const
{
    return Pages[l.Page]->SysLen(l.System) / Options.scaleSize();
}

QRectF CLayout::SysRect(const LayoutLocation &l) const
{
    if (l.Page >= Pages.size()) return QRectF();
    QRectF r(PageRect(l.Page));
    r.setTop(SysTop(l));
    r.setHeight(SysHeight(l));
    r.setLeft(r.right() - SysLen(l));
    return r;
}

void CLayout::AutoClear(const LayoutLocation& StartLocation)
{
    LayoutLocation l(StartLocation);
    if (l.isFirstSystem()) l.System ++;
    if (l.Page < Pages.size())
    {
        if (l.System == 0)
        {
            l.Page --;
        }
        else
        {
            for (int i = l.System; i< Pages[l.Page]->systemCount(); i++)
            {
                Pages[l.Page]->RemoveSystem();
            }
        }
    }
    while (Pages.size()-1 > l.Page) delete Pages.takeLast();
}

int CLayout::MoveBar(LayoutLocation& l, const int Direction)
{
    const LayoutLocation l1(l);
    int RetVal=0;
    switch (Direction)
    {
    case 1:
        if (Pages[l.Page]->RemoveBar(l.System, false))
        {
            if (!nextSystem(l))
            {
                l.System++;
                Pages[l.Page]->AddSystem();
                CLayoutSystem* s1=Pages[l.Page]->Sys(l1.System);
                CLayoutSystem* s2=Pages[l.Page]->Sys(l.System);
                s2->setTop(s1->top()+s1->height());
                s2->setHeight(s1->height());
                s2->setDefaultTop(s2->top());
                s2->setSysLen(s1->sysLen());
                s2->setStartBar(s1->endBar()+1);
                s2->setEndBar(s2->startBar()-1);
                s2->Template.copy(s1->Template);
            }
            /*
            l.System ++;
            if (l.System > Pages[l.Page]->systemCount()-1)
            {
                if (l.Page < Pages.size()-1)
                {
                    l.Page ++;
                    l.System = 0;
                }
                else
                {
                    Pages[l.Page]->AddSystem();
                    CLayoutSystem* s1=Pages[l.Page]->Sys(l1.System);
                    CLayoutSystem* s2=Pages[l.Page]->Sys(l.System);
                    s2->setTop(s1->top()+s1->height());
                    s2->setHeight(s1->height());
                    s2->setDefaultTop(s2->top());
                    s2->setSysLen(s1->sysLen());
                    s2->setStartBar(s1->endBar()+1);
                    s2->setEndBar(s2->startBar()-1);
                    s2->Template.copy(s1->Template);
                }
            }
            */
            Pages[l.Page]->AddBar(l.System, true);
            RetVal = -1;
        }
        break;
    case -1:
        /*
        l.System ++;
        if (l.System > Pages[l.Page]->systemCount()-1)
        {
            l.Page ++;
            if (l.Page > Pages.size()-1)
            {
                l = l1;
                return RetVal;
            }
            l.System = 0;
        }
        */
        if (!nextSystem(l)) return RetVal;
        if (Pages[l.Page]->RemoveBar(l.System, true))
        {
            Pages[l1.Page]->AddBar(l1.System, false);
            if (isLastSystem(l))
            {
                if (Pages[l.Page]->Sys(l.System)->startBar()>=Pages[l.Page]->Sys(l.System)->endBar())
                {
                    return 1;
                }
            }
            RetVal = -1;
        }
        else
        {
            if (isLastSystem(l))
            {
                Pages[l1.Page]->AddBar(l1.System, false);
                RetVal = 1;
            }
        }
    }
    if (RetVal == 0) l = l1;
    return RetVal;
}

void CLayout::MoveSystem(const int Page, const int Direction)
{
    switch (Sgn<int>(Direction))
    {
    case 1:
        if (isLastPage(Page)) AddPage();
        Pages[Page + 1]->PrependSystem(Pages[Page]->takeLastSystem());
        break;
    case -1:
        if (!isLastPage(Page))
        {
            Pages[Page]->AppendSystem(Pages[Page + 1]->takeFirstSystem());
        }
    }
}

double CLayout::TitleHeight(const int Page)
{
    if (Page==0) return Fonts.height();
    return 0;
}

void CLayout::AdjustSystems(const int Page)
{
    Pages[Page]->AdjustSystems(int(Options.scale(PageRect().height() - 400)), TitleHeight(Page));
}

void CLayout::FormatTitle()
{
    Pages.first()->FormatTitle(TitleHeight(0));
}

void CLayout::PrintIt(const int StartPage, XMLScoreWrapper& XMLScore, QGraphicsScene *Scene)
{
    OCDraw ScreenObj(Scene,Options.scaleSize());
    LayoutLocation l(StartPage,0);
    CLayoutPage* p = Pages[l.Page];
    int StartBar = p->StartBar(0);
    p->PlotTitle(l.Page, Fonts, PageRect(), Options, name(), ScreenObj);
    forever
    {
        const bool ShowAllStaves = (l.isFirstSystem() && Options.showAllOnFirstSystem());
        if (l.System > p->systemCount()-1) p->AddSystem();
        int ShowNames = Options.showNamesSwitch();
        if (l.isFirstSystem() && (Options.showNamesSwitch() > 1)) ShowNames = 1;
        p->Format(l.System, TitleHeight(l.Page), Score, XMLScore, Template, Options, StartBar, false, ShowAllStaves, ShowNames, Options.scale(PageRect().width()));
        if (Score->isEnded(Template)) break;
        p->PlotSystem(l.System, Score, XMLScore, Fonts, PageRect(), Options, ScreenObj);
        if (isBottomSystem(l)) break;
        l.System ++;
    }
}

void CLayout::Erase(const LayoutLocation &l, QGraphicsScene *Scene)
{
    if (l.Page<Pages.size()) Pages[l.Page]->Erase(l.System,Scene);
}

void CLayout::EraseTitle(QGraphicsScene *Scene)
{
    if (Pages.size()) Pages[0]->EraseTitle(Scene);
}

void CLayout::initPrinter()
{
    Printer->setPageMargins(QMarginsF(0,0,0,0));
    Printer->setPageOrientation(QPageLayout::Orientation(Options.orientation()));
    //Printer->setPaperSize(QPrinter::PaperSize(Options.paperSize()));
    Printer->setFullPage(true);
    //PrinterResFactor=DoubleDiv(Printer->resolution(),400);
    //PrinterResFactor = 1;
    m_PaperRect=QRect(QPoint(0,0),Printer->pageLayout().pageSize().sizePixels(720)); //Printer->resolution()
    //m_PaperRect.setSize(m_PaperRect.size()/PrinterResFactor);
    //m_PaperRect.setTopLeft(m_PaperRect.topLeft()/PrinterResFactor);
    //viewSize = m_PaperRect.height()/(920*6);
    //PrinterResFactor = 1;
    //double pf = 720.0 / Printer->resolution();
    Options.setSize(12);
    Options.setScaleSize(((Options.scoreType() * 0.75) + 0.75));
    if (isZero(Options.scaleSize())) Options.setScaleSize(1);
    //viewSize = 1;//6 / Options.scaleSize();
}

bool CLayout::ChangePageSetup(QWidget* Owner)
{
    QPageSetupDialog d(Printer,Owner);
    d.setWindowFlags(Qt::Sheet);
    d.setWindowModality(Qt::WindowModal);
    return (d.exec()==QDialog::Accepted);
}

bool CLayout::ChangePrinter(QWidget* Owner, QPrinter* Prn)
{
    QPrintDialog d(Prn, Owner);
    d.setWindowFlags(Qt::Sheet);
    d.setWindowModality(Qt::WindowModal);
    return (d.exec()==QDialog::Accepted);
}

void CLayout::PlotTitle(QGraphicsScene *Scene)
{
    OCDraw ScreenObj(Scene,Options.scaleSize());
    Pages.first()->PlotTitle(0, Fonts, PageRect(), Options, name(), ScreenObj);
}

CLayoutCollection::CLayoutCollection() : XMLLayoutCollectionWrapper(), m_ActiveLayout(0) {}

CLayoutCollection::~CLayoutCollection()
{
    qDeleteAll(Layouts);
    Layouts.clear();
}

CLayout *CLayoutCollection::activeLayout() const { return Layouts[m_ActiveLayout]; }

void CLayoutCollection::InitLayout(const int Index, XMLScoreWrapper &Score)
{
    Layouts[Index]->assignXML(Score.Layout(Index).xml());
    Layouts[Index]->setActiveLocation();
}

void CLayoutCollection::setActiveLayout(const QString &Name)
{
    for (int i=0;i<Layouts.size();i++)
    {
        if (Layouts[i]->name()==Name)
        {
            m_ActiveLayout=i;
            break;
        }
    }
}

void CLayoutCollection::setActiveLayout(const int index) { m_ActiveLayout=index; }

int CLayoutCollection::activeLayoutIndex() const { return m_ActiveLayout; }

CLayout *CLayoutCollection::layout(const int index) { return Layouts[index]; }

int CLayoutCollection::activePageIndex() const
{
    return Layouts[m_ActiveLayout]->activePageIndex();
}

int CLayoutCollection::activeSystemIndex() const
{
    return Layouts[m_ActiveLayout]->activeSystemIndex();
}

LayoutLocation CLayoutCollection::activeLayoutLocation() const {
    return LayoutLocation(activePageIndex(),activeSystemIndex());
}

void CLayoutCollection::assignXML(const XMLScoreWrapper &score)
{
    qDeleteAll(Layouts);
    Layouts.clear();
    shadowXML(score.LayoutCollection.xml());
    Score.assignXML(score);
    Score.createBarMap();
    for (int i = 0; i < layoutCount(); i++)
    {
        CLayout* l=new CLayout(XMLLayout(i).xml());
        l->Score = &Score;
        Layouts.append(l);
        l->setActiveLocation();
    }
}

bool CLayoutCollection::ChangePageSetup(QWidget *Owner)
{
    return activeLayout()->ChangePageSetup(Owner);
}

bool CLayoutCollection::ChangePrinter(QWidget *Owner)
{
    return activeLayout()->ChangePrinter(Owner,activeLayout()->Printer);
}

