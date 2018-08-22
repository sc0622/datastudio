#include "precomp.h"
#include "./icdparser_sql.h"
#include "private/icdparser_sql_p.h"
#include "icdwidget/icdwidget_global.h"

namespace Icd {

// class SqlParser

SqlParser::SqlParser(const Json::Value &config)
    : Parser(config)
    , d(new SqlParserData())
{
    std::string filePath = config["filePath"].asString();
    if (!filePath.empty()) {
        if (!d->db->init(filePath)) {
            QString error = QObject::tr("Initialize database failure! [reson: %1]")
                    .arg(QString::fromStdString(d->db->lastError()));
            qWarning() << error;
        }
    }
}

SqlParser::~SqlParser()
{

}

bool SqlParser::parse(RootPtr &root, int deep) const
{
    Q_UNUSED(root);
    Q_UNUSED(deep);
    return false;
}

bool SqlParser::parse(Icd::VehiclePtrArray &vehicles, int deep) const
{
    return d->parseVehicle(vehicles, deep);
}

bool SqlParser::parse(const std::string &vehicleId, Icd::VehiclePtr &vehicle,
                      int deep) const
{
    Q_ASSERT(deep <= Icd::ObjectItem);

    VehiclePtr _vehicle(new Vehicle());
    DMSpace::_vectorPS plane_system;
    DMSpace::_vectorIcdTR rules;

    // generate plane-systems
    plane_system.push_back(std::make_pair(d->planeBase(vehicleId),
                                          d->systemBaseArray(vehicleId)));
    if (deep >= Icd::ObjectTable) {// at least table node
        if (deep >= Icd::ObjectItem) { // deep to rule
            rules = d->tableRules(std::string(), std::string());
        } else {    // deep to table
            std::vector<stICDBase> tables
                    = d->icdBaseArray(std::string(), std::string(), true);
            const int count = tables.size();
            for (int i = 0; i < count; ++i) {
                rules.push_back(std::make_pair(tables[i],
                                               std::vector<stTableRules>()));
            }
        }
    }

    // parse plane
    if (!d->parseVehicle(vehicleId, _vehicle, plane_system, rules, deep)) {
        return false;
    }

    //
    vehicle = _vehicle;

    return true;
}

bool SqlParser::parse(const std::string &vehicleId,
                      Icd::SystemPtrArray &systems, int deep) const
{
    Q_ASSERT(deep <= Icd::ObjectItem);

    Icd::VehiclePtr _vehicle(new Icd::Vehicle());
    DMSpace::_vectorPS plane_system;
    DMSpace::_vectorIcdTR rules;

    // generate plane-systems
    plane_system.push_back(std::make_pair(d->planeBase(vehicleId),
                                          d->systemBaseArray(vehicleId)));
    if (deep >= Icd::ObjectTable) { // at least table node
        if (deep >= Icd::ObjectItem) { // deep to rule
            rules = d->tableRules(vehicleId, std::string());
        } else {    // deep to table
            std::vector<stICDBase> tables
                    = d->icdBaseArray(vehicleId, std::string(), true);
            const int count = tables.size();
            for (int i = 0; i < count; ++i) {
                rules.push_back(std::make_pair(tables[i],
                                               std::vector<stTableRules>()));
            }
        }
    }

    // parse system
    d->parseVehicle(vehicleId, _vehicle, plane_system, rules, deep);

    // save
    systems = _vehicle->allSystem();

    return true;
}

bool SqlParser::parse(const std::string &vehicleId,
                      const std::string &systemId,
                      Icd::SystemPtr &system, int deep) const
{
    Q_ASSERT(deep <= Icd::ObjectItem);

    // system base
    stSystem single = d->systemBase(vehicleId, systemId);

    // table-rules
    DMSpace::_vectorIcdTR rules;
    if (deep >= Icd::ObjectTable) { // at least table node
        if (deep >= Icd::ObjectItem) { // deep to rule
            rules = d->tableRules(vehicleId, systemId);
        } else {    // deep to table
            std::vector<stICDBase> tables
                    = d->icdBaseArray(vehicleId, systemId, true);
            const int count = tables.size();
            for (int i = 0; i < count; ++i) {
                rules.push_back(std::make_pair(tables[i],
                                               std::vector<stTableRules>()));
            }
        }
    }

    // parse system
    SystemPtr _system(new System());
    if (!d->parseSystem(vehicleId, single, _system, rules, deep)) {
        return false;
    }

    //
    system = _system;

    return true;
}

bool SqlParser::parse(const std::string &vehicleId,
                      const std::string &systemId,
                      Icd::TablePtrArray &tables, int deep) const
{
    Q_ASSERT(deep <= Icd::ObjectItem);

    // table-rules
    DMSpace::_vectorIcdTR rules;
    if (deep >= Icd::ObjectItem) {
        rules = d->tableRules(vehicleId, systemId);
    } else {
        std::vector<stICDBase> icdBase
                = d->icdBaseArray(vehicleId, systemId, true);
        const int count = icdBase.size();
        for (int i = 0; i < count; ++i) {
            rules.push_back(std::make_pair(icdBase[i],
                                           std::vector<stTableRules>()));
        }
    }

    // parse tables
    QString group = QString("%1/%2").arg(vehicleId.c_str()).arg(systemId.c_str());
    DMSpace::_vectorIcdTR_Cit it = rules.begin();
    for (; it != rules.end(); ++it) {
        const stICDBase &base = it->first;

        if (!base.sParentName.empty()
                || base.sGroup != group.toStdString()) {
            continue;   // skip sub tables and other group
        }
        Icd::TablePtr table(new Icd::Table());
        if (!d->parseTable(base.sID, table, rules, deep)) {
            return false;
        }
        table->setDomain(vehicleId + "/" + systemId + "/" + table->id());
        // save
        tables.push_back(table);
    }

    return true;
}

bool SqlParser::parse(const std::string &vehicleId,
                      const std::string &systemId,
                      const std::string &tableId,
                      Icd::TablePtr &table, int deep) const
{
    Q_ASSERT(deep <= Icd::ObjectItem);

    // table-rules
    DMSpace::_vectorIcdTR rules;
    if (deep >= Icd::ObjectItem) {
        rules = d->tableRules(tableId);
    } else {
        rules.push_back(std::make_pair(d->icdBase(tableId), std::vector<stTableRules>()));
    }

    // parse table
    TablePtr _table(new Table());
    if (!d->parseTable(tableId, _table, rules, deep)) {
        return false;   // parse failure
    }
    table = _table;
    table->setDomain(vehicleId + "/" + systemId + "/" + tableId);

    return true;
}

bool SqlParser::parse(TablePtrArray &tables) const
{
    Q_UNUSED(tables);
    return false;
}

bool SqlParser::parse(const std::string &tableId, TablePtr &table) const
{
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    return false;
}

bool SqlParser::parse(const std::string &vehicleId,
                      const std::string &systemId,
                      const std::string &tableId,
                      Icd::ItemPtrArray &items, int deep) const
{
    Q_ASSERT(deep <= Icd::ObjectItem);
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);

    stICDBase single;
    single.sID = tableId;
    // table-rules
    DMSpace::_vectorIcdTR _rules = d->tableRules(tableId);
    DMSpace::pairIcdTR icdTR = d->singleIcdTR(_rules, single);
    if (icdTR.first.sID.empty()) {
        return false;
    }
    const std::vector<stTableRules> &rules = icdTR.second;
    const int count = rules.size();
    for (int i = 0; i < count; ++i) {
        Icd::ItemPtr item;
        if (!d->parseItem(rules[i], item, _rules, deep)) {
            return false;
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

bool SqlParser::parse(const std::string &vehicleId,
                      const std::string &systemId,
                      const std::string &tableId,
                      const std::string &itemId,
                      Icd::ItemPtr &item, int deep) const
{
    Q_ASSERT(deep <= Icd::ObjectItem);

    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);

    // table-rules
    DMSpace::_vectorIcdTR _rules = d->tableRules(tableId);

    stTableRules rules;
    if (!d->queryItem(tableId, itemId, rules)) {
        return false;
    }

    // parse table
    Icd::ItemPtr _item;
    if (!d->parseItem(rules, _item, _rules, deep)) {
        return false;
    }

    //
    item = _item;

    return true;
}

bool SqlParser::save(const RootPtr &root) const
{
    Q_UNUSED(root);
    return false;
}

bool SqlParser::save(const Icd::VehiclePtrArray &vehicles) const
{
    DMSpace::_vectorPS infrastructure;
    DMSpace::_vectorIcdTR tableRules;
    TablePtrArray tableArray;
    stPlane planeBase;
    stSystem systemBase;
    const int count = vehicles.size();
    int bound = 0;
    int size = 0;
    std::string group;
    for (int i = 0; i < count; ++i) {
        const VehiclePtr &vehicle = vehicles[i];
        if (!d->vehicle2Base(vehicle, planeBase)) {
            return false;
        }
        std::vector<stSystem> systems;
        SystemPtrArray systemArray = vehicle->allSystem();
        bound = systemArray.size();
        for (int j = 0; j < bound; ++j) {
            const SystemPtr &system = systemArray[j];
            if (!d->system2Base(system, systemBase)) {
                return false;
            }
            group.clear();
            group.append(vehicle->id()).append("/").append(system->id());
            tableArray = system->allTable();
            size = tableArray.size();
            for (int k = 0; k < size; ++k) {
                if (!d->table2TableRules(tableArray[k], group, std::string(""), tableRules)) {
                    return false;
                }
            }
            // save
            systems.push_back(systemBase);
        }
        // save
        infrastructure.push_back(std::make_pair(planeBase, systems));
    }

    return d->saveData(infrastructure, tableRules);
}

bool SqlParser::save(const std::string &vehicleId, const VehiclePtr &vehicle) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(vehicle);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const SystemPtrArray &systems) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systems);
    return false;
}

bool SqlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const SystemPtr &system) const
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
                     const std::string &tableId, const TablePtr &table) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    return false;
}

bool SqlParser::save(const ::std::string &vehicleId, const ::std::string &systemId,
                     const ::std::string &tableId, const Icd::ItemPtrArray &items) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(items);
    return false;
}

bool SqlParser::save(const ::std::string &vehicleId, const ::std::string &systemId,
                     const ::std::string &tableId, const ::std::string &itemId,
                     const Icd::ItemPtr &item) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(itemId);
    Q_UNUSED(item);
    return false;
}

bool SqlParser::save(const TablePtrArray &tables) const
{
    Q_UNUSED(tables);
    return false;
}

bool SqlParser::save(const ::std::string &tableId, const Icd::TablePtr &table) const
{
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    return false;
}

bool SqlParser::save(const TablePtr &table) const
{
    Q_UNUSED(table);
    return false;
}

bool SqlParser::convert2Other(const Icd::VehiclePtrArray &vehicles, std::vector<int> &data)
{
    if (vehicles.empty()) {
        return true;
    }
    if (data.empty() || 2 != data.size()) {
        return false;
    }
    DMSpace::_vectorPS  &infrastructure = *reinterpret_cast<DMSpace::_vectorPS *>(data[0]);
    DMSpace::_vectorIcdTR &tableRules = *reinterpret_cast<DMSpace::_vectorIcdTR *>(data[1]);
    TablePtrArray tableArray;
    stPlane planeBase;
    stSystem systemBase;
    const int count = vehicles.size();
    int bound = 0, size = 0;
    std::string group;
    for (int i = 0; i < count; ++i) {
        const VehiclePtr &vehicle = vehicles[i];
        if (!d->vehicle2Base(vehicle, planeBase)) {
            return false;
        }
        std::vector<stSystem> systems;
        SystemPtrArray systemArray = vehicle->allSystem();
        bound = systemArray.size();
        for (int j = 0; j < bound; ++j) {
            const SystemPtr &system = systemArray[j];
            if (!d->system2Base(system, systemBase)) {
                return false;
            }
            group.clear();
            group.append(vehicle->id()).append("/").append(system->id());
            tableArray = system->allTable();
            size = tableArray.size();
            for (int k = 0; k < size; ++k) {
                if (!d->table2TableRules(tableArray[k], group, std::string(""), tableRules)) {
                    return false;
                }
            }
            // save
            systems.push_back(systemBase);
        }
        // save
        infrastructure.push_back(std::make_pair(planeBase, systems));
    }

    return true;
}

bool SqlParser::convert2Core(const std::vector<int> &data, Icd::VehiclePtrArray &vehicles)
{
    if (data.empty()) {
        return true;
    }
    if (2 != data.size()) {
        return false;
    }
    DMSpace::_vectorPS &plane_system = *reinterpret_cast<DMSpace::_vectorPS *>(data[0]);
    DMSpace::_vectorIcdTR &tableRules = *reinterpret_cast<DMSpace::_vectorIcdTR *>(data[1]);
    DMSpace::_vectorPS_Cit itPS = plane_system.begin();
    std::string planeID;
    for (; itPS != plane_system.end(); ++itPS) {
        const stPlane &plane = itPS->first;
        planeID = QString::number(plane.nCode).toStdString();
        Icd::VehiclePtr vehicle(new Icd::Vehicle());
        if (!d->parseVehicle(planeID, vehicle, plane_system, tableRules, Icd::ObjectItem)) {
            return false;
        }

        // save
        vehicles.push_back(vehicle);
    }

    return true;
}

} // end of namespace Icd
