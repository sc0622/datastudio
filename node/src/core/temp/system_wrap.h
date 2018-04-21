#pragma once

#include "node_global.h"
#include "icdcore/icd_object.h"

PROTOCORE_BEGIN

class ObjectWrap : public node::ObjectWrap
{
private:
    ObjectWrap(Icd::Object *data);

    NODE_METHOD_DECL(Parent)
    NODE_METHOD_DECL(SetParent)

    NODE_METHOD_DECL(ObjectType)
    NODE_METHOD_DECL(Id)
    NODE_METHOD_DECL(Domain)
    NODE_METHOD_DECL(Name)
    NODE_METHOD_DECL(Mark)
    NODE_METHOD_DECL(Desc)

    NODE_METHOD_DECL(SetId)
    NODE_METHOD_DECL(SetDomain)
    NODE_METHOD_DECL(SetName)
    NODE_METHOD_DECL(SetMark)
    NODE_METHOD_DECL(SetDesc)

    NODE_METHOD_DECL(IsPrivateMark)
    NODE_METHOD_DECL(ChildCount)
    NODE_METHOD_DECL(ObjectTypeString)
    NODE_METHOD_DECL(ResetData)
    NODE_METHOD_DECL(ClearData)
    NODE_METHOD_DECL(Clone)
    NODE_METHOD_DECL(DomainOfType)

private:
    NODE_DECL2(Object)
};

PROTOCORE_END
