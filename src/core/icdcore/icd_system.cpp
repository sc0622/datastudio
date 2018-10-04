#include "precomp.h"
#include "icd_system.h"
#include "icd_vehicle.h"
#include <assert.h>

namespace Icd {

class SystemData
{
    friend class System;
public:
    SystemData()
    {

    }

private:
    TablePtrArray tables;
};

System::System(Object *parent)
    : Object(ObjectSystem, parent)
    , d(new SystemData())
{

}

System::System(const std::string &id, Object *parent)
    : Object(id, ObjectSystem, parent)
    , d(new SystemData())
{

}

System::System(const System &other)
    : Object(other)
    , d(new SystemData())
{
    *this = other;
}

System::~System()
{
    delete d;
}

int System::rtti() const
{
    return ObjectSystem;
}

TablePtrArray System::allTable()
{
    return d->tables;
}

const TablePtrArray &System::allTable() const
{
    return d->tables;
}

void System::appendTable(const TablePtr &table)
{
    table->setParent(this);
    d->tables.push_back(table);
}

void System::insertTable(int index, const TablePtr &table)
{
    index = jBound(0, index, int(d->tables.size()));
    table->setParent(this);
    d->tables.insert(d->tables.cbegin() + index, table);
}

void System::removeTable(int index)
{
    if (index < 0 || index >= static_cast<int>(d->tables.size())) {
        return;     // overflow
    }

    d->tables.erase(d->tables.cbegin() + index);
}

void System::removeTableByMark(const std::string &mark)
{
    for (TablePtrArray::const_iterator citer = d->tables.cbegin();
         citer != d->tables.cend(); ++citer) {
        const TablePtr &table = *citer;
        if(table->mark() == mark) {
            d->tables.erase(citer);
            break;
        }
    }
}

void System::clearTable()
{
    d->tables.clear();
}

int System::tableCount() const
{
    return static_cast<int>(d->tables.size());
}

TablePtr System::tableAt(int index) const
{
    if (index < 0 || index >= static_cast<int>(d->tables.size())) {
        return TablePtr();
    }

    return d->tables.at(static_cast<size_t>(index));
}

TablePtr System::tableByName(const std::string &name) const
{
    for (TablePtrArray::const_iterator citer = d->tables.cbegin();
         citer != d->tables.cend(); ++citer) {
        const TablePtr &table = *citer;
        if(table->name() == name) {
            return table;
        }
    }

    return TablePtr();
}

TablePtr System::tableByMark(const std::string &mark) const
{
    for (TablePtrArray::const_iterator citer = d->tables.cbegin();
         citer != d->tables.cend(); ++citer) {
        const TablePtr &table = *citer;
        if(table->mark() == mark) {
            return table;
        }
    }

    return TablePtr();
}

bool System::isEmpty() const
{
    return d->tables.empty();
}

int System::childCount() const
{
    int count = static_cast<int>(d->tables.size());

    // children
    for (TablePtrArray::const_iterator citer = d->tables.cbegin();
         citer != d->tables.end(); ++citer) {
        count += (*citer)->childCount();
    }

    return count;
}

void System::resetData()
{
    for (TablePtrArray::const_iterator citer = d->tables.cbegin();
         citer != d->tables.end(); ++citer) {
        (*citer)->resetData();
    }
}

void System::clearData()
{
    for (TablePtrArray::const_iterator citer = d->tables.cbegin();
         citer != d->tables.end(); ++citer) {
        (*citer)->clearData();
    }
}

ObjectPtr System::copy() const
{
    SystemPtr newSystem = std::make_shared<System>(*this);
    newSystem->setParent(nullptr);
    return newSystem;
}

ObjectPtr System::clone() const
{
    SystemPtr newSystem = std::make_shared<System>(*this);
    newSystem->setParent(nullptr);
    // children
    const TablePtrArray &tables = d->tables;
    for (TablePtrArray::const_iterator citer = tables.cbegin();
         citer != tables.cend(); ++citer) {
        newSystem->appendTable(JHandlePtrCast<Table>((*citer)->clone()));
    }
    return newSystem;
}

System &System::operator =(const System &other)
{
    if (this == &other) {
        return *this;
    }
    Object::operator =(other);
    d->tables = other.allTable();
    return *this;
}

ObjectPtr System::findByDomain(const std::string &domain, int domainType,
                               bool ignoreComplex) const
{
    if (domain.empty()) {
        return Icd::ObjectPtr();
    }

    std::string::size_type index = domain.find_first_of('/');
    std::string current, next;
    if (index == std::string::npos) {
        current = domain;
    } else {
        current = domain.substr(0, index);
        next = domain.substr(index + 1);
    }

    for (TablePtrArray::const_iterator citer = d->tables.cbegin();
         citer != d->tables.cend(); ++citer) {
        const TablePtr &table = *citer;
        if (table->domainOfType(domainType) == current) {
            if (next.empty()) {
                return table;
            } else {
                Icd::ObjectPtr object = table->findByDomain(next, domainType, ignoreComplex);
                if (object) {
                    return object;
                }
            }
        }
    }

    return nullptr;
}

bool System::hasChildByName(const std::string &name, const Icd::ObjectPtr &exclude) const
{
    for (TablePtrArray::const_iterator citer = d->tables.cbegin();
         citer != d->tables.cend(); ++citer) {
        const TablePtr &table = *citer;
        if (exclude && table->id() == exclude->id()) {
            continue;
        }
        if (table->name() == name) {
            return true;
        }
    }

    return false;
}

bool System::hasChildByMark(const std::string &mark, const Icd::ObjectPtr &exclude) const
{
    for (TablePtrArray::const_iterator citer = d->tables.cbegin();
         citer != d->tables.cend(); ++citer) {
        const TablePtr &table = *citer;
        if (exclude && table->id() == exclude->id()) {
            continue;
        }
        if (table->mark() == mark) {
            return true;
        }
    }

    return false;
}

ObjectPtr System::childAt(icd_uint64 index) const
{
    return tableAt(int(index));
}

ObjectPtr System::replaceChild(icd_uint64 index, ObjectPtr &other)
{
    if (index >= d->tables.size()) {
        return ObjectPtr();
    }

    if (!other || other->objectType() != ObjectTable) {
        return ObjectPtr();
    }

    TablePtrArray::iterator iter = d->tables.begin() + int(index);
    TablePtr old = *iter;
    *iter = JHandlePtrCast<Table>(other);

    return old;
}

void System::removeChild(icd_uint64 index)
{
    if (index >= d->tables.size()) {
        return;
    }

    d->tables.erase(d->tables.cbegin() + int(index));
}

void System::clearChildren()
{
    d->tables.clear();
}

Json::Value System::save() const
{
    Json::Value json = Object::save();

    if (!d->tables.empty()) {
        Json::Value tablesJson(Json::arrayValue);
        for (TablePtrArray::const_iterator citer = d->tables.cbegin();
             citer != d->tables.cend(); ++citer) {
            const TablePtr &table = *citer;
            if (table) {
                tablesJson.append(table->save());
            }
        }
        json["tables"] = tablesJson;
    }

    return json;
}

bool System::restore(const Json::Value &json, int deep)
{
    if (!Object::restore(json, deep)) {
        return false;
    }

    if (deep <= ObjectSystem) {
        return true;
    }

    clearTable();
    if (json.isMember("tables")) {
        Json::Value tablesJson = json["tables"];
        for (Json::ValueConstIterator citer = tablesJson.begin();
             citer != tablesJson.end(); ++citer) {
            const Json::Value &tableJson = *citer;
            TablePtr table(new Table(this));
            if (!table->restore(tableJson, deep)) {
                continue;
            }
            appendTable(table);
        }
    }

    return true;
}

} // end of namespace Icd
