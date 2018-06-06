#ifndef ICDBITDATA_H
#define ICDBITDATA_H
#include "icdcommondata.h"
#include <map>


class ICDBitDataPrivate;
/**
 * @brief ICD����λ������
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
    // ���ݷ�Χ
    std::string dataRange() const;

    // ��ȡ��ʼ�ֺ�
    unsigned short start() const;
    // ��ȡ�����ֺ�
    unsigned short end() const;
    // ������ʼ�ֺ�
    void setStart(unsigned short start);
    // ��ȡ/���ñ��س���
    unsigned short bitLength() const;
    void setBitLength(unsigned short length);

    // bitλ��ռ�ֽ��ܳ���
    unsigned short rangeLength() const;

protected:
    // inherit ICDCommonData
    // ������ת��Ϊ�ַ���(ȡֵ[��������]#����#ȡֵ[��������])
    std::string stringValue() const;

private:
    ICDBitDataPrivate   *d;
};

#endif // ICDBITDATA_H
