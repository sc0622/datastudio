#include "precomp.h"
#include "filechannel_wrap.h"
#include "udpchannel_wrap.h"
#include "icdcomm/icdcomm_filechannel.h"

PROTOCORE_BEGIN

Napi::FunctionReference FileChannelWrap::ctor;

void FileChannelWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        CHANNELWRAP_METHODS_PROPS(FileChannelWrap),
        InstanceAccessor("filePath", &FileChannelWrap::GetFilePath, &FileChannelWrap::SetFilePath, napi_enumerable),
        InstanceAccessor("openMode", &FileChannelWrap::GetOpenMode, &FileChannelWrap::SetOpenMode, napi_enumerable),
        InstanceAccessor("saveFormat", &FileChannelWrap::GetSaveFormat, &FileChannelWrap::SetSaveFormat, napi_enumerable),
        InstanceAccessor("fileHeader", &FileChannelWrap::GetFileHeader, &FileChannelWrap::SetFileHeader, napi_enumerable),
        InstanceAccessor("domain", &FileChannelWrap::GetDomain, &FileChannelWrap::SetDomain, napi_enumerable)
    };
    ctor = napi_init<FileChannelWrap>(env, exports, "FileChannel", properties);
}

FileChannelWrap::FileChannelWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<FileChannelWrap>(info)
    , d(napi_unwrap_data<Icd::FileChannel>(info))
{

}

CHANNELWRAP_METHODS_IMPL(FileChannelWrap)

NAPI_GETTER(FileChannelWrap, FilePath) {
    return Napi::String::New(info.Env(), d->filePath());
}

NAPI_SETTER(FileChannelWrap, FilePath) {
    d->setFilePath(info[0].As<Napi::String>());
}

NAPI_GETTER(FileChannelWrap, OpenMode) {
    return Napi::Number::New(info.Env(), int(d->openMode()));
}

NAPI_SETTER(FileChannelWrap, OpenMode) {
    d->setOpenMode(Icd::FileChannel::OpenMode(info[0].As<Napi::Number>().Int32Value()));
}

NAPI_GETTER(FileChannelWrap, SaveFormat) {
    return Napi::Number::New(info.Env(), int(d->saveFormat()));
}

NAPI_SETTER(FileChannelWrap, SaveFormat) {
    d->setSaveFormat(Icd::FileChannel::SaveFormat(info[0].As<Napi::Number>().Int32Value()));
}

NAPI_GETTER(FileChannelWrap, FileHeader) {
    return Napi::Number::New(info.Env(), d->fileHeader());
}

NAPI_SETTER(FileChannelWrap, FileHeader) {
    d->setFileHeader(info[0].As<Napi::Number>().Uint32Value());
}

NAPI_GETTER(FileChannelWrap, Domain) {
    return Napi::String::New(info.Env(), d->domain());
}

NAPI_SETTER(FileChannelWrap, Domain) {
    d->setDomain(info[0].As<Napi::String>());
}

PROTOCORE_END
