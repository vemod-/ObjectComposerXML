#ifndef MIDIFILECLASSES_H
#define MIDIFILECLASSES_H
//---------------------------------------------------------------------------
#include "qbinarymemoryfile.h"
//#include "CommonClasses.h"
#include <QList>

class OCMIDIFileTrack
{
public:
    QString Title;
    QString Text;
    int TrackNum;
    int Time;
    OCMIDIFileTrack();
    void WriteToFile(QBinaryMemoryFile& FileData);
    void Append(int Message, const int Data1, int Data2, const int NewTime=-1, const int Tempo=-1, const QString& Buffer=QString(), const int TimeU=-1, const int TimeD=-1, const int Key=-8);
private:
    int RunningStatus;
    QBinaryMemoryFile DataList;
    void AddHeader(QBinaryMemoryFile& data, const int DataLen);
    const QByteArray IntToVariableLength(const int Value);
};

class OCMIDIFile
{
private:
    QList<OCMIDIFileTrack*> Tracks;
    int TrackNum;
public:
    int TicksPerQuarter;
    OCMIDIFile();
    ~OCMIDIFile();
    QString Title;
    QString composer;
    void InsertTrack();
    void OpenIt();
    void CloseIt(const QString& Path);
    QByteArray MIDIPointer();
    void Append(const int Message, const int Data1, const int Data2, const int NewTime=-1, const int Tempo=-1, const QString& Buffer=QString(), const int TimeU=-1, const int TimeD=-1, const int Key=-8);
    void SetTrackNumber(const int TrackNumber);
    void PlayController(const int Number, const int Value, const int Channel);
    void PlayExpression(const int Value, const int Channel);
    void PlayPatch(const int Value, const int Channel);
    void Playtempo(const int Value);
    void SetTime(const int New_Time);
    const int GetTime() const;
    void PlayTime(const int TimeU, const int TimeD);
    void PlayKey(const int Key);
};

#endif // MIDIFILECLASSES_H
