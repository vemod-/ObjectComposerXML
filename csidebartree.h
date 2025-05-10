#ifndef CSIDEBARTREE_H
#define CSIDEBARTREE_H

#include "qmactreewidget.h"
#include <QFileInfo>
#include <QStackedWidget>

namespace Ui {
    class CSidebarTree;
}

struct OCProject
{
    QString Name;
    QStringList Files;
};

enum SidebarDataIndexes
{
    SDI_ItemType=32,
    SDI_ItemIndex=33,
    SDI_Filepath=34,
    SDI_OpenIndex=35,
    SDI_Action=36,
    SDI_ParentText=37,
    SDI_ParentIndex=38
};

enum SidebarActions
{
    SBA_None=0,
    SBA_Close=1,
    SBA_RemoveFromRecent=2,
    SBA_RemoveFromProject=3,
    SBA_Save=4,
    SBA_Open=5,
    SBA_Reveal=6,
    SBA_OpenProject=7,
    SBA_DeleteProject
};

enum SidebarDocumentTypes
{
    SDT_NA=0,
    SDT_File=1,
    SDT_OpenDoc=2,
    SDT_OpenLayout=3,
    SDT_Project=4
};

struct SidebarItem
{
    SidebarDocumentTypes itemType;
    SidebarActions a;
    int itemIndex;
    int openIndex;
    int parentIndex;
    QString filePath;
};

class CSidebarTree : public QMacTreeWidget
{
    Q_OBJECT

public:
    explicit CSidebarTree(QWidget *parent = nullptr);
    ~CSidebarTree();
    QTreeWidgetItem* createProjectItem(const int index, const QString& ProjectName, const int FileCount);
    QTreeWidgetItem* createOpenDocumentItem(const QFileInfo& fi, const int index, const int DocCount);
    QTreeWidgetItem* createProjectDocumentItem(const QFileInfo& fi, const int index, const int parentIndex, const QString& ProjectName, const int OpenDocsIndex);
    QTreeWidgetItem* createLayoutItem(const QFileInfo& fi, const int index, const int parentIndex, const QString& LayoutName);
    QTreeWidgetItem* createFileItem(const QFileInfo& fi, const int i);
    void editProjectItem(const int index);
    void RebuildProjects(QList<OCProject*>& Projects);
    void RenameProjectItem(QTreeWidgetItem* item, QList<OCProject*>& Projects);
    void Fill(QStackedWidget* docsWidget, QList<OCProject*>& Projects, QStringList& RecentFiles);
    void SetSelected(const int docIndex, const int LayoutIndex, const bool isLayout);
    const SidebarItem GetItem(QTreeWidgetItem* item, const int Col);
signals:
    void MoveDocument();
    void itemClick(SidebarItem& item, int col);
private:
    Ui::CSidebarTree *ui;
    bool isDrag;
    const QString getProjectItemPath(const int projectIndex, const int index);
    QTreeWidgetItem* ProjectsItem();
private slots:
    void itemMoved();
    void dragEnter();
    void SelectItem(QTreeWidgetItem* item, int col);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // CSIDEBARTREE_H
