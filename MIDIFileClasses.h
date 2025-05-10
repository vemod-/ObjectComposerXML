#ifndef MIDIFILECLASSES_H
#define MIDIFILECLASSES_H
//---------------------------------------------------------------------------
#include "qbinarymemoryfile.h"
//#include <QList>
#include "cmidifilereader.h"
#include "softsynthsdefines.h"
;
#pragma pack(push,1)

typedef unsigned char BYTE;

class OCMIDIFileTrack
{
public:
    OCMIDIFileTrack()
    {
        m_Delta=0;
        m_RunningStatus=0;
    }
    void inline setDelta(const unsigned int DeltaTime) { m_Delta = DeltaTime; }
    void WriteToFile(QBinaryMemoryFile& FileData)
    {
        appendMeta(0x2F);
        MIDIFileTrackHeader h;
        setDescriptor(h.descriptor.id,"MTrk");
        h.descriptor.size=uint(qToBigEndian<qint32>(int(DataList.size())));
        FileData.append(&h,sizeof(MIDIFileTrackHeader));
        FileData.append(DataList);
    }
    inline void appendNoteOffEvent(const BYTE channel, const BYTE pitch, BYTE velocity=0, const int DeltaTime=-1)
    {
        if (m_RunningStatus==0x90+channel)
        {
            appendNoteOnEvent(channel,pitch,0,DeltaTime);
        }
        else
        {
            appendDeltaMessage(0x80+channel,DeltaTime);
            DataList.appendBytes(pitch,velocity);
        }
    }
    inline void appendNoteOnEvent(const BYTE channel, const BYTE pitch, const BYTE velocity, const int DeltaTime=-1)
    {
        appendDeltaMessage(0x90+channel,DeltaTime);
        DataList.appendBytes(pitch,velocity);
    }
    inline void appendAftertouchEvent(const BYTE channel, const BYTE pitch, const BYTE value, const int DeltaTime=-1)
    {
        appendDeltaMessage(0xA0+channel,DeltaTime);
        DataList.appendBytes(pitch,value);
    }
    inline void appendControllerEvent(const BYTE channel, const BYTE controller, const BYTE value, const int DeltaTime=-1)
    {
        appendDeltaMessage(0xB0+channel,DeltaTime);
        DataList.appendBytes(controller,value);
    }
    inline void appendPatchChangeEvent(const BYTE channel, const BYTE patch, const int DeltaTime=-1)
    {
        appendDeltaMessage(0xC0+channel,DeltaTime);
        DataList.appendByte(patch);
    }
    inline void appendChannelPressureEvent(const BYTE channel, const BYTE expression, const int DeltaTime=-1)
    {
        appendDeltaMessage(0xD0+channel,DeltaTime);
        DataList.appendByte(expression);
    }
    inline void appendPitchBendEvent(const BYTE channel, const BYTE lsb, const BYTE msb, const int DeltaTime=-1)
    {
        appendDeltaMessage(0xE0+channel,DeltaTime);
        DataList.appendBytes(lsb,msb);
    }
    inline void appendSysExEvent(const QByteArray& sysexMessage, const int DeltaTime=-1)
    {
        appendDeltaTime(DeltaTime);
        DataList.appendByte(0xF0);
        QByteArray s(sysexMessage);
        if (!s.endsWith(char(0xF7))) s.append(char(0xF7));
        appendVarLen(ulong(s.size()));
        DataList.append(s);
    }
    inline void appendMeta(const BYTE type, const QByteArray& message=QByteArray(), const int DeltaTime=-1)
    {
        appendDeltaMessage(0xFF,DeltaTime);
        DataList.appendByte(type);
        appendVarLen(ulong(message.size()));
        DataList.append(message);
    }
    inline void appendTempo(const int tempo, const int DeltaTime=-1)
    {
        appendDeltaMessage(0xFF,DeltaTime);
        DataList.appendChar(0x51);
        DataList.appendInt(0x3000000 | (int(60000000 / tempo) & 0x00FFFFFF));
    }
    inline void appendTimeSignature(const int upper, const int lower, const int DeltaTime=-1)
    {
        appendDeltaMessage(0xFF,DeltaTime);
        DataList.appendBytes(0x58,4);
        DataList.appendBytes(BYTE(upper),BYTE(qSqrt(lower)));
        DataList.appendBytes(BYTE((240 * 4) / lower),8);
    }
    inline void appendKey(const int key=-8, const int DeltaTime=-1)
    {
        appendDeltaMessage(0xFF,DeltaTime);
        DataList.appendChars(0x59,2);
        if (key < 0)
        {
            DataList.appendChars(char(!(qAbs<int>(key) - 1)),0);
        }
        else
        {
            DataList.appendChars(char(key),0);
        }
    }
    inline void appendMIDIEvent(const CMIDIEvent* e, const int DeltaTime=-1)
    {
        appendDeltaTime(DeltaTime);
        if (e->isSysEx())
        {
            appendMessage(e->message());
            appendVarLen(e->dataSize());
            DataList.append(e->dataPtr(),e->dataSize());
        }
        else
        {
            appendMessage(e->message());
            DataList.append(e->dataPtr(),e->dataSize());
        }
    }
private:
    BYTE m_RunningStatus;
    unsigned long m_Delta;
    QBinaryMemoryFile DataList;
    inline const std::vector<BYTE> varLen(unsigned long Value) const
    {
        std::vector<BYTE> Bytes;
        if (Value > 0x7F)
        {
            Bytes.push_back(Value & 0x7F);
            forever
            {
                Value /= 0x80;
                Bytes.insert(Bytes.begin(),(Value & 0x7F) | 0x80);
                if (Value < 0x80) break;
                if (Bytes.size() >= 10) break;
            }
        }
        else
        {
            Bytes.push_back(BYTE(Value));
        }
        return Bytes;
    }
    inline void appendVarLen(const unsigned long Value)
    {
        if (Value < 0x80)
        {
            DataList.appendByte(BYTE(Value));
        }
        else
        {
            DataList.append(varLen(Value));
        }
    }
    inline void appendDeltaTime(const int DeltaTime=-1)
    {
        if (DeltaTime != -1) m_Delta=ulong(DeltaTime);
        appendVarLen(m_Delta);
        m_Delta = 0;
    }
    inline void appendMessage(const BYTE message)
    {
        if ((message != m_RunningStatus) || (message < 0x80) || (message >= 0xF0)) DataList.appendByte(message);
        m_RunningStatus = message;
    }
    inline void appendDeltaMessage(const BYTE message, const int DeltaTime=-1)
    {
        appendDeltaTime(DeltaTime);
        appendMessage(message);
    }
};

class OCMIDIFile
{
private:
    QList<OCMIDIFileTrack*> Tracks;
    int m_TrackNum;
    QString m_Title;
    QString m_Composer;
public:
    int TicksPerQuarter;
    OCMIDIFile(const QString& title=QString(), const QString& composer=QString())
    {
        m_TrackNum=0;
        TicksPerQuarter = 240;
        qDeleteAll(Tracks);
        Tracks.clear();
        m_Title=title;
        m_Composer=composer;
    }
    ~OCMIDIFile()
    {
        qDeleteAll(Tracks);
        Tracks.clear();
    }
    void appendTrack(const QString& name=QString())
    {
        m_TrackNum=Tracks.size();
        Tracks.append(new OCMIDIFileTrack);
        if (m_TrackNum==0)
        {
            if (!m_Title.isEmpty()) appendMetaEvent(3,m_Title);
            if (!m_Composer.isEmpty()) appendMetaEvent(1,m_Composer);
        }
        if (!name.isEmpty()) appendMetaEvent(4,name);
    }
    QBinaryMemoryFile FileData()
    {
        QBinaryMemoryFile m;
        MIDIFileHeader h;
        setDescriptor(h.descriptor.id,"MThd");
        h.descriptor.size=uint(qToBigEndian<qint32>(6));
        h.fileType=qToBigEndian<qint16>(1);
        h.numTracks=qToBigEndian<qint16>(short(Tracks.size()));
        h.ticks=qToBigEndian<qint16>(short(TicksPerQuarter));
        m.append(&h,sizeof(MIDIFileHeader));
        for (OCMIDIFileTrack* t : std::as_const(Tracks)) t->WriteToFile(m);
        return m;
    }
    void Save(const QString& Path)
    {
        FileData().save(Path);
    }
    const QByteArray MIDIPointer()
    {
        return FileData().get();
    }
    inline void setTrackNumber(const int TrackNumber, const int DeltaTime)
    {
        m_TrackNum = TrackNumber;
        Tracks[m_TrackNum]->setDelta(uint(DeltaTime));
    }
    inline void appendControllerEvent(const int Channel, const int Number, const int Value, const int DeltaTime=-1)
    {
        Tracks[m_TrackNum]->appendControllerEvent(BYTE(Channel),BYTE(Number),BYTE(Value),DeltaTime);
    }
    inline void appendExpressionEvent(const int Channel, const int Value, const int DeltaTime=-1)
    {
        appendControllerEvent(Channel,0xB,Value,DeltaTime);
    }
    inline void appendPortamentoEvent(const int Channel, const int Value, const int DeltaTime=-1)
    {
        appendControllerEvent(Channel,0x54,Value,DeltaTime);
    }
    inline void appendPatchChangeEvent(const int Channel, const int Value, const int DeltaTime=-1)
    {
        Tracks[m_TrackNum]->appendPatchChangeEvent(BYTE(Channel),BYTE(Value),DeltaTime);
    }
    inline void appendTempoEvent(const int Value, const int DeltaTime=-1)
    {
        Tracks[m_TrackNum]->appendTempo(Value,DeltaTime);
    }
    inline void appendMetaEvent(const int Type, const QString& Value, const int DeltaTime=-1)
    {
        Tracks[m_TrackNum]->appendMeta(BYTE(Type),Value.toLatin1(),DeltaTime);
    }
    inline void appendSysExEvent(const QByteArray& Value, const int DeltaTime=-1)
    {
        Tracks[m_TrackNum]->appendSysExEvent(Value,DeltaTime);
    }
    inline void appendTimeEvent(const int TimeU, const int TimeD, const int DeltaTime=-1)
    {
        Tracks[m_TrackNum]->appendTimeSignature(TimeU,TimeD,DeltaTime);
    }
    inline void appendKeyEvent(const int Key, const int DeltaTime=-1)
    {
        Tracks[m_TrackNum]->appendKey(Key,DeltaTime);
    }
    inline void appendNoteOnEvent(const int Channel, const int Pitch, const int Velocity, const int DeltaTime=-1)
    {
        Tracks[m_TrackNum]->appendNoteOnEvent(BYTE(Channel),BYTE(Pitch),BYTE(Velocity),DeltaTime);
    }
    inline void appendNoteOffEvent(const int Channel, const int Pitch, const int Velocity=0, const int DeltaTime=-1)
    {
        Tracks[m_TrackNum]->appendNoteOffEvent(BYTE(Channel),BYTE(Pitch),BYTE(Velocity),DeltaTime);
    }
    inline void appendMIDIEvent(const CMIDIEvent* e, const int DeltaTime=-1)
    {
        Tracks[m_TrackNum]->appendMIDIEvent(e,DeltaTime);
    }
    inline void appendMIDIEventList(const CMIDIEventList& l, int DeltaTime=-1)
    {
        for (unsigned int i=0; i<l.size(); i++)
        {
            appendMIDIEvent(l[i],DeltaTime);
            DeltaTime=0;
        }
    }
    inline void appendMIDIBuffer(const CMIDIBuffer* b, const int DeltaTime=-1)
    {
        if (b) appendMIDIEventList(b->eventList(),DeltaTime);
    }
};

#pragma pack(pop)

#endif // MIDIFILECLASSES_H
