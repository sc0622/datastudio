#ifndef ICD_ITEM_FRAMECODE_H
#define ICD_ITEM_FRAMECODE_H

#include "icdcore_global.h"
#include "icd_item_frame.h"
#include <map>

namespace Icd {

//
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
typedef std::shared_ptr<FrameCodeItem> FrameCodeItemPtr;
typedef std::vector<FrameCodeItemPtr> FrameCodeItemPtrArray;

class ICDCORE_EXPORT FrameCodeItem : public Item
{
public:
    explicit FrameCodeItem(Object *parent = nullptr);
    explicit FrameCodeItem(const std::string &id, Object *parent = nullptr);
    ~FrameCodeItem();

    FrameCodeType frameCodeType() const;
    void setFrameCodeType(FrameCodeType type);
    std::string frameCodeTypeString() const;
    static std::string frameCodeTypeString(FrameCodeType type);
    static FrameCodeType stringFrameCodeType(const std::string &str);

    double data() const final;
    void setData(double data) final;
    std::string dataString() const final;

    std::string frameId() const;
    void setFrameId(const std::string &id);
    FrameItemPtr frame() const;
    void setFrame(const FrameItemPtr &frame);

    void updateSend(bool period);
    void updateRecv();
    void resetSend();
    void resetData() final;
    void clearData() final;

    std::string typeName() const final;
    std::string typeString() const final;

    Object *clone() const final;
    FrameCodeItem &operator =(const FrameCodeItem &other);

    // Serializable interface
public:
    Json::Value save() const final;
    bool restore(const Json::Value &json, int deep = -1) final;

protected:
    FrameCodeItem(const FrameCodeItem &other);

private:
    FrameCodeItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_FRAMECODE_H
