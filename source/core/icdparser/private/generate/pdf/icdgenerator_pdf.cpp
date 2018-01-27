#include "precomp.h"
#include "icdgenerator_pdf.h"
#include "icdgenerator_pdf_p.h"
#include "../word/icdgenerator_word.h"
#include <QTextDocument>
#include <QPrinter>
#include <QPdfWriter>

namespace Icd {

// class PdfGenerator

PdfGenerator::PdfGenerator(Parser *parser)
    : Generator(parser)
    , d(new PdfGeneratorData())
{

}

PdfGenerator::~PdfGenerator()
{

}

bool PdfGenerator::generate(const QStandardItem *item, bool exportAll, bool rt,
                            const std::string &filePath)
{
    if (!item) {
        return false;
    }

    // generate
    if (!WordGenerator(parser()).generate(item, exportAll, rt, filePath, 17)) {
        return false;
    }

    return true;
}

bool PdfGenerator::generate(const TablePtr &table, const std::string &filePath)
{
    if (!table) {
        return false;
    }

    // generate
    if (!WordGenerator(parser()).generate(table, filePath, 17)) {
        return false;
    }

    return true;
}

} // end of namespace Icd
