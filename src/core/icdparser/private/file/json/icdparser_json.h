#ifndef ICDPARSER_JSON_H
#define ICDPARSER_JSON_H

#include "../../../icdparser.h"
#include "icdcore/icdcore_inc.h"
#include "../icdparser_file.h"
#include "../../generate/icdgenerate.h"

namespace Icd {

class JsonParser;
typedef std::shared_ptr<JsonParser> JsonParserPtr;

class JsonParser : public FileParser, public Generator
{
public:
    explicit JsonParser(const Json::Value &config);
    ~JsonParser() override;

    bool parse(RootPtr &root, int deep) const override;
    bool parse(Icd::VehiclePtrArray &vehicles, int deep, Icd::Object *parent) const override;
    bool parse(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep,
               Icd::Object *parent) const override;
    bool parse(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep,
               Icd::Object *parent) const override;
    bool parse(const std::string &vehicleId, const std::string &systemId, Icd::SystemPtr &system,
               int deep, Icd::Object *parent) const override;
    bool parse(const std::string &vehicleId, const std::string &systemId, Icd::TablePtrArray &tables,
               int deep, Icd::Object *parent) const override;
    bool parse(const std::string &vehicleId, const std::string &systemId, const std::string &tableId,
               Icd::TablePtr &table, int deep, Icd::Object *parent) const override;
    bool parse(const std::string &vehicleId, const std::string &systemId, const std::string &tableId,
               Icd::ItemPtrArray &items, int deep, Icd::Object *parent) const override;
    bool parse(Icd::TablePtrArray &tables) const override;
    bool parse(const std::string &tableId, Icd::TablePtr &table) const override;

    bool save(const RootPtr &root) const override;
    bool save(const Icd::VehiclePtrArray &vehicles) const override;
    bool save(const std::string &vehicleId, const Icd::VehiclePtr &vehicle) const override;
    bool save(const std::string &vehicleId, const Icd::SystemPtrArray &systems) const override;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const Icd::SystemPtr &system) const override;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const Icd::TablePtrArray &tables) const override;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const std::string &tableId, const Icd::TablePtr &table) const override;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const std::string &tableId, const Icd::ItemPtrArray &items) const override;
    bool save(const Icd::TablePtrArray &tables) const override;
    bool save(const std::string &tableId, const Icd::TablePtr &table) const override;
    bool save(const Icd::TablePtr &table) const override;
    // for json
    bool saveJsonObject(const ::std::string &path, const Json::Value &value,
                        bool fast = false) const override;
    bool saveJsonArray(const ::std::string &path, const Json::Value &value,
                       bool unique = true, bool fast = false) const override;
    bool replaceJson(const ::std::string &path, const Json::Value &oldValue,
                     const Json::Value &newValue, bool fast = false) const override;
    bool removeJson(const ::std::string &path, const std::string &key,
                    const Json::Value &value, bool fast = false) const override;
    bool mergeJson(const ::std::string &path, const Json::Value &value, bool fast = false) const override;

    bool beginModify() override;
    bool commitModify() override;
    bool cancelModify() override;
    bool endModify() override;

public:
    Json::Value queryRoot() const;
    Json::Value queryVehicles() const;
    Json::Value queryVehicle(const std::string &vehicleId) const;
    Json::Value querySystems(const std::string &vehicleId) const;
    Json::Value querySystem(const std::string &vehicleId, const std::string &systemId) const;
    Json::Value queryTables(const std::string &vehicleId, const std::string &systemId) const;
    Json::Value queryTable(const std::string &vehicleId, const std::string &systemId,
                           const std::string &tableId) const;
    Json::Value queryTable(const std::string &domain, int domainType) const;
    Json::Value queryItems(const std::string &vehicleId, const std::string &systemId,
                           const std::string &tableId) const;
    Json::Value queryItem(const std::string &vehicleId, const std::string &systemId,
                          const std::string &tableId, const std::string &itemId) const;
    Json::Value queryTables() const;
    Json::Value queryTable(const std::string &tableId) const;

    // Generator interface
public:
    bool startup() override;
    void shutdown() override;
#ifndef J_NO_QT
    bool generate(const QStandardItem *item, bool exportAll, bool rt, const std::string &filePath) override;
#endif
    bool generate(const TablePtr &table, const std::string &filePath) override;
    Icd::Parser *parser() override;
    const Icd::Parser *parser() const override;

private:
#ifndef J_NO_QT
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
#endif
private:
};

} // end of namespace Icd

#endif // ICDPARSER_JSON_H

