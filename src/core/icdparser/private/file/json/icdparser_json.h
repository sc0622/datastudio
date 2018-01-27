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

    bool parseVehicle(Icd::VehiclePtrArray &vehicles, int deep);
    bool parseVehicle(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep);
    bool parseSystem(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep);
    bool parseSystem(const std::string &vehicleId, const std::string &systemId,
                     Icd::SystemPtr &system, int deep);
    bool parseTable(const std::string &vehicleId, const std::string &systemId,
                    Icd::TablePtrArray &tables, int deep);
    bool parseTable(const std::string &vehicleId, const std::string &systemId,
                    const std::string &tableId, Icd::TablePtr &table, int deep);
    bool parseTable(Icd::TablePtrArray &tables);
    bool parseTable(const std::string &tableId, Icd::TablePtr &table);
    bool parseItem(const std::string &vehicleId, const std::string &systemId,
                   const std::string &tableId, Icd::ItemPtrArray &items, int deep);
    bool parseItem(const std::string &vehicleId, const std::string &systemId,
                   const std::string &tableId, const std::string &itemId,
                   Icd::ItemPtr &item, int deep);
    bool save(const Icd::VehiclePtrArray &vehicles);
    bool save(const Icd::TablePtrArray &tables);
    bool save(const Icd::TablePtr &table);

public:
    bool rootJson(Json::Value &value, bool create = false) const;
    Json::Value findVehicle(const std::string &vehicleId);
    Json::Value findSystem(const std::string &vehicleId, const std::string &systemId);
    Json::Value findTable(const std::string &vehicleId, const std::string &systemId,
                          const std::string &tableId);
    Json::Value findTable(const std::string &tableId);
    Json::Value findItem(const std::string &vehicleId, const std::string &systemId,
                         const std::string &tableId, const std::string &itemId);

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
