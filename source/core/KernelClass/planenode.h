#ifndef PLANENODE_H
#define PLANENODE_H

#include "systemnode.h"

class PlaneNodePrivate;
/**
 * @brief 飞机型号类
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
     * @brief 构造函数
     * @param [in] id : 飞机型号码
     * @param [in] name : 飞机名称
     * @param [in] name : 飞机标志
     * @param [in] describe : 描述
     */
    PlaneNode(int id,
              const std::string& name,
              const std::string &sign,
              const std::string& describe = std::string());

    /**
     * @brief 构造函数
     * @param [in] plane : 飞机基本信息
     */
    PlaneNode(const stPlane& plane);
    ~PlaneNode();

    // 设置/获取数字Id
    void setNumeralId(int id);
    int numeralId() const;

    /**
     * @brief 设置分系统基本信息
     * @param [in] system : 分系统基本信息
     */
    void setPlane(const stPlane& plane);

    /**
     * @brief 获取分系统基本信息
     * @return 分系统基本信息
     */
    stPlane plane() const;

    /**
     * @brief 设置下挂系统信息
     * @param [in] system : 系统信息集
     */
    void setSystem(const SystemNode::systemVector& system);

    /**
     * @brief 获取所有下挂系统信息
     * @return 所有下挂系统信息
     */
    SystemNode::systemVector allSystem() const;

    /**
     * @brief 验证下挂系统是否为空
     * @return 下挂系统为空标识
     */
    bool isEmpty() const;

    /**
     * @brief 获取下挂系统信息
     * @param [in] id : 系统标识
     * @return 下挂系统信息
     */
    SystemNode::smtSystem system(int id) const;

    /**
     * @brief 增加下挂分系统
     * @param [in] system : 系统信息集
     */
    void addSystem(const SystemNode::smtSystem& system);

    /**
     * @brief 删除下挂分系统
     * @param [in] system : 系统标识集
     * @return 执行结果，true：成功；false：失败
     */
    bool deleteSystem(const std::vector<int> &system);

    /**
     * @brief 删除下挂所有系统
     */
    void clearSystem();

    /**
     * @brief 克隆（深拷贝）
     * @return 机型实例
     */
    PlaneNode::smtPlane clone() const;

protected:
    PlaneNode(const PlaneNode &rhs);
    PlaneNode &operator = (const PlaneNode &rhs);

private:
    PlaneNodePrivate    *d;
};

#endif // PLANENODE_H
