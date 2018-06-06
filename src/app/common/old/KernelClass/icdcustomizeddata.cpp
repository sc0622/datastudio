#include "icdcustomizeddata.h"


class ICDCustomizedDataPrivate
{
    friend class ICDCustomizedData;
public:
    ICDCustomizedDataPrivate()
    {}

private:
};

ICDCustomizedData::ICDCustomizedData(const stTableRules &rule)
    : ICDCommonData(rule)
    , d(new ICDCustomizedDataPrivate())
{
    q_cType = IcdDefine::icdCustom;
    if ((int)rule.dOffset < 1) {
        setLength(1);
    } else {
        setLength((int)rule.dOffset);
    }
}

ICDCustomizedData::ICDCustomizedData(const ICDCustomizedData &rhs)
    : ICDCommonData(rhs)
    , d(new ICDCustomizedDataPrivate())
{
    *this = rhs;
}

ICDCustomizedData & ICDCustomizedData::operator=(const ICDCustomizedData &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    *(ICDCommonData*)this = rhs;
    *d = *rhs.d;

    return *this;
}

ICDCustomizedData::~ICDCustomizedData()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

ICDMetaData::smtMeta ICDCustomizedData::clone() const
{
    ICDCustomizedData::smtCustom custome(new ICDCustomizedData(*this));

    return custome;
}


void ICDCustomizedData::setOffset(double offset)
{
    ICDCommonData::setOffset(offset);

    setLength((int)offset);
}
