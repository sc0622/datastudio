#pragma once

#include <string>
#include "KernelClass/globalstruct.h"

// 字典类，负责将数据库字典进行code和dec之间的转换

class CDictionaryPrivate;
struct stDictionary;

// 
class CDictionary
{
public:
    typedef std::map<std::string, std::map<int, stDictionary>> mapCode;
    typedef std::map<std::string, std::map<std::string, stDictionary>> mapDec;

    /**
    * @brief 获取字典类实例
    * @return 字典实例
    */
    static CDictionary& instance();

    /**
    * @brief 初始化字典表数据(在使用查询接口前需要调用一次，否则无法查询数据)
    * @param [in] dic : 字典表数据集<字典标识, 字典数据集>
    */
    void initDictionary(const std::map<std::string, std::vector<stDictionary>>& dic);

    /**
    * @brief 更新数据
    * @param [in] dic : 字典表数据集<字典标识, 字典数据集>
    * @return 执行结果，true：成功；false：失败
    */
    bool updateDictionary(const std::map<std::string, std::vector<stDictionary>>& dic);

    /**
    * @brief 更新数据
    * @param [in] dic : 单条字典数据
    * @param [in] opt : 操作类型
    * @return 执行结果，true：成功；false：失败
    */
    bool updateDictionary(const std::string &table, 
                          const stDictionary &dic,
                          GlobalDefine::OptionType opt);

    /**
    * @brief code转dec
    * @param [in] table : 表名
    * @param [in] code : 编码
    * @param [out] dec : 描述
    * @return 执行结果，true：成功；false：失败
    */
    bool convert2Dec(const std::string& table, int code, std::string&dec);

    /**
    * @brief dec转code
    * @param [in] table : 表名
    * @param [int] dec : 描述
    * @param [out] code : 编码
    * @return 执行结果，true：成功；false：失败
    */
    bool convert2Code(const std::string& table, const std::string& dec, int& code);

    /**
    * @brief 获取单张字典表所有数据
    * @param [in] table : 表名
    * @return 字典表数据集
    */
    std::vector<stDictionary> singleDic(const std::string& table);

    /**
     * @brief 获取单条数据信息
    * @param [in] table : 表名
    * @param [out] code : 编码
     * @return 字典表数据
     */
    stDictionary singleItem(const std::string& table, int code);

private:
    /**
    * @brief 构造函数
    */
    CDictionary(void);

    /**
    * @brief 析构函数
    */
    ~CDictionary(void);

    CDictionaryPrivate*     d;
};

