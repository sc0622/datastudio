#ifndef ICDGENERATOR_WORD_H
#define ICDGENERATOR_WORD_H

#include "../icdgenerate.h"

namespace Icd {

class WordGenerator;
class WordGeneratorData;
typedef JHandlePtr<WordGenerator> WordGeneratorPtr;

class WordGenerator : public Generator
{
public:
    explicit WordGenerator(Icd::Parser *parser);
    ~WordGenerator();

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
     * @param item
     * @param exportAll
     * @param rt
     * @param filePath
     * @param saveAsType
     * @return
     */
    bool generate(const QStandardItem *item, bool exportAll, bool rt,
                  const std::string &filePath, int saveAsType);

    /**
     * @brief generate
     * @param table
     * @param filePath
     * @param saveAsType : default: 0, doc
     * @return
     */
    bool generate(const TablePtr &table, const std::string &filePath);

    /**
     * @brief generate
     * @param table
     * @param filePath
     * @param saveAsType
     * @return
     */
    bool generate(const TablePtr &table, const std::string &filePath, int saveAsType);

    /**
     * @brief startup
     * @return
     */
    bool startup();

    /**
     * @brief shutdown
     */
    void shutdown();

private:
    JHandlePtr<WordGeneratorData> d;
};

} // end of namespace Icd

#endif // ICDGENERATOR_WORD_H
