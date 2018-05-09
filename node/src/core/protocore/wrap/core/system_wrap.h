#pragma once

#include "node_global.h"
#include "object_wrap.h"

namespace Icd {
class System;
typedef std::shared_ptr<System> SystemPtr;
}

PROTOCORE_BEGIN

class SystemWrap : public Napi::ObjectWrap<SystemWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    SystemWrap(const Napi::CallbackInfo &info);

    Icd::SystemPtr data() const { return d; }

    OBJECTWRAP_METHODS_DECL();

    NAPI_GETTER_DECL(Tables);
    NAPI_VOID_METHOD_DECL(AppendTable);
    NAPI_VOID_METHOD_DECL(InsertTable);
    NAPI_VOID_METHOD_DECL(RemoveTable);
    NAPI_VOID_METHOD_DECL(RemoveTableByMark);
    NAPI_VOID_METHOD_DECL(ClearTable);
    NAPI_GETTER_DECL(TableCount);
    NAPI_METHOD_DECL(TableAt);
    NAPI_METHOD_DECL(TableByMark);

private:
    Icd::SystemPtr d;
};

PROTOCORE_END
