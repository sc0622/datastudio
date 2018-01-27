#ifndef ICDGENERATOR_H
#define ICDGENERATOR_H

#include "../../icdparser_global.h"

class QStandardItem;

namespace Icd {

class Generator;
class GeneratorData;
typedef JHandlePtr<Generator> GeneratorPtr;

//
class DataObject;
typedef JHandlePtr<DataObject> DataObjectPtr;

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

class Parser;

/**
 * @brief The Generator class
 */
class ICDPARSER_EXPORT Generator
{
public:
    explicit Generator(Icd::Parser *parser);
    virtual ~Generator();

    /**
     * @brief fileType
     * @param filePath
     * @return
     */
    static std::string fileType(const std::string &filePath);

    /**
     * @brief create
     * @param filePath
     * @param parser
     * @return
     */
    static GeneratorPtr create(const std::string &filePath, Icd::Parser *parser);

    /**
     * @brief startup
     * @return
     */
    virtual bool startup();

    /**
     * @brief shutdown
     */
    virtual void shutdown();

    /**
     * @brief generate
     * @param item
     * @param exportAll
     * @param rt
     * @param filePath
     * @return
     */
    virtual bool generate(const QStandardItem *item, bool exportAll, bool rt,
                          const std::string &filePath);

    /**
     * @brief generate
     * @param table
     * @param filePath
     * @return
     */
    virtual bool generate(const TablePtr &table, const std::string &filePath) = 0;

    /**
     * @brief parser
     * @return
     */
    Icd::Parser *parser() const;

private:
    JHandlePtr<GeneratorData> d;
};

} // end of namespace Icd

#endif // ICDGENERATOR_H
