#ifndef ICDCOMM_ARINC429_H
#define ICDCOMM_ARINC429_H

#include "icdcomm_channel.h"

namespace Icd {

class Arinc429Channel;
class Arinc429ChannelData;
typedef std::shared_ptr<Arinc429Channel> Arinc429ChannelPtr;

class ICDCOMM_EXPORT Arinc429Channel : public Channel
{
public:
    explicit Arinc429Channel();
    virtual ~Arinc429Channel() override;

    bool isOpen() const override;
    bool open() override;
    void close() override;

    bool waitForReadyRead(int msecs) override;
    int read(char* buffer, int size) override;
    int write(const char* buffer, int size) override;
    int sizeOfIn() const override;
    int sizeOfOut() const override;
    void flush() override;

    /**
     * @brief config
     * @return "serial: -n [portname] -b [baudrate] -d [datebits] -s [stopbits] -p [parity]"
     */
    std::string config() const override;

    /**
     * @brief setConfig
     * @param config : "serial: -n [portname] -b [baudrate] -d [datebits] -s [stopbits] -p [parity]"
     * @return
     */
    bool setConfig(const std::string &config) override;

    std::string desc() const override;

    int lastErrorCode() const override;
    std::string lastError() const override;

private:
    std::shared_ptr<Arinc429ChannelData> d;
};

} // end of namespace Icd

#endif // ICDCOMM_ARINC429_H
