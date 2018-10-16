#ifndef ICDGENERATOR_HTML_P_H
#define ICDGENERATOR_HTML_P_H

#include "icdgenerator_html.h"
#ifndef J_NO_QT
#include <QTextStream>
#endif
#include "icdcore/icdcore_inc.h"

namespace Icd {

// class HtmlGeneratorData

class HtmlGeneratorData
{
public:
    HtmlGeneratorData();
#ifndef J_NO_QT
    bool generateTitle(const Icd::TablePtr &table, QTextStream &stream);
    bool generateTypes(QTextStream &tableStream);
    bool generatePrefix(const Icd::TablePtr &table, QTextStream &tableStream);
    bool generateSuffix(const Icd::TablePtr &table, QTextStream &tableStream);

    ////////////////////////data//////////////////////

    bool generateTable(const Icd::TablePtr &table, QTextStream &tableStream);
    bool generateItem(const Icd::ItemPtr &item, QTextStream &tableStream,
                      QTextStream &itemsStream);
    bool generateHeaderItem(const HeaderItemPtr &headItem, QTextStream &tableStream,
                            QTextStream &itemsStream);
    bool generateCounterItem(const CounterItemPtr &counterItem, QTextStream &tableStream,
                             QTextStream &itemsStream);
    bool generateCheckItem(const CheckItemPtr &checkItem, QTextStream &tableStream,
                           QTextStream &itemsStream);
    bool generateFrameCodeItem(const FrameCodeItemPtr &frameCodeItem, QTextStream &stream,
                               QTextStream &itemsStream);
    bool generateNumericItem(const NumericItemPtr &numericItem, QTextStream &tableStream,
                             QTextStream &itemsStream);
    bool generateBitItem(const BitItemPtr &bitItem, QTextStream &tableStream,
                         QTextStream &itemsStream);
    bool generateComplexItem(const Icd::ComplexItemPtr &complexItem,
                             QTextStream &tableStream, QTextStream &itemsStream);
    bool generateFrameItem(const Icd::FrameItemPtr &frameItem, QTextStream &tableStream,
                           QTextStream &itemsStream);
#endif
    ////////////////////////data//////////////////////

private:
#ifndef J_NO_QT
    QList<QString> d_variables;
#endif
    friend class HtmlGenerator;
};

} // end of namespace Icd

#endif // ICDGENERATOR_HTML_P_H
