#pragma once

#include "node_global.h"
#include "item_wrap.h"

namespace Icd {
class ComplexItem;
typedef std::shared_ptr<ComplexItem> ComplexItemPtr;
class Table;
typedef std::shared_ptr<Table> TablePtr;
}

PROTOCORE_BEGIN

class ComplexWrap : public Napi::ObjectWrap<ComplexWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    ComplexWrap(const Napi::CallbackInfo &info);

    Icd::ComplexItemPtr data() const { return d; }

    ITEMWRAP_METHODS_DECL();

    NAPI_GETTER_DECL(Table);
    NAPI_VOID_METHOD_DECL(UpdateSend);
    NAPI_VOID_METHOD_DECL(UpdateRecv);
    NAPI_VOID_METHOD_DECL(ResetSend);
    NAPI_METHOD_DECL(ItemByMark);
    NAPI_METHOD_DECL(TableByMark);
    NAPI_METHOD_DECL(ItemByDomain);
    NAPI_METHOD_DECL(TableByDomain);

private:
    Icd::ComplexItemPtr d;
};

PROTOCORE_END
