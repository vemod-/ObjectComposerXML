#ifndef LAYOUTVIEWXML_H
#define LAYOUTVIEWXML_H

#include "clayout.h"
#include <QWidget>
//#include <QGraphicsView>
//#include <QRubberBand>
#include "qmacrubberband.h"
//#include "ceditsystem.h"
#include "qgraphicsviewzoomer.h"

namespace Ui {
    class LayoutViewXML;
}

class PageGraphics
{
public:
    PageGraphics(QGraphicsItem* paper, QGraphicsItem* shadow)
    {
        Paper=paper;
        Shadow=shadow;
    }
    PageGraphics()
    {
        Paper=nullptr;
        Shadow=nullptr;
    }
    QGraphicsItem* Paper;
    QGraphicsItem* Shadow;
};

typedef QMap<int,PageGraphics> PageGraphicsMap;

class LayoutViewXML : public QGraphicsView
{
    Q_OBJECT

public:
    explicit LayoutViewXML(QWidget *parent = nullptr);
    ~LayoutViewXML();
    void FormatAndDraw(const LayoutLocation& StartLocation, const bool Auto);
    void Unformat();
    void MoveSystem(const int Direction);
    void MoveBar(const int Direction);
    void AddPaper();
    void RemovePaper();
    void Clear();
    void Init();
    void Reformat(const LayoutLocation& StartLocation = LayoutLocation(0,0), const int Stretch=0);
    void ToggleNames();
    void ResetSystem();
    void AdjustCurrentPage();
    void AdjustPage(const int Page);
    void AdjustPages(const int StartPage);
    void setActiveLayout(const int Index) {
        if (!layoutCount()) return;
        lc.setActiveLayout(Index);
        if (pageCount())
        {
            setActiveLocation(activeLayoutLocation());
        }
        emit SelectionChanged();
    }
    inline int activeLayoutIndex() const { return lc.activeLayoutIndex(); }
    inline int activePageIndex() const { return lc.activePageIndex(); }
    inline int activeSystemIndex() const { return lc.activeSystemIndex(); }
    inline QRectF activeSystemRect() const {
        return mapFromScene(sysRect(activeLayoutLocation()));
    }
    inline LayoutLocation activeLayoutLocation() const { return lc.activeLayoutLocation(); }
    inline LayoutLocation bottomSystem() { return LayoutLocation(activePageIndex(),systemCount()-1); }
    inline int activeStartBar() { return activeSystem().startBar(); }
    inline int activeEndBar() { return activeSystem().endBar(); }
    void AddTitle();
    void PrintIt(const int StartPage, QGraphicsScene* Scene);
    void SetXML(XMLScoreWrapper& Score);
    void ReloadXML();
    void InitLayout(const int Index) { lc.InitLayout(Index,XMLScore); }
    const QString LayoutName(const int Index) const { return XMLScore.LayoutName(Index); }
    inline int layoutCount() const { return XMLScore.layoutCount(); }
    const QPixmap firstPageIcon(const int i);
    XMLScoreWrapper XMLScore;
public slots:
    void PrintPreview();
    bool PrintAll(QPrinter* Printer);
    void PrinterPrint(const QString& pdfPath=QString());
    void PageSetup();
    void MakeBackup(const QString& text) { emit BackMeUp(text); }
    void changeZoom(double zoom) {
        DrawActiveRect();
        if (HoverRubberband->isVisible()) HoverRubberband->hide();
        emit ZoomChanged(zoom);
    }
    void setZoom(const double Zoom) {
        DrawActiveRect();
        if (HoverRubberband->isVisible()) HoverRubberband->hide();
        zoomer->setZoom(Zoom);
    }
public:
    double getZoom() const {
        return zoomer->getZoom();
    }
    void SelectSystem(const LayoutLocation& l);
    int pageOfBar(const int bar) const {
        return activeLayout()->PageOfBar(bar);
    }
    int systemOfBar(const int page, const int bar) const {
        return activeLayout()->SystemOfBar(page, bar);
    }
    LayoutLocation locationOfBar(const int bar) const {
        const int p = pageOfBar(bar);
        return LayoutLocation(p,systemOfBar(p,bar));
    }
    bool pageContainsBar(const int page, const int bar) const {
        return (activeLayout()->PageOfBar(bar) == page);
    }
    bool systemContainsBar(const int page, const int system, const int bar) const {
        return (activeLayout()->SystemOfBar(page, bar) == system);
    }
signals:
    void Changed();
    void BackMeUp(const QString& Text);
    void SelectionChanged();
    void DoubleClick();
    void Popup(QPoint pos);
    void PopupPage(QPoint pos);
    void PopupSystem(QPoint pos);
    void ZoomChanged(double Zoom);
protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent (QMouseEvent* event);
    virtual void scrollContentsBy(int dx, int dy);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void leaveEvent(QEvent *event);
private:
    Ui::LayoutViewXML *ui;
    QGraphicsScene* Scene;
    CLayoutCollection lc;
    QMacRubberband* rb;
    PageGraphicsMap PageList;
    QGraphicsViewZoomer* zoomer;
    bool NoScrollFlag;
    int SceneNumOfPages;
    //const QBrush paperbrush=QBrush(QPixmap(":/grey-paper-texture.jpg"));
    const QBrush paperbrush=QBrush(QPixmap(":/lightpaperfibers.png"));
    /*
    inline const QRectF sizeToView(const QRectF& r) const {
        return QRectF(r.topLeft(),r.size());
    }
    inline double sizeToView(const double v) { return v; }
*/
    inline int pageCount() const { return activeLayout()->pageCount(); }
    inline int systemCount() const { return systemCount(activePageIndex()); }
    inline int systemCount(const int Page) const { return activeLayout()->systemCount(Page); }
    inline CLayout* activeLayout() const { return lc.activeLayout(); }
    inline XMLLayoutSystemWrapper activeSystem() const { return *activeLayout()->activeSystem(); }
    void setActiveLocation(const LayoutLocation& l) { activeLayout()->setActiveLocation(l); }
    QHoverRubberband* HoverRubberband;
    double my;
    bool MD;
    LayoutLocation ML;
    int MoveSystemY;
    void DrawPaper(const int Page);
    void ClearSystem(const LayoutLocation& l);
    void ClearActiveSystem();
    void PlotActiveSystem();
    inline const QRectF sysRect(const LayoutLocation& l) const {
        return activeLayout()->SysRect(l);
    }
    inline const QRectF paperRect(const int page) const {
        return activeLayout()->PaperRect(page);
    }
    inline const QRectF paperRect() const {
        return activeLayout()->PaperRect();
    }
    inline const QRect mapFromScene(const QRectF& r) const {
        return QRect(QGraphicsView::mapFromScene(r.topLeft()),QGraphicsView::mapFromScene(r.bottomRight()));
    }
    LayoutLocation InsideLocation(const QPointF p) const {
        LayoutLocation l;
        l.Page = InsidePage(p);
        if (l.Page > -1) l.System = InsideSystem(p, l.Page);
        return l;
    }
    int InsidePage(const QPointF p) const {
        for (int i = 0; i < pageCount(); i++)
        {
            QRectF r(paperRect(i));
            if (r.contains(p)) {
                return i;
            }
        }
        return -1;
    }
    int InsideSystem(const QPointF p, const int Page) const {
        if (Page < 0 ) return -1;
        for (int i = 0; i < systemCount(Page); i++)
        {
            QRectF s(sysRect(LayoutLocation(Page,i)));
            if (s.contains(p)) return i;
        }
        return -1;
    }
    void DrawRect(const LayoutLocation& l, int y, bool Animated=false);
    void DrawActiveRect();
};

#endif // LAYOUTVIEWXML_H
