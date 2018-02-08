#ifndef ICDPARSER_XML_H
#define ICDPARSER_XML_H

#include "../../../icdparser.h"
#include "tinyxml.h"
#include "icdcore/icdcore_inc.h"
#include "../icdparser_file.h"
#include "../../generate/icdgenerate.h"

namespace Icd {

class XmlParser;
typedef JHandlePtr<XmlParser> XmlParserPtr;

class XmlParser : public FileParser, public Generator
{
public:
    explicit XmlParser(const Json::Value &config);
    ~XmlParser();

    bool parse(Icd::VehiclePtrArray &vehicles, int deep) const;
    bool parse(const std::string &vehicleId, Icd::VehiclePtr &vehicle, int deep) const;
    bool parse(const std::string &vehicleId, Icd::SystemPtrArray &systems, int deep) const;
    bool parse(const std::string &vehicleId, const std::string &systemId,
                     Icd::SystemPtr &system, int deep) const;
    bool parse(const std::string &vehicleId, const std::string &systemId,
                    Icd::TablePtrArray &tables, int deep) const;
    bool parse(const std::string &vehicleId, const std::string &systemId,
                    const std::string &tableId, Icd::TablePtr &table, int deep) const;
    bool parse(const std::string &vehicleId, const std::string &systemId,
                   const std::string &tableId, Icd::ItemPtrArray &items, int deep) const;
    bool parse(const std::string &vehicleId, const std::string &systemId,
                   const std::string &tableId, const std::string &itemId,
                   Icd::ItemPtr &item, int deep) const;
    bool parse(Icd::TablePtrArray &tables) const;
    bool parse(const std::string &tableId, Icd::TablePtr &table) const;

    bool save(const Icd::VehiclePtrArray &vehicles) const;
    bool save(const std::string &vehicleId, const Icd::VehiclePtr &vehicle) const;
    bool save(const std::string &vehicleId, const Icd::SystemPtrArray &systems) const;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const Icd::SystemPtr &system) const;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const Icd::TablePtrArray &tables) const;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const std::string &tableId, const Icd::TablePtr &table) const;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const std::string &tableId, const Icd::ItemPtrArray &items) const;
    bool save(const std::string &vehicleId, const std::string &systemId,
              const std::string &tableId, const std::string &itemId,
              const Icd::ItemPtr &item) const;
    bool save(const Icd::TablePtrArray &tables) const;
    bool save(const std::string &tableId, const Icd::TablePtr &table) const;

    bool save(const Icd::TablePtr &table) const;

private:
    //
    TiXmlElement *readElementRoot() const;
    TiXmlDocument *createDocument() const;
    bool saveDocument(TiXmlDocument *document) const;

    //
    TiXmlElement *findVehicleElement(const std::string &vehicleId) const;
    TiXmlElement *findSystemElement(const std::string &vehicleId, const std::string &systemId) const;
    TiXmlElement *findTableElement(const std::string &vehicleId, const std::string &systemId,
                                   const std::string &tableId) const;
    TiXmlElement *findTableElement(const std::string &tableId) const;

    // for parse
    bool parseObject(const TiXmlElement *emObject, const Icd::ObjectPtr &object) const;
    bool parseItem(const TiXmlElement *emItem, Icd::ItemPtr &item, int deep) const;
    bool parseItemHead(const TiXmlElement *emItem, const Icd::HeaderItemPtr &head) const;
    bool parseItemCounter(const TiXmlElement *emItem, const Icd::CounterItemPtr &counter) const;
    bool parseItemCheck(const TiXmlElement *emItem, const Icd::CheckItemPtr &check) const;
    bool parseItemFrameCode(const TiXmlElement *emItem, const Icd::FrameCodeItemPtr &frameCode) const;
    bool parseItemNumeric(const TiXmlElement *emItem, const Icd::NumericItemPtr &numeric) const;
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
    bool saveItemBit(TiXmlElement *emItem, const Icd::BitItemPtr &bit) const;
    bool saveItemComplex(TiXmlElement *emItem, const Icd::ComplexItemPtr &complex) const;
    bool saveItemFrame(TiXmlElement *emItem, const Icd::FrameItemPtr &frame) const;
    bool saveTable(TiXmlElement *emTable, const Icd::TablePtr &table) const;
    bool saveSystem(TiXmlElement *emSystem, const Icd::SystemPtr &system) const;
    bool saveVehicle(TiXmlElement *emVehicle, const Icd::VehiclePtr &vehicle) const;

    // Generator interface
public:
    bool startup();
    void shutdown();
    bool generate(const QStandardItem *item, bool exportAll, bool rt, const std::string &filePath);
    bool generate(const TablePtr &table, const std::string &filePath);
    Parser *parser();
    const Parser *parser() const;
};

}

#endif
