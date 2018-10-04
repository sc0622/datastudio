﻿#include "precomp.h"
#include "icd_item_complex.h"
#include "icd_table.h"

namespace Icd {

class ComplexItemData
{
    friend class ComplexItem;
public:
    ComplexItemData()
        : table(new Table())
    {

    }

private:
    TablePtr table;
};

ComplexItem::ComplexItem(Object *parent)
    : Item(ItemComplex, parent)
    , d(new ComplexItemData())
{
    d->table->setParent(this);
}

ComplexItem::ComplexItem(const std::string &id, Object *parent)
    : Item(id, ItemComplex, parent)
    , d(new ComplexItemData())
{
    d->table->setParent(this);
}

ComplexItem::~ComplexItem()
{
    delete d;
}

int ComplexItem::rtti() const
{
    return ObjectComplex;
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

void ComplexItem::setBufferSize(double size)
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
    newComplex->setParent(nullptr);
    return newComplex;
}

ObjectPtr ComplexItem::clone() const
{
    ComplexItemPtr newComplex = std::make_shared<ComplexItem>(*this);
    newComplex->setParent(nullptr);
    // children
    newComplex->setTable(JHandlePtrCast<Table>(d->table->clone()));
    return newComplex;
}

ComplexItem &ComplexItem::operator =(const ComplexItem &other)
{
    if (this == &other) {
        return *this;
    }
    Item::operator =(other);
    *d->table = *other.table();
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

void ComplexItem::clearChildren()
{
    d->table->clearChildren();
}

Json::Value ComplexItem::save() const
{
    Json::Value json = Item::save();

    json["size"] = bufferSize();

    if (d->table) {
        json["table"] = d->table->save();
    }

    return json;
}

bool ComplexItem::restore(const Json::Value &json, int deep)
{
    if (!Item::restore(json, deep)) {
        return false;
    }

    setBufferSize(json["size"].asInt());

    if (d->table) {
        if (!d->table->restore(json["table"], deep)) {
            return false;
        }
        // ignore table attributes
        d->table->setName(name());
        d->table->setMark(mark());
        //
        if (!d->table->isEmpty()) {
            setBufferSize(std::ceil(d->table->bufferSize()));
        }
    }

    return true;
}

ComplexItem::ComplexItem(const ComplexItem &other)
    : Item(other)
    , d(new ComplexItemData())
{
    operator =(other);
}

void ComplexItem::setBufferOffset(double offset)
{
    Item::setBufferOffset(offset);
    d->table->setBufferOffset(offset);
}

void ComplexItem::setTable(const TablePtr &table)
{
    d->table = table;
}

} // end of namespace Icd
