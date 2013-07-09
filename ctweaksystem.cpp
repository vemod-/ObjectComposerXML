#include "ctweaksystem.h"
#include "ui_ctweaksystem.h"

CTweakSystem::CTweakSystem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CTweakSystem)
{
    ui->setupUi(this);
    ui->FadingWidget->setWidget(ui->ScoreView);
    ui->LocationButtons->setSelectMode(QMacButtons::SelectNone);
    ui->LocationButtons->setFixedWidth(36*2);
    ui->LocationButtons->setFixedHeight(26);
    ui->LocationButtons->addButton("LocationBack","Previous Location",QIcon(":/24/24/locationback.png"));
    ui->LocationButtons->addButton("LocationForward","Next Location",QIcon(":/24/24/locationforward.png"));
    connect(ui->LocationButtons,SIGNAL(buttonClicked(int)),this,SLOT(LocationClicked(int)));
    connect(ui->ScoreView,SIGNAL(ActiveStaffChange(int)),this,SLOT(FillVoicesCombo(int)));
    connect(ui->VoiceCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(SelectVoice(int)));
    connect(ui->ScoreView,SIGNAL(NavigationForwardClicked()),this,SLOT(NextSystem()));
    connect(ui->ScoreView,SIGNAL(NavigationBackClicked()),this,SLOT(PrevSystem()));
    connect(ui->ScoreView,SIGNAL(NavigationEndClicked()),this,SLOT(LastSystem()));
    connect(ui->ScoreView,SIGNAL(NavigationHomeClicked()),this,SLOT(FirstSystem()));
}

CTweakSystem::~CTweakSystem()
{
    delete ui;
}

void CTweakSystem::FillVoicesCombo(int Staff)
{
    ui->VoiceCombo->blockSignals(true);
    ui->VoiceCombo->clear();
    for (int i=0;i<ui->ScoreView->VoiceCount(Staff);i++)
    {
        ui->VoiceCombo->addItem("Voice "+QString::number(i+1));
    }
    ui->VoiceCombo->setEnabled(ui->VoiceCombo->count()>1);
    ui->VoiceCombo->blockSignals(false);
}

void CTweakSystem::SelectVoice(int Voice)
{
    ui->ScoreView->setActiveVoice(Voice);
    ui->ScoreView->Paint(tsRedrawActiveStave);
    FillLabel();
}

void CTweakSystem::FillLabel()
{
    ui->label->setText("Page "+QString::number(m_ActivePage+1) + " - System "+QString::number(m_ActiveSystem+1) + " - Bar " + QString::number(ui->ScoreView->StartBar()+1) + " - "+ ui->ScoreView->StaffName(ui->ScoreView->ActiveStaff()) + " - Voice "+QString::number(ui->ScoreView->ActiveVoice()+1));
}

void CTweakSystem::Fill(XMLScoreWrapper& Score, CLayout *Layout, int ActivePage, int ActiveSystem)
{
    m_Layout=Layout;
    m_ActivePage=ActivePage;
    m_ActiveSystem=ActiveSystem;
    ui->ScoreView->SetXML(Score);
    ui->ScoreView->setLocked(false);
    ui->ScoreView->setFollowResize(false);
    ui->ScoreView->setNavigationVisible(true);
    ui->ScoreView->setSize(Layout->Options.ScaleSize*2.0);
    Paint();
    ui->ScoreView->adjustSize();
    ui->FadingWidget->adjustSize();
    ui->FadingWidget->setFixedSize(ui->FadingWidget->size());
    FillVoicesCombo(ui->ScoreView->ActiveStaff());
}

void CTweakSystem::Paint()
{
    CLayoutSystem* sys=m_Layout->ActivePage->ActiveSystem;
    ui->ScoreView->setActiveTemplate(&(sys->Template));
    ui->ScoreView->setStartBar(sys->StartBar);
    ui->ScoreView->setEndBar(sys->EndBar);
    ui->ScoreView->SetSystemLength(sys->Syslen);
    ui->ScoreView->setActiveStaff(XMLScoreWrapper::AllTemplateIndex(&sys->Template,0));
    ui->ScoreView->Paint(tsReformat);
    ui->LocationButtons->setEnabled(0,(m_ActivePage>0) || (m_ActiveSystem>0));
    ui->LocationButtons->setEnabled(1,(m_ActivePage<m_Layout->NumOfPages()-1) || (m_ActiveSystem<m_Layout->NumOfSystems(m_ActivePage)-1));
    FillLabel();
}

void CTweakSystem::GetResult(int &Page, int &System, XMLScoreWrapper& Score)
{
    Page=m_ActivePage;
    System=m_ActiveSystem;
    Score.replaceScore(ui->ScoreView->XMLScore.Score()->clone());
}

void CTweakSystem::LocationClicked(int value)
{
    if (value==0) PrevSystem();
    if (value==1) NextSystem();
}

void CTweakSystem::NextSystem()
{
    ui->FadingWidget->setTransitionType(QFadingWidget::PagingForward);
    ui->FadingWidget->prepare();
    if (m_ActivePage==m_Layout->NumOfPages()-1)
    {
        if (m_ActiveSystem==m_Layout->NumOfSystems(m_ActivePage)-1) return;
    }
    if (m_ActiveSystem>=m_Layout->NumOfSystems(m_ActivePage)-1)
    {
        m_ActivePage++;
        m_ActiveSystem=0;
    }
    else
    {
        m_ActiveSystem++;
    }
    m_Layout->setActiveObjects(m_ActivePage,m_ActiveSystem);
    Paint();
    ui->FadingWidget->fade();
}

void CTweakSystem::PrevSystem()
{
    ui->FadingWidget->setTransitionType(QFadingWidget::PagingBack);
    ui->FadingWidget->prepare();
    if (m_ActivePage==0)
    {
        if (m_ActiveSystem==0) return;
    }
    if (m_ActiveSystem==0)
    {
        m_ActivePage--;
        m_ActiveSystem=m_Layout->NumOfSystems(m_ActivePage)-1;
    }
    else
    {
        m_ActiveSystem--;
    }
    m_Layout->setActiveObjects(m_ActivePage,m_ActiveSystem);
    Paint();
    ui->FadingWidget->fade();
}

void CTweakSystem::LastSystem()
{
    ui->FadingWidget->setTransitionType(QFadingWidget::PagingForward);
    ui->FadingWidget->prepare();
    m_ActivePage=m_Layout->NumOfPages()-1;
    m_ActiveSystem=m_Layout->NumOfSystems(m_ActivePage)-1;
    m_Layout->setActiveObjects(m_ActivePage,m_ActiveSystem);
    Paint();
    ui->FadingWidget->fade();
}

void CTweakSystem::FirstSystem()
{
    ui->FadingWidget->setTransitionType(QFadingWidget::PagingBack);
    ui->FadingWidget->prepare();
    m_ActivePage=0;
    m_ActiveSystem=0;
    m_Layout->setActiveObjects(m_ActivePage,m_ActiveSystem);
    Paint();
    ui->FadingWidget->fade();
}
