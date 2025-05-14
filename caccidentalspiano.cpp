#include "caccidentalspiano.h"
#include "ui_caccidentalspiano.h"
#include "ocpiano.h"
//#include "CommonClasses.h"

CAccidentalsPiano::CAccidentalsPiano(QWidget *parent) :
    QCanvas(parent),
    ui(new Ui::CAccidentalsPiano)
{
    ui->setupUi(this);
    CurrentPitch=-1;

    Chromatic[0] = QStringList({"C", "B#", "Dbb"});
    Chromatic[1] = QStringList({"C#", "Bx", "Db"});
    Chromatic[2] = QStringList({"D", "Cx", "Ebb"});
    Chromatic[3] = QStringList({"Eb", "D#", "Fbb"});
    Chromatic[4] = QStringList({"E", "Dx", "Fb"});
    Chromatic[5] = QStringList({"F", "E#", "Gbb"});
    Chromatic[6] = QStringList({"F#", "Ex", "Gb"});
    Chromatic[7] = QStringList({"G", "Fx", "Abb"});
    Chromatic[8] = QStringList({"Ab", "G#", QString()});
    Chromatic[9] = QStringList({"A", "Gx", "Bbb"});
    Chromatic[10] = QStringList({"Bb", "A#", "Cbb"});
    Chromatic[11] = QStringList({"B", "Ax", "Cb"});

    this->setFixedSize(PitchToPos(12)+1,aWhiteKeyHeight+1);
}

CAccidentalsPiano::~CAccidentalsPiano()
{
    delete ui;
}

void CAccidentalsPiano::Paint()
{
    clear();
    for (int i=0;i<12;i++)
    {
        if (!IsBlackKey(i)) DrawKey(i,i==CurrentPitch);
    }
    for (int i=0;i<12;i++)
    {
        if (IsBlackKey(i)) DrawKey(i,i==CurrentPitch);
    }
    update();
}

void CAccidentalsPiano::DrawKey(const int Pitch, const bool Pressed)
{
    int Pos=PitchToPos(Pitch);
    QFont F(font());
    F.setPointSizeF(16);
    QFontMetrics fm(F);
    QString txt = Chromatic[uint(Pitch)][Keys[uint(Pitch)]];
    if (IsBlackKey(Pitch))
    {
        DrawBlackKey(Pos,Pressed);
        setPenBrush(Qt::gray,Qt::lightGray);
        setLayerFont(F);
        drawText(QPoint(Pos+((aBlackKeyWidth-fm.horizontalAdvance(txt))/2),aBlackKeyHeight-fm.height()-4),txt);
    }
    else
    {
        DrawWhiteKey(Pos,Pressed);
        setPenBrush(Qt::darkGray,Qt::gray);
        setLayerFont(F);
        drawText(QPoint(Pos+((aWhiteKeyWidth-fm.horizontalAdvance(txt))/2),aBlackKeyHeight+4),txt);
    }
}

void CAccidentalsPiano::DrawBlackKey(const int Pos, const bool Down)
{
    if (!Down)
    {
        setPenBrush(Qt::gray,Qt::gray);
        drawRectangle(Pos+2,1,aBlackKeyWidth,aBlackKeyHeight);
    }
    setPenBrush(Qt::black);
    drawRectangle(Pos,0,aBlackKeyWidth,aBlackKeyHeight);
    QColor col=Qt::white;
    if (Down) col=Qt::darkGray;
    setPen(col);
    drawLine(Pos+1,1,Pos+1,aBlackKeyHeight-2);
    drawLine(Pos+1,aBlackKeyHeight-2,Pos+aBlackKeyWidth-2,aBlackKeyHeight-2);
}

void CAccidentalsPiano::DrawWhiteKey(const int Pos, const bool Down)
{
    QColor col=Qt::white;
    if (Down) col="#eeeeee";
    setPenBrush(col);
    drawRectangle(Pos+1,0,aWhiteKeyWidth-1,aWhiteKeyHeight-1);
    setPen("#888888");
    drawLine(Pos,0,Pos+aWhiteKeyWidth,0);
    if (Down)
    {
        setPen("#dddddd");
        drawLine(Pos,1,Pos+aWhiteKeyWidth,1);
    }
    setPen(Qt::black);
    drawLine(Pos,0,Pos,aWhiteKeyHeight-1);
    drawLine(Pos+aWhiteKeyWidth,0,Pos+aWhiteKeyWidth,aWhiteKeyHeight-1);
    setPenBrush(Qt::gray);
    drawRectangle(Pos+aWhiteKeyWidth-1,1,1,aWhiteKeyHeight-2);
    if (Down)
    {
        drawRectangle(Pos+1,1,1,aWhiteKeyHeight-2);
    }
    setPen(Qt::black);
    drawLine(Pos+1,aWhiteKeyHeight,Pos+aWhiteKeyWidth-1,aWhiteKeyHeight);
}

int CAccidentalsPiano::PosToPitch(const QPoint& Pos)
{
    int Octave=IntDiv(IntDiv(Pos.x(),aWhiteKeyWidth),7);
    int Key=IntDiv((Pos.x())-((Octave*7)*aWhiteKeyWidth),aWhiteKeyWidth);
    int Pitch=0;
    switch (Key)
    {
    case 1:
        Pitch=2;
        break;
    case 2:
        Pitch=4;
        break;
    case 3:
        Pitch=5;
        break;
    case 4:
        Pitch=7;
        break;
    case 5:
        Pitch=9;
        break;
    case 6:
        Pitch=11;
        break;
    }
    if (Pos.y() < aBlackKeyHeight)
    {
        int xx=(Pos.x())-(((Octave*7)+Key)*aWhiteKeyWidth);
        switch (Pitch % 12)
        {
        case 0:
        case 2:
        case 5:
        case 7:
        case 9:
            if (xx>aBlackKeyOffset) Pitch++;
            break;
        }
        switch (Pitch % 12)
        {
        case 2:
        case 4:
        case 7:
        case 9:
        case 11:
            if (xx<(aWhiteKeyWidth-aBlackKeyOffset)+aBlackKeyWidth) Pitch--;
            break;
        }
    }
    return Pitch+(Octave*12);
}

bool CAccidentalsPiano::IsBlackKey(const int Pitch)
{
    switch (Pitch % 12)
    {
    case 1:
    case 3:
    case 6:
    case 8:
    case 10:
        return true;
    }
    return false;
}

int CAccidentalsPiano::PitchToPos(const int Pitch)
{
    int X=(IntDiv(Pitch, 12)*7);
    switch (Pitch % 12)
    {
    case 2:
    case 3:
        X+=1;
        break;
    case 4:
        X+=2;
        break;
    case 5:
    case 6:
        X+=3;
        break;
    case 7:
    case 8:
        X+=4;
        break;
    case 9:
    case 10:
       X+=5;
       break;
    case 11:
        X+=6;
        break;
    }
    X*=aWhiteKeyWidth;
    if (IsBlackKey(Pitch)) X+=aBlackKeyOffset;
    return X;
}

void CAccidentalsPiano::mousePressEvent(QMouseEvent *event)
{
    int Pitch=PosToPitch(event->pos());
    if (Pitch >= 0)
    {
        const auto p=uint(Pitch);
        Keys[p]++;
        if (p==8)
        {
            if (Keys[p]>1) Keys[p]=0;
        }
        else
        {
            if (Keys[p]>2) Keys[p]=0;
        }
        CurrentPitch=Pitch;
        Paint();
    }
}

void CAccidentalsPiano::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (CurrentPitch>=0)
    {
        CurrentPitch=-1;
    }
    Paint();
}

void CAccidentalsPiano::resizeEvent(QResizeEvent *event)
{
    QCanvas::resizeEvent(event);
    Paint();
}
