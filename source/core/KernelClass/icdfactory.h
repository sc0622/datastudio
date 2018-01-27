#ifndef ICDFACTORY_H
#define ICDFACTORY_H
#include "icdmetadata.h"

/**
 * @brief ICD工厂类
 */
class KERNELCLASS_EXPORT ICDFactory
{
public:
    /**
     * @brief 获取工厂访问实例
     * @return 工厂访问实例
     */
    static ICDFactory& instance();
    
    /**
    * @brief 克隆数据
    * @return 数据
    */
    static ICDElement::smtElement clone(const ICDElement::smtElement &elemet);

    /**
     * @brief 创建ICD数据元素
     * @param [in] rule : 规则数据
     * @return 数据元素
     */
    ICDMetaData::smtMeta CreatObject(const stTableRules& rule);

    /**
    * @brief 创建ICD数据元素
    * @param [in] type : ICD数据类型
    * @return 数据元素
    */
    ICDMetaData::smtMeta CreatObject(GlobalDefine::DataType type);

    /**
    * @brief 将ICD类数据转换成统一规则结构
    * @param [in] meta : ICD类
    * @return 规则数据
    */
    stTableRules convert2Rule(const ICDMetaData::smtMeta &meta);

    /**
    * @brief 从from拷贝基本数据到to
    * @param [in] from : 源数据
    * @param [in] to : 目标数据
    * @return 拷贝结果
    */
    bool copyBaseData(const ICDMetaData::smtMeta from,
                      const ICDMetaData::smtMeta to);

private:
    ICDFactory(){};
    ~ICDFactory(){};
};

#endif // ICDFACTORY_H
