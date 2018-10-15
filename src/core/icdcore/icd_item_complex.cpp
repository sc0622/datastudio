#include "precomp.h"
#include "icd_item_complex.h"
#include "icd_table.h"

namespace Icd {

class ComplexItemData
{
    friend class ComplexItem;
public:
    ComplexItemData()
    {

    }

private:
    TablePtr table;
};

ComplexItem::ComplexItem(Object *parent)
    : Item(ItemComplex, parent)
    , d(new ComplexItemData())
{
    d->table = std::make_shared<Table>(this);
}

ComplexItem::ComplexItem(const std::string &id, Object *parent)
    : Item(id, ItemComplex, parent)
    , d(new ComplexItemData())
{
    d->table = std::make_shared<Table>(id, this);
}

ComplexItem::~ComplexItem()
{
    delete d;
}

int ComplexItem::rtti() const
{
    return ObjectComplex;
}

bool ComplexItem::isSimpleItem() const
{
    return false;
}

bool ComplexItem::isEmpty() const
{
    return (d->table->itemCount() == 0);
}

TablePtr ComplexItem::table() const
{
    return d->table;
}

void ComplexItem::setBuffer(char *buffer)
{
    Item::setBuffer(buffer);
    d->table->setBuffer(buffer);
}

void ComplexItem::setBufferSize(int size)
{
    Item::setBufferSize(size);
}

std::string ComplexItem::typeName() const
{
    return "Complex_" + codeName();
}

int ComplexItem::childCount() const
{
    return d->table->childCount();
}

void ComplexItem::updateSend(bool period)
{
    d->table->updateSend(period);
}

void ComplexItem::updateRecv()
{
    d->table->updateRecv();
}

void ComplexItem::resetSend()
{
    d->table->resetSend();
}

void ComplexItem::resetData()
{
    d->table->resetData();
}

void ComplexItem::clearData()
{
    d->table->clearData();
}

ObjectPtr ComplexItem::copy() const
{
    ComplexItemPtr newComplex = std::make_shared<ComplexItem>(*this);
    return newComplex;
}

ObjectPtr ComplexItem::clone() const
{
    ComplexItemPtr newComplex = std::make_shared<ComplexItem>(*this);
    // children
    TablePtr newTable = JHandlePtrCast<Table>(d->table->clone());
    newTable->setParent(newComplex.get());
    newComplex->setTable(newTable);
    return newComplex;
}

ComplexItem &ComplexItem::operator =(const ComplexItem &other)
{
    if (this == &other) {
        return *this;
    }
    Item::operator =(other);
    d->table = other.d->table;
    return *this;
}

ObjectPtr ComplexItem::itemByMark(const std::string &mark, bool deep) const
{
    return d->table->itemByMark(mark, deep);
}

TablePtr ComplexItem::tableByMark(const std::string &mark, bool deep) const
{
    return d->table->tableByMark(mark, deep);
}

ObjectPtr ComplexItem::itemByDomain(const std::string &domain, DomainType domainType,
                                    bool ignoreComplex) const
{
    if (domain.empty()) {
        return ObjectPtr();
    }

    std::string::size_type index = domain.find_first_of('/');
    std::string current, next;
    if (index == std::string::npos) {
        current = domain;
    } else {
        current = domain.substr(0, index);
        next = domain.substr(index + 1);
    }

    if (d->table->domainOfType(domainType) != current) {
        return ObjectPtr();
    }

    if (next.empty()) {
        return d->table;
    } else {
        return d->table->itemByDomain(next, domainType, ignoreComplex);
    }
}

TablePtr ComplexItem::tableByDomain(const std::string &domain,
                                    DomainType domainType) const
{
    ObjectPtr item = itemByDomain(domain, domainType);
    if (!item || item->objectType() != ObjectTable) {
        return nullptr;
    }

    return JHandlePtrCast<Table>(item);
}

ObjectPtr ComplexItem::findByDomain(const std::string &domain, int domainType,
                                    bool ignoreComplex) const
{
    return itemByDomain(domain, Icd::DomainType(domainType), ignoreComplex);
}

ObjectPtr ComplexItem::replaceChild(icd_uint64 index, ObjectPtr &other)
{
    return d->table->replaceChild(index, other);
}

ObjectPtr ComplexItem::replaceChild(const std::string &id, ObjectPtr &other)
{
    return d->table->replaceChild(id, other);
}

void ComplexItem::moveChild(int sourceIndex, int targetIndex)
{
    d->table->moveChild(sourceIndex, targetIndex);
}

void ComplexItem::removeChild(icd_uint64 beginIndex, int endIndex)
{
    d->table->removeChild(beginIndex, endIndex);
}

void ComplexItem::removeChild(const std::list<icd_uint64> &indexes)
{
    d->table->removeChild(indexes);
}

void ComplexItem::clearChildren()
{
    d->table->clearChildren();
}

Json::Value ComplexItem::save() const
{
    Json::Value json = Item::save();
    // table
    json["table"] = d->table->save();

    return json;
}

bool ComplexItem::restore(const Json::Value &json, int deep)
{
    if (!Item::restore(json, deep)) {
        return false;
    }

    // table
    if (json.isMember("table")) {
        if (!d->table->restore(json["table"], deep)) {
            return false;
        }
    }
    // ignore table attributes
    d->table->setName(name());
    d->table->setMark(mark());
    d->table->setDesc(desc());
    // size
    if (!d->table->isEmpty()) {
        setBufferSize(d->table->bufferSize());
    }

    return true;
}

ComplexItem::ComplexItem(const ComplexItem &other)
    : Item(other)
    , d(new ComplexItemData())
{
    operator =(other);
}

void ComplexItem::setBufferOffset(int offset)
{
    Item::setBufferOffset(offset);
    d->table->setBufferOffset(offset);
}

void ComplexItem::adjustBufferOffset()
{
    d->table->setBufferOffset(bufferOffset());
}

void ComplexItem::setTable(const TablePtr &table)
{
    d->table = table;
}

} // end of namespace Icd
