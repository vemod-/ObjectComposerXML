#ifndef QCUSTOMFRAME_H
#define QCUSTOMFRAME_H

#include <QFrame>

class QCustomFrame : public QFrame
{
    Q_OBJECT
public:
    QCustomFrame(QWidget* parent=nullptr);
protected:
    void paintEvent(QPaintEvent *e);
};

#endif // QCUSTOMFRAME_H
