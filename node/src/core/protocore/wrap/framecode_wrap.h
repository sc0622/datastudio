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

private:
    Icd::FrameCodeItemPtr d;
};

PROTOCORE_END
