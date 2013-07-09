#include "cscoredoc.h"
#include "ui_cscoredoc.h"
#include "qtoolbuttongrid.h"
#include "ctweaksystem.h"
#include <QSettings>
#include <QItemDelegate>
#include "mouseevents.h"

class layoutsDelegate : public QItemDelegate {
public:
  layoutsDelegate(QObject *parent=0) : QItemDelegate(parent){}

  void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const
  {
      QRect r(0,rect.top()+rect.height()-16,128,14);
      if (option.state & QStyle::State_Selected)
      {
          painter->setPen("#eee");
      }
      else
      {
          painter->setPen(Qt::black);
      }
      painter->drawText(r,text,QTextOption(Qt::AlignHCenter));
  }
/*
  void drawFocus( QPainter * painter, const QStyleOptionViewItem & option, const QRect & rect ) const
  {

  }
*/
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
      QItemDelegate::updateEditorGeometry(editor,option,index);
      editor->setGeometry(0,(index.row()*option.rect.height())+(option.rect.height()-16),128,14);
  }
};

void CScoreDoc::showLayouts()
{
    if (lv->NumOfLayouts()==0)
    {
        if (!this->AddLayout()) return;
    }
    SetView(1);
}

void CScoreDoc::showScore()
{
    SetView(0);
}

CScoreDoc::CScoreDoc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CScoreDoc)
{
    ui->setupUi(this);
    Dirty=false;
    setAutoFillBackground(true);
    ZoomList << 2 << 3 << 4 << 6 << 8 << 9 << 12 << 16 << 18 << 24 << 32 << 36;

    QStringList classes=OCSymbolsCollection::Classes();
    foreach(QString s,classes)
    {
        if (s != "EndOfVoice") ui->searchCombo->addItem(OCSymbolsCollection::Icon(s,0),s);
    }

    connect(ui->searchCombo,SIGNAL(currentIndexChanged(QString)),this,SLOT(search(QString)));
    connect(ui->searchButton,SIGNAL(clicked()),this,SLOT(DeStyleSearchCombo()));
    ui->searchCombo->setFocusPolicy(Qt::NoFocus);
    StyleSearchCombo();
    ui->searchBack->setArrowDirection(QAppleArrowButton::RoundLeft);
    ui->searchForward->setArrowDirection(QAppleArrowButton::RoundRight);
    ui->searchBack->setEnabled(false);
    ui->searchForward->setEnabled(false);
    connect(ui->searchBack,SIGNAL(clicked()),this,SLOT(searchPrev()));
    connect(ui->searchForward,SIGNAL(clicked()),this,SLOT(searchNext()));
    ui->searchBack->setIcon(QIcon(":/24/24/locationback.png"));
    ui->searchForward->setIcon(QIcon(":/24/24/locationforward.png"));
    ui->searchLabel->setText(QString());

    ui->NavFrame->setAutoFillBackground(true);
    m_View=-1;
    ui->SlidingWidget->setWidget(ui->allFrame);
    ui->SlidingWidget->setEasingCurve(QEasingCurve::OutQuad);
    ui->SlidingWidget->setDuration(400);
    ui->prevButton->setArrowDirection(QAppleArrowButton::RoundLeft);
    ui->nextButton->setArrowDirection(QAppleArrowButton::RoundRight);
    ui->prevButton->setIcon(QIcon(":/24/24/locationback.png"));
    ui->nextButton->setIcon(QIcon(":/24/24/locationforward.png"));
    connect(ui->prevButton,SIGNAL(clicked()),this,SLOT(LocationBack()));
    connect(ui->nextButton,SIGNAL(clicked()),this,SLOT(LocationForward()));
    ui->toScore->setArrowDirection(QAppleArrowButton::Left);
    ui->toLayout->setArrowDirection(QAppleArrowButton::Right);
    ui->prevLayout->setArrowDirection(QAppleArrowButton::RoundLeft);
    ui->nextLayout->setArrowDirection(QAppleArrowButton::RoundRight);
    ui->prevLayout->setIcon(QIcon(":/24/24/locationback.png"));
    ui->nextLayout->setIcon(QIcon(":/24/24/locationforward.png"));
    ui->toScore->hide();
    connect(ui->toLayout,SIGNAL(clicked()),this,SLOT(showLayouts()));
    connect(ui->toScore,SIGNAL(clicked()),this,SLOT(showScore()));
    connect(ui->prevLayout,SIGNAL(clicked()),this,SLOT(PrevLayout()));
    connect(ui->nextLayout,SIGNAL(clicked()),this,SLOT(NextLayout()));

    m_XMLScoreBackup.clear("Undo","UndoScores");
    SelectionStaff=-1;
    LocationIndex=-1;
    sv=findChild<ScoreViewXML*>();
    lv=findChild<LayoutViewXML*>();
    pw=ui->PropertyWindow;
    bw=ui->BarWindow;
    mt=ui->MusicTree;
    pno=ui->Piano;

    ui->FadingWidget_2->setWidget(sv);
    ui->FadingWidget_2->setDuration(600);

    spMain=new QMacSplitter();
    spMain->addWidget(ui->Toolbox);

    ui->SplitterLayout->addWidget(spMain);
    spBottom=new QMacSplitter;
    spRight=new QMacSplitter;

    spBottom->setOrientation(Qt::Vertical);
    spBottom->addWidget(ui->SVFrame);
    spBottom->addWidget(bw);
    spRight->setOrientation(Qt::Vertical);
    spRight->addWidget(pw);
    spRight->addWidget(ui->TreeFrame);
    spMain->addWidget(spBottom);
    spMain->addWidget(spRight);
    spMain->setStretchFactor(0,2);
    spMain->setStretchFactor(2,1);
    spMain->setStretchFactor(1,3);
    spBottom->setStretchFactor(1,1);
    spBottom->setStretchFactor(0,5);
    spRight->setStretchFactor(1,2);
    spRight->setStretchFactor(0,1);

    connect(sv,SIGNAL(ActiveStaffChange(int)),this,SLOT(UpdateStaffsCombo()));
    connect(sv,SIGNAL(Changed()),this,SIGNAL(Changed()));
    connect(sv,SIGNAL(ActiveStaffChange(int)),this,SLOT(SaveLocation()));
    connect(sv,SIGNAL(BarChanged()),this,SLOT(SaveLocation()));
    connect(sv,SIGNAL(ActiveStaffChange(int)),this,SIGNAL(StaffChanged()));
    connect(sv,SIGNAL(NavigationForwardClicked()),this,SLOT(SwipeForward()));
    connect(sv,SIGNAL(NavigationBackClicked()),this,SLOT(SwipeBack()));
    connect(sv,SIGNAL(NavigationEndClicked()),this,SLOT(SwipeFinish()));
    connect(sv,SIGNAL(NavigationHomeClicked()),this,SLOT(SwipeFirst()));
    connect(sv,SIGNAL(SwipeRightToLeft()),this,SLOT(SwipeForward()));
    connect(sv,SIGNAL(SwipeLeftToRight()),this,SLOT(SwipeBack()));

    connect(lv,SIGNAL(Changed()),this,SIGNAL(Changed()));
    connect(lv,SIGNAL(DoubleClick()),this,SLOT(TweakSystem()));
    connect(sv,SIGNAL(BackMeUp(QString)),this,SLOT(MakeBackup(QString)));
    connect(lv,SIGNAL(BackMeUp(QString)),this,SLOT(MakeBackup(QString)));

    connect(mt,SIGNAL(SendBackspace()),sv,SLOT(selectBackSpace()));
    connect(mt,SIGNAL(SendDelete()),sv,SLOT(selectDelete()));

    sv->setLocked(false);
    sv->setSize(12);
    sv->setNavigationVisible(true);

    ui->SidebarFrame->setAutoFillBackground(true);
    spSidebar=new QMacSplitter;
    spSidebar->setObjectName("layoutSidebarSplitter");
    ui->LayoutFrame->layout()->addWidget(spSidebar);
    ui->FadingWidget->setWidget(lv);
    ui->FadingWidget->setTransitionType(QFadingWidget::DarkFade);
    spSidebar->addWidget(ui->FadingWidget);
    spSidebar->setStretchFactor(0,80);
    spSidebar->addWidget(ui->SidebarFrame);
    spSidebar->setStretchFactor(1,1);

    lv->SetXMLScore(sv->XMLScore);

    tbAddLayout=new QToolButton(lv->viewport());
    tbAddLayout->setGeometry(40,40,248,264);
    tbAddLayout->setIconSize(QSize(240,240));
    tbAddLayout->setToolTip("Add Layout");
    tbAddLayout->setText("Add Layout");
    tbAddLayout->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    tbAddLayout->setVisible(true);
    tbAddLayout->setIcon(QIcon(":/addlayout.png"));
    tbAddLayout->setProperty("transparent",true);
    connect(tbAddLayout,SIGNAL(clicked()),this,SLOT(AddLayout()));

    ui->layoutList->setMinimumWidth(130);
    ui->layoutList->setItemDelegate(new layoutsDelegate);
    connect(ui->layoutList,SIGNAL(itemSelectionChanged()),this,SLOT(SetCurrentLayout()));
    connect(ui->layoutList,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(RenameItem(QTableWidgetItem*)));
    connect(ui->AddLayout,SIGNAL(clicked()),this,SLOT(AddLayout()));
    connect(ui->RemoveLayout,SIGNAL(clicked()),this,SLOT(DeleteLayout()));

    UpdateLayoutList();

    fillStaffsCombo();
    fillVoicesCombo();
    sv->setFocus();

    ui->layoutList->setAttribute(Qt::WA_MacShowFocusRect, 0);
    OCNoteToolbox* tb=ui->NoteToolbox;

    connect(pno,SIGNAL(TriggerNotes(QList<QPair<int,int> >&)),tb,SLOT(TriggerNotes(QList<QPair<int,int> >&)));
    connect(tb,SIGNAL(OverwriteProperty(QString,QVariant,QString,bool)),this,SLOT(OverwriteProperty(QString,QVariant,QString,bool)));
    connect(tb,SIGNAL(PasteXML(XMLSimpleSymbolWrapper&,QString,bool)),this,SLOT(PasteXML(XMLSimpleSymbolWrapper&,QString,bool)));

    QMacTreeWidget* tw=ui->Toolbox;
    //tw->setAutoFillBackground(true);
    //tw->setIndentation(12);
    tw->setHeaderHidden(true);
    tw->setUniformRowHeights(false);
    tw->setIconMode(false);
    //tw->setAnimated(false);
    //tw->setAttribute(Qt::WA_MacShowFocusRect, 0);

    foreach(QString Category, OCSymbolsCollection::Categories())
    {
        CPropertiesToolGrid* tg=new CPropertiesToolGrid(this);
        foreach(QString s, OCSymbolsCollection::Category(Category)) tg->AddButton(s);
        connect(tg,SIGNAL(PasteXML(XMLSimpleSymbolWrapper&,QString,OCRefreshMode)),this,SLOT(PasteXML(XMLSimpleSymbolWrapper&,QString,OCRefreshMode)));
        tg->AddToTree(Category,tw);
    }

    foreach(QString Category, OCSymbolsCollection::DuratedCategories())
    {
        CPropertiesToolGrid* tg=new CPropertiesToolGrid(this);
        foreach(QString s, OCSymbolsCollection::DuratedCategory(Category)) tg->AddButton(s);
        connect(tg,SIGNAL(PasteXML(XMLSimpleSymbolWrapper&,QString,OCRefreshMode)),this,SLOT(PasteDuratedXML(XMLSimpleSymbolWrapper&,QString,OCRefreshMode)));
        tg->AddToTree(Category,tw);
        DuratedGridWidgets.append(tg);
    }

    MakeBackup(QString());
    SaveLocation();
    SetView(0);
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
        MakeBackup("Rename Layout");
        int index=ui->layoutList->currentRow();
        if (item->text() != sv->XMLScore.LayoutName(index))
        {
            sv->XMLScore.setLayoutName(index,item->text());
            lv->lc.CurrentLayout->Name=item->text();
        }
        emit Changed();
        emit LayoutsChanged();
    }
}

void CScoreDoc::UpdateLayoutList()
{
    ui->layoutList->blockSignals(true);
    ui->layoutList->clear();
    ui->layoutList->setRowCount(lv->NumOfLayouts());
    ui->layoutList->setColumnWidth(0,ui->layoutList->width());
    for (int i=0; i<lv->NumOfLayouts(); i++)
    {
        QTableWidgetItem *newItem = new QTableWidgetItem;
        newItem->setText(lv->LayoutName(i));
        newItem->setIcon(QIcon(":/layout.png"));
        newItem->setFlags( newItem->flags() | Qt::ItemIsEditable);
        newItem->setTextAlignment(Qt::AlignBottom | Qt::AlignLeft);
        ui->layoutList->setRowHeight(i,150);
        ui->layoutList->setItem(i,0,newItem);
    }
    ui->layoutList->setCurrentCell(lv->CurrentLayout(),0);
    ui->layoutList->blockSignals(false);
}

void CScoreDoc::InitLayout(const int Index)
{
    lv->InitLayout(Index);
    UpdateLayoutList();
    SetCurrentLayout(Index);
    emit Changed();
}

bool CScoreDoc::AddLayout()
{
    bool RetVal=false;
    CLayoutWizard* d=new CLayoutWizard(this);
    d->Fill(sv->XMLScore,-1);
    d->setWindowTitle("New Layout");
    if (d->exec()==QDialog::Accepted)
    {
        MakeBackup("Add Layout");
        d->ModifyXML(sv->XMLScore,-1);
        lv->AddLayout();
        InitLayout(lv->NumOfLayouts()-1);
        emit LayoutsChanged();
        RetVal=true;
    }
    delete d;
    return RetVal;
}

void CScoreDoc::EditLayout()
{
    if (ui->layoutList->rowCount()==0) return;
    int Index=ui->layoutList->currentRow();
    CLayoutWizard* d=new CLayoutWizard(this);
    d->Fill(sv->XMLScore,Index);
    d->setWindowTitle("Edit Layout");
    if (d->exec()==QDialog::Accepted)
    {
        MakeBackup("Edit Layout");
        d->ModifyXML(sv->XMLScore,Index);
        InitLayout(Index);
    }
    delete d;
}

void CScoreDoc::DeleteLayout()
{
    if (ui->layoutList->rowCount()==0) return;
    MakeBackup("Delete Layout");
    int Index=ui->layoutList->currentRow();
    lv->RemoveLayout(Index);
    UpdateLayoutList();
    if (ui->layoutList->rowCount())
    {
        Index--;
        if (Index<0) Index=0;
    }
    SetCurrentLayout(Index);
    emit Changed();
    emit LayoutsChanged();
}

void CScoreDoc::PageSetup()
{
    if (lv->NumOfLayouts()==0) return;
    ui->FadingWidget->prepare();
    lv->PageSetup();
    lv->Init();
    ui->FadingWidget->fade();
}

void CScoreDoc::Print()
{
    if (lv->NumOfLayouts()==0) return;
    lv->PrinterPrint();
    UpdateLayoutView();
}

void CScoreDoc::PrintPreview()
{
    if (lv->NumOfLayouts()==0) return;
    lv->PrintPreview();
    UpdateLayoutView();
}

void CScoreDoc::NextLayout()
{
    if (lv->NumOfLayouts()==0) return;
    int l=ui->layoutList->currentRow();
    l++;
    if (l>=lv->NumOfLayouts()) l=0;
    SetCurrentLayout(l);
}

void CScoreDoc::PrevLayout()
{
    if (lv->NumOfLayouts()==0) return;
    int l=ui->layoutList->currentRow();
    l--;
    if (l<0) l=lv->NumOfLayouts()-1;
    SetCurrentLayout(l);
}

void CScoreDoc::SetCurrentLayout()
{
    if (ui->layoutList->currentRow() != lv->CurrentLayout()) SetCurrentLayout(ui->layoutList->currentRow());
}

void CScoreDoc::SetCurrentLayout(const int Index)
{
    if (lv->NumOfLayouts()==0)
    {
        tbAddLayout->setVisible(true);
        return;
    }
    ui->FadingWidget->prepare();
    tbAddLayout->setVisible(false);
    ui->layoutList->blockSignals(true);
    lv->SetCurrentLayout(Index);
    lv->Init();
    lv->SetActiveObjects(0,0);
    ui->layoutList->setCurrentCell(lv->CurrentLayout(),0);
    ui->labelLayouts->setText("<font color=#aaa>"+QString::number(ui->layoutList->currentRow()+1)+" of "+QString::number(ui->layoutList->rowCount())+" ");
    ui->nextLayout->setEnabled(lv->CurrentLayout()<lv->NumOfLayouts()-1);
    ui->prevLayout->setEnabled(lv->CurrentLayout()>0);
    ui->layoutList->blockSignals(false);
    ui->FadingWidget->fade();
}

void CScoreDoc::UpdateLayoutView()
{
    UpdateLayoutList();
    if (lv->NumOfLayouts()==0) return;
    tbAddLayout->setVisible(false);
    ui->layoutList->blockSignals(true);
    int Index=lv->CurrentLayout();
    while (Index>=lv->NumOfLayouts()) Index--;
    if (Index<0) Index=0;
    lv->SetCurrentLayout(Index);
    int Page=lv->ActivePage();
    int System=lv->ActiveSystem();
    lv->Init();
    ui->layoutList->setCurrentCell(lv->CurrentLayout(),0);
    lv->SetActiveObjects(Page,System);
    ui->layoutList->blockSignals(false);
}

void CScoreDoc::UpdateLayoutTab(int i)
{
    if (i == 0) UpdateAll();
    if (i == 1) UpdateLayoutView();
}

CScoreDoc::~CScoreDoc()
{
    //QList<CPropertiesToolGrid*> items=findChildren<CPropertiesToolGrid*>();
    //qDeleteAll(items);
    //delete tbAddLayout;
    delete ui;
}

void CScoreDoc::closeEvent(QCloseEvent * event)
{
    bool Cancel=false;
    emit Close(this,Cancel);
    if (Cancel)
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void CScoreDoc::fillStaffsCombo()
{
    QComboBox* cb=ui->cbStaffs;
    cb->blockSignals(true);
    cb->clear();
    for (int i=0;i<sv->StaffCount();i++)
    {
        cb->addItem(sv->StaffName(i));
    }
    cb->setCurrentIndex(sv->ActiveStaff());
    cb->setEnabled(cb->count()>1);
    cb->blockSignals(false);
}

void CScoreDoc::fillVoicesCombo()
{
    QComboBox* cb=ui->cbVoices;
    cb->blockSignals(true);
    cb->clear();
    for (int i=0;i<sv->VoiceCount();i++)
    {
        cb->addItem("Voice "+QString::number(i+1));
    }
    cb->setCurrentIndex(sv->ActiveVoice());
    cb->setEnabled(cb->count()>1);
    cb->blockSignals(false);
}

void CScoreDoc::setActiveStaff(const int Staff)
{
    if ((Staff != sv->ActiveStaff()) && (Staff>-1) && (Staff<sv->StaffCount()))
    {
        sv->setActiveStaff(Staff);
        sv->Paint(tsVoiceIndexChanged,true);
        UpdateStaffsCombo();
        sv->EnsureVisible();
        SaveLocation();
        emit StaffChanged();
    }
}

void CScoreDoc::UpdateStaffsCombo()
{
    fillStaffsCombo();
    fillVoicesCombo();
}

void CScoreDoc::setActiveVoice(const int Voice)
{
    if ((Voice != sv->ActiveVoice()) && (Voice>-1) && (Voice<sv->VoiceCount()))
    {
        sv->setActiveVoice(Voice);
        sv->Paint(tsRedrawActiveStave,true);
        UpdateStaffsCombo();
        SaveLocation();
        emit StaffChanged();
    }
}

void CScoreDoc::SetXML(XMLScoreWrapper& Doc)
{
    sv->SetXML(Doc);
    UpdateXML();
}

void CScoreDoc::Load(QString Path)
{
    sv->Load(Path);
    UpdateXML();
}

void CScoreDoc::UpdateXML()
{
    int Index=qMax(ui->layoutList->currentRow(),0);
    fillStaffsCombo();
    fillVoicesCombo();
    sv->Paint(tsReformat,true);
    lv->Load(sv->XMLScore);
    UpdateLayoutList();
    Index=qMin(ui->layoutList->rowCount()-1,Index);
    if (Index>-1) SetCurrentLayout(Index);
}

void CScoreDoc::UpdateAll()
{
    fillStaffsCombo();
    fillVoicesCombo();
    sv->Paint(tsNavigate,true);
}

void CScoreDoc::ShowStaffsDialog()
{
    CStaffsDialog* d=new CStaffsDialog(this);
    d->Fill(sv->XMLScore);
    d->setWindowTitle("Edit Score");
    if (d->exec()==QDialog::Accepted)
    {
        prepareFade();
        MakeBackup("Edit Staffs");
        XMLScoreWrapper s(d->CreateXML());
        if (sv->ActiveStaff()>s.NumOfStaffs()-1) sv->setActiveStaff(s.NumOfStaffs()-1);
        sv->setActiveVoice(0);
        Locations.clear();
        LocationIndex=-1;
        UpdateLocationButtons();
        SetXML(s);
        emit ScoreChanged();
        ui->FadingWidget_2->fade();
    }
    delete d;
}

void CScoreDoc::ZoomIn()
{
    SetZoom(100);
}

void CScoreDoc::ZoomOut()
{
    SetZoom(25);
}

void CScoreDoc::SetZoom(const int Zoom)
{
    if (GetView()==0)
    {
        //float factor=(1.0/FloatDiv(Zoom,50))*12;
        //sv->setSize(factor);
        float factor=(1-FloatDiv(Zoom-25,75))*(ZoomList.count()-1);
        sv->setSize(ZoomList[(int)factor]);
        sv->Paint(tsReformat,true);
        sv->EnsureVisible();
    }
    else
    {
        lv->SetZoom(Zoom);
    }
}

const int CScoreDoc::GetZoom() const
{
    if (GetView()==0)
    {
        //return (1.0/FloatDiv(sv->Size(),12))*50;
        int Index=ZoomList.indexOf(sv->Size());
        float factor=1.0-FloatDiv(Index,ZoomList.count()-1);
        return (factor*75)+25;
    }
    else
    {
        return lv->GetZoom();
    }
}

void CScoreDoc::SwipeForward()
{
    ui->FadingWidget_2->setTransitionType(QFadingWidget::FadingForward);
    ui->FadingWidget_2->setEasingCurve(QEasingCurve::InQuart);
    ui->FadingWidget_2->prepare();
    sv->turnpage();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::SwipeBack()
{
    ui->FadingWidget_2->setTransitionType(QFadingWidget::FadingBack);
    ui->FadingWidget_2->setEasingCurve(QEasingCurve::InQuart);
    ui->FadingWidget_2->prepare();
    sv->turnback();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::SwipeFirst()
{
    ui->FadingWidget_2->setTransitionType(QFadingWidget::FadingBack);
    ui->FadingWidget_2->setEasingCurve(QEasingCurve::InQuart);
    ui->FadingWidget_2->prepare();
    sv->fastback();
    ui->FadingWidget_2->fade();
}

void CScoreDoc::SwipeFinish()
{
    ui->FadingWidget_2->setTransitionType(QFadingWidget::FadingForward);
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

void CScoreDoc::MakeBackup(const QString& Text)
{
    Dirty=true;
    if (Text.length()==0)
    {
        m_XMLScoreBackup.documentElement->clearChildren();
        UndoIndex=-1;
        return;
    }
    while (m_XMLScoreBackup.documentElement->childCount()>UndoIndex+1) m_XMLScoreBackup.documentElement->removeLast();
    QDomLiteElement* s = m_XMLScoreBackup.documentElement->appendClone(sv->XMLScore.documentElement());
    s->setAttribute("UndoName",Text);
    UndoIndex++;
    while (m_XMLScoreBackup.documentElement->childCount()>20)
    {
        m_XMLScoreBackup.documentElement->removeFirst();;
        UndoIndex--;
    }
}

QString CScoreDoc::UndoText()
{
    if (m_XMLScoreBackup.documentElement->childCount())
    {
        if (UndoIndex>-1) return m_XMLScoreBackup.documentElement->childElement(UndoIndex)->attribute("UndoName");
    }
    return QString();
}

QString CScoreDoc::RedoText()
{
    if (m_XMLScoreBackup.documentElement->childCount())
    {
        if (UndoIndex<m_XMLScoreBackup.documentElement->childCount()-1) return m_XMLScoreBackup.documentElement->childElement(UndoIndex+1)->attribute("UndoName");
    }
    return QString();
}

void CScoreDoc::Undo()
{
    QString txt=UndoText();
    if (txt.length())
    {
        ui->FadingWidget->prepare();
        sv->XMLScore.setAttribute("UndoName",txt);
        m_XMLScoreBackup.documentElement->childElement(UndoIndex)->setAttribute("UndoName",QString());
        /*
        QDomLiteElement temp(sv->XMLScore.documentElement());
        //sv->XMLScore.documentElement()->copy(m_XMLScoreBackup.documentElement->childElement(UndoIndex));
        sv->XMLScore.replaceDocumentElement(m_XMLScoreBackup.documentElement->childElement(UndoIndex)->clone());
        m_XMLScoreBackup.documentElement->childElement(UndoIndex)->copy(&temp);
        //sv->XMLScore.getXML()->swapDoc(m_XMLScoreBackup.documentElement->childElement(UndoIndex));
        */
        m_XMLScoreBackup.documentElement->swapChild(UndoIndex,sv->XMLScore.documentPointer());
        sv->ReloadXML();
        lv->ReadXML();
        if (lv->NumOfLayouts()==0) SetCurrentLayout(-1);
        UndoIndex--;
        emit Changed();
        ui->FadingWidget->fade();
    }
}

void CScoreDoc::Redo()
{
    QString txt=RedoText();
    if (txt.length())
    {
        ui->FadingWidget->prepare();
        sv->XMLScore.setAttribute("UndoName",txt);
        m_XMLScoreBackup.documentElement->childElement(UndoIndex+1)->setAttribute("UndoName",QString());
        /*
        QDomLiteElement temp(sv->XMLScore.documentElement());
        //sv->XMLScore.documentElement()->copy(m_XMLScoreBackup.documentElement->childElement(UndoIndex+1));
        sv->XMLScore.replaceDocumentElement(m_XMLScoreBackup.documentElement->childElement(UndoIndex+1)->clone());
        m_XMLScoreBackup.documentElement->childElement(UndoIndex+1)->copy(&temp);
        //sv->XMLScore.getXML()->swapDoc(m_XMLScoreBackup.documentElement->childElement(UndoIndex+1));
        */
        m_XMLScoreBackup.documentElement->swapChild(UndoIndex+1,sv->XMLScore.documentPointer());
        sv->ReloadXML();
        lv->ReadXML();
        if (lv->NumOfLayouts()==0) SetCurrentLayout(-1);
        UndoIndex++;
        emit Changed();
        ui->FadingWidget->fade();
    }
}

void CScoreDoc::BarToNext()
{
    ui->FadingWidget->prepare();
    MakeBackup("Move Bar");
    lv->MoveBar(1);
    lv->WriteXML();
    emit Changed();
    ui->FadingWidget->fade();
}

void CScoreDoc::BarFromNext()
{
    ui->FadingWidget->prepare();
    MakeBackup("Move Bar");
    lv->MoveBar(-1);
    lv->WriteXML();
    emit Changed();
    ui->FadingWidget->fade();
}

void CScoreDoc::SystemToNext()
{
    ui->FadingWidget->prepare();
    MakeBackup("Move System");
    lv->MoveSystem(1);
    lv->WriteXML();
    emit Changed();
    ui->FadingWidget->fade();
}

void CScoreDoc::SystemFromNext()
{
    ui->FadingWidget->prepare();
    MakeBackup("Move System");
    lv->MoveSystem(-1);
    lv->WriteXML();
    emit Changed();
    ui->FadingWidget->fade();
}

void CScoreDoc::ToggleNames()
{
    ui->FadingWidget->prepare();
    MakeBackup("Toggle Names");
    lv->ToggleNames();
    lv->WriteXML();
    emit Changed();
    ui->FadingWidget->fade();
}

void CScoreDoc::EditSystem()
{
    ui->FadingWidget->prepare();
    MakeBackup("Edit System");
    //int Index=lv->CurrentLayoutIndex;
    lv->EditSystem(this);
    lv->WriteXML();
    //lv->InitLayout(Index);
    //UpdateLayoutList();
    //SetCurrentLayout(Index);
    emit Changed();
    ui->FadingWidget->fade();
}

void CScoreDoc::FitMusic()
{
    ui->FadingWidget->prepare();
    MakeBackup("Fit Music");
    lv->AdjustCurrentPage();
    lv->WriteXML();
    emit Changed();
    ui->FadingWidget->fade();
}

void CScoreDoc::FitPages(const int StartPage)
{
    ui->FadingWidget->prepare();
    MakeBackup("Fit Music");
    lv->AdjustPages(StartPage);
    lv->WriteXML();
    emit Changed();
    ui->FadingWidget->fade();
}

void CScoreDoc::ResetSystem()
{
    ui->FadingWidget->prepare();
    MakeBackup("Reset Position");
    lv->ResetSystem();
    lv->WriteXML();
    emit Changed();
    ui->FadingWidget->fade();
}

const int CScoreDoc::GetView() const
{
    return m_View;
}

void CScoreDoc::SetView(const int View)
{
    if (m_View==View) return;
    ui->SlidingWidget->prepare();
    ui->ScoreFrame->setVisible(View==0);
    ui->LayoutFrame->setVisible(View==1);
    QTreeWidget* tw=ui->Toolbox;
    if (View==1)
    {
        lv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        lv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ui->SlidingWidget->setTransitionType(QFadingWidget::PagingForward);
        ui->LayoutFrame->setGeometry(ui->allFrame->rect());
    }
    else
    {
        tw->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tw->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ui->PnoScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        //ui->PnoScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ui->SlidingWidget->setTransitionType(QFadingWidget::PagingBack);
        ui->ScoreFrame->setGeometry(ui->allFrame->rect());
    }
    m_View=View;
    UpdateLayoutTab(View);

    ui->SlidingWidget->fade();
    if (View==1)
    {
        lv->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        lv->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    else
    {
        tw->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tw->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        ui->PnoScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        //ui->PnoScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    ui->toScore->setVisible(View==1);
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
    if (View==0)
    {
        if (SearchComboVisible)
        {
            ui->searchButton->hide();
        }
        else
        {
            ui->searchCombo->hide();
        }
    }
}

void CScoreDoc::ReformatLayout(const int StartPage, const int StartSystem, const int Stretch)
{
    ui->FadingWidget->prepare();
    MakeBackup("Reformat Layout");
    lv->Reformat(StartPage,StartSystem,Stretch);
    lv->WriteXML();
    emit Changed();
    ui->FadingWidget->fade();
}

void CScoreDoc::AutoAdjust()
{
    ui->FadingWidget->prepare();
    MakeBackup("Autoadjust");
    lv->Reformat(0,0);
    //lv->Save();
    lv->AdjustPages(0);
    lv->WriteXML();
    emit Changed();
    ui->FadingWidget->fade();
}

void CScoreDoc::TweakSystem()
{
    CTweakSystem* d=new CTweakSystem(this);
    XMLScoreWrapper s(sv->XMLScore.getClone());
    d->Fill(s, lv->lc.CurrentLayout,lv->ActivePage(),lv->ActiveSystem());
    if (d->exec()==QDialog::Accepted)
    {
        MakeBackup("Tweak System");
        ui->FadingWidget->prepare();
        int Page=0;
        int System=0;
        d->GetResult(Page,System,sv->XMLScore);
        lv->Init();
        lv->WriteXML();
        emit Changed();
        lv->lc.CurrentLayout->setActiveObjects(Page,System);
        lv->SetZoom(lv->GetZoom());
        ui->FadingWidget->fade();
    }
    delete d;
}

void CScoreDoc::PasteXML(XMLSimpleSymbolWrapper& Symbol, QString UndoText, bool Finished)
{
    if (UndoText.length()) MakeBackup(UndoText);
    sv->PasteXML(Symbol);
    if (Finished)
    {
        sv->Paint(tsReformat);
        emit ScoreChanged();
        sv->setFocus();
    }
}

void CScoreDoc::PasteXML(XMLSimpleSymbolWrapper& Symbol, QString UndoText, OCRefreshMode RefreshMode)
{
    if (UndoText.length()) MakeBackup(UndoText);
    sv->PasteXML(Symbol);
    sv->Paint(tsReformat);
    emit ScoreChanged();
    sv->setFocus();
}

void CScoreDoc::PasteDuratedXML(XMLSimpleSymbolWrapper& Symbol, QString UndoText, OCRefreshMode RefreshMode)
{
    int CountIt=0;
    int CurrentMeter=sv->FindCurrentMeter(sv->ActiveStaff(),sv->ActiveVoice(),sv->Cursor.SelStart());
    for (int py=sv->Cursor.SelStart();py<sv->Cursor.SelEnd();py++)
    {
        XMLSymbolWrapper XMLSymbol(sv->XMLScore.Voice(sv->ActiveStaff(),sv->ActiveVoice()),py,CurrentMeter);
        if (XMLSymbol.IsRestOrValuedNote())
        {
            CountIt+=XMLSymbol.ticks();
        }
        if (XMLSymbol.Compare("Time")) CurrentMeter=CTime::GetTicks(XMLSymbol);
    }
    Symbol.setAttribute("Ticks",CountIt);
    if (UndoText.length()) MakeBackup(UndoText);
    int SE=sv->Cursor.SelEnd();
    sv->Cursor.SetPos(sv->Cursor.SelStart());
    sv->PasteXML(Symbol);
    sv->Cursor.ExtendSel(SE+1);
    sv->Paint(tsReformat);
    emit ScoreChanged();
    sv->setFocus();
}

void CScoreDoc::OverwriteProperty(QString Name, QVariant Value, QString UndoText, bool Finished)
{
    if (sv->Cursor.SelCount()==0) return;
    if (UndoText.length()) MakeBackup(UndoText);
    //for (int i=sv->Cursor.SelStart();i<=sv->Cursor.SelEnd();i++)
    sv->ChangeProperty(sv->Cursor.Selected,Name,Value);
    /*
    for (int j=0;j<sv->Cursor.SelCount();j++)
    {
        int i=sv->Cursor.Sel(j);
        sv->ChangeProperty(i,Name,Value);
    }
    */
    if (Finished)
    {
        sv->Paint(tsReformat);
        emit ScoreChanged();
        sv->setFocus();
    }
}

void CScoreDoc::SetDurated()
{
    if (sv->Cursor.SelCount())
    {
        for (int py=sv->Cursor.SelStart();py<sv->Cursor.SelEnd();py++)
        {
            if (sv->GetSymbol(py).IsRestOrValuedNote())
            {
                foreach(QWidget* w,DuratedGridWidgets) w->setEnabled(true);
                return;
            }
        }
    }
    foreach(QWidget* w,DuratedGridWidgets) w->setEnabled(false);
}

void CScoreDoc::SaveLocation()
{
    ScoreLocation sl;
    sl.StartBar=sv->StartBar();
    sl.Staff=sv->ActiveStaff();
    sl.Voice=sv->ActiveVoice();
    if (LocationIndex==-1)
    {
        Locations.clear();
        Locations.append(sl);
        LocationIndex=0;
    }
    else
    {
        if ((sl.StartBar != Locations.at(LocationIndex).StartBar) || (sl.Staff != Locations.at(LocationIndex).Staff) || (sl.Voice != Locations.at(LocationIndex).Voice))
        {
            while (Locations.count()>LocationIndex+1)
            {
                Locations.removeLast();
            }
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
    if (LocationIndex+1<Locations.count())
    {
        prepareFade();
        LocationIndex++;
        const ScoreLocation* sl=&Locations.at(LocationIndex);
        if (sv->ActiveStaff() != sl->Staff)
        {
            sv->setActiveStaff(sl->Staff);
        }
        if (sv->ActiveVoice() != sl->Voice)
        {
            sv->setActiveVoice(sl->Voice);
        }
        if (sv->StartBar() != sl->StartBar)
        {
            sv->setStartBar(sl->StartBar);
        }
        sv->Paint(tsNavigate,true);
        sv->EnsureVisible();
        emit Changed();
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
        const ScoreLocation* sl=&Locations.at(LocationIndex);
        if (sv->ActiveStaff() != sl->Staff)
        {
            sv->setActiveStaff(sl->Staff);
        }
        if (sv->ActiveVoice() != sl->Voice)
        {
            sv->setActiveVoice(sl->Voice);
        }
        if (sv->StartBar() != sl->StartBar)
        {
            sv->setStartBar(sl->StartBar);
        }
        sv->Paint(tsNavigate,true);
        sv->EnsureVisible();
        emit Changed();
        UpdateLocationButtons();
        ui->FadingWidget_2->fade();
    }
}

void CScoreDoc::UpdateLocationButtons()
{
    ui->prevButton->setEnabled(LocationIndex>0);
    if (LocationIndex>0)
    {
        ui->prevButton->setToolTip(LocationString(&Locations.at(LocationIndex-1)));
    }
    else
    {
        ui->prevButton->setToolTip(QString());
    }
    ui->nextButton->setEnabled(LocationIndex<Locations.count()-1);
    if (LocationIndex<Locations.count()-1)
    {
        ui->nextButton->setToolTip(LocationString(&Locations.at(LocationIndex+1)));
    }
    else
    {
        ui->nextButton->setToolTip(QString());
    }

    /*
    ui->NoteToolbox->LocationEnabled(0,LocationIndex>0);
    if (LocationIndex>0)
    {
        ui->NoteToolbox->LocationTooltip(0,LocationString(Locations[LocationIndex-1]));
    }
    else
    {
        ui->NoteToolbox->LocationTooltip(0,"");
    }
    ui->NoteToolbox->LocationEnabled(1,LocationIndex<Locations.count()-1);
    if (LocationIndex<Locations.count()-1)
    {
        ui->NoteToolbox->LocationTooltip(1,LocationString(Locations[LocationIndex+1]));
    }
    else
    {
        ui->NoteToolbox->LocationTooltip(1,"");
    }
    */
}

QString CScoreDoc::LocationString(const ScoreLocation* sl)
{
    QString txt = "<b>"+sv->StaffName(sl->Staff)+"</b><br>";
    txt+="Voice <b>"+QString::number(sl->Voice+1)+"</b><br>";
    txt+="Bar <b>"+QString::number(sl->StartBar+1)+"</b>";
    return txt;
}

bool CScoreDoc::event(QEvent *event)
{
    if (event->type()==QEvent::Show)
    {
        QMacSplitter* sp=findChild<QMacSplitter*>("layoutSidebarSplitter");
        sp->Load("LayoutSidebarSplitter");
        QSettings s;
        for (int i=0;i<ui->Toolbox->topLevelItemCount();i++)
        {
            ui->Toolbox->topLevelItem(i)->setExpanded(!s.value("Tabs/"+ui->Toolbox->topLevelItem(i)->text(0)+"_tab_closed",false).toBool());
        }
    }
    if (event->type()==QEvent::Hide)
    {
        QMacSplitter* sp=findChild<QMacSplitter*>("layoutSidebarSplitter");
        sp->Save("LayoutSidebarSplitter");
        QSettings s;
        for (int i=0;i<ui->Toolbox->topLevelItemCount();i++)
        {
            s.setValue("Tabs/"+ui->Toolbox->topLevelItem(i)->text(0)+"_tab_closed",!ui->Toolbox->topLevelItem(i)->isExpanded());
        }
    }
    return QWidget::event(event);
}

void CScoreDoc::doSearch(const QString& SearchTerm)
{
    ui->searchLabel->setText(QString());
    SearchResult=sv->Search(SearchTerm);
    ui->searchBack->setEnabled(SearchResult.count()>1);
    ui->searchForward->setEnabled(SearchResult.count()>1);
    if (SearchIndex>=SearchResult.count()) SearchIndex=0;
    if (SearchIndex<0) SearchIndex=SearchResult.count()-1;
    if (SearchResult.count())
    {
        ui->searchLabel->setText("<font color=#eee>"+QString::number(SearchIndex+1)+" of "+QString::number(SearchResult.count()) +" </font>");
        SymbolSearchLocation s=SearchResult[SearchIndex];
        setActiveStaff(s.Staff);
        setActiveVoice(s.Voice);
        if ((s.Bar < sv->StartBar()) || (s.Bar >= sv->StartBar()+sv->BarsActuallyDisplayed())) sv->setStartBar(s.Bar);
        sv->Cursor.SetPos(s.Pointer);
        sv->Paint(tsNavigate,true);
        sv->flashSelected();
    }
}

void CScoreDoc::search(const QString& SearchTerm)
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

void CScoreDoc::DeStyleSearchCombo()
{
    ui->searchCombo->show();
    ui->searchButton->hide();
    SearchComboVisible=true;
    ui->searchCombo->showPopup();
}

void CScoreDoc::StyleSearchCombo()
{
    ui->searchCombo->hide();
    ui->searchButton->show();
    SearchComboVisible=false;
}
