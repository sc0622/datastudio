#ifndef ICDARRAYDATA_H
#define ICDARRAYDATA_H

#include "icdcommondata.h"

class ICDArrayDataPrivate;

class KERNELCLASS_EXPORT ICDArrayData : public ICDCommonData
{
public:
    typedef std::shared_ptr<ICDArrayData> smtArray;

public:
    explicit ICDArrayData(const stTableRules &rule);
    ICDArrayData(const ICDArrayData &rhs);
    ICDArrayData &operator = (const ICDArrayData &rhs);
    virtual ~ICDArrayData();

    // ICDMetaData interface
    ICDMetaData::smtMeta clone() const;

    // ����/��ȡ��������
    void setArrayType(int arrayType);
    int arrayType() const;
    std::string stringType() const;

    int count() const;
    void setCount(int count);

    int typeSize() const;

    std::string typeString() const override;

private:
    ICDArrayDataPrivate *d;
};

#endif // ICDARRAYDATA_H
