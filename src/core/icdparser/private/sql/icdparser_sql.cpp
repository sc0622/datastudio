#include "precomp.h"
#include "./icdparser_sql.h"
#include "icdparser_sql_p.h"
#include "icdwidget/icdwidget_global.h"

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

bool SqlParser::parse(Icd::RootPtr &root, int deep) const
{
    Q_UNUSED(root);
    Q_UNUSED(deep);
    return false;
}

bool SqlParser::parse(Icd::VehiclePtrArray &vehicles, int deep, Icd::Object *parent) const
{
    Q_UNUSED(vehicles);
    Q_UNUSED(deep);
    Q_UNUSED(parent);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep,
                      Icd::Object *parent) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(vehicle);
    Q_UNUSED(deep);
    Q_UNUSED(parent);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep,
                      Icd::Object *parent) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systems);
    Q_UNUSED(deep);
    Q_UNUSED(parent);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, const std::string &systemId,
                      Icd::SystemPtr &system, int deep, Icd::Object *parent) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(system);
    Q_UNUSED(deep);
    Q_UNUSED(parent);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, const std::string &systemId,
                      Icd::TablePtrArray &tables, int deep, Icd::Object *parent) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tables);
    Q_UNUSED(deep);
    Q_UNUSED(parent);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, const std::string &systemId, const std::string &tableId,
                      Icd::TablePtr &table, int deep, Icd::Object *parent) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    Q_UNUSED(deep);
    Q_UNUSED(parent);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, const std::string &systemId, const std::string &tableId,
                      Icd::ItemPtrArray &items, int deep, Icd::Object *parent) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(items);
    Q_UNUSED(deep);
    Q_UNUSED(parent);
    return false;
}

bool SqlParser::parse(Icd::TablePtrArray &tables) const
{
    Q_UNUSED(tables);
    return false;
}

bool SqlParser::parse(const std::string &tableId, Icd::TablePtr &table) const
{
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    return false;
}

bool SqlParser::save(const Icd::RootPtr &root) const
{
    Q_UNUSED(root);
    return false;
}

bool SqlParser::save(const Icd::VehiclePtrArray &vehicles) const
{
    Q_UNUSED(vehicles);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const Icd::VehiclePtr &vehicle) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(vehicle);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const Icd::SystemPtrArray &systems) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systems);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const Icd::SystemPtr &system) const
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
                     const std::string &tableId, const Icd::TablePtr &table) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const std::string &tableId, const Icd::ItemPtrArray &items) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(items);
    return false;
}

bool SqlParser::save(const Icd::TablePtrArray &tables) const
{
    Q_UNUSED(tables);
    return false;
}

bool SqlParser::save(const std::string &tableId, const Icd::TablePtr &table) const
{
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    return false;
}

bool SqlParser::save(const Icd::TablePtr &table) const
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
