#include "caccidentalspiano.h"
#include "ui_caccidentalspiano.h"

CAccidentalsPiano::CAccidentalsPiano(QWidget *parent) :
    QCanvas(parent),
    ui(new Ui::CAccidentalsPiano)
{
    ui->setupUi(this);
    CurrentPitch=-1;

    for (int i=0;i<12;i++)
    {
        Chromatic.append(QStringList());
        Keys[i]=0;
    }

    Chromatic[0] << "C" << "B#" << "Dbb";
    Chromatic[1] << "C#" << "Bx" << "Db";
    Chromatic[2] << "D" << "Cx" << "Ebb";
    Chromatic[3] << "Eb" << "D#" << "Fbb";
    Chromatic[4] << "E" << "Dx" << "Fb";
    Chromatic[5] << "F" << "E#" << "Gbb";
    Chromatic[6] << "F#" << "Ex" << "Gb";
    Chromatic[7] << "G" << "Fx" << "Abb";
    Chromatic[8] << "Ab" << "G#" << QString();
    Chromatic[9] << "A" << "Gx" << "Bbb";
    Chromatic[10] << "Bb" << "A#" << "Cbb";
    Chromatic[11] << "B" << "Ax" << "Cb";

    this->setFixedSize(PitchToPos(12)+1,aWhiteKeyHeight+1);
}

CAccidentalsPiano::~CAccidentalsPiano()
{
    delete ui;
}

void CAccidentalsPiano::Paint()
{
    Clear();
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
    QString txt=Chromatic[Pitch][Keys[Pitch]];
    if (IsBlackKey(Pitch))
    {
        DrawBlackKey(Pos,Pressed);
        SetPenBrush(Qt::gray,Qt::lightGray);
        SetFont(F);
        Text(QPoint(Pos+((aBlackKeyWidth-fm.width(txt))/2),aBlackKeyHeight-fm.height()-4),txt);
        /*
        QGraphicsSimpleTextItem* item=Scene.addSimpleText(txt);
        item->setPos(QPoint(Pos+((aBlackKeyWidth-fm.width(txt))/2),aBlackKeyHeight-fm.height()-4));
        item->setPen(QPen(Qt::gray));
        item->setBrush(QBrush(Qt::lightGray));
        item->setFont(F);
        */
    }
    else
    {
        DrawWhiteKey(Pos,Pressed);
        SetPenBrush(Qt::darkGray,Qt::gray);
        SetFont(F);
        Text(QPoint(Pos+((aWhiteKeyWidth-fm.width(txt))/2),aBlackKeyHeight+4),txt);
        /*
        QGraphicsSimpleTextItem* item=Scene.addSimpleText(txt);
        item->setPos(QPoint(Pos+((aWhiteKeyWidth-fm.width(txt))/2),aBlackKeyHeight+4));
        item->setPen(QPen(Qt::darkGray));
        item->setBrush(QBrush(Qt::gray));
        item->setFont(F);
        */

    }
}

void CAccidentalsPiano::DrawBlackKey(const int Pos, const bool Down)
{
    if (!Down)
    {
        SetPenBrush(Qt::gray,Qt::gray);
        Rectangle(Pos+2,1,aBlackKeyWidth,aBlackKeyHeight);
        //Scene.addRect(Pos+2,1,aBlackKeyWidth,aBlackKeyHeight,QPen(Qt::gray),QBrush(Qt::gray));
    }
    SetPenBrush(Qt::black);
    Rectangle(Pos,0,aBlackKeyWidth,aBlackKeyHeight);
    //Scene.addRect(Pos,0,aBlackKeyWidth,aBlackKeyHeight,QPen(Qt::black),QBrush(Qt::black));
    QColor col=Qt::white;
    if (Down) col=Qt::darkGray;
    SetPen(col);
    Line(Pos+1,1,Pos+1,aBlackKeyHeight-2);
    //Scene.addLine(Pos+1,1,Pos+1,aBlackKeyHeight-2,QPen(col));
    Line(Pos+1,aBlackKeyHeight-2,Pos+aBlackKeyWidth-2,aBlackKeyHeight-2);
    //Scene.addLine(Pos+1,aBlackKeyHeight-2,Pos+aBlackKeyWidth-2,aBlackKeyHeight-2,QPen(col));
}

void CAccidentalsPiano::DrawWhiteKey(const int Pos, const bool Down)
{
    QColor col=Qt::white;
    if (Down) col="#eeeeee";
    SetPenBrush(col);
    Rectangle(Pos+1,0,aWhiteKeyWidth-1,aWhiteKeyHeight-1);
    //Scene.addRect(Pos+1,0,aWhiteKeyWidth-1,aWhiteKeyHeight-1,QPen(col),QBrush(col));
    SetPen("#888888");
    Line(Pos,0,Pos+aWhiteKeyWidth,0);
    //Scene.addLine(Pos,0,Pos+aWhiteKeyWidth,0,QPen("#888888"));
    if (Down)
    {
        SetPen("#dddddd");
        Line(Pos,1,Pos+aWhiteKeyWidth,1);
        //Scene.addLine(Pos,1,Pos+aWhiteKeyWidth,1,QPen("#dddddd"));
    }
    SetPen(Qt::black);
    Line(Pos,0,Pos,aWhiteKeyHeight-1);
    //Scene.addLine(Pos,0,Pos,aWhiteKeyHeight-1,QPen(Qt::black));
    Line(Pos+aWhiteKeyWidth,0,Pos+aWhiteKeyWidth,aWhiteKeyHeight-1);
    //Scene.addLine(Pos+aWhiteKeyWidth,0,Pos+aWhiteKeyWidth,aWhiteKeyHeight-1,QPen(Qt::black));
    SetPenBrush(Qt::gray);
    Rectangle(Pos+aWhiteKeyWidth-1,1,1,aWhiteKeyHeight-2);
    //Scene.addRect(Pos+aWhiteKeyWidth-1,1,1,aWhiteKeyHeight-2,QPen(Qt::gray),QBrush(Qt::gray));
    if (Down)
    {
        Rectangle(Pos+1,1,1,aWhiteKeyHeight-2);
        //Scene.addRect(Pos+1,1,1,aWhiteKeyHeight-2,QPen(Qt::gray),QBrush(Qt::gray));
    }
    SetPen(Qt::black);
    Line(Pos+1,aWhiteKeyHeight,Pos+aWhiteKeyWidth-1,aWhiteKeyHeight);
    //Scene.addLine(Pos+1,aWhiteKeyHeight,Pos+aWhiteKeyWidth-1,aWhiteKeyHeight,QPen(Qt::black));
}

const int CAccidentalsPiano::PosToPitch(const QPoint& Pos)
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

const bool CAccidentalsPiano::IsBlackKey(const int Pitch)
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

const int CAccidentalsPiano::PitchToPos(const int Pitch)
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
        Keys[Pitch]++;
        if (Pitch==8)
        {
            if (Keys[Pitch]>1) Keys[Pitch]=0;
        }
        else
        {
            if (Keys[Pitch]>2) Keys[Pitch]=0;
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
        //emit NoteOff(CurrentPitch);
        CurrentPitch=-1;
    }
    Paint();
}

void CAccidentalsPiano::resizeEvent(QResizeEvent *event)
{
    QCanvas::resizeEvent(event);
    Paint();
}
