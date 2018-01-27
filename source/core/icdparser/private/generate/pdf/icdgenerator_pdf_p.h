#ifndef ICDGENERATOR_PDF_P_H
#define ICDGENERATOR_PDF_P_H

#include "icdgenerator_pdf.h"

namespace Icd {

// class PdfGeneratorData

class PdfGeneratorData
{
public:
    explicit PdfGeneratorData();
    ~PdfGeneratorData();

private:
    friend class PdfGenerator;
};

} // end of namespace Icd

#endif // ICDGENERATOR_PDF_P_H
