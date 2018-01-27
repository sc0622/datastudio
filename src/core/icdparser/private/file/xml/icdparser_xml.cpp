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

bool XmlParser::parseVehicle(Icd::VehiclePtrArray &vehicles, int deep)
{
    //
    vehicles.clear();

    //
    TiXmlElement *emRoot = readElementRoot();
    if (!emRoot) {
        return false;   //
    }

    // parse vehicle elements
    for (TiXmlElement *emVehicle = emRoot->FirstChildElement("vehicle");
         emVehicle != Q_NULLPTR;
         emVehicle = emVehicle->NextSiblingElement("vehicle")) {
        // parse vehicle informations
        VehiclePtr vehicle(new Vehicle());
        if (!parseVehicle(emVehicle, vehicle, deep)) {
            continue;
        }

        // save
        vehicles.push_back(vehicle);
    }

    delete emRoot->GetDocument();

    return true;
}

bool XmlParser::parseVehicle(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep)
{
    vehicle = Q_NULLPTR;

    //
    TiXmlElement *emRoot = readElementRoot();
    if (!emRoot) {
        return false;   //
    }

    // parse vehicle elements
    for (TiXmlElement *emVehicle = emRoot->FirstChildElement("vehicle");
         emVehicle != Q_NULLPTR;
         emVehicle = emVehicle->NextSiblingElement("vehicle")) {
        const std::string id = emVehicle->Attribute("id");
        if (id == vehicleId) {
            VehiclePtr _vehicle(new Vehicle());
            // parse vehicle informations
            if (!parseVehicle(emVehicle, _vehicle, deep)) {
                delete emRoot->GetDocument();
                return false;   // parse failure
            }
            vehicle = _vehicle;
            break;
        }
    }

    delete emRoot->GetDocument();

    return true;
}

bool XmlParser::parseSystem(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep)
{
    //
    systems.clear();

    // parse vehicle elements
    TiXmlElement *emVehicle = findVehicleElement(vehicleId);
    if (!emVehicle) {
        return false;
    }

    // parse system elements
    for (TiXmlElement *emSystem = emVehicle->FirstChildElement("system");
         emSystem != Q_NULLPTR;
         emSystem = emSystem->NextSiblingElement("system")) {
        // parse system informations
        SystemPtr system(new System());
        if (!parseSystem(emSystem, system, deep)) {
            continue;   // parse failure
        }
        // save
        systems.push_back(system);
    }

    delete emVehicle->GetDocument();

    return true;
}

bool XmlParser::parseSystem(const std::string &vehicleId, const std::string &systemId,
                            Icd::SystemPtr &system, int deep)
{
    system = Q_NULLPTR;

    // parse vehicle elements
    TiXmlElement *emVehicle = findVehicleElement(vehicleId);
    if (!emVehicle) {
        return false;
    }

    //
    const QString _systemId = QString::fromStdString(systemId);

    // parse system elements
    for (TiXmlElement *emSystem = emVehicle->FirstChildElement("system");
         emSystem != Q_NULLPTR;
         emSystem = emSystem->NextSiblingElement("system")) {
        const QString id = QString::fromUtf8(emSystem->Attribute("id"));
        if (id == _systemId) {
            SystemPtr _system(new System());
            // parse system informations
            if (!parseSystem(emSystem, _system, deep)) {
                return false;   // parse failure
            }
            system = _system;
            break;
        }
    }

    delete emVehicle->GetDocument();

    return true;
}

bool XmlParser::parseTable(const std::string &vehicleId, const std::string &systemId,
                           Icd::TablePtrArray &tables, int deep)
{
    //
    tables.clear();

    //
    TiXmlElement *emSystem = findSystemElement(vehicleId, systemId);
    if (!emSystem) {
        return false;   //
    }

    // parse table elements
    for (TiXmlElement *emTable = emSystem->FirstChildElement("table");
         emTable != Q_NULLPTR;
         emTable = emTable->NextSiblingElement("table")) {
        // parse table informations
        TablePtr table(new Table());
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

bool XmlParser::parseTable(const std::string &vehicleId, const std::string &systemId,
                           const std::string &tableId, Icd::TablePtr &table, int deep)
{
    table = Q_NULLPTR;

    //
    TiXmlElement *emSystem = findSystemElement(vehicleId, systemId);
    if (!emSystem) {
        return false;   //
    }

    //
    const QString _tableId = QString::fromStdString(tableId);

    // parse table elements
    for (TiXmlElement *emTable = emSystem->FirstChildElement("table");
         emTable != Q_NULLPTR;
         emTable = emTable->NextSiblingElement("table")) {
        const QString id = QString::fromUtf8(emTable->Attribute("id"));
        if (id == _tableId) {
            TablePtr _table(new Table());
            if (!parseTable(emTable, _table, deep)) {
                delete emSystem->GetDocument();
                return false;   // parse failure
            }
            table = _table;
            table->setDomain(vehicleId + "/" + systemId + "/" + tableId);
            delete emSystem->GetDocument();
            return true;
        }
    }

    delete emSystem->GetDocument();

    return false;
}

bool XmlParser::parseTable(TablePtrArray &tables)
{
    tables.clear();

    //
    TiXmlElement *emRoot = readElementRoot();
    if (!emRoot) {
        return false;
    }

    // parse table elements
    for (TiXmlElement *emTable = emRoot->FirstChildElement("table");
         emTable != Q_NULLPTR;
         emTable = emTable->NextSiblingElement("table")) {
        TablePtr table(new Table());
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

bool XmlParser::parseTable(const std::string &tableId, TablePtr &table)
{
    table = Q_NULLPTR;

    //
    TiXmlElement *emTable = findTableElement(tableId);
    if (!emTable) {
        return false;   //
    }

    //
    const QString _tableId = QString::fromStdString(tableId);

    TablePtr _table(new Table());
    if (!parseTable(emTable, _table, Icd::ObjectItem)) {
        delete emTable->GetDocument();
        return false;   // parse failure
    }

    table = _table;
    table->setDomain(tableId);
    delete emTable->GetDocument();

    return true;
}

bool XmlParser::parseItem(const std::string &vehicleId, const std::string &systemId,
                          const std::string &tableId, Icd::ItemPtrArray &items, int deep)
{
    Q_UNUSED(deep);
    //
    items.clear();

    // find table element
    TiXmlElement *emTable = findTableElement(vehicleId, systemId, tableId);
    if (!emTable) {
        return false;   //
    }

    std::string sVal;

    // parse table elements
    for (TiXmlElement *emItem = emTable->FirstChildElement("item");
         emItem != Q_NULLPTR;
         emItem = emItem->NextSiblingElement("item")) {
        // parse item informations
        if (emItem->QueryStringAttribute("type", &sVal) != TIXML_SUCCESS) {
            //
        }
        const std::string sType = sVal;
        Icd::ItemPtr item = Icd::Item::create(QString::number(items.size() + 1)
                                              .toStdString(), Icd::Item::stringType(sType));
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

bool XmlParser::parseItem(const std::string &vehicleId, const std::string &systemId,
                          const std::string &tableId, const std::string &itemId,
                          Icd::ItemPtr &item, int deep)
{
    Q_UNUSED(deep);
    item = Q_NULLPTR;

    // find table element
    TiXmlElement *emTable = findTableElement(vehicleId, systemId, tableId);
    if (!emTable) {
        return false;   //
    }

    //
    const QString _dataItemId = QString::fromStdString(itemId);

    std::string sVal;

    // parse table elements
    for (TiXmlElement *emItem = emTable->FirstChildElement("item");
         emItem != Q_NULLPTR;
         emItem = emItem->NextSiblingElement("item")) {
        const QString id = QString::fromUtf8(emItem->Attribute("id"));
        if (id == _dataItemId) {
            // parse item informations
            if (emItem->QueryStringAttribute("id", &sVal) != TIXML_SUCCESS) {
                //
            }
            const std::string id = sVal;
            if (emItem->QueryStringAttribute("type", &sVal) != TIXML_SUCCESS) {
                //
            }
            const std::string sType = sVal;
            Icd::ItemPtr _item = Icd::Item::create(id, Icd::Item::stringType(sType));
            if (!_item) {
                delete emTable->GetDocument();
                return false;
            }
            if (!parseItem(emItem, _item, deep)) {
                delete emTable->GetDocument();
                return false;   // parse failure
            }
            item = _item;
            break;
        }
    }

    delete emTable->GetDocument();

    return true;
}

bool XmlParser::save(const Icd::VehiclePtrArray &vehicles)
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

bool XmlParser::save(const TablePtrArray &tables)
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

bool XmlParser::save(const TablePtr &table)
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
