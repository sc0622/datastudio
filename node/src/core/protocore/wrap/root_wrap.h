#pragma once

#include "node_global.h"
#include "object_wrap.h"

namespace Icd {
class Root;
typedef std::shared_ptr<Icd::Root> RootPtr;
}

PROTOCORE_BEGIN

class RootWrap : public Napi::ObjectWrap<RootWrap>
{
public:
    RootWrap(const Napi::CallbackInfo &info);

    Napi::Value vehicles(const Napi::CallbackInfo &info);

    static void Initialize(Napi::Env env, Napi::Object exports);

private:
    Icd::RootPtr data;
    Napi::FunctionReference Constructor;
};

PROTOCORE_END
