#include "ctweaksystem.h"
#include "ui_ctweaksystem.h"
#include <QMenu>
#include <QWidgetAction>
#include "cpropertywindow.h"
#include "ocsymbolscollection.h"
//#include <QAction>

CTweakSystem::CTweakSystem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CTweakSystem)
{
    ui->setupUi(this);
    ui->FadingWidget->setWidget(ui->ScoreView);
    ui->LocationButtons->setSelectMode(QMacButtons::SelectNone);
    ui->LocationButtons->setFixedWidth(36*2);
    ui->LocationButtons->setFixedHeight(26);
    ui->LocationButtons->addButton("LocationBack","Previous Location",QIcon(":/24/24/locationback.png"));
    ui->LocationButtons->addButton("LocationForward","Next Location",QIcon(":/24/24/locationforward.png"));
    ui->propertiesButton->setIcon(QIcon(":/mini/mini/properties.png"));
    connect(ui->LocationButtons,qOverload<int>(&QMacButtons::buttonClicked),this,&CTweakSystem::LocationClicked);
    connect(ui->ScoreView,&ScoreViewXML::StaffIndexChanged,this,&CTweakSystem::FillVoicesCombo);
    connect(ui->VoiceCombo,qOverload<int>(&QComboBox::currentIndexChanged),this,&CTweakSystem::SelectVoice);
    connect(ui->propertiesButton,&QToolButton::clicked,this,&CTweakSystem::ShowProperties);
    connect(ui->ScoreView,&ScoreViewXML::NavigationForwardClicked,this,&CTweakSystem::NextSystem);
    connect(ui->ScoreView,&ScoreViewXML::NavigationBackClicked,this,&CTweakSystem::PrevSystem);
    connect(ui->ScoreView,&ScoreViewXML::NavigationEndClicked,this,&CTweakSystem::LastSystem);
    connect(ui->ScoreView,&ScoreViewXML::NavigationHomeClicked,this,&CTweakSystem::FirstSystem);

    connect(ui->acceptButton,&QAbstractButton::clicked,this,&CTweakSystem::Accept);
    connect(ui->ScoreView,&ScoreViewXML::accepted,this,&CTweakSystem::Accept);
    connect(ui->ScoreView,&ScoreViewXML::PropertiesPopup,this,&CTweakSystem::PopupProperties);

    ui->ScoreView->grabKeyboard();
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
    ui->label->setText("Page "+QString::number(m_ActiveLocation.Page+1) + " - System "+QString::number(m_ActiveLocation.System+1) + " - Bar " + QString::number(ui->ScoreView->StartBar()+1) + " - "+ ui->ScoreView->StaffName(ui->ScoreView->ActiveStaffId()) + " - Voice "+QString::number(ui->ScoreView->ActiveVoice()+1));
}

void CTweakSystem::Fill(XMLScoreWrapper& Score, const int activeLayoutIndex, const LayoutLocation& l, const double zoom)
{
    m_Layout=Score.Layout(activeLayoutIndex);
    m_ActiveLayout=activeLayoutIndex;
    m_ActiveLocation=l;
    ui->ScoreView->SetXML(Score);
    //qDebug() << ui->ScoreView->Size() << m_Layout.Options.scaleSize() << m_Layout.Options.size() << m_Layout.Options.scoreType() << m_Layout.Options.layoutZoom() << zoom;
    //ui->ScoreView->XMLScore.setOptions(m_Layout.Options);
    //ui->ScoreView->XMLScore.ScoreOptions.copy(m_Layout.Options);
    ui->ScoreView->setActiveOptions(m_Layout.Options);
    //ui->ScoreView->setActiveTemplate(m_Layout.XMLSystem(l).Template);
    ui->ScoreView->setLocked(false);
    ui->ScoreView->setFollowResize(ScoreViewXML::PageSizeFixed);
    ui->ScoreView->setNavigationVisible(false);
    ui->ScoreView->setSize(defaultscoresize);
    Paint();
    ui->ScoreView->setZoom(zoom * defaultscorezoom * (ui->ScoreView->Size() / m_Layout.Options.scaleSize()));
    ui->ScoreView->setFixedSize(QSizeF(ui->ScoreView->sceneRect().size() * ui->ScoreView->getZoom() * 1.02).toSize());
    ui->ScoreView->adjustSize();
    ui->FadingWidget->adjustSize();
    ui->FadingWidget->setFixedSize(ui->FadingWidget->size());
    FillVoicesCombo(ui->ScoreView->ActiveStaffId());
}

QPointF CTweakSystem::sysPos()
{
    return ui->ScoreView->systemRect().topLeft() * ui->ScoreView->getZoom();
}

void CTweakSystem::Paint()
{
    const XMLLayoutSystemWrapper& sys=m_Layout.XMLSystem(m_ActiveLocation);
    ui->ScoreView->setActiveTemplate(sys.Template);
    ui->ScoreView->setStartBar(sys.startBar());
    ui->ScoreView->setEndBar(sys.endBar());
    ui->ScoreView->SetSystemLength(int(sys.sysLen()));
    ui->ScoreView->setActiveStaffId(sys.Template.staffId(0));
    ui->ScoreView->Paint(tsReformat);
    ui->LocationButtons->setEnabled(0,!m_ActiveLocation.isFirstSystem());
    ui->LocationButtons->setEnabled(1,!m_Layout.isLastSystem(m_ActiveLocation));
    FillLabel();
}

void CTweakSystem::GetResult(LayoutLocation& l, XMLScoreWrapper& Score)
{
    l = m_ActiveLocation;
    Score.replaceScore(ui->ScoreView->XMLScore.Score);
}

void CTweakSystem::LocationClicked(int value)
{
    if (value==0) PrevSystem();
    if (value==1) NextSystem();
}

void CTweakSystem::NextSystem()
{
    ui->FadingWidget->setTransitionType(QFadingWidget::PageForward);
    ui->FadingWidget->prepare();
    if (m_Layout.isLastSystem(m_ActiveLocation))
    {
        ui->FadingWidget->clear();
        return;
    }
    m_Layout.nextSystem(m_ActiveLocation);
    Paint();
    ui->FadingWidget->fade();
}

void CTweakSystem::PrevSystem()
{
    ui->FadingWidget->setTransitionType(QFadingWidget::PageBack);
    ui->FadingWidget->prepare();
    if (m_ActiveLocation.isFirstSystem())
    {
        ui->FadingWidget->clear();
        return;
    }
    m_Layout.prevSystem(m_ActiveLocation);
    Paint();
    ui->FadingWidget->fade();
}

void CTweakSystem::LastSystem()
{
    ui->FadingWidget->setTransitionType(QFadingWidget::CoverRight);
    ui->FadingWidget->prepare();
    m_Layout.lastSystem(m_ActiveLocation);
    Paint();
    ui->FadingWidget->fade();
}

void CTweakSystem::FirstSystem()
{
    ui->FadingWidget->setTransitionType(QFadingWidget::UncoverLeft);
    ui->FadingWidget->prepare();
    m_ActiveLocation.Page=0;
    m_ActiveLocation.System=0;
    Paint();
    ui->FadingWidget->fade();
}

void CTweakSystem::PopupProperties(QPoint p)
{
    QMenu* d = new QMenu(this);
    d->setAttribute(Qt::WA_DeleteOnClose);
    QWidgetAction* a = new QWidgetAction(this);
    CPropertyWindow* w = new CPropertyWindow(d);
    a->setDefaultWidget(w);
    d->addAction(a);
    connect(w,&CPropertyWindow::Changed,this,&CTweakSystem::ChangeProperty);
    w->Fill(ui->ScoreView->CurrentSymbol(),ui->ScoreView->ActiveVoice());
    w->show();
    w->updateGeometry();
    w->setFixedSize(w->contentSize());
    d->setFixedSize(w->size()+QSize(10,10));
    d->popup(p);
}

void CTweakSystem::ChangeProperty(QString Name, QVariant Value, bool Custom)
{
    if (Custom)
    {
        OCRefreshMode RefreshMode;
        XMLSimpleSymbolWrapper s=ui->ScoreView->CurrentSymbol();
        if (!OCSymbolsCollection::editevent(s,RefreshMode,this)) return;
        ui->ScoreView->Paint(RefreshMode);
        for (CPropertyWindow* w : (const QList<CPropertyWindow*>)findChildren<CPropertyWindow*>()) w->UpdateProperties(ui->ScoreView->CurrentSymbol(),ui->ScoreView->ActiveVoice());
    }
    else
    {
        for (const int& i : ui->ScoreView->Cursor.SelectedPointers())
        {
            XMLSimpleSymbolWrapper s=ui->ScoreView->GetSymbol(i);
            OCSymbolsCollection::ChangeProperty(s,Name,Value);
        }
        if (Name=="Pitch") ui->ScoreView->sound();
        ui->ScoreView->Paint(tsReformat);
    }
}

void CTweakSystem::ShowProperties() {
    PopupProperties(mapToGlobal(ui->propertiesButton->pos()));
}
