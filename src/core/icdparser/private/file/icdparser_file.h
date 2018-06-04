#ifndef ICDPARSER_FILE_H
#define ICDPARSER_FILE_H

#include "../../icdparser_global.h"
#include "../../icdparser.h"

namespace Icd {

class FileParser;
class FileParserData;
typedef std::shared_ptr<FileParser> FileParserPtr;

class FileParser : public Parser
{
public:
    explicit FileParser(const Json::Value &config);
    virtual ~FileParser();

    static FileParserPtr create(const Json::Value &config);

    std::string filePath() const;
    void setFilePath(const std::string &filePath);

    std::string basePath() const;
    std::string fileName() const;
    std::string suffix() const;

    static std::string suffix(const std::string &fileName);
    static bool exists(const std::string &filePath);
    static bool copy(const std::string &from, const std::string &to);
    static bool rename(const std::string &from, const std::string &to);
    static bool remove(const std::string &filePath);
    static std::string fullPath(const std::string &basePath, const std::string &fileName);

private:
    FileParserData *d;
};

} // end of namespace Icd

#endif // ICDPARSER_FILE_H
