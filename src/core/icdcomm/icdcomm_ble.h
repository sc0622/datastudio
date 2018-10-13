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
    virtual ~BleChannel() override;

    bool isOpen() const override;
    bool open() override;
    void close() override;

    bool waitForReadyRead(int msecs) override;
    int read(char* buffer, int size) override;
    int write(const char* buffer, int size) override;
    int sizeOfIn() const override;
    int sizeOfOut() const override;
    void flush() override;

    std::string config() const override;
    bool setConfig(const std::string &config) override;

    std::string desc() const override;

    int lastErrorCode() const override;
    std::string lastError() const override;

private:
    std::shared_ptr<BleChannelData> d;
};

} // end of namespace Icd

#endif // ICDCOMM_BLE_H
