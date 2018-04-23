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
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    RootWrap(const Napi::CallbackInfo &info);

    Napi::Value vehicles(const Napi::CallbackInfo &info);

private:
    Icd::RootPtr data;
};

PROTOCORE_END
