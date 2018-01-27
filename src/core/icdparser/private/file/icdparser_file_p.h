#ifndef ICDPARSER_FILE_P_H
#define ICDPARSER_FILE_P_H

#include "../../icdparser_global.h"
#include "icdparser_file.h"

namespace Icd {

// class FileParserData

class FileParserData
{
    friend class FileParser;
public:
    FileParserData();

private:
    std::string filePath;
};

} // end of namespace Icd

#endif // ICDPARSER_FILE_P_H
