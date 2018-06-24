#ifndef GLOBALSTRUCT_H
#define GLOBALSTRUCT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

//
#ifndef MAX_BIT_COUNT
#define MAX_BIT_COUNT 64
#endif

namespace GlobalDefine {

/**
 * @brief ICD�������Ͷ��壬���ֵ��Dic_DataTypeͬ��
 */
enum DataType {
    dtInvalid   = -1,   // ��Ч
    dtHead      = 10,   // ��ͷ
    dtCounter,          // ֡����
    dtCheck,            // У��
    dtFrameCode,        // ֡ʶ����
    dtNumeric,          // ��ֵ
    dtArray,            // ����
    dtBitMap,           // BITMAP
    dtBitValue,         // BITVALUE
    dtComplex,          // ��������
    dtFrame,            // ֡����
    dtBuffer,           // ����Ԥ����
    dtTotal             // ��Ǳ߽�
};

/**
 * @brief ICDУ�����Ͷ��壬���ֵ��Dic_CheckTypeͬ��
 */
enum CheckType {
    ctInvalid   = -1,   // ��Ч
    ct8And,             // 8λ��У��
    ct16And,            // 16λ��У��
    ctCRC8,             // CRC8У��
    ctCRC16,            // CRC16У��
    ctXor8,             // 8λ����У��
    ctXor16,            // 16λ����У��
    ctTotal             // ��Ǳ߽�
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
 * @brief The NumericType enum
 */
enum NumericType {
    NumericI8,          // �з���8λ
    NumericU8,          // �޷���8λ
    NumericI16,         // �з���16λ
    NumericU16,         // �޷���16λ
    NumericI32,         // �з���32λ
    NumericU32,         // �޷���32λ
    NumericI64,         // �з���64λ
    NumericU64,         // �޷���64λ
    NumericF32,         // 32λ������
    NumericF64,         // 64λ������
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
    ntVehicle = 0,        // ����
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
    dicNumericType,     // ��ֵ����
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
        return sID < rhs.sID;
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
    unsigned short  subType;    // ������

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
