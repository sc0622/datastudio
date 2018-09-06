#ifndef ICD_ITEM_LIMIT_H
#define ICD_ITEM_LIMIT_H

#include "icd_object.h"

namespace Icd {

class LimitItem;
class LimitItemData;
typedef std::shared_ptr<LimitItem> LimitItemPtr;

class ICDCORE_EXPORT LimitItem : public Object
{
public:
    explicit LimitItem();
    ~LimitItem();

    double minimum() const;
    void setMinimum(double minimum);

    double maximum() const;
    void setMaximum(double maximum);

    bool minimumInf() const;
    void setMinimumInf(bool inf);

    bool maximumInf() const;
    void setMaximumInf(bool inf);

    std::string toString() const;
    virtual LimitItem *clone() const override;
    LimitItem &operator =(const LimitItem &other);

public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int /*deep*/ = -1) override;

protected:
    LimitItem(const LimitItem &other);

private:
    LimitItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_LIMIT_H
