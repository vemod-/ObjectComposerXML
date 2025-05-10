#ifndef CMUSICTREE_H
#define CMUSICTREE_H

//#include "CommonClasses.h"
#include <QWidget>
//#include <QGridLayout>
#include "qmactreewidget.h"
#include "ocbarmap.h"

namespace Ui {
    class CMusicTree;
}

class CMusicTree : public QWidget
{
    Q_OBJECT

public:
    explicit CMusicTree(QWidget *parent = nullptr);
    ~CMusicTree();
public slots:
    void Fill(XMLScoreWrapper& XMLScore, OCBarMap& BarMap, int BarCount, const OCBarLocation& BarLocation, OCCursor* C);
    QSize contentSize();
protected:
    //virtual void resizeEvent(QResizeEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
signals:
    void SelectionChanged();
    void SendDelete();
    void SendBackspace();
    void Popup(QPoint Pos);
    void Delete(int Pointer);
    void Properties(QPoint Pos);
    void Rearranged(QList<int> symbolOrder);
private:
    Ui::CMusicTree *ui;
    QAction* setAction(QKeySequence keySequence);
    QMacTreeWidget* table;
    OCCursor* Cursor=nullptr;
    void AdjustSelection();
private slots:
    void ItemChange();
    void ClickItem(QTreeWidgetItem* item, int Col);
    void showContextMenu(QPoint p);
    void DoubleClickItem(QTreeWidgetItem* item, int Col);
    void ItemsMoved(QList<QTreeWidgetItem*> dragItems);
    //void MouseRelease(QMouseEvent* event);
    //void MouseMove(QMouseEvent* event);
    //void MouseEnter(QEvent* event);
    //void MouseLeave(QEvent* event);
};

#endif // CMUSICTREE_H
