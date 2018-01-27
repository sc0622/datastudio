#include "precomp.h"
#include "icdparser_file.h"
#include "icdparser_file_p.h"
#include "../../../icdwidget/icdwidget_global.h"
#include "json/icdparser_json.h"
#include "xml/icdparser_xml.h"

namespace Icd {

// class FileParserData

FileParserData::FileParserData()
{

}

// class FileParser

FileParser::FileParser(const Json::Value &config)
    : Parser(config)
    , d(new FileParserData())
{
    d->filePath = config["filePath"].asString();
}

FileParser::~FileParser()
{
    delete d;
}

FileParserPtr FileParser::create(const Json::Value &config)
{
    if (!config.isMember("filePath")) {
        return FileParserPtr();
    }

    const std::string filePath = config["filePath"].asString();
    if(filePath.empty()) {
        return FileParserPtr();
    }

    if (Icd::endsWith(filePath, ".json")) {
        return JsonParserPtr(new JsonParser(config));
    } else if (Icd::endsWith(filePath, ".xml")) {
        return XmlParserPtr(new XmlParser(config));
    }

    return FileParserPtr();
}

std::string FileParser::filePath() const
{
    return d->filePath;
}

Json::Value findTable(const std::string &filePath, const std::string &vehicleId,
                      const std::string &systemId, const std::string &tableId)
{
    Json::Value config;
    config["filePath"] = filePath;
    JsonParserPtr parser = JsonParserPtr(new JsonParser(config));
    Json::Value value = parser->findTable(vehicleId, systemId, tableId);
    return value;
}

} // end of namespace Icd
