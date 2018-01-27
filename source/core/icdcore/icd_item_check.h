#ifndef ICD_ITEM_CHECK_H
#define ICD_ITEM_CHECK_H

#include "icdcore_global.h"
#include "icd_item.h"

namespace Icd {

/**
 * @brief The CheckType enum
 */
enum CheckType {
    CheckInvalid = -1, /**< 无效值 */
    CheckNone,      /**< 无校验 */
    CheckSum8,      /**< 8位和校验 */
    CheckSum16,     /**< 16位和校验 */
    CheckCrc8,      /**< Crc8校验 */
    CheckCrc16,     /**< Crc16校验 */
    CheckXor8,      /**< Xor8校验 */
    CheckXor16,     /**< Xor16校验 */
    CheckTotal      /**< 类型总数 */
};

class CheckItem;
class CheckItemData;
typedef JHandlePtr<CheckItem> CheckItemPtr;

/**
 * @brief The CheckItem class
 */
class ICDCORE_EXPORT CheckItem : public Item
{
public:
    /**
     * @brief CheckItem
     * @param parent
     */
    explicit CheckItem(Object *parent = 0);

    /**
     * @brief CheckItem
     * @param id
     * @param parent
     */
    explicit CheckItem(const std::string &id, Object *parent = 0);

    ~CheckItem();

    /**
     * @brief isValid
     * @return
     */
    bool isValid() const;

    /**
     * @brief 获取校验类型
     * @return
     */
    CheckType checkType() const;

    /**
     * @brief setCheckType
     * @param type
     */
    void setCheckType(CheckType type);

    /**
     * @brief checkTypeString
     * @return
     */
    std::string checkTypeString() const;

    /**
     * @brief checkTypeString
     * @param type
     * @return
     */
    static std::string checkTypeString(CheckType type);

    /**
     * @brief stringCheckType
     * @param str
     * @return
     */
    static CheckType stringCheckType(const std::string &str);

    /**
     * @brief 获取校验数据项数值
     * @return
     */
    double data() const;

    /**
     * @brief 设置校验数据项数值
     * @param [in] data : 指定数值
     */
    void setData(double data);

    /**
     * @brief 获取校验数据项数值字符串
     * @return
     */
    std::string dataString() const;

    /**
     * @brief 获取校验起始位置，从0开始计算
     * @return
     */
    int startPos() const;

    /**
     * @brief 设置校验起始位置，从0开始计算
     * @param [in] startPos : 起始位置
     */
    void setStartPos(int startPos);

    /**
     * @brief 获取校验终止位置,从0开始计算
     * @return
     */
    int endPos() const;

    /**
     * @brief 设置校验终止位置
     * @param [in] endPos : 终止位置
     */
    void setEndPos(int endPos);

    /**
     * @brief 校验数据区间长度
     * @return
     */
    int checkLength() const;

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
    CheckItem &operator =(const CheckItem &other);

    // Serializable interface
public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int deep = -1);

protected:
    /**
     * @brief CheckItem
     * @param other
     */
    CheckItem(const CheckItem &other);

private:
    JHandlePtr<CheckItemData> d;
};

} // end of namespace Icd

#endif // ICD_ITEM_CHECK_H
