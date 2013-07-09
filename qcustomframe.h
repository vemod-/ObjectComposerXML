#ifndef QCUSTOMFRAME_H
#define QCUSTOMFRAME_H

#include <QFrame>

class QCustomFrame : public QFrame
{
    Q_OBJECT
public:
    QCustomFrame(QWidget* parent=0);
protected:
    void paintEvent(QPaintEvent *e);
};

#endif // QCUSTOMFRAME_H
