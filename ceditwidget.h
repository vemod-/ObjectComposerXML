#ifndef CEDITWIDGET_H
#define CEDITWIDGET_H

#include <QWidget>
#include <QDomLite>
#include "ocxmlwrappers.h"

namespace Ui {
    class CEditWidget;
}

class CEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CEditWidget(QWidget *parent = nullptr);
    ~CEditWidget();
    void PutCombo(const QString& Caption, int Value, const QStringList& List);
    void PutSpin(const QString& Caption,int Value,int Min,int Max);
    void PutNoteval(int Noteval, int Dotted, bool Triplet);
    void PutTime(int Type, int Upper, int Lower);
    void PutKey(int Key);
    void PutAccidentals(const int* Acc);
    void PutTempo(int Tempo, int Noteval, bool Dotted);
    void PutSysEx(const QString& Sysex);
    void PutSysEx(QDomLiteElement* data);
    void PutText(const QString& Text, const QFont& Font);
    void PutText(const XMLTextElementWrapper& data);
    void PutFont(const QFont& Font);
    void PutController(int Controller, int Value);
    void PutController(QDomLiteElement* data);
    void PutPortamento(int Controller, int Value);
    void PutPortamento(QDomLiteElement* data);
    int GetCombo();
    int GetSpin();
    void GetNoteval(int& Noteval, int& Dotted, bool& Triplet);
    void GetTime(int& Type, int& Upper, int& Lower);
    void GetKey(int& Key);
    void GetAccidentals(int* Acc);
    void GetTempo(int& Tempo, int& Noteval, bool& Dotted);
    void GetSysEx(QString& Sysex);
    void GetSysEx(QDomLiteElement* data);
    void GetText(QString& Text, QFont& Font);
    void GetText(XMLTextElementWrapper& data);
    void GetFont(QFont& Font);
    void GetController(int& Controller, int& Value);
    void GetController(QDomLiteElement* data);
    void GetPortamento(int& Controller, int& Value);
    void GetPortamento(QDomLiteElement* data);
private:
    Ui::CEditWidget *ui;
    void SetVisible(QWidget* w);
private slots:
    void NoteButtonSelected(int Button);
    void SetTimeElements(int Value);
signals:
    void Changed();
};

#endif // CEDITWIDGET_H
