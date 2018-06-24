#ifndef ICDMETADATA_H
#define ICDMETADATA_H

#include "icdelement.h"
#include <map>

namespace IcdDefine {

enum IcdType {
    icdUnknown,         // δ֪
    icdMeta,            // ������
    icdCommon,          // ͨ����
    icdCustom,          // ר����
    icdBit,             // ����λ��
    icdComplex,         // ������
};

} // ICDDEF

class ICDMetaDataPrivate;

/**
 * @brief ICD�������������
 */
class KERNELCLASS_EXPORT ICDMetaData : public ICDElement
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
    // �����ֽڳ���
    virtual int byteLength() const;
    // ������ʵ����
    virtual double realLength() const;
    virtual std::string lengthOfByte() const;

    // ��¡
    virtual ICDMetaData::smtMeta clone() const;

    // ����/��ȡ�ֺ�
    void setIndex(int index);
    int index() const;

    // ��������
    IcdDefine::IcdType metaType() const;

    // ����/��ȡ�����������
    void setType(int type);
    int type() const;

    // ����/��ȡ��ʶ��
    void setSerial(int serial);
    int serial() const;
    std::string stringSerial() const;

    // ����/��ȡ��ע
    virtual void setRemark(const std::string &remark);
    std::string remark() const;

    //
    int subType() const;
    void setSubType(int subType);

    virtual std::string typeString() const override;

protected:
    void setLength(int length);

protected:
    IcdDefine::IcdType q_cType;    // ������

private:
    ICDMetaDataPrivate *d;
};

#endif // ICDMETADATA_H
