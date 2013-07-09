#include "mainwindow.h"
#include "CommonClasses.h"
#include "ui_mainwindow.h"
#include "cstaffsdialog.h"
#include "cbarwindow.h"
#include "cpropertywindow.h"
#include "cmusictree.h"
#include "cscorewizard.h"
#include "cpresetsdialog.h"
#include "qtoolbuttongrid.h"
#include "ocpiano.h"
#include <QThread>
#include "ocplaycontrol.h"
#include <QClipboard>
#include <QGraphicsDropShadowEffect>
#include <QDesktopWidget>

QTransMenu::QTransMenu(QWidget *parent) : QMenu(parent)
{
    this->setStyleSheet("QMenu{background-color:black;border:2px outset grey;border-radius:6px;margin:1px;padding:4px;}QMenu::item:disabled{color:grey;}QMenu::item{color:white;}QMenu::item:selected:enabled{background-color:QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #D0D0EF, stop: 0.3999 #9999df, stop: 0.4 #8989cf, stop: 1 #4444DF );}QMenu::separator{margin:6px;height:2px;background:grey;}");
}

void QTransMenu::paintEvent(QPaintEvent *e)
{
    this->setWindowOpacity(0.7);
    QMenu::paintEvent(e);
}

QCustomFrame::QCustomFrame(QWidget *parent) : QFrame(parent)
{
    this->setWindowFlags((Qt::WindowFlags)(this->windowFlags() | Qt::WindowStaysOnTopHint));
    this->setWindowModality(Qt::WindowModal);
    this->setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setBrush(QPalette::Window, QColor(0, 0, 0, 150) );
    this->setPalette(pal);
    this->setAutoFillBackground(true);

    //this->setWindowFlags((Qt::WindowFlags)(this->windowFlags() & !Qt::Sheet));
    //this->setWindowModality(Qt::WindowModal);
}

void MainWindow::ShowScoreWizard()
{
    CScoreWizard* d=new CScoreWizard(this);
    d->Open(":/Template.mus");
    d->setWindowTitle("New Score");
    if (d->exec()==QDialog::Accepted)
    {
        document=new CScoreDoc(SC,this);
        document->setWindowTitle("Untitled");
        document->sv->SetXML(d->CreateXML(SC));
        document->lv->Load(document->sv->GetXML());
        document->UpdateLayoutList();
        document->SetCurrentLayout(0);
        ui->mdiArea->addWidget(document);
        ui->mdiArea->setCurrentWidget(document);
        document->showMaximized();
        document->UpdateAll();
    }
    delete d;
}

void MainWindow::UpdateWindowList()
{
    QMenu* windowMenu=findChild<QMenu*>("menuWindow");

  windowMenu->clear();
  windowMenu->addAction(closeAction);
  windowMenu->addSeparator();
  windowMenu->addAction( nextAction );
  windowMenu->addAction( previousAction );
  windowMenu->addSeparator();

  int i=1;
  for(int j=0;j<ui->mdiArea->count();j++)
  {
      QWidget* w=ui->mdiArea->widget(j);
    QString text;
    if( i<10 )
      text = tr("&%1 %2").arg( i++ ).arg( w->windowTitle() );
    else
      text = w->windowTitle();

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
    while (true)
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
    QTreeWidgetItem* item=ui->SidebarTree->topLevelItem(0)->child(0)->child(Projects.count()-1);
    ui->SidebarTree->editItem(item);
}

void MainWindow::RenameProject(QTreeWidgetItem *item, int column)
{
    if (item->data(0,SDI_ItemType).toInt()==SDT_Project)
    {
        Projects[item->data(0,SDI_ItemIndex).toInt()]->Name=item->text(0);
    }
    UpdateSidebar();
}

void MainWindow::DragDocument()
{
    isDrag=true;
}

void MainWindow::MoveDocument()
{
    if (!isDrag) return;
    isDrag=false;
    QTreeWidgetItem* projectsItem=ui->SidebarTree->topLevelItem(0)->child(0);
    qDeleteAll(Projects);
    Projects=QList<OCProject*>();
    for(int i=0;i<projectsItem->childCount();i++)
    {
        OCProject* p=new OCProject;
        p->Name=projectsItem->child(i)->text(0);
        for (int j=0;j<projectsItem->child(i)->childCount();j++)
        {
            QString s;
            QTreeWidgetItem* documentItem=projectsItem->child(i)->child(j);
            if (documentItem->data(0,SDI_Filepath).toString().length())
            {
                s=documentItem->data(0,SDI_Filepath).toString();
            }
            else
            {
                s=documentItem->text(0);
            }
            p->Files.append(s);
        }
        Projects.append(p);
    }
    UpdateSidebar();
}

void MainWindow::UpdateSidebar()
{
    isDrag=false;
    QStringList OpenDocs;
    for (int j=0;j<ui->mdiArea->count();j++)
    {
        CScoreDoc* doc=(CScoreDoc*)ui->mdiArea->widget(j);
        QFileInfo fi(doc->windowTitle());
        OpenDocs.append(fi.filePath());
    }

    ui->SidebarTree->blockSignals(true);
    ui->SidebarTree->setDragEnabled(true);
    ui->SidebarTree->viewport()->setAcceptDrops(true);
    ui->SidebarTree->setDropIndicatorShown(true);
    ui->SidebarTree->setDragDropMode(QTreeWidget::InternalMove);
    ui->SidebarTree->invisibleRootItem()->setFlags(Qt::ItemIsEnabled);
    ui->SidebarTree->hide();
    ui->SidebarTree->clear();

    QTreeWidgetItem* libraryItem=new QTreeWidgetItem;
    libraryItem->setFlags(Qt::ItemIsEnabled);
    libraryItem->setText(0,"LIBRARY");
    libraryItem->setTextColor(0,"#666666");
    libraryItem->setSizeHint(0,QSize(-1,tablerowheight));
    ui->SidebarTree->addTopLevelItem(libraryItem);

    QTreeWidgetItem* projectsItem=new QTreeWidgetItem;
    projectsItem->setFlags(Qt::ItemIsEnabled);
    projectsItem->setText(0,"Projects");
    projectsItem->setIcon(0,QIcon(":/ocgrey.png"));
    libraryItem->addChild(projectsItem);
    for (int i=0;i<Projects.count();i++)
    {
        QTreeWidgetItem* projectItem=new QTreeWidgetItem;
        projectItem->setText(0,Projects[i]->Name);
        projectItem->setIcon(0,QIcon(":/bookgrey.png"));
        projectItem->setData(0,SDI_ItemType,SDT_Project);
        projectItem->setData(0,SDI_ItemIndex,i);
        projectItem->setData(0,SDI_Action,SBA_None);
        projectItem->setToolTip(0,"Project <b>"+Projects[i]->Name+"</b>");
        projectItem->setSizeHint(0,QSize(-1,tablerowheight));
        if (Projects[i]->Files.count())
        {
            projectItem->setIcon(1,QIcon(":/bookgrey.png"));
            projectItem->setData(1,SDI_Action,SBA_OpenProject);
            projectItem->setToolTip(1,"Open Project <b>"+Projects[i]->Name+"</b>");
            projectItem->setSizeHint(1,QSize(22,tablerowheight));
        }
        projectItem->setIcon(2,QIcon(":/fileclose.png"));
        projectItem->setData(2,SDI_Action,SBA_DeleteProject);
        projectItem->setToolTip(2,"Delete Project <b>"+Projects[i]->Name+"</b>");
        projectItem->setSizeHint(2,QSize(22,tablerowheight));
        projectItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDropEnabled);
        projectsItem->addChild(projectItem);
        for (int j=0;j<Projects[i]->Files.count();j++)
        {
            QString s=Projects[i]->Files[j];
            QTreeWidgetItem* documentItem=new QTreeWidgetItem;
            QFileInfo fi(s);
            documentItem->setText(0,fi.fileName());
            documentItem->setIcon(0,QIcon(":/scoregrey.png"));
            documentItem->setData(0,SDI_ItemIndex,j);
            documentItem->setData(0,SDI_ParentText,Projects[i]->Name);
            documentItem->setData(0,SDI_ParentIndex,i);
            documentItem->setData(0,SDI_Filepath,s);
            if (OpenDocs.indexOf(s)>-1)
            {
                documentItem->setData(0,SDI_OpenIndex,OpenDocs.indexOf(s));
                documentItem->setData(0,SDI_Action,SBA_Reveal);
                documentItem->setData(0,SDI_ItemType,SDT_OpenDoc);
                documentItem->setToolTip(0,"Switch to <b>"+fi.fileName()+"</b>");
            }
            else
            {
                documentItem->setData(0,SDI_ItemType,SDT_File);
                if (fi.exists())
                {
                    documentItem->setData(0,SDI_Action,SBA_Open);
                    documentItem->setToolTip(0,"Open <b>"+s+"</b>");
                }
                else
                {
                    documentItem->setTextColor(0,Qt::gray);
                    documentItem->setData(0,SDI_Action,SBA_None);
                    documentItem->setToolTip(0,"<b>"+s+"</b> does not exist");
                }
            }
            projectItem->addChild(documentItem);
            documentItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsDragEnabled);
            documentItem->setIcon(2,QIcon(":/fileclose.png"));
            documentItem->setData(2,SDI_Action,SBA_RemoveFromProject);
            documentItem->setToolTip(2,"Remove <b>"+fi.fileName()+"</b> from Project <b>"+Projects[i]->Name+"</b>");
        }
        projectItem->setExpanded(true);
    }
    projectsItem->setExpanded(true);
    libraryItem->setExpanded(true);

    QTreeWidgetItem* openDocumentsItem=new QTreeWidgetItem;
    openDocumentsItem->setFlags(Qt::ItemIsEnabled);
    openDocumentsItem->setText(0,"OPEN DOCUMENTS");
    openDocumentsItem->setTextColor(0,"#666666");
    openDocumentsItem->setSizeHint(0,QSize(-1,tablerowheight));
    ui->SidebarTree->addTopLevelItem(openDocumentsItem);
    openDocumentsItem->setExpanded(true);
    for (int j=0;j<ui->mdiArea->count();j++)
    {
        CScoreDoc* doc=(CScoreDoc*)ui->mdiArea->widget(j);
        QTreeWidgetItem* documentItem=new QTreeWidgetItem;
        QFileInfo fi(doc->windowTitle());
        documentItem->setText(0,fi.fileName());
        documentItem->setIcon(0,QIcon(":/scoregrey.png"));
        documentItem->setData(0,SDI_ItemType,SDT_OpenDoc);
        documentItem->setData(0,SDI_ItemIndex,j);
        documentItem->setData(0,SDI_OpenIndex,j);
        documentItem->setData(0,SDI_Filepath,fi.filePath());
        documentItem->setData(0,SDI_Action,SBA_Reveal);
        documentItem->setToolTip(0,"Switch to <b>"+fi.fileName()+"</b>");
        if (ui->mdiArea->count()>1)
        {
            documentItem->setIcon(2,QIcon(":/fileclose.png"));
            documentItem->setData(2,SDI_Action,SBA_Close);
            documentItem->setToolTip(2,"Close <b>"+fi.fileName()+"</b>");
            documentItem->setSizeHint(2,QSize(22,tablerowheight));
        }
        documentItem->setIcon(1,QIcon(":/filesave.png"));
        documentItem->setData(1,SDI_Action,SBA_Save);
        documentItem->setSizeHint(1,QSize(22,tablerowheight));
        documentItem->setToolTip(1,"Save <b>"+fi.fileName()+"</b>");

        documentItem->setSizeHint(0,QSize(-1,tablerowheight));
        documentItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsDragEnabled);
        openDocumentsItem->addChild(documentItem);
        documentItem->setExpanded(true);
        for (int i=0;i<doc->lv->NumOfLayouts();i++)
        {            
            QTreeWidgetItem* layoutitem=new QTreeWidgetItem;
            layoutitem->setText(0,doc->lv->LayoutName(i));
            layoutitem->setIcon(0,QIcon(":/layoutgrey.png"));
            layoutitem->setData(0,SDI_ItemType,SDT_OpenLayout);
            layoutitem->setData(0,SDI_ItemIndex,i);
            layoutitem->setData(0,SDI_OpenIndex,j);
            layoutitem->setData(0,SDI_Filepath,fi.filePath());
            layoutitem->setData(0,SDI_Action,SBA_Reveal);
            layoutitem->setData(0,SDI_ParentIndex,j);
            layoutitem->setData(0,SDI_ParentText,fi.fileName());
            layoutitem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
            layoutitem->setToolTip(0,"Switch to <b>"+fi.fileName()+"</b> Layout <b>"+doc->lv->LayoutName(i)+"</b>");
            documentItem->addChild(layoutitem);
        }
    }
    QTreeWidgetItem* recentDocumentsItem=new QTreeWidgetItem;
    recentDocumentsItem->setFlags(Qt::ItemIsEnabled);
    recentDocumentsItem->setText(0,"RECENT DOCUMENTS");
    recentDocumentsItem->setTextColor(0,"#666666");
    recentDocumentsItem->setSizeHint(0,QSize(-1,tablerowheight));
    ui->SidebarTree->addTopLevelItem(recentDocumentsItem);
    recentDocumentsItem->setExpanded(true);
    int i=0;
    foreach(QString s,RecentFiles)
    {
        QTreeWidgetItem* fileItem=new QTreeWidgetItem;
        QFileInfo fi(s);
        if (OpenDocs.indexOf(fi.filePath())==-1)
        {
            fileItem->setIcon(0,QIcon(":/scoregrey.png"));
            fileItem->setText(0,fi.fileName());
            fileItem->setData(0,SDI_ItemType,SDT_File);
            fileItem->setData(0,SDI_ItemIndex,i);
            i++;
            fileItem->setData(0,SDI_Filepath,s);
            if (fi.exists())
            {
                fileItem->setData(0,SDI_Action,SBA_Open);
                fileItem->setToolTip(0,"Open <b>"+fi.filePath()+"</b>");
                fileItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsDragEnabled);
            }
            else
            {
                fileItem->setTextColor(0,Qt::gray);
                fileItem->setData(0,SDI_Action,SBA_None);
                fileItem->setIcon(2,QIcon(":/fileclose.png"));
                fileItem->setData(2,SDI_Action,SBA_RemoveFromRecent);
                fileItem->setToolTip(0,"<b>"+fi.filePath()+"</b> does not exist");
                fileItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
                fileItem->setToolTip(2,"Remove <b>"+fi.fileName()+"</b> from Recent Files");
                //item->setFirstColumnSpanned(true);
            }
            recentDocumentsItem->addChild(fileItem);
        }
    }
    ui->SidebarTree->setColumnWidth(2,22);
    ui->SidebarTree->setColumnWidth(1,22);
    ui->SidebarTree->resizeColumnToContents(2);
    ui->SidebarTree->resizeColumnToContents(1);
    ui->SidebarTree->header()->adjustSize();
    SidebarMouseLeave(0);
    ui->SidebarTree->show();
    ResizeSidebar();
    ui->SidebarTree->blockSignals(false);
}

void MainWindow::SidebarMouseEnter(QEvent *event)
{
    ui->SidebarTree->setMouseTracking(true);
}

void MainWindow::SidebarMouseLeave(QEvent *event)
{
    QTreeWidgetItemIterator it(ui->SidebarTree);
    while (*it)
    {
        QTreeWidgetItem* item=(*it);
        item->setFirstColumnSpanned(true);
        ++it;
    }
    ui->SidebarTree->setMouseTracking(false);
}

void MainWindow::SidebarMouseMove(QMouseEvent *event)
{
    QTreeWidgetItemIterator it(ui->SidebarTree);
    QTreeWidgetItem* i=ui->SidebarTree->itemAt(event->pos());
    while (*it)
    {
        QTreeWidgetItem* item=(*it);
        if (i==item)
        {
            if ((!item->icon(1).isNull()) || (!item->icon(2).isNull()))
            {
                item->setFirstColumnSpanned(false);
            }
        }
        else
        {
            item->setFirstColumnSpanned(true);
        }
        ++it;
    }
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    ResizeSidebar();
}

void MainWindow::SidebarSetSelected()
{
    ui->SidebarTree->blockSignals(true);
    ui->SidebarTree->clearSelection();
    if (ui->SidebarTree->topLevelItemCount()>1)
    {
        QTreeWidgetItem* docitem=ui->SidebarTree->topLevelItem(1)->child(ui->mdiArea->currentIndex());
        if (CurrentDoc()->GetView()==0)
        {
            ui->SidebarTree->setCurrentItem(docitem);
        }
        else
        {
            if (docitem->childCount()>0)
            {
                if (docitem->childCount()>CurrentDoc()->lv->CurrentLayoutIndex)
                {
                    ui->SidebarTree->setCurrentItem(docitem->child(CurrentDoc()->lv->CurrentLayoutIndex));
                }
            }
        }
    }
    ui->SidebarTree->blockSignals(false);
}

void MainWindow::ResizeSidebar()
{
    if (ui->SidebarTree->verticalScrollBar()->isVisible())
    {
        ui->SidebarTree->setColumnWidth(0,ui->SidebarTree->width()-((22*2)+2)-ui->SidebarTree->verticalScrollBar()->width());
    }
    else
    {
        ui->SidebarTree->setColumnWidth(0,ui->SidebarTree->width()-((22*2)+2));
    }
}

void MainWindow::SidebarClicked(QTreeWidgetItem* item, int Col)
{
    SidebarDocumentTypes itemType=(SidebarDocumentTypes)(item->data(0,SDI_ItemType).toInt());
    SidebarActions a=(SidebarActions)(item->data(Col,SDI_Action).toInt());
    int itemIndex=item->data(0,SDI_ItemIndex).toInt();
    int openIndex=item->data(0,SDI_OpenIndex).toInt();
    int parentIndex=item->data(0,SDI_ParentIndex).toInt();
    QString filePath=item->data(0,SDI_Filepath).toString();
    if (Col>0)
    {
        if (a==SBA_Close)
        {
            ui->mdiArea->setCurrentIndex(openIndex);
            this->CloseDoc();
            this->UpdateStatus();
        }
        else if (a==SBA_RemoveFromRecent)
        {
            RecentFiles.removeOne(filePath);
            UpdateWindowList();
            UpdateSidebar();
        }
        else if (a==SBA_RemoveFromProject)
        {
            Projects[parentIndex]->Files.removeAt(itemIndex);
            this->UpdateSidebar();
        }
        else if (a==SBA_DeleteProject)
        {
            item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
            QMessageBox* msgBox=new QMessageBox(this);
            msgBox->setWindowModality(Qt::WindowModal);
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setText("<b>Delete Project "+Projects[itemIndex]->Name+"?<b>");
            msgBox->setInformativeText("This operation can not be undone! Are You sure?");
            msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox->setDefaultButton(QMessageBox::No);
            if (msgBox->exec()==QMessageBox::Yes)
            {
                delete Projects[itemIndex];
                Projects.removeAt(itemIndex);
                this->UpdateSidebar();
            }
            delete msgBox;
        }
        else if (a==SBA_Save)
        {
            ui->mdiArea->setCurrentIndex(openIndex);
            this->SaveDoc();
            this->UpdateStatus();
        }
        else if (a==SBA_OpenProject)
        {
            item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
            {
                foreach (QString s,Projects[itemIndex]->Files)
                {
                    OpenDoc(s);
                }
            }
        }
    }
    else
    {
        if (a==SBA_Open)
        {
            OpenDoc(filePath);
        }
        else if (a==SBA_Reveal)
        {
            bool isLayout=itemType==SDT_OpenLayout;
            if (openIndex != ui->mdiArea->currentIndex()) ui->mdiArea->setCurrentIndex(openIndex);
            if (isLayout)
            {
                if (CurrentDoc()->lv->CurrentLayoutIndex != itemIndex) CurrentDoc()->SetCurrentLayout(itemIndex);
                if (CurrentDoc()->GetView() != 1) ShowLayout();
            }
            else
            {
                if (CurrentDoc()->GetView() != 0) ShowScore();
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
    MacSplitter* splitter=this->findChild<MacSplitter*>();
    if (!splitter->isCollapsed())
    {
        splitter->collapse(0);
        LeftSideButton->setIcon(QIcon(":/showleft.png"));
    }
    else
    {
        splitter->expand();
        LeftSideButton->setIcon(QIcon(":/hideleft.png"));
    }
}

void MainWindow::ToggleLayoutSidebar()
{
    MacSplitter* splitter=CurrentDoc()->spSidebar;
    if (!splitter->isCollapsed())
    {
        splitter->collapse(1);
        RightSideButton->setIcon(QIcon(":/showright.png"));
    }
    else
    {
        splitter->expand();
        RightSideButton->setIcon(QIcon(":/hideright.png"));
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    #ifdef Q_WS_MAC
        qt_mac_set_menubar_icons(false);
    #endif
    this->setStyleSheet("QToolTip{background-color:#444;opacity:215;color:#eee;border:2px outset #666;padding:3px;margin:1px;border-radius:5px;font-size:13pt;}QProgressBar{background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #eee, stop: 0.4999 #bbb, stop: 0.5 #afafaf, stop: 1 #989898 );border:1px outset #666;padding:2px;color:white;text-align:center;border-radius:8px;} QProgressBar::chunk {  background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #78d, stop: 0.4999 #46a, stop: 0.5 #45a, stop: 1 #238 );border: 1px outset #666;border-radius:5px;}");
    //this->setWindowIcon(QIcon(":/grey-orb.icns"));
    mapper=new QSignalMapper(this);
    connect( mapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)) );

    RecentMapper=new QSignalMapper(this);
    connect( RecentMapper, SIGNAL(mapped(QString)), this, SLOT(OpenDoc(QString)));

    setWindowTitle(apptitle);

    LoadRecent();

    Projects.clear();
    CPresets P("Projects");
    int projectcount=P.GetValue("ProjectCount").toInt();
    for (int i=0;i<projectcount;i++)
    {
        QString n=P.GetString("ProjectName"+QString::number(i));
        OCProject* p=new OCProject;
        p->Name=n;
        p->Files=P.GetValue("Documents"+QString::number(i)).toStringList();
        Projects.append(p);
    }

    NewMenu=new QMenu(this);
    NewMenu->addAction(Action("actionNew"));
    NewMenu->addAction(Action("actionNew_Score"));
    NewMenu->addAction(Action("actionNew_Project"));
    NewMenu->addSeparator();
    NewMenu->addAction(Action("actionOpen"));

    MacSplitter* splitter=new MacSplitter;
    this->centralWidget()->layout()->addWidget(splitter);
    splitter->addWidget(ui->SidebarFrame);
    splitter->addWidget(ui->mdiArea);
    splitter->setStretchFactor(0,1);
    splitter->setStretchFactor(1,2);

    ui->SidebarFrame->setStyleSheet(buttongradient);
    ui->SidebarFrame->setWindowFlags((Qt::WindowFlags)(ui->SidebarFrame->windowFlags()|Qt::WA_OpaquePaintEvent|Qt::WA_PaintOnScreen));
    ui->SidebarFrame->setAutoFillBackground(true);
    ui->SidebarTree->header()->setFixedHeight(tablerowheight);
    ui->SidebarTree->setFont(QFont("Lucida Grande",11));
#if defined(__STYLESHEETS__)
    ui->SidebarTree->setStyleSheet(QString(widgetgradient)+selectiongradient);
#endif
    ui->SidebarTree->setWindowFlags((Qt::WindowFlags)(ui->SidebarTree->windowFlags()|Qt::WA_OpaquePaintEvent|Qt::WA_PaintOnScreen));
    ui->SidebarTree->setAutoFillBackground(true);
    ui->SidebarTree->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->SidebarTree->setIndentation(12);
    ui->SidebarTree->setColumnCount(3);
    //ui->SidebarTree->setIconSize(QSize(tablerowheight,tablerowheight));
    ui->SidebarTree->setUniformRowHeights(true);
    ui->SidebarTree->setAnimated(true);
    ui->SidebarTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->SidebarTree->setAnimated(false);

    InternalMoveEvent* ItemMoved=new InternalMoveEvent;
    ui->SidebarTree->installEventFilter(ItemMoved);
    connect(ItemMoved,SIGNAL(ItemMoved()),this,SLOT(MoveDocument()));
    connect(ItemMoved,SIGNAL(DragEnter()),this,SLOT(DragDocument()));
    MouseEvents* ev=new MouseEvents;
    ui->SidebarTree->viewport()->installEventFilter(ev);
    connect(ev,SIGNAL(MouseMove(QMouseEvent*)),this,SLOT(SidebarMouseMove(QMouseEvent*)));
    connect(ev,SIGNAL(MouseEnter(QEvent*)),this,SLOT(SidebarMouseEnter(QEvent*)));
    connect(ev,SIGNAL(MouseLeave(QEvent*)),this,SLOT(SidebarMouseLeave(QEvent*)));

    connect(ui->SidebarTree,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(SidebarClicked(QTreeWidgetItem*,int)));
    connect(ui->SidebarTree,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(RenameProject(QTreeWidgetItem*,int)));
    connect(splitter,SIGNAL(splitterMoved(int,int)),this,SLOT(ResizeSidebar()));
    //connect(ui->NewDocument,SIGNAL(clicked()),this,SLOT(ShowScoreWizard()));
    connect(ui->NewDocument,SIGNAL(clicked()),this,SLOT(ShowNewMenu()));
    //ui->NewDocument->setMenu(NewMenu);
    //connect(ui->LoadDocument,SIGNAL(clicked()),this,SLOT(OpenDialog()));
    connect(ui->SaveDocument,SIGNAL(clicked()),this,SLOT(SaveDoc()));
    connect(ui->CloseDocument,SIGNAL(clicked()),this,SLOT(CloseDoc()));
    connect(ui->EditDocument,SIGNAL(clicked()),this,SLOT(Settings()));
    //QMdiArea* mdiArea=findChild<QMdiArea*>();
    connect(ui->mdiArea,SIGNAL(currentChanged(int)),this,SLOT(DocumentActivated(int)));

    QToolBar* ltb=findChild<QToolBar*>("toolBar");

    OCPlayControl* pc=new OCPlayControl(this);
    pc->setFixedWidth(320);
    connect(pc,SIGNAL(RequestFile(QString&)),this,SLOT(RequestMidiFile(QString&)));
    connect(Action("actionPlay"),SIGNAL(triggered()),pc,SLOT(Trigger()));
    ltb->addWidget(pc);

    //QMenu* vm=findChild<QMenu*>("menuView");
    //foreach (QAction* a,vm->actions()) ltb->addAction(a);
    //ltb->addSeparator();
    //ltb->removeAction(ui->actionLayouts);
    //ltb->removeAction(ui->actionScore);

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ltb->addWidget(spacer);

    QMenu* sm=findChild<QMenu*>("menuScore");
    QMenu* lm=findChild<QMenu*>("menuLayouts");
    sm->removeAction(ui->actionTurn_Page);
    sm->removeAction(ui->actionBack);
    sm->removeAction(ui->actionFirst);
    sm->removeAction(ui->actionForward);
    sm->removeAction(ui->actionLast);
    foreach (QAction* a,sm->actions()) ltb->addAction(a);
    foreach (QAction* a,lm->actions()) ltb->addAction(a);

/*
    QWidget* spacer1 = new QWidget();
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ltb->addWidget(spacer1);
*/
    QMenu* em=findChild<QMenu*>("menuEdit");
    foreach (QAction* a,em->actions()) ltb->addAction(a);

    ui->toolBar->removeAction(ui->actionLayoutSettings);
    ui->toolBar->removeAction(ui->actionSettings);

    foreach (QAction* a,ui->toolBar->actions())
    {
        if (a->isSeparator()) ui->toolBar->removeAction(a);
    }
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionLayoutSettings);
    ui->toolBar->addAction(ui->actionSettings);

    QStatusBar* sb = findChild<QStatusBar*>();
    LeftSideButton=new QToolButton(this);
    LeftSideButton->setFixedSize(20,20);
    LeftSideButton->setIconSize(QSize(20,20));
    LeftSideButton->setStyleSheet("QToolButton{background:transparent;}");
    LeftSideButton->setIcon(QIcon(":/hideleft.png"));
    sb->addWidget(LeftSideButton);
    connect(LeftSideButton,SIGNAL(clicked()),this,SLOT(ToggleSidebar()));

    StatusLabel=new QLabel(this);
    StatusLabel->setFont(QFont("Lucida Grande",11));
    /*
    QGraphicsDropShadowEffect* shadow=new QGraphicsDropShadowEffect(this);
    shadow->setEnabled(true);
    shadow->setBlurRadius(0);
    shadow->setColor(Qt::white);
    shadow->setOffset(1,1);
    StatusLabel->setGraphicsEffect(shadow);
    */
    sb->addPermanentWidget(StatusLabel);

    QWidget* spacer3 = new QWidget();
    spacer3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sb->addPermanentWidget(spacer3);

    btnZoomOut=new QToolButton(this);
    btnZoomOut->setFixedSize(20,20);
    btnZoomOut->setIconSize(QSize(20,20));
    btnZoomOut->setStyleSheet("QToolButton{background:transparent;}");
    btnZoomOut->setIcon(QIcon(":/small.png"));
    sb->addPermanentWidget(btnZoomOut);

    ZoomSlider=new QSlider(this);
    ZoomSlider->setOrientation(Qt::Horizontal);
    ZoomSlider->setFixedWidth(120);
    ZoomSlider->setRange(25,100);
    sb->addPermanentWidget(ZoomSlider);

    btnZoomIn=new QToolButton(this);
    btnZoomIn->setFixedSize(20,20);
    btnZoomIn->setIconSize(QSize(20,20));
    btnZoomIn->setStyleSheet("QToolButton{background:transparent;}");
    btnZoomIn->setIcon(QIcon(":/large.png"));
    sb->addPermanentWidget(btnZoomIn);

    RightSideButton=new QToolButton(this);
    RightSideButton->setFixedSize(20,20);
    RightSideButton->setIconSize(QSize(20,20));
    RightSideButton->setStyleSheet("QToolButton{background:transparent;}");
    RightSideButton->setIcon(QIcon(":/hideright.png"));
    RightSideButton->setVisible(false);
    connect(RightSideButton,SIGNAL(clicked()),this,SLOT(ToggleLayoutSidebar()));
    sb->addPermanentWidget(RightSideButton);

    //this->setUnifiedTitleAndToolBarOnMac(true);
    /*
    tileAction = new QAction( tr("&Tile"), this );
    tileAction->setStatusTip( tr("Tile the windows") );
    connect( tileAction, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()) );

    cascadeAction = new QAction( tr("&Cascade"), this );
    cascadeAction->setStatusTip( tr("Cascade the windows") );
    connect( cascadeAction, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()) );
    */
    nextAction = new QAction( tr("&Next window"), this );
    nextAction->setStatusTip( tr("Move to the next window") );
    //connect( nextAction, SIGNAL(triggered()), ui->mdiArea, SLOT(activateNextSubWindow()) );

    previousAction = new QAction( tr("&Previous window"), this );
    previousAction->setStatusTip( tr("Move to the previous window") );
    //connect( previousAction, SIGNAL(triggered()), ui->mdiArea, SLOT(activatePreviousSubWindow()) );

    closeAction = new QAction( tr("&Close"), this );
    closeAction->setStatusTip( tr("Close Current Document") );
    connect( closeAction, SIGNAL(triggered()), this, SLOT(CloseDoc()) );


    MIDI=new RtMidiOut();
    if (MIDI->getPortCount()>0)
    {
        MIDI->openPort(0);
    }

    //CopyData=new QDomLiteDocument("CopyData","Score");

    SC=new OCSymbolsCollection();

    connect(Action("actionFirst"),SIGNAL(triggered()),this,SLOT(First()));
    connect(Action("actionBack"),SIGNAL(triggered()),this,SLOT(Back()));
    connect(Action("actionForward"),SIGNAL(triggered()),this,SLOT(Forward()));
    connect(Action("actionLast"),SIGNAL(triggered()),this,SLOT(Last()));
    connect(Action("actionTurn_Page"),SIGNAL(triggered()),this,SLOT(TurnPage()));
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
    connect(Action("actionReformat"),SIGNAL(triggered()),this,SLOT(ReformatLayout()));
/*
    QTreeWidget* tw=findChild<QTreeWidget*>();
    tw->setIndentation(12);
    tw->setHeaderHidden(true);
    tw->setUniformRowHeights(false);
    tw->setAnimated(false);
#if defined(__STYLESHEETS__)
    tw->setStyleSheet(widgetgradient);
#endif
    CPropertiesToolGrid* tg=new CPropertiesToolGrid(this);
    tg->SetSymbolsCollection(SC);
    tg->AddButton("Diminuendo","Add Diminuendo","dim",QFont("Times new Roman",13,-1,true),"DynamicChange",tsRedrawActiveStave,false,"DynamicType",0,"Speed","dimspeed");
    tg->AddButton("Crescendo","Add Crescendo","cresc",QFont("Times new Roman",13,-1,true),"DynamicChange",tsRedrawActiveStave,false,"DynamicType",1,"Speed","crescspeed");
    tg->AddButton("ppp","Add Dynamic ppp",QIcon(":/Notes/Notes/ppp.png"),"Dynamic",tsRedrawActiveStave,false,"DynamicSign",0,"Velocity","pppvel");
    tg->AddButton("pp","Add Dynamic pp",QIcon(":/Notes/Notes/pp.png"),"Dynamic",tsRedrawActiveStave,false,"DynamicSign",1,"Velocity","ppvel");
    tg->AddButton("p","Add Dynamic p",QIcon(":/Notes/Notes/p.png"),"Dynamic",tsRedrawActiveStave,false,"DynamicSign",2,"Velocity","pvel");
    tg->AddButton("mp","Add Dynamic mp",QIcon(":/Notes/Notes/mp.png"),"Dynamic",tsRedrawActiveStave,false,"DynamicSign",3,"Velocity","mpvel");
    tg->AddButton("mf","Add Dynamic mf",QIcon(":/Notes/Notes/mf.png"),"Dynamic",tsRedrawActiveStave,false,"DynamicSign",4,"Velocity","mfvel");
    tg->AddButton("f","Add Dynamic f",QIcon(":/Notes/Notes/f.png"),"Dynamic",tsRedrawActiveStave,false,"DynamicSign",5,"Velocity","fvel");
    tg->AddButton("ff","Add Dynamic ff",QIcon(":/Notes/Notes/ff.png"),"Dynamic",tsRedrawActiveStave,false,"DynamicSign",6,"Velocity","ffvel");
    tg->AddButton("fff","Add Dynamic fff",QIcon(":/Notes/Notes/fff.png"),"Dynamic",tsRedrawActiveStave,false,"DynamicSign",7,"Velocity","fffvel");
    tg->AddButton("Accent","Add Accent",">",QFont("Times new Roman",15),"Accent",tsRedrawActiveStave,false,"","","AddToVelocity","accentadd");
    tg->AddButton("fz","Add fz",QIcon(":/Notes/Notes/fz.png"),"fz",tsRedrawActiveStave,false,"","","AddToVelocity","fzadd");
    tg->AddButton("fp","Add fp",QIcon(":/Notes/Notes/fp.png"),"fp",tsRedrawActiveStave,false,"","","StartVelocity","fpstartvel","EndVelocity","fpendvel");
    connect(tg,SIGNAL(PasteProperties(OCProperties*,QString,OCRefreshMode)),this,SLOT(PasteProperties(OCProperties*,QString,OCRefreshMode)));

    tg->AddToTree("Dynamics",tw);

    tg=new CPropertiesToolGrid(this);
    tg->SetSymbolsCollection(SC);
    tg->AddButton("Tempo","Add Tempo",QIcon(":/Notes/Notes/tempo.png"),"Tempo",tsRedrawActiveStave,true);
    tg->AddButton("a tempo","Add atempo","a\ntempo",QFont("Times new Roman",11,-1,true),"TempoChange",tsRedrawActiveStave,false,"TempoType",0);
    tg->AddButton("Rit","Add Ritardando","rit",QFont("Times new Roman",13,-1,true),"TempoChange",tsRedrawActiveStave,false,"TempoType",1,"Speed","ritspeed");
    tg->AddButton("Accel","Add Accelerando","accel",QFont("Times new Roman",13,-1,true),"TempoChange",tsRedrawActiveStave,false,"TempoType",2,"Speed","accelspeed");
    tg->AddButton("Fermata","Add fermata",QIcon(":/Notes/Notes/fermata.png"),"Fermata",tsRedrawActiveStave);
    connect(tg,SIGNAL(PasteProperties(OCProperties*,QString,OCRefreshMode)),this,SLOT(PasteProperties(OCProperties*,QString,OCRefreshMode)));

    tg->AddToTree("Tempo",tw);

    tg=new CPropertiesToolGrid(this);
    tg->SetSymbolsCollection(SC);
    tg->AddButton("Transpose","Add Transpose",QIcon(":/Notes/Notes/transpose.png"),"Transpose",tsRedrawActiveStave,true);
    tg->AddButton("15ma Up","Add 15ma Up",QChar(0x2191)+QString("15"),QFont("Times new Roman",15,-1,true),"Octave",tsRedrawActiveStave,false,"OctaveType",4);
    tg->AddButton("8va Up","Add 8va Up",QChar(0x2191)+QString("8"),QFont("Times new Roman",15,-1,true),"Octave",tsRedrawActiveStave,false,"OctaveType",3);
    tg->AddButton("Loco","Add Loco","loco",QFont("Times new Roman",13,-1,true),"Octave",tsRedrawActiveStave,false,"OctaveType",2);
    tg->AddButton("8va Down","Add 8va Down",QChar(0x2193)+QString("8"),QFont("Times new Roman",15,-1,true),"Octave",tsRedrawActiveStave,false,"OctaveType",1);
    tg->AddButton("15ma Down","Add 15ma Down",QChar(0x2193)+QString("15"),QFont("Times new Roman",15,-1,true),"Octave",tsRedrawActiveStave,false,"OctaveType",0);

    connect(tg,SIGNAL(PasteProperties(OCProperties*,QString,OCRefreshMode)),this,SLOT(PasteProperties(OCProperties*,QString,OCRefreshMode)));

    tg->AddToTree("Pitch",tw);

    tg=new CPropertiesToolGrid(this);
    tg->SetSymbolsCollection(SC);
    tg->AddButton("Repeat Begin","Add Repeat Begin",QIcon(":/Notes/Notes/repeatbegin.png"),"Repeat",tsRedrawActiveStave,false,"RepeatType",1);
    tg->AddButton("Repeat End","Add Repeat End",QIcon(":/Notes/Notes/repeatend.png"),"Repeat",tsRedrawActiveStave,true,"RepeatType",0);
    tg->AddButton("Volta","Add Volta",QIcon(":/Notes/Notes/volte.png"),"Repeat",tsRedrawActiveStave,true,"RepeatType",2);
    tg->AddButton("Segno","Add Segno",QIcon(":/Notes/Notes/segno.png"),"Segno",tsRedrawActiveStave,false,"SegnoType",1);
    tg->AddButton("dal Segno","Add dal Segno","D.S.",QFont("Times new Roman",12,-1,true),"Segno",tsRedrawActiveStave,false,"SegnoType",0);
    tg->AddButton("Coda","Add Coda",QIcon(":/Notes/Notes/coda.png"),"Coda",tsRedrawActiveStave,false,"CodaType",0);
    tg->AddButton("To Coda","Add To Coda","to\nCoda",QFont("Times new Roman",12,-1,true),"Coda",tsRedrawActiveStave,false,"CodaType",1);
    tg->AddButton("Da Capo","Add Da Capo","D.C.",QFont("Times new Roman",12,-1,true),"DaCapo",tsRedrawActiveStave);
    tg->AddButton("Fine","Add Fine","Fine",QFont("Times new Roman",12,-1,true),"Fine",tsRedrawActiveStave);
    connect(tg,SIGNAL(PasteProperties(OCProperties*,QString,OCRefreshMode)),this,SLOT(PasteProperties(OCProperties*,QString,OCRefreshMode)));

    tg->AddToTree("Repeat",tw);

    tg=new CPropertiesToolGrid(this);
    tg->SetSymbolsCollection(SC);
    tg->AddButton("Beam Limit","Add Beam Limit",QIcon(":/Notes/Notes/beamlimit.png"),"BeamLimit",tsRedrawActiveStave,true);
    tg->AddButton("Stem Direction","Add Stem Direction",QIcon(":/Notes/Notes/stemdirection.png"),"StemDirection",tsRedrawActiveStave,true);
    tg->AddButton("Beam Slant","Add Beam Slant",QIcon(":/Notes/Notes/slant.png"),"BeamSlant",tsRedrawActiveStave,true);
    tg->AddButton("FlipTie","Add Flip Ties",QIcon(":/Notes/Notes/slurdirection.png"),"FlipTie",tsRedrawActiveStave,true);
    connect(tg,SIGNAL(PasteProperties(OCProperties*,QString,OCRefreshMode)),this,SLOT(PasteProperties(OCProperties*,QString,OCRefreshMode)));

    tg->AddToTree("Beams",tw);


    tg=new CPropertiesToolGrid(this);
    tg->SetSymbolsCollection(SC);
    tg->AddButton("Tenuto","Add Tenuto",QString(QChar(0x25ac))+"\n"+QString(QChar(0x2192)),QFont("Times new Roman",11),"Length",tsRedrawActiveStave,false,"PerformanceType",1,"Legato","legatolen");
    tg->AddButton("Staccato","Add Staccato",QString(QChar(0x25cf))+"\n"+QString(QChar(0x2192)),QFont("Times new Roman",11),"Length",tsRedrawActiveStave,false,"PerformanceType",2,"Legato","stacclen");
    tg->AddButton("Length","Add Length",QString("Len\n")+QChar(0x2192),QFont("Times new Roman",11),"Length",tsRedrawActiveStave,true,"PerformanceType",0);
    tg->AddButton("Tenuto x1","Add Tenuto x1",QChar(0x25ac)+QString("\nx1"),QFont("Times new Roman",11),"Length",tsRedrawActiveStave,false,"PerformanceType",4,"Legato","legatolen");
    tg->AddButton("Staccato x1","Add Staccato x1",QChar(0x25cf)+QString("\nx1"),QFont("Times new Roman",11),"Length",tsRedrawActiveStave,false,"PerformanceType",5,"Legato","stacclen");
    tg->AddButton("Length x1","Add Length x1","Len\nx1",QFont("Times new Roman",11),"Length",tsRedrawActiveStave,true,"PerformanceType",3);

    connect(tg,SIGNAL(PasteProperties(OCProperties*,QString,OCRefreshMode)),this,SLOT(PasteProperties(OCProperties*,QString,OCRefreshMode)));

    tg->AddToTree("Performance",tw);

    tg=new CPropertiesToolGrid(this);
    tg->SetSymbolsCollection(SC);
    tg->AddButton("Soprano Clef","Add Soprano CLef",QIcon(":/Notes/Notes/Gclef.png"),"Clef",tsReformat,false,"Clef",0);
    tg->AddButton("Bass Clef","Add Bass CLef",QIcon(":/Notes/Notes/Fclef.png"),"Clef",tsReformat,false,"Clef",1);
    tg->AddButton("Alto Clef","Add Alto CLef",QIcon(":/Notes/Notes/Cclef.png"),"Clef",tsReformat,false,"Clef",2);
    tg->AddButton("Tenor Clef","Add Tenor CLef",QIcon(":/Notes/Notes/Tclef.png"),"Clef",tsReformat,false,"Clef",3);
    tg->AddButton("Percussion Clef","Add Perciussion CLef",QIcon(":/Notes/Notes/neutralclef.png"),"Clef",tsReformat,false,"Clef",4);
    tg->AddButton("Time Signature","Add Time Signature",QIcon(":/Notes/Notes/cuttime.png"),"Time",tsReformat,true);
    tg->AddButton("Key Signature","Add Key Signature",QIcon(":/Notes/Notes/keysigflat.png"),"Key",tsReformat,true);
    tg->AddButton("Scale","Add Scale",QIcon(":/Notes/Notes/scale.png"),"Scale",tsRedrawActiveStave,true);
    tg->AddButton("Cue Letter","Add Cue Letter","A",QFont("Times new Roman",28),"Cue",tsReformat);
    tg->AddButton("Bar Width","Add Bar width",QIcon(":/Notes/Notes/barwidth.png"),"BarWidth",tsReformat,true);
    tg->AddButton("Text","Add Text","Text",QFont("Times new Roman",13),"Text",tsRedrawActiveStave,true);
    connect(tg,SIGNAL(PasteProperties(OCProperties*,QString,OCRefreshMode)),this,SLOT(PasteProperties(OCProperties*,QString,OCRefreshMode)));

    tg->AddToTree("Staff",tw);

    tg=new CPropertiesToolGrid(this);
    tg->SetSymbolsCollection(SC);
    tg->AddButton("Patch","Add Patch",QIcon(":/Notes/Notes/patch.png"),"Patch",tsRedrawActiveStave,true);
    tg->AddButton("Channel","Add Channel",QIcon(":/Notes/Notes/channel.png"),"Channel",tsRedrawActiveStave,true);
    tg->AddButton("SysEx","Add SysEx",QIcon(":/Notes/Notes/sysex.png"),"SysEx",tsRedrawActiveStave,true);
    tg->AddButton("Controller","Add Controller",QIcon(":/Notes/Notes/controller.png"),"Controller",tsRedrawActiveStave,true);
    tg->AddButton("Expression","Add Expression",QIcon(":/Notes/Notes/expression.png"),"Expression",tsRedrawActiveStave,true);
    tg->AddButton("Portamento","Add Portamento",QIcon(":/Notes/Notes/synth.png"),"Portamento",tsRedrawActiveStave,true);
    connect(tg,SIGNAL(PasteProperties(OCProperties*,QString,OCRefreshMode)),this,SLOT(PasteProperties(OCProperties*,QString,OCRefreshMode)));

    tg->AddToTree("MIDI",tw);

    tg=new CPropertiesToolGrid(this);
    tg->SetSymbolsCollection(SC);
    tg->AddButton("Finger 0","Add Fingering 0","0",QFont("Times new Roman",13),"Fingering",tsRedrawActiveStave,false,"Finger",0);
    tg->AddButton("Finger 1","Add Fingering 1","1",QFont("Times new Roman",13),"Fingering",tsRedrawActiveStave,false,"Finger",1);
    tg->AddButton("Finger 2","Add Fingering 2","2",QFont("Times new Roman",13),"Fingering",tsRedrawActiveStave,false,"Finger",2);
    tg->AddButton("Finger 3","Add Fingering 3","3",QFont("Times new Roman",13),"Fingering",tsRedrawActiveStave,false,"Finger",3);
    tg->AddButton("Finger 4","Add Fingering 4","4",QFont("Times new Roman",13),"Fingering",tsRedrawActiveStave,false,"Finger",4);
    tg->AddButton("Finger 5","Add Fingering 5","5",QFont("Times new Roman",13),"Fingering",tsRedrawActiveStave,false,"Finger",5);
    tg->AddButton("Stopped","Add Stopped","+",QFont("Times new Roman",20),"Stopped",tsRedrawActiveStave);
    tg->AddButton("Bartok Pizz","Add Bartok Pizz",QIcon(":/Notes/Notes/bartok.png"),"BartokPizz",tsRedrawActiveStave);
    tg->AddButton("Up Bow","Add Up Bow",QIcon(":/Notes/Notes/upbow.png"),"Bowing",tsRedrawActiveStave,false,"Bowing",0);
    tg->AddButton("Down Bow","Add Down Bow",QIcon(":/Notes/Notes/downbow.png"),"Bowing",tsRedrawActiveStave,false,"Bowing",1);

    tg->AddModifierButton("Parentheses","Add Parentheses to Accidentals","( )",QFont("Times new Roman",15));
    tg->AddButton("Flat","Add Flat",QIcon(":/Notes/Notes/flat.png"),"Accidental",tsRedrawActiveStave,false,"AccidentalSign",0,"","","","","Parentheses");
    tg->AddButton("Sharp","Add Sharp",QIcon(":/Notes/Notes/sharp.png"),"Accidental",tsRedrawActiveStave,false,"AccidentalSign",1,"","","","","Parentheses");
    tg->AddButton("Double Flat","Add Duble Flat",QIcon(":/Notes/Notes/doubleflat.png"),"Accidental",tsRedrawActiveStave,false,"AccidentalSign",2,"","","","","Parentheses");
    tg->AddButton("Double Sharp","Add Double Sharp",QIcon(":/Notes/Notes/doublesharp.png"),"Accidental",tsRedrawActiveStave,false,"AccidentalSign",3,"","","","","Parentheses");
    tg->AddButton("Natural","Add Natural",QIcon(":/Notes/Notes/natural.png"),"Accidental",tsRedrawActiveStave,false,"AccidentalSign",4,"","","","","Parentheses");
    tg->AddButton("Trill","Add Trill",QIcon(":/Notes/Notes/trill.png"),"Trill",tsRedrawActiveStave,false,"TrillType",0,"Speed","trillspeed");
    tg->AddButton("Trill b","Add Trill b",QIcon(":/Notes/Notes/trillb.png"),"Trill",tsRedrawActiveStave,false,"TrillType",1,"Speed","trillspeed","Range","1");
    tg->AddButton("Trill #","Add Trill #",QIcon(":/Notes/Notes/trillx.png"),"Trill",tsRedrawActiveStave,false,"TrillType",2,"Speed","trillspeed");
    tg->AddButton("Gliss","Add Glissando",QIcon(":/Notes/Notes/glissando.png"),"Glissando",tsRedrawActiveStave,true);
    tg->AddButton("Tremolo","Add Tremolo",QIcon(":/Notes/Notes/tremolo.png"),"Tremolo",tsRedrawActiveStave,false,"Beams",3,"Speed","trillspeed");
    tg->AddButton("Turn Up Down","Add Turn Up Down",QIcon(":/Notes/Notes/turnud.png"),"Turn",tsRedrawActiveStave,false,"Direction",0,"Speed","trillspeed");
    tg->AddButton("Turn Down Up","Add Turn Down Up",QIcon(":/Notes/Notes/turndu.png"),"Turn",tsRedrawActiveStave,false,"Direction",1,"Speed","trillspeed");
    tg->AddButton("Mordent Down","Add Mordent Down",QIcon(":/Notes/Notes/mordent1.png"),"Mordent",tsRedrawActiveStave,false,"Range",-2,"Speed","trillspeed");
    tg->AddButton("Mordent Up","Add Mordent Up",QIcon(":/Notes/Notes/mordent.png"),"Mordent",tsRedrawActiveStave,false,"Range",2,"Speed","trillspeed");

    connect(tg,SIGNAL(PasteProperties(OCProperties*,QString,OCRefreshMode)),this,SLOT(PasteProperties(OCProperties*,QString,OCRefreshMode)));

    tg->AddToTree("Other",tw);

    tg=new CPropertiesToolGrid(this);
    tg->SetSymbolsCollection(SC);
    tg->AddButton("Crescendo","Add Crescendo",QIcon(":/Notes/Notes/hpcresc.png"),"Hairpin",tsRedrawActiveStave,false,"HairpinType",0,"Speed","crescspeed");
    tg->AddButton("Diminuendo","Add Diminuendo",QIcon(":/Notes/Notes/hpdim.png"),"Hairpin",tsRedrawActiveStave,false,"HairpinType",1,"Speed","dimspeed");
    tg->AddButton("'Fish'","Add 'Fish'","<>",QFont("Times new Roman",20,700),"Hairpin",tsRedrawActiveStave,false,"HairpinType",3,"Speed","crescspeed");
    tg->AddButton("Inverted 'Fish'","Add Inverted 'Fish'","><",QFont("Times new Roman",20,700),"Hairpin",tsRedrawActiveStave,false,"HairpinType",2,"Speed","dimspeed");
    tg->AddButton("Length","Add Length","Len",QFont("Times new Roman",13),"DuratedLength",tsRedrawActiveStave,false,"PerformanceType",0);
    tg->AddButton("Legato","Add Legato",QIcon(":/Notes/Notes/durlegato.png"),"DuratedLength",tsRedrawActiveStave,false,"PerformanceType",1,"Legato","legatolen");
    tg->AddButton("Staccato","Add Staccato",QIcon(":/Notes/Notes/durstaccato.png"),"DuratedLength",tsRedrawActiveStave,false,"PerformanceType",2,"Legato","stacclen");
    tg->AddButton("Slur Down","Add Slur Down",QIcon(":/Notes/Notes/legato.png"),"Slur",tsRedrawActiveStave,false,"Direction",0);
    tg->AddButton("Slur Up","Add Slur Up",QIcon(":/Notes/Notes/legatoup.png"),"Slur",tsRedrawActiveStave,false,"Direction",1);
    tg->AddButton("Beam Slanting On","Add Beam Slanting On",QIcon(":/Notes/Notes/slanton.png"),"DuratedSlant",tsRedrawActiveStave,false,"Slanting",true);
    tg->AddButton("Beam Slanting Off","Add Beam Slanting Off",QIcon(":/Notes/Notes/slantoff.png"),"DuratedSlant",tsRedrawActiveStave,false,"Slanting",false);
    tg->AddButton("Stems Up","Add Stems Up",QIcon(":/Notes/Notes/stemsup.png"),"DuratedBeamDirection",tsRedrawActiveStave,false,"Direction",0);
    tg->AddButton("Stems Down","Add Stems Down",QIcon(":/Notes/Notes/stemsdown.png"),"DuratedBeamDirection",tsRedrawActiveStave,false,"Direction",1);
    tg->AddButton("Tuplet","Add Tuplet",QIcon(":/Notes/Notes/tuplet.png"),"Tuplet",tsRedrawActiveStave,true);
    tg->AddButton("Beam","Add Beam",QIcon(":/Notes/Notes/beam.png"),"Beam",tsRedrawActiveStave,true);
    connect(tg,SIGNAL(PasteProperties(OCProperties*,QString,OCRefreshMode)),this,SLOT(PasteDuratedProperties(OCProperties*,QString,OCRefreshMode)));

    tg->AddToTree("Durated",tw);
    Durated=tg;
    */
    NewDoc();

    //DocumentActivated(ui->mdiArea->currentIndex());
    this->setUnifiedTitleAndToolBarOnMac(true);
    grabGesture(Qt::PinchGesture);
    //grabGesture(Qt::SwipeGesture);
    //grabGesture(Qt::PanGesture);
    //this->show();
    //this->showMaximized();
    this->setGeometry(QApplication::desktop()->screenGeometry());
    TranspFrame=0;
    /*
    for (int i=0;i<tw->topLevelItemCount();i++)
    {
        if (P.GetValue(tw->topLevelItem(i)->text(0)+"_tab_closed").toBool())
        {
            tw->topLevelItem(i)->setExpanded(false);
        }
    }
    */
}

bool MainWindow::event(QEvent *event)
 {
     if (event->type() == QEvent::Gesture)
         return gestureEvent(static_cast<QGestureEvent*>(event));
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
    if (CurrentDoc()==0) return;
    if (CurrentDoc()->GetView()==0)
    {
        if (gesture->state() == Qt::GestureFinished)
        {
            float d=gesture->delta().x();
            if (d==0) return;
            //float v=gesture->acceleration();
            if (d < 0)
            {
                Action("actionTurn_Page")->trigger();
            }
            else if (d > 0)
            {
                //Action("actionBack")->trigger();
                CurrentDoc()->sv->turnback();
            }
        }
    }
}

void MainWindow::swipeTriggered(QSwipeGesture *gesture)
{
    /*
    if (gesture->state() == Qt::GestureFinished)
    {
        if (gesture->horizontalDirection() == QSwipeGesture::Left)
        {
            Action("actionForward")->trigger();
        }
        else if (gesture->horizontalDirection() == QSwipeGesture::Right)
        {
            Action("actionBack")->trigger();
        }
     }
     */
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
    ui->mdiArea->setCurrentWidget(w);
}

void MainWindow::ShowPresets()
{
    CPresetsDialog d(this);
    d.setWindowTitle("Presets");
    CPropertyWindow* pw=d.findChild<CPropertyWindow*>();
    CPresets P;
    pw->Fill(P.Properties());
    if (d.exec()==QDialog::Accepted)
    {
        P.SaveProperties();
    }
}

void MainWindow::NewDoc()
{
    document=new CScoreDoc(SC,this);
    document->setWindowTitle("Untitled");
    ui->mdiArea->addWidget(document);
    ui->mdiArea->setCurrentWidget(document);
    document->showMaximized();
}

void MainWindow::CloseDoc()
{
    if (CurrentDoc()==0) return;
    if (ui->mdiArea->count()>1)
    {
        QWidget* sw=CurrentDoc();
        if (sw->close())
        {
            ui->mdiArea->removeWidget(sw);
            delete sw;
            ui->mdiArea->setCurrentIndex(0);
        }
    }
}

void MainWindow::ShowProgress(int Steps)
{
    TranspFrame=new QCustomFrame(this);
    TranspFrame->setGeometry(this->centralWidget()->geometry());
    ui->toolBar->setEnabled(false);
    ui->menuBar->setEnabled(false);
    TranspFrame->show();
    TranspFrame->raise();
    progressbar=new QProgressBar(TranspFrame);
    progressbar->setGeometry((this->centralWidget()->width()/2)-150,(this->centralWidget()->height()/2)-14,300,28);
    progressbar->setMaximum(Steps-1);
    progressbar->setValue(0);
    progressbar->show();
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

void MainWindow::UpdateProgress()
{
    progressbar->setValue(progressbar->value()+1);
    TranspFrame->raise();
    progressbar->repaint();
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    //progressbar->repaint();
}

void MainWindow::HideProgress()
{
    TranspFrame->raise();
    progressbar->repaint();
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    progressbar->hide();
    TranspFrame->hide();
    delete progressbar;
    delete TranspFrame;
    TranspFrame=0;
    ui->toolBar->setEnabled(true);
    ui->menuBar->setEnabled(true);
}

void MainWindow::OpenDoc(QString Path)
{
    //QMdiArea* mdiArea=findChild<QMdiArea*>();
    for (int i=ui->mdiArea->count()-1;i>=0;i--)
    {
        QWidget* sw=ui->mdiArea->widget(i);
        if (sw->windowTitle()==Path)
        {
            //CScoreDoc* document=sw->findChild<CScoreDoc*>();
            CScoreDoc* document=(CScoreDoc*)sw;
            if (document->UndoText().length())
            {
                QMessageBox* d=new QMessageBox(QMessageBox::Warning,apptitle,"The document '" + document->windowTitle() + "' is already open.\nDo you want to save your changes?",QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,this,Qt::Sheet);
                d->setWindowModality(Qt::WindowModal);
                int ret=d->exec();
                delete d;
                //int ret=QMessageBox::warning(this, apptitle,"The document '" + document->windowTitle() + "' is already open.\nDo you want to save your changes?",QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
                if (ret==QMessageBox::Save)
                {
                    SaveDoc();
                    ui->mdiArea->setCurrentWidget(sw);
                    return;
                }
                if (ret==QMessageBox::Cancel)
                {
                    return;
                }
                ui->mdiArea->removeWidget(sw);
                //mdiArea->subWindowList.removeAt(i);
                delete sw;
            }
            else
            {
                ui->mdiArea->setCurrentWidget(sw);
                return;
            }
        }
    }
    ShowProgress(8);
    document=new CScoreDoc(SC,this);
    UpdateProgress();
    document->setWindowTitle(Path);
    document->Path=Path;
    document->sv->Load(Path);
    UpdateProgress();
    document->lv->Load(document->sv->GetXML());
    UpdateProgress();
    document->UpdateLayoutList();
    UpdateProgress();
    document->SetCurrentLayout(0);
    UpdateProgress();
    ui->mdiArea->addWidget(document);
    ui->mdiArea->setCurrentWidget(document);
    UpdateProgress();
    document->showMaximized();
    UpdateProgress();
    document->UpdateAll();
    UpdateProgress();
    AddRecent(Path);
    UpdateSidebar();
    SidebarSetSelected();
    HideProgress();
}

void MainWindow::RequestMidiFile(QString &path)
{
    if (CurrentDoc()==0) return;
    int StartBar=0;
    if (CurrentDoc()->GetView()==0) StartBar=CurrentDoc()->sv->StartBar();
    CurrentDoc()->sv->Play(StartBar,0,true,midifl);
    path=midifl;
}

void MainWindow::OpenDialog()
{
    QFileDialog* d=new QFileDialog(this,Qt::Sheet);
    //d->setWindowModality(Qt::WindowModal);
    d->setWindowTitle("Open Score");
    d->setFilter(tr("XML Score Files (*.mus *.xml)"));
    if (d->exec()==QDialog::Accepted)
    {
        for (int i=0;i<d->selectedFiles().count();i++)
        {
            OpenDoc(d->selectedFiles()[i]);
        }
    }
    delete d;
}

bool MainWindow::SaveDoc()
{
    if (CurrentDoc()==0) return false;
    if (CurrentDoc()->Path.length()==0)
    {
        return SaveDialog();
    }
    if (!CurrentDoc()->sv->Save(CurrentDoc()->Path))
    {
        QMessageBox* d=new QMessageBox(QMessageBox::Warning,"Object Composer XML","Could not Save File! It might be Read Only!",QMessageBox::Ok,this,Qt::Sheet);
        d->setWindowModality(Qt::WindowModal);
        //QMessageBox::warning(this, "Object Composer XML","Could not Save File! It might be Read Only!",QMessageBox::Ok, QMessageBox::Ok);
        d->exec();
        delete d;
    }
    AddRecent(CurrentDoc()->Path);
    UpdateSidebar();
    return true;
}

bool MainWindow::SaveDialog()
{
    if (CurrentDoc()==0) return false;
    QFileDialog d(this,Qt::Sheet);
    //d.setWindowModality(Qt::WindowModal);
    d.setWindowTitle("Save Score");
    d.setFilter(tr("XML Score Files (*.mus *.xml)"));
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setFileMode(QFileDialog::AnyFile);
    if (d.exec() != QDialog::Accepted) return false;
    if (d.selectedFiles().count())
    {
        QString filename=d.selectedFiles().first();
        //CurrentDoc()->sv->Save(filename);
        CurrentDoc()->setWindowTitle(filename);
        CurrentDoc()->Path=filename;
        //AddRecent(filename);
        return SaveDoc();
    }
    return false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //QMdiArea* ma=findChild<QMdiArea*>();
    for (int i=ui->mdiArea->count()-1;i>=0;i--)
    {
        QWidget* sw=ui->mdiArea->widget(i);
        //CScoreDoc* doc=sw->findChild<CScoreDoc*>();
        CScoreDoc* doc=(CScoreDoc*)sw;
        bool Cancel=false;
        DocumentClosed(doc,Cancel);
        if (Cancel)
        {
            event->ignore();
            return;
        }
        ui->mdiArea->removeWidget(sw);
        //ma->subWindowList.removeAt(i);
        delete sw;
        //ma->subWindowList()[i]->close();
    }
}

void MainWindow::LoadRecent()
{
    QMenu* recentMenu=findChild<QMenu*>("menuRecent_Documents");
    recentMenu->clear();
    foreach (QAction* a,RecentActions) RecentMapper->removeMappings(a);
    qDeleteAll(RecentActions);
    RecentActions.clear();
    CPresets P("RecentDocuments");
    RecentFiles=P.GetValue("RecentDocuments").toStringList();
    /*
    RecentFiles.clear();
    CPresets P("RecentDocuments");
    int RecentCount=P.GetValue("RecentCount",0).toInt();
    for (int i=0;i<RecentCount;i++)
    {
        if (i>=recentmax) break;
        QString s=P.GetString("RecentDocument"+QString::number(i));
        if (s.length())
        {
            RecentFiles.append(s);
            QAction* a=new QAction(QString::number(i+1)+" "+s,this);
            a->setEnabled(QFileInfo(s).exists());
            connect( a, SIGNAL(triggered()), RecentMapper, SLOT(map()) );
            RecentMapper->setMapping(a,s);
            RecentActions.append(a);
        }
    }
    */
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
    RecentFiles.insert(0,Path);
    while (RecentFiles.count()>recentmax)
    {
        RecentFiles.removeLast();
    }

    QMenu* recentMenu=findChild<QMenu*>("menuRecent_Documents");
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
    StatusLabel->setText("");
    UpdateWindowList();
    UpdateSidebar();
    UpdateStatus();
    setWindowTitle(apptitle);
    if (document->UndoText().length())
    {
        QMessageBox* d= new QMessageBox(QMessageBox::Warning, apptitle,"The document '" + document->windowTitle() + "' has been modified.\nDo you want to save your changes?",QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, this,Qt::Sheet);
        d->setWindowModality(Qt::WindowModal);
        int ret=d->exec();
        delete d;
        if (ret==QMessageBox::Save)
        {
            Cancel=!SaveDoc();
        }
        if (ret==QMessageBox::Cancel)
        {
            Cancel=true;
        }
    }
}

void MainWindow::LayoutsChanged()
{
    UpdateSidebar();
    UpdateStatus();
}

void MainWindow::DocumentActivated(int Index)
{
    //if (doc==0) return;
    if (Index==-1) return;
    for (int i=0;i<ui->mdiArea->count();i++)
    {
        CScoreDoc* document=(CScoreDoc*)ui->mdiArea->widget(i);
        document->disconnect(this);
        document->sv->disconnect(this);
        document->lv->disconnect(this);
        CPropertyWindow* pw=document->findChild<CPropertyWindow*>();
        CMusicTree* mt=document->findChild<CMusicTree*>();
        CBarWindow* bw=document->findChild<CBarWindow*>();
        OCPiano* pno=document->findChild<OCPiano*>();
        pw->disconnect(this);
        mt->disconnect(this);
        bw->disconnect(this);
        pno->disconnect(this);
    }
    ZoomSlider->disconnect();
    btnZoomOut->disconnect();
    btnZoomIn->disconnect();
    //CScoreDoc* document=doc->findChild<CScoreDoc*>();
    CScoreDoc* document=(CScoreDoc*)ui->mdiArea->widget(Index);
    //CScoreDoc* document=(CScoreDoc*)doc;
    CPropertyWindow* pw=document->findChild<CPropertyWindow*>();
    CMusicTree* mt=document->findChild<CMusicTree*>();
    CBarWindow* bw=document->findChild<CBarWindow*>();
    OCPiano* pno=document->findChild<OCPiano*>();

    connect(pno,SIGNAL(NoteOn(int)),this,SLOT(NoteOn(int)));
    connect(pno,SIGNAL(NoteOff(int)),this,SLOT(NoteOff(int)));

    connect(document->sv,SIGNAL(NoteOn(int)),this,SLOT(NoteOn(int)));
    connect(document->sv,SIGNAL(NoteOff(int)),this,SLOT(NoteOff(int)));
    connect(document,SIGNAL(Close(CScoreDoc*,bool&)),this,SLOT(DocumentClosed(CScoreDoc*,bool&)));

    connect(document->sv,SIGNAL(SelectionChanged()),this,SLOT(SelectionChanged()));
    connect(document->sv,SIGNAL(BarChanged(int)),this,SLOT(BarChanged(int)));
    connect(document->lv,SIGNAL(SelectionChanged()),this,SLOT(UpdateStatus()));
    connect(document->lv,SIGNAL(Popup(QPoint)),this,SLOT(ShowLayoutPopup(QPoint)));
    connect(document,SIGNAL(LayoutsChanged()),this,SLOT(LayoutsChanged()));
    connect(pw,SIGNAL(Changed(QString,QVariant)),this,SLOT(ChangeProperty(QString,QVariant)));
    connect(document->sv,SIGNAL(Popup(QPoint)),this,SLOT(ShowPropertiesPopup(QPoint)));
    connect(mt,SIGNAL(Popup(QPoint)),this,SLOT(ShowPropertiesPopup(QPoint)));
    connect(bw,SIGNAL(Popup(QPoint)),this,SLOT(ShowPropertiesPopup(QPoint)));
    connect(mt,SIGNAL(SelectionChanged()),this,SLOT(TreeSelectionChanged()));
    connect(mt,SIGNAL(Delete(int)),this,SLOT(DeleteItem(int)));
    connect(bw,SIGNAL(BarChanged(int,int,int)),this,SLOT(SetBar(int,int,int)));
    connect(document,SIGNAL(Changed()),this,SLOT(DocChanged()));
    connect(document,SIGNAL(ScoreChanged()),this,SLOT(ScoreChanged()));
    connect(bw,SIGNAL(SelectionChanged(QRect)),this,SLOT(BarSelectionChanged(QRect)));
    connect(ZoomSlider,SIGNAL(valueChanged(int)),document,SLOT(SetZoom(int)));
    connect(btnZoomOut,SIGNAL(clicked()),this,SLOT(ZoomOut()));
    connect(btnZoomIn,SIGNAL(clicked()),this,SLOT(ZoomIn()));

    UpdateTree();
    UpdateSV();
    UpdateBW();
    UpdatePW();
    UpdateWindowList();
    UpdateSidebar();
    UpdateStatus();
    DocChanged();
    setWindowTitle(document->windowTitle() + " - " + apptitle);
}

CScoreDoc* MainWindow::CurrentDoc()
{
    QWidget* win = ui->mdiArea->currentWidget();
    if (win==0)
    {
        win=ui->mdiArea->widget(0);
    }
    if (win==0) return 0;
    return (CScoreDoc*)win;
    //return win->findChild<CScoreDoc *>();
}

void MainWindow::SetBar(int Bar, int Staff, int Voice)
{
    QComboBox* cbStaffs=CurrentDoc()->findChild<QComboBox*>("cbStaffs");
    QComboBox* cbVoices=CurrentDoc()->findChild<QComboBox*>("cbVoices");
    cbStaffs->setCurrentIndex(Staff);
    cbVoices->setCurrentIndex(Voice);
    CurrentDoc()->sv->setStartBar(Bar);
    CurrentDoc()->sv->Paint(RedrawAll,true);
    CurrentDoc()->sv->EnsureVisible();
    UpdateStatus();
}

void MainWindow::ChangeProperty(QString Name,QVariant Value)
{
    if (CurrentDoc()==0) return;
    ScoreViewXML* sv=CurrentDoc()->sv;
    CurrentDoc()->MakeBackup("Change Property");
    OCProperties* Properties=sv->GetProperties(sv->Cursor.GetPos());
    if (Properties->GetItem(Name)->PropertyType==pwCustom)
    {
        OCRefreshMode RefreshMode;
        if (sv->EditProperties(Properties,RefreshMode))
        {
            sv->SetProperties(sv->Cursor.GetPos(),Properties);
            sv->Paint((SVDrawMode)RefreshMode);
            UpdateBW();
            UpdateTree();
            UpdateStatus();
            DocChanged();
        }
        return;
    }
    for (int i=sv->Cursor.SelStart();i<=sv->Cursor.SelEnd();i++)
    {
        OCProperties* Properties=sv->GetProperties(i);
        if (Properties->Exist(Name))
        {
            OCProperty* p=Properties->GetItem(Name);
            p->SetValue(Value);
            sv->SetProperties(i,Properties);
        }
    }
    sv->Paint(RedrawAll);
    UpdateBW();
    UpdateTree();
    UpdateStatus();
    DocChanged();
}
/*
void MainWindow::PasteProperties(OCProperties* Properties, QString UndoText, bool Finished)
{
    if (CurrentDoc()==0) return;
    if (UndoText.length()) CurrentDoc()->MakeBackup(UndoText);
    CurrentDoc()->sv->PasteProperties(Properties);
    if (Finished)
    {
        CurrentDoc()->sv->Paint(RedrawAll);
        UpdateBW();
        UpdateTree();
        UpdateStatus();
        DocChanged();
        CurrentDoc()->sv->setFocus();
    }
}

void MainWindow::PasteProperties(OCProperties* Properties, QString UndoText, OCRefreshMode RefreshMode)
{
    if (CurrentDoc()==0) return;
    if (UndoText.length()) CurrentDoc()->MakeBackup(UndoText);
    CurrentDoc()->sv->PasteProperties(Properties);
    CurrentDoc()->sv->Paint((SVDrawMode)RefreshMode);
    UpdateBW();
    UpdateTree();
    UpdateStatus();
    DocChanged();
    CurrentDoc()->sv->setFocus();
}

void MainWindow::PasteDuratedProperties(OCProperties* Properties, QString UndoText, OCRefreshMode RefreshMode)
{
    if (CurrentDoc()==0) return;
    int CountIt=0;
    ScoreViewXML* sv=CurrentDoc()->sv;
    for (int py=sv->Cursor.SelStart();py<sv->Cursor.SelEnd();py++)
    {
        OCProperties* p=sv->GetProperties(py);
        if (p->Exist("NoteValue"))
        {
            if ((p->GetValue("NoteType")==tsnote) || (p->GetValue("NoteType")==tstiednote))
            {
                CountIt+=Ticks(p->GetValue("NoteValue").toInt(),p->GetValue("Dotted").toBool(),p->GetValue("Triplet").toBool());
            }
        }
    }
    if (Properties->Exist("Ticks")) Properties->SetValue("Ticks",CountIt);
    if (UndoText.length()) CurrentDoc()->MakeBackup(UndoText);
    int SE=sv->Cursor.SelEnd();
    sv->Cursor.SetPos(sv->Cursor.SelStart());
    sv->Cursor.SetMode(CMInsert);
    sv->PasteProperties(Properties);
    sv->Cursor.ExtendSel(SE+1);
    sv->Paint((SVDrawMode)RefreshMode);
    UpdateBW();
    UpdateTree();
    UpdateStatus();
    DocChanged();
    sv->setFocus();
}

void MainWindow::OverwriteProperty(QString Name, QVariant Value, QString UndoText, bool Finished)
{
    if (CurrentDoc()==0) return;
    ScoreViewXML* sv=CurrentDoc()->sv;
    if (sv->Cursor.GetMode()==CMInsert) return;
    if (UndoText.length()) CurrentDoc()->MakeBackup(UndoText);
    for (int i=sv->Cursor.SelStart();i<=sv->Cursor.SelEnd();i++)
    {
        OCProperties* Properties=sv->GetProperties(i);
        if (Properties->Exist(Name))
        {
            OCProperty* p=Properties->GetItem(Name);
            p->SetValue(Value);
            sv->SetProperties(i,Properties);
        }
    }
    if (Finished)
    {
        sv->Paint(RedrawAll);
        UpdateBW();
        UpdateTree();
        UpdateStatus();
        DocChanged();
    }
}
*/
void MainWindow::ResetPositions()
{
    if (CurrentDoc()==0) return;
    ScoreViewXML* sv=CurrentDoc()->sv;
    CurrentDoc()->MakeBackup(Action("actionReset_Position")->text());
    CopyIsBars=CurrentDoc()->SelectionStaff>-1;
    QStringList l=QStringList() << "Left" << "Top" << "Size";
    if (CopyIsBars)
    {
        for (int i=0;i<CurrentDoc()->SelectionList.count();i++)
        {
            for (int p=CurrentDoc()->SelectionList[i].first;p<=CurrentDoc()->SelectionList[i].second;p++)
            {
                sv->ChangeProperties(p,CurrentDoc()->sv->MarkToStaff(CurrentDoc()->SelectionStaff+i),CurrentDoc()->sv->MarkToVoice(CurrentDoc()->SelectionStaff+i),l,0);
            }
        }
    }
    else
    {
        for (int i=sv->Cursor.SelStart();i<=sv->Cursor.SelEnd();i++)
        {
            sv->ChangeProperties(i,l,0);
        }
    }
    CurrentDoc()->sv->Paint(RedrawAll);
    UpdatePW();
    UpdateTree();
    UpdateStatus();
    DocChanged();
}

void MainWindow::ResetSystemPosition()
{
    if (CurrentDoc()==0) return;
    LayoutViewXML* lv=CurrentDoc()->lv;
    CurrentDoc()->MakeBackup(Action("actionReset_Position_2")->text());
    lv->lc.CurrentLayout->ActivePage->ActiveSystem->Top=lv->lc.CurrentLayout->ActivePage->ActiveSystem->DefaultTop;
    lv->Save(CurrentDoc()->sv->GetXML());
    CurrentDoc()->UpdateLayoutView();
}

void MainWindow::SelectAll()
{
    if (CurrentDoc()==0) return;
    CopyIsBars=CurrentDoc()->SelectionStaff>-1;
    if (CopyIsBars)
    {
        CBarWindow* bw=CurrentDoc()->findChild<CBarWindow*>();
        bw->SelectAll();
    }
    else
    {
        CurrentDoc()->sv->Cursor.SetPos(0);
        CurrentDoc()->sv->Cursor.ExtendSel(CurrentDoc()->sv->VoiceLen());
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
        CBarWindow* bw=CurrentDoc()->findChild<CBarWindow*>();
        bw->SelectFromStart();
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
        CBarWindow* bw=CurrentDoc()->findChild<CBarWindow*>();
        bw->SelectToEnd();
    }
    else
    {
        CurrentDoc()->sv->Cursor.ExtendSel(CurrentDoc()->sv->VoiceLen());
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
    UpdateBW();
    UpdateStatus();
    CurrentDoc()->SelectionList=CurrentDoc()->sv->SelectionList();
    CurrentDoc()->SelectionStaff=-1;
    Action("actionCut")->setText("Cut Voice");
    Action("actionCopy")->setText("Copy Voice");
}

void MainWindow::BarChanged(int bar)
{
    UpdateBW();
    UpdateStatus();
}

void MainWindow::TreeSelectionChanged()
{
    UpdatePW();
    UpdateSV();
    UpdateBW();
    UpdateStatus();
    CurrentDoc()->SelectionList==CurrentDoc()->sv->SelectionList();
    CurrentDoc()->SelectionStaff=-1;
    Action("actionCut")->setText("Cut Voice");
    Action("actionCopy")->setText("Copy Voice");
}

void MainWindow::BarSelectionChanged(QRect Selection)
{
    CurrentDoc()->SelectionList=CurrentDoc()->sv->SelectionList(Selection.left(),Selection.right(),Selection.top(),Selection.bottom());
    CurrentDoc()->SelectionStaff=Selection.top();
    CurrentDoc()->SelectionBar=Selection.left();
    Action("actionCut")->setText("Cut Voices");
    Action("actionCopy")->setText("Copy Voices");
}

void MainWindow::ShowScore()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->SetView(0);
    TabChanged(0);
}

void MainWindow::ShowLayout()
{
    if (CurrentDoc()==0) return;
    if (CurrentDoc()->lv->NumOfLayouts()==0)
    {
        if (!CurrentDoc()->AddLayout()) return;
    }
    CurrentDoc()->SetView(1);
    TabChanged(1);
}

void MainWindow::TabChanged(int Index)
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
        QMenu* lm=findChild<QMenu*>("menuLayouts");
        for (int i=0;i<lm->actions().count();i++) lm->actions()[i]->setEnabled(CurrentDoc()->lv->NumOfLayouts()>0);
    }
}

void MainWindow::UpdateTree()
{
    ScoreViewXML* sv=CurrentDoc()->sv;
    CMusicTree* mt=CurrentDoc()->findChild<CMusicTree*>();
    mt->Fill(sv->GetXMLScore(),sv->StartBar(),sv->ActiveStaff(),sv->ActiveVoice(),sv->FindPointerToBar(sv->StartBar()),sv->FindPointerToBar(sv->StartBar()+sv->BarsActuallyDisplayed()),sv->BarsActuallyDisplayed(),sv->FindFirstMeter(),SC,&sv->Cursor);
}

void MainWindow::UpdateSV()
{
    CurrentDoc()->sv->Paint(RedrawActiveStaff);
}

void MainWindow::UpdatePW()
{
    ScoreViewXML* sv=CurrentDoc()->sv;
    CPropertyWindow* pw=CurrentDoc()->findChild<CPropertyWindow*>();
    pw->Fill(sv->GetProperties(sv->Cursor.GetPos()));
}

void MainWindow::UpdateBW()
{
    CBarWindow* bw=CurrentDoc()->findChild<CBarWindow*>();
    bw->Fill(CurrentDoc()->sv);
}

void MainWindow::ShowMenu(QString Name,bool Show)
{
    QMenu* m=findChild<QMenu*>(Name);
    m->setEnabled(Show);
    for (int i=0;i<m->actions().count();i++) m->actions()[i]->setVisible(Show);
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
    //QTabWidget* Tabs=CurrentDoc()->findChild<QTabWidget*>();
    int View=CurrentDoc()->GetView();
    //QStatusBar* sb = findChild<QStatusBar*>();

    ShowAction("actionScore",View==1);
    ShowAction("actionLayouts",View==0);
    ShowMenu("menuScore",View==0);
    ShowMenu("menuLayouts",View==1);

    //QToolBar* tb=findChild<QToolBar*>("toolBarScore");
    //tb->setVisible(View==0);
    //tb=findChild<QToolBar*>("toolBarLayout");
    //tb->setVisible(View==1);
    /*
    for (int i=0;i<ScoreActions.count();i++)
    {
        ScoreActions[i]->setVisible(View==0);
    }
    for (int i=0;i<LayoutActions.count();i++)
    {
        LayoutActions[i]->setVisible(View==1);
    }
    */
    /*
    ShowAction("actionFirst",View==0);
    ShowAction("actionBack",View==0);
    ShowAction("actionForward",View==0);
    ShowAction("actionLast",View==0);
    ShowAction("actionTurn_Page",View==0);
    ShowAction("actionZoom_In",View==0);
    ShowAction("actionZoom_Out",View==0);
    ShowAction("actionPlay",View==0);
    ShowAction("actionSettings",View==0);
    */
    ShowAction("actionCut",View==0);
    ShowAction("actionCopy",View==0);
    ShowAction("actionPaste",View==0);
    ShowAction("actionReset_Position",View==0);

    Action("actionPrint_Preview")->setEnabled(View==1);
    Action("actionPrint")->setEnabled(View==1);
    //Action("editSeparator1")->setVisible(View==0);
    //Action("editSeparator2")->setVisible(View==0);
    /*
    ShowAction("actionPrint_Preview",View==1);
    ShowAction("actionPrint",View==1);
    ShowAction("actionPage_Setup",View==1);
    ShowAction("actionGet_1_Bar_from_Next_System",View==1);
    ShowAction("actionMove_1_Bar_to_Next_System",View==1);
    ShowAction("actionGet_1_System_from_Next_Page",View==1);
    ShowAction("actionMove_1_System_to_Next_Page",View==1);
    ShowAction("actionFit_Systems",View==1);
    ShowAction("actionToggle_Names",View==1);
    ShowAction("actionEdit_Staffs",View==1);
    */
    ZoomSlider->blockSignals(true);
    ZoomSlider->setValue(CurrentDoc()->GetZoom());
    //connect(ZoomSlider,SIGNAL(valueChanged(int)),CurrentDoc(),SLOT(SetZoom(int)));
    ZoomSlider->blockSignals(false);
    RightSideButton->setVisible(View==1);
    if (View==1)
    {
        if (CurrentDoc()->spSidebar->isCollapsed())
        {
            RightSideButton->setIcon(QIcon(":/showright.png"));
        }
        else
        {
            RightSideButton->setIcon(QIcon(":/hideright.png"));
        }
    }
    ui->CloseDocument->setEnabled(ui->mdiArea->count()>1);
    closeAction->setEnabled(ui->mdiArea->count()>1);

    if (View==0)
    {
        //sb->clearMessage();
        QString msg=CurrentDoc()->sv->StaffName(CurrentDoc()->sv->ActiveStaff());
        msg += " - Voice " + QString::number(CurrentDoc()->sv->ActiveVoice() + 1);
        msg += " - Bar " + QString::number(CurrentDoc()->sv->StartBar() + 1);
        msg += (CurrentDoc()->sv->Cursor.GetMode()==CMInsert) ? " - INSERT":" - OVERWRITE";
        //sb->showMessage(msg);
        StatusLabel->setText(msg);

        ScoreViewXML* sv=CurrentDoc()->sv;

        //Action("actionPaste")->setEnabled(CopyData->documentElement->childCount() != 0);
        QString docType=QApplication::clipboard()->text().left(25);
        Action("actionPaste")->setEnabled(docType.contains("<!DOCTYPE OCCopyDataVoice"));
        if (docType.contains("<!DOCTYPE OCCopyDataVoices>"))
        {
            Action("actionPaste")->setText("Paste Voices");
        }
        else if (docType.contains("<!DOCTYPE OCCopyDataVoice>"))
        {
            Action("actionPaste")->setText("Paste Voice");
        }
        bool EnableCopy=(sv->Cursor.SelCount() > 1) | (sv->Cursor.GetPos() < sv->VoiceLen()-1);
        EnableCopy=EnableCopy | (CurrentDoc()->SelectionStaff>-1);
        Action("actionCut")->setEnabled(EnableCopy);
        Action("actionCopy")->setEnabled(EnableCopy);
        Action("actionReset_Position")->setEnabled(EnableCopy);

        Action("actionBack")->setEnabled(CurrentDoc()->sv->StartBar() != 0);
        /*
        Action("actionFirst")->setEnabled(CurrentDoc()->sv->StartBar() != 0);
        qDebug(QString::number(CurrentDoc()->sv->FindPointerToBar(CurrentDoc()->sv->StartBar()+CurrentDoc()->sv->BarsActuallyDisplayed())).toAscii());
        qDebug(QString::number(CurrentDoc()->sv->VoiceLen()).toAscii());
        Action("actionForward")->setEnabled(CurrentDoc()->sv->BarsActuallyDisplayed()>1);
        Action("actionLast")->setEnabled(CurrentDoc()->sv->BarsActuallyDisplayed()>1);
        */
        Action("actionTurn_Page")->setEnabled(CurrentDoc()->sv->BarsActuallyDisplayed()>1);

        /*
        int CountIt=0;
        if (sv->Cursor.GetMode()==CMOverwrite)
        {
            for (int py=sv->Cursor.SelStart();py<sv->Cursor.SelEnd();py++)
            {
                OCProperties* p=sv->GetProperties(py);
                if (p->Exist("NoteValue"))
                {
                    if ((p->GetValue("NoteType")==tsnote) || (p->GetValue("NoteType")==tstiednote))
                    {
                        CountIt+=Ticks(p->GetValue("NoteValue").toInt(),p->GetValue("Dotted").toBool(),p->GetValue("Triplet").toBool());
                    }
                }
            }
        }
        */
        /*
        QTreeWidget* tb=findChild<QTreeWidget*>("Toolbox");
        QList<QTreeWidgetItem*> items;
        items = tb->findItems("DuratedItem",Qt::MatchFixedString,0);
        if (items.count()>0) tb->itemWidget(items[0],0)->setEnabled(CountIt>0);
        */
        //Durated->setEnabled(CountIt>0);
        //QToolBox* tb=findChild<QToolBox*>("toolBox");
        //QWidget* tab=findChild<QWidget*>("Duratedpage");
        //tb->setItemEnabled(tb->indexOf(tab),CountIt>0);
        CurrentDoc()->SetDurated();
    }
    else
    {
        //sb->clearMessage();
        if (CurrentDoc()->lv->NumOfLayouts()==0)
        {
            //sb->showMessage("");
            StatusLabel->setText("");
            return;
        }
        QString msg=CurrentDoc()->lv->LayoutName(CurrentDoc()->lv->CurrentLayoutIndex);
            msg += " - Page " + QString::number(CurrentDoc()->lv->ActivePage + 1);
        msg += " - System " + QString::number(CurrentDoc()->lv->ActiveSystem + 1);
        //sb->showMessage(msg);
        StatusLabel->setText(msg);
    }
    SidebarSetSelected();
}

void MainWindow::NoteOn(int Pitch)
{
    if (MIDI->getPortCount()==0) return;
    OCPlayControl* pc=findChild<OCPlayControl*>();
    std::vector<unsigned char> v;
    if (CurrentDoc() !=0)
    {
        v.push_back(0xC0+CurrentDoc()->sv->MIDIChannel());
        v.push_back(CurrentDoc()->sv->MIDIPatch());
        v.push_back(80);
        MIDI->sendMessage(&v);
        v.clear();
        v.push_back(0x90+CurrentDoc()->sv->MIDIChannel());
    }
    else
    {
        v.push_back(0x90);
    }
    v.push_back(Pitch);
    v.push_back(100.0*pc->Volume());
    MIDI->sendMessage(&v);
}

void MainWindow::NoteOff(int Pitch)
{
    if (MIDI->getPortCount()==0) return;
    std::vector<unsigned char> v;
    if (CurrentDoc() !=0)
    {
        v.push_back(0x80+CurrentDoc()->sv->MIDIChannel());
    }
    else
    {
        v.push_back(0x80);
    }
    v.push_back(Pitch);
    MIDI->sendMessage(&v);
}

void MainWindow::ExportMidi()
{
    if (CurrentDoc()==0) return;
    QFileDialog d(this,Qt::Sheet);
    //d.setWindowModality(Qt::WindowModal);
    d.setWindowTitle("Export MIDI");
    d.setFilter(tr("MIDI Files (*.mid)"));
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setFileMode(QFileDialog::AnyFile);
    if (d.exec() != QDialog::Accepted) return;
    if (d.selectedFiles().count())
    {
        QString path=d.selectedFiles().first();
        CurrentDoc()->sv->Play(0,0,true,path);
    }
}

MainWindow::~MainWindow()
{
    CPresets* P=new CPresets("RecentDocuments");
    while (RecentFiles.count()>recentmax) RecentFiles.removeLast();
    P->SetValue("RecentDocuments",RecentFiles);
    delete P;
    P=new CPresets("Projects");
    P->SetValue("ProjectCount",Projects.count());
    for (int i=0;i<Projects.count();i++)
    {
        P->SetValue("ProjectName"+QString::number(i),Projects[i]->Name);
        P->SetValue("Documents"+QString::number(i),Projects[i]->Files);
    }
    qDeleteAll(Projects);
    delete P;
    delete SC;
    delete MIDI;
    qDeleteAll(RecentActions);
    delete closeAction;
    delete nextAction;
    delete previousAction;
    delete RecentMapper;
    delete mapper;
    delete ui;
}

void MainWindow::Undo()
{
    //QTabWidget* Tabs=CurrentDoc()->findChild<QTabWidget*>();
    CurrentDoc()->Undo();
    if (CurrentDoc()->GetView()==0)
    {
        CurrentDoc()->sv->Paint(RedrawAll);
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
    //QTabWidget* Tabs=CurrentDoc()->findChild<QTabWidget*>();
    CurrentDoc()->Redo();
    if (CurrentDoc()->GetView()==0)
    {
        CurrentDoc()->sv->Paint(RedrawAll);
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
    CurrentDoc()->sv->Delete(Pointer);
    SelectionChanged();
    CurrentDoc()->sv->Paint(RedrawAll);
    CurrentDoc()->sv->setFocus();
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
            CurrentDoc()->sv->Delete(CurrentDoc()->sv->MarkToStaff(CurrentDoc()->SelectionStaff+i),CurrentDoc()->sv->MarkToVoice(CurrentDoc()->SelectionStaff+i),CurrentDoc()->SelectionList[i].first,CurrentDoc()->SelectionList[i].second);
        }
    }
    else
    {
        CurrentDoc()->sv->Delete(CurrentDoc()->SelectionList[0].first,CurrentDoc()->SelectionList[0].second);
    }
    CurrentDoc()->sv->Paint(RedrawAll,true);
    CurrentDoc()->sv->Cursor.SetMode(CMInsert);
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
        //Action("actionPaste")->setText("Paste Bars");
        CopyData=new QDomLiteDocument("OCCopyDataVoices","Score");
    }
    else
    {
        //Action("actionPaste")->setText("Paste Notes");
        CopyData=new QDomLiteDocument("OCCopyDataVoice","Score");
    }
    CopyData->documentElement->clearChildren();
    if (CopyIsBars)
    {
        for (int i=0;i<CurrentDoc()->SelectionList.count();i++)
        {
            CopyData->documentElement->appendClone(CurrentDoc()->sv->GetClipBoardData(CurrentDoc()->sv->MarkToStaff(CurrentDoc()->SelectionStaff+i),CurrentDoc()->sv->MarkToVoice(CurrentDoc()->SelectionStaff+i),CurrentDoc()->SelectionList[i].first,CurrentDoc()->SelectionList[i].second));
        }
    }
    else
    {
        CopyData->documentElement->appendClone(CurrentDoc()->sv->GetClipBoardData(CurrentDoc()->SelectionList[0].first,CurrentDoc()->SelectionList[0].second));
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
        CurrentDoc()->sv->Cursor.SetMode(CMInsert);
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
    CurrentDoc()->sv->Paint(RedrawAll,true);
    UpdateStatus();
    DocChanged();
    delete CopyData;
}

void MainWindow::ShowPropertiesPopup(QPoint Pos)
{
    if (CurrentDoc()==0) return;
    //QString n=CurrentDoc()->childAt(CurrentDoc()->mapFromGlobal(Pos))->objectName();
    QTransMenu* m=new QTransMenu(this);
    /*
    if (CurrentDoc()->sv->Cursor.GetPos()!=CurrentDoc()->sv->VoiceLen()-1)
    {
        CPropertyWindow* p=new CPropertyWindow(this);
        p->Fill(CurrentDoc()->sv->GetProperties(CurrentDoc()->sv->Cursor.GetPos()));
        connect(p,SIGNAL(Changed(QString,QVariant)),this,SLOT(ChangeProperty(QString,QVariant)));
        QWidgetAction* w=new QWidgetAction(this);
        w->setDefaultWidget(p);
        m->addAction(w);
    }
    */
    m->addAction(Action("actionCut"));
    m->addAction(Action("actionCopy"));
    m->addAction(Action("actionPaste"));
    m->addSeparator();
    m->addAction(ui->actionSelect_All);
    m->addAction(ui->actionFrom_Start_to_Here);
    m->addAction(ui->actionFrom_Here_to_End);
    m->addAction(Action("actionReset_Position"));
    //m->setDefaultAction(w);
    m->addSeparator();
    m->exec(Pos);
    delete m;
}

void MainWindow::ShowLayoutPopup(QPoint Pos)
{
    if (CurrentDoc()==0) return;
    QTransMenu* m=new QTransMenu(this);
    m->addAction(ui->actionReformat);
    m->addSeparator();
    m->addAction(ui->actionFit_Systems);
    m->addAction(ui->actionGet_1_System_from_Next_Page);
    m->addAction(ui->actionMove_1_System_to_Next_Page);
    m->addSeparator();
    m->addAction(ui->actionToggle_Names);
    m->addAction(ui->actionGet_1_Bar_from_Next_System);
    m->addAction(ui->actionMove_1_Bar_to_Next_System);
    m->addAction(ui->actionEdit_Staffs);
    //m->addMenu(ui->menuLayout);
    //m->addMenu(ui->menuPage);
    //m->addMenu(ui->menuSystem);
    m->addSeparator();
    m->addAction(Action("actionReset_Position_2"));
    //m->setDefaultAction(w);
    m->exec(Pos);
    delete m;
}

void MainWindow::First()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->First();
}

void MainWindow::Back()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->Back();
}

void MainWindow::Forward()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->Forward();
}

void MainWindow::Last()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->Finish();
}

void MainWindow::TurnPage()
{
    if (CurrentDoc()==0) return;
    CurrentDoc()->TurnPage();
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
    CurrentDoc()->ReformatLayout();
}
