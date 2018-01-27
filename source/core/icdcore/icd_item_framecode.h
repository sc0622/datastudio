#ifndef ICD_ITEM_FRAMECODE_H
#define ICD_ITEM_FRAMECODE_H

#include "icdcore_global.h"
#include "icd_item_frame.h"
#include <map>

namespace Icd {

/**
 * @brief The FrameCodeType enum
 */
enum FrameCodeType {
    FrameCodeInvalid = -1,
    FrameCodeU8,
    FrameCodeU16,
    FrameCodeU32,
    FrameCodeU64,
    FrameCodeTotal
};

class FrameCodeItem;
class FrameCodeItemData;
typedef JHandlePtr<FrameCodeItem> FrameCodeItemPtr;
typedef std::vector<FrameCodeItemPtr> FrameCodeItemPtrArray;

/**
 * @brief 帧识别码数据项
 */
class ICDCORE_EXPORT FrameCodeItem : public Item
{
public:
    /**
     * @brief FrameCodeItem
     * @param parent
     */
    explicit FrameCodeItem(Object *parent = 0);

    /**
     * @brief FrameCodeItem
     * @param id
     * @param parent
     */
    explicit FrameCodeItem(const std::string &id, Object *parent = 0);

    ~FrameCodeItem();

    /**
     * @brief frameCodeType
     * @return
     */
    FrameCodeType frameCodeType() const;

    /**
     * @brief setFrameCodeType
     * @param type
     */
    void setFrameCodeType(FrameCodeType type);

    /**
     * @brief frameCodeTypeString
     * @return
     */
    std::string frameCodeTypeString() const;

    /**
     * @brief frameCodeTypeString
     * @param type
     * @return
     */
    static std::string frameCodeTypeString(FrameCodeType type);

    /**
     * @brief stringFrameCodeType
     * @param str
     * @return
     */
    static FrameCodeType stringFrameCodeType(const std::string &str);

    /**
     * @brief 获取帧识别码数据
     * @return
     */
    double data() const;

    /**
     * @brief 设置帧识别码数据值
     * @param [in] data : 数值
     */
    void setData(double data);

    /**
     * @brief 获取帧识别码数据十六进制字符串
     * @return
     */
    std::string dataString() const;

    /**
     * @brief frameId
     * @return
     */
    std::string frameId() const;

    /**
     * @brief setFrameId
     * @param id
     */
    void setFrameId(const std::string &id);

    /**
     * @brief 获取数据帧
     * @return
     */
    FrameItemPtr frame() const;

    /**
     * @brief 绑定数据帧
     * @param [in] frame : 数据帧
     */
    void setFrame(const FrameItemPtr &frame);

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
    FrameCodeItem &operator =(const FrameCodeItem &other);

    // Serializable interface
public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int deep = -1);

protected:
    /**
     * @brief FrameCodeItem
     * @param other
     */
    FrameCodeItem(const FrameCodeItem &other);

private:
    JHandlePtr<FrameCodeItemData> d;
};

} // end of namespace Icd

#endif // ICD_ITEM_FRAMECODE_H
