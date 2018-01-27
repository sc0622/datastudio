#include "Dictionary.h"

class CDictionaryPrivate
{
public:
    CDictionary::mapCode    q_code2Dec;     // ����ת����
    CDictionary::mapDec     q_dec2Code;     // ����ת����
    std::map<std::string, std::vector<stDictionary>> q_mapAll; // ԭʼ����
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
* @brief ��ȡ�ֵ���ʵ��
* @return �ֵ�ʵ��
*/
CDictionary& CDictionary::instance()
{
    static CDictionary ins;

    return ins;
}

/**
* @brief ��ʼ���ֵ������(��ʹ�ò�ѯ�ӿ�ǰ��Ҫ����һ�Σ������޷���ѯ����)
* @param [in] dic : �ֵ�����ݼ�<�ֵ��ʶ, �ֵ����ݼ�>
*/
void CDictionary::initDictionary( const std::map<std::string, std::vector<stDictionary>>& dic )
{
    if (NULL == d) {
        d = new CDictionaryPrivate;
    }
    // �������
    d->q_code2Dec.clear();
    d->q_dec2Code.clear();
    d->q_mapAll.clear();

    std::map<int, stDictionary> code2Dec;
    std::map<std::string, stDictionary> dec2Code;
    std::map<std::string, std::vector<stDictionary>>::const_iterator it = dic.begin();
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
* @brief ��������
* @param [in] dic : �ֵ�����ݼ�<�ֵ��ʶ, �ֵ����ݼ�>
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool CDictionary::updateDictionary(const std::map<std::string, std::vector<stDictionary>>& dic)
{
    if (NULL == d) {
        return false;
    }
    std::map<int, stDictionary> code2Dec;
    std::map<std::string, stDictionary> dec2Code;

    std::map<std::string, std::vector<stDictionary>>::const_iterator
        it = dic.begin();
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
* @brief ��������
* @param [in] dic : �����ֵ�����
* @param [in] opt : ��������
* @return ִ�н����true���ɹ���false��ʧ��
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
    std::map<int, stDictionary>& mapC = itC->second;
    std::map<int, stDictionary>::iterator itID = mapC.end();
    // dec
    mapDec::iterator itD = d->q_dec2Code.find(table);
    if (itD == d->q_dec2Code.end()) {
        return false;
    }
    std::map<std::string, stDictionary>& mapD = itD->second;
    std::map<std::string, stDictionary>::iterator itSD = mapD.end();
    // all
    std::map<std::string, std::vector<stDictionary>>::iterator itA
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
* @brief codeתdec
* @param [in] table : ����
* @param [in] code : ����
* @param [out] dec : ����
* @return ִ�н����true���ɹ���false��ʧ��
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
    const std::map<int, stDictionary>& mapData = it->second;
    std::map<int, stDictionary>::const_iterator itData = mapData.find(code);
    if (itData == mapData.end()) {
        return false;
    }
    dec = itData->second.sDec;

    return true;
}

/**
* @brief decתcode
* @param [in] table : ����
* @param [int] dec : ����
* @param [out] code : ����
* @return ִ�н����true���ɹ���false��ʧ��
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
    const std::map<std::string, stDictionary>& mapData = it->second;
    std::map<std::string, stDictionary>::const_iterator itData = mapData.find(dec);
    if (itData == mapData.end()) {
        return false;
    }
    code = itData->second.nCode;

    return true;
}

/**
* @brief ��ȡ�����ֵ����������
* @param [in] table : ����
* @return �ֵ�����ݼ�
*/
std::vector<stDictionary> CDictionary::singleDic(const std::string& table)
{
    if (NULL == d) {
        return std::vector<stDictionary>();
    }
    std::map<std::string, std::vector<stDictionary>>::iterator it
        = d->q_mapAll.find(table);
    if (it == d->q_mapAll.end()) {
        return std::vector<stDictionary>();
    }
    else {
        return it->second;
    };
}

/**
 * @brief ��ȡ����������Ϣ
* @param [in] table : ����
* @param [out] code : ����
 * @return �ֵ������
 */
stDictionary CDictionary::singleItem(const std::string &table, int code)
{
    if (NULL == d) {
        return stDictionary();
    }
    std::map<std::string, std::vector<stDictionary>>::iterator
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
