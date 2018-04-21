#include "precomp.h"
#include "object_wrap.h"

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

void ObjectWrap::New(const node::Args &args)
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

PROTOCORE_END
