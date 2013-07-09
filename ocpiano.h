#ifndef OCPIANO_H
#define OCPIANO_H

#include "CommonClasses.h"
#include <qcanvas.h>
#include <QMouseEvent>

#define WhiteKeyHeight 60//30
#define WhiteKeyWidth 14//8
#define BlackKeyOffset 9//5
#define BlackKeyHeight 35//15
#define BlackKeyWidth 9//5

namespace Ui {
    class OCPiano;
}

class OCPiano : public QCanvas
{
    Q_OBJECT

public:
    explicit OCPiano(QWidget *parent = 0);
    ~OCPiano();
signals:
    void NoteOn(int Pitch);
    void NoteOff(int Pitch);
    void TriggerNotes(QList<QPair<int,int> > &Notes);
protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void resizeEvent(QResizeEvent *event);
private:
    Ui::OCPiano *ui;
    QList<int> Pitches;
    QList<int> Types;
    int CurrentPitch;
    void DrawKey(const int Pitch, const bool Pressed);
    const int PosToPitch(QPoint Pos);
    const int PitchToPos(const int Pitch);
    const bool IsBlackKey(const int Pitch);
    void DrawBlackKey(const int Pos, const bool Down);
    void DrawWhiteKey(const int Pos, const bool Down);
    void PressKey(const int Pitch, const bool Poly, const bool Tied);
    void Paint();
};

#endif // OCPIANO_H
