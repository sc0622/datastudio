#ifndef SQLOBJECT_H
#define SQLOBJECT_H

#include "../../icdparser_global.h"

namespace Json { class Value; }

namespace Icd {

class SqlObject;
typedef JHandlePtr<SqlObject> SqlObjectPtr;

class SqlObject
{
public:
    enum SqlType {
        SqlTypeInvalid = -1,

        SqlTypeMySql,
        SqlTypeOracle,
        SqlTyeSqlite,
        SqlTypeSqlServer,

        SqlTypeCount
    };

    explicit SqlObject(const Json::Value &config);
    virtual ~SqlObject();

    static SqlObjectPtr create(const Json::Value &config);

private:
};

}

#endif // SQLOBJECT_H
