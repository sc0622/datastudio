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

private:
    Icd::FrameItemPtr d;
};

PROTOCORE_END
