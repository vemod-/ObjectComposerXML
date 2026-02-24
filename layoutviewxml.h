#ifndef LAYOUTVIEWXML_H
#define LAYOUTVIEWXML_H

#include "clayout.h"
#include <QWidget>
//#include <QGraphicsView>
//#include <QRubberBand>
//#include "qmacrubberband.h"
//#include "ceditsystem.h"
#include "qgraphicsviewzoomer.h"
#include "qdprpixmap.h"

namespace Ui {
    class LayoutViewXML;
}
/*
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
*/
//typedef QMap<int,PageGraphics> PageGraphicsMap;

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
    void BarToNextSystem();
    void BarFromNextSystem();
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
    void setActiveLayout(const int Index);
    int activeLayoutIndex() const;
    int activePageIndex() const;
    int activeSystemIndex() const;
    QRectF activeSystemRect() const;
    LayoutLocation activeLayoutLocation() const;
    LayoutLocation bottomSystem();
    int activeStartBar();
    int activeEndBar();
    void AddTitle();
    void PrintIt(const int StartPage, QGraphicsScene* Scene);
    void SetXML(XMLScoreWrapper& Score);
    void ReloadXML();
    void InitLayout(const int Index);
    const QString LayoutName(const int Index) const;
    int layoutCount() const;
    const QPixmap firstPageIcon(const int i);
    XMLScoreWrapper XMLScore;
public slots:
    void PrintPreview();
    bool PrintAll(QPrinter* Printer);
    void PrinterPrint(const QString& pdfPath=QString());
    void PageSetup();
    void MakeBackup(const QString& text);
    void changeZoom(double zoom);
    void setZoom(const double Zoom);
private slots:
    void MoveSystemToNextPage();
    void GetSystemFromNextPage();
public:
    double getZoom() const;
    void SelectSystem(const LayoutLocation& l);
    int pageOfBar(const int bar) const;
    int systemOfBar(const int page, const int bar) const;
    LayoutLocation locationOfBar(const int bar) const;
    bool pageContainsBar(const int page, const int bar) const;
    bool systemContainsBar(const int page, const int system, const int bar) const;
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
    //QMacRubberband* rb;
    QGraphicsRubberBand* rubberBand;
    QGraphicsItemList PageList;
    //QGraphicsItemList PageButtonsList;
    //QList<QGraphicsItemList> SystemButtonsList;
    QGraphicsContainerItem* PageButtonsItem;
    QGraphicsContainerItem* SystemButtonsItem;
    QGraphicsToolButton* addSystemButton;
    QGraphicsToolButton* removeSystemButton;
    QGraphicsToolButton* addBarButton;
    QGraphicsToolButton* removeBarButton;
    QGraphicsToolButton* toggleNamesButton;
    QGraphicsViewZoomer* zoomer;
    LayoutLocation hoverLocation;
    bool NoScrollFlag;
    int SceneNumOfPages;
    //const QBrush paperbrush=QBrush(QPixmap(":/grey-paper-texture.jpg"));
    const QBrush paperbrush = QBrush(QDPRPixmap(":/lightpaperfibers.png"));
    /*
    inline const QRectF sizeToView(const QRectF& r) const {
        return QRectF(r.topLeft(),r.size());
    }
    inline double sizeToView(const double v) { return v; }
*/
    int pageCount() const;
    int systemCount() const;
    int systemCount(const int Page) const;
    CLayout* activeLayout() const;
    XMLLayoutSystemWrapper activeSystem() const;
    void setActiveLocation(const LayoutLocation& l);
    //QHoverRubberband* HoverRubberband;
    QGraphicsRubberBand* hoverRect;
    double my;
    bool MD;
    LayoutLocation ML;
    int MoveSystemY;
    void DrawPaper(const int Page);
    void ClearSystem(const LayoutLocation& l);
    void ClearActiveSystem();
    void MoveActiveSystemTop(int y);
    void MoveSystemTop(const LayoutLocation& l, int y);
    void PlotActiveSystem();
    const QRectF sysRect(const LayoutLocation& l) const;
    const QRectF paperRect(const int page) const;
    const QRectF paperRect() const;
    const QRect mapFromScene(const QRectF& r) const;
    LayoutLocation InsideLocation(const QPointF p) const;
    int InsidePage(const QPointF p) const;
    int InsideSystem(const QPointF p, const int Page) const;
    void placeButtons(LayoutLocation l);
    void DrawRect(const LayoutLocation& l, int y, bool Animated=false);
    void DrawActiveRect();
};

#endif // LAYOUTVIEWXML_H
