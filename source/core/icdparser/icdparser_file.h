#ifndef ICDPARSER_FILE_H
#define ICDPARSER_FILE_H

#include "icdparser_global.h"
#include "icdparser.h"

namespace Json { class Value; }

namespace Icd {

class FileParser;
class FileParserData;
typedef JHandlePtr<FileParser> FileParserPtr;

/**
 * @brief The FileParser class
 */
class ICDPARSER_EXPORT FileParser : public Parser
{
public:
    /**
     * @brief FileParser
     * @param filePath
     * @param parser
     */
    explicit FileParser(const std::string &filePath, Icd::Parser *parser = nullptr);

    ~FileParser();

    /**
     * @brief parseVehicle
     * @param vehicles
     * @param deep
     * @return
     */
    bool parseVehicle(Icd::VehiclePtrArray &vehicles, int deep);

    /**
     * @brief parseVehicle
     * @param vehicleId
     * @param vehicle
     * @param deep
     * @return
     */
    bool parseVehicle(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep);

    /**
     * @brief parseSystem
     * @param vehicleId
     * @param systems
     * @param deep
     * @return
     */
    bool parseSystem(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep);

    /**
     * @brief parseVehicle
     * @param vehicle
     * @param deep
     * @return
     */
    bool parseSystem(const std::string &vehicleId, const std::string &systemId,
                     Icd::SystemPtr &system, int deep);

    /**
     * @brief parseTable
     * @param vehicleId
     * @param systemId
     * @param tables
     * @param deep
     * @return
     */
    bool parseTable(const std::string &vehicleId, const std::string &systemId,
                    Icd::TablePtrArray &tables, int deep);

    /**
     * @brief parseTable
     * @param vehicleId
     * @param systemId
     * @param tableId
     * @param table
     * @param deep
     * @return
     */
    bool parseTable(const std::string &vehicleId, const std::string &systemId,
                    const std::string &tableId, Icd::TablePtr &table, int deep);

    /**
     * @brief parseTable
     * @param tables
     * @return
     */
    bool parseTable(Icd::TablePtrArray &tables);

    /**
     * @brief parseTable
     * @param tableId
     * @param table
     * @return
     */
    bool parseTable(const std::string &tableId, Icd::TablePtr &table);

    /**
     * @brief parseItem
     * @param vehicleId
     * @param systemId
     * @param tableId
     * @param items
     * @param deep
     * @return
     */
    bool parseItem(const std::string &vehicleId, const std::string &systemId,
                   const std::string &tableId, Icd::ItemPtrArray &items, int deep);

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
    bool parseItem(const std::string &vehicleId, const std::string &systemId,
                   const std::string &tableId, const std::string &itemId,
                   Icd::ItemPtr &item, int deep);

    /**
     * @brief save
     * @param vehicles
     * @return
     */
    bool save(const Icd::VehiclePtrArray &vehicles);

    /**
     * @brief save
     * @param tables
     * @return
     */
    bool save(const Icd::TablePtrArray &tables);

    /**
     * @brief save
     * @param table
     * @return
     */
    bool save(const Icd::TablePtr &table);

    /**
     * @brief convert
     * @param vehicles
     * @param data
     * @return
     */
    bool convert2Other(const Icd::VehiclePtrArray &vehicles, std::vector<int> &data);

    /**
     * @brief convert2Core
     * @param data
     * @param vehicles
     * @return
     */
    bool convert2Core(const std::vector<int> &data, Icd::VehiclePtrArray &vehicles);

private:
    JHandlePtr<FileParserData> d;
};

// extends

/**
 * @brief findTable
 * @param filePath
 * @param vehicleId
 * @param systemId
 * @param tableId
 * @return
 */
Json::Value ICDPARSER_EXPORT findTable(const std::string &filePath, const std::string &vehicleId,
                                       const std::string &systemId, const std::string &tableId);

} // end of namespace Icd

#endif // ICDPARSER_FILE_H
