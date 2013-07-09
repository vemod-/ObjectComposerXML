#ifndef OCPLAYCONTROL_H
#define OCPLAYCONTROL_H

#include <QWidget>
#include "qfadingframe.h"
/*
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
*/
#include <QToolButton>
#include <QTimer>
#include "EffectLabel.h"
#include <cmidi2wav.h>

namespace Ui {
    class OCPlayControl;
}

class OCPlayControl : public QFadingFrame
{
    Q_OBJECT

public:
    enum PlayLocation
    {
        PlayFromHere=0,
        PlayEverything=1,
        PlayStaffFromHere=2
    };
    explicit OCPlayControl(CMIDI2wav* m2w, QWidget *parent = 0);
    ~OCPlayControl();
    float Volume();
    QAction* actionFromHere;
    QAction* actionEverything;
    QAction* actionStaffFromHere;
    QAction* actionStop;
    QWidget* getPlayButton();
    QString MixerXML;
    void PreLoad(PlayLocation mode=PlayFromHere);
public slots:
    void Play(PlayLocation mode);
    void Stop();
    void StopAndHide();
    void TriggerFromHere();
    void TriggerEverything();
    void TriggerStaffFromHere();
    void ButtonPress(QMouseEvent* event);
signals:
    void RequestFile(QString &path,OCPlayControl::PlayLocation mode);
private:
    Ui::OCPlayControl *ui;
    CMIDI2wav* midi2Wav;
    EffectLabel* playButton;
    QTimer hideTimer;
    bool MouseWithin;
private slots:
    void StateChanged(MIDI2wavState s);
    void timedOut();
    void mouserEnter();
    void mouseLeave();
    void closeMe();
protected:
};

#endif // OCPLAYCONTROL_H
