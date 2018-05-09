#ifndef PARSER_WRAP_H
#define PARSER_WRAP_H

#include "node_global.h"

namespace Icd {
class Parser;
typedef std::shared_ptr<Parser> ParserPtr;
}

PROTOCORE_BEGIN

class ParserWrap : public Napi::ObjectWrap<ParserWrap>
{
public:
    static Napi::FunctionReference ctor;
    static void Initialize(Napi::Env env, Napi::Object exports);
    ParserWrap(const Napi::CallbackInfo &info);

    Icd::ParserPtr data() const { return d; }

    static NAPI_METHOD_DECL(Create);
    NAPI_METHOD_DECL(Parse);
    NAPI_METHOD_DECL(ParseRoot);
    NAPI_METHOD_DECL(ParseVehicle);
    NAPI_METHOD_DECL(ParseSystem);
    NAPI_METHOD_DECL(ParseTable);
    NAPI_METHOD_DECL(ParseItem);
    NAPI_METHOD_DECL(Save);
    NAPI_METHOD_DECL(BeginModify);
    NAPI_METHOD_DECL(CommitModify);
    NAPI_METHOD_DECL(CancelModify);
    NAPI_METHOD_DECL(EndModify);
    NAPI_GETTER_DECL(IsBeginModify);
    NAPI_METHOD_DECL(SaveAs);
    NAPI_PROPERTY_DECL(Message);
    NAPI_PROPERTY_DECL(ProgressValue);
    NAPI_GETTER_DECL(CanceledSaveAs);
    NAPI_VOID_METHOD_DECL(CancelSaveAs);

    static NAPI_METHOD_DECL(QueryTable);

private:
    Icd::ParserPtr d;
};

PROTOCORE_END

#endif // PARSER_WRAP_H
