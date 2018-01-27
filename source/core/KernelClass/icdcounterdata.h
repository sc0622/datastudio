#ifndef ICDCOUNTERDATA_H
#define ICDCOUNTERDATA_H
#include "icdcommondata.h"


class ICDCounterDataPrivate;
/**
 * @brief ICDͨ��������
 */
class KERNELCLASS_EXPORT ICDCounterData
    : public ICDCommonData
{
public:
    typedef std::shared_ptr<ICDCounterData> smtCounter;

public:
    ICDCounterData(const stTableRules &rule);
    ICDCounterData(const ICDCounterData &rhs);
    ICDCounterData &operator = (const ICDCounterData &rhs);
    virtual ~ICDCounterData();

    // ICDMetaData interface
    ICDMetaData::smtMeta clone() const;

    // ����/��ȡ֡��������
    void setCounterType(int counterType);
    int counterType() const;
    std::string stringType() const;

private:
    ICDCounterDataPrivate   *d;
};

#endif // ICDCOUNTERDATA_H
