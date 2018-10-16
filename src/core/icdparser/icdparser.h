#ifndef ICDPARSER_H
#define ICDPARSER_H

#include "icdparser_global.h"
#include <string>
#include "icdcore/icd_object.h"
#ifndef J_NO_QT
class QStandardItem;
#endif
namespace Json { class Value; }

namespace Icd {

//
class Root;
typedef std::shared_ptr<Root> RootPtr;

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

//
class Item;
typedef std::shared_ptr<Item> ItemPtr;
typedef std::vector<ItemPtr> ItemPtrArray;

//
class Parser;
class ParserData;
typedef std::shared_ptr<Parser> ParserPtr;

class ICDPARSER_EXPORT Parser
{
public:
    explicit Parser(const Json::Value &config);
    virtual ~Parser();

    static ParserPtr create(const Json::Value &config);

    virtual bool parse(RootPtr &root, int deep) const = 0;
    virtual bool parse(Icd::VehiclePtrArray &vehicles, int deep, Icd::Object *parent = nullptr) const = 0;
    virtual bool parse(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep,
                       Icd::Object *parent = nullptr) const = 0;
    virtual bool parse(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep,
                       Icd::Object *parent = nullptr) const = 0;
    virtual bool parse(const std::string &vehicleId, const std::string &systemId, Icd::SystemPtr &system,
                       int deep, Icd::Object *parent = nullptr) const = 0;
    virtual bool parse(const std::string &vehicleId, const std::string &systemId, Icd::TablePtrArray &tables,
                       int deep, Icd::Object *parent = nullptr) const = 0;
    virtual bool parse(const std::string &vehicleId, const std::string &systemId, const std::string &tableId,
                       Icd::TablePtr &table, int deep, Icd::Object *parent = nullptr) const = 0;
    virtual bool parse(const std::string &vehicleId, const std::string &systemId, const std::string &tableId,
                       Icd::ItemPtrArray &items, int deep, Icd::Object *parent = nullptr) const = 0;
    virtual bool parse(Icd::TablePtrArray &tables) const = 0;
    virtual bool parse(const std::string &tableId, Icd::TablePtr &table) const = 0;
    Icd::ObjectPtr parse(const std::string &domain, int objectType, int deep = 1, Object *parent = nullptr) const;

    virtual bool save(const RootPtr &root) const = 0;
    virtual bool save(const Icd::VehiclePtrArray &vehicles) const = 0;
    virtual bool save(const std::string &vehicleId, const Icd::VehiclePtr &vehicle) const = 0;
    virtual bool save(const std::string &vehicleId, const Icd::SystemPtrArray &systems) const = 0;
    virtual bool save(const std::string &vehicleId, const std::string &systemId,
                      const Icd::SystemPtr &system) const = 0;
    virtual bool save(const std::string &vehicleId, const std::string &systemId,
                      const Icd::TablePtrArray &tables) const = 0;
    virtual bool save(const std::string &vehicleId, const std::string &systemId,
                      const std::string &tableId, const Icd::TablePtr &table) const = 0;
    virtual bool save(const std::string &vehicleId, const std::string &systemId,
                      const std::string &tableId, const Icd::ItemPtrArray &items) const = 0;
    virtual bool save(const Icd::TablePtrArray &tables) const = 0;
    virtual bool save(const std::string &tableId, const Icd::TablePtr &table) const = 0;
    virtual bool save(const Icd::TablePtr &table) const = 0;
    bool save(const std::string &domain, const Icd::ObjectPtr &object,
              bool merge = false, bool fast = false) const;
    // for json
    virtual bool saveJsonObject(const ::std::string &path, const Json::Value &value,
                                bool fast = false) const;
    virtual bool saveJsonArray(const ::std::string &path, const Json::Value &value,
                               bool unique = true, bool fast = false) const;
    virtual bool replaceJson(const ::std::string &path, const Json::Value &oldValue,
                             const Json::Value &newValue, bool fast = false) const;
    virtual bool removeJson(const ::std::string &path, const std::string &key,
                            const Json::Value &value, bool fast = false) const;
    virtual bool mergeJson(const ::std::string &path, const Json::Value &value, bool fast = false) const;

    virtual bool beginModify();
    virtual bool commitModify();
    virtual bool cancelModify();
    virtual bool endModify();
    bool isBeginModify() const;
#ifndef J_NO_QT
    bool saveAs(const QStandardItem *item, bool exportAll, bool rt, const std::string &filePath);
#endif
    bool saveAs(const Icd::TablePtr &table, const std::string &filePath);

    std::string message() const;
    void setMessage(const std::string &message);

    double progressValue() const;
    void setProgressValue(double value);

    bool canceledSaveAs() const;
    void cancelSaveAs(bool cancel = true);

    static bool createXmlFile(const std::string &filePath);

private:
    Parser(const Parser&);
    Parser &operator=(const Parser&);
    ParserData *d;
    friend class Generator;
};

// extends

Json::Value ICDPARSER_EXPORT queryTable(const std::string &filePath, const std::string &vehicleId,
                                        const std::string &systemId, const std::string &tableId);
Json::Value ICDPARSER_EXPORT queryTable(const std::string &filePath, const std::string &domain,
                                        int domainType = Icd::DomainId);

} // end of namespace Icd

#endif // ICDPARSER_H
