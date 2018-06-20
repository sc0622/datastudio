#include "icdnumericdata.h"

class ICDNumericDataPrivate
{
    friend class ICDNumericData;
public:
    ICDNumericDataPrivate()
    {}

private:
};

ICDNumericData::ICDNumericData(const stTableRules &rule)
    : ICDCommonData(rule)
    , d(new ICDNumericDataPrivate())
{
    q_cType = IcdDefine::icdCommon;
    setNumericType(rule.subType);
}

ICDNumericData::ICDNumericData(const ICDNumericData &rhs)
    : ICDCommonData(rhs)
    , d(new ICDNumericDataPrivate())
{
    *this = rhs;
}

ICDNumericData & ICDNumericData::operator=(const ICDNumericData &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    *(ICDCommonData*)this = rhs;
    *d = *rhs.d;

    return *this;
}

ICDNumericData::~ICDNumericData()
{
    delete d;
}

ICDMetaData::smtMeta ICDNumericData::clone() const
{
    ICDNumericData::smtNumeric common(new ICDNumericData(*this));
    return common;
}

void ICDNumericData::setNumericType(int numericType)
{
    setSubType(numericType);
    setLength(typeSize());
}

int ICDNumericData::numericType() const
{
    return subType();
}

std::string ICDNumericData::stringType() const
{
    char szTmp[128] = {0};
    sprintf_s(szTmp, sizeof(szTmp), "%d", numericType());

    return std::string(szTmp);
}

int ICDNumericData::typeSize() const
{
    switch (numericType()) {
    case GlobalDefine::NumericI8:
    case GlobalDefine::NumericU8: return 1;
    case GlobalDefine::NumericI16:
    case GlobalDefine::NumericU16: return 2;
    case GlobalDefine::NumericI32:
    case GlobalDefine::NumericU32: return 4;
    case GlobalDefine::NumericI64:
    case GlobalDefine::NumericU64: return 8;
    case GlobalDefine::NumericF32: return 4;
    case GlobalDefine::NumericF64: return 8;
    default: return 0;
    }
}

std::string ICDNumericData::typeString() const
{
    std::string sType;
    switch (numericType()) {
    case GlobalDefine::NumericI8: sType = "i8"; break;
    case GlobalDefine::NumericU8: sType = "u8"; break;
    case GlobalDefine::NumericI16: sType = "i16"; break;
    case GlobalDefine::NumericU16: sType = "u16"; break;
    case GlobalDefine::NumericI32: sType = "i32"; break;
    case GlobalDefine::NumericU32: sType = "u32"; break;
    case GlobalDefine::NumericI64: sType = "i64"; break;
    case GlobalDefine::NumericU64: sType = "u64"; break;
    case GlobalDefine::NumericF32: sType = "f32"; break;
    case GlobalDefine::NumericF64: sType = "f64"; break;
    default: sType = "?"; break;
    }
    return /*ICDMetaData::typeString() + '#' + */sType;
}
