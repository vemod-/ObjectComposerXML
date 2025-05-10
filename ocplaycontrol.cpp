#include "ocplaycontrol.h"
#include "ui_ocplaycontrol.h"
//#include <QFileInfo>
#include "qtransmenu.h"
//#include <QTime>

OCPlayControl::OCPlayControl(CMIDI2wav *m2w, QWidget *parent) :
    QFadingFrame(parent),
    ui(new Ui::OCPlayControl)
{
    ui->setupUi(this);
    GeometryEvent* ge = new GeometryEvent();
    parentWidget()->installEventFilter(ge);
    connect(ge,&GeometryEvent::GeometryChanged,this,&OCPlayControl::ChangeGeometry);
    midi2Wav=m2w;
    playButton=nullptr;
    mixerButton=nullptr;
    MouseWithin=false;
    this->setWindowFlags(Qt::WindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint));
    ui->volumeSlider->setMaximum(200);
    ui->volumeSlider->setValue(100);

    connect(ui->seekSlider,&QAbstractSlider::valueChanged,this,&OCPlayControl::SliderSkip);
    ui->labelLeft->setTextColor(Qt::white);
    ui->labelRight->setTextColor(Qt::white);
    connect(ui->VolumeDown,&QToolButton::clicked,this,&OCPlayControl::Mute);
    connect(ui->VolumeUp,&QToolButton::clicked,this,&OCPlayControl::Max);
    connect(ui->volumeSlider,&QAbstractSlider::valueChanged,this,&OCPlayControl::VolChanged);

    MouseEvents* ev=new MouseEvents();
    ui->PlayButton->installEventFilter(ev);
    connect(ev,&MouseEvents::MousePressed,this,&OCPlayControl::RightButtonPress);
    connect(ui->PlayButton,&QToolButton::pressed,this,&OCPlayControl::LeftButtonPress);
    playButton=new QAction("Play",this);
    ev1=new MouseEvents;
    connect(playButton,&QAction::triggered,this,&OCPlayControl::LeftButtonPress);
    connect(ev1,&MouseEvents::MousePressed,this,&OCPlayControl::RightButtonPress);
    playButton->setVisible(false);
    ui->PlayButton->setIcon(QIcon(":/mini/mini/play-white.png"));
    ui->PlayButton->setToolTip("Play");
    playButton->setIcon(QIcon(":/mini/mini/play.png"));
    playButton->setText("Play");

    mixerButton=new QAction("Mixer",this);
    mixerButton->setIcon(QIcon(":/mini/mini/mixer.png"));
    //MouseEvents* ev3=new MouseEvents;
    //connect(ev3,&MouseEvents::MousePressed,this,&OCPlayControl::ToggleMixer);
    connect(mixerButton,&QAction::triggered,this,&OCPlayControl::ToggleMixer);
    //mixerButton->installEventFilter(ev3);
    mixerButton->setVisible(false);

    hideTimer.setInterval(5000);
    hideTimer.setSingleShot(true);
    connect(&hideTimer,&QTimer::timeout,this,&OCPlayControl::timedOut);
    ui->VolumeDown->setIcon(QIcon(":/mini/mini/volume_down-white.png"));
    ui->VolumeUp->setIcon(QIcon(":/mini/mini/volume_up-white.png"));
    ui->PlayButton->setIconSize(QSize(36,36));
    ui->frameInner->setMouseTracking(true);
    MouseEvents* ev2=new MouseEvents;
    connect(ev2,&MouseEvents::MouseEntered,this,&OCPlayControl::mouserEnter);
    connect(ev2,&MouseEvents::MouseLeft,this,&OCPlayControl::mouseLeave);
    ui->frameInner->installEventFilter(ev2);
    connect(ui->closeButton,&QAbstractButton::clicked,this,&OCPlayControl::closeMe);

    mixerMenu = new QMenu(this);
    mixerAction = new QWidgetAction(this);
    mixerAction->setDefaultWidget(midi2Wav->mixerWidget());
    mixerMenu->addAction(mixerAction);

    actionMenu = new QMenu("Play",this);
    actionFromHere = actionMenu->addAction("Play from Here",QKeySequence(Qt::Key_Space),this,&OCPlayControl::TriggerFromHere);
    actionEverything = actionMenu->addAction("Play all",QKeySequence(Qt::ALT | Qt::Key_Space),this,&OCPlayControl::TriggerEverything);
    actionStaffFromHere = actionMenu->addAction("Play Staff from Here",QKeySequence(Qt::ALT | Qt::CTRL | Qt::Key_Space),this,&OCPlayControl::TriggerStaffFromHere);
    actionStop = actionMenu->addAction("Stop",QKeySequence(Qt::Key_Space),this,&OCPlayControl::StopAndHide);
    m_TimerID = startTimer(50);
}

void OCPlayControl::ChangeGeometry()
{
    move((parentWidget()->width()-width())/2,parentWidget()->height()-(height()+60));
}

void OCPlayControl::mouserEnter()
{
    MouseWithin=true;
    if (hideTimer.isActive())
    {
        hideTimer.stop();
    }
}

void OCPlayControl::mouseLeave()
{
    MouseWithin=false;
    if (!midi2Wav->isPlaying())
    {
        hideTimer.start();
    }
}

OCPlayControl::~OCPlayControl()
{
    //midi2Wav->pause();
    killTimer(m_TimerID);
    m_TimerID = 0;
    hideTimer.disconnect();
    delete ui;
}

void OCPlayControl::timerEvent(QTimerEvent* /*e*/) {
    if (!m_TimerID) return;
    if (midi2Wav->isPlaying()) {
        ulong c = midi2Wav->currentMilliSecond();
        int v = c * 0.01;
        if (v != ui->seekSlider->value()) {
            ui->labelLeft->setText(elapsed(c));
            ui->labelRight->setText(remaining(c));
            ui->seekSlider->blockSignals(true);
            ui->seekSlider->setValue(v);
            ui->seekSlider->blockSignals(false);
        }
        if (ui->PlayButton->toolTip() != "Stop") {
            ui->PlayButton->setIcon(QIcon(":/mini/mini/stop-white.png"));
            ui->PlayButton->setToolTip("Stop");
            playButton->setIcon(QIcon(":/mini/mini/stop.png"));
            playButton->setText("Stop");
        }
    }
    else {
        if (ui->PlayButton->toolTip() != "Play") {
            ui->PlayButton->setIcon(QIcon(":/mini/mini/play-white.png"));
            ui->PlayButton->setToolTip("Play");
            playButton->setIcon(QIcon(":/mini/mini/play.png"));
            playButton->setText("Play");
        }
    }
}

void OCPlayControl::closeMe()
{
    if (QFadingFrame::state==QFadingFrame::FadingOut) return;
    timeLine->setDuration(800);
    hide();
    MouseWithin=false;
}

void OCPlayControl::PopupMenu() {
    Stop();
    QTransMenu* m=new QTransMenu(this);
    m->addAction(actionFromHere);
    m->addAction(actionEverything);
    m->addAction(actionStaffFromHere);
    connect(m,&QMenu::triggered,m,&QMenu::close);
    m->popup(this->cursor().pos());
}

void OCPlayControl::LeftButtonPress()
{
    if (midi2Wav->isPlaying())
    {
        StopAndHide();
    }
    else
    {
        Play(CMIDI2wav::PlayFromHere);
    }
}

void OCPlayControl::RightButtonPress(QMouseEvent* e) {
    if (e->button() == Qt::RightButton) {
        PopupMenu();
    }
}

void OCPlayControl::SetVol(int v) {
    ui->volumeSlider->setValue(v);
}

void OCPlayControl::Mute()
{
    ui->volumeSlider->setValue(0);
}

void OCPlayControl::Max()
{
    ui->volumeSlider->setValue(ui->volumeSlider->maximum());
}

void OCPlayControl::SliderSkip(int v) {
    midi2Wav->skip(CPresets::mSecsToSamples(v * 100));
    ui->labelLeft->setText(elapsed(v * 100));
    ui->labelRight->setText(remaining(v * 100));
}

void OCPlayControl::Play(CMIDI2wav::PlayLocation mode)
{
    hideTimer.stop();

    actionStop->setEnabled(true);
    actionFromHere->setEnabled(false);
    actionEverything->setEnabled(false);
    actionStaffFromHere->setEnabled(false);
    timeLine->setDuration(400);
    show();
    if (this->rect().contains(this->mapFromGlobal(this->cursor().pos())))
    {
        MouseWithin=true;
    }
    int bar = midi2Wav->RefreshMIDIFile(mode);
    ulong64 t = midi2Wav->mSecsToBar(bar);
    qDebug() << t;
    midi2Wav->skip(CPresets::mSecsToSamples(t));
    midi2Wav->play(false);
    m_mSecs = midi2Wav->milliSeconds();
    if (!closeEnough<double>(ui->seekSlider->maximum(),m_mSecs*0.01))
    {
        ui->seekSlider->blockSignals(true);
        double factor=double(ui->seekSlider->value())/double(ui->seekSlider->maximum());
        ui->seekSlider->setMaximum(int(m_mSecs*0.01));
        ui->seekSlider->setValue(int(m_mSecs*0.01*factor));
        ui->seekSlider->blockSignals(false);
    }
    ui->labelLeft->setText(elapsed(0));
    ui->labelRight->setText(remaining(0));
}

void OCPlayControl::ToggleMixer()
{
    QWidget* w = midi2Wav->mixerWidget();
    w->updateGeometry();
    w->show();
    mixerMenu->setFixedSize(w->sizeHint()+QSize(10,10));
    mixerMenu->popup(QCursor::pos());
}

void OCPlayControl::Stop()
{
    actionStop->setEnabled(false);
    actionFromHere->setEnabled(true);
    actionEverything->setEnabled(true);
    actionStaffFromHere->setEnabled(true);
    midi2Wav->pause();
    if (!MouseWithin) hideTimer.start();
}

void OCPlayControl::timedOut()
{
    timeLine->setDuration(2400);
    hide();
    MouseWithin=false;
}

void OCPlayControl::StopAndHide()
{
    Stop();
    if (MouseWithin) return;
    hideTimer.stop();
    timeLine->setDuration(800);
    hide();
    MouseWithin=false;
}

void OCPlayControl::TriggerFromHere()
{
    Play(CMIDI2wav::PlayFromHere);
}

void OCPlayControl::TriggerEverything()
{
    Play(CMIDI2wav::PlayEverything);
}

void OCPlayControl::TriggerStaffFromHere()
{
    Play(CMIDI2wav::PlayStaffFromHere);
}

float OCPlayControl::Volume()
{
    return ui->volumeSlider->value()*0.01f;
}

QAction* OCPlayControl::getPlayButton()
{
    playButton->setVisible(true);
    return playButton;
}

QAction* OCPlayControl::getMixerButton()
{
    for (QObject* w : playButton->associatedObjects()) w->installEventFilter(ev1);
    mixerButton->setVisible(true);
    return mixerButton;
}

QString OCPlayControl::elapsed(ulong c)
{
    return QTime().addMSecs(c).toString();
}

QString OCPlayControl::remaining(ulong c)
{
    if (c > m_mSecs) c=m_mSecs;
    return QTime().addMSecs(m_mSecs).addMSecs(-c).toString();
}
