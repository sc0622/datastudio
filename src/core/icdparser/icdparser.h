#ifndef ICDPARSER_H
#define ICDPARSER_H

#include "icdparser_global.h"
#include <string>

class QStandardItem;
namespace Json { class Value; }

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
     * @brief Parser
     * @param config
     */
    explicit Parser(const Json::Value &config);

    /**
     * @brief ~Parser
     */
    virtual ~Parser();

    /**
     * @brief create
     * @param config
     * @return
     */
    static ParserPtr create(const Json::Value &config);

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
    ParserData *d;
    friend class Generator;
};

// extends

Json::Value ICDPARSER_EXPORT findTable(const std::string &filePath, const std::string &vehicleId,
                                       const std::string &systemId, const std::string &tableId);

} // end of namespace Icd

#endif // ICDPARSER_H
