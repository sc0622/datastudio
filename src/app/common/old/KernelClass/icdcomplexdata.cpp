#include "icdcomplexdata.h"


class ICDComplexDataPrivate
{
    friend class ICDComplexData;
public:
    ICDComplexDataPrivate()
    {}

private:
    // �ӱ�����<����, <�������ݼ�>>
    TableNode::tableVector  q_table;
};

ICDComplexData::ICDComplexData(const stTableRules &rules)
    : ICDCommonData(rules)
    , d(new ICDComplexDataPrivate())
{
    q_cType = IcdDefine::icdComplex;
}

ICDComplexData::~ICDComplexData()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

/**
 * @brief ���ñ�����
 * @param [in] table : ������
 */
void ICDComplexData::setTable(const TableNode::tableVector &table)
{
    d->q_table = table;
    const int count = d->q_table.size();
    for (int i = 0; i < count; ++i) {
        TableNode::smtTable &_table = d->q_table[i];
        if (!_table) {
            continue;
        }
        // ���ø��ϱ�
        _table->setComplexity(true);
    }
    setRule(stringValue());
    calculateLength();
}

/**
 * @brief ���ӱ�����
 * @param [in] table : ������
 * @return ִ�н����true���ɹ���false��ʧ��
 */
void ICDComplexData::addTable(const TableNode::smtTable &table)
{
    if (!table) {
        return;
    }
    TableNode::smtTable _table = 0;
    const int count = d->q_table.size();
    for (int i = 0; i < count; ++i) {
        if ((_table = d->q_table[i]) && _table->id() == table->id()) {
            d->q_table[i] = table;
            _table = table;

            break;
        } else {
            _table = 0;
        }
    }
    if (!_table) {
        _table = table;
        d->q_table.push_back(table);
    }
    // ���ø��ϱ�
    _table->setComplexity(true);
    setRule(stringValue());
    calculateLength();
}

/**
* @brief ��ȡ���б�����
* @return �����ݼ�
*/
TableNode::tableVector ICDComplexData::allTable() const
{
    return d->q_table;
}

/**
* @brief ��ȡ���б����������ӱ�
* @return ������
*/
std::vector<std::string> ICDComplexData::tableIds() const
{
    std::vector<std::string> result;
    std::vector<std::string> subResult;
    const int count = d->q_table.size();
    TableNode::smtTable table = 0;
    for (int i = 0; i < count; ++i) {
        if (!(table = d->q_table[i])) {
            continue;
        }
        subResult = table->subTableIds();
        if (!subResult.empty()) {
            result.insert(result.end(), subResult.begin(), subResult.end());
        }
    }
    return result;
}

/**
* @brief ��ȡ�����ӱ�����
* @return �����ݼ�
*/
TableNode::tableVector ICDComplexData::subTables() const
{
    TableNode::tableVector result;
    TableNode::tableVector subResult;
    const int count = d->q_table.size();
    TableNode::smtTable table = 0;
    for (int i = 0; i < count; ++i) {
        if (!(table = d->q_table[i])) {
            continue;
        }
        result.push_back(table);
        subResult = table->subTables();
        if (!subResult.empty()) {
            result.insert(result.end(), subResult.begin(), subResult.end());
        }
    }

    return result;
}

/**
* @brief ��ѯ��
* @param [in] name : ����������1/����2/����/Ŀ���
* @return �ӱ�����
*/
TableNode::smtTable ICDComplexData::table(const std::string &id) const
{
    TableNode::smtTable result;
    if (id.empty()) {
        return result;
    }
    std::size_t pos = id.find("/");
    std::string subId = id.substr(0, pos);
    const int count = d->q_table.size();
    for (int i = 0; i < count; ++i) {
        if ((result = d->q_table[i]) && result->id() == subId) {
            break;
        } else {
            result = 0;
        }
    }
    if (result) {
        if (std::string::npos != pos) {
            subId = id.substr(pos + strlen("/"));
            result = result->subTable(subId);
        }
    }

    return result;
}

/**
* @brief ���³��ȣ������ӱ�
*/
void ICDComplexData::updateLength(const std ::string &id)
{
    if (!id.empty()) {
        std::size_t pos = id.find("/");
        std::string subId = id.substr(0, pos);
        const int count = d->q_table.size();
        TableNode::smtTable table = 0;
        for (int i = 0; i < count; ++i) {
            if ((table = d->q_table[i]) && table->id() == subId) {
                if (std::string::npos != pos) {
                    subId = (-1 == pos) ? "" : id.substr(pos + strlen("/"));
                    table->updateLength(subId);
                } else {
                    table->updateLength("");
                }
            }
        }
    }
    calculateLength();
}

/**
 * @brief ɾ��������
 * @param [in] table : ���ʶ��
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool ICDComplexData::deleteTable(const std::vector<std::string> &table)
{
    const int count(table.size());
    TableNode::smtTable _table = 0;
    TableNode::tableVector::iterator it = d->q_table.end();
    for (int i = 0; i < count; ++i) {
        for (it = d->q_table.begin(); it != d->q_table.end(); ++it) {
            if ((_table = *it) && _table->id() == table[i]) {
                d->q_table.erase(it);
                break;
            }
        }
    }
    setRule(stringValue());
    calculateLength();

    return true;
}

/**
 * @brief ɾ�����е�ĳһ������
 * @param [in] name : ���ʶ
 * @param [in] serial : �����
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool ICDComplexData::deleteTableItem(const std::string &id, int serial)
{
    TableNode::smtTable _table = this->table(id);
    if (!_table) {
        return _table->deleteRule(std::vector<int>(1, serial));
    }
    calculateLength();

    return false;
}

/**
 * @brief ɾ�����б�����
 */
void ICDComplexData::clearTable()
{
    d->q_table.clear();
    setRule(std::string());
    calculateLength();
}

// ���ӱ�����ת��Ϊ�ַ���(�ӱ�1@����@�ӱ�n)
std::string ICDComplexData::stringValue() const
{
    std::string result;
    const int count = d->q_table.size();
    TableNode::smtTable _table = 0;
    for (int i = 0; i < count; ++i) {
        if (!(_table = d->q_table[i])) {
            continue;
        }
        result.append(_table->icdBase().sID).append("@");
    }
    result = result.substr(0, result.find_last_of("@"));

    return result;
}

void ICDComplexData::calculateLength()
{
    setLength(byteLength());
}

void ICDComplexData::setProCode(const std::string& proCode)
{
    ICDCommonData::setProCode(proCode);

    // ֻ�и����������Ͳ�����һ��
    if (GlobalDefine::dtComplex == this->type()) {
        TableNode::smtTable smtData = table(rule());
        if (smtData) {
            stICDBase base = smtData->icdBase();
            base.sCode = proCode;
            smtData->setICDBase(base);
        }
    }
}

bool ICDComplexData::hasEmptyTable(std::string &tableIds) const
{
    if (d->q_table.empty()) {
        tableIds = id();
        return true;
    }
    const int count = d->q_table.size();
    for (int i = 0; i < count; ++i) {
        if (d->q_table[i]->isEmpty()) {
            if (GlobalDefine::dtComplex == type()) {
                tableIds = d->q_table[i]->id();
            } else {
                tableIds = id() + "|" + d->q_table[i]->id();
            }
            return true;
        }
    }
    return false;
}

void ICDComplexData::setName(const std::string &name)
{
    ICDCommonData::setName(name);
    // ֻ�и������ݲ����������ͬ��
    if (GlobalDefine::dtComplex == this->type()) {
        TableNode::smtTable smtData = table(rule());
        if (smtData) {
            stICDBase base = smtData->icdBase();
            base.sName = name;
            smtData->setICDBase(base);
        }
    }
}

void ICDComplexData::setRemark(const std::string &remark)
{
    ICDCommonData::setRemark(remark);
    // ֻ�и������ݲ����������ͬ��
    if (GlobalDefine::dtComplex == this->type()) {
        TableNode::smtTable smtData = table(rule());
        if (smtData) {
            stICDBase base = smtData->icdBase();
            base.sRemark = remark;
            smtData->setICDBase(base);
        }
    }
}

int ICDComplexData::byteLength() const
{
    // ȡ���б�������ֵ
    int result = 0;
    const int count = d->q_table.size();
    for (int i = 0; i < count; ++i) {
        if (d->q_table[i]->length() > result) {
            result = d->q_table[i]->length();
        }
    }

    return result;
}

double ICDComplexData::realLength() const
{
    // ȡ���б�����С��ֵ
    double result = 0.0;
    const int count = d->q_table.size();
    if (count > 0) {
        result = d->q_table[0]->realLength();
        for (int i = 0; i < count; ++i) {
            if (d->q_table[i]->realLength() < result) {
                result = d->q_table[i]->length();
            }
        }
    }

    return result;
}

std::string ICDComplexData::lengthOfByte() const
{
    char szTmp[128] = {0};
    // ֻ��BitMap���͵����ݴ洢�����ֽ�λ
    sprintf_s(szTmp, sizeof(szTmp), "%d", byteLength());

    return std::string(szTmp);
}

ICDComplexData::ICDComplexData(const ICDComplexData &rhs)
    : ICDCommonData(rhs)
    , d(new ICDComplexDataPrivate())
{
    *this = rhs;
}

ICDComplexData &ICDComplexData::operator =(const ICDComplexData &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    *(ICDCommonData*)this = rhs;

    d->q_table.clear();
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


ICDMetaData::smtMeta ICDComplexData::clone() const
{
    ICDComplexData::smtComplex complex(new ICDComplexData(*this));
    return complex;
}
