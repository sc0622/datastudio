#pragma once

#include <string>
#include "globalstruct.h"

// �ֵ��࣬�������ݿ��ֵ����code��dec֮���ת��

class CDictionaryPrivate;
struct stDictionary;

// 
class CDictionary
{
public:
    typedef std::map<std::string, std::map<int, stDictionary>> mapCode;
    typedef std::map<std::string, std::map<std::string, stDictionary>> mapDec;

    /**
    * @brief ��ȡ�ֵ���ʵ��
    * @return �ֵ�ʵ��
    */
    static CDictionary& instance();

    /**
    * @brief ��ʼ���ֵ������(��ʹ�ò�ѯ�ӿ�ǰ��Ҫ����һ�Σ������޷���ѯ����)
    * @param [in] dic : �ֵ�����ݼ�<�ֵ��ʶ, �ֵ����ݼ�>
    */
    void initDictionary(const std::map<std::string, std::vector<stDictionary>>& dic);

    /**
    * @brief ��������
    * @param [in] dic : �ֵ�����ݼ�<�ֵ��ʶ, �ֵ����ݼ�>
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool updateDictionary(const std::map<std::string, std::vector<stDictionary>>& dic);

    /**
    * @brief ��������
    * @param [in] dic : �����ֵ�����
    * @param [in] opt : ��������
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool updateDictionary(const std::string &table, 
                          const stDictionary &dic,
                          GlobalDefine::OptionType opt);

    /**
    * @brief codeתdec
    * @param [in] table : ����
    * @param [in] code : ����
    * @param [out] dec : ����
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool convert2Dec(const std::string& table, int code, std::string&dec);

    /**
    * @brief decתcode
    * @param [in] table : ����
    * @param [int] dec : ����
    * @param [out] code : ����
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool convert2Code(const std::string& table, const std::string& dec, int& code);

    /**
    * @brief ��ȡ�����ֵ����������
    * @param [in] table : ����
    * @return �ֵ�����ݼ�
    */
    std::vector<stDictionary> singleDic(const std::string& table);

    /**
     * @brief ��ȡ����������Ϣ
    * @param [in] table : ����
    * @param [out] code : ����
     * @return �ֵ������
     */
    stDictionary singleItem(const std::string& table, int code);

private:
    /**
    * @brief ���캯��
    */
    CDictionary(void);

    /**
    * @brief ��������
    */
    ~CDictionary(void);

    CDictionaryPrivate*     d;
};

