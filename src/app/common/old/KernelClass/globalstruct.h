#ifndef GLOBALSTRUCT_H
#define GLOBALSTRUCT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace GlobalDefine {

/**
 * @brief ICD�������Ͷ��壬���ֵ��Dic_DataTypeͬ��
 */
enum DataType {
    dtInvalid   = -1,       // ��Ч

    dtHead      = 10,       // ��ͷ
    dtCounter,              // ֡����
    dtCheck,                // У��
    dtFrameCode,            // ֡ʶ����
    dtU8,                   // �޷���8λ
    dt8,                    // �з���8λ
    dtU16,                  // �޷���16λ
    dt16,                   // �з���16λ
    dtU32,                  // �޷���32λ
    dt32,                   // �з���32λ
    dtU64,                  // �޷���64λ
    dt64,                   // �з���64λ
    dtF32,                  // 32λ������
    dtF64,                  // 64λ������
    dtArray,                // ����
    dtBitMap,               // BITMAP
    dtBitValue,             // BITVALUE
    dtComplex,              // ��������
    dtDiscern,              // ֡����
    dtBuffer,               // ����Ԥ����

    dtTotal                 // ��Ǳ߽�
};

/**
 * @brief ICDУ�����Ͷ��壬���ֵ��Dic_CheckTypeͬ��
 */
enum CheckType {
    ctInvalid   = -1,       // ��Ч

    ct8And      = 0,        // 8λ��У��
    ct16And,                // 16λ��У��
    ctCRC8,                 // CRC8У��
    ctCRC16,                  // CRC16У��
    ctXor8,                 // 8λ����У��
    ctXor16,                // 16λ����У��

    ctTotal                 // ��Ǳ߽�
};

/**
* @brief ICD֡�������Ͷ��壬���ֵ��Dic_CounterTypeͬ��
*/
enum CounterType {
    counterU8 = 0,      // 8λ����
    counterU16,         // 16λ����
    counterU32,         // 32λ����
    counterU64,         // 32λ����

    counterTotal        // ��Ǳ߽�
};

/**
* @brief ICD֡�������Ͷ��壬���ֵ��Dic_CounterTypeͬ��
*/
enum FrameType {
    frame8 = 0,     // 8λ֡ʶ����
    frame16,        // 16λ֡ʶ����
    frame32,        // 32λ֡ʶ����
    frame64,        // 64λ֡ʶ����

    frameTotal      // ��Ǳ߽�
};

/**
 * @brief The ArrayType enum
 */
enum ArrayType {
    InvalidArray = -1,
    Int8Array,
    UInt8Array,
    Int16Array,
    UInt16Array,
    Int32Array,
    UInt32Array,
    Int64Array,
    UInt64Array,
    Float32Array,
    Float64Array,
    ArrayTypeTotal
};

/**
* @brief ICD����Դ����
*/
enum DataSource {
    dsNone = 0,     // ��Ч
    dsDatabase,     // ���ݿ�
    dsFile,         // �ļ� [xml��json]

    dsTotal         // ��Ǳ߽�
};

/**
 * @brief ICDϵͳ�ṹ����
 */
enum NodeType{
    ntUnknown   = -1,   // δ֪
    ntPlane = 0,        // ����
    ntSystem,           // ��ϵͳ
    ntTable,            // �����
    ntRule,             // �������

    tlTotal             // ��Ǳ߽�
};

/**
 * @brief ICD�ֵ䶨��
 */
enum DictionaryIndex {
    dicDataType = 0,    // ��������
    dicPowerType,       // Ȩ������
    dicCounterType,     // ֡��������
    dicArrayType,       // ��������
    dicCheckType,       // У������
};

/**
 * @brief �ֵ��������
 */
enum DictionaryType {
    dictCode    = 0,    // ����
    dictDec,            // ����
    dictSign,           // ��ʶ
    dictRemark,         // ��ע
};

/**
* @brief �����������
*/
enum OptionType {
    optNew  = 0,    // ����
    optInsert,      // ����
    optEdit,        // �༭
    optDelete,      // ɾ��
    optClear,       // ���
};

/**
 * @brief ״̬���Ͷ���
 */
enum StateType {
    noneState   = 0,    // ��δ��״̬
    partiallyState,     // ������״̬
    wholeState          // ȫ����״̬
};

/**
 * @brief ������Ͷ���
 */
enum ResultType {
    failed  = 0,    // ʧ��
    success,        // �ɹ�
    special,        // ��������
};

} // GolbalDefine

// �ֵ�ṹ��
struct stDictionary {
    int             nCode;      // �����ʶ
    std::string     sDec;       // ����
    std::string     sSign;      // ��־
    std::string     sRemark;    // ��ע

    stDictionary()
    {
        nCode = -1;
    }
};

// �ֵ䵥�����ݲ�ѯ�ṹ
struct stQueryDic {
    GlobalDefine::DictionaryIndex dic;        // ��ѯ�ֵ�
    GlobalDefine::DictionaryType  dicType;    // ��ѯ����
    std::string         condition;  // ��ѯ����
    std::string         result;     // ��ѯ���

    stQueryDic()
    {
        dic = GlobalDefine::dicDataType;
        dicType = GlobalDefine::dictDec;
    }
};

// �ֵ�ṹ���ݲ�ѯ�ṹ
struct  stQueryDicItem {
    GlobalDefine::DictionaryIndex dic;        // ��ѯ�ֵ�
    int                 code;       // ����
    stDictionary        data;       // ��ѯ���

    stQueryDicItem() :dic(GlobalDefine::dicDataType), code(-1)
    {}
};

// ���ͽṹ��Ϣ
struct stPlane {
    int             nCode;      // ���ͱ��
    std::string     sName;      // ��������
    std::string     sSign;      // ���ͱ�־
    std::string     sDescribe;  // ����

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

// ϵͳ������Ϣ
struct stSystem {
    int             nCode;      // ϵͳ���
    std::string     sName;      // ϵͳ����
    std::string     sSign;      // ϵͳ��־
    std::string     sDescribe;  // ����

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

// ���ͺͷ�ϵͳ��ϵ
struct stPSRelation {
    int     nPlaneCode;     // ������
    int     nSystemCode;    // ��ϵͳ��

    stPSRelation() {
        nPlaneCode = -1;
        nSystemCode = -1;
    }
};

// ��Ȩ�޽ṹ
struct stTablePrivilege
{
    int             nPower;     // Ȩ������
    std::string     sName;      // ����

    stTablePrivilege()
    {
        nPower = -1;
    }
};

// �������Ϣ
struct stICDBase
{
    int             nLength;        // ����
    bool            check;          // У��
    std::string     sName;          // ����
    std::string     sID;            // ���
    std::string     sCode;          // ����
    std::string     sParentName;    // ������
    std::string     sDescribe;      // ������
    std::string     sGroup;         // ������
    std::string     sRemark;        // ��ע

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

// ��ṹ����
struct stTableRules
{
    int             nCode;      // �ֽ����
    int             nSerial;    // ��ţ���
    unsigned short  uLength;    // ���ݳ���
    unsigned short  uType;      // ��������
    std::string     sName;      // ��������
    std::string     sPrgCode;   // ����
    std::string     sDefault;   // Ĭ��ֵ
    std::string     sRange;     // ȡֵ��Χ
    double          dOffset;    // ƫ��
    double          dScale;     // ������
    std::string     sUnit;      // ��λ
    std::string     sRule;      // ����
    std::string     sRemark;    // ��ע

    stTableRules()
    {
        nCode = 0;
        nSerial = 0;
        uLength = 1;
        uType = -1;
        dOffset = 0.0;
        dScale = 1.0;
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
