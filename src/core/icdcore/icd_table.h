#ifndef ICD_TABLE_H
#define ICD_TABLE_H

#include "icdcore_global.h"
#include "icd_item.h"
#include <vector>

namespace Icd {

class CounterItem;
typedef JHandlePtr<CounterItem> CounterItemPtr;

class CheckItem;
typedef JHandlePtr<CheckItem> CheckItemPtr;

class FrameCodeItem;
typedef JHandlePtr<FrameCodeItem> FrameCodeItemPtr;
typedef std::vector<FrameCodeItemPtr> FrameCodeItemPtrArray;

class Table;
class TableData;
typedef JHandlePtr<Table> TablePtr;
typedef std::vector<TablePtr> TablePtrArray;

/**
 * @brief ICD表类
 */
class ICDCORE_EXPORT Table : public Object
{
public:
    /**
     * @brief Table
     */
    explicit Table(Object *parent = 0);

    /**
     * @brief Table
     * @param id
     * @param parent
     */
    explicit Table(const std::string &id, Object *parent = 0);

    virtual ~Table();

    /**
     * @brief itemOffset
     * @return
     */
    int itemOffset() const;

    /**
     * @brief 获取表数据长度
     * @return 表数据长度
     */
    double bufferSize() const;

    /**
     * @brief bufferOffset
     * @return
     */
    double bufferOffset() const;

    /**
     * @brief 获取ICD表数据块指针
     * @return
     */
    char *buffer() const;

    /**
     * @brief 指定ICD表对应的数据段
     * @param [in] buffer ：外部指定的数据段指针
     */
    void setBuffer(char *buffer);

    /**
     * @brief allItem
     * @return
     */
    ItemPtrArray allItem();

    /**
     * @brief allItem
     * @return
     */
    const ItemPtrArray &allItem() const;

    /**
     * @brief appendItem : 在ICD表中添加数据项
     * @param item : 数据项
     */
    void appendItem(const ItemPtr &item);

    /**
     * @brief insertItem : 在ICD表中插入数据项
     * @param [in] index : 待插入位置
     * @param [in] item : 待插入数据项
     */
    void insertItem(int index, const ItemPtr &item);

    /**
     * @brief 删除数据项
     * @param [in] index : 待删除数据项在数据项队列中的序号
     */
    void removeItem(int index);

    /**
     * @brief removeItemById : 根据标识删除数据项
     * @param id : 数据项标识
     */
    void removeItemById(const std::string &id);

    /**
     * @brief removeItemByMark : 根据标识删除数据项
     * @param [in] mark : 数据项标识
     */
    void removeItemByMark(const std::string &mark);

    /**
     * @brief clearItem : 清除所有数据项
     */
    void clearItem();

    /**
     * @brief itemCount
     * @return
     */
    int itemCount() const;

    /**
     * @brief itemAt
     * @param index
     * @return
     */
    ItemPtr itemAt(int index) const;

    /**
     * @brief itemById
     * @param id
     * @return
     */
    ItemPtr itemById(const std::string &id) const;

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

    /**
     * @brief headers
     * @return
     */
    const std::vector<char> &headers() const;

    /**
     * @brief counterItem
     * @return
     */
    CounterItemPtr counterItem();

    /**
     * @brief counterItem
     * @return
     */
    const CounterItemPtr &counterItem() const;

    /**
     * @brief isCheckValid
     * @return
     */
    bool isCheckValid() const;

    /**
     * @brief checkItem
     * @return
     */
    CheckItemPtr checkItem();

    /**
     * @brief checkItem
     * @return
     */
    const CheckItemPtr &checkItem() const;

    /**
     * @brief allFrameCode
     * @return
     */
    const FrameCodeItemPtrArray &allFrameCode() const;

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
     * @brief 绑定数据指针
     * @param [in] buffer : 外部程序指定的ICD表数据段指针
     */
    void bindBuffer(void *buffer);

    /**
     * @brief 获取发送时序（比例）（默认为 1）
     * @return
     */
    int sequence() const;

    /**
     * @brief 设置发送时序（默认为 1）
     * @param sequence
     */
    void setSequence(int sequence);

    /**
     * @brief period
     * @return
     */
    int period() const;

    /**
     * @brief isFrameTable
     * @return
     */
    bool isFrameTable() const;

    /**
     * @brief typeName
     * @return
     */
    std::string typeName() const;

    /**
     * @brief typeName
     * @return
     */
    std::string codeName() const;

    /**
     * @brief setParent
     * @param parent
     */
    void setParent(Object *parent);

    /**
     * @brief childCount
     * @return
     */
    int childCount() const;

    /**
     * @brief resetData
     */
    void resetData();

    /**
     * @brief clearData
     */
    void clearData();

    /**
     * @brief clone
     * @return.
     */
    Object *clone() const;

    /**
     * @brief operator =
     * @param other
     * @return
     */
    Table &operator =(const Table &other);

    // Serializable interface
public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int deep = -1);

protected:
    /**
     * @brief Table
     * @param other
     */
    Table(const Table &other);

private:
    /**
     * @brief setItemOffset
     * @param offset
     */
    void setItemOffset(int offset);

    /**
     * @brief 设置数据缓冲字节偏移量
     * @param size : 数据缓冲字节偏移量（单位：字节）
     */
    void setBufferOffset(double offset);

private:
    TableData *d;
    friend class ComplexItem;
    friend class FrameItem;
};

} // end of namespace Icd

#endif // ICD_TABLE_H
