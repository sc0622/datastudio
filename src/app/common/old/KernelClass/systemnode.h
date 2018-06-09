#ifndef SYSTEMNODE_H
#define SYSTEMNODE_H

#include "tablenode.h"

class SystemNodePrivate;
/**
 * @brief 分系统类
 */
class KERNELCLASS_EXPORT SystemNode
    : public ICDElement
{
public:
    typedef std::shared_ptr<SystemNode> smtSystem;
    typedef std::unordered_map<int, SystemNode::smtSystem> systemMap;
    typedef std::vector<SystemNode::smtSystem> systemVector;

public:
    /**
     * @brief 构造函数
     * @param [in] id : 分系统码
     * @param [in] name : 分系统名称
     * @param [in] sign : 分系统标志
     * @param [in] describe : 描述
     */
    SystemNode(int id,
               const std::string& name,
               const std::string& sign,
               const std::string& describe = std::string());

    /**
     * @brief 构造函数
     * @param [in] plane : 分系统基本信息
     */
    SystemNode(const stSystem& system);
    ~SystemNode();

    /**
     * @brief 设置分系统基本信息
     * @param [in] system : 分系统基本信息
     */
    void setSystem(const stSystem& system);

    /**
     * @brief 获取分系统基本信息
     * @return 分系统基本信息
     */
    stSystem system() const;

    // 设置/获取数字Id
    void setNumeralId(int id);
    int numeralId() const;

    /**
     * @brief 设置下挂ICD表
     * @param [in] table : ICD表信息集
     * @return 执行结果，true：成功；false：失败
     */
    void setTable(const TableNode::tableVector& table);

    /**
     * @brief 获取所有下挂ICD表信息
     * @return 所有下挂ICD表信息
     */
    TableNode::tableVector allTable() const;

    /**
     * @brief 验证下挂ICD表是否为空
     * @return 下挂ICD表为空标识
     */
    bool isEmpty() const;

    /**
     * @brief 获取下挂ICD表信息
     * @param [in] name : ICD表标识
     * @return 下挂ICD表信息
     */
    TableNode::smtTable table(const std::string& name) const;

    /**
     * @brief 增加下挂ICD表
     * @param [in] table : ICD表信息集
     */
    void addTable(const TableNode::smtTable& table);

    /**
     * @brief 删除下挂ICD表
     * @param [in] table : ICD表标识集
     * @return 执行结果，true：成功；false：失败
     */
    bool deleteTable(const std::vector<std::string> &table);

    /**
     * @brief 删除下挂所有ICD表
     */
    void clearTable();

    /**
     * @brief 克隆（深拷贝）
     * @return 分系统实例
     */
    SystemNode::smtSystem clone() const;

protected:
    SystemNode(const SystemNode &rhs);
    SystemNode &operator = (const SystemNode &rhs);

private:
    SystemNodePrivate   *d;
};

#endif // SYSTEMNODE_H
