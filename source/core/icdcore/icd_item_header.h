#ifndef ICD_ITEM_HEADER_H
#define ICD_ITEM_HEADER_H

#include "icdcore_global.h"
#include "icd_item.h"

namespace Icd {

class HeaderItem;
class HeaderItemData;
typedef JHandlePtr<HeaderItem> HeaderItemPtr;
typedef std::vector<HeaderItemPtr> HeaderItemPtrArray;

/**
 * @brief The HeaderItem class
 */
class ICDCORE_EXPORT HeaderItem : public Item
{
public:
    /**
     * @brief HeaderItem
     * @param parent
     */
    explicit HeaderItem(Object *parent = 0);

    /**
     * @brief HeaderItem
     * @param id
     * @param parent
     */
    explicit HeaderItem(const std::string &id, Object *parent = 0);

    virtual ~HeaderItem();

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
    std::string dataString();

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
     * @brief clone
     * @return
     */
    Object *clone() const;

    /**
     * @brief operator =
     * @param other
     * @return
     */
    HeaderItem &operator =(const HeaderItem &other);

    // Serializable interface
public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int deep = -1);

protected:
    /**
     * @brief HeadItem
     * @param other
     */
    HeaderItem(const HeaderItem &other);

private:
    JHandlePtr<HeaderItemData> d;
};

} // end of namespace Icd

#endif // ICD_ITEM_HEAD_H
