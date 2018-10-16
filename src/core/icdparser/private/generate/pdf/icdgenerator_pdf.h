#ifndef ICDGENERATOR_PDF_H
#define ICDGENERATOR_PDF_H

#include "../icdgenerate.h"

namespace Icd {

class PdfGenerator;
class PdfGeneratorData;
typedef std::shared_ptr<PdfGenerator> PdfGeneratorPtr;

class PdfGenerator : public Generator
{
public:
    explicit PdfGenerator(Icd::Parser *parser);
    ~PdfGenerator();
#ifndef J_NO_QT
    bool generate(const QStandardItem *item, bool exportAll, bool rt,
                  const std::string &filePath);
#endif
    bool generate(const TablePtr &table, const std::string &filePath);

private:
    std::shared_ptr<PdfGeneratorData> d;
};

} // end of namespace Icd

#endif // ICDGENERATOR_PDF_H
