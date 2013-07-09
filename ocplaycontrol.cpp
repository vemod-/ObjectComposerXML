#include "ocplaycontrol.h"
#include "ui_ocplaycontrol.h"
#include <QFileInfo>
#include "qtransmenu.h"
#include <QTime>
#include "mouseevents.h"

OCPlayControl::OCPlayControl(CMIDI2wav *m2w, QWidget *parent) :
    QFadingFrame(parent),
    ui(new Ui::OCPlayControl)
{
    ui->setupUi(this);
    midi2Wav=m2w;
    playButton=0;
    MouseWithin=false;
    MixerXML.clear();
    this->setWindowFlags((Qt::WindowFlags)(this->windowFlags() | Qt::WindowStaysOnTopHint));
    midi2Wav->setVolSlider(ui->volumeSlider);
    midi2Wav->setSlider(ui->seekSlider);
    midi2Wav->setElapsedLabel(ui->labelRight);
    midi2Wav->setRemainingLabel(ui->labelLeft);
    midi2Wav->setMuteButton(ui->VolumeDown);
    midi2Wav->setMaxButton(ui->VolumeUp);
    connect(midi2Wav,SIGNAL(StateChanged(MIDI2wavState)),this,SLOT(StateChanged(MIDI2wavState)));
    MouseEvents* ev=new MouseEvents();
    ui->PlayButton->installEventFilter(ev);
    connect(ev,SIGNAL(MousePress(QMouseEvent*)),this,SLOT(ButtonPress(QMouseEvent*)));
    playButton=new EffectLabel(parent);
    QFont f=playButton->font();
    f.setPixelSize(11);
    playButton->setFont(f);
    playButton->setEffect(EffectLabel::Sunken);
    playButton->setText("Play");
    playButton->setIconSize(QSize(32,32));
    playButton->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    MouseEvents* ev1=new MouseEvents;
    connect(ev1,SIGNAL(MousePress(QMouseEvent*)),this,SLOT(ButtonPress(QMouseEvent*)));
    playButton->installEventFilter(ev1);
    playButton->hide();
    hideTimer.setInterval(5000);
    hideTimer.setSingleShot(true);
    connect(&hideTimer,SIGNAL(timeout()),this,SLOT(timedOut()));
    ui->VolumeDown->setIcon(QIcon(":/mini/mini/volume_down-white.png"));
    ui->VolumeUp->setIcon(QIcon(":/mini/mini/volume_up-white.png"));
    ui->PlayButton->setIconSize(QSize(36,36));
    ui->frameInner->setMouseTracking(true);
    MouseEvents* ev2=new MouseEvents;
    connect(ev2,SIGNAL(MouseEnter(QEvent*)),this,SLOT(mouserEnter()));
    connect(ev2,SIGNAL(MouseLeave(QEvent*)),this,SLOT(mouseLeave()));
    ui->frameInner->installEventFilter(ev2);
    connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(closeMe()));
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
    if (!midi2Wav->IsPlaying())
    {
        hideTimer.start();
    }
}

OCPlayControl::~OCPlayControl()
{
    //midi2Wav->pause();
    delete ui;
}

void OCPlayControl::closeMe()
{
    if (QFadingFrame::state==QFadingFrame::FadingOut) return;
    timeLine->setDuration(800);
    hide();
    MouseWithin=false;
}

void OCPlayControl::ButtonPress(QMouseEvent *event)
{
    if (event->button()==Qt::RightButton)
    {
        Stop();
        QTransMenu* m=new QTransMenu(this);
        m->addAction(actionFromHere);
        m->addAction(actionEverything);
        m->addAction(actionStaffFromHere);
        m->exec(this->cursor().pos());
        delete m;
    }
    else
    {
        if (midi2Wav->IsPlaying())
        {
            StopAndHide();
        }
        else
        {
            Play(PlayFromHere);
        }
    }
}

void OCPlayControl::PreLoad(PlayLocation mode)
{
    QString path;
    emit RequestFile(path,mode);
    if (!QFileInfo(path).exists()) return;
    if (!MixerXML.isEmpty())
    {
        if (midi2Wav->MixerVisible()) MixerXML=midi2Wav->Save();
        QDomLiteElement XML("Custom");
        XML.fromString(MixerXML);
        XML.setAttribute("File",path);
        midi2Wav->LoadXML(XML.toString());
        midi2Wav->SetTitle(QString());
    }
    else
    {
        midi2Wav->load(path);
        midi2Wav->SetTitle(QString());
        MixerXML=midi2Wav->Save();
    }
}

void OCPlayControl::Play(PlayLocation mode)
{
    hideTimer.stop();
    //QString path;
    //emit RequestFile(path,mode);
    //if (!QFileInfo(path).exists()) return;
    ui->PlayButton->setIcon(QIcon(":/mini/mini/stop-white.png"));
    ui->PlayButton->setToolTip("Stop");
    if (playButton != 0)
    {
        playButton->setIcon(QIcon(":/mini/mini/stop.png"));
        playButton->setText("Stop");
    }
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
    /*
    if (!MixerXML.isEmpty())
    {
        if (midi2Wav->MixerVisible()) MixerXML=midi2Wav->Save();
        QDomLiteElement XML("Custom");
        XML.fromString(MixerXML);
        XML.setAttribute("File",path);
        midi2Wav->LoadXML(XML.toString());
    }
    else
    {
        midi2Wav->load(path);
        MixerXML=midi2Wav->Save();
    }
    */
    PreLoad(mode);
    midi2Wav->ShowMixer();
    midi2Wav->SetTitle(QString());
    midi2Wav->play();
}

void OCPlayControl::Stop()
{
    MixerXML=midi2Wav->Save();
    ui->PlayButton->setIcon(QIcon(":/mini/mini/play-white.png"));
    ui->PlayButton->setToolTip("Play");
    if (playButton != 0)
    {
        playButton->setIcon(QIcon(":/mini/mini/play.png"));
        playButton->setText("Play");
    }
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
    Play(OCPlayControl::PlayFromHere);
}

void OCPlayControl::TriggerEverything()
{
    Play(OCPlayControl::PlayEverything);
}

void OCPlayControl::TriggerStaffFromHere()
{
    Play(OCPlayControl::PlayStaffFromHere);
}

void OCPlayControl::StateChanged(MIDI2wavState s)
{
    if (s==m2w_Finished) Stop();
}

float OCPlayControl::Volume()
{
    return ui->volumeSlider->value()*0.01;
}

QWidget* OCPlayControl::getPlayButton()
{
    playButton->show();
    return playButton;
}
