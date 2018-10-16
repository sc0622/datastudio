#include "precomp.h"
#include "icdparser.h"
#include "icdcore/icd_root.h"
#include "icdcore/icd_vehicle.h"
#ifndef J_NO_QT
#include <QMutexLocker>
#endif
#include "private/file/icdparser_file.h"
#ifdef _MSC_VER
#include "private/generate/icdgenerate.h"
#include "private/sql/icdparser_sql.h"
#endif
#include "private/file/xml/icdparser_xml.h"

namespace Icd {

class ParserData
{
    friend class Parser;
public:
    ParserData()
        : progressValue(0)
        , canceledSaveAs(true)
        , isBeginModify(false)
    #ifndef J_NO_QT
        , mutex(QMutex::Recursive)
    #endif
    {

    }

private:
    std::string message;
    qreal progressValue;
    bool canceledSaveAs;
    bool isBeginModify;
#ifndef J_NO_QT
    QMutex mutex;
#endif
};

Parser::Parser(const Json::Value &config)
    : d(new ParserData())
{
    Q_UNUSED(config);
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
    Q_UNUSED(domain);
    Q_UNUSED(merge);
    Q_UNUSED(fast);
    if (!object) {
        return false;
    }

    return false;//TODO
}

bool Icd::Parser::saveJsonObject(const std::string &path, const Json::Value &value,
                                 bool fast) const
{
    Q_UNUSED(path);
    Q_UNUSED(value);
    Q_UNUSED(fast);
    return false;
}

bool Icd::Parser::saveJsonArray(const std::string &path, const Json::Value &value,
                                bool unique, bool fast) const
{
    Q_UNUSED(path);
    Q_UNUSED(value);
    Q_UNUSED(unique);
    Q_UNUSED(fast);
    return false;
}

bool Icd::Parser::replaceJson(const std::string &path, const Json::Value &oldValue,
                              const Json::Value &newValue, bool fast) const
{
    Q_UNUSED(path);
    Q_UNUSED(oldValue);
    Q_UNUSED(newValue);
    Q_UNUSED(fast);
    return false;
}

bool Icd::Parser::removeJson(const std::string &path, const std::string &key,
                             const Json::Value &value, bool fast) const
{
    Q_UNUSED(path);
    Q_UNUSED(key);
    Q_UNUSED(value);
    Q_UNUSED(fast);
    return false;
}

bool Icd::Parser::mergeJson(const std::string &path, const Json::Value &value, bool fast) const
{
    Q_UNUSED(path);
    Q_UNUSED(value);
    Q_UNUSED(fast);
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
#ifndef J_NO_QT
bool Parser::saveAs(const QStandardItem *item, bool exportAll, bool rt,
                    const std::string &filePath)
{
    setProgressValue(0);

    if (!item) {
        return false;
    }
#if defined(_MSC_VER)
    Json::Value config;
    config["filePath"] = filePath;
    GeneratorPtr generator = Generator::create(config, this);
    if (!generator) {
        return false;
    }

    return generator->generate(item, exportAll, rt, filePath);
#else
    Q_UNUSED(item);
    Q_UNUSED(exportAll);
    Q_UNUSED(rt);
    Q_UNUSED(filePath);
    return false;
#endif
}
#endif
bool Parser::saveAs(const TablePtr &table, const std::string &filePath)
{
    setProgressValue(0);
#if defined(_MSC_VER)
    Json::Value config;
    config["filePath"] = filePath;
    GeneratorPtr generator = Generator::create(config, this);
    if (!generator) {
        return false;
    }

    return generator->generate(table, filePath);
#else
    Q_UNUSED(table);
    Q_UNUSED(filePath);
    return false;
#endif
}

std::string Parser::message() const
{
#ifndef J_NO_QT
    QMutexLocker locker(&d->mutex);
#endif
    return d->message;
}

void Parser::setMessage(const std::string &message)
{
#ifndef J_NO_QT
    QMutexLocker locker(&d->mutex);
#endif
    d->message = message;
}

double Parser::progressValue() const
{
#ifndef J_NO_QT
    QMutexLocker locker(&d->mutex);
#endif
    return d->progressValue;
}

void Parser::setProgressValue(double value)
{
#ifndef J_NO_QT
    QMutexLocker locker(&d->mutex);
#endif
    d->progressValue = value;
}

bool Parser::canceledSaveAs() const
{
#ifndef J_NO_QT
    QMutexLocker locker(&d->mutex);
#endif
    return d->canceledSaveAs;
}

void Parser::cancelSaveAs(bool cancel)
{
#ifndef J_NO_QT
    QMutexLocker locker(&d->mutex);
#endif
    d->canceledSaveAs = cancel;
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
