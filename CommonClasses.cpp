#include "CommonClasses.h"
#include <QPaintEvent>
#include <QDebug>
#include <QFontDialog>

const QString MakeUnicode(const QString& txt)
{
    QString b;
    foreach (QChar chr, txt)
    {
        if (chr.unicode()>0xF000)
        {
            b+=chr;
        }
        else
        {
            b+=QChar(chr.unicode()+0xF000);
        }
    }
    return b;
}

const int Sgn(const int Num)
{
    if (Num==0) return 0;
    if (Num<0) return -1;
    return 1;
};

const int Abs(const int Num)
{
    if (Num<0) return -Num;
    return Num;
}

const int Sgn(const float Num)
{
    if (Num==0) return 0;
    if (Num<0) return -1;
    return 1;
}

const float Abs(const float Num)
{
    if (Num<0) return -Num;
    return Num;
}

const double Abs(const double Num)
{
    if (Num<0) return -Num;
    return Num;
}

const int IntDiv(const int a, const int b)
{
    return (a/b);
}

const float FloatDiv(const float a, const float b)
{
    return a/b;
}
/*
double GetVal(QDomLiteElement* XML, QString Attr)
{
    return XML->attributeValue(Attr);
}
*/
/*
void SetAttribute(QDomLiteElement* XML, QString Name, QVariant Value)
{
    XML->setAttribute(Name,Value);
}
*/
/*
float FnSize(float c, QDomLiteElement *Ev)
{
    return FnSize(c,SymbolSize(Ev));
}
*/
const float SizeFactor(const int SymbolSize)
{
    if (SymbolSize == 0) return 1;
    return FloatDiv(10-SymbolSize,10);
}

const int Inside(int num, int Low, int High, int Step)
{
    if (Step < 1) Step = 1;
    if (High < Low) qSwap(High,Low);
    while (num>High) num-=Step;
    while (num<Low) num+=Step;
    return num;
}

//--------------------------------------------------------------------------

OCSignType::OCSignType()
{
    Reset();
}

OCSignType::OCSignType(XMLSymbolWrapper& Symbol)
{
    Pos=Symbol.pos();
    Size=Symbol.size();
}

void OCSignType::Reset()
{
    Pos=QPointF();
    Size=0;
    val=0;
}

OCDurSignType::OCDurSignType()
{
    Reset();
}

OCDurSignType::OCDurSignType(XMLSymbolWrapper& Symbol, const bool ActualTicks) : OCSignType(Symbol)
{
    Ticks=Symbol.ticks();
    if (!ActualTicks) Ticks++;
    RemainingTicks=Ticks;
}

void OCDurSignType::Decrem(int Value)
{
    RemainingTicks-=Value;
    if (RemainingTicks < 0) RemainingTicks=0;
}

void OCDurSignType::Reset()
{
    OCSignType::Reset();
    Ticks=0;
    RemainingTicks=0;
}

OCToolButtonProps::OCToolButtonProps(const QString& ClassName, const int Button)
{
    classname=ClassName;
    buttonindex=Button;
    iconpath.clear();
    ismodifier=false;
    ishidden=false;
}

OCCursor::OCCursor()
{
    m_Pos=0;
}

const int OCCursor::SelEnd() const
{
    if (Selected.count())
    {
        return Selected.last();
    }
    return m_Pos;
}

const int OCCursor::SelStart() const
{
    if (Selected.count())
    {
        return Selected.first();
    }
    return m_Pos;
}

void OCCursor::AddSel(const int SymbolsIndex)
{
    if (!Selected.contains(SymbolsIndex))
    {
        Selected.append(SymbolsIndex);
        qSort(Selected);
    }
    m_Pos = SymbolsIndex;
}

void OCCursor::ExtendSel(const int SymbolsIndex)
{
    AddSel(m_Pos);
    if (SymbolsIndex > SelEnd())
    {
        int TempPointer = SelEnd() + 1;
        while (TempPointer <= SymbolsIndex)
        {
            AddSel(TempPointer);
            TempPointer++;
        }
    }
    else if (SymbolsIndex < SelStart())
    {
        int TempPointer = SelStart() - 1;
        while (TempPointer >= SymbolsIndex)
        {
            AddSel(TempPointer);
            TempPointer--;
        }
    }
    qSort(Selected);
}

const int OCCursor::SelCount() const
{
    return this->Selected.count();
}

const int OCCursor::Sel(const int Index) const
{
    if (Index >= Selected.count()) return -1;
    return Selected[Index];
}

const bool OCCursor::IsSelected(const int SymbolsIndex) const
{
    return Selected.contains(SymbolsIndex);
}

const bool OCCursor::IsMarked(const int SymbolsIndex) const
{
    if (Selected.contains(SymbolsIndex))
    {
        return true;
    }
    else if (m_Pos==SymbolsIndex)
    {
        return true;
    }
    return false;
}

void OCCursor::SetPos(const int NewPos)
{
    m_Pos = NewPos;
    Selected.clear();
}

void OCCursor::SetZero()
{
    SetPos(m_Pos);
}

const int OCCursor::GetPos() const
{
    return m_Pos;
}

void OCCursor::MaxSel(const int MaxPointer)
{
    qSort(Selected);
    foreach(int Pnt,Selected)
    {
        if (Pnt>=MaxPointer) Selected.removeOne(Pnt);
    }
    if (Selected.count())
    {
        if (m_Pos > MaxPointer-1) m_Pos=MaxPointer-1;
    }
    else
    {
        if (m_Pos > MaxPointer) SetPos(MaxPointer);
    }
}

const QList<int> OCCursor::Pointers() const
{
    QList<int> l(Selected);
    if (l.count()==0) l.append(m_Pos);
    qSort(l);
    return l;
}

//--------------------------------------------------------------------------

void CTextElement::Save(QDomLiteElement* data)
{
    data->setAttribute("Text", Text);
    data->setAttribute("FontName", FontName);
    data->setAttribute("Italic", Italic);
    data->setAttribute("Bold", Bold);
    data->setAttribute("Size", Size);
    //data->setAttribute("SizeFactor", (int)((Size - (int)(Size)) * 100));
    //data->setAttribute("ID", ID);
}

void CTextElement::Load(QDomLiteElement* data)
{
    Text = data->attribute("Text");
    FontName = data->attribute("FontName");
    Italic = data->attributeValue("Italic");
    Bold = data->attributeValue("Bold");
    Size = data->attributeValue("Size");
    //Size+=data->attributeValue("SizeFactor") / 100;
    //ID = data->attributeValue("ID");
}

void CTextElement::Load(XMLTextWrapper &XMLText)
{
    Load(XMLText.getXML());
}

XMLTextWrapper CTextElement::Save(const QString& Tag)
{
    QDomLiteElement* e=new QDomLiteElement(Tag);
    Save(e);
    return XMLTextWrapper(e);
}

CTextElement::CTextElement()
{
    FontName = "Times New Roman";
    Italic = false;
    Bold = false;
    Size = 10;
}

const QFont CTextElement::Font() const
{
    QFont F(FontName,Size);
    F.setBold(Bold);
    F.setItalic(Italic);
    return F;
}

void CTextElement::SetFont(const QFont& Font)
{
    FontName=Font.family();
    Size=Font.pointSizeF();
    Italic=Font.italic();
    Bold=Font.bold();
}

void CTextElement::SetFonts(QWidget* parent)
{
    SetFont(QFontDialog::getFont(0, Font(), parent));
}

//--------------------------------------------------------------------------

const QFont OCDraw::GetFont(const CTextElement* TextElem, const int viewsize)
{
    QFont F(TextElem->FontName);
    F.setBold(TextElem->Bold);
    F.setItalic(TextElem->Italic);
    F.setPointSizeF(FloatDiv(TextElem->Size * 10, viewsize));
    return F;
}

void OCDraw::DI()
{
    ZeroPoint=MovingPoint*XFactor;
}

const QList<QGraphicsItem*> OCDraw::DJ(const float xs, const float ys, const bool LineThickness)
{
    QPointF temp=MovingPoint/ScreenSize;
    MovingPoint+=QPointF(xs,-ys);
    return MyLine(temp, MovingPoint / ScreenSize, col, false, LineThickness);
}

void OCDraw::DM(const QPointF Pos)
{
    DM(Pos.x(),Pos.y());
}
/*
void OCDraw::DM(QPointF Pos, QDomLiteElement *Symbol)
{
    DM(Pos+GetPos(Symbol));
}
*/
void OCDraw::DM(const float xs, const float ys)
{
    MovingPoint=ZeroPoint+QPointF(xs,-ys);
}
/*
void OCDraw::DM(float xs, float ys, QDomLiteElement *Symbol)
{
    DM(xs+GetVal(Symbol,"Left"), ys+GetVal(Symbol,"Top"));
}
*/
void OCDraw::DM(const float xs, const float ys, const XMLSymbolWrapper& Symbol)
{
    DM(xs+Symbol.getVal("Left"), ys+Symbol.getVal("Top"));
}

void OCDraw::DR(const float xs, const float ys, const int Size)
{
    if (Size != 0)
    {
        MovingPoint+=QPointF(xs,-ys)/SizeFactor(Size);
    }
    else
    {
        MovingPoint+=QPointF(xs,-ys);
    }
}

const QList<QGraphicsItem*> OCDraw::DL(const float a, const float b)
{
    return DJ(a,b,true);
}

const QList<QGraphicsItem*> OCDraw::plDot(const float offsetX,const float offsetY,const int Size)
{
    QList<QGraphicsItem*> l;
    DR(offsetX-36,offsetY,Size);
    l.append(plLet(".",Size,"Courier",false,false,100));
    DR(-(offsetX-36),-(offsetY),Size);
    return l;
}

const QPainterPath OCDraw::TextPath(const QString& Letter, const int SignSize, const QString& Name, const bool Bold, const bool Italic, const int FontSize)
{
    if (Letter.isEmpty()) return QPainterPath();
    QString a = Letter;
    QFont F(Name);
    F.setBold(Bold);
    F.setItalic(Italic);
    F.setPointSizeF(FloatDiv(FontSize,SizeFactor(SignSize))/ScreenSize);
#ifdef __MACOSX_CORE__
    QPointF Pos=(MovingPoint+QPointF(0,FloatDiv(FontSize,8)))/ScreenSize;
    F.setPointSizeF(F.pointSizeF()*1.30);
#else
    int x1=FloatDiv(MovingPoint.x() - 0.1, sizx);
    int y1=FloatDiv(MovingPoint.y() - (Size + FloatDiv(Size ,6)), sizy);
#endif
    QPainterPath path;
    path.addText(Pos,F,a);
    path.setFillRule(Qt::WindingFill);
    return path;
}

const QList<QGraphicsItem*> OCDraw::plLet(const QString& Letter, const int SignSize, const QFont& Font, const Qt::Alignment Align)
{
    return plLet(Letter, SignSize, Font.family(), Font.bold(), Font.italic(), Font.pointSizeF(), Align);
}

const QList<QGraphicsItem*> OCDraw::plLet(const QString& Letter, const int SignSize, const QString& Name, const bool Bold, const bool Italic, const int FontSize, const Qt::Alignment Align)
{
    if (Letter.isEmpty()) return QList<QGraphicsItem*>();
    QPainterPath p=TextPath(Letter,SignSize,Name,Bold,Italic,FontSize);
    if (Align & Qt::AlignBottom) p.translate(0,-FloatDiv(FontSize,8)/ScreenSize);
    if (Align & Qt::AlignRight) p.translate(-p.boundingRect().width(),0);
    if (Align & Qt::AlignHCenter) p.translate(-p.boundingRect().width()/2,0);
    if (Align & Qt::AlignVCenter) p.translate(0,(FloatDiv(MovingPoint.y(),ScreenSize)-p.boundingRect().top())-p.boundingRect().height()/2);
    return plTextPath(p);
}

void OCDraw::translatePath(QPainterPath &p)
{
    p.translate(MovingPoint/ScreenSize);
}

const QList<QGraphicsItem*> OCDraw::plTextPath(const QPainterPath& path, const bool UsePen, const float PenWidth)
{
    QList<QGraphicsItem*> l;
    QBrush b=QBrush(col);
    QPen p(Qt::NoPen);
    if (UsePen)
    {
        if (PenWidth>-1)
        {
            b=QBrush(Qt::NoBrush);
            p=QPen(QBrush(col),PenWidth);
            p.setCapStyle(Qt::RoundCap);
        }
        else
        {
            p=QPen(QBrush(col),1);
        }
    }
    else
    {
        if (canColor())
        {
            if ((col==selectedcolor) || (col==markedcolor)) p=QPen(col);
        }
    }
    QGraphicsItem* i=Scene->addPath(path,p,b);
    l.append(i);
    AppendToList(i);
    return l;
}

void OCDraw::PlSquare1(const int h, const int ah, const int t)
{
    float height=(float)h/ScreenSize;
    float archeight=(float)ah/ScreenSize;
    float thickness=t/ScreenSize;
    float arcfragment=archeight/10.0;
    if (ah==0)
    {
        QPainterPath s(QPointF(0,0));
        s.lineTo(0,height);
        s.lineTo(thickness,height);
        s.lineTo(thickness,0);
        s.lineTo(0,0);
        AppendToList(Scene->addPath(s.translated(MovingPoint/ScreenSize),QPen(col),QBrush(col)));

    }
    else
    {
        QPainterPath s(QPointF(archeight,-arcfragment*6.0));
        s.cubicTo(QPointF(8.0*arcfragment,-4.0*arcfragment),QPointF(6.0*arcfragment,-2.0*arcfragment),QPointF(0,0));
        s.lineTo(0,height);
        s.lineTo(thickness,height);
        s.lineTo(thickness,0);
        s.cubicTo(QPointF(thickness+(3.0*arcfragment),-2.0*arcfragment),QPointF(archeight,-4.0*arcfragment),QPointF(archeight,-arcfragment*6.0));
        AppendToList(Scene->addPath(s.translated(MovingPoint/ScreenSize),QPen(col),QBrush(col)));
    }
}

void OCDraw::PlSquare2(const int h, const int ah, const int t)
{
    float height=(float)h/ScreenSize;
    float archeight=(float)ah/ScreenSize;
    float thickness=t/ScreenSize;
    float arcfragment=archeight/10.0;
    QPainterPath s(QPointF(0,0));
    s.lineTo(0,height);
    s.cubicTo(QPointF(6.0*arcfragment,height+(2.0*arcfragment)),QPointF(8.0*arcfragment,height+(4.0*arcfragment)),QPointF(archeight,height+(6.0*arcfragment)));
    s.cubicTo(QPointF(archeight,height+(4.0*arcfragment)),QPointF(thickness+(3.0*arcfragment),height+(2.0*arcfragment)),QPointF(thickness,height));
    s.lineTo(thickness,0);
    s.lineTo(0,0);
    AppendToList(Scene->addPath(s.translated(MovingPoint/ScreenSize),QPen(col),QBrush(col)));
}

void OCDraw::PlCurly(const int h, const int w, const int t)
{
    float height=(float)h/ScreenSize;
    float width=(float)w/ScreenSize;
    float thickness=t/ScreenSize;
    float ydist=height/10.0;
    float xdist=width/2.0;
    QPainterPath b(QPointF(0,0));
    b.cubicTo(QPointF(-xdist*4.0,ydist),QPointF(xdist,height/2.0),QPointF(-xdist*2,height/2.0));
    b.cubicTo(QPointF(xdist,height/2.0),QPointF(-xdist*4.0,height-ydist),QPointF(0,height));
    b.cubicTo(QPointF(-(xdist*4.0)+thickness,height-(ydist/2.0)),QPointF(xdist+thickness,(height/2.0)+(ydist/2.0)),QPointF(-xdist*2.0,height/2.0));
    b.cubicTo(QPointF(xdist+thickness,(height/2.0)-(ydist/2.0)),QPointF(-(xdist*4.0)+thickness,ydist/2.0),QPointF(0,0));
    AppendToList(Scene->addPath(b.translated(MovingPoint/ScreenSize),QPen(col),QBrush(col)));
}

const QList<QGraphicsItem*> OCDraw::PlRect(const float width, const float height, const int Size, const bool fill, const bool LineThickness)
{
    QList<QGraphicsItem*> l;
    float factor=SizeFactor(Size);
    QBrush b(col);
    if (!fill) b=QBrush(Qt::NoBrush);
    QPointF ep(MovingPoint+(QPointF(width,-height)/factor));
    QRectF r=QRectF(MovingPoint/ScreenSize,ep/ScreenSize).normalized();

    QPen p(col);
    if (LineThickness) p.setWidth(FloatDiv(LineHalfThickNess*4,ScreenSize)/factor);
    QGraphicsItem* i=Scene->addRect(r,p,b);
    AppendToList(i);
    l.append(i);
    return l;
}

void OCDraw::SetXY(const float X, const float Y)
{
    ZeroPoint=QPointF(X,Y);
}

void OCDraw::setcol(const int Pointer)
{
    if (canColor())
    {
        lastCol=col;
        if (Cursor->IsSelected(Pointer))
        {
            col=selectedcolor;
        }
        else if (Cursor->GetPos()==Pointer)
        {
            col=markedcolor;
        }
    }
}

void OCDraw::setcol(const QColor color)
{
    if (canColor())
    {
        lastCol=col;
        col=color;
    }
}

void OCDraw::resetcol()
{
    if (canColor())
    {
        col=lastCol;
    }
}

const bool OCDraw::canColor() const
{
    return (ColorOn & (col != inactivestaffcolor));
}

const bool OCDraw::IsSelected(int Pointer) const
{
    if (canColor())
    {
        return Cursor->IsSelected(Pointer);
    }
    return false;
}

const bool OCDraw::IsMarked(int Pointer) const
{
    if (canColor())
    {
        return Cursor->IsMarked(Pointer);
    }
    return false;
}
/*
void OCDraw::plBeam(float x1, float y1, float x2, float y2, int UpDown, int NumOfBeams)
{
    QPainterPath b(QPointF(0,0));
    for (int iTemp = 0;iTemp<=NumOfBeams-1;iTemp++)
    {
        QPainterPath p(QPointF(0,0));
        p.lineTo(QPointF(x2-x1,y1-y2)/ScreenSize);
        p.lineTo(QPointF(x2-x1,(y1-y2)+(-UpDown*BeamThickness))/ScreenSize);
        p.lineTo(QPointF(0,-UpDown*BeamThickness)/ScreenSize);
        p.lineTo(0,0);
        b.addPath(p.translated(QPointF(0,(BeamThickness + BeamSpace) * iTemp * -UpDown)/ScreenSize));
    }
    DM(x1,y1);
    b.translate(MovingPoint/ScreenSize);
    AppendToList(Scene->addPath(b,QPen(col),QBrush(col)));
}

QList<QGraphicsItem*> OCDraw::PlotHairPin(int Length, int gap, bool IsDim)
{
    QList<QGraphicsItem*> l;
    QPainterPath p(QPointF(0,0));
    if (IsDim)
    {
        p.moveTo(0,FloatDiv(-gap,2*ScreenSize));
        p.lineTo(FloatDiv(Length,ScreenSize),0);
        p.lineTo(0,FloatDiv(gap,2*ScreenSize));
    }
    else
    {
        p.moveTo(FloatDiv(Length,ScreenSize),FloatDiv(-gap,2*ScreenSize));
        p.lineTo(0,0);
        p.lineTo(FloatDiv(Length,ScreenSize),FloatDiv(gap,2*ScreenSize));
    }
    p.translate(MovingPoint/ScreenSize);
    QGraphicsItem* i=Scene->addPath(p,QPen(col,FloatDiv(LineHalfThickNess*3,ScreenSize)));
    AppendToList(i);
    l.append(i);
    return l;
}
*/
const int OCDraw::TextHeight(const CTextElement* TextElem, const int viewsize)
{
    QFontMetrics fm(GetFont(TextElem,viewsize));
    return fm.height();
}

const int OCDraw::TextWidth(const CTextElement* TextElem, const int viewsize)
{
    return TextWidth(TextElem->Text,TextElem,viewsize);
}

const int OCDraw::TextWidth(const QString& Text, const CTextElement* TextElem, const int viewsize)
{
    QFontMetrics fm(GetFont(TextElem,viewsize));
    return fm.width(Text);
}

void OCDraw::PrintTextElement(const int X, const int Y, const CTextElement* TextElem, const int viewsize)
{
    PrintTextElement(X,Y,TextElem->Text, TextElem, viewsize);
}

void OCDraw::PrintTextElement(const int X, const int Y, const QString& Text, const CTextElement* TextElem, const int viewsize)
{
    QFont F=GetFont(TextElem,viewsize);
    QBrush b(Qt::black);
    QPen p(Qt::NoPen);
    QGraphicsSimpleTextItem* item =Scene->addSimpleText(Text);
    item->setFont(F);
    item->setPos(X/viewsize,Y/viewsize);
    item->setPen(p);
    item->setBrush(b);
    AppendToList(item);
}
/*
QList<QGraphicsItem*> OCDraw::PlTime(QDomLiteElement* Symbol, int X, Qt::Alignment Align)
{
    QPainterPath p;
    switch (Symbol->attribute("TimeType").toInt())
    {
        case 2:
            DM(X, ScoreStaffHeight-48, Symbol);
            DR(0,-86,SymbolSize(Symbol));
            p=TextPath(QChar(OCTTFAllaBreve), SymbolSize(Symbol), OCTTFname, false, false, 1200);
            p.translate(-p.boundingRect().width()/2,0);
            break;
        case 1:
            DM(X, ScoreStaffHeight-48, Symbol);
            DR(0,-86,SymbolSize(Symbol));
            p=TextPath(QChar(OCTTFFourFour), SymbolSize(Symbol), OCTTFname, false, false, 1200);
            p.translate(-p.boundingRect().width()/2,0);
            break;
        case 0:
            DM(X, ScoreStaffHeight-40, Symbol);
            p=TextPath(MakeUnicode(Symbol->attribute("Upper")), SymbolSize(Symbol), OCTTFname, false, false, 1200);
            p.translate(-p.boundingRect().width()/2,0);
            DM(X, ScoreStaffHeight-40, Symbol);
            DR(0,-192,SymbolSize(Symbol));
            QPainterPath p1=TextPath(MakeUnicode(Symbol->attribute("Lower")), SymbolSize(Symbol), OCTTFname, false, false, 1200);
            p1.translate(-p1.boundingRect().width()/2,0);
            p.addPath(p1);
    }
    if (Align & Qt::AlignLeft) p.translate(p.boundingRect().width()/2,0);
    if (Align & Qt::AlignRight) p.translate(-p.boundingRect().width()/2,0);
    return plTextPath(p);
}
*/
/*
QList<QGraphicsItem*> OCDraw::PlClef(int Clef, int Size)
{
    QList<QGraphicsItem*> l;
    switch (Clef)
    {
        case 1:
            DR(0,96);
            DR(0,-96,Size);
            l.append(plLet(QChar(OCTTFSopranoClef), Size, OCTTFname, false, false, 624));
            break;
        case 2:
        DR(0,288);
        DR(0,-288,Size);
            l.append(plLet(QChar(OCTTFBassClef), Size, OCTTFname, false, false, 624));
            break;
        case 3:
        DR(0,195);
        DR(0,-192,Size);
            l.append(plLet(QChar(OCTTFAltoClef), Size, OCTTFname, false, false, 624));
            break;
        case 4:
            DR(0,96);
            DR(0,195);
            DR(0,-192,Size);
            l.append(plLet(QChar(OCTTFAltoClef), Size, OCTTFname, false, false, 624));
            break;
        case 5:
        DR(0,192);
        DR(0,-192,Size);
            l.append(plLet(QChar(OCTTFPercussionClef), Size, OCTTFname, false, false, 624));
            break;
    }
    return l;
}
*/
/*
QList<QGraphicsItem*> OCDraw::PlotLengths(int Sign, QPointF Pos, int UpDown, int Size)
{
    QList<QGraphicsItem*> l;
    DM(Pos);
    if (Sign == 1)
    {
        DR(0, UpDown * 10 * 12);
        l.append(plDot(0,0,Size));
    }
    else if (Sign == 2)
    {
        DR(FnSize(-5 * 12,Size), UpDown * 10 * 12);
        l.append(DL(FnSize(120,Size),0));
    }
    return l;
}
*/
/*
QList<QGraphicsItem*> OCDraw::PlPunkt(int Value, int TriolVal, int Size)
{
    QList<QGraphicsItem*> l;
    switch (Value)
    {
    case 9:
    case 18:
    case 36:
    case 72:
    case 144:
        if (TriolVal == 0)
        {
            l.append(plDot(10*12,0,Size));
        }
        break;
    case 3:
        case 6:
        case 12:
        case 24:
        case 48:
        case 96:
        if (TriolVal)
        {
            l.append(plDot(10*12,0,Size));
        }
        break;
    }
    return l;
}
*/
QGraphicsItemGroup* OCDraw::MakeGroup(const QList<QGraphicsItem *> &l)
{
    if (l.isEmpty()) return 0;
    if (!ColorOn) return 0;
    return Scene->createItemGroup(l);
}

const QList<QGraphicsItem*> OCDraw::MyLine(const QPointF p1, const QPointF p2, const QColor color, const bool BF, const bool LineThickness)
{
    return MyLine(p1.x(), p1.y(), p2.x(), p2.y(), color, BF, LineThickness);
}

const QList<QGraphicsItem*> OCDraw::MyLine(const float x1, const float y1, const float x2, const float y2, const QColor color, const bool BF, const bool LineThickness)
{
    QList<QGraphicsItem*> l;
    if (BF)
    {
        QRectF r=QRectF(x1,y1,x2,y2).normalized();
        QGraphicsItem* i=Scene->addRect(r,QPen(color),QBrush(color));
        AppendToList(i);
        l.append(i);
        return l;
    }
    QPen p(color);
    if (LineThickness)
    {
        p.setWidth(FloatDiv(LineHalfThickNess*3,ScreenSize));
        p.setCapStyle(Qt::RoundCap);
    }
    QGraphicsItem* i=Scene->addLine(x1,y1,x2,y2,p);
    AppendToList(i);
    l.append(i);
    return l;
}

OCDraw::OCDraw()
{
    UseList=false;
    ColorOn=true;
    ScreenSize=12;
    col=activestaffcolor;
}

void OCDraw::StartList()
{
    UseList=true;
    items.clear();
}

const QList<QGraphicsItem*>& OCDraw::EndList()
{
    UseList=false;
    return items;
}

void OCDraw::AppendToList(QGraphicsItem *item)
{
    if (UseList) items.append(item);
}

void OCDraw::AppendToList(const QList<QGraphicsItem *> &list)
{
    if (UseList) items.append(list);
}

//--------------------------------------------------------------------------

OCFrameProperties::OCFrameProperties(const int p)
{
    group=0;
    accidentalGroup=0;
    Pointer=p;
}

const QRectF OCFrameProperties::TranslateAccidental(const QPointF& Offset) const
{
    return AccidentalRect.adjusted(-4,-4,4,4).translated(Offset);
}

const QRectF OCFrameProperties::TranslateBounding(const QPointF& Offset) const
{
    return BoundingRect.adjusted(-4,-4,4,4).translated(Offset);
}

//--------------------------------------------------------------------------

OCFrame::OCFrame(QWidget* parent)
{
    RubberBand=new QMacRubberband(QRubberBand::Rectangle, QMacRubberband::MacRubberbandYellow, parent);
}

OCFrame::~OCFrame()
{
    delete RubberBand;
}

void OCFrame::EraseFrame()
{
    RubberBand->hideAnimated();
}

void OCFrame::showAnimated(QRectF g)
{
    if (!RubberBand->isVisible())
    {
        if (g==RubberBand->geometry())
        {
            RubberBand->show();
        }
        else
        {
            RubberBand->setGeometry(g.toRect());
            RubberBand->showAnimated();
        }
    }
    else
    {
        RubberBand->setGeometry(g.toRect());
    }
}

void OCFrame::DrawFrame(const XMLSimpleSymbolWrapper& Symbol, QPointF Offset, const QPointF& zero, const bool fortegns, const int shiftit, OCFrameProperties *FrameProps, OCDraw& ScreenObj)
{
    if (Symbol.IsAnyNote())
    {
        if (fortegns)
        {
            Offset.setY(0);
            //showAnimated(FrameProps->TranslateAccidental(Offset-zero));
            if (FrameProps->accidentalGroup != 0)
            {
                FrameProps->accidentalGroup->setPos(Offset);
                //ScreenObj.Scene->update();
                showAnimated(FrameProps->TranslateAccidental(Offset-zero));
            }
            else
            {
                showAnimated(FrameProps->TranslateAccidental(Offset-zero));
            }
        }
        else
        {
            if (Abs(Offset.x()) > Abs(Offset.y()))
            {
                Offset.setY(0);
            }
            else if (Abs(Offset.x()) < Abs(Offset.y()))
            {
                Offset.setX(0);
            }
            if (FrameProps->group != 0)
            {
                FrameProps->group->setPos(Offset);
                //ScreenObj.Scene->update();
                showAnimated(FrameProps->TranslateBounding(Offset-zero));
            }
            else
            {
                showAnimated(FrameProps->TranslateBounding(Offset-zero));
            }
        }
    }
    else
    {
        if (shiftit == Qt::ShiftModifier) Offset.setY((IntDiv((Offset.y() * ScreenObj.ScreenSize) / 6 ,16) * 16 * 6) / ScreenObj.ScreenSize);
        //showAnimated(FrameProps->TranslateBounding(Offset-zero));
        if (FrameProps->group != 0)
        {
            FrameProps->group->setPos(Offset);
            //ScreenObj.Scene->update();
            showAnimated(FrameProps->TranslateBounding(Offset-zero));
        }
        else
        {
            showAnimated(FrameProps->TranslateBounding(Offset-zero));
        }
    }
}

//--------------------------------------------------------------------------
void OCSymbolArray::AppendGroup(QGraphicsItemGroup* g, const int Pointer, const QRectF& bounding)
{
    if (g==0) return;
    if (g->childItems().count()==0) return;
    OCFrameProperties* F=new OCFrameProperties(Pointer);
    F->group=g;
    if (bounding.isNull())
    {
        F->BoundingRect=g->boundingRect().normalized();
    }
    else
    {
        F->BoundingRect=bounding.normalized();
    }
    if (F->BoundingRect.width()<4) F->BoundingRect.adjust(-2,0,2,0);
    if (F->BoundingRect.height()<4) F->BoundingRect.adjust(0,-2,0,2);
    FrameList.append(F);
}

void OCSymbolArray::AppendAccidentalGroup(QGraphicsItemGroup *g, QGraphicsItemGroup *a, const int Pointer)
{
    if (g==0) return;
    if (g->childItems().count()==0) return;
    OCFrameProperties* F=new OCFrameProperties(Pointer);
    F->BoundingRect=g->boundingRect().normalized();
    if (F->BoundingRect.width()<4) F->BoundingRect.adjust(-2,0,2,0);
    if (F->BoundingRect.height()<4) F->BoundingRect.adjust(0,-2,0,2);
    F->AccidentalRect=a->boundingRect().normalized();
    if (F->AccidentalRect.width()<4) F->AccidentalRect.adjust(-2,0,2,0);
    if (F->AccidentalRect.height()<4) F->AccidentalRect.adjust(0,-2,0,2);
    F->group=g;
    F->accidentalGroup=a;
    FrameList.append(F);
}
/*
void OCSymbolArray::AppendBoundingRect(QRect r, int Pointer)
{
    if (r.isEmpty()) return;
    OCFrameProperties* F=new OCFrameProperties(r,Pointer);
    FrameList.append(F);
}

void OCSymbolArray::AppendAccidentalRect(QRect r, QRect a, int Pointer)
{
    if (r.isEmpty()) return;
    OCFrameProperties* F=new OCFrameProperties(r,Pointer);
    F->AccidentalRect=a;
    FrameList.append(F);
}
*/
OCFrameProperties* OCSymbolArray::RetrieveFromPointer(const int Pointer)
{
    foreach(OCFrameProperties* Frame,FrameList)
    {
        if (Frame->Pointer==Pointer) return Frame;
    }
    return new OCFrameProperties(0);
}

const int OCSymbolArray::Inside(const QPoint& m) const
{
    //foreach(OCFrameProperties* Frame,FrameList)
    for (int i=FrameList.count()-1;i>=0;i--)
    {
        OCFrameProperties* Frame=FrameList[i];
        if (Frame->BoundingRect.contains(m)) return Frame->Pointer;
        //if (Frame->group->isUnderMouse()) return Frame->Pointer;
        //if (Frame->group->boundingRect().contains(m)) return Frame->Pointer;
    }
    return -1;
}

const QList<int> OCSymbolArray::PointersInside(const QRectF& r) const
{
    QList<int> ptrs;
    foreach(OCFrameProperties* Frame,FrameList)
    {
        if (r.contains(Frame->BoundingRect)) ptrs.append(Frame->Pointer);
    }
    return ptrs;

}

void OCSymbolArray::clear()
{
    qDeleteAll(FrameList);
    FrameList.clear();
}

OCSymbolArray::OCSymbolArray()
{
}

OCSymbolArray::~OCSymbolArray()
{
    clear();
}

//--------------------------------------------------------------------------

PrintSignProps::PrintSignProps()
{
}

void PrintSignProps::Fill(const int sign, const XMLSymbolWrapper& Symbol, const int pointer, const QColor color, const int modifier)
{
    Sign=sign;
    InitPos=Symbol.pos();
    Size=Symbol.size();
    Sizefactor=SizeFactor(Size);
    Pointer=pointer;
    Color=color;
    Modifier=modifier;
}

void PrintSignProps::Position(const QPointF& NoteCenter, const float balkheight, const float tielen)
{
    HeightOnBalk=balkheight+InitPos.y();
    Pos=InitPos+NoteCenter;
    TieLen=tielen;
}

void PrintSignProps::DM(OCDraw& ScreenObj)
{
    ScreenObj.DM(Pos.x(), Pos.y());
}

void PrintSignProps::DM(const float OffsetX, const float OffsetY, OCDraw& ScreenObj)
{
    ScreenObj.DM(Pos.x() + OffsetX, Pos.y() + OffsetY);
}

void PrintSignProps::DMVertical(const int UpDown, const float OffsetX, const float OffsetY, const float OffsetYbalk, OCDraw& ScreenObj)
{
    if (UpDown == -1)
    {
        float oy=OffsetYbalk;
        if (oy==0) oy=OffsetY;
        ScreenObj.DM(Pos.x() +OffsetX, HeightOnBalk + oy);
    }
    else
    {
        ScreenObj.DM(Pos.x() +OffsetX, Pos.y() + OffsetY);
    }
}

PlaySignProps::PlaySignProps()
{

}

void PlaySignProps::Fill(const int duration, const int value, const int modulate)
{
    Duration=duration;
    Value=value;
    Modulate=modulate;
}

//--------------------------------------------------------------------------

XMLFontWrapper::XMLFontWrapper(QDomLiteElement *XMLFont)
{
    XMLElement=XMLFont;
}

XMLFontWrapper::XMLFontWrapper(QDomLiteElement *XMLParent, const QString& Name)
{
    XMLElement=XMLParent->elementByTag(Name);
}

XMLFontWrapper::XMLFontWrapper(const QFont& f, const QString& Name)
{
    XMLElement=new QDomLiteElement(Name);
    setFont(f);
}

const QFont XMLFontWrapper::getFont() const
{
    QFont f(XMLElement->attribute("FontName"),XMLElement->attributeValue("Size"));
    f.setBold(XMLElement->attributeValue("Bold"));
    f.setItalic(XMLElement->attributeValue("Italic"));
    return f;
}

void XMLFontWrapper::setFont(const QFont& f)
{
    XMLElement->setAttribute("FontName",f.family());
    XMLElement->setAttribute("Size",f.pointSizeF());
    XMLElement->setAttribute("Bold",f.bold());
    XMLElement->setAttribute("Italic",f.italic());
}

QDomLiteElement* XMLFontWrapper::getXML()
{
    return XMLElement;
}

void XMLFontWrapper::showDialog(QWidget* parent)
{
    setFont(QFontDialog::getFont(0, getFont(), parent));
}

XMLTextWrapper::XMLTextWrapper(QDomLiteElement *XMLText) : XMLFontWrapper(XMLText)
{

}

XMLTextWrapper::XMLTextWrapper(QDomLiteElement *XMLParent, const QString& Name) : XMLFontWrapper(XMLParent,Name)
{

}

const QString XMLTextWrapper::getText() const
{
    return XMLElement->attribute("Text");
}

void XMLTextWrapper::setText(const QString& Text)
{
    XMLElement->setAttribute("Text",Text);
}

CStringCompare::CStringCompare(const QString& str)
{
    Val=str;
}

const bool CStringCompare::Compare(const QStringList& S) const
{
    return S.contains(Val);
}

const bool CStringCompare::Compare(const QString& S1) const
{
    return (Val==S1);
}

const bool CStringCompare::Compare(const QString& S1, const QString& S2) const
{
    return ((Val==S1) || (Val==S2));
}

const bool CStringCompare::Compare(const QString& S1, const QString& S2, const QString& S3) const
{
    return ((Val==S1) || (Val==S2) || (Val==S3));
}

const bool CStringCompare::Compare(const QString& S1, const QString& S2, const QString& S3, const QString& S4) const
{
    return ((Val==S1) || (Val==S2) || (Val==S3) || (Val==S4));
}

const bool CStringCompare::Compare(const QString& S1, const QString& S2, const QString& S3, const QString& S4, const QString& S5) const
{
    return ((Val==S1) || (Val==S2) || (Val==S3) || (Val==S4) || (Val==S5));
}

CNoteCompare::CNoteCompare(QDomLiteElement *XML) : CStringCompare (XML->attribute("SymbolName"))
{
    XMLElement=XML;
}

const bool CNoteCompare::IsNoteType(const bool Note, const bool TiedNote, const bool CompoundNote, const bool TiedCompoundNote) const
{
    if (Compare("Note"))
    {
        if (Note && (XMLElement->attributeValue("NoteType")==0)) return true;
        if (TiedNote && (XMLElement->attributeValue("NoteType")==1)) return true;
        if (CompoundNote && (XMLElement->attributeValue("NoteType")==2)) return true;
        if (TiedCompoundNote && (XMLElement->attributeValue("NoteType")==3)) return true;
    }
    return false;
}

const bool CNoteCompare::IsRestOrNoteType(const bool Note, const bool TiedNote, const bool CompoundNote, const bool TiedCompoundNote) const
{
    if (Compare("Rest")) return true;
    return IsNoteType(Note,TiedNote,CompoundNote,TiedCompoundNote);
}

const bool CNoteCompare::IsRestOrAnyNote() const
{
    return IsRestOrNoteType(true,true,true,true);
}

const bool CNoteCompare::IsRestOrValuedNote() const
{
    return IsRestOrNoteType(true,true);
}

const bool CNoteCompare::IsTiedNote() const
{
    return IsNoteType(false,true,false,true);
}

const bool CNoteCompare::IsValuedNote() const
{
    return IsNoteType(true,true);
}

const bool CNoteCompare::IsCompoundNote() const
{
    return IsNoteType(false,false,true,true);
}

const bool CNoteCompare::IsAnyNote() const
{
    return IsNoteType(true,true,true,true);
}

const bool CNoteCompare::IsRest() const
{
    return Compare("Rest");
}

const bool CNoteCompare::IsEndOfVoice() const
{
    return Compare("EndOfVoice");
}

XMLSimpleSymbolWrapper::XMLSimpleSymbolWrapper(QDomLiteElement *XMLSymbol) : CNoteCompare(XMLSymbol)
{
}

XMLSimpleSymbolWrapper::XMLSimpleSymbolWrapper(QDomLiteElement *XMLVoice, const int Pointer) : CNoteCompare(XMLVoice->childElement(Pointer))
{
}

const QString XMLSimpleSymbolWrapper::name() const
{
    return Val;
}

const int XMLSimpleSymbolWrapper::size() const
{
    return getVal("Size");//SymbolSize(XMLElement);
}

const QPointF XMLSimpleSymbolWrapper::pos() const
{
    return QPointF(getVal("Left"),getVal("Top"));
}

const QPointF XMLSimpleSymbolWrapper::move(const QPointF p)
{
    return pos()+p;
}

const QPointF XMLSimpleSymbolWrapper::move(const float x, const float y)
{
    return pos()+QPointF(x,y);
}

const float XMLSimpleSymbolWrapper::moveX(const float x)
{
    return pos().x()+x;
}

const float XMLSimpleSymbolWrapper::moveY(const float y)
{
    return pos().y()+y;
}

const double XMLSimpleSymbolWrapper::getVal(const QString& Tag) const
{
    return XMLElement->attributeValue(Tag);
}

const QString XMLSimpleSymbolWrapper::attribute(const QString& Tag) const
{
    return XMLElement->attribute(Tag);
}

void XMLSimpleSymbolWrapper::setAttribute(const QString& Tag, const QVariant& Value)
{
    XMLElement->setAttribute(Tag,Value);
}

const bool XMLSimpleSymbolWrapper::attributeExists(const QString& Tag) const
{
    return XMLElement->attributeExists(Tag);
}

QDomLiteElement* XMLSimpleSymbolWrapper::getXML()
{
    return XMLElement;
}

XMLSymbolWrapper::XMLSymbolWrapper(QDomLiteElement *XMLSymbol, const int CurrentMeter) : XMLSimpleSymbolWrapper(XMLSymbol)
{
    Init(CurrentMeter);
}

void XMLSymbolWrapper::Init(const int CurrentMeter)
{
    m_Ticks=getVal("Ticks");
    if (CNoteCompare::IsRest())
    {
        if (XMLElement->attributeValue( "NoteValue") == 7)
        {
            m_Ticks=CurrentMeter;
        }
        else
        {
            m_Ticks=Calc();
        }
        XMLElement->setAttribute("Ticks",m_Ticks);
    }
    else if (CNoteCompare::IsAnyNote())
    {
        m_Ticks=Calc();
        XMLElement->setAttribute("Ticks",m_Ticks);
    }
}

void XMLSimpleSymbolWrapper::SetNoteVal(int& Notevalue, bool& Dotted, bool& Triplet, const float Ticks)
{
    int t=Ticks*100;
    Dotted=false;
    Triplet=false;
    switch (t)
    {
    case 14400:
    case 9600:
    case 6400:
        Notevalue=0;
        break;
    case 7200:
    case 4800:
    case 3200:
        Notevalue=1;
        break;
    case 3600:
    case 2400:
    case 1600:
        Notevalue=2;
        break;
    case 1800:
    case 1200:
    case 800:
        Notevalue=3;
        break;
    case 900:
    case 600:
    case 400:
        Notevalue=4;
        break;
    //case 450:
    case 300:
    case 200:
        Notevalue=5;
        break;
    //case 225:
    //case 150:
    case 100:
        Notevalue=6;
    }
    switch (t)
    {
    case 14400:
    case 7200:
    case 3600:
    case 1800:
    case 900:
    //case 450:
    //case 225:
        Dotted=true;
    }
    switch (t)
    {
    case 6400:
    case 3200:
    case 1600:
    case 800:
    case 400:
    case 200:
    case 100:
        Triplet=true;
    }
}

const float XMLSimpleSymbolWrapper::CalcTicks(const int NoteValue, const bool Dotted, const bool Triplet)
{
    switch (NoteValue)
    {
    case 7:
        return 0;
    case 6:
        //if (Triplet)
        //{
            return 1;
        //}
        /*
        if (Dotted)
        {
            return 2.25;
        }
        return 1.5;
        */
    case 5:
        if (Triplet)
        {
            return 2;
        }
        /*
        if (Dotted)
        {
            return 4.5;
        }
        */
        return 3;
    case 4:
        if (Triplet)
        {
            return 4;
        }
        if (Dotted)
        {
            return 9;
        }
        return 6;
    case 3:
        if (Triplet)
        {
            return 8;
        }
        if (Dotted)
        {
            return 18;
        }
        return 12;
    case 2:
        if (Triplet)
        {
            return 16;
        }
        if (Dotted)
        {
            return 36;
        }
        return 24;
    case 1:
        if (Triplet)
        {
            return 32;
        }
        if (Dotted)
        {
            return 72;
        }
        return 48;
    case 0:
        if (Triplet)
        {
            return 64;
        }
        if (Dotted)
        {
            return 144;
        }
        return 96;
    }
    return 0;
}

const int XMLSymbolWrapper::Calc()
{
    return CalcTicks(XMLElement->attributeValue("NoteValue"),XMLElement->attributeValue("Dotted"),XMLElement->attributeValue("Triplet"));
}

XMLSymbolWrapper::XMLSymbolWrapper(QDomLiteElement *XMLVoice, const int Pointer, const int CurrentMeter) : XMLSimpleSymbolWrapper(XMLVoice->childElement(Pointer))
{
    Init(CurrentMeter);
}

const int XMLSymbolWrapper::ticks() const
{
    return m_Ticks;
}

XMLScoreWrapper::XMLScoreWrapper()
{
    m_XMLScore=0;
    isShadow=false;
}

XMLScoreWrapper::XMLScoreWrapper(QDomLiteDocument *Doc)
{
    shadowXML(Doc);
}

XMLScoreWrapper::~XMLScoreWrapper()
{
    if (!isShadow)
    {
        if (m_XMLScore != 0) delete m_XMLScore;
    }
}

void XMLScoreWrapper::setXML(XMLScoreWrapper &Doc)
{
    setXML(Doc.getXML());
}

void XMLScoreWrapper::setXML(QDomLiteDocument *Doc)
{
    isShadow=false;
    if ((m_XMLScore != 0) && (m_XMLScore != Doc)) delete m_XMLScore;
    m_XMLScore=Doc;
}

void XMLScoreWrapper::setCopy(XMLScoreWrapper &Doc)
{
    setCopy(Doc.getXML());
}

void XMLScoreWrapper::setCopy(QDomLiteDocument *Doc)
{
    isShadow=false;
    if (m_XMLScore != 0) delete m_XMLScore;
    m_XMLScore=Doc->clone();
}

void XMLScoreWrapper::shadowXML(XMLScoreWrapper &Doc)
{
    shadowXML(Doc.getXML());
}

void XMLScoreWrapper::shadowXML(QDomLiteDocument *Doc)
{
    isShadow=true;
    m_XMLScore=Doc;
}

QDomLiteDocument* XMLScoreWrapper::getXML()
{
    return m_XMLScore;
}

QDomLiteElement* XMLScoreWrapper::documentElement()
{
    return m_XMLScore->documentElement;
}

QDomLiteElement** XMLScoreWrapper::documentPointer()
{
    return &m_XMLScore->documentElement;
}

QDomLiteElement* XMLScoreWrapper::documentClone()
{
    return m_XMLScore->documentElement->clone();
}

QDomLiteDocument* XMLScoreWrapper::getClone()
{
    return m_XMLScore->clone();
}

void XMLScoreWrapper::newScore()
{
    newDoc();
    *m_XMLScore->documentElement << "Score" << "Templates";
    *Templates() << "AllTemplate";
    AddStaff(0,"New Staff");
    setAttribute("Size",12);
}

void XMLScoreWrapper::newDoc()
{
    isShadow=false;
    if (m_XMLScore != 0) delete m_XMLScore;
    m_XMLScore=new QDomLiteDocument("ObjectComposerProject","ObjectComposerScore");
}

void XMLScoreWrapper::replaceDocumentElement(QDomLiteElement* XMLDoc)
{
    m_XMLScore->replaceDoc(XMLDoc);
}

void XMLScoreWrapper::replaceScore(QDomLiteElement *XMLScore)
{
    m_XMLScore->documentElement->replaceChild(Score(), XMLScore);
}

void XMLScoreWrapper::replaceLayoutCollection(QDomLiteElement *XMLLayoutCollection)
{
    m_XMLScore->documentElement->replaceChild(LayoutCollection(), XMLLayoutCollection);
}

const bool XMLScoreWrapper::Load(const QString& Path)
{
    newDoc();
    return m_XMLScore->load(Path);
}

const bool XMLScoreWrapper::Save(const QString& Path)
{
    return m_XMLScore->save(Path);
}

const QString XMLScoreWrapper::attribute(const QString& Tag) const
{
    return m_XMLScore->documentElement->attribute(Tag);
}

const double XMLScoreWrapper::getVal(const QString& Tag) const
{
    return m_XMLScore->documentElement->attributeValue(Tag);
}

void XMLScoreWrapper::setAttribute(const QString& Tag, const QVariant& Value)
{
    m_XMLScore->documentElement->setAttribute(Tag,Value);
}

QDomLiteElement* XMLScoreWrapper::Score()
{
    return m_XMLScore->documentElement->elementByTag("Score");
}

QDomLiteElement* XMLScoreWrapper::Staff(const int Index)
{
    return  Score()->childElement(Index);
}

QDomLiteElement* XMLScoreWrapper::Voice(const int StaffIndex, const int Index)
{
    return Staff(StaffIndex)->childElement(Index);
}

QDomLiteElement* XMLScoreWrapper::Voice(QDomLiteElement *XMLStaff, const int Index)
{
    return XMLStaff->childElement(Index);
}

QDomLiteElement* XMLScoreWrapper::Voice(QDomLiteElement *XMLScore, const int StaffIndex, const int Index)
{
    return XMLScore->childElement(StaffIndex)->childElement(Index);
}

QDomLiteElement* XMLScoreWrapper::Symbol(const int StaffIndex, const int VoiceIndex, const int Index)
{
    return Voice(StaffIndex,VoiceIndex)->childElement(Index);
}

QDomLiteElement* XMLScoreWrapper::Symbol(QDomLiteElement *XMLVoice, const int Index)
{
    return XMLVoice->childElement(Index);
}

QDomLiteElement* XMLScoreWrapper::SymbolClone(const int StaffIndex, const int VoiceIndex, const int Index)
{
    return Voice(StaffIndex,VoiceIndex)->childElement(Index)->clone();

}

QDomLiteElement* XMLScoreWrapper::SymbolClone(QDomLiteElement *XMLVoice, const int Index)
{
    return XMLVoice->childElement(Index)->clone();
}

const int XMLScoreWrapper::FindSymbol(QDomLiteElement* XMLVoice, const QString& Name, const int Ptr, const QString& Attr, const double Val, const QString& Attr1, const double Val1)
{
    int Py=Ptr;
    forever
    {
        XMLSimpleSymbolWrapper XMLSymbol(XMLVoice,Py);
        if (XMLSymbol.Compare(Name))
        {
            if (!Attr.isEmpty())
            {
                if (XMLSymbol.getVal(Attr)==Val)
                {
                    if (!Attr1.isEmpty())
                    {
                        if (XMLSymbol.getVal(Attr1)==Val1) return Py;
                    }
                    else
                    {
                        return Py;
                    }
                }
            }
            else
            {
                return Py;
            }
        }
        if (XMLSymbol.IsEndOfVoice()) return Py-1;
        Py++;
    }
}

QDomLiteElement* XMLScoreWrapper::Templates()
{
    return m_XMLScore->documentElement->elementByTag("Templates");
}

QDomLiteElement* XMLScoreWrapper::Template(const int Index)
{
    return Templates()->childElement(Index);
}

QDomLiteElement* XMLScoreWrapper::TemplateStaff(const int TemplateIndex, const int Index)
{
    return Template(TemplateIndex)->childElement(Index);
}

QDomLiteElement* XMLScoreWrapper::TemplateStaff(QDomLiteElement *XMLTemplate, const int Index)
{
    return XMLTemplate->childElement(Index);
}

QDomLiteElement* XMLScoreWrapper::TemplateOrderStaff(QDomLiteElement *XMLTemplate, const int Order)
{
    return XMLTemplate->childElement(StaffOrder(XMLTemplate,Order));
}

QDomLiteElement* XMLScoreWrapper::TemplateOrderStaff(const int TemplateIndex, const int Order)
{
    return TemplateOrderStaff(Template(TemplateIndex),Order);
}

void XMLScoreWrapper::ValidateBrackets(const int TemplateIndex)
{
    ValidateBrackets(Template(TemplateIndex));
}

void XMLScoreWrapper::ValidateBrackets(QDomLiteElement* Template)
{
    for (int i = 1; i<Template->childCount()-1;i++)
    {
        if (XMLScoreWrapper::TemplateStaff(Template,i)->attributeValue("SquareBracket")==SBBegin)
        {
            if (XMLScoreWrapper::TemplateStaff(Template,i-1)->attributeValue("SquareBracket")==SBBegin)
            {
                if (XMLScoreWrapper::TemplateStaff(Template,i+1)->attributeValue("SquareBracket")==SBNone)
                {
                    XMLScoreWrapper::TemplateStaff(Template,i)->setAttribute("SquareBracket",SBEnd);
                }
            }
        }
        if (XMLScoreWrapper::TemplateStaff(Template,i)->attributeValue("SquareBracket")==SBEnd)
        {
            if (XMLScoreWrapper::TemplateStaff(Template,i-1)->attributeValue("SquareBracket")==SBEnd)
            {
                XMLScoreWrapper::TemplateStaff(Template,i)->setAttribute("SquareBracket",SBBegin);
            }
        }
        if (XMLScoreWrapper::TemplateStaff(Template,i)->attributeValue("SquareBracket")!=SBNone)
        {
            if (XMLScoreWrapper::TemplateStaff(Template,i+1)->attributeValue("SquareBracket")==SBNone)
            {
                if (XMLScoreWrapper::TemplateStaff(Template,i-1)->attributeValue("SquareBracket")!=SBBegin)
                {
                    XMLScoreWrapper::TemplateStaff(Template,i)->setAttribute("SquareBracket",SBNone);
                }
            }
        }
        if (XMLScoreWrapper::TemplateStaff(Template,i)->attributeValue("CurlyBracket")==CBBegin)
        {
            if (XMLScoreWrapper::AllTemplateIndex(Template,i+1)-XMLScoreWrapper::AllTemplateIndex(Template,i)>1)
            {
                XMLScoreWrapper::TemplateStaff(Template,i)->setAttribute("CurlyBracket",CBNone);
            }
        }
        if (XMLScoreWrapper::TemplateStaff(Template,i)->attributeValue("CurlyBracket")==CBBegin)
        {
            if (XMLScoreWrapper::TemplateStaff(Template,i-1)->attributeValue("CurlyBracket")==CBBegin)
            {
                XMLScoreWrapper::TemplateStaff(Template,i)->setAttribute("CurlyBracket",CBNone);
            }
        }
    }
    if (Template->childCount()>1)
    {
        if (XMLScoreWrapper::TemplateStaff(Template,0)->attributeValue("SquareBracket")==SBBegin)
        {
            if (XMLScoreWrapper::TemplateStaff(Template,1)->attributeValue("SquareBracket")==SBNone)
            {
                XMLScoreWrapper::TemplateStaff(Template,0)->setAttribute("SquareBracket",SBNone);
            }
        }
        if (XMLScoreWrapper::TemplateStaff(Template,Template->childCount()-1)->attributeValue("SquareBracket")>SBNone)
        {
            if (XMLScoreWrapper::TemplateStaff(Template,Template->childCount()-2)->attributeValue("SquareBracket")==SBNone)
            {
                XMLScoreWrapper::TemplateStaff(Template,Template->childCount()-1)->setAttribute("SquareBracket",SBNone);
            }
        }
        if (XMLScoreWrapper::TemplateStaff(Template,Template->childCount()-1)->attributeValue("SquareBracket")==SBBegin)
        {
            if (XMLScoreWrapper::TemplateStaff(Template,Template->childCount()-2)->attributeValue("SquareBracket")==SBBegin)
            {
                XMLScoreWrapper::TemplateStaff(Template,Template->childCount()-1)->setAttribute("SquareBracket",SBEnd);
            }
        }
        if (XMLScoreWrapper::TemplateStaff(Template,Template->childCount()-1)->attributeValue("CurlyBracket")==CBBegin)
        {
            XMLScoreWrapper::TemplateStaff(Template,Template->childCount()-1)->setAttribute("CurlyBracket",CBNone);
        }
    }
    if (Template->childCount()==1)
    {
        XMLScoreWrapper::TemplateStaff(Template,0)->setAttribute("CurlyBracket",CBNone);
        XMLScoreWrapper::TemplateStaff(Template,0)->setAttribute("SquareBracket",SBNone);
    }
}

const QFont XMLScoreWrapper::TempoFont(const QFont& defaultFont)
{
    if (m_XMLScore->documentElement->childCount("TempoFont"))
    {
        return XMLFontWrapper(m_XMLScore->documentElement,"TempoFont").getFont();
    }
    m_XMLScore->documentElement->appendChild(XMLFontWrapper(defaultFont, "TempoFont").getXML());
    return defaultFont;
}

const QFont XMLScoreWrapper::DynamicFont(const QFont& defaultFont)
{
    if (m_XMLScore->documentElement->childCount("DynamicFont"))
    {
        return XMLFontWrapper(m_XMLScore->documentElement,"DynamicFont").getFont();
    }
    m_XMLScore->documentElement->appendChild(XMLFontWrapper(defaultFont, "DynamicFont").getXML());
    return defaultFont;
}

void XMLScoreWrapper::setTempoFont(const QFont& Font)
{
    if (m_XMLScore->documentElement->childCount("TempoFont"))
    {
        m_XMLScore->documentElement->removeChild("TempoFont");
    }
    m_XMLScore->documentElement->appendChild(XMLFontWrapper(Font, "TempoFont").getXML());
}

void XMLScoreWrapper::setDynamicFont(const QFont& Font)
{
    if (m_XMLScore->documentElement->childCount("DynamicFont"))
    {
        m_XMLScore->documentElement->removeChild("DynamicFont");
    }
    m_XMLScore->documentElement->appendChild(XMLFontWrapper(Font, "DynamicFont").getXML());
}

const int XMLScoreWrapper::AllTemplateIndex(const int TemplateIndex, const int StaffIndex)
{
    return AllTemplateIndex(Template(TemplateIndex),StaffIndex);
}

const int XMLScoreWrapper::AllTemplateIndex(QDomLiteElement* XMLTemplate, const int StaffIndex)
{
    return AllTemplateIndex(XMLTemplate->childElement(StaffIndex));
}

const int XMLScoreWrapper::AllTemplateIndex(QDomLiteElement *XMLTemplateStaff)
{
    return XMLTemplateStaff->attributeValue("AllTemplateIndex");
}

const int XMLScoreWrapper::StaffOrder(const int TemplateIndex, const int StaffIndex)
{
    return StaffOrder(Template(TemplateIndex),StaffIndex);
}

const int XMLScoreWrapper::StaffOrder(QDomLiteElement *XMLTemplate, const int StaffIndex)
{
    for (int i=0;i<XMLTemplate->childCount();i++)
    {
        if (AllTemplateIndex(XMLTemplate,i)==StaffIndex)
        {
            return i;
        }
    }
    return 0;
}

const bool XMLScoreWrapper::StaffOnTemplate(QDomLiteElement *XMLTemplate, const int StaffIndex)
{
    for (int i=0;i<XMLTemplate->childCount();i++)
    {
        if (AllTemplateIndex(XMLTemplate,i)==StaffIndex)
        {
            return true;
        }
    }
    return false;
}

const int XMLScoreWrapper::NumOfStaffs()
{
    return Score()->childCount();
}

const int XMLScoreWrapper::NumOfVoices(const int StaffIndex)
{
    return Staff(StaffIndex)->childCount();
}

const int XMLScoreWrapper::NumOfVoices(QDomLiteElement *XMLStaff)
{
    return XMLStaff->childCount();
}

QDomLiteElement* XMLScoreWrapper::Paste1Voice(const int StaffIndex, const int VoiceIndex, const int Pointer, QDomLiteElement *data)
{
    return Paste1Voice(Voice(StaffIndex,VoiceIndex),Pointer,data);
}

QDomLiteElement* XMLScoreWrapper::Paste1Voice(QDomLiteElement *XMLVoice, const int Pointer, QDomLiteElement *data)
{
    for (int Pnt=0;Pnt<data->childCount();Pnt++)
    {
        XMLVoice->insertChild(data->childElement(Pnt)->clone(),Pointer+Pnt);
        //XMLInsertElement(XMLVoice,data->childElement(Pnt)->clone(),Pointer+Pnt);
    }
    return data;
}

void XMLScoreWrapper::Clear1Voice(const int StaffIndex, const int VoiceIndex, const int StartPointer, const int EndPointer)
{
    Clear1Voice(Voice(StaffIndex,VoiceIndex),StartPointer,EndPointer);
}

void XMLScoreWrapper::Clear1Voice(const int StaffIndex, const int VoiceIndex, const QList<int> Pointers)
{
    Clear1Voice(Voice(StaffIndex,VoiceIndex),Pointers);
}

void XMLScoreWrapper::Clear1Voice(QDomLiteElement *XMLVoice, const int StartPointer, const int EndPointer)
{
    if (StartPointer > EndPointer) return;
    for (int lTemp = EndPointer;lTemp>=StartPointer;lTemp--)
    {
        XMLVoice->removeChild(lTemp);
    }
}

void XMLScoreWrapper::Clear1Voice(QDomLiteElement *XMLVoice, const QList<int> Pointers)
{
    for(int i=Pointers.count()-1;i>=0;i--)
    {
        XMLVoice->removeChild(Pointers[i]);
    }
}

const int XMLScoreWrapper::VoiceLength(const int StaffIndex, const int VoiceIndex)
{
    return VoiceLength(Voice(StaffIndex,VoiceIndex));
}

const int XMLScoreWrapper::VoiceLength(QDomLiteElement *XMLVoice)
{
    return XMLVoice->childCount();
}

QDomLiteElement* XMLScoreWrapper::AddVoice(const int StaffIndex)
{
    return AddVoice(Staff(StaffIndex));
}

QDomLiteElement* XMLScoreWrapper::AddVoice(const int StaffIndex, const int NewNumber)
{
    return AddVoice(Staff(StaffIndex),NewNumber);
}

QDomLiteElement* XMLScoreWrapper::AddVoice(QDomLiteElement *XMLStaff)
{
    QDomLiteElement* NewVoice = XMLStaff->appendChild("Voice");
    QDomLiteElement* NewSymbol = NewVoice->appendChild("Symbol");
    NewSymbol->setAttribute("SymbolName","EndOfVoice");
    return NewVoice;
}

QDomLiteElement* XMLScoreWrapper::AddVoice(QDomLiteElement *XMLStaff, const int NewNumber)
{
    QDomLiteElement* NewVoice = XMLStaff->insertChild("Voice",NewNumber);
    QDomLiteElement* NewSymbol = NewVoice->appendChild("Symbol");
    NewSymbol->setAttribute("SymbolName","EndOfVoice");
    return NewVoice;
}

void XMLScoreWrapper::UpdateIndexes(QDomLiteElement *XMLTemplate)
{
    for (int lTemp=0;lTemp<XMLTemplate->childCount();lTemp++)
    {
        QDomLiteElement* TStaff = XMLTemplate->childElement(lTemp);
        TStaff->setAttribute("Index",lTemp);
        TStaff->setAttribute("AllTemplateIndex",lTemp);
    }

}

QDomLiteElement* XMLScoreWrapper::AddTemplateStaff(const int TemplateIndex, const int NewNumber, const QString& Name)
{
    return AddTemplateStaff(Template(TemplateIndex),NewNumber,Name);
}

QDomLiteElement* XMLScoreWrapper::AddTemplateStaff(QDomLiteElement *XMLTemplate, const int NewNumber, const QString& Name)
{
    QDomLiteElement* NewStaff = new QDomLiteElement("TemplateStaff");
    NewStaff->setAttribute("Name",Name);
    NewStaff->setAttribute("AllTemplateIndex",NewNumber);
    XMLTemplate->insertChild(NewStaff,NewNumber);
    UpdateIndexes(XMLTemplate);
    return NewStaff;
}

QDomLiteElement* XMLScoreWrapper::AddTemplateStaff(const int NewNumber, const QString& Name)
{
    return AddTemplateStaff(Template(0),NewNumber,Name);
}

QDomLiteElement* XMLScoreWrapper::AddStaff(const int NewNumber, const QString& Name)
{
    QString NewName=Name.trimmed();
    if (NewName.length()==0) NewName="Staff "+QString::number(NewNumber+1);
    QDomLiteElement* NewStaff = new QDomLiteElement("Staff");
    NewStaff->setAttribute("Name",NewName);
    Score()->insertChild(NewStaff,NewNumber);
    AddVoice(NewStaff);
    AddTemplateStaff(NewNumber, NewName);
    return NewStaff;
}

void XMLScoreWrapper::InsertSymbol(const int StaffIndex, const int VoiceIndex, QDomLiteElement *XMLSymbol, const int Pointer)
{
    InsertSymbol(Voice(StaffIndex,VoiceIndex),XMLSymbol,Pointer);
}

void XMLScoreWrapper::InsertSymbol(QDomLiteElement *XMLVoice, QDomLiteElement *XMLSymbol, const int Pointer)
{
    XMLVoice->insertChild(XMLSymbol,Pointer);
}

void XMLScoreWrapper::DeleteVoice(const int StaffIndex, const int VoiceIndex)
{
    DeleteVoice(Staff(StaffIndex),VoiceIndex);
}

void XMLScoreWrapper::DeleteVoice(QDomLiteElement *XMLStaff, const int VoiceIndex)
{
    XMLStaff->removeChild(VoiceIndex);
}

void XMLScoreWrapper::TemplateDeleteStaff(const int TemplateIndex, const int StaffIndex)
{
    TemplateDeleteStaff(Template(TemplateIndex),StaffIndex);
}

void XMLScoreWrapper::TemplateDeleteStaff(QDomLiteElement *XMLTemplate, const int StaffIndex)
{
    XMLTemplate->removeChild(StaffIndex);
    UpdateIndexes(XMLTemplate);
}

void XMLScoreWrapper::TemplateDeleteStaff(const int StaffIndex)
{
    TemplateDeleteStaff(Template(0),StaffIndex);
}

void XMLScoreWrapper::DeleteStaff(const int StaffIndex)
{
    Score()->removeChild(StaffIndex);
    TemplateDeleteStaff(StaffIndex);
}

void XMLScoreWrapper::MoveStaff(const int From, const int To)
{
    QDomLiteElement* XMLStaff=Score()->takeChild(From);
    QDomLiteElement* XMLTemplateStaff=Template(0)->takeChild(From);
    Score()->insertChild(XMLStaff,To);
    Template(0)->insertChild(XMLTemplateStaff,To);
    UpdateIndexes(Template(0));
}

const int XMLScoreWrapper::StaffPos(const int TemplateIndex, const int StaffIndex)
{
    return StaffPos(Template(TemplateIndex),StaffIndex);
}

const int XMLScoreWrapper::StaffPos(QDomLiteElement *XMLTemplate, const int StaffIndex)
{
    int RetVal = 82 * 12;
    for (int iTemp = 0; iTemp<StaffIndex; iTemp++)
    {
        RetVal += ((100 + TemplateStaff(XMLTemplate,iTemp)->attributeValue("Height")) * 12);
    }
    return RetVal;
}

const int XMLScoreWrapper::StaffPos(const int StaffIndex)
{
    return StaffPos(Template(0),StaffIndex);
}

const QString XMLScoreWrapper::StaffName(const int StaffIndex)
{
    return StaffName(TemplateStaff(0,StaffIndex));
}

const QString XMLScoreWrapper::StaffName(QDomLiteElement *XMLTemplateStaff)
{
    return XMLTemplateStaff->attribute("Name");
}

const QString XMLScoreWrapper::StaffName(QDomLiteElement *XMLTemplate, const int StaffIndex)
{
    return StaffName(TemplateStaff(XMLTemplate,StaffIndex));
}

void XMLScoreWrapper::setStaffName(const int StaffIndex, const QString& Name)
{
    TemplateStaff(0,StaffIndex)->setAttribute("Name",Name);
    Staff(StaffIndex)->setAttribute("Name",Name);
}

const QString XMLScoreWrapper::StaffAbbreviation(const int StaffIndex)
{
    return StaffAbbreviation(TemplateStaff(0,StaffIndex));
}

const QString XMLScoreWrapper::StaffAbbreviation(QDomLiteElement *XMLTemplateStaff)
{
    return XMLTemplateStaff->attribute("Abbreviation");
}

const QString XMLScoreWrapper::StaffAbbreviation(QDomLiteElement *XMLTemplate, const int StaffIndex)
{
    return StaffAbbreviation(TemplateStaff(XMLTemplate,StaffIndex));
}

void XMLScoreWrapper::setStaffAbbreviation(const int StaffIndex, const QString& Abbreviation)
{
    TemplateStaff(0,StaffIndex)->setAttribute("Abbreviation",Abbreviation);
    Staff(StaffIndex)->setAttribute("Abbreviation",Abbreviation);
}

QDomLiteElement* XMLScoreWrapper::LayoutCollection()
{
    return m_XMLScore->documentElement->elementByTag("LayoutCollection");
}

const bool XMLScoreWrapper::LayoutCollectionExists() const
{
    return (m_XMLScore->documentElement->childCount("LayoutCollection") > 0);
}

const int XMLScoreWrapper::NumOfLayouts()
{
    if (LayoutCollection()==0) return 0;
    return LayoutCollection()->childCount();
}

QDomLiteElement* XMLScoreWrapper::LayoutTemplate(const int Index)
{
    if (LayoutCollection()==0) return Template(0);
    return Layout(Index)->elementByTag("Template");
}

QDomLiteElement* XMLScoreWrapper::Layout(const int Index)
{
    return LayoutCollection()->childElement(Index);
}

QDomLiteElement* XMLScoreWrapper::Layout(const QString& Name)
{
    QDomLiteElement* RetVal=0;
    for (int i=0;i<NumOfLayouts();i++)
    {
        if (LayoutName(i)==Name)
        {
            RetVal=Layout(i);
            break;
        }
    }
    return RetVal;
}

const int XMLScoreWrapper::LayoutNumber(const QString& Name)
{
    for (int i=0;i<NumOfLayouts();i++)
    {
        if (LayoutName(i)==Name)
        {
            return i;
        }
    }
    return -1;
}

const QString XMLScoreWrapper::LayoutName(QDomLiteElement *XMLLayout)
{
    return XMLLayout->attribute("Name");
}

const QString XMLScoreWrapper::LayoutName(const int LayoutIndex)
{
    return LayoutName(Layout(LayoutIndex));
}

void XMLScoreWrapper::setLayoutName(const int LayoutIndex, const QString Name)
{
    Layout(LayoutIndex)->setAttribute("Name",Name);
}

//--------------------------------------------------------------------------

OCTieWrap::OCTieWrap()
{
    EraseTies=false;
}

void OCTieWrap::clear()
{
    TieWrap.clear();
}

void OCTieWrap::Add(const int Value)
{
    TieWrap.append(Value);
}

void OCTieWrap::plot(const int NoteX, const int NoteY, const int Alti, const int TieDirection, const int UpDown, OCDraw& ScreenObj)
{
    if (TieWrap.contains(Alti))
    {
        ScreenObj.DM(NoteX, NoteY);
        ScreenObj.DR(-240, 0);//UpDown * TieDirection * 108);
        PlSlur(20 * 12, -1, 0, (UpDown * TieDirection), 0, ScreenObj);
    }
}

void OCTieWrap::PlotTie(const bool LastTie, const int Antal, const int UpDown, const int TieDirection, const int TieLen, const int Alti, int NextHeight, OCDraw& ScreenObj)
{
    if (LastTie)
    {
        ScreenObj.DR(-84, 0);
        PlSlur(TieLen + 168, 1, 0, (UpDown * TieDirection), 0, ScreenObj);
    }
    else if (Antal == 1)
    {
        if (NextHeight <= 0) NextHeight = Alti;
        if (NextHeight != Alti)
        {
            ScreenObj.DR(-84, 0);// ' UpDown * 24
            PlSlur(TieLen + 168, 1, Alti - NextHeight, UpDown, 0, ScreenObj);
        }
        else
        {
            ScreenObj.DR(-84, 0);// ' UpDown * 24
            PlSlur(TieLen + 168, 1, 0, UpDown, 0, ScreenObj);
        }
    }
    else
    {
        //'ScreenObj.DR 0, UpDown * 24
        ScreenObj.DR(-84, 0);
        PlSlur(TieLen + 168, 1, 0, (UpDown * TieDirection), 0, ScreenObj);
    }
}

void OCTieWrap::EraseTie()
{
    if (EraseTies)
    {
        clear();
        EraseTies = false;
        //Tie = false;
    }
}

const QList<QGraphicsItem*> OCTieWrap::PlSlur(int length, const int leftright, const int slant, int updown, const int curve, OCDraw& ScreenObj)
{
    QList<QGraphicsItem*> l;
    int ud=updown;
    updown=-updown;
    int moveleft = 0;
    if (length < 9 * 12)
    {
        moveleft = (7 * 12) - length;
        length = 9 * 12;
    }
    length=(length*leftright)/ScreenObj.ScreenSize;
    float cr=1.0+((float)curve/50.0);
    float angle=(float)slant/ScreenObj.ScreenSize;
    float thickness=18.0/ScreenObj.ScreenSize;
    float anglefactor=(float)angle/(float)length;
    float xdist=length/10.0;
    float xaddhigh=-anglefactor*(length/2.0)*updown;
    float xaddlow=xaddhigh/12.0;
    float curvature=cr*(8+(length/12))*(qAbs(anglefactor/4)+1.0);
    //float curvature=(cr*(((length*1)*(qAbs(anglefactor)/1))+1.0))*6.0;
    QPainterPath b(QPointF(0,0));
    /*
    if (length<0)
    {
        updown=-updown;
        xaddhigh=-xaddhigh;
        xaddlow=-xaddlow;
        qSwap(xaddhigh,xaddlow);
    }
    */
    if (angle*updown>0) qSwap(xaddhigh,xaddlow);
    b.cubicTo(QPointF(xdist+xaddhigh,curvature*updown),QPointF(length-xdist+xaddlow,angle+(curvature*updown)),QPointF(length,angle));
    b.cubicTo(QPointF(length-xdist+xaddlow,angle+((curvature-thickness)*updown)),QPointF(xdist+xaddhigh,(curvature-thickness)*updown),QPointF(0,0));
    b.translate(QPointF((8 * 12) - moveleft, - (ud * 5 * 12))/ScreenObj.ScreenSize);
    ScreenObj.translatePath(b);
    ScreenObj.lastSlur=b.boundingRect();
    //QGraphicsItem* i=Scene->addPath(b,QPen(col),QBrush(col));
    //AppendToList(i);
    //l.append(i);
    l.append(ScreenObj.plTextPath(b,true));
    return l;
}

OCPrintStaffVarsType::OCPrintStaffVarsType(QDomLiteElement *XMLTemplateStaff)
{
    Square=XMLTemplateStaff->attributeValue("SquareBracket");
    Curly=XMLTemplateStaff->attributeValue("CurlyBracket");
    Height=XMLTemplateStaff->attributeValue("Height");
}

OCMIDIVars::OCMIDIVars()
{
    Reset();
}

void OCMIDIVars::Reset()
{
    Channel=0;
    Patch=0;
    Transpose=0;
    Octave=0;
}

OCPlayBackVarsType::OCPlayBackVarsType()
{
    PortamentoOn = true;
    ExpressionOn = true;
    express = 0;
    exprbegin = expressiondefault;
    changeexp = 0;
    currentcresc = 0;
    RepeatFromStart = true;
    Currentdynam = 70;
    currentlen = 80;
    Currenttime = 0;
    crescendo = 0;
    PlayMeter = 96;
    Playtempo = 120;
    HoldTempo = 120;
    Accel = 0;
    Volta = 0;
    Fine = false;
    MIDI.Reset();
}

OCPrintVarsType::OCPrintVarsType()
{
    Articulation.Reset();
    Articulationx1.Reset();
    CurrentKey.Reset();
    //Tie = false;
    FlipTie=false;
    CurrentClef.Reset();
    BalkLimit = 24;
    SlantFlag = 0;
    crescendo.Reset();
    diminuendo.Reset();
    SlurUp.Reset();
    SlurDown.Reset();
    BalkOverRide.Reset();
    UpDown = 0;
    Punktoverride.Reset();
    StregOverRide.Reset();
    UpDownOverRide.Reset();
    SlantOverRide.Reset();
    MIDI.Reset();
    Meter = 96;
    MeterText="4/4";
    for (int iTemp = 0; iTemp < 12;iTemp++) J[iTemp] = 0;
    TieWrap.clear();
    cueletter = 0;
    FilePointer=0;
    KeyChange=false;
    ClefChange=false;
    MasterStuff=false;
}

void OCPrintVarsType::Decrement(const int c)
{
    SlurDown.Decrem(c);
    SlurUp.Decrem(c);
    crescendo.Decrem(c);
    diminuendo.Decrem(c);
    BalkOverRide.Decrem(c);
    if (BalkOverRide.RemainingTicks <= 0) BalkOverRide.val = 0;
    Punktoverride.Decrem(c);
    StregOverRide.Decrem(c);
    UpDownOverRide.Decrem(c);
    SlantOverRide.Decrem(c);
    Articulationx1.val = 0;
    KeyChange=false;
    ClefChange=false;
    MasterStuff=false;
}

OCBarWindowBar::OCBarWindowBar()
{
    Meter=96;
    MeterText="4/4";
    Pointer=0;
    Density=0;
    Notes=0;
    IsFullRest=false;
    IsFullRestOnly=false;
    KeyChangeOnOne=false;
    ClefChangeOnOne=false;
    MasterStuff=false;
}

OCBarMap::OCBarMap()
{

}

const int OCBarMap::BarMapIndex(const int Staff, const int Voice) const
{
    for (int i=0;i<Voices.count();i++)
    {
        const OCBarWindowVoice& bwv=Voices[i];
        if (bwv.Staff==Staff)
        {
            if (bwv.Voice==Voice)
            {
                if (!bwv.Bars.isEmpty()) return i;
                return -1;
            }
        }
    }
    return -1;
}

const int OCBarMap::GetPointer(const int Bar, const int Staff, const int Voice) const
{
    int voiceNum=BarMapIndex(Staff,Voice);
    if (voiceNum<0) return 0;
    const OCBarWindowVoice& bwv=Voices[voiceNum];
    if (Bar>bwv.Bars.count()-1)
    {
        return bwv.EndPointer;
    }
    return bwv.Bars.at(qMin(Bar,bwv.Bars.count()-1)).Pointer;
}

const int OCBarMap::GetBar(const int Pointer, const int Staff, const int Voice) const
{
    int voiceNum=BarMapIndex(Staff,Voice);
    if (voiceNum<0) return 0;
    const OCBarWindowVoice& bwv=Voices[voiceNum];
    for (int i=0;i<bwv.Bars.count();i++)
    {
        if (Pointer<bwv.Bars.at(i).Pointer) return qMax(0,i-1);
    }
    if (Pointer<bwv.EndPointer) return bwv.Bars.count()-1;
    return 0;
}

const int OCBarMap::GetMeter(const int Bar, const int Staff, const int Voice) const
{
    int voiceNum=BarMapIndex(Staff,Voice);
    if (voiceNum<0) return 96;
    const OCBarWindowVoice& bwv=Voices[voiceNum];
    return bwv.Bars.at(qMin(Bar,bwv.Bars.count()-1)).Meter;
}

const int OCBarMap::BarCountAll() const
{
    int Count=0;
    for(int i=0;i<Voices.count();i++)
    {
        Count=qMax(Count,Voices.at(i).Bars.count());
    }
    return Count;
}

const int OCBarMap::BarCount(const int Staff, const int Voice) const
{
    int voiceNum=BarMapIndex(Staff,Voice);
    if (voiceNum<0) return 0;
    return Voices.at(voiceNum).Bars.count();
}

const int OCBarMap::BarCountAll(QDomLiteElement *XMLTemplate) const
{
    int Count=0;
    for (int i=0;i<XMLTemplate->childCount();i++)
    {
        int Staff=XMLScoreWrapper::AllTemplateIndex(XMLTemplate,i);
        int StaffIndex=BarMapIndex(Staff,0);
        if (StaffIndex>-1)
        {
            for (int Voice=0;Voice<Voices.at(StaffIndex).NumOfVoices;Voice++)
            {
                Count=qMax(Count,Voices.at(StaffIndex+Voice).Bars.count());
            }
        }
    }
    return Count;
}

const bool OCBarMap::IsEnded(const int Bar, QDomLiteElement *XMLTemplate) const
{
    for (int i=0;i<XMLTemplate->childCount();i++)
    {
        int Staff=XMLScoreWrapper::AllTemplateIndex(XMLTemplate,i);
        int StaffIndex=BarMapIndex(Staff,0);
        if (StaffIndex>-1)
        {
            for (int Voice=0;Voice<Voices.at(StaffIndex).NumOfVoices;Voice++)
            {
                const OCBarWindowVoice& bwv=Voices[StaffIndex+Voice];
                if (Bar<bwv.Bars.count())
                {
                    if ((bwv.Bars.at(Bar).Notes) || (bwv.Bars.at(Bar).IsFullRest)) return false;
                }
            }
        }
    }
    return true;
}

const int OCBarMap::EndOfVoiceBar(const int Staff, const int Voice) const
{
    int voiceNum=BarMapIndex(Staff,Voice);
    if (voiceNum<0) return 0;
    int Count=Voices.at(voiceNum).Bars.count();
    if (!Voices.at(voiceNum).Incomplete) return Count;
    return Count-1;
}

const int OCBarMap::NoteCount(const int Bar, const int Staff, const int Voice) const
{
    int voiceNum=BarMapIndex(Staff,Voice);
    if (voiceNum<0) return 0;
    if (Bar>Voices.at(voiceNum).Bars.count()-1) return 0;
    return Voices.at(voiceNum).Bars.at(Bar).Notes;
}

const int OCBarMap::NoteCountStaff(const int Staff, const int StartBar, const int EndBar) const
{
    int Count=0;
    int StaffIndex=BarMapIndex(Staff,0);
    if (StaffIndex<0) return 0;
    for (int i=StartBar; i<EndBar; i++)
    {
        for (int VoiceIndex=0; VoiceIndex<Voices.at(StaffIndex).NumOfVoices; VoiceIndex++)
        {
            Count+=NoteCount(i,Staff,VoiceIndex);
        }
    }
    return Count;
}

const bool OCBarMap::IsFullRest(const int Bar, QDomLiteElement* XMLTemplate) const
{
    if (BarCountAll(XMLTemplate)<=Bar) return false;
    bool FullRest=true;
    for (int i=0;i<XMLTemplate->childCount();i++)
    {
        int Staff=XMLScoreWrapper::AllTemplateIndex(XMLTemplate,i);
        int StaffIndex=BarMapIndex(Staff,0);
        if (StaffIndex>-1)
        {
            for (int Voice=0;Voice<Voices.at(StaffIndex).NumOfVoices;Voice++)
            {
                const OCBarWindowVoice& bwv=Voices[StaffIndex+Voice];
                if (Bar<=bwv.Bars.count()-1)
                {
                    FullRest=FullRest & bwv.Bars.at(Bar).IsFullRest;
                }
                else
                {
                    return false;
                }
            }
        }
    }
    return FullRest;
}

const bool OCBarMap::IsFullRestOnly(const int Bar, QDomLiteElement* XMLTemplate) const
{
    if (BarCountAll(XMLTemplate)<=Bar) return false;
    bool FullRest=true;
    for (int i=0;i<XMLTemplate->childCount();i++)
    {
        int Staff=XMLScoreWrapper::AllTemplateIndex(XMLTemplate,i);
        int StaffIndex=BarMapIndex(Staff,0);
        if (StaffIndex>-1)
        {
            for (int Voice=0;Voice<Voices.at(StaffIndex).NumOfVoices;Voice++)
            {
                const OCBarWindowVoice& bwv=Voices[StaffIndex+Voice];
                if (Bar<=bwv.Bars.count()-1)
                {
                    FullRest=FullRest & bwv.Bars.at(Bar).IsFullRestOnly;
                }
                else
                {
                    return false;
                }
            }
        }
    }
    return FullRest;
}

const bool OCBarMap::ClefChange(const int Bar, const int Staff, const int Voice) const
{
    int voiceNum=BarMapIndex(Staff,Voice);
    if (voiceNum<0) return false;
    if (Bar>Voices.at(voiceNum).Bars.count()-1) return false;
    return Voices.at(voiceNum).Bars.at(Bar).ClefChangeOnOne;
}

const bool OCBarMap::KeyChange(const int Bar, const int Staff, const int Voice) const
{
    int voiceNum=BarMapIndex(Staff,Voice);
    if (voiceNum<0) return false;
    if (Bar>Voices.at(voiceNum).Bars.count()-1) return false;
    return Voices.at(voiceNum).Bars.at(Bar).KeyChangeOnOne;
}

const bool OCBarMap::MasterStuff(const int Bar, const int Staff) const
{
    bool Master=false;
    int StaffIndex=BarMapIndex(Staff,0);
    if (StaffIndex>-1)
    {
        for (int Voice=0;Voice<Voices.at(StaffIndex).NumOfVoices;Voice++)
        {
            const OCBarWindowVoice& bwv=Voices[StaffIndex+Voice];
            if (Bar<=bwv.Bars.count()-1)
            {
                Master=Master | bwv.Bars.at(Bar).MasterStuff;
            }
        }
    }
    return Master;
}

DomAttributesModel::DomAttributesModel(QObject* /*parent*/)
{
    m_Element=0;
}

void DomAttributesModel::setXML(QDomLiteElement *XML)
{
    m_Element=XML;
}

int DomAttributesModel::rowCount(const QModelIndex & /*parent*/) const
{
    return 1;
}

int DomAttributesModel::columnCount(const QModelIndex & /*parent*/) const
{
    if (m_Element==0) return 0;
    return m_Attributes.count();
}

void DomAttributesModel::addColumn(int column, QString attributeName)
{
    m_Attributes.insert(column,attributeName);
}

QVariant DomAttributesModel::data(const QModelIndex &index, int role) const
{
    if ((role == Qt::EditRole) | (role == Qt::DisplayRole))
    {
        return m_Element->attribute(m_Attributes[index.column()]);
    }
    return QVariant();
}

bool DomAttributesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        m_Element->setAttribute(m_Attributes[index.column()], value);
        emit dataChanged(index,index);
    }
    return true;
}
