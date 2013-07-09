#include "qcustomstatusbar.h"
#include <QDebug>

QSpacerWidget::QSpacerWidget(QWidget *parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_TransparentForMouseEvents,true);
}

QCustomStatusBar::QCustomStatusBar(QWidget *parent) :
    QStatusBar(parent)
{
    cWidget=0;
    shadowWidget=new QWidget(this);
    shadowLayout=new QGridLayout(shadowWidget);
    shadowWidget->setLayout(shadowLayout);
    shadowLayout->setMargin(0);
    shadowLayout->setSpacing(0);
    shadowLayout->addWidget(new QSpacerWidget(shadowWidget),0,0);
    shadowLayout->addWidget(new QSpacerWidget(shadowWidget),0,2);
}

void QCustomStatusBar::addCenterWidget(QWidget *w)
{
    cWidget=w;
    shadowLayout->addWidget(w,0,1);
    shadowWidget->setGeometry(rect());
    //shadowWidget->raise();
    cWidget->raise();
}

QWidget* QCustomStatusBar::centerWidget()
{
    return cWidget;
}

bool QCustomStatusBar::event(QEvent *e)
{
    if ((e->type()==QEvent::Move) || (e->type()==QEvent::Resize))
    {
        shadowWidget->setGeometry(rect());
        //shadowWidget->raise();
        if (cWidget != 0)
        {
            cWidget->raise();
        }
    }
    return QStatusBar::event(e);
}
