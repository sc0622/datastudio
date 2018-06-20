#ifndef ICDCHECKDATA_H
#define ICDCHECKDATA_H
#include "icdcommondata.h"


class ICDCheckDataPrivate;
/**
 * @brief ICD通用数据类
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

    // 设置/获取校验类型
    void setCheckType(int checkType);
    int checkType() const;
    std::string stringType() const;

    // 设置/获取校验类型
    void setStart(int start);
    int start() const;

    // 设置/获取校验类型
    void setEnd(int end);
    int end() const;

    // 将校验范围转换为字符串(x~x)
    std::string checkRange() const;

    std::string typeString() const override;

private:
    ICDCheckDataPrivate     *d;
};

#endif // ICDCHECKDATA_H
