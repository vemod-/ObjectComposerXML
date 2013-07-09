#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define apptitle "Object Composer XML"
#define recentmax 30

#include <QMainWindow>
#include <QFileDialog>
#include <QSignalMapper>
#include <QGesture>
#include <QGestureEvent>
#include <QTreeWidget>
#include <QProcess>
//#include <QPropertyAnimation>
//#include <QMenu>
#include "ocplaycontrol.h"
#include "cscoredoc.h"
#include "qtransmenu.h"
#include "csidebartree.h"

// This is a "hidden" exported Qt function on the Mac for Qt-4.x.
#if QT_VERSION <= QT_VERSION_CHECK(4,4,0)
void qt_mac_set_menubar_icons(bool enable);
#endif

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void SidebarClicked(SidebarItem& i, int Col);
    void ShowScoreWizard();
    void NoteOn(int Pitch);
    void NoteOff(int Pitch);
    void SetZoom(int zoom);
    void ExportMidi();
    void DocumentActivated(int Index);
    void NewDoc();
    void CloseDoc();
    void UpdateTree();
    void UpdateSV();
    void UpdatePW();
    void UpdateBW();
    void UpdateStatus();
    void SelectionChanged();
    void BarChanged();
    void ScoreChanged();
    void TreeSelectionChanged();
    void BarSelectionChanged(QRect Selection);
    void DocChanged();
    void ChangeProperty(QString Name, OCProperties* p);
    void ResetPositions();
    void ResetSystemPosition();
    void SelectFromStart();
    void SelectToEnd();
    void SelectAll();

    void SetBar(int Bar, int Staff, int Voice);
    bool SaveDoc();
    bool SaveDialog();
    void OpenDoc(QString Path);
    void OpenDialog();
    void ShowPresets();
    void UpdateWindowList();
    void AddRecent(QString Path);
    void LoadRecent();
    void setActiveSubWindow(QWidget* w);
    void ShowScore();
    void ShowLayout();
    void UpdateSidebar();
    void AddProject();
    void RenameProject(QTreeWidgetItem* item,int column);
    void RebuildProjects();
protected:
    virtual void closeEvent(QCloseEvent *event);
    void pinchTriggered(QPinchGesture* gesture);
    void swipeTriggered(QSwipeGesture *gesture);
    void panTriggered(QPanGesture *gesture);
    bool event(QEvent *event);
    bool gestureEvent(QGestureEvent *event);
private:
    Ui::MainWindow *ui;
    /*
#ifdef __MACOSX_CORE__
    QProcess* VirtualMIDIProc;
#endif
*/
    OCPlayControl* playControl;
    CMIDI2wav* midi2Wav;
    CScoreDoc* CurrentDoc();
    void AppendMixerXML(CScoreDoc* document);
    OCSymbolsCollection SC;
    //RtMidiOut MIDI;
    bool CopyIsBars;
    QSignalMapper* mapper;
    QSignalMapper* RecentMapper;
    QAction* closeAction;
    QAction* nextAction;
    QAction* previousAction;
    QAction* tileAction;
    QAction* cascadeAction;
    QStringList RecentFiles;
    QList<QAction*> RecentActions;
    QList<QAction*> LayoutActions;
    QList<QAction*> ScoreActions;
    void ShowMenu(QString Name,bool Show);
    void ShowAction(QString Name,bool Show);
    QAction* Action(QString Name);
    QLabel* StatusLabel;
    QSlider* ZoomSlider;
    QToolButton* btnZoomIn;
    QToolButton* btnZoomOut;
    QTransMenu* NewMenu;
    QAction* LeftSideButton;
    QAction* RightSideButton;
    QList<OCProject*> Projects;
private slots:
    void LayoutsChanged();
    void ToggleSidebar();
    void ToggleLayoutSidebar();
    void ShowNewMenu();
    void DocumentClosed(CScoreDoc* document,bool &Cancel);
    void TabChanged();
    void Copy();
    void Paste();
    void Cut();
    void Undo();
    void Redo();
    void ShowPropertiesPopup(QPoint Pos);
    void ShowLayoutPopup(QPoint Pos);
    void ShowLayoutLayoutPopup();
    void ShowLayoutPagePopup();
    void ShowLayoutSystemPopup();
    void DeleteItem(int Pointer);
    void First();
    void Back();
    void Forward();
    void Last();
    void PrevStaff();
    void NextStaff();
    void PrevVoice();
    void NextVoice();
    //void TurnPage();
    void ZoomIn();
    void ZoomOut();
    void Settings();
    void PrintPreview();
    void Print();
    void PageSetup();
    void GetBarFromNext();
    void MoveBarToNext();
    void GetSystemFromNext();
    void MoveSystemToNext();
    void FitSystems();
    void FitFromHere();
    void FitAll();
    void ToggleNames();
    void EditStaffs();
    void EditLayout();
    void ReformatLayout();
    void ReformatFromHere();
    void AutoAdjustAll();
    void StretchFromHere();
    void CompressFromHere();
    void RequestMidiFile(QString &path, OCPlayControl::PlayLocation mode);
};

#endif // MAINWINDOW_H
