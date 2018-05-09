#include "precomp.h"
#include "Frame_wrap.h"
#include "icdcore/icd_item_Frame.h"
#include "icdcore/icd_table.h"
#include "table_wrap.h"

PROTOCORE_BEGIN

Napi::FunctionReference FrameWrap::ctor;

void FrameWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        ITEMWRAP_METHODS_PROPS(FrameWrap),
        InstanceMethod("addTable", &FrameWrap::AddTable, napi_enumerable),
        InstanceMethod("removeTable", &FrameWrap::RemoveTable, napi_enumerable),
        InstanceMethod("clearTable", &FrameWrap::ClearTable, napi_enumerable),
        InstanceMethod("replaceCode", &FrameWrap::ReplaceCode, napi_enumerable),
        InstanceAccessor("tables", &FrameWrap::GetTables, nullptr, napi_enumerable),
        InstanceAccessor("tableCount", &FrameWrap::GetTableCount, nullptr, napi_enumerable),
        InstanceMethod("tableAt", &FrameWrap::TableAt, napi_enumerable),
        InstanceMethod("setBufferSize", &FrameWrap::SetBufferSize, napi_enumerable),
        InstanceAccessor("sequenceCount", &FrameWrap::GetSequenceCount, &FrameWrap::SetSequenceCount, napi_enumerable)
    };
    ctor = napi_init<FrameWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Frame", properties);
}

FrameWrap::FrameWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<FrameWrap>(info)
    , d(napi_unwrap_data<Icd::FrameItem>(info, true))
{

}

ITEMWRAP_METHODS_IMPL(FrameWrap)

NAPI_VOID_METHOD(FrameWrap, AddTable) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    const TableWrap *table = TableWrap::Unwrap(info[0].As<Napi::Object>());
    if (!table) {
        return;
    }
    d->addTable(table->data());
}

NAPI_VOID_METHOD(FrameWrap, RemoveTable) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    d->removeTable(info[0].As<Napi::Number>().Int64Value());
}

NAPI_VOID_METHOD(FrameWrap, ClearTable) {
    d->clearTable();
}

NAPI_VOID_METHOD(FrameWrap, ReplaceCode) {
    if (info.Length() != 2) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need two arguments at least!"));
        return;
    }
    d->replaceCode(info[0].As<Napi::Number>().Int64Value(),
            info[1].As<Napi::Number>().Int64Value());
}

NAPI_GETTER(FrameWrap, Tables) {
    const auto &tables = d->allTable();
    Napi::Array array = Napi::Array::New(info.Env());
    for (auto citer = tables.cbegin(); citer != tables.cend(); ++citer) {
        if (citer->second) {
            //TODO
            array.Set(Napi::Number::New(info.Env(), citer->first),
                      napi_instance(info.Env(), TableWrap::ctor, citer->second));
        }
    }
    return array;
}

NAPI_GETTER(FrameWrap, TableCount) {
    return Napi::Number::New(info.Env(), d->tableCount());
}

NAPI_METHOD(FrameWrap, TableAt) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    auto table = d->tableAt(info[0].As<Napi::Number>().Int64Value());
    if (table) {
        return napi_instance(info.Env(), TableWrap::ctor, table);
    }

    return Napi::Value();
}

NAPI_VOID_METHOD(FrameWrap, SetBufferSize) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    d->setBufferSize(info[0].As<Napi::Number>());
}

NAPI_GETTER(FrameWrap, SequenceCount) {
    return Napi::Number::New(info.Env(), d->sequenceCount());
}

NAPI_SETTER(FrameWrap, SequenceCount) {
    d->setSequenceCount(info[0].As<Napi::Number>());
}

PROTOCORE_END
