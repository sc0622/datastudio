#ifndef ICDPARSER_XML_H
#define ICDPARSER_XML_H

#include "../../../icdparser.h"
#include "tinyxml/tinyxml.h"
#include "icdcore/icdcore_inc.h"
#include "../icdparser_file.h"
#include "../../generate/icdgenerate.h"

namespace Icd {

class XmlParser;
typedef std::shared_ptr<XmlParser> XmlParserPtr;

class XmlParser : public FileParser, public Generator
{
public:
    explicit XmlParser(const Json::Value &config);
    ~XmlParser() override;

    bool parse(RootPtr &root, int deep) const override;
    bool parse(Icd::VehiclePtrArray &vehicles, int deep, Icd::Object *parent) const override;
    bool parse(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep,
               Icd::Object *parent) const override;
    bool parse(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep,
               Icd::Object *parent) const override;
    bool parse(const std::string &vehicleId, const std::string &systemId, Icd::SystemPtr &system,
               int deep, Icd::Object *parent) const override;
    bool parse(const std::string &vehicleId, const std::string &systemId, Icd::TablePtrArray &tables,
               int deep, Icd::Object *parent) const override;
    bool parse(const std::string &vehicleId, const std::string &systemId, const std::string &tableId,
               Icd::TablePtr &table, int deep, Icd::Object *parent) const override;
    bool parse(const std::string &vehicleId, const std::string &systemId, const std::string &tableId,
               Icd::ItemPtrArray &items, int deep, Icd::Object *parent) const override;
    bool parse(Icd::TablePtrArray &tables) const override;
    bool parse(const std::string &tableId, Icd::TablePtr &table) const override;

    bool save(const RootPtr &root) const override;
    bool save(const Icd::VehiclePtrArray &vehicles) const override;
    bool save(const std::string &vehicleId, const Icd::VehiclePtr &vehicle) const override;
    bool save(const std::string &vehicleId, const Icd::SystemPtrArray &systems) const override;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const Icd::SystemPtr &system) const override;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const Icd::TablePtrArray &tables) const override;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const std::string &tableId, const Icd::TablePtr &table) const override;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const std::string &tableId, const Icd::ItemPtrArray &items) const override;
    bool save(const Icd::TablePtrArray &tables) const override;
    bool save(const std::string &tableId, const Icd::TablePtr &table) const override;
    bool save(const Icd::TablePtr &table) const override;

    bool beginModify() override;
    bool commitModify() override;
    bool cancelModify() override;
    bool endModify() override;

private:
    //
    TiXmlElement *readElementRoot() const;
    static TiXmlDocument *createDocument();
    bool saveDocument(TiXmlDocument *document) const;

    //
    TiXmlElement *findVehicleElement(const std::string &vehicleId) const;
    TiXmlElement *findSystemElement(const std::string &vehicleId, const std::string &systemId) const;
    TiXmlElement *findTableElement(const std::string &vehicleId, const std::string &systemId,
                                   const std::string &tableId) const;
    TiXmlElement *findTableElement(const std::string &tableId) const;

    // for parse
    Icd::ItemPtr createItem(int index, const TiXmlElement *element, int deep = -1,
                                   Object *parent = nullptr) const;

    bool parseObject(const TiXmlElement *emObject, const Icd::ObjectPtr &object) const;
    bool parseItem(const TiXmlElement *emItem, Icd::ItemPtr &item, int deep) const;
    bool parseItemHead(const TiXmlElement *emItem, const Icd::HeaderItemPtr &head) const;
    bool parseItemCounter(const TiXmlElement *emItem, const Icd::CounterItemPtr &counter) const;
    bool parseItemCheck(const TiXmlElement *emItem, const Icd::CheckItemPtr &check) const;
    bool parseItemFrameCode(const TiXmlElement *emItem, const Icd::FrameCodeItemPtr &frameCode) const;
    bool parseItemNumeric(const TiXmlElement *emItem, const Icd::NumericItemPtr &numeric) const;
    bool parseItemArray(const TiXmlElement *emItem, const Icd::ArrayItemPtr &array) const;
    bool parseItemBit(const TiXmlElement *emItem, const Icd::BitItemPtr &bit) const;
    bool parseItemComplex(const TiXmlElement *emItem, const Icd::ComplexItemPtr &complex, int deep) const;
    bool parseItemFrame(const TiXmlElement *emFrame, const Icd::FrameItemPtr &frame, int deep) const;
    bool parseTable(const TiXmlElement *emTable, const Icd::TablePtr &table, int deep) const;
    bool parseSystem(const TiXmlElement *emSystem, const Icd::SystemPtr &system, int deep) const;
    bool parseVehicle(const TiXmlElement *emVehicle, const Icd::VehiclePtr &vehicle, int deep) const;

    // for save
    bool saveObject(TiXmlElement *emObject, const Icd::ObjectPtr &object) const;
    bool saveItem(TiXmlElement *emItem, const Icd::ItemPtr &item) const;
    bool saveItemHead(TiXmlElement *emItem, const Icd::HeaderItemPtr &head) const;
    bool saveItemCounter(TiXmlElement *emItem, const Icd::CounterItemPtr &counter) const;
    bool saveItemCheck(TiXmlElement *emItem, const Icd::CheckItemPtr &check) const;
    bool saveItemFrameCode(TiXmlElement *emItem, const Icd::FrameCodeItemPtr &frameCode) const;
    bool saveItemNumeric(TiXmlElement *emItem, const Icd::NumericItemPtr &numeric) const;
    bool saveItemArray(TiXmlElement *emItem, const Icd::ArrayItemPtr &array) const;
    bool saveItemBit(TiXmlElement *emItem, const Icd::BitItemPtr &bit) const;
    bool saveItemComplex(TiXmlElement *emItem, const Icd::ComplexItemPtr &complex) const;
    bool saveItemFrame(TiXmlElement *emItem, const Icd::FrameItemPtr &frame) const;
    bool saveTable(TiXmlElement *emTable, const Icd::TablePtr &table) const;
    bool saveSystem(TiXmlElement *emSystem, const Icd::SystemPtr &system) const;
    bool saveVehicle(TiXmlElement *emVehicle, const Icd::VehiclePtr &vehicle) const;

    // Generator interface
public:
    bool startup() override;
    void shutdown() override;
#ifndef J_NO_QT
    bool generate(const QStandardItem *item, bool exportAll, bool rt, const std::string &filePath) override;
#endif
    bool generate(const TablePtr &table, const std::string &filePath) override;
    Parser *parser() override;
    const Parser *parser() const override;

private:
    friend class Parser;
};

}

#endif
