#include "precomp.h"
#include "complex_wrap.h"
#include "icdcore/icd_item_complex.h"
#include "icdcore/icd_table.h"
#include "table_wrap.h"

PROTOCORE_BEGIN

Napi::FunctionReference ComplexWrap::ctor;

void ComplexWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        ITEMWRAP_METHODS_PROPS(ComplexWrap),
        InstanceAccessor("table", &ComplexWrap::GetTable, nullptr, napi_enumerable),
        InstanceMethod("updateSend", &ComplexWrap::UpdateSend, napi_enumerable),
        InstanceMethod("updateRecv", &ComplexWrap::UpdateRecv, napi_enumerable),
        InstanceMethod("resetSend", &ComplexWrap::ResetSend, napi_enumerable),
        InstanceMethod("itemByMark", &ComplexWrap::ItemByMark, napi_enumerable),
        InstanceMethod("tableByMark", &ComplexWrap::TableByMark, napi_enumerable),
        InstanceMethod("itemByDomain", &ComplexWrap::ItemByDomain, napi_enumerable),
        InstanceMethod("tableByDomain", &ComplexWrap::TableByDomain, napi_enumerable),
    };
    ctor = napi_init<ComplexWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Complex", properties);
}

ComplexWrap::ComplexWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<ComplexWrap>(info)
    , d(napi_unwrap_data<Icd::ComplexItem>(info, true))
{

}

ITEMWRAP_METHODS_IMPL(ComplexWrap)

NAPI_GETTER(ComplexWrap, Table) {
    auto table = d->table();
    if (table) {
        return napi_instance(info.Env(), TableWrap::ctor, table);
    } else {
        return Napi::Value();
    }
}

NAPI_VOID_METHOD(ComplexWrap, UpdateSend) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    d->updateSend(info[0].As<Napi::Boolean>());
}

NAPI_VOID_METHOD(ComplexWrap, UpdateRecv) {
    d->updateRecv();
}

NAPI_VOID_METHOD(ComplexWrap, ResetSend) {
    d->resetSend();
}

NAPI_METHOD(ComplexWrap, ItemByMark) {
    const int length = info.Length();
    if (length < 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument at least!"));
        return Napi::Value();
    }
    bool deep = true;
    if (length >= 2) {
        deep = info[1].As<Napi::Boolean>();
    }
    auto item = d->itemByMark(info[0].As<Napi::String>(), deep);
    if (item) {
        return napi_instance(info.Env(), ItemWrap::ctor, item);
    }

    return Napi::Value();
}

NAPI_METHOD(ComplexWrap, TableByMark) {
    const int length = info.Length();
    if (length < 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument at least!"));
        return Napi::Value();
    }
    bool deep = true;
    if (length >= 2) {
        deep = info[1].As<Napi::Boolean>();
    }
    auto table = d->tableByMark(info[0].As<Napi::String>(), deep);
    if (table) {
        return napi_instance(info.Env(), ComplexWrap::ctor, table);
    }

    return Napi::Value();
}

NAPI_METHOD(ComplexWrap, ItemByDomain) {
    const int length = info.Length();
    if (length < 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument at least!"));
        return Napi::Value();
    }
    Icd::DomainType domainType = Icd::DomainId;
    if (length >= 2) {
        domainType = Icd::DomainType(info[1].As<Napi::Number>().Int32Value());
    }
    auto item = d->itemByDomain(info[0].As<Napi::String>());
    if (item) {
        return napi_instance(info.Env(), ItemWrap::ctor, item);
    }

    return Napi::Value();
}

NAPI_METHOD(ComplexWrap, TableByDomain) {
    const int length = info.Length();
    if (length < 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument at least!"));
        return Napi::Value();
    }
    Icd::DomainType domainType = Icd::DomainId;
    if (length >= 2) {
        domainType = Icd::DomainType(info[1].As<Napi::Number>().Int32Value());
    }
    auto table = d->tableByDomain(info[0].As<Napi::String>());
    if (table) {
        return napi_instance(info.Env(), ComplexWrap::ctor, table);
    }

    return Napi::Value();
}

PROTOCORE_END
