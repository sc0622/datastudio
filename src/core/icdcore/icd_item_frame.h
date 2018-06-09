#ifndef ICD_ITEM_FRAME_H
#define ICD_ITEM_FRAME_H

#include "icdcore_global.h"
#include "icd_item.h"
#include <unordered_map>

namespace Icd {

class Table;
typedef std::shared_ptr<Table> TablePtr;

class FrameItem;
class FrameItemData;
typedef std::shared_ptr<FrameItem> FrameItemPtr;
typedef std::unordered_map<icd_uint64/*code*/, TablePtr> TablePtrMap;

class ICDCORE_EXPORT FrameItem : public Item
{
public:
    explicit FrameItem(Object *parent = nullptr);
    explicit FrameItem(const std::string &id, Object *parent = nullptr);

    ~FrameItem();

    void addTable(const TablePtr &table);
    void removeTable(icd_uint64 code);
    void clearTable();
    bool replaceCode(icd_uint64 oldCode, icd_uint64 newCode);
    TablePtrMap allTable();
    const TablePtrMap &allTable() const;
    int tableCount() const;
    TablePtr tableAt(icd_uint64 code);

    double data() const override { return 0.0; }
    void setData(double) override { }
    std::string dataString() const override;

    void setBuffer(char *buffer) override;
    void setBufferSize(double size) override;

    int sequenceCount() const;
    void setSequenceCount(int count);

    std::string typeName() const override;

    int childCount() const override;
    void resetData() override;
    void clearData() override;

    Object *clone() const override;
    FrameItem &operator =(const FrameItem &other);

    ObjectPtr itemByMark(const std::string &mark, bool deep = true) const;
    TablePtr tableByMark(const std::string &mark, bool deep = true) const;
    ObjectPtr itemByDomain(const std::string &domain,
                           Icd::DomainType domainType = Icd::DomainId) const;
    TablePtr tableByDomain(const std::string &domain,
                           Icd::DomainType domainType = Icd::DomainId) const;

    icd_uint64 updateSend(icd_uint64 code);
    void updateRecv(icd_uint64 code);
    void resetSend();

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

protected:
    FrameItem(const FrameItem &other);

    void setBufferOffset(double offset) override;

private:
    FrameItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_FRAME_H
