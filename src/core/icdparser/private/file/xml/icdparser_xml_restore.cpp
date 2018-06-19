#include "precomp.h"
#include "icdparser_xml.h"
#include "icdcore/icdcore_inc.h"
#include "../../../../icdwidget/icdwidget_global.h"

namespace Icd {

/**
 * @brief XmlParser::readElementRoot
 * @return
 */
TiXmlElement *XmlParser::readElementRoot() const
{
    //
    QFile file(QString::fromStdString(filePath()));
    if (!file.exists()) {
        qWarning() << QString("File \"%1\" is not exists!").arg(file.fileName());
        return Q_NULLPTR;
    }

    //
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << QString("File \"%1\" open failure!").arg(file.fileName());
        return Q_NULLPTR;
    }
    file.close();

    TiXmlDocument *document = new TiXmlDocument();
    if (!document->LoadFile(QString::fromStdString(filePath()).toLocal8Bit(),
                            TIXML_ENCODING_UTF8)) {
        const QString text =
                QStringLiteral("File \"%1\" parse failure!\ndesc：%2\npos：（line: %3，column: %4）")
                .arg(file.fileName())
                .arg(QString::fromStdString(document->ErrorDesc()))
                .arg(document->ErrorRow()).arg(document->ErrorCol());
        qWarning() << text;
        delete document;
        return Q_NULLPTR;
    }

    return document->RootElement();
}

/**
 * @brief XmlParser::findVehicleElement
 * @param vehicleId
 * @return
 */
TiXmlElement *XmlParser::findVehicleElement(const std::string &vehicleId) const
{
    TiXmlElement *emRoot = readElementRoot();
    if (!emRoot) {
        return Q_NULLPTR;
    }

    const QString &_vehicleId = QString::fromStdString(vehicleId);

    std::string sVal;
    TiXmlElement *emVehicle = Q_NULLPTR;
    for (emVehicle = emRoot->FirstChildElement("vehicle");
         emVehicle != Q_NULLPTR;
         emVehicle = emVehicle->NextSiblingElement("vehicle")) {
        // match vehicle element by id attribute
        if (emVehicle->QueryStringAttribute("id", &sVal) == TIXML_SUCCESS) {
            if (QString::fromStdString(sVal).trimmed() == _vehicleId) {
                break;
            }
        }
    }

    if (!emVehicle) {
        delete emRoot->GetDocument();
    }

    return emVehicle;
}

/**
 * @brief XmlParser::findSystemElement
 * @param vehicleId
 * @param systemId
 * @return
 */
TiXmlElement *XmlParser::findSystemElement(const std::string &vehicleId,
                                           const std::string &systemId) const
{
    // find vehicle element
    TiXmlElement *emVehicle = findVehicleElement(vehicleId);
    if (!emVehicle) {
        return Q_NULLPTR;   //
    }

    //
    const QString &_systemId = QString::fromStdString(systemId);

    // find system element
    std::string sVal;
    TiXmlElement *emSystem = Q_NULLPTR;
    for (emSystem = emVehicle->FirstChildElement("system");
         emSystem != Q_NULLPTR;
         emSystem = emSystem->NextSiblingElement("system")) {
        // match system element by id attribute
        if (emSystem->QueryStringAttribute("id", &sVal) == TIXML_SUCCESS) {
            if (QString::fromStdString(sVal).trimmed() == _systemId) {
                break;
            }
        }
    }

    if (!emSystem) {
        delete emVehicle->GetDocument();
    }

    return emSystem;
}

/**
 * @brief XmlParser::findTableElement
 * @param vehicleId
 * @param systemId
 * @param tableId
 * @return
 */
TiXmlElement *XmlParser::findTableElement(const std::string &vehicleId,
                                          const std::string &systemId,
                                          const std::string &tableId) const
{
    // find system element
    TiXmlElement *emSystem = findSystemElement(vehicleId, systemId);
    if (!emSystem) {
        return Q_NULLPTR;   //
    }

    //
    const QString &_tableId = QString::fromStdString(tableId);

    // find table element
    std::string sVal;
    TiXmlElement *emTable = Q_NULLPTR;
    for (emTable = emSystem->FirstChildElement("table");
         emTable != Q_NULLPTR;
         emTable = emTable->NextSiblingElement("table")) {
        // match table element by id attribute
        if (emTable->QueryStringAttribute("id", &sVal) == TIXML_SUCCESS) {
            if (QString::fromStdString(sVal).trimmed() == _tableId) {
                break;
            }
        }
    }

    if (!emTable) {
        delete emSystem->GetDocument();
    }

    return emTable;
}

/**
 * @brief XmlParser::findTableElement
 * @param tableId
 * @return
 */
TiXmlElement *XmlParser::findTableElement(const std::string &tableId) const
{
    TiXmlElement *emRoot = readElementRoot();
    if (!emRoot) {
        return Q_NULLPTR;
    }

    //
    const QString &_tableId = QString::fromStdString(tableId);

    std::string sVal;
    TiXmlElement *emTable = Q_NULLPTR;
    for (emTable = emRoot->FirstChildElement("table");
         emTable != Q_NULLPTR;
         emTable = emTable->NextSiblingElement("table")) {
        // match vehicle element by id attribute
        if (emTable->QueryStringAttribute("id", &sVal) == TIXML_SUCCESS) {
            if (QString::fromStdString(sVal).trimmed() == _tableId) {
                break;
            }
        }
    }

    if (!emTable) {
        delete emRoot->GetDocument();
    }

    return emTable;
}

/**
 * @brief XmlParser::parseObject
 * @param emObject
 * @param object
 * @return
 */
bool XmlParser::parseObject(const TiXmlElement *emObject,
                            const Icd::ObjectPtr &object) const
{
    //
    if (!emObject || !object) {
        return false;
    }

    std::string sVal;

    // id
#if defined(PARSE_TABLE_DYNAMIC_ID)
    if (object->objectType() == Icd::ObjectTable) {
        object->setId("ICDTable_"
                      + QUuid::createUuid().toString().remove(QRegExp("[{}-]")).toStdString());
    } else {
        if (object->id().empty()) {
            if (emObject->QueryStringAttribute("id", &sVal) == TIXML_SUCCESS) {
                object->setId(sVal);
            }
        }
    }
#else
    if (object->id().empty()) {
        if (emObject->QueryStringAttribute("id", &sVal) == TIXML_SUCCESS) {
            object->setId(sVal);
        }
    }
#endif

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

    return true;
}

/**
 * @brief XmlParser::parseItem
 * @param emItem
 * @param item
 * @param deep
 * @return
 */
bool XmlParser::parseItem(const TiXmlElement *emItem, Icd::ItemPtr &item, int deep) const
{
    if (!emItem || !item) {
        return false;
    }

    // parse object informations
    if (!parseObject(emItem, JHandlePtrCast<Icd::Object, Icd::Item>(item))) {
        return false;
    }

    double dVal = 0.0;

    // defaultValue
    if (emItem->QueryDoubleAttribute("defaultValue", &dVal) == TIXML_SUCCESS) {
        item->setDefaultValue(dVal);
    }

    // parse self informations
    switch (item->type()) {
    case Icd::ItemHead:
        // parse head informations
        if (!parseItemHead(emItem, JHandlePtrCast<Icd::HeaderItem, Icd::Item>(item))) {
            return false;
        }
        break;
    case Icd::ItemCounter:
        // parse counter informations
        if (!parseItemCounter(emItem, JHandlePtrCast<Icd::CounterItem, Icd::Item>(item))) {
            return false;
        }
        break;
    case Icd::ItemCheck:
        // parse check informations
        if (!parseItemCheck(emItem, JHandlePtrCast<Icd::CheckItem, Icd::Item>(item))) {
            return false;
        }
        break;
    case Icd::ItemFrameCode:
        // parse frameCode informations
        if (!parseItemFrameCode(emItem, JHandlePtrCast<Icd::FrameCodeItem, Icd::Item>(item))) {
            return false;
        }
        break;
    case Icd::ItemNumeric:
        // parse numeric informations
        if (!parseItemNumeric(emItem, JHandlePtrCast<Icd::NumericItem, Icd::Item>(item))) {
            return false;
        }
        break;
    case Icd::ItemBitMap:
    case Icd::ItemBitValue:
        // parse bit informations
        if (!parseItemBit(emItem, JHandlePtrCast<Icd::BitItem, Icd::Item>(item))) {
            return false;
        }
        break;
    case Icd::ItemComplex:
        // parse complex informations
        if (!parseItemComplex(emItem, JHandlePtrCast<Icd::ComplexItem, Icd::Item>(item), deep)) {
            return false;
        }
        break;
    case Icd::ItemFrame:
        // parse frame informations
        if (!parseItemFrame(emItem, JHandlePtrCast<Icd::FrameItem, Icd::Item>(item), deep)) {
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
 * @brief XmlParser::parseItemHead
 * @param emItem
 * @param head
 * @return
 */
bool XmlParser::parseItemHead(const TiXmlElement *emItem,
                              const Icd::HeaderItemPtr &head) const
{
    //
    if (!emItem || !head) {
        return false;
    }

    std::string sVal;

    // defaultValue attribute
    if (emItem->QueryStringAttribute("defaultValue", &sVal) == TIXML_SUCCESS) {
        const QString text = QString::fromStdString(sVal).trimmed();
        if (text.startsWith("0x", Qt::CaseInsensitive)) {
            head->setDefaultValue((unsigned char)(text.toUInt(0, 16) & 0xFF));
        } else {
            head->setDefaultValue((unsigned char)(text.toUInt() & 0xFF));
        }
    }

    return true;
}

bool XmlParser::parseItemCounter(const TiXmlElement *emItem,
                                 const Icd::CounterItemPtr &counter) const
{
    //
    if (!emItem || !counter) {
        return false;
    }

    std::string sVal;

    // counterType attribute
    if (emItem->QueryStringAttribute("counterType", &sVal) != TIXML_SUCCESS) {
        return false;
    }
    const Icd::CounterType counterType = Icd::CounterItem::stringCounterType(
                QString::fromStdString(sVal).toStdString());
    if (counterType == Icd::CounterInvalid) {
        return false;
    }
    counter->setCounterType(counterType);

    return true;
}

/**
 * @brief XmlParser::parseItemCheck
 * @param emItem
 * @param check
 * @return
 */
bool XmlParser::parseItemCheck(const TiXmlElement *emItem,
                               const Icd::CheckItemPtr &check) const
{
    //
    if (!emItem || !check) {
        return false;
    }

    std::string sVal;
    int iVal = 0;

    // checkType attribute
    if (emItem->QueryStringAttribute("checkType", &sVal) != TIXML_SUCCESS) {
        return false;
    }
    const Icd::CheckType checkType = Icd::CheckItem::stringCheckType(
                QString::fromStdString(sVal).toStdString());
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

/**
 * @brief XmlParser::parseItemFrameCode
 * @param emItem
 * @param frameCode
 * @return
 */
bool XmlParser::parseItemFrameCode(const TiXmlElement *emItem,
                                   const Icd::FrameCodeItemPtr &frameCode) const
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
    const Icd::FrameCodeType frameCodeType = Icd::FrameCodeItem::stringFrameCodeType(
                QString::fromStdString(sVal).toStdString());
    frameCode->setFrameCodeType(frameCodeType);

    // frameId attribute
    if (emItem->QueryStringAttribute("frameId", &sVal) == TIXML_SUCCESS) {
        frameCode->setFrameId(QString::fromStdString(sVal).toStdString());
    }

    return true;
}

/**
 * @brief XmlParser::parseItemNumeric
 * @param emItem
 * @param numeric
 * @return
 */
bool XmlParser::parseItemNumeric(const TiXmlElement *emItem,
                                 const Icd::NumericItemPtr &numeric) const
{
    //
    if (!emItem || !numeric) {
        return false;
    }

    std::string sVal;
    double dVal = 0.0;

    // numericType attribute
    if (emItem->QueryStringAttribute("numericType", &sVal) != TIXML_SUCCESS) {
        return false;
    }
    const Icd::NumericType numericType = Icd::NumericItem::stringNumericType(
                QString::fromStdString(sVal).toStdString());
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
    if (emItem->QueryDoubleAttribute("decimals", &dVal) == TIXML_SUCCESS) {
        numeric->setDecimals(dVal);
    }

    // min attribute
    if (emItem->QueryStringAttribute("min", &sVal) == TIXML_SUCCESS) {
        if (sVal.empty()) {
            numeric->limit()->setLeftInf(true);
        } else {
            numeric->limit()->setMinimum(QString::fromStdString(sVal).toDouble());
            numeric->limit()->setLeftInf(false);
        }
    }

    // max attribute
    if (emItem->QueryStringAttribute("max", &sVal) == TIXML_SUCCESS) {
        if (sVal.empty()) {
            numeric->limit()->setRightInf(true);
        } else {
            numeric->limit()->setMaximum(QString::fromStdString(sVal).toDouble());
            numeric->limit()->setRightInf(false);
        }
    }

    // unit attribute
    if (emItem->QueryStringAttribute("unit", &sVal) == TIXML_SUCCESS) {
        numeric->setUnit(QString::fromStdString(sVal).trimmed().toStdString());
    }

    // spec
    for (const TiXmlElement *emSpec = emItem->FirstChildElement("spec");
         emSpec != Q_NULLPTR;
         emSpec = emSpec->NextSiblingElement("spec")) {
        // key attribute
        double key = 0;
        if (emSpec->QueryDoubleAttribute("key", &key) != TIXML_SUCCESS) {
            continue;
        }
        // info attribute
        if (emSpec->QueryStringAttribute("info", &sVal) == TIXML_SUCCESS) {
            QString info = QString::fromStdString(sVal).trimmed();
            if (info.isEmpty()) {
                continue;
            }
            numeric->addSpec(key, info.toStdString());
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
    const Icd::ArrayType arrayType = Icd::ArrayItem::stringArrayType(
                QString::fromStdString(sVal).toStdString());
    if (arrayType == Icd::InvalidArray) {
        return false;
    }
    array->setArrayType(arrayType);

    // count
    if (emItem->QueryIntAttribute("count", &iVal) == TIXML_SUCCESS) {
        array->setCount(iVal);
    }

    return true;
}

/**
 * @brief XmlParser::parseItemBit
 * @param emItem
 * @param bit
 * @return
 */
bool XmlParser::parseItemBit(const TiXmlElement *emItem,
                             const Icd::BitItemPtr &bit) const
{
    //
    if (!emItem || !bit) {
        return false;
    }

    std::string sVal;
    int iVal = 0;

    // start attribute
    if (emItem->QueryIntAttribute("start", &iVal) == TIXML_SUCCESS) {
        bit->setBitStart(iVal);
    }

    // count attribute
    if (emItem->QueryIntAttribute("count", &iVal) == TIXML_SUCCESS) {
        bit->setBitCount(iVal);
    }

    // typeSize attribute
    if (emItem->QueryIntAttribute("typeSize", &iVal) == TIXML_SUCCESS) {
        bit->setTypeSize(iVal);
    }

    // spec
    for (const TiXmlElement *emSpec = emItem->FirstChildElement("spec");
         emSpec != Q_NULLPTR;
         emSpec = emSpec->NextSiblingElement("spec")) {
        // key attribute
        int key = 0;
        if (emSpec->QueryStringAttribute("key", &sVal) == TIXML_SUCCESS) {
            key = QString::fromStdString(sVal).toInt(Q_NULLPTR, 16);
        }
        // info attribute
        if (emSpec->QueryStringAttribute("info", &sVal) == TIXML_SUCCESS) {
            const QString info = QString::fromStdString(sVal).trimmed();
            if (info.isEmpty()) {
                continue;
            }
            bit->addSpec(key, info.toStdString());
        }
    }

    return true;
}

/**
 * @brief XmlParser::parseItemComplex
 * @param emItem
 * @param complex
 * @param deep
 * @return
 */
bool XmlParser::parseItemComplex(const TiXmlElement *emItem,
                                 const Icd::ComplexItemPtr &complex, int deep) const
{
    //
    if (!emItem || !complex) {
        return false;
    }

    double dVal = 0.0;

    // bufferSize
    if (emItem->QueryDoubleAttribute("size", &dVal) == TIXML_SUCCESS) {
        complex->setBufferSize(dVal);
    }

    const Icd::TablePtr table = complex->table();

    // parse table informations
    if (!parseTable(emItem->FirstChildElement("table"), table, deep)) {
        return false;   // parse failure
    }

    // ignore table attributes
    table->setName(complex->name());
    table->setMark(complex->mark());

    return true;
}

/**
 * @brief XmlParser::parseItemFrame
 * @param emFrame
 * @param frame
 * @param deep
 * @return
 */
bool XmlParser::parseItemFrame(const TiXmlElement *emFrame,
                               const Icd::FrameItemPtr &frame, int deep) const
{
    //
    if (!emFrame || !frame) {
        return false;
    }

    // parse object attributes
    if (!parseObject(emFrame, JHandlePtrCast<Icd::Object, Icd::FrameItem>(frame))) {
        return false;       // parse failure
    }

    double dVal = 0.0;
    int iVal = 0;

    // bufferSize
    if (emFrame->QueryDoubleAttribute("size", &dVal) == TIXML_SUCCESS) {
        frame->setBufferSize(dVal);
    }

    // sequenceCount
    if (emFrame->QueryIntAttribute("sequenceCount", &iVal) == TIXML_SUCCESS) {
        frame->setSequenceCount(iVal);
    }

    // parse table elements
    for (const TiXmlElement *emTable = emFrame->FirstChildElement("table");
         emTable != Q_NULLPTR;
         emTable = emTable->NextSiblingElement("table")) {
        // code attribute
        int mark = 0;
        const QString text = QString::fromStdString(emTable->Attribute("mark")).trimmed();
        if (text.startsWith("0x", Qt::CaseInsensitive)) {
            mark = text.toInt(0, 16);
        } else {
            mark = text.toInt();
        }
        // create table object
        Icd::TablePtr table(new Icd::Table());
        // parse table informations
        if (!parseTable(emTable, table, deep)) {
            continue;   // parse failure
        }

        //
        frame->addTable(table);
    }

    return true;
}

/**
 * @brief XmlParser::parseTable
 * @param emTable
 * @param table
 * @param deep
 * @return
 */
bool XmlParser::parseTable(const TiXmlElement *emTable,
                           const Icd::TablePtr &table, int deep) const
{
    Q_ASSERT(deep <= Icd::ObjectItem);
    //
    if (!emTable || !table) {
        return false;
    }

    // parse object attributes
    if (!parseObject(emTable, JHandlePtrCast<Icd::Object, Icd::Table>(table))) {
        return false;       // parse failure
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
         emItem != Q_NULLPTR;
         emItem = emItem->NextSiblingElement("item")) {
        // parse item informations
        if (emItem->QueryStringAttribute("type", &sVal) != TIXML_SUCCESS) {
            continue;
        }
        const std::string sType = sVal;
        Icd::ItemPtr item = Icd::Item::create(QString::number(table->itemCount() + 1)
                                              .toStdString(), Icd::Item::stringType(sType));
        if (!parseItem(emItem, item, deep)) {
            continue;   // parse failure
        }
        item->setParent(table.get());
        // save
        table->appendItem(item);
        //
        if (QString::fromStdString(item->name()) == QStringLiteral("档位11")) {
            int i = 0;
            ++i;
        }
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
        Icd::FrameItemPtr frame = JHandlePtrCast<Icd::FrameItem, Icd::Item>(
                    table->itemById(frameId));
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

/**
 * @brief XmlParser::parseSystem
 * @param emSystem
 * @param system
 * @param deep
 * @return
 */
bool XmlParser::parseSystem(const TiXmlElement *emSystem,
                            const Icd::SystemPtr &system, int deep) const
{
    Q_ASSERT(deep <= Icd::ObjectItem);
    //
    if (!emSystem || !system) {
        return false;
    }

    // parse object attributes
    if (!parseObject(emSystem, JHandlePtrCast<Icd::Object, Icd::System>(system))) {
        return false;       // parse failure
    }

    // parse self informations

    //
    if (deep <= Icd::ObjectSystem) {
        return true;
    }

    // parse table elements
    for (const TiXmlElement *emTable = emSystem->FirstChildElement("table");
         emTable != Q_NULLPTR;
         emTable = emTable->NextSiblingElement("table")) {
        // parse table informations
        Icd::TablePtr table(new Table(system.get()));
        if (!parseTable(emTable, table, deep)) {
            continue;   // parse failure
        }
        // save
        system->appendTable(table);
    }

    return true;
}

/**
 * @brief XmlParser::parseVehicle
 * @param emVehicle
 * @param vehicle
 * @param deep
 * @return
 */
bool XmlParser::parseVehicle(const TiXmlElement *emVehicle,
                             const Icd::VehiclePtr &vehicle, int deep) const
{
    Q_ASSERT(deep <= Icd::ObjectItem);

    //
    if (!emVehicle || !vehicle) {
        return false;       //
    }

    // parse object attributes
    if (!parseObject(emVehicle, JHandlePtrCast<Icd::Object, Icd::Vehicle>(vehicle))) {
        return false;       // parse failure
    }

    // parse self informations

    //
    if (deep <= Icd::ObjectVehicle) {
        return true;
    }

    // parse system elements
    for (const TiXmlElement *emSystem = emVehicle->FirstChildElement("system");
         emSystem != Q_NULLPTR;
         emSystem = emSystem->NextSiblingElement("system")) {
        // parse system informations
        Icd::SystemPtr system(new System(vehicle.get()));
        if (!parseSystem(emSystem, system, deep)) {
            continue;   // parse failure
        }
        // save
        vehicle->appendSystem(system);
    }

    return true;
}


} // end of namespace Icd
