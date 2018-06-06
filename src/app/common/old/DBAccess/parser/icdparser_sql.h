#ifndef ICDPARSER_SQL_H
#define ICDPARSER_SQL_H

#include "icdparser/icdparser_global.h"
#include "icdparser/icdparser.h"

namespace Icd {

class SqlParser;
class SqlParserData;
typedef std::shared_ptr<SqlParser> SqlParserPtr;

class SqlParser : public Parser
{
public:
    explicit SqlParser(const Json::Value &config);

    ~SqlParser();

    bool parse(RootPtr &root, int deep) const override;
    bool parse(Icd::VehiclePtrArray &vehicles, int deep) const override;
    bool parse(const ::std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep) const override;
    bool parse(const ::std::string &vehicleId, Icd::SystemPtrArray &systems, int deep) const override;
    bool parse(const ::std::string &vehicleId, const ::std::string &systemId,
               Icd::SystemPtr &system, int deep) const override;
    bool parse(const ::std::string &vehicleId, const ::std::string &systemId,
               Icd::TablePtrArray &tables, int deep) const override;
    bool parse(const ::std::string &vehicleId, const ::std::string &systemId,
               const ::std::string &tableId, Icd::TablePtr &table, int deep) const override;
    bool parse(const ::std::string &vehicleId, const ::std::string &systemId,
               const ::std::string &tableId, Icd::ItemPtrArray &items, int deep) const override;
    bool parse(const ::std::string &vehicleId, const ::std::string &systemId,
               const ::std::string &tableId, const ::std::string &itemId,
               Icd::ItemPtr &item, int deep) const override;
    bool parse(Icd::TablePtrArray &tables) const override;
    bool parse(const ::std::string &tableId, Icd::TablePtr &table) const override;


    bool save(const RootPtr &root) const override;
    bool save(const Icd::VehiclePtrArray &vehicles) const override;
    bool save(const ::std::string &vehicleId, const Icd::VehiclePtr &vehicle) const override;
    bool save(const ::std::string &vehicleId, const Icd::SystemPtrArray &systems) const override;
    bool save(const ::std::string &vehicleId, const ::std::string &systemId,
              const ::Icd::SystemPtr &system) const override;
    bool save(const ::std::string &vehicleId, const ::std::string &systemId,
              const ::Icd::TablePtrArray &tables) const override;
    bool save(const ::std::string &vehicleId, const ::std::string &systemId,
              const ::std::string &tableId, const Icd::TablePtr &table) const override;
    bool save(const ::std::string &vehicleId, const ::std::string &systemId,
              const ::std::string &tableId, const Icd::ItemPtrArray &items) const override;
    bool save(const ::std::string &vehicleId, const ::std::string &systemId,
              const ::std::string &tableId, const ::std::string &itemId,
              const Icd::ItemPtr &item) const override;
    bool save(const Icd::TablePtrArray &tables) const override;
    bool save(const ::std::string &tableId, const Icd::TablePtr &table) const override;
    bool save(const Icd::TablePtr &table) const override;

public:
    bool convert2Other(const Icd::VehiclePtrArray &vehicles, std::vector<int> &data);
    bool convert2Core(const std::vector<int> &data, Icd::VehiclePtrArray &vehicles);

private:
    std::shared_ptr<SqlParserData> d;
};

} // end of namespace Icd

#endif // ICDPARSER_SQL_H
