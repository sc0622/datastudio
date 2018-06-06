#ifndef SYSTEMNODE_H
#define SYSTEMNODE_H

#include "tablenode.h"

class SystemNodePrivate;
/**
 * @brief ��ϵͳ��
 */
class KERNELCLASS_EXPORT SystemNode
    : public ICDElement
{
public:
    typedef std::shared_ptr<SystemNode> smtSystem;
    typedef std::map<int, SystemNode::smtSystem> systemMap;
    typedef std::vector<SystemNode::smtSystem> systemVector;

public:
    /**
     * @brief ���캯��
     * @param [in] id : ��ϵͳ��
     * @param [in] name : ��ϵͳ����
     * @param [in] sign : ��ϵͳ��־
     * @param [in] describe : ����
     */
    SystemNode(int id,
               const std::string& name,
               const std::string& sign,
               const std::string& describe = std::string());

    /**
     * @brief ���캯��
     * @param [in] plane : ��ϵͳ������Ϣ
     */
    SystemNode(const stSystem& system);
    ~SystemNode();

    /**
     * @brief ���÷�ϵͳ������Ϣ
     * @param [in] system : ��ϵͳ������Ϣ
     */
    void setSystem(const stSystem& system);

    /**
     * @brief ��ȡ��ϵͳ������Ϣ
     * @return ��ϵͳ������Ϣ
     */
    stSystem system() const;

    // ����/��ȡ����Id
    void setNumeralId(int id);
    int numeralId() const;

    /**
     * @brief �����¹�ICD��
     * @param [in] table : ICD����Ϣ��
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    void setTable(const TableNode::tableVector& table);

    /**
     * @brief ��ȡ�����¹�ICD����Ϣ
     * @return �����¹�ICD����Ϣ
     */
    TableNode::tableVector allTable() const;

    /**
     * @brief ��֤�¹�ICD���Ƿ�Ϊ��
     * @return �¹�ICD��Ϊ�ձ�ʶ
     */
    bool isEmpty() const;

    /**
     * @brief ��ȡ�¹�ICD����Ϣ
     * @param [in] name : ICD���ʶ
     * @return �¹�ICD����Ϣ
     */
    TableNode::smtTable table(const std::string& name) const;

    /**
     * @brief �����¹�ICD��
     * @param [in] table : ICD����Ϣ��
     */
    void addTable(const TableNode::smtTable& table);

    /**
     * @brief ɾ���¹�ICD��
     * @param [in] table : ICD���ʶ��
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool deleteTable(const std::vector<std::string> &table);

    /**
     * @brief ɾ���¹�����ICD��
     */
    void clearTable();

    /**
     * @brief ��¡�������
     * @return ��ϵͳʵ��
     */
    SystemNode::smtSystem clone() const;

protected:
    SystemNode(const SystemNode &rhs);
    SystemNode &operator = (const SystemNode &rhs);

private:
    SystemNodePrivate   *d;
};

#endif // SYSTEMNODE_H
