#include "qbinarymemoryfile.h"
#include <QFile>
#include <QDataStream>

QBinaryMemoryFile::QBinaryMemoryFile(QBinaryMemoryFile::Endians e)
{
    m_Endian=e;
    m_size=0;
    m_pageSize=10000;
    m_bufferSize=m_pageSize;
    m_buffer=new unsigned char[m_bufferSize];
}

QBinaryMemoryFile::QBinaryMemoryFile(const void *data, const size_t length, Endians e) :
    QBinaryMemoryFile()
{
    m_Endian=e;
    append(data, length);
}

QBinaryMemoryFile::QBinaryMemoryFile(const QByteArray &byteArray, Endians e) :
    QBinaryMemoryFile(byteArray.data(),uint(byteArray.size()),e)
{
}

QBinaryMemoryFile::QBinaryMemoryFile(const QString &path, Endians e) :
    QBinaryMemoryFile()
{
    m_Endian=e;
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) append(file.map(0,file.size()),uint(file.size()));
}

QBinaryMemoryFile::~QBinaryMemoryFile()
{
    delete [] m_buffer;
}

void QBinaryMemoryFile::save(const QString& path)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);
        out.writeRawData(reinterpret_cast<char*>(m_buffer),int(m_size));
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
