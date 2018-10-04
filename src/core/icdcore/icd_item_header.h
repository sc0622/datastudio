#ifndef ICD_ITEM_HEADER_H
#define ICD_ITEM_HEADER_H

#include "icdcore_global.h"
#include "icd_item.h"

namespace Icd {

class HeaderItem;
class HeaderItemData;
typedef std::shared_ptr<HeaderItem> HeaderItemPtr;
typedef std::vector<HeaderItemPtr> HeaderItemPtrArray;

class ICDCORE_EXPORT HeaderItem : public Item
{
public:
    explicit HeaderItem(Object *parent = nullptr);
    explicit HeaderItem(const std::string &id, Object *parent = nullptr);
    HeaderItem(const HeaderItem &other);
    ~HeaderItem() override;

    int rtti() const override;

    double data() const override;
    void setData(double data) override;
    std::string dataString() const override;
    double dataFromBuffer(const char *buffer) const override;

    unsigned char value() const;
    void setValue(unsigned char value);

    std::string typeName() const override;

    ObjectPtr copy() const override;
    ObjectPtr clone() const override;
    HeaderItem &operator =(const HeaderItem &other);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

private:
    HeaderItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_HEAD_H
