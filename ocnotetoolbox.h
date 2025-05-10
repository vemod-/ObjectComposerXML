#ifndef OCNOTETOOLBOX_H
#define OCNOTETOOLBOX_H

#include <QWidget>
#include <QFrame>
#include "ocxmlwrappers.h"
#include "ocpiano.h"

namespace Ui {
    class OCNoteToolbox;
}

class OCNoteToolbox : public QFrame
{
    Q_OBJECT

public:
    explicit OCNoteToolbox(QWidget *parent = nullptr);
    ~OCNoteToolbox();
public slots:
    void TriggerNotes(OCInputNoteList &Notes);
    void GetCurrentNote(XMLSimpleSymbolWrapper& note);
signals:
    void PasteXML(XMLSimpleSymbolWrapper& Symbol, QString UndoText, bool Finished);
    void OverwriteProperty(QString Name,QVariant Value, QString UndoText, bool Finished);
    void ToggleWriteMode(bool writeMode);
    void NoteChanged();
private:
    enum TableModes {
        PatternTableIdle,
        PatternTableRecording,
        PatternTableApplying
    };
    enum PatternButtons {
        RecordPattern,
        StopApplyingPattern,
        SavePattern,
        AbortRecordingPattern,
        ManagePatterns
    };
    Ui::OCNoteToolbox *ui;
    void DecrementTimes();
    int DotTimes;
    int TripletTimes;
    OCPatternNoteList RecordList;
    int RecordCount;
    bool isRecording;
    void SetTable(const TableModes value);
    TableModes TableMode;
    int getDotted();
    void setDotted(const int d);
private slots:
    void SelectNote(int value);
    void SelectTripletButton(int value);
    void SelectDotButton(int value);
    void PauseButtonClicked(int value);
    void PatternButtonClicked(int value);
    void ToggleWriteModeButton(int value);
};

#endif // OCNOTETOOLBOX_H
