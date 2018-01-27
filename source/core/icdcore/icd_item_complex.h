#ifndef ICD_ITEM_COMPLEX_H
#define ICD_ITEM_COMPLEX_H

#include "icdcore_global.h"
#include "icd_item.h"

namespace Icd {

class Table;
typedef JHandlePtr<Table> TablePtr;

class ComplexItem;
class ComplexItemData;
typedef JHandlePtr<ComplexItem> ComplexItemPtr;

/**
 * @brief 复合数据项类
 */
class ICDCORE_EXPORT ComplexItem : public Item
{
public:
    /**
     * @brief ComplexItem
     * @param parent
     */
    explicit ComplexItem(Object *parent = 0);

    /**
     * @brief ComplexItem
     * @param id
     * @param parent
     */
    explicit ComplexItem(const std::string &id, Object *parent = 0);

    virtual ~ComplexItem();

    /**
     * @brief 获取复合ICD数据表
     * @return
     */
    TablePtr table() const;

    /**
     * @brief setPData
     * @param pData
     */
    void setBuffer(char *buffer);

    /**
     * @brief 设置数据缓冲大小
     * @param size : 数据缓冲大小（单位：字节）
     */
    void setBufferSize(double size);

    /**
     * @brief typeName
     * @return
     */
    std::string typeName() const;

    /**
     * @brief childCount
     * @return
     */
    int childCount() const;

    /**
     * @brief updateSend
     * @param period
     */
    void updateSend(bool period);

    /**
     * @brief updateRecv
     */
    void updateRecv();

    /**
     * @brief resetSend
     */
    void resetSend();

    /**
     * @brief reset
     */
    void reset();

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
    ComplexItem &operator =(const ComplexItem &other);

    /**
     * @brief itemByMark
     * @param mark
     * @param deep
     * @return
     */
    ObjectPtr itemByMark(const std::string &mark, bool deep = true) const;

    /**
     * @brief tableByMark
     * @param mark
     * @param deep
     * @return
     */
    TablePtr tableByMark(const std::string &mark, bool deep = true) const;

    /**
     * @brief itemByDomain
     * @param domain
     * @param domainType
     * @return
     */
    ObjectPtr itemByDomain(const std::string &domain,
                           Icd::DomainType domainType = Icd::DomainId) const;

    /**
     * @brief tableByDomain
     * @param domain
     * @param domainType
     * @return
     */
    TablePtr tableByDomain(const std::string &domain,
                           Icd::DomainType domainType = Icd::DomainId) const;

    // Serializable interface
public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int deep = -1);

protected:
    /**
     * @brief ComplexItem
     * @param other
     */
    ComplexItem(const ComplexItem &other);

    /**
     * @brief setBufferOffset
     * @param offset
     */
    void setBufferOffset(double offset);

private:
    JHandlePtr<ComplexItemData> d;
};

} // end of namespace Icd

#endif // ICD_ITEM_COMPLEX_H
