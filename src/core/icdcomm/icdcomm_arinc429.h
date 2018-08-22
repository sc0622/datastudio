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
    virtual ~Arinc429Channel();

    bool isOpen() const;
    bool open();
    void close();

    bool waitForReadyRead(int msecs);
    int read(char* buffer, int size);
    int write(const char* buffer, int size);
    int sizeOfIn() const;
    int sizeOfOut() const;
    void flush();

    /**
     * @brief config
     * @return "serial: -n [portname] -b [baudrate] -d [datebits] -s [stopbits] -p [parity]"
     */
    std::string config() const;

    /**
     * @brief setConfig
     * @param config : "serial: -n [portname] -b [baudrate] -d [datebits] -s [stopbits] -p [parity]"
     * @return
     */
    bool setConfig(const std::string &config);

    std::string desc() const;

    int lastErrorCode() const;
    std::string lastError() const;

private:
    std::shared_ptr<Arinc429ChannelData> d;
};

} // end of namespace Icd

#endif // ICDCOMM_ARINC429_H
