#ifndef ICDGENERATOR_HTML_H
#define ICDGENERATOR_HTML_H

#include "../icdgenerate.h"

namespace Icd {

class HtmlGenerator;
class HtmlGeneratorData;
typedef std::shared_ptr<HtmlGenerator> HtmlGeneratorPtr;

class HtmlGenerator : public Generator
{
public:
    explicit HtmlGenerator(Icd::Parser *parser);
    ~HtmlGenerator();

    bool generate(const TablePtr &table, const std::string &filePath);

private:
    std::shared_ptr<HtmlGeneratorData> d;
};

} // end of namespace Icd

#endif // ICDGENERATOR_HTML_H
