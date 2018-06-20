#ifndef ICDNUMERICDATA_H
#define ICDNUMERICDATA_H

#include "icdcommondata.h"

class ICDNumericDataPrivate;

class KERNELCLASS_EXPORT ICDNumericData
    : public ICDCommonData
{
public:
    typedef std::shared_ptr<ICDNumericData> smtNumeric;

public:
    ICDNumericData(const stTableRules &rule);
    ICDNumericData(const ICDNumericData &rhs);
    ICDNumericData &operator = (const ICDNumericData &rhs);
    virtual ~ICDNumericData();

    // ICDMetaData interface
    ICDMetaData::smtMeta clone() const;

    // 设置/获取数值类型
    void setNumericType(int numericType);
    int numericType() const;
    std::string stringType() const;

    int typeSize() const;

    std::string typeString() const override;

private:
    ICDNumericDataPrivate *d;
};

#endif // ICDNUMERICDATA_H
