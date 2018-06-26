#include "precomp.h"
#include "icdcomm_channel.h"
#include "icdcomm_serialchannel.h"
#include "icdcomm_filechannel.h"
#include "icdcomm_udpchannel.h"
#include <sstream>
#include <unordered_map>

namespace Icd {

class ChannelData
{
    friend class Channel;
public:
    ChannelData()
        : channelType(Icd::ChannelInvalid)
        , identity(QUuid::createUuid().toString().toStdString())
        , name("")
        , desc("****")
        , relayer(0)
    {

    }

private:
    Icd::ChannelType channelType;
    std::string identity;
    std::string name;
    std::string desc;           // 描述信息
    Icd::ChannelPtr relayer;    // 中继者
};

Channel::Channel(ChannelType channelType)
    : d(new ChannelData())
{
    d->channelType = channelType;
}

Channel::~Channel()
{

}

ChannelPtr Channel::createInstance(Icd::ChannelType channelType)
{
    switch (channelType) {
    case ChannelSerial:
        return ChannelPtr(new SerialChannel());
    case ChannelUdp:
        return ChannelPtr(new UdpChannel());
    case ChannelFile:
        return ChannelPtr(new FileChannel());
    default:
        return ChannelPtr();
    }
}

ChannelPtr Channel::createInstance(const std::string &config)
{
    if (config.empty()) {
        return ChannelPtr();
    }
    // 获取通道类型
    std::string type = parserValue(config, "type");
    if (type.empty()) {
        return ChannelPtr();
    }
    //
    Icd::ChannelType channelType = Icd::ChannelInvalid;
    if (type == "serial") {
        channelType = Icd::ChannelSerial;
    } else if (type == "file") {
        channelType = Icd::ChannelFile;
    } else if (type == "udp") {
        channelType = Icd::ChannelUdp;
    } else {
        return ChannelPtr();   // not supported
    }
    //
    if (channelType == Icd::ChannelInvalid) {
        return ChannelPtr();
    }
    // create an instance
    ChannelPtr channel = Channel::createInstance(channelType);
    if (channel == 0) {
        return ChannelPtr();
    }
    // set config
    if (!channel->setConfig(config)) {
        return ChannelPtr();
    }

    return channel;
}

int Channel::sizeOfIn() const
{
    return 0;
}

int Channel::sizeOfOut() const
{
    return 0;
}

void Channel::flush()
{

}

Icd::ChannelType Channel::channelType() const
{
    return d->channelType;
}

std::string Channel::identity() const
{
    return d->identity;
}

void Channel::setIdentity(const std::string &identity)
{
    d->identity = identity;
}

std::string Channel::config() const
{
    std::ostringstream os;
    os << " --id=" << d->identity
       << " --name=" << d->name;
    if (d->relayer) {
        os << " --relayer=" << d->relayer->identity();
    }
    return os.str();
}

bool Channel::setConfig(const std::string &config)
{
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

std::string Channel::name() const
{
    return d->name;
}

void Channel::setName(const std::string &name)
{
    d->name = name;
}

std::string Channel::desc() const
{
    return d->desc;
}

void Channel::setDesc(const std::string &desc)
{
    d->desc = desc;
}

ChannelPtr Channel::relayer() const
{
    return d->relayer;
}

void Channel::setRelayer(const ChannelPtr &relayer)
{
    d->relayer = relayer;
}

std::unordered_map<std::string, std::string> Channel::parseConfig(const std::string &config)
{
    std::unordered_map<std::string, std::string> items;

    std::string::size_type start = 0;
    std::string::size_type end = 0;
    std::string name, value;

    while (end != std::string::npos) {
        // "--"
        start = config.find("--", end);
        if (start == std::string::npos) {
            break;
        }
        start += 2;
        // "="
        end = config.find('=', start);
        if (end == std::string::npos) {
            break;
        }
        // name
        name = config.substr(start, end - start);
        // trim
        name.erase(0, name.find_first_not_of(' '));   // left trim
        name.erase(name.find_last_not_of(' ') + 1);   // right trim
        end += 1;   //
        if (name.empty()) {
            continue;
        }
        // value
        start = config.find(" --", end);
        if (start == std::string::npos) {
            value = config.substr(end);
        } else {
            value = config.substr(end, start - end);
        }
        // trim
        value.erase(0, value.find_first_not_of(' '));   // left trim
        value.erase(value.find_last_not_of(' ') + 1);   // right trim
        // save
        if (!value.empty()) {
            items[name] = value;
        }
        //
        end = start;
    }

    return items;
}

std::string Channel::parserValue(const std::string &config, const std::string &key)
{
    if (config.empty() || key.empty()) {
        return std::string();
    }

    // key
    std::string::size_type start = config.find("--" + key + '=', 0);
    if (start == std::string::npos) {
        return std::string();
    }
    start += 7;
    // value
    std::string value;
    std::string::size_type end = config.find(" --", start);
    if (end == std::string::npos) {
        value = config.substr(start);
    } else {
        value = config.substr(start, end - start);
    }
    // trim
    value.erase(0, value.find_first_not_of(' '));   // left trim
    value.erase(value.find_last_not_of(' ') + 1);   // right trim

    return value;
}

} // end of namespace Icd
