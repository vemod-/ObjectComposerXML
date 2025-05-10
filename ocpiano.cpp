#include "ocpiano.h"
#include "ui_ocpiano.h"
#include <QApplication>

#define renderinghints QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing

OCPiano::OCPiano(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::OCPiano)
{
    ui->setupUi(this);
    setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
    setRenderHints(renderinghints);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    Scene = new QGraphicsScene(this);
    Scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(Scene);
    setMouseTracking(true);
    zoomer = new QGraphicsViewZoomer(this);
    zoomer->setMin(1);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    MD=false;
    MouseInside=false;
    this->setFixedSize(PitchToPos(127)+WhiteKeyWidth+1,WhiteKeyHeight+1);
}

OCPiano::~OCPiano()
{
    delete ui;
}

void OCPiano::Paint()
{
    //clearTransparent();
    Scene->clear();
    for (int i=1;i<128;i++)
    {
        if (!IsBlackKey(i)) DrawKey(i,Pitches.contains(i));
    }
    for (int i=1;i<128;i++)
    {
        if (IsBlackKey(i)) DrawKey(i,Pitches.contains(i));
    }

    //setPenBrush(Qt::black,Qt::darkGray);
    QPainterPath p(QPoint(PitchToPos(60)+2,WhiteKeyHeight+5));
    p.lineTo(PitchToPos(60)+2+((WhiteKeyWidth-4)/2.0),WhiteKeyHeight);
    p.lineTo(PitchToPos(60)+WhiteKeyWidth-2,WhiteKeyHeight+5);
    //drawPath(p);
    Scene->addPath(p,QPen(Qt::black),QBrush(Qt::darkGray));
    update();
}

void OCPiano::mousePressEvent(QMouseEvent *event)
{
    MD=true;
    AddKey(PosToPitch(mapToScene(event->pos()).toPoint()),event->button()==Qt::RightButton);
    Paint();
}

void OCPiano::mouseMoveEvent(QMouseEvent* event)
{
    if (MD)
    {
        if (event->button()!=Qt::RightButton)
        {
            int Pitch=PosToPitch(mapToScene(event->pos()).toPoint());
            if (!Pitches.empty())
            {
                if (Pitch != Pitches.last())
                {
                    if (event->modifiers()!=Qt::ShiftModifier) MouseUp();
                    AddKey(Pitch,event->button()==Qt::RightButton);
                    Paint();
                }
            }
            //qDebug() << "MouseMoveEvent" << Pitches;
        }
    }
}

void OCPiano::mouseReleaseEvent(QMouseEvent* event)
{
    if (!Pitches.empty())
    {
        if (event->modifiers() != Qt::ShiftModifier)
        {
            MouseUp();
            Paint();
            //qDebug() << "MouseReleaseEvent" << Pitches;
        }
    }
    MD=false;
}

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
void OCPiano::enterEvent(QEnterEvent* /*event*/)
#else
void OCPiano::enterEvent(QEvent */*event*/)
#endif
{
    grabKeyboard();
    MouseInside=true;
}

void OCPiano::leaveEvent(QEvent* /*event*/)
{
    if (Pitches.empty()) releaseKeyboard();
    MouseInside=false;
}

void OCPiano::setTooltip(const QPoint pos) {
    QString s;
    if (pos != QPoint()) {
        int pitch = PosToPitch(pos);
        s = "Pitch (" + QString::number(pitch) + ") " + CPitchTextConvert::pitch2Text(pitch) + "<br>";
    }
    this->setToolTip(s + "Click to enter a note<br>Right-click to enter a tied note<br>Hold shift to enter chords");
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

int OCPiano::PosToPitch(QPoint Pos)
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

int OCPiano::PitchToPos(const int Pitch)
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

bool OCPiano::IsBlackKey(const int Pitch)
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

void OCPiano::DrawBlackKey(const int Pos, const bool Down)
{
    if (!Down)
    {
        //setPenBrush("#888","#666");
        //drawRectangle(Pos+2,1,BlackKeyWidth,BlackKeyHeight);
        Scene->addRect(Pos+2,1,BlackKeyWidth,BlackKeyHeight,QPen("#888"),QBrush("#666"));
    }
    //setPenBrush(Qt::black);
    //drawRectangle(Pos,0,BlackKeyWidth,BlackKeyHeight);
    Scene->addRect(Pos,0,BlackKeyWidth,BlackKeyHeight,QPen(Qt::black),QBrush(Qt::black));
    QColor col("#666");
    if (Down) col=QColor("#444");
    //setPen(col);
    //drawLine(Pos+1,1,Pos+1,BlackKeyHeight-3);
    Scene->addLine(Pos+1,1,Pos+1,BlackKeyHeight-3,col);
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
    //setPen(Qt::black);
    //setBrush(lg);
    //drawPath(p);
    Scene->addPath(p,QPen(Qt::black),lg);
}

QColor OCPiano::octaveToColor(int midiPitch) {
    midiPitch = qBound(1,midiPitch,255);
    int octave = midiPitch / 12;  // Oktavindex 0–10
    if ((octave < 1) || (octave > 9)) return Qt::white;
    octave--;

    QColor color;
    const float bright = 0.6;
    const float varcol = 255 * (1 - bright);
    const float constcol = 255 * bright;
    if (octave <= 4) {
        // Interpolera från röd (255,0,0) till gul (255,255,0)
        float t = octave / 4.0f;
        int r = 255;
        int g = static_cast<int>((t * varcol) + constcol);
        int b = constcol;
        //qDebug() << "octave" << octave << t << r << g << b;
        color.setRgb(r, g, b);
    } else {
        // Interpolera från gul (255,255,0) till blå (0,0,255)
        float t = (octave - 4) / 4.0f;
        int r = static_cast<int>((varcol * (1 - t)) + constcol);
        int g = static_cast<int>((varcol * (1 - t)) + constcol);
        int b = static_cast<int>((varcol * t) + constcol);
        //qDebug() << "octave" << octave << t << r << g << b;
        color.setRgb(r, g, b);
    }
    return color;
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
        //int pitch = PosToPitch(QPoint(Pos + 5,BlackKeyHeight));
        int Octave=IntDiv(IntDiv(Pos+WhiteKeyWidth,WhiteKeyWidth),7);
        lg.setColorAt(0.8,octaveToColor(Octave * 12));
    }
    //setPen(QPen(Qt::NoPen));
    //setBrush(lg);
    //drawRectangle(Pos+1,0,WhiteKeyWidth-1,WhiteKeyHeight-1);
    Scene->addRect(Pos+1,0,WhiteKeyWidth-1,WhiteKeyHeight-1,Qt::NoPen,lg);
    //setPen("#666");
    //drawLine(Pos,0,Pos+WhiteKeyWidth,0);
    Scene->addLine(Pos,0,Pos+WhiteKeyWidth,0,QPen("#666"));
    if (Down)
    {
        //setPen("#888");
        //drawLine(Pos,1,Pos+WhiteKeyWidth,1);
        Scene->addLine(Pos,1,Pos+WhiteKeyWidth,1,QPen("#888"));
    }
    //setPen("#555");
    //drawLine(Pos,0,Pos,WhiteKeyHeight-1);
    Scene->addLine(Pos,0,Pos,WhiteKeyHeight-1,QPen("#555"));
    //setPen(Qt::black);
    //drawLine(Pos+WhiteKeyWidth,0,Pos+WhiteKeyWidth,WhiteKeyHeight-1);
    Scene->addLine(Pos+WhiteKeyWidth,0,Pos+WhiteKeyWidth,WhiteKeyHeight-1,QPen(Qt::black));
    //setPenBrush(Qt::gray);
    //drawRectangle(Pos+WhiteKeyWidth-1,1,1,WhiteKeyHeight-2);
    Scene->addRect(Pos+WhiteKeyWidth-1,1,1,WhiteKeyHeight-2,QPen(Qt::gray),QBrush(Qt::gray));
    if (Down)
    {
        //setBrush("#666");
        //drawRectangle(Pos+1,1,1,WhiteKeyHeight-2);
        Scene->addRect(Pos+1,1,1,WhiteKeyHeight-2,QPen(Qt::gray),QBrush("#666"));
    }
    //setPen("#555");
    //drawLine(Pos+1,WhiteKeyHeight,Pos+WhiteKeyWidth-1,WhiteKeyHeight);
    Scene->addLine(Pos+1,WhiteKeyHeight,Pos+WhiteKeyWidth-1,WhiteKeyHeight,QPen("#555"));
}

void OCPiano::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift)
    {
        for (int i = Pitches.size() -2; i >= 0; i--) RemoveKey(i);
        if (!Pitches.empty())
        {
            if (MD)
            {
                Notes.append(OCInputNote(Pitches.last(), 1));
            }
            else
            {
                RemoveKey(Pitches.size()-1);
            }
        }
        EmitTrigger();
        Paint();
        //qDebug() << "keyRelesaeEvent" << Pitches;
    }
    if (!MouseInside) releaseKeyboard();
    QWidget::keyReleaseEvent(event);
}

void OCPiano::MouseUp()
{
    RemoveKey(Pitches.size() -1);
    EmitTrigger();
    //qDebug() << "MouseUp" << Pitches;
}

void OCPiano::EmitTrigger()
{
    if (!Notes.empty()) emit TriggerNotes(Notes);
    Notes.clear();
    //qDebug() << "EmitTrigger" << Pitches;
}

void OCPiano::AddKey(const int Pitch, const bool Tie)
{
    if ((Pitch < 1) | (Pitch > 127)) return;
    Pitches.append(Pitch);
    Types.append(int(Tie));
    emit NoteOnTriggered(Pitch);
}

void OCPiano::RemoveKey(const int i)
{
    emit NoteOffTriggered(Pitches[i]);
    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        Notes.append(OCInputNote(Pitches[i],Types[i] + 2));
    }
    else
    {
        Notes.append(OCInputNote(Pitches[i],Types[i]+(int(Pitches.size() > 1) * 2)));
    }
    Pitches.removeAt(i);
    Types.removeAt(i);
    //qDebug() << "RemoveKey" << Pitches;
}

void OCPiano::resizeEvent(QResizeEvent *event)
{
    //QCanvas::resizeEvent(event);
    QGraphicsView::resizeEvent(event);
    Paint();
}

bool OCPiano::event(QEvent *event) {
    if (event->type() == QEvent::ToolTip) {
        setTooltip(mapToScene(mapFromGlobal(QCursor::pos())).toPoint());
    }
    return QGraphicsView::event(event);
}
