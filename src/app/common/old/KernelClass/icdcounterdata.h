#ifndef ICDCOUNTERDATA_H
#define ICDCOUNTERDATA_H
#include "icdcommondata.h"


class ICDCounterDataPrivate;
/**
 * @brief ICD通用数据类
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

    // 设置/获取帧计数类型
    void setCounterType(int counterType);
    int counterType() const;
    std::string stringType() const;

    std::string typeString() const override;

private:
    ICDCounterDataPrivate   *d;
};

#endif // ICDCOUNTERDATA_H
