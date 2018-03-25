//#include "precomp.h"
#include "jcircularbuffer.h"

// class JCircularBufferData

class JCircularBufferData
{
    friend class JCircularBuffer;
public:
    JCircularBufferData(size_t capacity);
    ~JCircularBufferData();

    size_t write(const char *data, size_t size);
    size_t read(char *data, size_t size);
    void clear();

private:
    size_t _beg_index, _end_index, _size, _capacity;
    char *_data;
    QReadWriteLock lock;
};

JCircularBufferData::JCircularBufferData(size_t capacity)
    : _beg_index(0)
    , _end_index(0)
    , _size(0)
    , _capacity(capacity)
{
    _data = new char[capacity];
}

JCircularBufferData::~JCircularBufferData()
{
    delete[] _data;
}

size_t JCircularBufferData::write(const char *data, size_t size)
{
    QWriteLocker locker(&lock);

    if (size == 0) {
        return 0;
    }

    size_t capacity = _capacity;
    size_t bytes_to_write = std::min(size, capacity - _size);
    if (bytes_to_write == 0) {
        return 0;
    }

    // write in a single step
    if (bytes_to_write < capacity - _end_index) {
        memcpy(_data + _end_index, data, bytes_to_write);
        _end_index += bytes_to_write;
        if (_end_index == capacity) {
            _end_index = 0;
        }
    }
    // write in two steps
    else {
        size_t size_1 = capacity - _end_index;
        memcpy(_data + _end_index, data, size_1);
        size_t size_2 = bytes_to_write - size_1;
        memcpy(_data, data + size_1, size_2);
        _end_index = size_2;
    }

    _size += bytes_to_write;
    return bytes_to_write;
}

size_t JCircularBufferData::read(char *data, size_t size)
{
    QReadLocker locker(&lock);

    if (size == 0) {
        return 0;
    }

    size_t bytes_to_read = std::min(size, _size);
    if (bytes_to_read == 0) {
        return 0;
    }
    size_t capacity = _capacity;

    // read in a single step
    if (bytes_to_read <= capacity - _beg_index) {
        memcpy(data, _data + _beg_index, bytes_to_read);
        _beg_index += bytes_to_read;
        if (_beg_index == capacity) {
            _beg_index = 0;
        }
    }
    // read in two steps
    else {
        size_t size_1 = capacity - _beg_index;
        memcpy(data, _data + _beg_index, size_1);
        size_t size_2 = bytes_to_read - size_1;
        memcpy(data + size_1, _data, size_2);
        _beg_index = size_2;
    }

    _size -= bytes_to_read;
    return bytes_to_read;
}

void JCircularBufferData::clear()
{
    QWriteLocker locker(&lock);
    _beg_index = 0;
    _end_index = 0;
    _size = 0;
}

// class JCircularBuffer

JCircularBuffer::JCircularBuffer(size_t capacity)
    : d(new JCircularBufferData(capacity))
{

}

JCircularBuffer::~JCircularBuffer()
{
    delete d;
}

size_t JCircularBuffer::size() const
{
    QReadLocker locker(&d->lock);
    return d->_size;
}

size_t JCircularBuffer::capacity() const
{
    return d->_capacity;
}

size_t JCircularBuffer::write(const char *data, size_t size)
{
    return d->write(data, size);
}

size_t JCircularBuffer::read(char *data, size_t size)
{
    return d->read(data, size);
}

void JCircularBuffer::clear()
{
    d->clear();
}
