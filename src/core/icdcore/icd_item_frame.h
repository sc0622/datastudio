﻿#ifndef ICD_ITEM_FRAME_H
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
    FrameItem(const FrameItem &other);
    ~FrameItem() override;

    int rtti() const override;
    bool isSimpleItem() const override;

    void addTable(const TablePtr &table);
    void removeTable(icd_uint64 code);
    void clearTable();
    bool replaceCode(icd_uint64 oldCode, icd_uint64 newCode);
    TablePtrMap allTable();
    const TablePtrMap &allTable() const;
    int tableCount() const;
    TablePtr tableAt(icd_uint64 code) const;
    TablePtr tableById(const std::string &id) const;

    double data() const override { return 0.0; }
    void setData(double) override { }
    std::string dataString() const override;

    void setBuffer(char *buffer) override;
    void setBufferSize(int size) override;

    int sequenceCount() const;
    void setSequenceCount(int count);

    std::string typeName() const override;

    int childCount() const override;
    void resetData() override;
    void clearData() override;

    ObjectPtr copy() const override;
    ObjectPtr clone() const override;
    FrameItem &operator =(const FrameItem &other);

    ObjectPtr itemByName(const std::string &name, bool deep = true) const;
    ObjectPtr itemByMark(const std::string &mark, bool deep = true) const;
    TablePtr tableByMark(const std::string &mark, bool deep = true) const;
    ObjectPtr itemByDomain(const std::string &domain, Icd::DomainType domainType = Icd::DomainId,
                           bool ignoreComplex = true) const;
    TablePtr tableByDomain(const std::string &domain, Icd::DomainType domainType = Icd::DomainId) const;

    ObjectPtr findByDomain(const std::string &domain, int domainType = Icd::DomainId,
                           bool ignoreComplex = true) const override;
    bool hasChildByName(const std::string &name, const Icd::ObjectPtr &exclude = Icd::ObjectPtr()) const override;
    bool hasChildByMark(const std::string &mark, const Icd::ObjectPtr &exclude = Icd::ObjectPtr()) const override;
    ObjectPtr childAt(icd_uint64 index) const override;
    ObjectPtr replaceChild(icd_uint64 index, ObjectPtr &other) override;
    ObjectPtr replaceChild(const std::string &id, ObjectPtr &other) override;
    void removeChild(icd_uint64 beginIndex, int endIndex = -1) override;
    void removeChild(const std::list<icd_uint64> &indexes) override;
    void clearChildren() override;

    icd_uint64 updateSend(icd_uint64 code);
    void updateRecv(icd_uint64 code);
    void resetSend();

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

protected:
    void setBufferOffset(int offset) override;
    void adjustBufferOffset() override;

private:
    FrameItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_FRAME_H
