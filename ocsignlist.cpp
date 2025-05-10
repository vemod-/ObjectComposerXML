#include "ocsignlist.h"
#include "ocnotelist.h"

OCSignList::~OCSignList() { ClearSigns(); }

OCPlaySignList::~OCPlaySignList() {}

void OCPlaySignList::PlayBeforeNote(const XMLSymbolWrapper& XMLNote, int& PlayDynam, int& NoteOnPitch, int& NoteOffPitch, OCPlayBackVarsType &voiceVars)
{
    for (CSymbol* Sign : std::as_const(Signs)) Sign->BeforeNote(XMLNote, PlayDynam, NoteOnPitch, NoteOffPitch, voiceVars);
}

void OCPlaySignList::PlayAfterNote(const XMLSymbolWrapper& XMLNote, OCPlayBackVarsType &voiceVars)
{
    for (CSymbol* Sign : std::as_const(Signs)) Sign->AfterNote(XMLNote, voiceVars);
}

int OCPlaySignList::PlayDuringNote(OCMIDIFile& MFile, const int NoteOnPitch, int StartTime, const int PlayTime, OCPlayBackVarsType &currentVars)
{
    int RemainingTime = StartTime;
    for (int i=1;i<=PlayTime;i++)
    {
        RemainingTime ++;
        for (CSymbol* Sign : std::as_const(Signs))
        {
            if (Sign->PlayProps.Modulate > 0)
            {
                if (i % Sign->PlayProps.Modulate == 0)
                {
                    Sign->DuringNote(MFile, NoteOnPitch, RemainingTime, i, PlayTime, currentVars);
                }
            }
        }
    }
    return RemainingTime;
}

OCPrintSignList::~OCPrintSignList() {}

void OCPrintSignList::PrintSigns(const int TieLen, OCFrameArray& FrameList, const IOCRhythmObject* Props, OCDraw& ScreenObj)
{
    int SignsUp=0;
    for (CSymbol* Sign : std::as_const(Signs))
    {
        ScreenObj.setcol(Sign->PrintProps.Color);
        Sign->PrintProps.setPosition(Props->Center(),Props->BalkEndY,TieLen);
        FrameList.AppendGroup(ScreenObj.MakeGroup(Sign->PrintSign(Props->UpDown, SignsUp, ScreenObj)),Sign->PrintProps.Location);
    }
    ClearSigns();
}
