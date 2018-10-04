#ifndef ICD_TABLE_H
#define ICD_TABLE_H

#include "icdcore_global.h"
#include "icd_item.h"
#include <vector>

namespace Icd {

class CounterItem;
typedef std::shared_ptr<CounterItem> CounterItemPtr;

class CheckItem;
typedef std::shared_ptr<CheckItem> CheckItemPtr;

class FrameCodeItem;
typedef std::shared_ptr<FrameCodeItem> FrameCodeItemPtr;
typedef std::vector<FrameCodeItemPtr> FrameCodeItemPtrArray;

class BitItem;
typedef std::shared_ptr<BitItem> BitItemPtr;

class Table;
class TableData;
typedef std::shared_ptr<Table> TablePtr;
typedef std::vector<TablePtr> TablePtrArray;

class ICDCORE_EXPORT Table : public Object
{
public:
    explicit Table(Object *parent = nullptr);
    explicit Table(const std::string &id, Object *parent = nullptr);
    Table(const Table &other);
    ~Table() override;

    int rtti() const override;

    int itemOffset() const;
    double bufferSize() const;
    double bufferOffset() const;
    char *buffer() const;
    void setBuffer(char *buffer);

    ItemPtrArray allItem();
    const ItemPtrArray &allItem() const;
    void appendItem(const ItemPtr &item);
    void clearItem();
    bool isEmpty() const;
    int itemCount() const;
    ItemPtr itemAt(int index) const;
    ItemPtr itemById(const std::string &id) const;
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
    void removeChild(icd_uint64 index) override;
    void clearChildren() override;

    const std::vector<char> &headers() const;
    CounterItemPtr counterItem();
    const CounterItemPtr &counterItem() const;
    bool isCheckValid() const;
    CheckItemPtr checkItem();
    const CheckItemPtr &checkItem() const;
    const FrameCodeItemPtrArray &allFrameCode() const;

    void updateSend(bool period);
    void updateRecv();
    void resetSend();

    void bindBuffer(void *buffer);

    int sequence() const;
    void setSequence(int sequence);
    int period() const;
    bool isFrameTable() const;
    bool isSubFrameTable() const;
    int frameCodeType() const;
    icd_uint64 frameCode() const;
    std::string typeName() const;
    std::string codeName() const;

    void setParent(Object *parent) override;
    int childCount() const override;
    void resetData() override;
    void clearData() override;

    ObjectPtr copy() const override;
    ObjectPtr clone() const override;
    Table &operator =(const Table &other);

    static double recalcBitBufferOffset(const Icd::BitItemPtr &bitItem,
                                        const Icd::ItemPtrArray &items,
                                        Icd::ItemPtrArray::const_reverse_iterator citer);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

private:
    void setItemOffset(int offset);
    void setBufferOffset(double offset);
    void setSubFrameTableFlag(bool flag);

private:
    TableData *d;
    friend class ComplexItem;
    friend class FrameItem;
};

} // end of namespace Icd

#endif // ICD_TABLE_H
