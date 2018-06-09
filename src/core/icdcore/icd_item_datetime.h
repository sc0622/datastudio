#ifndef ICD_ITEM_DATETIME_H
#define ICD_ITEM_DATETIME_H

#include "icd_item.h"

namespace Icd {

class DateTimeItem;
class DateTimeItemData;
typedef std::shared_ptr<DateTimeItem> DateTimeItemPtr;

class ICDCORE_EXPORT DateTimeItem : public Item
{
public:
    explicit DateTimeItem(Object *parent = nullptr);
    explicit DateTimeItem(const std::string &id, Object *parent = nullptr);
    ~DateTimeItem();

    std::string typeName() const override;

    Object *clone() const override;
    DateTimeItem &operator =(const DateTimeItem &other);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

protected:
    DateTimeItem(const DateTimeItem &other);

private:
    DateTimeItemData *d;
};

}

#endif // ICD_ITEM_DATETIME_H
