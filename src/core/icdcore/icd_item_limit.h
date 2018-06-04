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

    bool leftInf() const;
    void setLeftInf(bool leftInf);

    bool rightInf() const;
    void setRightInf(bool rightInf);

    std::string toString() const;
    virtual LimitItem *clone() const final;
    LimitItem &operator =(const LimitItem &other);

public:
    Json::Value save() const final;
    bool restore(const Json::Value &json, int /*deep*/ = -1) final;

protected:
    LimitItem(const LimitItem &other);

private:
    LimitItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_LIMIT_H
