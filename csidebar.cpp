#include "csidebar.h"
#include "ui_csidebar.h"

CSidebar::CSidebar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CSidebar)
{
    ui->setupUi(this);
    setStyleSheet(buttongradient);
    setAutoFillBackground(true);
    ui->SidebarTree->header()->setFixedHeight(tablerowheight);
    ui->SidebarTree->setFont(QFont("Lucida Grande",11));
#if defined(__STYLESHEETS__)
    ui->SidebarTree->setStyleSheet(QString(widgetgradient)+selectiongradient);
#endif
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

}

CSidebar::~CSidebar()
{
    delete ui;
}
