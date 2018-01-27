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
    //
    Q_UNUSED(config);

    return SqlParserPtr();
}

bool SqlParser::parseVehicle(Icd::VehiclePtrArray &vehicles, int deep)
{
    Q_UNUSED(vehicles);
    Q_UNUSED(deep);
    return false;
}

bool SqlParser::parseVehicle(const std::string &vehicleId,
                             VehiclePtr &vehicle, int deep)
{
    Q_ASSERT(deep <= Icd::ObjectItem);
    Q_UNUSED(vehicleId);
    Q_UNUSED(vehicle);
    Q_UNUSED(deep);
    return true;
}

bool SqlParser::parseSystem(const std::string &vehicleId,
                            Icd::SystemPtrArray &systems, int deep)
{
    Q_ASSERT(deep <= Icd::ObjectItem);
    Q_UNUSED(vehicleId);
    Q_UNUSED(systems);
    Q_UNUSED(deep);
    return true;
}

bool SqlParser::parseSystem(const std::string &vehicleId,
                            const std::string &systemId,
                            Icd::SystemPtr &system, int deep)
{
    Q_ASSERT(deep <= Icd::ObjectItem);
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(system);
    Q_UNUSED(deep);
    return true;
}

bool SqlParser::parseTable(const std::string &vehicleId, const std::string &systemId,
                           Icd::TablePtrArray &tables, int deep)
{
    Q_ASSERT(deep <= Icd::ObjectItem);
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tables);
    Q_UNUSED(deep);
    return true;
}

bool SqlParser::parseTable(const std::string &vehicleId, const std::string &systemId,
                           const std::string &tableId, TablePtr &table, int deep)
{
    Q_ASSERT(deep <= Icd::ObjectItem);
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    Q_UNUSED(deep);
    return true;
}

bool SqlParser::parseTable(TablePtrArray &tables)
{
    Q_UNUSED(tables);
    return false;
}

bool SqlParser::parseTable(const std::string &tableId, TablePtr &table)
{
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    return false;
}

bool SqlParser::parseItem(const std::string &vehicleId,
                          const std::string &systemId,
                          const std::string &tableId,
                          Icd::ItemPtrArray &items, int deep)
{
    Q_ASSERT(deep <= Icd::ObjectItem);
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(items);
    Q_UNUSED(deep);
    return true;
}

bool SqlParser::parseItem(const std::string &vehicleId,
                          const std::string &systemId,
                          const std::string &tableId,
                          const std::string &itemId,
                          Icd::ItemPtr &item, int deep)
{
    Q_ASSERT(deep <= Icd::ObjectItem);
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(itemId);
    Q_UNUSED(item);
    Q_UNUSED(deep);
    return true;
}

bool SqlParser::save(const Icd::VehiclePtrArray &vehicles)
{
    Q_UNUSED(vehicles);

    return false;
}

bool SqlParser::save(const TablePtrArray &tables)
{
    Q_UNUSED(tables);
    return false;
}

bool SqlParser::save(const TablePtr &table)
{
    Q_UNUSED(table);
    return false;
}

} // end of namespace Icd
