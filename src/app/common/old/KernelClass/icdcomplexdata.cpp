#include "icdcomplexdata.h"


class ICDComplexDataPrivate
{
    friend class ICDComplexData;
public:
    ICDComplexDataPrivate()
    {}

private:
    // 子表数据<表名, <规则数据集>>
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
 * @brief 设置表数据
 * @param [in] table : 表数据
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
        // 设置复合表
        _table->setComplexity(true);
    }
    setRule(stringValue());
    calculateLength();
}

/**
 * @brief 增加表数据
 * @param [in] table : 表数据
 * @return 执行结果，true：成功；false：失败
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
    // 设置复合表
    _table->setComplexity(true);
    setRule(stringValue());
    calculateLength();
}

/**
* @brief 获取所有表数据
* @return 表数据集
*/
TableNode::tableVector ICDComplexData::allTable() const
{
    return d->q_table;
}

/**
* @brief 获取所有表名（包括子表）
* @return 表名集
*/
std::vector<std::string> ICDComplexData::tableNames() const
{
    std::vector<std::string> result;
    std::vector<std::string> subResult;
    const int count = d->q_table.size();
    TableNode::smtTable table = 0;
    for (int i = 0; i < count; ++i) {
        if (!(table = d->q_table[i])) {
            continue;
        }
        subResult = table->subTableNames();
        if (!subResult.empty()) {
            result.insert(result.end(), subResult.begin(), subResult.end());
        }
    }
    return result;
}

/**
* @brief 获取所有子表数据
* @return 表数据集
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
* @brief 查询表
* @param [in] name : 表名（表名1/表名2/……/目标表）
* @return 子表名集
*/
TableNode::smtTable ICDComplexData::table(const std::string &name) const
{
    TableNode::smtTable result;
    if (name.empty()) {
        return result;
    }
    std::size_t pos = name.find("/");
    std::string subName = name.substr(0, pos);
    const int count = d->q_table.size();
    for (int i = 0; i < count; ++i) {
        if ((result = d->q_table[i]) && result->id() == subName) {
            break;
        } else {
            result = 0;
        }
    }
    if (result) {
        if (std::string::npos != pos) {
            subName = name.substr(pos + strlen("/"));
            result = result->subTable(subName);
        }
    }

    return result;
}

/**
* @brief 更新长度（包括子表）
*/
void ICDComplexData::updateLength(const std ::string &name)
{
    if (!name.empty()) {
        std::size_t pos = name.find("/");
        std::string subName = name.substr(0, pos);
        const int count = d->q_table.size();
        TableNode::smtTable table = 0;
        for (int i = 0; i < count; ++i) {
            if ((table = d->q_table[i]) && table->id() == subName) {
                if (std::string::npos != pos) {
                    subName = (-1 == pos) ? "" : name.substr(pos + strlen("/"));
                    table->updateLength(subName);
                } else {
                    table->updateLength("");
                }
            }
        }
    }
    calculateLength();
}

/**
 * @brief 删除表数据
 * @param [in] table : 表标识集
 * @return 执行结果，true：成功；false：失败
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
 * @brief 删除表中的某一项数据
 * @param [in] name : 表标识
 * @param [in] serial : 表序号
 * @return 执行结果，true：成功；false：失败
 */
bool ICDComplexData::deleteTableItem(const std::string &name, int serial)
{
    TableNode::smtTable _table = this->table(name);
    if (!_table) {
        return _table->deleteRule(std::vector<int>(1, serial));
    }
    calculateLength();

    return false;
}

/**
 * @brief 删除所有表数据
 */
void ICDComplexData::clearTable()
{
    d->q_table.clear();

    setRule(std::string());
    calculateLength();
}

// 将子表数据转换为字符串(子表1@……@子表n)
std::string ICDComplexData::stringValue() const
{
    std::string result;
    const int count = d->q_table.size();
    TableNode::smtTable _table = 0;
    for (int i = 0; i < count; ++i) {
        if (!(_table = d->q_table[i])) {
            continue;
        }
        result.append(_table->icdBase().sName).append("@");
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

    // 只有复合数据类型才设置一致
    if (GlobalDefine::dtComplex == this->type()) {
        TableNode::smtTable smtData = table(rule());
        if (smtData) {
            stICDBase base = smtData->icdBase();

            base.sCode = proCode;
            smtData->setICDBase(base);
        }
    }
}

bool ICDComplexData::hasEmptyTable(std::string &names) const
{
    if (d->q_table.empty()) {
        names = name();
        return true;
    }
    const int count = d->q_table.size();
    for (int i = 0; i < count; ++i) {
        if (d->q_table[i]->isEmpty()) {
            if (GlobalDefine::dtComplex == type()) {
                names = d->q_table[i]->name();
            } else {
                names = name() + "|" + d->q_table[i]->name();
            }
            return true;
        }
    }
    return false;
}

void ICDComplexData::setName(const std::string &name)
{
    ICDCommonData::setName(name);
    // 只有复合数据才与基础数据同步
    if (GlobalDefine::dtComplex == this->type()) {
        TableNode::smtTable smtData = table(rule());
        if (smtData) {
            stICDBase base = smtData->icdBase();

            base.sDescribe = name;
            smtData->setICDBase(base);
        }
    }
}

void ICDComplexData::setRemark(const std::string &remark)
{
    ICDCommonData::setRemark(remark);
    // 只有复合数据才与基础数据同步
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
    // 取所有表中最大的值
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
    // 取所有表中最小的值
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
    // 只有BitMap类型的数据存储的是字节位
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
