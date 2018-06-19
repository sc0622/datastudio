#include "icdArraydata.h"


class ICDArrayDataPrivate
{
    friend class ICDArrayData;
public:
    ICDArrayDataPrivate()
        : arrayType(GlobalDefine::Int8Array)
        , count(1)
    {

    }

private:
    int arrayType;      // 数组类型
    int count;          // 数组长度
};

ICDArrayData::ICDArrayData(const stTableRules &rule)
    : ICDCommonData(rule)
    , d(new ICDArrayDataPrivate())
{
    q_cType = IcdDefine::icdCommon;
    setArrayType(atoi(rule.sDefault.c_str()));
    setCount(rule.uLength / typeSize());
}

ICDArrayData::ICDArrayData(const ICDArrayData &rhs)
    : ICDCommonData(rhs)
    , d(new ICDArrayDataPrivate())
{
    *this = rhs;
}

ICDArrayData & ICDArrayData::operator=(const ICDArrayData &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    *(ICDCommonData*)this = rhs;
    *d = *rhs.d;

    return *this;
}

ICDArrayData::~ICDArrayData()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

ICDMetaData::smtMeta ICDArrayData::clone() const
{
    ICDArrayData::smtArray common(new ICDArrayData(*this));
    return common;
}

void ICDArrayData::setArrayType(int arrayType)
{
    d->arrayType = arrayType;
    setLength(d->count * typeSize());
}

int ICDArrayData::arrayType() const
{
    return d->arrayType;
}

std::string ICDArrayData::stringType() const
{
    char szTmp[128] = {0};
    sprintf_s(szTmp, sizeof(szTmp), "%d", arrayType());

    return std::string(szTmp);
}

int ICDArrayData::count() const
{
    return d->count;
}

void ICDArrayData::setCount(int count)
{
    if (count != d->count) {
        d->count = count;
        setLength(d->count * typeSize());
    }
}

int ICDArrayData::typeSize() const
{
    switch (d->arrayType) {
    case GlobalDefine::Int8Array:
    case GlobalDefine::UInt8Array: return 1;
    case GlobalDefine::Int16Array:
    case GlobalDefine::UInt16Array: return 2;
    case GlobalDefine::Int32Array:
    case GlobalDefine::UInt32Array: return 4;
    case GlobalDefine::Int64Array:
    case GlobalDefine::UInt64Array: return 8;
    case GlobalDefine::Float32Array: return 4;
    case GlobalDefine::Float64Array: return 8;
    default: return 0;
    }
}
