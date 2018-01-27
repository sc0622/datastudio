#ifndef ICD_ITEM_BIT_H
#define ICD_ITEM_BIT_H

#include "icdcore_global.h"
#include "icd_item.h"
#include <map>

namespace Icd {

class BitItem;
class BitItemData;
typedef JHandlePtr<BitItem> BitItemPtr;

/**
 * @brief The BitItem class
 */
class ICDCORE_EXPORT BitItem : public Item
{
public:
    /**
     * @brief BitItem
     * @param type
     * @param parent
     */
    explicit BitItem(ItemType type, Object *parent = 0);

    /**
     * @brief BitItem
     * @param id
     * @param type
     * @param parent
     */
    explicit BitItem(const std::string &id, ItemType type, Object *parent = 0);

    ~BitItem();

    /**
     * @brief 获取比特数据项数值
     * @return
     */
    double data() const;

    /**
     * @brief 设置比特数据项为指定数值
     * @param [in] d : 指定数值
     */
    void setData(double d);

    /**
     * @brief testBit
     * @param offset
     * @return
     */
    bool testBit(int offset) const;

    /**
     * @brief 获取比特数据项数值字符串
     * @return
     */
    std::string dataString() const;

    /**
     * @brief 获取起始位置
     * @return
     */
    int bitStart() const;

    /**
     * @brief 设置比特起始位置
     * @param [in] bitStart : 比特起始位置
     */
    void setBitStart(int bitStart);

    /**
     * @brief 获取比特位长度
     * @return
     */
    int bitCount() const;

    /**
     * @brief 设置比特位长度
     * @param [in] count : 比特位长度
     */
    void setBitCount(int count);

    /**
     * @brief typeSize
     * @return
     */
    int typeSize() const;

    /**
     * @brief setTypeSize
     * @param size
     */
    void setTypeSize(int size);

    /**
     * @brief mask
     * @return
     */
    icd_uint64 mask() const;

    /**
     * @brief 获取特征点信息
     * @return
     */
    std::map<icd_uint64, std::string> specs();

    /**
     * @brief 获取特征点信息
     * @return
     */
    const std::map<icd_uint64, std::string> &specs() const;

    /**
     * @brief 添加特征点
     * @param [in] key : 特征值
     * @param [in] info : 特征信息
     */
    void addSpec(icd_uint64 key, const std::string &info);

    /**
     * @brief 删除特诊信息
     * @param [in] key :特征值
     */
    void remveSpec(icd_uint64 key);

    /**
     * @brief clearSpec
     */
    void clearSpec();

    /**
     * @brief specAt
     * @param key
     * @return
     */
    std::string specAt(icd_uint64 key) const;

    /**
     * @brief nameAt
     * @param offset
     * @return
     */
    std::string nameAt(int offset) const;

    /**
     * @brief descAt, for bitmap type
     * @param offset
     * @return
     */
    std::string descAt(int offset) const;

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
    BitItem &operator =(const BitItem &other);

    // Serializable interface
public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int deep = -1);

protected:
    /**
     * @brief BitItem
     * @param other
     */
    BitItem(const BitItem &other);

private:
    BitItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_BIT_H
