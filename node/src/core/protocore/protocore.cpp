#include "node_global.h"
#include "wrap/comm/comm.h"
#include "wrap/core/core.h"
#include "wrap/parser/parser.h"
#include "wrap/worker/worker.h"

PROTOCORE_BEGIN

Napi::Object Initialize(Napi::Env env, Napi::Object exports)
{
    comm_init(env, exports);
    core_init(env, exports);
    parser_init(env, exports);
    worker_init(env, exports);
    return exports;
}

NODE_API_MODULE(protocore, Initialize)

PROTOCORE_END
