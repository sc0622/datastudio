#include "precomp.h"
#include "udpchannel_wrap.h"
#include "udpchannel_wrap.h"
#include "icdcomm/icdcomm_udpchannel.h"

PROTOCORE_BEGIN

Napi::FunctionReference UdpChannelWrap::ctor;

void UdpChannelWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        CHANNELWRAP_METHODS_PROPS(UdpChannelWrap),
        InstanceMethod("setConfig", &UdpChannelWrap::SetConfig, napi_enumerable),
        InstanceMethod("isMulticastIP", &UdpChannelWrap::IsMulticastIP, napi_enumerable),
        InstanceAccessor("localAddr", &UdpChannelWrap::GetLocalAddr, &UdpChannelWrap::SetLocalAddr, napi_enumerable),
        InstanceAccessor("localPort", &UdpChannelWrap::GetLocalPort, &UdpChannelWrap::SetLocalPort, napi_enumerable),
        InstanceAccessor("remoteAddr", &UdpChannelWrap::GetRemoteAddr, &UdpChannelWrap::SetRemoteAddr, napi_enumerable),
        InstanceAccessor("remotePort", &UdpChannelWrap::GetRemotePort, &UdpChannelWrap::SetRemotePort, napi_enumerable)
    };
    ctor = napi_init<UdpChannelWrap>(env, exports, "UdpChannel", properties);
}

UdpChannelWrap::UdpChannelWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<UdpChannelWrap>(info)
    , d(napi_unwrap_data<Icd::UdpChannel>(info))
{

}

NAPI_METHOD(UdpChannelWrap, SetConfig) {
    if (info.Length() != 4) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need four arguments!"));
    }

    return Napi::Boolean::New(info.Env(), d->setConfig(info[0].As<Napi::String>(), info[1].As<Napi::Number>().Int32Value(),
            info[2].As<Napi::String>(), info[3].As<Napi::Number>().Int32Value()));
}

NAPI_METHOD(UdpChannelWrap, IsMulticastIP) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
    }
    return Napi::Boolean::New(info.Env(), d->isMulticastIP(info[0].As<Napi::String>()));
}

NAPI_GETTER(UdpChannelWrap, LocalAddr) {
    return Napi::String::New(info.Env(), d->localIP());
}

NAPI_SETTER(UdpChannelWrap, LocalAddr) {
    d->setLocalIP(info[0].As<Napi::String>());
}

NAPI_GETTER(UdpChannelWrap, LocalPort) {
    return Napi::Number::New(info.Env(), d->localPort());
}

NAPI_SETTER(UdpChannelWrap, LocalPort) {
    d->setLocalPort(info[0].As<Napi::Number>().Int32Value());
}

NAPI_GETTER(UdpChannelWrap, RemoteAddr) {
    return Napi::String::New(info.Env(), d->remoteIP());
}

NAPI_SETTER(UdpChannelWrap, RemoteAddr) {
    d->setRemoteIP(info[0].As<Napi::String>());
}

NAPI_GETTER(UdpChannelWrap, RemotePort) {
    return Napi::Number::New(info.Env(), d->remotePort());
}

NAPI_SETTER(UdpChannelWrap, RemotePort) {
    d->setRemotePort(info[0].As<Napi::Number>().Int32Value());
}

CHANNELWRAP_METHODS_IMPL(UdpChannelWrap)

PROTOCORE_END
