#include "precomp.h"
#include "icdcomm_arinc429.h"
#include <sstream>
#include <assert.h>

namespace Icd {

// class Arinc429ChannelData

class Arinc429ChannelData
{
    friend class Arinc429Channel;
public:
    Arinc429ChannelData()
    {

    }

private:
};

// class Arinc429Channel

Arinc429Channel::Arinc429Channel()
    : Channel(ChannelArinc429)
    , d(new Arinc429ChannelData())
{

}

Arinc429Channel::~Arinc429Channel()
{

}

int Arinc429Channel::lastErrorCode() const
{
    return -1;
}

std::string Arinc429Channel::lastError() const
{
    return "";
}

bool Arinc429Channel::open()
{
    return false;
}

void Arinc429Channel::close()
{

}

bool Arinc429Channel::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

int Arinc429Channel::read(char *buffer, int size)
{
    Q_UNUSED(buffer);
    Q_UNUSED(size);
    return -1;
}

int Arinc429Channel::write(const char *buffer, int size)
{
    Q_UNUSED(buffer);
    Q_UNUSED(size);
    return -1;
}

int Arinc429Channel::sizeOfIn() const
{
    return 0;
}

int Arinc429Channel::sizeOfOut() const
{
    return 0;
}

void Arinc429Channel::flush()
{

}

bool Arinc429Channel::isOpen() const
{
    return false;
}

// "serial: -n [portname] -b [baudrate] -d [datebits] -s [stopbits] -p [parity]"
std::string Arinc429Channel::config() const
{
    std::ostringstream os;

    os << "--type=serial" << Channel::config();
    //

    return os.str();
}

// "serial: --portName[portname] -b [baudrate] -d [datebits] -s [stopbits] -p [parity]"
bool Arinc429Channel::setConfig(const std::string &config)
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

std::string Arinc429Channel::desc() const
{
    std::ostringstream os;
    //

    return os.str();
}

} // end of namespace Icd
