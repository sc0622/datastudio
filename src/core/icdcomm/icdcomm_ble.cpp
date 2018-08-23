#include "precomp.h"
#include "icdcomm_ble.h"
#include <sstream>
#include <assert.h>

namespace Icd {

// class BleChannelData

class BleChannelData
{
    friend class BleChannel;
public:
    BleChannelData()
    {

    }

private:
};

// class BleChannel

BleChannel::BleChannel()
    : Channel(ChannelBle)
    , d(new BleChannelData())
{

}

BleChannel::~BleChannel()
{

}

int BleChannel::lastErrorCode() const
{
    return -1;
}

std::string BleChannel::lastError() const
{
    return "";
}

bool BleChannel::open()
{
    return false;
}

void BleChannel::close()
{

}

bool BleChannel::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

int BleChannel::read(char *buffer, int size)
{
    Q_UNUSED(buffer);
    Q_UNUSED(size);
    return -1;
}

int BleChannel::write(const char *buffer, int size)
{
    Q_UNUSED(buffer);
    Q_UNUSED(size);
    return -1;
}

int BleChannel::sizeOfIn() const
{
    return 0;
}

int BleChannel::sizeOfOut() const
{
    return 0;
}

void BleChannel::flush()
{

}

bool BleChannel::isOpen() const
{
    return false;
}

std::string BleChannel::config() const
{
    std::ostringstream os;

    os << "--type=serial" << Channel::config();
    //

    return os.str();
}

bool BleChannel::setConfig(const std::string &config)
{
    // parse
    const std::unordered_map<std::string, std::string> items = parseConfig(config);
    if (items.empty()) {
        return false;
    }

    // id
    std::unordered_map<std::string, std::string>::const_iterator citer = items.find("id");
    if (citer == items.cend()) {
        return false;
    }
    setIdentity(citer->second);

    // name
    citer = items.find("name");
    if (citer == items.cend()) {
        return false;
    }
    setName(citer->second);

    return true;
}

std::string BleChannel::desc() const
{
    std::ostringstream os;
    //

    return os.str();
}

} // end of namespace Icd
