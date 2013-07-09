#include "clayout.h"
#include "layoutviewxml.h"

CLayoutFonts::CLayoutFonts()
{
    Title.FontName = "Times New Roman";
    Title.Size = 18;
    SubTitle.FontName = "Times New Roman";
    SubTitle.Size = 12;
    Composer.FontName = "Times New Roman";
    Composer.Size = 8;
    Names.FontName = "Times New Roman";
    Names.Size = 8;
}

QDomLiteElement* CLayoutFonts::Save(const QString& Tag)
{
    QDomLiteElement* data=new QDomLiteElement(Tag);
    QDomLiteElement* XMLTitle=data->appendChild("Title");
    QDomLiteElement* XMLSubTitle=data->appendChild("Subtitle");
    QDomLiteElement* XMLComposer=data->appendChild("Composer");
    QDomLiteElement* XMLNames=data->appendChild("Names");
    Title.Save(XMLTitle);
    SubTitle.Save(XMLSubTitle);
    Composer.Save(XMLComposer);
    Names.Save(XMLNames);
    return data;
}

void CLayoutFonts::Load(QDomLiteElement* data)
{
    Title.Load(data->elementByTag("Title"));
    SubTitle.Load(data->elementByTag("Subtitle"));
    Composer.Load(data->elementByTag("Composer"));
    Names.Load(data->elementByTag("Names"));
}

CLayoutOptions::CLayoutOptions()
{
    ShowNamesSwitch=0;
    ShowAllOnSys1=false;
    TransposeInstruments=false;
    ScoreType=1;
    ScaleSize=1;
    DontShowBN=false;
    BarNrOffset=0;
    NoteSpace=0;
    MasterStave=0;
    TopMargin=20;
    LeftMargin=15;
    RightMargin=15;
    BottomMargin=25;
    Orientation=0;
    PaperSize=0;
}

QDomLiteElement* CLayoutOptions::Save(const QString &Tag)
{
    QDomLiteElement* data=new QDomLiteElement(Tag);
    data->setAttribute("ShowNamesSwitch", ShowNamesSwitch);
    data->setAttribute("ShowAllOnSys1", ShowAllOnSys1);
    data->setAttribute("TransposeInstruments", TransposeInstruments);
    data->setAttribute("ScoreType", ScoreType);
    data->setAttribute("ScaleSize", ScaleSize);
    data->setAttribute("DontShowBN", DontShowBN);
    data->setAttribute("BarNrOffset", BarNrOffset);
    data->setAttribute("NoteSpace", NoteSpace);
    data->setAttribute("MasterStave", MasterStave);
    data->setAttribute("TopMargin", TopMargin);
    data->setAttribute("LeftMargin", LeftMargin);
    data->setAttribute("RightMargin", RightMargin);
    data->setAttribute("BottomMargin", BottomMargin);
    data->setAttribute("Orientation", Orientation);
    data->setAttribute("PaperSize", PaperSize);
    return data;
}

void CLayoutOptions::Load(QDomLiteElement* data)
{
    ShowNamesSwitch = data->attributeValue("ShowNamesSwitch");
    ShowAllOnSys1 = data->attributeValue("ShowAllOnSys1");
    TransposeInstruments = data->attributeValue("TransposeInstruments");
    ScoreType = data->attributeValue("ScoreType");
    ScaleSize = data->attributeValue("ScaleSize");
    DontShowBN = data->attributeValue("DontShowBN");
    BarNrOffset = data->attributeValue("BarNrOffset");
    NoteSpace = data->attributeValue("NoteSpace");
    MasterStave = data->attributeValue("MasterStave");
    TopMargin = data->attributeValue("TopMargin");
    LeftMargin = data->attributeValue("LeftMargin");
    RightMargin = data->attributeValue("RightMargin");
    BottomMargin = data->attributeValue("BottomMargin");
    Orientation = data->attributeValue("Orientation");
    PaperSize = data->attributeValue("PaperSize");

    if (TopMargin==0) TopMargin=20;
    if (LeftMargin==0) LeftMargin=15;
    if (RightMargin==0) RightMargin=15;
    if (BottomMargin==0) BottomMargin=25;
}

CLayoutSystem::CLayoutSystem()
{
    Template.clear("Template");
    StartBar=0;
    EndBar=0;
    PageNr=0;
    ShowNames=0;
    UpDown=0;
    Top=0;
    Distance=0;
    Height=0;
    Syslen=0;
    SystemList.clear();
}

CLayoutSystem::~CLayoutSystem()
{
}

void CLayoutSystem::Erase(QGraphicsScene* Scene)
{
    foreach(QGraphicsItem* item,SystemList)
    {
        if (Scene->items().contains(item))
        {
            Scene->removeItem(item);
            delete item;
        }
    }
    SystemList.clear();
}

void CLayoutSystem::plot(OCScore& Score, XMLScoreWrapper& XMLScore, CLayoutFonts& Fonts, const int Viewsize, const int Left, const int LayTop, CLayoutOptions& Options, OCDraw& ScreenObj, const float PrinterResFactor)
{
    int HoldSysLen = Score.SystemLength();
    int NameLen = 0;
    SystemList.clear();
    ScreenObj.StartList();
    float HoldSize=Fonts.Names.Size;
    Fonts.Names.Size=Fonts.Names.Size/(PrinterResFactor*1.0);
    if (ShowNames > 0)
    {
        for (int c = 0; c<Template.childCount();c++)
        {
            QString Text;
            switch (ShowNames)
            {
            case 1:
                Text = XMLScore.StaffAbbreviation(&Template,c).trimmed();
                break;
            case 2:
                Text = XMLScore.StaffName(&Template,c).trimmed();
                break;
            }
            float TopAdjust=(ScoreStaffLinesHeight-(ScreenObj.TextHeight(&Fonts.Names,Viewsize)*Viewsize*Options.ScaleSize))/2;
            ScreenObj.PrintTextElement(Left/Options.ScaleSize,(XMLScore.StaffPos(&Template,c) + Top + TopAdjust + LayTop)/Options.ScaleSize,Text,&Fonts.Names,Viewsize);
            if ((ScreenObj.TextWidth(Text,&Fonts.Names,Viewsize)*Viewsize) > NameLen) NameLen = (ScreenObj.TextWidth(Text,&Fonts.Names,Viewsize)*Viewsize);
        }
        if (NameLen > 0) NameLen = (NameLen  + 50)*Options.ScaleSize;
        HoldSysLen -= NameLen;
        Score.ReformatPage(HoldSysLen);
    }
    Fonts.Names.Size=HoldSize;
    for (int c = 0; c<Template.childCount();c++)
    {
        ScreenObj.SetXY(Left + NameLen,XMLScore.StaffPos(&Template,c) + Top + LayTop);
        QDomLiteElement* TemplateStaff = XMLScore.TemplateStaff(&Template, c);
        Score.PlSystemNoList(XMLScore.AllTemplateIndex(TemplateStaff), XMLScore, &Template, Qt::black, 0, 0, c, ScreenObj);
    }
    Syslen = HoldSysLen;
    SystemList.append(ScreenObj.EndList());
    foreach(QGraphicsItem* item,SystemList) item->setZValue(2);
}

void CLayoutSystem::Format(OCScore& Score, XMLScoreWrapper& XMLScore, QDomLiteElement* LayoutTemplate, const int m_StartBar, const bool Auto, const bool ShowAllStaves, const int ShowNamesOption, const int SystemLength)
{
    if (Auto)
    {
        EndBar = 0;
        StartBar = m_StartBar;
    }
    Score.FormatPage(XMLScore, LayoutTemplate, SystemLength, StartBar, EndBar);
    if (Auto)
    {
        Template.clearChildren();
        if (ShowNamesOption == 1) ShowNames = 1;
        for (int c = 0; c<LayoutTemplate->childCount(); c++)
        {
            bool StaffEmpty=Score.StaffEmpty(c,XMLScore,LayoutTemplate);
            if (StaffEmpty && (ShowNamesOption == 2)) ShowNames = 1;
            if (!StaffEmpty || ShowAllStaves)
            {
                Template.appendClone(XMLScore.TemplateStaff(LayoutTemplate,c));
            }
        }
        if (Template.childCount()==0)
        {
            Template.appendClone(XMLScore.TemplateStaff(LayoutTemplate,0));
        }
        XMLScore.ValidateBrackets(&Template);
        EndBar = StartBar + Score.ActuallyPrinted();

        SetHeight();// 'Height = Template.staveyins(Template.NumOfStaves)
    }
    else
    {
        for (int c=0;c<Template.childCount();c++)
        {
            int index=XMLScore.AllTemplateIndex(&Template,c);
            QDomLiteElement* TemplateStaff=XMLScore.TemplateOrderStaff(LayoutTemplate,index);
            Template.childElement(c)->setAttribute("SquareBracket",TemplateStaff->attribute("SquareBracket"));
            Template.childElement(c)->setAttribute("CurlyBracket",TemplateStaff->attribute("CurlyBracket"));
            XMLScore.ValidateBrackets(&Template);
        }
    }
}

void CLayoutSystem::Load(QDomLiteElement* data)
{
    StartBar = data->attributeValue("StartBar");
    EndBar = data->attributeValue("EndBar");
    PageNr = data->attributeValue("PageNumber");
    ShowNames = data->attributeValue("NamesVisible");
    UpDown = data->attributeValue("UpDown");
    Top = data->attributeValue("Top");
    DefaultTop=data->attributeValue("DefaultTop");
    Distance = data->attributeValue("Distance");
    Height = data->attributeValue("Height");
    Syslen = data->attributeValue("SystemLength");
    Template.copy(data->elementByTag("Template"));
}

QDomLiteElement* CLayoutSystem::Save(const QString &Tag)
{
    QDomLiteElement* data=new QDomLiteElement(Tag);
    data->setAttribute("StartBar", StartBar);
    data->setAttribute("EndBar", EndBar);
    data->setAttribute("PageNumber", PageNr);
    data->setAttribute("NamesVisible", ShowNames);
    data->setAttribute("UpDown", UpDown);
    data->setAttribute("Top", Top);
    data->setAttribute("DefaultTop", DefaultTop);
    data->setAttribute("Distance", Distance);
    data->setAttribute("Height", Height);
    data->setAttribute("SystemLength", Syslen);
    data->appendChild(Template.clone());
    return data;
}

void CLayoutSystem::SetStaveDistance(const int stavedistance)
{
    for (int iTemp = 0; iTemp<Template.childCount();iTemp++)
    {
        XMLScoreWrapper::TemplateStaff(&Template,iTemp)->setAttribute("Height",stavedistance);
    }
}

void CLayoutSystem::SetHeight()
{
    Height = XMLScoreWrapper::StaffPos(&Template,Template.childCount()) - 700;
}

CLayoutPage::CLayoutPage()
{
    TitleHeight=0;
}

CLayoutPage::~CLayoutPage()
{
    qDeleteAll(Systems);
    Systems.clear();
    TitleList.clear();
}

const int CLayoutPage::NumOfSystems() const
{
    return Systems.count();
}

void CLayoutPage::PlotSystem(const int System, OCScore& Score, XMLScoreWrapper& XMLScore, CLayoutFonts& Fonts, const int Viewsize, const int Left, const int Top, CLayoutOptions& Options, OCDraw& ScreenObj, const float PrinterResFactor)
{
    Systems[System]->plot(Score,XMLScore,Fonts,Viewsize,Left,Top,Options,ScreenObj, PrinterResFactor);
}

void CLayoutPage::Format(const int System, OCScore& Score, XMLScoreWrapper& XMLScore, QDomLiteElement* LayoutTemplate, int& StartBar, const bool Auto, const bool ShowAllStaves, const int ShowNamesOption, const int SystemLength)
{
    Systems[System]->Format(Score, XMLScore, LayoutTemplate, StartBar, Auto, ShowAllStaves, ShowNamesOption, SystemLength);
    if (Auto)
    {
        StartBar = Systems[System]->EndBar;
        Systems[System]->Top = YSpaceUsed(System-1);
        Systems[System]->DefaultTop=Systems[System]->Top;
    }
}

void CLayoutPage::Load(QDomLiteElement* data)
{
    qDeleteAll(Systems);
    Systems.clear();
    int SystemCount = data->attributeValue("SystemCount");
    for (int i=0;i<SystemCount;i++)
    {
        AddSystem();
        Systems[i]->Load(data->elementsByTag("System")[i]);
    }
}

QDomLiteElement* CLayoutPage::Save(const QString &Tag)
{
    QDomLiteElement* data=new QDomLiteElement(Tag);
    data->setAttribute("SystemCount", Systems.count());
    for (int i=0;i<Systems.count();i++)
    {
        data->appendChild(Systems[i]->Save("System"));
    }
    return data;
}

void CLayoutPage::AddSystem()
{
    Systems.append(new CLayoutSystem);
}

CLayoutSystem* CLayoutPage::Sys(const int Index)
{
    return Systems[Index];
}

CLayoutSystem* CLayoutPage::GetSystem(const bool Top)
{
    if (Top) return Systems.takeFirst();
    return Systems.takeLast();
}

void CLayoutPage::InsertSystem(const bool Top, CLayoutSystem *System)
{
    if (Top)
    {
        Systems.prepend(System);
    }
    else
    {
        Systems.append(System);
    }
}

void CLayoutPage::RemoveSystem(const bool Top)
{
    int i=0;
    if (!Top) i=Systems.count()-1;
    if (i<Systems.count())
    {
        delete Systems[i];
        Systems.removeAt(i);
    }
}

const int CLayoutPage::YSpaceUsed(const int CurrentSystem) const
{
    int RetVal = TitleHeight;
    if (CurrentSystem > -1)
    {
        RetVal = Systems[CurrentSystem]->Top + Systems[CurrentSystem]->Height;
    }
    return RetVal;
}

void CLayoutPage::ClearAllSystems()
{
    qDeleteAll(Systems);
    Systems.clear();
}

const int CLayoutPage::CurrentHeight(const int CurrentSystem) const
{
    return Systems[CurrentSystem]->Height;
}

void CLayoutPage::setActiveSystem(const int System)
{
    ActiveSystem = Systems[qMin(Systems.count()-1,System)];
}

const int CLayoutPage::getActiveSystem() const
{
    if (Systems.isEmpty()) return 0;
    return Systems.indexOf(ActiveSystem);
}

const int CLayoutPage::SysTop(const int System) const
{
    return Systems[System]->Top;
}

const int CLayoutPage::SysHeight(const int System) const
{
    return Systems[System]->Height;
}

const int CLayoutPage::StartBar(const int System) const
{
    return Systems[System]->StartBar;
}

void CLayoutPage::AddBar(const int System, const bool Top)
{
    if (Top)
    {
        Systems[System]->StartBar--;
    }
    else
    {
        Systems[System]->EndBar++;
    }
}

const bool CLayoutPage::RemoveBar(const int System, const bool Top)
{
    if (Systems[System]->StartBar + 1 == Systems[System]->EndBar) return false;
    if (Top)
    {
        Systems[System]->StartBar++;
    }
    else
    {
        Systems[System]->EndBar--;
    }
    return true;
}

void CLayoutPage::AdjustSystems(const int PageHeight)
{
    int NumOfStaffs=0;
    int YSpaceUsed = TitleHeight;
    for (int iTemp = 0; iTemp<Systems.count(); iTemp++)
    {
        Systems[iTemp]->SetStaveDistance(0);
        Systems[iTemp]->Top = YSpaceUsed;
        Systems[iTemp]->DefaultTop=YSpaceUsed;
        Systems[iTemp]->SetHeight();
        YSpaceUsed += Systems[iTemp]->Height;
        NumOfStaffs += Systems[iTemp]->Template.childCount();
    }
    int YSpaceAvailable = PageHeight - TitleHeight;
    int stavedistance=FloatDiv(YSpaceAvailable-((ScoreStaffHeight*NumOfStaffs)+(240*(Systems.count()-1))),NumOfStaffs)/12;
    YSpaceUsed = TitleHeight;
    for (int iTemp = 0; iTemp<Systems.count(); iTemp++)
    {
        Systems[iTemp]->SetStaveDistance(stavedistance);
        Systems[iTemp]->Top = YSpaceUsed;
        Systems[iTemp]->DefaultTop=YSpaceUsed;
        Systems[iTemp]->SetHeight();
        YSpaceUsed += Systems[iTemp]->Height;
    }
}

void CLayoutPage::FormatTitle()
{
    int YSpaceUsed = 0;
    for (int iTemp = 0; iTemp<Systems.count(); iTemp++)
    {
        Systems[iTemp]->Top = YSpaceUsed;
        Systems[iTemp]->DefaultTop=YSpaceUsed;
        YSpaceUsed += Systems[iTemp]->Height;
    }
    for (int iTemp = 0; iTemp<Systems.count(); iTemp++)
    {
        Systems[iTemp]->Top += TitleHeight;
        Systems[iTemp]->DefaultTop=Systems[iTemp]->Top;
    }
}

void CLayoutPage::Erase(const int System, QGraphicsScene *Scene)
{
    if (System<Systems.count()) Systems[System]->Erase(Scene);
}

void CLayoutPage::EraseTitle(QGraphicsScene* Scene)
{
    foreach(QGraphicsItem* item,TitleList)
    {
        if (Scene->items().contains(item))
        {
            Scene->removeItem(item);
            delete item;
        }
    }
    TitleList.clear();
}

void CLayoutPage::PlotTitle(const int Page, CLayoutFonts& Fonts, const int Viewsize, const int PageWidth, const int LayTop, CLayoutOptions& Options, const int LayRight, OCDraw& ScreenObj, const float PrinterResFactor)
{
    TitleList.clear();
    ScreenObj.StartList();
    ScreenObj.col = Qt::black;
    TitleHeight = 0;
    if (Page > 0) return;
    if (Fonts.Title.Size > 0)
    {
        float Tmp = Fonts.Title.Size;
        Fonts.Title.Size = Fonts.Title.Size / (PrinterResFactor*1.0);
        int Halfwidth = ScreenObj.TextWidth(&Fonts.Title, Viewsize) / 2;  //' Calculate one-half width.
        int TextHeight = (0.9 * ((ScreenObj.TextHeight(&Fonts.Title, Viewsize) * Viewsize) * Options.ScaleSize));
        if (Fonts.Title.Text.trimmed().length())
        {
            TitleHeight += TextHeight;
            ScreenObj.PrintTextElement((PageWidth  / 2) - (Halfwidth*Viewsize), LayTop/Options.ScaleSize, &Fonts.Title, Viewsize);
        }
        Fonts.Title.Size = Tmp;
    }
    if (Fonts.SubTitle.Size > 0)
    {
        float Tmp = Fonts.SubTitle.Size;
        Fonts.SubTitle.Size = Fonts.SubTitle.Size / (PrinterResFactor*1.0);
        int Halfwidth = ScreenObj.TextWidth(&Fonts.SubTitle, Viewsize) / 2; // ' Calculate one-half width.
        int TextHeight = (0.9 * ((ScreenObj.TextHeight(&Fonts.SubTitle, Viewsize) * Viewsize) * Options.ScaleSize));
        if (Fonts.SubTitle.Text.trimmed().length())
        {
            ScreenObj.PrintTextElement((PageWidth / 2) - (Halfwidth*Viewsize), (LayTop + TitleHeight)/Options.ScaleSize,&Fonts.SubTitle, Viewsize);
            TitleHeight += TextHeight;
        }
        Fonts.SubTitle.Size = Tmp;
    }
    if (Fonts.Composer.Size > 0)
    {
        float Tmp = Fonts.Composer.Size;
        Fonts.Composer.Size = Fonts.Composer.Size / (PrinterResFactor*1.0);
        int Halfwidth = ScreenObj.TextWidth(&Fonts.Composer, Viewsize)*Viewsize;
        int TextHeight = (0.9 * ((ScreenObj.TextHeight(&Fonts.Composer, Viewsize) * Viewsize) * Options.ScaleSize));
        if (Fonts.Composer.Text.trimmed().length())
        {
            ScreenObj.PrintTextElement(PageWidth - (LayRight/Options.ScaleSize) - Halfwidth , (LayTop + TitleHeight) / Options.ScaleSize,&Fonts.Composer, Viewsize);
            TitleHeight += TextHeight;
        }
        Fonts.Composer.Size = Tmp;
    }
    TitleList.append( ScreenObj.EndList());
    for (int i=0;i<TitleList.count();i++)
    {
        TitleList[i]->setZValue(2);
    }
}

CLayout::CLayout()
{
    Printer=new QPrinter(QPrinter::HighResolution);
    Template.clear("Template");
    PrinterExist=false;
    Name.clear();
    IsFormated=false;
    Viewsize=0;
    PrinterResFactor=1;
}

CLayout::~CLayout()
{
    qDeleteAll(Pages);
    Pages.clear();
    delete Printer;
}

const int CLayout::NumOfPages() const
{
    if (Pages.isEmpty()) return 0;
    return Pages.count();
}

void CLayout::AddPage()
{
    Pages.append(new CLayoutPage);
}

const float CLayout::MMToPixelX(const float MM) const
{
    float PixelPerMM=Printer->logicalDpiX()/25.4;
    return (MM*PixelPerMM)/PrinterResFactor;
}

const float CLayout::MMToPixelY(const float MM) const
{
    float PixelPerMM=Printer->logicalDpiY()/25.4;
    return (MM*PixelPerMM)/PrinterResFactor;
}

const int CLayout::MarginTop() const
{
    return (PageRect().top()-PaperRect().top())*Options.ScaleSize;
}

const int CLayout::MarginLeft() const
{
    return (PageRect().left()-PaperRect().left())*Options.ScaleSize;
}

const int CLayout::MarginRight() const
{
    return (PaperRect().right()-PageRect().right())*Options.ScaleSize;
}

const int CLayout::MarginBottom() const
{
    return (PaperRect().bottom()-PageRect().bottom())*Options.ScaleSize;
}

const QRectF CLayout::PageRect() const
{
    return PaperRect().adjusted(MMToPixelX(Options.LeftMargin),MMToPixelY(Options.TopMargin),-MMToPixelX(Options.RightMargin),-MMToPixelY(Options.BottomMargin));
}

const int CLayout::PageWidth() const
{
    return PageRect().width();
}

const int CLayout::PageHeight() const
{
    return PageRect().height();
}

const QRectF CLayout::PaperRect() const
{
    QRectF r(Printer->paperRect(QPrinter::DevicePixel));
    return QRectF(r.topLeft()/PrinterResFactor,r.size()/PrinterResFactor);
}

const int CLayout::PaperWidth() const
{
    return PaperRect().width();
}

const int CLayout::PaperHeight() const
{
    return PaperRect().height();
}

void CLayout::Plot(const int Page, const int System, XMLScoreWrapper& XMLScore, QGraphicsScene *Scene, const int PageLeft)
{
    OCDraw ScreenObj;
    ScreenObj.ColorOn=false;
    ScreenObj.Scene=Scene;
    int HoldSize = XMLScore.getVal("Size");
    bool HoldHideBarNumbers=XMLScore.getVal("DontShowBN");
    int HoldBarNumberOffset=XMLScore.getVal("BarNrOffset");
    int HoldSpace=XMLScore.getVal("NoteSpace");
    int HoldMasterStaff=XMLScore.getVal("MasteStave");
    XMLScore.setAttribute( "Size", Viewsize * Options.ScaleSize);
    XMLScore.setAttribute("DontShowBN",Options.DontShowBN);
    XMLScore.setAttribute("BarNrOffset",Options.BarNrOffset);
    XMLScore.setAttribute("NoteSpace",Options.NoteSpace+16);
    XMLScore.setAttribute("MasterStave",Options.MasterStave);
    ScreenObj.ScreenSize=Viewsize*Options.ScaleSize;
    //ScreenObj.sizx = Viewsize * Options.ScaleSize;
    //ScreenObj.sizy = Viewsize * Options.ScaleSize;
    int StartBar=0;
    Pages[Page]->Format(System, Score, XMLScore, &Template, StartBar, false, Options.ShowAllOnSys1, 0, PageWidth()*Options.ScaleSize);
    Pages[Page]->PlotSystem(System, Score, XMLScore, Fonts, Viewsize, (PageLeft * Viewsize * Options.ScaleSize) + MarginLeft(), MarginTop(), Options, ScreenObj, PrinterResFactor);
    XMLScore.setAttribute( "Size", HoldSize);
    XMLScore.setAttribute("DontShowBN",HoldHideBarNumbers);
    XMLScore.setAttribute("BarNrOffset",HoldBarNumberOffset);
    XMLScore.setAttribute("NoteSpace",HoldSpace);
    XMLScore.setAttribute("MasterStave",HoldMasterStaff);
}

void CLayout::AutoAll(const int StartPage, const int StartSystem, XMLScoreWrapper& XMLScore, QGraphicsScene *Scene, const bool Auto, LayoutViewXML* lv)
{
    bool Finished=false;
    OCDraw ScreenObj;
    ScreenObj.ColorOn=false;
    ScreenObj.Scene=Scene;
    int HoldSize = XMLScore.getVal("Size");
    bool HoldHideBarNumbers=XMLScore.getVal("DontShowBN");
    int HoldBarNumberOffset=XMLScore.getVal("BarNrOffset");
    int HoldSpace=XMLScore.getVal("NoteSpace");
    int HoldMasterStaff=XMLScore.getVal("MasteStave");
    XMLScore.setAttribute( "Size", Viewsize * Options.ScaleSize);
    XMLScore.setAttribute("DontShowBN",Options.DontShowBN);
    XMLScore.setAttribute("BarNrOffset",Options.BarNrOffset);
    XMLScore.setAttribute("NoteSpace",Options.NoteSpace+16);
    XMLScore.setAttribute("MasterStave",Options.MasterStave);
    ScreenObj.ScreenSize=Viewsize*Options.ScaleSize;
    //ScreenObj.sizx = Viewsize * Options.ScaleSize;
    //ScreenObj.sizy = Viewsize * Options.ScaleSize;
    int Page = StartPage;
    while (Page > Pages.count()-1) AddPage();
    while (Page > lv->SceneNumOfPages-1) lv->AddPage();
    int System = StartSystem;
    Pages[Page]->PlotTitle(Page, Fonts, Viewsize, PaperWidth(), MarginTop(), Options, MarginRight(), ScreenObj, PrinterResFactor);
    int StartBar=0;
    Score.CreateBarMap(XMLScore);
    forever
    {
        while (Page > Pages.count()-1) AddPage();
        while (Page > lv->SceneNumOfPages-1) lv->AddPage();
        forever
        {
            bool ShowAllStaves = false;
            if ((System == 0) && (Page == 0) && Options.ShowAllOnSys1) ShowAllStaves = true;
            while (System > Pages[Page]->NumOfSystems()-1) Pages[Page]->AddSystem();
            if ((Page == StartPage) && (System == StartSystem)) StartBar = Pages[Page]->StartBar(System);
            int ShowNames = Options.ShowNamesSwitch;
            if ((Page == 0) && (System == 0) && (Options.ShowNamesSwitch > 1)) ShowNames = 1;
            Pages[Page]->Format(System, Score, XMLScore, &Template, StartBar, Auto, ShowAllStaves, ShowNames, PageWidth()*Options.ScaleSize);
            //if (!Score.MoreNotes)
            if (Score.IsEnded(&Template))
            {
                if ((Page > 0) || (System > 0))
                {
                    Pages[Page]->RemoveSystem(false);
                    if (System == 0)
                    {
                        RemovePage();
                        lv->RemovePage();
                    }
                    break;
                }
            }
            Pages[Page]->PlotSystem(System, Score, XMLScore, Fonts, Viewsize, ((Page * IntDiv(PaperWidth(), Viewsize)) * Viewsize * Options.ScaleSize) + MarginLeft(), MarginTop(), Options, ScreenObj, PrinterResFactor);
            if (Auto)
            {
                if (FloatDiv((Pages[Page]->YSpaceUsed(System) + Pages[Page]->CurrentHeight(System)), Options.ScaleSize) > PageHeight()) break;
            }
            else
            {
                if ((Pages.count()-1 == Page) && (Pages[Page]->NumOfSystems()-1 == System))
                {
                    System ++;
                    Finished=true;
                    break;
                }
                if (Pages[Page]->NumOfSystems()-1 == System) break;
            }
            System ++;
        }
        //if ((!Score.MoreNotes) || Finished) break;
        if (Score.IsEnded(&Template) || Finished) break;
        System = 0;
        Page ++;
    }
    XMLScore.setAttribute( "Size", HoldSize);
    XMLScore.setAttribute("DontShowBN",HoldHideBarNumbers);
    XMLScore.setAttribute("BarNrOffset",HoldBarNumberOffset);
    XMLScore.setAttribute("NoteSpace",HoldSpace);
    XMLScore.setAttribute("MasterStave",HoldMasterStaff);
    AutoClear(Page, System);
}

void CLayout::AutoClear(const int StartPage, const int StartSystem)
{
    int SSys = StartSystem;
    int SPage = StartPage;
    if ((StartPage == 0) && (StartSystem == 0)) SSys ++;
    if (SPage < Pages.count())
    {
        if (SSys == 0)
        {
            SPage --;
        }
        else
        {
            for (int iTemp = SSys; iTemp< Pages[SPage]->NumOfSystems(); iTemp++)
            {
                Pages[SPage]->RemoveSystem(false);
            }
        }
    }
    if (SPage < Pages.count()-1)
    {
        for (int iTemp = Pages.count()-1; iTemp > SPage ;iTemp--)
        {
            delete Pages[iTemp];
            Pages.removeAt(iTemp);
        }
    }
}

void CLayout::RemovePage()
{
    if (Pages.count() <= 1) return;
    delete Pages.last();
    Pages.removeLast();
}

void CLayout::RemoveSystem(const int Page)
{
    Pages[Page]->RemoveSystem(false);
}

const int CLayout::NumOfSystems(const int Page) const
{
    if (Page>=Pages.count()) return 0;
    return Pages[Page]->NumOfSystems();
}

void CLayout::AddSystem(const int Page)
{
    Pages[Page]->AddSystem();
}

QDomLiteElement* CLayout::Save(const QString &Tag)
{
    QDomLiteElement* data=new QDomLiteElement(Tag);
    data->setAttribute("Name", Name);
    data->setAttribute("PageSize", Viewsize);
    data->setAttribute("PageCount", NumOfPages());
    data->setAttribute("IsFormated", IsFormated);
    for (int iTemp = 0; iTemp < Pages.count() ; iTemp++)
    {
        data->appendChild(Pages[iTemp]->Save("Page"));
    }
    data->appendClone(&Template);
    data->appendChild(Fonts.Save("Titles"));
    data->appendChild(Options.Save("Options"));
    return data;
}

void CLayout::Load(QDomLiteElement* data)
{
    qDeleteAll(Pages);
    Pages.clear();
    Name = data->attribute("Name");
    Viewsize = data->attributeValue("PageSize");
    int iTemp1 = data->attributeValue("PageCount");
    IsFormated = data->attributeValue("IsFormated");
    for (int iTemp = 0; iTemp < iTemp1; iTemp++)
    {
        AddPage();
        Pages[iTemp]->Load(data->elementsByTag("Page")[iTemp]);
    }
    Template.copy(data->elementByTag("Template"));
    Fonts.Load(data->elementByTag("Titles"));
    Options.Load(data->elementByTag("Options"));
}

void CLayout::setActiveObjects(const int Page, const int System)
{
    int P=qMin(Page,Pages.count()-1);
    P=qMax(0,P);
    if (Pages[P]->NumOfSystems()==0) P--;
    if (P<0) P=0;
    ActivePage = Pages[P];
    ActivePage->setActiveSystem(qMax(0,System));
}

const int CLayout::getActiveSystem() const
{
    if (Pages.isEmpty()) return 0;
    return ActivePage->getActiveSystem();
}

const int CLayout::getActivePage() const
{
    return Pages.indexOf(ActivePage);
}

const int CLayout::SysTop(const int Page, const int System) const
{
    return FloatDiv(Pages[Page]->SysTop(System) + MarginTop(), Options.ScaleSize);
}

const int CLayout::SysHeight(const int Page, const int System) const
{
    return FloatDiv(Pages[Page]->SysHeight(System), Options.ScaleSize);
}

const int CLayout::MoveBar(int& Page, int& System, const int Direction)
{
    int Pa = Page;
    int Sy = System;
    int RetVal=0;
    switch (Direction)
    {
    case 1:
        if (Pages[Page]->RemoveBar(System, false))
        {
            System ++;
            if (System > Pages[Page]->NumOfSystems()-1)
            {
                if (Page < Pages.count()-1)
                {
                    Page ++;
                    System = 0;
                }
                else
                {
                    Pages[Page]->AddSystem();
                    CLayoutSystem* s1=Pages[Page]->Sys(Sy);
                    CLayoutSystem* s2=Pages[Page]->Sys(System);
                    s2->Top=s1->Top+s1->Height;
                    s2->Height=s1->Height;
                    s2->DefaultTop=s2->Top;
                    s2->Syslen=s1->Syslen;
                    s2->StartBar=s1->EndBar+1;
                    s2->EndBar=s2->StartBar-1;
                    s2->Template.clear();
                    foreach(QDomLiteElement* e,s1->Template.childElements)
                    {
                        s2->Template.appendClone(e);
                    }
                }
            }
            Pages[Page]->AddBar(System, true);
            RetVal = -1;
        }
        break;
    case -1:
        System ++;
        if (System > Pages[Page]->NumOfSystems()-1)
        {
            Page ++;
            if (Page > Pages.count()-1)
            {
                Page = Pa;
                System = Sy;
                return RetVal;
            }
            System = 0;
        }
        if (Pages[Page]->RemoveBar(System, true))
        {
            Pages[Pa]->AddBar(Sy, false);
            if ((System==Pages[Page]->NumOfSystems()-1) & (Page==NumOfPages()-1))
            {
                if (Pages[Page]->Sys(System)->StartBar>=Pages[Page]->Sys(System)->EndBar)
                {
                    RetVal=1;
                    return RetVal;
                }
            }
            RetVal = -1;
        }
        else
        {
            if (Page == Pages.count()-1)
            {
                if (System == NumOfSystems(Page)-1)
                {
                    Pages[Pa]->AddBar(Sy, false);
                    RetVal = 1;
                }
            }
        }
    }
    if (RetVal == 0)
    {
        Page = Pa;
        System = Sy;
    }
    return RetVal;
}

void CLayout::MoveSystem(const int Page, const int Direction)
{
    switch (Sgn(Direction))
    {
    case 1:
        if (Page == Pages.count()-1) AddPage();
        Pages[Page + 1]->InsertSystem(true, Pages[Page]->GetSystem(false));
        break;
    case -1:
        if (Page < Pages.count()-1)
        {
            Pages[Page]->InsertSystem(false, Pages[Page + 1]->GetSystem(true));
        }
    }
}

void CLayout::AdjustSystems(const int Page)
{
    Pages[Page]->AdjustSystems((PageHeight() - 400) * Options.ScaleSize);
}

void CLayout::FormatTitle()
{
    Pages.first()->FormatTitle();
}

void CLayout::PlotTitle(QGraphicsScene *Scene)
{
    OCDraw ScreenObj;
    ScreenObj.ColorOn=false;
    ScreenObj.Scene=Scene;
    //ScreenObj.sizx = Viewsize * Options.ScaleSize;
    //ScreenObj.sizy = Viewsize * Options.ScaleSize;
    ScreenObj.ScreenSize=Viewsize*Options.ScaleSize;
    Pages.first()->PlotTitle(0, Fonts, Viewsize, PaperWidth(), MarginTop(), Options, MarginRight(), ScreenObj,PrinterResFactor);
}

void CLayout::Erase(const int Page, const int System, QGraphicsScene *Scene)
{
    if (Page<Pages.count()) Pages[Page]->Erase(System,Scene);
}

void CLayout::EraseTitle(QGraphicsScene *Scene)
{
    if (Pages.count()) Pages[0]->EraseTitle(Scene);
}

void CLayout::PrintIt(const int StartPage, XMLScoreWrapper& XMLScore, QGraphicsScene *Scene)
{
    int HoldViewsize = Viewsize;
    Viewsize = 1;
    OCDraw ScreenObj;
    ScreenObj.ColorOn=false;
    ScreenObj.Scene=Scene;
    int HoldSize = XMLScore.getVal("Size");
    bool HoldHideBarNumbers=XMLScore.getVal("DontShowBN");
    int HoldBarNumberOffset=XMLScore.getVal("BarNrOffset");
    int HoldSpace=XMLScore.getVal("NoteSpace");
    int HoldMasterStaff=XMLScore.getVal("MasteStave");
    XMLScore.setAttribute( "Size", Options.ScaleSize);
    XMLScore.setAttribute("DontShowBN",Options.DontShowBN);
    XMLScore.setAttribute("BarNrOffset",Options.BarNrOffset);
    XMLScore.setAttribute("NoteSpace",Options.NoteSpace+16);
    XMLScore.setAttribute("MasterStave",Options.MasterStave);
    //ScreenObj.sizx = Options.ScaleSize;
    //ScreenObj.sizy = Options.ScaleSize;
    ScreenObj.ScreenSize=Options.ScaleSize;
    int Page = StartPage;
    int System = 0;
    int StartBar=0;
    Pages[Page]->PlotTitle(Page, Fonts, Viewsize, PaperWidth(), MarginTop(), Options, MarginRight(), ScreenObj,PrinterResFactor);
    forever
    {
        bool ShowAllStaves = false;
        if ((System == 0) && (Page == 0) && Options.ShowAllOnSys1) ShowAllStaves = true;
        if (System > Pages[Page]->NumOfSystems()-1) Pages[Page]->AddSystem();
        if ((Page == StartPage) && (System == 0)) StartBar = Pages[Page]->StartBar(System);
        int ShowNames = Options.ShowNamesSwitch;
        if ((Page == 0) && (System == 0) && (Options.ShowNamesSwitch > 1)) ShowNames = 1;
        Pages[Page]->Format(System, Score, XMLScore, &Template, StartBar, false, ShowAllStaves, ShowNames, PageWidth()*Options.ScaleSize);
        //if (!Score.MoreNotes) break;
        if (Score.IsEnded(&Template)) break;
        Pages[Page]->PlotSystem(System, Score, XMLScore, Fonts, 1, MarginLeft(), MarginTop(), Options, ScreenObj, PrinterResFactor);
        if (Pages[Page]->NumOfSystems()-1 == System) break;
        System ++;
    }
    Viewsize = HoldViewsize;
    XMLScore.setAttribute( "Size", HoldSize);
    XMLScore.setAttribute("DontShowBN",HoldHideBarNumbers);
    XMLScore.setAttribute("BarNrOffset",HoldBarNumberOffset);
    XMLScore.setAttribute("NoteSpace",HoldSpace);
    XMLScore.setAttribute("MasterStave",HoldMasterStaff);
}

void CLayout::GetPrinter()
{
    Printer->setOrientation((QPrinter::Orientation)Options.Orientation);
    Printer->setPaperSize((QPrinter::PaperSize)Options.PaperSize);
    Printer->setFullPage(true);
    PrinterResFactor=FloatDiv(Printer->resolution(),400);
    Viewsize=PaperHeight()/920;
    Options.ScaleSize = Options.ScoreType+2;
    if (Options.ScaleSize==0) Options.ScaleSize=1;
}

void CLayout::SetPrinter()
{
    Printer->setOrientation((QPrinter::Orientation)Options.Orientation);
    Printer->setPaperSize((QPrinter::PaperSize)Options.PaperSize);
    Printer->setFullPage(true);
    PrinterResFactor=FloatDiv(Printer->resolution(),400);
    Viewsize=PaperHeight()/920;
    Options.ScaleSize = Options.ScoreType+2;
    if (Options.ScaleSize==0) Options.ScaleSize=1;

}

const bool CLayout::ChangePageSetup(QWidget* Owner)
{
    QPageSetupDialog d(Printer,Owner);
    d.setWindowFlags(Qt::Sheet);
    d.setWindowModality(Qt::WindowModal);
    return (d.exec()==QDialog::Accepted);
}

const bool CLayout::ChangePrinter(QWidget* Owner, QPrinter* Prn)
{
    QPrintDialog d(Prn, Owner);
    d.setWindowFlags(Qt::Sheet);
    d.setWindowModality(Qt::WindowModal);
    return (d.exec()==QDialog::Accepted);
}

CLayoutCollection::CLayoutCollection()
{

}

CLayoutCollection::~CLayoutCollection()
{
    qDeleteAll(Layouts);
    Layouts.clear();
}

void CLayoutCollection::AddLayout(const QString& Name)
{
    Layouts.append(new CLayout);
    Layouts.last()->Name=Name;
}

void CLayoutCollection::RemoveLayout(const int Index)
{
    delete Layouts[Index];
    Layouts.removeAt(Index);
}

const QString CLayoutCollection::Name(const int Index) const
{
    if (Index > Layouts.count()-1) return QString();
    return Layouts[Index]->Name;
}

void CLayoutCollection::InitLayout(const int Index, XMLScoreWrapper& Score)
{
    if (Score.NumOfLayouts()>Index)
    {
        Layouts[Index]->Load(Score.Layout(Index));
        Layouts[Index]->Score.MakeStaves(Score);
        if (Layouts[Index]->NumOfPages())
        {
            Layouts[Index]->setActiveObjects(0,0);
        }
    }
    else
    {
        Layouts[Index]->Template.copy(Score.Template(0));
    }
}

void CLayoutCollection::SetCurrentLayout(const int Index)
{
    CurrentLayout=Layouts[Index];
}

void CLayoutCollection::SetCurrentLayout(const QString& Name)
{
    for (int i=0; i<Layouts.count(); i++)
    {
        if (Layouts[i]->Name==Name)
        {
            CurrentLayout=Layouts[i];
            break;
        }
    }
}

const int CLayoutCollection::getCurrentLayout() const
{
    return Layouts.indexOf(CurrentLayout);
}

const int CLayoutCollection::getActivePage() const
{
    return CurrentLayout->getActivePage();
}

const int CLayoutCollection::getActiveSystem() const
{
    return CurrentLayout->getActiveSystem();
}

QDomLiteElement* CLayoutCollection::Save()
{
    QDomLiteElement* Doc=new QDomLiteElement("LayoutCollection");
    Doc->setAttribute("LayoutCount", Layouts.count());
    for (int iTemp=0; iTemp<Layouts.count(); iTemp++)
    {
        Doc->appendChild(Layouts[iTemp]->Save("Layout"));
    }
    return Doc;
}

void CLayoutCollection::Load(QDomLiteElement* data, XMLScoreWrapper& Score)
{
    qDeleteAll(Layouts);
    Layouts.clear();
    int iTemp1 = Score.NumOfLayouts();
    for (int iTemp = 0; iTemp<iTemp1; iTemp++)
    {
        AddLayout();
        Layouts[iTemp]->Load(data->elementsByTag("Layout")[iTemp]);
        Layouts[iTemp]->Score.MakeStaves(Score);
        if (Layouts[iTemp]->NumOfPages())
        {
            Layouts[iTemp]->setActiveObjects(0,0);
        }
    }
}



bool CLayoutCollection::ChangePageSetup(QWidget *Owner)
{
    return CurrentLayout->ChangePageSetup(Owner);
}

bool CLayoutCollection::ChangePrinter(QWidget *Owner)
{
    return CurrentLayout->ChangePrinter(Owner,CurrentLayout->Printer);
}

const int CLayoutCollection::NumOfLayouts() const
{
    return Layouts.count();
}

void CLayoutCollection::Clear()
{
    qDeleteAll(Layouts);
    Layouts.clear();
}
