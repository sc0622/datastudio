#ifndef ICD_ITEM_STRING_H
#define ICD_ITEM_STRING_H

#include "icd_item.h"

namespace Icd {

class StringItem;
class StringItemData;
typedef std::shared_ptr<StringItem> StringItemPtr;

class ICDCORE_EXPORT StringItem : public Item
{
public:
    explicit StringItem(Object *parent = nullptr);
    explicit StringItem(const std::string &id, Object *parent = nullptr);
    ~StringItem();

    std::string typeName() const override;

    Object *clone() const override;
    StringItem &operator =(const StringItem &other);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

protected:
    StringItem(const StringItem &other);

private:
    StringItemData *d;
};

}

#endif // ICD_ITEM_STRING_H
