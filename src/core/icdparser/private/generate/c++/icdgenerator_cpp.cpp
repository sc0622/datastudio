#include "precomp.h"
#include "icdgenerator_cpp.h"
#include "icdgenerator_cpp_p.h"
#include "../../../icdparser.h"

namespace Icd {

// class CppGenerator

CppGenerator::CppGenerator(Icd::Parser *parser)
    : Generator(GeneratorTypeCpp, parser)
    , d(new CppGeneratorData(this))
{

}

CppGenerator::~CppGenerator()
{

}

bool CppGenerator::generate(const Icd::ObjectPtr &object, bool exportAll, bool rt,
                            const std::string &filePath)
{
    (void)(exportAll);
    (void)(rt);
    if (!object) {
        return false;
    }

    if (object->objectType() != Icd::ObjectTable) {
        return false;
    }

    return generate(JHandlePtrCast<Icd::Table>(object), filePath);
}

bool CppGenerator::generate(const TablePtr &table, const std::string &filePath)
{
    if (!table) {
        return false;
    }

    std::ofstream ofs(filePath);
    if (!ofs) {
        printf("File \"%s\" create failure!", filePath.c_str());
        return false;
    }

    if (!startup()) {
        return false;
    }

    // generate header
    if (!d->generateHeader(ofs, filePath)) {
        return false;
    }

    // generate types
    if (!d->generateTypes(ofs)) {
        return false;
    }

    // generate namespace prefix
    if (!d->generateNamespacePrefix(ofs)) {
        return false;
    }

    // generate table information
    if (!d->generateTable(table, ofs)) {
        return false;
    }

    // generate namespace suffix
    if (!d->generateNamespaceSuffix(ofs)) {
        return false;
    }

    // generate footer
    if (!d->generateFooter(ofs, filePath)) {
        return false;
    }

    shutdown();

    return true;
}

bool CppGenerator::startup()
{
    if (!Generator::startup()) {
        return false;
    }

    return true;
}

void CppGenerator::shutdown()
{
    Generator::shutdown();
}

} // end of namespace Icd
