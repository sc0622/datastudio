#include "icdArraydata.h"


class ICDArrayDataPrivate
{
    friend class ICDArrayData;
public:
    ICDArrayDataPrivate()
        : arrayType(GlobalDefine::ArrayI8)
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
    sprintf_s(szTmp, sizeof(szTmp), "%d", d->arrayType);

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
    case GlobalDefine::ArrayI8:
    case GlobalDefine::ArrayU8: return 1;
    case GlobalDefine::ArrayI16:
    case GlobalDefine::ArrayU16: return 2;
    case GlobalDefine::ArrayI32:
    case GlobalDefine::ArrayU32: return 4;
    case GlobalDefine::ArrayI64:
    case GlobalDefine::ArrayU64: return 8;
    case GlobalDefine::ArrayF32: return 4;
    case GlobalDefine::ArrayF64: return 8;
    default: return 0;
    }
}

std::string ICDArrayData::typeString() const
{
    std::string sType;
    switch (d->arrayType) {
    case GlobalDefine::ArrayI8: sType = "i8"; break;
    case GlobalDefine::ArrayU8: sType = "u8"; break;
    case GlobalDefine::ArrayI16: sType = "i16"; break;
    case GlobalDefine::ArrayU16: sType = "u16"; break;
    case GlobalDefine::ArrayI32: sType = "i32"; break;
    case GlobalDefine::ArrayU32: sType = "u32"; break;
    case GlobalDefine::ArrayI64: sType = "i64"; break;
    case GlobalDefine::ArrayU64: sType = "u64"; break;
    case GlobalDefine::ArrayF32: sType = "f32"; break;
    case GlobalDefine::ArrayF64: sType = "f64"; break;
    default: sType = "?"; break;
    }
    return ICDMetaData::typeString() + '#' + sType;
}
