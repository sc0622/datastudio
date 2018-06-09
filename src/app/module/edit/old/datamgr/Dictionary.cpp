#include "Dictionary.h"

class CDictionaryPrivate
{
public:
    CDictionary::mapCode    q_code2Dec;     // 编码转描述
    CDictionary::mapDec     q_dec2Code;     // 描述转编码
    std::unordered_map<std::string, std::vector<stDictionary>> q_mapAll; // 原始数据
};

CDictionary::CDictionary(void)
    : d(new CDictionaryPrivate)
{
}

CDictionary::~CDictionary(void)
{
    if (NULL != d) {
        delete d;
        d = NULL;
    }
}

/**
* @brief 获取字典类实例
* @return 字典实例
*/
CDictionary& CDictionary::instance()
{
    static CDictionary ins;

    return ins;
}

/**
* @brief 初始化字典表数据(在使用查询接口前需要调用一次，否则无法查询数据)
* @param [in] dic : 字典表数据集<字典标识, 字典数据集>
*/
void CDictionary::initDictionary( const std::unordered_map<std::string, std::vector<stDictionary>>& dic )
{
    if (NULL == d) {
        d = new CDictionaryPrivate;
    }
    // 清空数据
    d->q_code2Dec.clear();
    d->q_dec2Code.clear();
    d->q_mapAll.clear();

    std::unordered_map<int, stDictionary> code2Dec;
    std::unordered_map<std::string, stDictionary> dec2Code;
    std::unordered_map<std::string, std::vector<stDictionary>>::const_iterator it = dic.begin();
    for (; it != dic.end(); ++it) {
        const std::vector<stDictionary>& table = it->second;
        const int count(table.size());
        code2Dec.clear();
        dec2Code.clear();
        for (int i = 0; i < count; ++i) {
            const stDictionary& data = table.at(i);

            code2Dec[data.nCode] = data;
            dec2Code[data.sDec] = data;
        }
        d->q_code2Dec[it->first] = code2Dec;
        d->q_dec2Code[it->first] = dec2Code;
    }
    d->q_mapAll = dic;
}

/**
* @brief 更新数据
* @param [in] dic : 字典表数据集<字典标识, 字典数据集>
* @return 执行结果，true：成功；false：失败
*/
bool CDictionary::updateDictionary(const std::unordered_map<std::string, std::vector<stDictionary>>& dic)
{
    if (NULL == d) {
        return false;
    }
    std::unordered_map<int, stDictionary> code2Dec;
    std::unordered_map<std::string, stDictionary> dec2Code;
    std::unordered_map<std::string, std::vector<stDictionary>>::const_iterator it = dic.begin();
    for (; it != dic.end(); ++it) {
        const std::vector<stDictionary>& table = it->second;
        const int count(table.size());
        code2Dec.clear();
        dec2Code.clear();
        for (int i = 0; i < count; ++i) {
            const stDictionary& data = table.at(i);

            code2Dec[data.nCode] = data;
            dec2Code[data.sDec] = data;
        }
        d->q_code2Dec[it->first] = code2Dec;
        d->q_dec2Code[it->first] = dec2Code;
        d->q_mapAll[it->first] = table;
    }

    return true;
}

/**
* @brief 更新数据
* @param [in] dic : 单条字典数据
* @param [in] opt : 操作类型
* @return 执行结果，true：成功；false：失败
*/
bool CDictionary::updateDictionary(const std::string &table,
                                   const stDictionary &dic, 
                                   GlobalDefine::OptionType opt)
{
    if (NULL == d) {
        return false;
    }
    // code
    mapCode::iterator itC = d->q_code2Dec.find(table);
    if (itC == d->q_code2Dec.end()) {
        return false;
    }
    std::unordered_map<int, stDictionary>& mapC = itC->second;
    std::unordered_map<int, stDictionary>::iterator itID = mapC.end();
    // dec
    mapDec::iterator itD = d->q_dec2Code.find(table);
    if (itD == d->q_dec2Code.end()) {
        return false;
    }
    std::unordered_map<std::string, stDictionary>& mapD = itD->second;
    std::unordered_map<std::string, stDictionary>::iterator itSD = mapD.end();
    // all
    std::unordered_map<std::string, std::vector<stDictionary>>::iterator itA
        = d->q_mapAll.find(table);
    if (itA == d->q_mapAll.end()) {
        return false;
    }
    std::vector<stDictionary> & vecA = itA->second;
    std::vector<stDictionary>::iterator itV = vecA.end();
//    const int count = vecA.size();

    switch (opt)
    {
    case GlobalDefine::optNew:
        mapC[dic.nCode] = dic;
        mapD[dic.sDec] = dic;
        vecA.push_back(dic);
        break;
    case GlobalDefine::optEdit:
        mapC[dic.nCode] = dic;
        mapD[dic.sDec] = dic;
        for (itV = vecA.begin(); itV != vecA.end(); ++itV) {
            stDictionary &lhs = *itV;
            if (dic.nCode == lhs.nCode) {
                lhs = dic;
                break;
            }
        }
        if (itV == vecA.end()) {
            vecA.push_back(dic);
        }
        break;
    case GlobalDefine::optDelete:
        if ((itID = mapC.find(dic.nCode)) != mapC.end()) {
            mapC.erase(itID);
        }
        if ((itSD = mapD.find(dic.sDec)) != mapD.end()) {
            mapD.erase(itSD);
        }
        for (itV = vecA.begin(); itV != vecA.end(); ++itV) {
            stDictionary &lhs = *itV;
            if (dic.nCode == lhs.nCode) {
                vecA.erase(itV);
                break;
            }
        }
        break;
    default:break;
    }
    return true;
}

/**
* @brief code转dec
* @param [in] table : 表名
* @param [in] code : 编码
* @param [out] dec : 描述
* @return 执行结果，true：成功；false：失败
*/
bool CDictionary::convert2Dec( const std::string& table, int code, std::string&dec )
{
    if (NULL == d) {
        return false;
    }
    mapCode::iterator it = d->q_code2Dec.find(table);
    if (it == d->q_code2Dec.end()) {
        return false;
    }
    const std::unordered_map<int, stDictionary>& mapData = it->second;
    std::unordered_map<int, stDictionary>::const_iterator itData = mapData.find(code);
    if (itData == mapData.end()) {
        return false;
    }
    dec = itData->second.sDec;

    return true;
}

/**
* @brief dec转code
* @param [in] table : 表名
* @param [int] dec : 描述
* @param [out] code : 编码
* @return 执行结果，true：成功；false：失败
*/
bool CDictionary::convert2Code( const std::string& table, const std::string& dec, int& code )
{
    if (NULL == d) {
        return false;
    }
    mapDec::iterator it = d->q_dec2Code.find(table);
    if (it == d->q_dec2Code.end()) {
        return false;
    }
    const std::unordered_map<std::string, stDictionary>& mapData = it->second;
    std::unordered_map<std::string, stDictionary>::const_iterator itData = mapData.find(dec);
    if (itData == mapData.end()) {
        return false;
    }
    code = itData->second.nCode;

    return true;
}

/**
* @brief 获取单张字典表所有数据
* @param [in] table : 表名
* @return 字典表数据集
*/
std::vector<stDictionary> CDictionary::singleDic(const std::string& table)
{
    if (NULL == d) {
        return std::vector<stDictionary>();
    }
    std::unordered_map<std::string, std::vector<stDictionary>>::iterator it
        = d->q_mapAll.find(table);
    if (it == d->q_mapAll.end()) {
        return std::vector<stDictionary>();
    }
    else {
        return it->second;
    };
}

/**
 * @brief 获取单条数据信息
* @param [in] table : 表名
* @param [out] code : 编码
 * @return 字典表数据
 */
stDictionary CDictionary::singleItem(const std::string &table, int code)
{
    if (NULL == d) {
        return stDictionary();
    }
    std::unordered_map<std::string, std::vector<stDictionary>>::iterator
        it = d->q_mapAll.find(table);
    if (it == d->q_mapAll.end()) {
        return stDictionary();
    }
    else {
        const std::vector<stDictionary>& dic = it->second;
        const int count(dic.size());
        for (int i = 0; i < count; ++i) {
            const stDictionary& data = dic.at(i);
            if (data.nCode == code) {
                return data;
            }
        }
        return stDictionary();
    }
}
