#include "layoutviewxml.h"
#include "ui_layoutviewxml.h"
//#include "qgraphicsitemlist.h"
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
    NoScrollFlag=false;
    setAutoFillBackground(true);
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
    this->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setMouseTracking(true);
    zoomer = new QGraphicsViewZoomer(this, defaultlayoutzoom);
    connect(zoomer,&QGraphicsViewZoomer::ZoomChanged,this,&LayoutViewXML::changeZoom);
    Scene = new QGraphicsScene();
    Scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(Scene);
    Scene->setBackgroundBrush(QDPRPixmap(":/paper-texture.jpg"));
    setRenderHints(renderinghints);
    //rb=new QMacRubberband(QRubberBand::Rectangle,QMacRubberband::MacRubberbandYellow,this);
    //rb->hide();
    rubberBand = new QGraphicsRubberBand(this);
    rubberBand->setPenWidth(4);
    SceneNumOfPages=0;
    my=0;
    MD=false;
    MoveSystemY=0;
    hoverRect = new QGraphicsRubberBand(this);
    hoverRect->setPenWidth(4);

    PageButtonsItem = new QGraphicsContainerItem(Scene);
    addSystemButton = new QGraphicsToolButton(QPixmap(":/mini/mini/add.png").scaled(240,240),PageButtonsItem);
    PageButtonsItem->append(addSystemButton);
    addSystemButton->setPos(0,240);
    connect(addSystemButton,&QGraphicsToolButton::clicked,this,&LayoutViewXML::GetSystemFromNextPage);
    //Scene->addItem(addSystemButton);
    removeSystemButton = new QGraphicsToolButton(QPixmap(":/mini/mini/minus.png").scaled(240,240),PageButtonsItem);
    connect(removeSystemButton,&QGraphicsToolButton::clicked,this,&LayoutViewXML::MoveSystemToNextPage);
    //Scene->addItem(removeSystemButton);
    PageButtonsItem->append(removeSystemButton);

    SystemButtonsItem = new QGraphicsContainerItem(Scene);
    addBarButton = new QGraphicsToolButton(QPixmap(":/mini/mini/add.png").scaled(240,240),SystemButtonsItem);
    SystemButtonsItem->append(addBarButton);
    connect(addBarButton,&QGraphicsToolButton::clicked,this,&LayoutViewXML::BarFromNextSystem);
    //Scene->addItem(addBarButton);
    removeBarButton = new QGraphicsToolButton(QPixmap(":/mini/mini/minus.png").scaled(240,240),SystemButtonsItem);
    SystemButtonsItem->append(removeBarButton);
    connect(removeBarButton,&QGraphicsToolButton::clicked,this,&LayoutViewXML::BarToNextSystem);
    //Scene->addItem(removeBarButton);
    toggleNamesButton = new QGraphicsToolButton(QPixmap(":/mini/mini/system.png").scaled(240,240),SystemButtonsItem);
    SystemButtonsItem->append(toggleNamesButton);
    connect(toggleNamesButton,&QGraphicsToolButton::clicked,this,&LayoutViewXML::ToggleNames);
    //Scene->addItem(toggleNamesButton);
}

LayoutViewXML::~LayoutViewXML()
{
    delete ui;
}

void LayoutViewXML::leaveEvent(QEvent *event)
{
    //if (HoverRubberband->isVisible()) HoverRubberband->hide();
    hoverRect->hide();
    QGraphicsView::leaveEvent(event);
}

int LayoutViewXML::pageCount() const { return activeLayout()->pageCount(); }

int LayoutViewXML::systemCount() const { return systemCount(activePageIndex()); }

int LayoutViewXML::systemCount(const int Page) const { return activeLayout()->systemCount(Page); }

CLayout *LayoutViewXML::activeLayout() const { return lc.activeLayout(); }

XMLLayoutSystemWrapper LayoutViewXML::activeSystem() const { return *activeLayout()->activeSystem(); }

void LayoutViewXML::setActiveLocation(const LayoutLocation &l) { activeLayout()->setActiveLocation(l); }

void LayoutViewXML::wheelEvent(QWheelEvent* event)
{
    //if (HoverRubberband->isVisible()) HoverRubberband->hide();
    //hoverRect->hide();
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
    //viewport()->update();
    DrawActiveRect();
    setUpdatesEnabled(true);
}

void LayoutViewXML::MoveSystem(const int Direction)
{
    if (Sgn<int>(Direction) == 1)
    {
        //Remove system
        if (systemCount() == 1) return;
        ClearSystem(bottomSystem());
        activeLayout()->MoveSystem(activePageIndex(), 1);
        setActiveLocation(LayoutLocation(activePageIndex() + 1, 0));
        FormatAndDraw(activeLayoutLocation(), true);
    }
    else if (Sgn<int>(Direction) == -1)
    {
        //Add System
        if (activeLayout()->isLastPage(activePageIndex())) return;
        activeLayout()->MoveSystem(activePageIndex(), -1);
        setActiveLocation(bottomSystem());
        FormatAndDraw(activeLayoutLocation(), true);
    }
    placeButtons(activeLayoutLocation());
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
    placeButtons(activeL);
    emit SelectionChanged();
    DrawActiveRect();
}

void LayoutViewXML::BarToNextSystem() { MoveBar(1); }

void LayoutViewXML::BarFromNextSystem() { MoveBar(-1); }

void LayoutViewXML::SelectSystem(const LayoutLocation& l)
{
    setActiveLocation(l);
    DrawActiveRect();
}

int LayoutViewXML::pageOfBar(const int bar) const {
    return activeLayout()->PageOfBar(bar);
}

int LayoutViewXML::systemOfBar(const int page, const int bar) const {
    return activeLayout()->SystemOfBar(page, bar);
}

LayoutLocation LayoutViewXML::locationOfBar(const int bar) const {
    const int p = pageOfBar(bar);
    return LayoutLocation(p,systemOfBar(p,bar));
}

bool LayoutViewXML::pageContainsBar(const int page, const int bar) const {
    return (activeLayout()->PageOfBar(bar) == page);
}

bool LayoutViewXML::systemContainsBar(const int page, const int system, const int bar) const {
    return (activeLayout()->SystemOfBar(page, bar) == system);
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
    //this->viewport()->update();
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
        //if (HoverRubberband->isVisible()) HoverRubberband->hide();
        hoverRect->hide();
        DrawRect(activeLayoutLocation(), 0, true);
    }
    QGraphicsView::mousePressEvent(event);
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
            MoveActiveSystemTop(MoveSystemY);
            SystemButtonsItem->setPos(s.left(),s.top() + MoveSystemY);
            //this->viewport()->update();
            /*
            ClearActiveSystem();
            this->viewport()->update();
            XMLLayoutSystemWrapper XMLSystem = activeSystem();
            const double t = XMLSystem.top();
            XMLSystem.setTop(XMLSystem.top() + activeLayout()->Options.scale(MoveSystemY));
            PlotActiveSystem();
            XMLSystem.setTop(t);
*/
            DrawRect(ML, MoveSystemY);
        }
    }
    else
    {
        const LayoutLocation l(InsideLocation(m));
        if (l.Page==-1)
        {
            //if (HoverRubberband->isVisible()) HoverRubberband->hide();
            hoverRect->hide();
        }
        else
        {
            if (l.System==-1)
            {
                //if (HoverRubberband->isVisible()) HoverRubberband->hide();
                hoverRect->hide();
            }
            else
            {
                if (!l.matches(hoverLocation)) {
                    hoverLocation = l;
                    placeButtons(l);
                }
                if (activeLayoutLocation().matches(l))
                {
                    //if (HoverRubberband->isVisible()) HoverRubberband->hide();
                    if (hoverRect->isVisible()) {
                        hoverRect->hide();
                    }
                }
                else
                {
                    /*
                    HoverRubberband->setGeometry(mapFromScene(sysRect(l)));
                    if (!HoverRubberband->isVisible()) {
                        //HoverRubberband->show(40);
                    }
*/
                    hoverRect->show(sysRect(l));
                }
            }
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}

void LayoutViewXML::mouseReleaseEvent(QMouseEvent *event)
{
    if (layoutCount()==0) return;
    //if (HoverRubberband->isVisible()) HoverRubberband->hide();
    hoverRect->hide();
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
                XMLLayoutSystemWrapper XMLSystem = activeSystem();
                XMLSystem.setTop(XMLSystem.top() + activeLayout()->Options.scale(MoveSystemY));
                PlotActiveSystem();
                DrawActiveRect();
                placeButtons(ML);
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
    QGraphicsView::mouseReleaseEvent(event);
}

void LayoutViewXML::AddPaper()
{
    SceneNumOfPages++;
    Scene->setSceneRect(0,0,paperRect(SceneNumOfPages-1).right()+10,paperRect().height() + 10);
    DrawPaper(SceneNumOfPages - 1);
}

void LayoutViewXML::RemovePaper()
{
    const int Page = SceneNumOfPages - 1;
    if (Page < 0) return;
    /*
    if (PageList.contains(Page))
    {
        Scene->removeItem(PageList[Page].Paper);
        Scene->removeItem(PageList[Page].Shadow);
        delete PageList[Page].Paper;
        delete PageList[Page].Shadow;
        PageList.remove(Page);
    }
*/
    if (Page < PageList.size()) delete PageList.takeAt(Page);
    //if (Page < PageButtonsList.size()) delete PageButtonsList.takeAt(Page);
    /*
    if (Page < SystemButtonsList.size()) {
        QGraphicsItemList& l = SystemButtonsList[Page];
        SystemButtonsList.removeAt(Page);
        qDeleteAll(l);
        l.clear();
    }
*/
    SceneNumOfPages --;
    Scene->setSceneRect(0,0,paperRect(SceneNumOfPages-1).right() + 10,paperRect().height() + 10);
}

void LayoutViewXML::Clear()
{
    NoScrollFlag=true;
    Scene->setSceneRect(0,0,1,1);
    if (layoutCount() > 0) activeLayout()->clear();
    PageList.clear();
    Scene->removeItem(rubberBand);
    Scene->removeItem(hoverRect);
    Scene->removeItem(PageButtonsItem);
    Scene->removeItem(SystemButtonsItem);
    Scene->clear();
    Scene->addItem(SystemButtonsItem);
    Scene->addItem(PageButtonsItem);
    Scene->addItem(hoverRect);
    Scene->addItem(rubberBand);
    SceneNumOfPages=0;
    //rb->hide();
    rubberBand->hide();
    hoverRect->hide();
    NoScrollFlag=false;
}

void LayoutViewXML::DrawPaper(const int Page)
{
    if (Page >= PageList.size()) {
        const QRectF r(paperRect(Page));
        QPen p;  // creates a default pen
        p.setStyle(Qt::SolidLine);
        p.setWidth(4);
        p.setBrush(QColor(0,0,0,10));
        p.setCapStyle(Qt::SquareCap);
        p.setJoinStyle(Qt::RoundJoin);
        QGraphicsItem* shadow = rectItem(r.adjusted(10,10,10,10),p,QBrush(QColor(0,0,0,80)));
        QGraphicsItem* paper = rectItem(r,QPen(Qt::black),paperbrush);
        QGraphicsContainerItem* pageItem = new QGraphicsContainerItem(Scene);
        pageItem->setFlag(QGraphicsItem::ItemStacksBehindParent);
        pageItem->append(shadow);
        pageItem->append(paper);
        PageList.append(pageItem);
    }
        /*
        for (int s = 0; s < activeLayout()->XMLPage(p).systemCount(); s++) {
            LayoutLocation l(p,s);
            qDebug() << activeLayout()->SysRect(l);
            //Scene->addRect(activeLayout()->SysRect(l),QPen(Qt::blue));
        }
*/
    //}
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
    placeButtons(activeLayoutLocation());
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
    activeLayout()->EraseSystem(l,Scene);

}

void LayoutViewXML::ClearActiveSystem()
{
    ClearSystem(activeLayoutLocation());
}

void LayoutViewXML::MoveActiveSystemTop(int y){
    MoveSystemTop(activeLayoutLocation(),y);
}

void LayoutViewXML::MoveSystemTop(const LayoutLocation &l, int y){
    activeLayout()->MoveSystemTop(l,y);
}

void LayoutViewXML::PlotActiveSystem()
{
    activeLayout()->PlotSystem(activeLayoutLocation(), XMLScore, Scene);
    DrawPaper(activeLayoutLocation().Page);
    //this->viewport()->update();
    DrawActiveRect();
}

const QRectF LayoutViewXML::sysRect(const LayoutLocation &l) const {
    return activeLayout()->SysRect(l);
}

const QRectF LayoutViewXML::paperRect(const int page) const {
    return activeLayout()->PaperRect(page);
}

const QRectF LayoutViewXML::paperRect() const {
    return activeLayout()->PaperRect();
}

const QRect LayoutViewXML::mapFromScene(const QRectF &r) const {
    return QRect(QGraphicsView::mapFromScene(r.topLeft()),QGraphicsView::mapFromScene(r.bottomRight()));
}

LayoutLocation LayoutViewXML::InsideLocation(const QPointF p) const {
    LayoutLocation l;
    l.Page = InsidePage(p);
    if (l.Page > -1) l.System = InsideSystem(p, l.Page);
    return l;
}

int LayoutViewXML::InsidePage(const QPointF p) const {
    for (int i = 0; i < pageCount(); i++)
    {
        QRectF r(paperRect(i));
        if (r.contains(p)) {
            return i;
        }
    }
    return -1;
}

int LayoutViewXML::InsideSystem(const QPointF p, const int Page) const {
    if (Page < 0 ) return -1;
    for (int i = 0; i < systemCount(Page); i++)
    {
        QRectF s(sysRect(LayoutLocation(Page,i)));
        if (s.contains(p)) return i;
    }
    return -1;
}

void LayoutViewXML::placeButtons(LayoutLocation l) {
    if (systemCount(l.Page)) {
        QRectF lastSystemRect = sysRect(LayoutLocation(l.Page,systemCount(l.Page)-1));
        PageButtonsItem->setPos(lastSystemRect.left(),lastSystemRect.bottom());
        for (QGraphicsItem* i : PageButtonsItem->childItems()) {
            QGraphicsToolButton* b = static_cast<QGraphicsToolButton*>(i);
            b->setSize(lastSystemRect.width(),240);
        }
        addSystemButton->setEnabled(!activeLayout()->isLastPage(l.Page));
        removeSystemButton->setEnabled(activeLayout()->systemCount(l.Page) > 1);
    }
    QRectF r = sysRect(l);
    addBarButton->setPos(r.width() - 240,r.height() * 0.5);
    addBarButton->setSize(240,r.height() * 0.5);
    addBarButton->setEnabled(!activeLayout()->isLastSystem(l));
    removeBarButton->setPos(r.width() - 240,0);
    removeBarButton->setSize(240,r.height() * 0.5);
    removeBarButton->setEnabled((activeLayout()->XMLSystem(l).endBar()-activeLayout()->XMLSystem(l).startBar()>1));
    toggleNamesButton->setSize(240,r.height());
    SystemButtonsItem->setPos(r.topLeft());
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

void LayoutViewXML::setActiveLayout(const int Index) {
    if (!layoutCount()) return;
    lc.setActiveLayout(Index);
    if (pageCount())
    {
        setActiveLocation(activeLayoutLocation());
    }
    emit SelectionChanged();
}

int LayoutViewXML::activeLayoutIndex() const { return lc.activeLayoutIndex(); }

int LayoutViewXML::activePageIndex() const { return lc.activePageIndex(); }

int LayoutViewXML::activeSystemIndex() const { return lc.activeSystemIndex(); }

QRectF LayoutViewXML::activeSystemRect() const {
    return mapFromScene(sysRect(activeLayoutLocation()));
}

LayoutLocation LayoutViewXML::activeLayoutLocation() const { return lc.activeLayoutLocation(); }

LayoutLocation LayoutViewXML::bottomSystem() { return LayoutLocation(activePageIndex(),systemCount() - 1); }

int LayoutViewXML::activeStartBar() { return activeSystem().startBar(); }

int LayoutViewXML::activeEndBar() { return activeSystem().endBar(); }

void LayoutViewXML::AdjustPage(const int Page)
{
    DrawPaper(Page);
    activeLayout()->AdjustSystems(Page);
    if (Page == 0) activeLayout()->PlotTitle(Scene);
    for (int i = 0; i < systemCount(Page); i++)
    {
        const LayoutLocation l(Page,i);
        ClearSystem(l);
        activeLayout()->PlotSystem(l, XMLScore, Scene);
        DrawPaper(l.Page);
    }
    //this->viewport()->update();
}

void LayoutViewXML::DrawRect(const LayoutLocation& l, int y, bool Animate)
{
    if (layoutCount() && activeLayout()->pageExists(l.Page))
    {
        //QRect tTempRect(mapFromScene(sysRect(l).translated(0,y)));
        QRectF rbRect(sysRect(l).translated(0,y));
        //if (rb->geometry() != tTempRect)
        if (rubberBand->rect() != rbRect)
        {
            //rb->setGeometry(tTempRect);
            if (Animate)
            {
                //rb->showAnimated();
                rubberBand->show(rbRect);
            }
            else
            {
                //if (!rb->isVisible()) rb->show();
                rubberBand->show(rbRect);
            }
        }
        else
        {
            //if (!rb->isVisible()) rb->show();
            rubberBand->show(rbRect);
        }
    }
    else
    {
        //rb->hide();
        rubberBand->hide();
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
    if (!lc.layout(i)->pageCount()) return QDPRPixmap(":/layout.png");
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

void LayoutViewXML::MakeBackup(const QString &text) { emit BackMeUp(text); }

void LayoutViewXML::changeZoom(double zoom) {
    DrawActiveRect();
    //if (HoverRubberband->isVisible()) HoverRubberband->hide();
    //hoverRect->hide();
    emit ZoomChanged(zoom);
}

void LayoutViewXML::setZoom(const double Zoom) {
    DrawActiveRect();
    //if (HoverRubberband->isVisible()) HoverRubberband->hide();
    //hoverRect->hide();
    zoomer->setZoom(Zoom);
}

void LayoutViewXML::MoveSystemToNextPage(){
    MoveSystem(1);
}

void LayoutViewXML::GetSystemFromNextPage(){
    MoveSystem(-1);
}

double LayoutViewXML::getZoom() const {
    return zoomer->getZoom();
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

void LayoutViewXML::InitLayout(const int Index) { lc.InitLayout(Index,XMLScore); }

const QString LayoutViewXML::LayoutName(const int Index) const { return XMLScore.LayoutName(Index); }

int LayoutViewXML::layoutCount() const { return XMLScore.layoutCount(); }
