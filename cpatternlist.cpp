#include "cpatternlist.h"
#include "csymbol.h"
#include "ui_cpatternlist.h"
//#include <QDesktopWidget>
//#include <QThread>
//#include <QSettings>
#include <unistd.h>

CPatternList::CPatternList(QWidget *parent) :
        QDialog(parent),
    ui(new Ui::CPatternList)
{
    ui->setupUi(this);
}

CPatternList::~CPatternList()
{
    delete ui;
}

void CPatternList::AppendPattern(const OCPatternNoteList& Pattern)
{
    OCSettings s;
    QDomLiteDocument XML;
    XML.fromString(s.value("OCStuff").toString());
    auto theNode=XML.documentElement->elementByTagCreate("Patterns");
    theNode->appendChild(ListToPattern(Pattern));
    s.setValue("OCStuff",XML.toString());
}

bool CPatternList::SelectPattern(OCPatternNoteList& Pattern)
{
    this->setWindowTitle("Select Pattern");
    OCSettings s;
    XML.fromString(s.value("OCStuff").toString());
    theNode=XML.documentElement->elementByTagCreate("Patterns");
    fill();
    connect(ui->tableWidget,&QTableWidget::cellClicked,this,&CPatternList::PatternSelected);
    this->setWindowFlags(Qt::Popup);
    this->move(cursor().pos());
    this->show();
    this->raise();
    QApplication::processEvents();
    while (this->isVisible()) {
        QApplication::processEvents();
        usleep(1000);
    }
    if (Success)
    {
        PatternToList(theNode->childElement(ui->tableWidget->currentRow()),Pattern);
    }
    this->deleteLater();
    return Success;
}

void CPatternList::createRow(const QDomLiteElement* p, QTableWidget* lw, const int i, const int offset)
{
    if (lw->columnCount() < p->attributeCount()+offset) lw->setColumnCount(p->attributeCount()+offset);
    for (int j=0;j<p->attributeCount();j++)
    {
        auto item=new QTableWidgetItem();
        item->setIcon(QIcon(QStringLiteral(":/patterns/patterns/")+p->attribute(QString::number(j)).string()+QStringLiteral(".png")));
        item->setSizeHint(QSize(20,32));
        lw->setItem(i,j+offset,item);
    }
}

void CPatternList::createRow(const OCPatternNoteList& List, QTableWidget* lw, const int i, const int offset)
{
    const auto e = ListToPattern(List);
    createRow(e,lw,i,offset);
    delete e;
}

void CPatternList::fill()
{
    QTableWidget* lw=ui->tableWidget;
    lw->blockSignals(true);
    lw->hide();
    lw->clear();
    lw->setIconSize(QSize(16,32));
    lw->setColumnCount(0);
    lw->setRowCount(theNode->childCount());
    for (int i=0;i<lw->rowCount();i++) lw->setRowHeight(i,32);
    for (int i=0;i<theNode->childCount();i++) createRow(theNode->childElement(i),lw,i);
    for (int i=0;i<lw->columnCount();i++) lw->setColumnWidth(i,20);
    lw->setColumnCount(lw->columnCount()+2);
    for (int i=0;i<theNode->childCount();i++)
    {
        auto item=new QTableWidgetItem();
        item->setIcon(QIcon(":/fileclose.png"));
        item->setSizeHint(QSize(24,24));
        lw->setItem(i,lw->columnCount()-1,item);
    }
    lw->setColumnWidth(lw->columnCount()-1,24);
    lw->resizeColumnToContents(lw->columnCount()-1);
    lw->horizontalHeader()->adjustSize();
    lw->show();
    int w=0;
    for (int i=0;i<lw->columnCount();i++) w+=lw->columnWidth(i);
    lw->setFixedSize(w,qMax(32,32*lw->rowCount()));
    this->setFixedSize(lw->size());
    lw->blockSignals(false);
}

void CPatternList::PatternSelected(int row, int col)
{
    if (col==ui->tableWidget->columnCount()-1)
    {
        theNode->removeChild(row);
        OCSettings s;
        s.setValue("OCStuff",XML.toString());
        fill();
    }
    else
    {
        Success=true;
        this->hide();
    }
}

void CPatternList::PatternToList(QDomLiteElement *Pattern, OCPatternNoteList &List)
{
    List.clear();
    for (int i=0;i<Pattern->attributeCount();i++)
    {
        const QString s=Pattern->attribute(QString::number(i));
        List.append(OCPatternNote(s.left(1).toInt(),s.right(1).toInt()));
    }
}

QDomLiteElement* CPatternList::ListToPattern(const OCPatternNoteList &List)
{
    QDomLiteElement* p=new QDomLiteElement("Pattern");
    for (int i=0;i<List.size();i++)
    {
        p->setAttribute(QString::number(i),QString(QString::number(List.at(i).Button)+QString::number(List.at(i).TripletDotFlag)));
    }
    return p;
}

