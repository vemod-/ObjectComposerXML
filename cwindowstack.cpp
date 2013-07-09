#include "cwindowstack.h"
#include "ui_cwindowstack.h"
#include <QVBoxLayout>
#include <QCloseEvent>

CWindowStack::CWindowStack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWindowStack)
{
    ui->setupUi(this);
    activeIndex=-1;
    QVBoxLayout* layout=new QVBoxLayout(this);
    layout->setMargin(0);
    this->setLayout(layout);
}

CWindowStack::~CWindowStack()
{
    delete ui;
}

QWidget* CWindowStack::addSubWindow(QWidget *sw)
{
    subWindowList.append(sw);
    this->layout()->addWidget(sw);
    sw->setVisible(false);
    sw->disconnect();
    sw->setEnabled(false);
    setActiveSubWindow(sw);
    return sw;
}

QWidget* CWindowStack::currentSubWindow()
{
    return activeSubWindow();
}

QWidget* CWindowStack::activeSubWindow()
{
    if (activeIndex>-1) return subWindowList[activeIndex];
    return 0;
}

QWidget* CWindowStack::setActiveSubWindow(QWidget *sw)
{
    int index=subWindowList.indexOf(sw);
    if (index>-1)
    {
        activeIndex=index;
        for (int i=0;i<subWindowList.count();i++)
        {
            subWindowList[i]->disconnect();
            if (i != index)
            {
                subWindowList[i]->setVisible(false);
                subWindowList[i]->blockSignals(true);
                subWindowList[i]->setEnabled(false);
            }
        }
        sw->setVisible(true);
        sw->blockSignals(false);
        sw->setEnabled(true);
        emit subWindowActivated(sw);
    }
    return sw;
}

void CWindowStack::activatePreviousSubWindow()
{
    if (subWindowList.count()>1)
    {
        int index=activeIndex-1;
        if (index<0) index=subWindowList.count()-1;
        setActiveSubWindow(subWindowList[index]);
    }
}

void CWindowStack::activateNextSubWindow()
{
    if (subWindowList.count()>1)
    {
        int index=activeIndex+1;
        if (index>=subWindowList.count()) index=0;
        setActiveSubWindow(subWindowList[index]);
    }
}

void CWindowStack::removeSubWindow(QWidget *sw)
{
    int index=subWindowList.indexOf(sw);
    if (index>-1)
    {
        sw->disconnect();
        subWindowList.removeAt(index);
        delete sw;
        activeIndex=-1;
    }
    if (subWindowList.count()) setActiveSubWindow(subWindowList.first());
    emit subWindowActivated(0);
}

QWidget* CWindowStack::closeActiveSubWindow()
{
    closeSubWindow(activeSubWindow());
}

QWidget* CWindowStack::closeSubWindow(QWidget *sw)
{
    if (subWindowList.count()<2) return sw;
    int index=subWindowList.indexOf(sw);
    if (index>-1)
    {
        if (!sw->close()) return sw;
        sw->disconnect();
        subWindowList.removeAt(index);
        delete sw;
        activeIndex=-1;
        if (subWindowList.count()) return setActiveSubWindow(subWindowList.first());
        emit subWindowActivated(0);
        return 0;
    }
    return sw;
}
