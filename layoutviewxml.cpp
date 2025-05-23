#include "layoutviewxml.h"
#include "ui_layoutviewxml.h"
//#include <QMatrix>
//#include <QFileDialog>
//#include <QGLWidget>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QPrintPreviewDialog>
#else
#include <QtPrintSupport/QPrintPreviewDialog>
#endif
//#include <QPrintPreviewWidget>
//#include <QPen>

LayoutViewXML::LayoutViewXML(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::LayoutViewXML)
{
    ui->setupUi(this);
    HoverRubberband=new QHoverRubberband(QRubberBand::Rectangle,this);
    NoScrollFlag=false;
    setAutoFillBackground(true);
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
    this->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
    setMouseTracking(true);
    zoomer = new QGraphicsViewZoomer(this, defaultlayoutzoom);
    connect(zoomer,&QGraphicsViewZoomer::ZoomChanged,this,&LayoutViewXML::changeZoom);
    Scene = new QGraphicsScene(this);
    Scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(Scene);
    Scene->setBackgroundBrush(QPixmap(":/paper-texture.jpg"));
    setRenderHints(renderinghints);
    rb=new QMacRubberband(QRubberBand::Rectangle,QMacRubberband::MacRubberbandYellow,this);
    rb->hide();
    SceneNumOfPages=0;
    my=0;
    MD=false;
    MoveSystemY=0;
}

LayoutViewXML::~LayoutViewXML()
{
    delete ui;
}

void LayoutViewXML::leaveEvent(QEvent *event)
{
    if (HoverRubberband->isVisible()) HoverRubberband->hide();
    QGraphicsView::leaveEvent(event);
}

void LayoutViewXML::wheelEvent(QWheelEvent* event)
{
    if (HoverRubberband->isVisible()) HoverRubberband->hide();
    QGraphicsView::wheelEvent(event);
}

void LayoutViewXML::Unformat()
{
    activeLayout()->setIsFormated(false);
}

void LayoutViewXML::FormatAndDraw(const LayoutLocation& StartLocation, const bool Auto)
{
    this->setUpdatesEnabled(false);
    CLayout* l=activeLayout();
    while (SceneNumOfPages - 1 > StartLocation.Page)
    {
        for (int i = 0; i < systemCount(SceneNumOfPages - 1); i++) ClearSystem(LayoutLocation(SceneNumOfPages - 1, i));
        RemovePaper();
        if (Auto) l->RemovePage();
    }
    if (l->pageExists(StartLocation.Page))
    {
        for (int i = systemCount(StartLocation.Page) - 1; i >= StartLocation.System; i--)
        {
            ClearSystem(LayoutLocation(StartLocation.Page,i));
            if (Auto) l->RemoveSystem(StartLocation.Page);
        }
    }
    if (StartLocation.isTopSystem())
    {
        if (StartLocation.isFirstPage()) l->EraseTitle(Scene);
        RemovePaper();
        if (Auto) l->RemovePage();
    }
    l->AutoAll(StartLocation, XMLScore, Scene, Auto);
    while (pageCount() > SceneNumOfPages) AddPaper();
    setActiveLocation(StartLocation);
    viewport()->update();
    DrawActiveRect();
    setUpdatesEnabled(true);
}

void LayoutViewXML::MoveSystem(const int Direction)
{
    if (Sgn<int>(Direction)==1)
    {
        //Remove system
        if (systemCount() == 1) return;
        ClearSystem(bottomSystem());
        activeLayout()->MoveSystem(activePageIndex(), 1);
        setActiveLocation(LayoutLocation(activePageIndex()+1, 0));
        FormatAndDraw(activeLayoutLocation(), true);
    }
    else if (Sgn<int>(Direction)==-1)
    {
        //Add System
        if (activeLayout()->isLastPage(activePageIndex())) return;
        activeLayout()->MoveSystem(activePageIndex(), -1);
        setActiveLocation(bottomSystem());
        FormatAndDraw(activeLayoutLocation(), true);
    }
    emit SelectionChanged();
}

void LayoutViewXML::MoveBar(const int Direction)
{
    const LayoutLocation activeL(activeLayoutLocation());
    LayoutLocation l(activeL);
    int Action = activeLayout()->MoveBar(l, Direction);
    if (Action == -1)
    {
        setActiveLocation(l);
        ClearActiveSystem();
        PlotActiveSystem();
    }
    else if (Action == 1)
    {
        setActiveLocation(l);
        ClearActiveSystem();
        activeLayout()->RemoveSystem(activePageIndex());
        if (systemCount()==0)
        {
            activeLayout()->RemovePage();
            this->RemovePaper();
        }
    }
    setActiveLocation(activeL);
    ClearActiveSystem();
    PlotActiveSystem();
    emit SelectionChanged();
    DrawActiveRect();
}

void LayoutViewXML::SelectSystem(const LayoutLocation& l)
{
    setActiveLocation(l);
    DrawActiveRect();
}

void LayoutViewXML::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit DoubleClick();
}

void LayoutViewXML::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx,dy);
    if (NoScrollFlag) return;
    this->viewport()->update();
    DrawActiveRect();
}

void LayoutViewXML::mousePressEvent(QMouseEvent *event)
{
    if (layoutCount()==0) return;
    const QPointF m(mapToScene(event->pos()));
    my = m.y();
    MD = true;
    ML = InsideLocation(m);
    MoveSystemY = 0;
    if (ML.Page > -1)
    {
        setActiveLocation(ML);
        emit SelectionChanged();
        if (HoverRubberband->isVisible()) HoverRubberband->hide();
        DrawRect(activeLayoutLocation(), 0, true);
    }
}

void LayoutViewXML::mouseMoveEvent(QMouseEvent *event)
{
    if (layoutCount()==0) return;
    const QPointF m(mapToScene(event->pos()));
    if (MD)
    {
        if (ML.isValid())
        {
            const QRectF s(sysRect(ML));
            MoveSystemY = qBound<int>(-s.top(),m.y() - my,paperRect().height() - s.bottom());
            ClearActiveSystem();
            this->viewport()->update();
            XMLLayoutSystemWrapper XMLSystem=activeSystem();
            const double t = XMLSystem.top();
            XMLSystem.setTop(XMLSystem.top() + activeLayout()->Options.scale(MoveSystemY));
            PlotActiveSystem();
            XMLSystem.setTop(t);
            DrawRect(ML, MoveSystemY);
        }
    }
    else
    {
        const LayoutLocation l(InsideLocation(m));
        if (l.Page==-1)
        {
            if (HoverRubberband->isVisible()) HoverRubberband->hide();
        }
        else
        {
            if (l.System==-1)
            {
                if (HoverRubberband->isVisible()) HoverRubberband->hide();
            }
            else
            {
                if (activeLayoutLocation().matches(l))
                {
                    if (HoverRubberband->isVisible()) HoverRubberband->hide();
                }
                else
                {
                    HoverRubberband->setGeometry(mapFromScene(sysRect(l)));
                    if (!HoverRubberband->isVisible()) HoverRubberband->show(40);
                }
            }
        }
    }
}

void LayoutViewXML::mouseReleaseEvent(QMouseEvent *event)
{
    if (layoutCount()==0) return;
    if (HoverRubberband->isVisible()) HoverRubberband->hide();
    if (MD)
    {
        MD = false;
        if (ML.isValid())
        {
            DrawRect(ML, MoveSystemY);
            if (MoveSystemY != 0)
            {
                MakeBackup("Move System");
                ClearActiveSystem();
                this->viewport()->update();
                XMLLayoutSystemWrapper XMLSystem=activeSystem();
                XMLSystem.setTop(XMLSystem.top() + activeLayout()->Options.scale(MoveSystemY));
                PlotActiveSystem();
                DrawActiveRect();
                emit Changed();
            }
        }
        if (event->button()==Qt::RightButton)
        {
            if (ML.Page > -1)
            {
                if (ML.System > -1)
                {
                    emit PopupSystem(QCursor::pos());
                }
                else
                {
                    emit PopupPage(QCursor::pos());
                }
            }
            else
            {
                emit Popup(QCursor::pos());
            }
        }
    }
}

void LayoutViewXML::AddPaper()
{
    SceneNumOfPages++;
    Scene->setSceneRect(0,0,paperRect(SceneNumOfPages-1).right()+10,paperRect().height() + 10);
    DrawPaper(SceneNumOfPages-1);
}

void LayoutViewXML::RemovePaper()
{
    const int Page = SceneNumOfPages-1;
    if (Page < 0) return;
    if (PageList.contains(Page))
    {
        Scene->removeItem(PageList[Page].Paper);
        Scene->removeItem(PageList[Page].Shadow);
        delete PageList[Page].Paper;
        delete PageList[Page].Shadow;
        PageList.remove(Page);
    }
    SceneNumOfPages --;
    Scene->setSceneRect(0,0,paperRect(SceneNumOfPages-1).right() + 10,paperRect().height() + 10);
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
    const QRectF r(paperRect(Page));
    QPen p;  // creates a default pen
    p.setStyle(Qt::SolidLine);
    p.setWidth(4);
    p.setBrush(QColor(0,0,0,10));
    p.setCapStyle(Qt::SquareCap);
    p.setJoinStyle(Qt::RoundJoin);
    QGraphicsItem* shadow=Scene->addRect(r.adjusted(10,10,10,10),p,QBrush(QColor(0,0,0,80)));
    QGraphicsItem* paper=Scene->addRect(r,QPen(Qt::black),paperbrush);
    if (PageList.contains(Page))
    {
        Scene->removeItem(PageList[Page].Paper);
        Scene->removeItem(PageList[Page].Shadow);
        delete PageList[Page].Paper;
        delete PageList[Page].Shadow;
    }
    PageList[Page]=PageGraphics(paper,shadow);
}

void LayoutViewXML::Init()
{
    activeLayout()->initPrinter();
    if (activeLayout()->isFormated())
    {
        Clear();
        FormatAndDraw(LayoutLocation(0,0),false);
    }
    else
    {
        Clear();
        FormatAndDraw(LayoutLocation(0,0),true);
        activeLayout()->setIsFormated(true);
    }
}

void LayoutViewXML::Reformat(const LayoutLocation& StartLocation, const int Stretch)
{
    activeLayout()->initPrinter();
    for (int s=StartLocation.System;s<systemCount(StartLocation.Page);s++) ClearSystem(LayoutLocation(StartLocation.Page,s));
    for (int p=StartLocation.Page+1;p<pageCount();p++)
    {
        for (int s=0;s<systemCount(p);s++) ClearSystem(LayoutLocation(p,s));
    }
    while(SceneNumOfPages>StartLocation.Page) RemovePaper();
    const int HoldSpace=activeLayout()->Options.noteSpace();
    activeLayout()->Options.setNoteSpace(activeLayout()->Options.noteSpace()+Stretch);
    FormatAndDraw(StartLocation,true);
    activeLayout()->Options.setNoteSpace(HoldSpace);
    activeLayout()->setIsFormated(true);
}

void LayoutViewXML::ToggleNames()
{
    XMLLayoutSystemWrapper XMLSystem=activeSystem();
    XMLSystem.setShowNames((XMLSystem.showNames()+1) % 3);
    ClearActiveSystem();
    PlotActiveSystem();
}

void LayoutViewXML::ResetSystem()
{
    XMLLayoutSystemWrapper XMLSystem=activeSystem();
    XMLSystem.setTop(XMLSystem.defaultTop());
    ClearActiveSystem();
    PlotActiveSystem();
}

void LayoutViewXML::ClearSystem(const LayoutLocation& l)
{
    activeLayout()->Erase(l,Scene);
}

void LayoutViewXML::ClearActiveSystem()
{
    ClearSystem(activeLayoutLocation());
}

void LayoutViewXML::PlotActiveSystem()
{
    activeLayout()->Plot(activeLayoutLocation(), XMLScore, Scene);
    this->viewport()->update();
    DrawActiveRect();
}

void LayoutViewXML::AdjustCurrentPage()
{
    AdjustPage(activePageIndex());
    DrawActiveRect();
}

void LayoutViewXML::AdjustPages(const int StartPage)
{
    for (int i=StartPage;i<pageCount();i++) AdjustPage(i);
    DrawActiveRect();
}

void LayoutViewXML::AdjustPage(const int Page)
{
    DrawPaper(Page);
    activeLayout()->AdjustSystems(Page);
    if (Page == 0) activeLayout()->PlotTitle(Scene);
    for (int i = 0; i < systemCount(Page); i++)
    {
        const LayoutLocation l(Page,i);
        ClearSystem(l);
        activeLayout()->Plot(l, XMLScore, Scene);
    }
    this->viewport()->update();
}

void LayoutViewXML::DrawRect(const LayoutLocation& l, int y, bool Animate)
{
    if (layoutCount() && activeLayout()->pageExists(l.Page))
    {
        QRect tTempRect(mapFromScene(sysRect(l).translated(0,y)));
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

void LayoutViewXML::DrawActiveRect()
{
    if (lc.layoutExists(activeLayoutIndex())) {
        DrawRect(activeLayoutLocation(),0);
    }
}

void LayoutViewXML::PrintIt(const int StartPage, QGraphicsScene* Scene)
{
    activeLayout()->PrintIt(StartPage, XMLScore, Scene);
    setActiveLocation(activeLayoutLocation());
}

void LayoutViewXML::PrintPreview()
{
    QPrintPreviewDialog d(activeLayout()->Printer,this,Qt::Sheet);
    connect(&d,&QPrintPreviewDialog::paintRequested,this,&LayoutViewXML::PrintAll);
    d.exec();
}

bool LayoutViewXML::PrintAll(QPrinter *Printer)
{
    QPainter P;
    P.begin(Printer);
    P.setWorldMatrixEnabled(false);
    P.setViewTransformEnabled(false);
    P.setViewport(Printer->pageLayout().fullRectPixels(720)); //Printer->resolution()
    P.setWindow(Printer->pageLayout().fullRectPixels(720)); //Printer->resolution()
    P.setRenderHints(renderinghints,true);
    int fromPage=Printer->fromPage();
    if (fromPage>0) fromPage--;
    int toPage=Printer->toPage();
    if (toPage==0) toPage=pageCount();
    if (toPage>pageCount()) toPage=pageCount();
    if (activeLayout()->Options.frontPage()) Printer->newPage();
    for (int i=fromPage; i<toPage;i++)
    {
        QGraphicsScene S(this);
        S.setItemIndexMethod(QGraphicsScene::NoIndex);
        PrintIt(i,&S);
        S.setSceneRect(activeLayout()->PaperRect());
        S.render(&P);
        if (i < toPage-1) Printer->newPage();
    }
    P.end();
    return true;
}

const QPixmap LayoutViewXML::firstPageIcon(const int i)
{
    if (!lc.layout(i)->pageCount()) return QPixmap(":/layout.png");
    QPixmap pm(lc.layout(i)->PaperRect().size().toSize());
    pm.fill(Qt::white);
    QPainter P;
    P.begin(&pm);
    P.setWorldMatrixEnabled(false);
    P.setViewTransformEnabled(false);
    QGraphicsScene S(this);
    S.setItemIndexMethod(QGraphicsScene::NoIndex);
    lc.layout(i)->PrintIt(0, XMLScore, &S);
    S.setSceneRect(pm.rect());
    S.render(&P);
    P.end();
    return pm.scaled(128,128,Qt::KeepAspectRatio,Qt::SmoothTransformation);
}

void LayoutViewXML::PrinterPrint(const QString& pdfPath)
{
    QPrinter P(QPrinter::HighResolution);
    P.setPrinterName(activeLayout()->Printer->printerName());
    P.setFullPage(true);
    P.setPageOrientation(activeLayout()->Printer->pageLayout().orientation());
    P.setOutputFileName(activeLayout()->Printer->outputFileName());
    P.setOutputFormat(activeLayout()->Printer->outputFormat());
    P.setPageMargins(QMarginsF(0,0,0,0));
    P.setPageSize(activeLayout()->Printer->pageLayout().pageSize());
    if (!pdfPath.isEmpty())
    {
        P.setOutputFormat(QPrinter::PdfFormat);
        P.setOutputFileName(pdfPath);
        PrintAll(&P);
    }
    else
    {
        if (activeLayout()->ChangePrinter(this,&P))
        {
            activeLayout()->Printer->setPrinterName(P.printerName());
            PrintAll(&P);
        }
    }
}

void LayoutViewXML::PageSetup()
{
    activeLayout()->ChangePageSetup(this);
}

void LayoutViewXML::SetXML(XMLScoreWrapper& Score)
{
    XMLScore.shadowXML(Score);
    ReloadXML();
}

void LayoutViewXML::ReloadXML()
{
    lc.assignXML(XMLScore);
    if (layoutCount()==0) Clear();
}
