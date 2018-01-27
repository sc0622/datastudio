#ifndef ICDFACTORY_H
#define ICDFACTORY_H
#include "icdmetadata.h"

/**
 * @brief ICD������
 */
class KERNELCLASS_EXPORT ICDFactory
{
public:
    /**
     * @brief ��ȡ��������ʵ��
     * @return ��������ʵ��
     */
    static ICDFactory& instance();
    
    /**
    * @brief ��¡����
    * @return ����
    */
    static ICDElement::smtElement clone(const ICDElement::smtElement &elemet);

    /**
     * @brief ����ICD����Ԫ��
     * @param [in] rule : ��������
     * @return ����Ԫ��
     */
    ICDMetaData::smtMeta CreatObject(const stTableRules& rule);

    /**
    * @brief ����ICD����Ԫ��
    * @param [in] type : ICD��������
    * @return ����Ԫ��
    */
    ICDMetaData::smtMeta CreatObject(GlobalDefine::DataType type);

    /**
    * @brief ��ICD������ת����ͳһ����ṹ
    * @param [in] meta : ICD��
    * @return ��������
    */
    stTableRules convert2Rule(const ICDMetaData::smtMeta &meta);

    /**
    * @brief ��from�����������ݵ�to
    * @param [in] from : Դ����
    * @param [in] to : Ŀ������
    * @return �������
    */
    bool copyBaseData(const ICDMetaData::smtMeta from,
                      const ICDMetaData::smtMeta to);

private:
    ICDFactory(){};
    ~ICDFactory(){};
};

#endif // ICDFACTORY_H
