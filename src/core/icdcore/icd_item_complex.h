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
     ~ComplexItem();

    TablePtr table() const;

    void setBuffer(char *buffer) final;
    void setBufferSize(double size) final;

    std::string typeName() const final;

    int childCount() const final;

    void updateSend(bool period);
    void updateRecv();
    void resetSend();
    void resetData() final;
    void clearData() final;

    Object *clone() const final;
    ComplexItem &operator =(const ComplexItem &other);

    ObjectPtr itemByMark(const std::string &mark, bool deep = true) const;
    TablePtr tableByMark(const std::string &mark, bool deep = true) const;
    ObjectPtr itemByDomain(const std::string &domain,
                           Icd::DomainType domainType = Icd::DomainId) const;
    TablePtr tableByDomain(const std::string &domain,
                           Icd::DomainType domainType = Icd::DomainId) const;

    // Serializable interface
public:
    Json::Value save() const final;
    bool restore(const Json::Value &json, int deep = -1) final;

protected:
    ComplexItem(const ComplexItem &other);

    void setBufferOffset(double offset) final;

private:
    ComplexItemData *d;
};

} // end of namespace Icd

#endif // ICD_ITEM_COMPLEX_H
