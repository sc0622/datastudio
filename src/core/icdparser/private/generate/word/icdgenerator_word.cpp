#include "precomp.h"
#include "icdgenerator_word.h"
#include "icdgenerator_word_p.h"
#include "../html/icdgenerator_html.h"
#include "../../../icdparser.h"

namespace Icd {

// class WordGenerator

WordGenerator::WordGenerator(Icd::Parser *parser)
    : Generator(GeneratorTypeWord, parser)
    , d(new WordGeneratorData(this))
{

}

WordGenerator::~WordGenerator()
{

}
#ifndef J_NO_QT
bool WordGenerator::generate(const QStandardItem *item, bool exportAll, bool rt,
                             const std::string &filePath)
{
    return generate(item, exportAll, rt, filePath, 0);
}

bool WordGenerator::generate(const QStandardItem *item, bool exportAll, bool rt,
                             const std::string &filePath, int saveAsType)
{
    if (!item) {
        return false;
    }

    QFileInfo fileInfo(QString::fromStdString(filePath));
    QDir dir = QDir(fileInfo.absolutePath());
    if (!dir.exists()) {
        dir.mkpath(dir.path());
        if (!dir.exists()) {
            return false;
        }
    }

    //
    if (!startup()) {
        return false;
    }

    if (!d->startup()) {
        return false;
    }

    if (d->generateDocument(item, exportAll, rt)) {
        d->shutdown(fileInfo.absoluteFilePath(), saveAsType);
        return true;
    } else {
        d->shutdown();
        return false;
    }
}
#endif
bool WordGenerator::generate(const TablePtr &table, const std::string &filePath)
{
    return generate(table, filePath, 0);
}

bool WordGenerator::generate(const TablePtr &table, const std::string &filePath,
                             int saveAsType)
{
    if (!table) {
        return false;
    }
#ifndef J_NO_QT
    QFileInfo fileInfo(QString::fromStdString(filePath));
    QDir dir = QDir(fileInfo.absolutePath());
    if (!dir.exists()) {
        dir.mkpath(dir.path());
        if (!dir.exists()) {
            return false;
        }
    }

    if (!startup()) {
        return false;
    }

    if (!d->startup()) {
        return false;
    }

    if (d->generateDocument(table)) {
        d->shutdown(fileInfo.absoluteFilePath(), saveAsType);
        return true;
    } else {
        d->shutdown();
        return false;
    }
#else
    return false;
#endif
}

bool WordGenerator::startup()
{
    if (!Generator::startup()) {
        return false;
    }

    return true;
}

void WordGenerator::shutdown()
{
    Generator::shutdown();
}

} // end of namespace Icd
