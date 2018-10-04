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
    FrameCodeItem(const FrameCodeItem &other);
    ~FrameCodeItem() override;

    int rtti() const override;

    FrameCodeType frameCodeType() const;
    void setFrameCodeType(FrameCodeType type);
    std::string frameCodeTypeString() const;
    static std::string frameCodeTypeString(FrameCodeType type);
    static FrameCodeType stringFrameCodeType(const std::string &str);

    double data() const override;
    void setData(double data) override;
    std::string dataString() const override;
    double dataFromBuffer(const char *buffer) const override;

    std::string frameId() const;
    void setFrameId(const std::string &id);
    FrameItemPtr frame() const;
    void setFrame(const FrameItemPtr &frame);
    bool isFrameValid() const;

    void updateSend(bool period);
    void updateRecv();
    void resetSend();
    void resetData() override;
    void clearData() override;

    std::string typeName() const override;
    std::string typeString() const override;

    ObjectPtr copy() const override;
    ObjectPtr clone() const override;
    FrameCodeItem &operator =(const FrameCodeItem &other);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

private:
    FrameCodeItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_FRAMECODE_H
