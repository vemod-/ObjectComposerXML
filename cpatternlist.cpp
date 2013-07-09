#include "cpatternlist.h"
#include "ui_cpatternlist.h"
#include "mouseevents.h"

CPatternList::CPatternList(QWidget *parent) :
        QDialog(parent,Qt::Sheet),
    ui(new Ui::CPatternList)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
}

CPatternList::~CPatternList()
{
    delete ui;
}

void CPatternList::AppendPattern(const QList<QPair<int, int> > &Pattern)
{
    XML=new QDomLiteDocument(settingsfile);
    theNode=XML->documentElement->elementByTag("Patterns");
    if (theNode==0) theNode=XML->documentElement->appendChild("Patterns");
    theNode->appendChild(ListToPattern(Pattern));
    XML->save(settingsfile);
    delete XML;
}

void CPatternList::MouseRelease(QMouseEvent *event)
{
    QPoint p=event->pos();
    if (rect().contains(p))
    {
        ui->tableWidget->viewport()->releaseMouse();
        return;
    }
    this->setResult(0);
    this->close();
}

bool CPatternList::SelectPattern(QList<QPair<int, int> > &Pattern)
{

    this->setWindowTitle("Select Pattern");
    XML=new QDomLiteDocument(settingsfile);
    theNode=XML->documentElement->elementByTag("Patterns");
    if (theNode==0) theNode=XML->documentElement->appendChild("Patterns");
    fill();
    MouseEvents* ev=new MouseEvents;
    ui->tableWidget->viewport()->installEventFilter(ev);
    connect(ev,SIGNAL(MousePress(QMouseEvent*)),this,SLOT(MouseRelease(QMouseEvent*)));
    this->setVisible(true);
    ui->tableWidget->viewport()->grabMouse();
    connect(ui->tableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(PatternSelected(int,int)));
    bool RetVal=this->exec()==QDialog::Accepted;
    if (RetVal)
    {
        QDomLiteElement* p=theNode->childElement(ui->tableWidget->currentRow());
        PatternToList(p,Pattern);
    }
    delete XML;
    return RetVal;
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
    for (int i=0;i<theNode->childCount();i++)
    {
        lw->setRowHeight(i,32);
        QDomLiteElement* p=theNode->childElement(i);
        if (lw->columnCount()-2<p->attributeCount()) lw->setColumnCount(p->attributeCount()+2);
        for (int j=0;j<p->attributeCount();j++)
        {
            QTableWidgetItem* item=new QTableWidgetItem();
            item->setIcon(QIcon(":/patterns/patterns/"+p->attribute(QString::number(j))+".png"));
            item->setSizeHint(QSize(20,32));
            lw->setItem(i,j,item);
        }
    }
    for (int i=0;i<lw->columnCount()-2;i++)
    {
        lw->setColumnWidth(i,20);
    }
    for (int i=0;i<theNode->childCount();i++)
    {
        QTableWidgetItem* item=new QTableWidgetItem();
        item->setIcon(QIcon(":/fileclose.png"));
        item->setSizeHint(QSize(24,24));
        lw->setItem(i,lw->columnCount()-1,item);
    }
    lw->setColumnWidth(lw->columnCount()-1,24);
    lw->resizeColumnToContents(lw->columnCount()-1);
    lw->horizontalHeader()->adjustSize();
    lw->show();
    int w=0;
    for (int i=0;i<lw->columnCount();i++)
    {
        w+=lw->columnWidth(i);
    }
    lw->setFixedSize(w,32*lw->rowCount());
    this->setFixedWidth(w);
    this->adjustSize();
    this->setFixedSize(this->width(),this->height());
    lw->blockSignals(false);
}

void CPatternList::PatternSelected(int row, int col)
{
    if (col==ui->tableWidget->columnCount()-1)
    {
        theNode->removeChild(row);
        XML->save(settingsfile);
        fill();
        this->setResult(0);
        this->close();
        return;
    }
    this->accept();
}

void CPatternList::PatternToList(QDomLiteElement *Pattern, QList<QPair<int, int> > &List)
{
    List.clear();
    for (int i=0;i<Pattern->attributeCount();i++)
    {
        QString s=Pattern->attribute(QString::number(i));
        QPair<int,int> p=qMakePair(s.left(1).toInt(),s.right(1).toInt());
        List.append(p);
    }
}

QDomLiteElement* CPatternList::ListToPattern(const QList<QPair<int, int> > &List)
{
    QDomLiteElement* p=new QDomLiteElement("Pattern");
    for (int i=0;i<List.count();i++)
    {
        p->appendAttribute(QString::number(i),QString::number(List.at(i).first)+QString::number(List.at(i).second));
    }
    return p;
}

