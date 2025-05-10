#ifndef CPATTERNLIST_H
#define CPATTERNLIST_H

#include <QDialog>
//#include "CommonClasses.h"
//#include "scoreviewxml.h"
#include "ocxmlwrappers.h"
#include <QTableWidget>

namespace Ui {
    class CPatternList;
}

class CPatternList : public QDialog
{
    Q_OBJECT

public:
    explicit CPatternList(QWidget *parent = nullptr);
    ~CPatternList();
    static void AppendPattern(const OCPatternNoteList& Pattern);
    bool SelectPattern(OCPatternNoteList& Pattern);
    static void createRow(const QDomLiteElement* p, QTableWidget* lw, const int i, const int offset=0);
    static void createRow(const OCPatternNoteList& List, QTableWidget* lw, const int i, const int offset=0);
    static void PatternToList(QDomLiteElement* Pattern,OCPatternNoteList& List);
    static QDomLiteElement* ListToPattern(const OCPatternNoteList& List);
private:
    Ui::CPatternList *ui;
    void fill();
    QDomLiteDocument XML;
    QDomLiteElement* theNode=nullptr;
    bool Success=false;
private slots:
    void PatternSelected(int row,int col);
};

#endif // CPATTERNLIST_H
