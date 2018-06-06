#ifndef ICDCOMMONDATA_H
#define ICDCOMMONDATA_H
#include "icdmetadata.h"


class ICDCommonDataPrivate;
/**
 * @brief ICD通用数据类
 */
class KERNELCLASS_EXPORT ICDCommonData
        : public ICDMetaData
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

    // 设置/获取编码
    virtual void setProCode(const std::string& proCode);
    virtual std::string proCode() const;

    // 设置/获取默认值
    virtual void setDefaultStr(const std::string& str);
    virtual std::string defaultStr() const;

    // 设置/获取取值范围
    virtual void setRange(const std::string& range);
    virtual std::string range() const;

    // 设置/获取偏置
    virtual void setOffset(double offset);
    virtual double offset() const;

    // 设置/获取单位
    virtual void setUnit(const std::string& unit);
    virtual std::string unit() const;

    // 设置/获取比例尺
    virtual void setScale(double scale);
    virtual double scale() const;

    // 设置/获取规则
    virtual void setRule(const std::string& rule);
    virtual std::string rule() const;

    // 添加数据
    void addValue(double value, const std::string &meaning);

    // 删除数据
    void deleteValue(double value);

    // 设置数据
    void setValues(const std::map<double, std::string> &values);

    // 获取所有数据
    std::map<double, std::string> values() const;

    // 判定数据是否在限制范围内
    bool dataInRange(double value) const;

protected:
    // 将数据转换为字符串(取值[中文描述]#……#取值[中文描述])
    virtual std::string stringValue() const;

    // 从字字符串解析数据(取值[中文描述]#……#取值[中文描述])
    void fromString(const std::string &rule);

private:
    ICDCommonDataPrivate    *d;
};

#endif // ICDCOMMONDATA_H
