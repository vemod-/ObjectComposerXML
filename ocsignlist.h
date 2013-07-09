#ifndef OCSIGNLIST_H
#define OCSIGNLIST_H

#include "CommonClasses.h"
#include "csymbol.h"

enum Durations
{
    KillInstantly = 0,
    NotDecrementable = -32000
};

struct PrintSignType
{
    PrintSignProps Props;
    PlaySignProps PlayProps;
    CSymbol* Symbol;
};

class OCSignList
{
private:
    QList<PrintSignType*> Signs;
    void ClearSigns();
    void KillElem(PrintSignType* Sign);
public:
    OCSignList();
    ~OCSignList();
    void Append(const int Sign, const int Parantes, XMLSymbolWrapper& XMLSymbol, const QColor SignCol, const int Pointer, CSymbol* Symbol);
    void AppendPlay(const int Duration, const int Nr1, const int Modulate, CSymbol* Symbol);
    const int Count() const;
    void KillByName(const QString& Name);
    void Decrement(const int val);
    void PlayBeforeNote(XMLSymbolWrapper& XMLNote, int& PlayDynam , int& NoteOnPitch, int& NoteOffPitch, OCMIDIFile& MFile, OCPlayBackVarsType& pcurrent);
    void PlayDuringNote(OCMIDIFile& MFile, const int NoteOnPitch, int& LastTime, const int NoteTime, OCPlayBackVarsType& pcurrent);
    void PlayAfterNote(XMLSymbolWrapper& XMLNote, OCPlayBackVarsType& pcurrent);
    void PlayCheckTempoChange(OCMIDIFile& MFile, const int NoteOnPitch, int& PlayTime, OCPlayBackVarsType& pcurrent, const int NoteTime);
    const QVariant Value(const QString& Name, const QString& PropertyName) const;
    const QVariant Value(const QString& PropertyName) const;
    const bool Exist(const QString& Name) const;
    void PrintSign(const int TieLen, OCSymbolArray& SymbolList, const int Signx, const int UpDown, const float balkheight, const int Signy, OCDraw& ScreenObj);
};

#endif // OCSIGNLIST_H
