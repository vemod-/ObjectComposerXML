#ifndef QIPHOTORUBBERBAND_H
#define QIPHOTORUBBERBAND_H

#include <QWidget>

class QiPhotoRubberband : public QWidget
{
    Q_OBJECT
public:
    explicit QiPhotoRubberband(QWidget *parent = 0);
    void setWindowGeometry(QRect g);
    QRect windowGeometry();
signals:

public slots:
protected:
    void paintEvent(QPaintEvent *e);
private:
    QRect wRect;
};

#endif // QIPHOTORUBBERBAND_H
