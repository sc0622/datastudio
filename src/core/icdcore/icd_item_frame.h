#ifndef ICD_ITEM_FRAME_H
#define ICD_ITEM_FRAME_H

#include "icdcore_global.h"
#include "icd_item.h"
#include <map>

namespace Icd {

class Table;
typedef std::shared_ptr<Table> TablePtr;

class FrameItem;
class FrameItemData;
typedef std::shared_ptr<FrameItem> FrameItemPtr;
typedef std::map<icd_uint64/*code*/, TablePtr> TablePtrMap;

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

    double data() const final { return 0.0; }
    void setData(double) final { }
    std::string dataString() const final;

    void setBuffer(char *buffer) final;
    void setBufferSize(double size) final;

    int sequenceCount() const;
    void setSequenceCount(int count);

    std::string typeName() const final;

    int childCount() const final;
    void resetData() final;
    void clearData() final;

    Object *clone() const final;
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
    Json::Value save() const final;
    bool restore(const Json::Value &json, int deep = -1) final;

protected:
    FrameItem(const FrameItem &other);

    void setBufferOffset(double offset) final;

private:
    FrameItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_FRAME_H
