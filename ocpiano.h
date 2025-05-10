#ifndef OCPIANO_H
#define OCPIANO_H

//#include "qcanvas.h"
#include "../QGraphicsViewZoomer/qgraphicsviewzoomer.h"
#include <QMouseEvent>
#include "cpitchtextconvert.h"
#include <QPainterPath>

#ifndef COMMONCLASSES_H
inline int IntDiv(const int a, const int b)
{
    return a/b;
}
#endif

class OCInputNote
{
public:
    inline OCInputNote(const int p,const int t)
    {
        Pitch=p;
        Type=t;
    }
    int Pitch;
    int Type;
};

typedef QList<OCInputNote> OCInputNoteList;

#define WhiteKeyHeight 60//30
#define WhiteKeyWidth 14//8
#define BlackKeyOffset 9//5
#define BlackKeyHeight 35//15
#define BlackKeyWidth 9//5

namespace Ui {
    class OCPiano;
}

class OCPiano : public QGraphicsView
{
    Q_OBJECT

public:
    explicit OCPiano(QWidget *parent = nullptr);
    ~OCPiano();
signals:
    void NoteOnTriggered(int Pitch);
    void NoteOffTriggered(int Pitch);
    void TriggerNotes(OCInputNoteList &Notes);
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void resizeEvent(QResizeEvent *event);
    bool event(QEvent* event);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    void enterEvent(QEnterEvent *event);
#else
    void enterEvent(QEvent *event);
#endif
    void leaveEvent(QEvent* event);

private:
    Ui::OCPiano *ui;
    QVector<int> Pitches;
    QVector<int> Types;
    OCInputNoteList Notes;
    QGraphicsViewZoomer* zoomer;
    QGraphicsScene* Scene;
    bool MD;
    bool MouseInside;
    void setTooltip(const QPoint pos = QPoint());
    void DrawKey(const int Pitch, const bool Pressed);
    int PosToPitch(QPoint Pos);
    int PitchToPos(const int Pitch);
    bool IsBlackKey(const int Pitch);
    void DrawBlackKey(const int Pos, const bool Down);
    QColor octaveToColor(int midiPitch);
    void DrawWhiteKey(const int Pos, const bool Down);
    void RemoveKey(const int i);
    void AddKey(const int Pitch, const bool Tie=false);
    void MouseUp();
    void EmitTrigger();
    void Paint();
};

#endif // OCPIANO_H
