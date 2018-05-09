#pragma once

#include "node_global.h"
#include "object_wrap.h"

namespace Icd {
class Table;
typedef std::shared_ptr<Table> TablePtr;
}

PROTOCORE_BEGIN

class TableWrap : public Napi::ObjectWrap<TableWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    TableWrap(const Napi::CallbackInfo &info);

    Icd::TablePtr data() const { return d; }

    OBJECTWRAP_METHODS_DECL();

    NAPI_GETTER_DECL(ItemOffset);
    NAPI_GETTER_DECL(BufferSize);
    NAPI_GETTER_DECL(BufferOffset);
    //NAPI_PROPERTY_DECL(Buffer);
    NAPI_GETTER_DECL(Items);
    NAPI_VOID_METHOD_DECL(AppendItem);
    NAPI_VOID_METHOD_DECL(InsertItem);
    NAPI_VOID_METHOD_DECL(RemoveItem);
    NAPI_VOID_METHOD_DECL(RemoveItemById);
    NAPI_VOID_METHOD_DECL(RemoveItemByMark);
    NAPI_VOID_METHOD_DECL(ClearItem);
    NAPI_GETTER_DECL(ItemCount);
    NAPI_METHOD_DECL(ItemAt);
    NAPI_METHOD_DECL(ItemById);
    NAPI_METHOD_DECL(ItemByMark);
    NAPI_METHOD_DECL(TableByMark);
    NAPI_METHOD_DECL(ItemByDomain);
    NAPI_METHOD_DECL(TableByDomain);
    //NAPI_GETTER_DECL(Headers);
    NAPI_GETTER_DECL(CounterItem);
    NAPI_GETTER_DECL(IsCheckValid);
    NAPI_GETTER_DECL(CheckItem);
    NAPI_GETTER_DECL(FrameCodes);
    NAPI_VOID_METHOD_DECL(UpdateSend);
    NAPI_VOID_METHOD_DECL(UpdateRecv);
    NAPI_VOID_METHOD_DECL(ResetSend);
    //NAPI_VOID_METHOD_DECL(BindBuffer);
    NAPI_PROPERTY_DECL(Sequence);
    NAPI_GETTER_DECL(Period);
    NAPI_GETTER_DECL(IsFrameTable);
    NAPI_GETTER_DECL(TypeName);
    NAPI_GETTER_DECL(CodeName);

private:
    Icd::TablePtr d;
};

PROTOCORE_END
