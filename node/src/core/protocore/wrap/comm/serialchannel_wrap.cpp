#include "precomp.h"
#include "serialchannel_wrap.h"
#include "udpchannel_wrap.h"
#include "icdcomm/icdcomm_serialchannel.h"

PROTOCORE_BEGIN

Napi::FunctionReference SerialChannelWrap::ctor;

void SerialChannelWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        CHANNELWRAP_METHODS_PROPS(SerialChannelWrap),
        InstanceMethod("setConfig", &SerialChannelWrap::SetConfig, napi_enumerable),
        InstanceAccessor("portName", &SerialChannelWrap::GetPortName, &SerialChannelWrap::SetPortName, napi_enumerable),
        InstanceAccessor("baudRate", &SerialChannelWrap::GetBaudRate, &SerialChannelWrap::SetBaudRate, napi_enumerable),
        InstanceAccessor("dataBits", &SerialChannelWrap::GetDataBits, &SerialChannelWrap::SetDataBits, napi_enumerable),
        InstanceAccessor("stopBits", &SerialChannelWrap::GetStopBits, &SerialChannelWrap::SetStopBits, napi_enumerable),
        InstanceAccessor("parity", &SerialChannelWrap::GetParity, &SerialChannelWrap::SetParity, napi_enumerable)
    };
    ctor = napi_init<SerialChannelWrap>(env, exports, "SerialChannel", properties);
}

SerialChannelWrap::SerialChannelWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<SerialChannelWrap>(info)
    , d(napi_unwrap_data<Icd::SerialChannel>(info))
{

}

CHANNELWRAP_METHODS_IMPL(SerialChannelWrap)

NAPI_METHOD(SerialChannelWrap, SetConfig) {
    if (info.Length() != 5) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need five arguments!"));
    }
    return Napi::Boolean::New(info.Env(), d->setConfig(info[0].As<Napi::String>(),
                              info[1].As<Napi::Number>().Int32Value(),
            Icd::SerialChannel::DataBits(info[2].As<Napi::Number>().Int32Value()),
            Icd::SerialChannel::StopBits(info[3].As<Napi::Number>().Int32Value()),
            Icd::SerialChannel::Parity(info[4].As<Napi::Number>().Int32Value())
            ));
}

NAPI_GETTER(SerialChannelWrap, PortName) {
    return Napi::String::New(info.Env(), d->portName());
}

NAPI_SETTER(SerialChannelWrap, PortName) {
    d->setPortName(info[0].As<Napi::String>());
}

NAPI_GETTER(SerialChannelWrap, BaudRate) {
    return Napi::Number::New(info.Env(), int(d->baudRate()));
}

NAPI_SETTER(SerialChannelWrap, BaudRate) {
    d->setBaudRate(info[0].As<Napi::Number>().Int32Value());
}

NAPI_GETTER(SerialChannelWrap, DataBits) {
    return Napi::Number::New(info.Env(), int(d->dataBits()));
}

NAPI_SETTER(SerialChannelWrap, DataBits) {
    d->setDataBits(Icd::SerialChannel::DataBits(info[0].As<Napi::Number>().Int32Value()));
}

NAPI_GETTER(SerialChannelWrap, StopBits) {
    return Napi::Number::New(info.Env(), int(d->stopBits()));
}

NAPI_SETTER(SerialChannelWrap, StopBits) {
    d->setStopBits(Icd::SerialChannel::StopBits(info[0].As<Napi::Number>().Int32Value()));
}

NAPI_GETTER(SerialChannelWrap, Parity) {
    return Napi::Number::New(info.Env(), int(d->parity()));
}

NAPI_SETTER(SerialChannelWrap, Parity) {
    d->setParity(Icd::SerialChannel::Parity(info[0].As<Napi::Number>().Int32Value()));
}

PROTOCORE_END
