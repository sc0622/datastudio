#include "icdfactory.h"
#include "planenode.h"
#include "systemnode.h"
#include "tablenode.h"
#include "icdbitdata.h"
#include "icdcomplexdata.h"
#include "icdcustomizeddata.h"
#include "icdcheckdata.h"
#include "icdcounterdata.h"
#include "icdframecodedata.h"

/**
 * @brief 获取工厂访问实例
 * @return 工厂访问实例
 */
ICDFactory &ICDFactory::instance()
{
    static ICDFactory ins;

    return ins;
}

/**
* @brief 克隆数据
* @return 数据
*/
ICDElement::smtElement ICDFactory::clone(const ICDElement::smtElement &elemet)
{
    if (!elemet) {
        return ICDElement::smtElement();
    }
    switch (elemet->objectType()) {
    case GlobalDefine::ntPlane: {
            PlaneNode::smtPlane plane = SMT_CONVERT(PlaneNode, elemet);
            return plane->clone();
        }
        break;
    case GlobalDefine::ntSystem: {
            SystemNode::smtSystem system = SMT_CONVERT(SystemNode, elemet);
            return system->clone();
        }
        break;
    case GlobalDefine::ntTable: {
            TableNode::smtTable table = SMT_CONVERT(TableNode, elemet);
            return table->clone();
        }
        break;
    case GlobalDefine::ntRule: {
            ICDMetaData::smtMeta meta = SMT_CONVERT(ICDMetaData, elemet);
            return meta->clone();
        }
        break;
    default :
        return elemet->clone();
        break;
    }
}

/**
 * @brief 创建ICD数据元素
 * @param [in] rule : 规则数据
 * @return 数据元素
 */
ICDMetaData::smtMeta ICDFactory::CreatObject(const stTableRules &rule)
{
    ICDMetaData::smtMeta result;
    if (!(rule.uType >= GlobalDefine::dtHead && rule.uType < GlobalDefine::dtTotal)) {
        return result;
    }

    switch (rule.uType) {
    case GlobalDefine::dtCounter:   // 帧计数
        result = ICDMetaData::smtMeta(new ICDCounterData(rule));
        break;
    case GlobalDefine::dtCheck:    // 校验
        result = ICDMetaData::smtMeta(new ICDCheckData(rule));
        break;
    case GlobalDefine::dtFrameCode: // 帧识别码
        result = ICDMetaData::smtMeta(new ICDFrameCodeData(rule));
        break;
    case GlobalDefine::dtHead:      // 包头
    case GlobalDefine::dtU8:        // 无符号8位
    case GlobalDefine::dt8:         // 有符号8位
    case GlobalDefine::dtU16:       // 无符号16位
    case GlobalDefine::dt16:        // 有符号16位
    case GlobalDefine::dtU32:       // 无符号32位
    case GlobalDefine::dt32:        // 有符号32位
    case GlobalDefine::dtF32:       // 32位浮点数
    case GlobalDefine::dtF64:       // 64位浮点数
        result = ICDMetaData::smtMeta(new ICDCommonData(rule));
        break;
    case GlobalDefine::dtBitMap:    // BITMAP
    case GlobalDefine::dtBitValue:  // BITVALUE
        result = ICDMetaData::smtMeta(new ICDBitData(rule));
        break;
    case GlobalDefine::dtComplex:   // 复合数据
    case GlobalDefine::dtDiscern:   // 帧数据
        result = ICDMetaData::smtMeta(new ICDComplexData(rule));
        break;
    case GlobalDefine::dtBuffer:    // 数据预留区
        result = ICDMetaData::smtMeta(new ICDCustomizedData(rule));
        break;
    default:
        result = ICDMetaData::smtMeta(new ICDMetaData(rule));
        break;
    }
    return result;
}

/**
* @brief 创建ICD数据元素
* @param [in] type : ICD数据类型
* @return 数据元素
*/
ICDMetaData::smtMeta ICDFactory::CreatObject(GlobalDefine::DataType type)
{
    ICDMetaData::smtMeta result;
    stTableRules rule;
    rule.uType = type;
    char szTmp[20] = { 0 };
    switch (type) {
    case GlobalDefine::dtHead:      // 包头
    case GlobalDefine::dtU8:        // 无符号8位
    case GlobalDefine::dt8:         // 有符号8位
        result = ICDMetaData::smtMeta(new ICDCommonData(rule));
        break;
    case GlobalDefine::dtCounter:   // 帧计数
        sprintf_s(szTmp, "%d", GlobalDefine::counterU8);
        rule.sDefault = std::string(szTmp);
        result = ICDMetaData::smtMeta(new ICDCounterData(rule));
        break;
    case GlobalDefine::dtCheck:     // 校验
        sprintf_s(szTmp, "%d", GlobalDefine::ctInvalid);
        rule.sDefault = std::string(szTmp);
        result = ICDMetaData::smtMeta(new ICDCheckData(rule));
        break;
    case GlobalDefine::dtFrameCode: // 帧识别码
        sprintf_s(szTmp, "%d", GlobalDefine::frame8);
        rule.sDefault = std::string(szTmp);
        result = ICDMetaData::smtMeta(new ICDFrameCodeData(rule));
        break;
    case GlobalDefine::dtU16:       // 无符号16位
    case GlobalDefine::dt16:        // 有符号16位
        result = ICDMetaData::smtMeta(new ICDCommonData(rule));
        break;
    case GlobalDefine::dtU32:       // 无符号32位
    case GlobalDefine::dt32:        // 有符号32位
    case GlobalDefine::dtF32:       // 32位浮点数
        result = ICDMetaData::smtMeta(new ICDCommonData(rule));
        break;
    case GlobalDefine::dtF64:       // 64位浮点数
        result = ICDMetaData::smtMeta(new ICDCommonData(rule));
        break;
    case GlobalDefine::dtBitMap:    // BITMAP
    case GlobalDefine::dtBitValue:  // BITVALUE
        result = ICDMetaData::smtMeta(new ICDBitData(rule));
        break;
    case GlobalDefine::dtComplex:   // 复合数据
    case GlobalDefine::dtDiscern:   // 帧数据
        result = ICDMetaData::smtMeta(new ICDComplexData(rule));
        break;
    case GlobalDefine::dtBuffer:    // 数据预留区
        result = ICDMetaData::smtMeta(new ICDCustomizedData(rule));
    default:
        result = ICDMetaData::smtMeta(new ICDMetaData(rule));
        break;
    }
    return result;
}

/**
* @brief 将ICD类数据转换成统一规则结构
* @param [in] meta : ICD类
* @return 规则数据
*/
stTableRules ICDFactory::convert2Rule(const ICDMetaData::smtMeta &meta)
{
    stTableRules result;
    if (!meta) {
        return result;
    }
    // 通用属性
    result.nCode = meta->index();
    result.nSerial = meta->serial();
    result.uLength = meta->length();
    result.uType = meta->type();
    result.sName = meta->name();
    result.sRemark = meta->remark();
    switch (meta->type()) {
    case GlobalDefine::dtCounter:
        {
            ICDCounterData::smtCounter counter = SMT_CONVERT(ICDCounterData, meta);
            if (counter) {
                result.sPrgCode = counter->proCode();
                result.sDefault = counter->stringType();
            }
        }
        break;
    case GlobalDefine::dtCheck:    // 校验
        {
            ICDCheckData::smtCheck check = SMT_CONVERT(ICDCheckData, meta);
            if (check) {
                result.sPrgCode = check->proCode();
                result.sDefault = check->stringType();
                result.sRange = check->checkRange();
            }
        }
        break;
    case GlobalDefine::dtFrameCode: // 帧识别码
        {
            ICDFrameCodeData::smtFrameCode frameCode = SMT_CONVERT(ICDFrameCodeData, meta);
            if (frameCode) {
                result.sPrgCode = frameCode->proCode();
                result.sDefault = frameCode->bindingStringSerial();
                result.dScale = frameCode->frameType();
            }
        }
        break;
    case GlobalDefine::dtHead:     // 包头
    case GlobalDefine::dtU8:       // 无符号8位
    case GlobalDefine::dt8:        // 有符号8位
    case GlobalDefine::dtU16:      // 无符号16位
    case GlobalDefine::dt16:       // 有符号16位
    case GlobalDefine::dtU32:      // 无符号32位
    case GlobalDefine::dt32:       // 有符号32位
    case GlobalDefine::dtF32:      // 32位浮点数
    case GlobalDefine::dtF64:      // 64位浮点数
        {
            ICDCommonData::smtCommon common = SMT_CONVERT(ICDCommonData, meta);
            if (common) {
                result.sPrgCode = common->proCode();
                result.sDefault = common->defaultStr();
                result.sRange = common->range();
                result.dOffset = common->offset();
                result.sUnit = common->unit();
                result.dScale = common->scale();
                result.sRule = common->rule();
            }
        }
        break;
    case GlobalDefine::dtBitMap:   // BITMAP
    case GlobalDefine::dtBitValue: // BITVALUE
        {
            ICDBitData::smtBit bit = SMT_CONVERT(ICDBitData, meta);
            if (bit) {
                result.sPrgCode = bit->proCode();
                result.sDefault = bit->defaultStr();
                result.sRange = bit->range();
                result.dOffset = bit->offset();
                result.dScale = bit->scale();
                result.sRule = bit->rule();
            }
        }
        break;
    case GlobalDefine::dtComplex:  // 复合数据
    case GlobalDefine::dtDiscern:  // 帧数据
        {
            ICDComplexData::smtComplex complex = SMT_CONVERT(ICDComplexData, meta);
            if (complex) {
                result.sPrgCode = complex->proCode();
                result.sDefault = complex->defaultStr();
                result.dOffset = complex->offset();
                result.dScale = complex->scale();
                result.sRule = complex->rule();
            }
        }
        break;
    case GlobalDefine::dtBuffer:    //
        {
            ICDCustomizedData::smtCustom custom = SMT_CONVERT(ICDCustomizedData, meta);
            result.dOffset = custom->offset();
        }
        break;
    }

    return result;
}

bool ICDFactory::copyBaseData(ICDMetaData::smtMeta from, 
                              ICDMetaData::smtMeta to)
{
    if (!from || !to) {
        return false;
    }
    if (from->type() != to->type()) {
        return false;
    }
    to->setName(from->name());
    to->setIndex(from->index());
    to->setSerial(from->serial());
    to->setRemark(from->remark());

    ICDCommonData::smtCommon _comFrom = SMT_CONVERT(ICDCommonData, from);
    ICDCommonData::smtCommon _comTo = SMT_CONVERT(ICDCommonData, to);
    if (!_comFrom || !_comTo) {
        return false;
    }
    _comTo->setProCode(_comFrom->proCode());
    _comTo->setDefaultStr(_comFrom->defaultStr());
    _comTo->setRange(_comFrom->range());
    _comTo->setOffset(_comFrom->offset());
    _comTo->setUnit(_comFrom->unit());
    _comTo->setScale(_comFrom->scale());
    _comTo->setRule(_comFrom->rule());
    _comTo->setValues(_comFrom->values());

    switch (to->type()) {
    case GlobalDefine::dtCounter:
        {
            ICDCounterData::smtCounter _from = SMT_CONVERT(ICDCounterData, from);
            ICDCounterData::smtCounter _to = SMT_CONVERT(ICDCounterData, to);
            if (!_from || !_to) {
                return false;
            }
            _to->setCounterType(_from->counterType());
        }
        break;
    case GlobalDefine::dtCheck:    // 校验
        {
            ICDCheckData::smtCheck _from = SMT_CONVERT(ICDCheckData, from);
            ICDCheckData::smtCheck _to = SMT_CONVERT(ICDCheckData, to);
            if (!_from || !_to) {
                return false;
            }
            _to->setCheckType(_from->checkType());
            _to->setStart(_from->start());
            _to->setEnd(_from->end());
        }
        break;
    case GlobalDefine::dtFrameCode: // 帧识别码
        {
            ICDFrameCodeData::smtFrameCode _from = SMT_CONVERT(ICDFrameCodeData, from);
            ICDFrameCodeData::smtFrameCode _to = SMT_CONVERT(ICDFrameCodeData, to);
            if (!_from || !_to) {
                return false;
            }
            _to->setFrameType(_from->frameType());
            _to->bindData(_from->data());
        }
        break;
    case GlobalDefine::dtHead:     // 包头
    case GlobalDefine::dtU8:       // 无符号8位
    case GlobalDefine::dt8:        // 有符号8位
    case GlobalDefine::dtU16:      // 无符号16位
    case GlobalDefine::dt16:       // 有符号16位
    case GlobalDefine::dtU32:      // 无符号32位
    case GlobalDefine::dt32:       // 有符号32位
    case GlobalDefine::dtF32:      // 32位浮点数
    case GlobalDefine::dtF64:      // 64位浮点数
        {
        }
        break;
    case GlobalDefine::dtBitMap:   // BITMAP
    case GlobalDefine::dtBitValue: // BITVALUE
        {
            ICDBitData::smtBit _from = SMT_CONVERT(ICDBitData, from);
            ICDBitData::smtBit _to = SMT_CONVERT(ICDBitData, to);
            if (!_from || !_to) {
                return false;
            }
            _to->setStart(_from->start());
            _to->setBitLength(_from->bitLength());
        }
        break;
    case GlobalDefine::dtComplex:  // 复合数据
    case GlobalDefine::dtDiscern:  // 帧数据
        {
            ICDComplexData::smtComplex _from = SMT_CONVERT(ICDComplexData, from);
            ICDComplexData::smtComplex _to = SMT_CONVERT(ICDComplexData, to);
            if (!_from || !_to) {
                return false;
            }
            _to->setRemark(_from->remark());
            _to->setProCode(_from->proCode());
        }
        break;
    }

    return true;
}
