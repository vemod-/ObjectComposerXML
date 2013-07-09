#include "qbinarymemoryfile.h"
#include <QFile>
#include <QDataStream>
#include <QtEndian>

QBinaryMemoryFile::QBinaryMemoryFile(Endians e)
{
    m_Endian=e;
    m_size=0;
    m_pageSize=10000;
    m_bufferSize=m_pageSize;
    m_buffer=new unsigned char[m_bufferSize];
}

QBinaryMemoryFile::QBinaryMemoryFile(const void *data, const size_t length, Endians e)
{
    QBinaryMemoryFile();
    m_Endian=e;
    append(data, length);
}

QBinaryMemoryFile::QBinaryMemoryFile(const QByteArray &byteArray, Endians e)
{
    QBinaryMemoryFile(byteArray.data(),byteArray.count(),e);
}

QBinaryMemoryFile::QBinaryMemoryFile(const QString &path, Endians e)
{
    QBinaryMemoryFile();
    m_Endian=e;
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) append(file.map(0,file.size()),file.size());
}

QBinaryMemoryFile::~QBinaryMemoryFile()
{
    delete [] m_buffer;
}

const size_t QBinaryMemoryFile::size() const
{
    return m_size;
}

const unsigned char QBinaryMemoryFile::getByte(const size_t index) const
{
    return m_buffer[index];
}

void QBinaryMemoryFile::append(QBinaryMemoryFile& memFile)
{
    append(memFile.data(), memFile.size());
}

QByteArray QBinaryMemoryFile::get(const size_t index, const size_t length)
{
    return QByteArray::fromRawData((char*)(&m_buffer[index]),length);
}

unsigned char* QBinaryMemoryFile::data(const size_t index)
{
    return &m_buffer[index];
}

QByteArray QBinaryMemoryFile::get()
{
    return get(0,m_size);
}
/*
unsigned int QBinaryMemoryFile::getValue(const size_t index, const size_t length)
{
    unsigned char* TempChar=&m_buffer[index];
    switch (length)
    {
    case 1:
        return TempChar[0];
    case 2:
        return (TempChar[0]<<8) | TempChar[1];
    case 3:
        return (TempChar[0]<<16) | (TempChar[1]<<8) | TempChar[2];
    case 4:
        return (TempChar[0]<<24) | (TempChar[1]<<16) | (TempChar[2]<<8) | TempChar[3];
    }
    return 0;
}
*/
int QBinaryMemoryFile::getInt(const size_t index)
{
    if (m_Endian==BigEndian) return qFromBigEndian<qint32>(*(int*)index);
    return qFromLittleEndian<qint32>(*(int*)index);
}

short QBinaryMemoryFile::getShort(const size_t index)
{
    if (m_Endian==BigEndian) return qFromBigEndian<qint16>(*(short*)index);
    return qFromLittleEndian<qint16>(*(short*)index);
}

QString QBinaryMemoryFile::getString(const size_t index, const size_t length)
{
    return QString(get(index,length));
}

void QBinaryMemoryFile::append(const QByteArray& byteArray)
{
    append(byteArray.data(),byteArray.count());
}

void QBinaryMemoryFile::append(const void *data, const size_t length)
{
    put(m_size,data,length);
}

void QBinaryMemoryFile::put(const size_t index, QBinaryMemoryFile& memFile)
{
    put(index, memFile.data(), memFile.size());
}

void QBinaryMemoryFile::put(const size_t index, const QByteArray& byteArray)
{
    put(index, byteArray.data(), byteArray.count());
}

void QBinaryMemoryFile::put(const size_t index, const void *data, const size_t length)
{
    expand(length+index);
    memcpy(&m_buffer[index],data,length);
}

void QBinaryMemoryFile::putInt(const size_t index, int value)
{
    if (m_Endian==BigEndian) value=qToBigEndian<qint32>(value);
    if (m_Endian==LittleEndian) value=qToLittleEndian<qint32>(value);
    put(index,&value,sizeof(int));
}

void QBinaryMemoryFile::putShort(const size_t index, short value)
{
    if (m_Endian==BigEndian) value=qToBigEndian<qint16>(value);
    if (m_Endian==LittleEndian) value=qToLittleEndian<qint16>(value);
    put(index,&value,sizeof(short));
}

void QBinaryMemoryFile::putByte(const size_t index, char value)
{
    put(index,&value,sizeof(char));
}

void QBinaryMemoryFile::putString(const size_t index, const QString &value)
{
    put(index,QByteArray(value.toLatin1()));
}

void QBinaryMemoryFile::appendInt(int value)
{
    if (m_Endian==BigEndian) value=qToBigEndian<qint32>(value);
    if (m_Endian==LittleEndian) value=qToLittleEndian<qint32>(value);
    append(&value,sizeof(int));
}

void QBinaryMemoryFile::appendShort(short value)
{
    if (m_Endian==BigEndian) value=qToBigEndian<qint16>(value);
    if (m_Endian==LittleEndian) value=qToLittleEndian<qint16>(value);
    append(&value,sizeof(short));
}

void QBinaryMemoryFile::appendByte(char value)
{
    append(&value,sizeof(char));
}

void QBinaryMemoryFile::appendString(const QString& value)
{
    append(QByteArray(value.toLatin1()));
}

void QBinaryMemoryFile::expand(const size_t newSize)
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

void QBinaryMemoryFile::save(const QString& path)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);
        out.writeRawData((char*)m_buffer,m_size);
    }
}

void QBinaryMemoryFile::clear()
{
    if (m_size > 0)
    {
        m_size=0;
        if (m_bufferSize != m_pageSize)
        {
            m_bufferSize=m_pageSize;
            delete [] m_buffer;
            m_buffer=new unsigned char[m_bufferSize];
        }
    }
}
