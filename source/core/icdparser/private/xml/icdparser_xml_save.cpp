#include "precomp.h"
#include "icdparser_xml.h"
#include "icdcore/icdcore_inc.h"

namespace Icd {

/**
 * @brief XmlParser::createDocument
 * @return
 */
TiXmlDocument *XmlParser::createDocument() const
{
    //
    TiXmlDocument *document = new TiXmlDocument();

    // create instruction node
    TiXmlDeclaration *emDeclaration = new TiXmlDeclaration("1.0", "utf-8", "");
    document->LinkEndChild(emDeclaration);

    // create root element
    TiXmlElement *emRoot = new TiXmlElement("icd");
    document->LinkEndChild(emRoot);

    return document;
}

/**
 * @brief XmlParser::saveDocument
 * @param document
 * @return
 */
bool XmlParser::saveDocument(TiXmlDocument *document) const
{
    if (!document) {
        return false;
    }

    return document->SaveFile(QString::fromStdString(d_filePath).toLocal8Bit());
}

/**
 * @brief XmlParser::saveObject
 * @param emObject
 * @param object
 * @return
 */
bool XmlParser::saveObject(TiXmlElement *emObject,
                           const Icd::ObjectPtr &object) const
{
    //
    if (!emObject|| object == 0) {
        return false;       //
    }

    // id
    emObject->SetAttribute("id", object->id());

    // name
    emObject->SetAttribute("name", object->name());

    // mark
    emObject->SetAttribute("mark", object->mark());

    // desc
    emObject->SetAttribute("desc", object->desc());

    return true;
}

/**
 * @brief XmlParser::saveItem
 * @param emItem
 * @param item
 * @return
 */
bool XmlParser::saveItem(TiXmlElement *emItem, const Icd::ItemPtr &item) const
{
    //
    if (!emItem || item == 0) {
        return false;       //
    }

    // save object attributes
    if (!saveObject(emItem, JHandlePtrCast<Icd::Object, Icd::Item>(item))) {
        return false;   // save failure
    }

    // type
    emItem->SetAttribute("type", item->Icd::Item::typeString());

    // defaultValue
    emItem->SetDoubleAttribute("defaultValue", item->defaultValue());

    //
    switch (item->type()) {
    case Icd::ItemHead:
        if (!saveItemHead(emItem, JHandlePtrCast<Icd::HeaderItem, Icd::Item>(item))) {
            return false;
        }
        break;
    case Icd::ItemCounter:
        if (!saveItemCounter(emItem, JHandlePtrCast<Icd::CounterItem, Icd::Item>(item))) {
            return false;
        }
        break;
    case Icd::ItemCheck:
        if (!saveItemCheck(emItem, JHandlePtrCast<Icd::CheckItem, Icd::Item>(item))) {
            return false;
        }
        break;
    case Icd::ItemFrameCode:
        if (!saveItemFrameCode(emItem, JHandlePtrCast<Icd::FrameCodeItem, Icd::Item>(item))) {
            return false;
        }
        break;
    case Icd::ItemNumeric:
        if (!saveItemNumeric(emItem, JHandlePtrCast<Icd::NumericItem, Icd::Item>(item))) {
            return false;
        }
        break;
    case Icd::ItemBitMap:
    case Icd::ItemBitValue:
        if (!saveItemBit(emItem, JHandlePtrCast<Icd::BitItem, Icd::Item>(item))) {
            return false;
        }
        break;
    case Icd::ItemComplex:
        if (!saveItemComplex(emItem, JHandlePtrCast<Icd::ComplexItem, Icd::Item>(item))) {
            return false;
        }
        break;
    case Icd::ItemFrame:
        if (!saveItemFrame(emItem, JHandlePtrCast<Icd::FrameItem, Icd::Item>(item))) {
            return false;
        }
        break;
    default:
        Q_ASSERT(false);
        return false;   // not supported data type
    }

    return true;
}

/**
 * @brief XmlParser::saveItemHead
 * @param emItem
 * @param head
 * @return
 */
bool XmlParser::saveItemHead(TiXmlElement *emItem,
                             const Icd::HeaderItemPtr &head) const
{
    //
    if (!emItem || head == 0) {
        return false;       //
    }

    // defaultValue
    emItem->SetAttribute("defaultValue", QString("0x%1")
                         .arg((uint)head->defaultValue(), 2, 16, QChar('0')).toStdString());

    return true;
}

bool XmlParser::saveItemCounter(TiXmlElement *emItem,
                                const Icd::CounterItemPtr &counter) const
{
    //
    if (!emItem || counter == 0) {
        return false;
    }

    // counterType
    emItem->SetAttribute("counterType", counter->counterTypeString());

    return true;
}

/**
 * @brief XmlParser::saveItemCheck
 * @param emItem
 * @param check
 * @return
 */
bool XmlParser::saveItemCheck(TiXmlElement *emItem, const Icd::CheckItemPtr &check) const
{
    //
    if (!emItem || check == 0) {
        return false;       //
    }

    // checkType
    emItem->SetAttribute("checkType", check->checkTypeString());
    // startPos
    emItem->SetAttribute("startPos", check->startPos());
    // endPos
    emItem->SetAttribute("endPos", check->endPos());

    return true;
}

/**
 * @brief XmlParser::saveItemFrameCode
 * @param emItem
 * @param frameCode
 * @return
 */
bool XmlParser::saveItemFrameCode(TiXmlElement *emItem,
                                  const Icd::FrameCodeItemPtr &frameCode) const
{
    //
    if (!emItem || frameCode == 0) {
        return false;       //
    }

    // frameCodeType
    emItem->SetAttribute("frameCodeType", frameCode->frameCodeTypeString());

    // frameId
    const std::string frameId = frameCode->frameId();
    if (!frameId.empty()) {
        emItem->SetAttribute("frameId", frameId);
    }

    return true;
}

/**
 * @brief XmlParser::saveItemNumeric
 * @param emItem
 * @param numeric
 * @return
 */
bool XmlParser::saveItemNumeric(TiXmlElement *emItem,
                                const Icd::NumericItemPtr &numeric) const
{
    //
    if (!emItem || numeric == 0) {
        return false;       //
    }

    // numericType
    emItem->SetAttribute("numericType", numeric->numericTypeString());

    // scale
    QString sValue = QString::number(numeric->scale(), 'g', 16);
    emItem->SetAttribute("scale", sValue.toStdString());

    // offset
    emItem->SetDoubleAttribute("offset", numeric->offset());

    // decimals
    emItem->SetDoubleAttribute("decimals", numeric->decimals());

    // min
    if (numeric->limit()->leftInf()) {
        emItem->SetAttribute("min", "");
    } else {
        emItem->SetDoubleAttribute("min", numeric->limit()->minimum());
    }

    // max
    if (numeric->limit()->rightInf()) {
        emItem->SetAttribute("max", "");
    } else {
        emItem->SetDoubleAttribute("max", numeric->limit()->maximum());
    }

    // unit
    emItem->SetAttribute("unit", numeric->unit());

    // spec
    const std::map<double, std::string> &specs = numeric->specs();
    for (std::map<double, std::string>::const_iterator citer = specs.cbegin();
         citer != specs.cend(); ++citer) {
        // create spec element
        TiXmlElement *emSpec = new TiXmlElement("spec");
        emItem->LinkEndChild(emSpec);
        // key
        emSpec->SetAttribute("key", citer->first);
        // info
        emSpec->SetAttribute("info", citer->second);
    }

    return true;
}

/**
 * @brief XmlParser::saveItemBit
 * @param emItem
 * @param bit
 * @return
 */
bool XmlParser::saveItemBit(TiXmlElement *emItem,
                            const Icd::BitItemPtr &bit) const
{
    //
    if (!emItem || bit == 0) {
        return false;       //
    }

    // start
    emItem->SetAttribute("start", bit->bitStart());

    // count
    emItem->SetAttribute("count", bit->bitCount());

    // typeSize
    emItem->SetAttribute("typeSize", bit->typeSize());

    // spec
    const std::map<icd_uint64, std::string> &specs = bit->specs();
    for (std::map<icd_uint64, std::string>::const_iterator citer = specs.cbegin();
         citer != specs.cend(); ++citer) {
        // create spec element
        TiXmlElement *emSpec = new TiXmlElement("spec");
        emItem->LinkEndChild(emSpec);
        // key
        emSpec->SetAttribute("key", "0x" + QString::number(citer->first, 16).toUpper().toStdString());
        // info
        emSpec->SetAttribute("info", citer->second);
    }

    return true;
}

/**
 * @brief XmlParser::saveItemComplex
 * @param emItem
 * @param complex
 * @return
 */
bool XmlParser::saveItemComplex(TiXmlElement *emItem,
                                const Icd::ComplexItemPtr &complex) const
{
    //
    if (!emItem || complex == 0) {
        return false;       //
    }

    // bufferSize
    emItem->SetDoubleAttribute("size", complex->bufferSize());

    // creat a table element from document
    TiXmlElement *emTable = new TiXmlElement("table");
    emItem->LinkEndChild(emTable);
    if (!saveTable(emTable, complex->table())) {
        return false;
    }

    return true;
}

/**
 * @brief XmlParser::saveItemFrame
 * @param emItem
 * @param frame
 * @return
 */
bool XmlParser::saveItemFrame(TiXmlElement *emItem,
                              const Icd::FrameItemPtr &frame) const
{
    //
    if (!emItem || frame == 0) {
        return false;       //
    }

    // save object attributes
    if (!saveObject(emItem, JHandlePtrCast<Icd::Object, Icd::FrameItem>(frame))) {
        return false;   // save failure
    }

    // bufferSize
    emItem->SetDoubleAttribute("size", frame->bufferSize());

    // sequenceCount
    emItem->SetAttribute("sequenceCount", frame->sequenceCount());

    //
    const Icd::TablePtrMap &allTable = frame->allTable();
    for (Icd::TablePtrMap::const_iterator citer = allTable.cbegin();
         citer != allTable.cend(); ++citer) {
        const Icd::TablePtr &table = citer->second;
        // creat a table element from document
        TiXmlElement *emTable = new TiXmlElement("table");
        emItem->LinkEndChild(emTable);
        if (!saveTable(emTable, table)) {
            continue;
        }
        //
    }

    return true;
}

/**
 * @brief XmlParser::saveTable
 * @param emTable
 * @param table
 * @return
 */
bool XmlParser::saveTable(TiXmlElement *emTable,
                          const Icd::TablePtr &table) const
{
    //
    if (!emTable || table == 0) {
        return false;       //
    }

    // save object attributes
    if (!saveObject(emTable, JHandlePtrCast<Icd::Object, Icd::Table>(table))) {
        return false;   // save failure
    }

    // sequence
    emTable->SetAttribute("sequence", table->sequence());

    // save table elements
    const Icd::ItemPtrArray &items = table->allItem();
    for (Icd::ItemPtrArray::const_iterator citer = items.cbegin();
         citer != items.cend(); ++citer) {
        // creat a table element from document
        TiXmlElement *emItem = new TiXmlElement("item");
        emTable->LinkEndChild(emItem);
        if (!saveItem(emItem, *citer)) {
            continue;
        }
    }

    return true;
}

/**
 * @brief XmlParser::saveSystem
 * @param emSystem
 * @param system
 * @return
 */
bool XmlParser::saveSystem(TiXmlElement *emSystem,
                           const Icd::SystemPtr &system) const
{
    //
    if (!emSystem || system == 0) {
        return false;       //
    }

    // save object attributes
    if (!saveObject(emSystem, JHandlePtrCast<Icd::Object, Icd::System>(system))) {
        return false;   // save failure
    }

    // save table elements
    const Icd::TablePtrArray &tables = system->allTable();
    for (Icd::TablePtrArray::const_iterator citer = tables.cbegin();
         citer != tables.cend(); ++citer) {
        // creat a table element from document
        TiXmlElement *emTable = new TiXmlElement("table");
        emSystem->LinkEndChild(emTable);
        if (!saveTable(emTable, *citer)) {
            continue;
        }
    }

    return true;
}

/**
 * @brief XmlParser::saveVehicle
 * @param emVehicle
 * @param vehicle
 * @return
 */
bool XmlParser::saveVehicle(TiXmlElement *emVehicle,
                            const Icd::VehiclePtr &vehicle) const
{
    //
    if (!emVehicle || vehicle == 0) {
        return false;   //
    }

    // save object attributes
    if (!saveObject(emVehicle, JHandlePtrCast<Icd::Object, Icd::Vehicle>(vehicle))) {
        return false;   // save failure
    }

    // save system elements
    const Icd::SystemPtrArray &systems = vehicle->allSystem();
    for (Icd::SystemPtrArray::const_iterator citer = systems.cbegin();
         citer != systems.cend(); ++citer) {
        // creat a system element from document
        TiXmlElement *emSystem = new TiXmlElement("system");
        emVehicle->LinkEndChild(emSystem);
        if (!saveSystem(emSystem, *citer)) {
            continue;
        }
    }

    return true;
}

} // end of namespace Icd
