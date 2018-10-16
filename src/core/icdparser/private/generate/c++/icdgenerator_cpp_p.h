#ifndef ICDGENERATOR_CPP_P_H
#define ICDGENERATOR_CPP_P_H

#include "icdgenerator_cpp.h"
#include "icdcore/icdcore_inc.h"

namespace Icd {

// class CppGeneratorData

class CppGeneratorData
{
public:
    CppGeneratorData(CppGenerator *q);
    ~CppGeneratorData();

    bool generateHeader(std::ostream &stream, const std::string &filePath);
    bool generateTypes(std::ostream &stream);
    bool generateFooter(std::ostream &stream, const std::string &filePath);
    bool generateNamespacePrefix(std::ostream &stream);
    bool generateNamespaceSuffix(std::ostream &stream);

    ////////////////////////data//////////////////////

    bool generateTable(const Icd::TablePtr &table, std::ostream &tableStream);
    bool generateDataItem(const Icd::ItemPtr &item,
                          std::ostream &tableStream, std::ostream &itemsStream);
    bool generateHeaderItem(const Icd::HeaderItemPtr &headItem,
                            std::ostream &tableStream, std::ostream &itemsStream);
    bool generateCounterItem(const Icd::CounterItemPtr &counterItem,
                             std::ostream &tableStream, std::ostream &itemsStream);
    bool generateCheckItem(const Icd::CheckItemPtr &checkItem,
                           std::ostream &tableStream, std::ostream &itemsStream);
    bool generateFrameCodeItem(const Icd::FrameCodeItemPtr &frameCodeItem,
                               std::ostream &tableStream, std::ostream &itemsStream);
    bool generateNumericItem(const Icd::NumericItemPtr &numericItem,
                             std::ostream &tableStream, std::ostream &itemsStream);
    bool generateBitItem(const Icd::BitItemPtr &bitItem,
                         std::ostream &tableStream, std::ostream &itemsStream);
    bool generateComplexItem(const Icd::ComplexItemPtr &complexItem,
                             std::ostream &tableStream, std::ostream &itemsStream);
    bool generateFrameItem(const Icd::FrameItemPtr &frameItem,
                           std::ostream &tableStream, std::ostream &itemsStream);
    bool generateDateTimeItem(const Icd::DateTimeItemPtr &dateTimeItem,
                              std::ostream &tableStream, std::ostream &itemsStream);
    bool generateArrayItem(const Icd::ArrayItemPtr &arrayItem,
                              std::ostream &tableStream, std::ostream &itemsStream);

    ////////////////////////data//////////////////////

private:
    std::string headerName(const std::string &fileName);

private:
    friend class CppGenerator;
    CppGenerator *q_ptr_;
    std::list<std::string> d_variables;
};

} // end of namespace Icd

#endif // ICDGENERATOR_CPP_P_H
