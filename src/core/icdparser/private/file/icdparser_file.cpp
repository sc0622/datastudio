#include "precomp.h"
#include "icdparser_file.h"
#include "icdparser_file_p.h"
#include "json/icdparser_json.h"
#if defined(_MSC_VER)
#include "../../../icdwidget/icdwidget_global.h"
#include "xml/icdparser_xml.h"
#endif
#include <fstream>
#if defined(_MSC_VER)
#include <io.h>
#include <direct.h>
#include <stdint.h>
#elif defined(__unix__)
#include <unistd.h>
#include <sys/stat.h>
#elif defined(__APPLE__)
#include <unistd.h>
#include <sys/stat.h>
#else
#error(not supported os!)
#endif

#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN 256
#endif

#ifdef _MSC_VER
#define ACCESS(fileName, accessMode) \
    _access(fileName, accessMode)
#define MKDIR(path) _mkdir(path)
#else
#define ACCESS(fileName, accessMode) \
    access(fileName, accessMode)
#define MKDIR(path) mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#endif

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
#ifndef J_NO_QT
    const QString filePath = QString::fromStdString(config["filePath"].asString());
    if (filePath.toLower().trimmed().endsWith(".json")) {
        d->filePath = filePath.toLocal8Bit().data();
    } else {
        d->filePath = filePath.toStdString();
    }
#else
    d->filePath = config["filePath"].asString();
#endif
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
#if defined(_MSC_VER)
        return XmlParserPtr(new XmlParser(config));
#endif
    }

    return FileParserPtr();
}

std::string FileParser::filePath() const
{
    return d->filePath;
}

void FileParser::setFilePath(const std::string &filePath)
{
    if (d->filePath.empty()) {
        d->filePath = filePath;
    } else if (suffix() == suffix(filePath)) {
        d->filePath = filePath;
    }
}

std::string FileParser::basePath() const
{
    std::string::size_type index = d->filePath.find_last_of('/');
    if (index == std::string::npos) {
        return d->filePath.substr(0, d->filePath.find_last_of('\\'));
    } else {
        return d->filePath.substr(0, index);
    }
}

std::string FileParser::fileName() const
{
    std::string::size_type index = d->filePath.find_last_of('/');
    if (index == std::string::npos) {
        index = d->filePath.find_last_of('\\');
        if (index == std::string::npos) {
            return d->filePath;
        } else {
            return d->filePath.substr(index + 1);
        }
    } else {
        return d->filePath.substr(index + 1);
    }
}

std::string FileParser::suffix() const
{
    return suffix(d->filePath);
}

std::string FileParser::suffix(const std::string &fileName)
{
    std::string::size_type index = fileName.find_last_of('.');
    if (index == std::string::npos) {
        return std::string();
    }

    return fileName.substr(index + 1);
}

bool FileParser::exists(const std::string &filePath)
{
    return (ACCESS(filePath.c_str(), 0) != -1);
}

bool FileParser::copy(const std::string &from, const std::string &to)
{
    std::ifstream in(from, std::ios::binary);
    if (!in.is_open()) {
        return false;
    }
    std::ofstream out(to, std::ios::binary);
    if (!out.is_open()) {
        return false;
    }

    out << in.rdbuf();

    return true;
}

bool FileParser::rename(const std::string &from, const std::string &to)
{
    return (::rename(from.c_str(), to.c_str()) != 0);
}

bool FileParser::remove(const std::string &filePath)
{
    return (::remove(filePath.c_str()) == 0);
}

std::string FileParser::fullPath(const std::string &basePath, const std::string &fileName)
{
    if (basePath.empty()) {
        return fileName;
    } else if (fileName.empty()) {
        return basePath;
    } else {
        return basePath + '/' + fileName;
    }
}

Json::Value queryTable(const std::string &filePath, const std::string &vehicleId,
                       const std::string &systemId, const std::string &tableId)
{
    Json::Value config;
    config["filePath"] = filePath;
    JsonParserPtr parser = JsonParserPtr(new JsonParser(config));
    Json::Value value = parser->queryTable(vehicleId, systemId, tableId);
    return value;
}

Json::Value queryTable(const std::string &filePath, const std::string &domain, int domainType)
{
    Json::Value config;
    config["filePath"] = filePath;
    JsonParserPtr parser = JsonParserPtr(new JsonParser(config));
    Json::Value value = parser->queryTable(domain, domainType);
    return value;
}

} // end of namespace Icd
