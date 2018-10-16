#include "precomp.h"
#include "icdparser.h"
#include "icdcore/icd_root.h"
#include "icdcore/icd_vehicle.h"
#include "private/file/icdparser_file.h"
#include "private/generate/icdgenerate.h"
#include "private/sql/icdparser_sql.h"
#include "private/file/xml/icdparser_xml.h"
#include <algorithm>
#include <mutex>

namespace Icd {

class ParserData
{
    friend class Parser;
public:
    ParserData()
        : progressValue(0.0)
        , canceledSaveAs(true)
        , isBeginModify(false)
    {

    }

private:
    std::string message;
    double progressValue;
    bool canceledSaveAs;
    bool isBeginModify;
    std::mutex mutex;
};

Parser::Parser(const Json::Value &config)
    : d(new ParserData())
{
    (void)config;
}

Parser::~Parser()
{
    endModify();
    delete d;
}

ParserPtr Parser::create(const Json::Value &config)
{
    if (!config.isMember("sourceType")) {
        return ParserPtr();
    }

    const std::string sourceType = config["sourceType"].asString();
    if (sourceType.empty()) {
        return ParserPtr();
    }

    if (sourceType == "file") {
        return FileParser::create(config);
    } else if (sourceType == "sql") {
        return SqlParser::create(config);
    }

    return ParserPtr();
}

ObjectPtr Parser::parse(const std::string &domain, int objectType, int deep, Icd::Object *parent) const
{
    switch (objectType) {
    case Icd::ObjectRoot:
    {
        Icd::RootPtr root;
        if (!parse(root, std::min<int>(objectType + deep, Icd::ObjectItem))) {
            return Icd::ObjectPtr();
        }
        return root;
    }
    case Icd::ObjectVehicle:
    {
        if (parent && parent->objectType() != Icd::ObjectRoot) {
            return Icd::ObjectPtr();
        }
        Icd::VehiclePtr vehicle;
        if (!parse(domain, vehicle, std::min<int>(objectType + deep, Icd::ObjectItem), parent)) {
            return Icd::ObjectPtr();
        }
        return vehicle;
    }
    case Icd::ObjectSystem:
    {
        if (parent && parent->objectType() != Icd::ObjectVehicle) {
            return Icd::ObjectPtr();
        }
        Icd::SystemPtr system;
        if (!parse(Icd::stringSection(domain, '/', 0, 0),
                   Icd::stringSection(domain, '/', 1, 1),
                   system,
                   std::min<int>(objectType + deep, Icd::ObjectItem),
                   parent)) {
            return Icd::ObjectPtr();
        }
        return system;
    }
    case Icd::ObjectTable:
    {
        if (parent && parent->objectType() != Icd::ObjectSystem) {
            return Icd::ObjectPtr();
        }
        Icd::TablePtr table;
        if (!parse(Icd::stringSection(domain, '/', 0, 0),
                   Icd::stringSection(domain, '/', 1, 1),
                   Icd::stringSection(domain, '/', 2, 2),
                   table,
                   std::min<int>(objectType + deep, Icd::ObjectItem),
                   parent)) {
            return Icd::ObjectPtr();
        }
        return table;
    }
    default:
        break;
    }

    return Icd::ObjectPtr();
}

bool Parser::save(const std::string &domain, const ObjectPtr &object,
                  bool merge, bool fast) const
{
    (void)(domain);
    (void)(merge);
    (void)(fast);
    if (!object) {
        return false;
    }

    return false;//TODO
}

bool Icd::Parser::saveJsonObject(const std::string &path, const Json::Value &value,
                                 bool fast) const
{
    (void)(path);
    (void)(value);
    (void)(fast);
    return false;
}

bool Icd::Parser::saveJsonArray(const std::string &path, const Json::Value &value,
                                bool unique, bool fast) const
{
    (void)(path);
    (void)(value);
    (void)(unique);
    (void)(fast);
    return false;
}

bool Icd::Parser::replaceJson(const std::string &path, const Json::Value &oldValue,
                              const Json::Value &newValue, bool fast) const
{
    (void)(path);
    (void)(oldValue);
    (void)(newValue);
    (void)(fast);
    return false;
}

bool Icd::Parser::removeJson(const std::string &path, const std::string &key,
                             const Json::Value &value, bool fast) const
{
    (void)(path);
    (void)(key);
    (void)(value);
    (void)(fast);
    return false;
}

bool Icd::Parser::mergeJson(const std::string &path, const Json::Value &value, bool fast) const
{
    (void)(path);
    (void)(value);
    (void)(fast);
    return false;
}

bool Parser::beginModify()
{
    d->isBeginModify = true;

    return true;
}

bool Parser::commitModify()
{
    return true;
}

bool Parser::cancelModify()
{
    return true;
}

bool Parser::endModify()
{
    d->isBeginModify = false;

    return true;
}

bool Parser::isBeginModify() const
{
    return d->isBeginModify;
}

bool Parser::saveAs(const Icd::ObjectPtr &object, bool exportAll, bool rt,
                    const std::string &filePath)
{
    setProgressValue(0);

    if (!object) {
        return false;
    }

    Json::Value config;
    config["filePath"] = filePath;
    GeneratorPtr generator = Generator::create(config, this);
    if (!generator) {
        return false;
    }

    return generator->generate(object, exportAll, rt, filePath);
}

bool Parser::saveAs(const TablePtr &table, const std::string &filePath)
{
    setProgressValue(0);

    Json::Value config;
    config["filePath"] = filePath;
    GeneratorPtr generator = Generator::create(config, this);
    if (!generator) {
        return false;
    }

    return generator->generate(table, filePath);
}

std::string Parser::message() const
{
    d->mutex.lock();
    const std::string message = d->message;
    d->mutex.unlock();
    return message;
}

void Parser::setMessage(const std::string &message)
{
    d->mutex.lock();
    d->message = message;
    d->mutex.unlock();
}

double Parser::progressValue() const
{
    d->mutex.lock();
    const double value = d->progressValue;
    d->mutex.unlock();
    return value;
}

void Parser::setProgressValue(double value)
{
    d->mutex.lock();
    d->progressValue = value;
    d->mutex.unlock();
}

bool Parser::canceledSaveAs() const
{
    d->mutex.lock();
    const bool flag = d->canceledSaveAs;
    d->mutex.unlock();
    return flag;
}

void Parser::cancelSaveAs(bool cancel)
{
    d->mutex.lock();
    d->canceledSaveAs = cancel;
    d->mutex.unlock();
}

bool Parser::createXmlFile(const std::string &filePath)
{
    TiXmlDocument *document = Icd::XmlParser::createDocument();
    if (!document) {
        return false;
    }

    bool result = document->SaveFile(filePath);

    delete document;

    return result;
}

} // end of namespace Icd
