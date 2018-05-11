#ifndef CHANNEL_WRAP_H
#define CHANNEL_WRAP_H

#include "node_global.h"

namespace Icd {
class Channel;
typedef std::shared_ptr<Channel> ChannelPtr;
}

PROTOCORE_BEGIN

#ifndef CHANNELWRAP_METHODS_DECL
#define CHANNELWRAP_METHODS_DECL() \
    NAPI_METHOD_DECL(Open); \
    NAPI_VOID_METHOD_DECL(Close); \
    NAPI_METHOD_DECL(WaitForReadyRead); \
    NAPI_METHOD_DECL(Read); \
    NAPI_METHOD_DECL(Write); \
    NAPI_VOID_METHOD_DECL(Flush)
#endif

class ChannelWrap : public Napi::ObjectWrap<ChannelWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    ChannelWrap(const Napi::CallbackInfo &info);

    Icd::ChannelPtr data() const { return d; }

    CHANNELWRAP_METHODS_DECL();
    static NAPI_METHOD_DECL(Create); \
    NAPI_GETTER_DECL(IsOpen);
    NAPI_GETTER_DECL(SizeOfIn);
    NAPI_GETTER_DECL(SizeOfOut);
    NAPI_GETTER_DECL(ChannelType);
    NAPI_PROPERTY_DECL(Identity);
    NAPI_PROPERTY_DECL(Config);
    NAPI_PROPERTY_DECL(Name);
    NAPI_PROPERTY_DECL(Desc);
    NAPI_GETTER_DECL(LastErrorCode);
    NAPI_GETTER_DECL(LastError);
    NAPI_PROPERTY_DECL(Relayer);
    static NAPI_METHOD_DECL(ParseConfig);

    static Icd::ChannelPtr fromObject(Napi::Env env, const Napi::Value &value);
    static Napi::Value toObject(Napi::Env env, const Icd::ChannelPtr &data);

private:
    Icd::ChannelPtr d;
};

#ifndef CHANNELWRAP_METHODS_PROPS
#define CHANNELWRAP_METHODS_PROPS(Class) \
    InstanceMethod("open", &Class::Open, napi_enumerable), \
    InstanceMethod("close", &Class::Close, napi_enumerable), \
    InstanceMethod("waitForReadyRead", &Class::WaitForReadyRead, napi_enumerable), \
    InstanceMethod("read", &Class::Read, napi_enumerable), \
    InstanceMethod("write", &Class::Write, napi_enumerable), \
    InstanceMethod("flush", &Class::Flush, napi_enumerable)
#endif

#ifndef CHANNELWRAP_METHODS_IMPL
#define CHANNELWRAP_METHODS_IMPL(Class) \
    \
    NAPI_METHOD(Class, Open) { \
    return Napi::Boolean::New(info.Env(), d->open()); \
    } \
    \
    NAPI_VOID_METHOD(Class, Close) { \
    d->close(); \
    } \
    \
    NAPI_METHOD(Class, WaitForReadyRead) { \
    if (info.Length() != 1) { \
    return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument.")); \
    } \
    const int msecs = info[0].As<Napi::Number>().Int32Value(); \
    return Napi::Boolean::New(info.Env(), d->waitForReadyRead(msecs)); \
    } \
    \
    NAPI_METHOD(Class, Read) { \
    if (info.Length() != 1) { \
    return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument.")); \
    } \
    const int size = info[0].As<Napi::Number>().Int32Value(); \
    char *buffer = new char[size]; \
    return Napi::Buffer<char>::New(info.Env(), buffer, size, [=](Napi::Env, char *data){ \
    delete[] data; \
    }); \
    } \
    \
    NAPI_METHOD(Class, Write) { \
    if (info.Length() != 1) { \
    return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument.")); \
    } \
    const Napi::Buffer<char> buffer = info[0].As<Napi::Buffer<char> >(); \
    if (buffer.Data() == nullptr) { \
    return napi_throwjs(Napi::TypeError::New(info.Env(), "argument is not Int8Array")); \
    } \
    return Napi::Number::New(info.Env(), d->write(buffer.Data(), int(buffer.Length()))); \
    } \
    \
    NAPI_VOID_METHOD(Class, Flush) { \
    d->flush(); \
    }
#endif

PROTOCORE_END

#endif // CHANNEL_WRAP_H
