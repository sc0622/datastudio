#include "precomp.h"
#include "icdgenerator_html.h"
#include "icdgenerator_html_p.h"

namespace Icd {

// class HtmlGenerator

HtmlGenerator::HtmlGenerator(Parser *parser)
    : Generator(GeneratorTypeHtml, parser)
    , d(new HtmlGeneratorData())
{

}

HtmlGenerator::~HtmlGenerator()
{

}

bool HtmlGenerator::generate(const TablePtr &table, const std::string &filePath)
{
    if (!table) {
        return false;
    }

    std::ofstream ofs(filePath);
    if (!ofs) {
        printf("File \"%s\" create failure!", filePath.c_str());
        return false;
    }

    if (!startup()) {
        return false;
    }

    // generate prefix
    if (!d->generatePrefix(table, ofs)) {
        return false;
    }

    // generate title
    if (!d->generateTitle(table, ofs, filePath)) {
        return false;
    }

    // generate types
    if (!d->generateTypes(ofs)) {
        return false;
    }

    // generate table information
    if (!d->generateTable(table, ofs)) {
        return false;
    }

    // generate suffix
    if (!d->generateSuffix(table, ofs)) {
        return false;
    }

    shutdown();

    return true;
}

} // end of namespace Icd
