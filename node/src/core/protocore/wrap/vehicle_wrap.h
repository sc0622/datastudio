#pragma once

#include "node_global.h"

namespace Icd { class Vehicle; }

PROTOCORE_BEGIN

class VehicleWrap : public node::ObjectWrap
{
public:
    VehicleWrap(Icd::Vehicle *data);

    static node::Object New(v8::Isolate *isolate, Icd::Vehicle *data);

    static node::Object NewInstance(Icd::Vehicle *data);

private:

private:
    NODE_DECL2(Vehicle)
};

PROTOCORE_END
