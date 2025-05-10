#ifndef CSCOREDOC_H
#define CSCOREDOC_H

#include <QWidget>
#include <QListWidgetItem>
#include "scoreviewxml.h"
#include "layoutviewxml.h"
#include "ocpiano.h"
#include "qmacsplitter.h"
#include <QMenu>
#include <QTableWidgetItem>
#include "ocplaycontrol.h"
//#include "../SoftSynthsWidgets/ceditmenu.h"
#include "../SoftSynthsWidgets/cprojectapp.h"
#include "ocsymbolscollection.h"

#define apptitle "Object Composer XML"
#define _DocumentPath QStandardPaths::writableLocation(QStandardPaths::MusicLocation) + "/Object Composer/"

namespace Ui {
    class CScoreDoc;
}

class CScoreDoc : public QGraphicsView, public IFileDocument//, public IEditDocument
{
    Q_OBJECT

public:
    explicit CScoreDoc(QWidget* mainWindow, QWidget *parent = nullptr);
    ~CScoreDoc();
    ScoreViewXML* sv;
    LayoutViewXML* lv;
    OCPiano* pno;
    OCPlayControl* playControl;
    CMIDI2wav* MIDI2wav;
    ImportResult Load(QString Path);
    bool Save(QString Path);
    void SetXML(XMLScoreWrapper& Doc);
    void SetXML(QDomLiteDocument* Doc);
    void SaveOptions() const;
    void LoadOptions();
    void ActivateDoc();
    OCSelectionList SelectionList;
    int SelectionStaff;
    int SelectionBar;
    const OCVoiceLocation MarkToVoiceLocation(const int MarkIndex) const;
    void UpdateLayoutList();
    void serialize(QDomLiteElement* xml) const;
    void unserialize(const QDomLiteElement* xml);
    bool canCopy();
    void UpdateStatus();
    int GetView() const;
    QMacSplitter* spSidebar;
    //bool Dirty;
    OCBarSymbolLocationList SearchResult;
    int SearchIndex;
    ImportResult lastImport;
    bool isClosed = true;
    double GetZoom() const;

    CMainMenu* MainMenu;

    QMenu* mainMenu;

    QMenu* printMenu;
    QAction* actionPrintPreview;
    QAction* actionPrint;
    QAction* actionPrintSetup;

    //CEditMenu* m_EditMenu;

    //QMenu* EditMenu;
    QAction* actionProperties;
    QAction* actionVoiceList;
    QAction* actionBarMap;
    QAction* actionResetPositions;
    QMenu* rhythmMenu;
    QAction* actionTriolize;
    QAction* actionDottify;
    QAction* actionDoubleDottify;
    QAction* actionStraighten;

    //QMenu* ViewMenu;
    QAction* actionScore;
    QAction* actionLayouts;

    QMenu* ScoreMenu;
    QAction* actionSwipeBack;
    QAction* actionSwipeForward;
    QAction* actionSwipeFirst;
    QAction* actionSwipeLast;

    QAction* actionAddStaff;
    QAction* actionDeleteStaff;
    QAction* actionMoveStaffUp;
    QAction* actionMoveStaffDown;
    QAction* actionAddVoice;
    QAction* actionDeleteVoice;

    QMenu* menuGotoVoice();

    QAction* actionSettings;
    QMenu* LayoutsMenu;
    QMenu* LayoutMenu;
    QAction* actionAutoadjustAll;
    QAction* actionReformat;
    QAction* actionReformatFromHere;
    QAction* actionStretchFromHere;
    QAction* actionCompressFromHere;
    QAction* actionFitSystems;
    QAction* actionFitSystemsFromHere;

    QMenu* PageMenu;
    QAction* actionSystemFromNextPage;
    QAction* actionSystemToNextPage;
    QAction* actionFitSystemsOnPage;

    QMenu* SystemMenu;
    QAction* actionBarFromNextSystem;
    QAction* actionBarToNextSystem;
    QAction* actionAddBarReformatFromHere;
    QAction* actionRemoveBarReformatFromHere;
    QAction* actionToggleNames;
    QAction* actionEditStaffs;
    QAction* actionSystemResetPositions;

    QAction* actionLayoutSettings;

    QAction* actionLayoutLayout;
    QAction* actionLayoutPage;
    QAction* actionLayoutSystem;

    QAction* RightSideButton;

    //QMenu* ToolBarActions;
public slots:
    void SetStatusLabel(QString text);
    void setActiveStaffId(const int Staff);
    void setActiveVoice(const int Voice);
    void setBar(int Bar, int Staff, int Voice);
    void SetCurrentLayout(const int Index);
    void SetCurrentLayout(QTableWidgetItem* item);
    void NextLayout();
    void PrevLayout();
    void InitLayout(const int Index);
    bool AddLayout();
    void EditLayout();
    void DeleteLayout();
    void UpdateScoreView();
    void UpdateLayoutView();
    void SetView(const int View, bool silent=false);
    void ShowStaffsDialog();
    void ZoomIn();
    void ZoomOut();
    void SetZoom(const double Zoom);
    void SwipeForward();
    void SwipeBack();
    void Forward();
    void Back();
    void First();
    void Finish();
    void SwipeFirst();
    void SwipeFinish();
    void PageSetup();
    void Print();
    void ExportPDF(const QString& pdfPath);
    void ExportMXL(const QString& mxlPath);
    void ExportMusicXML(const QString& mxlPath);
    void PrintPreview();
    void BarToNext();
    void BarFromNext();
    void BarToNextReformat();
    void BarFromNextReformat();
    void SystemToNext();
    void SystemFromNext();
    void ToggleNames();
    void EditSystem();
    void FitMusic();
    void FitPages(const int StartPage);
    void FitFromHere();
    void FitAll();
    void ResetSystem();
    void ReformatLayout(const LayoutLocation& StartLocation, const int Stretch=0);
    void ReformatLayoutFromStart();
    void ReformatFromHere();
    void StretchFromHere();
    void CompressFromHere();
    void AutoAdjust();
    void PasteSymbol(XMLSimpleSymbolWrapper& Symbol, QString UndoText, bool Finished);
    void PasteSymbol(XMLSimpleSymbolWrapper& Symbol, QString UndoText);
    void PasteDuratedSymbol(XMLSimpleSymbolWrapper& Symbol, QString UndoText);
    void OverwriteProperty(QString Name, QVariant Value, QString UndoText, bool Finished);
    void SetDurated();
    void TweakSystem();
    void search(const QString SearchTerm);
    void doSearch(const QString& SearchTerm);
    void searchNext();
    void searchPrev();
    void gotoBar(const int Bar);
    void gotoSymbol(const OCSymbolLocation& Symbol);
    void UpdateBW();
    void UpdatePW();
    void ShowContextPopup(QPoint Pos);
    void ShowLayoutPopup(QPoint Pos);
    void ShowLayoutLayoutPopup();
    void ShowLayoutPagePopup();
    void ShowLayoutSystemPopup();

    void PropertiesMenu();
    void VoiceListMenu();
    void BarMapMenu();
    void PopupProperties(QPoint p);
    void PopupVoiceList(QPoint p);
    void PopupBarMap(QPoint p);
    void UpdateTree();
    void UpdateSV(OCRefreshMode RefreshMode=tsReformat);
    void ResetPositions();
    void Triolize();
    void Dottify();
    void DoubleDottify();
    void Straighten();
    void SelectFromStart();
    void SelectToEnd();
    void SelectAll();
    void DeleteDoc() override;
    void CopyDoc(QDomLiteElement* xml) override;
    void PasteDoc(const QDomLiteElement* xml) override;
    void SwapForward();
    void SwapBack();
    void showLayouts();
    void showScore();
    void addStaff();
    void addVoice();
    void moveStaffUp();
    void moveStaffDown();
    void deleteStaff();
    void deleteVoice();
private slots:
    void SVChanged();
    void BarChanged();
    void PWChanged(OCRefreshMode refreshMode);
    void ChangeProperty(QString Name, QVariant Value, bool Custom);
    void BarSelectionChanged(QRect Selection);
    void TreeSelectionChanged();
    void TreeItemsRearranged(QList<int> itemOrder);
    void ScoreSelectionChanged();
    void DeleteItem(int Pointer);
    void toggleView();
    void RequestMidiFile(QString &path, int& bar, CMIDI2wav::PlayLocation mode, QStringList &TrackIDs);
protected:
    void closeEvent(QCloseEvent *event) override;
    bool event(QEvent *event) override;
signals:
    void MidiMessage(std::vector<unsigned char>* msg);
    void Close(CScoreDoc* document,bool &Cancel);
    void ZoomChanged(double Zoom);
private slots:
    void ToggleLayoutSidebar();
    void staffConfigChanged();
    void RenameItem(QTableWidgetItem* item);
    void LocationBack();
    void LocationForward();
    void SaveLocation();
    void destyleSearchCombo();
    void styleSearchCombo();
    void deleteVoiceSearch();
    void deleteSearch();
    void searchMenuPopup(QPoint pos);
    void NoteOnOff(bool On, int Pitch, int MixerTrack, OCMIDIVars MIDIInfo);
private:
    Ui::CScoreDoc *ui;
    void UpdateXML();
    OCSymbolsCollection SC;
    void prepareFade();
    int LocationIndex;
    void UpdateLocationButtons();
    bool SearchComboVisible;
    const QString LocationString(const OCBarSymbolLocation& sl) const;
    OCBarSymbolLocationList Locations;
    QList<QWidget*> DuratedGridWidgets;
    QSplitter* spMain;
    int currentpitch;
    QToolButton* tbAddLayout;
    int m_View;
    QMenu* searchMenu;
    QAction* deleteVoiceSearchAction;
    QAction* deleteSearchAction;
public:
    void NewDoc() override;
    void OpenDoc(QString Path) override;
    void WizardDoc() override;
    void SaveDoc(QString path) override;
    void UpdateAppTitle();
    void undoSerialize(QDomLiteElement* xml) const override;
    void undoUnserialize(const QDomLiteElement* xml) override;
    void CloseDoc() override;

    QAction* actionExportMIDI;
    QAction* actionExportAudio;
    QAction* actionExportPDF;
    QAction* actionExportMXL;
    QAction* actionExportMusicXML;
    QAction* actionPreferences;

    void ShowPresets();
    void ExportMidi();
    void ExportWave();
    void ExportPDFDialog();
    void ExportMXLDialog();
    void ExportMusicXMLDialog();
protected:
    void Render(QString path);
private:
    QWidget* m_MainWindow;
};

#endif // CSCOREDOC_H
