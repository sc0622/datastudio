#include "precomp.h"
#include "icdparser.h"
#include "icdcore/icd_root.h"
#include "icdcore/icd_vehicle.h"
#include <QMutexLocker>
#include "private/file/icdparser_file.h"
#if defined(_MSC_VER)
#include "private/generate/icdgenerate.h"
#include "private/sql/icdparser_sql.h"
#endif

namespace Icd {

class ParserData
{
    friend class Parser;
public:
    ParserData()
        : progressValue(0)
        , canceledSaveAs(true)
        , isBeginModify(false)
        , mutex(QMutex::Recursive)
    {

    }

private:
    std::string message;
    qreal progressValue;
    bool canceledSaveAs;
    bool isBeginModify;
    QMutex mutex;
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
#if defined(_MSC_VER)
        return SqlParser::create(config);
#endif
    }

    return ParserPtr();
}

ObjectPtr Parser::parse(const std::string &domain, int objectType, int deep) const
{
    switch (objectType) {
    case Icd::ObjectRoot:
    {
        Icd::RootPtr root;
        if (!parse(root, qMin<int>(objectType + deep, Icd::ObjectItem))) {
            return Icd::ObjectPtr();
        }
        return root;
    }
    case Icd::ObjectVehicle:
    {
        Icd::VehiclePtr vehicle;
        if (!parse(domain, vehicle, qMin<int>(objectType + deep, Icd::ObjectItem))) {
            return Icd::ObjectPtr();
        }
        return vehicle;
    }
    case Icd::ObjectSystem:
    {
        Icd::SystemPtr system;
        if (!parse(Icd::stringSection(domain, '/', 0, 0),
                   Icd::stringSection(domain, '/', 1, 1),
                   system, qMin<int>(objectType + deep, Icd::ObjectItem))) {
            return Icd::ObjectPtr();
        }
        return system;
    }
    case Icd::ObjectTable:
    {
        Icd::TablePtr table;
        if (!parse(Icd::stringSection(domain, '/', 0, 0),
                   Icd::stringSection(domain, '/', 1, 1),
                   Icd::stringSection(domain, '/', 2, 2),
                   table, qMin<int>(objectType + deep, Icd::ObjectItem))) {
            return Icd::ObjectPtr();
        }
        return table;
    }
    case Icd::ObjectItem:
    {
        Icd::TablePtr table;
        if (!parse(Icd::stringSection(domain, '/', 0, 0),
                   Icd::stringSection(domain, '/', 1, 1),
                   Icd::stringSection(domain, '/', 2, 2),
                   table, qMin<int>(objectType + deep, Icd::ObjectItem))) {
            return Icd::ObjectPtr();
        }
        return table->itemByDomain(Icd::stringSection(domain, '/', 3));
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
    QMutexLocker locker(&d->mutex);
    return d->message;
}

void Parser::setMessage(const std::string &message)
{
    QMutexLocker locker(&d->mutex);
    d->message = message;
}

double Parser::progressValue() const
{
    QMutexLocker locker(&d->mutex);
    return d->progressValue;
}

void Parser::setProgressValue(double value)
{
    QMutexLocker locker(&d->mutex);
    d->progressValue = value;
}

bool Parser::canceledSaveAs() const
{
    QMutexLocker locker(&d->mutex);
    return d->canceledSaveAs;
}

void Parser::cancelSaveAs(bool cancel)
{
    QMutexLocker locker(&d->mutex);
    d->canceledSaveAs = cancel;
}

} // end of namespace Icd
