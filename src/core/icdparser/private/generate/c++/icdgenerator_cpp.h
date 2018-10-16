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

    bool generate(const Icd::ObjectPtr &object, bool exportAll, bool rt,
                  const std::string &filePath);
    bool generate(const TablePtr &table, const std::string &filePath);
    bool startup();
    void shutdown();

private:
    std::shared_ptr<CppGeneratorData> d;
};

} // end of namespace Icd

#endif // ICDGENERATOR_CPP_H
