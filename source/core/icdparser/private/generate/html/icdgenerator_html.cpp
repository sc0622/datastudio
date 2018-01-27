#include "precomp.h"
#include "icdgenerator_html.h"
#include "icdgenerator_html_p.h"

namespace Icd {

// class HtmlGenerator

HtmlGenerator::HtmlGenerator(Parser *parser)
    : Generator(parser)
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

    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::Text | QIODevice::WriteOnly)) {
        return false;
    }

    QTextStream tableStream(&file);
    tableStream.setCodec(QTextCodec::codecForName("GB2312"));

    // generate prefix
    if (!d->generatePrefix(table, tableStream)) {
        return false;
    }

    // generate title
    if (!d->generateTitle(table, tableStream)) {
        return false;
    }

    // generate types
    if (!d->generateTypes(tableStream)) {
        return false;
    }

    // generate table information
    if (!d->generateTable(table, tableStream)) {
        return false;
    }

    // generate suffix
    if (!d->generateSuffix(table, tableStream)) {
        return false;
    }

    return true;
}

} // end of namespace Icd
