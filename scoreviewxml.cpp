#include "scoreviewxml.h"
#include "ui_scoreviewxml.h"
//#include <QGLWidget>

float signedSqrt(float value)
{
    return Sgn(value)*qSqrt(Abs(value));
}

void ScoreViewXML::zeroSwipe()
{
    if (swipeDelta)
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

void ScoreViewXML::swipeProc(int value)
{
    this->scroll(value-swipeBackPos,0);
    swipeBackPos=value;
}

void ScoreViewXML::turnpage()
{
    setStartBar(StartBar()+BarsActuallyDisplayed());
    Paint(tsNavigate);
    emit BarChanged();
    if (!CanTurnPage()) turnpagebutton->MouseLeave(0);
}

void ScoreViewXML::leaveEvent(QEvent *event)
{
    if (HoverRubberband->isVisible())
    {
        HoverRubberband->hide();
        setToolTip(QString());
        setCursor(Qt::ArrowCursor);
    }
    zeroSwipe();
    QGraphicsView::leaveEvent(event);
}

bool ScoreViewXML::viewportEvent(QEvent *event)
{
    //qDebug() << event;
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
    if (HoverRubberband->isVisible())
    {
        HoverRubberband->hide();
        setToolTip(QString());
        setCursor(Qt::ArrowCursor);
    }
    if (this->FollowResize())
    {
        if (ScrollTimer.isActive()) return;
        if (swipeLine.state()==QTimeLine::Running) return;
        if (event->orientation()==Qt::Horizontal)
        {
            if (touchDown)
            {
                if ((event->delta()>40) & (StartBar()>0))
                {
                    ScrollTimer.setSingleShot(true);
                    ScrollTimer.start(1000);
                    touchDown=false;
                    this->scroll(-swipePos,0);
                    swipeDelta=0;
                    swipePos=0;
                    emit SwipeLeftToRight();
                    swipeDelta=0;
                    swipePos=0;
                    event->accept();
                    return;
                }
                if ((event->delta()<-40) & (CanTurnPage()))
                {
                    ScrollTimer.setSingleShot(true);
                    ScrollTimer.start(1000);
                    touchDown=false;
                    this->scroll(-swipePos,0);
                    swipeDelta=0;
                    swipePos=0;
                    emit SwipeRightToLeft();
                    swipeDelta=0;
                    swipePos=0;
                    event->accept();
                    return;
                }
                else
                {
                    float f=signedSqrt(swipeDelta+event->delta());
                    int i=qRound(f)-qRound((float)swipePos);
                    swipePos+=i;
                    this->scroll(i,0);
                    swipeDelta+=event->delta();
                    event->accept();
                    return;
                }
            }
        }
    }
    QGraphicsView::wheelEvent(event);
}

void ScoreViewXML::turnback()
{
    Score.PageBackFormat(XMLScore,m_ActiveTemplate);
    setStartBar(Score.StartBar());
    Paint(tsNavigate);
    emit BarChanged();
    if (StartBar()<=0) turnbackbutton->MouseLeave(0);
}

void ScoreViewXML::fastforward()
{
    setStartBar(EndOfVoiceBar());
    Paint(tsNavigate);
    emit BarChanged();
    fastforwardbutton->MouseLeave(0);
}

void ScoreViewXML::fastback()
{
    setStartBar(0);
    Paint(tsNavigate);
    emit BarChanged();
    fastbackbutton->MouseLeave(0);
}

const bool ScoreViewXML::CanTurnPage() const
{
    if (this->FollowResize())
    {
        return (EndOfVoiceBar() >= StartBar() + BarsActuallyDisplayed()) & (BarsActuallyDisplayed()>0);
    }
    return (BarMap().BarCountAll(m_ActiveTemplate) > StartBar() + BarsActuallyDisplayed());
}

ScoreViewXML::ScoreViewXML(QWidget* parent) : QGraphicsView(parent),
ui(new Ui::ScoreViewXML)
{
    ui->setupUi(this);
    this->viewport()->setAttribute(Qt::WA_AcceptTouchEvents,true);
    CurrentSymbol=0;
    swipeDelta=0;
    swipePos=0;
    swipeBackPos=0;
    swipeLine.setDuration(200);
    swipeLine.setEasingCurve(QEasingCurve::InCurve);
    connect(&swipeLine, SIGNAL(frameChanged(int)), this, SLOT(swipeProc(int)));
    setAutoFillBackground(true);
    SelectRubberband=new QiPhotoRubberband(this);
    HoverRubberband=new QHoverRubberband(QRubberBand::Rectangle,this);
    SelectRubberband->hide();
    HoverRubberband->hide();
    m_XMLLastPasted.clear("Voice");
    turnpagebutton=new QHoverButton(this,QIcon(":/turnpage.png"),QSize(96,96),QHoverButton::TopRight);
    connect(turnpagebutton,SIGNAL(clicked()),this,SIGNAL(NavigationForwardClicked()));
    turnbackbutton=new QHoverButton(this,QIcon(":/turnback.png"),QSize(96,96),QHoverButton::LeftTop);
    connect(turnbackbutton,SIGNAL(clicked()),this,SIGNAL(NavigationBackClicked()));
    fastforwardbutton=new QHoverButton(this,QIcon(":/fast-forward.png"),QSize(128,128),QHoverButton::Right);
    connect(fastforwardbutton,SIGNAL(clicked()),this,SIGNAL(NavigationEndClicked()));
    fastbackbutton=new QHoverButton(this,QIcon(":/fast-back.png"),QSize(128,128),QHoverButton::Left);
    connect(fastbackbutton,SIGNAL(clicked()),this,SIGNAL(NavigationHomeClicked()));
    connect(&soundTimer,SIGNAL(timeout()),this,SLOT(SoundOff()));

    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
    setRenderHints(renderinghints);
    this->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
    Scene = new QGraphicsScene(this);
    Scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(Scene);
    ScreenObj.Scene=Scene;
    ScreenObj.Cursor = &Cursor;
    Frame=new OCFrame(this);
    MouseDown=false;
    toneon=false;
    m_Locked=false;
    m_NavigationVisible=false;
    setMouseTracking(true);
    d2pitch=0;

    XMLScore.newScore();
    setFollowResize(true);
    setBarNrOffset(0);
    setHideBarNumbers(false);
    setMasterStaff(0);
    setNoteSpace(16);
    setStartBar(0);
    setSize(12);
    setActiveTemplate(0);
    setActiveStaff(0);
    setActiveVoice(0);
    setEndBar(0);
    m_SystemLength = (900 * 12);
    Cursor.SetPos(0);
    Cursor.SetZero();
    setSceneRect(0,0,800,SceneRect().height());

    connect(setAction(QKeySequence::MoveToNextChar),SIGNAL(triggered()),this,SLOT(selectNextSymbol()));
    connect(setAction(QKeySequence::MoveToPreviousChar),SIGNAL(triggered()),this,SLOT(selectPrevSymbol()));
    connect(setAction(QKeySequence::SelectNextChar),SIGNAL(triggered()),this,SLOT(selectNextSymbolExtend()));
    connect(setAction(QKeySequence::SelectPreviousChar),SIGNAL(triggered()),this,SLOT(selectPrevSymbolExtend()));
    connect(setAction(QKeySequence::MoveToStartOfLine),SIGNAL(triggered()),this,SLOT(selectHome()));
    connect(setAction(QKeySequence::MoveToEndOfLine),SIGNAL(triggered()),this,SLOT(selectEnd()));
    connect(setAction(QKeySequence::SelectStartOfLine),SIGNAL(triggered()),this,SLOT(selectHomeExtend()));
    connect(setAction(QKeySequence::SelectEndOfLine),SIGNAL(triggered()),this,SLOT(selectEndExtend()));
    connect(setAction(QKeySequence::MoveToStartOfBlock),SIGNAL(triggered()),this,SLOT(selectPrevStaff()));
    connect(setAction(QKeySequence::MoveToEndOfBlock),SIGNAL(triggered()),this,SLOT(selectNextStaff()));

    connect(setAction(QKeySequence(Qt::CTRL +Qt::ALT + Qt::Key_Up)),SIGNAL(triggered()),this,SLOT(selectPrevVoice()));
    connect(setAction(QKeySequence(Qt::CTRL +Qt::ALT + Qt::Key_Down)),SIGNAL(triggered()),this,SLOT(selectNextVoice()));

    connect(setAction(QKeySequence::MoveToNextPage),SIGNAL(triggered()),this,SLOT(turnpage()));
    connect(setAction(QKeySequence::MoveToPreviousPage),SIGNAL(triggered()),this,SLOT(turnback()));
    connect(setAction(QKeySequence::SelectNextWord),SIGNAL(triggered()),this,SLOT(selectSwapForward()));
    connect(setAction(QKeySequence::SelectPreviousWord),SIGNAL(triggered()),this,SLOT(selectSwapBack()));
    connect(setAction(QKeySequence::MoveToNextLine),SIGNAL(triggered()),this,SLOT(selectPitchDown()));
    connect(setAction(QKeySequence::MoveToPreviousLine),SIGNAL(triggered()),this,SLOT(selectPitchUp()));
    connect(setAction(QKeySequence::SelectNextLine),SIGNAL(triggered()),this,SLOT(selectOctaveDown()));
    connect(setAction(QKeySequence::SelectPreviousLine),SIGNAL(triggered()),this,SLOT(selectOctaveUp()));
    connect(setAction(Qt::Key_Backspace),SIGNAL(triggered()),this,SLOT(selectBackSpace()));
    connect(setAction(QKeySequence::Delete),SIGNAL(triggered()),this,SLOT(selectDelete()));
    Paint(tsReformat);
}

ScoreViewXML::~ScoreViewXML()
{
    Score.EraseAll(Scene);
    delete Scene;
    delete Frame;
    if (CurrentSymbol != 0) delete CurrentSymbol;
    delete ui;
}

QAction* ScoreViewXML::setAction(const QKeySequence keySequence)
{
    QAction* a=new QAction(this);
    a->setShortcut(keySequence);
    a->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    this->addAction(a);
    return a;
}

const int ScoreViewXML::StaffPos(const int Staff) const
{
    return XMLScore.StaffPos(m_ActiveTemplate,Staff);
}

void ScoreViewXML::MakeBackup(const QString& Text)
{
    emit BackMeUp(Text);
}

const bool ScoreViewXML::Locked() const
{
    return m_Locked;
}

void ScoreViewXML::setLocked(const bool NewLocked)
{
    m_Locked=NewLocked;
    this->setMouseTracking(!NewLocked);
}

const bool ScoreViewXML::navigationVisible() const
{
    return m_NavigationVisible;
}

void ScoreViewXML::setNavigationVisible(const bool newShowNavigation)
{
    m_NavigationVisible=newShowNavigation;
}

void ScoreViewXML::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Shift) || (event->key() == Qt::Key_Control)) return;
    SelectRubberband->hide();
}

void ScoreViewXML::selectNextSymbolExtend()
{
    SelectRubberband->hide();
    if (Cursor.SelCount()==0)
    {
        if (Cursor.SelEnd() + 1 < VoiceLen()) Cursor.ExtendSel(Cursor.SelEnd());
    }
    else
    {
        if (Cursor.SelEnd() + 1 < VoiceLen()-1) Cursor.ExtendSel(Cursor.SelEnd() + 1);
    }
    Cursor.MaxSel(VoiceLen() - 1);
    if ((Cursor.SelEnd() > FindPointerToBar(StartBar() + BarsActuallyDisplayed())) && (m_FollowResize))
    {
        setStartBar(StartBar()+BarsActuallyDisplayed());
        Paint(tsNavigate);
        emit BarChanged();
    }
    else
    {
        Paint(tsRedrawActiveStave);
    }
    Sound();
    emit SelectionChanged();
}

void ScoreViewXML::selectPrevSymbolExtend()
{
    SelectRubberband->hide();
    if (Cursor.SelStart() > 0)
    {
        if (Cursor.SelCount()==0)
        {
            Cursor.SetPos(Cursor.GetPos()-1);
            Cursor.ExtendSel(Cursor.GetPos());
        }
        else
        {
            Cursor.ExtendSel(Cursor.SelStart() - 1);
        }
        Cursor.MaxSel(VoiceLen() - 1);
        if ((Cursor.SelStart() < PointerBegin()) && (m_FollowResize))
        {
            setStartBar(StartBar()-1);
            Paint(tsNavigate);
            emit BarChanged();
        }
        else
        {
            Paint(tsRedrawActiveStave);
        }
        Sound();
        emit SelectionChanged();
    }
}

void ScoreViewXML::selectNextSymbol()
{
    SelectRubberband->hide();
    if (Cursor.GetPos() < VoiceLen() - 1)
    {
        Cursor.SetPos(Cursor.GetPos()+1);
        Cursor.MaxSel(VoiceLen() - 1);
        if (Cursor.GetPos() >= VoiceLen() - 1)
        {
            Cursor.SetZero();
        }
        if ((Cursor.GetPos() >= FindPointerToBar(StartBar() + BarsActuallyDisplayed())) && (m_FollowResize))
        {
            setStartBar(StartBar()+BarsActuallyDisplayed());
            Paint(tsNavigate);
            emit BarChanged();
        }
        else
        {
            Paint(tsRedrawActiveStave);
        }
        Sound();
        emit SelectionChanged();
    }
}

void ScoreViewXML::selectPrevSymbol()
{
    SelectRubberband->hide();
    if (Cursor.GetPos() > 0)
    {
        Cursor.SetPos(Cursor.GetPos()-1);
        if ((Cursor.GetPos() < PointerBegin()) && (m_FollowResize))
        {
            setStartBar(StartBar()-1);
            Paint(tsNavigate);
            emit BarChanged();
        }
        else
        {
            Paint(tsRedrawActiveStave);
        }
        Sound();
        emit SelectionChanged();
    }
}

void ScoreViewXML::selectEnd()
{
    SelectRubberband->hide();
    if (!m_FollowResize) return;
    setStartBar(EndOfVoiceBar());
    Cursor.SetZero();
    while (Cursor.GetPos() != VoiceLen() - 1)
    {
        Cursor.SetPos(Cursor.GetPos()+1);
    }
    Paint(tsNavigate);
    emit SelectionChanged();
    emit BarChanged();
}

void ScoreViewXML::selectHome()
{
    SelectRubberband->hide();
    if (!m_FollowResize) return;
    setStartBar(0);
    Cursor.SetPos(0);
    Paint(tsNavigate);
    emit SelectionChanged();
    emit BarChanged();
}

void ScoreViewXML::selectEndExtend()
{
    SelectRubberband->hide();
    if (!m_FollowResize) return;
    setStartBar(EndOfVoiceBar());
    Cursor.ExtendSel(VoiceLen()-1);
    Cursor.MaxSel(VoiceLen() - 1);
    Paint(tsNavigate);
    emit SelectionChanged();
    emit BarChanged();
}

void ScoreViewXML::selectHomeExtend()
{
    SelectRubberband->hide();
    if (!m_FollowResize) return;
    setStartBar(0);
    Cursor.ExtendSel(0);
    Cursor.MaxSel(VoiceLen()-1);
    Paint(tsNavigate);
    emit SelectionChanged();
    emit BarChanged();
}

void ScoreViewXML::selectNextStaff()
{
    NextStaff(1);
}

void ScoreViewXML::selectPrevStaff()
{
    NextStaff(-1);
}

void ScoreViewXML::selectNextVoice()
{
    SelectRubberband->hide();
    setActiveVoice(KeepIn(m_ActiveVoice + 1,0,VoiceCount()-1));
    Cursor.MaxSel(VoiceLen()-1);
    Paint(tsVoiceIndexChanged);
    emit ActiveStaffChange(m_ActiveStaff);
    emit SelectionChanged();
}

void ScoreViewXML::selectPrevVoice()
{
    SelectRubberband->hide();
    setActiveVoice(KeepIn(m_ActiveVoice - 1,0,VoiceCount()-1));
    Cursor.MaxSel(VoiceLen()-1);
    Paint(tsVoiceIndexChanged);
    emit ActiveStaffChange(m_ActiveStaff);
    emit SelectionChanged();
}

void ScoreViewXML::transposeSelected(const int add)
{
    MakeBackup("Pitch");
    int PitchChanged=0;
    const QList<int>& l=Cursor.Pointers();
    for (int lTemp = 0 ; lTemp < l.count() ; lTemp++)
    {
        int Pointer = l[lTemp];
        XMLSimpleSymbolWrapper XMLSymbol=GetSymbol(Pointer);
        if (XMLSymbol.IsAnyNote())
        {
            int iTemp1=XMLSymbol.getVal("Pitch");
            if (iTemp1>0)
            {
                XMLSymbol.setAttribute("Pitch",Inside(iTemp1 + add, 1, 127, 1));
                PitchChanged = iTemp1+add;
            }
        }
    }
    if (PitchChanged != 0)
    {
        Paint(tsRedrawActiveStave);
        Sound();
        emit SelectionChanged();
    }
    emit Changed();
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

void ScoreViewXML::selectBackSpace()
{
    SelectRubberband->hide();
    if (!m_FollowResize) return;
    if (Cursor.SelCount()==0)
    {
        //Back Delete
        if (Cursor.SelStart() > 0)
        {
            MakeBackup("Backspace");
            Cursor.SetPos(Cursor.SelStart() - 1);
            Delete(Cursor.GetPos());// SelectedVoice.Symbols.Remove Cursor.Pos
            if (Cursor.GetPos() < PointerBegin())
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
        MakeBackup("Delete");
        Delete();
        Paint(tsReformat);
        emit ScoreChanged();
    }
}

void ScoreViewXML::selectDelete()
{
    SelectRubberband->hide();
    if (!m_FollowResize) return;
    if (Cursor.SelCount()==0)
    {
        //Delete
        if (Cursor.SelEnd() < VoiceLen()-1)
        {
            MakeBackup("Delete");
            Delete(Cursor.GetPos());// SelectedVoice.Symbols.Remove Cursor.Pos
            Paint(tsReformat);
            emit ScoreChanged();
        }
    }
    else
    {
        //Delete selection
        MakeBackup("Delete");
        Delete();
        Paint(tsReformat);
        emit ScoreChanged();
    }
}

void ScoreViewXML::selectSwapForward()
{
    SelectRubberband->hide();
    if (!m_FollowResize) return;
    if (Cursor.GetPos() + 1 < VoiceLen() - 1)
    {
        Cursor.SetPos(Cursor.GetPos()+1);
        if (Cursor.GetPos() < VoiceLen() - 1)
        {
            MakeBackup("Swap");
            QDomLiteElement* Edit = GetClipBoardData(Cursor.GetPos(), Cursor.GetPos());
            Delete(Cursor.GetPos());//SelectedVoice.Symbols.Remove Cursor.Pos
            PasteClipBoardData(Cursor.GetPos() - 1, Edit);
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
    if (!m_FollowResize) return;
    if (Cursor.GetPos() > 0)
    {
        Cursor.SetPos(Cursor.GetPos()-1);
        if (Cursor.GetPos() + 1 != VoiceLen() - 1)
        {
            if (Cursor.GetPos() != VoiceLen() - 1)
            {
                MakeBackup("Swap");
                QDomLiteElement* Edit = GetClipBoardData(Cursor.GetPos(), Cursor.GetPos());
                Delete(Cursor.GetPos());//SelectedVoice.Symbols.Remove Cursor.Pos
                PasteClipBoardData(Cursor.GetPos() + 1, Edit);
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

void ScoreViewXML::Sound()
{
    if (soundTimer.isActive())
    {
        soundTimer.stop();
        SoundOff();
    }
    soundPitch=-1;
    XMLSimpleSymbolWrapper XMLSymbol=GetSymbol();
    if (XMLSymbol.IsAnyNote())
    {
        soundPitch=XMLSymbol.getVal("Pitch");
    }
    if (soundPitch>-1) emit NoteOn(soundPitch);
    soundTimer.setSingleShot(true);
    soundTimer.start(300);
}

void ScoreViewXML::SoundOff()
{
    if (soundPitch>-1) emit NoteOff(soundPitch);
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
    this->viewport()->update();
}

void ScoreViewXML::resizeEvent(QResizeEvent *event)
{
    if (m_FollowResize)
    {
        int WindowWidth=this->geometry().width();//visibleArea.width();
        if (Scene->width() != WindowWidth)
        {
            Paint(tsReformat);
            return;
        }
    }
    else
    {
        setSceneRect(SceneRect());
    }
    QGraphicsView::resizeEvent(event);
}

void ScoreViewXML::Paint(const OCRefreshMode DrawMode, const bool UpdateSelection)
{
    SelectRubberband->hide();
    this->setUpdatesEnabled(false);
    if (DrawMode != tsRedrawActiveStave)
    {
        Score.EraseAll(Scene);
        int picheight = SceneRect().height();
        if (m_FollowResize)
        {
            int WindowWidth=this->geometry().width();//visibleArea.width();
            if (picheight>this->geometry().height()) WindowWidth-=this->verticalScrollBar()->width();
            setSceneRect(0,0,WindowWidth,picheight);
            m_SystemLength = (WindowWidth - 40) * XMLScore.getVal("Size");
        }
        else
        {
            setSceneRect(SceneRect());
        }
        if ((DrawMode == tsReformat) || (DrawMode == tsNavigate) || (DrawMode == tsVoiceIndexChanged))
        {
            if (DrawMode == tsReformat) Score.CreateBarMap(XMLScore);
            setStartBar(qMin(StartBar(),EndOfVoiceBar()));
            //if (StartBar()>EndOfVoiceBar()) setStartBar(EndOfVoiceBar());
            Score.FormatPage(XMLScore, m_ActiveTemplate, SystemLength(), StartBar(), EndBar());
        }
        QColor col(inactivestaffcolor);
        if (m_Locked) col=activestaffcolor;
        for (int c = 0 ; c < m_ActiveTemplate->childCount() ; c++)
        {
            ScreenObj.SetXY(ScoreLeftMargin,StaffPos(c));
            if (XMLScore.AllTemplateIndex(m_ActiveTemplate,c) != m_ActiveStaff)
            {
                if (c > -1) Score.PlSystem(XMLScore.AllTemplateIndex(m_ActiveTemplate,c), XMLScore, m_ActiveTemplate, col, m_ActiveStaff, m_ActiveVoice, c, ScreenObj);
            }
        }
    }
    Cursor.MaxSel(VoiceLen() - 1);
    Score.FakePlot(m_ActiveStaff, m_ActiveVoice, Cursor.GetPos(), XMLScore, CurrentMIDI);
    ScreenObj.SetXY(ScoreLeftMargin,StaffPos(StaffOrder(m_ActiveStaff)));// '(82 * 12) + (100 * 12 * CLng(osystem.stavenum ))
    if (DrawMode == tsRedrawActiveStave) Score.EraseSystem(m_ActiveStaff,Scene);
    Score.PlSystem(m_ActiveStaff, XMLScore, m_ActiveTemplate, activestaffcolor, m_ActiveStaff, m_ActiveVoice, StaffOrder(m_ActiveStaff), ScreenObj);
    this->viewport()->update();
    this->setUpdatesEnabled(true);
    if (UpdateSelection) emit SelectionChanged();
}

void ScoreViewXML::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void ScoreViewXML::flashSelected()
{
    QPoint zero(mapToScene(0,0).toPoint());
    XMLSimpleSymbolWrapper Symbol=GetSymbol();
    if (!Symbol.IsEndOfVoice())
    {
        OCFrameProperties* fp=Score.GetFrame(Cursor.GetPos());
        Frame->DrawFrame(Symbol, QPointF(),zero, false, 0, fp, ScreenObj);
        Frame->EraseFrame();
    }
}

void ScoreViewXML::putFrame(const QPointF& moved, const int Modifiers)
{
    HoverRubberband->hide();
    QPoint zero(mapToScene(0,0).toPoint());
    QPointF os=moved;
    if (CurrentSymbol->IsAnyNote() && (MouseButton == Qt::RightButton))
    {
        Frame->DrawFrame(*CurrentSymbol, os.toPoint(),zero, true, Modifiers, CurrentFrame, ScreenObj);
    }
    else if ((MouseButton == Qt::RightButton) & (CurrentSymbol->Compare("Slur", "Hairpin")))
    {
        os.setX(0);
        Frame->DrawFrame(*CurrentSymbol, os.toPoint(), zero, false, 100, CurrentFrame, ScreenObj);
    }
    else if (!CurrentSymbol->IsEndOfVoice())
    {
        if (MouseButton!=Qt::RightButton) Frame->DrawFrame(*CurrentSymbol, os.toPoint(),zero, false, Modifiers, CurrentFrame, ScreenObj);
    }
}

void ScoreViewXML::mousePressEvent(QMouseEvent *event)
{
    if (m_Locked) return;
    Dragging=false;
    MouseDown = true;
    MouseButton=event->button();
    QPointF m(mapToScene(event->pos()));
    Holdm=m;
    int newsys = m_ActiveStaff;
    for (int c = 0 ; c < m_ActiveTemplate->childCount() ; c++)
    {
        int staffPos = StaffPos(c);
        if ((m.y() > (staffPos/XMLScore.getVal("Size"))) & (m.y() < ((staffPos+ScoreStaffLinesHeight)/XMLScore.getVal("Size"))))
        {
            newsys = XMLScore.AllTemplateIndex(m_ActiveTemplate,c);
            break;
        }
    }
    if (m_ActiveStaff != newsys)
    {
        HoverRubberband->hide();
        setActiveStaff(newsys);
        setActiveVoice(0);
        ScreenObj.SetXY(ScoreLeftMargin,StaffPos(StaffOrder(m_ActiveStaff)));
        Paint(tsVoiceIndexChanged);
        emit ActiveStaffChange(m_ActiveStaff);
    }
    int CursorPos;
    if (MouseButton==Qt::RightButton)
    {
        CursorPos=Cursor.GetPos();
    }
    else
    {
        SelectRubberband->hide();
        CursorPos= Score.InsideFrame(m.toPoint());
    }
    int EOV = VoiceLen() - 1;
    if (CursorPos == -1) CursorPos = EOV;
    if (CursorPos > EOV) CursorPos = EOV;
    Score.FakePlot(m_ActiveStaff, m_ActiveVoice, CursorPos, XMLScore, CurrentMIDI);
    if (CurrentSymbol != 0) delete CurrentSymbol;
    CurrentSymbol = new XMLSimpleSymbolWrapper(XMLScore.Voice(m_ActiveStaff, m_ActiveVoice),CursorPos);
    if (CurrentSymbol->IsAnyNote())
    {
        d2pitch = CurrentSymbol->getVal("Pitch");
        toneon = true;
    }
    CurrentFrame = Score.GetFrame(CursorPos);
    if (event->modifiers() == Qt::ShiftModifier)
    {
        Cursor.ExtendSel(CursorPos);
        Cursor.MaxSel(EOV);
        emit SelectionChanged();
    }
    else if (event->modifiers() == Qt::ControlModifier)
    {
        if (Cursor.SelCount()==0) Cursor.AddSel(Cursor.GetPos());
        Cursor.AddSel(CursorPos);
        Cursor.MaxSel(EOV);
        emit SelectionChanged();
    }
    else if (event->modifiers() == (Qt::ControlModifier | Qt::AltModifier))
    {
        MakeBackup("Paste");
        XMLScore.Paste1Voice(m_ActiveStaff, m_ActiveVoice, CursorPos, &m_XMLLastPasted);
        Cursor.SetZero();
        Cursor.SetPos(CursorPos);
        Paint(tsReformat);
        emit ScoreChanged();
    }
    else if (MouseButton!=Qt::RightButton)
    {
        Cursor.SetPos(CursorPos);
        if (CursorPos == EOV)
        {
            Cursor.SetZero();
        }
        emit SelectionChanged();
    }
    if (toneon) emit NoteOn(d2pitch);
    if (!(event->modifiers() & Qt::ControlModifier))
    {
        putFrame(QPoint(),event->modifiers());
    }
}

void ScoreViewXML::mouseMoveEvent(QMouseEvent *event)
{
    if (!MouseDown)
    {
        if (m_NavigationVisible)
        {
            if (CanTurnPage())
            {
                int w=this->width()+1;
                if (this->verticalScrollBar()->isVisible()) w-=this->verticalScrollBar()->width();
                turnpagebutton->Activate(QPointF(w-turnpagebutton->width(),0),event->pos(),QRect(w-25,0,25,25));
                fastforwardbutton->Activate(QPointF(w-fastforwardbutton->width(),(this->height()-fastforwardbutton->height())/2),event->pos(),QRect(w-5,(this->height()-fastforwardbutton->height())/2,5,128));
            }
            if (StartBar()>0)
            {
                turnbackbutton->Activate(QPointF(0,0),event->pos(),QRect(0,0,25,25));
                fastbackbutton->Activate(QPointF(0,(this->height()-fastbackbutton->height())/2),event->pos(),QRect(5,(this->height()-fastbackbutton->height())/2,5,128));
            }
        }
    }
    static int pnt=-1;
    if (m_Locked) return;
    int prevpitch=d2pitch;
    QPointF m(mapToScene(event->pos()));
    int TempPointer=-1;
    if (!SelectRubberband->isVisible())
    {
        TempPointer = Score.InsideFrame(m.toPoint());
        if (TempPointer>=VoiceLen()) TempPointer=-1;
        QPoint zero(mapToScene(0,0).toPoint());
        if (TempPointer > -1)
        {
            if (pnt != TempPointer)
            {
                pnt=TempPointer;
                this->setCursor(Qt::PointingHandCursor);
                HoverRubberband->setGeometry(Score.GetFrame(pnt)->TranslateBounding(-zero).toRect());
                HoverRubberband->show(40);
                XMLSimpleSymbolWrapper XMLSymbol=GetSymbol(pnt);
                this->setToolTip("<b>"+OCSymbolsCollection::Description(XMLSymbol)+"</b><br>"+Score.ToolTipText(TempPointer,m_ActiveStaff,m_ActiveVoice)+"<br><b>"+XMLScore.StaffName(XMLScore.TemplateOrderStaff(m_ActiveTemplate,m_ActiveStaff))+"</b> Voice <b>"+QString::number(m_ActiveVoice+1)+"</b>");
            }
        }
        else
        {
            if (pnt>-1)
            {
                pnt=-1;
                this->setCursor(Qt::ArrowCursor);
                HoverRubberband->hide();
                this->setToolTip(QString());
            }
            bool Match=false;
            for (int c = 0 ; c < m_ActiveTemplate->childCount() ; c++)
            {
                if (XMLScore.AllTemplateIndex(m_ActiveTemplate,c) != m_ActiveStaff)
                {
                    int staffPos = StaffPos(c);
                    if ((m.y() > (staffPos/XMLScore.getVal("Size"))) & (m.y() < ((staffPos+ScoreStaffLinesHeight)/XMLScore.getVal("Size"))))
                    {
                        this->setCursor(Qt::PointingHandCursor);
                        HoverRubberband->setGeometry(QRect(ScoreLeftMargin/XMLScore.getVal("Size"),staffPos/XMLScore.getVal("Size"),m_SystemLength/XMLScore.getVal("Size"),ScoreStaffLinesHeight/XMLScore.getVal("Size")).translated(-zero).adjusted(-4,-4,4,4));
                        HoverRubberband->show(20);
                        this->setToolTip(XMLScore.StaffName(m_ActiveTemplate,c));
                        Match=true;
                        break;
                    }
                }
            }
            if (!Match)
            {
                this->setCursor(Qt::ArrowCursor);
                HoverRubberband->hide();
                this->setToolTip(QString());
            }
        }
    }
    if (MouseDown)
    {
        if ((!Dragging) && (pnt==-1) && (MouseButton!=Qt::RightButton))
        {
            SelectRubberband->setGeometry(this->rect());
            SelectRubberband->setWindowGeometry(QRect(mapFromScene(Holdm),mapFromScene(m)).normalized());
            SelectRubberband->show();
            this->setCursor(Qt::CrossCursor);
            HoverRubberband->hide();
        }
        else
        {
            Dragging=true;
            this->setCursor(Qt::PointingHandCursor);
            HoverRubberband->hide();
            QPointF moved=m-Holdm;
            if (toneon)
            {
                if (m != Holdm)
                {
                    if (Abs(moved.x()) < Abs(moved.y()))
                    {
                        d2pitch = CurrentSymbol->getVal("Pitch") - ((moved.y() * XMLScore.getVal("Size")) / 28);
                    }
                }
            }
            if (!(event->modifiers() & Qt::ControlModifier))
            {
                putFrame(moved,event->modifiers());
            }
        }
    }
    if (toneon)
    {
        if (d2pitch != prevpitch)
        {
            emit NoteOff(prevpitch);
            emit NoteOn(d2pitch);
        }
    }
}

void ScoreViewXML::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_Locked) return;
    Dragging=false;
    this->setCursor(Qt::ArrowCursor);
    HoverRubberband->hide();
    if (MouseDown)
    {
        QPointF m(mapToScene(event->pos()));
        if (!(event->modifiers() & Qt::ControlModifier))
        {
            Frame->EraseFrame();
        }
        if (m != Holdm)
        {
            if (SelectRubberband->isVisible())
            {
                SelectRubberband->setWindowGeometry(QRect(mapFromScene(Holdm),mapFromScene(Holdm)).united(QRect(mapFromScene(m),mapFromScene(m))));
                QList<int> Ptrs=Score.PointersInside(QRectF(mapToScene(SelectRubberband->windowGeometry().topLeft()),mapToScene(SelectRubberband->windowGeometry().bottomRight())).toRect());
                if (Ptrs.count())
                {
                    Cursor.SetPos(Ptrs.first());
                    foreach(int i, Ptrs)
                    {
                        Cursor.AddSel(i);  //???
                    }
                    emit SelectionChanged();
                }
                else
                {
                    SelectRubberband->hide();
                }
            }
            else
            {
                MakeBackup("Drag");
                QPointF moved=m-Holdm;
                QPoint qp=(moved*(float)XMLScore.getVal("Size")*SizeFactor(XMLScore.TemplateOrderStaff(m_ActiveTemplate,m_ActiveStaff)->attributeValue("Size"))).toPoint();
                if (MouseButton!=Qt::RightButton)
                {
                    this->setCursor(Qt::PointingHandCursor);
                    HoverRubberband->hide();
                    const QList<int>& ptrs=Cursor.Pointers();
                    for (int lTemp = 0 ; lTemp < ptrs.count() ; lTemp++)
                    {
                        int Pnt = ptrs[lTemp];
                        XMLSimpleSymbolWrapper XMLSymbol=GetSymbol(Pnt);
                        if (!XMLSymbol.IsAnyNote())
                        {
                            XMLSymbol.setAttribute("Left",qp.x() + XMLSymbol.getVal("Left"));
                            if (event->modifiers()==Qt::ShiftModifier) qp.setY(IntDiv(qp.y() / 6,16) * 16*6);
                            XMLSymbol.setAttribute("Top",-qp.y() + XMLSymbol.getVal("Top"));
                        }
                        else
                        {
                            if (Abs(moved.x()) > Abs(moved.y()))
                            {
                                XMLSymbol.setAttribute("Left",qp.x() + XMLSymbol.getVal("Left"));
                            }
                            else
                            {
                                int iTemp = (-qp.y() / 28) + XMLSymbol.getVal("Pitch");
                                XMLSymbol.setAttribute("Pitch",Inside(iTemp, 1, 127, 12));
                            }
                        }
                    }
                }
                else if (MouseButton==Qt::RightButton)
                {
                    if (Abs(moved.x()) > Abs(moved.y()))
                    {
                        const QList<int>& ptrs=Cursor.Pointers();
                        for (int lTemp = 0 ; lTemp < ptrs.count() ; lTemp++)
                        {
                            int Pnt = ptrs[lTemp];
                            XMLSimpleSymbolWrapper XMLSymbol=GetSymbol(Pnt);
                            if (XMLSymbol.IsAnyNote()) XMLSymbol.setAttribute("AccidentalLeft",qp.x() + XMLSymbol.getVal("AccidentalLeft"));
                        }
                    }
                    else
                    {
                        const QList<int>& ptrs=Cursor.Pointers();
                        for (int lTemp = 0 ; lTemp < ptrs.count() ; lTemp++)
                        {
                            int Pnt = ptrs[lTemp];
                            qp.setY(moved.y());
                            XMLSimpleSymbolWrapper XMLSymbol=GetSymbol(Pnt);
                            if (XMLSymbol.Compare("Hairpin")) XMLSymbol.setAttribute("Gap",Inside(-qp.y() + XMLSymbol.getVal("Gap"),-9,9,1));
                            if (XMLSymbol.Compare("Slur")) XMLSymbol.setAttribute("Curve",Inside(-qp.y() + XMLSymbol.getVal("Curve"),-9,9,1));
                        }
                    }
                }
            }
        }
        Paint(tsRedrawActiveStave);
        emit Changed();
        emit SelectionChanged();
        if (toneon) emit NoteOff(d2pitch);
        toneon = false;
        MouseDown=false;
        if (m == Holdm) if (MouseButton==Qt::RightButton) emit Popup(this->cursor().pos());
    }
}

void ScoreViewXML::Delete(const int Staff, const int Voice, const int StartPointer, const int EndPointer)
{
    int Pnt=0;
    if (EndPointer <= StartPointer)
    {
        Pnt = StartPointer;
    }
    else
    {
        Pnt = EndPointer;
        if (Pnt > VoiceLen(Staff, Voice) - 1) Pnt = VoiceLen(Staff, Voice) - 1;
    }
    XMLScore.Clear1Voice(Staff, Voice, StartPointer, Pnt);
    Cursor.SetPos(StartPointer);
}

void ScoreViewXML::Delete(const int StartPointer, const int EndPointer)
{
    Delete(m_ActiveStaff,m_ActiveVoice,StartPointer,EndPointer);
}

void ScoreViewXML::Delete(const int Staff, const int Voice, QList<int>& Pointers)
{
    qSort(Pointers);
    if (Pointers.last() > VoiceLen(Staff, Voice) - 1) Pointers.removeLast();
    XMLScore.Clear1Voice(Staff, Voice, Pointers);
    Cursor.SetPos(Pointers.first());
}

void ScoreViewXML::Delete(QList<int>& Pointers)
{
    Delete(m_ActiveStaff,m_ActiveVoice,Pointers);
}

void ScoreViewXML::Delete()
{
    Delete(Cursor.Selected);
}

void ScoreViewXML::PasteClipBoardData(const int Staff, const int Voice, const int Pointer, QDomLiteElement* ClipBoardData)
{
    m_XMLLastPasted.copy(XMLScore.Paste1Voice(Staff, Voice, Pointer, ClipBoardData));
}

void ScoreViewXML::PasteClipBoardData(const int Pointer, QDomLiteElement* ClipBoardData)
{
    m_XMLLastPasted.copy(XMLScore.Paste1Voice(m_ActiveStaff, m_ActiveVoice, Pointer, ClipBoardData));
}

QDomLiteElement* ScoreViewXML::GetClipBoardData(const int Staff, const int Voice, const int StartPointer, const int EndPointer)
{
    QDomLiteElement* data=new QDomLiteElement("Voice");
    int Pnt = EndPointer;
    if (Pnt == VoiceLen(Staff, Voice) - 1) Pnt --;
    if (Pnt - StartPointer >= 0)
    {
        for (int lTemp = StartPointer ; lTemp <= Pnt ; lTemp++)
        {
            data->appendChild(XMLScore.SymbolClone(Staff,Voice,lTemp));
        }
    }
    return data;
}

QDomLiteElement* ScoreViewXML::GetClipBoardData(const int StartPointer, const int EndPointer)
{
    return GetClipBoardData(m_ActiveStaff,m_ActiveVoice,StartPointer,EndPointer);
}

QDomLiteElement* ScoreViewXML::GetClipBoardData(const int Staff,const int Voice, const QList<int>& Pointers)
{
    QDomLiteElement* data=new QDomLiteElement("Voice");
    for (int lTemp = 0 ; lTemp < Pointers.count() ; lTemp++)
    {
        data->appendChild(XMLScore.SymbolClone(Staff,Voice,Pointers[lTemp]));
    }
    return data;
}

QDomLiteElement* ScoreViewXML::GetClipBoardData(const QList<int>& Pointers)
{
    return GetClipBoardData(m_ActiveStaff,m_ActiveVoice,Pointers);
}

QDomLiteElement* ScoreViewXML::GetClipBoardData()
{
    return GetClipBoardData(Cursor.Selected);
}

void ScoreViewXML::EnsureVisible(const int iStaff)
{
    int lTemp = StaffPos(StaffOrder(iStaff)) / XMLScore.getVal("Size");
    this->verticalScrollBar()->setValue(lTemp-(this->height()/3));
    if (HoverRubberband->isVisible())
    {
        HoverRubberband->hide();
        setToolTip(QString());
        setCursor(Qt::ArrowCursor);
    }
}

void ScoreViewXML::EnsureVisible()
{
    EnsureVisible(m_ActiveStaff);
}

const int ScoreViewXML::StartBar() const
{
    return m_StartBar;
}

void ScoreViewXML::setStartBar(const int NewStartBar)
{
    m_StartBar = NewStartBar;
}

const int ScoreViewXML::Size() const
{
    return XMLScore.getVal("Size");
}

void ScoreViewXML::setSize(const int NewSize)
{
    XMLScore.setAttribute("Size",NewSize);
}

const int ScoreViewXML::PointerBegin(const int Staff, const int Voice) const
{
    return BarMap().GetPointer(StartBar(),Staff,Voice);
}

const int ScoreViewXML::PointerBegin() const
{
    return PointerBegin(m_ActiveStaff, m_ActiveVoice);
}

const int ScoreViewXML::BarsActuallyDisplayed() const
{
    return Score.ActuallyPrinted();
}

const int ScoreViewXML::FindPointerToBar(const int Staff, const int Voice, const int BarToFind) const
{
    return Score.BarMap().GetPointer(BarToFind,Staff,Voice);
}

const int ScoreViewXML::FindPointerToBar(const int BarToFind) const
{
    return FindPointerToBar(m_ActiveStaff,m_ActiveVoice,BarToFind);
}

void ScoreViewXML::Play(const int PlayFromBar, const int Silence, const QString& Path, const int Staff)
{
    OCMIDIFile MFile;
    Score.Play(PlayFromBar, XMLScore, Silence, MFile, Path, Staff);
}

const QByteArray ScoreViewXML::MIDIPointer(const int PlayFromBar, const int Silence)
{
    OCMIDIFile MFile;
    return Score.MIDIPointer(PlayFromBar, XMLScore, Silence, MFile);
}

void ScoreViewXML::NextStaff(const int Direction)
{
    SelectRubberband->hide();
    setActiveStaff(KeepIn(m_ActiveStaff + Direction,0,XMLScore.NumOfStaffs()-1));
    setActiveVoice(0);
    Cursor.MaxSel(VoiceLen()-1);
    Paint(tsVoiceIndexChanged);
    emit ActiveStaffChange(m_ActiveStaff);
    emit SelectionChanged();
    EnsureVisible();
}

const int ScoreViewXML::FindCurrentMeter(const int Staff, const int Voice, const int Pointer) const
{
    int Bar=Score.BarMap().GetBar(Pointer, Staff, Voice);
    return Score.BarMap().GetMeter(Bar, Staff, Voice);
}

const int ScoreViewXML::KeepIn(const int num, const int Low, const int High) const
{
    int RetVal = num;
    int Differ = (High - Low) + 1;
    if (num < Low) RetVal = num + Differ;
    if (num > High) RetVal = num - Differ;
    return RetVal;
}

const int ScoreViewXML::BarNrOffset() const
{
    return XMLScore.getVal("BarNrOffset");
}

void ScoreViewXML::setBarNrOffset(const int NewBarNrOffset)
{
    XMLScore.setAttribute("BarNrOffset", NewBarNrOffset);
}

const bool ScoreViewXML::HideBarNumbers() const
{
    return XMLScore.getVal("DontShowBN");
}

void ScoreViewXML::setHideBarNumbers(const bool NewHideBarNumbers)
{
    XMLScore.setAttribute("DontShowBN", NewHideBarNumbers);
}

const int ScoreViewXML::MasterStaff() const
{
    return XMLScore.getVal("MasterStave");
}

void ScoreViewXML::setMasterStaff(const int NewMasterStaff)
{
    XMLScore.setAttribute("MasterStave", NewMasterStaff);
}

const int ScoreViewXML::NoteSpace() const
{
    return XMLScore.getVal("NoteSpace");
}

void ScoreViewXML::setNoteSpace(const int NewNoteSpace)
{
    XMLScore.setAttribute("NoteSpace", NewNoteSpace);
}

void ScoreViewXML::SetSystemLength(const int NewSystemLength)
{
    m_SystemLength = NewSystemLength;
}

const int ScoreViewXML::SystemLength() const
{
    return m_SystemLength;
}

QRectF ScoreViewXML::SceneRect()
{
    return QRectF(0,0,(m_SystemLength / XMLScore.getVal("Size")) + 40,((StaffPos(m_ActiveTemplate->childCount() - 1) + 800) / XMLScore.getVal("Size")) + 30);
}

void ScoreViewXML::Mute(const int Staff, const bool Mute)
{
    Score.Mute(Staff, Mute);
}

void ScoreViewXML::Solo(const int Staff, const bool Solo)
{
    Score.Solo(Staff, Solo);
}

void ScoreViewXML::PasteXML(XMLSimpleSymbolWrapper& Symbol)
{
    if (Cursor.SelCount())
    {
        Delete();
    }
    InsertXML(Cursor.GetPos(),Symbol);
    Cursor.SetZero();
    Cursor.SetPos(Cursor.GetPos()+1);
    Cursor.MaxSel(VoiceLen() - 1);
    if (Cursor.SelEnd() > FindPointerToBar(StartBar() + BarsActuallyDisplayed()))
    {
        setStartBar(StartBar()+BarsActuallyDisplayed());
        emit BarChanged();
    }
}

void ScoreViewXML::InsertXML(XMLSimpleSymbolWrapper& Symbol)
{
    InsertXML(VoiceLen()-1,Symbol);
}

void ScoreViewXML::InsertXML(const int Pointer, XMLSimpleSymbolWrapper& Symbol)
{
    InsertXML(m_ActiveStaff,m_ActiveVoice,Pointer,Symbol);
}

void ScoreViewXML::InsertXML(const int Staff, const int Voice, const int Pointer, XMLSimpleSymbolWrapper& Symbol)
{
    XMLScore.Voice(Staff,Voice)->insertChild(Symbol.getXML(),Pointer);
    m_XMLLastPasted.clearChildren();
    m_XMLLastPasted.appendClone(Symbol.getXML());
}

void ScoreViewXML::AddStaff(const int NewNumber, const QString& Name)
{
    XMLScore.AddStaff(NewNumber,Name);
    Score.AddStave(NewNumber);
    if (NewNumber <= m_ActiveStaff) setActiveStaff(m_ActiveStaff);
}

void ScoreViewXML::AddVoice(const int iStaff)
{
    XMLScore.AddVoice(iStaff);
    Score.AddTrack(iStaff);
    setActiveVoice(VoiceCount(iStaff) - 1);
    Cursor.SetZero();
}

void ScoreViewXML::AddVoice()
{
    AddVoice(m_ActiveStaff);
}

const int ScoreViewXML::VoiceCount(const int Staff)
{
    return XMLScore.NumOfVoices(Staff);
}

const int ScoreViewXML::VoiceCount()
{
    return XMLScore.NumOfVoices(m_ActiveStaff);
}

const int ScoreViewXML::StaffCount()
{
    return XMLScore.NumOfStaffs();
}

void ScoreViewXML::DeleteVoice(const int iStaff, const int iVoice)
{
    XMLScore.DeleteVoice(iStaff,iVoice);
    Score.DeleteTrack(iStaff, iVoice);
    setActiveVoice(0);
    Cursor.SetZero();
}

void ScoreViewXML::DeleteVoice(const int iVoice)
{
    DeleteVoice(m_ActiveStaff,iVoice);
}

void ScoreViewXML::DeleteVoice()
{
    DeleteVoice(m_ActiveStaff,m_ActiveVoice);
}

void ScoreViewXML::DeleteStaff(const int iStaff)
{
    XMLScore.DeleteStaff(iStaff);
    Score.DeleteStave(iStaff);
    setActiveStaff(0);
    setActiveVoice(0);
    Cursor.SetZero();
}

void ScoreViewXML::DeleteStaff()
{
    DeleteStaff(m_ActiveStaff);
}

void ScoreViewXML::Load(const QString& Path)
{
    if (!XMLScore.Load(Path)) return;
    OCSymbolsCollection::ParseFileVersion(XMLScore);

    setActiveTemplate(0);
    Score.MakeStaves(XMLScore);
    setActiveStaff(0);
    setActiveVoice(0);
    //MakeBackup("");
}

const bool ScoreViewXML::Save(const QString& Path)
{
    return XMLScore.Save(Path);
}

const int ScoreViewXML::StaffOrder(const int Staff) const
{
    return XMLScore.StaffOrder(m_ActiveTemplate,Staff);
}

const int ScoreViewXML::ActiveStaff() const
{
    return m_ActiveStaff;
}

void ScoreViewXML::setActiveStaff(const int NewActiveStaff)
{
    if (m_ActiveStaff != NewActiveStaff)
    {
        m_ActiveStaff = NewActiveStaff;
        setActiveVoice(0);
    }
}

const int ScoreViewXML::ActiveVoice() const
{
    return m_ActiveVoice;
}

void ScoreViewXML::setActiveVoice(const int NewActiveVoice)
{
    if (m_ActiveVoice != NewActiveVoice)
    {
        m_ActiveVoice = NewActiveVoice;
    }
}

const bool ScoreViewXML::FollowResize() const
{
    return m_FollowResize;
}

void ScoreViewXML::setFollowResize(const bool NewFollowResize)
{
    m_FollowResize = NewFollowResize;
    if (m_FollowResize)
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    else
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }

}

OCProperties* ScoreViewXML::GetProperties(const int Pointer, const int Staff, const int Voice)
{
    //return Score.GetProperties(XMLScore, Pointer, Staff, Voice);
    XMLSimpleSymbolWrapper s(XMLScore.Voice(Staff,Voice),Pointer);
    OCProperties* p = OCSymbolsCollection::GetProperties(s);
    if (p->Exist("Common"))
    {
        p->GetItem("Common")->Hidden=(Voice>0);
    }
    return p;
}

OCProperties* ScoreViewXML::GetProperties(const int Pointer)
{
    return GetProperties(Pointer,m_ActiveStaff,m_ActiveVoice);
}

OCProperties* ScoreViewXML::GetProperties()
{
    return GetProperties(Cursor.GetPos());
}

XMLSimpleSymbolWrapper ScoreViewXML::GetSymbol(const int Pointer, const int Staff, const int Voice)
{
    return XMLSimpleSymbolWrapper(XMLScore.Voice(Staff,Voice),Pointer);
}

XMLSimpleSymbolWrapper ScoreViewXML::GetSymbol(const int Pointer)
{
    return GetSymbol(Pointer, m_ActiveStaff, m_ActiveVoice);
}

XMLSimpleSymbolWrapper ScoreViewXML::GetSymbol()
{
    return GetSymbol(Cursor.GetPos());
}

void ScoreViewXML::ChangeProperty(const int Pointer, const int Staff, const int Voice, const QString& Name, const QVariant& Value)
{
    XMLSimpleSymbolWrapper s(XMLScore.Voice(Staff,Voice),Pointer);
    if (OCSymbolsCollection::PropetyExists(s.name(),Name))
    {
        s.setAttribute(Name,Value);
    }
}

void ScoreViewXML::ChangeProperty(const QList<int>& Pointers, const QString& Name, const QVariant& Value)
{
    foreach (int i, Pointers) ChangeProperty(i, Name, Value);
}

void ScoreViewXML::ChangeProperty(const int Pointer, const QString& Name, const QVariant& Value)
{
    ChangeProperty(Pointer,m_ActiveStaff,m_ActiveVoice,Name,Value);
}

void ScoreViewXML::ChangeProperties(const int Pointer, const int Staff, const int Voice, const QStringList& Names, const QVariant& Value)
{
    XMLSimpleSymbolWrapper s(XMLScore.Voice(Staff,Voice),Pointer);
    foreach (QString n,Names)
    {
        if (OCSymbolsCollection::PropetyExists(s.name(),n))
        {
            s.setAttribute(n,Value);
        }
    }
}

void ScoreViewXML::ChangeProperties(const int Pointer, const QStringList& Names, const QVariant& Value)
{
    ChangeProperties(Pointer,m_ActiveStaff,m_ActiveVoice,Names,Value);
}

void ScoreViewXML::setEndBar(const int NewEndBar)
{
    m_EndBar = NewEndBar;
}

const int ScoreViewXML::EndBar() const
{
    return m_EndBar;
}

const OCBarMap& ScoreViewXML::BarMap() const
{
    return Score.BarMap();//Score.FillBarsArray(XMLScore);
}

const int ScoreViewXML::VoiceLen(const int Staff, const int Voice)
{
    return XMLScore.VoiceLength(Staff, Voice);
}

const int ScoreViewXML::VoiceLen()
{
    return XMLScore.VoiceLength(m_ActiveStaff,m_ActiveVoice);
}

const int ScoreViewXML::EndOfVoiceBar(const int Staff, const int Voice) const
{
    return BarMap().EndOfVoiceBar(Staff, Voice);
}

const int ScoreViewXML::EndOfVoiceBar() const
{
    return EndOfVoiceBar(m_ActiveStaff, m_ActiveVoice);
}

const OCMIDIVars ScoreViewXML::GetCurrentMIDI() const
{
    return CurrentMIDI;
}

void ScoreViewXML::SetXML(XMLScoreWrapper& NewXML)
{
    XMLScore.setXML(NewXML);
    ReloadXML();
}

void ScoreViewXML::ReloadXML()
{
    setActiveTemplate(0);
    Score.MakeStaves(XMLScore);
}

const QString ScoreViewXML::StaffName(int Staff)
{
    return XMLScore.StaffName(Staff);
}

QDomLiteElement* ScoreViewXML::ActiveTemplate()
{
    return m_ActiveTemplate;
}

void ScoreViewXML::setActiveTemplate(const int Template)
{
    m_ActiveTemplate=XMLScore.Template(Template);
}

void ScoreViewXML::setActiveTemplate(QDomLiteElement* Template)
{
    m_ActiveTemplate=Template;
}

const QList<QPair<int,int> > ScoreViewXML::SelectionList()
{
    QList<QPair<int,int> > RetVal;
    RetVal.append(qMakePair(Cursor.SelStart(),Cursor.SelEnd()));
    return RetVal;
}

const QList<QPair<int,int> > ScoreViewXML::SelectionList(const int Bar1, const int Bar2, const int Staff1, const int Staff2)
{
    QList<QPair<int,int> > RetVal;
    for (int i=Staff1;i<=Staff2;i++)
    {
        int Staff=MarkToStaff(i);
        int Voice=MarkToVoice(i);
        int p1=FindPointerToBar(Staff,Voice,Bar1);
        if (p1==VoiceLen(Staff,Voice)) p1--;
        int p2=FindPointerToBar(Staff,Voice,Bar2+1)-1;
        if (p2==VoiceLen(Staff,Voice)) p2--;
        RetVal.append(qMakePair(p1,p2));
    }
    return RetVal;
}

const int ScoreViewXML::MarkToVoice(const int Mark)
{
    int s=0;
    int Staff=0;
    forever
    {
        if (Mark<s+VoiceCount(Staff))
        {
            return Mark-s;
        }
        s+=VoiceCount(Staff);
        Staff++;
    }
}

const int ScoreViewXML::MarkToStaff(const int Mark)
{
    int s=0;
    int Staff=0;
    forever
    {
        if (Mark<s+VoiceCount(Staff))
        {
            return Staff;
        }
        s+=VoiceCount(Staff);
        Staff++;
    }
}

const QList<SymbolSearchLocation> ScoreViewXML::Search(const QString& SearchTerm, const int Staff, const int Voice)
{
    return Score.Search(XMLScore,SearchTerm,Staff,Voice);
}
