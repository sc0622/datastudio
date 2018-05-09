#include "precomp.h"
#include "icdparser_json.h"
#include "icdcore/3rdpart/jsoncpp/json/json.h"
#include "../../../../icdwidget/icdwidget_global.h"

namespace Icd {

JsonParser::JsonParser(const Json::Value &config)
    : FileParser(config)
    , Generator(Generator::GeneratorTypeJson)
{

}

JsonParser::~JsonParser()
{

}

bool JsonParser::parse(RootPtr &root, int deep) const
{
    const Json::Value rootJson = queryRoot();
    if (rootJson == Json::Value::null) {
        return false;
    }

    Icd::RootPtr _root(new Icd::Root());
    if (!_root->restore(rootJson, deep)) {
        return false;
    }

    root = _root;

    return true;
}

bool JsonParser::parse(Icd::VehiclePtrArray &vehicles, int deep) const
{
    const Json::Value vehiclesJson = queryVehicles();
    if (vehiclesJson == Json::Value::null) {
        return false;
    }

    vehicles.clear();

    for (Json::ValueConstIterator citer = vehiclesJson.begin();
         citer != vehiclesJson.end(); ++citer) {
        const Json::Value &vehicleJson = *citer;
        Icd::VehiclePtr vehicle(new Icd::Vehicle());
        if (!vehicle->restore(vehicleJson, deep)) {
            continue;
        }
        vehicles.push_back(vehicle);
    }

    return true;
}

bool JsonParser::parse(const std::string &vehicleId, Icd::VehiclePtr &vehicle,
                       int deep) const
{
    const Json::Value vehicleJson = queryVehicle(vehicleId);
    if (vehicleJson == Json::Value::null) {
        return false;
    }

    Icd::VehiclePtr _vehicle(new Icd::Vehicle());
    if (!_vehicle->restore(vehicleJson, deep)) {
        return false;
    }

    vehicle = _vehicle;

    return true;
}

bool JsonParser::parse(const std::string &vehicleId, Icd::SystemPtrArray &systems,
                       int deep) const
{
    const Json::Value systemsJson = querySystems(vehicleId);
    if (systemsJson == Json::Value::null) {
        return false;
    }

    systems.clear();

    for (Json::ValueConstIterator citer = systemsJson.begin();
         citer != systemsJson.end(); ++citer) {
        const Json::Value &systemJson = *citer;
        Icd::SystemPtr system(new Icd::System());
        if (!system->restore(systemJson, deep)) {
            continue;
        }
        systems.push_back(system);
    }

    return true;
}

bool JsonParser::parse(const std::string &vehicleId, const std::string &systemId,
                       Icd::SystemPtr &system, int deep) const
{
    const Json::Value systemJson = querySystem(vehicleId, systemId);
    if (systemJson == Json::Value::null) {
        return false;
    }

    Icd::SystemPtr _system(new Icd::System());
    if (!_system->restore(systemJson, deep)) {
        return false;
    }

    system = _system;

    return true;
}

bool JsonParser::parse(const std::string &vehicleId, const std::string &systemId,
                       Icd::TablePtrArray &tables, int deep) const
{
    const Json::Value tablesJson = queryTables(vehicleId, systemId);
    if (tablesJson == Json::Value::null) {
        return false;
    }

    tables.clear();

    const std::string domain = vehicleId + '/' + systemId;

    for (Json::ValueConstIterator citer = tablesJson.begin();
         citer != tablesJson.end(); ++citer) {
        const Json::Value &tableJson = *citer;
        Icd::TablePtr table(new Icd::Table());
        if (!table->restore(tableJson, deep)) {
            continue;
        }
        table->setDomain(domain + '/' + table->id());
        tables.push_back(table);
    }

    return true;
}

bool JsonParser::parse(const std::string &vehicleId, const std::string &systemId,
                       const std::string &tableId, Icd::TablePtr &table, int deep) const
{
    const Json::Value tableJson = queryTable(vehicleId, systemId, tableId);
    if (tableJson == Json::Value::null) {
        return false;
    }

    Icd::TablePtr _table(new Icd::Table());
    if (!_table->restore(tableJson, deep)) {
        return false;
    }
    _table->setDomain(vehicleId + '/' + systemId + '/' + tableId);
    table = _table;

    return true;
}

bool JsonParser::parse(const std::string &vehicleId, const std::string &systemId,
                       const std::string &tableId, Icd::ItemPtrArray &items, int deep) const
{
    const Json::Value itemsJson = queryItems(vehicleId, systemId, tableId);
    if (itemsJson == Json::Value::null) {
        return false;
    }

    items.clear();

    for (Json::ValueConstIterator citer = itemsJson.begin();
         citer != itemsJson.end(); ++citer) {
        const Json::Value &itemJson = *citer;
        Icd::ItemPtr item = Icd::Item::create(Icd::itoa(items.size() + 1),
                                              Icd::Item::stringType(itemJson["type"].asString()));
        if (!item) {
            continue;
        }

        if (!item->restore(itemJson, deep)) {
            continue;
        }
        // offset
        if (!items.empty()) {
            const Icd::ItemPtr &last = *items.crbegin();
            item->setItemOffset(last->itemOffset() + 1);
            item->setBufferOffset(last->bufferOffset() + last->bufferSize());
        }
        items.push_back(item);
    }

    return true;
}

bool JsonParser::parse(const std::string &vehicleId, const std::string &systemId,
                       const std::string &tableId, const std::string &itemId,
                       Icd::ItemPtr &item, int deep) const
{
    const Json::Value itemJson = queryItem(vehicleId, systemId, tableId, itemId);
    if (itemJson.isNull()) {
        return false;
    }

    Icd::ItemPtr _item = Icd::Item::create(itemJson["id"].asString(),
            Icd::Item::stringType(itemJson["type"].asString()));
    if (!_item) {
        return false;
    }

    if (!_item->restore(itemJson, deep)) {
        return false;
    }

    item = _item;

    return true;
}

bool JsonParser::parse(TablePtrArray &tables) const
{
    const Json::Value tableJson = queryTables();
    if (tableJson == Json::Value::null) {
        return false;
    }

    if (!tableJson.isArray()) {
        return false;
    }

    tables.clear();

    for (Json::ValueConstIterator citer = tableJson.begin();
         citer != tableJson.end(); ++citer) {
        const Json::Value &tableJson = *citer;
        Icd::TablePtr table(new Icd::Table());
        if (!table->restore(tableJson, Icd::ObjectItem)) {
            continue;
        }
        table->setDomain(table->id());
        tables.push_back(table);
    }

    return true;
}

bool JsonParser::parse(const std::string &tableId, TablePtr &table) const
{
    const Json::Value tableJson = queryTable(tableId);
    if (tableJson == Json::Value::null) {
        return false;
    }

    Icd::TablePtr _table(new Icd::Table());
    if (!_table->restore(tableJson, Icd::ObjectItem)) {
        return false;
    }

    _table->setDomain(tableId);
    table = _table;

    return true;
}

bool JsonParser::save(const RootPtr &root) const
{
    if (!root) {
        return false;
    }

    return Json::make(filePath(), root->save(), true, true);
}

bool JsonParser::save(const VehiclePtrArray &vehicles) const
{
    Json::Value vehiclesJson(Json::arrayValue);
    for (Icd::VehiclePtrArray::const_iterator citer = vehicles.cbegin();
         citer != vehicles.cend(); ++citer) {
        const Icd::VehiclePtr &vehicle = *citer;
        if (!vehicle) {
            continue;
        }
        vehiclesJson.append(vehicle->save());
    }
    Json::Value rootJson;
    rootJson["vehicles"] = vehiclesJson;

    return Json::make(filePath(), rootJson, true, true);
}

bool JsonParser::save(const std::string &vehicleId, const VehiclePtr &vehicle) const
{
    if (!vehicle) {
        return false;
    }

    return Json::make(filePath(), ".vehicles[id=" + vehicleId + "]",
                      vehicle->save(), true, true);
}

bool JsonParser::save(const std::string &vehicleId, const SystemPtrArray &systems) const
{
    Json::Value systemsJson(Json::arrayValue);
    for (Icd::SystemPtrArray::const_iterator citer = systems.cbegin();
         citer != systems.cend(); ++citer) {
        const Icd::SystemPtr &system = *citer;
        if (!system) {
            continue;
        }
        systemsJson.append(system->save());
    }

    return Json::make(filePath(), ".vehicles[id=" + vehicleId + "].systems",
                      systemsJson, true, true);
}

bool JsonParser::save(const std::string &vehicleId, const std::string &systemId,
                      const SystemPtr &system) const
{
    if (!system) {
        return false;
    }

    return Json::make(filePath(), ".vehicles[id=" + vehicleId + "].systems[id="
                      + systemId + "]", system->save(), true, true);
}

bool JsonParser::save(const std::string &vehicleId, const std::string &systemId,
                      const TablePtrArray &tables) const
{
    Json::Value tablesJson(Json::arrayValue);
    for (Icd::TablePtrArray::const_iterator citer = tables.cbegin();
         citer != tables.cend(); ++citer) {
        const Icd::TablePtr &table = *citer;
        if (!table) {
            continue;
        }
        tablesJson.append(table->save());
    }

    return Json::make(filePath(), ".vehicles[id=" + vehicleId + "].systems[id="
                      + systemId + "].tables", tablesJson, true, true);
}

bool JsonParser::save(const std::string &vehicleId, const std::string &systemId,
                      const std::string &tableId, const TablePtr &table) const
{
    if (!table) {
        return false;
    }

    return Json::make(filePath(), ".vehicles[id=" + vehicleId + "].systems[id="
                      + systemId + "].tables[id=" + tableId + "]", table->save(),
                      true, true);
}

bool JsonParser::save(const std::string &vehicleId, const std::string &systemId,
                      const std::string &tableId, const ItemPtrArray &items) const
{
    Json::Value itemsJson(Json::arrayValue);
    for (Icd::ItemPtrArray::const_iterator citer = items.cbegin();
         citer != items.cend(); ++citer) {
        const Icd::ItemPtr &item = *citer;
        if (!item) {
            continue;
        }
        itemsJson.append(item->save());
    }

    return Json::make(filePath(), ".vehicles[id=" + vehicleId + "].systems[id="
                      + systemId + "].tables[id=" + tableId, itemsJson, true, true);
}

bool JsonParser::save(const std::string &vehicleId, const std::string &systemId,
                      const std::string &tableId, const std::string &itemId,
                      const ItemPtr &item) const
{
    if (!item) {
        return false;
    }

    return Json::make(filePath(), ".vehicles[id=" + vehicleId + "].systems[id="
                      + systemId + "].tables[id=" + tableId + "].items[id=" + itemId,
                      item->save(), true, true);
}

bool JsonParser::save(const TablePtrArray &tables) const
{
    Json::Value tablesJson(Json::arrayValue);
    for (Icd::TablePtrArray::const_iterator citer = tables.cbegin();
         citer != tables.cend(); ++citer) {
        const Icd::TablePtr &table = *citer;
        if (!table) {
            continue;
        }
        tablesJson.append(table->save());
    }

    return Json::make(filePath(), tablesJson, true, true);
}

bool JsonParser::save(const std::string &tableId, const TablePtr &table) const
{
    if (!table) {
        return false;
    }

    return Json::make(filePath(), ".tables[id=" + tableId + "]", table->save(),
                      true, true);
}

bool JsonParser::save(const TablePtr &table) const
{
    Json::Value tablesJson(Json::arrayValue);
    tablesJson.append(table->save());
    return Json::make(filePath(), tablesJson, true, true);
}

bool JsonParser::beginModify()
{
    if (Parser::isBeginModify()) {
        return true;
    }

    const std::string basePath = this->basePath();
    const std::string fileName = this->fileName();

    if (fileName.empty() || fileName.at(0) == '~') {
        return false;
    }

    const std::string tFilePath = FileParser::fullPath(basePath, '~' + fileName);
    const std::string filePath = FileParser::fullPath(basePath, fileName);

    if (FileParser::exists(tFilePath)) {
        if (!FileParser::remove(tFilePath)) {
            return false;
        }
    }

    if (!FileParser::copy(filePath, tFilePath)) {
        return false;
    }

    setFilePath(tFilePath);

    return Parser::beginModify();
}

bool JsonParser::commitModify()
{
    if (!Parser::isBeginModify()) {
        return false;
    }

    const std::string basePath = this->basePath();
    const std::string fileName = this->fileName();

    if (fileName.empty() || fileName.at(0) != '~') {
        return false;
    }

    const std::string tFilePath = FileParser::fullPath(basePath, fileName);

    if (!FileParser::exists(tFilePath)) {
        return false;
    }

    const std::string filePath = FileParser::fullPath(basePath, fileName.substr(1));

    if (!FileParser::remove(filePath)) {
        return false;
    }

    if (!FileParser::copy(tFilePath, filePath)) {
        return false;
    }

    return true;
}

bool JsonParser::cancelModify()
{
    if (!Parser::isBeginModify()) {
        return true;
    }

    const std::string basePath = this->basePath();
    const std::string fileName = this->fileName();

    if (fileName.empty() || fileName.at(0) != '~') {
        return false;
    }

    const std::string tFilePath = FileParser::fullPath(basePath, fileName);
    const std::string filePath = FileParser::fullPath(basePath, fileName.substr(1));

    if (FileParser::exists(tFilePath)) {
        if (!FileParser::remove(tFilePath)) {
            return false;
        }
    }

    if (!FileParser::copy(filePath, tFilePath)) {
        return false;
    }

    return Parser::cancelModify();
}

bool JsonParser::endModify()
{
    if (!Parser::isBeginModify()) {
        return true;
    }

    const std::string basePath = this->basePath();
    const std::string fileName = this->fileName();

    if (fileName.empty() || fileName.at(0) != '~') {
        return false;
    }

    const std::string tFilePath = FileParser::fullPath(basePath, fileName);
    const std::string filePath = FileParser::fullPath(basePath, fileName.substr(1));

    if (FileParser::exists(tFilePath)) {
        if (!FileParser::remove(tFilePath)) {
            return false;
        }
    }

    setFilePath(filePath);

    return Parser::endModify();
}

Json::Value JsonParser::queryRoot() const
{
    Json::Value rootJson;
    if (!Json::resolve(filePath(), rootJson) || !rootJson.isObject()) {
        return Json::Value::null;
    }

    return rootJson;
}

Json::Value JsonParser::queryVehicles() const
{
    const Json::Value vehiclesJson = Json::resolve(filePath(), ".vehicles");
    if (!vehiclesJson.isArray()) {
        return Json::Value::null;
    }
    return vehiclesJson;
}

Json::Value JsonParser::queryVehicle(const std::string &vehicleId) const
{
    const Json::Value vehicleJson = Json::resolve(
                filePath(), ".vehicles[id=" + vehicleId + "]");
    if (!vehicleJson.isObject()) {
        return Json::Value::null;
    }
    return vehicleJson;
}

Json::Value JsonParser::querySystems(const std::string &vehicleId) const
{
    const Json::Value systemsJson = Json::resolve(
                filePath(), ".vehicles[id=" + vehicleId + "].systems");
    if (!systemsJson.isArray()) {
        return Json::Value::null;
    }
    return systemsJson;
}

Json::Value JsonParser::querySystem(const std::string &vehicleId, const std::string &systemId) const
{
    const Json::Value systemJson = Json::resolve(
                filePath(), ".vehicles[id=" + vehicleId + "].systems[id=" + systemId + "]");
    if (!systemJson.isObject()) {
        return Json::Value::null;
    }
    return systemJson;
}

Json::Value JsonParser::queryTables(const std::string &vehicleId, const std::string &systemId) const
{
    const Json::Value tablesJson = Json::resolve(
                filePath(), ".vehicles[id=" + vehicleId + "].systems[id=" + systemId + "].tables");
    if (!tablesJson.isArray()) {
        return Json::Value::null;
    }
    return tablesJson;
}

Json::Value JsonParser::queryTable(const std::string &vehicleId, const std::string &systemId,
                                   const std::string &tableId) const
{
    const Json::Value tableJson = Json::resolve(
                filePath(), ".vehicles[id=" + vehicleId + "].systems[id=" + systemId
                + "].tables[id=" + tableId + "]");
    if (!tableJson.isObject()) {
        return Json::Value::null;
    }
    return tableJson;
}

Json::Value JsonParser::queryItems(const std::string &vehicleId, const std::string &systemId,
                                   const std::string &tableId) const
{
    const Json::Value itemsJson = Json::resolve(
                filePath(), ".vehicles[id=" + vehicleId + "].systems[id=" + systemId
                + "].tables[id=" + tableId + "].items");
    if (!itemsJson.isArray()) {
        return Json::Value::null;
    }
    return itemsJson;
}

Json::Value JsonParser::queryItem(const std::string &vehicleId, const std::string &systemId,
                                  const std::string &tableId, const std::string &itemId) const
{
    const Json::Value itemJson = Json::resolve(
                filePath(), ".vehicles[id=" + vehicleId + "].systems[id=" + systemId
                + "].tables[id=" + tableId + "].items[id=" + itemId + "]");
    if (!itemJson.isObject()) {
        return Json::Value::null;
    }
    return itemJson;
}

Json::Value JsonParser::queryTables() const
{
    const Json::Value tablesJson = Json::resolve(filePath(), ".tables");
    if (!tablesJson.isArray()) {
        return Json::Value::null;
    }
    return tablesJson;
}

Json::Value JsonParser::queryTable(const std::string &tableId) const
{
    const Json::Value tableJson = Json::resolve(filePath(), ".tables[id=" + tableId + "]");
    if (!tableJson.isObject()) {
        return Json::Value::null;
    }
    return tableJson;
}

bool JsonParser::startup()
{
    if (!Generator::startup()) {
        return false;
    }

    return true;
}

void JsonParser::shutdown()
{
    Generator::shutdown();
}

bool JsonParser::generate(const QStandardItem *item, bool exportAll, bool rt,
                          const std::string &filePath)
{
    if (!item) {
        return false;
    }

    if (filePath.empty()) {
        return false;
    }

    //
    if (!startup()) {
        return false;
    }

    //
    bool result = false;
    const int itemType = item->type();
    switch (itemType) {
    case TreeItemTypeRoot:
        result = generateRoot(item, exportAll, rt, filePath);
        break;
    case TreeItemTypeVehicle:
        result = generateVehicle(item, exportAll, rt, filePath);
        break;
    case TreeItemTypeSystem:
        result = generateSystem(item, exportAll, rt, filePath);
        break;
    case TreeItemTypeTable:
        result = generateTable(item, exportAll, rt, filePath);
        break;
    case TreeItemTypeDataItem:
        result = generateDataItem(item, exportAll, filePath);
        break;
    default:
        break;
    }

    if (!result) {
        //
    }

    shutdown();

    return result;
}

bool JsonParser::generate(const TablePtr &table, const std::string &filePath)
{
    if (!table || filePath.empty()) {
        return false;
    }

    return Json::make(filePath, ".table", table->save(), true, true);
}

Icd::Parser *JsonParser::parser()
{
    return dynamic_cast<Icd::Parser *>(this);
}

const Parser *JsonParser::parser() const
{
    return dynamic_cast<const Icd::Parser *>(this);
}

bool JsonParser::generateRoot(const QStandardItem *itemRoot, bool exportAll, bool rt,
                              const std::string &filePath)
{
    if (!itemRoot) {
        return false;
    }

    Json::Value vehiclesJson;

    if (exportAll) {
        parser()->setMessage(QStringLiteral("获取所有机型数据\n%1")
                             .arg(itemRoot->text().remove(QRegExp("<([^>]*)>")))
                             .toStdString());
        Icd::VehiclePtrArray vehicles;
        if (!parser()->parse(vehicles, Icd::ObjectItem)) {
            return false;
        }
        if (!vehicles.empty()) {
            Icd::VehiclePtrArray::const_iterator citer = vehicles.cbegin();
            for (; citer != vehicles.cend(); ++citer) {
                const Icd::VehiclePtr &vehicle = *citer;
                if (vehicle) {
                    vehiclesJson.append(vehicle->save());
                }
            }
        }
    } else {
        int count = itemRoot->rowCount();
        for (int i = 0; i < count; ++i) {
            const QStandardItem *itemVehicle = itemRoot->child(i);
            if (!itemVehicle) {
                continue;
            }
            Json::Value vehicleJson;
            if (!generateVehicle(itemVehicle, vehicleJson, exportAll, rt)) {
                return false;
            }
            vehiclesJson.append(vehicleJson);
        }
    }

    const Json::Value rootJson = generateJson(itemRoot, vehiclesJson);
    if (rootJson == Json::Value::null) {
        return false;
    }

    return Json::make(filePath, rootJson, true, true);
}

bool JsonParser::generateVehicle(const QStandardItem *itemVehicle, bool exportAll,
                                 bool rt, const std::string &filePath)
{
    if (!itemVehicle) {
        return false;
    }

    //
    parser()->setMessage(QStringLiteral("获取机型数据\n机型：%1")
                         .arg(itemVehicle->text().remove(QRegExp("<([^>]*)>")))
                         .toStdString());
    //
    const QString vehicleId = itemVehicle->data(TreeItemDomainRole).toString();
    Icd::VehiclePtr vehicle;
    if (!parser()->parse(vehicleId.toStdString(), vehicle,
                         exportAll ? Icd::ObjectItem : Icd::ObjectVehicle)) {
        return false;
    }

    Json::Value vehicleJson = vehicle->save();

    if (!exportAll) {
        Json::Value systemsJson;
        int count = itemVehicle->rowCount();
        for (int i = 0; i < count; ++i) {
            const QStandardItem *itemSystem = itemVehicle->child(i);
            if (!itemSystem) {
                continue;
            }
            Json::Value systemJson;
            if (!generateSystem(itemSystem, systemJson, exportAll, rt)) {
                return false;
            }
            systemsJson.append(systemJson);
        }

        vehicleJson["systems"] = systemsJson;
    }

    const Json::Value rootJson = generateJson(itemVehicle, vehicleJson);
    if (rootJson.isNull()) {
        return false;
    }

    return Json::make(filePath, rootJson, true, true);
}

bool JsonParser::generateSystem(const QStandardItem *itemSystem, bool exportAll,
                                bool rt, const std::string &filePath)
{
    if (!itemSystem) {
        return false;
    }

    //
    parser()->setMessage(QStringLiteral("获取系统数据\n系统：%1")
                         .arg(itemSystem->text().remove(QRegExp("<([^>]*)>")))
                         .toStdString());

    //
    const QString domain = itemSystem->data(TreeItemDomainRole).toString();
    Icd::SystemPtr system;
    if (!parser()->parse(domain.section('/', 0, 0).toStdString(),
                         domain.section('/', 1).toStdString(), system,
                         exportAll ? Icd::ObjectItem : Icd::ObjectSystem)) {
        return false;
    }

    Json::Value systemJson = system->save();

    if (!exportAll) {
        Json::Value tablesJson;
        int count = itemSystem->rowCount();
        for (int i = 0; i < count; ++i) {
            const QStandardItem *itemTable = itemSystem->child(i);
            if (!itemTable) {
                continue;
            }
            Json::Value tableJson;
            if (!generateSystem(itemTable, tableJson, exportAll, rt)) {
                return false;
            }
            tablesJson.append(tableJson);
        }

        systemJson["tables"] = tablesJson;
    }

    const Json::Value rootJson = generateJson(itemSystem, systemJson);
    if (rootJson.isNull()) {
        return false;
    }

    return Json::make(filePath, rootJson, true, true);
}

bool JsonParser::generateTable(const QStandardItem *itemTable, bool exportAll,
                               bool rt, const std::string &filePath)
{
    if (!itemTable) {
        return false;
    }

    if (!exportAll && itemTable->rowCount() == 0) {
        return true;
    }

    //
    Icd::TablePtr table;
#if defined(ICDWORKER_LIB)
    const QVariant varChannelId = itemTable->data(Icd::TreeChannelIdRole);
    if (varChannelId.isValid()) {
        Icd::WorkerPtr worker = Icd::WorkerPool::getInstance()
                ->workerByChannelIdentity(varChannelId.toString().toStdString());
        if (worker) {
            if (rt) {
                table = worker->workerRecv()->table();
            } else {
                table = worker->workerSend()->table();
            }
        }
    }
#endif
    if (!table) {
        //
        parser()->setMessage(QStringLiteral("获取表数据\n表：%1")
                             .arg(itemTable->text().remove(QRegExp("<([^>]*)>")))
                             .toStdString());
        //
        const QString domain = itemTable->data(Icd::TreeItemDomainRole).toString();
        if (!parser()->parse(domain.section('/', 0, 0).toStdString(),
                             domain.section('/', 1, 1).toStdString(),
                             domain.section('/', 2).toStdString(),
                             table, Icd::ObjectItem)) {
            return false;
        }
    }

    Json::Value tableJson = table->save();

    const Json::Value rootJson = generateJson(itemTable, tableJson);
    if (rootJson.isNull()) {
        return false;
    }

    return Json::make(filePath, rootJson, true, true);
}

bool JsonParser::generateDataItem(const QStandardItem *itemDataItem, bool exportAll,
                                  const std::string &filePath)
{
    if (!itemDataItem) {
        return false;
    }

    if (!exportAll && itemDataItem->rowCount() == 0) {
        return false;
    }

    //
    const QString domain = itemDataItem->data(TreeItemDomainRole).toString();
    Icd::ItemPtr item;
    if (!parser()->parse(domain.section('/', 0, 0).toStdString(),
                         domain.section('/', 1, 1).toStdString(),
                         domain.section('/', 2, 2).toStdString(),
                         domain.section('/', 3).toStdString(),
                         item, Icd::ObjectItem)) {
        return false;
    }

    Json::Value itemJson = item->save();

    const Json::Value rootJson = generateJson(itemDataItem, itemJson);
    if (rootJson.isNull()) {
        return false;
    }

    return Json::make(filePath, rootJson, true, true);
}

bool JsonParser::generateVehicle(const QStandardItem *itemVehicle, Json::Value &vehicleJson,
                                 bool exportAll, bool rt)
{
    if (!itemVehicle) {
        return false;
    }

    //
    parser()->setMessage(QStringLiteral("获取机型数据\n机型：%1")
                         .arg(itemVehicle->text().remove(QRegExp("<([^>]*)>")))
                         .toStdString());
    //
    const QString vehicleId = itemVehicle->data(TreeItemDomainRole).toString();
    Icd::VehiclePtr vehicle;
    if (!parser()->parse(vehicleId.toStdString(), vehicle, Icd::ObjectVehicle)) {
        return false;
    }

    vehicleJson = vehicle->save();

    if (!exportAll) {
        Json::Value systemsJson;
        int count = itemVehicle->rowCount();
        for (int i = 0; i < count; ++i) {
            const QStandardItem *itemSystem = itemVehicle->child(i);
            if (!itemSystem) {
                continue;
            }
            Json::Value systemJson;
            if (!generateSystem(itemSystem, systemJson, exportAll, rt)) {
                return false;
            }
            systemsJson.append(systemJson);
        }

        vehicleJson["systems"] = systemsJson;
    }

    return true;
}

bool JsonParser::generateSystem(const QStandardItem *itemSystem, Json::Value &systemJson,
                                bool exportAll, bool rt)
{
    if (!itemSystem) {
        return false;
    }

    //
    parser()->setMessage(QStringLiteral("获取系统数据\n系统：%1")
                         .arg(itemSystem->text().remove(QRegExp("<([^>]*)>")))
                         .toStdString());

    //
    const QString domain = itemSystem->data(TreeItemDomainRole).toString();
    Icd::SystemPtr system;
    if (!parser()->parse(domain.section('/', 0, 0).toStdString(),
                         domain.section('/', 1).toStdString(), system,
                         Icd::ObjectSystem)) {
        return false;
    }

    systemJson = system->save();

    if (!exportAll) {
        Json::Value tablesJson;
        int count = itemSystem->rowCount();
        for (int i = 0; i < count; ++i) {
            const QStandardItem *itemTable = itemSystem->child(i);
            if (!itemTable) {
                continue;
            }
            Json::Value tableJson;
            if (!generateSystem(itemTable, tableJson, exportAll, rt)) {
                return false;
            }
            tablesJson.append(tableJson);
        }

        systemJson["tables"] = tablesJson;
    }

    return true;
}

bool JsonParser::generateTable(const QStandardItem *itemTable, Json::Value &tableJson,
                               bool exportAll, bool rt)
{
    if (!itemTable) {
        return false;
    }

    if (!exportAll && itemTable->rowCount() == 0) {
        return true;
    }

    //
    Icd::TablePtr table;
#if defined(ICDWORKER_LIB)
    const QVariant varChannelId = itemTable->data(Icd::TreeChannelIdRole);
    if (varChannelId.isValid()) {
        Icd::WorkerPtr worker = Icd::WorkerPool::getInstance()
                ->workerByChannelIdentity(varChannelId.toString().toStdString());
        if (worker) {
            if (rt) {
                table = worker->workerRecv()->table();
            } else {
                table = worker->workerSend()->table();
            }
        }
    }
#endif
    if (!table) {
        //
        parser()->setMessage(QStringLiteral("获取表数据\n表：%1")
                             .arg(itemTable->text().remove(QRegExp("<([^>]*)>")))
                             .toStdString());
        //
        const QString domain = itemTable->data(Icd::TreeItemDomainRole).toString();
        if (!parser()->parse(domain.section('/', 0, 0).toStdString(),
                             domain.section('/', 1, 1).toStdString(),
                             domain.section('/', 2).toStdString(),
                             table, Icd::ObjectItem)) {
            return false;
        }
    }

    tableJson = table->save();

    return true;
}

Json::Value JsonParser::generateJson(const QStandardItem *item, Json::Value &json)
{
    if (!item) {
        return json;
    }

    switch (item->type()) {
    case Icd::TreeItemTypeRoot:
    {
        Json::Value rootJson;
        rootJson["vehicles"] = json;

        return rootJson;
    }
    case Icd::TreeItemTypeVehicle:
    {
        const QString vehicleId = item->data(TreeItemDomainRole).toString();
        Icd::VehiclePtr vehicle;
        if (!parser()->parse(vehicleId.toStdString(), vehicle, Icd::ObjectVehicle)) {
            return Json::Value();
        }

        Json::merge(vehicle->save(), json);

        Json::Value vehiclesJson;
        vehiclesJson.append(json);

        Json::Value rootJson = generateJson(item->parent(), vehiclesJson);
        if (rootJson.isNull()) {
            return Json::Value();
        }

        return rootJson;
    }
    case Icd::TreeItemTypeSystem:
    {
        const QString domain = item->data(TreeItemDomainRole).toString();
        Icd::SystemPtr system;
        if (!parser()->parse(domain.section('/', 0, 0).toStdString(),
                             domain.section('/', 1).toStdString(), system,
                             Icd::ObjectSystem)) {
            return false;
        }

        Json::merge(system->save(), json);

        Json::Value systemsJson;
        systemsJson.append(json);

        Json::Value vehicleJson;
        vehicleJson["systems"] = systemsJson;

        Json::Value rootJson = generateJson(item->parent(), vehicleJson);
        if (rootJson.isNull()) {
            return Json::Value();
        }

        return rootJson;
    }
    case Icd::TreeItemTypeTable:
    {
        const QString domain = item->data(Icd::TreeItemDomainRole).toString();
        Icd::TablePtr table;
        if (!parser()->parse(domain.section('/', 0, 0).toStdString(),
                             domain.section('/', 1, 1).toStdString(),
                             domain.section('/', 2).toStdString(),
                             table, Icd::ObjectTable)) {
            return Json::Value();
        }

        Json::merge(table->save(), json);

        Json::Value tablesJson;
        tablesJson.append(json);

        Json::Value systemJson;
        systemJson["tables"] = tablesJson;

        Json::Value rootJson = generateJson(item->parent(), systemJson);
        if (rootJson.isNull()) {
            return Json::Value();
        }

        return rootJson;
    }
    case Icd::TreeItemTypeItemTable:
    {
        Json::Value itemsJson;
        itemsJson.append(json);

        Json::Value parentItemJson;
        parentItemJson["items"] = itemsJson;

        Json::Value rootJson = generateJson(item->parent(), parentItemJson);
        if (rootJson.isNull()) {
            return Json::Value();
        }

        return rootJson;
    }
    case Icd::TreeItemTypeDataItem:
    {
        Json::Value tablesJson;
        tablesJson.append(json);

        Json::Value parentTableJson;
        parentTableJson["tables"] = tablesJson;

        Json::Value rootJson = generateJson(item->parent(), parentTableJson);
        if (rootJson.isNull()) {
            return Json::Value();
        }

        return rootJson;
    }
    default:
        return Json::Value();
    }

    return Json::Value();
}

} // end of namespace Icd
