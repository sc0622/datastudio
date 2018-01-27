#include "precomp.h"
#include "icdparser_file.h"
#include "private/icdparser_file_p.h"
#include "../icdwidget/icdwidget_global.h"
#include "private/json/icdparser_json.h"
#include "private/xml/icdparser_xml.h"
#include "jsoncpp/json/value.h"
#include "private/json/icdparser_json.h"

namespace Icd {

// class FileParserData

FileParserData::FileParserData(const std::string &filePath, Parser *parser)
{
    if (Icd::endsWith(filePath, ".json")) {
        this->parser = JHandlePtr<JsonParser>(new JsonParser(filePath, parser));
    } else if (Icd::endsWith(filePath, ".xml")) {
        this->parser = JHandlePtr<XmlParser>(new XmlParser(filePath, parser));
    }
}

// class FileParser

FileParser::FileParser(const std::string &filePath, Parser *parser)
    : Parser(SourceTypeFile)
    , d(new FileParserData(filePath, parser))
{

}

FileParser::~FileParser()
{

}

bool FileParser::parseVehicle(Icd::VehiclePtrArray &vehicles, int deep)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->parseVehicle(vehicles, deep);
}

bool FileParser::parseVehicle(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->parseVehicle(vehicleId, vehicle, deep);
}

bool FileParser::parseSystem(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->parseSystem(vehicleId, systems, deep);
}

bool FileParser::parseSystem(const std::string &vehicleId, const std::string &systemId,
                             Icd::SystemPtr &system, int deep)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->parseSystem(vehicleId, systemId, system, deep);
}

bool FileParser::parseTable(const std::string &vehicleId, const std::string &systemId,
                            Icd::TablePtrArray &tables, int deep)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->parseTable(vehicleId, systemId, tables, deep);
}

bool FileParser::parseTable(const std::string &vehicleId, const std::string &systemId,
                            const std::string &tableId, Icd::TablePtr &table, int deep)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->parseTable(vehicleId, systemId, tableId, table, deep);
}

bool FileParser::parseTable(TablePtrArray &tables)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->parseTable(tables);
}

bool FileParser::parseTable(const std::string &tableId, TablePtr &table)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->parseTable(tableId, table);
}

bool FileParser::parseItem(const std::string &vehicleId, const std::string &systemId,
                           const std::string &tableId, Icd::ItemPtrArray &items, int deep)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->parseItem(vehicleId, systemId, tableId, items, deep);
}

bool FileParser::parseItem(const std::string &vehicleId, const std::string &systemId,
                           const std::string &tableId, const std::string &itemId,
                           Icd::ItemPtr &item, int deep)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->parseItem(vehicleId, systemId, tableId, itemId, item, deep);
}

bool FileParser::save(const Icd::VehiclePtrArray &vehicles)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->save(vehicles);
}

bool FileParser::save(const TablePtrArray &tables)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->save(tables);
}

bool FileParser::save(const TablePtr &table)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->save(table);
}

bool FileParser::convert2Other(const Icd::VehiclePtrArray &vehicles, std::vector<int> &data)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->convert2Other(vehicles, data);
}

bool FileParser::convert2Core(const std::vector<int> &data, Icd::VehiclePtrArray &vehicles)
{
    if (!d->parser) {
        return false;
    }

    return d->parser->convert2Core(data, vehicles);
}

Json::Value findTable(const std::string &filePath, const std::string &vehicleId,
                      const std::string &systemId, const std::string &tableId)
{
    JsonParser *parser = new JsonParser(filePath);
    Json::Value value = parser->findTable(vehicleId, systemId, tableId);
    delete parser;
    return value;
}

} // end of namespace Icd
