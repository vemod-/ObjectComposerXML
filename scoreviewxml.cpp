#include "scoreviewxml.h"
#include "ui_ScoreViewXML.h"
#include <QAction>
#include <QMenu>
#include <QScrollBar>
#include "ocsymbolscollection.h"
#include "cmusicxml.h"
//#include <QGLWidget>

double signedSqrt(double value)
{
    return Sgn<double>(value)*qSqrt(qAbs<double>(value));
}

ScoreViewXML::ScoreViewXML(QWidget* parent) : QGraphicsView(parent),
    ui(new Ui::ScoreViewXML)
{
    ui->setupUi(this);
    viewport()->setAttribute(Qt::WA_AcceptTouchEvents,true);
    touchDown=false;
    swipeDelta=0;
    swipePos=0;
    swipeBackPos=0;
    swipeLine.setDuration(200);
    swipeLine.setEasingCurve(QEasingCurve::InCurve);
    connect(&swipeLine, &QTimeLine::frameChanged, this, &ScoreViewXML::swipeProc);
    setAutoFillBackground(true);
    SelectRubberband=new QiPhotoRubberband(this);
    HoverRubberband=new QHoverRubberband(QRubberBand::Rectangle,this);
    SelectRubberband->hide();
    HoverRubberband->hide();
    m_XMLLastPasted.clear();
    turnpagebutton=new QHoverButton(this,QIcon(":/turnpage.png"),QSize(96,96),QHoverButton::TopRight);
    connect(turnpagebutton,&QAbstractButton::clicked,this,&ScoreViewXML::NavigationForwardClicked);
    turnbackbutton=new QHoverButton(this,QIcon(":/turnback.png"),QSize(96,96),QHoverButton::LeftTop);
    connect(turnbackbutton,&QAbstractButton::clicked,this,&ScoreViewXML::NavigationBackClicked);
    fastforwardbutton=new QHoverButton(this,QIcon(":/fast-forward.png"),QSize(128,128),QHoverButton::Right);
    connect(fastforwardbutton,&QAbstractButton::clicked,this,&ScoreViewXML::NavigationEndClicked);
    fastbackbutton=new QHoverButton(this,QIcon(":/fast-back.png"),QSize(128,128),QHoverButton::Left);
    connect(fastbackbutton,&QAbstractButton::clicked,this,&ScoreViewXML::NavigationHomeClicked);
    connect(&soundTimer,&QTimer::timeout,this,&ScoreViewXML::soundOff);

    setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
    setRenderHints(renderinghints);
    setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
    Scene = new QGraphicsScene(this);
    Scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(Scene);
    Scene->setBackgroundBrush(paperbrush);
    ScreenObj.Scene=Scene;
    ScreenObj.Cursor = &Cursor;
    CursorFrame=new OCCursorFrame(this);
    MouseDown=false;
    Dragging=false;
    MouseButton=Qt::NoButton;
    MouseArea=MouseOutside;
    MouseAreaIndex=0;
    soundMark=0;
    soundPitch=0;
    m_Locked=false;
    m_NavigationVisible=false;
    setMouseTracking(true);
    zoomer = new QGraphicsViewZoomer(this);
    connect(zoomer,&QGraphicsViewZoomer::ZoomChanged,this,&ScoreViewXML::changeZoom);

    XMLScore.newScore();
    Score.assignXML(XMLScore);
    setFollowResize(PageSizeFollowsResize);
    setStartBar(0);
    setSize(12);
    setActiveTemplate();
    setActiveOptions();
    m_EndBar=0;
    m_StartBar=0;
    m_SystemLength = (900 * 12);
    setActiveStaffId(0);
    setSceneRect(0,0,800,systemRect().height());

    EditMenu = new QMenu(this);
    actionSwapForward = EditMenu->addAction("Swap forward",QKeySequence::SelectNextWord,this,&ScoreViewXML::selectSwapForward);
    actionSwapBack = EditMenu->addAction("Swap back",QKeySequence::SelectPreviousWord,this,&ScoreViewXML::selectSwapBack);
    EditMenu->addSeparator();
    actionSelectAll = EditMenu->addAction("Select All",QKeySequence::SelectAll,this,&ScoreViewXML::selectAll);
    actionSelectHome = EditMenu->addAction("Select from Start to here",QKeySequence::SelectStartOfLine,this,&ScoreViewXML::selectHomeExtend);
    actionSelectEnd = EditMenu->addAction("Select from here to End",QKeySequence::SelectEndOfLine,this,&ScoreViewXML::selectEndExtend);

    ScoreMenu = new QMenu(this);
    actionPreviousVoice = ScoreMenu->addAction("Previous Voice",QKeySequence(Qt::META | Qt::ALT | Qt::Key_Up),this,&ScoreViewXML::selectPrevVoice);
    actionNextVoice = ScoreMenu->addAction("Next Voice",QKeySequence(Qt::META | Qt::ALT | Qt::Key_Down),this,&ScoreViewXML::selectNextVoice);
    actionPreviousStaff = ScoreMenu->addAction("Previous Staff",QKeySequence::MoveToStartOfBlock,this,&ScoreViewXML::selectPrevStaff);
    actionNextStaff = ScoreMenu->addAction("Next Staff",QKeySequence::MoveToEndOfBlock,this,&ScoreViewXML::selectNextStaff);

    connect(setAction(QKeySequence::MoveToNextChar),&QAction::triggered,this,&ScoreViewXML::selectNextSymbol);
    connect(setAction(QKeySequence::MoveToPreviousChar),&QAction::triggered,this,&ScoreViewXML::selectPrevSymbol);
    connect(setAction(QKeySequence::SelectNextChar),&QAction::triggered,this,&ScoreViewXML::selectNextSymbolExtend);
    connect(setAction(QKeySequence::SelectPreviousChar),&QAction::triggered,this,&ScoreViewXML::selectPrevSymbolExtend);
    connect(setAction(QKeySequence::MoveToStartOfLine),&QAction::triggered,this,&ScoreViewXML::selectHome);
    connect(setAction(QKeySequence::MoveToEndOfLine),&QAction::triggered,this,&ScoreViewXML::selectEnd);

    connect(setAction(QKeySequence(Qt::ALT | Qt::Key_Left)),&QAction::triggered,this,&ScoreViewXML::selectHome);
    connect(setAction(QKeySequence(Qt::ALT | Qt::Key_Right)),&QAction::triggered,this,&ScoreViewXML::selectEnd);

    connect(setAction(QKeySequence(Qt::META | Qt::Key_Up)),&QAction::triggered,this,&ScoreViewXML::selectPreferedUp);
    connect(setAction(QKeySequence(Qt::META | Qt::Key_Down)),&QAction::triggered,this,&ScoreViewXML::selectPreferedDown);

    //connect(setAction(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Up)),SIGNAL(triggered()),this,SLOT(selectOctaveUp()));
    //connect(setAction(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Down)),SIGNAL(triggered()),this,SLOT(selectOctaveDown()));

    connect(setAction(QKeySequence::MoveToNextPage),&QAction::triggered,this,&ScoreViewXML::turnpage);
    connect(setAction(QKeySequence::MoveToPreviousPage),&QAction::triggered,this,&ScoreViewXML::turnback);
    connect(setAction(QKeySequence::MoveToNextLine),&QAction::triggered,this,&ScoreViewXML::selectPitchDown);
    connect(setAction(QKeySequence::MoveToPreviousLine),&QAction::triggered,this,&ScoreViewXML::selectPitchUp);
    connect(setAction(QKeySequence::SelectNextLine),&QAction::triggered,this,&ScoreViewXML::selectOctaveDown);
    connect(setAction(QKeySequence::SelectPreviousLine),&QAction::triggered,this,&ScoreViewXML::selectOctaveUp);
    connect(setAction(Qt::Key_Backspace),&QAction::triggered,this,&ScoreViewXML::selectBackSpace);
    connect(setAction(QKeySequence::Delete),&QAction::triggered,this,&ScoreViewXML::selectDelete);

    connect(setAction(Qt::Key_Return),&QAction::triggered,this,&ScoreViewXML::accepted);
    connect(setAction(Qt::Key_Escape),&QAction::triggered,this,&ScoreViewXML::canceled);

    Paint(tsReformat);
}

ScoreViewXML::~ScoreViewXML()
{
    soundTimer.disconnect();
    Score.eraseAll(Scene);
    delete ui;
}

void ScoreViewXML::zeroSwipe()
{
    if (!isZero(swipeDelta))
    {
        touchDown=false;
        if (swipeLine.state()==QTimeLine::Running)
        {
            scroll(-swipePos,0);
        }
        else
        {
            swipeLine.setFrameRange(swipePos, 0);
            swipeBackPos=swipePos;
            swipeLine.start();
        }
        swipeDelta=0;
        swipePos=0;
    }
}

QRectF ScoreViewXML::mapToSceneRect(const QRect &r)
{
    return QRectF(mapToScene(r.topLeft()),mapToScene(r.bottomRight()));
}

QRect ScoreViewXML::mapFromSceneRect(const QPointF &a, const QPointF &b)
{
    return QRect(QGraphicsView::mapFromScene(a),QGraphicsView::mapFromScene(b));
}

const QRect ScoreViewXML::mapFromSceneRect(const QRectF &r)
{
    return QRect(QGraphicsView::mapFromScene(r.topLeft()),QGraphicsView::mapFromScene(r.bottomRight()));
}

void ScoreViewXML::swipeProc(int value)
{
    scroll(value-swipeBackPos,0);
    swipeBackPos=value;
}

bool ScoreViewXML::altModifier(const QMouseEvent *event) {
    if (event) return altMod || (event->modifiers() & Qt::AltModifier);
    return altMod || (QApplication::queryKeyboardModifiers() & Qt::AltModifier);
}

void ScoreViewXML::writeAltKeySymbol(const OCPointerList &ptrs) {
    if (LastSymbol.isDurated()) {
        Cursor.SetSel(ptrs,VoiceLen());
        emit RequestDuratedSymbol(LastSymbol,"Paste Durated Symbol");
    }
    else {
        for (int i = ptrs.size()-1; i >= 0; i--) {
            if (GetSymbol(ptrs[i]).IsValuedNote()) {
                Cursor.SetPos(ptrs[i]);
                emit RequestSymbol(LastSymbol,"Paste Symbol");
            }
        }
    }
}

void ScoreViewXML::writeAltkeyNote(const QPointF &mappedPos) {
    const OCSymbolLocation& l = Score.nearestLocation(mappedPos.x()-16,Cursor.location());
    LastNote.setAttribute("NoteType",0);
    if (l.Pointer > -1) {
        const OCFrameProperties& p = Score.getFrame(l);
        if ((p.BoundingRect.left() < mappedPos.x()) && (p.BoundingRect.right() > mappedPos.x())) {
            LastNote.setAttribute("NoteType",2);
        }
        Cursor.setLocation(l);
    }
    else {
        Cursor.SetPos(VoiceLen());
    }
    if (!LastNote.IsRest())
    {
        LastNote.setPitch(pitchFromPoint(Cursor.location(),mappedPos));
        sound(LastNote.pitch());
    }
}

void ScoreViewXML::writeMoveSymbol(const QPointF &moved, const Qt::KeyboardModifiers &modifiers) {
    QPoint qp=(moved*Size()*SizeFactor(ActiveTemplate.staffFromId(Cursor.location().StaffId).size())).toPoint();
    if (MouseButton != Qt::RightButton) {
        setCursor(Qt::PointingHandCursor);
        HoverRubberband->hide();
        for (const int& i : Cursor.SelectedPointers()) {
            XMLSimpleSymbolWrapper XMLSymbol=GetSymbol(i);
            if (!XMLSymbol.IsPitchedNote()) {
                if (modifiers == Qt::ShiftModifier) qp.setY(IntDiv(qp.y() / 6,16) * 16 * 6);
                XMLSymbol.setLeft(qp.x() + XMLSymbol.left());
                XMLSymbol.setTop(-qp.y() + XMLSymbol.top());
            }
            else
            {
                if (qAbs<qreal>(moved.x()) > qAbs<qreal>(moved.y())) {
                    XMLSymbol.setLeft(qp.x() + XMLSymbol.left());
                }
                else {
                    const int p = boundStep<int>(0, (-qp.y() / 28) + XMLSymbol.pitch(), 127);
                    if (p != soundPitch) sound(p);
                    XMLSymbol.setPitch(p);
                }
            }
        }
    }
    else if (MouseButton==Qt::RightButton) {
        if (qAbs<qreal>(moved.x()) >= qAbs<qreal>(moved.y())) {
            for (const int& i : Cursor.SelectedPointers()) {
                XMLSimpleSymbolWrapper XMLSymbol=GetSymbol(i);
                if (XMLSymbol.IsPitchedNote()) XMLSymbol.setAttribute("AccidentalLeft",qp.x() + XMLSymbol.getIntVal("AccidentalLeft"));
            }
        }
        else {
            for (const int& i : Cursor.SelectedPointers()) {
                XMLSimpleSymbolWrapper XMLSymbol=GetSymbol(i);
                if (XMLSymbol.IsTiedNote()) XMLSymbol.setAttribute("TieTop",-qp.y() + XMLSymbol.getIntVal("TieTop"));
                qp.setY(int(moved.y()));
                if (XMLSymbol.Compare("Hairpin")) XMLSymbol.setAttribute("Gap",qBound<int>(-9, -qp.y() + XMLSymbol.getIntVal("Gap"), 20));
                if (XMLSymbol.Compare("Slur")) XMLSymbol.setAttribute("Curve",qBound<int>(-9, -qp.y() + XMLSymbol.getIntVal("Curve"), 20));
            }
        }
    }
}

int ScoreViewXML::insideStaffId(const QPointF &p) const
{
    for (int StaffPos = 0 ; StaffPos < ActiveTemplate.staffCount() ; StaffPos++) {
        const int staffTop = ActiveTemplate.staffTop(StaffPos);
        if (p.y() > scaled(staffTop)) {
            if (p.y() < scaled(staffTop+ScoreStaffLinesHeight)) {
                return ActiveTemplate.staffId(StaffPos);
            }
        }
    }
    return -1;
}

int ScoreViewXML::pitchFromPoint(const OCSymbolLocation &l, const QPointF &m)
{
    const int clef = Score.fakePlotClef(l);
    const int pitch = 74 - int(((m.y() * Size()) - ActiveTemplate.staffTopFromId(l.StaffId)) / 28) - int((52 - CClef::LineDiff(clef)) * 12.0 / 28.0);
    return boundStep<int>(0, pitch, 127);
}

const OCFrameProperties &ScoreViewXML::CurrentFrame() { return Score.getFrame(Cursor.location()); }

int ScoreViewXML::activeStaffTop() const { return ActiveTemplate.staffTopFromId(ActiveStaffId()); }

const OCBarSymbolLocation ScoreViewXML::pointerBegin(const OCVoiceLocation &Voice) const
{
    return BarMap().GetPointer(OCBarLocation(Voice,StartBar()));
}

const OCBarSymbolLocation ScoreViewXML::pointerBegin() const
{
    return pointerBegin(Cursor.location());
}

void ScoreViewXML::turnpage()
{
    setStartBar(StartBar()+BarsActuallyDisplayed());
    Paint(tsNavigate);
    emit BarChanged();
    if (!CanTurnPage()) turnpagebutton->MouseLeave(nullptr);
}

void ScoreViewXML::leaveEvent(QEvent *event)
{
    if (HoverRubberband->isVisible())
    {
        HoverRubberband->hide();
        setToolTip(QString());
        setCursor(Qt::ArrowCursor);
    }
    if (altModifier()) Paint(tsRedrawActiveStave);
    zeroSwipe();
    QGraphicsView::leaveEvent(event);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void ScoreViewXML::enterEvent(QEnterEvent* event)
#else
void ScoreViewXML::enterEvent(QEvent* event)
#endif
{
    QGraphicsView::enterEvent(event);
}

bool ScoreViewXML::viewportEvent(QEvent *event)
{
    if (event->type()==QTouchEvent::TouchBegin)
    {
        zeroSwipe();
        touchDown=true;
    }
    if (event->type()==QEvent::TouchEnd)
    {
        zeroSwipe();
    }
    return QGraphicsView::viewportEvent(event);
}

void ScoreViewXML::wheelEvent(QWheelEvent* event)
{
    if ((zoomer->getZoom() > 1) && (FollowResize() != PageSizeFixed))
    {
        QGraphicsView::wheelEvent(event);
        return;
    }
    if (HoverRubberband->isVisible())
    {
        HoverRubberband->hide();
        setToolTip(QString());
        setCursor(Qt::ArrowCursor);
    }
    if (FollowResize() != PageSizeUnlimited)
    {
        if (ScrollTimer.isActive()) return;
        if (swipeLine.state()==QTimeLine::Running) return;
        const int xDelta = event->angleDelta().x();
        if (xDelta)
        {
            if (touchDown)
            {
                if (xDelta > 40)
                {
                    if (StartBar() > 0)
                    {
                        ScrollTimer.setSingleShot(true);
                        ScrollTimer.start(1000);
                        touchDown=false;
                        scroll(-swipePos,0);
                        swipeDelta=0;
                        swipePos=0;
                        emit SwipeLeftToRight();
                        swipeDelta=0;
                        swipePos=0;
                        event->accept();
                        return;
                    }
                }
                if (xDelta < -40)
                {
                    if (CanTurnPage())
                    {
                        ScrollTimer.setSingleShot(true);
                        ScrollTimer.start(1000);
                        touchDown=false;
                        scroll(-swipePos,0);
                        swipeDelta=0;
                        swipePos=0;
                        emit SwipeRightToLeft();
                        swipeDelta=0;
                        swipePos=0;
                        event->accept();
                        return;
                    }
                }
                const double f=signedSqrt(swipeDelta+xDelta);
                const int i=qRound(f)-swipePos;
                swipePos+=i;
                scroll(i,0);
                swipeDelta+=xDelta;
                event->accept();
                return;
            }
        }
    }
    QGraphicsView::wheelEvent(event);
}

void ScoreViewXML::turnback()
{
    Score.formatPageBack(XMLScore,ActiveTemplate,ActiveOptions);
    setStartBar(Score.startBar());
    Paint(tsNavigate);
    emit BarChanged();
    if (StartBar()<=0) turnbackbutton->MouseLeave(nullptr);
}

void ScoreViewXML::fastforward()
{
    setStartBar(EndOfVoiceBar());
    Paint(tsNavigate);
    emit BarChanged();
    fastforwardbutton->MouseLeave(nullptr);
}

void ScoreViewXML::fastback()
{
    setStartBar(0);
    Paint(tsNavigate);
    emit BarChanged();
    fastbackbutton->MouseLeave(nullptr);
}

bool ScoreViewXML::CanTurnPage() const
{
    if (FollowResize()==PageSizeFollowsResize) return (EndOfVoiceBar() >= StartBar() + BarsActuallyDisplayed()) && (BarsActuallyDisplayed()>0);
    if (FollowResize()==PageSizeFixed) return (BarMap().BarCountAll(ActiveTemplate) > StartBar() + BarsActuallyDisplayed());
    return false;
}

const OCBarSymbolLocation ScoreViewXML::findPointerToBar(const OCBarLocation &Bar) const
{
    return Score.BarMap().GetPointer(Bar);
}

const OCBarSymbolLocation ScoreViewXML::findPointerToBar(const OCVoiceLocation &VoiceLocation, const int BarToFind) const
{
    return Score.BarMap().GetPointer(OCBarLocation(VoiceLocation,BarToFind));
}

const OCBarSymbolLocation ScoreViewXML::findPointerToBar(const int BarToFind) const
{
    return findPointerToBar(Cursor.location(),BarToFind);
}

ScoreViewXML::PageMode ScoreViewXML::FollowResize() const { return PageMode(ActiveOptions.followResize()); }

int ScoreViewXML::ActiveStaffId() const { return Cursor.location().StaffId; }

int ScoreViewXML::ActiveVoice() const { return Cursor.location().Voice; }

const OCVoiceLocation ScoreViewXML::ActiveVoiceLocation() const { return Cursor.location(); }

const OCBarLocation ScoreViewXML::ActiveBarLocation() const { return OCBarLocation(Cursor.location(),m_StartBar); }

int ScoreViewXML::BarNrOffset() const { return ActiveOptions.barNumberOffset(); }

int ScoreViewXML::MasterStaff() const { return ActiveOptions.masterStaff(); }

int ScoreViewXML::NoteSpace() const { return ActiveOptions.noteSpace(); }

int ScoreViewXML::StartBar() const { return m_StartBar; }

int ScoreViewXML::EndBar() const { return m_EndBar; }

double ScoreViewXML::Size() const { return ActiveOptions.size(); }

bool ScoreViewXML::Locked() const { return m_Locked; }

bool ScoreViewXML::HideBarNumbers() const { return ActiveOptions.hideBarNumbers(); }

void ScoreViewXML::SetSystemLength(const int NewSystemLength) { m_SystemLength = NewSystemLength; }

double ScoreViewXML::SystemLength() const { return m_SystemLength; }

QAction* ScoreViewXML::setAction(const QKeySequence keySequence, const QString title)
{
    auto a=new QAction(this);
    a->setShortcut(keySequence);
    a->setShortcutContext(Qt::ApplicationShortcut);
    a->setText(title);
    addAction(a);
    return a;
}

void ScoreViewXML::keyPressEvent(QKeyEvent *event)
{
    QGraphicsView::keyPressEvent(event);
    if ((event->key() == Qt::Key_Shift) || (event->key() == Qt::Key_Control)) return;
    if (event->key() == Qt::Key_Return) emit accepted();
    SelectRubberband->hide();
}

void ScoreViewXML::keyReleaseEvent(QKeyEvent * event) {
    QGraphicsView::keyReleaseEvent(event);
    if (event->key() == Qt::Key_Alt) Paint(tsRedrawActiveStave);
}

void ScoreViewXML::selectNextSymbolExtend()
{
    SelectRubberband->hide();
    if (Cursor.SelCount()==0) {
        if (Cursor.SelEnd() + 1 < VoiceLen()) Cursor.ExtendSel(Cursor.SelEnd(),VoiceLen());
    }
    else {
        if (Cursor.SelEnd() + 1 < VoiceLen()) Cursor.ExtendSel(Cursor.SelEnd() + 1,VoiceLen());
    }
    if ((Cursor.SelEnd() > findPointerToBar(StartBar() + BarsActuallyDisplayed()).Pointer) && (FollowResize()==PageSizeFollowsResize)) {
        setStartBar(StartBar()+BarsActuallyDisplayed());
        Paint(tsNavigate);
        emit BarChanged();
    }
    else {
        Paint(tsRedrawActiveStave);
    }
    sound();
    emit SelectionChanged();
}

void ScoreViewXML::selectPrevSymbolExtend()
{
    SelectRubberband->hide();
    if (Cursor.SelStart() > 0)
    {
        (Cursor.SelCount() == 0) ? Cursor.SetRange(OCSymbolRange(Cursor.currentPointer()-1,Cursor.currentPointer()),VoiceLen()) :
                                 Cursor.ExtendSel(Cursor.SelStart() - 1,VoiceLen());
        if ((Cursor.SelStart() < pointerBegin().Pointer) && (FollowResize()==PageSizeFollowsResize)) {
            setStartBar(StartBar()-1);
            Paint(tsNavigate);
            emit BarChanged();
        }
        else {
            Paint(tsRedrawActiveStave);
        }
        sound();
        emit SelectionChanged();
    }
}

void ScoreViewXML::selectNextSymbol()
{
    SelectRubberband->hide();
    if (Cursor.currentPointer() < VoiceLen())
    {
        Cursor.SetPos(Cursor.currentPointer() + 1, VoiceLen());
        if ((Cursor.currentPointer() >= findPointerToBar(StartBar() + BarsActuallyDisplayed()).Pointer) && (FollowResize()==PageSizeFollowsResize))
        {
            setStartBar(StartBar() + BarsActuallyDisplayed());
            Paint(tsNavigate);
            emit BarChanged();
        }
        else
        {
            Paint(tsRedrawActiveStave);
        }
        sound();
        emit SelectionChanged();
    }
}

void ScoreViewXML::selectPrevSymbol()
{
    SelectRubberband->hide();
    if (Cursor.currentPointer() > 0)
    {
        Cursor.SetPos(Cursor.currentPointer()-1);
        if ((Cursor.currentPointer() < pointerBegin().Pointer) && (FollowResize()==PageSizeFollowsResize))
        {
            setStartBar(StartBar()-1);
            Paint(tsNavigate);
            emit BarChanged();
        }
        else
        {
            Paint(tsRedrawActiveStave);
        }
        sound();
        emit SelectionChanged();
    }
}

void ScoreViewXML::selectEnd()
{
    SelectRubberband->hide();
    while (Cursor.currentPointer()+1 < VoiceLen())
    {
        Cursor.SetPos(Cursor.currentPointer()+1);
    }
    if (FollowResize() != PageSizeUnlimited)
    {
        setStartBar(EndOfVoiceBar());
        Paint(tsNavigate);
    }
    else
    {
        Paint(tsRedrawActiveStave);
        scrollToBar(EndOfVoiceBar());
    }
    emit SelectionChanged();
    emit BarChanged();
}

void ScoreViewXML::selectHome()
{
    SelectRubberband->hide();
    Cursor.SetPos(0);
    if (FollowResize() != PageSizeUnlimited)
    {
        setStartBar(0);
        Paint(tsNavigate);
    }
    else
    {
        Paint(tsRedrawActiveStave);
        scrollToBar(0);
    }
    emit SelectionChanged();
    emit BarChanged();
}

void ScoreViewXML::selectToEnd()
{
    Cursor.ExtendSel(VoiceLen()-1);
    Paint(tsRedrawActiveStave);
    emit SelectionChanged();
}

void ScoreViewXML::selectToStart()
{
    Cursor.ExtendSel(0);
    Paint(tsRedrawActiveStave);
    emit SelectionChanged();
}

void ScoreViewXML::selectAll()
{
    Cursor.ExtendSel(OCSymbolRange(0,VoiceLen()-1));
    Paint(tsRedrawActiveStave);
    emit SelectionChanged();
}

void ScoreViewXML::selectEndExtend()
{
    SelectRubberband->hide();
    Cursor.ExtendSel(VoiceLen()-1);
    if (FollowResize() != PageSizeUnlimited)
    {
        setStartBar(EndOfVoiceBar());
        Paint(tsNavigate);
    }
    else
    {
        Paint(tsRedrawActiveStave);
        scrollToBar(EndOfVoiceBar());
    }
    emit SelectionChanged();
    emit BarChanged();
}

void ScoreViewXML::selectHomeExtend()
{
    SelectRubberband->hide();
    Cursor.ExtendSel(0);
    if (FollowResize() != PageSizeUnlimited)
    {
        setStartBar(0);
        Paint(tsNavigate);
    }
    else
    {
        Paint(tsRedrawActiveStave);
        scrollToBar(0);
    }
    emit SelectionChanged();
    emit BarChanged();
}

void ScoreViewXML::selectNextStaff()
{
    nextStaff(1);
}

void ScoreViewXML::selectPrevStaff()
{
    nextStaff(-1);
}

void ScoreViewXML::selectNextVoice()
{
    SelectRubberband->hide();
    setActiveVoice(boundRoll(0, Cursor.location().Voice + 1, VoiceCount()-1));
    Cursor.MaxSel(VoiceLen());
    Paint(tsVoiceIndexChanged);
    emit StaffIndexChanged(Cursor.location().StaffId);
    emit SelectionChanged();
}

void ScoreViewXML::selectPrevVoice()
{
    SelectRubberband->hide();
    setActiveVoice(boundRoll(0, Cursor.location().Voice - 1, VoiceCount()-1));
    Cursor.MaxSel(VoiceLen());
    Paint(tsVoiceIndexChanged);
    emit StaffIndexChanged(Cursor.location().StaffId);
    emit SelectionChanged();
}

void ScoreViewXML::selectPreferedUp()
{
    shiftPrefered(1);
}

void ScoreViewXML::selectPreferedDown()
{
    shiftPrefered(-1);
}

void ScoreViewXML::selectPitchUp()
{
    transposeSelected(1);
}

void ScoreViewXML::selectPitchDown()
{
    transposeSelected(-1);
}

void ScoreViewXML::selectOctaveUp()
{
    transposeSelected(12);
}

void ScoreViewXML::selectOctaveDown()
{
    transposeSelected(-12);
}

void ScoreViewXML::shiftPrefered(const int add)
{
    if (!Cursor.SelCount()) return;
    emit BackMeUp("Prefered");
    int PreferedChanged = 0;
    for (const int& Pointer : Cursor.SelectedPointers())
    {
        XMLSimpleSymbolWrapper XMLSymbol=GetSymbol(Pointer);
        if (!XMLSymbol.IsPitchedNote())
        {
            OCProperties& p = OCSymbolsCollection::GetProperties(XMLSymbol);
            const QString s = p.preferedCaption();
            if (!s.isEmpty())
            {
                OCProperty& e = p.property(s);
                const int i = boundRoll(e.Min.toInt(),XMLSymbol.getIntVal(s) + Sgn<int>(add),e.Max.toInt());
                XMLSymbol.setAttribute(s,i);
                PreferedChanged ++;
            }
        }
    }
    if (PreferedChanged) Paint(tsRedrawActiveStave);
    emit Changed();
}

void ScoreViewXML::transposeSelected(const int add)
{
    if (!Cursor.SelCount()) return;
    emit BackMeUp("Pitch");
    int PitchChanged=0;
    for (const int& Pointer : Cursor.SelectedPointers())
    {
        XMLSimpleSymbolWrapper XMLSymbol=GetSymbol(Pointer);
        if (XMLSymbol.IsPitchedNote())
        {
            const int iTemp1=XMLSymbol.pitch();
            if (iTemp1>0)
            {
                XMLSymbol.setPitch(boundStep<int>(0, iTemp1 + add, 127));
                PitchChanged = iTemp1+add;
            }
        }
    }
    if (PitchChanged != 0)
    {
        Paint(tsRedrawActiveStave);
        sound();
        emit SelectionChanged();
    }
    emit Changed();
}

void ScoreViewXML::dottify(int factor)
{
    const OCPointerList& l = Cursor.SelectedPointers();
    int i = 0;

    QList<XMLSimpleSymbolWrapper> symbols;

    int ticks = 0;
    int beatTicks = 0;
    QString undotext;
    switch (factor)
    {
        case 2:
        {
            undotext="Straighten";
            break;
        }
        case 3:
        {
            undotext="Triolize";
            break;
        }
        case 8:
        {
            undotext="Double Dottify";
            break;
        }
        case 4:
        default:
        {
            undotext="Dottify";
            factor = 4;
            break;
        }
    }
    const double longLen = double(factor - 1) / factor;
    const double shortLen = 1.0 / factor;
    while ((i < l.size()) && (symbols.size() < 2))
    {
        XMLSimpleSymbolWrapper s = GetSymbol(l[i++]);
        if (s.IsValuedRestOrValuedNote())
        {
            symbols.append(s);
            ticks += s.tickCalc();
        }
    }
    if (symbols.size() == 2)
    {
        beatTicks = symbols[0].tickCalc() + symbols[1].tickCalc();
        emit BackMeUp(undotext);
        if (symbols[0].tickCalc() >= symbols[1].tickCalc())
        {
            symbols[0].setNoteValFromTicks(beatTicks*longLen);
            symbols[1].setNoteValFromTicks(beatTicks*shortLen);
        }
        else
        {
            symbols[0].setNoteValFromTicks(beatTicks*shortLen);
            symbols[1].setNoteValFromTicks(beatTicks*longLen);
        }
    }
    if (!beatTicks) return;

    while (i < l.size())
    {
        symbols.clear();
        while (ticks - beatTicks >= 0) ticks -= beatTicks;
        while ((i < l.size()) && ticks < beatTicks)
        {
            XMLSimpleSymbolWrapper s = GetSymbol(l[i++]);
            if (s.IsValuedRestOrValuedNote())
            {
                symbols.append(s);
                ticks += s.tickCalc();
            }
        }
        if (symbols.size() == 2)
        {
            if (symbols[0].tickCalc() + symbols[1].tickCalc() == beatTicks)
            {
                if (symbols[0].tickCalc() >= symbols[1].tickCalc())
                {
                    symbols[0].setNoteValFromTicks(beatTicks*longLen);
                    symbols[1].setNoteValFromTicks(beatTicks*shortLen);
                }
                else
                {
                    symbols[0].setNoteValFromTicks(beatTicks*shortLen);
                    symbols[1].setNoteValFromTicks(beatTicks*longLen);
                }
            }
        }
    }

    Paint(tsRedrawActiveStave);
    emit SelectionChanged();
    emit Changed();
}

void ScoreViewXML::Delete(const OCVoiceLocation &VoiceLocation, const OCSymbolRange &SymbolRange)
{
    XMLScore.Clear1Voice(VoiceLocation, SymbolRange);
    Cursor.SetPos(SymbolRange.Start,VoiceLen());
}

void ScoreViewXML::Delete(const OCSymbolRange &SymbolRange)
{
    Delete(Cursor.location(),SymbolRange);
}

void ScoreViewXML::Delete(const OCVoiceLocation &VoiceLocation, const OCPointerList &Pointers)
{
    XMLScore.Clear1Voice(VoiceLocation, Pointers);
    Cursor.SetPos(Pointers.first(),VoiceLen());
}

void ScoreViewXML::Delete(const OCPointerList &Pointers)
{
    Delete(Cursor.location(),Pointers);
}

void ScoreViewXML::Delete()
{
    Delete(Cursor.SelectedPointers());
}

void ScoreViewXML::selectBackSpace()
{
    SelectRubberband->hide();
    if (m_Locked) return;
    if (Cursor.SelCount()==0)
    {
        //Back Delete
        if (Cursor.SelStart() > 0)
        {
            emit BackMeUp("Backspace");
            Cursor.SetPos(Cursor.SelStart() - 1);
            Delete(Cursor.currentPointer());
            if (Cursor.currentPointer() < pointerBegin().Pointer)
            {
                setStartBar(StartBar()-1);
                Paint(tsReformat);
            }
            else
            {
                Paint(tsReformat);
            }
            emit ScoreChanged();
        }
    }
    else
    {
        //Delete selection
        emit BackMeUp("Delete");
        Delete();
        Paint(tsReformat);
        emit ScoreChanged();
    }
}

void ScoreViewXML::selectDelete()
{
    SelectRubberband->hide();
    if (m_Locked) return;
    if (Cursor.SelCount()==0)
    {
        //Delete
        if (Cursor.SelEnd() < VoiceLen())
        {
            emit BackMeUp("Delete");
            Delete(Cursor.currentPointer());
            Paint(tsReformat);
            emit ScoreChanged();
        }
    }
    else
    {
        //Delete selection
        emit BackMeUp("Delete");
        Delete();
        Paint(tsReformat);
        emit ScoreChanged();
    }
}

void ScoreViewXML::selectSwapForward()
{
    SelectRubberband->hide();
    if (m_Locked) return;
    if (Cursor.currentPointer() + 1 < VoiceLen())
    {
        Cursor.SetPos(Cursor.currentPointer()+1);
        if (Cursor.currentPointer() < VoiceLen())
        {
            emit BackMeUp("Swap");
            const XMLVoiceWrapper Edit = GetClipBoardData(OCSymbolRange(Cursor.currentPointer(), Cursor.currentPointer()));
            Delete(Cursor.currentPointer());
            PasteClipBoardData(Cursor.currentPointer() - 1, Edit);
            Cursor.SetPos(Cursor.currentPointer());
            Paint(tsReformat);
        }
        else
        {
            Paint(tsRedrawActiveStave);
        }
        emit ScoreChanged();
    }
}

void ScoreViewXML::selectSwapBack()
{
    SelectRubberband->hide();
    if (m_Locked) return;
    if (Cursor.currentPointer() > 0)
    {
        Cursor.SetPos(Cursor.currentPointer()-1);
        if (Cursor.currentPointer() + 1 < VoiceLen())
        {
            if (Cursor.currentPointer() < VoiceLen())
            {
                emit BackMeUp("Swap");
                XMLVoiceWrapper Edit = GetClipBoardData(OCSymbolRange(Cursor.currentPointer(), Cursor.currentPointer()));
                Delete(Cursor.currentPointer());
                PasteClipBoardData(Cursor.currentPointer() + 1, Edit);
                Cursor.SetPos(Cursor.currentPointer());
                Paint(tsReformat);
            }
        }
        else
        {
            Paint(tsRedrawActiveStave);
        }
        emit ScoreChanged();
    }
}

void ScoreViewXML::sound(const int pitch)
{
    if (soundTimer.isActive())
    {
        soundTimer.stop();
        soundOff();
    }
    soundPitch=pitch;
    if (soundPitch==-1)
    {
        if (CurrentSymbol().IsPitchedNote()) soundPitch=CurrentSymbol().pitch();
    }
    if (soundPitch>-1)
    {
        CurrentMIDI = Score.fakePlot(Cursor.location());
        soundMark=VoiceLocationToMark(Cursor.location());
        emit NoteOnOff(true, soundPitch, soundMark, CurrentMIDI);
        soundTimer.setSingleShot(true);
        soundTimer.start(300);
    }
}

void ScoreViewXML::sound(const OCSymbolLocation& symbol)
{
    if (!GetSymbol(symbol).IsPitchedNote()) return;
    if (soundTimer.isActive())
    {
        soundTimer.stop();
        soundOff();
    }
    soundPitch=GetSymbol(symbol).pitch();
    if (soundPitch>-1)
    {
        CurrentMIDI = Score.fakePlot(symbol);
        soundMark=VoiceLocationToMark(symbol);
        emit NoteOnOff(true, soundPitch, soundMark, CurrentMIDI);
        soundTimer.setSingleShot(true);
        soundTimer.start(300);
    }
}

void ScoreViewXML::soundOff()
{
    if (soundPitch>-1) emit NoteOnOff(false, soundPitch, soundMark, CurrentMIDI);
    soundPitch=-1;
}

void ScoreViewXML::scrollContentsBy(int dx, int dy)
{
    if (HoverRubberband->isVisible())
    {
        HoverRubberband->hide();
        setToolTip(QString());
        setCursor(Qt::ArrowCursor);
    }
    QGraphicsView::scrollContentsBy(dx,dy);
    viewport()->update();
}

void ScoreViewXML::resizeEvent(QResizeEvent *event)
{
    if (FollowResize()==PageSizeFollowsResize)
    {
        if (!closeEnough<double>(Scene->width(),width()))
        {
            Paint(tsReformat);
            return;
        }
    }
    else
    {
        createSceneRect();
    }
    QGraphicsView::resizeEvent(event);
}

void ScoreViewXML::Paint(const OCRefreshMode DrawMode, const bool UpdateSelection)
{
    SelectRubberband->hide();
    setUpdatesEnabled(false);
    if (DrawMode != tsRedrawActiveStave)
    {
        Score.eraseAll(Scene);
        double picheight = systemRect().height();
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        if (FollowResize()==PageSizeFollowsResize)
        {
            int WindowWidth = width();
            if (picheight > height()) WindowWidth -= verticalScrollBar()->width();
            setSceneRect(0,0,WindowWidth,picheight);
            m_SystemLength = (WindowWidth - 40) * Size();
            if (zoomer->getZoom()<1) m_SystemLength /= zoomer->getZoom();
        }
        else if (FollowResize()==PageSizeFixed)
        {
            createSceneRect();
        }
        if (FollowResize()==PageSizeUnlimited)
        {
            if (DrawMode == tsReformat)
            {
                if (DrawMode == tsReformat) Score.createBarMap();
                setStartBar(0);
                Score.formatPage(XMLScore, ActiveTemplate, ActiveOptions, SystemLength(), StartBar(), -1);
                m_SystemLength = Score.systemLength();
                createSceneRect();
            }
        }
        else
        {
            if ((DrawMode == tsReformat) || (DrawMode == tsNavigate) || (DrawMode == tsVoiceIndexChanged))
            {
                if (DrawMode == tsReformat) Score.createBarMap();
                setStartBar(hiBound<int>(StartBar(),EndOfVoiceBar()));
                Score.formatPage(XMLScore, ActiveTemplate, ActiveOptions, SystemLength(), StartBar(), EndBar());
            }
        }
        QColor col(inactivestaffcolor);
        if (m_Locked) col=activestaffcolor;
        for (int StaffPos = 0; StaffPos < ActiveTemplate.staffCount(); StaffPos++)
        {
            const int StaffId = ActiveTemplate.staffId(StaffPos);
            if (StaffId != Cursor.location().StaffId)
            {
                ScreenObj.init(ScoreLeftMargin,ActiveTemplate.staffTopFromId(StaffId));
                Score.plotStaff(StaffId, XMLScore, ActiveTemplate, ActiveOptions, col, ScreenObj);
            }
        }
    }
    Cursor.MaxSel(VoiceLen());
    ScreenObj.init(ScoreLeftMargin,activeStaffTop());
    if (DrawMode == tsRedrawActiveStave) Score.eraseSystem(Cursor.location().StaffId,Scene);
    Score.plotStaff(Cursor.location().StaffId, XMLScore, ActiveTemplate, ActiveOptions, activestaffcolor, ScreenObj);
    viewport()->update();
    setUpdatesEnabled(true);
    if (UpdateSelection) emit SelectionChanged();
}

void ScoreViewXML::PasteClipBoardData(const OCSymbolLocation &SymbolLocation, const XMLVoiceWrapper &ClipBoardData)
{
    XMLScore.Paste1Voice(SymbolLocation, ClipBoardData);
    m_XMLLastPasted.copy(ClipBoardData);
}

void ScoreViewXML::PasteClipBoardData(const int Pointer, const XMLVoiceWrapper &ClipBoardData)
{
    XMLScore.Paste1Voice(OCSymbolLocation(Cursor.location(), Pointer), ClipBoardData);
    m_XMLLastPasted.copy(ClipBoardData);
}

const XMLVoiceWrapper ScoreViewXML::GetClipBoardData(const OCVoiceLocation &VoiceLocation, const OCSymbolRange &SymbolRange) const
{
    XMLVoiceWrapper data;
    if (SymbolRange.End - SymbolRange.Start >= 0)
    {
        int count=0;
        for (int i = SymbolRange.Start ; i <= SymbolRange.End ; i++)
        {
            data.insertChild(XMLScore.Symbol(VoiceLocation.StaffId,VoiceLocation.Voice,i),count++);
        }
    }
    return data;
}

const XMLVoiceWrapper ScoreViewXML::GetClipBoardData(const OCSymbolRange &SymbolRange) const
{
    return GetClipBoardData(Cursor.location(),SymbolRange);
}

const XMLVoiceWrapper ScoreViewXML::GetClipBoardData(const OCVoiceLocation &VoiceLocation, const OCPointerList &Pointers) const
{
    XMLVoiceWrapper data;
    for (int i = 0 ; i < Pointers.size() ; i++)
    {
        data.insertChild(XMLScore.Symbol(VoiceLocation.StaffId,VoiceLocation.Voice,Pointers[i]),i);
    }
    return data;
}

const XMLVoiceWrapper ScoreViewXML::GetClipBoardData(const OCPointerList &Pointers) const
{
    return GetClipBoardData(Cursor.location(),Pointers);
}

const XMLVoiceWrapper ScoreViewXML::GetClipBoardData() const
{
    return GetClipBoardData(Cursor.SelectedPointers());
}

void ScoreViewXML::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (MouseArea == MouseOutside)
        emit BarsPopup(QCursor::pos());
    else if (MouseArea == MouseOnBar)
        emit ListPopup(QCursor::pos());
    else
        emit PropertiesPopup(QCursor::pos()+QPoint(5,5));
}

void ScoreViewXML::flashSelected()
{
    CursorFrame->showAnimated(CurrentFrame().TranslateBounding());
    CursorFrame->hideAnimated();
}

void ScoreViewXML::changeZoom(double zoom)
{
    if (HoverRubberband->isVisible()) HoverRubberband->hide();
    if ((FollowResize()==PageSizeFollowsResize) && (zoomer->getZoom()<1)) Paint(tsReformat,true);
    emit ZoomChanged(zoom);
}

void ScoreViewXML::setZoom(const double Zoom)
{
    zoomer->setZoom(Zoom);
    if (HoverRubberband->isVisible()) HoverRubberband->hide();
    if ((FollowResize()==PageSizeFollowsResize) && (zoomer->getZoom()<1)) Paint(tsReformat,true);
}

void ScoreViewXML::toggleAltKey(bool v) {
    qDebug() << "altkey" << v << altMod;
    altMod = v;
}

void ScoreViewXML::setNoteConfig() {
    emit RequestNote(LastNote);
    qDebug() << LastNote.noteValue() << LastNote.pitch() << LastNote.getIntVal("Ticks");
}

double ScoreViewXML::getZoom() const
{
    return zoomer->getZoom();
}

void ScoreViewXML::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "Press";
    if (m_Locked) return;
    Paint(tsRedrawActiveStave);
    Dragging=false;
    MouseDown = true;
    MouseButton=event->button();
    const QPointF m(mapToScene(event->pos()));
    m_HoldMappedPos=m;
    const int newstaffId = insideStaffId(m);
    int newbar = Score.insideBarline(m);
    if (newstaffId == -1) newbar=-1;
    const OCSymbolLocation& newLocation = Score.insideFrame(m);
    if (newbar > -1)
    {
        if (newstaffId != Cursor.location().StaffId)
        {
            HoverRubberband->hide();
            setActiveStaffId(newstaffId);
            Paint(tsVoiceIndexChanged);
            emit StaffIndexChanged(Cursor.location().StaffId);
        }
    }
    else if (newLocation.Pointer > -1)
    {
        if ((newLocation.StaffId != Cursor.location().StaffId) || (newLocation.Voice != Cursor.location().Voice))
        {
            HoverRubberband->hide();
            Cursor.setLocation(newLocation);
            Paint(tsVoiceIndexChanged);
            emit StaffIndexChanged(Cursor.location().StaffId);
        }
    }
    if (newbar > -1)
    {
        MouseArea = MouseOnBar;
        MouseAreaIndex=newbar;
    }
    else if (newLocation.Pointer > -1)
    {
        MouseArea = MouseOnSymbol;
        MouseAreaIndex=newLocation.Pointer;
        SelectRubberband->hide();
        if (event->modifiers() & Qt::ShiftModifier)
        {
            Cursor.ExtendSel(newLocation.Pointer,VoiceLen());
            if (altModifier(event) && (LastSymbol.isValid()))
            {
                writeAltKeySymbol(Cursor.SelectedPointers());
                Paint(tsReformat);
                emit ScoreChanged();
            }
            else
            {
                Paint(tsRedrawActiveStave);
                emit SelectionChanged();
            }
        }
        else if (event->modifiers() == Qt::ControlModifier)
        {
            if (Cursor.SelCount()==0) Cursor.AddSel(Cursor.currentPointer());
            Cursor.AddSel(newLocation.Pointer,VoiceLen());
            Paint(tsRedrawActiveStave);
            emit SelectionChanged();
        }
        else if (altModifier(event))
        {
            Cursor.SetPos(newLocation.Pointer);
            if (LastSymbol.isValid() && (!LastSymbol.isDurated()))
            {
                emit RequestSymbol(LastSymbol,"Paste Symbol");
                Paint(tsReformat);
                emit ScoreChanged();
            }
            else
            {
                Paint(tsRedrawActiveStave);
                emit SelectionChanged();
            }
        }
        else
        {
            Cursor.SetPos(newLocation.Pointer);
            Paint(tsRedrawActiveStave);
            emit SelectionChanged();
        }
        sound();
        HoverRubberband->hide();
        CursorFrame->showAnimated(CurrentFrame().TranslateBounding());
    }
    else if (newstaffId > -1)
    {
        MouseArea=MouseOutside;
        MouseAreaIndex=-1;
        HoverRubberband->hide();
        setActiveStaffId(newstaffId);
        Paint(tsVoiceIndexChanged);
        emit StaffIndexChanged(Cursor.location().StaffId);
    }
    else
    {
        MouseArea=MouseOutside;
        MouseAreaIndex=-1;
    }
}

void ScoreViewXML::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "Mouse move";
    if (!MouseDown)
    {
        if (m_NavigationVisible)
        {
            if (CanTurnPage())
            {
                int w = width() + 1;
                if (verticalScrollBar()->isVisible()) w -= verticalScrollBar()->width();
                turnpagebutton->Activate(QPointF(w - turnpagebutton->width(),0),event->pos(),QRect(w-25,0,25,25));
                fastforwardbutton->Activate(QPointF(w - fastforwardbutton->width(),(height() - fastforwardbutton->height()) / 2),event->pos(),QRect(w - 5,(height() - fastforwardbutton->height()) / 2,5,128));
            }
            if (StartBar()>0)
            {
                turnbackbutton->Activate(QPointF(0,0),event->pos(),QRect(0,0,25,25));
                fastbackbutton->Activate(QPointF(0,(height() - fastbackbutton->height()) / 2),event->pos(),QRect(5,(height() - fastbackbutton->height()) / 2,5,128));
            }
        }
    }
    if (m_Locked) return;
    const QPointF mappedPos(mapToScene(event->pos()));
    if (!MouseDown)
    {
        if (!SelectRubberband->isVisible())
        {
            const int newStaffIndex = insideStaffId(mappedPos);
            int newBar = Score.insideBarline(mappedPos);
            if (newStaffIndex == -1) newBar=-1;
            const OCSymbolLocation& newLocation = Score.insideFrame(mappedPos);
            if (!altModifier(event))
            {
                if (newBar > -1)
                {
                    setCursor(Qt::PointingHandCursor);
                    QRectF barX=Score.getBarlineX(newBar);
                    barX.setTop(staffTopScaled(newStaffIndex));
                    barX.setHeight(scaled(ScoreStaffLinesHeight));
                    barX.setRight(Score.getBarlineX(newBar+1).left()+3);
                    HoverRubberband->setGeometry(mapFromSceneRect(barX.adjusted(-3,-3,3,3)));
                    HoverRubberband->show(40);
                    setToolTip("Bar "+QString::number(newBar+1));
                }
                else if (newLocation.Pointer > -1)
                {
                    setCursor(Qt::PointingHandCursor);
                    HoverRubberband->setGeometry(mapFromSceneRect(Score.getFrame(newLocation).TranslateBounding()));
                    HoverRubberband->show(40);
                    const XMLSimpleSymbolWrapper& XMLSymbol=GetSymbol(newLocation);
                    setToolTip("<b>"+XMLSymbol.description()+"</b><br>"+Score.toolTipText(newLocation)+"<br><b>"+ XMLScore.StaffName(newLocation.StaffId)+"</b> Voice <b>"+QString::number(newLocation.Voice+1)+"</b>");
                    sound(newLocation);
                }
                else if (newStaffIndex > -1)
                {
                    setCursor(Qt::PointingHandCursor);
                    HoverRubberband->setGeometry(mapFromSceneRect(QRectF(scaled(ScoreLeftMargin),staffTopScaled(newStaffIndex),scaled(m_SystemLength),scaled(ScoreStaffLinesHeight)).adjusted(-4,-4,4,4)));
                    HoverRubberband->show(40);
                    setToolTip(XMLScore.StaffName(newStaffIndex));
                }
                else
                {
                    setCursor(Qt::ArrowCursor);
                    HoverRubberband->hide();
                    setToolTip(QString());
                }
            }
            else // altKey
            {
                Paint(tsRedrawActiveStave);
                HoverRubberband->hide();
                if ((newLocation.Pointer > -1) && (newLocation.Pointer < VoiceLen(newLocation)))
                {
                    if ((LastSymbol.isValid()) && (GetSymbol(newLocation).IsAnyNote()))
                    {
                        setCursor(QCursor(OCSymbolsCollection::SymbolIcon(LastSymbol).pixmap(32,32)));
                        setToolTip(LastSymbol.name());
                    }
                    else
                    {
                        setCursor(Qt::ArrowCursor);
                        setToolTip(QString());
                    }
                }
                else
                {
                    if (LastNote.isValid())
                    {
                        setCursor(Qt::ArrowCursor);
                        setToolTip(LastNote.name());

                        Cursor.backup();

                        writeAltkeyNote(mappedPos);
                        const OCSymbolLocation oldLocation = Cursor.location();
                        XMLScore.Voice(oldLocation).insertChild(LastNote,oldLocation.Pointer);

                        Paint(tsRedrawActiveStave);
                        XMLScore.Voice(oldLocation).deleteChild(oldLocation.Pointer);

                        Cursor.restore();
                    }
                    else
                    {
                        setCursor(Qt::ArrowCursor);
                        setToolTip(QString());
                    }
                }
            }
        }
    }
    else
    {
        if (MouseArea==MouseOutside)
        {
            if (!altModifier(event)) {
                setCursor(Qt::SizeAllCursor);
            }
            else {
                Paint(tsRedrawActiveStave);
                setCursor(QCursor(OCSymbolsCollection::SymbolIcon(LastSymbol).pixmap(64,64)));
            }
            SelectRubberband->setGeometry(rect());
            SelectRubberband->setWindowGeometry(mapFromSceneRect(m_HoldMappedPos,mappedPos));
            SelectRubberband->show();
            HoverRubberband->hide();
        }
        else if (MouseArea==MouseOnBar)
        {
            QRectF r(m_HoldMappedPos,mappedPos);
            r.setTop(activeStaffTopScaled());
            r.setHeight(scaled(ScoreStaffLinesHeight));
            SelectRubberband->setGeometry(rect());
            SelectRubberband->setWindowGeometry(mapFromSceneRect(r.normalized().adjusted(-4,-4,4,4)));
            SelectRubberband->show();
            setCursor(Qt::SizeHorCursor);
            HoverRubberband->hide();
        }
        else if (MouseArea==MouseOnSymbol)
        {
            Dragging=true;
            setCursor(Qt::PointingHandCursor);
            HoverRubberband->hide();
            const QPointF moved = mappedPos-m_HoldMappedPos;

            QList<XMLSimpleSymbolWrapper> oldVoice;
            const XMLVoiceWrapper currentVoice = XMLScore.Voice(Cursor.location());
            const OCPointerList oldPointers = Cursor.SelectedPointers();
            for (const int& i : oldPointers) oldVoice.append(currentVoice.XMLSimpleSymbol(i).xml()->clone());
            Cursor.backup();

            writeMoveSymbol(moved,event->modifiers());
            Paint(tsRedrawActiveStave);

            Cursor.restore();
            for (int i = 0; i < oldPointers.size(); i++) {
                currentVoice.XMLSimpleSymbol(oldPointers[i]).copy(oldVoice[i].xml());
            }

            HoverRubberband->hide();
            if (CurrentSymbol().IsPitchedNote() && (MouseButton == Qt::RightButton)){
                if (qAbs<qreal>(moved.x()) >= qAbs<qreal>(moved.y())){
                    CursorFrame->showAnimated(CurrentFrame().TranslateAccidental());
                }
                else {
                    CursorFrame->showAnimated(CurrentFrame().TranslateTie());
                }
            }
            else {
                CursorFrame->showAnimated(CurrentFrame().TranslateBounding());
            }
        }
    }
}

void ScoreViewXML::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "Release";
    if (m_Locked) return;
    Dragging=false;
    setCursor(Qt::ArrowCursor);
    HoverRubberband->hide();
    if (MouseDown)
    {
        const QPointF mappedPos(mapToScene(event->pos()));
        QPoint popupPoint=QCursor::pos();
        if (MouseArea==MouseOnSymbol) popupPoint=mapToGlobal(QGraphicsView::mapFromScene(CurrentFrame().TranslateBounding().bottomRight()));
        CursorFrame->hideAnimated();
        if (mappedPos != m_HoldMappedPos)
        {
            if ((MouseArea==MouseOutside) || (MouseArea==MouseOnBar))
            {
                if (MouseArea==MouseOutside)
                {
                    SelectRubberband->setWindowGeometry(mapFromSceneRect(m_HoldMappedPos,mappedPos));
                }
                else
                {
                    QRectF r(m_HoldMappedPos,mappedPos);
                    r.setTop(activeStaffTopScaled());
                    r.setHeight(scaled(ScoreStaffLinesHeight));
                    SelectRubberband->setWindowGeometry(mapFromSceneRect(r.normalized().adjusted(-4,-4,4,4)));
                }
                QRect r=SelectRubberband->windowGeometry();
                popupPoint=mapToGlobal(r.bottomRight());
                OCPointerList Ptrs;
                if (MouseArea==MouseOnBar)
                {
                    r.setTop(0);
                    r.setHeight(int(Scene->height()*Size()));
                    Ptrs=Score.pointersInsideVoice(mapToSceneRect(r),Cursor.location());
                }
                else
                {
                    Ptrs=Score.pointersInsideVoice(mapToSceneRect(r),Cursor.location());
                    if (Ptrs.empty())
                    {
                        const OCLocationList p1 = Score.locationsInside(mapToSceneRect(r));
                        if (!p1.empty())
                        {
                            Ptrs.append(p1.matchingVoice(p1.first()).pointers());
                            if (!Ptrs.empty())
                            {
                                setActiveVoiceLocation(p1.first());
                                Paint(tsVoiceIndexChanged);
                                emit StaffIndexChanged(Cursor.location().StaffId);
                            }
                        }
                    }
                }
                if (!Ptrs.isEmpty())
                {
                    if (event->modifiers() == Qt::ShiftModifier) {
                        Cursor.ExtendSel(Ptrs,VoiceLen());
                    }
                    else if (event->modifiers() == Qt::ControlModifier) {
                        Cursor.AddSel(Ptrs,VoiceLen());
                    }
                    else if (altModifier(event))
                    {
                        if (LastSymbol.isValid())
                        {
                            writeAltKeySymbol(Ptrs);
                            Paint(tsReformat);
                            emit ScoreChanged();
                        }
                        else {
                            Cursor.SetSel(Ptrs,VoiceLen());
                        }
                    }
                    else {
                        Cursor.SetSel(Ptrs,VoiceLen());
                    }
                }
                else
                {
                    Cursor.SetZero(VoiceLen());
                }
            }
            else if (MouseArea==MouseOnSymbol)
            {
                emit BackMeUp("Drag");
                writeMoveSymbol(mappedPos - m_HoldMappedPos, event->modifiers());
            }
            SelectRubberband->hide();
        }
        else
        {
            if ((MouseArea != MouseOnSymbol) && altModifier(event))
            {
                if (LastNote.isValid())
                {
                    writeAltkeyNote(mappedPos);
                    emit RequestSymbol(LastNote,"Paste Note");

                    Cursor.SetPos(Cursor.currentPointer()-1);
                    Paint(tsReformat);
                    emit ScoreChanged();
                }
            }
            else if (MouseArea==MouseOnBar)
            {
                QRectF barX=Score.getBarlineX(MouseAreaIndex);
                barX.setTop(activeStaffTopScaled());
                barX.setHeight(scaled(ScoreStaffLinesHeight));
                barX.setRight(Score.getBarlineX(MouseAreaIndex+1).left()+3);
                popupPoint=mapToGlobal(QGraphicsView::mapFromScene(barX.bottomRight()));
                const OCSymbolRange r(findPointerToBar(MouseAreaIndex).Pointer,findPointerToBar(MouseAreaIndex+1).Pointer-1);
                if (r.End-r.Start > -1)
                {
                    if (event->modifiers()==Qt::ShiftModifier)  {
                        Cursor.ExtendSel(r,VoiceLen());
                    }
                    else if (event->modifiers()==Qt::ControlModifier) {
                        Cursor.AddSel(r,VoiceLen());
                    }
                    else {
                        Cursor.SetRange(r,VoiceLen());
                    }
                }
                else
                {
                    Cursor.SetZero(VoiceLen());
                }
            }
            else if (MouseArea == MouseOutside)
            {
                if (insideStaffId(mappedPos)==Cursor.location().StaffId)
                {
                    if (Score.nearestLocation(mappedPos.x(),Cursor.location()).Pointer == -1) {
                        Cursor.SetZero(VoiceLen());
                        if (FollowResize()==PageSizeUnlimited) scrollToBar(BarMap().barCount(Cursor.location())-1);
                    }
                    else {
                        Cursor.SetRange(0,VoiceLen(),VoiceLen());
                    }
                }
                else
                {
                    Cursor.SetZero(VoiceLen());
                    if (FollowResize()==PageSizeUnlimited) scrollToBar(BarMap().barCount(Cursor.location())-1);
                }
            }
        }
        Paint(tsRedrawActiveStave);
        emit Changed();
        emit SelectionChanged();
        MouseDown=false;
        if (mappedPos == m_HoldMappedPos)
        {
            if (MouseButton==Qt::RightButton) emit Popup(popupPoint);
            if (event->modifiers()  == Qt::ControlModifier) emit ListPopup(popupPoint);
        }
        else
        {
            if (Cursor.SelCount())
            {
                if (MouseArea == MouseOnBar)
                {
                    if (event->modifiers() == Qt::ShiftModifier) emit Popup(popupPoint);
                    if (event->modifiers() == Qt::ControlModifier) emit PropertiesPopup(popupPoint);
                }
                if (MouseArea == MouseOutside)
                {
                    if (event->modifiers() == Qt::ShiftModifier) emit Popup(popupPoint);
                    if (event->modifiers() == Qt::ControlModifier) emit PropertiesPopup(popupPoint);
                }
            }
        }
    }
}

void ScoreViewXML::ensureVisible(const int iStaff)
{
    const auto i = int(staffTopScaled(iStaff));
    verticalScrollBar()->setValue(i-(height()/3));
    if (HoverRubberband->isVisible())
    {
        HoverRubberband->hide();
        setToolTip(QString());
        setCursor(Qt::ArrowCursor);
    }
}

void ScoreViewXML::ensureVisible()
{
    ensureVisible(Cursor.location().StaffId);
}

void ScoreViewXML::scrollToBar(const int Bar)
{
    const int x = Score.getBarlineX(Bar).x()*zoomer->getZoom();
    if (x > horizontalScrollBar()->maximum())
    {
        horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
    }
    else if (x < 0)
    {
        horizontalScrollBar()->setValue(0);
    }
    else
    {
        horizontalScrollBar()->setValue(x);
    }
}

int ScoreViewXML::BarsActuallyDisplayed() const
{
    return Score.barsActuallyPrinted();
}

void ScoreViewXML::play(const int PlayFromBar, const int Silence, const QString& Path)
{
    Score.play(PlayFromBar, Silence, Path);
}

const QByteArray ScoreViewXML::MIDIPointer(const int PlayFromBar, const int Silence)
{
    return Score.MIDIPointer(PlayFromBar, Silence);
}

void ScoreViewXML::nextStaff(const int Direction)
{
    SelectRubberband->hide();
    const int newstaffId=boundRoll(0, Cursor.location().StaffId + Direction, XMLScore.NumOfStaffs()-1);
    setActiveStaffId(newstaffId);
    Paint(tsVoiceIndexChanged);
    emit StaffIndexChanged(Cursor.location().StaffId);
    emit SelectionChanged();
    ensureVisible();
}

int ScoreViewXML::activeStaffPos() const { return ActiveTemplate.staffPosFromId(Cursor.location().StaffId); }

double ScoreViewXML::scaled(const double v) const { return v/Size(); }

double ScoreViewXML::staffTopScaled(int id) const {
    return scaled(ActiveTemplate.staffTopFromId(id));
}

double ScoreViewXML::activeStaffTopScaled() const {
    return scaled(ActiveTemplate.staffTopFromId(ActiveStaffId()));
}

int ScoreViewXML::findCurrentMeter(const OCSymbolLocation& SymbolLocation) const
{
    OCBarSymbolLocation Bar=Score.BarMap().GetBar(SymbolLocation);
    return Score.BarMap().GetMeter(Bar);
}

const QVector<ulong> ScoreViewXML::tickList() const
{
    QVector<ulong> retval;
    int StaffPos = ActiveTemplate.staffPosFromId(0);
    ulong tickCount=0;
    for (int i=0;i<Score.BarMap().barCount(OCVoiceLocation(StaffPos));i++)
    {
        retval.push_back(tickCount);
        OCBarSymbolLocation Bar(i,StaffPos,0);
        tickCount+=uint(Score.BarMap().GetMeter(Bar)*10);
    }
    retval.push_back(tickCount);
    return retval;
}

const QRectF ScoreViewXML::systemRect() const
{
    const double w = scaled(m_SystemLength);
    return QRectF(scaled(ScoreLeftMargin),0,w,scaled(ActiveTemplate.staffTop(ActiveTemplate.staffCount() - 1) + 800) + 30);
}

void ScoreViewXML::createSceneRect()
{
    QRectF r(systemRect());
    r.setX(0);
    r.setWidth(scaled(m_SystemLength + (ScoreLeftMargin * 2)));
    setSceneRect(r);
}

void ScoreViewXML::pasteSymbol(XMLSimpleSymbolWrapper& Symbol)
{
    if (!Symbol.IsRestOrAnyNote())
    {
        if (!Symbol.compare(LastSymbol)) LastSymbol.copy(Symbol.xml());
    }
    else if (!Symbol.IsAnyNote())
    {
        if (!Symbol.compare(LastNote)) LastNote.copy(Symbol.xml());
    }
    if (Cursor.SelCount() > 1) Delete();
    insertSymbol(Cursor.currentPointer(),Symbol);
    Cursor.SetPos(Cursor.currentPointer()+1,VoiceLen());
    //Cursor.MaxSel(VoiceLen());
    int nextBar = StartBar()+BarsActuallyDisplayed();
    if (Cursor.SelEnd() > findPointerToBar(nextBar).Pointer)
    {
        if (Cursor.SelEnd() == VoiceLen()) nextBar--;
        setStartBar(nextBar);
        emit BarChanged();
    }
}

void ScoreViewXML::insertSymbol(const OCSymbolLocation &SymbolLocation, XMLSimpleSymbolWrapper &Symbol)
{
    if (SymbolLocation.Voice > 0) {
        if (Symbol.IsRest() && Symbol.noteValue() == 7) Symbol.setVisible(false);
    }
    XMLScore.Voice(SymbolLocation).insertChild(Symbol,SymbolLocation.Pointer);
    m_XMLLastPasted.clear();
    m_XMLLastPasted.addChild(Symbol);
}

void ScoreViewXML::insertSymbol(const int Pointer, XMLSimpleSymbolWrapper &Symbol)
{
    insertSymbol(OCSymbolLocation(Cursor.location(),Pointer),Symbol);
}

void ScoreViewXML::insertSymbol(XMLSimpleSymbolWrapper &Symbol)
{
    insertSymbol(VoiceLen(),Symbol);
}

int ScoreViewXML::VoiceCount(const int Staff) const
{
    return XMLScore.NumOfVoices(Staff);
}

int ScoreViewXML::VoiceCount() const
{
    return XMLScore.NumOfVoices(Cursor.location().StaffId);
}

int ScoreViewXML::StaffCount() const
{
    return XMLScore.NumOfStaffs();
}

void ScoreViewXML::SetXMLScore(XMLScoreWrapper &Doc)
{
    XMLScore.shadowXML(Doc);
}

ImportResult ScoreViewXML::Load(const QString &Path)
{
    const QString s = QFileInfo(Path).suffix();
    if ((s == "mxl") || (s == "musicxml"))
    {
        if (!CMusicXMLReader::parseMXLFile(Path,XMLScore)) return NoImport;
        ReloadXML();
        setActiveStaffId(0);
        return ImportMusicXML;
    }
    if (!XMLScore.Load(Path)) return NoImport;
    ReloadXML();
    setActiveStaffId(0);
    return ImportNativeXML;
}

bool ScoreViewXML::Save(const QString &Path)
{
    return XMLScore.Save(Path);
}

void ScoreViewXML::serialize(QDomLiteElement *xml) const {
    XMLScore.serialize(xml);
}

void ScoreViewXML::unserialize(const QDomLiteElement *xml) {
    XMLScore.unserialize(xml);
    ReloadXML();
}

XMLSimpleSymbolWrapper ScoreViewXML::GetSymbol(const OCSymbolLocation &SymbolLocation) const
{
    return XMLScore.Symbol(SymbolLocation);
}

XMLSimpleSymbolWrapper ScoreViewXML::GetSymbol(const int Pointer) const
{
    return GetSymbol(OCSymbolLocation(Cursor.location(),Pointer));
}

XMLSimpleSymbolWrapper ScoreViewXML::CurrentSymbol() const
{
    const XMLVoiceWrapper& v=XMLScore.Voice(Cursor.location());
    const int i=Cursor.currentPointer();
    if ((i >= v.symbolCount()) || (i < 0)) return XMLSimpleSymbolWrapper();
    return GetSymbol(i);
}

const OCBarMap &ScoreViewXML::BarMap() const
{
    return Score.BarMap();//Score.FillBarsArray(XMLScore);
}

int ScoreViewXML::VoiceLen(const OCVoiceLocation &VoiceLocation) const
{
    return XMLScore.Voice(VoiceLocation).symbolCount();
}

int ScoreViewXML::VoiceLen() const
{
    return XMLScore.Voice(Cursor.location()).symbolCount();
}

int ScoreViewXML::EndOfVoiceBar(const OCVoiceLocation &VoiceLocation) const
{
    return BarMap().EndOfVoiceBar(VoiceLocation);
}

int ScoreViewXML::EndOfVoiceBar() const
{
    return EndOfVoiceBar(Cursor.location());
}

void ScoreViewXML::SetXML(XMLScoreWrapper &NewXML)
{
    XMLScore.setXML(NewXML);
    ReloadXML();
}

void ScoreViewXML::SetXML(QDomLiteDocument *NewXML)
{
    XMLScore.setXML(NewXML);
    ReloadXML();
}

void ScoreViewXML::ReloadXML()
{
    const OCVoiceLocation l=ActiveVoiceLocation();
    setActiveTemplate();
    setActiveOptions();
    Score.assignXML(XMLScore);
    if (l.StaffId > StaffCount()-1) setActiveStaffId(StaffCount()-1);
    if (l.Voice > VoiceCount()-1) setActiveVoice(VoiceCount()-1);
    Cursor.MaxSel(VoiceLen());
}

const QString ScoreViewXML::StaffName(const int Staff) const
{
    return XMLScore.StaffName(Staff);
}

const OCSelectionList ScoreViewXML::SelectionList() const
{
    OCSelectionList RetVal;
    RetVal.append(Cursor.Range());
    return RetVal;
}

const OCSelectionList ScoreViewXML::SelectionList(const int Bar1, const int Bar2, const int Staff1, const int Staff2) const
{
    OCSelectionList RetVal;
    for (int i=Staff1;i<=Staff2;i++)
    {
        OCVoiceLocation v=MarkToVoiceLocation(i);
        int p1=findPointerToBar(v,Bar1).Pointer;
        if (p1 >= VoiceLen(v)) p1 = VoiceLen(v)-1;
        int p2=findPointerToBar(v,Bar2+1).Pointer-1;
        if (p2 >= VoiceLen(v)) p2 = VoiceLen(v)-1;
        RetVal.append(OCSymbolRange(p1,p2));
    }
    return RetVal;
}

int ScoreViewXML::MarkToVoice(const int Mark) const
{
    int s=0;
    int Staff=0;
    forever
    {
        if (Mark < s+VoiceCount(Staff)) return Mark-s;
        s+=VoiceCount(Staff);
        Staff++;
    }
}

int ScoreViewXML::MarkToStaff(const int Mark) const
{
    int s=0;
    int Staff=0;
    forever
    {
        if (Mark < s+VoiceCount(Staff)) return Staff;
        s+=VoiceCount(Staff);
        Staff++;
    }
}

const OCVoiceLocation ScoreViewXML::MarkToVoiceLocation(const int Mark) const
{
    return OCVoiceLocation(MarkToStaff(Mark),MarkToVoice(Mark));
}

int ScoreViewXML::VoiceLocationToMark(const OCVoiceLocation &v) const
{
    int t=0;
    for (int i=0;i<v.StaffId;i++) t+=VoiceCount(i);
    for (int j=0;j<v.Voice;j++) t++;
    return t;
}

bool ScoreViewXML::navigationVisible() const
{
    return m_NavigationVisible;
}

const OCBarSymbolLocationList ScoreViewXML::search(const QString& SearchTerm, const int Staff, const int Voice)
{
    return Score.search(SearchTerm,Staff,Voice);
}

void ScoreViewXML::setNavigationVisible(const bool newShowNavigation)
{
    m_NavigationVisible=newShowNavigation;
}

void ScoreViewXML::setActiveTemplate()
{
    ActiveTemplate=XMLScore.Template;
}

void ScoreViewXML::setActiveTemplate(const XMLTemplateWrapper &XMLTemplate)
{
    ActiveTemplate=XMLTemplate;
}

void ScoreViewXML::setActiveOptions()
{
    ActiveOptions=XMLScore.ScoreOptions;
}

void ScoreViewXML::setActiveOptions(const XMLScoreOptionsWrapper &XMLOptions)
{
    ActiveOptions=XMLOptions;
}

void ScoreViewXML::setFollowResize(const PageMode NewFollowResize)
{
    ActiveOptions.setFollowResize(NewFollowResize);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void ScoreViewXML::setActiveStaffId(const int id)
{
    if (Cursor.location().StaffId != id)
    {
        OCVoiceLocation v(id,0);
        Cursor.SetZero(OCSymbolLocation(v,VoiceLen(v)));
    }
}

void ScoreViewXML::setActiveVoice(int NewActiveVoice)
{
    if (Cursor.location().Voice != NewActiveVoice)
    {
        OCVoiceLocation v(Cursor.location().StaffId,NewActiveVoice);
        Cursor.SetZero(OCSymbolLocation(v,VoiceLen(v)));
    }
}

void ScoreViewXML::setActiveVoiceLocation(const OCVoiceLocation &v)
{
    if (ActiveStaffId() != v.StaffId) setActiveStaffId(v.StaffId);
    if (ActiveVoice() != v.Voice) setActiveVoice(v.Voice);
}

void ScoreViewXML::setActiveBarLocation(const OCBarLocation &b)
{
    setActiveVoiceLocation(b);
    if (FollowResize() != PageSizeUnlimited)
    {
        if (StartBar() != b.Bar) setStartBar(b.Bar);
    }
}

void ScoreViewXML::setBarNrOffset(const int NewBarNrOffset)
{
    ActiveOptions.setBarNumberOffset(NewBarNrOffset);
}

void ScoreViewXML::setMasterStaff(const int NewMasterStaff)
{
    ActiveOptions.setMasterStaff(NewMasterStaff);
}

void ScoreViewXML::setNoteSpace(const int NewNoteSpace)
{
    ActiveOptions.setNoteSpace(NewNoteSpace);
}

void ScoreViewXML::setStartBar(const int NewStartBar)
{
    m_StartBar = NewStartBar;
}

void ScoreViewXML::setEndBar(const int NewEndBar)
{
    m_EndBar = NewEndBar;
}

void ScoreViewXML::setSize(const double NewSize)
{
    ActiveOptions.setSize(NewSize);
}

void ScoreViewXML::setLocked(const bool NewLocked)
{
    m_Locked=NewLocked;
    this->setMouseTracking(!NewLocked);
}

void ScoreViewXML::setHideBarNumbers(const bool NewHideBarNumbers)
{
    ActiveOptions.setHideBarNumbers(NewHideBarNumbers);
}
