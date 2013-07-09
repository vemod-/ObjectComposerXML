#ifndef CACCIDENTALSPIANO_H
#define CACCIDENTALSPIANO_H

#include <qcanvas.h>
#include <QMouseEvent>
#include "CommonClasses.h"

#define aWhiteKeyHeight 150//30
#define aWhiteKeyWidth 36//8
#define aBlackKeyOffset 24//5
#define aBlackKeyHeight 90//15
#define aBlackKeyWidth 24//5

namespace Ui {
    class CAccidentalsPiano;
}

class CAccidentalsPiano : public QCanvas
{
    Q_OBJECT

public:
    explicit CAccidentalsPiano(QWidget *parent = 0);
    ~CAccidentalsPiano();
    QList<QStringList> Chromatic;
    int Keys[12];
    void Paint();
protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void resizeEvent(QResizeEvent *event);
private:
    Ui::CAccidentalsPiano *ui;
    int CurrentPitch;
    void DrawKey(const int Pitch, const bool Pressed);
    const int PosToPitch(const QPoint& Pos);
    const int PitchToPos(const int Pitch);
    const bool IsBlackKey(const int Pitch);
    void DrawBlackKey(const int Pos, const bool Down);
    void DrawWhiteKey(const int Pos, const bool Down);
};

#endif // CACCIDENTALSPIANO_H
