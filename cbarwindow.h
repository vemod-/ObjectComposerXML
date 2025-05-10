#ifndef CBARWINDOW_H
#define CBARWINDOW_H

//#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QGridLayout>
#include "ocbarmap.h"

namespace Ui {
    class CBarWindow;
}

class CBarWindow : public QWidget
{
    Q_OBJECT
public:
    explicit CBarWindow(QWidget *parent = nullptr);
    ~CBarWindow();
    QTableWidget* table;
    void Fill(OCBarMap& bars, const OCBarLocation& BarVoiceLocation, const int BarCount);
    void SelectAll();
    void SelectToEnd();
    void SelectFromStart();
    QSize contentSize();
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
signals:
    void BarChanged(int Bar, int Staff, int Voice);
    void SelectionChanged(QRect Selection);
    void Popup(QPoint Pos);
private:
    Ui::CBarWindow *ui;
    //OCSelectionList VoiceList;
    OCBarMap BarMap;
private slots:
    void Edit(int Row,int Col);
    void SelectVoice(int Row);
    void Select();
};

#endif // CBARWINDOW_H
