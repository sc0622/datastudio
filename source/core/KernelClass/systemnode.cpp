#include "systemnode.h"
#include "tablenode.h"


class SystemNodePrivate
{
    friend class SystemNode;
public:
    SystemNodePrivate() : q_code(-1)
    {}

private:
    int     q_code;     // 系统编号
    // 下挂表 
    TableNode::tableVector  q_table;
};
/**
 * @brief 构造函数
 * @param [in] id : 分系统码
 * @param [in] name : 分系统名称
 * @param [in] describe : 描述
 */
SystemNode::SystemNode(int id,
                       const std::string &name,
                       const std::string &sign,
                       const std::string &describe)
                       : ICDElement(GlobalDefine::ntSystem)
                       , d(new SystemNodePrivate())
{
    setNumeralId(id);
    setName(name);
    setSign(sign);
    setDescribe(describe);
}

/**
 * @brief 构造函数
 * @param [in] plane : 分系统基本信息
 */
SystemNode::SystemNode(const stSystem &system)
    : ICDElement(GlobalDefine::ntSystem)
    , d(new SystemNodePrivate())
{
    setNumeralId(system.nCode);
    setName(system.sName);
    setSign(system.sSign);
    setDescribe(system.sDescribe);
}

SystemNode::~SystemNode()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

void SystemNode::setSystem(const stSystem& system)
{
    setNumeralId(system.nCode);
    setName(system.sName);
    setSign(system.sSign);
    setDescribe(system.sDescribe);
}

stSystem SystemNode::system() const
{
    stSystem system;

    system.nCode = numeralId();
    system.sName = name();
    system.sSign = sign();
    system.sDescribe = describe();

    return system;
}

void SystemNode::setNumeralId(int id)
{
    char szTmp[32] = {0};
    sprintf_s(szTmp, sizeof(szTmp), "%d", id);

    ICDElement::setID(std::string(szTmp));

    d->q_code = id;
}

int SystemNode::numeralId() const
{
    return d->q_code;
}

void SystemNode::setTable(const TableNode::tableVector& table)
{
    d->q_table = table;
}

TableNode::tableVector SystemNode::allTable() const
{
    return d->q_table;
}

bool SystemNode::isEmpty() const
{
    return d->q_table.empty();
}

/**
 * @brief 获取下挂ICD表信息
 * @param [in] name : ICD表标识
 * @return 下挂ICD表信息
 */
TableNode::smtTable SystemNode::table(const std::string &name) const
{
    TableNode::smtTable result = 0;
    const int count = d->q_table.size();
    for (int i = 0; i < count; ++i) {
        if ((result = d->q_table[i]) && result->key() == name) {
            break;
        } else {
            result = 0;
        }
    }

    return result;
}

/**
 * @brief 增加下挂分ICD表
 * @param [in] table : ICD表信息集
 */
void SystemNode::addTable(const TableNode::smtTable &table)
{
    if (!table) {
        return;
    }
    TableNode::smtTable _table = 0;
    const int count = d->q_table.size();
    for (int i = 0; i < count; ++i) {
        if ((_table = d->q_table[i]) && _table->id() == table->id()) {
            d->q_table[i] = table;

            break;
        } else {
            _table = 0;
        }
    }
    if (!_table) {
        d->q_table.push_back(table);
    }
}

/**
 * @brief 删除下挂分ICD表
 * @param [in] table : ICD表标识集
 * @return 执行结果，true：成功；false：失败
 */
bool SystemNode::deleteTable(const std::vector<std::string> &table)
{
    const int count(table.size());
    TableNode::smtTable _table = 0;
    TableNode::tableVector::iterator it = d->q_table.begin();
    for (int i = 0; i < count; ++i) {
        for (it = d->q_table.begin(); it != d->q_table.end(); ++it) {
            if ((_table = *it) && _table->id() == table[i]) {
                d->q_table.erase(it);
                break;
            }
        }
    }

    return true;
}

/**
 * @brief 删除下挂所有ICD表
 */
void SystemNode::clearTable()
{
    d->q_table.clear();
}

/**
 * @brief 克隆（深拷贝）
 * @return 分系统实例
 */
SystemNode::smtSystem SystemNode::clone() const
{
    SystemNode::smtSystem system(new SystemNode(*this));

    return system;
}

SystemNode::SystemNode(const SystemNode &rhs)
    : ICDElement(rhs)
    , d(new SystemNodePrivate())
{
    *this = rhs;
}

SystemNode &SystemNode::operator =(const SystemNode &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    *(ICDElement *)this = rhs;
    d->q_code = rhs.d->q_code;

    TableNode::tableVector cloneTable;
    const int count = rhs.d->q_table.size();
    for (int i = 0; i < count; ++i) {
        const TableNode::smtTable &table = rhs.d->q_table[i];
        if (table) {
            cloneTable.push_back(table->clone());
        }
    }
    setTable(cloneTable);

    return *this;
}
