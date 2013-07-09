#ifndef COCOAPROGRESSBAR_H
#define COCOAPROGRESSBAR_H

#include <QPaintEvent>
#include <qpicframe.h>

class QMacProgressIndicator : public QPicFrame
{
    Q_OBJECT
public:
    QMacProgressIndicator(QWidget* parent, QSize Size=QSize(200,200));
    QMacProgressIndicator(QWidget* parent, QString path, QSize Size=QSize(0,0));
private:
    void init();
protected:
    void paintEvent(QPaintEvent *e);
};

#endif // COCOAPROGRESSBAR_H
