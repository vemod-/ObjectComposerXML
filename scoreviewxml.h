#ifndef SCOREVIEWXML_H
#define SCOREVIEWXML_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QScrollBar>
#include <QWheelEvent>
#include <QWidget>
#include <QTimer>
#include "ocscore.h"
#include "qhoverbutton.h"
#include "qiphotorubberband.h"

namespace Ui {
    class ScoreViewXML;
}

class ScoreViewXML : public QGraphicsView
{
    Q_OBJECT
public:
    OCCursor Cursor;
    ScoreViewXML(QWidget *parent = NULL);
    ~ScoreViewXML();
    const bool FollowResize() const;
    const int ActiveStaff() const;
    const int ActiveVoice() const;
    const int BarNrOffset() const;
    const int MasterStaff() const;
    const int NoteSpace() const;
    const int StartBar() const;
    const int EndBar() const;
    const int Size() const;
    const bool Locked() const;
    const bool HideBarNumbers() const;

    void SetSystemLength(const int NewSystemLength);
    const int SystemLength() const;

    QRectF SceneRect();
    const bool EditProperties(XMLSimpleSymbolWrapper& Symbol , OCRefreshMode& RefreshMode);
    void Paint(const OCRefreshMode Mode, const bool UpdateSelection=false);
    void PasteClipBoardData(const int Staff, const int Voice, const int Pointer, QDomLiteElement* ClipBoardData);
    void PasteClipBoardData(const int Pointer, QDomLiteElement* ClipBoardData);
    QDomLiteElement* GetClipBoardData(const int Staff, const int Voice, const int StartPointer, const int EndPointer = 0);
    QDomLiteElement* GetClipBoardData(const int StartPointer, const int EndPointer = 0);
    QDomLiteElement* GetClipBoardData(const int Staff,const int Voice, const QList<int>& Pointers);
    QDomLiteElement* GetClipBoardData(const QList<int>& Pointers);
    QDomLiteElement* GetClipBoardData();
    void EnsureVisible(const int iStaff);
    void EnsureVisible();
    const int BarsActuallyDisplayed() const;
    const bool CanTurnPage() const;
    const int FindPointerToBar(const int Staff, const int Voice, const int BarToFind) const;
    const int FindPointerToBar(const int BarToFind) const;
    void Play(const int PlayFromBar, const int Silence, const QString& Path, const int Staff=-1);
    const QByteArray MIDIPointer(const int PlayFromBar, const int Silence);
    const int FindCurrentMeter(const int Staff, const int Voice, const int Pointer) const;
    void Mute(const int Staff, const bool Mute);
    void Solo(const int Staff, const bool Solo);
    void PasteXML(XMLSimpleSymbolWrapper& Symbol);
    void InsertXML(const int Staff, const int Voice, const int Pointer, XMLSimpleSymbolWrapper& Symbol);
    void InsertXML(const int Pointer, XMLSimpleSymbolWrapper& Symbol);
    void InsertXML(XMLSimpleSymbolWrapper& Symbol);
    void AddStaff(const int NewNumber, const QString& Name = "New Staff");
    void AddVoice(const int iStaff);
    void AddVoice();
    const int VoiceCount(const int Staff);
    const int VoiceCount();
    const int StaffCount();
    void DeleteVoice(const int iStaff, const int iVoice);
    void DeleteVoice(const int iVoice);
    void DeleteVoice();
    void DeleteStaff(const int iStaff);
    void DeleteStaff();
    void Load(const QString& Path);
    const bool Save(const QString& Path);
    OCProperties* GetProperties(const int Pointer, const int Staff, const int Voice);
    OCProperties* GetProperties(const int Pointer);
    OCProperties* GetProperties();
    XMLSimpleSymbolWrapper GetSymbol(const int Pointer, const int Staff, const int Voice);
    XMLSimpleSymbolWrapper GetSymbol(const int Pointer);
    XMLSimpleSymbolWrapper GetSymbol();
    void ChangeProperty(const QList<int>& Pointers, const QString& Name, const QVariant& Value);
    void ChangeProperty(const int Pointer, const int Staff, const int Voice, const QString& Name, const QVariant& Value);
    void ChangeProperty(const int Pointer, const QString& Name, const QVariant& Value);
    void ChangeProperties(const int Pointer, const int Staff, const int Voice, const QStringList& Names, const QVariant& Value);
    void ChangeProperties(const int Pointer, const QStringList& Names, const QVariant& Value);
    const OCBarMap& BarMap() const;
    const int VoiceLen(const int Staff, const int Voice);
    const int VoiceLen();
    const int EndOfVoiceBar(const int Staff, const int Voice) const;
    const int EndOfVoiceBar() const;
    void About();
    const OCMIDIVars GetCurrentMIDI() const;
    void SetXML(XMLScoreWrapper& NewXML);
    void ReloadXML();
    XMLScoreWrapper XMLScore;
    const QString StaffName(const int Staff);
    QDomLiteElement* ActiveTemplate();
    const QList<QPair<int,int> > SelectionList();
    const QList<QPair<int,int> > SelectionList(const int Bar1, const int Bar2, const int Staff1, const int Staff2);
    const int MarkToVoice(const int Mark);
    const int MarkToStaff(const int Mark);
    const bool navigationVisible() const;
    QAction* setAction(const QKeySequence keySequence);
    const QList<SymbolSearchLocation> Search(const QString& SearchTerm, const int Staff=-1, const int Voice=-1);
public slots:
    void setNavigationVisible(const bool newShowNavigation);
    void setActiveTemplate(const int Template);
    void setActiveTemplate(QDomLiteElement* Template);
    void setFollowResize(const bool NewFollowResize);
    void setActiveStaff(const int NewActiveStaff);
    void setActiveVoice(const int NewActiveVoice);
    void setBarNrOffset(const int NewBarNrOffset);
    void setMasterStaff(const int NewMasterStaff);
    void setNoteSpace(const int NewNoteSpace);
    void setStartBar(const int NewStartBar);
    void setEndBar(const int NewEndBar);
    void setSize(const int NewSize);
    void setLocked(const bool NewLocked);
    void setHideBarNumbers(const bool NewHideBarNumbers);
    void Sound();
    void SoundOff();
    void MakeBackup(const QString& text);
    void transposeSelected(const int add);
    void Delete(const int Staff, const int Voice, const int StartPointer, const int EndPointer = 0);
    void Delete(const int StartPointer, const int EndPointer = 0);
    void Delete(const int Staff, const int Voice, QList<int>& Pointers);
    void Delete(QList<int>& Pointers);
    void Delete();
    void turnback();
    void turnpage();
    void fastforward();
    void fastback();
    void selectNextSymbol();
    void selectPrevSymbol();
    void selectNextSymbolExtend();
    void selectPrevSymbolExtend();
    void selectEnd();
    void selectHome();
    void selectEndExtend();
    void selectHomeExtend();
    void selectNextStaff();
    void selectPrevStaff();
    void selectNextVoice();
    void selectPrevVoice();
    void selectPitchUp();
    void selectPitchDown();
    void selectOctaveUp();
    void selectOctaveDown();
    //void selectInsert();
    void selectBackSpace();
    void selectDelete();
    void selectSwapForward();
    void selectSwapBack();
    void flashSelected();
protected:
    bool viewportEvent(QEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseDoubleClickEvent (QMouseEvent* event);
    void resizeEvent(QResizeEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void scrollContentsBy(int dx, int dy);
    void wheelEvent(QWheelEvent* event);
    void leaveEvent(QEvent *event);
signals:
    void Changed();
    void ScoreChanged();
    void SelectionChanged();
    void NoteOff(int Pitch);
    void NoteOn(int Pitch);
    void ActiveStaffChange(int Staff);
    void BarChanged();
    void ProcessingStaff(int Staff);
    void Popup(QPoint Pos);
    void BackMeUp(QString Text);
    void NavigationForwardClicked();
    void NavigationBackClicked();
    void NavigationEndClicked();
    void NavigationHomeClicked();
    void SwipeLeftToRight();
    void SwipeRightToLeft();
private slots:
    void swipeProc(int value);
private:
    Ui::ScoreViewXML *ui;
    float swipeDelta;
    bool touchDown;
    int swipePos;
    int swipeBackPos;
    int m_SystemLength;
    int m_StartBar;
    int m_EndBar;
    bool Dragging;
    bool MouseDown;
    int MouseButton;
    QPointF Holdm;
    bool toneon;
    OCMIDIVars CurrentMIDI;
    //int m_MIDIChannel;
    //int m_MIDIPatch;
    int d2pitch;
    int soundPitch;
    QTimer soundTimer;
    //int m_MIDITranspose;
    bool m_NavigationVisible;
    bool m_FollowResize;
    int m_ActiveStaff;
    int m_ActiveVoice;
    bool m_Locked;
    QiPhotoRubberband* SelectRubberband;
    QHoverRubberband* HoverRubberband;
    OCScore Score;
    OCFrame* Frame;
    QGraphicsScene* Scene;
    QHoverButton* turnpagebutton;
    QHoverButton* turnbackbutton;
    QHoverButton* fastforwardbutton;
    QHoverButton* fastbackbutton;
    QDomLiteElement m_XMLLastPasted;
    XMLSimpleSymbolWrapper* CurrentSymbol;
    QDomLiteElement* m_ActiveTemplate;
    OCDraw ScreenObj;
    OCFrameProperties* CurrentFrame;
    OCMIDIFile MidiData;
    QTimer ScrollTimer;
    QTimeLine swipeLine;
    const int StaffPos(const int Staff) const;
    const int PointerBegin(const int Staff, const int Voice) const;
    const int PointerBegin() const;
    void NextStaff(const int Direction);
    const int KeepIn(const int num, const int Low, const int High) const;
    const int StaffOrder(const int Staff) const;
    void putFrame(const QPointF& moved, const int Modifiers);
    void zeroSwipe();
};

#endif // SCOREVIEWXML_H
