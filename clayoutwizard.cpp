#include "clayoutwizard.h"
#include "ui_clayoutwizard.h"
//#include <QMessageBox>
#include "idevice.h"
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
//#include <QPrinter>
#include <QPageSetupDialog>
//#include <QPrinterInfo>
#else
//#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPageSetupDialog>
//#include <QtPrintSupport/QPrinterInfo>
#endif

CLayoutWizard::CLayoutWizard(QWidget *parent) :
        QDialog(parent,Qt::Sheet),
    ui(new Ui::CLayoutWizard)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    setVisible(false);
    connect(ui->topMargin,&CMeasureControl::Changed,this,&CLayoutWizard::UpdateMargins);
    connect(ui->leftMargin,&CMeasureControl::Changed,this,&CLayoutWizard::UpdateMargins);
    connect(ui->rightMargin,&CMeasureControl::Changed,this,&CLayoutWizard::UpdateMargins);
    connect(ui->bottomMargin,&CMeasureControl::Changed,this,&CLayoutWizard::UpdateMargins);
    connect(ui->tabWidget,&QTabWidget::currentChanged,this,&CLayoutWizard::UpdateMargins);
    connect(ui->NoteSpace,&QAbstractSlider::valueChanged,this,&CLayoutWizard::SpacingTooltip);
    connect(ui->ImportButton,&QPushButton::clicked,this,&CLayoutWizard::ImportLayout);
    ui->graphicsView->setScene(&S);
    Printer=new QPrinter();
    pageSetupButton=new QToolButton(ui->graphicsView);
    pageSetupButton->setProperty("transparent",true);
    pageSetupButton->setIcon(QIcon(":/preferences.png"));
    pageSetupButton->setIconSize(QSize(32,32));
    pageSetupButton->setFixedSize(QSize(36,36));
    connect(pageSetupButton,&QAbstractButton::clicked,this,&CLayoutWizard::ShowPageSetup);
    UpdateMargins();
}

CLayoutWizard::~CLayoutWizard()
{
    delete ui;
    delete Printer;
}

int CLayoutWizard::ShowModal()
{
    show();
    while (isVisible())
    {
        QApplication::processEvents();
    }
    return result();
}

void CLayoutWizard::Fill(XMLScoreWrapper& XMLScore, int Index)
{
    m_Score.shadowXML(XMLScore);
    ui->StaffListLeft->clear();
    ui->StaffListRight->clear();

    for (int i=0;i<XMLScore.NumOfStaffs();i++)
    {
        QListWidgetItem* item=new QListWidgetItem(XMLScore.StaffName(i));
        item->setData(LWStaffId,i);
        item->setData(LWName,XMLScore.StaffName(i));
        item->setData(LWAbbreviation,XMLScore.StaffAbbreviation(i));
        ui->StaffListLeft->addItem(item);
    }
    auto bb=findChild<QDialogButtonBox*>();
    QPushButton* pb = bb->button(QDialogButtonBox::Ok);

    XMLLayoutOptionsWrapper Options;
    XMLLayoutFontsWrapper Fonts;
    if (Index > -1)
    {
        pb->setText("Apply Changes");
        XMLLayoutWrapper XMLLayout=XMLScore.Layout(Index);
        for (int i=0;i<XMLLayout.Template.staffCount();i++)
        {
            XMLTemplateStaffWrapper s(XMLLayout.Template.staff(i));
            const int ati=s.id();
            ui->StaffListLeft->item(ati)->setSelected(true);
            QListWidgetItem* item=new QListWidgetItem(ui->StaffListLeft->item(ati)->text());
            item->setData(LWStaffId,ati);
            item->setData(LWSquareBracket,s.squareBracket());
            item->setData(LWCurlyBracket,s.curlyBracket());
            item->setData(LWName,XMLScore.StaffName(ati));
            item->setData(LWAbbreviation,XMLScore.StaffAbbreviation(ati));
            item->setData(LWSize,s.size());
            ui->StaffListRight->addItem(item);
        }
        for (int i=ui->StaffListLeft->count()-1;i>=0;i--)
        {
            if (ui->StaffListLeft->item(i)->isSelected()) ui->StaffListLeft->takeItem(i);
        }
        ui->Name->setText(XMLLayout.name());
        Fonts = XMLLayout.Fonts;
        Options = XMLLayout.Options;
    }
    else
    {
        pb->setText("Create Layout");
        ui->Name->setText("New Layout");
        if (XMLScore.layoutCount())
        {
            int MatchCount=1;
            forever
            {
                bool Match=false;
                for (int i=0; i<XMLScore.layoutCount(); i++)
                {
                    if (ui->Name->text()==XMLScore.LayoutName(i))
                    {
                        Match=true;
                        ui->Name->setText("New Layout - "+QString::number(MatchCount++));
                    }
                }
                if (!Match) break;
            }
        }

        Options.setHideBarNumbers(XMLScore.ScoreOptions.hideBarNumbers());
        Options.setBarNumberOffset(XMLScore.ScoreOptions.barNumberOffset());
        //options.setNoteSpace(XMLScore.XMLOptions.noteSpace()-16);
        Options.setNoteSpace(XMLScore.ScoreOptions.noteSpace());
        Options.setMasterStaff(XMLScore.ScoreOptions.masterStaff());

        Options.setOrientation(Printer->pageLayout().orientation());
        //Options.setPaperSize(Printer->paperSize());

        MoveAllRight();
    }

    ValidateStaffs();

    FillOptions(Options,Fonts,XMLScore,ui->Name->text());
 /*
    for (int i=0;i<XMLScore.NumOfStaffs();i++)
    {
        ui->MasterStaff->addItem(XMLScore.StaffName(i));
    }

    ui->Type->clear();
    ui->Type->addItems(QStringList{"Large Part", "Part", "Score", "Pocket Score"});
    ui->ShowNames->clear();
    ui->ShowNames->addItems(QStringList{"Always", "When Staff Config Changes", "On First System Only"});

    ui->TitleBox->fill(Fonts.title.font(),Fonts.title.text());
    ui->SubtitleBox->fill(Fonts.subtitle.font(),Fonts.subtitle.text());
    ui->ComposerBox->fill(Fonts.composer.font(),Fonts.composer.text(),false,Qt::AlignRight | Qt::AlignVCenter);
    ui->StaffNamesBox->fill(Fonts.names.font(),"Sample Text",true,Qt::AlignLeft | Qt::AlignVCenter);

    ui->Type->setCurrentIndex(Options.scoreType()-1);
    ui->HideBarNumbers->setChecked(Options.hideBarNumbers());
    ui->BarNumberOffset->setValue(Options.barNumberOffset());
    ui->NoteSpace->setValue(Options.noteSpace()-16);
    ui->MasterStaff->setCurrentIndex(Options.masterStaff());
    ui->ShowNames->setCurrentIndex(Options.showNamesSwitch()-1);
    ui->ShowAllStaffs->setChecked(Options.showAllOnFirstSystem());
    ui->ShowAsSound->setChecked(!Options.transposeInstruments());
    ui->topMargin->setMillimeters(Options.topMargin());
    ui->leftMargin->setMillimeters(Options.leftMargin());
    ui->rightMargin->setMillimeters(Options.rightMargin());
    ui->bottomMargin->setMillimeters(Options.bottomMargin());
    ui->frontPage->setChecked(Options.frontPage());

    Printer->setPageOrientation(QPageLayout::Orientation(Options.orientation()));
    //Printer->setPaperSize(QPrinter::PaperSize(Options.paperSize()));

    ui->ImportCombo->clear();
    if (Index > -1) {
        if (XMLScore.layoutCount() > 1) {
            for (int i = 0; i < XMLScore.layoutCount(); i++) {
                if (XMLScore.Layout(i).name() != XMLScore.Layout(Index).name()) ui->ImportCombo->addItem(XMLScore.Layout(i).name());
            }
        }
    }
    else {
        if (XMLScore.layoutCount() > 0) {
            for (int i = 0; i < XMLScore.layoutCount(); i++) {
                ui->ImportCombo->addItem(XMLScore.Layout(i).name());
            }
        }
    }

    UpdateMargins();
    SpacingTooltip(ui->NoteSpace->value());
*/
}



void CLayoutWizard::ModifyXML(XMLScoreWrapper& XMLScore, int Index )
{
    XMLLayoutCollectionWrapper LayoutCollection=XMLScore.LayoutCollection;
    XMLTemplateWrapper XMLTemplate;
    for (int i=0; i<ui->StaffListRight->count(); i++)
    {
        XMLTemplateStaffWrapper XMLTemplateStaff;
        XMLTemplateStaff.setId(ui->StaffListRight->item(i)->data(LWStaffId).toInt());
        XMLTemplateStaff.setCurlyBracket(CurlyBracketConstants(ui->StaffListRight->item(i)->data(LWCurlyBracket).toInt()));
        XMLTemplateStaff.setSquareBracket(SquareBracketConstants(ui->StaffListRight->item(i)->data(LWSquareBracket).toInt()));
        //XMLTemplateStaff.setName(ui->StaffListRight->item(i)->data(LWName).toString());
        //XMLTemplateStaff.setAbbreviation(ui->StaffListRight->item(i)->data(LWAbbreviation).toString());
        XMLTemplateStaff.setSize(ui->StaffListRight->item(i)->data(LWSize).toInt());
        XMLTemplate.addChild(XMLTemplateStaff);
    }
    XMLTemplate.validateBrackets();
    XMLLayoutOptionsWrapper options;
    options.setShowLayoutName(ui->ShowLayoutName->isChecked());
    options.setShowNamesSwitch(ui->ShowNames->currentIndex()+1);
    options.setShowAllOnFirstSystem(ui->ShowAllStaffs->isChecked());
    options.setTransposeInstruments(!ui->ShowAsSound->isChecked());
    options.setTopMargin(ui->topMargin->Millimeters());
    options.setLeftMargin(ui->leftMargin->Millimeters());
    options.setRightMargin(ui->rightMargin->Millimeters());
    options.setBottomMargin(ui->bottomMargin->Millimeters());
    options.setOrientation(Printer->pageLayout().orientation());
    //options.setPaperSize(Printer->paperSize());
    options.setScoreType(ui->Type->currentIndex()+1);
    options.setHideBarNumbers(ui->HideBarNumbers->isChecked());
    options.setBarNumberOffset(ui->BarNumberOffset->value());
    options.setNoteSpace(ui->NoteSpace->value()+16);
    options.setMasterStaff(ui->MasterStaff->currentIndex());
    options.setFrontPage(ui->frontPage->isChecked());

    XMLLayoutWrapper XMLLayout;
    if (Index > -1)
    {
        QString Msg;
        XMLLayout=LayoutCollection.XMLLayout(Index);
        XMLTemplateWrapper oldTemplate(XMLLayout.Template.xml()->clone());
        XMLLayoutOptionsWrapper oldOptions(XMLLayout.Options.xml()->clone());
        if (oldOptions.orientation() != options.orientation())
        {
            Msg+="The Printer Orientation has Changed\n";
        }
        //if (oldOptions.paperSize() != options.paperSize())
        //{
        //    Msg+="The Paper Size has Changed\n";
        //}
        if (oldOptions.noteSpace() != options.noteSpace())
        {
            Msg+="The Note Spacing has Changed\n";
        }
        if (oldOptions.scoreType() != options.scoreType())
        {
            Msg+="The ScoreType has Changed\n";
        }
        if (oldTemplate.staffCount() != XMLTemplate.staffCount())
        {
            Msg+="The Number off Staffs has Changed\n";
        }
        else
        {
            bool Match=true;
            for (int i=0;i<XMLTemplate.staffCount();i++)
            {
                if (XMLTemplate.staffId(i) != oldTemplate.staffId(i))
                {
                    Match=false;
                    break;
                }
            }
            if (!Match) Msg+="The Staff Setup has Changed\n";
        }
        if (Msg.length())
        {
            int r = nativeAlert(this,"Object Composer",Msg+"The Layout will be Reformated, Continue ?",{"Cancel","Ok"});
            if (r == 1000) return;

            XMLLayout.xml()->removeChildren("Page");
            XMLLayout.setIsFormated(false);
        }
    }
    else
    {
        LayoutCollection.addChild();
        XMLLayout = LayoutCollection.XMLLayout(LayoutCollection.layoutCount()-1);
    }
    XMLLayout.Template.copy(XMLTemplate);
    XMLLayout.Options.copy(options);

    XMLLayout.Fonts.title.fill(ui->TitleBox->font(),ui->TitleBox->text());
    XMLLayout.Fonts.subtitle.fill(ui->SubtitleBox->font(),ui->SubtitleBox->text());
    XMLLayout.Fonts.composer.fill(ui->ComposerBox->font(),ui->ComposerBox->text());
    XMLLayout.Fonts.names.setFont(ui->StaffNamesBox->font());

    qDebug() << XMLLayout.Fonts.title.xml()->toString();

    XMLLayout.setName(ui->Name->text());
}

void CLayoutWizard::MoveRight()
{
    int i=0;
    while (i<ui->StaffListLeft->count())
    {
        if (ui->StaffListLeft->item(i)->isSelected())
        {
            ui->StaffListLeft->item(i)->setData(LWSquareBracket,0);
            ui->StaffListLeft->item(i)->setData(LWCurlyBracket,0);
            ui->StaffListRight->addItem(ui->StaffListLeft->takeItem(i));
        }
        else
        {
            i++;
        }
    }
    ValidateStaffs();
}

void CLayoutWizard::MoveLeft()
{
    int i=0;
    while (i<ui->StaffListRight->count())
    {
        if (ui->StaffListRight->item(i)->isSelected())
        {
            ui->StaffListRight->item(i)->setData(LWSquareBracket,0);
            ui->StaffListRight->item(i)->setData(LWCurlyBracket,0);
            ui->StaffListLeft->addItem(ui->StaffListRight->takeItem(i));
        }
        else
        {
            i++;
        }
    }
    ValidateStaffs();
}

void CLayoutWizard::MoveAllRight()
{
    int i=0;
    while (i<ui->StaffListLeft->count())
    {
        ui->StaffListLeft->item(i)->setData(LWSquareBracket,0);
        ui->StaffListLeft->item(i)->setData(LWCurlyBracket,0);
        ui->StaffListRight->addItem(ui->StaffListLeft->takeItem(i));
    }
    ValidateStaffs();
}

void CLayoutWizard::MoveAllLeft()
{
    int i=0;
    while (i<ui->StaffListRight->count())
    {
        ui->StaffListRight->item(i)->setData(LWSquareBracket,0);
        ui->StaffListRight->item(i)->setData(LWCurlyBracket,0);
        ui->StaffListLeft->addItem(ui->StaffListRight->takeItem(i));
    }
    ValidateStaffs();
}

void CLayoutWizard::FillBrackets()
{
    ui->BracketsTable->clear();
    ui->BracketsTable->setRowCount(ui->StaffListRight->count());
    this->ValidateBrackets();
    ui->BracketsTable->setColumnWidth(0,16);
    ui->BracketsTable->setColumnWidth(1,16);
    ui->BracketsTable->setColumnWidth(2,ui->BracketsTable->width()/2);
    ui->BracketsTable->setIconSize(QSize(16,16));
    for (int i=0; i<ui->StaffListRight->count(); i++)
    {
        //ui->BracketsTable->setRowHeight(i,24);
        auto item=new QTableWidgetItem(ui->StaffListRight->item(i)->text());
        auto item1=new QTableWidgetItem;
        if (ui->StaffListRight->item(i)->data(LWSquareBracket)==SBBegin)
        {
            QString IconPath=":/square1.png";
            if (i>0)
            {
                if (ui->StaffListRight->item(i-1)->data(LWSquareBracket)==SBBegin)
                {
                    IconPath=":/square2.png";
                }
            }
            item1->setIcon(QIcon(IconPath));
        }
        else if (ui->StaffListRight->item(i)->data(LWSquareBracket)==SBEnd)
        {
            item1->setIcon(QIcon(":/square3.png"));
        }
        auto item2=new QTableWidgetItem;
        if (ui->StaffListRight->item(i)->data(LWCurlyBracket)==CBBegin)
        {
            item2->setIcon(QIcon(":/curly1.png"));
        }
        if (i>0)
        {
            if (ui->StaffListRight->item(i-1)->data(LWCurlyBracket)==CBBegin)
            {
                item2->setIcon(QIcon(":/curly2.png"));
            }
        }
        auto sp=new QSlider(Qt::Horizontal,this);
        sp->setMinimum(-3);
        sp->setMaximum(0);
        sp->setValue(ui->StaffListRight->item(i)->data(LWSize).toInt()/4);
        sp->setToolTip(SliderTooltip(sp->value()));
        connect(sp,&QAbstractSlider::valueChanged,this,&CLayoutWizard::SizeChanged);
        ui->BracketsTable->setCellWidget(i,3,sp);

        ui->BracketsTable->setItem(i,1,item1);
        ui->BracketsTable->setItem(i,0,item2);
        ui->BracketsTable->setItem(i,2,item);
        ui->BracketsTable->setRowHeight(i,16);
    }
}

void CLayoutWizard::SizeChanged(int Size)
{
    Q_UNUSED(Size);
    for (int i=0;i<ui->BracketsTable->rowCount();i++)
    {
        auto sl=dynamic_cast<QSlider*>(ui->BracketsTable->cellWidget(i,3));
        ui->StaffListRight->item(i)->setData(LWSize,sl->value()*4);
        sl->setToolTip(SliderTooltip(sl->value()));
    }
}

QString CLayoutWizard::SliderTooltip(int Value)
{
    switch (Value)
    {
    case 0:
        return "Normal Size Staff";
    case -1:
        return "Slightly diminished Staff";
    case -2:
        return "Diminished Staff";
    case -3:
        return "Very diminished Staff";
    }
    return "Staff";
}

void CLayoutWizard::SpacingTooltip(int Value)
{
    int v=qRound((double(Value+8)/25.0)*9);
    QString retval;
    switch (v)
    {
    case 9:
        retval=QString("Extremely Wide");
        break;
    case 8:
        retval=QString("Very Wide");
        break;
    case 7:
        retval=QString("Wide");
        break;
    case 6:
        retval=QString("Quite Wide");
        break;
    case 5:
        retval=QString("Slightly Wide");
        break;
    case 4:
        retval=QString("Normal");
        break;
    case 3:
        retval=QString("Slightly Narrow");
        break;
    case 2:
        retval=QString("Quite Narrow");
        break;
    case 1:
        retval=QString("Very Narrow");
        break;
    case 0:
        retval=QString("Extremely Narrow");
        break;
    }
    retval+=" ("+QString::number(double(Value)/25.0)+")";
    ui->SpacingLabel->setText(retval);
}

void CLayoutWizard::AddSquare()
{
    int First=0;
    for (int i=ui->BracketsTable->rowCount()-1; i>=0; i--)
    {
        if (ui->BracketsTable->item(i,2)->isSelected())
        {
            if (First==0)
            {
                ui->StaffListRight->item(i)->setData(LWSquareBracket,SBEnd);
                First=i;
            }
            else
            {
                ui->StaffListRight->item(i)->setData(LWSquareBracket,SBBegin);
            }
        }
    }
    FillBrackets();
}

void CLayoutWizard::AddCurly()
{
    for (int i=0; i<ui->BracketsTable->rowCount(); i++)
    {
        if (ui->BracketsTable->item(i,2)->isSelected())
        {
            ui->StaffListRight->item(i)->setData(LWCurlyBracket,CBBegin);
            break;
        }
    }
    FillBrackets();
}

void CLayoutWizard::RemoveBracket()
{
    for (int i=0; i<ui->BracketsTable->rowCount(); i++)
    {
        if (ui->BracketsTable->item(i,2)->isSelected())
        {
            ui->StaffListRight->item(i)->setData(LWSquareBracket,0);
            ui->StaffListRight->item(i)->setData(LWCurlyBracket,0);
        }
    }
    FillBrackets();
}

void CLayoutWizard::ValidateStaffs()
{
    auto bb=findChild<QDialogButtonBox*>();
    QPushButton* pb = bb->button(QDialogButtonBox::Ok);
    pb->setEnabled(ui->StaffListRight->count()>0);
    ui->tabWidget->setTabEnabled(1,ui->StaffListRight->count()>1);
}

void CLayoutWizard::FillOptions(XMLLayoutOptionsWrapper &Options, XMLLayoutFontsWrapper &Fonts, XMLScoreWrapper &Score, const QString &LayoutName) {
    ui->MasterStaff->clear();
    for (int i=0;i<Score.NumOfStaffs();i++)
    {
        ui->MasterStaff->addItem(Score.StaffName(i));
    }

    ui->Type->clear();
    ui->Type->addItems(QStringList{"Large Part", "Part", "Score", "Pocket Score"});
    ui->ShowNames->clear();
    ui->ShowNames->addItems(QStringList{"Always", "When Staff Config Changes", "On First System Only"});

    ui->TitleBox->fill(Fonts.title.font(),Fonts.title.text());
    ui->SubtitleBox->fill(Fonts.subtitle.font(),Fonts.subtitle.text());
    ui->ComposerBox->fill(Fonts.composer.font(),Fonts.composer.text(),false,Qt::AlignRight | Qt::AlignVCenter);
    ui->StaffNamesBox->fill(Fonts.names.font(),"Sample Text",true,Qt::AlignLeft | Qt::AlignVCenter);

    ui->Type->setCurrentIndex(Options.scoreType()-1);
    ui->HideBarNumbers->setChecked(Options.hideBarNumbers());
    ui->BarNumberOffset->setValue(Options.barNumberOffset());
    ui->NoteSpace->setValue(Options.noteSpace()-16);
    ui->MasterStaff->setCurrentIndex(Options.masterStaff());
    ui->ShowLayoutName->setChecked(Options.showLayoutName());
    ui->ShowNames->setCurrentIndex(Options.showNamesSwitch()-1);
    ui->ShowAllStaffs->setChecked(Options.showAllOnFirstSystem());
    ui->ShowAsSound->setChecked(!Options.transposeInstruments());
    ui->topMargin->setMillimeters(Options.topMargin());
    ui->leftMargin->setMillimeters(Options.leftMargin());
    ui->rightMargin->setMillimeters(Options.rightMargin());
    ui->bottomMargin->setMillimeters(Options.bottomMargin());
    ui->frontPage->setChecked(Options.frontPage());

    Printer->setPageOrientation(QPageLayout::Orientation(Options.orientation()));
    //Printer->setPaperSize(QPrinter::PaperSize(Options.paperSize()));

    ui->ImportCombo->blockSignals(true);
    ui->ImportCombo->clear();
    for (int i = 0; i < Score.layoutCount(); i++) {
        if (Score.Layout(i).name() != LayoutName) ui->ImportCombo->addItem(Score.Layout(i).name());
    }
    ui->ImportCombo->blockSignals(false);
    UpdateMargins();
    SpacingTooltip(ui->NoteSpace->value());

    qDebug() << "Fill" << Fonts.title.xml()->toString();
}

void CLayoutWizard::ValidateBrackets()
{
    XMLTemplateWrapper XMLTemplate;
    for(int i=0;i<ui->StaffListRight->count();i++)
    {
        XMLTemplateStaffWrapper XMLTemplateStaff;//(T.appendChild("Staff"));
        XMLTemplateStaff.setSquareBracket(SquareBracketConstants(ui->StaffListRight->item(i)->data(LWSquareBracket).toInt()));
        XMLTemplateStaff.setCurlyBracket(CurlyBracketConstants(ui->StaffListRight->item(i)->data(LWCurlyBracket).toInt()));
        XMLTemplateStaff.setId(ui->StaffListRight->item(i)->data(LWStaffId).toInt());
        XMLTemplate.addChild(XMLTemplateStaff);
        //TS.setId(i);
    }
    XMLTemplate.validateBrackets();
    for(int i=0;i<ui->StaffListRight->count();i++)
    {
        ui->StaffListRight->item(i)->setData(LWSquareBracket,XMLTemplate.staff(i).squareBracket());
        ui->StaffListRight->item(i)->setData(LWCurlyBracket,XMLTemplate.staff(i).curlyBracket());
    }
}

void CLayoutWizard::TabChanged(int Index)
{
    if (Index==1)
    {
        FillBrackets();
    }
}

void CLayoutWizard::UpdateMargins()
{
    Printer->setFullPage(false);
    Printer->setPageMargins(QMarginsF(ui->leftMargin->Millimeters(),ui->topMargin->Millimeters(),ui->rightMargin->Millimeters(),ui->bottomMargin->Millimeters()),QPageLayout::Millimeter);
    QTransform m=ui->graphicsView->transform();
    m.reset();
    m.scale(0.3,0.3);
    ui->graphicsView->setTransform(m);
    S.clear();
    S.setBackgroundBrush(QBrush());
    QRect page = Printer->pageLayout().paintRectPixels(72); //Printer->resolution()
    QRect paper = Printer->pageLayout().fullRectPixels(72); //Printer->resolution()

    QLinearGradient lg(0,0,page.width(),page.height());
    lg.setColorAt(0,Qt::white);
    lg.setColorAt(1,Qt::gray);
    S.addRect(paper.translated(20,20),QPen(Qt::NoPen),QBrush(QColor(0,0,0,150)));
    S.addRect(paper,QPen(Qt::black),QBrush(Qt::white));
    S.addRect(page,QPen(Qt::NoPen),QBrush(lg));
    S.addLine(page.left(),paper.top(),page.left(),paper.bottom(),QPen(Qt::black));
    S.addLine(page.right(),paper.top(),page.right(),paper.bottom(),QPen(Qt::black));
    S.addLine(paper.left(),page.top(),paper.right(),page.top(),QPen(Qt::black));
    S.addLine(paper.left(),page.bottom(),paper.right(),page.bottom(),QPen(Qt::black));
    S.setSceneRect(paper);
    ui->graphicsView->viewport()->update();
    Printer->setFullPage(true);
    Printer->setPageMargins(QMarginsF(0,0,0,0));
    pageSetupButton->setGeometry(ui->graphicsView->width()-pageSetupButton->width(),ui->graphicsView->height()-pageSetupButton->height(),pageSetupButton->width(),pageSetupButton->height());
}

void CLayoutWizard::ShowPageSetup()
{
    QPageSetupDialog d(Printer,this);
    d.setWindowFlags(Qt::Sheet);
    d.setWindowModality(Qt::WindowModal);
    d.exec();
    UpdateMargins();
}

void CLayoutWizard::ImportLayout() {
    const QString name = ui->ImportCombo->currentText();
    for (int i = 0; i < m_Score.layoutCount(); i++) {
        if (m_Score.LayoutName(i) == name) {
            XMLLayoutWrapper l = m_Score.Layout(i);
            FillOptions(l.Options,l.Fonts,m_Score,ui->Name->text());
            break;
        }
    }
}

