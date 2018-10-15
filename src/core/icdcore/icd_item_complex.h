#ifndef ICD_ITEM_COMPLEX_H
#define ICD_ITEM_COMPLEX_H

#include "icdcore_global.h"
#include "icd_item.h"

namespace Icd {

class Table;
typedef std::shared_ptr<Table> TablePtr;

class ComplexItem;
class ComplexItemData;
typedef std::shared_ptr<ComplexItem> ComplexItemPtr;

class ICDCORE_EXPORT ComplexItem : public Item
{
public:
    explicit ComplexItem(Object *parent = nullptr);
    explicit ComplexItem(const std::string &id, Object *parent = nullptr);
    ComplexItem(const ComplexItem &other);
    ~ComplexItem() override;

    int rtti() const override;
    bool isSimpleItem() const override;

    bool isEmpty() const;
    TablePtr table() const;

    void setBuffer(char *buffer) override;
    void setBufferSize(int size) override;

    std::string typeName() const override;

    int childCount() const override;

    void updateSend(bool period);
    void updateRecv();
    void resetSend();
    void resetData() override;
    void clearData() override;

    ObjectPtr copy() const override;
    ObjectPtr clone() const override;
    ComplexItem &operator =(const ComplexItem &other);

    ObjectPtr itemByMark(const std::string &mark, bool deep = true) const;
    TablePtr tableByMark(const std::string &mark, bool deep = true) const;
    ObjectPtr itemByDomain(const std::string &domain, Icd::DomainType domainType = Icd::DomainId,
                           bool ignoreComplex = true) const;
    TablePtr tableByDomain(const std::string &domain, Icd::DomainType domainType = Icd::DomainId) const;

    ObjectPtr findByDomain(const std::string &domain, int domainType = Icd::DomainId,
                           bool ignoreComplex = true) const override;
    ObjectPtr replaceChild(icd_uint64 index, ObjectPtr &other) override;
    ObjectPtr replaceChild(const std::string &id, ObjectPtr &other) override;
    void moveChild(int sourceIndex, int targetIndex) override;
    void removeChild(icd_uint64 beginIndex, int endIndex = -1) override;
    void removeChild(const std::list<icd_uint64> &indexes) override;
    void clearChildren() override;

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

protected:
    void setBufferOffset(int offset) override;
    void adjustBufferOffset() override;

private:
    void setTable(const Icd::TablePtr &table);

private:
    ComplexItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_COMPLEX_H
