#ifndef ICD_ITEM_H
#define ICD_ITEM_H

#include "icdcore_global.h"
#include "icd_object.h"
#include <vector>

namespace Icd {

/**
 * @brief The ItemType enum
 */
enum ItemType {
    ItemInvalid = -1,   /**< 无效 */
    ItemHead,           /**< 包头 */
    ItemCounter,        /**< 帧计数 */
    ItemCheck,          /**< 校验 */
    ItemFrameCode,      /**< 帧识别码 */
    ItemNumeric,        /**< 数值类型 */
    ItemBitMap,         /**< BITMAP */
    ItemBitValue,       /**< BITVALUE */
    ItemComplex,        /**< 复合数据 */
    ItemFrame,          /**< 多帧数据 */
    ItemTotal           /**< 类型总数 */
};

class Item;
class ItemData;
typedef JHandlePtr<Item> ItemPtr;
typedef std::vector<ItemPtr> ItemPtrArray;

/**
 * @brief ICD数据项类
 */
class ICDCORE_EXPORT Item : public Object
{
public:
    /**
     * @brief Item
     * @param type
     * @param parent
     */
    explicit Item(ItemType type = ItemInvalid, Object *parent = 0);

    /**
     * @brief Item
     * @param id
     * @param type
     * @param parent
     */
    explicit Item(const std::string &id, ItemType type = ItemInvalid,
                  Object *parent = 0);

    virtual ~Item();

    /**
     * @brief 获取数据项类型
     * @return 数据项类型
     */
    ItemType type() const;

    /**
     * @brief itemOffset
     * @return
     */
    int itemOffset() const;

    /**
     * @brief 获取数据缓冲指针
     * @return 数据缓冲指针
     */
    char *buffer() const;

    /**
     * @brief 获取数据缓冲长度
     * @return 数据缓冲长度（单位：字节）
     */
    virtual double bufferSize() const;

    /**
     * @brief bufferOffset
     * @return
     */
    virtual double bufferOffset() const;

    /**
     * @brief 获取数据项数值，不同类型数据项应在子类中重载此函数
     * @return 数值
     */
    virtual double data() const { return 0.0; }

    /**
     * @brief 设置数据项数值，不同类型数据项应在子类中重载此函数
     * @param [in] d : 目标数值
     */
    virtual void setData(double d) { (void)d; }

    /**
     * @brief 获取数据项数值字符串，不同类型数据项应在子类中重载此函数
     * @return 数值字符串
     */
    virtual std::string dataString() const { return ""; }

    /**
     * @brief defaultValue
     * @return
     */
    double defaultValue() const;

    /**
     * @brief setDefaultValue
     * @param value
     */
    void setDefaultValue(double value);

    /**
     * @brief typeName
     * @return
     */
    virtual std::string typeName() const;

    /**
     * @brief codeName
     * @return
     */
    virtual std::string codeName() const;

    /**
     * @brief typeString
     * @return
     */
    virtual std::string typeString() const;

    /**
     * @brief typeString
     * @param type
     * @return
     */
    static std::string typeString(ItemType type);

    /**
     * @brief stringType
     * @param str
     * @return
     */
    static ItemType stringType(const std::string &str);

    /**
     * @brief resetData
     */
    virtual void resetData();

    /**
     * @brief clearData
     */
    virtual void clearData();

    /**
     * @brief clone
     * @return
     */
    virtual Object *clone() const;

    /**
     * @brief operator =
     * @param other
     * @return
     */
    Item &operator =(const Item &other);

    /**
     * @brief create
     * @param id
     * @param type
     * @return
     */
    static ItemPtr create(const std::string &id, ItemType type);

protected:
    /**
     * @brief Item
     * @param other
     */
    Item(const Item &other);

    /**
     * @brief 设置数据项类型，不同类型的子类可能重载此函数
     * @param [in] type : 数据类型
     */
    void setType(ItemType type);

    /**
     * @brief 设置数据缓冲指针
     * @param [in] buffer : 数据缓冲指针
     */
    virtual void setBuffer(char *buffer);

    /**
     * @brief setItemOffset
     * @param offset
     */
    virtual void setItemOffset(int offset);

    /**
     * @brief 设置数据缓冲大小
     * @param size : 数据缓冲大小（单位：字节）
     */
    virtual void setBufferSize(double size);

    /**
     * @brief 设置数据缓冲字节偏移量
     * @param size : 数据缓冲字节偏移量（单位：字节）
     */
    virtual void setBufferOffset(double offset);

    // Serializable interface
public:
    virtual Json::Value save() const;
    virtual bool restore(const Json::Value &json, int deep = -1);

private:
    ItemData *d;
    friend class Table;
    friend class JsonParser;
    friend class XmlParser;
    friend class SqlParser;
};

} // end of namespace Icd

#endif // ICD_ITEM_H
