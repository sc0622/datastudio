#pragma once

#include "node_global.h"
#include "item_wrap.h"

namespace Icd {
class FrameCodeItem;
typedef std::shared_ptr<FrameCodeItem> FrameCodeItemPtr;
}

PROTOCORE_BEGIN

class FrameCodeWrap : public Napi::ObjectWrap<FrameCodeWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    FrameCodeWrap(const Napi::CallbackInfo &info);

    Icd::FrameCodeItemPtr data() const { return d; }

    ITEMWRAP_METHODS_DECL();

    NAPI_PROPERTY_DECL(FrameCodeType);
    NAPI_GETTER_DECL(FrameCodeTypeString);
    static NAPI_METHOD_DECL(FrameCodeTypeString);
    static NAPI_METHOD_DECL(StringFrameCodeType);
    NAPI_PROPERTY_DECL(FrameId);
    NAPI_PROPERTY_DECL(Frame);
    NAPI_VOID_METHOD_DECL(UpdateSend);
    NAPI_VOID_METHOD_DECL(UpdateRecv);
    NAPI_VOID_METHOD_DECL(ResetSend);

private:
    Icd::FrameCodeItemPtr d;
};

PROTOCORE_END
