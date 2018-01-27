#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include "icdcomm_global.h"

namespace Icd {

class RingBufferData;

/**
 * @brief 环形缓冲区类
 */
class ICDCOMM_EXPORT RingBuffer
{
public:
    /**
     * @brief RingBuffer
     * @param size
     */
    explicit RingBuffer(int size);

    ~RingBuffer();

    /**
     * @brief write
     * @param buffer
     * @param size
     * @return
     */
    int write(const char* buffer, int size);

    /**
     * @brief read
     * @param buffer
     * @param size
     * @return
     */
    int read(char* buffer, int size);

    /**
     * @brief reset
     */
    void reset();

    /**
     * @brief size
     * @return
     */
    int size() const;

private:
    RingBufferData *d;
};

} // end of namespace Icd

#endif // RINGBUFFER_H
