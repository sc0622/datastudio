#ifndef ICDGENERATOR_PDF_H
#define ICDGENERATOR_PDF_H

#include "../icdgenerate.h"

namespace Icd {

class PdfGenerator;
class PdfGeneratorData;
typedef JHandlePtr<PdfGenerator> PdfGeneratorPtr;

class PdfGenerator : public Generator
{
public:
    explicit PdfGenerator(Icd::Parser *parser);
    ~PdfGenerator();

    /**
     * @brief generate
     * @param item
     * @param exportAll
     * @param rt
     * @param filePath
     * @return
     */
    bool generate(const QStandardItem *item, bool exportAll, bool rt,
                  const std::string &filePath);

    /**
     * @brief generate
     * @param table
     * @param filePath
     * @param saveAsType : default: 0, doc
     * @return
     */
    bool generate(const TablePtr &table, const std::string &filePath);

private:
    JHandlePtr<PdfGeneratorData> d;
};

} // end of namespace Icd

#endif // ICDGENERATOR_PDF_H
