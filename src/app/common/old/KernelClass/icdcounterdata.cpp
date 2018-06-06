#include "icdcounterdata.h"


class ICDCounterDataPrivate
{
    friend class ICDCounterData;
public:
    ICDCounterDataPrivate()
        : counterType(GlobalDefine::counterU8)
    {}

private:
    int     counterType;    // ֡��������
};

ICDCounterData::ICDCounterData(const stTableRules &rule)
    : ICDCommonData(rule)
    , d(new ICDCounterDataPrivate())
{
    q_cType = IcdDefine::icdCommon;

    setCounterType(atoi(rule.sDefault.c_str()));
}

ICDCounterData::ICDCounterData(const ICDCounterData &rhs)
    : ICDCommonData(rhs)
    , d(new ICDCounterDataPrivate())
{
    *this = rhs;
}

ICDCounterData & ICDCounterData::operator=(const ICDCounterData &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    *(ICDCommonData*)this = rhs;
    *d = *rhs.d;

    return *this;
}

ICDCounterData::~ICDCounterData()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

ICDMetaData::smtMeta ICDCounterData::clone() const
{
    ICDCounterData::smtCounter common(new ICDCounterData(*this));

    return common;
}

void ICDCounterData::setCounterType(int counterType)
{
    d->counterType = counterType;
    switch (counterType) {
    case GlobalDefine::counterU8:
        setLength(1);
        break;
    case GlobalDefine::counterU16:
        setLength(2);
        break;
    case GlobalDefine::counterU32:
        setLength(4);
        break;
    default:break;
    }
}

int ICDCounterData::counterType() const
{
    return d->counterType;
}

std::string ICDCounterData::stringType() const
{
    char szTmp[128] = {0};
    sprintf_s(szTmp, sizeof(szTmp), "%d", counterType());

    return std::string(szTmp);
}
