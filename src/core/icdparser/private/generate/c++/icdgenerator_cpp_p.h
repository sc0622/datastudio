#ifndef ICDGENERATOR_CPP_P_H
#define ICDGENERATOR_CPP_P_H

#include "icdgenerator_cpp.h"
#ifndef J_NO_QT
#include <QTextStream>
#endif
#include "icdcore/icdcore_inc.h"

namespace Icd {

// class CppGeneratorData

class CppGeneratorData
{
public:
    CppGeneratorData(CppGenerator *q);
    ~CppGeneratorData();
#ifndef J_NO_QT
    bool generateHeader(QTextStream &stream);
    bool generateTypes(QTextStream &stream);
    bool generateFooter(QTextStream &stream);
    bool generateNamespacePrefix(QTextStream &stream);
    bool generateNamespaceSuffix(QTextStream &stream);

    ////////////////////////data//////////////////////

    bool generateTable(const Icd::TablePtr &table,
                       QTextStream &tableStream);
    bool generateDataItem(const Icd::ItemPtr &item,
                          QTextStream &tableStream,
                          QTextStream &itemsStream);
    bool generateHeaderItem(const Icd::HeaderItemPtr &headItem,
                            QTextStream &tableStream,
                            QTextStream &itemsStream);
    bool generateCounterItem(const Icd::CounterItemPtr &counterItem,
                             QTextStream &tableStream,
                             QTextStream &itemsStream);
    bool generateCheckItem(const Icd::CheckItemPtr &checkItem,
                           QTextStream &tableStream,
                           QTextStream &itemsStream);
    bool generateFrameCodeItem(const Icd::FrameCodeItemPtr &frameCodeItem,
                               QTextStream &stream,
                               QTextStream &itemsStream);
    bool generateNumericItem(const Icd::NumericItemPtr &numericItem,
                             QTextStream &tableStream,
                             QTextStream &itemsStream);
    bool generateBitItem(const Icd::BitItemPtr &bitItem,
                         QTextStream &tableStream,
                         QTextStream &itemsStream);
    bool generateComplexItem(const Icd::ComplexItemPtr &complexItem,
                             QTextStream &tableStream,
                             QTextStream &itemsStream);
    bool generateFrameItem(const Icd::FrameItemPtr &frameItem,
                           QTextStream &tableStream,
                           QTextStream &itemsStream);
    bool generateDateTimeItem(const Icd::DateTimeItemPtr &dateTimeItem,
                              QTextStream &tableStream,
                              QTextStream &itemsStream);
    bool generateArrayItem(const Icd::ArrayItemPtr &arrayItem,
                              QTextStream &tableStream,
                              QTextStream &itemsStream);

    ////////////////////////data//////////////////////

private:
    QString headerName(const QString &fileName);
#endif
private:
    friend class CppGenerator;
    CppGenerator *J_QPTR;
#ifndef J_NO_QT
    QList<QString> d_variables;
#endif
};

} // end of namespace Icd

#endif // ICDGENERATOR_CPP_P_H
