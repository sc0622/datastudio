#ifndef ICDPARSER_JSON_H
#define ICDPARSER_JSON_H

#include "../../../icdparser.h"
#include "icdcore/icdcore_inc.h"
#include "../icdparser_file.h"
#include "../../generate/icdgenerate.h"

class QJsonObject;
class QJsonArray;

namespace Icd {

class JsonParser;
typedef JHandlePtr<JsonParser> JsonParserPtr;

class JsonParser : public FileParser, public Generator
{
public:
    explicit JsonParser(const Json::Value &config);
    ~JsonParser();

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

    bool save(const Icd::VehiclePtrArray &vehicles) const;
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
    bool save(const Icd::TablePtr &table) const;

public:
    Json::Value queryVehicles() const;
    Json::Value queryVehicle(const std::string &vehicleId) const;
    Json::Value querySystems(const std::string &vehicleId) const;
    Json::Value querySystem(const std::string &vehicleId, const std::string &systemId) const;
    Json::Value queryTables(const std::string &vehicleId, const std::string &systemId) const;
    Json::Value queryTable(const std::string &vehicleId, const std::string &systemId,
                           const std::string &tableId) const;
    Json::Value queryItems(const std::string &vehicleId, const std::string &systemId,
                           const std::string &tableId) const;
    Json::Value queryItem(const std::string &vehicleId, const std::string &systemId,
                          const std::string &tableId, const std::string &itemId) const;
    Json::Value queryTables() const;
    Json::Value queryTable(const std::string &tableId) const;

    // Generator interface
public:
    bool startup();
    void shutdown();
    bool generate(const QStandardItem *item, bool exportAll, bool rt, const std::string &filePath);
    bool generate(const TablePtr &table, const std::string &filePath);
    Icd::Parser *parser();
    const Icd::Parser *parser() const;

private:
    bool generateRoot(const QStandardItem *itemRoot, bool exportAll, bool rt,
                      const std::string &filePath);
    bool generateVehicle(const QStandardItem *itemVehicle, bool exportAll, bool rt,
                         const std::string &filePath);
    bool generateSystem(const QStandardItem *itemSystem, bool exportAll, bool rt,
                        const std::string &filePath);
    bool generateTable(const QStandardItem *itemTable, bool exportAll, bool rt,
                       const std::string &filePath);
    bool generateDataItem(const QStandardItem *itemDataItem, bool exportAll,
                          const std::string &filePath);
    bool generateVehicle(const QStandardItem *itemVehicle, Json::Value &vehicleJson,
                         bool exportAll, bool rt);
    bool generateSystem(const QStandardItem *itemSystem,  Json::Value &systemJson,
                        bool exportAll, bool rt);
    bool generateTable(const QStandardItem *itemTable, Json::Value &tableJson,
                       bool exportAll, bool rt);

    Json::Value generateJson(const QStandardItem *item, Json::Value &json);

private:
};

} // end of namespace Icd

#endif // ICDPARSER_JSON_H
