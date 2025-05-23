#ifndef SCOREVIEWXML_H
#define SCOREVIEWXML_H

//#include <QGraphicsView>
//#include <QGraphicsScene>
//#include <QScrollBar>
//#include <QWheelEvent>
#include <QApplication>
#include <QWidget>
#include <QTimer>
#include "ocscore.h"
#include "qhoverbutton.h"
#include "qiphotorubberband.h"
#include "qgraphicsviewzoomer.h"

#define defaultsystemlength 12900

namespace Ui {
    class ScoreViewXML;
}

enum ImportResult {
    NoImport,
    ImportNativeXML,
    ImportMusicXML,
    ImportNativeByteArray
};

class ScoreViewXML : public QGraphicsView
{
    Q_OBJECT
public:
    enum PageMode
    {
        PageSizeFixed,
        PageSizeFollowsResize,
        PageSizeUnlimited
    };
    OCCursor Cursor;
    ScoreViewXML(QWidget *parent = nullptr);
    ~ScoreViewXML();
    PageMode FollowResize() const;
    int ActiveStaffId() const;
    int ActiveVoice() const;
    const OCVoiceLocation ActiveVoiceLocation() const;
    const OCBarLocation ActiveBarLocation() const;
    int BarNrOffset() const;
    int MasterStaff() const;
    int NoteSpace() const;
    int StartBar() const;
    int EndBar() const;
    double Size() const;
    bool Locked() const;
    bool HideBarNumbers() const;
    void SetSystemLength(const int NewSystemLength);
    double SystemLength() const;
    const QRectF systemRect() const;
    void createSceneRect();
    void Paint(const OCRefreshMode Mode, const bool UpdateSelection=false);
    void PasteClipBoardData(const OCSymbolLocation& SymbolLocation, const XMLVoiceWrapper& ClipBoardData);
    void PasteClipBoardData(const int Pointer, const XMLVoiceWrapper& ClipBoardData);
    const XMLVoiceWrapper GetClipBoardData(const OCVoiceLocation& VoiceLocation, const OCSymbolRange& SymbolRange) const;
    const XMLVoiceWrapper GetClipBoardData(const OCSymbolRange& SymbolRange) const;
    const XMLVoiceWrapper GetClipBoardData(const OCVoiceLocation& VoiceLocation, const OCPointerList& Pointers) const;
    const XMLVoiceWrapper GetClipBoardData(const OCPointerList& Pointers) const;
    const XMLVoiceWrapper GetClipBoardData() const;
    void ensureVisible(const int iStaff);
    void ensureVisible();
    void scrollToBar(const int Bar);
    int BarsActuallyDisplayed() const;
    bool CanTurnPage() const;
    const OCBarSymbolLocation findPointerToBar(const OCBarLocation& Bar) const;
    const OCBarSymbolLocation findPointerToBar(const OCVoiceLocation& VoiceLocation, const int BarToFind) const;
    const OCBarSymbolLocation findPointerToBar(const int BarToFind) const;
    void play(const int PlayFromBar, const int Silence, const QString& Path);
    const QByteArray MIDIPointer(const int PlayFromBar, const int Silence);
    int findCurrentMeter(const OCSymbolLocation& SymbolLocation) const;
    const QVector<unsigned long> tickList() const;
    void pasteSymbol(XMLSimpleSymbolWrapper& Symbol);
    void insertSymbol(const OCSymbolLocation& SymbolLocation, XMLSimpleSymbolWrapper& Symbol);
    void insertSymbol(const int Pointer, XMLSimpleSymbolWrapper& Symbol);
    void insertSymbol(XMLSimpleSymbolWrapper& Symbol);
    int VoiceCount(const int Staff) const;
    int VoiceCount() const;
    int StaffCount() const;
    void SetXMLScore(XMLScoreWrapper& Doc);
    ImportResult Load(const QString& Path);
    bool Save(const QString& Path);
    void serialize(QDomLiteElement* xml) const;
    void unserialize(const QDomLiteElement* xml);
    XMLSimpleSymbolWrapper GetSymbol(const OCSymbolLocation& SymbolLocation) const;
    XMLSimpleSymbolWrapper GetSymbol(const int Pointer) const;
    XMLSimpleSymbolWrapper CurrentSymbol() const;
    const OCBarMap& BarMap() const;
    int VoiceLen(const OCVoiceLocation& VoiceLocation) const;
    int VoiceLen() const;
    int EndOfVoiceBar(const OCVoiceLocation& VoiceLocation) const;
    int EndOfVoiceBar() const;
    void SetXML(XMLScoreWrapper& NewXML);
    void SetXML(QDomLiteDocument* NewXML);
    void ReloadXML();
    XMLScoreWrapper XMLScore;
    const QString StaffName(const int Staff) const;
    const OCSelectionList SelectionList() const;
    const OCSelectionList SelectionList(const int Bar1, const int Bar2, const int Staff1, const int Staff2) const;
    int MarkToVoice(const int Mark) const;
    int MarkToStaff(const int Mark) const;
    const OCVoiceLocation MarkToVoiceLocation(const int Mark) const;
    int VoiceLocationToMark(const OCVoiceLocation& v) const;
    bool navigationVisible() const;
    QAction* setAction(const QKeySequence keySequence, const QString title = QString());
    const OCBarSymbolLocationList search(const QString& SearchTerm, const int Staff=-1, const int Voice=-1);
    QMenu* EditMenu;
    QAction* actionSelectAll;
    QAction* actionSelectHome;
    QAction* actionSelectEnd;
    QAction* actionSwapForward;
    QAction* actionSwapBack;
    QMenu* ScoreMenu;
    QAction* actionPreviousVoice;
    QAction* actionNextVoice;
    QAction* actionPreviousStaff;
    QAction* actionNextStaff;

public slots:
    void setNavigationVisible(const bool newShowNavigation);
    void setActiveTemplate();
    void setActiveTemplate(const XMLTemplateWrapper& XMLTemplate);
    void setActiveOptions();
    void setActiveOptions(const XMLScoreOptionsWrapper& XMLOptions);
    void setFollowResize(const ScoreViewXML::PageMode NewFollowResize);
    void setActiveStaffId(const int id);
    void setActiveVoice(int NewActiveVoice);
    void setActiveVoiceLocation(const OCVoiceLocation& v);
    void setActiveBarLocation(const OCBarLocation& b);
    void setBarNrOffset(const int NewBarNrOffset);
    void setMasterStaff(const int NewMasterStaff);
    void setNoteSpace(const int NewNoteSpace);
    void setStartBar(const int NewStartBar);
    void setEndBar(const int NewEndBar);
    void setSize(const double NewSize);
    void setLocked(const bool NewLocked);
    void setHideBarNumbers(const bool NewHideBarNumbers);
    void sound(const int pitch=-1);
    void sound(const OCSymbolLocation& symbol);
    void soundOff();
    void shiftPrefered(const int add);
    void transposeSelected(const int add);
    void dottify(int factor = 0);
    void Delete(const OCVoiceLocation& VoiceLocation, const OCSymbolRange& SymbolRange);
    void Delete(const OCSymbolRange& SymbolRange);
    void Delete(const OCVoiceLocation& VoiceLocation, const OCPointerList& Pointers);
    void Delete(const OCPointerList& Pointers);
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
    void selectToEnd();
    void selectToStart();
    void selectAll();
    void selectEndExtend();
    void selectHomeExtend();
    void selectNextStaff();
    void selectPrevStaff();
    void selectNextVoice();
    void selectPrevVoice();
    void selectPreferedUp();
    void selectPreferedDown();
    void selectPitchUp();
    void selectPitchDown();
    void selectOctaveUp();
    void selectOctaveDown();
    void selectBackSpace();
    void selectDelete();
    void selectSwapForward();
    void selectSwapBack();
    void flashSelected();
    void changeZoom(double zoom);
    void setZoom(const double Zoom);
    void toggleAltKey(bool v);
    void setNoteConfig();
public:
    double getZoom() const;
protected:
    bool viewportEvent(QEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseDoubleClickEvent (QMouseEvent* event);
    void resizeEvent(QResizeEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void scrollContentsBy(int dx, int dy);
    void wheelEvent(QWheelEvent* event);
    void leaveEvent(QEvent *event);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent* event);
#else
    void enterEvent(QEvent* event);
#endif
signals:
    void Changed();
    void ScoreChanged();
    void SelectionChanged();
    void NoteOnOff(bool On, int Pitch, int MixerTrack, OCMIDIVars MIDIInfo);
    void StaffIndexChanged(int Staff);
    void BarChanged();
    void ProcessingStaff(int Staff);
    void Popup(QPoint Pos);
    void PropertiesPopup(QPoint Pos);
    void BarsPopup(QPoint Pos);
    void ListPopup(QPoint Pos);
    void BackMeUp(const QString& Text);
    void NavigationForwardClicked();
    void NavigationBackClicked();
    void NavigationEndClicked();
    void NavigationHomeClicked();
    void SwipeLeftToRight();
    void SwipeRightToLeft();
    void ZoomChanged(double Zoom);
    void RequestSymbol(XMLSimpleSymbolWrapper& Symbol, QString Text);
    void RequestDuratedSymbol(XMLSimpleSymbolWrapper& Symbol, QString Text);
    void RequestNote(XMLSimpleSymbolWrapper& note);
    void accepted();
    void canceled();
private slots:
    void swipeProc(int value);
private:
    QGraphicsViewZoomer* zoomer;
    const QBrush paperbrush=QBrush(QPixmap(":/lightpaperfibers.png"));
    int m_StartBar;
    enum MouseAreas
    {
        MouseOutside,
        MouseOnSymbol,
        MouseOnBar
    };
    Ui::ScoreViewXML *ui;
    double swipeDelta;
    bool touchDown;
    int swipePos;
    int swipeBackPos;
    double m_SystemLength;
    int m_EndBar;
    bool Dragging;
    bool MouseDown;
    Qt::MouseButton MouseButton;
    MouseAreas MouseArea;
    bool altMod = false;
    int MouseAreaIndex;
    bool altModifier (const QMouseEvent* event = nullptr);
    void writeAltKeySymbol(const OCPointerList& ptrs);
    void writeAltkeyNote(const QPointF& mappedPos);
    void writeMoveSymbol(const QPointF& moved, const Qt::KeyboardModifiers& modifiers);
    int insideStaffId(const QPointF& p) const;
    int pitchFromPoint(const OCSymbolLocation& l, const QPointF& m);
    QPointF m_HoldMappedPos;
    OCMIDIVars CurrentMIDI;
    int soundMark;
    int soundPitch;
    QTimer soundTimer;
    bool m_NavigationVisible;
    bool m_Locked;
    QiPhotoRubberband* SelectRubberband;
    QHoverRubberband* HoverRubberband;
    OCScore Score;
    OCCursorFrame* CursorFrame;
    QGraphicsScene* Scene;
    QHoverButton* turnpagebutton;
    QHoverButton* turnbackbutton;
    QHoverButton* fastforwardbutton;
    QHoverButton* fastbackbutton;
    XMLVoiceWrapper m_XMLLastPasted;
    XMLTemplateWrapper ActiveTemplate;
    XMLScoreOptionsWrapper ActiveOptions;
    XMLSimpleSymbolWrapper LastSymbol;
    XMLSimpleSymbolWrapper LastNote;
    OCDraw ScreenObj;
    const OCFrameProperties& CurrentFrame();
    OCMIDIFile MidiData;
    QTimer ScrollTimer;
    QTimeLine swipeLine;
    int activeStaffTop() const;
    const OCBarSymbolLocation pointerBegin(const OCVoiceLocation& Voice) const;
    const OCBarSymbolLocation pointerBegin() const;
    void nextStaff(const int Direction);
    int activeStaffPos() const;
    double scaledToScene(const double v) const;
    QRectF scaledToScene(const QRectF v) const;
    //double staffTopScaledToScene(int id) const;
    //double activeStaffTopScaledToScene() const;
    double scaledFromScene(const double v) const;
    QPointF scaledFromScene(const QPointF v) const;
    void zeroSwipe();
    QRectF mapToSceneRect(const QRect& r);
    QRect mapFromSceneRect(const QPointF& a, const QPointF& b);
    const QRect mapFromSceneRect(const QRectF& r);
};

#endif // SCOREVIEWXML_H
