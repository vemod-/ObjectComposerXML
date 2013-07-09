#ifndef CMUSICTREE_H
#define CMUSICTREE_H

#include "CommonClasses.h"
#include <QWidget>
#include <QGridLayout>
#include "qmactreewidget.h"

namespace Ui {
    class CMusicTree;
}

class CMusicTree : public QWidget
{
    Q_OBJECT

public:
    explicit CMusicTree(QWidget *parent = 0);
    ~CMusicTree();
public slots:
    void Fill(XMLScoreWrapper& XMLScore, OCBarMap& BarMap, int StartBar, int BarCount, int Staff, int Voice, OCCursor* C);
protected:
    //virtual void resizeEvent(QResizeEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
signals:
    void SelectionChanged();
    void SendDelete();
    void SendBackspace();
    void Popup(QPoint Pos);
    void Delete(int Pointer);
private:
    Ui::CMusicTree *ui;
    QAction* setAction(QKeySequence keySequence);
    QMacTreeWidget* table;
    OCCursor* Cursor;
    void AdjustSelection();
private slots:
    void ItemChange();
    void ClickItem(QTreeWidgetItem* item, int Col);
    //void MouseRelease(QMouseEvent* event);
    //void MouseMove(QMouseEvent* event);
    //void MouseEnter(QEvent* event);
    //void MouseLeave(QEvent* event);
};

#endif // CMUSICTREE_H
