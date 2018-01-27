#ifndef ICDPARSER_FILE_P_H
#define ICDPARSER_FILE_P_H

#include "../icdparser_global.h"
#include "../icdparser_file.h"

namespace Icd {

// class FileParserData

class JsonParser;
class XmlParser;

class FileParserData
{
    friend class FileParser;
public:
    FileParserData(const std::string &filePath, Parser *parser);

private:
    JHandlePtr<Parser> parser;
};

} // end of namespace Icd

#endif // ICDPARSER_FILE_P_H
