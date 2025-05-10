#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cscoredoc.h"
//#include "cdocumentloader.h"
//#include <QFileDialog>
//#include <QTreeWidget>
//#include <QProcess>
#include <qfadingwidget.h>
//#include <QMessageBox>
//#include <QFileDialog>
//#include <QMenuBar>
//#include <quazipfile.h>
//#include <QHBoxLayout>
#include "corebuffer.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow, public IMainPlayer
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    CCoreMainBuffers mainBuffer;
//public slots:
//    void NoteOnOff(bool On, int Pitch, int MixerTrack, OCMIDIVars MIDIInfo);
protected:
    virtual void closeEvent(QCloseEvent *event);
    //bool event(QEvent *event);
private:
    Ui::MainWindow *ui;
    CScoreDoc* m_Document;
    //CProjectPage* m_ProjectPage;
    //CProjectApp* m_ProjectApp;
    //OCPlayControl* playControl;
    //CMIDI2wav* midi2Wav;
    void play(const bool FromStart);
    void skip(const ulong64 samples);
    void pause();
    bool isPlaying() const;
    ulong64 samples() const;
    ulong milliSeconds() const;
    ulong ticks() const;
    ulong64 currentSample() const;
    ulong currentMilliSecond() const;
    void renderWaveFile(const QString path);
private slots:
    void SetVol(int v) {
        mainBuffer.outputVol = v*0.03;
    }
};

#endif // MAINWINDOW_H
