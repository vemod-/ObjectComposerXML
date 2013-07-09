#ifndef QFADINGWIDGET_H
#define QFADINGWIDGET_H

#include <QTimeLine>
#include "cocoaprogressbar.h"
#include <qcanvas.h>

class QFadingWidget : public QWidget
{
    Q_OBJECT
public:
    enum TransistionType
    {
        Fade=0,
        SlideRightToLeft=1,
        SlideLeftToRight=2,
        PagingForward=3,
        PagingBack=4,
        DarkFade=5,
        FadingForward=6,
        FadingBack=7
    };

    explicit QFadingWidget(QWidget *parent = 0);
    void setWidget(QWidget* w);
    void setTransitionType(TransistionType t);
    void setEasingCurve(QEasingCurve e);
    void setDuration(int d);
signals:

public slots:
    void prepare();
    void fade();
private:
    QTimeLine* timeline;
    QCanvasLayer* oldImage;
    QCanvasLayer* newImage;
    float smoke;
    float fadefactor;
    QRectF newRect;
    QRectF oldRect;
    QWidget* Widget;
    TransistionType transitionType;
    QRectF WRect;
    QMacProgressIndicator* progressWheel;
    int progressCount;
private slots:
    void progress(int count);
    void finish();
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // QFADINGWIDGET_H
