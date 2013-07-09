#include "ocpiano.h"
#include "ui_ocpiano.h"

OCPiano::OCPiano(QWidget *parent) :
    QCanvas(parent),
    ui(new Ui::OCPiano)
{
    ui->setupUi(this);
    //setAutoFillBackground(true);
    CurrentPitch=0;
    /*
    setScene(&Scene);
    this->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    Scene.setSceneRect(0,0,PitchToPos(128)+WhiteKeyWidth+1,WhiteKeyHeight+1);
    */
    this->setFixedSize(PitchToPos(127)+WhiteKeyWidth+1,WhiteKeyHeight+1);
    /*
    setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    this->setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    */
    this->setToolTip("Click to enter a note<br>Right-click to enter a tied note<br>Shift-click to enter a chord note<br>Shift-right-click to enter a tied chord note");
    //Paint();
}

OCPiano::~OCPiano()
{
    delete ui;
}

void OCPiano::Paint()
{
    //Scene.clear();
    ClearTransparent();
    for (int i=1;i<128;i++)
    {
        if (!IsBlackKey(i)) DrawKey(i,Pitches.contains(i) || (i==CurrentPitch));
    }
    for (int i=1;i<128;i++)
    {
        if (IsBlackKey(i)) DrawKey(i,Pitches.contains(i) || (i==CurrentPitch));
    }

    SetPenBrush(Qt::black,Qt::darkGray);
    QPainterPath p(QPoint(PitchToPos(60)+2,WhiteKeyHeight+5));
    p.lineTo(PitchToPos(60)+2+((WhiteKeyWidth-4)/2),WhiteKeyHeight);
    p.lineTo(PitchToPos(60)+WhiteKeyWidth-2,WhiteKeyHeight+5);
    Path(p);

    //Scene.addPath(p,QPen(Qt::black),QBrush(Qt::darkGray));
    //this->viewport()->update();
    update();
}

void OCPiano::DrawKey(const int Pitch, const bool Pressed)
{
    int Pos=PitchToPos(Pitch);
    if (IsBlackKey(Pitch))
    {
        DrawBlackKey(Pos,Pressed);
    }
    else
    {
        DrawWhiteKey(Pos,Pressed);
    }
}

void OCPiano::DrawBlackKey(const int Pos, const bool Down)
{
    if (!Down)
    {
        SetPenBrush("#888","#666");
        //Scene.addRect(Pos+2,1,BlackKeyWidth,BlackKeyHeight,QPen("#888"),QBrush("#666"));
        Rectangle(Pos+2,1,BlackKeyWidth,BlackKeyHeight);
    }
    SetPenBrush(Qt::black);
    //Scene.addRect(Pos,0,BlackKeyWidth,BlackKeyHeight,QPen(Qt::black),QBrush(Qt::black));
    Rectangle(Pos,0,BlackKeyWidth,BlackKeyHeight);
    QColor col("#666");
    if (Down) col=QColor("#444");
    SetPen(col);
    //Scene.addLine(Pos+1,1,Pos+1,BlackKeyHeight-3,QPen(col));
    Line(Pos+1,1,Pos+1,BlackKeyHeight-3);
    QLinearGradient lg(0,0,0,BlackKeyHeight-7);
    if (Down)
    {
        lg.setColorAt(0,"#000");
        lg.setColorAt(1,"#444");
    }
    else
    {
        lg.setColorAt(0,"#000");
        lg.setColorAt(1,"#666");
    }
    QPainterPath p(QPoint(Pos+2,1));
    p.lineTo(Pos+2,BlackKeyHeight-5);
    p.cubicTo(Pos+2,BlackKeyHeight-3,Pos+BlackKeyWidth-1,BlackKeyHeight-3,Pos+BlackKeyWidth-1,BlackKeyHeight-5);
    p.lineTo(Pos+BlackKeyWidth-1,1);
    SetPen(Qt::black);
    SetBrush(lg);
    Path(p);
    //Scene.addPath(p,QPen(Qt::black),QBrush(lg));
}

void OCPiano::DrawWhiteKey(const int Pos, const bool Down)
{
    QLinearGradient lg(0,0,0,WhiteKeyHeight);
    if (Down)
    {
        lg.setColorAt(0,"#999");
        lg.setColorAt(0.6,"#ddd");
    }
    else
    {
        lg.setColorAt(0,"#ccc");
        lg.setColorAt(0.8,"#fff");
    }
    SetPen(QPen(Qt::NoPen));
    SetBrush(lg);
    Rectangle(Pos+1,0,WhiteKeyWidth-1,WhiteKeyHeight-1);
    //Scene.addRect(Pos+1,0,WhiteKeyWidth-1,WhiteKeyHeight-1,QPen(Qt::NoPen),QBrush(lg));
    SetPen("#666");
    Line(Pos,0,Pos+WhiteKeyWidth,0);
    //Scene.addLine(Pos,0,Pos+WhiteKeyWidth,0,QPen("#666"));
    if (Down)
    {
        SetPen("#888");
        Line(Pos,1,Pos+WhiteKeyWidth,1);
        //Scene.addLine(Pos,1,Pos+WhiteKeyWidth,1,QPen("#888"));
    }
    SetPen("#555");
    Line(Pos,0,Pos,WhiteKeyHeight-1);
    //Scene.addLine(Pos,0,Pos,WhiteKeyHeight-1,QPen("#555"));
    SetPen(Qt::black);
    Line(Pos+WhiteKeyWidth,0,Pos+WhiteKeyWidth,WhiteKeyHeight-1);
    //Scene.addLine(Pos+WhiteKeyWidth,0,Pos+WhiteKeyWidth,WhiteKeyHeight-1,QPen(Qt::black));
    SetPenBrush(Qt::gray);
    Rectangle(Pos+WhiteKeyWidth-1,1,1,WhiteKeyHeight-2);
    //Scene.addRect(Pos+WhiteKeyWidth-1,1,1,WhiteKeyHeight-2,QPen(Qt::gray),QBrush(Qt::gray));
    if (Down)
    {
        SetBrush("#666");
        Rectangle(Pos+1,1,1,WhiteKeyHeight-2);
        //Scene.addRect(Pos+1,1,1,WhiteKeyHeight-2,QPen(Qt::gray),QBrush("#666"));
    }
    SetPen("#555");
    Line(Pos+1,WhiteKeyHeight,Pos+WhiteKeyWidth-1,WhiteKeyHeight);
    //Scene.addLine(Pos+1,WhiteKeyHeight,Pos+WhiteKeyWidth-1,WhiteKeyHeight,QPen("#555"));
}

const int OCPiano::PosToPitch(QPoint Pos)
{
    Pos.setX(Pos.x()-5);
    int Octave=IntDiv(IntDiv(Pos.x()+WhiteKeyWidth,WhiteKeyWidth),7);
    int Key=IntDiv((Pos.x()+WhiteKeyWidth)-((Octave*7)*WhiteKeyWidth),WhiteKeyWidth);
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
    if (Pos.y() < BlackKeyHeight)
    {
        int xx=(Pos.x()+WhiteKeyWidth)-(((Octave*7)+Key)*WhiteKeyWidth);
        switch (Pitch % 12)
        {
        case 0:
        case 2:
        case 5:
        case 7:
        case 9:
            if (xx>BlackKeyOffset) Pitch++;
            break;
        }
        switch (Pitch % 12)
        {
        case 2:
        case 4:
        case 7:
        case 9:
        case 11:
            if (xx<(WhiteKeyWidth-BlackKeyOffset)+BlackKeyWidth) Pitch--;
            break;
        }
    }
    return Pitch+(Octave*12);
}

const bool OCPiano::IsBlackKey(const int Pitch)
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

const int OCPiano::PitchToPos(const int Pitch)
{
    int X=(IntDiv(Pitch, 12)*7)-1;
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
    X*=WhiteKeyWidth;
    if (IsBlackKey(Pitch)) X+=BlackKeyOffset;
    return X+5;
}

void OCPiano::mousePressEvent(QMouseEvent *event)
{
    //QPointF m(mapToScene(event->pos()));
    int Pitch=PosToPitch(event->pos());
    if ((Pitch > 0) & (Pitch < 128))
    {
        PressKey(Pitch,event->modifiers()&Qt::ShiftModifier,event->button()&Qt::RightButton);
        Paint();
    }
}

void OCPiano::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (CurrentPitch>0)
    {
        emit NoteOff(CurrentPitch);
        CurrentPitch=0;
    }
    Paint();
}

void OCPiano::PressKey(const int Pitch, const bool Poly, const bool Tied)
{
    emit NoteOn(Pitch);
    int NoteType=tsnote;
    if (Tied) NoteType=tstiednote;
    if (Poly)
    {
        Types.append(NoteType+2);
        Pitches.append(Pitch);
        CurrentPitch=0;
    }
    else
    {
        Types.append(NoteType);
        Pitches.append(Pitch);
        CurrentPitch=Pitch;
        QList<QPair<int,int> >Notes;
        for (int i=0;i<Pitches.count();i++)
        {
            Notes.append(qMakePair(Pitches[i],Types[i]));
        }
        emit TriggerNotes(Notes);
        for (int i=0;i<Pitches.count()-1;i++)
        {
            emit NoteOff(Pitches[i]);
        }
        Pitches.clear();
        Types.clear();
    }
}

void OCPiano::resizeEvent(QResizeEvent *event)
{
    QCanvas::resizeEvent(event);
    Paint();
}
