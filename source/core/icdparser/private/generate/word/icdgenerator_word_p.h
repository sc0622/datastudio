#ifndef ICDGENERATOR_WORD_P_H
#define ICDGENERATOR_WORD_P_H

#include "icdgenerator_word.h"
#include <QString>
#include "icdcore/icdcore_inc.h"

class QAxObject;

namespace Icd {

// class WordGeneratorData

class WordGeneratorData
{
public:
    explicit WordGeneratorData(WordGenerator *q);
    ~WordGeneratorData();

    /**
     * @brief startup
     * @return
     */
    bool startup();

    /**
     * @brief shutdown
     * @param filePath
     * @param saveAsType
     */
    void shutdown(const QString &filePath = QString::null, int saveAsType = 0);

    /**
     * @brief generateDocument
     * @param item
     * @param exportAll
     * @param rt
     * @return
     */
    bool generateDocument(const QStandardItem *item, bool exportAll, bool rt);

    /**
     * @brief generateDocument
     * @param table
     * @return
     */
    bool generateDocument(const Icd::TablePtr &table);

private:
    /**
     * @brief generateType
     * @return
     */
    bool generateType();

    /**
     * @brief generateRoot
     * @param itemRoot
     * @param exportAll
     * @param rt
     * @param level
     * @return
     */
    bool generateRoot(const QStandardItem *itemRoot, bool exportAll, bool rt, int level);

    /**
     * @brief generateVehicle
     * @param itemVehicle
     * @param exportAll
     * @param rt
     * @param level
     * @return
     */
    bool generateVehicle(const QStandardItem *itemVehicle, bool exportAll, bool rt, int level);

    /**
     * @brief generateVehicle
     * @param vehicle
     * @param exportAll
     * @param level
     * @return
     */
    bool generateVehicle(const Icd::VehiclePtr &vehicle, bool exportAll, int level);

    /**
     * @brief generateSystem
     * @param itemSystem
     * @param exportAll
     * @param rt
     * @param level
     * @return
     */
    bool generateSystem(const QStandardItem *itemSystem, bool exportAll, bool rt, int level);

    /**
     * @brief generateSystem
     * @param vehicleId
     * @param system
     * @param exportAll
     * @param level
     * @return
     */
    bool generateSystem(const std::string &vehicleId, const Icd::SystemPtr &system, bool exportAll,
                        int level);

    /**
     * @brief generateTable
     * @param itemTable
     * @param exportAll
     * @param rt
     * @param level
     * @return
     */
    bool generateTable(const QStandardItem *itemTable, bool exportAll, bool rt, int level);

    /**
     * @brief generateTable
     * @param table
     * @param level
     * @return
     */
    bool generateTable(const TablePtr &table, int level);

private:
    /**
     * @brief generateContents
     * @param text
     * @param level
     * @return
     */
    bool generateContents();

    /**
     * @brief generateHeading
     * @param text
     * @param level
     * @return
     */
    bool generateHeading(const QString &text, int level);

    /**
     * @brief generateTableCaption
     * @param title
     * @param tableObject
     * @return
     */
    bool generateTableCaption(const QString &title, QAxObject *tableObject);

    /**
     * @brief generateItem
     * @param item
     * @param axTable
     * @param row
     * @param level
     * @return
     */
    bool generateItem(const Icd::ItemPtr &item, QAxObject *axTable, int row, int level);

    /**
     * @brief generateHeaderItem
     * @param headerItem
     * @param axTable
     * @param row
     * @return
     */
    bool generateHeaderItem(const Icd::HeaderItemPtr &headerItem, QAxObject *axTable, int row);

    /**
     * @brief generateCounterItem
     * @param counterItem
     * @param axTable
     * @param row
     * @return
     */
    bool generateCounterItem(const Icd::CounterItemPtr &counterItem, QAxObject *axTable, int row);

    /**
     * @brief generateCheckItem
     * @param checkItem
     * @param axTable
     * @param row
     * @return
     */
    bool generateCheckItem(const Icd::CheckItemPtr &checkItem, QAxObject *axTable, int row);

    /**
     * @brief generateFrameCodeItem
     * @param frameCodeItem
     * @param axTable
     * @param row
     * @return
     */
    bool generateFrameCodeItem(const Icd::FrameCodeItemPtr &frameCodeItem, QAxObject *axTable, int row);

    /**
     * @brief generateNumericItem
     * @param numericItem
     * @param axTable
     * @param row
     * @return
     */
    bool generateNumericItem(const Icd::NumericItemPtr &numericItem, QAxObject *axTable, int row);

    /**
     * @brief generateBitItem
     * @param bitItem
     * @param axTable
     * @param row
     * @return
     */
    bool generateBitItem(const Icd::BitItemPtr &bitItem, QAxObject *axTable, int row);

    /**
     * @brief generateComplexItem
     * @param complexItem
     * @param axTable
     * @param row
     * @param level
     * @return
     */
    bool generateComplexItem(const Icd::ComplexItemPtr &complexItem, QAxObject *axTable, int row, int level);

    /**
     * @brief generateFrameItem
     * @param frameItem
     * @param axTable
     * @param row
     * @param level
     * @return
     */
    bool generateFrameItem(const Icd::FrameItemPtr &frameItem, QAxObject *axTable, int row, int level);

private:
    /**
     * @brief setCellText
     * @param axTable
     * @param row
     * @param column
     * @param text
     * @return
     */
    bool setCellText(QAxObject *axTable, int row, int column, const QVariant &text);

private:
    friend class WordGenerator;
    WordGenerator *q_ptr;
    QAxObject *word;
    QAxObject *document;
    QAxObject *tableCaption;
    QAxObject *listTemplate;
    QAxObject *tables;
    QAxObject *selection;
};

} // end of namespace Icd

#endif // ICDGENERATOR_WORD_P_H


/**
 *
    QString s = captionLabels->generateDocumentation();
    QFile file(QApplication::applicationDirPath().append("/CaptionLabels.htm"));
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }
    QTextStream ts(&file);
    ts << s;
    file.close();


    */
