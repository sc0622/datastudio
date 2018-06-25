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

class Table;
class TableData;
typedef std::shared_ptr<Table> TablePtr;
typedef std::vector<TablePtr> TablePtrArray;

class ICDCORE_EXPORT Table : public Object
{
public:
    explicit Table(Object *parent = nullptr);
    explicit Table(const std::string &id, Object *parent = nullptr);
    ~Table();

    int itemOffset() const;
    double bufferSize() const;
    double bufferOffset() const;
    char *buffer() const;
    void setBuffer(char *buffer);

    ItemPtrArray allItem();
    const ItemPtrArray &allItem() const;
    void appendItem(const ItemPtr &item);
    void insertItem(int index, const ItemPtr &item);
    void removeItem(int index);
    void removeItemById(const std::string &id);
    void removeItemByMark(const std::string &mark);
    void clearItem();
    bool isEmpty() const;
    int itemCount() const;
    ItemPtr itemAt(int index) const;
    ItemPtr itemById(const std::string &id) const;
    ObjectPtr itemByMark(const std::string &mark, bool deep = true) const;
    TablePtr tableByMark(const std::string &mark, bool deep = true) const;
    ObjectPtr itemByDomain(const std::string &domain, Icd::DomainType domainType = Icd::DomainId,
                           bool ignoreComplex = true) const;
    TablePtr tableByDomain(const std::string &domain, Icd::DomainType domainType = Icd::DomainId) const;

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
    std::string typeName() const;
    std::string codeName() const;

    void setParent(Object *parent) override;
    int childCount() const override;
    void resetData() override;
    void clearData() override;

    Object *clone() const override;
    Table &operator =(const Table &other);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

protected:
    Table(const Table &other);

private:
    void setItemOffset(int offset);
    void setBufferOffset(double offset);

private:
    TableData *d;
    friend class ComplexItem;
    friend class FrameItem;
};

} // end of namespace Icd

#endif // ICD_TABLE_H
