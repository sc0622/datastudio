#include "precomp.h"
#include "icdparser_json.h"
#include "jsoncpp/json/json.h"
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

bool JsonParser::parse(Icd::VehiclePtrArray &vehicles, int deep) const
{
    vehicles.clear();

    Json::Value rootJson;
    if (!this->rootJson(rootJson, false)) {
        return false;
    }

    if (!rootJson.isMember("vehicles")) {
        return false;
    }

    Json::Value vehiclesJson = rootJson["vehicles"];
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
    Json::Value vehicleJson = findVehicle(vehicleId);
    if (vehicleJson.isNull()) {
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
    systems.clear();

    Json::Value vehicleJson = findVehicle(vehicleId);
    if (vehicleJson.isNull()) {
        return false;
    }

    Json::Value systemsJson = vehicleJson["systems"];
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
    Json::Value systemJson = findSystem(vehicleId, systemId);
    if (systemJson.isNull()) {
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
    tables.clear();

    Json::Value systemJson = findSystem(vehicleId, systemId);
    if (systemJson.isNull()) {
        return false;
    }

    const std::string domain = vehicleId + '/' + systemId;

    Json::Value tablesJson = systemJson["tables"];
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
    Json::Value tableJson = findTable(vehicleId, systemId, tableId);
    if (tableJson.isNull()) {
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

bool JsonParser::parse(TablePtrArray &tables) const
{
    tables.clear();

    Json::Value rootJson;
    if (!this->rootJson(rootJson, false)) {
        return false;
    }

    if (!rootJson.isArray()) {
        return false;
    }

    for (Json::ValueConstIterator citer = rootJson.begin();
         citer != rootJson.end(); ++citer) {
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
    Json::Value tableJson = findTable(tableId);
    if (tableJson.isNull()) {
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

bool JsonParser::parse(const std::string &vehicleId, const std::string &systemId,
                       const std::string &tableId, Icd::ItemPtrArray &items, int deep) const
{
    items.clear();

    Json::Value tableJson = findTable(vehicleId, systemId, tableId);
    if (tableJson.isNull()) {
        return false;
    }

    Json::Value itemsJson = tableJson["items"];
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
    Json::Value itemJson = findItem(vehicleId, systemId, tableId, itemId);
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

bool JsonParser::save(VehiclePtrArray &vehicles) const
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

    return JJson::setValue(filePath(), "", rootJson);
}

bool JsonParser::save(const std::string &vehicleId, const VehiclePtr &vehicle) const
{
    if (!vehicle) {
        return false;
    }

    Json::Value vehiclesJson = JJson::value(filePath(), "vehicles", true);
    if (!vehiclesJson.isArray()) {
        return false;
    }

    vehiclesJson[vehicleId] = vehicle->save();

    Json::Path;

    return JJson::setValue(filePath(), "vehicles", vehiclesJson);
}

bool JsonParser::save(const std::string &vehicleId, const SystemPtrArray &systems) const
{
    Json::Value vehiclesJson = JJson::value(filePath(), "vehicles", true);
    if (!vehiclesJson.isArray()) {
        return false;
    }

    Json::Value systemsJson(Json::arrayValue);
    for (Icd::SystemPtrArray::const_iterator citer = systems.cbegin();
         citer != systems.cend(); ++citer) {
        const Icd::SystemPtr &system = *citer;
        if (!system) {
            continue;
        }
        systemsJson.append(system->save());
    }
    Json::Value rootJson;
    rootJson["vehicles"] = vehiclesJson;

    return JJson::setValue(filePath(), "vehicles", vehiclesJson);
}

bool JsonParser::save(const std::string &vehicleId, const std::string &systemId,
                      const SystemPtr &system) const
{

}

bool JsonParser::save(const std::string &vehicleId, const std::string &systemId,
                      const TablePtrArray &tables) const
{

}

bool JsonParser::save(const std::string &vehicleId, const std::string &systemId,
                      const std::string &tableId, const TablePtr &table) const
{

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

    return JJson::setValue(filePath(), "", tablesJson);
}

bool JsonParser::save(const std::string &tableId, const TablePtr &table) const
{

}

bool JsonParser::save(const std::string &vehicleId, const std::string &systemId,
                      const std::string &tableId, const ItemPtrArray &items) const
{

}

bool JsonParser::save(const std::string &vehicleId, const std::string &systemId,
                      const std::string &tableId, const std::string &itemId,
                      const ItemPtr &item) const
{

}

bool JsonParser::save(const TablePtr &table) const
{
    Json::Value tablesJson(Json::arrayValue);
    tablesJson.append(table->save());
    return JJson::setValue(filePath(), "", tablesJson);
}

bool JsonParser::rootJson(Json::Value &value, bool create) const
{
    return JJson::parse(filePath(), value, create);
}

Json::Value JsonParser::findVehicle(const std::string &vehicleId) const
{
    Json::Value rootJson;
    if (!this->rootJson(rootJson, false)) {
        return Json::Value();
    }

    if (!rootJson.isMember("vehicles")) {
        return Json::Value();
    }

    Json::Value vehiclesJson = rootJson["vehicles"];
    for (Json::ValueConstIterator citer = vehiclesJson.begin();
         citer != vehiclesJson.end(); ++citer) {
        const Json::Value &vehicleJson = *citer;
        if (vehicleJson["id"].asString() == vehicleId) {
            return vehicleJson;
        }
    }

    return Json::Value();
}

Json::Value JsonParser::findSystem(const std::string &vehicleId, const std::string &systemId) const
{
    Json::Value vehicleJson = findVehicle(vehicleId);
    if (vehicleJson.isNull()) {
        return Json::Value();
    }

    if (!vehicleJson.isMember("systems")) {
        return Json::Value();
    }

    Json::Value systemsJson = vehicleJson["systems"];
    for (Json::ValueConstIterator citer = systemsJson.begin();
         citer != systemsJson.end(); ++citer) {
        const Json::Value &systemJson = *citer;
        if (systemJson["id"].asString() == systemId) {
            return systemJson;
        }
    }

    return Json::Value();
}

Json::Value JsonParser::findTable(const std::string &vehicleId, const std::string &systemId,
                                  const std::string &tableId) const
{
    Json::Value systemJson = findSystem(vehicleId, systemId);
    if (systemJson.isNull()) {
        return Json::Value();
    }

    if (!systemJson.isMember("tables")) {
        return Json::Value();
    }

    Json::Value tablesJson = systemJson["tables"];
    for (Json::ValueConstIterator citer = tablesJson.begin();
         citer != tablesJson.end(); ++citer) {
        const Json::Value &tableJson = *citer;
        if (tableJson["id"].asString() == tableId) {
            return tableJson;
        }
    }

    return Json::Value();
}

Json::Value JsonParser::findTable(const std::string &tableId) const
{
    Json::Value rootJson;
    if (!this->rootJson(rootJson, false)) {
        return Json::Value();
    }

    if (!rootJson.isArray()) {
        return Json::Value();
    }

    for (Json::ValueConstIterator citer = rootJson.begin();
         citer != rootJson.end(); ++citer) {
        const Json::Value &tableJson = *citer;
        if (tableJson["id"].asString() == tableId) {
            return tableJson;
        }
    }

    return Json::Value();
}

Json::Value JsonParser::findItem(const std::string &vehicleId, const std::string &systemId,
                                 const std::string &tableId, const std::string &itemId) const
{
    Json::Value tableJson = findTable(vehicleId, systemId, tableId);
    if (tableJson.isNull()) {
        return Json::Value();
    }

    if (!tableJson.isMember("items")) {
        return Json::Value();
    }

    Json::Value itemsJson = tableJson["items"];
    for (Json::ValueConstIterator citer = itemsJson.begin();
         citer != itemsJson.end(); ++citer) {
        const Json::Value &itemJson = *citer;
        if (itemJson["id"].asString() == itemId) {
            return itemJson;
        }
    }

    return Json::Value();
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

    Json::Value rootJson;
    rootJson["table"] = table->save();
    if (!JJson::setValue(filePath, "", rootJson, true)) {
        return false;
    }

    return true;
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
        if (!parser()->parseVehicle(vehicles, Icd::ObjectItem)) {
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
    if (rootJson.isNull()) {
        return false;
    }

    if (!JJson::setValue(filePath, "", rootJson, true)) {
        return false;
    }

    return true;
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
    if (!parser()->parseVehicle(vehicleId.toStdString(), vehicle,
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

    if (!JJson::setValue(filePath, "", rootJson, true)) {
        return false;
    }

    return true;
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
    if (!parser()->parseSystem(domain.section('/', 0, 0).toStdString(),
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

    if (!JJson::setValue(filePath, "", rootJson, true)) {
        return false;
    }

    return true;
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
    Icd::TablePtr table = Icd::TablePtr(0);
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

    if (!table) {
        //
        parser()->setMessage(QStringLiteral("获取表数据\n表：%1")
                             .arg(itemTable->text().remove(QRegExp("<([^>]*)>")))
                             .toStdString());
        //
        const QString domain = itemTable->data(Icd::TreeItemDomainRole).toString();
        if (!parser()->parseTable(domain.section('/', 0, 0).toStdString(),
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

    if (!JJson::setValue(filePath, "", rootJson, true)) {
        return false;
    }

    return true;
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
    if (!parser()->parseItem(domain.section('/', 0, 0).toStdString(),
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

    if (!JJson::setValue(filePath, "", rootJson, true)) {
        return false;
    }

    return true;
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
    if (!parser()->parseVehicle(vehicleId.toStdString(), vehicle,
                                Icd::ObjectVehicle)) {
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
    if (!parser()->parseSystem(domain.section('/', 0, 0).toStdString(),
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
    Icd::TablePtr table = Icd::TablePtr(0);
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

    if (!table) {
        //
        parser()->setMessage(QStringLiteral("获取表数据\n表：%1")
                             .arg(itemTable->text().remove(QRegExp("<([^>]*)>")))
                             .toStdString());
        //
        const QString domain = itemTable->data(Icd::TreeItemDomainRole).toString();
        if (!parser()->parseTable(domain.section('/', 0, 0).toStdString(),
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
        if (!parser()->parseVehicle(vehicleId.toStdString(), vehicle,
                                    Icd::ObjectVehicle)) {
            return Json::Value();
        }

        JJson::merge(vehicle->save(), json);

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
        if (!parser()->parseSystem(domain.section('/', 0, 0).toStdString(),
                                   domain.section('/', 1).toStdString(), system,
                                   Icd::ObjectSystem)) {
            return false;
        }

        JJson::merge(system->save(), json);

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
        if (!parser()->parseTable(domain.section('/', 0, 0).toStdString(),
                                  domain.section('/', 1, 1).toStdString(),
                                  domain.section('/', 2).toStdString(),
                                  table, Icd::ObjectTable)) {
            return Json::Value();
        }

        JJson::merge(table->save(), json);

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
