#ifndef ICDGENERATOR_CPP_H
#define ICDGENERATOR_CPP_H

#include "../icdgenerate.h"

namespace Icd {

class CppGenerator;
class CppGeneratorData;
typedef JHandlePtr<CppGenerator> CppGeneratorPtr;

class CppGenerator : public Generator
{
public:
    explicit CppGenerator(Icd::Parser *parser);
    ~CppGenerator();

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
     * @return
     */
    bool generate(const TablePtr &table, const std::string &filePath);

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
    JHandlePtr<CppGeneratorData> d;
};

} // end of namespace Icd

#endif // ICDGENERATOR_CPP_H
