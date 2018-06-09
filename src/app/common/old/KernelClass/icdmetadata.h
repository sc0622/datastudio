#ifndef ICDMETADATA_H
#define ICDMETADATA_H

#include "icdelement.h"
#include <map>

namespace IcdDefine {

enum IcdType {
    icdUnknown,         // 未知
    icdMeta,            // 基本类
    icdCommon,          // 通用类
    icdCustom,          // 专用类
    icdBit,             // 比特位类
    icdComplex,         // 复合类
};

} // ICDDEF

class ICDMetaDataPrivate;

/**
 * @brief ICD规则基本数据类
 */
class KERNELCLASS_EXPORT ICDMetaData
    : public ICDElement
{
public:
    typedef std::shared_ptr<ICDMetaData> smtMeta;
    typedef std::map<int, ICDMetaData::smtMeta> ruleMap;
    typedef std::vector<ICDMetaData::smtMeta> ruleVector;

public:
    ICDMetaData(const stTableRules &rules);
    ICDMetaData(const ICDMetaData &rhs);
    ICDMetaData &operator = (const ICDMetaData &rhs);
    virtual ~ICDMetaData();

    int length() const;
    // 数据字节长度
    virtual int byteLength() const;
    // 数据真实长度
    virtual double realLength() const;
    virtual std::string lengthOfByte() const;

    // 克隆
    virtual ICDMetaData::smtMeta clone() const;

    // 设置/获取字号
    void setIndex(int index);
    int index() const;

    // 规则类型
    IcdDefine::IcdType metaType() const;

    // 设置/获取规则具体类型
    void setType(int type);
    int type() const;

    // 设置/获取标识号
    void setSerial(int serial);
    int serial() const;
    std::string stringSerial() const;

    // 设置/获取备注
    virtual void setRemark(const std::string &remark);
    std::string remark() const;

protected:
    void setLength(int length);

protected:
    IcdDefine::IcdType q_cType;    // 类类型

private:
    ICDMetaDataPrivate *d;
};

#endif // ICDMETADATA_H
