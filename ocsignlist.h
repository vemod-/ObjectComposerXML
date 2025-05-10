#ifndef OCSIGNLIST_H
#define OCSIGNLIST_H

//#include "CommonClasses.h"
#include "csymbol.h"

class IOCRhythmObject;

enum Durations
{
    KillInstantly = 0,
    NotDecrementable = -32000
};

class OCSignList
{
protected:
    QList<CSymbol*> Signs;
    void ClearSigns()
    {
        qDeleteAll(Signs);
        Signs.clear();
    }
    void remove(CSymbol* Sign)
    {
        Signs.removeOne(Sign);
        delete Sign;
    }
public:
    OCSignList() {}
    virtual ~OCSignList();
    void append(const QColor& SignCol, const OCSymbolLocation& Location, CSymbol* Symbol)
    {
        Symbol->PrintProps.fill(*Symbol,Location,SignCol);
        Signs.append(Symbol);
    }
    void append(const int Duration, const int Modulate, CSymbol* Symbol)
    {
        Symbol->PlayProps.fill(Duration,Modulate);
        Signs.append(Symbol);
    }
    inline int size() const { return Signs.size(); }
    void remove(const QString& Name)
    {
        for (CSymbol* Sign : std::as_const(Signs)) if (Sign->name() == Name) remove(Sign);
    }
    void decrement(const int val) {
        for (CSymbol* Sign : std::as_const(Signs)) {
            if (Sign->PlayProps.Duration > val) {
                Sign->PlayProps.Duration -= val;
            }
            else if (Sign->PlayProps.Duration == NotDecrementable) {}
            else {
                remove(Sign);
            }
        }
    }
    inline int value(const QString& Name, const QString& PropertyName) const
    {
        for (CSymbol* Sign : Signs) if (Sign->name()==Name) return Sign->XMLIntValue(PropertyName);
        return 0;
    }
    inline int value(const QString& PropertyName) const
    {
        for (CSymbol* Sign : Signs) if (Sign->PropertyExists(PropertyName)) return Sign->XMLIntValue(PropertyName);
        return 0;
    }
    inline bool exist(const QString& Name) const
    {
        for (CSymbol* Sign : Signs) if (Sign->name() == Name) return true;
        return false;
    }
};

class OCPrintSignList : public OCSignList
{
public:
    OCPrintSignList() {}
    ~OCPrintSignList();
    void PrintSigns(const int TieLen, OCFrameArray& FrameList, const IOCRhythmObject* Props, OCDraw& ScreenObj);
};

class OCPlaySignList : public OCSignList
{
public:
    OCPlaySignList() {}
    ~OCPlaySignList();
    void PlayBeforeNote(const XMLSymbolWrapper& XMLNote, int& PlayDynam , int& NoteOnPitch, int& NoteOffPitch, OCPlayBackVarsType& voiceVars);
    void PlayAfterNote(const XMLSymbolWrapper& XMLNote, OCPlayBackVarsType& voiceVars);
    int PlayDuringNote(OCMIDIFile& MFile, const int NoteOnPitch, int StartTime, const int PlayTime, OCPlayBackVarsType& voiceVars);
};

typedef QVector<OCPlaySignList> OCPlaySignListArray;

#endif // OCSIGNLIST_H
