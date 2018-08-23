#ifndef ICDCOMM_BLE_H
#define ICDCOMM_BLE_H

#include "icdcomm_channel.h"

namespace Icd {

class BleChannel;
class BleChannelData;
typedef std::shared_ptr<BleChannel> BleChannelPtr;

class ICDCOMM_EXPORT BleChannel : public Channel
{
public:
    explicit BleChannel();
    virtual ~BleChannel();

    bool isOpen() const;
    bool open();
    void close();

    bool waitForReadyRead(int msecs);
    int read(char* buffer, int size);
    int write(const char* buffer, int size);
    int sizeOfIn() const;
    int sizeOfOut() const;
    void flush();

    std::string config() const;
    bool setConfig(const std::string &config);

    std::string desc() const;

    int lastErrorCode() const;
    std::string lastError() const;

private:
    std::shared_ptr<BleChannelData> d;
};

} // end of namespace Icd

#endif // ICDCOMM_BLE_H
