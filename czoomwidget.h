#ifndef CZOOMWIDGET_H
#define CZOOMWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QToolButton>
#include <QSlider>
#include <QToolBar>
#include "EffectLabel.h"
//#include <QFrame>

class CToolBar : public QToolBar {
    Q_OBJECT
public:
    CToolBar(QWidget* parent = nullptr)
        : QToolBar(parent) {
        setFixedHeight(52);
        setIconSize(QSize(32,32));
        setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        setMovable(false);
        setFloatable(false);
        setAllowedAreas(Qt::NoToolBarArea);
    }
};

class CStatusBar : public QFrame {
    Q_OBJECT
public:
    CStatusBar(QWidget* parent = nullptr)
        : QFrame(parent) {
        setObjectName("StatusBar");
        setFixedHeight(52);
        slo=new QHBoxLayout(this);
        slo->setSpacing(0);
        slo->setContentsMargins(0,0,0,0);
        setLayout(slo);
    }
    void addWidget(QWidget* w, int stretch, Qt::Alignment alignment) {
        slo->addWidget(w, stretch, alignment);
    }
    void addSpacing(int size) {
        slo->addSpacing(size);
    }
private:
    QHBoxLayout* slo;
};

class CZoomWidget : public QWidget {
    Q_OBJECT
public:
    CZoomWidget(QWidget* parent = nullptr)
        : QWidget(parent) {
        //QWidget* zoomer=new QWidget(this);
        QGridLayout* l=new QGridLayout(this);
        l->setContentsMargins(2,2,2,2);
        l->setHorizontalSpacing(2);
        l->setVerticalSpacing(8);
        setLayout(l);
        setFixedWidth(168);

        QWidget* spc=new QWidget(this);
        spc->setFixedHeight(8);
        l->addWidget(spc,0,0,1,3);

        btnZoomOut=new QToolButton(this);
        btnZoomOut->setFixedSize(20,20);
        btnZoomOut->setIconSize(QSize(20,20));
        btnZoomOut->setProperty("transparent",true);
        btnZoomOut->setIcon(QIcon(":/mini/mini/zoom_out.png"));
        l->addWidget(btnZoomOut,1,0);

        ZoomSlider=new QSlider(this);
        ZoomSlider->setOrientation(Qt::Horizontal);
        ZoomSlider->setFixedWidth(120);
        ZoomSlider->setRange(5,200);
        l->addWidget(ZoomSlider,1,1);

        btnZoomIn=new QToolButton(this);
        btnZoomIn->setFixedSize(20,20);
        btnZoomIn->setIconSize(QSize(20,20));
        btnZoomIn->setProperty("transparent",true);
        btnZoomIn->setIcon(QIcon(":/mini/mini/zoom_in.png"));
        l->addWidget(btnZoomIn,1,2);

        EffectLabel* btnzoomtxt=new EffectLabel(this);
        //QLabel* btnzoomtxt=new QLabel(this);
        btnzoomtxt->setText("Zoom");
        btnzoomtxt->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        btnzoomtxt->setTextColor("#111111");
        btnzoomtxt->setShadowColor(Qt::white);
        btnzoomtxt->setEffect(EffectLabel::Plain);
        QFont fnt=btnzoomtxt->font();
        fnt.setPointSize(9);
        btnzoomtxt->setFont(fnt);
        btnzoomtxt->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        l->addWidget(btnzoomtxt,2,0,1,3);

        connect(ZoomSlider,&QAbstractSlider::valueChanged,this,&CZoomWidget::SliderChange);
        connect(btnZoomOut,&QAbstractButton::clicked,this,&CZoomWidget::ZoomOut);
        connect(btnZoomIn,&QAbstractButton::clicked,this,&CZoomWidget::ZoomIn);

    }
public slots:
    void setValue(double v) {
        ZoomSlider->blockSignals(true);
        ZoomSlider->setValue(v * 50.0);
        ZoomSlider->setToolTip("Zoom "+QString::number(v*100.0)+"%");
        ZoomSlider->blockSignals(false);
    }
signals:
    void valueChanged(double v);
private:
    QToolButton* btnZoomOut;
    QToolButton* btnZoomIn;
    QSlider* ZoomSlider;
private slots:
    void SliderChange(int v) {
        ZoomSlider->setToolTip("Zoom "+QString::number(v * 2.0)+"%");
        emit valueChanged(v * 0.02);
    }
    void ZoomIn() {
        ZoomSlider->setValue(ZoomSlider->maximum());
    }
    void ZoomOut() {
        ZoomSlider->setValue(ZoomSlider->minimum());
    }
};

#endif // CZOOMWIDGET_H
