#ifndef ICD_ITEM_NUMERIC_H
#define ICD_ITEM_NUMERIC_H

#include "icdcore_global.h"
#include "icd_item.h"
#include "icd_item_limit.h"
#include <map>

namespace Icd {

/**
 * @brief The NumericType enum
 */
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
    NumericFloat64,
    NumericFloat32,
    NumericTotal
};

class NumericItem;
class NumericItemData;
typedef JHandlePtr<NumericItem> NumericItemPtr;

/**
 * @brief 数值类型数据项类
 */
class ICDCORE_EXPORT NumericItem : public Item
{
public:
    /**
     * @brief NumericItem
     * @param type
     * @param size
     * @param parent
     */
    explicit NumericItem(Object *parent = 0);

    /**
     * @brief NumericItem
     * @param id
     * @param parent
     */
    explicit NumericItem(const std::string &id, Object *parent = 0);

    ~NumericItem();

    /**
     * @brief isValid
     * @return
     */
    bool isValid() const;

    /**
     * @brief numericType
     * @return
     */
    NumericType numericType() const;

    /**
     * @brief setNumericType
     * @param type
     */
    void setNumericType(NumericType type);

    /**
     * @brief numericTypeString
     * @return
     */
    std::string numericTypeString() const;

    /**
     * @brief numericTypeString
     * @param type
     * @return
     */
    static std::string numericTypeString(NumericType type);

    /**
     * @brief stringNumericType
     * @param str
     * @return
     */
    static NumericType stringNumericType(const std::string &str);

    /**
     * @brief orignalData
     * @return
     */
    double orignalData() const;

    /**
     * @brief 获取数据项数值
     * @return
     */
    double data() const;

    /**
     * @brief 设置数值数据项为指定数值
     * @param [in] d : 数值
     */
    void setData(double data);

    /**
     * @brief 获取数据项字符串
     * @return
     */
    std::string dataString() const;

    /**
     * @brief 获取比例尺
     * @return
     */
    double scale() const;
    /**
     * @brief 设置比例尺
     * @param [in] scale : 比例尺
     */
    void setScale(double scale);

    /**
     * @brief decimals
     * @return
     */
    double decimals() const;

    /**
     * @brief setDecimals
     * @param value
     */
    void setDecimals(double value);

    /**
     * @brief 获取偏置
     * @return
     */
    double offset() const;

    /**
     * @brief 设置偏置
     * @param [in] offset : 偏置
     */
    void setOffset(double offset);

    /**
     * @brief 获取限制条件
     * @return
     */
    LimitItemPtr limit() const;

    /**
     * @brief 设置显示条件
     * @param [in] limit : 限制条件
     */
    void setLimit(const LimitItemPtr &limit);

    /**
     * @brief 获取单位
     * @return
     */
    std::string unit() const;

    /**
     * @brief 设置单位
     * @param [in] unit : 单位
     */
    void setUnit(const std::string &unit);

    /**
     * @brief 获取特征值信息
     * @return
     */
    std::map<double, std::string> specs();

    /**
     * @brief 获取特征值信息
     * @return
     */
    const std::map<double, std::string> &specs() const;

    /**
     * @brief 添加特征点
     * @param [in] key : 特征值
     * @param [in] info : 特征信息
     */
    void addSpec(double key, const std::string info);

    /**
     * @brief specAt
     * @param key
     * @return
     */
    std::string specAt(double key) const;

    /**
     * @brief 删除特征点
     * @param [in] key : 特征值
     */
    void removeSpec(double key);

    /**
     * @brief clearSpec
     */
    void clearSpec();

    /**
     * @brief dataRange
     * @return
     */
    std::pair<double, double> dataRange() const;

    /**
     * @brief valueRange
     * @return
     */
    std::pair<double, double> valueRange() const;

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
     * @brief outOfLimit
     * @return
     */
    bool outOfLimit() const;

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
    NumericItem &operator =(const NumericItem &other);

    // Serializable interface
public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int deep = -1);

protected:
    /**
     * @brief NumericItem
     * @param other
     */
    NumericItem(const NumericItem &other);

private:
    NumericItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_NUMERIC_H
