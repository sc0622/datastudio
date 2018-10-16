#include "precomp.h"
#include "icdgenerate.h"
#include "c++/icdgenerator_cpp.h"
#include "html/icdgenerator_html.h"
//#include "excel/icdgenerator_excel.h"
#include "word/icdgenerator_word.h"
#include "pdf/icdgenerator_pdf.h"
#include "../file/json/icdparser_json.h"
#include "../file/xml/icdparser_xml.h"
#include "../../icdparser.h"
#include <algorithm>

namespace Icd {

// class GeneratorData

class GeneratorData
{
public:
    GeneratorData()
        : generatorType(Generator::GeneratorTypeInvalid)
        , parser(nullptr)
    {

    }

private:
    friend class Generator;
    Generator::GeneratorType generatorType;
    Icd::Parser *parser;
};

// class Generator

Generator::Generator(GeneratorType type, Icd::Parser *parser)
    : d(new GeneratorData())
{
    d->generatorType = type;
    d->parser = parser;
}

Generator::~Generator()
{

}

Generator::GeneratorType Generator::generateType() const
{
    return d->generatorType;
}

std::string Generator::fileType(const std::string &filePath)
{
    std::string::size_type index = filePath.find_last_of('.');
    if (index == std::string::npos) {
        return std::string();
    }

    std::string suffix = filePath.substr(index + 1);
    if (suffix.empty()) {
        return std::string();
    }

    std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);

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

GeneratorPtr Generator::create(const Json::Value &config, Icd::Parser *parser)
{
    if (!config.isMember("filePath")) {
        return GeneratorPtr();
    }

    const std::string filePath = config["filePath"].asString();
    if (filePath.empty()) {
        return GeneratorPtr();
    }

    const std::string fileType = Generator::fileType(filePath);
    if (fileType.empty()) {
        return GeneratorPtr();
    }

    if (fileType == "xml") {
        return XmlParserPtr(new XmlParser(config));
    } else if (fileType == "json") {
        return JsonParserPtr(new JsonParser(config));
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

    return GeneratorPtr();
}

bool Generator::startup()
{
    Icd::Parser *parser = this->parser();
    if (!parser) {
        return false;
    }

    parser->cancelSaveAs(false);

    return true;
}

void Generator::shutdown()
{

}

bool Generator::generate(const Icd::ObjectPtr &object, bool exportAll, bool rt,
                         const std::string &filePath)
{
    (void)(object);
    (void)(exportAll);
    (void)(rt);
    (void)(filePath);
    return false;
}

Icd::Parser *Generator::parser()
{
    return d->parser;
}

const Icd::Parser *Generator::parser() const
{
    return d->parser;
}

} // end of namespace Icd
