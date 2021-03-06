#include "precomp.h"
#include "parser_wrap.h"
#include "icdparser/icdparser.h"
#include "icdcore/icdcore_inc.h"
#include "../core/root_wrap.h"
#include "../core/vehicle_wrap.h"
#include "../core/system_wrap.h"
#include "../core/table_wrap.h"
#include "../core/item_wrap.h"

PROTOCORE_BEGIN

Napi::FunctionReference ParserWrap::ctor;

void ParserWrap::Initialize(Napi::Env env, Napi::Object exports)
{
    std::vector<PropertyDescriptor> properties = {
        StaticMethod("create", &ParserWrap::Create, napi_enumerable),
        InstanceMethod("parse", &ParserWrap::Parse, napi_enumerable),
        InstanceMethod("parseRoot", &ParserWrap::ParseRoot, napi_enumerable),
        InstanceMethod("parseVehicle", &ParserWrap::ParseVehicle, napi_enumerable),
        InstanceMethod("parseSystem", &ParserWrap::ParseSystem, napi_enumerable),
        InstanceMethod("parseTable", &ParserWrap::ParseTable, napi_enumerable),
        InstanceMethod("parseItem", &ParserWrap::ParseItem, napi_enumerable),
        InstanceMethod("save", &ParserWrap::Save, napi_enumerable),
        InstanceMethod("beginModify", &ParserWrap::BeginModify, napi_enumerable),
        InstanceMethod("commitModify", &ParserWrap::CommitModify, napi_enumerable),
        InstanceMethod("cancelModify", &ParserWrap::CancelModify, napi_enumerable),
        InstanceMethod("endModify", &ParserWrap::EndModify, napi_enumerable),
        InstanceAccessor("isBeginModify", &ParserWrap::GetIsBeginModify, nullptr, napi_enumerable),
        InstanceMethod("saveAs", &ParserWrap::SaveAs, napi_enumerable),
        InstanceAccessor("message", &ParserWrap::GetMessage, &ParserWrap::SetMessage, napi_enumerable),
        InstanceAccessor("progressValue", &ParserWrap::GetProgressValue, &ParserWrap::SetProgressValue, napi_enumerable),
        InstanceAccessor("canceledSaveAs", &ParserWrap::GetCanceledSaveAs, nullptr, napi_enumerable),
        InstanceMethod("cancelSaveAs", &ParserWrap::CancelSaveAs, napi_enumerable),
        StaticMethod("queryTable", &ParserWrap::QueryTable, napi_enumerable)
    };
    ctor = napi_init<ParserWrap>(env, exports, "Parser", properties);
}

ParserWrap::ParserWrap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<ParserWrap>(info)
    , d(napi_unwrap_data<Icd::Parser>(info))
{

}

NAPI_METHOD(ParserWrap, Create) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
    }
    const std::string _config = info[0].As<Napi::String>();
    Json::Value config;
    if (!Json::Reader().parse(_config, config)) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Argument error!"));
    }
    return napi_instance(info.Env(), ctor, Icd::Parser::create(config));
}

NAPI_METHOD(ParserWrap, Parse) {
    if (info.Length() != 3) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need three arguments!"));
    }
    Icd::ObjectPtr object = d->parse(info[0].As<Napi::String>(),
            int(info[1].As<Napi::Number>()), info[2].As<Napi::Number>());
    if (object) {
        return protocore::ObjectWrap::toObject(info.Env(), object);
    }
    return Napi::Boolean::New(info.Env(), false);
}

NAPI_METHOD(ParserWrap, ParseRoot) {
    if (info.Length() != 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
    }
    Icd::RootPtr root;
    if (d->parse(root, info[0].As<Napi::Number>())) {
        return napi_instance(info.Env(), RootWrap::ctor, root);
    }
    return Napi::Boolean::New(info.Env(), false);
}

NAPI_METHOD(ParserWrap, ParseVehicle) {
    if (info.Length() < 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument at least!"));
    }
    switch (info.Length()) {
    case 1:
    {
        Icd::VehiclePtrArray vehicles;
        if (!d->parse(vehicles, info[0].As<Napi::Number>())) {
            break;
        }
        Napi::Array array = Napi::Array::New(info.Env());
        for (std::size_t i = 0; i < vehicles.size(); ++i) {
            array.Set(i, napi_instance(info.Env(), VehicleWrap::ctor, vehicles.at(i)));
        }
        return array;
    }
    case 2:
    {
        Icd::VehiclePtr vehicle;
        if (!d->parse(info[0].As<Napi::String>(), vehicle, info[1].As<Napi::Number>())) {
            break;
        }
        return napi_instance(info.Env(), VehicleWrap::ctor, vehicle);
    }
    default:
        break;
    }

    return Napi::Boolean::New(info.Env(), false);
}

NAPI_METHOD(ParserWrap, ParseSystem) {
    if (info.Length() < 2) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need two arguments at least!"));
    }
    switch (info.Length()) {
    case 2:
    {
        Icd::SystemPtrArray systems;
        if (!d->parse(info[0].As<Napi::String>(), systems, info[1].As<Napi::Number>())) {
            break;
        }
        Napi::Array array = Napi::Array::New(info.Env());
        for (std::size_t i = 0; i < systems.size(); ++i) {
            array.Set(i, napi_instance(info.Env(), SystemWrap::ctor, systems.at(i)));
        }
        return array;
    }
    case 3:
    {
        Icd::SystemPtr system;
        if (!d->parse(info[0].As<Napi::String>(), info[1].As<Napi::String>(),
                      system, info[2].As<Napi::Number>())) {
            break;
        }
        return napi_instance(info.Env(), SystemWrap::ctor, system);
    }
    default:
        break;
    }

    return Napi::Boolean::New(info.Env(), false);
}

NAPI_METHOD(ParserWrap, ParseTable) {
    switch (info.Length()) {
    case 0:
    {
        Icd::TablePtrArray tables;
        if (!d->parse(tables)) {
            break;
        }
        Napi::Array array = Napi::Array::New(info.Env());
        for (std::size_t i = 0; i < tables.size(); ++i) {
            array.Set(i, napi_instance(info.Env(), TableWrap::ctor, tables.at(i)));
        }
        return array;
    }
    case 1:
    {
        Icd::TablePtr table;
        if (!d->parse(info[0].As<Napi::String>(), table)) {
            break;
        }
        return napi_instance(info.Env(), TableWrap::ctor, table);
    }
    case 3:
    {
        Icd::TablePtrArray tables;
        if (!d->parse(info[0].As<Napi::String>(), info[1].As<Napi::String>(),
                      tables, info[2].As<Napi::Number>())) {
            break;
        }
        Napi::Array array = Napi::Array::New(info.Env());
        for (std::size_t i = 0; i < tables.size(); ++i) {
            array.Set(i, napi_instance(info.Env(), TableWrap::ctor, tables.at(i)));
        }
        return array;
    }
    case 4:
    {
        Icd::TablePtr table;
        if (!d->parse(info[0].As<Napi::String>(), info[1].As<Napi::String>(),
                      info[2].As<Napi::String>(), table, info[3].As<Napi::Number>())) {
            break;
        }
        return napi_instance(info.Env(), TableWrap::ctor, table);
    }
    default:
        break;
    }

    return Napi::Boolean::New(info.Env(), false);
}

NAPI_METHOD(ParserWrap, ParseItem) {
    if (info.Length() < 4) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need four arguments at least!"));
    }
    switch (info.Length()) {
    case 4:
    {
        Icd::ItemPtrArray items;
        if (!d->parse(info[0].As<Napi::String>(), info[1].As<Napi::String>(),
                      info[2].As<Napi::String>(), items, info[3].As<Napi::Number>())) {
            break;
        }
        Napi::Array array = Napi::Array::New(info.Env());
        for (std::size_t i = 0; i < items.size(); ++i) {
            array.Set(i, napi_instance(info.Env(), ItemWrap::ctor, items.at(i)));
        }
        return array;
    }
    case 5:
    {
        Icd::ItemPtr item;
        if (!d->parse(info[0].As<Napi::String>(), info[1].As<Napi::String>(),
                      info[2].As<Napi::String>(), info[3].As<Napi::String>(),
                      item, info[4].As<Napi::Number>())) {
            break;
        }
        return napi_instance(info.Env(), ItemWrap::ctor, item);
    }
    default:
        break;
    }

    return Napi::Boolean::New(info.Env(), false);
}

NAPI_METHOD(ParserWrap, Save) {
    if (info.Length() < 1) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument at least!"));
    }
    bool result = false;
    switch (info.Length()) {
    case 1:
    {
        const Napi::Value value = info[0];
        if (value.IsArray()) {
            const Napi::Array array = value.As<Napi::Array>();
            const uint32_t length = array.Length();
            if (length == 0) {
                result = true;
                break;
            }
            const Napi::Value first = array.Get(uint32_t(0));
            if (!first.IsObject()) {
                break;
            }
            const Napi::Object object = first.ToObject();
            if (object.InstanceOf(VehicleWrap::ctor.Value())) {
                Icd::VehiclePtrArray vehicles;
                for (uint32_t i = 0; i < length; ++i) {
                    vehicles.push_back(VehicleWrap::Unwrap(array.Get(i).ToObject())->data());
                }
                result = d->save(vehicles);
            } else if (object.InstanceOf(TableWrap::ctor.Value())) {
                Icd::TablePtrArray tables;
                for (uint32_t i = 0; i < length; ++i) {
                    tables.push_back(TableWrap::Unwrap(array.Get(i).ToObject())->data());
                }
                result = d->save(tables);
            }
        } else if (value.IsObject()) {
            const Napi::Object object = value.ToObject();
            if (object.InstanceOf(RootWrap::ctor.Value())) {
                result = d->save(RootWrap::Unwrap(object)->data());
            } else if (object.InstanceOf(TableWrap::ctor.Value())) {
                result = d->save(TableWrap::Unwrap(object)->data());
            }
        }
        break;
    }
    case 2:
    {
        const Napi::Value value = info[0];
        if (value.IsArray()) {
            const Napi::Array array = value.As<Napi::Array>();
            const uint32_t length = array.Length();
            if (length == 0) {
                result = true;
                break;
            }
            const Napi::Value first = array.Get(uint32_t(0));
            if (!first.IsObject()) {
                break;
            }
            const Napi::Object object = first.ToObject();
            if (object.InstanceOf(SystemWrap::ctor.Value())) {
                Icd::SystemPtrArray systems;
                for (uint32_t i = 0; i < length; ++i) {
                    systems.push_back(SystemWrap::Unwrap(array.Get(i).ToObject())->data());
                }
                result = d->save(info[1].As<Napi::String>(), systems);
            } else if (object.InstanceOf(TableWrap::ctor.Value())) {
                Icd::TablePtrArray tables;
                for (uint32_t i = 0; i < length; ++i) {
                    tables.push_back(TableWrap::Unwrap(array.Get(i).ToObject())->data());
                }
                const std::string domain = info[1].As<Napi::String>();
                result = d->save(Icd::stringSection(domain, '/', 0, 0),
                                 Icd::stringSection(domain, '/', 1, 1),
                                 tables);
            } else if (object.InstanceOf(ItemWrap::ctor.Value())) {
                Icd::ItemPtrArray items;
                for (uint32_t i = 0; i < length; ++i) {
                    items.push_back(ItemWrap::fromObject(info.Env(), array.Get(i)));
                }
                const std::string domain = info[1].As<Napi::String>();
                result = d->save(Icd::stringSection(domain, '/', 0, 0),
                                 Icd::stringSection(domain, '/', 1, 1),
                                 Icd::stringSection(domain, '/', 2, 2),
                                 items);
            }
        } else if (value.IsObject()) {
            const Napi::Object object = value.ToObject();
            if (object.InstanceOf(VehicleWrap::ctor.Value())) {
                result = d->save(info[1].As<Napi::String>(), VehicleWrap::Unwrap(object)->data());
            } else if (object.InstanceOf(TableWrap::ctor.Value())) {
                result = d->save(info[1].As<Napi::String>(), TableWrap::Unwrap(object)->data());
            }
        }
        break;
    }
    case 3:
    {
        const Napi::Value value = info[0];
        if (value.IsArray()) {
            const Napi::Array array = value.As<Napi::Array>();
            const uint32_t length = array.Length();
            if (length == 0) {
                result = true;
                break;
            }
            const Napi::Value first = array.Get(uint32_t(0));
            if (!first.IsObject()) {
                break;
            }
            const Napi::Object object = first.ToObject();
            if (object.InstanceOf(TableWrap::ctor.Value())) {
                Icd::TablePtrArray tables;
                for (uint32_t i = 0; i < length; ++i) {
                    tables.push_back(TableWrap::Unwrap(array.Get(i).ToObject())->data());
                }
                result = d->save(info[1].As<Napi::String>(), info[2].As<Napi::String>(), tables);
            }
        } else if (value.IsObject()) {
            const Napi::Object object = value.ToObject();
            if (object.InstanceOf(SystemWrap::ctor.Value())) {
                result = d->save(info[1].As<Napi::String>(), info[2].As<Napi::String>(),
                        SystemWrap::Unwrap(object)->data());
            }
        }
        break;
    }
    case 4:
    {
        const Napi::Value value = info[0];
        switch (value.Type()) {
        case napi_string:
        {
            const Icd::ObjectPtr object = PROTOCORE_DOMAIN::ObjectWrap::fromObject(info.Env(), value);
            if (!object) {
                break;
            }
            result = d->save(value.ToString(), object, info[2].ToBoolean(), info[3].ToBoolean());
        }
        case napi_object:
        {
            const Napi::Object object = value.ToObject();
            if (object.InstanceOf(TableWrap::ctor.Value())) {
                result = d->save(info[1].As<Napi::String>(), info[2].ToString(),
                        info[3].ToString(), TableWrap::Unwrap(object)->data());
            }
            break;
        }
        default:
            if (value.IsArray()) {
                const Napi::Array array = value.As<Napi::Array>();
                const uint32_t length = array.Length();
                if (length == 0) {
                    result = true;
                    break;
                }
                const Napi::Value first = array.Get(uint32_t(0));
                if (!first.IsObject()) {
                    break;
                }
                const Napi::Object object = first.ToObject();
                if (object.InstanceOf(ItemWrap::ctor.Value())) {
                    Icd::ItemPtrArray items;
                    for (uint32_t i = 0; i < length; ++i) {
                        items.push_back(ItemWrap::fromObject(info.Env(), array.Get(i)));
                    }
                    result = d->save(info[1].ToString(), info[2].ToString(), info[3].ToString(), items);
                }
            }
            break;
        }
    }
    case 5:
    {
        const Napi::Value value = info[0];
        if (value.IsObject()) {
            const Napi::Object object = value.ToObject();
            if (object.InstanceOf(ItemWrap::ctor.Value())) {
                result = d->save(info[1].As<Napi::String>(), info[2].ToString(),
                        info[3].ToString(), info[4].ToString(), ItemWrap::fromObject(info.Env(), object));
            }
        }
        break;
    }
    default:
        break;
    }

    return Napi::Boolean::New(info.Env(), result);
}

NAPI_METHOD(ParserWrap, BeginModify) {
    return Napi::Boolean::New(info.Env(), d->beginModify());
}

NAPI_METHOD(ParserWrap, CommitModify) {
    return Napi::Boolean::New(info.Env(), d->commitModify());
}

NAPI_METHOD(ParserWrap, CancelModify) {
    return Napi::Boolean::New(info.Env(), d->cancelModify());
}

NAPI_METHOD(ParserWrap, EndModify) {
    return Napi::Boolean::New(info.Env(), d->endModify());
}

NAPI_GETTER(ParserWrap, IsBeginModify) {
    return Napi::Boolean::New(info.Env(), d->isBeginModify());
}

NAPI_METHOD(ParserWrap, SaveAs) {
    if (info.Length() != 2) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need two arguments!"));
    }
    return Napi::Boolean::New(info.Env(),
                              d->saveAs(TableWrap::Unwrap(info[0].As<Napi::Object>())->data(),
                              info[1].As<Napi::String>()));
}

NAPI_GETTER(ParserWrap, Message) {
    return Napi::String::New(info.Env(), d->message());
}

NAPI_SETTER(ParserWrap, Message) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
    }
    d->setMessage(info[0].As<Napi::String>());
}

NAPI_GETTER(ParserWrap, ProgressValue) {
    return Napi::Number::New(info.Env(), d->progressValue());
}

NAPI_SETTER(ParserWrap, ProgressValue) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
    }
    d->setProgressValue(info[0].As<Napi::Number>());
}

NAPI_GETTER(ParserWrap, CanceledSaveAs) {
    return Napi::Boolean::New(info.Env(), d->canceledSaveAs());
}

NAPI_VOID_METHOD(ParserWrap, CancelSaveAs) {
    if (info.Length() != 1) {
        napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need one argument!"));
    }
    d->cancelSaveAs(info[0].As<Napi::Boolean>());
}

NAPI_METHOD(ParserWrap, QueryTable) {
    if (info.Length() != 4) {
        return napi_throwjs(Napi::ArgumentError::New(info.Env(), "Need four arguments!"));
    }
    const Json::Value value = Icd::queryTable(info[0].As<Napi::String>(),
            info[1].As<Napi::String>(), info[2].As<Napi::String>(), info[3].As<Napi::String>());
    try {
        const std::string json = Json::FastWriter().write(value);
        return Napi::String::New(info.Env(), json);
    } catch (...) {
        return napi_throwjs(Napi::Error::New(info.Env(), "(Fast) Write json value failure!"));
    }

    return Napi::Boolean::New(info.Env(), false);
}

PROTOCORE_END
