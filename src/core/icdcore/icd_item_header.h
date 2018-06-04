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
    virtual ~HeaderItem();

    double data() const final;
    void setData(double data) final;
    std::string dataString() const final;
    unsigned char value() const;
    void setValue(unsigned char value);

    std::string typeName() const final;

    Object *clone() const final;
    HeaderItem &operator =(const HeaderItem &other);

    // Serializable interface
public:
    Json::Value save() const final;
    bool restore(const Json::Value &json, int deep = -1) final;

protected:
    HeaderItem(const HeaderItem &other);

private:
    HeaderItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_HEAD_H
