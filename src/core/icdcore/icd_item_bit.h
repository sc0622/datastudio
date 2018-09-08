#ifndef ICD_ITEM_BIT_H
#define ICD_ITEM_BIT_H

#include "icd_item.h"
#include "icd_item_limit.h"
#include <unordered_map>

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
    BitItem(const BitItem &other);
    ~BitItem();

    double data() const override;
    void setData(double d) override;
    bool testBit(int offset) const;
    std::string dataString() const override;
    double dataFromBuffer(const char *buffer) const override;
    bool testBitFromBuffer(int offset, const char *buffer) const;

    int bitStart() const;
    void setBitStart(int bitStart);

    int bitCount() const;
    void setBitCount(int count);

    int typeSize() const;
    void setTypeSize(int size);

    int calcSize() const;

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

    ObjectPtr copy() const override;
    ObjectPtr clone() const override;
    BitItem &operator =(const BitItem &other);

    // for bitvalue
    double originalData() const;
    double originalDataFromBuffer(const char *buffer) const;

    double offset() const;
    void setOffset(double offset);

    double scale() const;
    void setScale(double scale);

    int decimals() const;
    void setDecimals(int value);

    LimitItemPtr limit() const;
    void setLimit(const LimitItemPtr &limit);

    std::string unit() const;
    void setUnit(const std::string &unit);

    std::pair<double, double> dataRange() const;
    std::pair<double, double> valueRange() const;

    bool outOfLimit() const;

    std::string prettyValue() const;
    static std::string prettyValue(double value);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

private:
    BitItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_BIT_H
