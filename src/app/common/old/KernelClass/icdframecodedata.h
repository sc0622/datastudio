#ifndef ICDFRAMECODEDATA_H
#define ICDFRAMECODEDATA_H
#include "icdcomplexdata.h"

class ICDFrameCodeDataPrivate;
/**
 * @brief ICD帧识别码类
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

    // 设置/获取帧码类型
    void setFrameType(int frameType);
    int frameType() const;
    // 绑定帧数据
    void bindData(ICDComplexData::smtComplex data);
    // 获取绑定帧数据
    ICDComplexData::smtComplex data() const;
    // 获取绑定数据标志
    int bindingSerial() const;
    std::string bindingStringSerial() const;

    std::string typeString() const override;

private:
    ICDFrameCodeDataPrivate *d;
};

#endif // ICDFRAMECODEDATA_H
