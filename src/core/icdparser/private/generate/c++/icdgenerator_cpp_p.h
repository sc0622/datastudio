#ifndef ICDGENERATOR_CPP_P_H
#define ICDGENERATOR_CPP_P_H

#include "icdgenerator_cpp.h"
#include <QTextStream>
#include "icdcore/icdcore_inc.h"

namespace Icd {

// class CppGeneratorData

class CppGeneratorData
{
public:
    CppGeneratorData(CppGenerator *q);
    ~CppGeneratorData();

    /**
     * @brief generateHeader
     * @param stream
     * @return
     */
    bool generateHeader(QTextStream &stream);

    /**
     * @brief generateTypes
     * @param stream
     * @return
     */
    bool generateTypes(QTextStream &stream);

    /**
     * @brief generateFooter
     * @param stream
     * @return
     */
    bool generateFooter(QTextStream &stream);

    /**
     * @brief generateNamespacePrefix
     * @param stream
     * @return
     */
    bool generateNamespacePrefix(QTextStream &stream);

    /**
     * @brief generateNamespaceSuffix
     * @param stream
     * @return
     */
    bool generateNamespaceSuffix(QTextStream &stream);

    ////////////////////////data//////////////////////

    /**
     * @brief generateTable
     * @param table
     * @param tableStream
     * @return
     */
    bool generateTable(const Icd::TablePtr &table,
                       QTextStream &tableStream);

    /**
     * @brief generateDataItem
     * @param item
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateDataItem(const Icd::ItemPtr &item,
                          QTextStream &tableStream,
                          QTextStream &itemsStream);

    /**
     * @brief generateHeaderItem
     * @param headItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateHeaderItem(const Icd::HeaderItemPtr &headItem,
                            QTextStream &tableStream,
                            QTextStream &itemsStream);

    /**
     * @brief generateCounterItem
     * @param counterItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateCounterItem(const Icd::CounterItemPtr &counterItem,
                             QTextStream &tableStream,
                             QTextStream &itemsStream);

    /**
     * @brief generateCheckItem
     * @param checkItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateCheckItem(const Icd::CheckItemPtr &checkItem,
                           QTextStream &tableStream,
                           QTextStream &itemsStream);

    /**
     * @brief generateFrameCodeItem
     * @param frameCodeItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateFrameCodeItem(const Icd::FrameCodeItemPtr &frameCodeItem,
                               QTextStream &stream,
                               QTextStream &itemsStream);

    /**
     * @brief generateNumericItem
     * @param numericItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateNumericItem(const Icd::NumericItemPtr &numericItem,
                             QTextStream &tableStream,
                             QTextStream &itemsStream);

    /**
     * @brief generateBitItem
     * @param bitItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateBitItem(const Icd::BitItemPtr &bitItem,
                         QTextStream &tableStream,
                         QTextStream &itemsStream);

    /**
     * @brief generateComplexItem
     * @param complexItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateComplexItem(const Icd::ComplexItemPtr &complexItem,
                             QTextStream &tableStream,
                             QTextStream &itemsStream);

    /**
     * @brief generateFrameItem
     * @param frameItem
     * @param tableStream
     * @param itemsStream
     * @return
     */
    bool generateFrameItem(const Icd::FrameItemPtr &frameItem,
                           QTextStream &tableStream,
                           QTextStream &itemsStream);

    ////////////////////////data//////////////////////

private:
    /**
     * @brief headerName
     * @param fileName
     * @return
     */
    QString headerName(const QString &fileName);

private:
    friend class CppGenerator;
    CppGenerator *q_ptr;
    QList<QString> d_variables;
};

} // end of namespace Icd

#endif // ICDGENERATOR_CPP_P_H
