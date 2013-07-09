#ifndef QMACRUBBERBAND_H
#define QMACRUBBERBAND_H

#include <QRubberBand>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QTimeLine>

class QMacRubberband : public QRubberBand
{
    Q_OBJECT
public:
    enum MacRubberbandTypes
    {
        MacRubberbandYellow=0,
        MacRubberbandHaircross=1
    };
    QMacRubberband(QRubberBand::Shape shape,MacRubberbandTypes Type,QWidget* parent=0);
    void showAnimated();
    void hideAnimated();
protected:
    virtual void paintEvent(QPaintEvent *event);
private:
    QPropertyAnimation* animation;
    MacRubberbandTypes m_type;
};

class QHoverRubberband : public QRubberBand
{
    Q_OBJECT
public:
    QHoverRubberband(QRubberBand::Shape shape,QWidget* parent=0);
    void show(int Opacity=40);
    //void show();
    void hide();
protected:
    virtual void paintEvent(QPaintEvent *event);
private:
    QTimeLine* animation;
    float m_Opacity;
    int m_DefaultOpacity;
    bool m_Hiding;
private slots:
    void TimeStep(int Step);
    void TimeFinished();
};

#endif // QMACRUBBERBAND_H
