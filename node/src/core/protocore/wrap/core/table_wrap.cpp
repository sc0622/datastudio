#include "precomp.h"
#include "table_wrap.h"
#include "item_wrap.h"
#include "check_wrap.h"
#include "counter_wrap.h"
#include "framecode_wrap.h"
#include "icdcore/icd_table.h"
#include "icdcore/icd_item.h"
#include "icdcore/icd_item_counter.h"
#include "icdcore/icd_item_check.h"
#include "icdcore/icd_item_framecode.h"

PROTOCORE_BEGIN

Napi::FunctionReference TableWrap::ctor;

void TableWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        OBJECTWRAP_METHODS_PROPS(TableWrap),
        InstanceAccessor("itemOffset", &TableWrap::GetItemOffset, nullptr, napi_enumerable),
        InstanceAccessor("bufferSize", &TableWrap::GetBufferSize, nullptr, napi_enumerable),
        InstanceAccessor("bufferOffset", &TableWrap::GetBufferOffset, nullptr, napi_enumerable),
        //InstanceAccessor("buffer", &TableWrap::GetBuffer, &TableWrap::SetBuffer, napi_enumerable),
        InstanceAccessor("items", &TableWrap::GetItems, nullptr, napi_enumerable),
        InstanceMethod("appendItem", &TableWrap::AppendItem, napi_enumerable),
        InstanceMethod("insertItem", &TableWrap::InsertItem, napi_enumerable),
        InstanceMethod("removeItem", &TableWrap::RemoveItem, napi_enumerable),
        InstanceMethod("removeItemById", &TableWrap::RemoveItemById, napi_enumerable),
        InstanceMethod("removeItemByMark", &TableWrap::RemoveItemByMark, napi_enumerable),
        InstanceMethod("clearItem", &TableWrap::ClearItem, napi_enumerable),
        InstanceAccessor("itemCount", &TableWrap::GetItemCount, nullptr, napi_enumerable),
        InstanceMethod("itemAt", &TableWrap::ItemAt, napi_enumerable),
        InstanceMethod("itemById", &TableWrap::ItemById, napi_enumerable),
        InstanceMethod("itemByMark", &TableWrap::ItemByMark, napi_enumerable),
        InstanceMethod("tableByMark", &TableWrap::TableByMark, napi_enumerable),
        InstanceMethod("itemByDomain", &TableWrap::ItemByDomain, napi_enumerable),
        InstanceMethod("tableByDomain", &TableWrap::TableByDomain, napi_enumerable),
        //InstanceAccessor("headers", &TableWrap::GetHeaders, nullptr, napi_enumerable),
        InstanceAccessor("counter", &TableWrap::GetCounterItem, nullptr, napi_enumerable),
        InstanceAccessor("isCheckValid", &TableWrap::GetIsCheckValid, nullptr, napi_enumerable),
        InstanceAccessor("check", &TableWrap::GetCheckItem, nullptr, napi_enumerable),
        InstanceAccessor("frameCodes", &TableWrap::GetFrameCodes, nullptr, napi_enumerable),
        InstanceMethod("updateSend", &TableWrap::UpdateSend, napi_enumerable),
        InstanceMethod("updateRecv", &TableWrap::UpdateRecv, napi_enumerable),
        InstanceMethod("resetSend", &TableWrap::ResetSend, napi_enumerable),
        //InstanceMethod("bindBuffer", &TableWrap::BindBuffer, napi_enumerable),
        InstanceAccessor("sequence", &TableWrap::GetSequence, &TableWrap::SetSequence, napi_enumerable),
        InstanceAccessor("period", &TableWrap::GetPeriod, nullptr, napi_enumerable),
        InstanceAccessor("isFrameTable", &TableWrap::GetIsFrameTable, nullptr, napi_enumerable),
        InstanceAccessor("typeName", &TableWrap::GetTypeName, nullptr, napi_enumerable),
        InstanceAccessor("codeName", &TableWrap::GetCodeName, nullptr, napi_enumerable)
    };
    ctor = napi_init<TableWrap, PROTOCORE_DOMAIN::ObjectWrap>(env, exports, "Table", properties);
}

TableWrap::TableWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<TableWrap>(info)
    , d(napi_unwrap_data<Icd::Table>(info, true))
{

}

OBJECTWRAP_METHODS_IMPL(TableWrap)

NAPI_GETTER(TableWrap, ItemOffset) {
    return Napi::Number::New(info.Env(), d->itemOffset());
}

NAPI_GETTER(TableWrap, BufferSize) {
    return Napi::Number::New(info.Env(), d->bufferSize());
}

NAPI_GETTER(TableWrap, BufferOffset) {
    return Napi::Number::New(info.Env(), d->bufferOffset());
}

NAPI_GETTER(TableWrap, Items) {
    const auto &items = d->allItem();
    Napi::Array array = Napi::Array::New(info.Env(), items.size());
    for (int i = 0; i < items.size(); ++i) {
        const Icd::ItemPtr &item = items[i];
        if (item) {
            array.Set(i, napi_instance(info.Env(), ItemWrap::ctor, item));
        }
    }
    return array;
}

NAPI_VOID_METHOD(TableWrap, AppendItem) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    const ItemWrap *item = ItemWrap::Unwrap(info[0].As<Napi::Object>());
    if (!item) {
        return;
    }
    d->appendItem(item->data());
}

NAPI_VOID_METHOD(TableWrap, InsertItem) {
    if (info.Length() != 2) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need two arguments!"));
        return;
    }
    const int index = info[0].As<Napi::Number>();
    const ItemWrap *item = ItemWrap::Unwrap(info[1].As<Napi::Object>());
    if (!item) {
        return;
    }
    d->insertItem(index, item->data());
}

NAPI_VOID_METHOD(TableWrap, RemoveItem) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    d->removeItem(info[0].As<Napi::Number>());
}

NAPI_VOID_METHOD(TableWrap, RemoveItemById) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    d->removeItemById(info[0].As<Napi::String>());
}

NAPI_VOID_METHOD(TableWrap, RemoveItemByMark) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    d->removeItemByMark(info[0].As<Napi::String>());
}

NAPI_VOID_METHOD(TableWrap, ClearItem) {
    d->clearItem();
}

NAPI_GETTER(TableWrap, ItemCount) {
    return Napi::Number::New(info.Env(), d->itemCount());
}

NAPI_METHOD(TableWrap, ItemAt) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    auto item = d->itemAt(info[0].As<Napi::Number>());
    if (item) {
        return napi_instance(info.Env(), ItemWrap::ctor, item);
    }

    return Napi::Value();
}

NAPI_METHOD(TableWrap, ItemById) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return Napi::Value();
    }
    auto item = d->itemById(info[0].As<Napi::String>());
    if (item) {
        return napi_instance(info.Env(), ItemWrap::ctor, item);
    }

    return Napi::Value();
}

NAPI_METHOD(TableWrap, ItemByMark) {
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

NAPI_METHOD(TableWrap, TableByMark) {
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
        return napi_instance(info.Env(), TableWrap::ctor, table);
    }

    return Napi::Value();
}

NAPI_METHOD(TableWrap, ItemByDomain) {
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

NAPI_METHOD(TableWrap, TableByDomain) {
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
        return napi_instance(info.Env(), TableWrap::ctor, table);
    }

    return Napi::Value();
}

NAPI_GETTER(TableWrap, CounterItem) {
    return napi_instance(info.Env(), CounterWrap::ctor, d->counterItem());
}

NAPI_GETTER(TableWrap, IsCheckValid) {
    return Napi::Boolean::New(info.Env(), d->isCheckValid());
}

NAPI_GETTER(TableWrap, CheckItem) {
    return napi_instance(info.Env(), CheckWrap::ctor, d->checkItem());
}

NAPI_GETTER(TableWrap, FrameCodes) {
    const auto &frameCodes = d->allFrameCode();
    Napi::Array array = Napi::Array::New(info.Env(), frameCodes.size());
    for (int i = 0; i < frameCodes.size(); ++i) {
        const Icd::FrameCodeItemPtr &frameCode = frameCodes[i];
        if (frameCode) {
            array.Set(i, napi_instance(info.Env(), FrameCodeWrap::ctor, frameCode));
        }
    }
    return array;
}

NAPI_VOID_METHOD(TableWrap, UpdateSend) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    d->updateSend(info[0].As<Napi::Boolean>());
}

NAPI_VOID_METHOD(TableWrap, UpdateRecv) {
    d->updateRecv();
}

NAPI_VOID_METHOD(TableWrap, ResetSend) {
    d->resetSend();
}

NAPI_GETTER(TableWrap, Sequence) {
    return Napi::Number::New(info.Env(), d->sequence());
}

NAPI_SETTER(TableWrap, Sequence) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
        return;
    }
    d->setSequence(info[0].As<Napi::Number>());
}

NAPI_GETTER(TableWrap, Period) {
    return Napi::Number::New(info.Env(), d->period());
}

NAPI_GETTER(TableWrap, IsFrameTable) {
    return Napi::Boolean::New(info.Env(), d->isFrameTable());
}

NAPI_GETTER(TableWrap, TypeName) {
    return Napi::String::New(info.Env(), d->typeName());
}

NAPI_GETTER(TableWrap, CodeName) {
    return Napi::String::New(info.Env(), d->codeName());
}

PROTOCORE_END
