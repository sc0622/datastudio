#ifndef ICDBITDATA_H
#define ICDBITDATA_H
#include "icdcommondata.h"
#include <map>


class ICDBitDataPrivate;
/**
 * @brief ICD比特位数据类
 */
class KERNELCLASS_EXPORT ICDBitData
        : public ICDCommonData
{
public:
    typedef std::shared_ptr<ICDBitData> smtBit;

public:
    ICDBitData(const stTableRules &rule);
    ICDBitData(const ICDBitData &rhs);
    ICDBitData &operator=(const ICDBitData &rhs);
    ~ICDBitData();
    // inherit ICDMetaData
    virtual int byteLength() const;
    virtual double realLength() const;
    virtual std::string lengthOfByte() const;
    ICDMetaData::smtMeta clone() const;

    // inherit ICDCommonData
    virtual std::string range() const override;
    // 数据范围
    std::string dataRange() const;

    // 获取起始字号
    unsigned short start() const;
    // 获取结束字号
    unsigned short end() const;
    // 设置起始字号
    void setStart(unsigned short start);
    // 获取/设置比特长度
    unsigned short bitLength() const;
    void setBitLength(unsigned short length);

    // bit位所占字节总长度
    unsigned short rangeLength() const;

protected:
    // inherit ICDCommonData
    // 将数据转换为字符串(取值[中文描述]#……#取值[中文描述])
    std::string stringValue() const;

private:
    ICDBitDataPrivate   *d;
};

#endif // ICDBITDATA_H
