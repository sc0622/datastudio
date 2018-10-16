#include "precomp.h"
#include "SqlObject.h"

namespace Icd {

SqlObject::SqlObject(const Json::Value &config)
{
    (void)(config);
}

SqlObject::~SqlObject()
{

}

SqlObjectPtr SqlObject::create(const Json::Value &config)
{
    (void)(config);
    return SqlObjectPtr();
}

}
