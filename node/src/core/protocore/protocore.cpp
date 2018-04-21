
#include "node_global.h"
#include "wrap/object_wrap.h"
#include "wrap/root_wrap.h"

PROTOCORE_BEGIN
/*
napi_value Initialize(napi_env env, napi_value exports)
{


    return exports;
}
*/
void Initialize(node::Object exports)
{
    ObjectWrap::Initialize(exports);
    RootWrap::Initialize(exports);
}

NODE_MODULE(protocore, Initialize)

PROTOCORE_END
