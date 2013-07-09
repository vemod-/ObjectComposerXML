#include "ocsignlist.h"

OCSignList::OCSignList()
{
    ClearSigns();
}

OCSignList::~OCSignList()
{
    ClearSigns();
}

void OCSignList::Append(const int Sign, const int Parantes, XMLSymbolWrapper& XMLSymbol, const QColor SignCol, const int Pointer, CSymbol *Symbol)
{
    PrintSignType* s=new PrintSignType;
    s->Props.Fill(Sign,XMLSymbol,Pointer,SignCol,Parantes);
    s->Symbol=Symbol;
    Signs.append(s);
}

void OCSignList::AppendPlay(const int Duration, const int Nr1, const int Modulate, CSymbol *Symbol)
{
    PrintSignType* s=new PrintSignType;
    s->Symbol=Symbol;
    s->PlayProps.Fill(Duration,Nr1,Modulate);
    Signs.append(s);
}

const int OCSignList::Count() const
{
    return Signs.count();
}

void OCSignList::ClearSigns()
{
    foreach (PrintSignType* Sign,Signs) delete Sign->Symbol;
    qDeleteAll(Signs);
    Signs.clear();
}

void OCSignList::KillByName(const QString& Name)
{
    foreach (PrintSignType* Sign,Signs) if (Sign->Symbol->Name()==Name) KillElem(Sign);
}

void OCSignList::Decrement(const int val)
{
    foreach (PrintSignType* Sign,Signs)
    {
        if (Sign->PlayProps.Duration>val)
        {
            Sign->PlayProps.Duration -= val;
        }
        else if (Sign->PlayProps.Duration==NotDecrementable)
        {
        }
        else
        {
            KillElem(Sign);
        }
    }
}

void OCSignList::KillElem(PrintSignType* Sign)
{
    Signs.removeOne(Sign);
    delete Sign->Symbol;
    delete Sign;
}

void OCSignList::PlayBeforeNote(XMLSymbolWrapper& XMLNote, int& PlayDynam, int& NoteOnPitch, int& NoteOffPitch, OCMIDIFile& MFile, OCPlayBackVarsType &pcurrent)
{
    foreach (PrintSignType* Sign,Signs) Sign->Symbol->BeforeNote(XMLNote, PlayDynam, NoteOnPitch, NoteOffPitch, MFile, pcurrent);
}

void OCSignList::PlayDuringNote(OCMIDIFile& MFile, const int NoteOnPitch, int& LastTime, const int NoteTime, OCPlayBackVarsType &pcurrent)
{
    for (int iTemp=1;iTemp<=NoteTime;iTemp++)
    {
        LastTime ++;
        foreach (PrintSignType* Sign,Signs)
        {
            if (Sign->PlayProps.Modulate > 0)
            {
                if (iTemp % Sign->PlayProps.Modulate == 0)
                {
                    Sign->Symbol->DuringNote(Sign->PlayProps, MFile, NoteOnPitch, LastTime, iTemp, NoteTime, pcurrent);
                }
            }
        }
    }
}

void OCSignList::PlayAfterNote(XMLSymbolWrapper& XMLNote, OCPlayBackVarsType &pcurrent)
{
    foreach (PrintSignType* Sign,Signs) Sign->Symbol->AfterNote(XMLNote, pcurrent);
}

void OCSignList::PlayCheckTempoChange(OCMIDIFile& MFile, const int NoteOnPitch, int& PlayTime, OCPlayBackVarsType &pcurrent, const int NoteTime)
{
    if (Signs.count()==0) return;
    int LastTime=0;
    MFile.SetTime(0);
    for (int iTemp=1;iTemp<=NoteTime;iTemp++)
    {
        LastTime ++;
        foreach (PrintSignType* Sign,Signs)
        {
            if (Sign->PlayProps.Modulate > 0)
            {
                if (iTemp % Sign->PlayProps.Modulate == 0)
                {
                    if (Sign->PlayProps.Modulate < NoteTime - iTemp)
                    {
                        Sign->Symbol->DuringNote(Sign->PlayProps, MFile, NoteOnPitch, LastTime, iTemp, NoteTime, pcurrent);
                    }
                }
            }
        }
    }
    MFile.SetTime(LastTime);
    PlayTime = LastTime;
    pcurrent.Currenttime = LastTime;
}

const QVariant OCSignList::Value(const QString& Name, const QString& PropertyName) const
{
    foreach (PrintSignType* Sign,Signs) if (Sign->Symbol->Name()==Name) return Sign->Symbol->PropertyValue(PropertyName);
    return QVariant();
}

const QVariant OCSignList::Value(const QString& PropertyName) const
{
    foreach (PrintSignType* Sign,Signs) if (Sign->Symbol->PropertyExists(PropertyName)) return Sign->Symbol->PropertyValue(PropertyName);
    return false;
}

const bool OCSignList::Exist(const QString& Name) const
{
    foreach (PrintSignType* Sign,Signs) if (Sign->Symbol->Name()==Name) return true;
    return false;
}

void OCSignList::PrintSign(const int TieLen, OCSymbolArray& SymbolList, const int Signx, const int UpDown, const float balkheight, const int Signy, OCDraw& ScreenObj)
{
    int SignsUp=0;
    foreach (PrintSignType* Sign,Signs)
    {
        ScreenObj.setcol(Sign->Props.Color);
        Sign->Props.Position(QPointF(Signx,Signy),balkheight,TieLen);
        Sign->Symbol->PrintSign(SymbolList, Sign->Props, UpDown, SignsUp, ScreenObj);
    }
    ClearSigns();
}
