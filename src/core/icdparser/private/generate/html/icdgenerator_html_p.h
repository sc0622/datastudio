#ifndef ICDGENERATOR_HTML_P_H
#define ICDGENERATOR_HTML_P_H

#include "icdgenerator_html.h"
#include <QTextStream>
#include "icdcore/icdcore_inc.h"

namespace Icd {

// class HtmlGeneratorData

class HtmlGeneratorData
{
public:
    HtmlGeneratorData();

    /**
     * @brief generateTitle
     * @param table
     * @param stream
     * @return
     */
    bool generateTitle(const Icd::TablePtr &table, QTextStream &stream);

    /**
     * @brief generateTypes
     * @param tableStream
     * @return
     */
    bool generateTypes(QTextStream &tableStream);

    /**
     * @brief generatePrefix
     * @param table
     * @param tableStream
     * @return
     */
    bool generatePrefix(const Icd::TablePtr &table, QTextStream &tableStream);

    /**
     * @brief generateSuffix
     * @param table
     * @param tableStream
     * @return
     */
    bool generateSuffix(const Icd::TablePtr &table, QTextStream &tableStream);

    ////////////////////////data//////////////////////

    /**
     * @brief generateTable
     * @param table
     * @param tableStream
     * @return
     */
    bool generateTable(const Icd::TablePtr &table, QTextStream &tableStream);

    /**
     * @brief generateItem
     * @param item
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateItem(const Icd::ItemPtr &item, QTextStream &tableStream,
                      QTextStream &itemsStream);

    /**
     * @brief generateHeaderItem
     * @param headItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateHeaderItem(const HeaderItemPtr &headItem, QTextStream &tableStream,
                            QTextStream &itemsStream);

    /**
     * @brief generateCounterItem
     * @param counterItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateCounterItem(const CounterItemPtr &counterItem, QTextStream &tableStream,
                             QTextStream &itemsStream);

    /**
     * @brief generateCheckItem
     * @param checkItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateCheckItem(const CheckItemPtr &checkItem, QTextStream &tableStream,
                           QTextStream &itemsStream);

    /**
     * @brief generateFrameCodeItem
     * @param frameCodeItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateFrameCodeItem(const FrameCodeItemPtr &frameCodeItem, QTextStream &stream,
                               QTextStream &itemsStream);

    /**
     * @brief generateNumericItem
     * @param numericItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateNumericItem(const NumericItemPtr &numericItem, QTextStream &tableStream,
                             QTextStream &itemsStream);

    /**
     * @brief generateBitItem
     * @param bitItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateBitItem(const BitItemPtr &bitItem, QTextStream &tableStream,
                         QTextStream &itemsStream);

    /**
     * @brief generateComplexItem
     * @param complexItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateComplexItem(const Icd::ComplexItemPtr &complexItem,
                             QTextStream &tableStream, QTextStream &itemsStream);

    /**
     * @brief generateFrameItem
     * @param frameItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateFrameItem(const Icd::FrameItemPtr &frameItem, QTextStream &tableStream,
                           QTextStream &itemsStream);

    ////////////////////////data//////////////////////

private:
    QList<QString> d_variables;
    friend class HtmlGenerator;
};

} // end of namespace Icd

#endif // ICDGENERATOR_HTML_P_H
