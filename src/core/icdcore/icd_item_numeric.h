#ifndef ICD_ITEM_NUMERIC_H
#define ICD_ITEM_NUMERIC_H

#include "icdcore_global.h"
#include "icd_item.h"
#include "icd_item_limit.h"
#include <map>

namespace Icd {

//
enum NumericType {
    NumericInvalid = -1,
    NumericI8,
    NumericU8,
    NumericI16,
    NumericU16,
    NumericI32,
    NumericU32,
    NumericI64,
    NumericU64,
    NumericF32,
    NumericF64,
    NumericTotal
};

class NumericItem;
class NumericItemData;
typedef std::shared_ptr<NumericItem> NumericItemPtr;

class ICDCORE_EXPORT NumericItem : public Item
{
public:
    explicit NumericItem(Object *parent = nullptr);
    explicit NumericItem(const std::string &id, Object *parent = nullptr);
    ~NumericItem();

    bool isValid() const;

    NumericType numericType() const;
    void setNumericType(NumericType type);
    std::string numericTypeString() const;
    static std::string numericTypeString(NumericType type);
    static NumericType stringNumericType(const std::string &str);

    double originalData() const;
    double data() const override;
    void setData(double data) override;
    std::string dataString() const override;

    double originalDataFromBuffer(const char *buffer) const;
    double dataFromBuffer(const char *buffer) const override;

    double scale() const;
    void setScale(double scale);

    int decimals() const;
    void setDecimals(int value);

    double offset() const;
    void setOffset(double offset);

    LimitItemPtr limit() const;
    void setLimit(const LimitItemPtr &limit);

    std::string unit() const;
    void setUnit(const std::string &unit);

    std::map<double, std::string> specs();
    const std::map<double, std::string> &specs() const;
    void addSpec(double key, const std::string info);
    std::string specAt(double key) const;
    void removeSpec(double key);
    void clearSpec();

    std::pair<double, double> dataRange() const;
    std::pair<double, double> valueRange() const;

    std::string typeName() const override;
    std::string typeString() const override;

    bool outOfLimit() const;

    Object *clone() const override;
    NumericItem &operator =(const NumericItem &other);

    std::string prettyValue() const;
    static std::string prettyValue(double value, int numericType);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

protected:
    NumericItem(const NumericItem &other);

private:
    NumericItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_NUMERIC_H
