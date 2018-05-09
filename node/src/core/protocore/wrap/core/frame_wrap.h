#pragma once

#include "node_global.h"
#include "item_wrap.h"

namespace Icd {
class FrameItem;
typedef std::shared_ptr<FrameItem> FrameItemPtr;
}

PROTOCORE_BEGIN

class FrameWrap : public Napi::ObjectWrap<FrameWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    FrameWrap(const Napi::CallbackInfo &info);

    Icd::FrameItemPtr data() const { return d; }

    ITEMWRAP_METHODS_DECL();

    NAPI_VOID_METHOD_DECL(AddTable);
    NAPI_VOID_METHOD_DECL(RemoveTable);
    NAPI_VOID_METHOD_DECL(ClearTable);
    NAPI_VOID_METHOD_DECL(ReplaceCode);
    NAPI_GETTER_DECL(Tables);
    NAPI_GETTER_DECL(TableCount);
    NAPI_METHOD_DECL(TableAt);
    //NAPI_VOID_METHOD_DECL(SetBuffer);
    NAPI_VOID_METHOD_DECL(SetBufferSize);
    NAPI_PROPERTY_DECL(SequenceCount);

private:
    Icd::FrameItemPtr d;
};

PROTOCORE_END
