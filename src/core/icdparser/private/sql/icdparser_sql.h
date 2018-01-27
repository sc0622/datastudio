#ifndef ICDPARSER_SQL_H
#define ICDPARSER_SQL_H

#include "icdparser_global.h"
#include "icdparser.h"

namespace Icd {

class SqlParser;
class SqlParserData;
typedef JHandlePtr<SqlParser> SqlParserPtr;

class SqlParser : public Parser
{
public:
    explicit SqlParser(const Json::Value &config);
    virtual ~SqlParser();

    static SqlParserPtr create(const Json::Value &config);

    bool parseVehicle(Icd::VehiclePtrArray &vehicles, int deep);
    bool parseVehicle(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep);
    bool parseSystem(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep);
    bool parseSystem(const std::string &vehicleId, const std::string &systemId,
                     Icd::SystemPtr &system, int deep);
    bool parseTable(const std::string &vehicleId, const std::string &systemId,
                    Icd::TablePtrArray &tables, int deep);
    bool parseTable(const std::string &vehicleId, const std::string &systemId,
                    const std::string &tableId, TablePtr &table, int deep);
    bool parseTable(Icd::TablePtrArray &tables);
    bool parseTable(const std::string &tableId, Icd::TablePtr &table);
    bool parseItem(const std::string &vehicleId, const std::string &systemId,
                   const std::string &tableId, Icd::ItemPtrArray &items, int deep);
    bool parseItem(const std::string &vehicleId, const std::string &systemId,
                   const std::string &tableId, const std::string &itemId,
                   Icd::ItemPtr &item, int deep);
    bool save(const VehiclePtrArray &vehicles);
    bool save(const Icd::TablePtrArray &tables);
    bool save(const Icd::TablePtr &table);

private:
    SqlParserData *d;
};

} // end of namespace Icd

#endif // ICDPARSER_SQL_H
