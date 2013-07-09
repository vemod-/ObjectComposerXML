#ifndef QBINARYMEMORYFILE_H
#define QBINARYMEMORYFILE_H

#include <QString>
#include <QByteArray>

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
    void expand(const size_t newSize);
    Endians m_Endian;
public:
    QBinaryMemoryFile(Endians e=BigEndian);
    QBinaryMemoryFile(const QByteArray& byteArray, Endians e=BigEndian);
    QBinaryMemoryFile(const QString& path, Endians e=BigEndian);
    QBinaryMemoryFile(const void* data, const size_t length, Endians e=BigEndian);
    ~QBinaryMemoryFile();
    const size_t size() const;
    const unsigned char getByte(const size_t index) const;
    QByteArray get(const size_t index, const size_t length);
    QByteArray get();
    //unsigned int getValue(const size_t index, const size_t length);
    int getInt(const size_t index);
    short getShort(const size_t index);
    QString getString(const size_t index, const size_t length);
    unsigned char* data(const size_t index=0);
    void append(QBinaryMemoryFile& memFile);
    void append(const QByteArray& byteArray);
    void append(const void* data, const size_t length);
    void put(const size_t index, QBinaryMemoryFile& memFile);
    void put(const size_t index, const QByteArray& byteArray);
    void put(const size_t index, const void* data, const size_t length);
    void putInt(const size_t index, int value);
    void putShort(const size_t index, short value);
    void putByte(const size_t index, char value);
    void putString(const size_t index, const QString& value);
    void appendInt(int value);
    void appendShort(short value);
    void appendByte(char value);
    void appendString(const QString& value);
    void save(const QString& path);
    void clear();
};

#endif // QBINARYMEMORYFILE_H
