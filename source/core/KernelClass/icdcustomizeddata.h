#ifndef ICDCUSTOMIZEDDATA_H
#define ICDCUSTOMIZEDDATA_H
#include "icdcommondata.h"
#include <vector>

class ICDCustomizedDataPrivate;
/**
 * @brief ICD专用数据类
 */
class KERNELCLASS_EXPORT ICDCustomizedData
        : public ICDCommonData
{
public:
    typedef std::shared_ptr<ICDCustomizedData> smtCustom;

public:
    ICDCustomizedData(const stTableRules &rule);
    ICDCustomizedData(const ICDCustomizedData &rhs);
    ICDCustomizedData &operator = (const ICDCustomizedData &rhs);
    ~ICDCustomizedData();

    // ICDMetaData interface
    ICDMetaData::smtMeta clone() const;

    // ICDCommonData interface
public:
    void setOffset(double offset);

private:
    ICDCustomizedDataPrivate    *d;
};

#endif // ICDCUSTOMIZEDDATA_H
