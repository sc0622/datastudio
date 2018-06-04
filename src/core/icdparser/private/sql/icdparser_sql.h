#ifndef ICDPARSER_SQL_H
#define ICDPARSER_SQL_H

#include "../../icdparser_global.h"
#include "../../icdparser.h"

namespace Icd {

class SqlParser;
class SqlParserData;
typedef std::shared_ptr<SqlParser> SqlParserPtr;

class SqlParser : public Parser
{
public:
    explicit SqlParser(const Json::Value &config);
    virtual ~SqlParser();

    static SqlParserPtr create(const Json::Value &config);

    bool parse(RootPtr &root, int deep) const;
    bool parse(Icd::VehiclePtrArray &vehicles, int deep) const;
    bool parse(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep) const;
    bool parse(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep) const;
    bool parse(const std::string &vehicleId, const std::string &systemId,
               Icd::SystemPtr &system, int deep) const;
    bool parse(const std::string &vehicleId, const std::string &systemId,
               Icd::TablePtrArray &tables, int deep) const;
    bool parse(const std::string &vehicleId, const std::string &systemId,
               const std::string &tableId, Icd::TablePtr &table, int deep) const;
    bool parse(const std::string &vehicleId, const std::string &systemId,
               const std::string &tableId, Icd::ItemPtrArray &items, int deep) const;
    bool parse(const std::string &vehicleId, const std::string &systemId,
               const std::string &tableId, const std::string &itemId,
               Icd::ItemPtr &item, int deep) const;
    bool parse(Icd::TablePtrArray &tables) const;
    bool parse(const std::string &tableId, Icd::TablePtr &table) const;

    bool save(const RootPtr &root) const;
    bool save(Icd::VehiclePtrArray &vehicles) const;
    bool save(const std::string &vehicleId, const Icd::VehiclePtr &vehicle) const;
    bool save(const std::string &vehicleId, const Icd::SystemPtrArray &systems) const;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const Icd::SystemPtr &system) const;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const Icd::TablePtrArray &tables) const;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const std::string &tableId, const Icd::TablePtr &table) const;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const std::string &tableId, const Icd::ItemPtrArray &items) const;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const std::string &tableId, const std::string &itemId,
              const Icd::ItemPtr &item) const;
    bool save(const Icd::TablePtrArray &tables) const;
    bool save(const std::string &tableId, const Icd::TablePtr &table) const;
    bool save(const std::string &domain, const Icd::ObjectPtr &object,
              bool merge = false, bool fast = false) const;
    bool save(const Icd::TablePtr &table) const;

    bool beginModify();
    bool commitModify();
    bool cancelModify();
    bool endModify();

private:
    SqlParserData *d;
};

} // end of namespace Icd

#endif // ICDPARSER_SQL_H
