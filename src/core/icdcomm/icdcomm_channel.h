#ifndef ICDCOMM_CHANNEL_H
#define ICDCOMM_CHANNEL_H

#include "icdcomm_global.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace Icd {

enum ChannelType {
    ChannelInvalid = -1,    // 通信类型不可用
    ChannelSerial = 0,      // 串口通信
    ChannelUdp,             // UDP通信
    ChannelFile,            // 文件
    ChannelArinc429,        // Arinc429
    ChannelBle,             // Ble-bluetooth
    ChannelTotal            // 标记边界
};

class Channel;
class ChannelData;
typedef std::shared_ptr<Channel> ChannelPtr;
typedef std::vector<ChannelPtr> ChannelPtrArray;

class ICDCOMM_EXPORT Channel
{
public:
    explicit Channel(ChannelType channelType = ChannelInvalid);
    virtual ~Channel();

    static ChannelPtr createInstance(ChannelType channelType);
    static ChannelPtr createInstance(const std::string &config);

    virtual bool isOpen() const = 0;
    virtual bool open() = 0;
    virtual void close() = 0;

    virtual bool waitForReadyRead(int msecs) { (void)msecs; return false; }
    virtual int read(char* buffer, int size) = 0;
    virtual int write(const char* buffer, int size) = 0;

    virtual int sizeOfIn() const;
    virtual int sizeOfOut() const;
    virtual void flush();

    ChannelType channelType() const;

    std::string identity() const;
    void setIdentity(const std::string &identity);

    virtual std::string config() const;
    virtual bool setConfig(const std::string &config);

    virtual std::string name() const;
    void setName(const std::string &name);

    virtual std::string desc() const;
    virtual void setDesc(const std::string &desc);

    virtual int lastErrorCode() const { return 0; }
    virtual std::string lastError() const { return ""; }

    Icd::ChannelPtr relayer() const;
    void setRelayer(const Icd::ChannelPtr &relayer);

    static std::unordered_map<std::string, std::string> parseConfig(const std::string &config);
    static std::string parserValue(const std::string &config, const std::string &key);

private:
    std::shared_ptr<ChannelData> d;
};

} // end of namespace Icd

#endif // ICDCOMM_CHANNEL_H
