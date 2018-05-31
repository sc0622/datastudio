#include "precomp.h"
#include "icdparser.h"
#include "icdcore/icd_root.h"
#include "icdcore/icd_vehicle.h"
#include "private/generate/icdgenerate.h"
#include <QMutexLocker>
#include "private/file/icdparser_file.h"
#include "private/sql/icdparser_sql.h"

namespace Icd {

class ParserData
{
    friend class Parser;
public:
    ParserData()
        : progressValue(0)
        , canceledSaveAs(true)
        , mutex(QMutex::Recursive)
        , isBeginModify(false)
    {

    }

private:
    std::string message;
    qreal progressValue;
    bool canceledSaveAs;
    QMutex mutex;
    bool isBeginModify;
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
    Q_UNUSED(object);
    Q_UNUSED(merge);
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

    Json::Value config;
    config["filePath"] = filePath;
    GeneratorPtr generator = Generator::create(config, this);
    if (!generator) {
        return false;
    }

    return generator->generate(item, exportAll, rt, filePath);
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
    QMutexLocker locker(&d->mutex);
    return d->message;
}

void Parser::setMessage(const std::string &message)
{
    QMutexLocker locker(&d->mutex);
    d->message = message;
}

qreal Parser::progressValue() const
{
    QMutexLocker locker(&d->mutex);
    return d->progressValue;
}

void Parser::setProgressValue(qreal value)
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
