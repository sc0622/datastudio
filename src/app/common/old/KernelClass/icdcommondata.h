#ifndef ICDCOMMONDATA_H
#define ICDCOMMONDATA_H
#include "icdmetadata.h"

class ICDCommonDataPrivate;
/**
 * @brief ICDͨ��������
 */
class KERNELCLASS_EXPORT ICDCommonData : public ICDMetaData
{
public:
    typedef std::shared_ptr<ICDCommonData> smtCommon;

public:
    ICDCommonData(const stTableRules &rule);
    ICDCommonData(const ICDCommonData &rhs);
    ICDCommonData &operator = (const ICDCommonData &rhs);
    virtual ~ICDCommonData();

    // ICDMetaData interface
    ICDMetaData::smtMeta clone() const;

    // ����/��ȡ����
    virtual void setProCode(const std::string& proCode) override;
    virtual std::string proCode() const override;

    // ����/��ȡĬ��ֵ
    virtual void setDefaultStr(const std::string& str);
    virtual std::string defaultStr() const;

    // ����/��ȡȡֵ��Χ
    virtual void setRange(const std::string& range);
    virtual std::string range() const;

    // ����/��ȡƫ��
    virtual void setOffset(double offset);
    virtual double offset() const;

    // ����/��ȡ��λ
    virtual void setUnit(const std::string& unit);
    virtual std::string unit() const;

    // ����/��ȡ������
    virtual void setScale(double scale);
    virtual double scale() const;

    // ����/��ȡ����
    virtual void setRule(const std::string& rule);
    virtual std::string rule() const;

    // �������
    void addValue(double value, const std::string &meaning);

    // ɾ������
    void deleteValue(double value);

    // ��������
    void setValues(const std::map<double, std::string> &values);

    // ��ȡ��������
    std::map<double, std::string> values() const;

    // �ж������Ƿ������Ʒ�Χ��
    bool dataInRange(double value) const;

protected:
    // ������ת��Ϊ�ַ���(ȡֵ[��������]#����#ȡֵ[��������])
    virtual std::string stringValue() const;

    // �����ַ�����������(ȡֵ[��������]#����#ȡֵ[��������])
    void fromString(const std::string &rule);

private:
    ICDCommonDataPrivate    *d;
};

#endif // ICDCOMMONDATA_H
