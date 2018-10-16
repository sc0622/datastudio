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

bool WordGenerator::generate(const Icd::ObjectPtr &object, bool exportAll, bool rt,
                             const std::string &filePath)
{
    return generate(object, exportAll, rt, filePath, 0);
}

bool WordGenerator::generate(const Icd::ObjectPtr &object, bool exportAll, bool rt,
                             const std::string &filePath, int saveAsType)
{
    if (!object) {
        return false;
    }

    const std::string path = Icd::pathOfFile(filePath);
    if (path.empty()) {
        return false;
    }

    if (Icd::createPath(path) != 0) {
        return false;
    }

    //
    if (!startup()) {
        return false;
    }

    if (!d->startup()) {
        return false;
    }

    if (d->generateDocument(object, exportAll, rt)) {
        d->shutdown(filePath, saveAsType);
        return true;
    } else {
        d->shutdown();
        return false;
    }
}

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

    const std::string path = Icd::pathOfFile(filePath);
    if (path.empty()) {
        return false;
    }

    if (Icd::createPath(path) != 0) {
        return false;
    }

    if (!startup()) {
        return false;
    }

    if (!d->startup()) {
        return false;
    }

    if (d->generateDocument(table)) {
        d->shutdown(filePath, saveAsType);
        return true;
    } else {
        d->shutdown();
        return false;
    }
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
