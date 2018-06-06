#ifndef ICDCOMPLEXDATA_H
#define ICDCOMPLEXDATA_H

#include "icdcommondata.h"
#include "tablenode.h"


class ICDComplexDataPrivate;
/**
 * @brief ICD复合数据类
 */
class KERNELCLASS_EXPORT ICDComplexData
        : public ICDCommonData
{
public:
    typedef std::shared_ptr<ICDComplexData> smtComplex;

public:
    ICDComplexData(const stTableRules &rules);
    ICDComplexData(const ICDComplexData &rhs);
    ICDComplexData &operator = (const ICDComplexData &rhs);
    ~ICDComplexData();
    // ICDMetaData interface
    ICDMetaData::smtMeta clone() const;
    virtual void setName(const std::string &name) override;
    virtual void setRemark(const std::string &remark) override;
    virtual int byteLength() const override;
    virtual double realLength() const override;
    virtual std::string lengthOfByte() const override;
    // ICDCommonData interface
    virtual void setProCode(const std::string& proCode) override;

    /**
    * @brief 验证是否有空表
    * @param [in] names : 空表名字（父表到子表）
    * @return true：有；false：没有
    */
    bool hasEmptyTable(std::string &names) const;

    /**
     * @brief 设置表数据
     * @param [in] table : 表数据
     */
    void setTable(const TableNode::tableVector& table);

    /**
     * @brief 增加表数据
     * @param [in] table : 表数据
     * @return 执行结果，true：成功；false：失败
     */
    void addTable(const TableNode::smtTable& table);

    /**
     * @brief 获取所有表数据
     * @return 表数据集
     */
    TableNode::tableVector allTable() const;

    /**
    * @brief 获取所有表名（包括子表）
    * @return 表名集
    */
    std::vector<std::string> tableNames() const;

    /**
    * @brief 获取所有子表数据
    * @return 表数据集
    */
    TableNode::tableVector subTables() const;

    /**
    * @brief 查询表
    * @param [in] name : 表名（表名1/表名2/……/目标表）
    * @return 子表名集
    */
    TableNode::smtTable table(const std ::string &name) const;

    /**
    * @brief 更新长度（包括子表）
    */
    void updateLength(const std ::string &name);

    /**
     * @brief 删除表数据
     * @param [in] table : 表标识集
     * @return 执行结果，true：成功；false：失败
     */
    bool deleteTable(const std::vector<std::string> &table);

    /**
     * @brief 删除表中的某一项数据
     * @param [in] name : 表标识
     * @param [in] serial : 表序号
     * @return 执行结果，true：成功；false：失败
     */
    bool deleteTableItem(const std::string& name, int serial);

    /**
     * @brief 删除所有表数据
     */
    void clearTable();

protected:
    // 将子表数据转换为字符串(子表1@……@子表n)
    virtual std::string stringValue() const;
    void calculateLength();

private:
    ICDComplexDataPrivate   *d;
};

#endif // ICDCOMPLEXDATA_H
