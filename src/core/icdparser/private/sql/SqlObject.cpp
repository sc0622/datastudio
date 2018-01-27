#include "precomp.h"
#include "SqlObject.h"

namespace Icd {

SqlObject::SqlObject(const Json::Value &config)
{
    Q_UNUSED(config);
}

SqlObject::~SqlObject()
{

}

SqlObjectPtr SqlObject::create(const Json::Value &config)
{
    Q_UNUSED(config);
    return SqlObjectPtr();
}

}
