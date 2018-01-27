#include "precomp.h"
#include "icdparser.h"
#include "icd_vehicle.h"
#include "private/generate/icdgenerate.h"
#include <QMutexLocker>

namespace Icd {

class ParserData
{
    friend class Parser;
public:
    ParserData(Parser::SourceType sourceType)
        : sourceType(sourceType)
        , canceledSaveAs(true)
        , progressValue(0)
        , mutex(QMutex::Recursive)
    {

    }

private:
    Parser::SourceType sourceType;
    std::string message;
    qreal progressValue;
    bool canceledSaveAs;
    QMutex mutex;
};

Parser::Parser(SourceType sourceType)
    : d(new ParserData(sourceType))
{

}

Parser::~Parser()
{

}

bool Parser::isValid() const
{
    return (d->sourceType != SourceTypeInvalid);
}

Parser::SourceType Parser::sourceType() const
{
    return d->sourceType;
}

bool Parser::saveAs(const QStandardItem *item, bool exportAll, bool rt,
                    const std::string &filePath)
{
    setProgressValue(0);

    if (!item) {
        return false;
    }

    GeneratorPtr generator = Generator::create(filePath, this);
    if (!generator) {
        return false;
    }

    return generator->generate(item, exportAll, rt, filePath);
}

bool Parser::saveAs(const TablePtr &table, const std::string &filePath)
{
    setProgressValue(0);

    GeneratorPtr generator = Generator::create(filePath, nullptr);
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
