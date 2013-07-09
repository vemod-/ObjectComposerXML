#ifndef LAYOUTVIEWXML_H
#define LAYOUTVIEWXML_H

#include "clayout.h"
#include <QWidget>
#include <QGraphicsView>
#include <QRubberBand>
#include "qmacrubberband.h"
#include "ceditsystem.h"

namespace Ui {
    class LayoutViewXML;
}

class LayoutViewXML : public QGraphicsView
{
    Q_OBJECT

public:
    explicit LayoutViewXML(QWidget *parent = 0);
    ~LayoutViewXML();
    int SceneNumOfPages;
    //int ActiveSystem;
    //int ActivePage;
    //int CurrentLayoutIndex;
    float ScaleFactor;
    void FormatAndDraw(const int StartPage, const int StartSystem, const bool Auto);
    void Unformat();
    void MoveSystem(const int Direction);
    void MoveBar(const int Direction);
    void AddPage();
    void RemovePage();
    void Clear();
    void SetXMLScore(XMLScoreWrapper& Doc);
    void Init();
    void Reformat(const int StartPage=0, const int StartSystem=0, const int Stretch=0);
    void ToggleNames();
    void ResetSystem();
    void AdjustCurrentPage();
    void AdjustPage(const int Page);
    void AdjustPages(const int StartPage);
    void SetCurrentLayout(const int Index);
    const int CurrentLayout() const;
    const int ActivePage() const;
    const int ActiveSystem() const;
    //void SetLayoutCollectionCurrentLayout();
    void AddTitle();
    void EditSystem(QWidget* parent);
    void PrintIt(const int StartPage, QGraphicsScene* Scene);
    void Load(XMLScoreWrapper& Score);
    void ReadXML();
    void WriteXML();
    void AddLayout(const QString& Name=QString());
    void RemoveLayout(const int Index);
    void InitLayout(const int Index);
    const QString LayoutName(const int Index) const;
    const int NumOfLayouts() const;
    CLayoutCollection lc;
    const int GetZoom() const;
public slots:
    void PrintPreview();
    const bool PrintAll(QPrinter* Printer);
    void PrinterPrint();
    void PageSetup();
    void MakeBackup(const QString& text);
    void SetZoom(const int Zoom);
    void SetActiveObjects(const int Page, const int System);
signals:
    void Changed();
    void BackMeUp(QString Text);
    void SelectionChanged();
    void DoubleClick();
    void Popup(QPoint pos);
protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent (QMouseEvent* event);
    virtual void scrollContentsBy(int dx, int dy);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void leaveEvent(QEvent *event);
    //virtual void resizeEvent(QResizeEvent* event);
private:
    Ui::LayoutViewXML *ui;
    QGraphicsScene* Scene;
    XMLScoreWrapper XMLScore;
    CLayout* Layout;
    QMacRubberband* rb;
    QMap<int,QPair<QGraphicsItem*,QGraphicsItem*> > PageList;
    qreal zoomfactor;
    bool IsInitialized;
    bool NoScrollFlag;

    QHoverRubberband* HoverRubberband;
    float my;
    bool MD;
    int MP;
    int MS;
    int MoveSystemY;
    void DrawPaper(const int Page);
    void ClearSystem(const int Page, const int System);
    void PlotActiveSystem();
    const int SysLeft(const int Page);
    const int SysRight(const int Page);
    const int SysTop(const int Page, const int System);
    const int SysBottom(const int Page, const int System);
    const int InsideSystem(const float y, const int Page);
    const int PageLeft(const int Page);
    const int PageRight(const int Page);
    void DrawRect(int Page, int System, int y, bool Animated=false);
};

#endif // LAYOUTVIEWXML_H
