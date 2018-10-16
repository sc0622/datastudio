#ifndef ICDGENERATOR_HTML_P_H
#define ICDGENERATOR_HTML_P_H

#include "icdgenerator_html.h"
#include "icdcore/icdcore_inc.h"

namespace Icd {

// class HtmlGeneratorData

class HtmlGeneratorData
{
public:
    HtmlGeneratorData();
    bool generateTitle(const Icd::TablePtr &table, std::ostream &stream,
                       const std::string &filePath);
    bool generateTypes(std::ostream &tableStream);
    bool generatePrefix(const Icd::TablePtr &table, std::ostream &tableStream);
    bool generateSuffix(const Icd::TablePtr &table, std::ostream &tableStream);

    ////////////////////////data//////////////////////

    bool generateTable(const Icd::TablePtr &table, std::ostream &tableStream);
    bool generateItem(const Icd::ItemPtr &item, std::ostream &tableStream,
                      std::ostream &itemsStream);
    bool generateHeaderItem(const HeaderItemPtr &headItem, std::ostream &tableStream,
                            std::ostream &itemsStream);
    bool generateCounterItem(const CounterItemPtr &counterItem, std::ostream &tableStream,
                             std::ostream &itemsStream);
    bool generateCheckItem(const CheckItemPtr &checkItem, std::ostream &tableStream,
                           std::ostream &itemsStream);
    bool generateFrameCodeItem(const FrameCodeItemPtr &frameCodeItem, std::ostream &stream,
                               std::ostream &itemsStream);
    bool generateNumericItem(const NumericItemPtr &numericItem, std::ostream &tableStream,
                             std::ostream &itemsStream);
    bool generateBitItem(const BitItemPtr &bitItem, std::ostream &tableStream,
                         std::ostream &itemsStream);
    bool generateComplexItem(const Icd::ComplexItemPtr &complexItem,
                             std::ostream &tableStream, std::ostream &itemsStream);
    bool generateFrameItem(const Icd::FrameItemPtr &frameItem, std::ostream &tableStream,
                           std::ostream &itemsStream);

    ////////////////////////data//////////////////////

private:
    std::list<std::string> d_variables;
    friend class HtmlGenerator;
};

} // end of namespace Icd

#endif // ICDGENERATOR_HTML_P_H
