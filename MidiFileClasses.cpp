#include "MIDIFileClasses.h"
//#include <qmath.h>
#include "CommonClasses.h"

void OCMIDIFileTrack::WriteToFile(QBinaryMemoryFile& FileData)
{
    DataList.appendInt(0xFF2F00);
    AddHeader(FileData, DataList.size());// ' + 8
    FileData.append(DataList);
}

void OCMIDIFileTrack::Append(int Message, const int Data1, int Data2, const int NewTime, const int Tempo, const QString& Buffer, const int TimeU, const int TimeD, const int Key)
{
    if ((Message >= 0x80) && (Message <= 0x8F))
    {
        Message += 0x10;
        Data2 = 0;
    }
    if (NewTime!=-1) Time = NewTime;
    DataList.append(IntToVariableLength(Time));
    Time = 0;
    if ((Message != RunningStatus) || (Message < 0x80) || (Message > 0xDF)) DataList.appendByte(Message);
    RunningStatus = Message;
    if (Message == 0xF0)
    {
        DataList.append(IntToVariableLength(Buffer.length() - 1));
        DataList.appendString(Buffer);
        return;
    }
    DataList.appendByte(Data1);
    if (Tempo > -1)
    {
        DataList.appendInt(0x3000000 | Tempo);
    }
    else
    {
        if (! ((Message >= 0xC0) && (Message <= 0xDF))) DataList.appendByte(Data2);
    }
    if (TimeU > -1)
    {
        DataList.appendByte(TimeU);
        DataList.appendByte(qSqrt(TimeD));
        DataList.appendByte((240 * 4) / TimeD);
        DataList.appendByte(8);
    }
    if (Key > -8)
    {
        if (Key < 0)
        {
            DataList.appendByte(!(Abs(Key) - 1));
        }
        else
        {
            DataList.appendByte(Key);
        }
        DataList.appendByte(0);
    }
}

void OCMIDIFileTrack::AddHeader(QBinaryMemoryFile& data, const int DataLen)
{
    int DataLength = DataLen;
    if (Title.length() > 0) DataLength += 4 + Title.length();
    if (Text.length() > 0) DataLength += 4 + Text.length();
    data.appendString("MTrk");
    data.appendInt(DataLength);
    if (Title.length() > 0)
    {
         data.appendShort(0xFF);
         data.appendByte(3);
         data.appendByte(Title.length());
         data.appendString(Title);
    }
    if (Text.length() > 0)
    {
         data.appendShort(0xFF);
         data.appendByte(1);
         data.appendByte(Text.length());
         data.appendString(Text);
    }
}

const QByteArray OCMIDIFileTrack::IntToVariableLength(const int Value)
{
    QByteArray Bytes;
    unsigned int TempVal = Value;
    if (TempVal > 0x7F)
    {
        Bytes.append(TempVal & 0x7F);
        forever
        {
            TempVal = IntDiv(TempVal, 0x80);
            Bytes.prepend((TempVal & 0x7F) | 0x80);
            if (TempVal < 0x80) break;
            if (Bytes.count()>=10) break;
        }
    }
    else
    {
        Bytes.append(TempVal);
    }
    return Bytes;
}

void OCMIDIFile::InsertTrack()
{
    TrackNum=Tracks.count();
    Tracks.append(new OCMIDIFileTrack());
    Tracks[TrackNum]->TrackNum=TrackNum;
}

OCMIDIFileTrack::OCMIDIFileTrack()
{
    TrackNum=0;
    Time=0;
    RunningStatus=0;
}

OCMIDIFile::OCMIDIFile()
{
    TrackNum=0;
    TicksPerQuarter = 240;
    //QBinaryMemoryFile::copyMemory(0,0,0);
}

void OCMIDIFile::OpenIt()
{
    qDeleteAll(Tracks);
    Tracks.clear();
}

void OCMIDIFile::CloseIt(const QString& Path)
{
    QBinaryMemoryFile FileData;
    FileData.appendString("MThd");
    FileData.appendInt(6);
    FileData.appendShort(1);
    FileData.appendShort(Tracks.count());
    FileData.appendShort(TicksPerQuarter);
    Tracks[0]->Title = Title;
    Tracks[0]->Text = composer;
    foreach(OCMIDIFileTrack* t,Tracks) t->WriteToFile(FileData);
    FileData.save(Path);
}

QByteArray OCMIDIFile::MIDIPointer()
{
    QBinaryMemoryFile FileData;
    FileData.appendString("MThd");
    FileData.appendInt(6);
    FileData.appendShort(1);
    FileData.appendShort(Tracks.count());
    FileData.appendShort(TicksPerQuarter);
    Tracks[0]->Title = Title;
    Tracks[0]->Text = composer;
    foreach(OCMIDIFileTrack* t,Tracks) t->WriteToFile(FileData);
    return FileData.get();
}

void OCMIDIFile::Append(const int Message, const int Data1, const int Data2, const int NewTime, const int Tempo, const QString& Buffer, const int TimeU, const int TimeD, const int Key)
{
    Tracks[TrackNum]->Append(Message, Data1, Data2, NewTime, Tempo, Buffer, TimeU, TimeD, Key);
}

void OCMIDIFile::SetTrackNumber(const int TrackNumber)
{
    TrackNum = TrackNumber;
}

void OCMIDIFile::PlayController(const int Number, const int Value, const int Channel)
{
    Append(0xB0 + Channel, Number, Value);
}

void OCMIDIFile::PlayExpression(const int Value, const int Channel)
{
    Append(0xB0 + Channel, 0xB, Value);
}

void OCMIDIFile::PlayPatch(const int Value, const int Channel)
{
    Append(0xC0 + Channel, Value, 80);
}

void OCMIDIFile::Playtempo(const int Value)
{
    Append(0xFF, 0x51, 3,-1 , IntDiv(60000000, Value));
}

void OCMIDIFile::SetTime(const int New_Time)
{
    Tracks[TrackNum]->Time=New_Time;
}

const int OCMIDIFile::GetTime() const
{
    return Tracks[TrackNum]->Time;
}

void OCMIDIFile::PlayTime(const int TimeU, const int TimeD)
{
    Append(0xFF, 0x58, 4,-1 ,-1 ,QString() , TimeU, TimeD);
}

void OCMIDIFile::PlayKey(const int Key)
{
    Append(0xFF, 0x59, 2,-1 ,-1 ,QString() ,-1 ,-1 , Key);
}

OCMIDIFile::~OCMIDIFile()
{
    qDeleteAll(Tracks);
    Tracks.clear();
}
