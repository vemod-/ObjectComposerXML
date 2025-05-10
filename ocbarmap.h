#ifndef OCBARMAP_H
#define OCBARMAP_H

#include "CommonClasses.h"

class OCBarWindowBar
{
public:
    inline OCBarWindowBar()
    {
        Meter=96;
        MeterText="4/4";
        Pointer=0;
        reset();
    }
    inline void reset() {
        Density = 0;
        Notes = 0;
        IsFullRest = false;
        IsFullRestOnly = false;
        ClefChangeOnOne = false;
        KeyChangeOnOne = false;
        MasterStuff = false;
    }
    inline void setMeter(const int meter, const QString& meterText) {
        Meter=meter;
        MeterText=meterText;
    }
    int Meter;
    QString MeterText;
    int Pointer;
    int Density;
    int Notes;
    bool IsFullRest;
    bool IsFullRestOnly;
    bool KeyChangeOnOne;
    bool ClefChangeOnOne;
    bool MasterStuff;
};

class OCBarWindowVoice
{
public:
    inline OCBarWindowVoice(const int voice) {
        Voice=voice;
        Incomplete=false;
        EndPointer=0;
    }
    inline int barCount() const { return Bars.size(); }
    inline int minBar(const int bar) const { return hiBound<int>(bar,barCount()-1); }
    inline const OCBarWindowBar& bar(const int index) const { return Bars.at(index); }
    inline void append(const OCBarWindowBar& b) { Bars.append(b); }
    inline bool isEmpty() const { return Bars.isEmpty(); }
    inline bool voiceMatch(const OCVoiceLocation& VoiceLocation) const {
        if (VoiceLocation.StaffId==StaffId) if (VoiceLocation.Voice==Voice) return true;
        return false;
    }
    inline void setStaffParams(const int staffId, const int numVoices, const QString& name) {
        StaffId=staffId;
        NumOfVoices=numVoices;
        Name=name;
    }
    QString Name;
    int StaffId;
    int Voice;
    int NumOfVoices;
    int EndPointer;
    bool Incomplete;
private:
    QList<OCBarWindowBar> Bars;
};

class OCBarMap
{
public:
    OCBarMap() {}
    inline int BarMapIndex(const OCVoiceLocation& VoiceLocation) const {
        for (int i=0;i<Voices.size();i++)
        {
            const OCBarWindowVoice& bwv=Voices.at(i);
            if (bwv.voiceMatch(VoiceLocation))
            {
                if (!bwv.isEmpty()) return i;
                return -1;
            }
        }
        return -1;
    }
    inline int BarMapIndex(const int StaffId)
    {
        return BarMapIndex(OCVoiceLocation(StaffId));
    }
    inline const OCBarSymbolLocation GetPointer(const OCBarLocation& BarLocation) const {
        int voiceNum=BarMapIndex(BarLocation);
        if (voiceNum < 0) return OCBarSymbolLocation(BarLocation,0);
        const OCBarWindowVoice& bwv=Voices.at(voiceNum);
        if (BarLocation.Bar >= bwv.barCount()) return OCBarSymbolLocation(BarLocation,bwv.EndPointer);
        return OCBarSymbolLocation(BarLocation,bwv.bar(bwv.minBar(BarLocation.Bar)).Pointer);
    }
    inline const OCBarSymbolLocation GetBar(const OCSymbolLocation& SymbolLocation) const {
        int voiceNum=BarMapIndex(SymbolLocation);
        if (voiceNum < 0) return OCBarSymbolLocation(0,SymbolLocation);
        const OCBarWindowVoice& bwv=Voices.at(voiceNum);
        for (int i=0;i<bwv.barCount();i++) if (SymbolLocation.Pointer < bwv.bar(i).Pointer) return OCBarSymbolLocation(loBound<int>(0,i-1),SymbolLocation,bwv.bar(loBound<int>(0,i-1)).Pointer);
        if (SymbolLocation.Pointer < bwv.EndPointer) return OCBarSymbolLocation(bwv.barCount()-1,SymbolLocation,bwv.bar(bwv.barCount()-1).Pointer);
        return OCBarSymbolLocation(0,SymbolLocation);
    }
    /*
    inline int GetBar(const int Pointer, const OCVoiceLocation& VoiceLocation) const {
        return GetBar(OCSymbolLocation(VoiceLocation,Pointer));
    }
    */
    inline int GetMeter(const OCBarSymbolLocation& BarSymbolLocation) const {
        int voiceNum=BarMapIndex(BarSymbolLocation);
        if (voiceNum < 0) return 96;
        const OCBarWindowVoice& bwv=Voices.at(voiceNum);
        return bwv.bar(bwv.minBar(BarSymbolLocation.Bar)).Meter;
    }
    inline int GetMeter(const OCBarLocation& BarLocation) const {
        return GetMeter(OCBarSymbolLocation(BarLocation,0));
    }
    inline int BarCountAll() const {
        int Count=0;
        for(int i=0;i<Voices.size();i++) Count=qMax<int>(Voices.at(i).barCount(),Count);
        return Count;
    }
    inline int barCount(const OCVoiceLocation& VoiceLocation) const { return barCount(BarMapIndex(VoiceLocation)); }
    inline int barCount(const int Voice) const {
        if (Voice < 0) return 0;
        return Voices.at(Voice).barCount();
    }
    int BarCountAll(const XMLTemplateWrapper& XMLTemplate) const
    {
        int Count=0;
        for (int StaffPos=0;StaffPos<XMLTemplate.staffCount();StaffPos++)
        {
            const int BarMapStaffIndex=BarMapIndex(XMLTemplate.staffId(StaffPos));
            if (BarMapStaffIndex>-1) {
                for (int Voice=0;Voice<Voices.at(BarMapStaffIndex).NumOfVoices;Voice++) Count=qMax<int>(Voices.at(BarMapStaffIndex+Voice).barCount(),Count);
            }
        }
        return Count;
    }
    bool IsEnded(const int Bar, const XMLTemplateWrapper& XMLTemplate) const
    {
        for (int StaffPos=0;StaffPos<XMLTemplate.staffCount();StaffPos++)
        {
            const int BarMapStaffIndex=BarMapIndex(XMLTemplate.staffId(StaffPos));
            if (BarMapStaffIndex>-1)
            {
                for (int Voice=0;Voice<Voices.at(BarMapStaffIndex).NumOfVoices;Voice++)
                {
                    const OCBarWindowVoice& bwv=Voices.at(BarMapStaffIndex+Voice);
                    if (Bar<bwv.barCount())
                    {
                        if ((bwv.bar(Bar).Notes) || (bwv.bar(Bar).IsFullRest)) return false;
                    }
                }
            }
        }
        return true;
    }
    int EndOfVoiceBar(const OCVoiceLocation& VoiceLocation) const
    {
        int voiceNum=BarMapIndex(VoiceLocation);
        if (voiceNum<0) return 0;
        int Count=Voices.at(voiceNum).barCount();
        if (!Voices.at(voiceNum).Incomplete) return Count;
        return Count-1;
    }
    int NoteCount(const OCBarLocation& BarLocation) const
    {
        int voiceNum=BarMapIndex(BarLocation);
        if (voiceNum<0) return 0;
        if (BarLocation.Bar >= Voices.at(voiceNum).barCount()) return 0;
        return Voices.at(voiceNum).bar(BarLocation.Bar).Notes;
    }
    int NoteCountStaff(const int Staff, const int StartBar, const int EndBar) const
    {
        int Count=0;
        int StaffIndex=BarMapIndex(Staff);
        if (StaffIndex<0) return 0;
        for (int i=StartBar; i<EndBar; i++)
        {
            for (int VoiceIndex=0; VoiceIndex<Voices.at(StaffIndex).NumOfVoices; VoiceIndex++)
            {
                Count+=NoteCount(OCBarLocation(Staff,VoiceIndex,i));
            }
        }
        return Count;
    }
    bool IsFullRest(const int Bar, const XMLTemplateWrapper& XMLTemplate) const
    {
        if (BarCountAll(XMLTemplate)<=Bar) return false;
        bool FullRest=true;
        for (int StaffPos=0;StaffPos<XMLTemplate.staffCount();StaffPos++)
        {
            const int BarMapStaffIndex=BarMapIndex(XMLTemplate.staffId(StaffPos));
            if (BarMapStaffIndex>-1)
            {
                for (int Voice=0;Voice<Voices.at(BarMapStaffIndex).NumOfVoices;Voice++)
                {
                    const OCBarWindowVoice& bwv=Voices.at(BarMapStaffIndex+Voice);
                    if (Bar < bwv.barCount())
                    {
                        FullRest &= bwv.bar(Bar).IsFullRest;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
        return FullRest;
    }
    bool IsFullRestOnly(const int Bar, const XMLTemplateWrapper& XMLTemplate) const
    {
        if (BarCountAll(XMLTemplate)<=Bar) return false;
        bool FullRest=true;
        for (int StaffPos=0;StaffPos<XMLTemplate.staffCount();StaffPos++)
        {
            const int BarMapStaffIndex=BarMapIndex(XMLTemplate.staffId(StaffPos));
            if (BarMapStaffIndex>-1)
            {
                for (int Voice=0;Voice<Voices.at(BarMapStaffIndex).NumOfVoices;Voice++)
                {
                    const OCBarWindowVoice& bwv=Voices.at(BarMapStaffIndex+Voice);
                    if (Bar<=bwv.barCount()-1)
                    {
                        FullRest &= bwv.bar(Bar).IsFullRestOnly;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
        return FullRest;
    }
    bool ClefChange(const OCBarLocation& BarLocation) const
    {
        int voiceNum=BarMapIndex(BarLocation);
        if (voiceNum<0) return false;
        if (BarLocation.Bar >= Voices.at(voiceNum).barCount()) return false;
        return Voices.at(voiceNum).bar(BarLocation.Bar).ClefChangeOnOne;
    }
    bool KeyChange(const OCBarLocation& BarLocation) const
    {
        int voiceNum=BarMapIndex(BarLocation);
        if (voiceNum<0) return false;
        if (BarLocation.Bar >= Voices.at(voiceNum).barCount()) return false;
        return Voices.at(voiceNum).bar(BarLocation.Bar).KeyChangeOnOne;
    }
    bool MasterStuff(const int Bar, const int Staff) const
    {
        bool Master=false;
        int StaffIndex=BarMapIndex(Staff);
        if (StaffIndex>-1)
        {
            for (int Voice=0;Voice<Voices.at(StaffIndex).NumOfVoices;Voice++)
            {
                const OCBarWindowVoice& bwv=Voices[StaffIndex+Voice];
                if (Bar < bwv.barCount()) Master=Master | bwv.bar(Bar).MasterStuff;
            }
        }
        return Master;
    }
    inline int StaffNum(const int Voice) const { return Voices.at(Voice).StaffId; }
    inline int VoiceNum(const int Voice) const { return Voices.at(Voice).Voice; }
    inline bool isIncomplete(const int Voice) const { return Voices.at(Voice).Incomplete; }
    inline const QString voiceCaption(const int Voice) const {
        const OCBarWindowVoice& bwv=Voices.at(Voice);
        if (bwv.NumOfVoices>1) return bwv.Name+" ("+QString::number(bwv.Voice+1)+")";
        return bwv.Name;
    }
    inline const QString tooltipText(const int Voice, const int Bar) const {
        const OCBarWindowBar& bwb=Voices.at(Voice).bar(Bar);
        return "\nBar "+ QString::number(Bar+1)
                         +"\nMeter"+" "+bwb.MeterText
                         //+"\nNotes "+QString::number(bwb.Notes)
                        //+"\nPointer "+QString::number(bwb.Pointer)
                         + "\n" + (bwb.IsFullRest ? QStringLiteral("Rest only"):QStringLiteral("Notes ")+QString::number(bwb.Notes))
                         //+"\nFullRest"+QString::number(bwb.IsFullRest)
                         //+"\nFullRestOnly"+QString::number(bwb.IsFullRestOnly)
                         //+"\nKeyChange"+QString::number(bwb.KeyChangeOnOne)
                         //+"\nClefChange"+QString::number(bwb.ClefChangeOnOne)
                         //+"\nMasterStuff"+QString::number(bwb.MasterStuff)
                ;
    }
    inline int density(const int Voice, const int Bar) const { return Voices.at(Voice).bar(Bar).Density; }
    //inline const OCBarWindowVoice& Voice(const int index) { return Voices.at(index); }
    inline void appendBar(const int Voice, const OCBarWindowBar& Bar) { Voices[Voice].append(Bar); }
    inline void setStaffParams(const int Voice, const int staffId, const int numVoices, const QString& name) {
        Voices[Voice].setStaffParams(staffId,numVoices,name);
    }
    inline void clear() { Voices.clear(); }
    inline int voiceCount() const { return Voices.size(); }
    inline bool isEmpty() const { return Voices.isEmpty(); }
    inline void appendVoice(const int index) { Voices.append(OCBarWindowVoice(index)); }
    inline void setIncomplete(const int Voice) { Voices[Voice].Incomplete=true; }
    inline void setEndPointer(const int Voice, const int Pointer) { Voices[Voice].EndPointer=Pointer; }
private:
    QList<OCBarWindowVoice> Voices;
};

#endif // OCBARMAP_H
