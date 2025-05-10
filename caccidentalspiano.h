#ifndef CACCIDENTALSPIANO_H
#define CACCIDENTALSPIANO_H

#include <qcanvas.h>
#include <QMouseEvent>

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
    explicit CAccidentalsPiano(QWidget *parent = nullptr);
    ~CAccidentalsPiano();
    std::array<QStringList,12> Chromatic;
    std::array<int,12> Keys={{0}};
    void Paint();
protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void resizeEvent(QResizeEvent *event);
private:
    Ui::CAccidentalsPiano *ui;
    int CurrentPitch;
    void DrawKey(const int Pitch, const bool Pressed);
    int PosToPitch(const QPoint& Pos);
    int PitchToPos(const int Pitch);
    bool IsBlackKey(const int Pitch);
    void DrawBlackKey(const int Pos, const bool Down);
    void DrawWhiteKey(const int Pos, const bool Down);
};

#endif // CACCIDENTALSPIANO_H
