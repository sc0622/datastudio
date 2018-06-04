﻿#ifndef ICD_ITEM_CHECK_H
#define ICD_ITEM_CHECK_H

#include "icdcore_global.h"
#include "icd_item.h"

namespace Icd {

//
enum CheckType {
    CheckInvalid = -1,  /**<  */
    CheckNone,          /**<  */
    CheckSum8,          /**< 8 bits sum */
    CheckSum16,         /**< 16 bits sum */
    CheckCrc8,          /**< 8 bits crc */
    CheckCrc16,         /**< 16 bits crc */
    CheckXor8,          /**< 8 bits xor */
    CheckXor16,         /**< 16 bits xor */
    CheckTotal          /**<  */
};

class CheckItem;
class CheckItemData;
typedef std::shared_ptr<CheckItem> CheckItemPtr;

class ICDCORE_EXPORT CheckItem : public Item
{
public:
    explicit CheckItem(Object *parent = nullptr);
    explicit CheckItem(const std::string &id, Object *parent = nullptr);

    ~CheckItem();

    bool isValid() const;

    CheckType checkType() const;
    void setCheckType(CheckType type);
    std::string checkTypeString() const;
    static std::string checkTypeString(CheckType type);
    static CheckType stringCheckType(const std::string &str);

    double data() const final;
    void setData(double data) final;
    std::string dataString() const final;

    int startPos() const;
    void setStartPos(int startPos);

    int endPos() const;
    void setEndPos(int endPos);

    int checkLength() const;

    std::string typeName() const final;
    std::string typeString() const final;

    Object *clone() const final;
    CheckItem &operator =(const CheckItem &other);

    // Serializable interface
public:
    Json::Value save() const final;
    bool restore(const Json::Value &json, int deep = -1) final;

protected:
    CheckItem(const CheckItem &other);

private:
    CheckItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_CHECK_H
