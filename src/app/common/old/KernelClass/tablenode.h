#ifndef TABLENODE_H
#define TABLENODE_H
#include "icdmetadata.h"
#include <vector>

class TableNodePrivate;
/**
 * @brief ICD表类
 */
class KERNELCLASS_EXPORT TableNode : public ICDElement
{
public:
    typedef std::shared_ptr<TableNode> smtTable;
    typedef std::unordered_map<std::string, TableNode::smtTable> tableMap;
    typedef std::vector<TableNode::smtTable> tableVector;

public:

    /**
     * @brief 构造函数
     * @param [in] icdBase : 表基本信息
     */
    TableNode(const stICDBase& icdBase, bool complex = false);
    ~TableNode();

    /**
     * @brief 设置表基本信息
     * @param [in] icdBase : 表基本信息
     */
    void setICDBase(const stICDBase& icdBase);

    /**
     * @brief 获取表基本信息
     * @return 表基本信息
     */
    stICDBase icdBase() const;

    /**
     * @brief 设置复合表状态
     * @param [in] complex : 复合状态
     */
    void setComplexity(bool complex);

    /**
     * @brief 设置长度校验
     * @param [in] check : 校验状态
     */
    void setLengthCheck(bool check) const;

    /**
     * @brief 验证规则信息是否为空
     * @return 规则信息为空标识
     */
    bool isEmpty() const;

    /**
     * @brief 验证本表是否复合表
     * @return 复合表状态，true：是；false：不是
     */
    bool isComplex() const;

    /**
    * @brief 验证本表是否有空子表
    * @param [in] names : 空表名字（父表到子表）
    * @return true：有；false：没有
    */
    bool hasEmptySubTable(std::string &names) const;

    /**
     * @brief 校验长度
     * @return 表长度与真实长度一致时返回true，否则false
     */
    bool lengthCheck() const;

    /**
     * @brief 数据字节长度
     * @return
     */
    int length() const;

    /**
     * @brief 数据真实长度
     * @return
     */
    double realLength() const;

    /**
     * @brief 获取表标识
     * @return 表标识
     */
    std::string key() const;

    /**
     * @brief 设置下挂规则信息
     * @param [in] rule : 规则信息集<规则字号, 规则信息>
     */
    void setRule(const ICDMetaData::ruleMap& rule);

    /**
     * @brief 获取所有下挂规则信息
     * @return 所有下挂规则信息
     */
    ICDMetaData::ruleMap allRule() const;

    /**
     * @brief 获取下挂规则信息
     * @param [in] serial : 规则序号
     * @return 下挂规则信息
     */
    ICDMetaData::smtMeta rule(int serial) const;

    /**
     * @brief 增加下挂规则信息
     * @param [in] rule : 规则信息
     * @return 执行结果，true：成功；false：失败
     */
    bool addRule(const ICDMetaData::smtMeta& rule);

    /**
     * @brief 删除下规则信息
     * @param [in] rule : 规则序号
     * @return 执行结果，true：成功；false：失败
     */
    bool deleteRule(const std::vector<int> &rule);

    /**
     * @brief 删除下挂所有ICD表
     */
    void clearRule();

    /**
    * @brief 获取所有ICD子表
    * @return 子表名集
    */
    std::vector<std::string> subTableIds() const;

    /**
    * @brief 获取所有ICD子表
    * @return 子表数据集
    */
    tableVector subTables() const;

    /**
    * @brief 查询子表
    * @param [in] name : 子表名（规则序号1/表名1/……/规则序号n/目标表）
    * @return 子表名集
    */
    TableNode::smtTable subTable(const std::string &name) const;

    /**
    * @brief 更新子表长度
    */
    void updateLength(const std::string &name);

    /**
     * @brief 克隆（深拷贝）
     * @return ICD表实例
     */
    TableNode::smtTable clone() const;

    // 设置/获取ID
    void setId(std::string id) override;
    std::string id() const override;

    // 设置/获取名称
    void setName(const std::string& name) override;
    std::string name() const override;

    void setProCode(const std::string& proCode) override;
    std::string proCode() const override;

    // 设置/获取标识
    void setSign(const std::string& sign) override;
    std::string sign() const override;

    // 设置/获取描述
    void setDescribe(const std::string& describe) override;
    std::string describe() const override;

protected:
    TableNode(const TableNode &rhs);
    TableNode &operator = (const TableNode &rhs);

    void calculateLength();

private:
    TableNodePrivate    *d;
};

#endif // TABLENODE_H
