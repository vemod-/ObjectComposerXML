#ifndef CEDITWIDGET_H
#define CEDITWIDGET_H

#include <QWidget>
#include <caccidentalspiano.h>
#include <QDomLite>

namespace Ui {
    class CEditWidget;
}

class CEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CEditWidget(QWidget *parent = 0);
    ~CEditWidget();
    void PutCombo(QString Caption, int Value, QStringList List);
    void PutSpin(QString Caption,int Value,int Min,int Max);
    void PutNoteval(int Noteval, bool Dotted, bool Triplet);
    void PutChannel(int Channel);
    void PutTime(int Type, int Upper, int Lower);
    void PutKey(int Key);
    void PutAccidentals(int* Acc);
    void PutTempo(int Tempo, int Noteval, bool Dotted);
    void PutSlanting(int Slanting);
    void PutRepeats(int Repeats);
    void PutStemDirection(int StemDirection);
    void PutSysEx(QString Sysex);
    void PutSysEx(QDomLiteElement* data);
    void PutText(QString Text, QFont Font);
    void PutText(QDomLiteElement* data);
    void PutFont(QFont Font);
    void PutTranspose(int Transpose);
    void PutController(int Controller, int Value);
    void PutController(QDomLiteElement* data);
    int GetCombo();
    int GetSpin();
    void GetNoteval(int& Noteval, bool& Dotted, bool& Triplet);
    void GetChannel(int& Channel);
    void GetTime(int& Type, int& Upper, int& Lower);
    void GetKey(int& Key);
    void GetAccidentals(int* Acc);
    void GetTempo(int& Tempo, int& Noteval, bool& Dotted);
    void GetSlanting(int& Slanting);
    void GetRepeats(int& Repeats);
    void GetStemDirection(int& StemDirection);
    void GetSysEx(QString& Sysex);
    void GetSysEx(QDomLiteElement* data);
    void GetText(QString& Text, QFont& Font);
    void GetText(QDomLiteElement* data);
    void GetFont(QFont& Font);
    void GetTranspose(int& Transpose);
    void GetController(int& Controller, int& Value);
    void GetController(QDomLiteElement* data);
private:
    Ui::CEditWidget *ui;
    void SetVisible(QWidget* w);
private slots:
    void NoteButtonSelected(int Button);
    void SetTimeElements(int Value);
};

#endif // CEDITWIDGET_H
