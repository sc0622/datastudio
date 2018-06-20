#include "precomp.h"
#include "icdparser_sql_p.h"

namespace Icd {

// for convert begin
/**
 * @brief SqlParserData::object2rule
 * @param object
 * @param rule
 * @return
 */
bool SqlParserData::object2rule(const Icd::ObjectPtr &object,
                                stTableRules &rule) const
{
    if (0 == object) {
        return false;
    }

    // id attribute
    rule.nSerial = QString::fromStdString(object->id()).toInt();

    // name attribute
    rule.sName = object->name();

    // mark attribute
    rule.sPrgCode = object->mark();

    // desc attribute
    rule.sRemark = object->desc();

    return true;
}

/**
 * @brief SqlParserData::Item2rule
 * @param item
 * @param rule
 * @return
 */
bool SqlParserData::Item2rule(const Icd::ItemPtr &item, stTableRules &rule) const
{
    bool result = false;
    if (0 == item) {
        return result;
    }

    // object attribute
    if (!object2rule(JHandlePtrCast<Icd::Object, Icd::Item>(item), rule)) {
        return result;
    }

    // other attribute
    switch (item->type()) {
    case Icd::ItemHead:
        result = ItemHead2rule(JHandlePtrCast<Icd::HeaderItem, Icd::Item>(item),
                               rule);
        // type attribute
        rule.uType = GlobalDefine::dtHead;
        break;
    case Icd::ItemCounter:
        result = ItemCounter2rule(JHandlePtrCast<Icd::CounterItem, Icd::Item>(item), rule);
        // type attribute
        rule.uType = GlobalDefine::dtCounter;
        break;
    case Icd::ItemCheck:
        result = ItemCheck2rule(JHandlePtrCast<Icd::CheckItem, Icd::Item>(item), rule);
        // type attribute
        rule.uType = GlobalDefine::dtCheck;
        break;
    case Icd::ItemFrameCode:
        result = ItemFrameCode2rule(JHandlePtrCast<Icd::FrameCodeItem, Icd::Item>(item), rule);
        // type attribute
        rule.uType = GlobalDefine::dtFrameCode;
        break;
    case Icd::ItemNumeric:
        result = ItemNumeric2rule(JHandlePtrCast<Icd::NumericItem, Icd::Item>(item), rule);
        break;
    case Icd::ItemArray:
        result = ItemArray2rule(JHandlePtrCast<Icd::ArrayItem, Icd::Item>(item), rule);
        break;
    case Icd::ItemBitMap:
        result = ItemBit2rule(JHandlePtrCast<Icd::BitItem, Icd::Item>(item), rule);
        // type attribute
        rule.uType = GlobalDefine::dtBitMap;
        break;
    case Icd::ItemBitValue:
        result = ItemBit2rule(JHandlePtrCast<Icd::BitItem, Icd::Item>(item), rule);
        // type attribute
        rule.uType = GlobalDefine::dtBitValue;
        break;
    case Icd::ItemComplex:
    {
        Icd::ComplexItemPtr complex = JHandlePtrCast<Icd::ComplexItem, Icd::Item>(item);
        if (!complex) {
            break;
        }
        result = ItemComplex2rule(complex, rule);
        // type attribute
        if (complex->table()->allItem().empty()) {
            rule.uType = GlobalDefine::dtBuffer;
            rule.dOffset = complex->bufferSize();
        } else {
            rule.uType = GlobalDefine::dtComplex;
        }
    }
        break;
    case Icd::ItemFrame:
        result = ItemFrame2rule(JHandlePtrCast<Icd::FrameItem, Icd::Item>(item), rule);
        // type attribute
        rule.uType = GlobalDefine::dtDiscern;
        break;
    default:
        break;
    }

    return result;
}

/**
 * @brief SqlParserData::Item2TableRules
 * @param item
 * @param rules
 * @return
 */
bool SqlParserData::Item2TableRules(const Icd::ItemPtr &item,
                                    const std::string &group,
                                    const std::string &parent,
                                    DMSpace::_vectorIcdTR &rules) const
{
    bool result = true;
    if (0 == item) {
        return false;
    }

    switch (item->type()) {
    case Icd::ItemFrameCode:
        result = ItemFrameCode2TableRules(JHandlePtrCast<Icd::FrameCodeItem, Icd::Item>(item),
                                          group,
                                          parent,
                                          rules);
        break;
    case Icd::ItemComplex:
        result = ItemComplex2TableRules(JHandlePtrCast<Icd::ComplexItem, Icd::Item>(item),
                                        group,
                                        parent,
                                        rules);
        break;
    case Icd::ItemFrame:
        result = ItemFrame2TableRules(JHandlePtrCast<Icd::FrameItem, Icd::Item>(item),
                                      group,
                                      parent,
                                      rules);
        break;
    default:
        break;
    }

    return result;
}

/**
 * @brief SqlParserData::ItemHead2rule
 * @param head
 * @param rule
 * @return
 */
bool SqlParserData::ItemHead2rule(const Icd::HeaderItemPtr &head,
                                  stTableRules &rule) const
{
    if (0 == head) {
        return false;
    }

    // value attribute
    rule.sDefault = QString::number((uint)head->defaultValue(), 16)
            .insert(0, "0x").toStdString();

    return true;
}

bool SqlParserData::ItemCounter2rule(const Icd::CounterItemPtr &counter,
                                     stTableRules &rule) const
{
    if (0 == counter) {
        return false;
    }

    // counterType attribute
    switch (counter->counterType())
    {
    case Icd::CounterU8:
        rule.sDefault = QString::number(GlobalDefine::counterU8).toStdString();
        break;
    case Icd::CounterU16:
        rule.sDefault = QString::number(GlobalDefine::counterU16).toStdString();
        break;
    case Icd::CounterU32:
        rule.sDefault = QString::number(GlobalDefine::counterU32).toStdString();
        break;
    case Icd::CounterU64:
        rule.sDefault = QString::number(GlobalDefine::counterU64).toStdString();
        break;
    default:
        break;
    }
    // length attribute
    rule.uLength = (unsigned short)counter->bufferSize();

    return true;
}

/**
 * @brief SqlParserData::ItemCheck2rule
 * @param check
 * @param rule
 * @return
 */
bool SqlParserData::ItemCheck2rule(const Icd::CheckItemPtr &check,
                                   stTableRules &rule) const
{
    if (0 == check) {
        return false;
    }

    // checkType attribute
    switch (check->checkType()) {
    case Icd::CheckNone:
        rule.sDefault = QString::number(GlobalDefine::ctInvalid).toStdString();
        break;
    case Icd::CheckSum8:
        rule.sDefault = QString::number(GlobalDefine::ct8And).toStdString();
        break;
    case Icd::CheckSum16:
        rule.sDefault = QString::number(GlobalDefine::ct16And).toStdString();
        break;
    case Icd::CheckCrc8:
        rule.sDefault = QString::number(GlobalDefine::ctCRC8).toStdString();
        break;
    case Icd::CheckCrc16:
        rule.sDefault = QString::number(GlobalDefine::ctCRC16).toStdString();
        break;
    case Icd::CheckXor8:
        rule.sDefault = QString::number(GlobalDefine::ctXor8).toStdString();
        break;
    case Icd::CheckXor16:
        rule.sDefault = QString::number(GlobalDefine::ctXor16).toStdString();
        break;
    default:
        break;
    }

    // length attribute
    rule.uLength = (unsigned short)check->bufferSize();

    // check range attribute
    rule.sRange = QString("%1~%2").arg(check->startPos())
            .arg(check->endPos()).toStdString();

    return true;
}

/**
 * @brief SqlParserData::ItemFrameCode2rule
 * @param frameCode
 * @param rule
 * @return
 */
bool SqlParserData::ItemFrameCode2rule(const Icd::FrameCodeItemPtr &frameCode,
                                       stTableRules &rule) const
{
    if (0 == frameCode) {
        return false;
    }
    // frame attribute
    Icd::FrameItemPtr frame = frameCode->frame();
    if (frame) {
        rule.sDefault = frame->id();
    }

    // frame type attribute
    switch(frameCode->frameCodeType()) {
    case Icd::FrameCodeU8:
        rule.dScale = GlobalDefine::frame8;
        break;
    case Icd::FrameCodeU16:
        rule.dScale = GlobalDefine::frame16;
        break;
    case Icd::FrameCodeU32:
        rule.dScale = GlobalDefine::frame32;
        break;
    case Icd::FrameCodeU64:
        rule.dScale = GlobalDefine::frame64;
        break;
    default:
        break;
    }

    // length attribute
    rule.uLength = (unsigned short)frameCode->bufferSize();

    return true;
}

/**
 * @brief SqlParserData::ItemFrameCode2TableRules
 * @param frameCode
 * @param rules
 * @return
 */
bool SqlParserData::ItemFrameCode2TableRules(const Icd::FrameCodeItemPtr &frameCode,
                                             const std::string &group,
                                             const std::string &parent,
                                             DMSpace::_vectorIcdTR &rules) const
{
    Q_UNUSED(frameCode);
    Q_UNUSED(group);
    Q_UNUSED(parent);
    Q_UNUSED(rules);
    if (0 == frameCode) {
        return false;
    }

    // frameId attribute

    return true;
}

/**
 * @brief SqlParserData::ItemNumeric2rule
 * @param numeric
 * @param rule
 * @return
 */
bool SqlParserData::ItemNumeric2rule(const Icd::NumericItemPtr &numeric,
                                     stTableRules &rule) const
{
    if (0 == numeric) {
        return false;
    }

    // numericType attribute
    switch (numeric->numericType()) {
    case Icd::NumericInt8: rule.uType = GlobalDefine::dt8; break;
    case Icd::NumericUint8: rule.uType = GlobalDefine::dtU8; break;
    case Icd::NumericInt16: rule.uType = GlobalDefine::dt16; break;
    case Icd::NumericUint16: rule.uType = GlobalDefine::dtU16; break;
    case Icd::NumericInt32: rule.uType = GlobalDefine::dt32; break;
    case Icd::NumericUint32: rule.uType = GlobalDefine::dtU32; break;
    case Icd::NumericInt64: rule.uType = GlobalDefine::dt64; break;
    case Icd::NumericUint64: rule.uType = GlobalDefine::dtU64; break;
    case Icd::NumericFloat32: rule.uType = GlobalDefine::dtF32; break;
    case Icd::NumericFloat64: rule.uType = GlobalDefine::dtF64; break;
    default: return false;
    }

    // length attribute
    rule.uLength = (unsigned short)numeric->bufferSize();

    // value attribute
    rule.sDefault = QString::number(numeric->defaultValue()).toStdString();

    // scale attribute
    rule.dScale = numeric->scale();

    // offset attribute
    rule.dOffset = numeric->offset();

    // limit attribute
    const Icd::LimitItemPtr limit = numeric->limit();
    if (limit) {
        rule.sRange = QString("%1~%2")
                .arg(limit->leftInf() ?
                         "" : QString::number(limit->minimum()))
                .arg(limit->rightInf() ?
                         "" : QString::number(limit->maximum()))
                .toStdString();
    } else {
        rule.sRange = "-Inf~+Inf";
    }

    // unit attribute
    rule.sUnit = numeric->unit();

    // specs attribute
    std::unordered_map<double, std::string> values = numeric->specs();
    std::unordered_map<double, std::string>::iterator it = values.begin();
    QString set;
    for (; it != values.end(); ++it) {
        set += QString("%1[%2]#").arg(it->first).arg(it->second.c_str());
    }
    rule.sRule = set.left(set.lastIndexOf("#")).toStdString();

    return true;
}

bool SqlParserData::ItemArray2rule(const ArrayItemPtr &array, stTableRules &rule) const
{
    if (!array) {
        return false;
    }

    // arrayType attribute
    switch (array->arrayType()) {
    case Icd::Int8Array:
        rule.sDefault = QString::number(GlobalDefine::Int8Array).toStdString();
        break;
    case Icd::UInt8Array:
        rule.sDefault = QString::number(GlobalDefine::UInt8Array).toStdString();
        break;
    case Icd::Int16Array:
        rule.sDefault = QString::number(GlobalDefine::Int16Array).toStdString();
        break;
    case Icd::UInt16Array:
        rule.sDefault = QString::number(GlobalDefine::UInt16Array).toStdString();
        break;
    case Icd::Int32Array:
        rule.sDefault = QString::number(GlobalDefine::Int32Array).toStdString();
        break;
    case Icd::UInt32Array:
        rule.sDefault = QString::number(GlobalDefine::UInt32Array).toStdString();
        break;
    case Icd::Int64Array:
        rule.sDefault = QString::number(GlobalDefine::Int64Array).toStdString();
        break;
    case Icd::UInt64Array:
        rule.sDefault = QString::number(GlobalDefine::UInt64Array).toStdString();
        break;
    case Icd::Float32Array:
        rule.sDefault = QString::number(GlobalDefine::Float32Array).toStdString();
        break;
    case Icd::Float64Array:
        rule.sDefault = QString::number(GlobalDefine::Float64Array).toStdString();
        break;
    default:
        break;
    }
    // length attribute
    rule.uLength = (unsigned short)array->bufferSize();

    return true;
}

/**
 * @brief SqlParserData::ItemBit2rule
 * @param bit
 * @param rule
 * @return
 */
bool SqlParserData::ItemBit2rule(const Icd::BitItemPtr &bit,
                                 stTableRules &rule) const
{
    if (0 == bit) {
        return false;
    }
    // value attribute
    rule.sDefault = QString::number(bit->defaultValue()).toStdString();

    // range attribute

    // start & count attribute
    rule.sRange = QString("#%1~%2").arg(bit->bitStart())
            .arg(bit->bitStart() + bit->bitCount() - 1).toStdString();

    // length attribute
    rule.uLength = bit->bitCount();

    // code attribute
    rule.nCode = bit->bitStart() / 8;

    // spec attribute
    std::unordered_map<icd_uint64, std::string> values = bit->specs();
    std::unordered_map<icd_uint64, std::string>::iterator it = values.begin();
    QString set;
    for (; it != values.end(); ++it) {
        set += QString("%1[%2]#").arg(it->first).arg(it->second.c_str());
    }
    rule.sRule = set.left(set.lastIndexOf("#")).toStdString();

    return true;
}

/**
 * @brief SqlParserData::ItemComplex2rule
 * @param complex
 * @param rule
 * @return
 */
bool SqlParserData::ItemComplex2rule(const Icd::ComplexItemPtr &complex,
                                     stTableRules &rule) const
{
    if (0 == complex) {
        return false;
    }

    // length attribute
    rule.uLength = (unsigned short)complex->bufferSize();

    // rule attribute
    Icd::TablePtr table = complex->table();
    if (table) {
        rule.sRule = table->id();
    }

    return true;
}

bool SqlParserData::ItemComplex2TableRules(const Icd::ComplexItemPtr &complex,
                                           const std::string &group,
                                           const std::string &parent,
                                           DMSpace::_vectorIcdTR &rules) const
{
    if (0 == complex) {
        return false;
    }
    rules.clear();
    DMSpace::_vectorIcdTR subTables;

    // rules
    Icd::TablePtr table = complex->table();
    if (table) {
        // icdbase and rules
        if (!table2TableRules(table, group, parent, subTables)) {
            return false;
        }

        // save
        if (!subTables.empty()) {
            rules.insert(rules.end(), subTables.begin(), subTables.end());
        }
    }

    return true;
}

/**
 * @brief SqlParserData::ItemFrame2rule
 * @param frame
 * @param rule
 * @return
 */
bool SqlParserData::ItemFrame2rule(const Icd::FrameItemPtr &frame,
                                   stTableRules &rule) const
{
    if (0 == frame) {
        return false;
    }

    // length attribute
    rule.uLength = (unsigned short)frame->bufferSize();

    // sequence
    rule.dOffset = (double)frame->sequenceCount();

    // rule attribute
    Icd::TablePtrMap tables = frame->allTable();
    if (!tables.empty()) {
        Icd::TablePtrMap::iterator it = tables.begin();
        Icd::TablePtr table = it->second;

        // rule attribute
        QString names;
        for (; it != tables.end(); ++it) {
            if (table = it->second) {
                names += QString("%1@").arg(table->id().c_str());
            }
        }
        rule.sRule = names.left(names.lastIndexOf("@")).toStdString();
    }

    return true;
}

/**
 * @brief SqlParserData::ItemFrame2TableRules
 * @param frame
 * @param rules
 * @return
 */
bool SqlParserData::ItemFrame2TableRules(const Icd::FrameItemPtr &frame,
                                         const std::string &group,
                                         const std::string &parent,
                                         DMSpace::_vectorIcdTR &rules) const
{
    if (0 == frame) {
        return false;
    }
    rules.clear();

    Icd::TablePtrMap tables = frame->allTable();
    if (!tables.empty()) {
        // length attribute
        Icd::TablePtrMap::iterator it = tables.begin();
        DMSpace::_vectorIcdTR subTables;
        Icd::TablePtr table = 0;
        for (; it != tables.end(); ++it) {
            if (table = it->second) {
                // rules
                if (!table2TableRules(table, group, parent, subTables)) {
                    return false;
                }

                // save
                rules.insert(rules.end(), subTables.begin(), subTables.end());
                subTables.clear();
            }
        }
    }

    return true;
}

bool SqlParserData::table2TableRules(const Icd::TablePtr &table,
                                     const std::string &group,
                                     const std::string &parent,
                                     DMSpace::_vectorIcdTR &rules) const
{
    if (0 == table) {
        return false;
    }
    stICDBase base;
    std::vector<stTableRules> tableRules;
    DMSpace::_vectorIcdTR subTables;

    // name attribute
    base.sName = table->id();

    // id attribute
    base.sID = base.sName.substr(base.sName.find("_") + strlen("_"));

    // code attribute
    base.sCode = table->mark();

    // parent attribute
    base.sParentName = parent;

    // describe attribute
    base.sDescribe = table->name();

    // group attribute
    base.sGroup = group;

    // remark attribute
    base.sRemark = QString("%1##%2")
            .arg(table->desc().c_str()).arg(table->sequence()).toStdString();

    // length attribute
    Icd::FrameItem *frame
            = dynamic_cast<Icd::FrameItem *>(table->parent());
    Icd::ComplexItem *complex
            = dynamic_cast<Icd::ComplexItem *>(table->parent());
    if (frame) {
        base.nLength = qCeil(frame->bufferSize());
    } else if (complex) {
        base.nLength = qCeil(complex->bufferSize());
    } else {
        base.nLength = qCeil(table->bufferSize());
    }

    int itemLength = 0;
    // rules
    Icd::ItemPtrArray items = table->allItem();
    const int count = items.size();
    Icd::ItemPtr item = 0;
    stTableRules *previous = 0;
    Icd::BitItemPtr preBit = 0;
    Icd::BitItemPtr bit = 0;
    for (int i = 0; i < count; ++i) {
        if (item = items[i]) {
            // rules
            stTableRules rule;
            if (!Item2rule(item, rule)) {
                return false;
            }

            // code attribute
            if (previous) {
                if (GlobalDefine::dtBitMap == previous->uType
                        || GlobalDefine::dtBitValue == previous->uType) {
                    if (GlobalDefine::dtBitMap == rule.uType
                            || GlobalDefine::dtBitValue == rule.uType) {
                        bit = JHandlePtrCast<Icd::BitItem, Icd::Item>(item);
                        rule.nCode = previous->nCode + bit->bitStart() / 8;
                        if (bit->bitStart() < (preBit->bitStart() + preBit->bitCount())) {
                            // can not merged,need plus byte offset
                            rule.nCode += qCeil((qreal)preBit->bitCount() / 8.0);
                        }
                    } else {
                        rule.nCode = previous->nCode + qCeil((qreal)preBit->bitCount() / 8.0);
                    }
                } else {
                    rule.nCode = previous->nCode + previous->uLength;
                    if (GlobalDefine::dtBitMap == rule.uType
                            || GlobalDefine::dtBitValue == rule.uType) {
                        bit = JHandlePtrCast<Icd::BitItem, Icd::Item>(item);
                        // need plus byte offset
                        rule.nCode += bit->bitStart() / 8;
                    }
                }
            } else {
                rule.nCode = 0;
                if (GlobalDefine::dtBitMap == rule.uType
                        || GlobalDefine::dtBitValue == rule.uType) {
                    bit = JHandlePtrCast<Icd::BitItem, Icd::Item>(item);
                    // need plus byte offset
                    rule.nCode += bit->bitStart() / 8;
                }
            }

            // sub tablerules
            if (!Item2TableRules(item, group, base.sName, subTables)) {
                return false;
            }

            // save
            tableRules.push_back(rule);
            if (!subTables.empty()) {
                rules.insert(rules.end(), subTables.begin(), subTables.end());
                subTables.clear();
            }

            // update previous
            previous = &tableRules[tableRules.size() - 1];
            preBit = bit;
        }
    }

    // calculate item length
    if (previous) {
        if (GlobalDefine::dtBitMap == previous->uType
                || GlobalDefine::dtBitValue == previous->uType) {
            itemLength = previous->nCode + previous->uLength / 8;
        } else {
            itemLength = previous->nCode + previous->uLength;
        }
    }

    // take the larger one as table length
    base.nLength = qMax(base.nLength, itemLength);

    // save
    rules.push_back(std::make_pair(base, tableRules));

    return true;
}

/**
 * @brief SqlParserData::system2Base
 * @param system
 * @param base
 * @return
 */
bool SqlParserData::system2Base(const Icd::SystemPtr &system, stSystem &base) const
{
    if (0 == system) {
        return false;
    }

    // id attribute
    base.nCode = QString::fromStdString(system->id()).toInt();

    // name attribute
    base.sName = system->name();

    // mark attribute
    base.sSign = system->mark();

    // desc attribute
    base.sDescribe = system->desc();

    return true;
}

/**
 * @brief SqlParserData::vehicle2Base
 * @param vehicle
 * @param base
 * @return
 */
bool SqlParserData::vehicle2Base(const Icd::VehiclePtr &vehicle, stPlane &base) const
{
    if (0 == vehicle) {
        return false;
    }

    // id attribute
    base.nCode = QString::fromStdString(vehicle->id()).toInt();

    // name attribute
    base.sName = vehicle->name();

    // mark attribute
    base.sSign = vehicle->mark();

    // desc attribute
    base.sDescribe = vehicle->desc();

    return true;
}

/**
 * @brief SqlParserData::saveData
 * @param infrastructure
 * @param tableRules
 * @return
 */
bool SqlParserData::saveData(const DMSpace::_vectorPS &infrastructure,
                             const DMSpace::_vectorIcdTR &tableRules) const
{
    return db->savePlanes(infrastructure, tableRules);
}
// for convert end

} // end of namespace Icd
