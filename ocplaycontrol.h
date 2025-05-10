#ifndef OCPLAYCONTROL_H
#define OCPLAYCONTROL_H

#include <QWidget>
#include "qfadingframe.h"
//#include <QToolButton>
#include <QTimer>
#include <cmidi2wav.h>
#include <QWidgetAction>
#include <mouseevents.h>

namespace Ui {
    class OCPlayControl;
}

class OCPlayControl : public QFadingFrame
{
    Q_OBJECT

public:
    /*
    enum PlayLocation
    {
        PlayFromHere=0,
        PlayEverything=1,
        PlayStaffFromHere=2
    };
*/
    explicit OCPlayControl(CMIDI2wav* m2w, QWidget *parent = nullptr);
    ~OCPlayControl();
    float Volume();
    QAction* actionFromHere;
    QAction* actionEverything;
    QAction* actionStaffFromHere;
    QAction* actionStop;
    QMenu* actionMenu;
    QAction* getPlayButton();
    QAction* getMixerButton();
    void SetVol(int v);
public slots:
    void Play(CMIDI2wav::PlayLocation mode);
    void ToggleMixer();
    void Stop();
    void StopAndHide();
    void TriggerFromHere();
    void TriggerEverything();
    void TriggerStaffFromHere();
    void LeftButtonPress();
    void PopupMenu();
signals:
    void PlayPointerChanged(int b);
    void VolChanged(int v);
private:
    Ui::OCPlayControl *ui;
    CMIDI2wav* midi2Wav;
    QAction* playButton;
    QAction* mixerButton;
    QTimer hideTimer;
    bool MouseWithin;
    QMenu* mixerMenu;
    QWidgetAction* mixerAction;
    int m_TimerID = 0;
    QString elapsed(ulong c);
    QString remaining(ulong c);
    ulong m_mSecs = 0;
    MouseEvents* ev1;
    void ChangeGeometry();
private slots:
    void SliderSkip(int v);
    void Mute();
    void Max();
    void timedOut();
    void mouserEnter();
    void mouseLeave();
    void closeMe();
    void RightButtonPress(QMouseEvent* e);
protected:
    void timerEvent(QTimerEvent* e);
};

#endif // OCPLAYCONTROL_H
