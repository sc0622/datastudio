//#include "precomp.h"
#include "icdcomm_ringbuffer.h"
#include <QReadWriteLock>

namespace Icd {

class RingBufferData
{
    friend class RingBuffer;
public:
    RingBufferData(int size)
        : buffer(new char[size])
        , readIndex(0)
        , writeIndex(0)
        , size(size)
        , validLen(0)
    {

    }

    ~RingBufferData()
    {
        delete[] buffer;
    }

    void write(char data);
    char read();

private:
    char *buffer;
    int readIndex;
    int writeIndex;
    int size;
    int validLen;
    QReadWriteLock lock;
};

void RingBufferData::write(char data)
{
    buffer[writeIndex] = data;
    writeIndex++;
    validLen++;
    if (writeIndex == size) {
        writeIndex = 0;
    }
}

char RingBufferData::read()
{
    char data = buffer[readIndex];
    readIndex++;
    if ( readIndex == size) {
        readIndex = 0;
    }

    validLen--;

    return data;
}

RingBuffer::RingBuffer(int size)
    : d(new RingBufferData(size))
{

}

RingBuffer::~RingBuffer()
{

}

int RingBuffer::write(const char* data, int size)
{
    d->lock.lockForWrite();

    for (int i = 0; i < size; ++i) {
        if (d->validLen < d->size)  {
            d->write(data[i]);
        } else {
            d->lock.unlock();
            return i;   // 如果缓冲区已满，则不再写入数据
        }
    }

    d->lock.unlock();

    return size;
}

int RingBuffer::read(char *data, int size)
{
    int dataNum = size;

    d->lock.lockForRead();

    if (d->validLen < size) {
        dataNum = d->validLen;
    }

    for (int i = 0; i < dataNum; ++i) {
        if (d->validLen > 0) {
            data[i] = d->read();
        } else {
            d->lock.unlock();
            return i;   // 无可用数据时返回已读取数据长度
        }
    }

    d->lock.unlock();

    return dataNum;
}

void RingBuffer::reset()
{
    d->readIndex = 0;
    d->writeIndex = 0;
    d->validLen = 0;
}

int RingBuffer::size() const
{
    QReadLocker locker(&d->lock);
    return d->size;
}

} // end of namespace Icd
