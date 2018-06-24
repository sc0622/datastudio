#include "systemnode.h"
#include "tablenode.h"


class SystemNodePrivate
{
    friend class SystemNode;
public:
    SystemNodePrivate() : q_code(-1)
    {}

private:
    int     q_code;     // ϵͳ���
    // �¹ұ� 
    TableNode::tableVector  q_table;
};
/**
 * @brief ���캯��
 * @param [in] id : ��ϵͳ��
 * @param [in] name : ��ϵͳ����
 * @param [in] describe : ����
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
 * @brief ���캯��
 * @param [in] plane : ��ϵͳ������Ϣ
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

    ICDElement::setId(std::string(szTmp));

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
 * @brief ��ȡ�¹�ICD����Ϣ
 * @param [in] name : ICD���ʶ
 * @return �¹�ICD����Ϣ
 */
TableNode::smtTable SystemNode::table(const std::string &id) const
{
    TableNode::smtTable result = nullptr;
    const int count = d->q_table.size();
    for (int i = 0; i < count; ++i) {
        if ((result = d->q_table[i]) && result->key() == id) {
            break;
        } else {
            result = nullptr;
        }
    }

    return result;
}

/**
 * @brief �����¹ҷ�ICD��
 * @param [in] table : ICD����Ϣ��
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
 * @brief ɾ���¹ҷ�ICD��
 * @param [in] table : ICD���ʶ��
 * @return ִ�н����true���ɹ���false��ʧ��
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
 * @brief ɾ���¹�����ICD��
 */
void SystemNode::clearTable()
{
    d->q_table.clear();
}

/**
 * @brief ��¡�������
 * @return ��ϵͳʵ��
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
