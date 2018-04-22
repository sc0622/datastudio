#include "precomp.h"
#include "object_wrap.h"
#include "icdcore/icd_object.h"

PROTOCORE_BEGIN

NODE_IMPL2(Object)

ObjectWrap::ObjectWrap(Icd::Object *data)
    : d_(data)
{

}

void ObjectWrap::Parent(node::CArgsRef args)
{
    NODE_D(Object, args);
    ObjectWrap *object = new ObjectWrap(d->parent());
    object->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
}

void ObjectWrap::SetParent(node::CArgsRef args)
{
    if (!args[0]->IsObject()) {
        return;
    }

    NODE_D(Object, args);
    ObjectWrap *object = Unwrap<ObjectWrap>(args[0]->ToObject());
    if (!object) {
        return;
    }
    d->setParent(object->wrapped()->parent());
}

void ObjectWrap::ObjectType(node::CArgsRef args)
{
    NODE_D(Object, args);
    NODE_SET_RETURN(Int32, int(d->objectType()));
}

void ObjectWrap::Id(node::CArgsRef args)
{
    NODE_D(Object, args);
    args.GetReturnValue().Set(node::fromStdString(isolate, d->id()));
}

void ObjectWrap::Domain(node::CArgsRef args)
{
    NODE_D(Object, args);
    args.GetReturnValue().Set(node::fromStdString(isolate, d->domain()));
}

void ObjectWrap::Name(node::CArgsRef args)
{
    NODE_D(Object, args);
    args.GetReturnValue().Set(node::fromStdString(isolate, d->name()));
}

void ObjectWrap::Mark(node::CArgsRef args)
{
    NODE_D(Object, args);
    args.GetReturnValue().Set(node::fromStdString(isolate, d->mark()));
}

void ObjectWrap::Desc(node::CArgsRef args)
{
    NODE_D(Object, args);
    args.GetReturnValue().Set(node::fromStdString(isolate, d->desc()));
}

void ObjectWrap::SetId(node::CArgsRef args)
{
    NODE_D(Object, args);
    d->setId(node::toStdString(args[0]));
}

void ObjectWrap::SetDomain(node::CArgsRef args)
{
    NODE_D(Object, args);
    d->setDomain(node::toStdString(args[0]));
}

void ObjectWrap::SetName(node::CArgsRef args)
{
    NODE_D(Object, args);
    d->setName(node::toStdString(args[0]));
}

void ObjectWrap::SetMark(node::CArgsRef args)
{
    NODE_D(Object, args);
    d->setMark(node::toStdString(args[0]));
}

void ObjectWrap::SetDesc(node::CArgsRef args)
{
    NODE_D(Object, args);
    d->setDesc(node::toStdString(args[0]));
}

void ObjectWrap::IsPrivateMark(node::CArgsRef args)
{
    NODE_D(Object, args);
    args.GetReturnValue().Set(d->isPrivateMark());
}

void ObjectWrap::ChildCount(node::CArgsRef args)
{
    NODE_D(Object, args);
    args.GetReturnValue().Set(d->childCount());
}

void ObjectWrap::ObjectTypeString(node::CArgsRef args)
{
    NODE_D(Object, args);
    args.GetReturnValue().Set(node::fromStdString(isolate, d->objectTypeString()));
}

void ObjectWrap::ResetData(node::CArgsRef args)
{
    NODE_D(Object, args);
    d->resetData();
}

void ObjectWrap::ClearData(node::CArgsRef args)
{
    NODE_D(Object, args);
    d->clearData();
}

void ObjectWrap::Clone(node::CArgsRef args)
{
    NODE_D(Object, args);
    ObjectWrap *object = new ObjectWrap(d->clone());
    object->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
}

void ObjectWrap::DomainOfType(node::CArgsRef args)
{
    NODE_D(Object, args);
    if (args[0]->IsUndefined()) {
        return;
    }
    args.GetReturnValue().Set(node::fromStdString(isolate, d->domainOfType(args[0]->Int32Value())));
}

void ObjectWrap::New(node::CArgsRef args)
{
    if (args.IsConstructCall()) {
        if (args[0]->IsObject()) {
            ObjectWrap *object = Unwrap<ObjectWrap>(args[0]->ToObject());
            Icd::Object *data = object->wrapped();
            if (data) {
                ObjectWrap *newObject = new ObjectWrap(data);
                newObject->Wrap(args.This());
                args.GetReturnValue().Set(args.This());
                return;
            }
        }
    } else {
        v8::Local<v8::Value> argv[1] = { args[0] };
        NODE_NEW_INST(argv);
    }
}

NODE_METHOD_MAP_BEGIN(ObjectWrap, Object)
NODE_METHOD_MAP(objectType, ObjectType)
NODE_METHOD_MAP(parent, Parent)
NODE_METHOD_MAP(setParent, SetParent)
NODE_METHOD_MAP(id, Id)
NODE_METHOD_MAP(domain, Domain)
NODE_METHOD_MAP(name, Name)
NODE_METHOD_MAP(mark, Mark)
NODE_METHOD_MAP(desc, Desc)
NODE_METHOD_MAP(setId, SetId)
NODE_METHOD_MAP(setDomain, SetDomain)
NODE_METHOD_MAP(setName, SetName)
NODE_METHOD_MAP(setMark, SetMark)
NODE_METHOD_MAP(setDesc, SetDesc)
NODE_METHOD_MAP(isPrivateMark, IsPrivateMark)
NODE_METHOD_MAP(childCount, ChildCount)
NODE_METHOD_MAP(objectTypeString, ObjectTypeString)
NODE_METHOD_MAP(resetData, ResetData)
NODE_METHOD_MAP(clearData, ClearData)
NODE_METHOD_MAP(clone, Clone)
NODE_METHOD_MAP(domainOfType, DomainOfType)
NODE_METHOD_MAP_END()

// MyObject

napi_ref MyObject::constructor;

MyObject::MyObject(double value)
    : value_(value), env_(nullptr), wrapper_(nullptr) {}

MyObject::~MyObject() { napi_delete_reference(env_, wrapper_); }

void MyObject::Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
    reinterpret_cast<MyObject*>(nativeObject)->~MyObject();
}

napi_value MyObject::Init(napi_env env, napi_value exports) {
    napi_status status;
    napi_property_descriptor properties[] = {
        NAPI_DESCR_PROPERTY(value, GetValue, SetValue),
        NAPI_DESCR_METHOD(plusOne, PlusOne),
        NAPI_DESCR_METHOD(multiply, Multiply),
    };

    napi_value cons;
    status = napi_define_class(env, "MyObject", NAPI_AUTO_LENGTH, New, nullptr,
                               _countof(properties), properties, &cons);
    assert(status == napi_ok);

    status = napi_create_reference(env, cons, 1, &constructor);
    assert(status == napi_ok);

    status = napi_set_named_property(env, exports, "MyObject", cons);
    assert(status == napi_ok);
    return exports;
}

napi_value MyObject::New(napi_env env, napi_callback_info info) {
    napi_status status;

    napi_value target;
    status = napi_get_new_target(env, info, &target);
    assert(status == napi_ok);
    bool is_constructor = target != nullptr;

    if (is_constructor) {
        // Invoked as constructor: `new MyObject(...)`
        size_t argc = 1;
        napi_value args[1];
        napi_value jsthis;
        status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
        assert(status == napi_ok);

        double value = 0;

        napi_valuetype valuetype;
        status = napi_typeof(env, args[0], &valuetype);
        assert(status == napi_ok);

        if (valuetype != napi_undefined) {
            status = napi_get_value_double(env, args[0], &value);
            assert(status == napi_ok);
        }

        MyObject* obj = new MyObject(value);

        obj->env_ = env;
        status = napi_wrap(env,
                           jsthis,
                           reinterpret_cast<void*>(obj),
                           MyObject::Destructor,
                           nullptr,  // finalize_hint
                           &obj->wrapper_);
        assert(status == napi_ok);

        return jsthis;
    } else {
        // Invoked as plain function `MyObject(...)`, turn into construct call.
        size_t argc_ = 1;
        napi_value args[1];
        status = napi_get_cb_info(env, info, &argc_, args, nullptr, nullptr);
        assert(status == napi_ok);

        const size_t argc = 1;
        napi_value argv[argc] = {args[0]};

        napi_value cons;
        status = napi_get_reference_value(env, constructor, &cons);
        assert(status == napi_ok);

        napi_value instance;
        status = napi_new_instance(env, cons, argc, argv, &instance);
        assert(status == napi_ok);

        return instance;
    }
}

napi_value MyObject::GetValue(napi_env env, napi_callback_info info) {
    napi_status status;

    napi_value jsthis;
    status = napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr);
    assert(status == napi_ok);

    MyObject* obj;
    status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
    assert(status == napi_ok);

    napi_value num;
    status = napi_create_double(env, obj->value_, &num);
    assert(status == napi_ok);

    return num;
}

napi_value MyObject::SetValue(napi_env env, napi_callback_info info) {
    napi_status status;

    size_t argc = 1;
    napi_value value;
    napi_value jsthis;
    status = napi_get_cb_info(env, info, &argc, &value, &jsthis, nullptr);
    assert(status == napi_ok);


    MyObject* obj;
    status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
    assert(status == napi_ok);

    status = napi_get_value_double(env, value, &obj->value_);
    assert(status == napi_ok);

    return nullptr;
}

napi_value MyObject::PlusOne(napi_env env, napi_callback_info info) {
    napi_status status;

    napi_value jsthis;
    status = napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr);
    assert(status == napi_ok);

    MyObject* obj;
    status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
    assert(status == napi_ok);

    obj->value_ += 1;

    napi_value num;
    status = napi_create_double(env, obj->value_, &num);
    assert(status == napi_ok);

    return num;
}

napi_value MyObject::Multiply(napi_env env, napi_callback_info info) {
    napi_status status;

    size_t argc = 1;
    napi_value args[1];
    napi_value jsthis;
    status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
    assert(status == napi_ok);

    napi_valuetype valuetype;
    status = napi_typeof(env, args[0], &valuetype);
    assert(status == napi_ok);

    double multiple = 1;
    if (valuetype != napi_undefined) {
        status = napi_get_value_double(env, args[0], &multiple);
        assert(status == napi_ok);
    }

    MyObject* obj;
    status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
    assert(status == napi_ok);

    napi_value cons;
    status = napi_get_reference_value(env, constructor, &cons);
    assert(status == napi_ok);

    const int kArgCount = 1;
    napi_value argv[kArgCount];
    status = napi_create_double(env, obj->value_ * multiple, argv);
    assert(status == napi_ok);

    napi_value instance;
    status = napi_new_instance(env, cons, kArgCount, argv, &instance);
    assert(status == napi_ok);

    return instance;
}

PROTOCORE_END
