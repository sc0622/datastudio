#include "precomp.h"
#include "channel_wrap.h"
#include "filechannel_wrap.h"
#include "serialchannel_wrap.h"
#include "udpchannel_wrap.h"
#include "icdcomm/icdcomm_channel.h"
#include "icdcomm/icdcomm_filechannel.h"
#include "icdcomm/icdcomm_serialchannel.h"
#include "icdcomm/icdcomm_udpchannel.h"

PROTOCORE_BEGIN

Napi::FunctionReference ChannelWrap::ctor;

void ChannelWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        CHANNELWRAP_METHODS_PROPS(ChannelWrap),
        StaticMethod("create", &ChannelWrap::Create, napi_enumerable),
        InstanceAccessor("sizeofIn", &ChannelWrap::GetSizeOfIn, nullptr, napi_enumerable),
        InstanceAccessor("sizeofOut", &ChannelWrap::GetSizeOfOut, nullptr, napi_enumerable),
        InstanceAccessor("isOpen", &ChannelWrap::GetIsOpen, nullptr, napi_enumerable),
        InstanceAccessor("channelType", &ChannelWrap::GetChannelType, nullptr, napi_enumerable),
        InstanceAccessor("id", &ChannelWrap::GetIdentity, &ChannelWrap::SetIdentity, napi_enumerable),
        InstanceAccessor("config", &ChannelWrap::GetConfig, &ChannelWrap::SetConfig, napi_enumerable),
        InstanceAccessor("name", &ChannelWrap::GetName, &ChannelWrap::SetName, napi_enumerable),
        InstanceAccessor("desc", &ChannelWrap::GetDesc, &ChannelWrap::SetDesc, napi_enumerable),
        InstanceAccessor("lastErrorCode", &ChannelWrap::GetLastErrorCode, nullptr, napi_enumerable),
        InstanceAccessor("lastError", &ChannelWrap::GetLastError, nullptr, napi_enumerable),
        InstanceAccessor("relayer", &ChannelWrap::GetRelayer, &ChannelWrap::SetRelayer, napi_enumerable),
        StaticMethod("parseConfig", &ChannelWrap::ParseConfig, napi_enumerable)
    };
    ctor = napi_init<ChannelWrap>(env, exports, "Channel", properties);
}

ChannelWrap::ChannelWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<ChannelWrap>(info)
    , d(napi_unwrap_data<Icd::Channel>(info))
{

}

CHANNELWRAP_METHODS_IMPL(ChannelWrap)

NAPI_METHOD(ChannelWrap, Create) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
    }

    Icd::ChannelPtr newChannel;
    Napi::Object object = info[0].As<Napi::Object>();
    switch (object.Type()) {
    case napi_number:
        newChannel = Icd::Channel::createInstance(
                    Icd::ChannelType(object.ToNumber().Int32Value()));
        break;
    case napi_string:
        newChannel = Icd::Channel::createInstance(object.ToString());
        break;
    default:
        break;
    }

    if (newChannel) {
        return toObject(info.Env(), newChannel);
    }

    return Napi::Boolean::New(info.Env(), false);
}

NAPI_GETTER(ChannelWrap, SizeOfIn) {
    return Napi::Number::New(info.Env(), d->sizeOfIn());
}

NAPI_GETTER(ChannelWrap, SizeOfOut) {
    return Napi::Number::New(info.Env(), d->sizeOfOut());
}

NAPI_GETTER(ChannelWrap, IsOpen) {
    return Napi::Boolean::New(info.Env(), d->isOpen());
}

NAPI_GETTER(ChannelWrap, ChannelType) {
    return Napi::Number::New(info.Env(), int(d->channelType()));
}

NAPI_GETTER(ChannelWrap, Identity) {
    return Napi::String::New(info.Env(), d->identity());
}

NAPI_SETTER(ChannelWrap, Identity) {
    d->setIdentity(info[0].As<Napi::String>());
}

NAPI_GETTER(ChannelWrap, Config) {
    return Napi::String::New(info.Env(), d->config());
}

NAPI_SETTER(ChannelWrap, Config) {
    d->setConfig(info[0].As<Napi::String>());
}

NAPI_GETTER(ChannelWrap, Name) {
    return Napi::String::New(info.Env(), d->name());
}

NAPI_SETTER(ChannelWrap, Name) {
    d->setName(info[0].As<Napi::String>());
}

NAPI_GETTER(ChannelWrap, Desc) {
    return Napi::String::New(info.Env(), d->desc());
}

NAPI_SETTER(ChannelWrap, Desc) {
    d->setDesc(info[0].As<Napi::String>());
}

NAPI_GETTER(ChannelWrap, LastErrorCode) {
    return Napi::Number::New(info.Env(), d->lastErrorCode());
}

NAPI_GETTER(ChannelWrap, LastError) {
    return Napi::String::New(info.Env(), d->lastError());
}

NAPI_GETTER(ChannelWrap, Relayer) {
    return toObject(info.Env(), d->relayer());
}

NAPI_SETTER(ChannelWrap, Relayer) {
    d->setRelayer(fromObject(info.Env(), info[0]));
}

NAPI_METHOD(ChannelWrap, ParseConfig) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument."));
    }
    const std::map<std::string, std::string> configs
            = Icd::Channel::parseConfig(info[0].As<Napi::String>());
    Napi::Object object = Napi::Object::New(info.Env());
    for (auto citer = configs.cbegin(); citer != configs.cend(); ++citer) {
        object.Set(citer->first, citer->second);
    }
    return object;
}

Icd::ChannelPtr ChannelWrap::fromObject(Napi::Env env, const Napi::Value &value)
{
    if (!value.IsObject()) {
        napi_throwjs(Napi::TypeError::New(env, "argument is not a object."));
        return Icd::ChannelPtr();
    }
    const Napi::Object object = value.ToObject();
    if (object.InstanceOf(SerialChannelWrap::ctor.Value())) {
        return SerialChannelWrap::Unwrap(object)->data();
    } else if (object.InstanceOf(UdpChannelWrap::ctor.Value())) {
        return UdpChannelWrap::Unwrap(object)->data();
    } else if (object.InstanceOf(FileChannelWrap::ctor.Value())) {
        return FileChannelWrap::Unwrap(object)->data();
    }

    return Icd::ChannelPtr();
}

Napi::Value ChannelWrap::toObject(Napi::Env env, const Icd::ChannelPtr &data)
{
    if (data) {
        switch (data->channelType()) {
        case Icd::ChannelFile:
            return napi_instance(env, FileChannelWrap::ctor,
                                 JHandlePtrCast<Icd::FileChannel, Icd::Channel>(data));
        case Icd::ChannelSerial:
            return napi_instance(env, SerialChannelWrap::ctor,
                                 JHandlePtrCast<Icd::SerialChannel, Icd::Channel>(data));
        case Icd::ChannelUdp:
            return napi_instance(env, UdpChannelWrap::ctor,
                                 JHandlePtrCast<Icd::UdpChannel, Icd::Channel>(data));
        default:
            break;
        }
    }

    return Napi::Boolean::New(env, false);
}

PROTOCORE_END
