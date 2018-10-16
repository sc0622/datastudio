#ifndef ICDGENERATOR_H
#define ICDGENERATOR_H

#include "../../icdparser_global.h"
#ifndef J_NO_QT
class QStandardItem;
#endif
namespace Json { class Value; }

namespace Icd {

class Generator;
class GeneratorData;
typedef std::shared_ptr<Generator> GeneratorPtr;

//
class DataObject;
typedef std::shared_ptr<DataObject> DataObjectPtr;

//
class Vehicle;
typedef std::shared_ptr<Vehicle> VehiclePtr;
typedef std::vector<VehiclePtr> VehiclePtrArray;

//
class System;
typedef std::shared_ptr<System> SystemPtr;
typedef std::vector<SystemPtr> SystemPtrArray;

//
class Table;
typedef std::shared_ptr<Table> TablePtr;
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
#ifndef J_NO_QT
    virtual bool generate(const QStandardItem *item, bool exportAll, bool rt,
                          const std::string &filePath);
#endif
    virtual bool generate(const TablePtr &table, const std::string &filePath) = 0;
    virtual Icd::Parser *parser();
    virtual const Icd::Parser *parser() const;

private:
    std::shared_ptr<GeneratorData> d;
};

} // end of namespace Icd

#endif // ICDGENERATOR_H
