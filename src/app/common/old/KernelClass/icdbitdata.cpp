#include "icdbitdata.h"
#include <vector>


class ICDBitDataPrivate
{
    friend class ICDBitData;
public:
    ICDBitDataPrivate() : q_start(0)
    {}

private:
    unsigned short  q_start;
};

ICDBitData::ICDBitData(const stTableRules &rule)
    : ICDCommonData(rule)
    , d(new ICDBitDataPrivate())
{
    q_cType = IcdDefine::icdBit;
    // ��ʼ������
    // rule.sRange��ʽ�����ݷ�Χ#����ֵ��Χ
    std::string range = rule.sRange;
    std::string::size_type pos = range.find("#");
    if (std::string::npos != pos) {
        range = range.substr(pos + strlen("#"));
        if (std::string::npos != (pos = range.find("~"))) {
            std::string sValue = range.substr(0, pos);
            setStart(atoi(sValue.c_str()));
            sValue = range.substr(pos + strlen("~"));
            if (sValue.empty()) {
                setLength(1);
            } else {
                setLength(atoi(sValue.c_str()) - start() + 1);
            }
        } else {
            setStart(0);
            setLength(1);
        }
    } else {
        setStart(0);
        setLength(1);
    }
    setRule(rule.sRule);
}

ICDBitData::ICDBitData(const ICDBitData& rhs)
    : ICDCommonData(rhs)
    , d(new ICDBitDataPrivate())
{
    *this = rhs;
}

ICDBitData & ICDBitData::operator=(const ICDBitData &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    *(ICDCommonData*)this = rhs;
    *d = *rhs.d;

    return *this;
}

ICDBitData::~ICDBitData()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

std::string ICDBitData::lengthOfByte() const
{
    char szTmp[128] = {0};
    // ֻ��BitMap���͵����ݴ洢�����ֽ�λ
    sprintf_s(szTmp, sizeof(szTmp), "%d/%d", 
              ICDMetaData::length(), rangeLength() * 8);

    return std::string(szTmp);
}

std::string ICDBitData::range() const
{
    std::string result;
    char szTmp[32] = { 0 };

    result = ICDCommonData::range();
    sprintf_s(szTmp, sizeof(szTmp), "#%d~%d",
              start(), end());
    result += std::string(szTmp);

    return result;
}

std::string ICDBitData::dataRange() const
{
    return ICDCommonData::range();
}

unsigned short ICDBitData::rangeLength() const
{
    return (end() / 8 + 1);
}

int ICDBitData::byteLength() const
{
    return rangeLength() - start() / 8;
}

double ICDBitData::realLength() const
{
    return (double)ICDMetaData::length() / 8.0;
}

ICDMetaData::smtMeta ICDBitData::clone() const
{
    ICDBitData::smtBit bit(new ICDBitData(*this));

    return bit;
}

// ��ȡ��ʼ�ֺ�
unsigned short ICDBitData::start() const
{
    return d->q_start;
}

// ��ȡ�����ֺ�
unsigned short ICDBitData::end() const
{
    return (start() + (unsigned short)ICDMetaData::length() - 1);
}

// ������ʼ�ֺ�
void ICDBitData::setStart(unsigned short start)
{
    d->q_start = start;
}

// ��ȡ���س���
unsigned short ICDBitData::bitLength() const
{
    return (unsigned short)length();
}

// ���ñ��س���
void ICDBitData::setBitLength(unsigned short length)
{
    setLength(length);
}

// ������ת��Ϊ�ַ���(ȡֵ[��������]#����#ȡֵ[��������])
std::string ICDBitData::stringValue() const
{
    std::string result;
    char szTmp[128] = {0};
    std::map<double, std::string> eigenvalue = values();
    std::map<double, std::string>::iterator it = eigenvalue.begin();
    for (; it != eigenvalue.end(); ++it) {
        sprintf_s(szTmp, sizeof(szTmp), "%d", (int)it->first);
        result.append(szTmp).append("[").append(it->second).append("]#");
    }
    if (!result.empty()) {
        result = result.substr(0, result.find_last_of("#"));
    }
    return result;
}
