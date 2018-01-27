#include "precomp.h"
#include "icdparser_sql_p.h"
#include "jframe_facade.h"
#include "../icdwidget/icdwidget_global.h"

namespace Icd {

// for query begin

/**
 * @brief SqlParserData::queryItem
 * @param tableId
 * @param dataItemId
 * @param rules
 * @return
 */
bool SqlParserData::queryItem(const std::string &tableId,
                              const std::string &itemId, stTableRules &rules)
{
    //
    DMSpace::srMap mapRules;
    if (!db->readTableRules(tableId, mapRules)) {
        return false;
    }

    DMSpace::srMap::const_iterator citer = mapRules.find(atoi(itemId.c_str()));
    if (citer == mapRules.cend()) {
        return false;
    }

    rules = citer->second;

    return true;
}

/**
 * @brief SqlParserData::icdBase
 * @param id
 * @return
 */
stICDBase SqlParserData::icdBase(const std::string &id) const
{
    stICDBase result;
    if (id.empty()) {
        return result;
    }

    // read base infomation
    db->readICDBase(id, result);

    return result;
}

/**
 * @brief SqlParserData::icdBaseArray
 * @param planeID
 * @param systemID
 * @return
 */
std::vector<stICDBase> SqlParserData::icdBaseArray(const std::string &planeID,
                                                   const std::string &systemID,
                                                   bool topOnly) const
{
    std::vector<stICDBase> result;
    DMSpace::_vectorSB icdBase;
    std::string condition;

    if (!planeID.empty()) {
        condition = planeID + "/" + systemID;
    }
    // read icdbase
    if (!db->readSpecificICD(condition, icdBase, topOnly)) {
        return result;
    }

    const int count = icdBase.size();
    for (int i = 0; i < count; ++i) {
        const std::vector<stICDBase> &base = icdBase[i].second;
        result.insert(result.end(), base.begin(), base.end());
    }

    return result;
}

/**
 * @brief SqlParserData::systemBase
 * @param id
 * @return
 */
stSystem SqlParserData::systemBase(const std::string &planeID,
                                   const std::string &systemID) const
{
    stSystem result;
    if (planeID.empty()) {
        return result;
    }

    std::vector<stSystem> systems;
    // read system
    if (!db->readSystem(atoi(planeID.c_str()), systems)) {
        return result;
    }

    // find system
    const int count = systems.size();
    for (int i = 0; i < count; ++i) {
        stSystem &system = systems[i];
        if (system.nCode == QString::fromStdString(systemID).toInt()) {
            return system;
        }
    }

    return result;
}

/**
 * @brief SqlParserData::systemBaseArray
 * @param planeID
 * @return
 */
std::vector<stSystem> SqlParserData::systemBaseArray(const std::string &planeID) const
{
    std::vector<stSystem> result;
    // read system
    db->readSystem(atoi(planeID.c_str()), result);

    return result;
}

/**
 * @brief SqlParserData::planeBase
 * @param id
 * @return
 */
stPlane SqlParserData::planeBase(const std::string &id) const
{
    stPlane result;
    if (id.empty()) {
        return result;
    }

    std::vector<stPlane> planes;
    // read plane
    if (!db->readPlane(planes)) {
        return result;
    }

    if (!planes.empty()) {
        const int count = planes.size();
        for (int i = 0; i < count; ++i) {
            stPlane &plane = planes[i];
            if (plane.nCode == QString::fromStdString(id).toInt()) {
                return plane;
            }
        }
    }

    return result;
}

/**
 * @brief SqlParserData::planeBaseArray
 * @return
 */
std::vector<stPlane> SqlParserData::planeBaseArray() const
{
    std::vector<stPlane> result;
    // read plane
    if (!db->readPlane(result)) {
        return result;
    }

    return result;
}

DMSpace::_vectorIcdTR SqlParserData::tableRules(const std::string &planeID,
                                                const std::string &systemID) const
{
    DMSpace::_vectorIcdTR result;

    // read icdbase
    std::vector<std::string> names;
    std::vector<stICDBase> tables = icdBaseArray(planeID, systemID);
    const int count = tables.size();
    for (int i = 0; i < count; ++i) {
        names.push_back(tables[i].sName);
    }

    DMSpace::svrMap rules;
    // read rules
    if (!db->readTableRules(names, rules)) {
        return result;
    }

    DMSpace::svrMap::iterator it = rules.end();
    for (int i = 0; i < count; ++i) {
        const stICDBase &single = tables[i];
        if ((it = rules.find(single.sName)) != rules.end()) {
            result.push_back(std::make_pair(single, it->second));
        } else {
            result.push_back(std::make_pair(single, std::vector<stTableRules>()));
        }
    }

    return result;
}

DMSpace::_vectorIcdTR SqlParserData::tableRules(const std::string &tableId) const
{
    DMSpace::_vectorIcdTR result;
    std::vector<std::string> names;

    // read subTables
    names = db->querySubTables(tableId);
    names.insert(names.begin(), tableId);

    // read icdbase
    std::vector<stICDBase> tables;
    if (!db->readICDBase(names, tables)) {
        return result;
    }

    DMSpace::svrMap rules;
    // read rules
    if (!db->readTableRules(names, rules)) {
        return result;
    }

    DMSpace::svrMap::iterator it = rules.end();
    const int count = tables.size();
    for (int i = 0; i < count; ++i) {
        const stICDBase &single = tables[i];
        if ((it = rules.find(single.sName)) != rules.end()) {
            result.push_back(std::make_pair(single, it->second));
        } else {
            result.push_back(std::make_pair(single, std::vector<stTableRules>()));
        }
    }

    return result;
}

DMSpace::pairIcdTR SqlParserData::singleIcdTR(const DMSpace::_vectorIcdTR &rules,
                                              const stICDBase &icdBase) const
{
    DMSpace::pairIcdTR result;

    const int count = rules.size();
    for (int i = 0; i < count; ++i) {
        const DMSpace::pairIcdTR &icdTR = rules[i];
        if (icdTR.first.sName == icdBase.sName) {
            result = icdTR;
            break;
        }
    }

    return result;
}

DMSpace::pairPS SqlParserData::singlePS(const DMSpace::_vectorPS &ps,
                                        const stPlane &plane) const
{
    DMSpace::pairPS result;

    const int count = ps.size();
    for (int i = 0; i < count; ++i) {
        const DMSpace::pairPS &data = ps[i];
        if (data.first.nCode == plane.nCode) {
            result = data;
            break;
        }
    }

    return result;
}
// for query end

// for parse begin

/**
 * @brief SqlParserData::parseObject
 * @param rule
 * @param object
 * @return
 */
bool SqlParserData::parseObject(const stTableRules &rule,
                                const Icd::ObjectPtr &object) const
{
    if (0 == object) {
        return false;
    }

    // id attribute
    object->setId(QString::number(rule.nSerial).toStdString());

    // name attribute
    object->setName(rule.sName);

    // mark attribute
    object->setMark(rule.sPrgCode);

    // desc attribute
    object->setDesc(rule.sRemark);

    return true;
}

/**
 * @brief SqlParserData::parseItem
 * @param emItem
 * @param item
 * @param deep
 * @return
 */
bool SqlParserData::parseItem(const stTableRules &rule,
                              Icd::ItemPtr &item,
                              const DMSpace::_vectorIcdTR &tableRules, int deep) const
{
    item = 0;
    bool result = false;
    switch (rule.uType) {
    case GlobalDefine::dtHead:
        item = Icd::HeaderItemPtr(new Icd::HeaderItem());
        break;
    case GlobalDefine::dtCounter:
        item = Icd::CounterItemPtr(new Icd::CounterItem());
        break;
    case GlobalDefine::dtCheck:
        item = Icd::CheckItemPtr(new Icd::CheckItem());
        break;
    case GlobalDefine::dtFrameCode:
        item = Icd::FrameCodeItemPtr(new Icd::FrameCodeItem());
        break;
    case GlobalDefine::dtU8:
    case GlobalDefine::dt8:
    case GlobalDefine::dtU16:
    case GlobalDefine::dt16:
    case GlobalDefine::dtU32:
    case GlobalDefine::dt32:
        //case GlobalDefine::dtU64:
        //case GlobalDefine::dt64:
    case GlobalDefine::dtF32:
    case GlobalDefine::dtF64:
        item = Icd::NumericItemPtr(new Icd::NumericItem());
        break;
    case GlobalDefine::dtBitMap:
        item = Icd::BitItemPtr(new Icd::BitItem(Icd::ItemBitMap));
        break;
    case GlobalDefine::dtBitValue:
        item =Icd::BitItemPtr( new Icd::BitItem(Icd::ItemBitValue));
        break;
    case GlobalDefine::dtDiscern:
        item = Icd::FrameItemPtr(new Icd::FrameItem());
        break;
    case GlobalDefine::dtComplex:
        item = Icd::ComplexItemPtr(new Icd::ComplexItem());
        break;
    case GlobalDefine::dtBuffer:
        item = Icd::ComplexItemPtr(new Icd::ComplexItem());
        break;
    default:
        return false;
    }

    // parse object
    if (!parseObject(rule, JHandlePtrCast<Icd::Object, Icd::Item>(item))) {
        return result;
    }

    // parse other
    switch (item->type()) {
    case Icd::ItemHead:
        result = parseItemHead(rule, JHandlePtrCast<Icd::HeaderItem, Icd::Item>(item));
        break;
    case Icd::ItemCounter:
        result = parseItemCounter(rule, JHandlePtrCast<Icd::CounterItem, Icd::Item>(item));
        break;
    case Icd::ItemCheck:
        result = parseItemCheck(rule, JHandlePtrCast<Icd::CheckItem, Icd::Item>(item));
        break;
    case Icd::ItemFrameCode:
        result = parseItemFrameCode(rule, JHandlePtrCast<Icd::FrameCodeItem, Icd::Item>(item));
        break;
    case Icd::ItemNumeric:
        result = parseItemNumeric(rule, JHandlePtrCast<Icd::NumericItem, Icd::Item>(item));
        break;
    case Icd::ItemBitMap:
    case Icd::ItemBitValue:
        result = parseItemBit(rule, JHandlePtrCast<Icd::BitItem, Icd::Item>(item));
        break;
    case Icd::ItemComplex:
        result = parseItemComplex(rule, JHandlePtrCast<Icd::ComplexItem, Icd::Item>(item),
                                  tableRules, deep);
        break;
    case Icd::ItemFrame:
        result = parseItemFrame(rule, JHandlePtrCast<Icd::FrameItem, Icd::Item>(item),
                                tableRules, deep);
        break;
    default:
        break;
    }

    return result;
}

/**
 * @brief SqlParserData::parseItemHead
 * @param rule
 * @param head
 * @return
 */
bool SqlParserData::parseItemHead(const stTableRules &rule,
                                  const Icd::HeaderItemPtr &head) const
{
    if (0 == head) {
        return false;
    }

    // value attribute
    const QString value = QString::fromStdString(rule.sDefault);
    if (true/*value.startsWith("0x", Qt::CaseInsensitive)*/) {
        head->setDefaultValue((unsigned char)(value.toUInt(0, 16) & 0xFF));
    } else {
        head->setDefaultValue((unsigned char)(value.toUInt() & 0xFF));
    }

    return true;
}

bool SqlParserData::parseItemCounter(const stTableRules &rule,
                                     const CounterItemPtr &counter) const
{
    if (0 == counter) {
        return false;
    }

    // counterType attribute
    switch (atoi(rule.sDefault.c_str())) {
    case GlobalDefine::counterU8:
        counter->setCounterType(Icd::CounterU8);
        break;
    case GlobalDefine::counterU16:
        counter->setCounterType(Icd::CounterU16);
        break;
    case GlobalDefine::counterU32:
        counter->setCounterType(Icd::CounterU32);
        break;
        /*case GlobalDefine::counterU64:
        counter->setCounterType(Icd::CounterU64);
        break;*/
    default:
        break;
    }

    return true;
}

/**
 * @brief SqlParserData::parseItemCheck
 * @param rule
 * @param check
 * @return
 */
bool SqlParserData::parseItemCheck(const stTableRules &rule,
                                   const Icd::CheckItemPtr &check) const
{
    if (0 == check) {
        return false;
    }

    // checkType attribute
    switch (atoi(rule.sDefault.c_str()))
    {
    case GlobalDefine::ctInvalid:
        check->setCheckType(Icd::CheckNone);
        break;
    case GlobalDefine::ct8And:
        check->setCheckType(Icd::CheckSum8);
        break;
    case GlobalDefine::ct16And:
        check->setCheckType(Icd::CheckSum16);
        break;
        /*case GlobalDefine::ctCRC8:
        check->setCheckType(Icd::CheckCrc8);
        break;*/
    case GlobalDefine::ctCRC:
        check->setCheckType(Icd::CheckCrc16);
        break;
    case GlobalDefine::ctXor8:
        check->setCheckType(Icd::CheckXor8);
        break;
    case GlobalDefine::ctXor16:
        check->setCheckType(Icd::CheckXor16);
        break;
    default:
        break;
    }

    // check range attribute
    if (!rule.sRange.empty()) {
        QStringList lstRange = QString::fromStdString(rule.sRange).split("~");
        check->setStartPos(lstRange.first().toInt());
        check->setEndPos(lstRange.last().toInt());
    }

    return true;
}

/**
 * @brief SqlParserData::parseItemFrameCode
 * @param rule
 * @param frameCode
 * @return
 */
bool SqlParserData::parseItemFrameCode(const stTableRules &rule,
                                       const Icd::FrameCodeItemPtr &frameCode) const
{
    if (0 == frameCode) {
        return false;
    }

    // frameCodeType attribute
    switch ((int)rule.dScale) {
    case GlobalDefine::frame8:
        frameCode->setFrameCodeType(Icd::FrameCodeU8);
        break;
    case GlobalDefine::frame16:
        frameCode->setFrameCodeType(Icd::FrameCodeU16);
        break;
    case GlobalDefine::frame32:
        frameCode->setFrameCodeType(Icd::FrameCodeU32);
        break;
    case GlobalDefine::frame64:
        frameCode->setFrameCodeType(Icd::FrameCodeU64);
        break;
    default:
        break;
    }

    // frameId
    frameCode->setFrameId(rule.sDefault);

    return true;
}

/**
 * @brief SqlParserData::parseItemNumeric
 * @param rule
 * @param numeric
 * @return
 */
bool SqlParserData::parseItemNumeric(const stTableRules &rule,
                                     const Icd::NumericItemPtr &numeric) const
{
    if (0 == numeric) {
        return false;
    }

    // numericType attribute
    switch (rule.uType) {
    case GlobalDefine::dtU8:
        numeric->setNumericType(Icd::NumericUint8);
        break;
    case GlobalDefine::dt8:
        numeric->setNumericType(Icd::NumericInt8);
        break;
    case GlobalDefine::dtU16:
        numeric->setNumericType(Icd::NumericUint16);
        break;
    case GlobalDefine::dt16:
        numeric->setNumericType(Icd::NumericInt16);
        break;
    case GlobalDefine::dtU32:
        numeric->setNumericType(Icd::NumericUint32);
        break;
    case GlobalDefine::dt32:
        numeric->setNumericType(Icd::NumericInt32);
        break;
        /*case GlobalDefine::dtU64:
        numeric->setNumericType(Icd::NumericUint64);
        break;
    case GlobalDefine::dt64:
        numeric->setNumericType(Icd::NumericInt64);
        break;*/
    case GlobalDefine::dtF32:
        numeric->setNumericType(Icd::NumericFloat32);
        break;
    case GlobalDefine::dtF64:
        numeric->setNumericType(Icd::NumericFloat64);
        break;
    default:
        break;
    }

    // value attribute
    numeric->setDefaultValue(QString::fromStdString(rule.sDefault).toDouble());

    // scale attribute
    numeric->setScale(rule.dScale);

    // offset attribute
    numeric->setOffset(rule.dOffset);

    // limit attribute
    if (!rule.sRange.empty()) {
        QStringList lstRange = QString::fromStdString(rule.sRange).split("~");
        Icd::LimitItemPtr limit = numeric->limit();
        if (lstRange.first().isEmpty()) {
            limit->setLeftInf(true);
        } else {
            limit->setMinimum(lstRange.first().toDouble());
            limit->setLeftInf(false);
        }
        if (lstRange.last().isEmpty()) {
            limit->setRightInf(true);
        } else {
            limit->setMaximum(lstRange.last().toDouble());
            limit->setRightInf(false);
        }
    }

    // unit attribute
    numeric->setUnit(rule.sUnit);

    // specs attribute
    if (!rule.sRule.empty()) {
        QStringList lstValue = QString::fromStdString(rule.sRule)
                .split("#", QString::SkipEmptyParts);
        QStringListIterator it = lstValue;
        while (it.hasNext()) {
            QString item = it.next();
            double key = item.left(item.indexOf("[")).toDouble();
            item = item.left(item.lastIndexOf("]"));
            std::string info = item.mid(item.indexOf("[") + strlen("["))
                    .toStdString();
            numeric->addSpec(key, info);
        }
    }


    return true;
}

/**
 * @brief SqlParserData::parseItemBit
 * @param rule
 * @param bit
 * @return
 */
bool SqlParserData::parseItemBit(const stTableRules &rule,
                                 const Icd::BitItemPtr &bit) const
{
    if (0 == bit) {
        return false;
    }

    // value attribute
    bit->setDefaultValue(atof(rule.sDefault.c_str()));

    QStringList lstValue = QString::fromStdString(rule.sRange).split("#");
    // range attribute

    // start & count attribute
    if (lstValue.size() > 1) {
        QStringList range = lstValue.last().split("~");
        bit->setBitStart(range.first().toInt());
        bit->setBitCount(range.last().toInt() - bit->bitStart() + 1);
    }
    // typeSize attribute
    bit->setTypeSize((bit->bitStart() + bit->bitCount() - 1) / 8 + 1);

    // spec attribute
    if (!rule.sRule.empty()) {
        lstValue = QString::fromStdString(rule.sRule)
                .split("#", QString::SkipEmptyParts);
        QStringListIterator it = lstValue;
        while (it.hasNext()) {
            QString item = it.next();
            int key = item.left(item.indexOf("[")).toInt();
            item = item.left(item.lastIndexOf("]"));
            std::string info = item.mid(item.indexOf("[") + strlen("["))
                    .toStdString();
            bit->addSpec(key, info);
        }
    }

    return true;
}

/**
 * @brief SqlParserData::parseItemComplex
 * @param rule
 * @param complex
 * @return
 */
bool SqlParserData::parseItemComplex(const stTableRules &rule,
                                     const Icd::ComplexItemPtr &complex,
                                     const DMSpace::_vectorIcdTR &tableRules,
                                     int deep) const
{
    if (0 == complex) {
        return false;
    }

    // bufferSize
    if (GlobalDefine::dtBuffer == rule.uType) {
        complex->setBufferSize(rule.dOffset);
    } else {
        complex->setBufferSize(rule.uLength);
    }

    if (GlobalDefine::dtBuffer == rule.uType) {
        Icd::TablePtr table = complex->table();
        if (table) {
            // id attribute
            if (rule.sRule.empty()) {
                table->setId(QUuid::createUuid().toString().remove(QRegExp("[{}-]"))
                        .prepend("ICDTable_").toStdString());
            } else {
                table->setId(rule.sRule);
            }
            // name attribute
            table->setName(rule.sName);
            // mark attribute
            table->setMark(rule.sPrgCode);
            // sequence attribute
            table->setSequence(1);
            // desc attribute
            table->setDesc(rule.sRemark);
        }

        return true;
    } else {
        // parse table
        return parseTable(rule.sRule, complex->table(), tableRules, deep);
    }
}

/**
 * @brief SqlParserData::parseItemFrame
 * @param rule
 * @param frame
 * @return
 */
bool SqlParserData::parseItemFrame(const stTableRules &rule,
                                   const Icd::FrameItemPtr &frame,
                                   const DMSpace::_vectorIcdTR &tableRules,
                                   int deep) const
{
    if (0 == frame) {
        return false;
    }

    // bufferSize
    frame->setBufferSize(rule.uLength);

    QStringList lstTable = QString::fromStdString(rule.sRule)
            .split("@", QString::SkipEmptyParts);
    QStringListIterator it = lstTable;

    // sequenceCount
    frame->setSequenceCount((int)rule.dOffset);

    stICDBase base;
    DMSpace::pairIcdTR icdTR;
    while (it.hasNext()) {
        base.sName = it.next().toStdString();
        // find base
        icdTR = singleIcdTR(tableRules, base);
        if (icdTR.first.sName.empty()) {
            qDebug() << "find:" << QString::fromStdString(base.sName);
            return false;
        }
        base = icdTR.first;
        // parse table
        Icd::TablePtr table(new Icd::Table());
        if (!parseTable(base.sName, table, tableRules, deep)) {
            qDebug() << "parseTable:" << QString::fromStdString(base.sName);
            return false;
        }

        //
        frame->addTable(table);
    }

    return true;
}

/**
 * @brief SqlParserData::parseTable
 * @param base
 * @param table
 * @param deep
 * @return
 */
bool SqlParserData::parseTable(const std::string &tableId,
                               const Icd::TablePtr &table,
                               const DMSpace::_vectorIcdTR &tableRules,
                               int deep) const
{
    if (0 == table) {
        return false;
    }

    // icdbase
    stICDBase base;
    base.sName = tableId;
    DMSpace::pairIcdTR icdTR = singleIcdTR(tableRules, base);
    if (icdTR.first.sName.empty()) {
        return false;
    }
    base = icdTR.first;

    // id attribute
    table->setId(base.sName);

    // name attribute
    table->setName(base.sDescribe);

    // mark attribute
    table->setMark(base.sCode);

    // sequence attribute
    QStringList lstData = QString::fromStdString(base.sRemark).split("##");
    if (lstData.size() > 1) {
        table->setSequence(lstData.last().toInt());
    }

    // desc attribute
    table->setDesc(lstData.first().toStdString());

    if (deep <= Icd::ObjectTable) {
        return true;
    }

    // read rules
    const std::vector<stTableRules> &rules = icdTR.second;
    // parse item
    const int count = rules.size();
    for (int i = 0; i < count; ++i) {
        Icd::ItemPtr item = 0;
        const stTableRules &rule = rules[i];
        if (!parseItem(rule, item, tableRules, deep)) {
            QString error = QStringLiteral("解析规则数据失败！")
                    + QStringLiteral("（表：[%1]；数据定义：[%2]）")
                    .arg(base.sName.c_str())
                    .arg(rule.sName.c_str());
            qWarning() << error;
            return false;
        }
        item->setParent(table.get());
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
 * @brief SqlParserData::parseSystem
 * @param planeID
 * @param systemBase
 * @param system
 * @param deep
 * @return
 */
bool SqlParserData::parseSystem(const std::string &planeID,
                                const stSystem &systemBase,
                                const Icd::SystemPtr &system,
                                const DMSpace::_vectorIcdTR &tableRules,
                                int deep) const
{
    if (0 == system) {
        return false;
    }
    // system base

    // id attribute
    system->setId(QString::number(systemBase.nCode).toStdString());

    // name attribute
    system->setName(systemBase.sName);

    // mark attribute
    system->setMark(systemBase.sSign);

    // desc attribute
    system->setDesc(systemBase.sDescribe);

    //
    if (deep <= Icd::ObjectSystem) {
        return true;
    }

    // parse table
    QString group = QString("%1/%2")
            .arg(planeID.c_str()).arg(systemBase.nCode);
    DMSpace::_vectorIcdTR_Cit it = tableRules.begin();
    for (; it != tableRules.end(); ++it) {
        const stICDBase &base = it->first;

        if (!base.sParentName.empty()
                || base.sGroup != group.toStdString()) {
            continue;   // skip sub tables and other group
        }
        Icd::TablePtr table(new Icd::Table(system.get()));
        if (!parseTable(base.sName, table, tableRules, deep)) {
            return false;
        }
        // save
        system->appendTable(table);
    }

    return true;
}

/**
 * @brief SqlParserData::parseVehicle
 * @param planeID
 * @param vehicle
 * @param deep
 * @return
 */
bool SqlParserData::parseVehicle(const std::string &planeID,
                                 const Icd::VehiclePtr &vehicle,
                                 const DMSpace::_vectorPS &plane_system,
                                 const DMSpace::_vectorIcdTR &tableRules,
                                 int deep) const
{
    if (0 == vehicle) {
        return false;
    }
    stPlane plane;
    plane.nCode = atoi(planeID.c_str());
    DMSpace::pairPS ps = singlePS(plane_system, plane);
    // find plane base
    if (-1 == ps.first.nCode) {
        return false;
    }
    plane = ps.first;

    // id attribute
    vehicle->setId(QString::number(plane.nCode).toStdString());

    // name attribute
    vehicle->setName(plane.sName);

    // mark attribute
    vehicle->setMark(plane.sSign);

    // desc attribute
    vehicle->setDesc(plane.sDescribe);

    //
    if (deep <= Icd::ObjectVehicle) {
        return true;
    }

    const std::vector<stSystem> &systems = ps.second;
    DMSpace::_vectorIcdTR_Cit itR = tableRules.end();
    std::string group;
    const int count = systems.size();
    for (int i = 0; i < count; ++i) {
        DMSpace::_vectorIcdTR rules;
        const stSystem &single = systems[i];
        Icd::SystemPtr system(new Icd::System(vehicle.get()));
        // group
        group = QString("%1/%2")
                .arg(planeID.c_str()).arg(single.nCode).toStdString();

        // generate rules
        if (deep >= Icd::ObjectTable) {
            for (itR = tableRules.begin(); itR != tableRules.end(); ++itR) {
                const stICDBase &base = itR->first;
                if (base.sGroup == group) {
                    rules.push_back(std::make_pair(base, itR->second));
                }
            }
        }

        // parse system
        if (!parseSystem(planeID, single, system, rules, deep)) {
            return false;
        }

        // save
        vehicle->appendSystem(system);
    }

    return true;
}

/**
 * @brief SqlParserData::parseVehicle
 * @param vehicles
 * @param deep
 * @return
 */
bool SqlParserData::parseVehicle(Icd::VehiclePtrArray &vehicles, int deep)
{
    std::vector<stPlane> planes;
    // read vehicle base
    if (!db->readPlane(planes)) {
        return false;
    }

    DMSpace::_vectorPS plane_system;
    DMSpace::_vectorIcdTR rules;
    const int count = planes.size();
    if (deep >= Icd::ObjectSystem) {
        // read system base
        std::map<int, std::vector<stSystem>> allSystem;
        if (!db->readSystem(allSystem)) {
            return false;
        }
        std::map<int, std::vector<stSystem>>::iterator it = allSystem.end();
        for (int i = 0; i < count; ++i) {
            const stPlane &plane = planes[i];
            if ((it = allSystem.find(plane.nCode)) != allSystem.end()) {
                plane_system.push_back(std::make_pair(plane, it->second));
            } else {
                plane_system.push_back(std::make_pair(plane,
                                                      std::vector<stSystem>()));
            }
        }

        // read table-rules
        if (deep >= Icd::ObjectTable) {   // at least table node
            if (deep >= Icd::ObjectItem) {  // deep to rule
                rules = tableRules(std::string(), std::string());
            } else {    // deep to table
                std::vector<stICDBase> tables
                        = icdBaseArray(std::string(), std::string(), true);
                const int count = tables.size();
                for (int i = 0; i < count; ++i) {
                    rules.push_back(std::make_pair(tables[i],
                                                   std::vector<stTableRules>()));
                }
            }
        }
    } else {
        for (int i = 0; i < count; ++i) {
            plane_system.push_back(std::make_pair(planes[i],
                                                  std::vector<stSystem>()));
        }
    }

    // parse
    std::string planeID;
    for (int i = 0; i < count; ++i) {
        planeID = QString::number(planes[i].nCode).toStdString();
        Icd::VehiclePtr vehicle(new Icd::Vehicle());
        if (!parseVehicle(planeID, vehicle, plane_system, rules, deep)) {
            return false;
        }

        // save
        vehicles.push_back(vehicle);
    }

    return true;
}
// for parse end

} // end of namespace Icd
