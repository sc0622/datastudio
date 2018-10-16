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
    (void)(config);

    return SqlParserPtr();
}

bool SqlParser::parse(Icd::RootPtr &root, int deep) const
{
    (void)(root);
    (void)(deep);
    return false;
}

bool SqlParser::parse(Icd::VehiclePtrArray &vehicles, int deep, Icd::Object *parent) const
{
    (void)(vehicles);
    (void)(deep);
    (void)(parent);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep,
                      Icd::Object *parent) const
{
    (void)(vehicleId);
    (void)(vehicle);
    (void)(deep);
    (void)(parent);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep,
                      Icd::Object *parent) const
{
    (void)(vehicleId);
    (void)(systems);
    (void)(deep);
    (void)(parent);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, const std::string &systemId,
                      Icd::SystemPtr &system, int deep, Icd::Object *parent) const
{
    (void)(vehicleId);
    (void)(systemId);
    (void)(system);
    (void)(deep);
    (void)(parent);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, const std::string &systemId,
                      Icd::TablePtrArray &tables, int deep, Icd::Object *parent) const
{
    (void)(vehicleId);
    (void)(systemId);
    (void)(tables);
    (void)(deep);
    (void)(parent);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, const std::string &systemId, const std::string &tableId,
                      Icd::TablePtr &table, int deep, Icd::Object *parent) const
{
    (void)(vehicleId);
    (void)(systemId);
    (void)(tableId);
    (void)(table);
    (void)(deep);
    (void)(parent);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId, const std::string &systemId, const std::string &tableId,
                      Icd::ItemPtrArray &items, int deep, Icd::Object *parent) const
{
    (void)(vehicleId);
    (void)(systemId);
    (void)(tableId);
    (void)(items);
    (void)(deep);
    (void)(parent);
    return false;
}

bool SqlParser::parse(Icd::TablePtrArray &tables) const
{
    (void)(tables);
    return false;
}

bool SqlParser::parse(const std::string &tableId, Icd::TablePtr &table) const
{
    (void)(tableId);
    (void)(table);
    return false;
}

bool SqlParser::save(const Icd::RootPtr &root) const
{
    (void)(root);
    return false;
}

bool SqlParser::save(const Icd::VehiclePtrArray &vehicles) const
{
    (void)(vehicles);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const Icd::VehiclePtr &vehicle) const
{
    (void)(vehicleId);
    (void)(vehicle);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const Icd::SystemPtrArray &systems) const
{
    (void)(vehicleId);
    (void)(systems);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const Icd::SystemPtr &system) const
{
    (void)(vehicleId);
    (void)(systemId);
    (void)(system);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const TablePtrArray &tables) const
{
    (void)(vehicleId);
    (void)(systemId);
    (void)(tables);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const std::string &tableId, const Icd::TablePtr &table) const
{
    (void)(vehicleId);
    (void)(systemId);
    (void)(tableId);
    (void)(table);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const std::string &tableId, const Icd::ItemPtrArray &items) const
{
    (void)(vehicleId);
    (void)(systemId);
    (void)(tableId);
    (void)(items);
    return false;
}

bool SqlParser::save(const Icd::TablePtrArray &tables) const
{
    (void)(tables);
    return false;
}

bool SqlParser::save(const std::string &tableId, const Icd::TablePtr &table) const
{
    (void)(tableId);
    (void)(table);
    return false;
}

bool SqlParser::save(const Icd::TablePtr &table) const
{
    (void)(table);
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
