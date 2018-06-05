#ifndef ICD_ITEM_BIT_H
#define ICD_ITEM_BIT_H

#include "icdcore_global.h"
#include "icd_item.h"
#include <map>

namespace Icd {

class BitItem;
class BitItemData;
typedef std::shared_ptr<BitItem> BitItemPtr;

class ICDCORE_EXPORT BitItem : public Item
{
public:
    explicit BitItem(Object *parent = nullptr);
    explicit BitItem(ItemType type, Object *parent = nullptr);
    explicit BitItem(const std::string &id, ItemType type, Object *parent = nullptr);
    ~BitItem();

    double data() const override;
    void setData(double d) override;
    bool testBit(int offset) const;
    std::string dataString() const override;

    int bitStart() const;
    void setBitStart(int bitStart);

    int bitCount() const;
    void setBitCount(int count);

    int typeSize() const;
    void setTypeSize(int size);

    icd_uint64 mask() const;

    std::map<icd_uint64, std::string> specs();
    const std::map<icd_uint64, std::string> &specs() const;
    void addSpec(icd_uint64 key, const std::string &info);
    void removeSpec(icd_uint64 key);
    void clearSpec();
    std::string specAt(icd_uint64 key) const;
    std::string nameAt(int offset) const;
    static std::string nameOf(const std::string &spec);
    std::string descAt(int offset) const;

    std::string typeName() const override;

    Object *clone() const override;
    BitItem &operator =(const BitItem &other);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

protected:
    BitItem(const BitItem &other);

private:
    BitItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_BIT_H
