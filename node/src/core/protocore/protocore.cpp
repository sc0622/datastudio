#include "node_global.h"
#include "wrap/core/core.h"
#include "wrap/parser/parser.h"

PROTOCORE_BEGIN

Napi::Object Initialize(Napi::Env env, Napi::Object exports)
{
    core_init(env, exports);
    parser_init(env, exports);
    return exports;
}

NODE_API_MODULE(protocore, Initialize)

PROTOCORE_END
