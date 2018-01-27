#ifndef ICDGENERATOR_H
#define ICDGENERATOR_H

#include "../../icdparser_global.h"

class QStandardItem;
namespace Json { class Value; }

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

class ICDPARSER_EXPORT Generator
{
public:
    enum GeneratorType {
        GeneratorTypeInvalid = -1,
        GeneratorTypeXml,
        GeneratorTypeJson,
        GeneratorTypeCSharp,
        GeneratorTypeCpp,
        GeneratorTypeHtml,
        GeneratorTypeExcel,
        GeneratorTypeMatlab,
        GeneratorTypeWord,
        GeneratorTypePdf
    };

    explicit Generator(GeneratorType type, Icd::Parser *parser = nullptr);
    virtual ~Generator();

    GeneratorType generateType() const;

    static std::string fileType(const std::string &filePath);
    static GeneratorPtr create(const Json::Value &config, Icd::Parser *parser = nullptr);
    virtual bool startup();
    virtual void shutdown();
    virtual bool generate(const QStandardItem *item, bool exportAll, bool rt,
                          const std::string &filePath);
    virtual bool generate(const TablePtr &table, const std::string &filePath) = 0;
    virtual Icd::Parser *parser();
    virtual const Icd::Parser *parser() const;

private:
    JHandlePtr<GeneratorData> d;
};

} // end of namespace Icd

#endif // ICDGENERATOR_H
