#ifndef ICD_ITEM_COUNTER_H
#define ICD_ITEM_COUNTER_H

#include "icdcore_global.h"
#include "icd_item.h"

namespace Icd {

//
enum CounterType {
    CounterInvalid = -1,
    CounterU8,
    CounterU16,
    CounterU32,
    CounterU64,
    CounterTotal
};

class CounterItem;
class CounterItemData;
typedef std::shared_ptr<CounterItem> CounterItemPtr;
typedef std::vector<CounterItemPtr> CounterItemPtrArray;

class ICDCORE_EXPORT CounterItem : public Item
{
public:
    explicit CounterItem(Object *parent = nullptr);
    explicit CounterItem(const std::string &id, Object *parent = nullptr);
    virtual ~CounterItem();

    CounterType counterType() const;
    void setCounterType(CounterType type);
    std::string counterTypeString() const;
    static std::string counterTypeString(CounterType type);
    static CounterType stringCounterType(const std::string &str);


    double data() const final;
    void setData(double data) final;
    std::string dataString() const final;
    unsigned char value() const;
    void setValue(unsigned char value);

    std::string typeName() const final;
    virtual std::string typeString() const final;

    Object *clone() const final;
    CounterItem &operator =(const CounterItem &other);

    // Serializable interface
public:
    Json::Value save() const final;
    bool restore(const Json::Value &json, int deep = -1) final;

protected:
    CounterItem(const CounterItem &other);

private:
    CounterItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_COUNTER_H
