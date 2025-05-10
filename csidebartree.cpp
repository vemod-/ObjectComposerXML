#include "csidebartree.h"
#include "ui_csidebartree.h"
#include "mouseevents.h"
//#include "CommonClasses.h"
#include "cscoredoc.h"

CSidebarTree::CSidebarTree(QWidget *parent) :
    QMacTreeWidget(parent),
    ui(new Ui::CSidebarTree)
{
    ui->setupUi(this);
    header()->setFixedHeight(tablerowheight);
    setAutoFillBackground(true);
    setAttribute(Qt::WA_MacShowFocusRect, false);
    setIndentation(12);
    setColumnCount(3);
    setUniformRowHeights(true);
    //setAnimated(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAnimated(false);
    auto ItemMoved=new InternalMoveEvent;
    installEventFilter(ItemMoved);
    connect(ItemMoved,SIGNAL(ItemMoved()),this,SLOT(itemMoved()));
    connect(ItemMoved,SIGNAL(DragEnter()),this,SLOT(dragEnter()));
    //connect(this,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(ItemClicked(QTreeWidgetItem*,int)));
    //connect(this,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(ItemClicked(QTreeWidgetItem*,int)));
    viewport()->installEventFilter(this);
    isDrag=false;
}

CSidebarTree::~CSidebarTree()
{
    delete ui;
}

void CSidebarTree::dragEnter()
{
    isDrag=true;
}

void CSidebarTree::itemMoved()
{
    if (!isDrag) return;
    isDrag=false;
    emit MoveDocument();
    setMouseTracking(true);
}
QTreeWidgetItem* CSidebarTree::createProjectItem(const int index, const QString& ProjectName, const int FileCount)
{
    QTreeWidgetItem* projectItem=createStandardItem(ProjectName,":/bookgrey.png",2,1);
    projectItem->setData(0,SDI_ItemType,SDT_Project);
    projectItem->setData(0,SDI_ItemIndex,index);
    projectItem->setData(0,SDI_Action,SBA_None);
    projectItem->setToolTip(0,"Project <b>"+ProjectName+"</b>");
    projectItem->setSizeHint(0,QSize(-1,tablerowheight));
    if (FileCount)
    {
        projectItem->setIcon(1,QIcon(":/bookgrey.png"));
        projectItem->setData(1,SDI_Action,SBA_OpenProject);
        projectItem->setToolTip(1,"Open Project <b>"+ProjectName+"</b>");
    }
    projectItem->setData(2,SDI_Action,SBA_DeleteProject);
    projectItem->setToolTip(2,"Delete Project <b>"+ProjectName+"</b>");
    projectItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDropEnabled);
    return projectItem;
}

QTreeWidgetItem* CSidebarTree::createOpenDocumentItem(const QFileInfo& fi, const int index, const int DocCount)
{
    int CloseIcon=0;
    int SaveIcon=2;
    if (DocCount>1)
    {
        CloseIcon=2;
        SaveIcon=1;
    }
    QTreeWidgetItem* documentItem=createStandardItem(fi.fileName(),":/scoregrey.png",CloseIcon,SaveIcon);
    documentItem->setData(0,SDI_ItemType,SDT_OpenDoc);
    documentItem->setData(0,SDI_ItemIndex,index);
    documentItem->setData(0,SDI_OpenIndex,index);
    documentItem->setData(0,SDI_Filepath,fi.filePath());
    documentItem->setData(0,SDI_Action,SBA_Reveal);
    documentItem->setToolTip(0,"Switch to <b>"+fi.fileName()+"</b>");
    if (DocCount>1)
    {
        documentItem->setData(CloseIcon,SDI_Action,SBA_Close);
        documentItem->setToolTip(CloseIcon,"Close <b>"+fi.fileName()+"</b>");
    }
    documentItem->setData(SaveIcon,SDI_Action,SBA_Save);
    documentItem->setToolTip(SaveIcon,"Save <b>"+fi.fileName()+"</b>");

    documentItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsDragEnabled);
    return documentItem;
}

QTreeWidgetItem* CSidebarTree::createProjectDocumentItem(const QFileInfo& fi, const int index, const int parentIndex, const QString& ProjectName, const int OpenDocsIndex)
{
    QTreeWidgetItem* documentItem=createStandardItem(fi.fileName(),":/scoregrey.png",2);
    documentItem->setData(0,SDI_ItemIndex,parentIndex);
    documentItem->setData(0,SDI_ParentText,ProjectName);
    documentItem->setData(0,SDI_ParentIndex,index);
    documentItem->setData(0,SDI_Filepath,fi.filePath());
    if (OpenDocsIndex > -1)
    {
        documentItem->setData(0,SDI_OpenIndex,OpenDocsIndex);
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
            documentItem->setToolTip(0,"Open <b>"+fi.filePath()+"</b>");
        }
        else
        {
            documentItem->setForeground(0,Qt::gray);
            documentItem->setData(0,SDI_Action,SBA_None);
            documentItem->setToolTip(0,"<b>"+fi.filePath()+"</b> does not exist");
        }
    }
    documentItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsDragEnabled);
    documentItem->setData(2,SDI_Action,SBA_RemoveFromProject);
    documentItem->setToolTip(2,"Remove <b>"+fi.fileName()+"</b> from Project <b>"+ProjectName+"</b>");
    return documentItem;
}

QTreeWidgetItem* CSidebarTree::createLayoutItem(const QFileInfo& fi, const int index, const int parentIndex, const QString& LayoutName)
{
    QTreeWidgetItem* layoutitem=createStandardItem(LayoutName,":/layoutgrey.png");
    layoutitem->setData(0,SDI_ItemType,SDT_OpenLayout);
    layoutitem->setData(0,SDI_ItemIndex,index);
    layoutitem->setData(0,SDI_OpenIndex,parentIndex);
    layoutitem->setData(0,SDI_Filepath,fi.filePath());
    layoutitem->setData(0,SDI_Action,SBA_Reveal);
    layoutitem->setData(0,SDI_ParentIndex,parentIndex);
    layoutitem->setData(0,SDI_ParentText,fi.fileName());
    layoutitem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    layoutitem->setToolTip(0,"Switch to <b>"+fi.fileName()+"</b> Layout <b>"+LayoutName+"</b>");
    return layoutitem;
}

QTreeWidgetItem* CSidebarTree::createFileItem(const QFileInfo& fi, const int i)
{
    int CloseIcon=0;
    if (!fi.exists()) CloseIcon=2;
    QTreeWidgetItem* fileItem=createStandardItem(fi.fileName(),":/scoregrey.png",CloseIcon);
    fileItem->setData(0,SDI_ItemType,SDT_File);
    fileItem->setData(0,SDI_ItemIndex,i);
    fileItem->setData(0,SDI_Filepath,fi.filePath());
    if (fi.exists())
    {
        fileItem->setData(0,SDI_Action,SBA_Open);
        fileItem->setToolTip(0,"Open <b>"+fi.filePath()+"</b>");
        fileItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsDragEnabled);
    }
    else
    {
        fileItem->setForeground(0,Qt::gray);
        fileItem->setData(0,SDI_Action,SBA_None);
        fileItem->setData(2,SDI_Action,SBA_RemoveFromRecent);
        fileItem->setToolTip(0,"<b>"+fi.filePath()+"</b> does not exist");
        fileItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        fileItem->setToolTip(2,"Remove <b>"+fi.fileName()+"</b> from Recent Files");
    }
    return fileItem;
}

void CSidebarTree::editProjectItem(const int index)
{
    editItem(topLevelItem(0)->child(0)->child(index),0);
}

QTreeWidgetItem* CSidebarTree::ProjectsItem()
{
    return topLevelItem(0)->child(0);
}

void CSidebarTree::RebuildProjects(QList<OCProject*>& Projects)
{
    QTreeWidgetItem* projectsItem=ProjectsItem();
    qDeleteAll(Projects);
    Projects.clear();
    for(int i=0;i<projectsItem->childCount();i++)
    {
        auto p=new OCProject;
        p->Name=projectsItem->child(i)->text(0);
        for (int j=0;j<projectsItem->child(i)->childCount();j++)
        {
            p->Files.append(getProjectItemPath(i,j));
        }
        Projects.append(p);
    }
}

void CSidebarTree::RenameProjectItem(QTreeWidgetItem *item, QList<OCProject *> &Projects)
{
    if (item->data(0,SDI_ItemType).toInt()==SDT_Project)
    {
        Projects[item->data(0,SDI_ItemIndex).toInt()]->Name=item->text(0);
    }
}

const QString CSidebarTree::getProjectItemPath(const int projectIndex, const int index)
{
    QString s;
    QTreeWidgetItem* documentItem=ProjectsItem()->child(projectIndex)->child(index);
    if (documentItem->data(0,SDI_Filepath).toString().length())
    {
        s=documentItem->data(0,SDI_Filepath).toString();
    }
    else
    {
        s=documentItem->text(0);
    }
    return s;
}

void CSidebarTree::Fill(QStackedWidget* docsWidget, QList<OCProject*>& Projects, QStringList& RecentFiles)
{
    isDrag=false;
    QStringList OpenDocs;
    for (int i=0;i<docsWidget->count();i++)
    {
        QFileInfo fi(docsWidget->widget(i)->windowTitle());
        OpenDocs.append(fi.filePath());
    }

    blockSignals(true);
    setUpdatesEnabled(false);
    setDragEnabled(true);
    viewport()->setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QTreeWidget::InternalMove);
    invisibleRootItem()->setFlags(Qt::ItemIsEnabled);
    hide();
    clear();

    QTreeWidgetItem* libraryItem=createTopLevelItem(tr("Library"));
    addTopLevelItem(libraryItem);

    QTreeWidgetItem* projectsItem=createStandardItem(tr("Projects"),":/ocgrey.png");
    projectsItem->setFlags(Qt::ItemIsEnabled);
    libraryItem->addChild(projectsItem);
    for (int i=0;i<Projects.size();i++)
    {
        QTreeWidgetItem* projectItem=createProjectItem(i,Projects[i]->Name,Projects[i]->Files.size());
        projectsItem->addChild(projectItem);
        for (int j=0;j<Projects[i]->Files.size();j++)
        {
            QString s=Projects[i]->Files[j];
            projectItem->addChild(createProjectDocumentItem(QFileInfo(s),i,j,Projects[i]->Name,OpenDocs.indexOf(s)));
        }
        projectItem->setExpanded(true);
    }
    projectsItem->setExpanded(true);
    libraryItem->setExpanded(true);

    QTreeWidgetItem* openDocumentsItem=createTopLevelItem(tr("Open Documents"));
    addTopLevelItem(openDocumentsItem);
    openDocumentsItem->setExpanded(true);
    for (int j=0;j<docsWidget->count();j++)
    {
        auto doc=dynamic_cast<CScoreDoc*>(docsWidget->widget(j));
        QFileInfo fi(doc->windowTitle());
        QTreeWidgetItem* documentItem=createOpenDocumentItem(fi,j,docsWidget->count());
        openDocumentsItem->addChild(documentItem);
        documentItem->setExpanded(true);
        for (int i=0;i<doc->lv->layoutCount();i++)
        {
            documentItem->addChild(createLayoutItem(fi,i,j,doc->lv->LayoutName(i)));
        }
    }
    QTreeWidgetItem* recentDocumentsItem=createTopLevelItem(tr("Recent Documents"));
    addTopLevelItem(recentDocumentsItem);
    recentDocumentsItem->setExpanded(true);
    int i=0;
    for (const QString& s : RecentFiles)
    {
        QFileInfo fi(s);
        if (!OpenDocs.contains(fi.filePath()))
        {
            recentDocumentsItem->addChild(createFileItem(fi,i));
            i++;
        }
    }
    show();
    adjust();
    blockSignals(false);
    setUpdatesEnabled(true);
}

void CSidebarTree::SetSelected(const int docIndex, const int LayoutIndex, const bool isLayout)
{
    setUpdatesEnabled(false);
    blockSignals(true);
    clearSelection();
    if (topLevelItemCount()>1)
    {
        QTreeWidgetItem* docitem=topLevelItem(1)->child(docIndex);
        if (!isLayout)
        {
            setCurrentItem(docitem);
        }
        else
        {
            if (docitem->childCount()>0)
            {
                if (docitem->childCount()>LayoutIndex)
                {
                    setCurrentItem(docitem->child(LayoutIndex));
                }
            }
        }
    }
    blockSignals(false);
    setUpdatesEnabled(true);
}

bool CSidebarTree::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        auto e = static_cast<QMouseEvent*>(event);
        if (e->button() & Qt::LeftButton)
        {
            //qDebug() << event;
            QTreeWidgetItem* i=this->itemAt(this->mapFromGlobal(QCursor::pos()));
            int Col=this->columnAt(this->mapFromGlobal(QCursor::pos()).x());
            if (i)
            {
                if (this->indexOfTopLevelItem(i) == -1)
                {
                    //i->setSelected(true);
                    if (i->isFirstColumnSpanned()) Col=0;
                    this->SelectItem(i,Col);
                }
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void CSidebarTree::SelectItem(QTreeWidgetItem *item, int col)
{
    //static int count=0;
    //qDebug() << count++;
    SidebarItem i=GetItem(item,col);
    emit itemClick(i,col);
    setMouseTracking(true);
}

const SidebarItem CSidebarTree::GetItem(QTreeWidgetItem *item, const int Col)
{
    SidebarItem i;
    i.itemType=SidebarDocumentTypes(item->data(0,SDI_ItemType).toInt());
    i.a=SidebarActions(item->data(Col,SDI_Action).toInt());
    i.itemIndex=item->data(0,SDI_ItemIndex).toInt();
    i.openIndex=item->data(0,SDI_OpenIndex).toInt();
    i.parentIndex=item->data(0,SDI_ParentIndex).toInt();
    i.filePath=item->data(0,SDI_Filepath).toString();
    if ((i.itemType==SDT_Project) && (Col>0)) item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    return i;
}
