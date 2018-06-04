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
    double data() const final;
    void setData(double data) final;
    std::string dataString() const final;

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

    std::map<double, std::string> specs();
    const std::map<double, std::string> &specs() const;
    void addSpec(double key, const std::string info);
    std::string specAt(double key) const;
    void removeSpec(double key);
    void clearSpec();

    std::pair<double, double> dataRange() const;
    std::pair<double, double> valueRange() const;

    std::string typeName() const final;
    std::string typeString() const final;

    bool outOfLimit() const;

    Object *clone() const final;
    NumericItem &operator =(const NumericItem &other);

    // Serializable interface
public:
    Json::Value save() const final;
    bool restore(const Json::Value &json, int deep = -1) final;

protected:
    NumericItem(const NumericItem &other);

private:
    NumericItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_NUMERIC_H
