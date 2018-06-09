#ifndef ICD_ITEM_NUMERIC_H
#define ICD_ITEM_NUMERIC_H

#include "icdcore_global.h"
#include "icd_item.h"
#include "icd_item_limit.h"
#include <unordered_map>

namespace Icd {

//
enum NumericType {
    NumericInvalid = -1,
    NumericInt8,
    NumericUint8,
    NumericInt16,
    NumericUint16,
    NumericInt32,
    NumericUint32,
    NumericInt64,
    NumericUint64,
    NumericFloat32,
    NumericFloat64,
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

    double scale() const;
    void setScale(double scale);

    double decimals() const;
    void setDecimals(double value);

    double offset() const;
    void setOffset(double offset);

    LimitItemPtr limit() const;
    void setLimit(const LimitItemPtr &limit);

    std::string unit() const;
    void setUnit(const std::string &unit);

    std::unordered_map<double, std::string> specs();
    const std::unordered_map<double, std::string> &specs() const;
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
