#include "mainwindow.h"
#include "CommonClasses.h"
#include "ui_mainwindow.h"
//#include "cstaffsdialog.h"
#include "cbarwindow.h"
#include "cpropertywindow.h"
#include "cmusictree.h"
#include "cscorewizard.h"
#include "cpresetsdialog.h"
//#include "qtoolbuttongrid.h"
#include "ocpiano.h"
//#include <QThread>
#include <QClipboard>
//#include <QGraphicsDropShadowEffect>
#include <QDesktopWidget>
#include <cocoaprogressbar.h>
#include "EffectLabel.h"
#include "mouseevents.h"
#include "qcustomframe.h"
#include <QMessageBox>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#if QT_VERSION <= QT_VERSION_CHECK(4,4,0)
    qt_mac_set_menubar_icons(false);
#endif
#ifdef __STYLESHEETS__
    QFile qss( QDir::homePath() + "/Documents/C++/ObjectComposerXML/mac.qss" );
    if (qss.open( QIODevice::ReadOnly ))
    {
        this->setStyleSheet( qss.readAll() );
        qss.close();
    }
#endif
    mapper=new QSignalMapper(this);
    connect( mapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)) );

    RecentMapper=new QSignalMapper(this);
    connect( RecentMapper, SIGNAL(mapped(QString)), this, SLOT(OpenDoc(QString)));

    setWindowTitle(apptitle);

    LoadRecent();

    Projects.clear();
    OCPresets P("Projects");
    int projectcount=P.GetValue("ProjectCount").toInt();
    for (int i=0;i<projectcount;i++)
    {
        QString n=P.GetString("ProjectName"+QString::number(i));
        OCProject* p=new OCProject;
        p->Name=n;
        p->Files=P.GetValue("Documents"+QString::number(i)).toStringList();
        if (p->Files.count())
        {
            Projects.append(p);
        }
        else
        {
            delete p;
        }
    }

    NewMenu=new QTransMenu(this);
    NewMenu->addAction(Action("actionNew"));
    NewMenu->addAction(Action("actionNew_Score"));
    NewMenu->addAction(Action("actionNew_Project"));
    NewMenu->addSeparator();
    NewMenu->addAction(Action("actionOpen"));

    QMacSplitter* splitter=new QMacSplitter;
    splitter->setObjectName("sidebarSplitter");
    ui->centralFrame->layout()->addWidget(splitter);
    ui->FadingWidget->setGeometry(0,0,this->width()-(this->width()/8),ui->FadingWidget->height());
    ui->FadingWidget->setWidget(ui->mdiArea);
    splitter->addWidget(ui->SidebarFrame);
    splitter->addWidget(ui->FadingWidget);
    splitter->setStretchFactor(0,1);
    splitter->setStretchFactor(1,3);

    ui->SidebarFrame->setAutoFillBackground(true);
    ui->SidebarTree->header()->setFixedHeight(tablerowheight);
    ui->SidebarTree->setAutoFillBackground(true);
    ui->SidebarTree->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->SidebarTree->setIndentation(12);
    ui->SidebarTree->setColumnCount(3);
    //ui->SidebarTree->setIconSize(QSize(tablerowheight,tablerowheight));
    ui->SidebarTree->setUniformRowHeights(true);
    ui->SidebarTree->setAnimated(true);
    ui->SidebarTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->SidebarTree->setAnimated(false);

    connect(ui->SidebarTree,SIGNAL(itemClick(SidebarItem&,int)),this,SLOT(SidebarClicked(SidebarItem&,int)));
    connect(ui->SidebarTree,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(RenameProject(QTreeWidgetItem*,int)));
    connect(ui->SidebarTree,SIGNAL(MoveDocument()),this,SLOT(RebuildProjects()));
    connect(ui->NewDocument,SIGNAL(clicked()),this,SLOT(ShowNewMenu()));
    connect(ui->SaveDocument,SIGNAL(clicked()),this,SLOT(SaveDoc()));
    connect(ui->CloseDocument,SIGNAL(clicked()),this,SLOT(CloseDoc()));
    connect(ui->mdiArea,SIGNAL(currentChanged(int)),this,SLOT(DocumentActivated(int)));

    QWidget* zoomer=new QWidget(this);
    QGridLayout* l=new QGridLayout(zoomer);
    l->setMargin(2);
    l->setHorizontalSpacing(2);
    l->setVerticalSpacing(8);
    zoomer->setLayout(l);
    zoomer->setFixedWidth(168);

    QWidget* spc=new QWidget(this);
    spc->setFixedHeight(8);
    l->addWidget(spc,0,0,1,3);

    btnZoomOut=new QToolButton(this);
    btnZoomOut->setFixedSize(20,20);
    btnZoomOut->setIconSize(QSize(20,20));
    btnZoomOut->setProperty("transparent",true);
    btnZoomOut->setIcon(QIcon(":/mini/mini/zoom_out.png"));
    l->addWidget(btnZoomOut,1,0);

    ZoomSlider=new QSlider(this);
    ZoomSlider->setOrientation(Qt::Horizontal);
    ZoomSlider->setFixedWidth(120);
    ZoomSlider->setRange(25,100);
    l->addWidget(ZoomSlider,1,1);

    btnZoomIn=new QToolButton(this);
    btnZoomIn->setFixedSize(20,20);
    btnZoomIn->setIconSize(QSize(20,20));
    btnZoomIn->setProperty("transparent",true);
    btnZoomIn->setIcon(QIcon(":/mini/mini/zoom_in.png"));
    l->addWidget(btnZoomIn,1,2);

    EffectLabel* btnzoomtxt=new EffectLabel(this);
    btnzoomtxt->setText("Zoom");
    btnzoomtxt->setAlignment(Qt::AlignHCenter);
    btnzoomtxt->setEffect(EffectLabel::Sunken);
    QFont fnt=btnzoomtxt->font();
    fnt.setPointSize(11);
    btnzoomtxt->setFont(fnt);
    btnzoomtxt->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    l->addWidget(btnzoomtxt,2,0,1,3);

    midi2Wav=new CMIDI2wav(this);
    playControl=new OCPlayControl(midi2Wav,this);
    connect(playControl,SIGNAL(RequestFile(QString&,OCPlayControl::PlayLocation)),this,SLOT(RequestMidiFile(QString&,OCPlayControl::PlayLocation)));
    playControl->actionFromHere=ui->actionPlay;
    playControl->actionEverything=ui->actionPlay_All;
    playControl->actionStaffFromHere=ui->actionPlay_Staff_From_Here;
    playControl->actionStop=ui->actionStop;
    connect(ui->actionPlay,SIGNAL(triggered()),playControl,SLOT(TriggerFromHere()));
    connect(ui->actionPlay_All,SIGNAL(triggered()),playControl,SLOT(TriggerEverything()));
    connect(ui->actionPlay_Staff_From_Here,SIGNAL(triggered()),playControl,SLOT(TriggerStaffFromHere()));
    connect(ui->actionStop,SIGNAL(triggered()),playControl,SLOT(StopAndHide()));
    playControl->Stop();

    ui->actionPage_Setup->setVisible(false);

    QToolBar* mtb=new QToolBar(this);
    mtb->setFixedHeight(52);
    mtb->setIconSize(QSize(32,32));
    mtb->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mtb->setMovable(false);
    mtb->setFloatable(false);
    mtb->setAllowedAreas(Qt::NoToolBarArea);

    mtb->addAction(ui->actionLayoutSystem);
    mtb->addAction(ui->actionLayoutPage);
    mtb->addAction(ui->actionLayoutLayout);
    connect(ui->actionLayoutSystem,SIGNAL(triggered()),this,SLOT(ShowLayoutSystemPopup()));
    connect(ui->actionLayoutPage,SIGNAL(triggered()),this,SLOT(ShowLayoutPagePopup()));
    connect(ui->actionLayoutLayout,SIGNAL(triggered()),this,SLOT(ShowLayoutLayoutPopup()));

    mtb->addAction(ui->actionUndo);
    mtb->addAction(ui->actionRedo);
    mtb->addAction(ui->actionCut);
    mtb->addAction(ui->actionCopy);
    mtb->addAction(ui->actionPaste);
    mtb->addAction(ui->actionReset_Position);

    //ui->toolBar->removeAction(ui->actionLayoutSettings);
    //ui->toolBar->removeAction(ui->actionSettings);

    //foreach (QAction* a,ui->toolBar->actions()) if (a->isSeparator()) ui->toolBar->removeAction(a);
    mtb->addSeparator();
    mtb->addAction(ui->actionLayoutSettings);
    mtb->addAction(ui->actionSettings);

    RightSideButton=new QAction(QIcon(":/mini/mini/right.png").pixmap(20,20),QString(),this);
    mtb->addAction(RightSideButton);
    RightSideButton->setToolTip("Hide Sidebar");
    connect(RightSideButton,SIGNAL(triggered()),this,SLOT(ToggleLayoutSidebar()));
    /*
    LeftSideButton=new QToolButton(this);
    LeftSideButton->setFixedSize(40,20);
    LeftSideButton->setIconSize(QSize(20,20));
    LeftSideButton->setToolTip("Hide Sidebar");
    LeftSideButton->setProperty("transparent",true);
    LeftSideButton->setIcon(QIcon(":/mini/mini/left.png"));
    connect(LeftSideButton,SIGNAL(clicked()),this,SLOT(ToggleSidebar()));
    */
    LeftSideButton=new QAction(QIcon(":/mini/mini/left.png").pixmap(20,20),QString(),this);
    LeftSideButton->setToolTip("Hide Sidebar");
    connect(LeftSideButton,SIGNAL(triggered()),this,SLOT(ToggleSidebar()));

    QToolBar* ltb=new QToolBar(this);
    ltb->setFixedHeight(52);
    ltb->setIconSize(QSize(32,32));
    ltb->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ltb->setMovable(false);
    ltb->setFloatable(false);
    ltb->setAllowedAreas(Qt::NoToolBarArea);
    ltb->addAction(LeftSideButton);
    //ltb->addWidget(LeftSideButton);
    ltb->addWidget(zoomer);

    playControl->hide();

    //ui->statusBar->addWidget(LeftSideButton);
    //ui->statusBar->addWidget(zoomer);
    /*
    ui->statusBar->addWidget(ltb);
    ui->statusBar->addWidget(new QSpacerWidget(this));

    ui->statusBar->addCenterWidget(playControl->getPlayButton());

    ui->statusBar->addPermanentWidget(new QSpacerWidget(this));
    ui->statusBar->addPermanentWidget(ui->toolBar);

    ui->statusBar->setMaximumHeight(52);
    */
    //ui->statusBar->hide();

    QWidget* stb=new QWidget(this);
    stb->setObjectName("StatusBar");
    stb->setFixedHeight(52);
    QHBoxLayout* slo=new QHBoxLayout(stb);
    slo->setSpacing(0);
    slo->setMargin(0);
    stb->setLayout(slo);
    slo->addWidget(ltb,1,Qt::AlignLeft);
    slo->addWidget(playControl->getPlayButton(),0,Qt::AlignHCenter);
    slo->addWidget(mtb,1,Qt::AlignRight);

    QVBoxLayout* cwl=(QVBoxLayout*)centralWidget()->layout();
    cwl->setStretch(0,1);
    cwl->addWidget(stb,0);

    closeAction = new QAction( tr("&Close"), this );
    closeAction->setStatusTip( tr("Close Current Document") );
    connect( closeAction, SIGNAL(triggered()), this, SLOT(CloseDoc()) );

    connect(Action("actionFirst"),SIGNAL(triggered()),this,SLOT(First()));
    connect(Action("actionBack"),SIGNAL(triggered()),this,SLOT(Back()));
    connect(Action("actionForward"),SIGNAL(triggered()),this,SLOT(Forward()));
    connect(Action("actionLast"),SIGNAL(triggered()),this,SLOT(Last()));
    connect(ui->actionPrevious_Staff,SIGNAL(triggered()),this,SLOT(PrevStaff()));
    connect(ui->actionNext_Staff,SIGNAL(triggered()),this,SLOT(NextStaff()));
    connect(ui->actionPrevious_Voice,SIGNAL(triggered()),this,SLOT(PrevVoice()));
    connect(ui->actionNext_Voice,SIGNAL(triggered()),this,SLOT(NextVoice()));

    connect(Action("actionSettings"),SIGNAL(triggered()),this,SLOT(Settings()));

    connect(Action("actionPrint_Preview"),SIGNAL(triggered()),this,SLOT(PrintPreview()));
    connect(Action("actionPrint"),SIGNAL(triggered()),this,SLOT(Print()));
    connect(Action("actionPage_Setup"),SIGNAL(triggered()),this,SLOT(PageSetup()));
    connect(Action("actionGet_1_Bar_from_Next_System"),SIGNAL(triggered()),this,SLOT(GetBarFromNext()));
    connect(Action("actionMove_1_Bar_to_Next_System"),SIGNAL(triggered()),this,SLOT(MoveBarToNext()));
    connect(Action("actionGet_1_System_from_Next_Page"),SIGNAL(triggered()),this,SLOT(GetSystemFromNext()));
    connect(Action("actionMove_1_System_to_Next_Page"),SIGNAL(triggered()),this,SLOT(MoveSystemToNext()));
    connect(Action("actionFit_Systems"),SIGNAL(triggered()),this,SLOT(FitSystems()));
    connect(Action("actionToggle_Names"),SIGNAL(triggered()),this,SLOT(ToggleNames()));
    connect(Action("actionEdit_Staffs"),SIGNAL(triggered()),this,SLOT(EditStaffs()));
    connect(Action("actionLayoutSettings"),SIGNAL(triggered()),this,SLOT(EditLayout()));
    connect(ui->actionAutoadjust_All,SIGNAL(triggered()),this,SLOT(AutoAdjustAll()));
    connect(Action("actionReformat"),SIGNAL(triggered()),this,SLOT(ReformatLayout()));
    connect(ui->actionReformat_From_Here_to_End,SIGNAL(triggered()),this,SLOT(ReformatFromHere()));
    connect(ui->actionStretch_From_Here_to_End,SIGNAL(triggered()),this,SLOT(StretchFromHere()));
    connect(ui->actionCompress_From_Here_to_End,SIGNAL(triggered()),this,SLOT(CompressFromHere()));
    connect(ui->actionFit_Systems_2,SIGNAL(triggered()),this,SLOT(FitAll()));
    connect(ui->actionFit_Systems_From_Here,SIGNAL(triggered()),this,SLOT(FitFromHere()));
    this->setGeometry(QApplication::desktop()->screenGeometry());
    splitter->Load("sidebarSplitter");
    NewDoc();

    //this->setUnifiedTitleAndToolBarOnMac(true);
    grabGesture(Qt::PinchGesture);
/*
#ifdef __MACOSX_CORE__
    VirtualMIDIProc=new QProcess(this);
    if (MIDI.getPortCount()==0)
    {
        QString VMPpath(QApplication::applicationDirPath()+"/../Resources/OCVirtualMIDIBridge.app");
        if (QFile::exists(VMPpath))
        {
            VirtualMIDIProc->start(VMPpath);
            VirtualMIDIProc->waitForStarted();
            while (MIDI.getPortCount()==0) QApplication::processEvents();
        }
    }
#endif
    if (MIDI.getPortCount()>0) MIDI.openPort(0);
    */
}

MainWindow::~MainWindow()
{
    /*
#ifdef __MACOSX_CORE__
    if (VirtualMIDIProc->state()==QProcess::Running)
    {
        VirtualMIDIProc->close();
        VirtualMIDIProc->waitForFinished();
    }
#endif
*/
    midi2Wav->HideMixer();
    midi2Wav->pause();
    delete midi2Wav;
    QMacSplitter* sp=findChild<QMacSplitter*>("sidebarSplitter");
    sp->Save("sidebarSplitter");
    while (RecentFiles.count()>recentmax) RecentFiles.removeLast();
    OCPresets("RecentDocuments").SetValue("RecentDocuments",RecentFiles);
    OCPresets P("Projects");
    P.SetValue("ProjectCount",Projects.count());
    for (int i=0;i<Projects.count();i++)
    {
        P.SetValue("ProjectName"+QString::number(i),Projects[i]->Name);
        P.SetValue("Documents"+QString::number(i),Projects[i]->Files);
    }
    qDeleteAll(Projects);
    qDeleteAll(RecentActions);
    delete ui;
}

void MainWindow::ShowScoreWizard()
{
    CScoreWizard* d=new CScoreWizard(this);
    d->Open(":/Template.mus");
    d->setWindowTitle("New Score");
    if (d->exec()==QDialog::Accepted)
    {
        ui->FadingWidget->setTransitionType(QFadingWidget::Fade);
        ui->FadingWidget->prepare();
        CScoreDoc* document=new CScoreDoc(this);
        document->setWindowTitle("Untitled");
        XMLScoreWrapper s(d->CreateXML());
        document->SetXML(s);
        ui->mdiArea->addWidget(document);
        ui->mdiArea->setCurrentWidget(document);
        document->showMaximized();
        ui->FadingWidget->fade();
    }
    delete d;
}

void MainWindow::UpdateWindowList()
{
    QMenu* windowMenu=ui->menuWindow;

    windowMenu->clear();
    windowMenu->addAction(closeAction);
    windowMenu->addSeparator();

    int i=1;
    for(int j=0;j<ui->mdiArea->count();j++)
    {
        QWidget* w=ui->mdiArea->widget(j);
        QString text;
        if ( i<10 )
        {
            text = tr("&%1 %2").arg( i++ ).arg( w->windowTitle() );
        }
        else
        {
            text = w->windowTitle();
        }

        QAction *action = windowMenu->addAction( text );
        action->setCheckable( true );
        action->setChecked( w == ui->mdiArea->currentWidget() );
        connect( action, SIGNAL(triggered()), mapper, SLOT(map()) );
        mapper->setMapping( action, w );
    }
}

void MainWindow::AddProject()
{
    OCProject* pr=new OCProject();
    QString name="New Project";
    int i=1;
    forever
    {
        bool match=false;
        foreach (OCProject* p,Projects)
        {
            if (p->Name==name) match=true;
        }
        if (match)
        {
            name="New Project-"+QString::number(i++);
        }
        else
        {
            break;
        }
    }
    pr->Name=name;
    Projects.append(pr);
    UpdateSidebar();
    ui->SidebarTree->editProjectItem(Projects.count()-1);
}

void MainWindow::RenameProject(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    ui->SidebarTree->RenameProjectItem(item, Projects);
    UpdateSidebar();
}

void MainWindow::RebuildProjects()
{
    ui->SidebarTree->RebuildProjects(Projects);
    UpdateSidebar();
}

void MainWindow::UpdateSidebar()
{
    ui->SidebarTree->Fill(ui->mdiArea,Projects,RecentFiles);
}

void MainWindow::SidebarClicked(SidebarItem& i, int Col)
{
    //SidebarItem i=ui->SidebarTree->GetItem(item, Col);
    if (Col>0)
    {
        if (i.a==SBA_Close)
        {
            ui->mdiArea->setCurrentIndex(i.openIndex);
            this->CloseDoc();
            this->UpdateStatus();
        }
        else if (i.a==SBA_RemoveFromRecent)
        {
            RecentFiles.removeOne(i.filePath);
            UpdateWindowList();
            UpdateSidebar();
        }
        else if (i.a==SBA_RemoveFromProject)
        {
            Projects[i.parentIndex]->Files.removeAt(i.itemIndex);
            UpdateSidebar();
        }
        else if (i.a==SBA_DeleteProject)
        {
            //ui->SidebarTree->LockProjectItem(item);
            QMessageBox* msgBox=new QMessageBox(this);
            msgBox->setWindowModality(Qt::WindowModal);
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setText("<b>Delete Project "+Projects[i.itemIndex]->Name+"?<b>");
            msgBox->setInformativeText("This operation can not be undone! Are You sure?");
            msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox->setDefaultButton(QMessageBox::No);
            if (msgBox->exec()==QMessageBox::Yes)
            {
                delete Projects[i.itemIndex];
                Projects.removeAt(i.itemIndex);
                UpdateSidebar();
            }
            delete msgBox;
        }
        else if (i.a==SBA_Save)
        {
            ui->mdiArea->setCurrentIndex(i.openIndex);
            this->SaveDoc();
            this->UpdateStatus();
        }
        else if (i.a==SBA_OpenProject)
        {
            //ui->SidebarTree->LockProjectItem(item);
            foreach (QString s,Projects[i.itemIndex]->Files) OpenDoc(s);
        }
    }
    else
    {
        if (i.a==SBA_Open)
        {
            OpenDoc(i.filePath);
        }
        else if (i.a==SBA_Reveal)
        {
            bool isLayout=i.itemType==SDT_OpenLayout;
            CScoreDoc* doc=(CScoreDoc*)ui->mdiArea->widget(i.openIndex);
            if (isLayout)
            {
                if (doc->lv->CurrentLayout() != i.itemIndex) doc->SetCurrentLayout(i.itemIndex);
                if (doc->GetView() != 1) doc->SetView(1);
            }
            else
            {
                if (doc->GetView() != 0)
                {
                    ShowScore();
                    doc->SetView(0);
                }
            }
            if (i.openIndex != ui->mdiArea->currentIndex())
            {
                ui->FadingWidget->setTransitionType(QFadingWidget::Fade);
                ui->FadingWidget->prepare();
                ui->mdiArea->setCurrentIndex(i.openIndex);
                ui->FadingWidget->fade();
            }
        }
    }
}

void MainWindow::ShowNewMenu()
{
    NewMenu->popup(this->cursor().pos());
}

void MainWindow::ToggleSidebar()
{
    QMacSplitter* splitter=this->findChild<QMacSplitter*>();
    if (!splitter->isCollapsed())
    {
        splitter->collapse(0);
        LeftSideButton->setIcon(QIcon(":/mini/mini/right.png").pixmap(20,20));
        LeftSideButton->setToolTip("Show Sidebar");
    }
    else
    {
        splitter->expand();
        LeftSideButton->setIcon(QIcon(":/mini/mini/left.png").pixmap(20,20));
        LeftSideButton->setToolTip("Hide Sidebar");
    }
}

void MainWindow::ToggleLayoutSidebar()
{
    QMacSplitter* splitter=CurrentDoc()->spSidebar;
    if (!splitter->isCollapsed())
    {
        splitter->collapse(1);
        RightSideButton->setIcon(QIcon(":/mini/mini/left.png").pixmap(20,20));
        RightSideButton->setToolTip("Show Sidebar");
    }
    else
    {
        splitter->expand();
        RightSideButton->setIcon(QIcon(":/mini/mini/right.png").pixmap(20,20));
        RightSideButton->setToolTip("Hide Sidebar");
    }
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture) return gestureEvent(static_cast<QGestureEvent*>(event));
    if ((event->type() == QEvent::Move) || (event->type()==QEvent::Resize))
    {
        playControl->move((this->width()-playControl->width())/2,this->height()-playControl->height()-100);
    }
    return QMainWindow::event(event);
}

bool MainWindow::gestureEvent(QGestureEvent *event)
{
    if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
    {
        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
    }
    if (QGesture *pan = event->gesture(Qt::PanGesture))
    {
        panTriggered(static_cast<QPanGesture *>(pan));
    }
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
    {
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    }
    return true;
}

void MainWindow::panTriggered(QPanGesture *gesture)
{
    Q_UNUSED(gesture);
}

void MainWindow::swipeTriggered(QSwipeGesture *gesture)
{
    Q_UNUSED(gesture);
}

void MainWindow::pinchTriggered(QPinchGesture *gesture)
{
    if (gesture->state() == Qt::GestureFinished)
    {
        double factor=((gesture->scaleFactor()-1.0)*0.5)+1.0;
        int zoom=Inside(ZoomSlider->value()*factor,ZoomSlider->minimum(),ZoomSlider->maximum(),1);
        ZoomSlider->setValue(zoom);
    }
}

void MainWindow::setActiveSubWindow(QWidget *w)
{
    ui->FadingWidget->setTransitionType(QFadingWidget::Fade);
    ui->FadingWidget->prepare();
    ui->mdiArea->setCurrentWidget(w);
    ui->FadingWidget->fade();
}

void MainWindow::ShowPresets()
{
    CPresetsDialog d(this);
    d.setWindowTitle("Presets");
    CPropertyWindow* pw=d.findChild<CPropertyWindow*>();
    OCPresets P;
    pw->Fill(P.Properties());
    //P.Properties()->fillPropertyBrowser(d.editor1);
    if (d.exec()==QDialog::Accepted) P.SaveProperties();
}

void MainWindow::NewDoc()
{
    ui->FadingWidget->setTransitionType(QFadingWidget::Fade);
    ui->FadingWidget->prepare();
    CScoreDoc* document=new CScoreDoc(this);
    document->sv->Paint(tsReformat);
    document->setWindowTitle("Untitled");
    ui->mdiArea->addWidget(document);
    ui->mdiArea->setCurrentWidget(document);
    document->showMaximized();
    document->Dirty=false;
    ui->FadingWidget->fade();
}

void MainWindow::CloseDoc()
{
    if (CurrentDoc()==0) return;
    if (ui->mdiArea->count()>1)
    {
        ui->FadingWidget->setTransitionType(QFadingWidget::Fade);
        ui->FadingWidget->prepare();
        QWidget* sw=CurrentDoc();
        if (sw->close())
        {
            ui->mdiArea->removeWidget(sw);
            delete sw;
            ui->mdiArea->setCurrentIndex(0);
        }
        ui->FadingWidget->fade();
    }
}

void MainWindow::OpenDoc(QString Path)
{
    for (int i=ui->mdiArea->count()-1;i>=0;i--)
    {
        QWidget* sw=ui->mdiArea->widget(i);
        if (sw->windowTitle()==Path)
        {
            CScoreDoc* document=(CScoreDoc*)sw;
            if (document->UndoText().length())
            {
                QMessageBox* d=new QMessageBox(QMessageBox::Warning,apptitle,"The document '" + document->windowTitle() + "' is already open.\nDo you want to save your changes?",QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,this,Qt::Sheet);
                d->setWindowModality(Qt::WindowModal);
                int ret=d->exec();
                delete d;
                if (ret==QMessageBox::Save)
                {
                    SaveDoc();
                    ui->mdiArea->setCurrentWidget(sw);
                    return;
                }
                if (ret==QMessageBox::Cancel) return;
                ui->mdiArea->removeWidget(sw);
                delete sw;
            }
            else
            {
                ui->mdiArea->setCurrentWidget(sw);
                return;
            }
        }
    }
    ui->FadingWidget->setTransitionType(QFadingWidget::DarkFade);
    ui->FadingWidget->prepare();
    //QCustomFrame* TranspFrame=new QCustomFrame(this);
    //TranspFrame->setGeometry(ui->centralFrame->geometry());
    //TranspFrame->show();
    //TranspFrame->raise();
    //QSpinner* spin=new QSpinner(TranspFrame);
    //QMacProgressIndicator* progressWheel= new QMacProgressIndicator(this,":/aqua_icon_gray.png",QSize(150,150));
    CScoreDoc* document=new CScoreDoc(this);
    document->setWindowTitle(Path);
    document->Path=Path;
    document->Load(Path);
    ui->mdiArea->addWidget(document);
    ui->mdiArea->setCurrentWidget(document);
    document->showMaximized();
    document->Dirty=false;
    AddRecent(Path);
    UpdateSidebar();
    //ui->SidebarTree->SetSelected(ui->mdiArea->currentIndex(),CurrentDoc()->lv->CurrentLayout(),CurrentDoc()->GetView()==1);
    UpdateStatus();
    //delete spin;
    //delete TranspFrame;
    //delete progressWheel;
    ui->FadingWidget->fade();
}

void MainWindow::RequestMidiFile(QString &path, OCPlayControl::PlayLocation mode)
{
    if (CurrentDoc()==0) return;
    int StartBar=0;
    if ((mode!=OCPlayControl::PlayEverything) && (CurrentDoc()->GetView()==0)) StartBar=CurrentDoc()->sv->StartBar();
    if ((mode==OCPlayControl::PlayStaffFromHere) && (CurrentDoc()->GetView()==0))
    {
        CurrentDoc()->sv->Play(StartBar,100,midifl,CurrentDoc()->sv->ActiveStaff());
    }
    else
    {
        CurrentDoc()->sv->Play(StartBar,100,midifl);
    }
    path=midifl;
}

void MainWindow::OpenDialog()
{
    QFileDialog* d=new QFileDialog(this,Qt::Sheet);
    //d->setWindowModality(Qt::WindowModal);
    d->setWindowTitle("Open Score");
    d->setNameFilter(tr("XML Score Files (*.mus *.xml)"));
    d->setFileMode(QFileDialog::ExistingFiles);
    if (d->exec()==QDialog::Accepted) foreach(QString s,d->selectedFiles()) OpenDoc(s);
    delete d;
}

bool MainWindow::SaveDoc()
{
    if (CurrentDoc()==0) return false;
    AppendMixerXML(CurrentDoc());
    if (CurrentDoc()->Path.length()==0) return SaveDialog();
    if (!CurrentDoc()->sv->Save(CurrentDoc()->Path))
    {
        QMessageBox* d=new QMessageBox(QMessageBox::Warning,"Object Composer XML","Could not Save File! It might be Read Only!",QMessageBox::Ok,this,Qt::Sheet);
        d->setWindowModality(Qt::WindowModal);
        d->exec();
        delete d;
    }
    AddRecent(CurrentDoc()->Path);
    UpdateSidebar();
    CurrentDoc()->Dirty=false;
    return true;
}

bool MainWindow::SaveDialog()
{
    if (CurrentDoc()==0) return false;
    QFileDialog d(this,Qt::Sheet);
    //d.setWindowModality(Qt::WindowModal);
    d.setWindowTitle("Save Score");
    d.setNameFilter(tr("XML Score Files (*.mus *.xml)"));
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setFileMode(QFileDialog::AnyFile);
    if (CurrentDoc()->Path.isEmpty())
    {
        d.selectFile("Untitled.mus");
    }
    else
    {
        d.selectFile(CurrentDoc()->Path);
    }
    if (d.exec() != QDialog::Accepted) return false;
    if (d.selectedFiles().count())
    {
        QString filename=d.selectedFiles().first();
        CurrentDoc()->setWindowTitle(filename);
        CurrentDoc()->Path=filename;
        AddRecent(CurrentDoc()->Path);
        UpdateSidebar();
        return SaveDoc();
    }
    return false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef __MACOSX_CORE__
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
#endif
    playControl->Stop();
    midi2Wav->HideMixer();
    for (int i=ui->mdiArea->count()-1;i>=0;i--)
    {
        QWidget* sw=ui->mdiArea->widget(i);
        CScoreDoc* doc=(CScoreDoc*)sw;
        bool Cancel=false;
        DocumentClosed(doc,Cancel);
        if (Cancel)
        {
            event->ignore();
            return;
        }
        ui->mdiArea->removeWidget(sw);
        delete sw;
    }
}

void MainWindow::LoadRecent()
{
    QMenu* recentMenu=ui->menuRecent_Documents;
    recentMenu->clear();
    foreach (QAction* a,RecentActions) RecentMapper->removeMappings(a);
    qDeleteAll(RecentActions);
    RecentActions.clear();
    RecentFiles=OCPresets("RecentDocuments").GetValue("RecentDocuments").toStringList();
    int i=0;
    foreach (QString s, RecentFiles)
    {
        QAction* a=new QAction(QString::number(i+1)+" "+s,this);
        a->setEnabled(QFileInfo(s).exists());
        connect( a, SIGNAL(triggered()), RecentMapper, SLOT(map()) );
        RecentMapper->setMapping(a,s);
        RecentActions.append(a);
        i++;
    }
    recentMenu->addActions(RecentActions);
}

void MainWindow::AddRecent(QString Path)
{
    RecentFiles.removeOne(Path);
    RecentFiles.prepend(Path);
    while (RecentFiles.count()>recentmax) RecentFiles.removeLast();

    QMenu* recentMenu=ui->menuRecent_Documents;
    recentMenu->clear();
    foreach(QAction* a,RecentActions) RecentMapper->removeMappings(a);
    qDeleteAll(RecentActions);
    RecentActions.clear();
    for (int i=0;i<RecentFiles.count();i++)
    {
        QString s=RecentFiles[i];
        QAction* a=new QAction(QString::number(i+1)+" "+s,this);
        a->setEnabled(QFileInfo(s).exists());
        connect( a, SIGNAL(triggered()), RecentMapper, SLOT(map()) );
        RecentMapper->setMapping(a,s);
        RecentActions.append(a);
    }
    recentMenu->addActions(RecentActions);
}

void MainWindow::DocumentClosed(CScoreDoc* document,bool& Cancel)
{
    AppendMixerXML(document);
    UpdateWindowList();
    UpdateSidebar();
    UpdateStatus();
    setWindowTitle(apptitle);
    if (document->Dirty && (document->UndoText().length()!=0))
    {
        this->raise();
        QMessageBox* d= new QMessageBox(QMessageBox::Warning, apptitle,"The document '" + document->windowTitle() + "' has been modified.\nDo you want to save your changes?",QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, this,Qt::Sheet);
        d->setWindowModality(Qt::WindowModal);
        int ret=d->exec();
        delete d;
        if (ret==QMessageBox::Save) Cancel=!SaveDoc();
        if (ret==QMessageBox::Cancel) Cancel=true;
    }
}

void MainWindow::LayoutsChanged()
{
    UpdateSidebar();
    UpdateStatus();
}

void MainWindow::AppendMixerXML(CScoreDoc* document)
{
    if (!playControl->MixerXML.isEmpty())
    {
        QDomLiteElement* mixer=new QDomLiteElement("MIDIMixer");
        mixer->appendChildFromString(midi2Wav->Save());
        QDomLiteElement* Doc=document->sv->XMLScore.getXML()->documentElement;
        QDomLiteElement* mx=Doc->elementByTag("MIDIMixer");
        if (mx != NULL)
        {
            Doc->replaceChild(mx,mixer);
        }
        else
        {
            Doc->appendChild(mixer);
        }
    }
}

void MainWindow::DocumentActivated(int Index)
{
    if (Index==-1) return;
    for (int i=0;i<ui->mdiArea->count();i++)
    {
        CScoreDoc* document=(CScoreDoc*)ui->mdiArea->widget(i);
        if (document->disconnect(this))
        {
            document->sv->disconnect(this);
            document->lv->disconnect(this);
            document->pw->disconnect(this);
            document->mt->disconnect(this);
            document->bw->disconnect(this);
            document->pno->disconnect(this);
            if (!playControl->MixerXML.isEmpty())
            {
                playControl->Stop();
                midi2Wav->HideMixer();
                AppendMixerXML(document);
            }
            playControl->MixerXML.clear();
        }
    }
    ZoomSlider->disconnect();
    btnZoomOut->disconnect();
    btnZoomIn->disconnect();
    CScoreDoc* document=(CScoreDoc*)ui->mdiArea->widget(Index);

    QDomLiteElement* mx=document->sv->XMLScore.getXML()->documentElement->elementByTag("MIDIMixer");
    playControl->MixerXML.clear();
    if (mx != NULL) playControl->MixerXML=mx->firstChild()->toString();
    playControl->PreLoad();
    connect(document->pno,SIGNAL(NoteOn(int)),this,SLOT(NoteOn(int)));
    connect(document->pno,SIGNAL(NoteOff(int)),this,SLOT(NoteOff(int)));

    connect(document->sv,SIGNAL(NoteOn(int)),this,SLOT(NoteOn(int)));
    connect(document->sv,SIGNAL(NoteOff(int)),this,SLOT(NoteOff(int)));
    connect(document,SIGNAL(Close(CScoreDoc*,bool&)),this,SLOT(DocumentClosed(CScoreDoc*,bool&)));

    connect(document->sv,SIGNAL(SelectionChanged()),this,SLOT(SelectionChanged()));
    connect(document->sv,SIGNAL(BarChanged()),this,SLOT(BarChanged()));
    connect(document->sv,SIGNAL(ScoreChanged()),this,SLOT(ScoreChanged()));
    connect(document->lv,SIGNAL(SelectionChanged()),this,SLOT(UpdateStatus()));
    connect(document->lv,SIGNAL(Popup(QPoint)),this,SLOT(ShowLayoutPopup(QPoint)));
    connect(document,SIGNAL(LayoutsChanged()),this,SLOT(LayoutsChanged()));
    connect(document->pw,SIGNAL(Changed(QString,OCProperties*)),this,SLOT(ChangeProperty(QString,OCProperties*)));
    connect(document->sv,SIGNAL(Popup(QPoint)),this,SLOT(ShowPropertiesPopup(QPoint)));
    connect(document->mt,SIGNAL(Popup(QPoint)),this,SLOT(ShowPropertiesPopup(QPoint)));
    connect(document->bw,SIGNAL(Popup(QPoint)),this,SLOT(ShowPropertiesPopup(QPoint)));
    connect(document->mt,SIGNAL(SelectionChanged()),this,SLOT(TreeSelectionChanged()));
    connect(document->mt,SIGNAL(Delete(int)),this,SLOT(DeleteItem(int)));
    connect(document->bw,SIGNAL(BarChanged(int,int,int)),this,SLOT(SetBar(int,int,int)));
    connect(document,SIGNAL(Changed()),this,SLOT(DocChanged()));
    connect(document,SIGNAL(ScoreChanged()),this,SLOT(ScoreChanged()));
    connect(document,SIGNAL(StaffChanged()),this,SLOT(UpdateBW()));
    connect(document->bw,SIGNAL(SelectionChanged(QRect)),this,SLOT(BarSelectionChanged(QRect)));
    connect(ZoomSlider,SIGNAL(valueChanged(int)),this,SLOT(SetZoom(int)));
    connect(btnZoomOut,SIGNAL(clicked()),this,SLOT(ZoomOut()));
    connect(btnZoomIn,SIGNAL(clicked()),this,SLOT(ZoomIn()));

    UpdateSV();
    UpdateTree();
    UpdateBW();
    UpdatePW();
    UpdateWindowList();
    UpdateSidebar();
    UpdateStatus();
    DocChanged();
}

CScoreDoc* MainWindow::CurrentDoc()
{
    QWidget* win = ui->mdiArea->currentWidget();
    if (win==0) win=ui->mdiArea->widget(0);
    if (win==0) return 0;
    return (CScoreDoc*)win;
}

void MainWindow::SetBar(int Bar, int Staff, int Voice)
{
    QComboBox* cbStaffs=CurrentDoc()->findChild<QComboBox*>("cbStaffs");
    QComboBox* cbVoices=CurrentDoc()->findChild<QComboBox*>("cbVoices");
    cbStaffs->setCurrentIndex(Staff);
    cbVoices->setCurrentIndex(Voice);
    CurrentDoc()->sv->setStartBar(Bar);
    CurrentDoc()->sv->Paint(tsNavigate,true);
    CurrentDoc()->sv->EnsureVisible();
    UpdateStatus();
}

void MainWindow::ChangeProperty(QString Name, OCProperties* p)
{
    if (CurrentDoc()==0) return;
    ScoreViewXML* sv=CurrentDoc()->sv;
    CurrentDoc()->MakeBackup("Change Property "+Name);
    if (p->GetItem(Name)->PropertyType==pwCustom)
    {
        OCRefreshMode RefreshMode;
        XMLSimpleSymbolWrapper XMLSymbol=sv->GetSymbol();
        if (OCSymbolsCollection::editevent(XMLSymbol,RefreshMode,this))
        {
            sv->Paint(RefreshMode);
            UpdateBW();
            UpdateTree();
            UpdateStatus();
            DocChanged();
            p=sv->GetProperties();
        }
        return;
    }
    sv->ChangeProperty(sv->Cursor.Pointers(),Name,p->GetValue(Name));
    if (Name=="Pitch") sv->Sound();
    sv->Paint(tsReformat);
    UpdateBW();
    UpdateTree();
    UpdateStatus();
    DocChanged();
    p=sv->GetProperties();
}

void MainWindow::ResetPositions()
{
    if (CurrentDoc()==0) return;
    ScoreViewXML* sv=CurrentDoc()->sv;
    CurrentDoc()->MakeBackup(Action("actionReset_Position")->text());
    CopyIsBars=CurrentDoc()->SelectionStaff>-1;
    QStringList l=QStringList() << "Left" << "Top" << "Size" << "AccidentalLeft" << "Curve" << "Gap";
    if (CopyIsBars)
    {
        for (int i=0;i<CurrentDoc()->SelectionList.count();i++)
        {
            for (int p=CurrentDoc()->SelectionList.at(i).first;p<=CurrentDoc()->SelectionList.at(i).second;p++)
            {
                sv->ChangeProperties(p,CurrentDoc()->sv->MarkToStaff(CurrentDoc()->SelectionStaff+i),CurrentDoc()->sv->MarkToVoice(CurrentDoc()->SelectionStaff+i),l,0);
            }
        }
    }
    else
    {
        const QList<int>& ptrs=sv->Cursor.Pointers();
        for (int j=0;j<ptrs.count();j++)
        {
            sv->ChangeProperties(ptrs[j],l,0);
        }
    }
    CurrentDoc()->sv->Paint(tsVoiceIndexChanged);
    UpdatePW();
    UpdateTree();
    UpdateStatus();
    DocChanged();
}

void MainWindow::ResetSystemPosition()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->ResetSystem();
}

void MainWindow::SelectAll()
{
    if (CurrentDoc()==0) return;
    CopyIsBars=CurrentDoc()->SelectionStaff>-1;
    if (CopyIsBars)
    {
        CurrentDoc()->bw->SelectAll();
    }
    else
    {
        CurrentDoc()->sv->Cursor.SetPos(0);
        CurrentDoc()->sv->Cursor.ExtendSel(CurrentDoc()->sv->VoiceLen()-1);
        UpdateSV();
        SelectionChanged();
    }
}

void MainWindow::SelectFromStart()
{
    if (CurrentDoc()==0) return;
    CopyIsBars=CurrentDoc()->SelectionStaff>-1;
    if (CopyIsBars)
    {
        CurrentDoc()->bw->SelectFromStart();
    }
    else
    {
        CurrentDoc()->sv->Cursor.ExtendSel(0);
        UpdateSV();
        SelectionChanged();
    }
}

void MainWindow::SelectToEnd()
{
    if (CurrentDoc()==0) return;
    CopyIsBars=CurrentDoc()->SelectionStaff>-1;
    if (CopyIsBars)
    {
        CurrentDoc()->bw->SelectToEnd();
    }
    else
    {
        CurrentDoc()->sv->Cursor.ExtendSel(CurrentDoc()->sv->VoiceLen()-1);
        UpdateSV();
        SelectionChanged();
    }
}

void MainWindow::ScoreChanged()
{
    UpdatePW();
    UpdateBW();
    UpdateTree();
    UpdateStatus();
    DocChanged();
}

void MainWindow::SelectionChanged()
{
    UpdatePW();
    UpdateTree();
    //UpdateBW();
    UpdateStatus();
    CurrentDoc()->SelectionList=CurrentDoc()->sv->SelectionList();
    CurrentDoc()->SelectionStaff=-1;
    Action("actionCut")->setText(tr("Cut Symbols"));
    Action("actionCopy")->setText(tr("Copy Symbols"));
}

void MainWindow::BarChanged()
{
    UpdateBW();
    UpdateTree();
    UpdateStatus();
}

void MainWindow::TreeSelectionChanged()
{
    UpdateSV();
    UpdatePW();
    CurrentDoc()->sv->Sound();
    //UpdateBW();
    UpdateStatus();
    CurrentDoc()->SelectionList=CurrentDoc()->sv->SelectionList();
    CurrentDoc()->SelectionStaff=-1;
    Action("actionCut")->setText(tr("Cut Symbols"));
    Action("actionCopy")->setText(tr("Copy Symbols"));
}

void MainWindow::BarSelectionChanged(QRect Selection)
{
    CurrentDoc()->SelectionList=CurrentDoc()->sv->SelectionList(Selection.left(),Selection.right(),Selection.top(),Selection.bottom());
    CurrentDoc()->SelectionStaff=Selection.top();
    CurrentDoc()->SelectionBar=Selection.left();
    UpdateStatus();
    Action("actionCut")->setText(tr("Cut Voices"));
    Action("actionCopy")->setText(tr("Copy Voices"));
}

void MainWindow::ShowScore()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->SetView(0);
    TabChanged();
}

void MainWindow::ShowLayout()
{
    if (CurrentDoc()==0) return;
    if (CurrentDoc()->lv->NumOfLayouts()==0) if (!CurrentDoc()->AddLayout()) return;
    CurrentDoc()->SetView(1);
    TabChanged();
}

void MainWindow::TabChanged()
{
    UpdateStatus();
    DocChanged();
}

void MainWindow::DocChanged()
{
    if (CurrentDoc()==0) return;
    if (CurrentDoc()->UndoText().length())
    {
        Action("actionUndo")->setEnabled(true);
        Action("actionUndo")->setText("Undo "+CurrentDoc()->UndoText());
    }
    else
    {
        Action("actionUndo")->setEnabled(false);
        Action("actionUndo")->setText("Undo");
    }
    if (CurrentDoc()->RedoText().length())
    {
        Action("actionRedo")->setEnabled(true);
        Action("actionRedo")->setText("Redo "+CurrentDoc()->RedoText());
    }
    else
    {
        Action("actionRedo")->setEnabled(false);
        Action("actionRedo")->setText("Redo");
    }
    if (CurrentDoc()->GetView()==1)
    {
        foreach(QAction* a,ui->menuLayouts->actions()) a->setEnabled(CurrentDoc()->lv->NumOfLayouts()>0);
    }
}

void MainWindow::UpdateTree()
{
    ScoreViewXML* sv=CurrentDoc()->sv;
    OCBarMap l=sv->BarMap();
    //CurrentDoc()->mt->Fill(sv->XMLScore,sv->StartBar(),sv->ActiveStaff(),sv->ActiveVoice(),sv->FindPointerToBar(sv->StartBar()),sv->FindPointerToBar(sv->StartBar()+sv->BarsActuallyDisplayed()),sv->FindFirstMeter(),&sv->Cursor);
    CurrentDoc()->mt->Fill(sv->XMLScore,l,sv->StartBar(),sv->BarsActuallyDisplayed(),sv->ActiveStaff(),sv->ActiveVoice(),&sv->Cursor);
}

void MainWindow::UpdateSV()
{
    CurrentDoc()->sv->Paint(tsRedrawActiveStave);
}

void MainWindow::UpdatePW()
{
    CurrentDoc()->pw->Fill(CurrentDoc()->sv->GetProperties());
}

void MainWindow::UpdateBW()
{
    ScoreViewXML* sv=CurrentDoc()->sv;
    OCBarMap l=sv->BarMap();
    CurrentDoc()->bw->Fill(l,sv->StartBar(),sv->ActiveStaff(),sv->ActiveVoice());
}

void MainWindow::ShowMenu(QString Name,bool Show)
{
    QMenu* m=findChild<QMenu*>(Name);
    m->menuAction()->setVisible(Show);
    foreach(QAction* a,m->actions()) a->setVisible(Show);
}

void MainWindow::ShowAction(QString Name, bool Show)
{
    Action(Name)->setVisible(Show);
}

QAction* MainWindow::Action(QString Name)
{
    return findChild<QAction*>(Name);
}

void MainWindow::UpdateStatus()
{
    QString title=CurrentDoc()->windowTitle();
    if (CurrentDoc()->Dirty && (CurrentDoc()->UndoText().length()!=0)) title+="*";
    setWindowTitle(title + " - " + apptitle);

    int View=CurrentDoc()->GetView();

    ui->actionScore->setEnabled(View==1);
    ui->actionLayouts->setEnabled(View==0);
    ShowMenu("menuScore",View==0);
    ShowMenu("menuLayouts",View==1);

    ShowAction("actionCut",View==0);
    ShowAction("actionCopy",View==0);
    ShowAction("actionPaste",View==0);
    ShowAction("actionSelect_All",View==0);
    ShowAction("actionFrom_Here_to_End",View==0);
    ShowAction("actionFrom_Start_to_Here",View==0);
    ShowAction("actionReset_Position",View==0);
    ShowAction("actionLayoutSystem",View==1);
    ShowAction("actionLayoutPage",View==1);
    ShowAction("actionLayoutLayout",View==1);

    Action("actionPrint_Preview")->setEnabled(View==1);
    Action("actionPrint")->setEnabled(View==1);

    ZoomSlider->blockSignals(true);
    ZoomSlider->setValue(CurrentDoc()->GetZoom());
    ZoomSlider->setToolTip("Zoom "+QString::number(CurrentDoc()->GetZoom()*2)+"%");
    ZoomSlider->blockSignals(false);

    RightSideButton->setVisible(View==1);
    if (View==1)
    {
        if (CurrentDoc()->spSidebar->isCollapsed())
        {
            RightSideButton->setIcon(QIcon(":/mini/mini/left.png").pixmap(20,20));
            RightSideButton->setToolTip("Show Sidebar");
        }
        else
        {
            RightSideButton->setIcon(QIcon(":/mini/mini/right.png").pixmap(20.20));
            RightSideButton->setToolTip("Hide Sidebar");
        }
    }
    QMacSplitter* sp=findChild<QMacSplitter*>("sidebarSplitter");
    if (sp->isCollapsed())
    {
        LeftSideButton->setIcon(QIcon(":/mini/mini/right.png").pixmap(20,20));
        LeftSideButton->setToolTip("Show Sidebar");
    }
    else
    {
        LeftSideButton->setIcon(QIcon(":/mini/mini/left.png").pixmap(20.20));
        LeftSideButton->setToolTip("Hide Sidebar");
    }
    ui->CloseDocument->setEnabled(ui->mdiArea->count()>1);
    closeAction->setEnabled(ui->mdiArea->count()>1);

    if (View==0)
    {
        QString msg=CurrentDoc()->sv->StaffName(CurrentDoc()->sv->ActiveStaff());
        msg += " - Voice " + QString::number(CurrentDoc()->sv->ActiveVoice() + 1);
        msg += " - Bar " + QString::number(CurrentDoc()->sv->StartBar() + 1);
        //msg += (CurrentDoc()->sv->Cursor.GetMode()==CMInsert) ? " - INSERT":" - OVERWRITE";
        CurrentDoc()->SetStatusLabel(msg);

        ScoreViewXML* sv=CurrentDoc()->sv;

        Action("actionPaste")->setEnabled(QApplication::clipboard()->text().startsWith("<!DOCTYPE OCCopyData"));
        if (QApplication::clipboard()->text().startsWith("<!DOCTYPE OCCopyDataVoices>"))
        {
            Action("actionPaste")->setText(tr("Paste Voices"));
        }
        else if (QApplication::clipboard()->text().startsWith("<!DOCTYPE OCCopyDataSymbols>"))
        {
            Action("actionPaste")->setText(tr("Paste Symbols"));
        }
        bool EnableCopy=((sv->Cursor.SelCount() > 0) & (sv->Cursor.GetPos() < sv->VoiceLen()-1));
        EnableCopy=EnableCopy | (CurrentDoc()->SelectionStaff>-1);
        Action("actionCut")->setEnabled(EnableCopy);
        Action("actionCopy")->setEnabled(EnableCopy);
        Action("actionReset_Position")->setEnabled((sv->Cursor.GetPos() < sv->VoiceLen()-1) | (CurrentDoc()->SelectionStaff>-1));
        Action("actionSelect_All")->setEnabled(sv->VoiceLen()>2);
        Action("actionFrom_Here_to_End")->setEnabled(sv->Cursor.GetPos()<sv->VoiceLen()-2);
        Action("actionFrom_Start_to_Here")->setEnabled((sv->Cursor.GetPos()<sv->VoiceLen()-1) && (sv->Cursor.GetPos()>0));
        Action("actionBack")->setEnabled(CurrentDoc()->sv->StartBar() != 0);
        Action("actionFirst")->setEnabled(CurrentDoc()->sv->StartBar() != 0);
        ui->actionForward->setEnabled(CurrentDoc()->sv->CanTurnPage());
        ui->actionLast->setEnabled(CurrentDoc()->sv->CanTurnPage());
        ui->actionPrevious_Staff->setEnabled(CurrentDoc()->sv->StaffCount()>1);
        ui->actionNext_Staff->setEnabled(CurrentDoc()->sv->StaffCount()>1);
        ui->actionPrevious_Voice->setEnabled(CurrentDoc()->sv->VoiceCount()>1);
        ui->actionNext_Voice->setEnabled(CurrentDoc()->sv->VoiceCount()>1);

        CurrentDoc()->SetDurated();
    }
    else
    {
        if (CurrentDoc()->lv->NumOfLayouts()==0)
        {
            CurrentDoc()->SetStatusLabel(QString());
            return;
        }
        LayoutViewXML* lv=CurrentDoc()->lv;
        CLayout* l=lv->lc.CurrentLayout;
        if (l->NumOfPages()>0)
        {
            bool isLastPage=(lv->ActivePage()==l->NumOfPages()-1);
            bool isLastSystem=(lv->ActiveSystem()==l->NumOfSystems(lv->ActivePage())-1);
            ui->actionGet_1_Bar_from_Next_System->setEnabled(!(isLastSystem & isLastPage));
            ui->actionMove_1_Bar_to_Next_System->setEnabled((l->ActivePage->ActiveSystem->EndBar-l->ActivePage->ActiveSystem->StartBar>1));
            ui->actionGet_1_System_from_Next_Page->setEnabled(!isLastPage);
            ui->actionMove_1_System_to_Next_Page->setEnabled(l->NumOfSystems(lv->ActivePage())>1);
            ui->actionEdit_Staffs->setEnabled(CurrentDoc()->sv->StaffCount()>1);
        }
        else
        {
            ui->actionGet_1_Bar_from_Next_System->setEnabled(false);
            ui->actionMove_1_Bar_to_Next_System->setEnabled(false);
            ui->actionGet_1_System_from_Next_Page->setEnabled(false);
            ui->actionMove_1_System_to_Next_Page->setEnabled(false);
            ui->actionEdit_Staffs->setEnabled(false);
        }
        QString msg=CurrentDoc()->lv->LayoutName(CurrentDoc()->lv->CurrentLayout());
        msg += " - Page " + QString::number(CurrentDoc()->lv->ActivePage() + 1);
        msg += " - System " + QString::number(CurrentDoc()->lv->ActiveSystem() + 1);
        CurrentDoc()->SetStatusLabel(msg);
    }
    ui->SidebarTree->SetSelected(ui->mdiArea->currentIndex(),CurrentDoc()->lv->CurrentLayout(),CurrentDoc()->GetView()==1);
}

void MainWindow::SetZoom(int zoom)
{
    ZoomSlider->setToolTip("Zoom "+QString::number(zoom*2)+"%");
    if (CurrentDoc() !=0) CurrentDoc()->SetZoom(zoom);
}

void MainWindow::NoteOn(int Pitch)
{
    OCMIDIVars m=CurrentDoc()->sv->GetCurrentMIDI();
    if (CurrentDoc() !=0)
    {
        midi2Wav->NoteOn(Inside(Pitch+m.Transpose+m.Octave,1,127,12),m.Channel,100.0*playControl->Volume(),m.Patch);
    }
    else
    {
        midi2Wav->NoteOn(Inside(Pitch+m.Transpose+m.Octave,1,127,12),0,100.0*playControl->Volume());
    }
    /*
    if (MIDI.getPortCount()==0) return;
    OCPlayControl* pc=findChild<OCPlayControl*>();
    std::vector<unsigned char> v;
    OCMIDIVars m=CurrentDoc()->sv->GetCurrentMIDI();
    if (CurrentDoc() !=0)
    {
        v.push_back(0xC0+m.Channel);
        v.push_back(m.Patch);
        v.push_back(80);
        MIDI.sendMessage(&v);
        v.clear();
        v.push_back(0x90+m.Channel);
    }
    else
    {
        v.push_back(0x90);
    }
    v.push_back(Inside(Pitch+m.Transpose+m.Octave,1,127,12));
    v.push_back(100.0*pc->Volume());
    MIDI.sendMessage(&v);
    */
}

void MainWindow::NoteOff(int Pitch)
{
    OCMIDIVars m=CurrentDoc()->sv->GetCurrentMIDI();
    if (CurrentDoc() !=0)
    {
        midi2Wav->NoteOff(Inside(Pitch+m.Transpose+m.Octave,1,127,12),m.Channel);
    }
    else
    {
        midi2Wav->NoteOff(Inside(Pitch+m.Transpose+m.Octave,1,127,12));
    }
    /*
    if (MIDI.getPortCount()==0) return;
    std::vector<unsigned char> v;
    OCMIDIVars m=CurrentDoc()->sv->GetCurrentMIDI();
    if (CurrentDoc() !=0)
    {
        v.push_back(0x80+m.Channel);
    }
    else
    {
        v.push_back(0x80);
    }
    v.push_back(Inside(Pitch+m.Transpose+m.Octave,1,127,12));
    MIDI.sendMessage(&v);
    */
}

void MainWindow::ExportMidi()
{
    if (CurrentDoc()==0) return;
    QFileDialog d(this,Qt::Sheet);
    //d.setWindowModality(Qt::WindowModal);
    d.setWindowTitle("Export MIDI");
    d.setNameFilter(tr("MIDI Files (*.mid)"));
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setFileMode(QFileDialog::AnyFile);
    if (CurrentDoc()->Path.isEmpty())
    {
        d.selectFile("Untitled.mid");
    }
    else
    {
        d.selectFile(CurrentDoc()->Path.replace(".mus",".mid",Qt::CaseInsensitive));
    }
    if (d.exec() != QDialog::Accepted) return;
    if (d.selectedFiles().count())
    {
        QString path=d.selectedFiles().first();
        CurrentDoc()->sv->Play(0,0,path);
    }
}

void MainWindow::Undo()
{
    CurrentDoc()->Undo();
    if (CurrentDoc()->GetView()==0)
    {
        CurrentDoc()->sv->Paint(tsReformat);
        UpdatePW();
        UpdateBW();
        UpdateTree();
        UpdateStatus();
    }
    else
    {
        CurrentDoc()->UpdateLayoutView();
    }
}

void MainWindow::Redo()
{
    CurrentDoc()->Redo();
    if (CurrentDoc()->GetView()==0)
    {
        CurrentDoc()->sv->Paint(tsReformat);
        UpdatePW();
        UpdateBW();
        UpdateTree();
        UpdateStatus();
    }
    else
    {
        CurrentDoc()->UpdateLayoutView();
    }
}

void MainWindow::DeleteItem(int Pointer)
{
    if (CurrentDoc() == 0 ) return;
    CurrentDoc()->MakeBackup("Delete");
    CurrentDoc()->sv->Delete(Pointer);
    SelectionChanged();
    CurrentDoc()->sv->Paint(tsReformat);
    UpdateBW();
    UpdatePW();
    UpdateTree();
    CurrentDoc()->sv->setFocus();
    UpdateStatus();
    DocChanged();
}

void MainWindow::Cut()
{
    if (CurrentDoc() == 0 ) return;
    CurrentDoc()->MakeBackup(Action("actionCut")->text());
    Copy();
    if (CopyIsBars)
    {
        for (int i=0;i<CurrentDoc()->SelectionList.count();i++)
        {
            CurrentDoc()->sv->Delete(CurrentDoc()->sv->MarkToStaff(CurrentDoc()->SelectionStaff+i),CurrentDoc()->sv->MarkToVoice(CurrentDoc()->SelectionStaff+i),CurrentDoc()->SelectionList.at(i).first,CurrentDoc()->SelectionList.at(i).second);
        }
    }
    else
    {
        CurrentDoc()->sv->Delete();
    }
    CurrentDoc()->sv->Paint(tsReformat,true);
    //CurrentDoc()->sv->Cursor.SetMode(CMInsert);
    CurrentDoc()->sv->Cursor.SetZero();
    UpdateStatus();
    DocChanged();
}

void MainWindow::Copy()
{
    if (CurrentDoc() == 0 ) return;
    QDomLiteDocument* CopyData;
    CopyIsBars=CurrentDoc()->SelectionStaff>-1;
    if (CopyIsBars)
    {
        CopyData=new QDomLiteDocument("OCCopyDataVoices","Score");
    }
    else
    {
        CopyData=new QDomLiteDocument("OCCopyDataSymbols","Score");
    }
    CopyData->documentElement->clearChildren();
    if (CopyIsBars)
    {
        for (int i=0;i<CurrentDoc()->SelectionList.count();i++)
        {
            CopyData->documentElement->appendClone(CurrentDoc()->sv->GetClipBoardData(CurrentDoc()->sv->MarkToStaff(CurrentDoc()->SelectionStaff+i),CurrentDoc()->sv->MarkToVoice(CurrentDoc()->SelectionStaff+i),CurrentDoc()->SelectionList.at(i).first,CurrentDoc()->SelectionList.at(i).second));
        }
    }
    else
    {
        CopyData->documentElement->appendClone(CurrentDoc()->sv->GetClipBoardData());
    }
    QApplication::clipboard()->setText(CopyData->toString());
    delete CopyData;
    UpdateStatus();
}

void MainWindow::Paste()
{
    if (CurrentDoc() == 0 ) return;
    QDomLiteDocument* CopyData=new QDomLiteDocument("CopyData","Score");
    CopyData->fromString(QApplication::clipboard()->text());
    CurrentDoc()->MakeBackup(Action("actionPaste")->text());
    if (CurrentDoc()->SelectionStaff<0)
    {
        //CurrentDoc()->sv->Cursor.SetMode(CMInsert);
        CurrentDoc()->sv->Cursor.SetZero();
        CurrentDoc()->sv->PasteClipBoardData(CurrentDoc()->sv->Cursor.GetPos(),CopyData->documentElement->childElement(0)->clone());
        CurrentDoc()->sv->Cursor.SetPos(CurrentDoc()->sv->Cursor.GetPos()+CopyData->documentElement->childElement(0)->childCount());
    }
    else
    {
        for (int i=0;i<CopyData->documentElement->childCount();i++)
        {
            if (CurrentDoc()->sv->MarkToStaff(CurrentDoc()->SelectionStaff+i)>CurrentDoc()->sv->StaffCount()) break;
            int Pointer=CurrentDoc()->sv->FindPointerToBar(CurrentDoc()->sv->MarkToStaff(CurrentDoc()->SelectionStaff+i),CurrentDoc()->sv->MarkToVoice(CurrentDoc()->SelectionStaff+i),CurrentDoc()->SelectionBar);
            CurrentDoc()->sv->PasteClipBoardData(CurrentDoc()->sv->MarkToStaff(CurrentDoc()->SelectionStaff+i),CurrentDoc()->sv->MarkToVoice(CurrentDoc()->SelectionStaff+i),Pointer,CopyData->documentElement->childElement(i)->clone());
        }

    }
    CurrentDoc()->sv->Paint(tsReformat,true);
    UpdateStatus();
    DocChanged();
    delete CopyData;
}

void MainWindow::ShowPropertiesPopup(QPoint Pos)
{
    if (CurrentDoc()==0) return;
    QTransMenu* m=new QTransMenu(this);
    m->addAction(Action("actionCut"));
    m->addAction(Action("actionCopy"));
    m->addAction(Action("actionPaste"));
    m->addSeparator();
    m->addAction(ui->actionUndo);
    m->addAction(ui->actionRedo);
    m->addSeparator();
    m->addAction(ui->actionSelect_All);
    m->addAction(ui->actionFrom_Start_to_Here);
    m->addAction(ui->actionFrom_Here_to_End);
    m->addSeparator();
    m->addAction(Action("actionReset_Position"));
    m->addSeparator();
    m->addAction(ui->actionSettings);
    m->exec(Pos);
    delete m;
}

void MainWindow::ShowLayoutPopup(QPoint Pos)
{
    if (CurrentDoc()==0) return;
    QTransMenu* m=new QTransMenu(this);
    m->addAction(ui->actionUndo);
    m->addAction(ui->actionRedo);
    m->addSeparator();
    m->addAction(ui->actionAutoadjust_All);
    m->addAction(ui->actionReformat);
    m->addAction(ui->actionReformat_From_Here_to_End);
    m->addAction(ui->actionStretch_From_Here_to_End);
    m->addAction(ui->actionCompress_From_Here_to_End);
    m->addAction(ui->actionFit_Systems_2);
    m->addAction(ui->actionFit_Systems_From_Here);
    m->addSeparator();
    m->addAction(ui->actionGet_1_System_from_Next_Page);
    m->addAction(ui->actionMove_1_System_to_Next_Page);
    m->addAction(ui->actionFit_Systems);
    m->addSeparator();
    m->addAction(ui->actionGet_1_Bar_from_Next_System);
    m->addAction(ui->actionMove_1_Bar_to_Next_System);
    m->addAction(ui->actionToggle_Names);
    m->addAction(ui->actionEdit_Staffs);
    m->addAction(Action("actionReset_Position_2"));
    m->addSeparator();
    m->addAction(ui->actionLayoutSettings);
    m->exec(Pos);
    delete m;
}

void MainWindow::ShowLayoutLayoutPopup()
{
    if (CurrentDoc()==0) return;
    QTransMenu* m=new QTransMenu(this);
    m->addAction(ui->actionAutoadjust_All);
    m->addAction(ui->actionReformat);
    m->addAction(ui->actionReformat_From_Here_to_End);
    m->addAction(ui->actionStretch_From_Here_to_End);
    m->addAction(ui->actionCompress_From_Here_to_End);
    m->addSeparator();
    m->addAction(ui->actionFit_Systems_2);
    m->addAction(ui->actionFit_Systems_From_Here);
    QPoint Pos(cursor().pos());
    m->exec(Pos);
    delete m;
}

void MainWindow::ShowLayoutPagePopup()
{
    if (CurrentDoc()==0) return;
    QTransMenu* m=new QTransMenu(this);
    m->addAction(ui->actionGet_1_System_from_Next_Page);
    m->addAction(ui->actionMove_1_System_to_Next_Page);
    m->addAction(ui->actionFit_Systems);
    QPoint Pos(cursor().pos());
    m->exec(Pos);
    delete m;
}

void MainWindow::ShowLayoutSystemPopup()
{
    if (CurrentDoc()==0) return;
    QTransMenu* m=new QTransMenu(this);
    m->addAction(ui->actionGet_1_Bar_from_Next_System);
    m->addAction(ui->actionMove_1_Bar_to_Next_System);
    m->addAction(ui->actionToggle_Names);
    m->addAction(ui->actionEdit_Staffs);
    m->addAction(Action("actionReset_Position_2"));
    QPoint Pos(cursor().pos());
    m->exec(Pos);
    delete m;
}

void MainWindow::First()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->SwipeFirst();
}

void MainWindow::Back()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->SwipeBack();
}

void MainWindow::Forward()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->SwipeForward();
}

void MainWindow::Last()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->SwipeFinish();
}

void MainWindow::PrevStaff()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->sv->selectPrevStaff();
}

void MainWindow::NextStaff()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->sv->selectNextStaff();
}

void MainWindow::PrevVoice()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->sv->selectPrevVoice();
}

void MainWindow::NextVoice()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->sv->selectNextVoice();
}

void MainWindow::ZoomIn()
{
    ZoomSlider->setValue(ZoomSlider->maximum());
}

void MainWindow::ZoomOut()
{
    ZoomSlider->setValue(ZoomSlider->minimum());
}

void MainWindow::Settings()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->ShowStaffsDialog();
}

void MainWindow::PrintPreview()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->PrintPreview();
}

void MainWindow::Print()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->Print();
}

void MainWindow::PageSetup()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->PageSetup();
}

void MainWindow::GetBarFromNext()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->BarFromNext();
}

void MainWindow::MoveBarToNext()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->BarToNext();
}

void MainWindow::GetSystemFromNext()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->SystemFromNext();
}

void MainWindow::MoveSystemToNext()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->SystemToNext();
}

void MainWindow::FitSystems()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->FitMusic();
}

void MainWindow::FitFromHere()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->FitPages(CurrentDoc()->lv->ActivePage());
}

void MainWindow::FitAll()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->FitPages(0);
}

void MainWindow::ToggleNames()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->ToggleNames();
}

void MainWindow::EditStaffs()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->EditSystem();
}

void MainWindow::EditLayout()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->EditLayout();
}

void MainWindow::ReformatLayout()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->ReformatLayout(0,0);
}

void MainWindow::ReformatFromHere()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->ReformatLayout(CurrentDoc()->lv->ActivePage(),CurrentDoc()->lv->ActiveSystem());
}

void MainWindow::StretchFromHere()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->ReformatLayout(CurrentDoc()->lv->ActivePage(),CurrentDoc()->lv->ActiveSystem(),5);
}

void MainWindow::CompressFromHere()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->ReformatLayout(CurrentDoc()->lv->ActivePage(),CurrentDoc()->lv->ActiveSystem(),-5);
}

void MainWindow::AutoAdjustAll()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->AutoAdjust();
}
