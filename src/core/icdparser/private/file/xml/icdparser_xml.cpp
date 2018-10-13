#include "precomp.h"
#include "icdparser_xml.h"
#include "../../../../icdwidget/icdwidget_global.h"

namespace Icd {

XmlParser::XmlParser(const Json::Value &config)
    : FileParser(config)
    , Generator(Generator::GeneratorTypeXml)
{

}

XmlParser::~XmlParser()
{

}

bool XmlParser::parse(RootPtr &root, int deep) const
{
    TiXmlElement *emRoot = readElementRoot();
    if (!emRoot) {
        return false;
    }

    if (root) {
        root->clearVehicle();
    } else {
        root = std::make_shared<Icd::Root>(nullptr);
    }

    // parse root element
    for (TiXmlElement *emVehicle = emRoot->FirstChildElement("vehicle");
         emVehicle != nullptr;
         emVehicle = emVehicle->NextSiblingElement("vehicle")) {
        // parse vehicle informations
        const std::string id = emVehicle->Attribute("id");
        Icd::VehiclePtr vehicle = std::make_shared<Icd::Vehicle>(id, root.get());
        if (!parseVehicle(emVehicle, vehicle, deep)) {
            continue;   // parse failure
        }
        // save
        root->appendVehicle(vehicle);
    }

    delete emRoot->GetDocument();

    return true;
}

bool XmlParser::parse(Icd::VehiclePtrArray &vehicles, int deep, Icd::Object *parent) const
{
    TiXmlElement *emRoot = readElementRoot();
    if (!emRoot) {
        return false;
    }

    vehicles.clear();

    // parse vehicle elements
    for (TiXmlElement *emVehicle = emRoot->FirstChildElement("vehicle");
         emVehicle != nullptr;
         emVehicle = emVehicle->NextSiblingElement("vehicle")) {
        // parse vehicle informations
        const std::string id = emVehicle->Attribute("id");
        Icd::VehiclePtr vehicle = std::make_shared<Icd::Vehicle>(id, parent);
        if (!parseVehicle(emVehicle, vehicle, deep)) {
            continue;
        }
        // save
        vehicles.push_back(vehicle);
    }

    delete emRoot->GetDocument();

    return true;
}

bool XmlParser::parse(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep,
                      Icd::Object *parent) const
{
    TiXmlElement *emRoot = readElementRoot();
    if (!emRoot) {
        return false;
    }

    // parse vehicle elements
    for (TiXmlElement *emVehicle = emRoot->FirstChildElement("vehicle");
         emVehicle != nullptr;
         emVehicle = emVehicle->NextSiblingElement("vehicle")) {
        const std::string id = emVehicle->Attribute("id");
        if (id == vehicleId) {
            // parse vehicle informations
            if (vehicle) {
                vehicle->clearSystem();
            } else {
                vehicle = std::make_shared<Icd::Vehicle>(id, parent);
            }
            if (!parseVehicle(emVehicle, vehicle, deep)) {
                delete emRoot->GetDocument();
                return false;   // parse failure
            }
            break;
        }
    }

    delete emRoot->GetDocument();

    return true;
}

bool XmlParser::parse(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep,
                      Icd::Object *parent) const
{
    TiXmlElement *emVehicle = findVehicleElement(vehicleId);
    if (!emVehicle) {
        return false;
    }

    systems.clear();

    // parse system elements
    for (TiXmlElement *emSystem = emVehicle->FirstChildElement("system");
         emSystem != nullptr;
         emSystem = emSystem->NextSiblingElement("system")) {
        // parse system informations
        const std::string id = emSystem->Attribute("id");
        Icd::SystemPtr system = std::make_shared<Icd::System>(id, parent);
        if (!parseSystem(emSystem, system, deep)) {
            continue;   // parse failure
        }
        // save
        systems.push_back(system);
    }

    delete emVehicle->GetDocument();

    return true;
}

bool XmlParser::parse(const std::string &vehicleId, const std::string &systemId,
                      Icd::SystemPtr &system, int deep, Icd::Object *parent) const
{
    TiXmlElement *emVehicle = findVehicleElement(vehicleId);
    if (!emVehicle) {
        return false;
    }

    // parse system elements
    for (TiXmlElement *emSystem = emVehicle->FirstChildElement("system");
         emSystem != nullptr;
         emSystem = emSystem->NextSiblingElement("system")) {
        const std::string id = emSystem->Attribute("id");
        if (id == systemId) {
            if (system) {
                system->clearTable();
            } else {
                system = std::make_shared<Icd::System>(id, parent);
            }
            // parse system informations
            if (!parseSystem(emSystem, system, deep)) {
                return false;   // parse failure
            }
            break;
        }
    }

    delete emVehicle->GetDocument();

    return true;
}

bool XmlParser::parse(const std::string &vehicleId, const std::string &systemId,
                      Icd::TablePtrArray &tables, int deep, Icd::Object *parent) const
{
    TiXmlElement *emSystem = findSystemElement(vehicleId, systemId);
    if (!emSystem) {
        return false;
    }

    tables.clear();

    // parse table elements
    for (TiXmlElement *emTable = emSystem->FirstChildElement("table");
         emTable != nullptr;
         emTable = emTable->NextSiblingElement("table")) {
        // parse table informations
        const std::string id = emTable->Attribute("id");
        Icd::TablePtr table = std::make_shared<Icd::Table>(id, parent);
        if (!parseTable(emTable, table, deep)) {
            continue;   // parse failure
        }
        table->setDomain(vehicleId + "/" + systemId + "/" + table->id());
        // save
        tables.push_back(table);
    }

    delete emSystem->GetDocument();

    return true;
}

bool XmlParser::parse(const std::string &vehicleId, const std::string &systemId, const std::string &tableId,
                      Icd::TablePtr &table, int deep, Icd::Object *parent) const
{
    TiXmlElement *emSystem = findSystemElement(vehicleId, systemId);
    if (!emSystem) {
        return false;
    }

    // parse table elements
    for (TiXmlElement *emTable = emSystem->FirstChildElement("table");
         emTable != nullptr;
         emTable = emTable->NextSiblingElement("table")) {
        const std::string id = emTable->Attribute("id");
        if (id == tableId) {
            if (table) {
                table->clearItem();
            } else {
                table = std::make_shared<Icd::Table>(id, parent);
            }
            if (!parseTable(emTable, table, deep)) {
                delete emSystem->GetDocument();
                return false;   // parse failure
            }
            table->setDomain(vehicleId + "/" + systemId + "/" + tableId);
            delete emSystem->GetDocument();
            return true;
        }
    }

    delete emSystem->GetDocument();

    return false;
}

bool XmlParser::parse(const std::string &vehicleId, const std::string &systemId, const std::string &tableId,
                      Icd::ItemPtrArray &items, int deep, Icd::Object *parent) const
{
    Q_UNUSED(deep);
    TiXmlElement *emTable = findTableElement(vehicleId, systemId, tableId);
    if (!emTable) {
        return false;   //
    }

    items.clear();

    std::string sVal;

    // parse table elements
    for (TiXmlElement *emItem = emTable->FirstChildElement("item");
         emItem != nullptr;
         emItem = emItem->NextSiblingElement("item")) {
        // parse item informations
        if (emItem->QueryStringAttribute("type", &sVal) != TIXML_SUCCESS) {
            //
        }
        const std::string sType = sVal;
        Icd::ItemPtr item = Icd::Item::create(Icd::Item::stringType(sType), parent);
        if (!item) {
            continue;
        }
        if (!parseItem(emItem, item, deep)) {
            continue;   // parse failure
        }
        // offset
        if (!items.empty()) {
            const Icd::ItemPtr &last = *items.crbegin();
            item->setItemOffset(last->itemOffset() + 1);
            item->setBufferOffset(last->bufferOffset() + last->bufferSize());
        }
        // save
        items.push_back(item);
    }

    delete emTable->GetDocument();

    return true;
}

bool XmlParser::parse(TablePtrArray &tables) const
{
    //
    TiXmlElement *emRoot = readElementRoot();
    if (!emRoot) {
        return false;
    }

    tables.clear();

    // parse table elements
    for (TiXmlElement *emTable = emRoot->FirstChildElement("table");
         emTable != nullptr;
         emTable = emTable->NextSiblingElement("table")) {
        const std::string id = emTable->Attribute("id");
        TablePtr table = std::make_shared<Icd::Table>(id, nullptr);
        if (!parseTable(emTable, table, Icd::ObjectItem)) {
            continue;   // parse failure
        }
        table->setDomain(table->id());
        // save
        tables.push_back(table);
    }

    delete emRoot->GetDocument();

    return false;
}

bool XmlParser::parse(const std::string &tableId, TablePtr &table) const
{
    TiXmlElement *emTable = findTableElement(tableId);
    if (!emTable) {
        return false;
    }

    if (table) {
        table->clearItem();
    } else {
        table = std::make_shared<Icd::Table>(tableId, nullptr);
    }

    if (!parseTable(emTable, table, Icd::ObjectItem)) {
        delete emTable->GetDocument();
        return false;   // parse failure
    }

    table->setDomain(tableId);
    delete emTable->GetDocument();

    return true;
}

bool XmlParser::save(const RootPtr &root) const
{
    if (!root) {
        return false;
    }

    return save(root->allVehicle());
}

bool XmlParser::save(const Icd::VehiclePtrArray &vehicles) const
{
    // create root element
    TiXmlDocument *document = createDocument();
    if (!document) {
        return false;
    }

    //
    TiXmlElement *emRoot = document->RootElement();
    if (!emRoot) {
        delete document;
        return false;
    }

    // create vehicles element
    for (VehiclePtrArray::const_iterator citer = vehicles.cbegin();
         citer != vehicles.cend(); ++citer) {
        // create a vehicle element from document
        TiXmlElement *emVehicle = new TiXmlElement("vehicle");
        emRoot->LinkEndChild(emVehicle);
        if (!saveVehicle(emVehicle, *citer)) {
            continue;
        }
    }

    // save
    if (!saveDocument(emRoot->GetDocument())) {
        delete document;
        return false;
    }

    delete document;

    return true;
}

bool XmlParser::save(const std::string &vehicleId, const VehiclePtr &vehicle) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(vehicle);
    return false;
}

bool XmlParser::save(const std::string &vehicleId, const SystemPtrArray &systems) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systems);
    return false;
}

bool XmlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const SystemPtr &system) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(system);
    return false;
}

bool XmlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const TablePtrArray &tables) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tables);
    return false;
}

bool XmlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const std::string &tableId, const TablePtr &table) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    return false;
}

bool XmlParser::save(const std::string &vehicleId, const std::string &systemId,
                     const std::string &tableId, const ItemPtrArray &items) const
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(systemId);
    Q_UNUSED(tableId);
    Q_UNUSED(items);
    return false;
}

bool XmlParser::save(const TablePtrArray &tables) const
{
    // create root element
    TiXmlDocument *document = createDocument();
    if (!document) {
        return false;
    }

    //
    TiXmlElement *emRoot = document->RootElement();
    if (!emRoot) {
        delete document;
        return false;
    }

    // create vehicles element
    for (TablePtrArray::const_iterator citer = tables.cbegin();
         citer != tables.cend(); ++citer) {
        // create a table element from document
        TiXmlElement *emTable = new TiXmlElement("table");
        emRoot->LinkEndChild(emTable);
        if (!saveTable(emTable, *citer)) {
            continue;
        }
    }

    // save
    if (!saveDocument(emRoot->GetDocument())) {
        delete document;
        return false;
    }

    delete document;

    return true;
}

bool XmlParser::save(const std::string &tableId, const TablePtr &table) const
{
    Q_UNUSED(tableId);
    Q_UNUSED(table);
    return false;
}

bool XmlParser::save(const TablePtr &table) const
{
    // create root element
    TiXmlDocument *document = createDocument();
    if (!document) {
        return false;
    }

    //
    TiXmlElement *emRoot = document->RootElement();
    if (!emRoot) {
        delete document;
        return false;
    }

    // create a table element from document
    TiXmlElement *emTable = new TiXmlElement("table");
    emRoot->LinkEndChild(emTable);
    if (!saveTable(emTable, table)) {
        delete document;
        return false;
    }

    // save
    if (!saveDocument(emRoot->GetDocument())) {
        delete document;
        return false;
    }

    delete document;

    return true;
}

bool XmlParser::beginModify()
{
    return false;
}

bool XmlParser::commitModify()
{
    return false;
}

bool XmlParser::cancelModify()
{
    return false;
}

bool XmlParser::endModify()
{
    return false;
}

bool XmlParser::startup()
{
    if (!Generator::startup()) {
        return false;
    }

    return true;
}

void XmlParser::shutdown()
{
    Generator::shutdown();
}

bool XmlParser::generate(const QStandardItem *item, bool exportAll, bool rt,
                         const std::string &filePath)
{
    Q_UNUSED(item);
    Q_UNUSED(exportAll);
    Q_UNUSED(rt);
    Q_UNUSED(filePath);
    return true;
}

bool XmlParser::generate(const TablePtr &table, const std::string &filePath)
{
    Q_UNUSED(table);
    Q_UNUSED(filePath);

    return true;
}

Parser *XmlParser::parser()
{
    return dynamic_cast<Parser *>(this);
}

const Parser *XmlParser::parser() const
{
    return dynamic_cast<const Parser *>(this);
}

} // end of namespace Icd
