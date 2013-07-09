#ifndef QPICFRAME_H
#define QPICFRAME_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>

class QPicFrame : public QWidget
{
    Q_OBJECT
public:
    explicit QPicFrame(QWidget* parent=0);
    ~QPicFrame();
    void setPicture(QImage* p);
    const QImage* picture();
    void fill(const QSize size, const float smoke);
    void grabWidget(QWidget* w, const float smoke=0);
    void grabWidget(QWidget* w, const QRect sourceRect, const float smoke=0);
    void load(const QString& path, const QSize size=QSize(0,0));
    void setOpacity(const qreal o);
    const qreal opacity();
protected:
    virtual void paintEvent(QPaintEvent * event);
    QImage* pic;
private:
    qreal opc;
};

#endif // QPICFRAME_H
