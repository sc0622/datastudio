#ifndef ICDGENERATOR_WORD_H
#define ICDGENERATOR_WORD_H

#include "../icdgenerate.h"

namespace Icd {

class WordGenerator;
class WordGeneratorData;
typedef std::shared_ptr<WordGenerator> WordGeneratorPtr;

class WordGenerator : public Generator
{
public:
    explicit WordGenerator(Icd::Parser *parser);
    ~WordGenerator();
#ifndef J_NO_QT
    bool generate(const QStandardItem *item, bool exportAll, bool rt,
                  const std::string &filePath);
    bool generate(const QStandardItem *item, bool exportAll, bool rt,
                  const std::string &filePath, int saveAsType);
#endif
    bool generate(const TablePtr &table, const std::string &filePath);
    bool generate(const TablePtr &table, const std::string &filePath, int saveAsType);
    bool startup();
    void shutdown();

private:
    std::shared_ptr<WordGeneratorData> d;
};

} // end of namespace Icd

#endif // ICDGENERATOR_WORD_H
