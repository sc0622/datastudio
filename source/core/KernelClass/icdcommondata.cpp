#include "icdcommondata.h"
#include "datalimitation.h"
#include <vector>

class ICDCommonDataPrivate
{
    friend class ICDCommonData;
public:
    ICDCommonDataPrivate()
        : q_offset(0.0)
        , q_scale(1.0)
    {}

private:
    double      q_offset;   // ƫ��
    double      q_scale;    // ������
    std::string q_default;  // Ĭ��ֵ
    std::string q_range;    // ȡֵ��Χ
    std::string q_unit;     // ��λ
    std::string q_rule;     // ����
    // ���ݶ�Ӧ��ϵ<��ֵ, ���ĺ���>
    std::map<double, std::string> q_value;
    DataLimitation q_limit;
};

ICDCommonData::ICDCommonData(const stTableRules &rule)
    : ICDMetaData(rule)
    , d(new ICDCommonDataPrivate())
{
    q_cType = IcdDefine::icdCommon;

    setProCode(rule.sPrgCode);
    setDefaultStr(rule.sDefault);
    setRange(rule.sRange);
    setOffset(rule.dOffset);
    setUnit(rule.sUnit);
    setScale(rule.dScale);
    setRule(rule.sRule);
    switch (rule.uType) {
    case GlobalDefine::dtHead:      // ��ͷ
    case GlobalDefine::dtU8:        // �޷���8λ
    case GlobalDefine::dt8:         // �з���8λ
        setLength(1);
        break;
    case GlobalDefine::dtU16:       // �޷���16λ
    case GlobalDefine::dt16:        // �з���16λ
        setLength(2);
        break;
    case GlobalDefine::dtU32:       // �޷���32λ
    case GlobalDefine::dt32:        // �з���32λ
    case GlobalDefine::dtF32:       // 32λ������
        setLength(4);
        break;
    case GlobalDefine::dtF64:       // 64λ������
        setLength(8);
        break;
    default:
        break;
    }
}

ICDCommonData::ICDCommonData(const ICDCommonData &rhs)
    : ICDMetaData(rhs)
    , d(new ICDCommonDataPrivate())
{
    *this = rhs;
}

ICDCommonData & ICDCommonData::operator=(const ICDCommonData &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    *(ICDMetaData*)this = rhs;
    *d = *rhs.d;

    return *this;
}

ICDCommonData::~ICDCommonData()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

ICDMetaData::smtMeta ICDCommonData::clone() const
{
    ICDCommonData::smtCommon common(new ICDCommonData(*this));

    return common;
}

void ICDCommonData::setProCode(const std::string& proCode)
{
    setSign(proCode);
}

std::string ICDCommonData::proCode() const
{
    return sign();
}

void ICDCommonData::setDefaultStr(const std::string& str)
{
    d->q_default = str;
}

std::string ICDCommonData::defaultStr() const
{
    return d->q_default;
}

void ICDCommonData::setRange(const std::string& range)
{
    if (range.empty()) {
        d->q_range = "~";
        d->q_limit.setLeftInf(true);
        d->q_limit.setRightInf(true);
    } else {
        d->q_range = range;
        std::string::size_type pos = range.find("#");
        if (std::string::npos != pos) {
            d->q_range = range.substr(0, pos);
        }
        pos = d->q_range.find("~");
        if (std::string::npos != pos) {
            // ����
            std::string value = range.substr(0, pos);
            if (value.empty()) {
                d->q_limit.setLeftInf(true);
            } else {
                d->q_limit.setMin(atof(value.c_str()));
            }
            // ����
            value = range.substr(pos + strlen("~"));
            if (value.empty()) {
                d->q_limit.setRightInf(true);
            } else {
                d->q_limit.setMax(atof(value.c_str()));
            }
        }
    }
}

std::string ICDCommonData::range() const
{
    return d->q_range;
}

void ICDCommonData::setOffset(double offset)
{
    d->q_offset = offset;
}

double ICDCommonData::offset() const
{
    return d->q_offset;
}

void ICDCommonData::setUnit(const std::string& unit)
{
    d->q_unit = unit;
}

std::string ICDCommonData::unit() const
{
    return d->q_unit;
}

void ICDCommonData::setScale(double scale)
{
    d->q_scale = scale;
}

double ICDCommonData::scale() const
{
    return d->q_scale;
}

void ICDCommonData::setRule(const std::string& rule)
{
    d->q_rule = rule;

    fromString(rule);
}

std::string ICDCommonData::rule() const
{
    return d->q_rule;
}

// �������
void ICDCommonData::addValue(double value, const std::string &meaning)
{
    d->q_value[value] = meaning;

    d->q_rule = stringValue();
}

// ɾ������
void ICDCommonData::deleteValue(double value)
{
    std::map<double, std::string>::iterator it = d->q_value.find(value);
    if (it != d->q_value.end()) {
        d->q_value.erase(it);
    }

    d->q_rule = stringValue();
}

// ��������
void ICDCommonData::setValues(const std::map<double, std::string> &values)
{
    d->q_value = values;

    d->q_rule = stringValue();
}

// ��ȡ��������
std::map<double, std::string> ICDCommonData::values() const
{
    return d->q_value;
}

// �ж������Ƿ������Ʒ�Χ��
bool ICDCommonData::dataInRange(double value) const
{
    if (d->q_limit.leftInf()) {
        if (d->q_limit.rightInf()) {
            return true;
        } else {
            return !(value > d->q_limit.max());
        }
    } else {
        if (d->q_limit.rightInf()) {
            return !(value < d->q_limit.min());
        } else {
            return !((value < d->q_limit.min()) || (value > d->q_limit.max()));
        }
    }
}

// ������ת��Ϊ�ַ���(ȡֵ[��������]#����#ȡֵ[��������])
std::string ICDCommonData::stringValue() const
{
    std::string result;
    std::string value;
    char szTmp[128] = {0};
    std::string::size_type dotPos = std::string::npos;
    std::string::size_type lastZero = std::string::npos;
    std::map<double, std::string>::iterator it = d->q_value.begin();
    for (; it != d->q_value.end(); ++it) {
        sprintf_s(szTmp, sizeof(szTmp), "%f", it->first);
        value = szTmp;
        dotPos = value.find(".");
        lastZero = value.find_last_not_of("0");
        value = value.substr(0, lastZero > dotPos ? lastZero + 1 : dotPos);
        result.append(value).append("[").append(it->second).append("]#");
    }
    if (!result.empty()) {
        result = result.substr(0, result.find_last_of("#"));
    }
    return result;
}

// �����ַ�����������(ȡֵ[��������]#����#ȡֵ[��������])
void ICDCommonData::fromString(const std::string &rule)
{
    std::string tmpRule(rule);
    if (!tmpRule.empty()) {
        double value = 0;
        std::vector<std::string> result;
        std::string::size_type index(std::string::npos);
        std::string::size_type rank = std::string::npos;
        std::string::size_type offset = 0;
        std::string item;
        std::string meaning;
        do {
            index = tmpRule.find("#", offset);
            item = tmpRule.substr(offset, index - offset);
            if (std::string::npos != (rank = item.find("["))) {
                meaning = item.substr(rank + strlen("["),
                                      item.length() - rank - strlen("[]"));
                value = atoi(item.substr(0, rank).c_str());
                d->q_value[value] = meaning;
            }
            offset = index + strlen("#");
        } while (std::string::npos != index);
    }
}
