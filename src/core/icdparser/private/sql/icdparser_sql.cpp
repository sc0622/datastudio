#include "precomp.h"
#include "./icdparser_sql.h"
#include "icdparser_sql_p.h"
#include "../icdwidget/icdwidget_global.h"

namespace Icd {

// class SqlParser

SqlParser::SqlParser(const Json::Value &config)
    : Parser(config)
    , d(new SqlParserData())
{

}

SqlParser::~SqlParser()
{
    delete d;
}

SqlParserPtr SqlParser::create(const Json::Value &config)
{
    Q_UNUSED(config);

    return SqlParserPtr();
}

bool SqlParser::parse(RootPtr &root, int deep) const
{
    Q_UNUSED(root);
    Q_UNUSED(deep);
    return false;
}

bool SqlParser::parse(VehiclePtrArray &vehicles, int deep) const
{
    Q_UNUSED(vehicles);
    Q_UNUSED(deep);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, VehiclePtr &vehicle,
                      int deep) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(vehicle);
    Q_UNUSED(deep);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, SystemPtrArray &systems,
                      int deep) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systems);
    Q_UNUSED(deep);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, const std::string &systemId,
                      SystemPtr &system, int deep) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(system);
    Q_UNUSED(deep);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, const std::string &systemId,
                      TablePtrArray &tables, int deep) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tables);
    Q_UNUSED(deep);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, const std::string &systemId,
                      const std::string &tableId, TablePtr &table, int deep) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    Q_UNUSED(deep);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, const std::string &systemId,
                      const std::string &tableId, ItemPtrArray &items, int deep) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(items);
    Q_UNUSED(deep);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, const std::string &systemId,
                      const std::string &tableId, const std::string &itemId, ItemPtr &item,
                      int deep) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(itemId);
    Q_UNUSED(item);
    Q_UNUSED(deep);
    return false;
}

bool SqlParser::parse(TablePtrArray &tables) const
{
    Q_UNUSED(tables);
    return false;
}

bool SqlParser::parse(const std::string &tableId, TablePtr &table) const
{
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    return false;
}

bool SqlParser::save(const RootPtr &root) const
{
    Q_UNUSED(root);
    return false;
}

bool SqlParser::save(VehiclePtrArray &vehicles) const
{
    Q_UNUSED(vehicles);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const VehiclePtr &vehicle) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(vehicle);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const SystemPtrArray &systems) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systems);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const SystemPtr &system) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(system);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const TablePtrArray &tables) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tables);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const std::string &tableId, const TablePtr &table) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const std::string &tableId, const ItemPtrArray &items) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(items);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const std::string &tableId, const std::string &itemId,
                     const ItemPtr &item) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(itemId);
    Q_UNUSED(item);
    return false;
}

bool SqlParser::save(const TablePtrArray &tables) const
{
    Q_UNUSED(tables);
    return false;
}

bool SqlParser::save(const std::string &tableId, const TablePtr &table) const
{
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    return false;
}

bool SqlParser::save(const std::string &domain, const ObjectPtr &object, bool merge,
                     bool fast) const
{
    Q_UNUSED(domain);
    Q_UNUSED(object);
    Q_UNUSED(merge);
    Q_UNUSED(fast);
    return false;
}

bool SqlParser::save(const TablePtr &table) const
{
    Q_UNUSED(table);
    return false;
}

bool SqlParser::beginModify()
{
    return false;
}

bool SqlParser::commitModify()
{
    return false;
}

bool SqlParser::cancelModify()
{
    return false;
}

bool SqlParser::endModify()
{
    return false;
}

} // end of namespace Icd
