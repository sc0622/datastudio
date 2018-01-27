#ifndef ICDPARSER_FILE_H
#define ICDPARSER_FILE_H

#include "../../icdparser_global.h"
#include "../../icdparser.h"

namespace Icd {

class FileParser;
class FileParserData;
typedef JHandlePtr<FileParser> FileParserPtr;

class FileParser : public Parser
{
public:
    explicit FileParser(const Json::Value &config);
    virtual ~FileParser();

    static FileParserPtr create(const Json::Value &config);

    std::string filePath() const;
    void setFilePath(const std::string &filePath);

private:
    FileParserData *d;
};

} // end of namespace Icd

#endif // ICDPARSER_FILE_H
