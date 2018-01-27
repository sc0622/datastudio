#include "precomp.h"
#include "icdparser.h"
#include "icd_vehicle.h"
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
        : canceledSaveAs(true)
        , progressValue(0)
        , mutex(QMutex::Recursive)
    {

    }

private:
    std::string message;
    qreal progressValue;
    bool canceledSaveAs;
    QMutex mutex;
};

Parser::Parser(const Json::Value &config)
    : d(new ParserData())
{
    Q_UNUSED(config);
}

Parser::~Parser()
{
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
