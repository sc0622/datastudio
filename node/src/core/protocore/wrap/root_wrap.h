#pragma once

#include "node_global.h"

namespace Icd { class Root; }

PROTOCORE_BEGIN

class RootWrap : public node::ObjectWrap
{
public:
    RootWrap(Icd::Root *data);

private:
    NODE_METHOD_DECL(AllVehicle)

private:
    NODE_DECL2(Root)
};

PROTOCORE_END
