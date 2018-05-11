#ifndef FILECHANNEL_WRAP_H
#define FILECHANNEL_WRAP_H

#include "channel_wrap.h"

namespace Icd {
class FileChannel;
typedef std::shared_ptr<FileChannel> FileChannelPtr;
}

PROTOCORE_BEGIN

class FileChannelWrap : public Napi::ObjectWrap<FileChannelWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    FileChannelWrap(const Napi::CallbackInfo &info);

    Icd::FileChannelPtr data() const { return d; }

    CHANNELWRAP_METHODS_DECL();
    NAPI_PROPERTY_DECL(FilePath);
    NAPI_PROPERTY_DECL(OpenMode);
    NAPI_PROPERTY_DECL(SaveFormat);
    NAPI_PROPERTY_DECL(FileHeader);
    NAPI_PROPERTY_DECL(Domain);

private:
    Icd::FileChannelPtr d;
};

PROTOCORE_END

#endif // FILECHANNEL_WRAP_H
