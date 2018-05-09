#include "precomp.h"
#include "System_wrap.h"
#include "table_wrap.h"
#include "icdcore/icd_system.h"
#include "icdcore/icd_table.h"

PROTOCORE_BEGIN

Napi::FunctionReference SystemWrap::ctor;

void SystemWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        OBJECTWRAP_METHODS_PROPS(SystemWrap),
        InstanceAccessor("Tables", &SystemWrap::GetTables, nullptr, napi_enumerable),
        InstanceMethod("appendTable", &SystemWrap::AppendTable, napi_enumerable),
        InstanceMethod("insertTable", &SystemWrap::InsertTable, napi_enumerable),
        InstanceMethod("removeTable", &SystemWrap::RemoveTable, napi_enumerable),
        InstanceMethod("removeTableByMark", &SystemWrap::RemoveTableByMark, napi_enumerable),
        InstanceMethod("clearTable", &SystemWrap::ClearTable, napi_enumerable),
        InstanceAccessor("tableCount", &SystemWrap::GetTableCount, nullptr, napi_enumerable),
        InstanceMethod("tableAt", &SystemWrap::TableAt, napi_enumerable),
        InstanceMethod("tableByMark", &SystemWrap::TableByMark, napi_enumerable)
    };
    ctor = napi_init<SystemWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "System", properties);
}

SystemWrap::SystemWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<SystemWrap>(info)
    , d(napi_unwrap_data<Icd::System>(info, true))
{

}

OBJECTWRAP_METHODS_IMPL(SystemWrap)

NAPI_GETTER(SystemWrap, Tables) {
    const auto &tables = d->allTable();
    Napi::Array items = Napi::Array::New(info.Env(), tables.size());
    for (int i = 0; i < tables.size(); ++i) {
        const Icd::TablePtr &table = tables[i];
        if (table) {
            items.Set(i, napi_instance(info.Env(), TableWrap::ctor, table));
        }
    }
    return items;
}

NAPI_VOID_METHOD(SystemWrap, AppendTable) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    const TableWrap *table = TableWrap::Unwrap(info[0].As<Napi::Object>());
    if (!table) {
        return;
    }
    d->appendTable(table->data());
}

NAPI_VOID_METHOD(SystemWrap, InsertTable) {
    if (info.Length() != 2) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need two arguments!"));
        return;
    }
    const int index = info[0].As<Napi::Number>();
    const TableWrap *table = TableWrap::Unwrap(info[1].As<Napi::Object>());
    if (!table) {
        return;
    }
    d->insertTable(index, table->data());
}

NAPI_VOID_METHOD(SystemWrap, RemoveTable) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    d->removeTable(info[0].As<Napi::Number>());
}

NAPI_VOID_METHOD(SystemWrap, RemoveTableByMark) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    d->removeTableByMark(info[0].As<Napi::String>());
}

NAPI_VOID_METHOD(SystemWrap, ClearTable) {
    d->clearTable();
}

NAPI_GETTER(SystemWrap, TableCount) {
    return Napi::Number::New(info.Env(), d->tableCount());
}

NAPI_METHOD(SystemWrap, TableAt) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    auto table = d->tableAt(info[0].As<Napi::Number>());
    if (table) {
        return napi_instance(info.Env(), TableWrap::ctor, table);
    }

    return Napi::Value();
}

NAPI_METHOD(SystemWrap, TableByMark) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    auto table = d->tableByMark(info[0].As<Napi::String>());
    if (table) {
        return napi_instance(info.Env(), TableWrap::ctor, table);
    }

    return Napi::Value();
}

PROTOCORE_END
