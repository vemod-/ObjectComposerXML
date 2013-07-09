#ifndef OCNOTETOOLBOX_H
#define OCNOTETOOLBOX_H

#include <QWidget>
#include <QFrame>
#include "CommonClasses.h"
#include "csymbol.h"
#include "ocsymbolscollection.h"

namespace Ui {
    class OCNoteToolbox;
}

class OCNoteToolbox : public QFrame
{
    Q_OBJECT

public:
    explicit OCNoteToolbox(QWidget *parent = 0);
    ~OCNoteToolbox();
public slots:
    void TriggerNotes(QList<QPair<int, int> > &Notes);
signals:
    void PasteXML(XMLSimpleSymbolWrapper& Symbol, QString UndoText, bool Finished);
    void OverwriteProperty(QString Name,QVariant Value, QString UndoText, bool Finished);
private:
    Ui::OCNoteToolbox *ui;
    void DecrementTimes();
    int DotTimes;
    int TripletTimes;
    QList<QPair<int,int> > RecordList;
    int RecordCount;
    bool isRecording;
private slots:
    void SelectNote(int value);
    void SelectPatternButton(int value);
    void SelectTripletButton(int value);
    void SelectDotButton(int value);
    void PatternSelectClicked(int value);
    void PauseButtonClicked(int value);
};

#endif // OCNOTETOOLBOX_H
