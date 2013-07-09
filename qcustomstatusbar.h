#ifndef QCUSTOMSTATUSBAR_H
#define QCUSTOMSTATUSBAR_H

#include <QStatusBar>
#include <QPaintEvent>
#include <QGridLayout>

class QSpacerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QSpacerWidget(QWidget* parent = 0);
};

class QCustomStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit QCustomStatusBar(QWidget *parent = 0);
    void addCenterWidget(QWidget* w);
    QWidget* centerWidget();
signals:

public slots:
protected:
    bool event(QEvent *e);
private:
    QWidget* cWidget;
    QWidget* shadowWidget;
    QGridLayout* shadowLayout;
};

#endif // QCUSTOMSTATUSBAR_H
