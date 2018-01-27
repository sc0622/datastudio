#ifndef ICD_ITEM_COUNTER_H
#define ICD_ITEM_COUNTER_H

#include "icdcore_global.h"
#include "icd_item.h"

namespace Icd {

/**
 * @brief The CounterType enum
 */
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
typedef JHandlePtr<CounterItem> CounterItemPtr;
typedef std::vector<CounterItemPtr> CounterItemPtrArray;

/**
 * @brief The CounterItem class
 */
class ICDCORE_EXPORT CounterItem : public Item
{
public:
    /**
     * @brief CounterItem
     * @param parent
     */
    explicit CounterItem(Object *parent = 0);

    /**
     * @brief CounterItem
     * @param id
     * @param parent
     */
    explicit CounterItem(const std::string &id, Object *parent = 0);

    virtual ~CounterItem();

    /**
     * @brief counterType
     * @return
     */
    CounterType counterType() const;

    /**
     * @brief setCounterType
     * @param type
     */
    void setCounterType(CounterType type);

    /**
     * @brief counterTypeString
     * @return
     */
    std::string counterTypeString() const;

    /**
     * @brief counterTypeString
     * @param type
     * @return
     */
    static std::string counterTypeString(CounterType type);

    /**
     * @brief stringCounterType
     * @param str
     * @return
     */
    static CounterType stringCounterType(const std::string &str);

    /**
     * @brief 获取包头实际数值
     * @return
     */
    double data() const;

    /**
     * @brief 设置包头数据项为指定数值
     * @param [in] d : 数值
     */
    void setData(double data);

    /**
     * @brief 获取包头字符串
     * @return
     */
    virtual std::string dataString();

    /**
     * @brief 获取包头常值
     * @return 包头常值
     */
    unsigned char value() const;

    /**
     * @brief 设置包头常值
     * @param 包头常值
     */
    void setValue(unsigned char value);

    /**
     * @brief typeName
     * @return
     */
    std::string typeName() const;

    /**
     * @brief typeString
     * @return
     */
    virtual std::string typeString() const;

    /**
     * @brief clone
     * @return
     */
    Object *clone() const;

    /**
     * @brief operator =
     * @param other
     * @return
     */
    CounterItem &operator =(const CounterItem &other);

    // Serializable interface
public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int deep = -1);

protected:
    /**
     * @brief CounterItem
     * @param other
     */
    CounterItem(const CounterItem &other);

private:
    JHandlePtr<CounterItemData> d;
};

} // end of namespace Icd

#endif // ICD_ITEM_COUNTER_H
