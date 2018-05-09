#include "precomp.h"
#include "parser.h"
#include "parser_wrap.h"

PROTOCORE_BEGIN

void parser_init(Napi::Env env, Napi::Object exports)
{
    ParserWrap::Initialize(env, exports);
}

PROTOCORE_END
