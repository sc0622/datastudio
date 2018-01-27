#include "precomp.h"
#include "icdgenerate.h"
#include "c++/icdgenerator_cpp.h"
#include "html/icdgenerator_html.h"
//#include "excel/icdgenerator_excel.h"
#include "word/icdgenerator_word.h"
#include "pdf/icdgenerator_pdf.h"
#include "../json/icdparser_json.h"
#include "../xml/icdparser_xml.h"
#include "../../icdparser.h"

namespace Icd {

// class GeneratorData

class GeneratorData
{
public:
    GeneratorData(Icd::Parser *parser)
        : parser(parser)
    {

    }

private:
    friend class Generator;
    Icd::Parser *parser;
};

// class Generator

Generator::Generator(Parser *parser)
    : d(new GeneratorData(parser))
{

}

Generator::~Generator()
{

}

std::string Generator::fileType(const std::string &filePath)
{
    QFileInfo fileInfo(QString::fromStdString(filePath));
    const QString suffix = fileInfo.suffix().toLower();
    if (suffix == "xml") {
        return "xml";
    } else if (suffix == "json") {
        return "json";
    } else if (suffix == "cs") {
        return "c#";
    } else if (suffix == "h" || suffix == "hpp" || suffix == "hxx") {
        return "c++";
    } else if (suffix == "htm" || suffix == "html") {
        return "html";
    } else if (suffix == "xls" || suffix == "xlsx") {
        return "excel";
    } else if (suffix == "m") {
        return "matlab";
    } else if (suffix == "doc" || suffix == "docx") {
        return "word";
    } else if (suffix == "pdf") {
        return "pdf";
    }

    return std::string();
}

GeneratorPtr Generator::create(const std::string &filePath, Icd::Parser *parser)
{
    const std::string fileType = Generator::fileType(filePath);

    if (fileType == "xml") {
        return XmlParserPtr(new XmlParser(filePath, parser));
    } else if (fileType == "json") {
        return JsonParserPtr(new JsonParser(filePath, parser));
    } else if (fileType == "c#") {
        //
    } else if (fileType == "c++") {
        return CppGeneratorPtr(new CppGenerator(parser));
    } else if (fileType == "html") {
        return HtmlGeneratorPtr(new HtmlGenerator(parser));
    } else if (fileType == "excel") {
        //
    } else if (fileType == "matlab") {
        //
    } else if (fileType == "word") {
        return WordGeneratorPtr(new WordGenerator(parser));
    } else if (fileType == "pdf") {
        return PdfGeneratorPtr(new PdfGenerator(parser));
    }

    return GeneratorPtr(0);
}

bool Generator::startup()
{
    parser()->cancelSaveAs(false);

    return true;
}

void Generator::shutdown()
{

}

bool Generator::generate(const QStandardItem *item, bool exportAll, bool rt,
                         const std::string &filePath)
{
    Q_UNUSED(item);
    Q_UNUSED(exportAll);
    Q_UNUSED(rt);
    Q_UNUSED(filePath);
    return false;
}

Parser *Generator::parser() const
{
    return d->parser;
}

} // end of namespace Icd
