#include "layoutviewxml.h"
#include "ui_layoutviewxml.h"
#include <QMatrix>
//#include <QGLWidget>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QPrintPreviewDialog>
#else
#include <QtPrintSupport/QPrintPreviewDialog>
#endif
//#include <QPrintPreviewWidget>
#include <QPen>

LayoutViewXML::LayoutViewXML(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::LayoutViewXML)
{
    ui->setupUi(this);
    HoverRubberband=new QHoverRubberband(QRubberBand::Rectangle,this);
    NoScrollFlag=false;
    setAutoFillBackground(true);
    IsInitialized=false;
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
    //this->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    //this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
    setMouseTracking(true);
    Scene = new QGraphicsScene(this);
    Scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(Scene);
    setRenderHints(renderinghints);
    rb=new QMacRubberband(QRubberBand::Rectangle,QMacRubberband::MacRubberbandYellow,this);
    rb->hide();
    SceneNumOfPages=0;
    my=0;
    MD=false;
    MP=0;
    MS=0;
    MoveSystemY=0;
    ScaleFactor=1.0;
    SetZoom(50);
}

LayoutViewXML::~LayoutViewXML()
{
    delete Scene;
    delete rb;
    delete ui;
}

void LayoutViewXML::leaveEvent(QEvent *event)
{
    if (HoverRubberband->isVisible())
    {
        HoverRubberband->hide();
    }
    QGraphicsView::leaveEvent(event);
}

void LayoutViewXML::wheelEvent(QWheelEvent* event)
{
    if (HoverRubberband->isVisible())
    {
        HoverRubberband->hide();
    }
    QGraphicsView::wheelEvent(event);
}

void LayoutViewXML::Unformat()
{
    Layout = lc.CurrentLayout;
    Layout->IsFormated=false;
}

void LayoutViewXML::FormatAndDraw(const int StartPage, const int StartSystem, const bool Auto)
{
    //Dim iTemp As Integer
    //Dim iTemp1 As Integer
    //int InitNumOfPages=Layout->NumOfPages();
    //this->setCursor(Qt::WaitCursor);
    this->setUpdatesEnabled(false);
    /*
    for (int iTemp = StartPage; iTemp<Layout->NumOfPages(); iTemp++)
    {
        RemovePage();
        if (Auto) Layout->RemovePage();
    }
    */
    while (SceneNumOfPages-1>StartPage)
    {
        for (int i=0;i<Layout->NumOfSystems(SceneNumOfPages-1);i++)
        {
            ClearSystem(SceneNumOfPages-1,i);
        }
        RemovePage();
    }
    if (Auto)
    {
        while (Layout->NumOfPages()-1>StartPage)
        {
            Layout->RemovePage();
        }
    }
    if (StartPage <= Layout->NumOfPages()-1)
    {
        for (int iTemp = Layout->NumOfSystems(StartPage)-1; iTemp > StartSystem; iTemp--)
        {
            //ClearSystem(StartPage, iTemp);// ????
            ClearSystem(StartPage,iTemp);
            if (Auto) Layout->RemoveSystem(StartPage);
        }
        ClearSystem(StartPage,StartSystem);
    }
    if (StartSystem == 0)
    {
        if (StartPage==0) Layout->EraseTitle(Scene);
        RemovePage();
        //DrawPaper(StartPage);
    }
    //DrawPaper(StartPage);
    Layout->AutoAll(StartPage, StartSystem, XMLScore, Scene, Auto ,this);
    //if (InitNumOfPages<=0)
        Layout->setActiveObjects(StartPage,StartSystem);
    this->viewport()->update();
    //Layout->setActiveObjects(ActivePage(), ActiveSystem());
    DrawRect(ActivePage(),ActiveSystem(),0);
    //this->setCursor(Qt::ArrowCursor);
    this->setUpdatesEnabled(true);
}

void LayoutViewXML::MoveSystem(const int Direction)
{
    switch (Sgn(Direction))
    {
    case 1:
        //Remove system
        if (Layout->NumOfSystems(ActivePage()) == 1) return;
        ClearSystem(ActivePage(), Layout->NumOfSystems(ActivePage())-1);
        Layout->MoveSystem(ActivePage(), 1);
        Layout->setActiveObjects(ActivePage()+1, 0);
        FormatAndDraw(ActivePage(), ActiveSystem(), true);
        break;
    case -1:
        //Add System
        if (ActivePage() == Layout->NumOfPages()-1) return;
        Layout->MoveSystem(ActivePage(), -1);
        Layout->setActiveObjects(ActivePage(),Layout->NumOfSystems(ActivePage())-1);
        FormatAndDraw(ActivePage(), ActiveSystem(), true);
    }
    emit SelectionChanged();
}

void LayoutViewXML::MoveBar(const int Direction)
{
    int Pa = ActivePage();
    int Sy = ActiveSystem();
    int Pa1=Pa;
    int Sy1=Sy;
    int Action = Layout->MoveBar(Pa1, Sy1, Direction);
    if (Action == -1)
    {
        Layout->setActiveObjects(Pa1, Sy1);
        ClearSystem(ActivePage(), ActiveSystem());
        PlotActiveSystem();
        Layout->setActiveObjects(Pa, Sy);
        ClearSystem(ActivePage(), ActiveSystem());
        PlotActiveSystem();
    }
    else if (Action == 1)
    {
        Layout->setActiveObjects(Pa1, Sy1);
        ClearSystem(ActivePage(), ActiveSystem());
        Layout->RemoveSystem(ActivePage());
        if (Layout->NumOfSystems(ActivePage())==0)
        {
            Layout->RemovePage();
            this->RemovePage();
        }
        Layout->setActiveObjects(Pa, Sy);
        ClearSystem(ActivePage(), ActiveSystem());
        PlotActiveSystem();
    }
    emit SelectionChanged();
    DrawRect(ActivePage(),ActiveSystem(),0);
}

void LayoutViewXML::SetActiveObjects(const int Page, const int System)
{
    lc.CurrentLayout->setActiveObjects(Page,System);
    DrawRect(ActivePage(),ActiveSystem(),0);
}

void LayoutViewXML::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //int Page = MP;
    //int System = MS;
    //RaiseEvent DblClick(Page, System)
    emit DoubleClick();
}

void LayoutViewXML::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx,dy);
    if (NoScrollFlag) return;
    this->viewport()->update();
    DrawRect(ActivePage(),ActiveSystem(),0);
    //rb->setGeometry(rb->pos().x()+dx,rb->pos().y()+dy,rb->width(),rb->height());
}

void LayoutViewXML::mousePressEvent(QMouseEvent *event)
{
    if (NumOfLayouts()==0) return;
    QPointF m(mapToScene(event->pos()));
    my = m.y();
    MD = true;
    int Page = IntDiv(m.x() + 5, IntDiv(Layout->PaperWidth(), Layout->Viewsize));
    if (Page < 0) Page = 0;
    if (Page > Layout->NumOfPages()-1) Page = Layout->NumOfPages()-1;
    int System = InsideSystem(m.y(), Page);
    MS = System;
    MP = Page;
    MoveSystemY = 0;
    //ActivePage = MP;
    //ActiveSystem = MS;
    Layout->setActiveObjects(MP, MS);
    emit SelectionChanged();
    if (HoverRubberband->isVisible()) HoverRubberband->hide();
    DrawRect(MP, MS, 0, true);
    //RaiseEvent MouseDown(Button, Shift, x, y, Page, System)
}

void LayoutViewXML::mouseMoveEvent(QMouseEvent *event)
{
    if (NumOfLayouts()==0) return;
    QPointF m(mapToScene(event->pos()));
    if (MD)
    {
        //DrawRect(MP, MS, MoveSystemY);
        MoveSystemY = m.y() - my;
        if (MoveSystemY + SysTop(MP, MS) < 0) MoveSystemY = -SysTop(MP, MS);
        if (MoveSystemY + SysBottom(MP, MS) > (Layout->PageHeight() / Layout->Viewsize)) MoveSystemY = ((Layout->PageHeight() / Layout->Viewsize) - SysBottom(MP, MS));
        //RaiseEvent MouseMove(Button, Shift, x, y)
        DrawRect(MP, MS, MoveSystemY);
    }
    else
    {
        int Page = IntDiv(m.x() + 5, IntDiv(Layout->PaperWidth(), Layout->Viewsize));
        //if (Page < 0) Page = 0;
        if (Page > Layout->NumOfPages()-1) Page = -1;
        if (Page==-1)
        {
            if (HoverRubberband->isVisible()) HoverRubberband->hide();
        }
        else
        {
            int System=-1;
            for (int iTemp = 0; iTemp<Layout->NumOfSystems(Page); iTemp++)
            {
                if ((m.y() > SysTop(Page, iTemp)) && (m.y()<SysBottom(Page,iTemp))) System = iTemp;
            }
            if (System==-1)
            {
                if (HoverRubberband->isVisible()) HoverRubberband->hide();
            }
            else
            {
                if ((Page==ActivePage() && System==ActiveSystem()))
                {
                    if (HoverRubberband->isVisible()) HoverRubberband->hide();
                }
                else
                {
                    QRect tTempRect(mapFromScene(QPoint(SysLeft(Page),SysTop(Page,System))),mapFromScene(QPoint(SysRight(Page),SysBottom(Page,System))));
                    HoverRubberband->setGeometry(tTempRect);
                    if (!HoverRubberband->isVisible()) HoverRubberband->show(40);
                }
            }
        }
    }
}

void LayoutViewXML::mouseReleaseEvent(QMouseEvent *event)
{
    if (NumOfLayouts()==0) return;
    if (HoverRubberband->isVisible()) HoverRubberband->hide();
    QPointF m(mapToScene(event->pos()));
    if (MD)
    {
        MD = false;
        DrawRect(MP, MS, MoveSystemY);
        //RaiseEvent MouseUp(Button, Shift, x, y)
        if (MoveSystemY != 0)
        {
            MakeBackup("Move System");
            ClearSystem(ActivePage(), ActiveSystem());
            this->viewport()->update();
            Layout->ActivePage->ActiveSystem->Top = Layout->ActivePage->ActiveSystem->Top + (MoveSystemY * Layout->Options.ScaleSize * Layout->Viewsize);
            PlotActiveSystem();
            DrawRect(ActivePage(),ActiveSystem(),0);
            WriteXML();
            emit Changed();
        }
        if (event->button()==Qt::RightButton) emit Popup(this->cursor().pos());
    }
}

void LayoutViewXML::AddPage()
{
    int Page = SceneNumOfPages;
    int X2 = PageLeft(Page) + IntDiv(Layout->PaperWidth(), Layout->Viewsize);
    int SceneWidth=Scene->width()/ScaleFactor;
    if (SceneWidth < X2) Scene->setSceneRect(0,0,(X2+10)*ScaleFactor,(IntDiv(Layout->PaperHeight(), Layout->Viewsize) + 10) * ScaleFactor);
    DrawPaper(Page);
    SceneNumOfPages++;
}

void LayoutViewXML::RemovePage()
{
    int Page = SceneNumOfPages-1;
    if (Page < 0) return;
    if (PageList.contains(Page))
    {
        Scene->removeItem(PageList[Page].first);
        Scene->removeItem(PageList[Page].second);
        delete PageList[Page].first;
        delete PageList[Page].second;
        PageList.remove(Page);
    }
    Scene->setSceneRect(0,0,(PageLeft(Page) + 20) * ScaleFactor,(IntDiv(Layout->PaperHeight(), Layout->Viewsize) + 10) * ScaleFactor);
    SceneNumOfPages --;
}

void LayoutViewXML::Clear()
{
    NoScrollFlag=true;
    Scene->setSceneRect(0,0,1,1);
    Scene->clear();
    PageList.clear();
    SceneNumOfPages=0;
    this->viewport()->update();
    rb->hide();
    NoScrollFlag=false;
}

void LayoutViewXML::DrawPaper(const int Page)
{
    int Y1=0;
    int X1 = PageLeft(Page);
    int X2 = PageRight(Page);
    int Y2 = Y1+IntDiv(Layout->PaperHeight(), Layout->Viewsize);
    QPen p;  // creates a default pen
    p.setStyle(Qt::SolidLine);
    p.setWidth(4);
    p.setBrush(QColor(0,0,0,10));
    p.setCapStyle(Qt::SquareCap);
    p.setJoinStyle(Qt::RoundJoin);
    QGraphicsItem* shadow=Scene->addRect(X1+10,Y1+10,X2-X1,Y2-Y1,p,QBrush(QColor(0,0,0,80)));
    QGraphicsItem* paper=Scene->addRect(X1,Y1,X2-X1,Y2-Y1,QPen(Qt::black),QBrush(Qt::white));
    paper->setZValue(1);
    if (PageList.contains(Page))
    {
        Scene->removeItem(PageList[Page].first);
        Scene->removeItem(PageList[Page].second);
        delete PageList[Page].first;
        delete PageList[Page].second;
    }
    PageList[Page]=qMakePair(paper,shadow);
}

void LayoutViewXML::SetXMLScore(XMLScoreWrapper& Doc)
{
    XMLScore.shadowXML(Doc);
}

void LayoutViewXML::Init()
{
    IsInitialized=true;
    Layout = lc.CurrentLayout;
    Layout->Score.MakeStaves(XMLScore);
    //Layout->Template.copy(XMLLayoutTemplate(XMLScore.getXML(),CurrentLayout()));
    Layout->Template.copy(XMLScore.LayoutTemplate(CurrentLayout()));
    //ActiveSystem = 0;
    //ActivePage = 0;
    if (Layout->IsFormated)
    {
        Layout->SetPrinter();
        Clear();
        FormatAndDraw(0,0,false);
    }
    else
    {
        Layout->GetPrinter();
        Clear();
        FormatAndDraw(0,0,true);
        Layout->IsFormated = true;
    }
}

void LayoutViewXML::Reformat(const int StartPage, const int StartSystem, const int Stretch)
{
    IsInitialized=true;
    Layout = lc.CurrentLayout;
    Layout->Score.MakeStaves(XMLScore);
    Layout->Template.copy(XMLScore.LayoutTemplate(CurrentLayout()));
    //ActiveSystem = 0;
    //ActivePage = 0;
    Layout->GetPrinter();
    for (int s=StartSystem;s<Layout->NumOfSystems(StartPage);s++) ClearSystem(StartPage,s);
    for (int p=StartPage+1;p<Layout->NumOfPages();p++)
    {
        for (int s=0;s<Layout->NumOfSystems(p);s++) ClearSystem(p,s);
    }
    while(SceneNumOfPages>StartPage) RemovePage();
    int HoldSpace=Layout->Options.NoteSpace;
    Layout->Options.NoteSpace+=Stretch;
    FormatAndDraw(StartPage,StartSystem,true);
    Layout->Options.NoteSpace=HoldSpace;
    Layout->IsFormated = true;
}

void LayoutViewXML::ToggleNames()
{
    Layout->ActivePage->ActiveSystem->ShowNames++;
    Layout->ActivePage->ActiveSystem->ShowNames %= 3;
    ClearSystem(ActivePage(), ActiveSystem());
    PlotActiveSystem();
}

void LayoutViewXML::ResetSystem()
{
    lc.CurrentLayout->ActivePage->ActiveSystem->Top=lc.CurrentLayout->ActivePage->ActiveSystem->DefaultTop;
    ClearSystem(ActivePage(), ActiveSystem());
    PlotActiveSystem();
}

void LayoutViewXML::ClearSystem(const int Page, const int System)
{
    Layout->Erase(Page,System,Scene);
}

void LayoutViewXML::PlotActiveSystem()
{
    Layout->Plot(ActivePage(), ActiveSystem(), XMLScore, Scene, PageLeft(ActivePage()));
    this->viewport()->update();
    DrawRect(ActivePage(),ActiveSystem(),0);
}

const int LayoutViewXML::SysLeft(const int Page)
{
    return PageLeft(Page)+IntDiv((Layout->MarginLeft()/Layout->Options.ScaleSize)-100,Layout->Viewsize);
}

const int LayoutViewXML::SysRight(const int Page)
{
    return PageRight(Page)-IntDiv((Layout->MarginRight()/Layout->Options.ScaleSize)-100,Layout->Viewsize);
}

const int LayoutViewXML::SysTop(const int Page, const int System)
{
    return IntDiv((Layout->SysTop(Page, System) + 120), Layout->Viewsize);
}

const int LayoutViewXML::SysBottom(const int Page, const int System)
{
    return SysTop(Page, System) + IntDiv(Layout->SysHeight(Page, System), Layout->Viewsize);
}

const int LayoutViewXML::InsideSystem(const float y, const int Page)
{
    for (int iTemp = 0; iTemp<Layout->NumOfSystems(Page); iTemp++)
    {
        if ((y > SysTop(Page, iTemp)) && (y<SysBottom(Page,iTemp))) return iTemp;
    }
    return 0;
}

const int LayoutViewXML::PageLeft(const int Page)
{
    return Page * IntDiv(Layout->PaperWidth(), Layout->Viewsize);
}

const int LayoutViewXML::PageRight(const int Page)
{
    return PageLeft(Page) + IntDiv(Layout->PaperWidth(), Layout->Viewsize);
}

void LayoutViewXML::AdjustCurrentPage()
{
    AdjustPage(ActivePage());
    DrawRect(ActivePage(),ActiveSystem(),0);
}

void LayoutViewXML::AdjustPages(const int StartPage)
{
    for (int i=StartPage;i<Layout->NumOfPages();i++)
    {
        AdjustPage(i);
    }
    DrawRect(ActivePage(),ActiveSystem(),0);
}

void LayoutViewXML::AdjustPage(const int Page)
{
    DrawPaper(Page);
    Layout->AdjustSystems(Page);
    if (Page == 0) Layout->PlotTitle(Scene);
    for (int iTemp = 0; iTemp<Layout->NumOfSystems(Page); iTemp++)
    {
        //Layout->Erase(ActivePage,iTemp,Scene);
        ClearSystem(Page,iTemp);
        Layout->Plot(Page, iTemp, XMLScore, Scene, PageLeft(Page));
    }
    this->viewport()->update();
}

const int LayoutViewXML::CurrentLayout() const
{
    return lc.getCurrentLayout();
}

const int LayoutViewXML::ActivePage() const
{
    return lc.getActivePage();
}

const int LayoutViewXML::ActiveSystem() const
{
    return lc.getActiveSystem();
}


void LayoutViewXML::SetCurrentLayout(const int Index)
{
    lc.SetCurrentLayout(Index);
    //CurrentLayoutIndex=Index;
    Layout=lc.CurrentLayout;
    if (Layout->NumOfPages())
    {
        //if (ActivePage()>=Layout->NumOfPages()) ActivePage=Layout->NumOfPages()-1;
        //if (ActiveSystem()>=Layout->NumOfSystems(ActivePage)) ActiveSystem=Layout->NumOfSystems(ActivePage)-1;
        Layout->setActiveObjects(ActivePage(),ActiveSystem());
    }
    emit SelectionChanged();
}
/*
void LayoutViewXML::SetLayoutCollectionCurrentLayout()
{
    lc.CurrentLayout = Layout;
}
*/
void LayoutViewXML::AddTitle()
{
    DrawPaper(0);
    Layout->PlotTitle(Scene);
    Layout->FormatTitle();
    for (int iTemp = 0; iTemp<Layout->NumOfSystems(0); iTemp++)
    {
        Layout->Plot(0, iTemp, XMLScore, Scene, PageLeft(1));
    }
    DrawRect(ActivePage(),ActiveSystem(),0);
    this->viewport()->update();
}

void LayoutViewXML::DrawRect(int Page, int System, int y, bool Animate)
{
    if (lc.NumOfLayouts())
    {
        QRect tTempRect(mapFromScene(QPoint(SysLeft(Page),SysTop(Page,System)+y)),mapFromScene(QPoint(SysRight(Page),SysBottom(Page,System)+y)));
        //QRect tTempRect((SysLeft(Page)+1-this->horizontalScrollBar()->value())*zoomfactor,(SysTop(Page,System)+1+y-this->verticalScrollBar()->value())*zoomfactor,(SysRight(Page)-(SysLeft(Page)+1))*zoomfactor,((SysBottom(Page,System)+y)-(SysTop(Page,System)+1+y))*zoomfactor);
        if (rb->geometry() != tTempRect)
        {
            rb->setGeometry(tTempRect);
            if (Animate)
            {
                rb->showAnimated();
            }
            else
            {
                if (!rb->isVisible()) rb->show();
            }
        }
        else
        {
            if (!rb->isVisible()) rb->show();
        }
    }
    else
    {
        rb->hide();
    }
}

void LayoutViewXML::EditSystem(QWidget* parent)
{
    QDomLiteElement* LayoutTemplate = &lc.CurrentLayout->Template;
    QDomLiteElement* SystemTemplate = &lc.CurrentLayout->ActivePage->ActiveSystem->Template;
    CEditSystem* d=new CEditSystem(parent);
    d->Fill(LayoutTemplate,SystemTemplate);
    if (d->exec()==QDialog::Accepted)
    {
        d->GetTemplate(LayoutTemplate,SystemTemplate);
        ClearSystem(ActivePage(),ActiveSystem());
        Layout->Plot(ActivePage(), ActiveSystem(), XMLScore, Scene, PageLeft(ActivePage()));
        DrawRect(ActivePage(),ActiveSystem(),0);
        this->viewport()->update();
    }
    delete d;
}

void LayoutViewXML::PrintIt(const int StartPage, QGraphicsScene* Scene)
{
    Layout->PrintIt(StartPage, XMLScore, Scene);
    Layout->setActiveObjects(ActivePage(), ActiveSystem());
}

void LayoutViewXML::PrintPreview()
{
    QPrintPreviewDialog* d=new QPrintPreviewDialog(Layout->Printer,this,Qt::Sheet);
    //QPrintPreviewWidget* d=new QPrintPreviewWidget(Layout->Printer,this);
    connect(d,SIGNAL(paintRequested(QPrinter*)),this,SLOT(PrintAll(QPrinter*)));
    //d->setGeometry(this->geometry());
    //d->setWindowModality(Qt::WindowModal);
    d->setWindowState(d->windowState() | Qt::WindowMaximized);
    d->exec();
    delete d;
    //d->show();
}

const bool LayoutViewXML::PrintAll(QPrinter *Printer)
{
    //this->setCursor(Qt::WaitCursor);
    QPainter P;
    P.setRenderHint(QPainter::SmoothPixmapTransform);
    P.setWorldMatrixEnabled(false);
    P.setViewTransformEnabled(false);
    P.begin(Printer);
    P.setViewport(Printer->paperRect());
    P.setWindow(Printer->paperRect());
    P.setRenderHints(renderinghints,true);
    //P.setRenderHint(QPainter::Antialiasing,true);
    //P.setRenderHint(QPainter::TextAntialiasing,true);
    //P.setRenderHint(QPainter::HighQualityAntialiasing,true);
    //P.setRenderHint(QPainter::SmoothPixmapTransform,true);
    int fromPage=Printer->fromPage();
    if (fromPage>0) fromPage--;
    int toPage=Printer->toPage();
    if (toPage==0) toPage=Layout->NumOfPages();
    if (toPage>Layout->NumOfPages()) toPage=Layout->NumOfPages();
    for (int i=fromPage; i<toPage;i++)
    {
        QGraphicsScene* S=new QGraphicsScene(this);
        S->setItemIndexMethod(QGraphicsScene::NoIndex);
        //S->addRect(Layout->PageRect(),QPen(Qt::black),QBrush(Qt::transparent));
        PrintIt(i,S);
        S->setSceneRect(Layout->PaperRect());
        S->render(&P);
        if (i < toPage-1) Printer->newPage();
        delete S;
    }
    P.end();
    //this->setCursor(Qt::ArrowCursor);
    return true;
}

void LayoutViewXML::PrinterPrint()
{
    QPrinter* P=new QPrinter(QPrinter::HighResolution);
    P->setPrinterName(Layout->Printer->printerName());
    P->setFullPage(true);
    P->setOrientation(Layout->Printer->orientation());
    P->setOutputFileName(Layout->Printer->outputFileName());
    P->setOutputFormat(Layout->Printer->outputFormat());
    P->setPageMargins(0,0,0,0,QPrinter::DevicePixel);
    P->setPaperSize(Layout->Printer->paperSize());
    if (Layout->ChangePrinter(this,P))
    {
        Layout->Printer->setPrinterName(P->printerName());
        PrintAll(P);
    }
    delete P;
}

void LayoutViewXML::PageSetup()
{
    Layout->ChangePageSetup(this);
}

void LayoutViewXML::Load(XMLScoreWrapper& Score)
{
    //m_XMLScore=Score;
    XMLScore.shadowXML(Score); //???
    ReadXML();
}

void LayoutViewXML::ReadXML()
{
    lc.Clear();
    if (XMLScore.NumOfLayouts())
    {
        lc.Load(XMLScore.LayoutCollection(),XMLScore);
    }
    else
    {
        Clear();
    }
}

void LayoutViewXML::WriteXML()
{
    //XMLScore.getXML()->documentElement->replaceChild(lc.Save(),XMLScore.LayoutCollection());
    XMLScore.replaceLayoutCollection(lc.Save());
}

const QString LayoutViewXML::LayoutName(const int Index) const
{
    return lc.Name(Index);
}

const int LayoutViewXML::NumOfLayouts() const
{
    //return lc.NumOfLayouts();
    return lc.NumOfLayouts();
}

void LayoutViewXML::AddLayout(const QString& Name)
{
    lc.AddLayout(Name);
}

void LayoutViewXML::InitLayout(const int Index)
{
    lc.InitLayout(Index,XMLScore);
}

void LayoutViewXML::RemoveLayout(const int Index)
{
    lc.RemoveLayout(Index);
    XMLScore.LayoutCollection()->removeChild(Index);
    if (lc.NumOfLayouts()==0)
    {
        Clear();
    }
}

void LayoutViewXML::MakeBackup(const QString& Text)
{
    emit BackMeUp(Text);
}

void LayoutViewXML::SetZoom(const int Zoom)
{
    zoomfactor=(float)Zoom*0.01;
    QMatrix matrix = this->matrix();
    matrix.reset();
    matrix.scale(zoomfactor,zoomfactor);
    this->setMatrix(matrix);
    if (NumOfLayouts()) DrawRect(ActivePage(),ActiveSystem(),0);
    if (HoverRubberband->isVisible()) HoverRubberband->hide();
}

const int LayoutViewXML::GetZoom() const
{
    return zoomfactor*100.0;
}
