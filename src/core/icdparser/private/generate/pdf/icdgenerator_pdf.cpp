#include "precomp.h"
#include "icdgenerator_pdf.h"
#include "icdgenerator_pdf_p.h"
#include "../word/icdgenerator_word.h"

namespace Icd {

// class PdfGenerator

PdfGenerator::PdfGenerator(Parser *parser)
    : Generator(GeneratorTypePdf, parser)
    , d(new PdfGeneratorData())
{

}

PdfGenerator::~PdfGenerator()
{

}

bool PdfGenerator::generate(const Icd::ObjectPtr &object, bool exportAll, bool rt,
                            const std::string &filePath)
{
    if (!object) {
        return false;
    }

    // generate
    if (!WordGenerator(parser()).generate(object, exportAll, rt, filePath, 17)) {
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
