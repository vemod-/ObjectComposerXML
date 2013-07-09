#ifndef QFADINGFRAME_H
#define QFADINGFRAME_H

#include <QFrame>
#include <QTimeLine>
#include "qpicframe.h"

class QFadingFrame : public QFrame
{
    Q_OBJECT
public:
    enum FadingStates
    {
        FadingFinished=0,
        FadingIn=1,
        FadingOut=2
    };
    explicit QFadingFrame(QWidget *parent = 0);

signals:

public slots:
    void show();
    void hide();
protected:
    QPicFrame* backPic;
    QTimeLine* timeLine;
    FadingStates state;
protected slots:
    void processing(int count);
    void finished();
};

#endif // QFADINGFRAME_H
