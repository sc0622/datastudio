#include "precomp.h"
#include "icdparser_xml.h"
#include "icdcore/icdcore_inc.h"

namespace Icd {

TiXmlDocument *XmlParser::createDocument()
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

bool XmlParser::saveDocument(TiXmlDocument *document) const
{
    if (!document) {
        return false;
    }
#ifndef J_NO_QT
    const std::string filePath = QString::fromStdString(this->filePath()).toLocal8Bit();
#else
    const std::string filePath = this->filePath();
#endif
    return document->SaveFile(filePath);
}

bool XmlParser::saveObject(TiXmlElement *emObject, const Icd::ObjectPtr &object) const
{
    //
    if (!emObject|| !object) {
        return false;
    }

    // id
    if (object->objectType() != Icd::ObjectItem) {
        const std::string id = object->id();
        if (!id.empty()) {
            emObject->SetAttribute("id", id);
        }
    }
    //
    Icd::Object *parent = object->parent();
    if (!parent || parent->rtti() != Icd::ObjectComplex) {
        // name
        emObject->SetAttribute("name", object->name());
        // mark
        const std::string mark = object->mark();
        if (!mark.empty()) {
            emObject->SetAttribute("mark", mark);
        }
        // desc
        const std::string desc = object->desc();
        if (!desc.empty()) {
            emObject->SetAttribute("desc", desc);
        }
    }

    return true;
}

bool XmlParser::saveItem(TiXmlElement *emItem, const Icd::ItemPtr &item) const
{
    if (!emItem || !item) {
        return false;
    }

    // save object attributes
    if (!saveObject(emItem, item)) {
        return false;
    }
    // type
    emItem->SetAttribute("type", item->Icd::Item::typeString());
    // defaultValue
    emItem->SetDoubleAttribute("defaultValue", item->defaultValue());
    //
    switch (item->type()) {
    case Icd::ItemHeader:
        if (!saveItemHeader(emItem, JHandlePtrCast<Icd::HeaderItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemCounter:
        if (!saveItemCounter(emItem, JHandlePtrCast<Icd::CounterItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemCheck:
        if (!saveItemCheck(emItem, JHandlePtrCast<Icd::CheckItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemFrameCode:
        if (!saveItemFrameCode(emItem, JHandlePtrCast<Icd::FrameCodeItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemNumeric:
        if (!saveItemNumeric(emItem, JHandlePtrCast<Icd::NumericItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemArray:
        if (!saveItemArray(emItem, JHandlePtrCast<Icd::ArrayItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemBitMap:
    case Icd::ItemBitValue:
        if (!saveItemBit(emItem, JHandlePtrCast<Icd::BitItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemComplex:
        if (!saveItemComplex(emItem, JHandlePtrCast<Icd::ComplexItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemFrame:
        if (!saveItemFrame(emItem, JHandlePtrCast<Icd::FrameItem>(item))) {
            return false;
        }
        break;
    default:
        return false;   // not supported data type
    }

    return true;
}

bool XmlParser::saveItemHeader(TiXmlElement *emItem, const Icd::HeaderItemPtr &header) const
{
    if (!emItem || !header) {
        return false;
    }

    // defaultValue
    emItem->SetAttribute("defaultValue",
                         Icd::utoa(static_cast<unsigned int>(header->defaultValue()), true, 2));

    return true;
}

bool XmlParser::saveItemCounter(TiXmlElement *emItem, const Icd::CounterItemPtr &counter) const
{
    if (!emItem || !counter) {
        return false;
    }

    // counterType
    emItem->SetAttribute("counterType", counter->counterTypeString());

    return true;
}

bool XmlParser::saveItemCheck(TiXmlElement *emItem, const Icd::CheckItemPtr &check) const
{
    if (!emItem || !check) {
        return false;
    }

    // checkType
    emItem->SetAttribute("checkType", check->checkTypeString());
    // startPos
    emItem->SetAttribute("startPos", check->startPos());
    // endPos
    emItem->SetAttribute("endPos", check->endPos());

    return true;
}

bool XmlParser::saveItemFrameCode(TiXmlElement *emItem, const Icd::FrameCodeItemPtr &frameCode) const
{
    if (!emItem || !frameCode) {
        return false;
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

bool XmlParser::saveItemNumeric(TiXmlElement *emItem, const Icd::NumericItemPtr &numeric) const
{
    if (!emItem || !numeric) {
        return false;
    }

    // numericType
    emItem->SetAttribute("numericType", numeric->numericTypeString());
    // scale
    emItem->SetAttribute("scale", Icd::dtoa(numeric->scale()));
    // offset
    emItem->SetDoubleAttribute("offset", numeric->offset());
    // decimals
    if (numeric->decimals() > 0) {
        emItem->SetDoubleAttribute("decimals", numeric->decimals());
    }
    // min
    if (!numeric->limit()->minimumInf()) {
        emItem->SetDoubleAttribute("min", numeric->limit()->minimum());
    }
    // max
    if (!numeric->limit()->maximumInf()) {
        emItem->SetDoubleAttribute("max", numeric->limit()->maximum());
    }
    // unit
    const std::string unit = numeric->unit();
    if (!unit.empty()) {
        emItem->SetAttribute("unit", numeric->unit());
    }
    // spec
    const std::map<double, std::string> &specs = numeric->specs();
    for (std::map<double, std::string>::const_iterator citer = specs.cbegin();
         citer != specs.cend(); ++citer) {
        // create spec element
        TiXmlElement *emSpec = new TiXmlElement("spec");
        emItem->LinkEndChild(emSpec);
        // key
        emSpec->SetDoubleAttribute("key", citer->first);
        // info
        emSpec->SetAttribute("info", citer->second);
    }

    return true;
}

bool XmlParser::saveItemArray(TiXmlElement *emItem, const ArrayItemPtr &array) const
{
    if (!emItem || !array) {
        return false;
    }

    // arrayType
    emItem->SetAttribute("arrayType", array->arrayTypeString());
    // count
    emItem->SetAttribute("count", array->count());

    return true;
}

bool XmlParser::saveItemBit(TiXmlElement *emItem, const Icd::BitItemPtr &bit) const
{
    if (!emItem || !bit) {
        return false;
    }

    // start
    emItem->SetAttribute("start", bit->bitStart());
    // count
    emItem->SetAttribute("count", bit->bitCount());
    // for bitvalue
    if (bit->type() == Icd::ItemBitValue) {
        // scale
        emItem->SetAttribute("scale", Icd::dtoa(bit->scale()));
        // offset
        emItem->SetDoubleAttribute("offset", bit->offset());
        // decimals
        emItem->SetDoubleAttribute("decimals", bit->decimals());
        // min
        if (!bit->limit()->minimumInf()) {
            emItem->SetDoubleAttribute("min", bit->limit()->minimum());
        }
        // max
        if (bit->limit()->maximumInf()) {
            emItem->SetDoubleAttribute("max", bit->limit()->maximum());
        }
        // unit
        emItem->SetAttribute("unit", bit->unit());
    }
    // spec
    const std::map<icd_uint64, std::string> &specs = bit->specs();
    for (std::map<icd_uint64, std::string>::const_iterator citer = specs.cbegin();
         citer != specs.cend(); ++citer) {
        // create spec element
        TiXmlElement *emSpec = new TiXmlElement("spec");
        emItem->LinkEndChild(emSpec);
        // key
        emSpec->SetAttribute("key", Icd::touppered(Icd::u64toa(citer->first, true)));
        // info
        emSpec->SetAttribute("info", citer->second);
    }

    return true;
}

bool XmlParser::saveItemComplex(TiXmlElement *emItem, const Icd::ComplexItemPtr &complex) const
{
    if (!emItem || !complex) {
        return false;
    }

    // create a table element from document
    TiXmlElement *emTable = new TiXmlElement("table");
    emItem->LinkEndChild(emTable);
    if (!saveTable(emTable, complex->table())) {
        return false;
    }

    return true;
}

bool XmlParser::saveItemFrame(TiXmlElement *emItem, const Icd::FrameItemPtr &frame) const
{
    if (!emItem || !frame) {
        return false;
    }

    // save object attributes
    if (!saveObject(emItem, frame)) {
        return false;
    }
    // sequenceCount
    emItem->SetAttribute("sequenceCount", frame->sequenceCount());
    // tables
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

bool XmlParser::saveTable(TiXmlElement *emTable, const Icd::TablePtr &table) const
{
    if (!emTable || !table) {
        return false;
    }

    // save object attributes
    if (!saveObject(emTable, table)) {
        return false;
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

bool XmlParser::saveSystem(TiXmlElement *emSystem, const Icd::SystemPtr &system) const
{
    if (!emSystem || !system) {
        return false;
    }

    // save object attributes
    if (!saveObject(emSystem, system)) {
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

bool XmlParser::saveVehicle(TiXmlElement *emVehicle, const Icd::VehiclePtr &vehicle) const
{
    if (!emVehicle || !vehicle) {
        return false;
    }

    // save object attributes
    if (!saveObject(emVehicle, vehicle)) {
        return false;
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
