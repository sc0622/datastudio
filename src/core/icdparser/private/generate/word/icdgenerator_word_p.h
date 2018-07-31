#ifndef ICDGENERATOR_WORD_P_H
#define ICDGENERATOR_WORD_P_H

#include "icdgenerator_word.h"
#include <QString>
#include <QVariant>
#include "icdcore/icdcore_inc.h"

class QAxObject;

namespace Icd {

// class WordGeneratorData

class WordGeneratorData
{
public:
    explicit WordGeneratorData(WordGenerator *q);
    ~WordGeneratorData();

    bool startup();
    void shutdown(const QString &filePath = QString::null, int saveAsType = 0);
    bool generateDocument(const QStandardItem *item, bool exportAll, bool rt);
    bool generateDocument(const Icd::TablePtr &table);

private:
    bool generateType();
    bool generateRoot(const QStandardItem *itemRoot, bool exportAll, bool rt, int level);
    bool generateVehicle(const QStandardItem *itemVehicle, bool exportAll, bool rt, int level);
    bool generateVehicle(const Icd::VehiclePtr &vehicle, bool exportAll, int level);
    bool generateSystem(const QStandardItem *itemSystem, bool exportAll, bool rt, int level);
    bool generateSystem(const std::string &vehicleId, const Icd::SystemPtr &system, bool exportAll, int level);
    bool generateTable(const QStandardItem *itemTable, bool exportAll, bool rt, int level);
    bool generateTable(const TablePtr &table, int level);
    bool generateDataItem(const QStandardItem *itemData, bool exportAll, bool rt, int level);
    bool generateDataItem(const Icd::ItemPtr &item, int level);

private:
    bool generateContents();
    bool generateHeading(const QString &text, int level);
    bool generateTableCaption(const QString &title, QAxObject *tableObject);
    bool generateItem(const Icd::ItemPtr &item, QAxObject *axTable, int row, int level);
    bool generateHeaderItem(const Icd::HeaderItemPtr &headerItem, QAxObject *axTable, int row);
    bool generateCounterItem(const Icd::CounterItemPtr &counterItem, QAxObject *axTable, int row);
    bool generateCheckItem(const Icd::CheckItemPtr &checkItem, QAxObject *axTable, int row);
    bool generateFrameCodeItem(const Icd::FrameCodeItemPtr &frameCodeItem, QAxObject *axTable, int row);
    bool generateNumericItem(const Icd::NumericItemPtr &numericItem, QAxObject *axTable, int row);
    bool generateBitItem(const Icd::BitItemPtr &bitItem, QAxObject *axTable, int row);
    bool generateComplexItem(const Icd::ComplexItemPtr &complexItem, QAxObject *axTable, int row, int level);
    bool generateFrameItem(const Icd::FrameItemPtr &frameItem, QAxObject *axTable, int row, int level);
    bool generateSubTable(const TablePtr &table, QAxObject *axTable, int row, int level);
    bool generateDateTimeItem(const Icd::DateTimeItemPtr &dateTimeItem, QAxObject *axTable, int row);
    bool generateArrayItem(const Icd::ArrayItemPtr &arrayItem, QAxObject *axTable, int row);

private:
    bool setCellText(QAxObject *axTable, int row, int column, const QVariant &text);

private:
    friend class WordGenerator;
    WordGenerator *q_ptr_;
    QAxObject *word_;
    QAxObject *document_;
    QAxObject *tableCaption_;
    QAxObject *listTemplate_;
    QAxObject *tables_;
    QAxObject *selection_;
};

} // end of namespace Icd

#endif // ICDGENERATOR_WORD_P_H
