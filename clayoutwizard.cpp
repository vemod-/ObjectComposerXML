#include "clayoutwizard.h"
#include "ui_clayoutwizard.h"
#include <QMessageBox>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QPrinter>
#include <QPageSetupDialog>
#include <QPrinterInfo>
#else
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPageSetupDialog>
#include <QtPrintSupport/QPrinterInfo>
#endif

CLayoutWizard::CLayoutWizard(QWidget *parent) :
        QDialog(parent,Qt::Sheet),
    ui(new Ui::CLayoutWizard)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);
    setVisible(false);
    TitleElement.SetFont(QFont("Times New Roman",24));
    SubtitleElement.SetFont(QFont("Times New Roman",18));
    ComposerElement.SetFont(QFont("Times New Roman",12));
    NamesElement.SetFont(QFont("Times New Roman",8));
    connect(ui->topMargin,SIGNAL(Changed()),this,SLOT(UpdateMargins()));
    connect(ui->leftMargin,SIGNAL(Changed()),this,SLOT(UpdateMargins()));
    connect(ui->rightMargin,SIGNAL(Changed()),this,SLOT(UpdateMargins()));
    connect(ui->bottomMargin,SIGNAL(Changed()),this,SLOT(UpdateMargins()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(UpdateMargins()));
    connect(ui->NoteSpace,SIGNAL(valueChanged(int)),this,SLOT(SpacingTooltip(int)));
    ui->graphicsView->setScene(&S);
    Printer=new QPrinter();
    pageSetupButton=new QToolButton(ui->graphicsView);
    pageSetupButton->setProperty("transparent",true);
    pageSetupButton->setIcon(QIcon(":/preferences.png"));
    pageSetupButton->setIconSize(QSize(32,32));
    pageSetupButton->setFixedSize(QSize(36,36));
    connect(pageSetupButton,SIGNAL(clicked()),this,SLOT(ShowPageSetup()));
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
    while (this->isVisible())
    {
        QApplication::processEvents();
    }
    return result();
}

void CLayoutWizard::Fill(XMLScoreWrapper& XMLScore, int Index)
{
    //m_XMLScore=XMLScore;
    QDomLiteElement* XMLLayout;
    ui->StaffListLeft->clear();
    ui->StaffListRight->clear();
    for (int i=0;i<XMLScore.NumOfStaffs();i++)
    {
        ui->MasterStaff->addItem(XMLScore.StaffName(i));
    }

    ui->Type->clear();
    ui->Type->addItems(QStringList() << "Large Part" << "Part" << "Score" << "Pocket Score");
    ui->ShowNames->clear();
    ui->ShowNames->addItems(QStringList() << "Always" << "When Staff Config Changes" << "On First System Only");

    for (int i=0;i<XMLScore.Template(0)->childCount();i++)
    {
        QListWidgetItem* item=new QListWidgetItem(XMLScore.StaffName(i));
        item->setData(LWAllTemplateIndex,i);
        item->setData(LWName,XMLScore.StaffName(i));
        item->setData(LWAbbreviation,XMLScore.StaffAbbreviation(i));
        ui->StaffListLeft->addItem(item);
    }
    QDialogButtonBox* bb=findChild<QDialogButtonBox*>();
    QPushButton* pb = bb->button(QDialogButtonBox::Ok);
    if (Index>-1)
    {
        pb->setText("Apply Changes");
        XMLLayout=XMLScore.Layout(Index);
        QDomLiteElement* LayoutTemplate=XMLScore.LayoutTemplate(Index);
        for (int i=0;i<LayoutTemplate->childCount();i++)
        {
            QDomLiteElement* s=LayoutTemplate->childElement(i);
            int ati=XMLScore.AllTemplateIndex(s);
            ui->StaffListLeft->item(ati)->setSelected(true);
            QListWidgetItem* item=new QListWidgetItem(ui->StaffListLeft->item(ati)->text());
            item->setData(LWAllTemplateIndex,ati);
            item->setData(LWSquareBracket,s->attributeValue("SquareBracket"));
            item->setData(LWCurlyBracket,s->attributeValue("CurlyBracket"));
            item->setData(LWName,XMLScore.StaffName(s));
            item->setData(LWAbbreviation,XMLScore.StaffAbbreviation(s));
            item->setData(LWSize,s->attributeValue("Size"));
            ui->StaffListRight->addItem(item);
        }
        for (int i=ui->StaffListLeft->count()-1;i>=0;i--)
        {
            if (ui->StaffListLeft->item(i)->isSelected()) ui->StaffListLeft->takeItem(i);
        }
        ui->Name->setText(XMLScore.LayoutName(XMLLayout));
        QDomLiteElement* XMLOptions=XMLLayout->elementByTag("Options");

        switch ((int)(XMLOptions->attributeValue("ScoreType")*10))
        {
        case 10:
            ui->Type->setCurrentIndex(0);
            break;
        case 15:
            ui->Type->setCurrentIndex(1);
            break;
        case 20:
            ui->Type->setCurrentIndex(2);
            break;
        case 40:
            ui->Type->setCurrentIndex(3);
            break;
        }

        ui->Type->setCurrentIndex(XMLOptions->attributeValue("ScoreType")-1);
        ui->HideBarNumbers->setChecked(XMLOptions->attributeValue("DontShowBN"));
        ui->BarNumberOffset->setValue(XMLOptions->attributeValue("BarNrOffset"));
        ui->NoteSpace->setValue(XMLOptions->attributeValue("NoteSpace"));
        ui->MasterStaff->setCurrentIndex(XMLOptions->attributeValue("MasterStave"));
        ui->ShowNames->setCurrentIndex(XMLOptions->attributeValue("ShowNamesSwitch")-1);
        ui->ShowAllStaffs->setChecked(XMLOptions->attributeValue("ShowAllOnSys1"));
        ui->ShowAsSound->setChecked(!XMLOptions->attributeValue("TransposeInstruments"));
        int temp=XMLOptions->attributeValue("TopMargin");
        if (temp==0) temp=20;
        ui->topMargin->setMillimeters(temp);
        temp=XMLOptions->attributeValue("LeftMargin");
        if (temp==0) temp=15;
        ui->leftMargin->setMillimeters(temp);
        temp=XMLOptions->attributeValue("RightMargin");
        if (temp==0) temp=15;
        ui->rightMargin->setMillimeters(temp);
        temp=XMLOptions->attributeValue("BottomMargin");
        if (temp==0) temp=25;
        ui->bottomMargin->setMillimeters(temp);
        /*
        ui->topMargin->setMillimeters(XMLOptions->attributeValue("TopMargin"));
        ui->leftMargin->setMillimeters(XMLOptions->attributeValue("LeftMargin"));
        ui->rightMargin->setMillimeters(XMLOptions->attributeValue("RightMargin"));
        ui->bottomMargin->setMillimeters(XMLOptions->attributeValue("BottomMargin"));
        */
        Printer->setOrientation((QPrinter::Orientation)XMLOptions->attributeValue("Orientation"));
        Printer->setPaperSize((QPrinter::PaperSize)XMLOptions->attributeValue("PaperSize"));

        QDomLiteElement* XMLTitles=XMLLayout->elementByTag("Titles");
        TitleElement.Load(XMLTitles->elementByTag("Title"));
        SubtitleElement.Load(XMLTitles->elementByTag("Subtitle"));
        ComposerElement.Load(XMLTitles->elementByTag("Composer"));
        NamesElement.Load(XMLTitles->elementByTag("Names"));
        //QLineEdit* Title=findChild<QLineEdit*>("Title");
        //QLineEdit* Subtitle=findChild<QLineEdit*>("Subtitle");
        //QLineEdit* Composer=findChild<QLineEdit*>("Composer");
        //Title->setText(TitleElement.Text);
        //Subtitle->setText(SubtitleElement.Text);
        //Composer->setText(ComposerElement.Text);
    }
    else
    {
        pb->setText("Create Layout");
        ui->Name->setText("New Layout");
        if (XMLScore.NumOfLayouts())
        {
            int MatchCount=1;
            forever
            {
                bool Match=false;
                for (int i=0; i<XMLScore.NumOfLayouts(); i++)
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

        ui->Type->setCurrentIndex(0);
        ui->ShowNames->setCurrentIndex(2);
        ui->ShowAllStaffs->setChecked(true);
        ui->HideBarNumbers->setChecked(XMLScore.getVal("DontShowBN"));
        ui->BarNumberOffset->setValue(XMLScore.getVal("BarNrOffset"));
        ui->NoteSpace->setValue(XMLScore.getVal("NoteSpace")-16);
        ui->MasterStaff->setCurrentIndex(XMLScore.getVal("MasterStave"));
        ui->topMargin->setMillimeters(20);
        ui->leftMargin->setMillimeters(15);
        ui->rightMargin->setMillimeters(15);
        ui->bottomMargin->setMillimeters(25);
        MoveAllRight();
    }
    ui->TitleBox->Fill(TitleElement.Font(),TitleElement.Text);
    ui->SubtitleBox->Fill(SubtitleElement.Font(),SubtitleElement.Text);
    ui->ComposerBox->Fill(ComposerElement.Font(),ComposerElement.Text,false,Qt::AlignRight | Qt::AlignVCenter);
    ui->StaffNamesBox->Fill(NamesElement.Font(),"Sample Text",true,Qt::AlignLeft | Qt::AlignVCenter);
    ValidateStaffs();
    UpdateMargins();
    SpacingTooltip(ui->NoteSpace->value());
}

void CLayoutWizard::ModifyXML(XMLScoreWrapper& XMLScore, int Index )
{
    QDomLiteElement* LayoutCollection;
    if (!XMLScore.LayoutCollectionExists())
    {
        //LayoutCollection = XMLNewChild("LayoutCollection",XMLScore->documentElement);
        LayoutCollection=XMLScore.getXML()->documentElement->appendChild("LayoutCollection");
    }
    else
    {
        LayoutCollection=XMLScore.LayoutCollection();
    }
    QDomLiteElement* XMLTemplate=new QDomLiteElement("Template");
    for (int i=0; i<ui->StaffListRight->count(); i++)
    {
        QDomLiteElement* XMLTemplateStaff=XMLTemplate->appendChild("TemplateStaff");
        XMLTemplateStaff->setAttribute("AllTemplateIndex",ui->StaffListRight->item(i)->data(LWAllTemplateIndex));
        XMLTemplateStaff->setAttribute("CurlyBracket",ui->StaffListRight->item(i)->data(LWCurlyBracket));
        XMLTemplateStaff->setAttribute("SquareBracket",ui->StaffListRight->item(i)->data(LWSquareBracket));
        XMLTemplateStaff->setAttribute("Name",ui->StaffListRight->item(i)->data(LWName));
        XMLTemplateStaff->setAttribute("Abbreviation",ui->StaffListRight->item(i)->data(LWAbbreviation));
        XMLTemplateStaff->setAttribute("Size",ui->StaffListRight->item(i)->data(LWSize));
        XMLTemplateStaff->setAttribute("Index",i);
    }
    QDomLiteElement* XMLOptions=new QDomLiteElement("Options");
    XMLOptions->setAttribute("ShowNamesSwitch",ui->ShowNames->currentIndex()+1);
    XMLOptions->setAttribute("ShowAllOnSys1",ui->ShowAllStaffs->isChecked());
    XMLOptions->setAttribute("TransposeInstruments",!ui->ShowAsSound->isChecked());
    XMLOptions->setAttribute("DontShowBN",ui->HideBarNumbers->isChecked());
    XMLOptions->setAttribute("BarNrOffset",ui->BarNumberOffset->value());
    XMLOptions->setAttribute("NoteSpace",ui->NoteSpace->value());
    XMLOptions->setAttribute("MasterStave",ui->MasterStaff->currentIndex());
    XMLOptions->setAttribute("TopMargin",ui->topMargin->Millimeters());
    XMLOptions->setAttribute("LeftMargin",ui->leftMargin->Millimeters());
    XMLOptions->setAttribute("RightMargin",ui->rightMargin->Millimeters());
    XMLOptions->setAttribute("BottomMargin",ui->bottomMargin->Millimeters());
    XMLOptions->setAttribute("Orientation",(int)Printer->orientation());
    XMLOptions->setAttribute("PaperSize",(int)Printer->paperSize());

    switch (ui->Type->currentIndex())
    {
    case 0:
        XMLOptions->setAttribute("ScoreType",1);
        break;
    case 1:
        XMLOptions->setAttribute("ScoreType",1.5);
        break;
    case 2:
        XMLOptions->setAttribute("ScoreType",2);
        break;
    case 3:
        XMLOptions->setAttribute("ScoreType",4);
        break;
    }

    XMLOptions->setAttribute("ScoreType",ui->Type->currentIndex()+1);

    QDomLiteElement* XMLTitles=new QDomLiteElement("Titles");

    TitleElement.Text=ui->TitleBox->Text();
    TitleElement.SetFont(ui->TitleBox->Font());
    SubtitleElement.Text=ui->SubtitleBox->Text();
    SubtitleElement.SetFont(ui->SubtitleBox->Font());
    ComposerElement.Text=ui->ComposerBox->Text();
    ComposerElement.SetFont(ui->ComposerBox->Font());
    NamesElement.SetFont(ui->StaffNamesBox->Font());

    TitleElement.Save(XMLTitles->appendChild("Title"));
    SubtitleElement.Save(XMLTitles->appendChild("Subtitle"));
    ComposerElement.Save(XMLTitles->appendChild("Composer"));
    NamesElement.Save(XMLTitles->appendChild("Names"));

    QDomLiteElement* XMLLayout;
    if (Index>-1)
    {
        QString Msg;
        XMLLayout=LayoutCollection->childElement(Index);
        QDomLiteElement* oldTemplate=XMLLayout->elementByTag("Template");
        if (XMLLayout->elementByTag("Options")->attribute("Orientation") != XMLOptions->attribute("Orientation"))
        {
            Msg+="The Printer Orientation has Changed\n";
        }
        if (XMLLayout->elementByTag("Options")->attribute("PaperSize") != XMLOptions->attribute("PaperSize"))
        {
            Msg+="The Paper Size has Changed\n";
        }
        if (XMLLayout->elementByTag("Options")->attribute("NoteSpace") != XMLOptions->attribute("NoteSpace"))
        {
            Msg+="The Note Spacing has Changed\n";
        }
        if (XMLLayout->elementByTag("Options")->attribute("ScoreType") != XMLOptions->attribute("ScoreType"))
        {
            Msg+="The ScoreType has Changed\n";
        }
        if (oldTemplate->childCount() != XMLTemplate->childCount())
        {
            Msg+="The Number off Staffs has Changed\n";
        }
        else
        {
            bool Match=true;
            for (int i=0;i<XMLTemplate->childCount();i++)
            {
                if (XMLScore.AllTemplateIndex(XMLTemplate,i) != XMLScore.AllTemplateIndex(oldTemplate,i))
                {
                    Match=false;
                    break;
                }
            }
            if (!Match) Msg+="The Staff Setup has Changed\n";
        }
        if (Msg.length())
        {
            int ret=QMessageBox::warning(this, "Object Composer XML",Msg+"The Layout will be Reformatted, Continue ?",QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
            if (ret==QMessageBox::Cancel)
            {
                return;
            }
            XMLLayout=LayoutCollection->replaceChild(Index, "Layout");
        }
    }
    else
    {
        XMLLayout=LayoutCollection->appendChild("Layout");
    }
    if (XMLLayout->childCount("Template")==0)
    {
        XMLLayout->appendChild(XMLTemplate);
        XMLLayout->appendChild(XMLOptions);
        XMLLayout->appendChild(XMLTitles);
    }
    else
    {
        XMLLayout->replaceChild(XMLLayout->elementByTag("Template"), XMLTemplate);
        XMLLayout->replaceChild(XMLLayout->elementByTag("Options"), XMLOptions);
        XMLLayout->replaceChild(XMLLayout->elementByTag("Titles"), XMLTitles);
    }
    XMLLayout->setAttribute("Name",ui->Name->text());
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
        QTableWidgetItem* item=new QTableWidgetItem(ui->StaffListRight->item(i)->text());
        QTableWidgetItem* item1=new QTableWidgetItem;
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
        QTableWidgetItem* item2=new QTableWidgetItem;
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
        QSlider* sp=new QSlider(Qt::Horizontal,this);
        sp->setMinimum(-3);
        sp->setMaximum(0);
        sp->setValue(ui->StaffListRight->item(i)->data(LWSize).toInt()/4);
        sp->setToolTip(SliderTooltip(sp->value()));
        connect(sp,SIGNAL(valueChanged(int)),this,SLOT(SizeChanged(int)));
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
        QSlider* sl=(QSlider*)ui->BracketsTable->cellWidget(i,3);
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
    int v=qRound(((float)(Value+8)/25.0)*9);
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
    retval+=" ("+QString::number((float)Value/25.0)+")";
    //ui->NoteSpace->setToolTip(retval);
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
    QDialogButtonBox* bb=findChild<QDialogButtonBox*>();
    QPushButton* pb = bb->button(QDialogButtonBox::Ok);
    pb->setEnabled(ui->StaffListRight->count()>0);
    ui->tabWidget->setTabEnabled(1,ui->StaffListRight->count()>1);
}

void CLayoutWizard::ValidateBrackets()
{
    QDomLiteElement T("Template");
    for(int i=0;i<ui->StaffListRight->count();i++)
    {
        QDomLiteElement* TS=T.appendChild("Staff");
        TS->setAttribute("SquareBracket",ui->StaffListRight->item(i)->data(LWSquareBracket));
        TS->setAttribute("CurlyBracket",ui->StaffListRight->item(i)->data(LWCurlyBracket));
        TS->setAttribute("AllTemplateIndex",i);
    }
    XMLScoreWrapper::ValidateBrackets(&T);
    for(int i=0;i<ui->StaffListRight->count();i++)
    {
        QDomLiteElement* TS=T.childElement(i);
        ui->StaffListRight->item(i)->setData(LWSquareBracket,TS->attributeValue("SquareBracket"));
        ui->StaffListRight->item(i)->setData(LWCurlyBracket,TS->attributeValue("CurlyBracket"));
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
    Printer->setPageMargins(ui->leftMargin->Millimeters(),ui->topMargin->Millimeters(),ui->rightMargin->Millimeters(),ui->bottomMargin->Millimeters(),QPrinter::Millimeter);
    QMatrix m=ui->graphicsView->matrix();
    m.reset();
    m.scale(0.3,0.3);
    ui->graphicsView->setMatrix(m);
    S.clear();
    S.setBackgroundBrush(QBrush());
    QLinearGradient lg(0,0,Printer->pageRect().width(),Printer->pageRect().height());
    lg.setColorAt(0,Qt::white);
    lg.setColorAt(1,Qt::gray);
    S.addRect(Printer->paperRect().translated(20,20),QPen(Qt::NoPen),QBrush(QColor(0,0,0,150)));
    S.addRect(Printer->paperRect(),QPen(Qt::black),QBrush(Qt::white));
    S.addRect(Printer->pageRect(),QPen(Qt::NoPen),QBrush(lg));
    S.addLine(Printer->pageRect().left(),Printer->paperRect().top(),Printer->pageRect().left(),Printer->paperRect().bottom(),QPen(Qt::black));
    S.addLine(Printer->pageRect().right(),Printer->paperRect().top(),Printer->pageRect().right(),Printer->paperRect().bottom(),QPen(Qt::black));
    S.addLine(Printer->paperRect().left(),Printer->pageRect().top(),Printer->paperRect().right(),Printer->pageRect().top(),QPen(Qt::black));
    S.addLine(Printer->paperRect().left(),Printer->pageRect().bottom(),Printer->paperRect().right(),Printer->pageRect().bottom(),QPen(Qt::black));
    S.setSceneRect(Printer->paperRect());
    ui->graphicsView->viewport()->update();
    Printer->setFullPage(true);
    Printer->setPageMargins(0,0,0,0,QPrinter::DevicePixel);
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

