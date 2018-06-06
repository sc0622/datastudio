#include "icdframecodedata.h"


class ICDFrameCodeDataPrivate
{
    friend class ICDFrameCodeData;
public:
    ICDFrameCodeDataPrivate()
        : q_data(0), frameType(GlobalDefine::frame8)
    {}

private:
    int     frameType;  // ֡������
    ICDComplexData::smtComplex  q_data;
};

ICDFrameCodeData::ICDFrameCodeData(const stTableRules &rule)
    : ICDCommonData(rule)
    , d(new ICDFrameCodeDataPrivate())
{
    q_cType = IcdDefine::icdCommon;

    setFrameType((int)rule.dScale);
}

ICDFrameCodeData::ICDFrameCodeData(const ICDFrameCodeData &rhs)
: ICDCommonData(rhs)
    , d(new ICDFrameCodeDataPrivate())
{
    *this = rhs;
}

ICDFrameCodeData & ICDFrameCodeData::operator=(const ICDFrameCodeData &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    *(ICDCommonData*)this = rhs;
    *d = *rhs.d;

    return *this;
}

ICDFrameCodeData::~ICDFrameCodeData()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

ICDMetaData::smtMeta ICDFrameCodeData::clone() const
{
    ICDFrameCodeData::smtFrameCode common(new ICDFrameCodeData(*this));

    return common;
}

void ICDFrameCodeData::setFrameType(int frameType)
{
    d->frameType = frameType;
    switch (frameType) {
    case GlobalDefine::frame8:
        setLength(1);
        break;
    case GlobalDefine::frame16:
        setLength(2);
        break;
    case GlobalDefine::frame32:
        setLength(4);
        break;
    case GlobalDefine::frame64:
        setLength(8);
        break;
    default:
        setLength(1);
        d->frameType = GlobalDefine::frame8;
        break;
    }
}

int ICDFrameCodeData::frameType() const
{
    return d->frameType;
}

// ��֡����
void ICDFrameCodeData::bindData(ICDComplexData::smtComplex data)
{
    d->q_data = data;
    if (data) {
        setDefaultStr(bindingStringSerial());
    } else {
        setDefaultStr(std::string("-1"));
    }
}

// ��ȡ��֡����
ICDComplexData::smtComplex ICDFrameCodeData::data() const
{
    return d->q_data;
}

// ��ȡ�����ݱ�־
int ICDFrameCodeData::bindingSerial() const
{
    if (!d->q_data) {
        return -1;
    }

    return d->q_data->serial();
}

// ��ȡ�����ݱ�־
std::string ICDFrameCodeData::bindingStringSerial() const
{
    if (!d->q_data) {
        return std::string("-1");
    }

    return d->q_data->stringSerial();
}
