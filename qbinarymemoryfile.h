#ifndef QBINARYMEMORYFILE_H
#define QBINARYMEMORYFILE_H

#include <QString>
#include <QByteArray>
#include <QtEndian>
//#include <vector>

class QBinaryMemoryFile
{
    enum Endians
    {
        BigEndian,
        LittleEndian
    };
private:
    unsigned char* m_buffer;
    size_t m_size;
    size_t m_bufferSize;
    size_t m_pageSize;
    inline void expand(const size_t newSize)
    {
        if (newSize > m_bufferSize)
        {
            while (newSize > m_bufferSize) m_bufferSize+=m_pageSize;
            unsigned char* t=new unsigned char[m_bufferSize];
            if (m_size > 0) memcpy(t,m_buffer,m_size);
            delete [] m_buffer;
            m_buffer=t;
        }
        m_size=newSize;
    }
    Endians m_Endian;
public:
    QBinaryMemoryFile(Endians e=BigEndian);
    QBinaryMemoryFile(const QByteArray& byteArray, Endians e=BigEndian);
    QBinaryMemoryFile(const QString& path, Endians e=BigEndian);
    QBinaryMemoryFile(const void* data, const size_t length, Endians e=BigEndian);
    ~QBinaryMemoryFile();
    inline size_t size() const
    {
        return m_size;
    }
    inline unsigned char getByte(const size_t index) const {
        return m_buffer[index];
    }
    inline QByteArray get(const size_t index, const size_t length) {
        return QByteArray::fromRawData(reinterpret_cast<char*>(&m_buffer[index]),int(length));
    }
    inline QByteArray get() {
        return get(0,m_size);
    }
    inline int getInt(const size_t index)
    {
        if (m_Endian==BigEndian) return qFromBigEndian<qint32>(*reinterpret_cast<int*>(index));
        return qFromLittleEndian<qint32>(*reinterpret_cast<int*>(index));
    }
    inline short getShort(const size_t index)
    {
        if (m_Endian==BigEndian) return qFromBigEndian<qint16>(*reinterpret_cast<short*>(index));
        return qFromLittleEndian<qint16>(*reinterpret_cast<short*>(index));
    }
    inline QString getString(const size_t index, const size_t length)
    {
        return QString(get(index,length));
    }
    inline unsigned char* data(const size_t index=0)
    {
        return &m_buffer[index];
    }
    inline void append(QBinaryMemoryFile& memFile)
    {
        append(memFile.data(), memFile.size());
    }
    inline void append(const QByteArray& byteArray)
    {
        append(byteArray.data(),size_t(byteArray.size()));
    }
    inline void append(const std::vector<unsigned char>& vector)
    {
        append(vector.data(),vector.size());
    }
    inline void append(const void* data, const size_t length)
    {
        put(m_size,data,length);
    }
    inline void put(const size_t index, QBinaryMemoryFile& memFile)
    {
        put(index, memFile.data(), memFile.size());
    }
    inline void put(const size_t index, const QByteArray& byteArray)
    {
        put(index, byteArray.data(), size_t(byteArray.size()));
    }
    inline void put(const size_t index, const void* data, const size_t length)
    {
        expand(length+index);
        memcpy(&m_buffer[index],data,length);
    }
    inline void putInt(const size_t index, int value)
    {
        if (m_Endian==BigEndian) value=qToBigEndian<qint32>(value);
        if (m_Endian==LittleEndian) value=qToLittleEndian<qint32>(value);
        put(index,&value,sizeof(int));
    }
    inline void putShort(const size_t index, short value)
    {
        if (m_Endian==BigEndian) value=qToBigEndian<qint16>(value);
        if (m_Endian==LittleEndian) value=qToLittleEndian<qint16>(value);
        put(index,&value,sizeof(short));
    }
    inline void putChar(const size_t index, char value)
    {
        put(index,&value,sizeof(char));
    }
    inline void putByte(const size_t index, uchar value)
    {
        put(index,&value,sizeof(uchar));
    }
    inline void putString(const size_t index, const QString& value)
    {
        put(index,QByteArray(value.toLatin1()));
    }
    inline void appendInt(int value)
    {
        if (m_Endian==BigEndian) value=qToBigEndian<qint32>(value);
        if (m_Endian==LittleEndian) value=qToLittleEndian<qint32>(value);
        append(&value,sizeof(int));
    }
    inline void appendShort(short value)
    {
        if (m_Endian==BigEndian) value=qToBigEndian<qint16>(value);
        if (m_Endian==LittleEndian) value=qToLittleEndian<qint16>(value);
        append(&value,sizeof(short));
    }
    inline void appendChar(char value)
    {
        append(&value,sizeof(char));
    }
    inline void appendChars(char value1, char value2)
    {
        append(&value1,sizeof(char));
        append(&value2,sizeof(char));
    }
    inline void appendByte(uchar value)
    {
        append(&value,sizeof(uchar));
    }
    inline void appendBytes(uchar value1, uchar value2)
    {
        append(&value1,sizeof(uchar));
        append(&value2,sizeof(uchar));
    }
    inline void appendString(const QString& value)
    {
        append(QByteArray(value.toLatin1()));
    }
    void save(const QString& path);
    void clear();
};

#endif // QBINARYMEMORYFILE_H
