#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QToolBar>
#include <QFontDatabase>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifdef __STYLESHEETS__
    QFile qss(":/mac.qss");
    if (qss.open( QIODevice::ReadOnly ))
    {
        this->setStyleSheet( qss.readAll() );
        qss.close();
    }
#endif
    QFontDatabase::addApplicationFont(":/OCFMAC.TTF");
#ifdef __Lelandfont
    QFontDatabase::addApplicationFont(":/Leland.otf");
#endif

    if (!QDir(_DocumentPath).exists()) QDir(QStandardPaths::writableLocation(QStandardPaths::MusicLocation)).mkdir("Object Composer");

    QSettings s;
    if (s.value("OCStuff").toString().isEmpty())
    {
        QDomLiteDocument XMLSettings(settingsfile);
        if (XMLSettings.documentElement->childCount())
        {
            s.setValue("OCStuff",XMLSettings.toString());
        }
    }

    setWindowTitle(apptitle);

    m_Document = new CScoreDoc(this,this);
    //m_DocLoader = new CDocumentLoader(doc,this);

    mainBuffer.addTickerDevice(&m_Document->MIDI2wav->DeviceList);
    mainBuffer.init(0,this);
    mainBuffer.createBuffer();
    for (int i=0;i<mainBuffer.jackCount();i++) m_Document->MIDI2wav->DeviceList.addJack(mainBuffer.jack(i));
    m_Document->MIDI2wav->DeviceList.connect("This Out","MIDIFile2Wave Out");
    connect(m_Document->playControl,&OCPlayControl::VolChanged,this,&MainWindow::SetVol);

    CProjectPage* ProjectPage = new CProjectPage(_DocumentPath,true,":/grey-paper-texture.jpg","mus",this);
    CProjectApp* ProjectApp = new CProjectApp(m_Document,m_Document,ProjectPage,this);

    QVBoxLayout* centralWidgetLayout=dynamic_cast<QVBoxLayout*>(centralWidget()->layout());
    centralWidgetLayout->addWidget(ProjectApp,100);

    QMenuBar* menuBar = ui->menuBar;

    QMenu* fileMenu = menuBar->addMenu("File");
    fileMenu->addActions(m_Document->menuFile->actions());
    menuBar->addActions(m_Document->mainMenu->actions());

    this->setGeometry(QGuiApplication::screens().first()->geometry());

    int viewMode = 0;
    QString defaultPath;
    for (const QString& s : qApp->arguments())
    {
        if (s == "-layout") {
            viewMode = 1;
        }
        else if (s == "-new") {
            ProjectApp->newInit();
            m_Document->SetView(viewMode);
            return;
        }
        else
        {
            const QFileInfo f(s);
            if (f.exists(s))
            {
                const CStringCompare c(f.suffix());
                if (c.Compare("mus","zip","mxl","musicxml"))
                {
                    defaultPath = s;
                    break;
                }
            }
        }
    }
    if (!defaultPath.isEmpty()) {
        //m_ProjectApp->openInit(QFileInfo(defaultPath).absoluteFilePath());
        ProjectApp->openInit(QFileInfo(defaultPath).absoluteFilePath());
        m_Document->SetView(viewMode);
        return;
    }
    else {
        if (!m_Document->menuRecent->actions().empty())
        {
            QStringList l = m_Document->menuRecent->recentDocuments();
            if (!l.isEmpty()) ProjectApp->openInit(m_Document->menuRecent->recentDocuments().first());
        }
    }
    if (m_Document->isClosed) emit m_Document->showProjects();
}

MainWindow::~MainWindow()
{
    mainBuffer.pause();
    mainBuffer.finish();
    delete ui;
}
/*
bool MainWindow::event(QEvent *event)
{
    if ((event->type() == QEvent::Move) || (event->type()==QEvent::Resize))
    {
        m_DocLoader->playControl()->move((this->width()-m_DocLoader->playControl()->width())/2,this->height()-m_DocLoader->playControl()->height()-100);
    }
    return QMainWindow::event(event);
}
*/
void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef __MACOSX_CORE__
    /*
    if (event->spontaneous())
    {
        QStringList args;
        args << "-e";
        args << "tell application \"System Events\"";
        args << "-e";
        args << "set visible of process \""+QFileInfo(QApplication::applicationFilePath()).baseName()+"\" to false";
        args << "-e";
        args << "end tell";
        QProcess::execute("osascript", args);
        event->ignore();
        return;
    }
    */
#endif
    if (!m_Document->CleanDoc()) event->ignore();
}
/*
void MainWindow::UpdateAppTitle()
{
    setWindowTitle(m_DocLoader->document()->Title() + " - " + apptitle);
}

void MainWindow::NoteOnOff(bool On, int Pitch, int MixerTrack, OCMIDIVars MIDIInfo)
{
    OCMIDIVars m = MIDIInfo;
    if (On) {
        m_DocLoader->midi2wav()->NoteOn(MixerTrack,byte(m.pitch(Pitch)),byte(m.Channel),byte(100.f*m_DocLoader->playControl()->Volume()),byte(m.Patch));
    }
    else {
        m_DocLoader->midi2wav()->NoteOff(MixerTrack,byte(m.pitch(Pitch)),byte(m.Channel));
    }
}
*/
void MainWindow::play(const bool FromStart) {
    mainBuffer.play(FromStart);
}

void MainWindow::skip(const ulong64 samples) {
    mainBuffer.skip(samples);
}

void MainWindow::pause() {
    mainBuffer.pause();
}

bool MainWindow::isPlaying() const {
    return mainBuffer.isPlaying();
}

ulong64 MainWindow::samples() const {
    return mainBuffer.samples();
}

ulong MainWindow::milliSeconds() const {
    return mainBuffer.milliSeconds();
}

ulong MainWindow::ticks() const {
    return mainBuffer.ticks();
}

ulong64 MainWindow::currentSample() const {
    return mainBuffer.currentSample();
}

ulong MainWindow::currentMilliSecond() const {
    return mainBuffer.currentMilliSecond();
}

void MainWindow::renderWaveFile(const QString path) {
    mainBuffer.render(path);
}
