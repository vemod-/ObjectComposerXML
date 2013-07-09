#ifndef CBARWINDOW_H
#define CBARWINDOW_H

#include <QWidget>
#include "scoreviewxml.h"
#include <QTableWidget>
#include <QGridLayout>

namespace Ui {
    class CBarWindow;
}

class CBarWindow : public QWidget
{
    Q_OBJECT
public:
    explicit CBarWindow(QWidget *parent = 0);
    ~CBarWindow();
    QTableWidget* table;
    void Fill(OCBarMap& bars, int StartBar, int Staff, int Voice);
    void SelectAll();
    void SelectToEnd();
    void SelectFromStart();
signals:
    void BarChanged(int Bar, int Staff, int Voice);
    void SelectionChanged(QRect Selection);
    void Popup(QPoint Pos);
private:
    Ui::CBarWindow *ui;
    //QList<QPair<int,int> > VoiceList;
    OCBarMap BarMap;
private slots:
    void Edit(int Row,int Col);
    void SelectVoice(int Row);
    void Select();
    void MouseRelease(QMouseEvent* event);
};

#endif // CBARWINDOW_H
