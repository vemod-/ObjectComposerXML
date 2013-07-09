#ifndef CPATTERNLIST_H
#define CPATTERNLIST_H

#include <QDialog>
#include "CommonClasses.h"
#include "scoreviewxml.h"

namespace Ui {
    class CPatternList;
}

class CPatternList : public QDialog
{
    Q_OBJECT

public:
    explicit CPatternList(QWidget *parent = 0);
    ~CPatternList();
    void AppendPattern(const QList<QPair<int,int> >& Pattern);
    bool SelectPattern(QList<QPair<int,int> >& Pattern);
private:
    Ui::CPatternList *ui;
    void fill();
    void PatternToList(QDomLiteElement* Pattern,QList<QPair<int,int> >& List);
    QDomLiteElement* ListToPattern(const QList<QPair<int,int> >& List);
    QDomLiteDocument* XML;
    QDomLiteElement* theNode;
private slots:
    void PatternSelected(int row,int col);
    void MouseRelease(QMouseEvent* event);
};

#endif // CPATTERNLIST_H
