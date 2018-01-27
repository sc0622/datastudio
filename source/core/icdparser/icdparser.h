#ifndef ICDPARSER_H
#define ICDPARSER_H

#include "icdparser_global.h"
#include <string>

//
class QStandardItem;

namespace Icd {

//
class Vehicle;
typedef JHandlePtr<Vehicle> VehiclePtr;
typedef std::vector<VehiclePtr> VehiclePtrArray;

//
class System;
typedef JHandlePtr<System> SystemPtr;
typedef std::vector<SystemPtr> SystemPtrArray;

//
class Table;
typedef JHandlePtr<Table> TablePtr;
typedef std::vector<TablePtr> TablePtrArray;

//
class Item;
typedef JHandlePtr<Item> ItemPtr;
typedef std::vector<ItemPtr> ItemPtrArray;

//
class Parser;
class ParserData;
typedef JHandlePtr<Parser> ParserPtr;

/**
 * @brief The Parser class
 */
class ICDPARSER_EXPORT Parser
{
public:
    /**
     * @brief The SourceType enum
     */
    enum SourceType {
        SourceTypeInvalid = -1,
        SourceTypeFile,
        SourceTypeSql,

        SourceTypeCount
    };

    /**
     * @brief Parser
     * @param sourceType
     */
    explicit Parser(SourceType sourceType = SourceTypeInvalid);

    virtual ~Parser();

    /**
     * @brief isValid
     * @return
     */
    bool isValid() const;

    /**
     * @brief sourceType
     * @return
     */
    SourceType sourceType() const;

    /**
     * @brief parseVehicle
     * @param vehicles
     * @param deep
     * @return
     */
    virtual bool parseVehicle(Icd::VehiclePtrArray &vehicles, int deep) = 0;

    /**
     * @brief parseVehicle
     * @param vehicleId
     * @param vehicle
     * @param deep
     * @return
     */
    virtual bool parseVehicle(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep) = 0;

    /**
     * @brief parseSystem
     * @param vehicleId
     * @param systems
     * @param deep
     * @return
     */
    virtual bool parseSystem(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep) = 0;

    /**
     * @brief parseVehicle
     * @param vehicle
     * @param deep
     * @return
     */
    virtual bool parseSystem(const std::string &vehicleId, const std::string &systemId,
                             Icd::SystemPtr &system, int deep) = 0;

    /**
     * @brief parseTable
     * @param vehicleId
     * @param systemId
     * @param tables
     * @param deep
     * @return
     */
    virtual bool parseTable(const std::string &vehicleId, const std::string &systemId,
                            Icd::TablePtrArray &tables, int deep) = 0;

    /**
     * @brief parseTable
     * @param vehicleId
     * @param systemId
     * @param tableId
     * @param table
     * @param deep
     * @return
     */
    virtual bool parseTable(const std::string &vehicleId, const std::string &systemId,
                            const std::string &tableId, Icd::TablePtr &table, int deep) = 0;

    /**
     * @brief parseTable
     * @param tables
     * @return
     */
    virtual bool parseTable(Icd::TablePtrArray &tables) = 0;

    /**
     * @brief parseTable
     * @param tableId
     * @param table
     * @return
     */
    virtual bool parseTable(const std::string &tableId, Icd::TablePtr &table) = 0;

    /**
     * @brief parseItem
     * @param vehicleId
     * @param systemId
     * @param tableId
     * @param items
     * @param deep
     * @return
     */
    virtual bool parseItem(const std::string &vehicleId, const std::string &systemId,
                           const std::string &tableId, Icd::ItemPtrArray &items, int deep) = 0;

    /**
     * @brief parseItem
     * @param vehicleId
     * @param systemId
     * @param tableId
     * @param tableId
     * @param itemId
     * @param deep
     * @return
     */
    virtual bool parseItem(const std::string &vehicleId, const std::string &systemId,
                           const std::string &tableId, const std::string &itemId,
                           Icd::ItemPtr &item, int deep) = 0;

    /**
     * @brief save
     * @param vehicles
     * @return
     */
    virtual bool save(const Icd::VehiclePtrArray &vehicles) = 0;

    /**
     * @brief save
     * @param tables
     * @return
     */
    virtual bool save(const Icd::TablePtrArray &tables) = 0;

    /**
     * @brief save
     * @param table
     * @return
     */
    virtual bool save(const Icd::TablePtr &table) = 0;

    /**
     * @brief saveAs
     * @param item
     * @param exportAll
     * @param rt
     * @param filePath
     * @return
     */
    bool saveAs(const QStandardItem *item, bool exportAll, bool rt, const std::string &filePath);

    /**
     * @brief saveAs
     * @param table
     * @param filePath
     * @return
     */
    bool saveAs(const Icd::TablePtr &table, const std::string &filePath);

    /**
     * @brief convert
     * @param vehicles
     * @param data
     * @return
     */
    virtual bool convert2Other(const Icd::VehiclePtrArray &vehicles, std::vector<int> &data) = 0;

    /**
     * @brief convert2Core
     * @param data
     * @param vehicles
     * @return
     */
    virtual bool convert2Core(const std::vector<int> &data, VehiclePtrArray &vehicles) = 0;

    /**
     * @brief message
     * @return
     */
    std::string message() const;

    /**
     * @brief setMessage
     * @param message
     */
    void setMessage(const std::string &message);

    /**
     * @brief progressValue
     * @return
     */
    qreal progressValue() const;

    /**
     * @brief setProgressValue
     * @param value
     */
    void setProgressValue(qreal value);

    /**
     * @brief canceledSaveAs
     * @return
     */
    bool canceledSaveAs() const;

    /**
     * @brief cancelSaveAs
     * @param cancel
     */
    void cancelSaveAs(bool cancel = true);

private:
    JHandlePtr<ParserData> d;
    friend class Generator;
};

} // end of namespace Icd

#endif // ICDPARSER_H
