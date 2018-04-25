#ifndef ICD_ITEM_LIMIT_H
#define ICD_ITEM_LIMIT_H

#include "icd_object.h"

namespace Icd {

class LimitItem;
class LimitItemData;
typedef JHandlePtr<LimitItem> LimitItemPtr;

/**
 * @brief 数据项范围类
 */
class ICDCORE_EXPORT LimitItem : public Object
{
public:
    /**
     * @brief LimitItem
     */
    explicit LimitItem();

    ~LimitItem();

    /**
     * @brief minimum
     * @return
     */
    double minimum() const;

    /**
     * @brief setMinimum
     * @param minimum
     */
    void setMinimum(double minimum);

    /**
     * @brief maximum
     * @return
     */
    double maximum() const;

    /**
     * @brief setMaximum
     * @param maximum
     */
    void setMaximum(double maximum);

    /**
     * @brief leftInf
     * @return
     */
    bool leftInf() const;

    /**
     * @brief setLeftInf
     * @param leftInf
     */
    void setLeftInf(bool leftInf);

    /**
     * @brief rightInf
     * @return
     */
    bool rightInf() const;

    /**
     * @brief setRightInf
     * @param rightInf
     */
    void setRightInf(bool rightInf);

    /**
     * @brief toString
     * @return
     */
    std::string toString() const;

    /**
     * @brief clone
     * @return
     */
    virtual LimitItem *clone() const;

    /**
     * @brief operator =
     * @param other
     * @return
     */
    LimitItem &operator =(const LimitItem &other);

public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int /*deep*/ = -1);

protected:
    /**
     * @brief LimitItem
     * @param other
     */
    LimitItem(const LimitItem &other);

private:
    LimitItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_LIMIT_H
