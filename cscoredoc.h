#ifndef CSCOREDOC_H
#define CSCOREDOC_H

#include <QWidget>
#include <QListWidgetItem>
#include "scoreviewxml.h"
#include "layoutviewxml.h"
#include "cpropertywindow.h"
#include "cbarwindow.h"
#include "cmusictree.h"
#include "ocpiano.h"
#include "cstaffsdialog.h"
#include "clayoutwizard.h"
#include "qmacsplitter.h"

namespace Ui {
    class CScoreDoc;
}

class CScoreDoc : public QWidget
{
    Q_OBJECT

public:
    explicit CScoreDoc(QWidget *parent = 0);
    ~CScoreDoc();
    ScoreViewXML* sv;
    LayoutViewXML* lv;
    CPropertyWindow* pw;
    CBarWindow* bw;
    CMusicTree* mt;
    OCPiano* pno;
    QString Path;
    void fillStaffsCombo();
    void fillVoicesCombo();
    void Load(QString Path);
    void SetXML(XMLScoreWrapper& Doc);
    QList<QPair<int,int> > SelectionList;
    int SelectionStaff;
    int SelectionBar;
    void UpdateLayoutList();
    void Undo();
    void Redo();
    QString UndoText();
    QString RedoText();
    const int GetView() const;
    QMacSplitter* spSidebar;
    bool Dirty;
    QList<SymbolSearchLocation> SearchResult;
    int SearchIndex;
public slots:
    void SetStatusLabel(QString text);
    void setActiveStaff(const int Staff);
    void setActiveVoice(const int Voice);
    void UpdateStaffsCombo();
    void SetCurrentLayout(const int Index);
    void SetCurrentLayout();
    void NextLayout();
    void PrevLayout();
    void InitLayout(const int Index);
    bool AddLayout();
    void EditLayout();
    void DeleteLayout();
    void MakeBackup(const QString& text);
    void UpdateLayoutView();
    void SetView(const int View);
    void ShowStaffsDialog();
    void ZoomIn();
    void ZoomOut();
    void SetZoom(const int Zoom);
    const int GetZoom() const;
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
    void PrintPreview();
    void BarToNext();
    void BarFromNext();
    void SystemToNext();
    void SystemFromNext();
    void ToggleNames();
    void EditSystem();
    void FitMusic();
    void FitPages(const int StartPage);
    void ResetSystem();
    void ReformatLayout(const int StartPage, const int StartSystem, const int Stretch=0);
    void AutoAdjust();
    void PasteXML(XMLSimpleSymbolWrapper& Symbol, QString UndoText, bool Finished);
    void PasteXML(XMLSimpleSymbolWrapper& Symbol, QString UndoText, OCRefreshMode RefreshMode);
    void PasteDuratedXML(XMLSimpleSymbolWrapper& Symbol, QString UndoText, OCRefreshMode RefreshMode);
    void OverwriteProperty(QString Name, QVariant Value, QString UndoText, bool Finished);
    void SetDurated();
    void TweakSystem();
    void search(const QString& SearchTerm);
    void doSearch(const QString& SearchTerm);
    void searchNext();
    void searchPrev();
protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual bool event(QEvent *event);
signals:
    void MidiMessage(std::vector<unsigned char>* msg);
    void Close(CScoreDoc* document,bool &Cancel);
    void Changed();
    void StaffChanged();
    void ScoreChanged();
    void LayoutsChanged();
private slots:
    void UpdateLayoutTab(int i);
    void RenameItem(QTableWidgetItem* item);
    void LocationBack();
    void LocationForward();
    void SaveLocation();
    void showLayouts();
    void showScore();
    void DeStyleSearchCombo();
    void StyleSearchCombo();
private:
    struct ScoreLocation
    {
        int StartBar;
        int Staff;
        int Voice;
    };
    Ui::CScoreDoc *ui;
    void UpdateXML();
    void UpdateAll();
    void prepareFade();
    int LocationIndex;
    void UpdateLocationButtons();
    bool SearchComboVisible;
    QString LocationString(const ScoreLocation* sl);
    QList<ScoreLocation> Locations;
    QList<QWidget*> DuratedGridWidgets;
    QSplitter* spMain;
    QSplitter* spBottom;
    QSplitter* spRight;
    int currentpitch;
    QDomLiteDocument m_XMLScoreBackup;
    int UndoIndex;
    QToolButton* tbAddLayout;
    QList<int> ZoomList;
    int m_View;
};

#endif // CSCOREDOC_H
