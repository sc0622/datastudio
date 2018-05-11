#ifndef SERIALCHANNEL_WRAP_H
#define SERIALCHANNEL_WRAP_H

#include "channel_wrap.h"

namespace Icd {
class SerialChannel;
typedef std::shared_ptr<SerialChannel> SerialChannelPtr;
}

PROTOCORE_BEGIN

class SerialChannelWrap : public Napi::ObjectWrap<SerialChannelWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    SerialChannelWrap(const Napi::CallbackInfo &info);

    Icd::SerialChannelPtr data() const { return d; }

    CHANNELWRAP_METHODS_DECL();
    NAPI_METHOD_DECL(SetConfig);
    NAPI_PROPERTY_DECL(PortName);
    NAPI_PROPERTY_DECL(BaudRate);
    NAPI_PROPERTY_DECL(DataBits);
    NAPI_PROPERTY_DECL(StopBits);
    NAPI_PROPERTY_DECL(Parity);

private:
    Icd::SerialChannelPtr d;
};

PROTOCORE_END

#endif // SERIALCHANNEL_WRAP_H
