#include "cscoredoc.h"
#include "ui_cscoredoc.h"
#include "qtoolbuttongrid.h"
#include "ctweaksystem.h"
//#include <QSettings>
#include <QItemDelegate>
//#include <QClipboard>
//#include <QWidgetAction>
#include "qtransmenu.h"
#include "cbarwindow.h"
#include "cpropertywindow.h"
#include "cmusictree.h"
#include "clayoutwizard.h"
#include "cstaffsdialog.h"
#include "czoomwidget.h"
#include "cscorewizard.h"
#include "cpresetsdialog.h"
#include "ceditsystem.h"
#include "cmusicxml.h"

class layoutsDelegate : public QItemDelegate {
public:
    layoutsDelegate(QObject *parent=nullptr) : QItemDelegate(parent){}
    virtual ~layoutsDelegate();
    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const
    {
        QRect r(0,rect.top()+rect.height()-16,128,14);
        (option.state & QStyle::State_Selected) ? painter->setPen("#eee") : painter->setPen(Qt::black);
        painter->drawText(r,text,QTextOption(Qt::AlignHCenter));
    }
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QItemDelegate::updateEditorGeometry(editor,option,index);
        editor->setGeometry(0,(index.row()*option.rect.height())+(option.rect.height()-16),128,14);
    }
};

layoutsDelegate::~layoutsDelegate(){}

void CScoreDoc::showLayouts()
{
    sv->ReloadXML();
    lv->SetXML(sv->XMLScore);
    if (lv->layoutCount()==0) {
        if (!AddLayout()) return;
    }
    SetView(1);
    UpdateStatus();
    //UpdateUndo();
}

CScoreDoc::CScoreDoc(QWidget* mainWindow, QWidget *parent) :
    QGraphicsView(parent),
    //CFileDocument(OCPresets().Organization(), OCPresets().Application(), _DocumentPath,parent),
    ui(new Ui::CScoreDoc),
    m_MainWindow(mainWindow) {
    Q_INIT_RESOURCE(OCResources);
    ui->setupUi(this);
    //Dirty=false;
    setAutoFillBackground(true);

    QStringList classes=OCSymbolsCollection::Classes();
    for (const QString& s : classes)
    {
        ui->searchCombo->addItem(OCSymbolsCollection::Icon(s,0),s);
    }

    connect(ui->searchCombo,&QComboBox::currentTextChanged,this,&CScoreDoc::search);
    connect(ui->searchButton,&QAbstractButton::clicked,this,&CScoreDoc::destyleSearchCombo);
    ui->searchCombo->setFocusPolicy(Qt::NoFocus);
    styleSearchCombo();
    ui->searchBack->setArrowDirection(QAppleArrowButton::RoundLeft);
    ui->searchForward->setArrowDirection(QAppleArrowButton::RoundRight);
    ui->searchBack->setEnabled(false);
    ui->searchForward->setEnabled(false);
    connect(ui->searchBack,&QAbstractButton::clicked,this,&CScoreDoc::searchPrev);
    connect(ui->searchForward,&QAbstractButton::clicked,this,&CScoreDoc::searchNext);
    ui->searchBack->setIcon(QIcon(":/24/24/locationback.png"));
    ui->searchForward->setIcon(QIcon(":/24/24/locationforward.png"));
    ui->searchLabel->setText(QString());
    ui->searchLabel->setContextMenuPolicy(Qt::CustomContextMenu);
    searchMenu = new QMenu(this);
    deleteSearchAction = searchMenu->addAction("Delete from Score");
    deleteVoiceSearchAction = searchMenu->addAction("Delete from current Voice");

    connect(deleteSearchAction,&QAction::triggered,this,&CScoreDoc::deleteSearch);
    connect(deleteVoiceSearchAction,&QAction::triggered,this,&CScoreDoc::deleteVoiceSearch);

    connect(ui->searchLabel,&QWidget::customContextMenuRequested,this,&CScoreDoc::searchMenuPopup);

    ui->NavFrame->setAutoFillBackground(true);
    m_View=-1;
    ui->SlidingWidget->setWidget(ui->allFrame);
    ui->SlidingWidget->setEasingCurve(QEasingCurve::OutQuad);
    ui->SlidingWidget->setDuration(600);
    ui->prevButton->setArrowDirection(QAppleArrowButton::RoundLeft);
    ui->nextButton->setArrowDirection(QAppleArrowButton::RoundRight);
    ui->prevButton->setIcon(QIcon(":/24/24/locationback.png"));
    ui->nextButton->setIcon(QIcon(":/24/24/locationforward.png"));
    connect(ui->prevButton,&QAbstractButton::clicked,this,&CScoreDoc::LocationBack);
    connect(ui->nextButton,&QAbstractButton::clicked,this,&CScoreDoc::LocationForward);
    ui->toScore->setArrowDirection(QAppleArrowButton::Left);
    ui->toLayout->setArrowDirection(QAppleArrowButton::Right);
    ui->prevLayout->setArrowDirection(QAppleArrowButton::RoundLeft);
    ui->nextLayout->setArrowDirection(QAppleArrowButton::RoundRight);
    ui->prevLayout->setIcon(QIcon(":/24/24/locationback.png"));
    ui->nextLayout->setIcon(QIcon(":/24/24/locationforward.png"));
    //ui->toScore->hide();
    connect(ui->toLayout,&QAbstractButton::clicked,this,&CScoreDoc::showLayouts);
    connect(ui->toScore,&QAbstractButton::clicked,this,&CScoreDoc::showScore);
    connect(ui->toggleView,&QAbstractButton::clicked,this,&CScoreDoc::toggleView);
    connect(ui->prevLayout,&QAbstractButton::clicked,this,&CScoreDoc::PrevLayout);
    connect(ui->nextLayout,&QAbstractButton::clicked,this,&CScoreDoc::NextLayout);

    SelectionStaff=-1;
    LocationIndex=-1;
    sv=findChild<ScoreViewXML*>();
    lv=findChild<LayoutViewXML*>();
    lv->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    lv->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ui->PnoScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    pno=ui->Piano;

    ui->FadingWidget_2->setWidget(sv);
    ui->FadingWidget_2->setDuration(600);

    spMain=new QMacSplitter(this);
    spMain->addWidget(ui->Toolbox);

    ui->SplitterLayout->addWidget(spMain);
    spMain->addWidget(ui->SVFrame);
    spMain->setStretchFactor(0,2);
    spMain->setStretchFactor(1,3);

    MIDI2wav = new CMIDI2wav(mainWindow);

    connect(MIDI2wav,&CMIDI2wav::PlayPointerChanged,this,&CScoreDoc::gotoBar,Qt::QueuedConnection);
    connect(MIDI2wav,&CMIDI2wav::RequestFile,this,&CScoreDoc::RequestMidiFile,Qt::DirectConnection);

    playControl=new OCPlayControl(MIDI2wav,this);
    playControl->SetVol(100);
    playControl->hide();
    playControl->Stop();

    printMenu = new QMenu(this);
    actionPrintPreview = printMenu->addAction("Print Preview...",this,&CScoreDoc::PrintPreview);
    actionPrintSetup = printMenu->addAction("Print Setup...",this,&CScoreDoc::PageSetup);
    actionPrintSetup->setMenuRole(QAction::NoRole);
    actionPrint = printMenu->addAction("Print...",QKeySequence::Print,this,&CScoreDoc::Print);

    MainMenu = new CMainMenu(this,m_MainWindow,OCSettings().organizationName(),OCSettings().applicationName(),"XML files (*.xml *.zip *.mus *.mxl *.musicxml)",_DocumentPath,this);

    mainMenu = new QMenu(this);

    MainMenu->FileMenu->addSeparator();
    actionExportMIDI = MainMenu->FileMenu->addAction("Export MIDI...",this,&CScoreDoc::ExportMidi);
    actionExportAudio = MainMenu->FileMenu->addAction("Export Audio...",this,&CScoreDoc::ExportWave);
    MainMenu->FileMenu->addSeparator();
    MainMenu->FileMenu->addActions(playControl->actionMenu->actions());
    MainMenu->FileMenu->addSeparator();
    printMenu->addSeparator();
    actionExportPDF = printMenu->addAction("Export PDF...",this,&CScoreDoc::ExportPDFDialog);
    actionExportPDF->setEnabled(false);
    printMenu->addSeparator();
    actionExportMXL = printMenu->addAction("Export MXL...",this,&CScoreDoc::ExportMXLDialog);
    actionExportMXL->setEnabled(false);
    actionExportMusicXML = printMenu->addAction("Export musicXML...",this,&CScoreDoc::ExportMusicXMLDialog);
    actionExportMusicXML->setEnabled(false);
    MainMenu->FileMenu->addActions(printMenu->actions());
    MainMenu->FileMenu->addSeparator();
    actionPreferences = MainMenu->FileMenu->addAction("Preferences..",this,&CScoreDoc::ShowPresets);
    actionPreferences->setMenuRole(QAction::ApplicationSpecificRole);
    mainMenu->addMenu(MainMenu->FileMenu);

    MainMenu->UndoMenu->actionUndo->setIcon(QIcon(":/mini/mini/repeat.png"));
    MainMenu->UndoMenu->actionRedo->setIcon(QIcon(":/mini/mini/redo.png"));
    MainMenu->EditMenu->actionCut->setIcon(QIcon(":/mini/mini/cut.png"));
    MainMenu->EditMenu->actionCopy->setIcon(QIcon(":/mini/mini/copy_page.png"));
    MainMenu->EditMenu->actionPaste->setIcon(QIcon(":/mini/mini/paste.png"));
    //EditMenu->addActions(MainMenu->EditActions->actions());
    MainMenu->EditMenu->addSeparator();
    actionProperties = MainMenu->EditMenu->addAction(QIcon(":/mini/mini/properties.png"),"Properties...",QKeySequence(Qt::ALT | Qt::Key_P),
                                           this,&CScoreDoc::PropertiesMenu);
    actionVoiceList = MainMenu->EditMenu->addAction(QIcon(":/mini/mini/list.png"),"Voice List...",QKeySequence(Qt::ALT | Qt::Key_V),
                                          this,&CScoreDoc::VoiceListMenu);
    actionBarMap = MainMenu->EditMenu->addAction(QIcon(":/mini/mini/bars.png"),"Barmap...",QKeySequence(Qt::ALT | Qt::Key_B),
                                       this,&CScoreDoc::BarMapMenu);
    MainMenu->EditMenu->addSeparator();
    MainMenu->EditMenu->addActions(sv->EditMenu->actions());
    actionResetPositions = MainMenu->EditMenu->addAction(QIcon(":/mini/mini/map.png"),"Reset Positions",this,&CScoreDoc::ResetPositions);
    MainMenu->EditMenu->addSeparator();
    rhythmMenu = MainMenu->EditMenu->addMenu("Rhythm");
    actionTriolize = rhythmMenu->addAction("Triolize",this,&CScoreDoc::Triolize);
    actionDottify = rhythmMenu->addAction("Dottify",this,&CScoreDoc::Dottify);
    actionDoubleDottify = rhythmMenu->addAction("Double dottify",this,&CScoreDoc::DoubleDottify);
    actionStraighten = rhythmMenu->addAction("Straighten",this,&CScoreDoc::Straighten);
    QMenu* EditMenu = mainMenu->addMenu("Edit");
    EditMenu->addActions(MainMenu->EditMenu->actions());

    QMenu* ViewMenu = mainMenu->addMenu("View");
    actionScore = ViewMenu->addAction("Score",this,&CScoreDoc::showScore);
    actionLayouts = ViewMenu->addAction("Layouts",this,&CScoreDoc::showLayouts);

    connect(sv,&ScoreViewXML::ScoreChanged,this,&CScoreDoc::SVChanged);
    connect(sv,&ScoreViewXML::Changed,this,&CScoreDoc::UpdateStatus);
    connect(sv,&ScoreViewXML::StaffIndexChanged,this,&CScoreDoc::SaveLocation);
    connect(sv,&ScoreViewXML::BarChanged,this,&CScoreDoc::SaveLocation);

    ScoreMenu = mainMenu->addMenu("Score");
    ScoreMenu->addActions(sv->ScoreMenu->actions());
    actionSwipeBack = ScoreMenu->addAction("Previous Page",QKeySequence::MoveToPreviousPage,this,&CScoreDoc::SwipeBack);
    actionSwipeForward = ScoreMenu->addAction("Next Page",QKeySequence::MoveToNextPage,this,&CScoreDoc::SwipeForward);
    actionSwipeFirst = ScoreMenu->addAction("First Page",QKeySequence::MoveToStartOfDocument,this,&CScoreDoc::SwipeFirst);
    actionSwipeLast = ScoreMenu->addAction("Last Page",QKeySequence::MoveToEndOfDocument,this,&CScoreDoc::SwipeFinish);
    ScoreMenu->addSeparator();
    actionAddStaff = ScoreMenu->addAction("Insert Staff",this,&CScoreDoc::addStaff);
    actionDeleteStaff = ScoreMenu->addAction("Delete Staff",this,&CScoreDoc::deleteStaff);
    actionMoveStaffUp = ScoreMenu->addAction("Move Staff Up",this,&CScoreDoc::moveStaffUp);
    actionMoveStaffDown = ScoreMenu->addAction("Move Staff Down",this,&CScoreDoc::moveStaffDown);
    actionAddVoice = ScoreMenu->addAction("Add Voice",this,&CScoreDoc::addVoice);
    actionDeleteVoice = ScoreMenu->addAction("Delete Voice",this,&CScoreDoc::deleteVoice);
    ScoreMenu->addSeparator();
    actionSettings = ScoreMenu->addAction(QIcon(":/mini/mini/process.png"),"Settings",this,&CScoreDoc::ShowStaffsDialog);
    actionSettings->setMenuRole(QAction::NoRole);

    LayoutsMenu = mainMenu->addMenu("Layouts");
    LayoutMenu = LayoutsMenu->addMenu("Layout");
    actionAutoadjustAll = LayoutMenu->addAction("Autoadjust",this,&CScoreDoc::AutoAdjust);
    actionReformat = LayoutMenu->addAction("Reformat",this,&CScoreDoc::ReformatLayoutFromStart);
    actionReformatFromHere = LayoutMenu->addAction("Reformat from here to End",this,&CScoreDoc::ReformatFromHere);
    actionStretchFromHere = LayoutMenu->addAction("Stretch from here to End",this,&CScoreDoc::StretchFromHere);
    actionCompressFromHere = LayoutMenu->addAction("Compress from here to End",this,&CScoreDoc::CompressFromHere);
    LayoutMenu->addSeparator();
    actionFitSystems = LayoutMenu->addAction("Fit Systems",this,&CScoreDoc::FitAll);
    actionFitSystemsFromHere = LayoutMenu->addAction("Fit Systems from here to End",this,&CScoreDoc::FitFromHere);
    LayoutMenu->addSeparator();
    actionLayoutSettings = LayoutMenu->addAction(QIcon(":/mini/mini/process.png"),"Settings",this,&CScoreDoc::EditLayout);
    actionLayoutSettings->setMenuRole(QAction::NoRole);

    PageMenu = LayoutsMenu->addMenu("Page");
    actionSystemFromNextPage = PageMenu->addAction("Get System from next Page",this,&CScoreDoc::SystemFromNext);
    actionSystemToNextPage = PageMenu->addAction("Move System to next Page",this,&CScoreDoc::SystemToNext);
    actionFitSystemsOnPage = PageMenu->addAction("Fit Systems on Page",this,&CScoreDoc::FitMusic);

    SystemMenu = LayoutsMenu->addMenu("System");
    actionBarFromNextSystem = SystemMenu->addAction("Get Bar from next System",this,&CScoreDoc::BarFromNext);
    actionBarToNextSystem = SystemMenu->addAction("Move Bar to next System",this,&CScoreDoc::BarToNext);
    actionAddBarReformatFromHere = SystemMenu->addAction("Get Bar from next System and Reformat from here",this,&CScoreDoc::BarFromNextReformat);
    actionRemoveBarReformatFromHere = SystemMenu->addAction("Move Bar to next System and Reformat from here",this,&CScoreDoc::BarToNextReformat);
    actionToggleNames = SystemMenu->addAction("Toggle Instrument Names",this,&CScoreDoc::ToggleNames);
    actionEditStaffs = SystemMenu->addAction("Edit System",this,&CScoreDoc::EditSystem);
    actionSystemResetPositions = SystemMenu->addAction("Reset Position",this,&CScoreDoc::ResetSystem);

    actionLayoutLayout = new QAction("Layout",this);
    actionLayoutLayout->setIcon(QIcon(":/mini/mini/notebook.png"));
    connect(actionLayoutLayout,&QAction::triggered,this,&CScoreDoc::ShowLayoutLayoutPopup);

    actionLayoutPage = new QAction("Page",this);
    actionLayoutPage->setIcon(QIcon(":/mini/mini/new_page.png"));
    connect(actionLayoutPage,&QAction::triggered,this,&CScoreDoc::ShowLayoutPagePopup);

    actionLayoutSystem = new QAction("System",this);
    actionLayoutSystem->setIcon(QIcon(":/mini/mini/system.png"));
    connect(actionLayoutSystem,&QAction::triggered,this,&CScoreDoc::ShowLayoutSystemPopup);

    connect(sv,&ScoreViewXML::NavigationForwardClicked,this,&CScoreDoc::SwipeForward);
    connect(sv,&ScoreViewXML::NavigationBackClicked,this,&CScoreDoc::SwipeBack);
    connect(sv,&ScoreViewXML::NavigationEndClicked,this,&CScoreDoc::SwipeFinish);
    connect(sv,&ScoreViewXML::NavigationHomeClicked,this,&CScoreDoc::SwipeFirst);
    connect(sv,&ScoreViewXML::SwipeRightToLeft,this,&CScoreDoc::SwipeForward);
    connect(sv,&ScoreViewXML::SwipeLeftToRight,this,&CScoreDoc::SwipeBack);
    connect(sv,&ScoreViewXML::SelectionChanged,this,&CScoreDoc::ScoreSelectionChanged);

    connect(lv,&LayoutViewXML::Changed,this,&CScoreDoc::UpdateStatus);
    connect(lv,&LayoutViewXML::DoubleClick,this,&CScoreDoc::TweakSystem);
    connect(sv,&ScoreViewXML::BackMeUp,MainMenu->UndoMenu,&CUndoMenu::addItem,Qt::DirectConnection);
    connect(lv,&LayoutViewXML::BackMeUp,MainMenu->UndoMenu,&CUndoMenu::addItem,Qt::DirectConnection);

    connect(sv,&ScoreViewXML::StaffIndexChanged,this,&CScoreDoc::BarChanged);
    connect(sv,&ScoreViewXML::BarChanged,this,&CScoreDoc::BarChanged);

    connect(sv,&ScoreViewXML::PropertiesPopup,this,&CScoreDoc::PopupProperties);
    connect(sv,&ScoreViewXML::BarsPopup,this,&CScoreDoc::PopupBarMap);
    connect(sv,&ScoreViewXML::ListPopup,this,&CScoreDoc::PopupVoiceList);

    connect(pno,&OCPiano::NoteOnTriggered,sv,qOverload<int>(&ScoreViewXML::sound));

    connect(sv,&ScoreViewXML::NoteOnOff,this,&CScoreDoc::NoteOnOff);

    connect(sv,&ScoreViewXML::Popup,this,&CScoreDoc::ShowContextPopup);

    connect(lv,&LayoutViewXML::Popup,this,&CScoreDoc::ShowLayoutLayoutPopup);
    connect(lv,&LayoutViewXML::PopupPage,this,&CScoreDoc::ShowLayoutPagePopup);
    connect(lv,&LayoutViewXML::PopupSystem,this,&CScoreDoc::ShowLayoutSystemPopup);

    connect(lv,&LayoutViewXML::SelectionChanged,this,&CScoreDoc::UpdateStatus);

    connect(sv,&ScoreViewXML::ZoomChanged,this,&CScoreDoc::ZoomChanged);
    connect(lv,&LayoutViewXML::ZoomChanged,this,&CScoreDoc::ZoomChanged);

    sv->setLocked(false);
    sv->setSize(12);
    sv->setNavigationVisible(true);

    ui->SidebarFrame->setAutoFillBackground(true);
    spSidebar=new QMacSplitter(this);
    spSidebar->setObjectName("layoutSidebarSplitter");
    ui->LayoutFrame->layout()->addWidget(spSidebar);
    ui->FadingWidget->setWidget(lv);
    ui->FadingWidget->setTransitionType(QFadingWidget::Fade);
    spSidebar->addWidget(ui->FadingWidget);
    spSidebar->addWidget(ui->SidebarFrame);
    spSidebar->setStretchFactor(1,0);
    spSidebar->setStretchFactor(0,10);

    RightSideButton=new QAction(QIcon(":/mini/mini/right.png").pixmap(20,20),"Sidebar",this);
    RightSideButton->setToolTip("Hide Sidebar");
    connect(RightSideButton,&QAction::triggered,this,&CScoreDoc::ToggleLayoutSidebar);

    lv->XMLScore.shadowXML(sv->XMLScore);

    tbAddLayout=new QToolButton(lv->viewport());
    tbAddLayout->setGeometry(40,40,248,264);
    tbAddLayout->setIconSize(QSize(240,240));
    tbAddLayout->setToolTip("Add Layout");
    tbAddLayout->setText("Add Layout");
    tbAddLayout->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    tbAddLayout->setVisible(true);
    tbAddLayout->setIcon(QIcon(":/addlayout.png"));
    tbAddLayout->setProperty("transparent",true);
    connect(tbAddLayout,&QAbstractButton::clicked,this,&CScoreDoc::AddLayout);

    ui->layoutList->setMinimumWidth(130);
    ui->layoutList->setItemDelegate(new layoutsDelegate);
    connect(ui->layoutList,&QTableWidget::itemClicked,this,qOverload<QTableWidgetItem*>(&CScoreDoc::SetCurrentLayout));
    connect(ui->layoutList,&QTableWidget::itemChanged,this,&CScoreDoc::RenameItem);
    connect(ui->AddLayout,&QAbstractButton::clicked,this,&CScoreDoc::AddLayout);
    connect(ui->RemoveLayout,&QAbstractButton::clicked,this,&CScoreDoc::DeleteLayout);

    UpdateLayoutList();

    sv->setFocus();

    ui->layoutList->setAttribute(Qt::WA_MacShowFocusRect, 0);
    OCNoteToolbox* tb=ui->NoteToolbox;

    connect(pno,&OCPiano::TriggerNotes,tb,&OCNoteToolbox::TriggerNotes);
    connect(tb,&OCNoteToolbox::OverwriteProperty,this,&CScoreDoc::OverwriteProperty);
    connect(sv,&ScoreViewXML::RequestSymbol,this,qOverload<XMLSimpleSymbolWrapper&,QString>(&CScoreDoc::PasteSymbol));
    connect(tb,&OCNoteToolbox::NoteChanged,sv,&ScoreViewXML::setNoteConfig);
    connect(sv,&ScoreViewXML::RequestNote,tb,&OCNoteToolbox::GetCurrentNote);
    connect(sv,&ScoreViewXML::RequestDuratedSymbol,this,&CScoreDoc::PasteDuratedSymbol);
    connect(tb,&OCNoteToolbox::PasteXML,this,qOverload<XMLSimpleSymbolWrapper&,QString,bool>(&CScoreDoc::PasteSymbol));
    connect(tb,&OCNoteToolbox::ToggleWriteMode,sv,&ScoreViewXML::toggleAltKey);

    sv->setNoteConfig();

    QMacTreeWidget* tw=ui->Toolbox;
    tw->setHeaderHidden(true);
    tw->setUniformRowHeights(false);
    tw->setIconMode(false);
    tw->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    tw->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    for (const QString& Category : OCSymbolsCollection::Categories())
    {
        if (!Category.isEmpty())
        {
            CPropertiesToolGrid* tg=new CPropertiesToolGrid(this);
            for(const QString& s : OCSymbolsCollection::Classes()) tg->AddButton(s,Category);
            if (Category=="Durated")
            {
                connect(tg,&CPropertiesToolGrid::PasteXML,this,&CScoreDoc::PasteDuratedSymbol);
                DuratedGridWidgets.append(tg);
            }
            else
            {
                connect(tg,&CPropertiesToolGrid::PasteXML,this,qOverload<XMLSimpleSymbolWrapper&,QString>(&CScoreDoc::PasteSymbol));
            }
            tg->AddToTree(Category,tw);
        }
    }

    //NameFilter = "XML files (*.xml *.zip *.mus)";

    CZoomWidget* ZoomWidget = new CZoomWidget(this);
    connect(ZoomWidget,&CZoomWidget::valueChanged,this,&CScoreDoc::SetZoom);
    connect(this,&CScoreDoc::ZoomChanged,ZoomWidget,&CZoomWidget::setValue);

    CToolBar* leftToolBar=new CToolBar(this);
    leftToolBar->addWidget(ZoomWidget);

    CToolBar* playToolBar = new CToolBar(this);
    playToolBar->addAction(playControl->getPlayButton());
    playToolBar->addAction(playControl->getMixerButton());

    CToolBar* rightToolBar=new CToolBar(this);
    rightToolBar->addAction(actionLayoutSystem);
    rightToolBar->addAction(actionLayoutPage);
    rightToolBar->addAction(actionLayoutLayout);
    rightToolBar->addAction(actionProperties);
    rightToolBar->addAction(actionVoiceList);
    rightToolBar->addAction(actionBarMap);
    rightToolBar->addSeparator();
    rightToolBar->addActions(MainMenu->UndoMenu->actions());
    rightToolBar->addAction(MainMenu->EditMenu->actionCut);
    rightToolBar->addAction(MainMenu->EditMenu->actionCopy);
    rightToolBar->addAction(MainMenu->EditMenu->actionPaste);
    rightToolBar->addAction(actionResetPositions);
    rightToolBar->addSeparator();
    rightToolBar->addAction(actionLayoutSettings);
    rightToolBar->addAction(actionSettings);
    rightToolBar->addAction(RightSideButton);

    CStatusBar* statusBar = new CStatusBar(this);
    statusBar->addSpacing(10);
    statusBar->addWidget(leftToolBar,1,Qt::AlignLeft);
    statusBar->addWidget(playToolBar,0,Qt::AlignHCenter);
    statusBar->addWidget(rightToolBar,1,Qt::AlignRight);
    layout()->addWidget(statusBar);

    SaveLocation();
    showScore();
}

void CScoreDoc::showScore()
{
    if (m_View==0) {
        MainMenu->actionProjects->trigger();//showProjects();
    }
    else {
        SetView(0);
    }
    UpdateStatus();
}

void CScoreDoc::addStaff()
{
    QStringList Names;
    for (int i=0;i<sv->XMLScore.NumOfStaffs();i++)
    {
        Names.append(sv->XMLScore.StaffName(i));
    }
    QString NewName="New Staff";
    int StaffCount=1;
    while (Names.contains(NewName))
    {
        NewName="New Staff - "+QString::number(StaffCount);
        StaffCount++;
    }
    prepareFade();
    MainMenu->UndoMenu->addItem("Insert Staff");

    sv->XMLScore.AddStaff(sv->ActiveStaffId(),NewName);
    sv->ReloadXML();
    staffConfigChanged();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::addVoice()
{
    prepareFade();
    MainMenu->UndoMenu->addItem("Add Voice");

    sv->XMLScore.AddVoice(sv->ActiveStaffId());
    sv->ReloadXML();
    sv->setActiveVoice(sv->VoiceCount()-1);
    staffConfigChanged();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::moveStaffUp()
{
    prepareFade();
    MainMenu->UndoMenu->addItem("Move Staff Up");

    const int ToIndex=sv->ActiveStaffId()-1;
    sv->XMLScore.MoveStaff(sv->ActiveStaffId(),ToIndex);
    sv->setActiveStaffId(ToIndex);
    sv->ReloadXML();
    staffConfigChanged();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::moveStaffDown()
{
    prepareFade();
    MainMenu->UndoMenu->addItem("Move Staff Down");

    const int ToIndex=sv->ActiveStaffId()+1;
    sv->XMLScore.MoveStaff(sv->ActiveStaffId(),ToIndex);
    sv->setActiveStaffId(ToIndex);
    sv->ReloadXML();
    staffConfigChanged();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::deleteStaff()
{
    prepareFade();
    MainMenu->UndoMenu->addItem("Delete Staff");
    sv->XMLScore.DeleteStaff(sv->ActiveStaffId());
    sv->ReloadXML();
    staffConfigChanged();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::deleteVoice()
{
    prepareFade();
    MainMenu->UndoMenu->addItem("Delete Voice");
    sv->XMLScore.DeleteVoice(sv->ActiveVoiceLocation());
    sv->ReloadXML();
    staffConfigChanged();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::ToggleLayoutSidebar()
{
    QMacSplitter* splitter=spSidebar;
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

void CScoreDoc::ShowContextPopup(QPoint Pos)
{
    QTransMenu* m=new QTransMenu(this);
    //m->addActions(MainMenu->UndoMenu->actions());
    //m->addSeparator();
    m->addActions(MainMenu->EditMenu->actions());
    /*
    m->addSeparator();
    m->addAction(actionProperties);
    m->addAction(actionVoiceList);
    m->addAction(actionBarMap);
    m->addSeparator();
    m->addAction(sv->actionSwapForward);
    m->addAction(sv->actionSwapBack);
    m->addSeparator();
    m->addAction(sv->actionSelectAll);
    m->addAction(sv->actionSelectHome);
    m->addAction(sv->actionSelectEnd);
*/
    m->addSeparator();
    m->addAction(actionAddStaff);
    actionDeleteStaff->setEnabled(sv->StaffCount() > 1);
    m->addAction(actionDeleteStaff);
    actionMoveStaffUp->setEnabled(sv->ActiveStaffId() > 0);
    m->addAction(actionMoveStaffUp);
    actionMoveStaffDown->setEnabled(sv->ActiveStaffId() < sv->StaffCount()-1);
    m->addAction(actionMoveStaffDown);
    m->addAction(actionAddVoice);
    actionDeleteVoice->setEnabled(sv->VoiceCount() > 1);
    m->addAction(actionDeleteVoice);
    QTransMenu* vm = new QTransMenu(menuGotoVoice(),m,false);
    vm->setTitle("Goto Voice");
    m->addMenu(vm);
    vm->setEnabled(sv->VoiceCount() > 1);
    m->addSeparator();
 /*
    m->addAction(actionResetPositions);
    QTransMenu* r = new QTransMenu(m,false);
    r->setTitle("Rhythm");
    m->addMenu(r);
    r->addAction(actionTriolize);
    r->addAction(actionDottify);
    r->addAction(actionDoubleDottify);
    r->addAction(actionStraighten);
    */
    //m->addSeparator();
    m->addAction(actionSettings);
    m->popup(Pos);
}

void CScoreDoc::ShowLayoutPopup(QPoint Pos)
{
    QTransMenu* m=new QTransMenu(this);
    m->addActions(MainMenu->UndoMenu->actions());
    m->addSeparator();
    m->addActions(LayoutMenu->actions());
    m->addSeparator();
    m->addActions(PageMenu->actions());
    m->addSeparator();
    m->addActions(SystemMenu->actions());
    m->popup(Pos);
}

void CScoreDoc::ShowLayoutLayoutPopup()
{
    QTransMenu* m=new QTransMenu(LayoutMenu,this);
    m->popup(cursor().pos());
}

void CScoreDoc::ShowLayoutPagePopup()
{
    QTransMenu* m=new QTransMenu(PageMenu,this);
    m->popup(cursor().pos());
}

void CScoreDoc::ShowLayoutSystemPopup()
{
    QTransMenu* m=new QTransMenu(SystemMenu,this);
    m->popup(cursor().pos());
}

void CScoreDoc::toggleView()
{
    qDebug() << sv->FollowResize();
    if (sv->FollowResize()==ScoreViewXML::PageSizeUnlimited)
    {
        sv->setFollowResize(ScoreViewXML::PageSizeFollowsResize);
        sv->Paint(tsReformat);
        return;
    }
    sv->setFollowResize(ScoreViewXML::PageSizeUnlimited);
    sv->Paint(tsReformat);
}

void CScoreDoc::SetStatusLabel(QString text)
{
    text="<font color=#ccc> - "+text+"</font>";
    if (GetView()==0)
    {
        text="<font color=#eee>Score</font> "+text;
    }
    if (GetView()==1)
    {
        text="<font color=#eee>Layouts</font> "+text;
    }
    ui->NavLabel->setText(text);
}

void CScoreDoc::RenameItem(QTableWidgetItem* item)
{
    if (ui->layoutList->currentRow()>-1)
    {
        if (item->text() != sv->XMLScore.LayoutName(item->row()))
        {
            MainMenu->UndoMenu->addItem("Rename Layout");
            sv->XMLScore.setLayoutName(item->row(),item->text());
            UpdateStatus();
        }
    }
}

void CScoreDoc::UpdateLayoutList()
{
    ui->layoutList->blockSignals(true);
    ui->layoutList->clear();
    ui->layoutList->setRowCount(lv->layoutCount());
    ui->layoutList->setColumnWidth(0,ui->layoutList->width());
    for (int i=0; i<lv->layoutCount(); i++)
    {
        QTableWidgetItem *newItem = new QTableWidgetItem;
        newItem->setText(lv->LayoutName(i));
        newItem->setIcon(lv->firstPageIcon(i));
        newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
        newItem->setTextAlignment(Qt::AlignBottom | Qt::AlignLeft);
        ui->layoutList->setRowHeight(i,150);
        ui->layoutList->setItem(i,0,newItem);
    }
    ui->layoutList->setCurrentCell(lv->activeLayoutIndex(),0);
    ui->layoutList->blockSignals(false);
}

void CScoreDoc::InitLayout(const int Index)
{
    lv->InitLayout(Index);
    SetCurrentLayout(Index);
    UpdateLayoutList();
    UpdateStatus();
}

bool CScoreDoc::AddLayout()
{
    bool RetVal=false;
    CLayoutWizard d(this);
    d.Fill(sv->XMLScore,-1);
    d.setWindowTitle("New Layout");
    if ((lv->layoutCount()==0) || (d.exec()==QDialog::Accepted))
    {
        MainMenu->UndoMenu->addItem("Add Layout");
        d.ModifyXML(sv->XMLScore,-1);
        lv->ReloadXML();
        InitLayout(lv->layoutCount()-1);
        UpdateStatus();
        RetVal=true;
    }
    return RetVal;
}

void CScoreDoc::EditLayout()
{
    if (ui->layoutList->rowCount()==0) return;
    int Index=ui->layoutList->currentRow();
    CLayoutWizard d(this);
    d.Fill(sv->XMLScore,Index);
    d.setWindowTitle("Edit Layout");
    if (d.exec()==QDialog::Accepted)
    {
        MainMenu->UndoMenu->addItem("Edit Layout");
        d.ModifyXML(sv->XMLScore,Index);
        InitLayout(Index);
    }
}

void CScoreDoc::DeleteLayout()
{
    if (ui->layoutList->rowCount()==0) return;
    MainMenu->UndoMenu->addItem("Delete Layout");
    int Index=ui->layoutList->currentRow();
    sv->XMLScore.LayoutCollection.deleteChild(Index);
    lv->ReloadXML();
    UpdateLayoutList();
    if (ui->layoutList->rowCount())
    {
        Index--;
        if (Index<0) Index=0;
        SetCurrentLayout(Index);
    }
    else
    {
        SetView(0);
    }
    UpdateStatus();
}

void CScoreDoc::PageSetup()
{
    if (lv->layoutCount()==0) return;
    ui->FadingWidget->prepare();
    lv->PageSetup();
    lv->Init();
    ui->FadingWidget->fade();
}

void CScoreDoc::Print()
{
    if (lv->layoutCount()==0) return;
    lv->PrinterPrint();
    UpdateLayoutView();
}

void CScoreDoc::ExportPDF(const QString& pdfPath)
{
    if (lv->layoutCount()==0) return;
    lv->PrinterPrint(pdfPath);
    UpdateLayoutView();
}

void CScoreDoc::ExportMXL(const QString& mxlPath)
{
    if (lv->layoutCount()==0) return;
    CMusicXMLWriter::saveMXLDoc(mxlPath,lv->XMLScore,lv->XMLScore.LayoutCollection.XMLLayout(lv->activeLayoutIndex()));
}

void CScoreDoc::ExportMusicXML(const QString& mxlPath)
{
    if (lv->layoutCount()==0) return;
    CMusicXMLWriter::saveMusicXMLDoc(mxlPath,lv->XMLScore,lv->XMLScore.LayoutCollection.XMLLayout(lv->activeLayoutIndex()));
}

void CScoreDoc::PrintPreview()
{
    if (lv->layoutCount()==0) return;
    lv->PrintPreview();
    UpdateLayoutView();
}

void CScoreDoc::NextLayout()
{
    if (lv->layoutCount()==0) return;
    int l=ui->layoutList->currentRow()+1;
    if (l>=lv->layoutCount()) l=0;
    SetCurrentLayout(l);
}

void CScoreDoc::PrevLayout()
{
    if (lv->layoutCount()==0) return;
    int l=ui->layoutList->currentRow()-1;
    if (l<0) l=lv->layoutCount()-1;
    SetCurrentLayout(l);
}

void CScoreDoc::SetCurrentLayout(QTableWidgetItem* item)
{
    if (item->row() != lv->activeLayoutIndex()) SetCurrentLayout(item->row());
}

void CScoreDoc::SetCurrentLayout(const int Index)
{
    if (lv->layoutCount()==0)
    {
        tbAddLayout->setVisible(true);
        return;
    }
    ui->FadingWidget->prepare();
    tbAddLayout->setVisible(false);
    ui->layoutList->blockSignals(true);
    lv->setActiveLayout(Index);
    lv->Init();
    lv->SelectSystem(LayoutLocation(0,0));
    ui->layoutList->setCurrentCell(lv->activeLayoutIndex(),0);
    ui->labelLayouts->setText("<font color=#aaa>"+QString::number(lv->activeLayoutIndex()+1)+" of "+QString::number(lv->layoutCount())+" ");
    ui->nextLayout->setEnabled(lv->activeLayoutIndex()<lv->layoutCount()-1);
    ui->prevLayout->setEnabled(lv->activeLayoutIndex()>0);
    ui->layoutList->blockSignals(false);
    ui->FadingWidget->fade();
}

void CScoreDoc::UpdateLayoutView()
{
    UpdateLayoutList();
    if (lv->layoutCount()==0) return;
    tbAddLayout->setVisible(false);
    ui->layoutList->blockSignals(true);
    lv->setActiveLayout(qBound<int>(0,lv->activeLayoutIndex(),lv->layoutCount()-1));
    const LayoutLocation l(lv->activeLayoutLocation());
    lv->Init();
    ui->layoutList->setCurrentCell(lv->activeLayoutIndex(),0);
    ui->labelLayouts->setText("<font color=#aaa>"+QString::number(lv->activeLayoutIndex()+1)+" of "+QString::number(lv->layoutCount())+" ");
    ui->nextLayout->setEnabled(lv->activeLayoutIndex()<lv->layoutCount()-1);
    ui->prevLayout->setEnabled(lv->activeLayoutIndex()>0);
    lv->SelectSystem(l);
    ui->layoutList->blockSignals(false);
}

void CScoreDoc::UpdateScoreView()
{
    UpdateSV();
    UpdatePW();
    UpdateBW();
    UpdateTree();
}

CScoreDoc::~CScoreDoc()
{
    MIDI2wav->clear();
    delete ui;
}

void CScoreDoc::closeEvent(QCloseEvent * event)
{
    bool Cancel=false;
    emit Close(this,Cancel);
    (Cancel) ? event->ignore() : event->accept();
}

void CScoreDoc::setActiveStaffId(const int id)
{
    if ((id != sv->ActiveStaffId()) && (id>-1) && (id<sv->StaffCount()))
    {
        sv->setActiveStaffId(id);
        sv->Paint(tsVoiceIndexChanged,true);
        sv->ensureVisible();
        SaveLocation();
        UpdateBW();
    }
}

void CScoreDoc::setActiveVoice(const int Voice)
{
    if ((Voice != sv->ActiveVoice()) && (Voice>-1) && (Voice<sv->VoiceCount()))
    {
        sv->setActiveVoice(Voice);
        sv->Paint(tsRedrawActiveStave,true);
        SaveLocation();
        UpdateBW();
    }
}

void CScoreDoc::setBar(int Bar, int Staff, int Voice)
{
    setActiveStaffId(Staff);
    setActiveVoice(Voice);
    if (sv->FollowResize()==ScoreViewXML::PageSizeUnlimited)
    {
        sv->scrollToBar(Bar);
    }
    else
    {
        sv->setStartBar(Bar);
        sv->Paint(tsNavigate,true);
        sv->ensureVisible();
    }
    UpdateStatus();
}

void CScoreDoc::SetXML(XMLScoreWrapper& Doc)
{
    sv->SetXML(Doc);
    UpdateXML();
}

void CScoreDoc::SetXML(QDomLiteDocument* Doc)
{
    sv->SetXML(Doc);
    UpdateXML();
}

void CScoreDoc::LoadOptions() {
    XMLScoreOptionsWrapper w = sv->XMLScore.ScoreOptions;
    lv->setZoom(w.layoutZoom());
    lv->setActiveLayout(w.layoutIndex());
    sv->setZoom(w.scoreZoom());
    sv->setStartBar(w.startBar());
    SetView(w.view(),true);
}

void CScoreDoc::SaveOptions() const {
    XMLScoreOptionsWrapper w = sv->XMLScore.ScoreOptions;
    w.setView(GetView());
    w.setLayoutZoom(lv->getZoom());
    w.setLayoutIndex(lv->activeLayoutIndex());
    w.setScoreZoom(sv->getZoom());
    w.setStartBar(sv->StartBar());
}

void CScoreDoc::ActivateDoc() {
    isClosed = false;
    UpdateScoreView();
    UpdateLayoutView();
    UpdateStatus();
}

ImportResult CScoreDoc::Load(QString Path)
{
    lastImport = sv->Load(Path);
    UpdateXML();
    MIDI2wav->updateMIDIConfig();
    return lastImport;
}

bool CScoreDoc::Save(QString Path)
{
    return sv->Save(Path);
}

void CScoreDoc::UpdateXML()
{
    int Index=loBound<int>(0,ui->layoutList->currentRow());
    UpdateScoreView();
    lv->SetXML(sv->XMLScore);
    UpdateLayoutList();
    Index=hiBound<int>(Index,ui->layoutList->rowCount()-1);
    if (Index > -1) SetCurrentLayout(Index);
    UpdateStatus();
}

void CScoreDoc::ShowStaffsDialog()
{
    CStaffsDialog d(this);
    d.Fill(sv->XMLScore);
    d.setWindowTitle("Edit Score");
    if (d.exec()==QDialog::Accepted)
    {
        prepareFade();
        MainMenu->UndoMenu->addItem("Edit Staffs");
        XMLScoreWrapper s(d.CreateXML());
        if (sv->ActiveStaffId()>s.NumOfStaffs()-1) sv->setActiveStaffId(s.NumOfStaffs()-1);
        sv->setActiveVoice(0);
        Locations.clear();
        LocationIndex=-1;
        UpdateLocationButtons();
        SetXML(s);
        SVChanged();
        MIDI2wav->updateMIDIConfig();
        ui->FadingWidget_2->fade();
    }
}

void CScoreDoc::ZoomIn()
{
    SetZoom(2);
}

void CScoreDoc::ZoomOut()
{
    SetZoom(0.5);
}

void CScoreDoc::SetZoom(const double Zoom)
{
    (GetView()==0) ? sv->setZoom(Zoom) : lv->setZoom(Zoom);
}

double CScoreDoc::GetZoom() const
{
    return (GetView()==0) ? sv->getZoom() : lv->getZoom();
}

QMenu *CScoreDoc::menuGotoVoice() {
    QMenu* s = new QMenu("Go to Voice...", this);
    s->setEnabled(sv->VoiceCount() > 1);
    for (int i = 0 ; i < sv->VoiceCount() ; i++)
    {
        QAction* a = s->addAction("Voice "+QString::number(i+1));
        connect(a, &QAction::triggered, [=] { setActiveVoice(i); });
        a->setVisible(true);
        a->setEnabled(i != sv->ActiveVoice());
    }
    return s;
}

void CScoreDoc::SwipeForward()
{
    ui->FadingWidget_2->setTransitionType(QFadingWidget::PageForward);
    ui->FadingWidget_2->setEasingCurve(QEasingCurve::InQuart);
    ui->FadingWidget_2->prepare();
    sv->turnpage();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::SwipeBack()
{
    ui->FadingWidget_2->setTransitionType(QFadingWidget::PageBack);
    ui->FadingWidget_2->setEasingCurve(QEasingCurve::InQuart);
    ui->FadingWidget_2->prepare();
    sv->turnback();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::SwipeFirst()
{
    ui->FadingWidget_2->setTransitionType(QFadingWidget::PageBack);
    ui->FadingWidget_2->setEasingCurve(QEasingCurve::InQuart);
    ui->FadingWidget_2->prepare();
    sv->fastback();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::SwipeFinish()
{
    ui->FadingWidget_2->setTransitionType(QFadingWidget::PageForward);
    ui->FadingWidget_2->setEasingCurve(QEasingCurve::InQuart);
    ui->FadingWidget_2->prepare();
    sv->fastforward();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::Forward()
{
    prepareFade();
    sv->turnpage();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::Back()
{
    prepareFade();
    sv->turnback();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::First()
{
    prepareFade();
    sv->fastback();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::Finish()
{
    prepareFade();
    sv->fastforward();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::prepareFade()
{
    ui->FadingWidget_2->setTransitionType(QFadingWidget::Fade);
    ui->FadingWidget_2->setEasingCurve(QEasingCurve::OutCurve);
    ui->FadingWidget_2->prepare();
}

void CScoreDoc::UpdateStatus()
{
    UpdateAppTitle();
    ScoreMenu->menuAction()->setVisible(m_View == 0);
    LayoutsMenu->menuAction()->setVisible(m_View == 1);
    emit ZoomChanged((m_View==0) ? sv->getZoom() : lv->getZoom());
    RightSideButton->setVisible(m_View==1);
    if (m_View==0)
    {
        /*
        actionPaste->setEnabled(QApplication::clipboard()->text().contains("<!DOCTYPE OCCopyData"));
        if (QApplication::clipboard()->text().contains("<!DOCTYPE OCCopyDataVoices>"))
        {
            actionPaste->setText(tr("Paste Voices"));
        }
        else if (QApplication::clipboard()->text().contains("<!DOCTYPE OCCopyDataSymbols>"))
        {
            actionPaste->setText(tr("Paste Symbols"));
        }
*/
        //actionCut->setEnabled(canCopy());
        //actionCopy->setEnabled(canCopy());
        MainMenu->EditMenu->setSelectionStatus(canCopy());
        if ((sv->Cursor.currentPointer() == sv->VoiceLen()) && (sv->VoiceLen() > 0)) {
            MainMenu->EditMenu->actionDelete->setEnabled(true);
        }
        for (QAction* a : (const QList<QAction*>)sv->actions()) {
            if (a->shortcut() == MainMenu->EditMenu->actionDelete->shortcut()) a->setEnabled(false);
        }
        /*
        if (SelectionStaff==-1)
        {
            actionCut->setText(tr("Cut Symbols"));
            actionCopy->setText(tr("Copy Symbols"));
        }
        else
        {
            actionCut->setText(tr("Cut Voices"));
            actionCopy->setText(tr("Copy Voices"));
        }
*/
        actionTriolize->setEnabled(sv->SelectionList().size()>1);
        actionDottify->setEnabled(sv->SelectionList().size()>1);
        actionDoubleDottify->setEnabled(sv->SelectionList().size()>1);
        actionStraighten->setEnabled(sv->SelectionList().size()>1);

        sv->actionSwapForward->setEnabled(sv->Cursor.currentPointer()+1 < sv->VoiceLen());
        sv->actionSwapBack->setEnabled(sv->Cursor.currentPointer() > 0);
        actionResetPositions->setEnabled((sv->Cursor.currentPointer() < sv->VoiceLen()) | (SelectionStaff>-1));
        sv->actionSelectAll->setEnabled(sv->VoiceLen()>1);
        sv->actionSelectEnd->setEnabled(sv->Cursor.currentPointer()+1 < sv->VoiceLen());
        sv->actionSelectHome->setEnabled((sv->Cursor.currentPointer() < sv->VoiceLen()) && (sv->Cursor.currentPointer()>0));
        actionSwipeBack->setEnabled(sv->StartBar() != 0);
        actionSwipeFirst->setEnabled(sv->StartBar() != 0);
        actionSwipeForward->setEnabled(sv->CanTurnPage());
        actionSwipeLast->setEnabled(sv->CanTurnPage());
        sv->actionPreviousStaff->setEnabled(sv->StaffCount()>1);
        sv->actionNextStaff->setEnabled(sv->StaffCount()>1);
        sv->actionPreviousVoice->setEnabled(sv->VoiceCount()>1);
        sv->actionNextVoice->setEnabled(sv->VoiceCount()>1);

        SetDurated();
    }
    else
    {
        if (spSidebar->isCollapsed())
        {
            RightSideButton->setIcon(QIcon(":/mini/mini/left.png").pixmap(20,20));
            RightSideButton->setToolTip("Show Sidebar");
        }
        else
        {
            RightSideButton->setIcon(QIcon(":/mini/mini/right.png").pixmap(20,20));
            RightSideButton->setToolTip("Hide Sidebar");
        }
        XMLLayoutWrapper XMLLayout=lv->XMLScore.Layout(lv->activeLayoutIndex());
        if (XMLLayout.pageCount()>0)
        {
            XMLLayoutPageWrapper XMLPage=XMLLayout.XMLPage(lv->activePageIndex());
            XMLLayoutSystemWrapper XMLSystem=XMLPage.XMLSystem(lv->activeSystemIndex());
            actionBarFromNextSystem->setEnabled(!XMLLayout.isLastSystem(lv->activeLayoutLocation()));
            actionBarToNextSystem->setEnabled((XMLSystem.endBar()-XMLSystem.startBar()>1));
            actionAddBarReformatFromHere->setEnabled(!XMLLayout.isLastSystem(lv->activeLayoutLocation()));
            actionRemoveBarReformatFromHere->setEnabled((XMLSystem.endBar()-XMLSystem.startBar()>1));
            actionSystemFromNextPage->setEnabled(!XMLLayout.isLastPage(lv->activePageIndex()));
            actionSystemToNextPage->setEnabled(XMLPage.systemCount()>1);
            actionEditStaffs->setEnabled(sv->StaffCount()>1);
        }
        else
        {
            actionBarFromNextSystem->setEnabled(false);
            actionBarToNextSystem->setEnabled(false);
            actionAddBarReformatFromHere->setEnabled(false);
            actionRemoveBarReformatFromHere->setEnabled(false);
            actionSystemFromNextPage->setEnabled(false);
            actionSystemToNextPage->setEnabled(false);
            actionEditStaffs->setEnabled(false);
        }
        if (lv->layoutCount()==0)
        {
            SetStatusLabel(QString());
            return;
        }
        QString msg=lv->LayoutName(lv->activeLayoutIndex());
        msg += " - Page " + QString::number(lv->activePageIndex() + 1);
        msg += " - System " + QString::number(lv->activeSystemIndex() + 1);
        SetStatusLabel(msg);
        return;
    }
    QString msg=sv->StaffName(sv->ActiveStaffId());
    msg += " - Voice " + QString::number(sv->ActiveVoice() + 1);
    msg += " - Bar " + QString::number(sv->StartBar() + 1);
    SetStatusLabel(msg);
}

void CScoreDoc::serialize(QDomLiteElement* xml) const {
    sv->XMLScore.setMixer(MIDI2wav->SaveXML());
    SaveOptions();
    sv->serialize(xml);
    sv->Cursor.serialize(xml);
}

void CScoreDoc::unserialize(const QDomLiteElement* xml) {
    sv->unserialize(xml);
    sv->Cursor.unserialize(xml);
    OCVoiceLocation l=sv->ActiveVoiceLocation();
    if (l.StaffId > sv->StaffCount()-1) sv->setActiveStaffId(sv->StaffCount()-1);
    if (l.Voice > sv->VoiceCount()-1) sv->setActiveVoice(sv->VoiceCount()-1);
    lv->SetXML(sv->XMLScore);
    MIDI2wav->LoadMIDI(sv->XMLScore.getMixer());
    LoadOptions();
    if (sv->XMLScore.layoutCount()==0) SetView(0);
    if (GetView()==0)
    {
        UpdateScoreView();
        UpdateStatus();
    }
    else
    {
        UpdateLayoutView();
        UpdateStatus();
    }
}

bool CScoreDoc::canCopy() {
    bool EnableCopy=((sv->Cursor.SelCount() > 0) && (sv->Cursor.currentPointer() < sv->VoiceLen()));
    return EnableCopy | (SelectionStaff>-1);
}

void CScoreDoc::BarToNext()
{
    ui->FadingWidget->prepare();
    MainMenu->UndoMenu->addItem("Move Bar");
    lv->MoveBar(1);
    UpdateStatus();
    ui->FadingWidget->fade();
}

void CScoreDoc::BarFromNext()
{
    ui->FadingWidget->prepare();
    MainMenu->UndoMenu->addItem("Move Bar");
    lv->MoveBar(-1);
    UpdateStatus();
    ui->FadingWidget->fade();
}

void CScoreDoc::BarToNextReformat()
{
    ui->FadingWidget->prepare();
    MainMenu->UndoMenu->addItem("Move Bar");
    const int b = lv->activeEndBar();
    lv->MoveBar(1);
    const LayoutLocation l(lv->locationOfBar(b));
    if (l.isValid()) lv->Reformat(l);
    UpdateStatus();
    ui->FadingWidget->fade();
}

void CScoreDoc::BarFromNextReformat()
{
    ui->FadingWidget->prepare();
    MainMenu->UndoMenu->addItem("Move Bar");
    lv->MoveBar(-1);
    const int b = lv->activeEndBar()+1;
    const LayoutLocation l(lv->locationOfBar(b));
    if (l.isValid()) lv->Reformat(l);
    UpdateStatus();
    ui->FadingWidget->fade();
}

void CScoreDoc::SystemToNext()
{
    ui->FadingWidget->prepare();
    MainMenu->UndoMenu->addItem("Move System");
    lv->MoveSystem(1);
    UpdateStatus();
    ui->FadingWidget->fade();
}

void CScoreDoc::SystemFromNext()
{
    ui->FadingWidget->prepare();
    MainMenu->UndoMenu->addItem("Move System");
    lv->MoveSystem(-1);
    UpdateStatus();
    ui->FadingWidget->fade();
}

void CScoreDoc::ToggleNames()
{
    ui->FadingWidget->prepare();
    MainMenu->UndoMenu->addItem("Toggle Names");
    lv->ToggleNames();
    UpdateStatus();
    ui->FadingWidget->fade();
}

void CScoreDoc::EditSystem()
{
    XMLTemplateWrapper LayoutTemplate = lv->XMLScore.Layout(lv->activeLayoutIndex()).Template;
    XMLTemplateWrapper SystemTemplate = lv->XMLScore.Layout(lv->activeLayoutIndex()).XMLSystem(lv->activeLayoutLocation()).Template;
    CEditSystem d(this);
    d.Fill(LayoutTemplate,SystemTemplate,lv->XMLScore);
    if (d.exec()==QDialog::Accepted)
    {
        MainMenu->UndoMenu->addItem("Edit System");
        d.GetTemplate(LayoutTemplate,SystemTemplate);
        ui->FadingWidget->prepare();
        sv->ReloadXML();
        lv->ReloadXML();
        lv->Init();
        UpdateStatus();
        ui->FadingWidget->fade();
    }
}

void CScoreDoc::FitMusic()
{
    ui->FadingWidget->prepare();
    MainMenu->UndoMenu->addItem("Fit Music");
    lv->AdjustCurrentPage();
    UpdateStatus();
    ui->FadingWidget->fade();
}

void CScoreDoc::FitPages(const int StartPage)
{
    ui->FadingWidget->prepare();
    MainMenu->UndoMenu->addItem("Fit Music");
    lv->AdjustPages(StartPage);
    UpdateStatus();
    ui->FadingWidget->fade();
}

void CScoreDoc::FitFromHere() {
    FitPages(lv->activePageIndex());
}

void CScoreDoc::FitAll()
{
    FitPages(0);
}

void CScoreDoc::ResetSystem()
{
    ui->FadingWidget->prepare();
    MainMenu->UndoMenu->addItem("Reset Position");
    lv->ResetSystem();
    UpdateStatus();
    ui->FadingWidget->fade();
}

int CScoreDoc::GetView() const
{
    return m_View;
}

void CScoreDoc::SetView(const int View,bool silent)
{
    if (m_View==View) return;
    m_View=View;
    if (!silent)
    {
        if (View==1)
        {
            ui->SlidingWidget->setTransitionType(QFadingWidget::UncoverLeft);
        }
        else
        {
            ui->SlidingWidget->setTransitionType(QFadingWidget::CoverRight);
        }
        ui->SlidingWidget->prepare();
    }
    ui->ScoreFrame->setVisible(View==0);
    ui->LayoutFrame->setVisible(View==1);
    ui->toggleView->setVisible(View==0);
    if (View == 0)
    {
        sv->Paint(tsNavigate,true);
    }
    else if (View == 1)
    {
        sv->ReloadXML();
        lv->SetXML(sv->XMLScore);
        UpdateLayoutView();
    }
    if (!silent) ui->SlidingWidget->fade();
    (View == 1) ? ui->toScore->setText("Score") : ui->toScore->setText("Projects");

    ui->toLayout->setVisible(View==0);
    ui->prevButton->setVisible(View==0);
    ui->nextButton->setVisible(View==0);
    ui->prevLayout->setVisible(View==1);
    ui->nextLayout->setVisible(View==1);
    ui->labelLayouts->setVisible(View==1);
    ui->searchCombo->setVisible(View==0);
    ui->searchBack->setVisible(View==0);
    ui->searchForward->setVisible(View==0);
    ui->searchLabel->setVisible(View==0);
    ui->searchButton->setVisible(View==0);

    actionSettings->setVisible(View == 0);
    actionLayoutSettings->setVisible(View == 1);

    actionScore->setEnabled(View==1);
    actionLayouts->setEnabled(View==0);

    for (QAction* a : (const QList<QAction*>)MainMenu->EditMenu->actions()) a->setVisible(View == 0);
    for (QAction* a : (const QList<QAction*>)MainMenu->UndoMenu->actions()) a->setVisible(true);

    actionLayoutSystem->setVisible(View==1);
    actionLayoutPage->setVisible(View==1);
    actionLayoutLayout->setVisible(View==1);

    for (QAction* a : (const QList<QAction*>)printMenu->actions()) a->setEnabled(View == 1);

    if (View==0)
    {
        (SearchComboVisible) ? ui->searchButton->hide() : ui->searchCombo->hide();
    }
}

void CScoreDoc::ReformatLayout(const LayoutLocation& StartLocation, const int Stretch)
{
    ui->FadingWidget->prepare();
    MainMenu->UndoMenu->addItem("Reformat Layout");
    lv->Reformat(StartLocation,Stretch);
    UpdateStatus();
    ui->FadingWidget->fade();
}

void CScoreDoc::ReformatLayoutFromStart()
{
    ReformatLayout(LayoutLocation(0,0));
}

void CScoreDoc::ReformatFromHere()
{
    ReformatLayout(lv->activeLayoutLocation());
}

void CScoreDoc::StretchFromHere()
{
    ReformatLayout(lv->activeLayoutLocation(),5);
}

void CScoreDoc::CompressFromHere()
{
    ReformatLayout(lv->activeLayoutLocation(),-5);
}

void CScoreDoc::AutoAdjust()
{
    ui->FadingWidget->prepare();
    MainMenu->UndoMenu->addItem("Autoadjust");
    lv->Reformat();
    lv->AdjustPages(0);
    UpdateStatus();
    ui->FadingWidget->fade();
}

void CScoreDoc::TweakSystem()
{
    QMenu m(this);
    m.addAction("");
    CTweakSystem d(&m);
    XMLScoreWrapper s(sv->XMLScore.getClone());
    d.Fill(s, lv->activeLayoutIndex(),lv->activeLayoutLocation(),lv->getZoom());
    d.adjustSize();
    m.setFixedSize(d.size()+QSize(10,10));
    d.move(5,5);
    m.exec(lv->mapToGlobal(QPointF(lv->activeSystemRect().topLeft()-d.sysPos()).toPoint())-QPoint(5,5));
    if (d.accepted())
    {
        MainMenu->UndoMenu->addItem("Tweak System");
        ui->FadingWidget->prepare();
        LayoutLocation l(0,0);
        d.GetResult(l,sv->XMLScore);
        sv->ReloadXML();
        lv->SetXML(sv->XMLScore);
        lv->Init();
        UpdateStatus();
        lv->SelectSystem(l);
        ui->FadingWidget->fade();
    }
}

void CScoreDoc::PasteSymbol(XMLSimpleSymbolWrapper& Symbol, QString UndoText, bool Finished)
{
    if (UndoText.length()) MainMenu->UndoMenu->addItem(UndoText);
    sv->pasteSymbol(Symbol);
    if (Finished)
    {
        sv->Paint(tsReformat);
        SVChanged();
        sv->setFocus();
    }
}

void CScoreDoc::PasteSymbol(XMLSimpleSymbolWrapper& Symbol, QString UndoText)
{
    if (UndoText.length()) MainMenu->UndoMenu->addItem(UndoText);
    sv->pasteSymbol(Symbol);
    sv->Paint(tsReformat);
    SVChanged();
    sv->setFocus();
}

void CScoreDoc::PasteDuratedSymbol(XMLSimpleSymbolWrapper& Symbol, QString UndoText)
{
    int CountIt=0;
    int CurrentMeter=sv->findCurrentMeter(OCSymbolLocation(sv->ActiveVoiceLocation(),sv->Cursor.SelStart()));
    for (int i=sv->Cursor.SelStart();i<sv->Cursor.SelEnd();i++)
    {
        XMLSymbolWrapper XMLSymbol=sv->XMLScore.Voice(sv->ActiveVoiceLocation()).XMLSymbol(i,CurrentMeter);
        if (XMLSymbol.IsRestOrValuedNote()) {
            CountIt+=XMLSymbol.ticks();
        }
        else if (XMLSymbol.IsTime()) CurrentMeter=OCCounter::calcTime(XMLSymbol);
    }
    Symbol.setAttribute("Ticks",CountIt);
    if (UndoText.length()) MainMenu->UndoMenu->addItem(UndoText);
    int SE=sv->Cursor.SelEnd();
    sv->Cursor.SetPos(sv->Cursor.SelStart());
    sv->pasteSymbol(Symbol);
    sv->Cursor.ExtendSel(SE+1);
    sv->Paint(tsReformat);
    SVChanged();
    sv->setFocus();
}

void CScoreDoc::OverwriteProperty(QString Name, QVariant Value, QString UndoText, bool Finished)
{
    if (sv->Cursor.SelCount()==0) return;
    if (UndoText.length()) MainMenu->UndoMenu->addItem(UndoText);
    for (const int& i : sv->Cursor.SelectedPointers())
    {
        XMLSimpleSymbolWrapper s=sv->GetSymbol(i);
        OCSymbolsCollection::ChangeProperty(s,Name,Value);
    }
    if (Finished)
    {
        sv->Paint(tsReformat);
        SVChanged();
        sv->setFocus();
    }
}

void CScoreDoc::SetDurated()
{
    if (sv->Cursor.SelCount() > 1)
    {
        for (int py=sv->Cursor.SelStart();py<sv->Cursor.SelEnd();py++)
        {
            if (sv->GetSymbol(py).IsRestOrValuedNote())
            {
                for (QWidget* w : std::as_const(DuratedGridWidgets)) w->setEnabled(true);
                return;
            }
        }
    }
    for (QWidget* w : std::as_const(DuratedGridWidgets)) w->setEnabled(false);
}

const OCVoiceLocation CScoreDoc::MarkToVoiceLocation(const int MarkIndex) const
{
    return sv->MarkToVoiceLocation(SelectionStaff+MarkIndex);
}

void CScoreDoc::SaveLocation()
{
    OCBarSymbolLocation sl=sv->findPointerToBar(sv->StartBar());
    if (LocationIndex==-1)
    {
        Locations.clear();
        Locations.append(sl);
        LocationIndex=0;
    }
    else
    {
        if (!sl.matches(Locations[LocationIndex]))
        {
            while (Locations.size()>LocationIndex+1) Locations.removeLast();
            Locations.append(sl);
            LocationIndex++;
        }
    }
    while (LocationIndex>20)
    {
        Locations.removeFirst();
        LocationIndex--;
    }
    UpdateLocationButtons();
}

void CScoreDoc::LocationForward()
{
    if (LocationIndex+1<Locations.size())
    {
        prepareFade();
        LocationIndex++;
        sv->setActiveBarLocation(Locations[LocationIndex]);
        sv->Paint(tsNavigate,true);
        sv->ensureVisible();
        UpdateStatus();
        UpdateLocationButtons();
        ui->FadingWidget_2->fade();
    }
}

void CScoreDoc::LocationBack()
{
    if (LocationIndex>0)
    {
        prepareFade();
        LocationIndex--;
        sv->setActiveBarLocation(Locations[LocationIndex]);
        sv->Paint(tsNavigate,true);
        sv->ensureVisible();
        UpdateStatus();
        UpdateLocationButtons();
        ui->FadingWidget_2->fade();
    }
}

void CScoreDoc::UpdateLocationButtons()
{
    ui->prevButton->setEnabled(LocationIndex>0);
    if (LocationIndex>0)
    {
        ui->prevButton->setToolTip(LocationString(Locations[LocationIndex-1]));
    }
    else
    {
        ui->prevButton->setToolTip(QString());
    }
    ui->nextButton->setEnabled(LocationIndex<Locations.size()-1);
    if (LocationIndex<Locations.size()-1)
    {
        ui->nextButton->setToolTip(LocationString(Locations[LocationIndex+1]));
    }
    else
    {
        ui->nextButton->setToolTip(QString());
    }
}

const QString CScoreDoc::LocationString(const OCBarSymbolLocation& sl) const
{
    QString txt = "<b>"+sv->StaffName(sl.StaffId)+"</b><br>";
    txt+="Voice <b>"+QString::number(sl.Voice+1)+"</b><br>";
    txt+="Bar <b>"+QString::number(sl.Bar+1)+"</b>";
    return txt;
}

bool CScoreDoc::event(QEvent *event)
{
    if (event->type()==QEvent::Show)
    {
        QMacSplitter* sp=findChild<QMacSplitter*>("layoutSidebarSplitter");
        OCSettings s;
        sp->Load("LayoutSidebarSplitter",s);
        for (int i=0;i<ui->Toolbox->topLevelItemCount();i++)
        {
            ui->Toolbox->topLevelItem(i)->setExpanded(!s.value("Tabs/"+ui->Toolbox->topLevelItem(i)->text(0)+"_tab_closed",false).toBool());
        }
    }
    if (event->type()==QEvent::Hide)
    {
        QMacSplitter* sp=findChild<QMacSplitter*>("layoutSidebarSplitter");
        OCSettings s;
        sp->Save("LayoutSidebarSplitter",s);
        for (int i=0;i<ui->Toolbox->topLevelItemCount();i++)
        {
            s.setValue("Tabs/"+ui->Toolbox->topLevelItem(i)->text(0)+"_tab_closed",!ui->Toolbox->topLevelItem(i)->isExpanded());
        }
    }
    return QWidget::event(event);
}

void CScoreDoc::staffConfigChanged()
{
    UpdateXML();
    Locations.clear();
    LocationIndex=-1;
    UpdateLocationButtons();
    SVChanged();
    MIDI2wav->updateMIDIConfig();
}

void CScoreDoc::gotoBar(const int Bar)
{
    if (sv->FollowResize()!=ScoreViewXML::PageSizeUnlimited)
    {
        if ((Bar < sv->StartBar()) || (Bar >= sv->StartBar()+sv->BarsActuallyDisplayed())) sv->setStartBar(Bar);
        sv->Paint(tsNavigate,true);
    }
    else
    {
        sv->scrollToBar(Bar);
    }

}

void CScoreDoc::gotoSymbol(const OCSymbolLocation &Symbol)
{
    gotoBar(sv->BarMap().GetBar(Symbol).Bar);
}

void CScoreDoc::doSearch(const QString& SearchTerm)
{
    ui->searchLabel->setText(QString());
    SearchResult=sv->search(SearchTerm);
    ui->searchBack->setEnabled(SearchResult.size()>1);
    ui->searchForward->setEnabled(SearchResult.size()>1);
    if (SearchIndex>=SearchResult.size()) SearchIndex=0;
    if (SearchIndex<0) SearchIndex=SearchResult.size()-1;
    if (SearchResult.size())
    {
        ui->searchLabel->setText("<font color=#eee>"+QString::number(SearchIndex+1)+" of "+QString::number(SearchResult.size()) +" </font>");
        OCBarSymbolLocation s=SearchResult[SearchIndex];
        setActiveStaffId(s.StaffId);
        setActiveVoice(s.Voice);
        sv->Cursor.SetPos(s.Pointer);
        gotoBar(s.Bar);
        sv->flashSelected();
    }
}

void CScoreDoc::search(const QString SearchTerm)
{
    SearchIndex=0;
    doSearch(SearchTerm);
}

void CScoreDoc::searchNext()
{
    SearchIndex++;
    doSearch(ui->searchCombo->currentText());
}

void CScoreDoc::searchPrev()
{
    SearchIndex--;
    doSearch(ui->searchCombo->currentText());
}

void CScoreDoc::UpdateBW()
{
    OCBarMap l=sv->BarMap();
    for (CBarWindow* w : (const QList<CBarWindow*>)findChildren<CBarWindow*>()) w->Fill(l,sv->ActiveBarLocation(), sv->BarsActuallyDisplayed());
}

void CScoreDoc::UpdatePW()
{
    for (CPropertyWindow* w : (const QList<CPropertyWindow*>)findChildren<CPropertyWindow*>()) w->Fill(sv->CurrentSymbol(),sv->ActiveVoice());
}

void CScoreDoc::PropertiesMenu()
{
    PopupProperties(cursor().pos());
}

void CScoreDoc::VoiceListMenu()
{
    PopupVoiceList(cursor().pos());
}

void CScoreDoc::BarMapMenu()
{
    PopupBarMap(cursor().pos());
}

void CScoreDoc::PopupProperties(QPoint p)
{
    QMenu* d = new QMenu(this);
    d->setAttribute(Qt::WA_DeleteOnClose);
    QWidgetAction* a = new QWidgetAction(this);
    CPropertyWindow* w = new CPropertyWindow(d);
    a->setDefaultWidget(w);
    d->addAction(a);
    connect(w,&CPropertyWindow::Changed,this,&CScoreDoc::ChangeProperty);
    w->Fill(sv->CurrentSymbol(),sv->ActiveVoice());
    w->show();
    w->updateGeometry();
    w->setFixedSize(w->contentSize());
    d->setFixedSize(w->size()+QSize(10,10));
    d->popup(p);
}

void CScoreDoc::PopupVoiceList(QPoint p)
{
    QMenu* d = new QMenu(this);
    d->setAttribute(Qt::WA_DeleteOnClose);
    QWidgetAction* a = new QWidgetAction(this);
    CMusicTree* w = new CMusicTree(d);
    a->setDefaultWidget(w);
    d->addAction(a);
    connect(w,&CMusicTree::SelectionChanged,this,&CScoreDoc::TreeSelectionChanged);
    connect(w,&CMusicTree::Rearranged,this,&CScoreDoc::TreeItemsRearranged);
    connect(w,&CMusicTree::SendBackspace,sv,&ScoreViewXML::selectBackSpace);
    connect(w,&CMusicTree::SendDelete,sv,&ScoreViewXML::selectDelete);
    connect(w,&CMusicTree::Delete,this,&CScoreDoc::DeleteItem);
    connect(w,&CMusicTree::Popup,this,&CScoreDoc::ShowContextPopup);
    connect(w,&CMusicTree::Properties,this,&CScoreDoc::PopupProperties);
    OCBarMap l=sv->BarMap();
    w->Fill(sv->XMLScore,l,sv->BarsActuallyDisplayed(),sv->ActiveBarLocation(),&sv->Cursor);
    w->show();
    w->updateGeometry();
    w->setFixedSize(w->contentSize());
    if (w->height()>sv->height()) w->setFixedHeight(sv->height());
    if (w->width()>sv->width()) w->setFixedWidth(sv->width());
    d->setFixedSize(w->size()+QSize(10,10));
    d->popup(p);
}

void CScoreDoc::PopupBarMap(QPoint p)
{
    QMenu* d = new QMenu(this);
    d->setAttribute(Qt::WA_DeleteOnClose);
    QWidgetAction* a = new QWidgetAction(this);
    CBarWindow* w = new CBarWindow(d);
    a->setDefaultWidget(w);
    d->addAction(a);
    connect(w,&CBarWindow::SelectionChanged,this,&CScoreDoc::BarSelectionChanged);
    connect(w,&CBarWindow::BarChanged,this,&CScoreDoc::setBar);
    connect(w,&CBarWindow::Popup,this,&CScoreDoc::ShowContextPopup);
    OCBarMap l=sv->BarMap();
    w->Fill(l,sv->ActiveBarLocation(),sv->BarsActuallyDisplayed());
    w->show();
    w->setFixedSize(w->contentSize());
    w->updateGeometry();
    if (w->height()>sv->height()) w->setFixedHeight(sv->height());
    if (w->width()>sv->width()) w->setFixedWidth(sv->width());
    d->setFixedSize(w->size()+QSize(10,10));
    d->popup(p);
}

void CScoreDoc::UpdateTree()
{
    OCBarMap l=sv->BarMap();
    for (CMusicTree* w : (const QList<CMusicTree*>)findChildren<CMusicTree*>()) w->Fill(sv->XMLScore,l,sv->BarsActuallyDisplayed(),sv->ActiveBarLocation(),&sv->Cursor);
}

void CScoreDoc::UpdateSV(OCRefreshMode RefreshMode)
{
    sv->Paint(RefreshMode);
}

void CScoreDoc::ResetPositions()
{
    static QStringList l{"Left", "Top", "Size", "AccidentalLeft", "TieTop", "Curve", "Gap"};
    if (SelectionStaff>-1)
    {
        if (!SelectionList.isEmpty()) return;
        MainMenu->UndoMenu->addItem("Reset Positions");
        for (int i=0;i<SelectionList.size();i++)
        {
            for (int p=SelectionList[i].Start;p<=SelectionList[i].End;p++)
            {
                XMLSimpleSymbolWrapper s=sv->GetSymbol(OCSymbolLocation(MarkToVoiceLocation(i),p));
                OCSymbolsCollection::ChangeProperties(s,l,0);
            }
        }
    }
    else
    {
        if (!sv->Cursor.SelCount()) return;
        MainMenu->UndoMenu->addItem("Reset Positions");
        for (const int& i : sv->Cursor.SelectedPointers())
        {
            XMLSimpleSymbolWrapper s=sv->GetSymbol(i);
            OCSymbolsCollection::ChangeProperties(s,l,0);
        }
    }
    PWChanged(tsVoiceIndexChanged);
}

void CScoreDoc::Triolize()
{
    sv->dottify(3);
}

void CScoreDoc::Dottify()
{
    sv->dottify(4);
}

void CScoreDoc::DoubleDottify()
{
    sv->dottify(8);
}

void CScoreDoc::Straighten()
{
    sv->dottify(2);
}

void CScoreDoc::DeleteDoc()
{
    if (SelectionStaff>-1)
    {
        MainMenu->UndoMenu->addItem("Delete");
        for (int i=0;i<SelectionList.size();i++)
        {
            sv->Delete(MarkToVoiceLocation(i),SelectionList[i]);
        }
    }
    else
    {
        sv->selectBackSpace();
    }
    UpdateScoreView();
    UpdateStatus();
}

void CScoreDoc::CopyDoc(QDomLiteElement* xml)
{
    if (SelectionStaff>-1)
    {
        for (int i=0;i<SelectionList.size();i++)
        {
            xml->appendClone(sv->GetClipBoardData(MarkToVoiceLocation(i),SelectionList[i]).xml());
        }
    }
    else
    {
        xml->appendClone(sv->GetClipBoardData().xml());
    }
    UpdateStatus();
}

void CScoreDoc::PasteDoc(const QDomLiteElement* xml)
{
    MainMenu->UndoMenu->addItem("Paste");
    if (SelectionStaff < 0)
    {
        if (sv->Cursor.SelCount() > 1) sv->Delete();
        const XMLVoiceWrapper& v(xml->childElement(0));
        sv->PasteClipBoardData(sv->Cursor.currentPointer(),v);
        sv->Cursor.SetPos(sv->Cursor.currentPointer()+v.symbolCount());
    }
    else
    {
        for (int i = 0; i < xml->childCount(); i++)
        {
            const OCVoiceLocation& l=MarkToVoiceLocation(i);
            if (l.StaffId > sv->StaffCount()) break;
            const OCBarSymbolLocation& Pointer=sv->findPointerToBar(l,SelectionBar);
            const XMLVoiceWrapper& v(xml->childElement(i));
            sv->PasteClipBoardData(Pointer,v);
        }
    }
    UpdateScoreView();
    UpdateStatus();
}

void CScoreDoc::SwapForward()
{
    sv->selectSwapForward();
}

void CScoreDoc::SwapBack()
{
    sv->selectSwapBack();
}

void CScoreDoc::SelectAll()
{
    if (SelectionStaff>-1)
    {
        for (CBarWindow* w : (const QList<CBarWindow*>)findChildren<CBarWindow*>()) w->SelectAll();
    }
    else
    {
        sv->selectAll();
    }
}

void CScoreDoc::SelectFromStart()
{
    if (SelectionStaff>-1)
    {
        for (CBarWindow* w : (const QList<CBarWindow*>)findChildren<CBarWindow*>()) w->SelectFromStart();
    }
    else
    {
        sv->selectToStart();
    }
}

void CScoreDoc::SelectToEnd()
{
    if (SelectionStaff>-1)
    {
        for (CBarWindow* w : (const QList<CBarWindow*>)findChildren<CBarWindow*>()) w->SelectToEnd();
    }
    else
    {
        sv->selectToEnd();
    }
}

void CScoreDoc::SVChanged()
{
    UpdatePW();
    UpdateBW();
    UpdateTree();
    UpdateStatus();
}

void CScoreDoc::PWChanged(OCRefreshMode refreshMode)
{
    sv->Paint(refreshMode);
    UpdateBW();
    UpdateTree();
    UpdateStatus();
}

void CScoreDoc::BarChanged()
{
    UpdateBW();
    UpdateTree();
    UpdateStatus();
}

void CScoreDoc::ChangeProperty(QString Name, QVariant Value, bool Custom)
{
    MainMenu->UndoMenu->addItem("Change Property "+Name);
    if (Custom)
    {
        OCRefreshMode RefreshMode;
        XMLSimpleSymbolWrapper s=sv->CurrentSymbol();
        if (!OCSymbolsCollection::editevent(s,RefreshMode,this)) return;
        PWChanged(RefreshMode);
        for (CPropertyWindow* w : (const QList<CPropertyWindow*>)findChildren<CPropertyWindow*>()) w->UpdateProperties(sv->CurrentSymbol(),sv->ActiveVoice());
    }
    else
    {
        for (const int& i : sv->Cursor.SelectedPointers())
        {
            XMLSimpleSymbolWrapper s=sv->GetSymbol(i);
            OCSymbolsCollection::ChangeProperty(s,Name,Value);
        }
        if (Name=="Pitch") sv->sound();
        PWChanged(tsReformat);
    }
}

void CScoreDoc::BarSelectionChanged(QRect Selection)
{
    SelectionList = sv->SelectionList(Selection.left(),Selection.right(),Selection.top(),Selection.bottom());
    SelectionStaff = Selection.top();
    SelectionBar = Selection.left();
    UpdateStatus();
}

void CScoreDoc::TreeSelectionChanged()
{
    UpdateSV(tsRedrawActiveStave);
    UpdatePW();
    sv->sound();
    SelectionList=sv->SelectionList();
    SelectionStaff=-1;
    UpdateStatus();
}

void CScoreDoc::TreeItemsRearranged(QList<int> itemOrder) {
    XMLVoiceWrapper v = sv->XMLScore.Voice(sv->ActiveStaffId(),sv->ActiveVoice());
    const QDomLiteElement t(v.xml()->clone());
    QList<int> oldOrder = itemOrder;
    std::sort(oldOrder.begin(), oldOrder.end());
    for (int i = 0; i < itemOrder.size(); i++) v.replaceChild(oldOrder[i],t.childElement(itemOrder[i]));
    UpdateSV(tsReformat);
    SVChanged();
}

void CScoreDoc::ScoreSelectionChanged()
{
    UpdatePW();
    UpdateTree();
    SelectionList=sv->SelectionList();
    SelectionStaff=-1;
    UpdateStatus();
}

void CScoreDoc::DeleteItem(int Pointer)
{
    MainMenu->UndoMenu->addItem("Delete");
    sv->Delete(Pointer);
    sv->Paint(tsReformat);
    sv->setFocus();
    SVChanged();
}

void CScoreDoc::destyleSearchCombo()
{
    ui->searchCombo->show();
    ui->searchButton->hide();
    SearchComboVisible=true;
    ui->searchCombo->showPopup();
}

void CScoreDoc::styleSearchCombo()
{
    ui->searchCombo->hide();
    ui->searchButton->show();
    SearchComboVisible=false;
}

void CScoreDoc::deleteVoiceSearch()
{
    const OCVoiceLocation& v = sv->ActiveVoiceLocation();
    //OCPointerList p;
    //for (OCBarSymbolLocation& l : SearchResult) {
    //    if (v.matches(l)) p.append(l.Pointer);
    //}
    const OCPointerList p = SearchResult.matchingVoice(v).pointers();
    if (!p.empty()) {
        MainMenu->UndoMenu->addItem("Delete from Voice " + ui->searchCombo->currentText());
        sv->Delete(v,p);
        sv->Paint(tsNavigate);
        SearchIndex = 0;
        doSearch(ui->searchCombo->currentText());
    }
}

void CScoreDoc::deleteSearch()
{
    if (SearchResult.empty()) return;
    MainMenu->UndoMenu->addItem("Delete from Score " + ui->searchCombo->currentText());
    for (int s = 0; s < sv->StaffCount(); s++)
    {
        for (int v = 0; v < sv->VoiceCount(s); v++)
        {
            const OCVoiceLocation vl(s,v);
            //const OCPointerList p = SearchResult.matchingVoice(vl).pointers();
            //for (OCBarSymbolLocation& l : SearchResult) {
            //    if (vl.matches(l)) p.append(l.Pointer);
            //}
            sv->Delete(vl,SearchResult.matchingVoice(vl).pointers());
        }
    }
    sv->Paint(tsNavigate);
    SearchIndex = 0;
    doSearch(ui->searchCombo->currentText());
}

void CScoreDoc::searchMenuPopup(QPoint pos)
{
    searchMenu->popup(ui->searchLabel->mapToGlobal(pos));
}

void CScoreDoc::NoteOnOff(bool On, int Pitch, int MixerTrack, OCMIDIVars MIDIInfo)
{
    OCMIDIVars m = MIDIInfo;
    if (On) {
        MIDI2wav->NoteOn(MixerTrack,byte(m.pitch(Pitch)),byte(m.Channel),byte(100.f*playControl->Volume()),byte(m.Patch));
    }
    else {
        MIDI2wav->NoteOff(MixerTrack,byte(m.pitch(Pitch)),byte(m.Channel));
    }
}

void CScoreDoc::RequestMidiFile(QString &path, int& bar, CMIDI2wav::PlayLocation mode, QStringList &TrackIDs)
{
    int StartBar=0;
    if ((mode!=CMIDI2wav::PlayEverything) && (GetView()==0))
    {
        StartBar=sv->BarMap().GetBar(sv->Cursor.location()).Bar;
    }
    if ((mode==CMIDI2wav::PlayStaffFromHere) && (GetView()==0))
    {
        MIDI2wav->muteAllTracks(false);
        sv->play(0,0,midifl);
        int trackNum=0;
        for (int s=0;s<sv->StaffCount();s++)
        {
            for (int t=0;t<sv->VoiceCount(s);t++)
            {
                if (s != sv->ActiveStaffId()) MIDI2wav->muteTrack(trackNum);
                trackNum++;
            }
        }
    }
    else
    {
        MIDI2wav->muteAllTracks(false);
        sv->play(0,0,midifl);
    }
    TrackIDs.clear();
    for (int i = 0; i < sv->BarMap().voiceCount(); i++) TrackIDs.append(sv->BarMap().voiceCaption(i));
    bar = StartBar;
    path=midifl;
}

void CScoreDoc::NewDoc()
{
    qDebug() << "Documentloader NewDoc";
    XMLScoreWrapper s;
    s.newScore();
    SetView(0);
    SetXML(s.getXML()->clone());
    MIDI2wav->updateMIDIConfig();
    setWindowTitle("Untitled");
    ActivateDoc();
}

void CScoreDoc::OpenDoc(QString Path)
{
    qDebug() << "Documentloader OpenDoc";
    ImportResult isImport = NoImport;
    QFileInfo f(Path);
    if (f.suffix()=="zip") {
        QDomLiteDocument d = CProjectPage::openFile(Path,".mus");
        QDomLiteElement xml;
        xml.appendChild(d.documentElement->clone());
        unserialize(&xml);  //m_Document->SetXML(d.clone());
        //setView(xml.firstChild()->attributeValueBool("DocumentView",true));
        isImport = ImportNativeXML;
    }
    else {
        isImport = Load(Path);
    }
    if (isImport)
    {
        (isImport > ImportNativeXML) ? setWindowTitle(f.baseName()) : setWindowTitle(Path);
        //m_Document->Path = (isImport==ImportMusicXML) ? QString() : Path;
        ActivateDoc();
        if (isImport != ImportMusicXML) MainMenu->RecentMenu->AddRecentFile(Path);
    }

    UpdateStatus();
    if (!isImport)
    {
        NewDoc();
        nativeAlert(m_MainWindow,"Object Composer","Could not Open File: " + Path,{"Ok"});
    }
}

void CScoreDoc::WizardDoc()
{
    qDebug() << "Documentloader WizardDoc";
    CScoreWizard d(m_MainWindow);
    d.Open(":/Template.mus");
    d.setWindowTitle("New Score");
    if (d.exec()==QDialog::Accepted)
    {
        setWindowTitle("Untitled");
        SetXML(d.CreateXML());
        ActivateDoc();
    }
}

void CScoreDoc::ShowPresets()
{
    CPresetsDialog d(m_MainWindow);
    d.setWindowTitle("Presets");
    CPropertyWindow* pw=d.findChild<CPropertyWindow*>();
    OCPresets P;
    pw->Fill(P.Properties());
    if (d.exec()==QDialog::Accepted) P.SaveProperties();
}

void CScoreDoc::UpdateAppTitle()
{
    QString title=windowTitle();
    if (MainMenu->UndoMenu->isDirty()) title+="*";
    m_MainWindow->setWindowTitle(title + " - " + apptitle);
}

void CScoreDoc::undoSerialize(QDomLiteElement* xml) const  {
    serialize(xml);
}

void CScoreDoc::undoUnserialize(const QDomLiteElement* xml)  {
    unserialize(xml);
}

void CScoreDoc::Render(QString path) {
    MIDI2wav->renderWaveFile(path);
}

void CScoreDoc::CloseDoc() {
    qDebug() << "Documentloader CloseDoc";
    if (!isClosed) {
        if (MIDI2wav->isPlaying()) MIDI2wav->pause();
        m_MainWindow->setWindowTitle(apptitle);
        playControl->hide();
        MIDI2wav->clearMixer();
        isClosed = true;
    }
}

void CScoreDoc::SaveDoc(QString path)
{
    qDebug() << "Documentloader SaveDoc";
    //if (m_Document->Path.length()==0) return SaveDialog();
    QFileInfo f(path);
    QString p = _DocumentPath + f.baseName() + ".zip";
    QDomLiteElement xml;
    serialize(&xml);
    xml.firstChild()->setAttribute("DocumentView",isVisible());
    QDomLiteDocument Doc;
    Doc.replaceDoc(xml.firstChild()->clone());
    CProjectPage::saveFile(p,&Doc,grab());
}

void CScoreDoc::ExportMidi()
{
    QFileDialog d(m_MainWindow,Qt::Sheet);
    d.setWindowTitle("Export MIDI");
    d.setNameFilter(tr("MIDI Files (*.mid)"));
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setFileMode(QFileDialog::AnyFile);
    if (MainMenu->FileName.isEmpty())
    {
        d.selectFile("Untitled.mid");
    }
    else
    {
        d.selectFile(MainMenu->FileName.replace("." + QFileInfo(MainMenu->FileName).completeSuffix(),".mid",Qt::CaseInsensitive));
    }
    if (d.exec() != QDialog::Accepted) return;
    const QStringList l = d.selectedFiles();
    if (!l.empty())  sv->play(0,0,l.first());
}

void CScoreDoc::ExportWave()
{
    QFileDialog d(m_MainWindow,Qt::Sheet);
    d.setWindowTitle("Export Audio");
    d.setNameFilter(tr("Wave Files (*.wav)"));
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setFileMode(QFileDialog::AnyFile);
    if (MainMenu->FileName.isEmpty())
    {
        d.selectFile("Untitled.wav");
    }
    else
    {
        d.selectFile(MainMenu->FileName.replace("." + QFileInfo(MainMenu->FileName).completeSuffix(),".wav",Qt::CaseInsensitive));
    }
    if (d.exec() != QDialog::Accepted) return;
    const QStringList l = d.selectedFiles();
    if (!l.empty()) CConcurrentDialog::run(this,&CScoreDoc::Render,l.first());
}

void CScoreDoc::ExportPDFDialog() {
    QFileDialog d(m_MainWindow,Qt::Sheet);
    d.setWindowTitle("Export PDF");
    d.setNameFilter(tr("PDF Files (*.pdf)"));
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setFileMode(QFileDialog::AnyFile);
    if (MainMenu->FileName.isEmpty())
    {
        d.selectFile("Untitled.pdf");
    }
    else
    {
        QFileInfo f(MainMenu->FileName);
        QString LayoutName = f.baseName();
        int index = lv->activeLayoutIndex();
        if (index > -1) LayoutName = lv->LayoutName(index);
        QString path = f.path() + "/" + LayoutName + ".pdf";
        d.selectFile(path);
    }
    if (d.exec() != QDialog::Accepted) return;
    const QStringList l = d.selectedFiles();
    if (!l.empty()) ExportPDF(l.first());
}

void CScoreDoc::ExportMXLDialog() {
    QFileDialog d(m_MainWindow,Qt::Sheet);
    d.setWindowTitle("Export MXL");
    d.setNameFilter(tr("MXL Files (*.mxl)"));
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setFileMode(QFileDialog::AnyFile);
    if (MainMenu->FileName.isEmpty())
    {
        d.selectFile("Untitled.mxl");
    }
    else
    {
        QFileInfo f(MainMenu->FileName);
        QString LayoutName = f.baseName();
        int index = lv->activeLayoutIndex();
        if (index > -1) LayoutName = lv->LayoutName(index);
        QString path = f.path() + "/" + LayoutName + ".mxl";
        d.selectFile(path);
    }
    if (d.exec() != QDialog::Accepted) return;
    const QStringList l = d.selectedFiles();
    if (!l.empty()) ExportMXL(l.first());
}

void CScoreDoc::ExportMusicXMLDialog() {
    QFileDialog d(m_MainWindow,Qt::Sheet);
    d.setWindowTitle("Export musicXML");
    d.setNameFilter(tr("musicXML Files (*.musicxml)"));
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setFileMode(QFileDialog::AnyFile);
    if (MainMenu->FileName.isEmpty())
    {
        d.selectFile("Untitled.mxl");
    }
    else
    {
        QFileInfo f(MainMenu->FileName);
        QString LayoutName = f.baseName();
        int index = lv->activeLayoutIndex();
        if (index > -1) LayoutName = lv->LayoutName(index);
        QString path = f.path() + "/" + LayoutName + ".musicxml";
        d.selectFile(path);
    }
    if (d.exec() != QDialog::Accepted) return;
    const QStringList l = d.selectedFiles();
    if (!l.empty()) ExportMusicXML(l.first());
}
