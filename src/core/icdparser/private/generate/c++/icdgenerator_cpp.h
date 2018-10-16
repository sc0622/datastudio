#ifndef ICDGENERATOR_CPP_H
#define ICDGENERATOR_CPP_H

#include "../icdgenerate.h"

namespace Icd {

class CppGenerator;
class CppGeneratorData;
typedef std::shared_ptr<CppGenerator> CppGeneratorPtr;

class CppGenerator : public Generator
{
public:
    explicit CppGenerator(Icd::Parser *parser);
    ~CppGenerator();
#ifndef J_NO_QT
    bool generate(const QStandardItem *item, bool exportAll, bool rt,
                  const std::string &filePath);
#endif
    bool generate(const TablePtr &table, const std::string &filePath);
    bool startup();
    void shutdown();

private:
    std::shared_ptr<CppGeneratorData> d;
};

} // end of namespace Icd

#endif // ICDGENERATOR_CPP_H
