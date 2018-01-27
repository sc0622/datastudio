#ifndef JCIRCULARBUFFER_H
#define JCIRCULARBUFFER_H

#include <algorithm>

class JCircularBufferData;

class JCircularBuffer
{
public:
    explicit JCircularBuffer(size_t capacity);
    ~JCircularBuffer();

    size_t size() const;
    size_t capacity() const;
    size_t write(const char *data, size_t size);
    size_t read(char *data, size_t size);
    void clear();

private:
    JCircularBufferData *d;
};

#endif // JCIRCULARBUFFER_H
