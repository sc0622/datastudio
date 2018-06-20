#ifndef ICDFRAMECODEDATA_H
#define ICDFRAMECODEDATA_H
#include "icdcomplexdata.h"

class ICDFrameCodeDataPrivate;
/**
 * @brief ICD֡ʶ������
 */
class KERNELCLASS_EXPORT ICDFrameCodeData
        : public ICDCommonData
{
public:
    typedef std::shared_ptr<ICDFrameCodeData> smtFrameCode;

public:
    ICDFrameCodeData(const stTableRules &rule);
    ICDFrameCodeData(const ICDFrameCodeData &rhs);
    ICDFrameCodeData &operator = (const ICDFrameCodeData &rhs);
    virtual ~ICDFrameCodeData();

    // ICDMetaData interface
    ICDMetaData::smtMeta clone() const;

    // ����/��ȡ֡������
    void setFrameType(int frameType);
    int frameType() const;
    // ��֡����
    void bindData(ICDComplexData::smtComplex data);
    // ��ȡ��֡����
    ICDComplexData::smtComplex data() const;
    // ��ȡ�����ݱ�־
    int bindingSerial() const;
    std::string bindingStringSerial() const;

    std::string typeString() const override;

private:
    ICDFrameCodeDataPrivate *d;
};

#endif // ICDFRAMECODEDATA_H
