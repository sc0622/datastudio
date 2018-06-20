#ifndef ICDCHECKDATA_H
#define ICDCHECKDATA_H
#include "icdcommondata.h"


class ICDCheckDataPrivate;
/**
 * @brief ICDͨ��������
 */
class KERNELCLASS_EXPORT ICDCheckData
        : public ICDCommonData
{
public:
    typedef std::shared_ptr<ICDCheckData> smtCheck;

public:
    ICDCheckData(const stTableRules &rule);
    ICDCheckData(const ICDCheckData &rhs);
    ICDCheckData &operator = (const ICDCheckData &rhs);
    virtual ~ICDCheckData();

    // ICDMetaData interface
    ICDMetaData::smtMeta clone() const;

    // ����/��ȡУ������
    void setCheckType(int checkType);
    int checkType() const;
    std::string stringType() const;

    // ����/��ȡУ������
    void setStart(int start);
    int start() const;

    // ����/��ȡУ������
    void setEnd(int end);
    int end() const;

    // ��У�鷶Χת��Ϊ�ַ���(x~x)
    std::string checkRange() const;

    std::string typeString() const override;

private:
    ICDCheckDataPrivate     *d;
};

#endif // ICDCHECKDATA_H
