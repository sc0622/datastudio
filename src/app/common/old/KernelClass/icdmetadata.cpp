#include "icdmetadata.h"

class ICDMetaDataPrivate
{
    friend class ICDMetaData;
public:
    ICDMetaDataPrivate()
        : q_index(0)
        , q_type(-1)
        , q_serial(1)
        , q_length(0)
        , q_subType(-1)
    {}

private:
    int         q_index;    // 字号
    int         q_type;     // 规则具体类型
    int         q_serial;   // 标志号
    int         q_length;   // 数据长度
    int         q_subType;  //
};

ICDMetaData::ICDMetaData(const stTableRules &rules)
    : ICDElement(GlobalDefine::ntRule)
    , d(new ICDMetaDataPrivate())
    , q_cType(IcdDefine::icdMeta)
{
    setName(rules.sName);
    setIndex(rules.nCode);
    setType(rules.uType);
    setSerial(rules.nSerial);
    setRemark(rules.sRemark);
    setSubType(rules.subType);
}

ICDMetaData::ICDMetaData(const ICDMetaData &rhs)
    : ICDElement(rhs)
    , d(new ICDMetaDataPrivate())
{
    *this = rhs;
}

ICDMetaData & ICDMetaData::operator=(const ICDMetaData &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    *(ICDElement *)this = rhs;

    *d = *rhs.d;
    q_cType = rhs.q_cType;

    return *this;
}

ICDMetaData::~ICDMetaData()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

int ICDMetaData::length() const
{
    return d->q_length;
}

void ICDMetaData::setLength(int length)
{
    d->q_length = length;
}

int ICDMetaData::byteLength() const
{
    return d->q_length;
}

double ICDMetaData::realLength() const
{
    return (double)d->q_length;
}

std::string ICDMetaData::lengthOfByte() const
{
    char szTmp[128] = { 0 };

    sprintf_s(szTmp, sizeof(szTmp), "%d", length());

    return std::string(szTmp);
}

ICDMetaData::smtMeta ICDMetaData::clone() const
{
    ICDMetaData::smtMeta meta(new ICDMetaData(*this));

    return meta;
}

void ICDMetaData::setIndex(int index)
{
    d->q_index = index;
}

int ICDMetaData::index() const
{
    return d->q_index;
}

IcdDefine::IcdType ICDMetaData::metaType() const
 {
    return q_cType;
 }

void ICDMetaData::setType(int type)
{
    d->q_type = type;
}

int ICDMetaData::type() const
{
    return d->q_type;
}

void ICDMetaData::setSerial(int serial)
{
    //
    char szTmp[32] = {0};
    sprintf_s(szTmp, sizeof(szTmp), "%d", serial);

    setID(std::string(szTmp));

    d->q_serial = serial;
}

int ICDMetaData::serial() const
{
    return d->q_serial;
}

std::string ICDMetaData::stringSerial() const
{
    return id();
}

void ICDMetaData::setRemark(const std::string &remark)
{
    setDescribe(remark);
}

std::string ICDMetaData::remark() const
{
    return describe();
}

int ICDMetaData::subType() const
{
    return d->q_subType;
}

void ICDMetaData::setSubType(int subType)
{
    d->q_subType = subType;
}

std::string ICDMetaData::typeString() const
{
    switch (d->q_type) {
    case GlobalDefine::dtHead: return "head";
    case GlobalDefine::dtCounter: return "counter";
    case GlobalDefine::dtCheck: return "check";
    case GlobalDefine::dtFrameCode: return "framecode";
    case GlobalDefine::dtNumeric: return "numeric";
    case GlobalDefine::dtArray: return "array";
    case GlobalDefine::dtBitMap: return "bitmap";
    case GlobalDefine::dtBitValue: return "bitvalue";
    case GlobalDefine::dtComplex: return "complex";
    case GlobalDefine::dtDiscern: return "frame";
    case GlobalDefine::dtBuffer: return "buffer";
    default: return "?";
    }
}
