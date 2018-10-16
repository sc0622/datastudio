#include "precomp.h"
#include "icdparser_xml.h"
#include "icdcore/icdcore_inc.h"
#include "../../../../icdwidget/icdwidget_global.h"

namespace Icd {

TiXmlElement *XmlParser::readElementRoot() const
{
    const std::string filePath = this->filePath();
#ifndef J_NO_QT
    //
    QFile file(QString::fromStdString(filePath));
    if (!file.exists()) {
        qWarning() << QString("File \"%1\" is not exists!").arg(file.fileName());
        return nullptr;
    }

    //
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << QString("File \"%1\" open failure!").arg(file.fileName());
        return nullptr;
    }
    file.close();

    TiXmlDocument *document = new TiXmlDocument();
    if (!document->LoadFile(QString::fromStdString(filePath).toLocal8Bit(),
                            TIXML_ENCODING_UTF8)) {
        const QString text =
                QStringLiteral("File \"%1\" parse failure!\ndesc：%2\npos：（line: %3，column: %4）")
                .arg(file.fileName())
                .arg(QString::fromStdString(document->ErrorDesc()))
                .arg(document->ErrorRow()).arg(document->ErrorCol());
        qWarning() << text;
        delete document;
        return nullptr;
    }
#else
    std::ifstream ifs(filePath);
    if (!ifs) {
        printf("File \"%s\" open failure!\n", filePath.c_str());
        return nullptr;
    }
    ifs.close();

    TiXmlDocument *document = new TiXmlDocument();
    if (!document->LoadFile(filePath, TIXML_ENCODING_UTF8)) {
        printf("File \"%s\" parse failure!\ndesc：%s\npos：（line: %d，column: %d）",
               filePath.c_str(), document->ErrorDesc(), document->ErrorRow(), document->ErrorCol());
        delete document;
        return nullptr;
    }
#endif
    return document->RootElement();
}

TiXmlElement *XmlParser::findVehicleElement(const std::string &vehicleId) const
{
    TiXmlElement *emRoot = readElementRoot();
    if (!emRoot) {
        return nullptr;
    }

    std::string sVal;
    TiXmlElement *emVehicle = nullptr;
    for (emVehicle = emRoot->FirstChildElement("vehicle");
         emVehicle != nullptr;
         emVehicle = emVehicle->NextSiblingElement("vehicle")) {
        // match vehicle element by id attribute
        if (emVehicle->QueryStringAttribute("id", &sVal) == TIXML_SUCCESS) {
            if (sVal == vehicleId) {
                break;
            }
        }
    }

    if (!emVehicle) {
        delete emRoot->GetDocument();
    }

    return emVehicle;
}

TiXmlElement *XmlParser::findSystemElement(const std::string &vehicleId, const std::string &systemId) const
{
    // find vehicle element
    TiXmlElement *emVehicle = findVehicleElement(vehicleId);
    if (!emVehicle) {
        return nullptr;   //
    }

    //
    // find system element
    std::string sVal;
    TiXmlElement *emSystem = nullptr;
    for (emSystem = emVehicle->FirstChildElement("system");
         emSystem != nullptr;
         emSystem = emSystem->NextSiblingElement("system")) {
        // match system element by id attribute
        if (emSystem->QueryStringAttribute("id", &sVal) == TIXML_SUCCESS) {
            if (sVal == systemId) {
                break;
            }
        }
    }

    if (!emSystem) {
        delete emVehicle->GetDocument();
    }

    return emSystem;
}

TiXmlElement *XmlParser::findTableElement(const std::string &vehicleId, const std::string &systemId,
                                          const std::string &tableId) const
{
    // find system element
    TiXmlElement *emSystem = findSystemElement(vehicleId, systemId);
    if (!emSystem) {
        return nullptr;
    }

    // find table element
    std::string sVal;
    TiXmlElement *emTable = nullptr;
    for (emTable = emSystem->FirstChildElement("table");
         emTable != nullptr;
         emTable = emTable->NextSiblingElement("table")) {
        // match table element by id attribute
        if (emTable->QueryStringAttribute("id", &sVal) == TIXML_SUCCESS) {
            if (sVal == tableId) {
                break;
            }
        }
    }

    if (!emTable) {
        delete emSystem->GetDocument();
    }

    return emTable;
}

TiXmlElement *XmlParser::findTableElement(const std::string &tableId) const
{
    TiXmlElement *emRoot = readElementRoot();
    if (!emRoot) {
        return nullptr;
    }

    std::string sVal;
    TiXmlElement *emTable = nullptr;
    for (emTable = emRoot->FirstChildElement("table");
         emTable != nullptr;
         emTable = emTable->NextSiblingElement("table")) {
        // match vehicle element by id attribute
        if (emTable->QueryStringAttribute("id", &sVal) == TIXML_SUCCESS) {
            if (sVal == tableId) {
                break;
            }
        }
    }

    if (!emTable) {
        delete emRoot->GetDocument();
    }

    return emTable;
}

Icd::ItemPtr XmlParser::createItem(int index, const TiXmlElement *element, int deep,
                                   Object *parent) const
{
    if (!element) {
        return Icd::ItemPtr();
    }

    int iVal = 0;
    std::string sVal;

    // parse item informations
    if (element->QueryStringAttribute("type", &sVal) != TIXML_SUCCESS) {
        return Icd::ItemPtr();
    }
    Icd::ItemType itemType = Item::stringType(sVal);
    TiXmlElement *emNew = nullptr;
    const TiXmlElement *emChildTable = nullptr;
    if (itemType == Icd::ItemComplex
            && (element->QueryIntAttribute("size", &iVal) == TIXML_SUCCESS)
            && (element->NoChildren() || !(emChildTable = element->FirstChildElement("table"))
                || emChildTable->NoChildren())) {
        itemType = Icd::ItemArray;
        emNew = element->Clone()->ToElement();
        emNew->RemoveChild(emNew->FirstChildElement("table"));
        emNew->SetAttribute("count", iVal);
        emNew->RemoveAttribute("size");
        emNew->SetAttribute("type", "array");
        emNew->SetAttribute("arrayType", "i8");
    }
    //
    Icd::ItemPtr newItem = Icd::Item::create(Icd::itoa(index), itemType, parent);
    if (!newItem) {
        return Icd::ItemPtr();
    }

    if (emNew) {
        const bool result = parseItem(emNew, newItem, deep);
        delete emNew;
        emNew = nullptr;
        if (!result) {
            return Icd::ItemPtr();
        }
    } else {
        if (!parseItem(element, newItem, deep)) {
            return Icd::ItemPtr();
        }
    }

    return newItem;
}

bool XmlParser::parseObject(const TiXmlElement *emObject, const Icd::ObjectPtr &object) const
{
    if (!emObject || !object) {
        return false;
    }

    std::string sVal;

    Icd::Object *parent = object->parent();
    if (!parent || parent->rtti() != Icd::ObjectComplex) {
        // name
        if (emObject->QueryStringAttribute("name", &sVal) == TIXML_SUCCESS) {
            object->setName(sVal);
        }
        // mark
        if (emObject->QueryStringAttribute("mark", &sVal) == TIXML_SUCCESS) {
            object->setMark(sVal);
        }
        // desc
        if (emObject->QueryStringAttribute("desc", &sVal) == TIXML_SUCCESS) {
            object->setDesc(sVal);
        }
    }

    return true;
}

bool XmlParser::parseItem(const TiXmlElement *emItem, Icd::ItemPtr &item, int deep) const
{
    if (!emItem || !item) {
        return false;
    }

    // parse object informations
    if (!parseObject(emItem, item)) {
        return false;
    }

    double dVal = 0.0;

    // defaultValue
    if (emItem->QueryDoubleAttribute("defaultValue", &dVal) == TIXML_SUCCESS) {
        item->setDefaultValue(dVal);
    }

    // parse self informations
    switch (item->type()) {
    case Icd::ItemHeader:
        // parse head informations
        if (!parseItemHead(emItem, JHandlePtrCast<Icd::HeaderItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemCounter:
        // parse counter informations
        if (!parseItemCounter(emItem, JHandlePtrCast<Icd::CounterItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemCheck:
        // parse check informations
        if (!parseItemCheck(emItem, JHandlePtrCast<Icd::CheckItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemFrameCode:
        // parse frameCode informations
        if (!parseItemFrameCode(emItem, JHandlePtrCast<Icd::FrameCodeItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemNumeric:
        // parse numeric informations
        if (!parseItemNumeric(emItem, JHandlePtrCast<Icd::NumericItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemArray:
        // parse array informations
        if (!parseItemArray(emItem, JHandlePtrCast<Icd::ArrayItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemBitMap:
    case Icd::ItemBitValue:
        // parse bit informations
        if (!parseItemBit(emItem, JHandlePtrCast<Icd::BitItem>(item))) {
            return false;
        }
        break;
    case Icd::ItemComplex:
        // parse complex informations
        if (!parseItemComplex(emItem, JHandlePtrCast<Icd::ComplexItem>(item), deep)) {
            return false;
        }
        break;
    case Icd::ItemFrame:
        // parse frame informations
        if (!parseItemFrame(emItem, JHandlePtrCast<Icd::FrameItem>(item), deep)) {
            return false;
        }
        break;
    default:
        return false;   // not supported data type
    }

    return true;
}

bool XmlParser::parseItemHead(const TiXmlElement *emItem, const Icd::HeaderItemPtr &head) const
{
    if (!emItem || !head) {
        return false;
    }

    std::string sVal;

    // defaultValue attribute
    if (emItem->QueryStringAttribute("defaultValue", &sVal) == TIXML_SUCCESS) {
        head->setDefaultValue(static_cast<unsigned char>((Icd::atou(sVal) & 0xFF)));
    }

    return true;
}

bool XmlParser::parseItemCounter(const TiXmlElement *emItem, const Icd::CounterItemPtr &counter) const
{
    if (!emItem || !counter) {
        return false;
    }

    std::string sVal;

    // counterType attribute
    if (emItem->QueryStringAttribute("counterType", &sVal) != TIXML_SUCCESS) {
        return false;
    }
    const Icd::CounterType counterType = Icd::CounterItem::stringCounterType(sVal);
    if (counterType == Icd::CounterInvalid) {
        return false;
    }
    counter->setCounterType(counterType);

    return true;
}

bool XmlParser::parseItemCheck(const TiXmlElement *emItem, const Icd::CheckItemPtr &check) const
{
    if (!emItem || !check) {
        return false;
    }

    std::string sVal;
    int iVal = 0;

    // checkType attribute
    if (emItem->QueryStringAttribute("checkType", &sVal) != TIXML_SUCCESS) {
        return false;
    }
    const Icd::CheckType checkType = Icd::CheckItem::stringCheckType(sVal);
    check->setCheckType(checkType);
    // startPos attribute
    if (emItem->QueryIntAttribute("startPos", &iVal) == TIXML_SUCCESS) {
        check->setStartPos(iVal);
    }
    // endPos attribute
    if (emItem->QueryIntAttribute("endPos", &iVal) == TIXML_SUCCESS) {
        check->setEndPos(iVal);
    }

    return true;
}

bool XmlParser::parseItemFrameCode(const TiXmlElement *emItem, const Icd::FrameCodeItemPtr &frameCode) const
{
    //
    if (!emItem || !frameCode) {
        return false;
    }

    std::string sVal;

    // frameCodeType attribute
    if (emItem->QueryStringAttribute("frameCodeType", &sVal) != TIXML_SUCCESS) {
        return false;
    }
    const Icd::FrameCodeType frameCodeType = Icd::FrameCodeItem::stringFrameCodeType(sVal);
    frameCode->setFrameCodeType(frameCodeType);
    // frameId attribute
    if (emItem->QueryStringAttribute("frameId", &sVal) == TIXML_SUCCESS) {
        frameCode->setFrameId(sVal);
    }

    return true;
}

bool XmlParser::parseItemNumeric(const TiXmlElement *emItem, const Icd::NumericItemPtr &numeric) const
{
    if (!emItem || !numeric) {
        return false;
    }

    std::string sVal;
    int iVal = 0;
    double dVal = 0.0;

    // numericType attribute
    if (emItem->QueryStringAttribute("numericType", &sVal) != TIXML_SUCCESS) {
        return false;
    }
    const Icd::NumericType numericType = Icd::NumericItem::stringNumericType(sVal);
    numeric->setNumericType(numericType);
    // scale attribute
    if (emItem->QueryDoubleAttribute("scale", &dVal) == TIXML_SUCCESS) {
        numeric->setScale(dVal);
    }
    // offset attribute
    if (emItem->QueryDoubleAttribute("offset", &dVal) == TIXML_SUCCESS) {
        numeric->setOffset(dVal);
    }
    // decimals attribute
    if (emItem->QueryIntAttribute("decimals", &iVal) == TIXML_SUCCESS) {
        numeric->setDecimals(iVal);
    }
    // min attribute
    if (emItem->QueryDoubleAttribute("min", &dVal) == TIXML_SUCCESS) {
        numeric->limit()->setMinimumInf(false);
        numeric->limit()->setMinimum(dVal);
    }
    // max attribute
    if (emItem->QueryDoubleAttribute("max", &dVal) == TIXML_SUCCESS) {
        numeric->limit()->setMaximumInf(false);
        numeric->limit()->setMaximum(dVal);
    }
    // unit attribute
    if (emItem->QueryStringAttribute("unit", &sVal) == TIXML_SUCCESS) {
        numeric->setUnit(sVal);
    }
    // spec
    for (const TiXmlElement *emSpec = emItem->FirstChildElement("spec");
         emSpec != nullptr;
         emSpec = emSpec->NextSiblingElement("spec")) {
        // key attribute
        double key = 0;
        if (emSpec->QueryDoubleAttribute("key", &key) != TIXML_SUCCESS) {
            continue;
        }
        // info attribute
        if (emSpec->QueryStringAttribute("info", &sVal) == TIXML_SUCCESS) {
            const std::string info = Icd::trimString(sVal);
            if (info.empty()) {
                continue;
            }
            numeric->addSpec(key, info);
        }
    }

    return true;
}

bool XmlParser::parseItemArray(const TiXmlElement *emItem, const ArrayItemPtr &array) const
{
    if (!emItem || !array) {
        return false;
    }

    int iVal = 0;
    std::string sVal;

    // arrayType attribute
    if (emItem->QueryStringAttribute("arrayType", &sVal) != TIXML_SUCCESS) {
        return false;
    }
    const Icd::ArrayType arrayType = Icd::ArrayItem::stringArrayType(sVal);
    if (arrayType == Icd::ArrayInvalid) {
        return false;
    }
    array->setArrayType(arrayType);

    // count
    if (emItem->QueryIntAttribute("count", &iVal) == TIXML_SUCCESS) {
        array->setCount(iVal);
    }

    return true;
}

bool XmlParser::parseItemBit(const TiXmlElement *emItem, const Icd::BitItemPtr &bit) const
{
    if (!emItem || !bit) {
        return false;
    }

    std::string sVal;
    int iVal = 0;
    double dVal = 0.0;

    // start attribute
    if (emItem->QueryIntAttribute("start", &iVal) == TIXML_SUCCESS) {
        bit->setBitStart(iVal);
    }
    // count attribute
    if (emItem->QueryIntAttribute("count", &iVal) == TIXML_SUCCESS) {
        bit->setBitCount(iVal);
    }
    // for bitvalue
    if (bit->type() == Icd::ItemBitValue) {
        // scale attribute
        if (emItem->QueryDoubleAttribute("scale", &dVal) == TIXML_SUCCESS) {
            bit->setScale(dVal);
        }
        // offset attribute
        if (emItem->QueryDoubleAttribute("offset", &dVal) == TIXML_SUCCESS) {
            bit->setOffset(dVal);
        }
        // decimals attribute
        if (emItem->QueryIntAttribute("decimals", &iVal) == TIXML_SUCCESS) {
            bit->setDecimals(iVal);
        }
        // min attribute
        if (emItem->QueryDoubleAttribute("min", &dVal) == TIXML_SUCCESS) {
            bit->limit()->setMinimumInf(false);
            bit->limit()->setMinimum(dVal);
        }
        // max attribute
        if (emItem->QueryDoubleAttribute("max", &dVal) == TIXML_SUCCESS) {
            bit->limit()->setMaximumInf(false);
            bit->limit()->setMaximum(dVal);
        }
        // unit attribute
        if (emItem->QueryStringAttribute("unit", &sVal) == TIXML_SUCCESS) {
            bit->setUnit(Icd::trimString(sVal));
        }
    }
    // spec
    for (const TiXmlElement *emSpec = emItem->FirstChildElement("spec");
         emSpec != nullptr;
         emSpec = emSpec->NextSiblingElement("spec")) {
        // key attribute
        icd_uint64 key = 0;
        if (emSpec->QueryStringAttribute("key", &sVal) == TIXML_SUCCESS) {
            key = Icd::atou64(sVal);
        }
        // info attribute
        if (emSpec->QueryStringAttribute("info", &sVal) == TIXML_SUCCESS) {
            const std::string info = Icd::trimString(sVal);
            if (info.empty()) {
                continue;
            }
            bit->addSpec(key, info);
        }
    }

    return true;
}

bool XmlParser::parseItemComplex(const TiXmlElement *emItem, const Icd::ComplexItemPtr &complex,
                                 int deep) const
{
    if (!emItem || !complex) {
        return false;
    }

    const Icd::TablePtr table = complex->table();

    // parse table informations
    if (!parseTable(emItem->FirstChildElement("table"), table, deep)) {
        return false;   // parse failure
    }
    // ignore table attributes
    table->setName(complex->name());
    table->setMark(complex->mark());
    table->setDesc(complex->desc());
    // size
    if (!table->isEmpty()) {
        complex->setBufferSize(table->bufferSize());
    }

    return true;
}

bool XmlParser::parseItemFrame(const TiXmlElement *emFrame, const Icd::FrameItemPtr &frame,
                               int deep) const
{
    if (!emFrame || !frame) {
        return false;
    }

    // parse object attributes
    if (!parseObject(emFrame, frame)) {
        return false;
    }

    int iVal = 0;

    // sequenceCount
    if (emFrame->QueryIntAttribute("sequenceCount", &iVal) == TIXML_SUCCESS) {
        frame->setSequenceCount(iVal);
    }
    // parse table elements
    for (const TiXmlElement *emTable = emFrame->FirstChildElement("table");
         emTable != nullptr;
         emTable = emTable->NextSiblingElement("table")) {
        // create table object
        const std::string id = emTable->Attribute("id");
        Icd::TablePtr table = std::make_shared<Icd::Table>(id, frame.get());
        // parse table informations
        if (!parseTable(emTable, table, deep)) {
            continue;   // parse failure
        }
        //
        frame->addTable(table);
    }

    return true;
}

bool XmlParser::parseTable(const TiXmlElement *emTable, const Icd::TablePtr &table, int deep) const
{
    if (!emTable || !table) {
        return false;
    }

    // parse object attributes
    if (!parseObject(emTable, table)) {
        return false;
    }

    int iVal = 0;
    std::string sVal;

    // sequence
    if (emTable->QueryIntAttribute("sequence", &iVal) == TIXML_SUCCESS) {
        table->setSequence(iVal);
    }
    //
    if (deep <= Icd::ObjectTable) {
        return true;
    }
    // parse item elements
    for (const TiXmlElement *emItem = emTable->FirstChildElement("item");
         emItem != nullptr;
         emItem = emItem->NextSiblingElement("item")) {
        Icd::ItemPtr item = createItem(table->itemCount() + 1, emItem, deep, table.get());
        if (!item) {
            continue;
        }

        table->appendItem(item);
    }

    // link frame and frame code
    const Icd::FrameCodeItemPtrArray &frameCodes = table->allFrameCode();
    Icd::FrameCodeItemPtrArray::const_iterator citer = frameCodes.cbegin();
    for (; citer != frameCodes.cend(); ++citer) {
        const Icd::FrameCodeItemPtr &frameCode = *citer;
        const std::string frameId = frameCode->frameId();
        if (frameId.empty()) {
            continue;
        }
        // find frame
        Icd::FrameItemPtr frame = JHandlePtrCast<Icd::FrameItem>(table->itemById(frameId));
        if (!frame) {
            continue;
        }
        // frame attribute
        frame->setParent(frameCode.get());
        // binding
        frameCode->setFrame(frame);
    }

    return true;
}

bool XmlParser::parseSystem(const TiXmlElement *emSystem, const Icd::SystemPtr &system, int deep) const
{
    if (!emSystem || !system) {
        return false;
    }

    // parse object attributes
    if (!parseObject(emSystem, system)) {
        return false;
    }

    //
    if (deep <= Icd::ObjectSystem) {
        return true;
    }

    // parse table elements
    for (const TiXmlElement *emTable = emSystem->FirstChildElement("table");
         emTable != nullptr;
         emTable = emTable->NextSiblingElement("table")) {
        // parse table informations
        const std::string id = emTable->Attribute("id");
        Icd::TablePtr table = std::make_shared<Icd::Table>(id, system.get());
        if (!parseTable(emTable, table, deep)) {
            continue;   // parse failure
        }
        //
        system->appendTable(table);
    }

    return true;
}

bool XmlParser::parseVehicle(const TiXmlElement *emVehicle, const Icd::VehiclePtr &vehicle, int deep) const
{
    if (!emVehicle || !vehicle) {
        return false;
    }

    // parse object attributes
    if (!parseObject(emVehicle, vehicle)) {
        return false;
    }

    //
    if (deep <= Icd::ObjectVehicle) {
        return true;
    }

    // parse system elements
    for (const TiXmlElement *emSystem = emVehicle->FirstChildElement("system");
         emSystem != nullptr;
         emSystem = emSystem->NextSiblingElement("system")) {
        // parse system informations
        const std::string id = emSystem->Attribute("id");
        Icd::SystemPtr system = std::make_shared<Icd::System>(id, vehicle.get());
        if (!parseSystem(emSystem, system, deep)) {
            continue;   // parse failure
        }
        // save
        vehicle->appendSystem(system);
    }

    return true;
}

} // end of namespace Icd
