#ifndef ICD_ITEM_FRAME_H
#define ICD_ITEM_FRAME_H

#include "icdcore_global.h"
#include "icd_item.h"
#include <map>

namespace Icd {

class Table;
typedef JHandlePtr<Table> TablePtr;

class FrameItem;
class FrameItemData;
typedef JHandlePtr<FrameItem> FrameItemPtr;
typedef std::map<icd_uint64/*code*/, TablePtr> TablePtrMap;

/**
 * @brief The FrameItem class
 */
class ICDCORE_EXPORT FrameItem : public Item
{
public:
    /**
     * @brief FrameItem
     * @param parent
     */
    explicit FrameItem(Object *parent = 0);

    /**
     * @brief FrameItem
     * @param id
     * @param parent
     */
    explicit FrameItem(const std::string &id, Object *parent = 0);

    ~FrameItem();

    /**
     * @brief addTable
     * @param table
     */
    void addTable(const TablePtr &table);

    /**
     * @brief 删除子帧
     * @param [in] code : 帧识别码
     */
    void removeTable(icd_uint64 code);

    /**
     * @brief clearTable
     */
    void clearTable();

    /**
     * @brief 重新指定帧识别码
     * @param [in] oldCode : 旧的帧识别码
     * @param [in] newCode : 新的识别码
     * @return true:重新指定成功，false:重新指定失败，如果新制定的帧识别码已经存在，则返回失败
     */
    bool replaceCode(icd_uint64 oldCode, icd_uint64 newCode);

    /**
     * @brief allTable
     * @return
     */
    TablePtrMap allTable();

    /**
     * @brief allTable
     * @return
     */
    const TablePtrMap &allTable() const;

    /**
     * @brief tableCount
     * @return
     */
    int tableCount() const;

    /**
     * @brief 获取指定帧识别码的子帧表对象
     * @param [in] code : 帧识别码
     * @return 子帧表对象
     */
    TablePtr tableAt(icd_uint64 code);

    /**
     * @brief 获取数值，无意义
     * @return
     */
    double data() const { return 0.0; }

    /**
     * @brief 设置数值，无意义
     * @param d
     */
    void setData(double) { }

    /**
     * @brief 获取数值字符串，为连续字节的十六进制表示
     * @return
     */
    std::string dataString() const;

    /**
     * @brief 绑定数据指针
     * @param [in] buffer ：外部程序指定的数据块指针
     */
    void setBuffer(char *buffer);

    /**
     * @brief setBufferSize
     * @param size
     */
    void setBufferSize(double size);

    /**
     * @brief 获取发送时序（比例）（默认为 1）
     * @return
     */
    int sequenceCount() const;

    /**
     * @brief 设置发送时序数（默认为 1）
     * @param sequence
     */
    void setSequenceCount(int count);

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
    FrameItem &operator =(const FrameItem &other);

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
     * @brief updateSend
     * @param code
     * @return
     */
    int updateSend(icd_uint64 code);

    /**
     * @brief updateRecv
     * @param code
     */
    void updateRecv(icd_uint64 code);

    /**
     * @brief resetSend
     */
    void resetSend();

    // Serializable interface
public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int deep = -1);

protected:
    /**
     * @brief FrameItem
     * @param other
     */
    FrameItem(const FrameItem &other);

    /**
     * @brief setBufferOffset
     * @param offset
     */
    void setBufferOffset(double offset);

private:
    JHandlePtr<FrameItemData> d;
};

} // end of namespace Icd

#endif // ICD_ITEM_FRAME_H
