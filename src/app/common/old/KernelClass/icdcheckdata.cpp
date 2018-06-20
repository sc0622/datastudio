#include "icdcheckdata.h"


class ICDCheckDataPrivate
{
    friend class ICDCheckData;
public:
    ICDCheckDataPrivate()
        : checkType(-1)
        , start(0)
        , end(0)
    {}

private:
    int     checkType;  // 校验类型
    int     start;      // 校验起始位
    int     end;        // 校验终止位
};

ICDCheckData::ICDCheckData(const stTableRules &rule)
    : ICDCommonData(rule)
    , d(new ICDCheckDataPrivate())
{
    q_cType = IcdDefine::icdCommon;

    setCheckType(atoi(rule.sDefault.c_str()));
    setStart(0);
    setEnd(0);
    if (!rule.sRange.empty()) {
        std::string::size_type index(std::string::npos);
        std::string::size_type offset = strlen("~");

        if (std::string::npos != (index = rule.sRange.find("~"))) {
            setStart(atoi(rule.sRange.substr(0, index).c_str()));
            setEnd(atoi(rule.sRange.substr(index + offset).c_str()));
        }
    }
}

ICDCheckData::ICDCheckData(const ICDCheckData &rhs)
: ICDCommonData(rhs)
    , d(new ICDCheckDataPrivate())
{
    *this = rhs;
}

ICDCheckData & ICDCheckData::operator=(const ICDCheckData &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    *(ICDCommonData*)this = rhs;
    *d = *rhs.d;

    return *this;
}

ICDCheckData::~ICDCheckData()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

ICDMetaData::smtMeta ICDCheckData::clone() const
{
    ICDCheckData::smtCheck common(new ICDCheckData(*this));

    return common;
}

void ICDCheckData::setCheckType(int checkType)
{
    d->checkType = checkType;
    switch (checkType)
    {
    case GlobalDefine::ct8And:      // 8位和校验
    case GlobalDefine::ctCRC8:      // CRC8校验
    case GlobalDefine::ctXor8:      // 8位异或和校验
        setLength(1);
        break;
    case GlobalDefine::ct16And:     // 16位和校验
    case GlobalDefine::ctCRC16:     // CRC16校验
    case GlobalDefine::ctXor16:     // 16位异或和校验
        setLength(2);
        break;
    default:
        setLength(1);
        break;
    }
}

int ICDCheckData::checkType() const
{
    return d->checkType;
}

std::string ICDCheckData::stringType() const
{
    char szTmp[128] = {0};
    sprintf_s(szTmp, sizeof(szTmp), "%d", checkType());

    return std::string(szTmp);
}

void ICDCheckData::setStart(int start)
{
    d->start = start;
}

int ICDCheckData::start() const
{
    return d->start;
}

void ICDCheckData::setEnd(int end)
{
    d->end = end;
}

int ICDCheckData::end() const
{
    return d->end;
}

std::string ICDCheckData::checkRange() const
{
    char szTmp[128] = {0};
    sprintf_s(szTmp, sizeof(szTmp), "%d~%d", start(), end());

    return std::string(szTmp);
}
