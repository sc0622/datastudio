﻿#include "precomp.h"
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

System::System(Vehicle *parent)
    : Object(ObjectSystem, parent)
    , d(new SystemData())
{

}

System::System(const std::string &id, Vehicle *parent)
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
    if (index < 0 || index >= static_cast<int>(d->tables.size())) {
        return;
    }

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

Object *System::clone() const
{
    return new System(*this);
}

System &System::operator =(const System &other)
{
    Object::operator =(other);

    //
    d->tables.clear();
    const TablePtrArray &tables = other.d->tables;
    for (TablePtrArray::const_iterator citer = tables.cbegin();
         citer != tables.cend(); ++citer) {
        appendTable(TablePtr(reinterpret_cast<Table *>((*citer)->clone())));
    }

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
