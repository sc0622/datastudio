#ifndef PLANENODE_H
#define PLANENODE_H

#include "systemnode.h"

class PlaneNodePrivate;
/**
 * @brief �ɻ��ͺ���
 */
class KERNELCLASS_EXPORT PlaneNode
    : public ICDElement
{
public:
    typedef std::shared_ptr<PlaneNode> smtPlane;
    typedef std::map<int, PlaneNode::smtPlane> planeMap;
    typedef std::vector<PlaneNode::smtPlane> planeVector;

public:

    /**
     * @brief ���캯��
     * @param [in] id : �ɻ��ͺ���
     * @param [in] name : �ɻ�����
     * @param [in] name : �ɻ���־
     * @param [in] describe : ����
     */
    PlaneNode(int id,
              const std::string& name,
              const std::string &sign,
              const std::string& describe = std::string());

    /**
     * @brief ���캯��
     * @param [in] plane : �ɻ�������Ϣ
     */
    PlaneNode(const stPlane& plane);
    ~PlaneNode();

    // ����/��ȡ����Id
    void setNumeralId(int id);
    int numeralId() const;

    /**
     * @brief ���÷�ϵͳ������Ϣ
     * @param [in] system : ��ϵͳ������Ϣ
     */
    void setPlane(const stPlane& plane);

    /**
     * @brief ��ȡ��ϵͳ������Ϣ
     * @return ��ϵͳ������Ϣ
     */
    stPlane plane() const;

    /**
     * @brief �����¹�ϵͳ��Ϣ
     * @param [in] system : ϵͳ��Ϣ��
     */
    void setSystem(const SystemNode::systemVector& system);

    /**
     * @brief ��ȡ�����¹�ϵͳ��Ϣ
     * @return �����¹�ϵͳ��Ϣ
     */
    SystemNode::systemVector allSystem() const;

    /**
     * @brief ��֤�¹�ϵͳ�Ƿ�Ϊ��
     * @return �¹�ϵͳΪ�ձ�ʶ
     */
    bool isEmpty() const;

    /**
     * @brief ��ȡ�¹�ϵͳ��Ϣ
     * @param [in] id : ϵͳ��ʶ
     * @return �¹�ϵͳ��Ϣ
     */
    SystemNode::smtSystem system(int id) const;

    /**
     * @brief �����¹ҷ�ϵͳ
     * @param [in] system : ϵͳ��Ϣ��
     */
    void addSystem(const SystemNode::smtSystem& system);

    /**
     * @brief ɾ���¹ҷ�ϵͳ
     * @param [in] system : ϵͳ��ʶ��
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool deleteSystem(const std::vector<int> &system);

    /**
     * @brief ɾ���¹�����ϵͳ
     */
    void clearSystem();

    /**
     * @brief ��¡�������
     * @return ����ʵ��
     */
    PlaneNode::smtPlane clone() const;

protected:
    PlaneNode(const PlaneNode &rhs);
    PlaneNode &operator = (const PlaneNode &rhs);

private:
    PlaneNodePrivate    *d;
};

#endif // PLANENODE_H
