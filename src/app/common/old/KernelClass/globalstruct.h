#ifndef GLOBALSTRUCT_H
#define GLOBALSTRUCT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace GlobalDefine {

/**
 * @brief ICD数据类型定义，与字典表Dic_DataType同步
 */
enum DataType {
    dtInvalid   = -1,   // 无效
    dtHead      = 10,   // 包头
    dtCounter,          // 帧计数
    dtCheck,            // 校验
    dtFrameCode,        // 帧识别码
    dtNumeric,          // 数值
    dtArray,            // 数组
    dtBitMap,           // BITMAP
    dtBitValue,         // BITVALUE
    dtComplex,          // 复合数据
    dtDiscern,          // 帧数据
    dtBuffer,           // 数据预留区
    dtTotal             // 标记边界
};

/**
 * @brief ICD校验类型定义，与字典表Dic_CheckType同步
 */
enum CheckType {
    ctInvalid   = -1,   // 无效
    ct8And,             // 8位和校验
    ct16And,            // 16位和校验
    ctCRC8,             // CRC8校验
    ctCRC16,            // CRC16校验
    ctXor8,             // 8位异或和校验
    ctXor16,            // 16位异或和校验
    ctTotal             // 标记边界
};

/**
* @brief ICD帧计数类型定义，与字典表Dic_CounterType同步
*/
enum CounterType {
    counterU8 = 0,      // 8位计数
    counterU16,         // 16位计数
    counterU32,         // 32位计数
    counterU64,         // 32位计数
    counterTotal        // 标记边界
};

/**
* @brief ICD帧计数类型定义，与字典表Dic_CounterType同步
*/
enum FrameType {
    frame8 = 0,     // 8位帧识别码
    frame16,        // 16位帧识别码
    frame32,        // 32位帧识别码
    frame64,        // 64位帧识别码
    frameTotal      // 标记边界
};

/**
 * @brief The NumericType enum
 */
enum NumericType {
    NumericI8,          // 有符号8位
    NumericU8,          // 无符号8位
    NumericI16,         // 有符号16位
    NumericU16,         // 无符号16位
    NumericI32,         // 有符号32位
    NumericU32,         // 无符号32位
    NumericI64,         // 有符号64位
    NumericU64,         // 无符号64位
    NumericF32,         // 32位浮点数
    NumericF64,         // 64位浮点数
    NumericTotal
};

/**
 * @brief The ArrayType enum
 */
enum ArrayType {
    ArrayInvalid = -1,
    ArrayI8,
    ArrayU8,
    ArrayI16,
    ArrayU16,
    ArrayI32,
    ArrayU32,
    ArrayI64,
    ArrayU64,
    ArrayF32,
    ArrayF64,
    ArrayTotal
};

/**
* @brief ICD数据源定义
*/
enum DataSource {
    dsNone = 0,     // 无效
    dsDatabase,     // 数据库
    dsFile,         // 文件 [xml、json]
    dsTotal         // 标记边界
};

/**
 * @brief ICD系统结构定义
 */
enum NodeType{
    ntUnknown   = -1,   // 未知
    ntPlane = 0,        // 机型
    ntSystem,           // 分系统
    ntTable,            // 规则表
    ntRule,             // 具体规则
    tlTotal             // 标记边界
};

/**
 * @brief ICD字典定义
 */
enum DictionaryIndex {
    dicDataType = 0,    // 数据类型
    dicPowerType,       // 权限类型
    dicCounterType,     // 帧计数类型
    dicArrayType,       // 数组类型
    dicNumericType,     // 数值类型
    dicCheckType,       // 校验类型
};

/**
 * @brief 字典数据项定义
 */
enum DictionaryType {
    dictCode    = 0,    // 编码
    dictDec,            // 描述
    dictSign,           // 标识
    dictRemark,         // 备注
};

/**
* @brief 操作类型项定义
*/
enum OptionType {
    optNew  = 0,    // 新增
    optInsert,      // 插入
    optEdit,        // 编辑
    optDelete,      // 删除
    optClear,       // 清空
};

/**
 * @brief 状态类型定义
 */
enum StateType {
    noneState   = 0,    // 均未置状态
    partiallyState,     // 部分置状态
    wholeState          // 全部置状态
};

/**
 * @brief 结果类型定义
 */
enum ResultType {
    failed  = 0,    // 失败
    success,        // 成功
    special,        // 其他含义
};

} // GolbalDefine

// 字典结构体
struct stDictionary {
    int             nCode;      // 编码标识
    std::string     sDec;       // 描述
    std::string     sSign;      // 标志
    std::string     sRemark;    // 备注

    stDictionary()
    {
        nCode = -1;
    }
};

// 字典单条数据查询结构
struct stQueryDic {
    GlobalDefine::DictionaryIndex dic;        // 查询字典
    GlobalDefine::DictionaryType  dicType;    // 查询类型
    std::string         condition;  // 查询条件
    std::string         result;     // 查询结果

    stQueryDic()
    {
        dic = GlobalDefine::dicDataType;
        dicType = GlobalDefine::dictDec;
    }
};

// 字典结构数据查询结构
struct  stQueryDicItem {
    GlobalDefine::DictionaryIndex dic;        // 查询字典
    int                 code;       // 编码
    stDictionary        data;       // 查询结果

    stQueryDicItem() :dic(GlobalDefine::dicDataType), code(-1)
    {}
};

// 机型结构信息
struct stPlane {
    int             nCode;      // 机型编号
    std::string     sName;      // 机型名称
    std::string     sSign;      // 机型标志
    std::string     sDescribe;  // 描述

    stPlane()
    {
        nCode = -1;
    }

    bool operator < (const stPlane &rhs) const
    {
        return nCode < rhs.nCode;
    }

    stDictionary toDictionary() const
    {
        stDictionary result;
        result.nCode = nCode;
        result.sDec = sName;
        result.sSign = sSign;
        result.sRemark = sDescribe;

        return result;
    }
};

// 系统基本信息
struct stSystem {
    int             nCode;      // 系统编号
    std::string     sName;      // 系统名称
    std::string     sSign;      // 系统标志
    std::string     sDescribe;  // 描述

    stSystem()
    {
        nCode = -1;
    }

    bool operator < (const stSystem &rhs) const
    {
        return nCode < rhs.nCode;
    }

    stDictionary toDictionary() const
    {
        stDictionary result;
        result.nCode = nCode;
        result.sDec = sName;
        result.sSign = sSign;
        result.sRemark = sDescribe;

        return result;
    }
};

// 机型和分系统关系
struct stPSRelation {
    int     nPlaneCode;     // 机型码
    int     nSystemCode;    // 分系统码

    stPSRelation() {
        nPlaneCode = -1;
        nSystemCode = -1;
    }
};

// 表权限结构
struct stTablePrivilege
{
    int             nPower;     // 权限类型
    std::string     sName;      // 表名

    stTablePrivilege()
    {
        nPower = -1;
    }
};

// 表基本信息
struct stICDBase
{
    int             nLength;        // 长度
    bool            check;          // 校验
    std::string     sName;          // 表名
    std::string     sID;            // 编号
    std::string     sCode;          // 编码
    std::string     sParentName;    // 父表名
    std::string     sDescribe;      // 表描述
    std::string     sGroup;         // 所属组
    std::string     sRemark;        // 备注

    stICDBase()
    {
        check = true;
        nLength = 0;
    }

    bool operator < (const stICDBase &rhs) const
    {
        return sName < rhs.sName;
    }
};

// 表结构数据
struct stTableRules
{
    int             nCode;      // 字节序号
    int             nSerial;    // 序号（表）
    unsigned short  uLength;    // 数据长度
    unsigned short  uType;      // 数据类型
    std::string     sName;      // 中文名称
    std::string     sPrgCode;   // 编码
    std::string     sDefault;   // 默认值
    std::string     sRange;     // 取值范围
    double          dOffset;    // 偏置
    double          dScale;     // 比例尺
    std::string     sUnit;      // 单位
    std::string     sRule;      // 规则
    std::string     sRemark;    // 备注
    unsigned short  subType;    // 子类型

    stTableRules()
    {
        nCode = 0;
        nSerial = 0;
        uLength = 1;
        uType = -1;
        dOffset = 0.0;
        dScale = 1.0;
        subType = -1;
    }

    bool operator < (const stTableRules &rhs) const
    {
        return nSerial < rhs.nSerial;
    }
};

namespace DMSpace {
    typedef std::pair<stPlane, std::vector<stSystem>> pairPS;
    typedef std::vector<pairPS> _vectorPS;
    typedef std::pair<std::string, std::vector<stICDBase>> pairSB;
    typedef std::vector<pairSB> _vectorSB;
    typedef std::pair<stICDBase, std::vector<stTableRules>> pairIcdTR;
    typedef std::vector<pairIcdTR> _vectorIcdTR;
    typedef _vectorPS::iterator _vectorPS_It;
    typedef _vectorPS::const_iterator _vectorPS_Cit;
    typedef _vectorIcdTR::iterator _vectorIcdTR_It;
    typedef _vectorIcdTR::const_iterator _vectorIcdTR_Cit;
    typedef std::unordered_map<int, stTableRules> srMap;
    typedef std::unordered_map<std::string, srMap> ssrMap;
    typedef std::unordered_map<std::string, std::vector<stTableRules>> svrMap;
}

#endif // GLOBALSTRUCT_H
