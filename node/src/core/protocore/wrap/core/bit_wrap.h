#pragma once

#include "node_global.h"
#include "item_wrap.h"

namespace Icd {
class BitItem;
typedef std::shared_ptr<BitItem> BitItemPtr;
}

PROTOCORE_BEGIN

class BitWrap : public Napi::ObjectWrap<BitWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    BitWrap(const Napi::CallbackInfo &info);

    Icd::BitItemPtr data() const { return d; }

    ITEMWRAP_METHODS_DECL();

    NAPI_METHOD_DECL(TestBit);
    NAPI_PROPERTY_DECL(BitStart);
    NAPI_PROPERTY_DECL(BitCount);
    NAPI_PROPERTY_DECL(TypeSize);
    NAPI_GETTER_DECL(Mask);
    NAPI_GETTER_DECL(Specs);
    NAPI_VOID_METHOD_DECL(AddSpec);
    NAPI_VOID_METHOD_DECL(RemoveSpec);
    NAPI_VOID_METHOD_DECL(ClearSpec);
    NAPI_METHOD_DECL(SpecAt);
    NAPI_METHOD_DECL(NameAt);
    NAPI_METHOD_DECL(NameOf);
    NAPI_METHOD_DECL(DescAt);

private:
    Icd::BitItemPtr d;
};

PROTOCORE_END
