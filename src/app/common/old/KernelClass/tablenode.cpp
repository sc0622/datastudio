#include "tablenode.h"
#include "icdcomplexdata.h"
#include "icdbitdata.h"
#include "icdcustomizeddata.h"
#include "icdcheckdata.h"
#include "icdcounterdata.h"
#include "icdframecodedata.h"

using std::make_pair;

class TableNodePrivate
{
    friend class TableNode;
public:
    TableNodePrivate()
        : q_complex(false)
        , q_length(0)
        , q_real(0.0)
    {}

private:
    // 表基本信息
    stICDBase       q_base;
    // 复合表标识
    bool            q_complex;
    // 表长度(bit型长度取整)
    int             q_length;
    // 表真实长度
    double          q_real;
    // 表规则数据 <表序号, 规则数据>
    ICDMetaData::ruleMap    q_rule;
};

TableNode::TableNode(const stICDBase &icdBase, bool complex)
    : ICDElement(GlobalDefine::ntTable)
    , d(new TableNodePrivate())
{
    setICDBase(icdBase);
    setComplexity(complex);
}

TableNode::~TableNode()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

void TableNode::setICDBase(const stICDBase& icdBase)
{
    setID(icdBase.sName);
    setName(icdBase.sDescribe);
    setSign(icdBase.sCode);
    setDescribe(icdBase.sRemark);

    d->q_base = icdBase;
    // 子表不进行长度校验
    if (!icdBase.sParentName.empty()) {
        setLengthCheck(false);
    }
}

stICDBase TableNode::icdBase() const
{
    return d->q_base;
}

void TableNode::setComplexity(bool complex)
{
    d->q_complex = complex;
}

void TableNode::setLengthCheck(bool check) const
{
    d->q_base.check = check;
}

bool TableNode::isEmpty() const
{
    return (d->q_rule.size() > 0) ? false : true;
}

bool TableNode::isComplex() const
{
    return d->q_complex;
}

bool TableNode::hasEmptySubTable(std::string &names) const
{
    if (!isComplex()) {
        return false;
    }
    ICDMetaData::ruleMap::const_iterator it = d->q_rule.begin();
    for (; it != d->q_rule.end(); ++it) {
        const ICDMetaData::smtMeta &meta = it->second;
        if (!meta) {
            continue;
        }
        if (IcdDefine::icdComplex == meta->metaType()) {
            ICDComplexData::smtComplex complex
                = SMT_CONVERT(ICDComplexData, meta);
            if (complex->hasEmptyTable(names) || 0 == complex->byteLength()) {
                names.insert(0, name() + "|");
                return true;
            }
        }
    }
    return false;
}

bool TableNode::lengthCheck() const
{
    return d->q_base.check;
}

int TableNode::length() const
{
    return d->q_length;
}

double TableNode::realLength() const
{
    return d->q_real;
}

std::string TableNode::key() const
{
    return id();
}

/**
 * @brief 设置下挂规则信息
 * @param [in] rule : 规则信息集<规则字号, 规则信息>
 */
void TableNode::setRule(const ICDMetaData::ruleMap &rule)
{
    // 先重置复合表标志
    setComplexity(false);

    d->q_rule = rule;
    ICDFrameCodeData::smtFrameCode frameCode;
    ICDMetaData::ruleMap::const_iterator it = d->q_rule.begin();
    for (; it != d->q_rule.end(); ++it) {
        // 只要有一项规则数据为复合数据，则设置复合表标识
        const ICDMetaData::smtMeta &meta = it->second;
        if (IcdDefine::icdComplex == meta->metaType()) {
            setComplexity(true);
        } else if (GlobalDefine::dtFrameCode == meta->type()) {
            frameCode = std::dynamic_pointer_cast<ICDFrameCodeData>(meta);
            if (frameCode) {
                frameCode->setDefaultStr(frameCode->bindingStringSerial());
            }
        }
    }
    calculateLength();
}

ICDMetaData::ruleMap TableNode::allRule() const
{
    return d->q_rule;
}

/**
 * @brief 获取下挂规则信息
 * @param [in] serial : 规则序号
 * @return 下挂规则信息
 */
ICDMetaData::smtMeta TableNode::rule(int serial) const
{
    ICDMetaData::ruleMap::const_iterator it = d->q_rule.find(serial);
    if (it == d->q_rule.end()) {
        return ICDMetaData::smtMeta();
    }
    return it->second;
}

/**
 * @brief 增加下挂规则信息
 * @param [in] rule : 规则信息
 * @return 执行结果，true：成功；false：失败
 */
bool TableNode::addRule(const ICDMetaData::smtMeta &rule)
{
    if (!rule) {
        return false;
    }

    d->q_rule[rule->serial()] = rule;
    // 规则数据为复合数据，则设置复合表标识
    if (IcdDefine::icdComplex == rule->metaType()) {
        setComplexity(true);
    }
    ICDFrameCodeData::smtFrameCode frameCode
        = std::dynamic_pointer_cast<ICDFrameCodeData>(rule);
    if (frameCode) {
        frameCode->setDefaultStr(frameCode->bindingStringSerial());
    }
    /*
    ICDMetaData::ruleMap::iterator it = d->q_rule.find(rule->serial());
    if (it == d->q_rule.end()) {
        d->q_rule[rule->serial()] = rule;
        //d->q_rule.insert(make_pair(rule->serial(), rule));
        // 规则数据为复合数据，则设置复合表标识
        if (IcdDefine::icdComplex == rule->metaType()) {
            setComplexity(true);
        }
    } else {
        switch (rule->type())
        {
        case GlobalDefine::dtCounter:   // 帧计数
            *std::dynamic_pointer_cast<ICDCounterData>(it->second)
                = *std::dynamic_pointer_cast<ICDCounterData>(rule);
            break;
        case GlobalDefine::dtCheck:     // 校验
            *std::dynamic_pointer_cast<ICDCheckData>(it->second)
                = *std::dynamic_pointer_cast<ICDCheckData>(rule);
            break;
        case GlobalDefine::dtFrameCode: // 帧识别码
            *std::dynamic_pointer_cast<ICDFrameCodeData>(it->second)
                = *std::dynamic_pointer_cast<ICDFrameCodeData>(rule);
            break;
        case GlobalDefine::dtHead:      // 包头
        case GlobalDefine::dtU8:        // 无符号8位
        case GlobalDefine::dt8:         // 有符号8位
        case GlobalDefine::dtU16:       // 无符号16位
        case GlobalDefine::dt16:        // 有符号16位
        case GlobalDefine::dtU32:       // 无符号32位
        case GlobalDefine::dt32:        // 有符号32位
        case GlobalDefine::dtF32:       // 32位浮点数
        case GlobalDefine::dtF64:       // 64位浮点数
            *std::dynamic_pointer_cast<ICDCommonData>(it->second)
                = *std::dynamic_pointer_cast<ICDCommonData>(rule);
            break;
        case GlobalDefine::dtBitMap:   // BITMAP
        case GlobalDefine::dtBitValue: // BITVALUE
            *std::dynamic_pointer_cast<ICDBitData>(it->second)
                = *std::dynamic_pointer_cast<ICDBitData>(rule);
            break;
        case GlobalDefine::dtComplex:  // 复合数据
        case GlobalDefine::dtDiscern:  // 帧数据
        {
            *std::dynamic_pointer_cast<ICDComplexData>(it->second)
                = *std::dynamic_pointer_cast<ICDComplexData>(rule);
        }
            break;
        }
    }
    */
    calculateLength();

    return true;
}

/**
 * @brief 删除下规则信息
 * @param [in] rule : 规则序号
 * @return 执行结果，true：成功；false：失败
 */
bool TableNode::deleteRule(const std::vector<int> &rule)
{
    bool dirty = false; // 删除帧数据后，需要重置帧识别码数据
    const int count(rule.size());
    ICDMetaData::ruleMap::iterator it = d->q_rule.end();
    for (int i = 0; i < count; ++i) {
        it = d->q_rule.find(rule.at(i));
        if (it != d->q_rule.end()) {
            ICDMetaData::smtMeta meta = it->second;
            // 帧数据
            if (meta && GlobalDefine::dtDiscern == meta->type()) {
                dirty = true;
            }
            d->q_rule.erase(it);
        }
    }
    // 
    if (dirty) {
        ICDMetaData::ruleMap::iterator it = d->q_rule.end();
        for (; it != d->q_rule.end(); ++it) {
            // 帧识别码
            ICDFrameCodeData::smtFrameCode frameCode
                = std::dynamic_pointer_cast<ICDFrameCodeData>(it->second);
            if (frameCode) {
                // 查询不到数据，证明绑定的帧数据已经被删除
                if (!(this->rule(frameCode->bindingSerial()))) {
                    frameCode->bindData(0);
                }
            }
        }
    }
    calculateLength();

    return true;
}

/**
 * @brief 删除下挂所有ICD表
 */
void TableNode::clearRule()
{
    d->q_rule.clear();
    // 重置复合表标志
    setComplexity(false);
    calculateLength();
}

/**
* @brief 获取所有ICD子表
* @return 子表名集
*/
std::vector<std::string> TableNode::subTableNames() const
{
    std::vector<std::string> result;
    if (!isComplex()) {
        return result;
    }
    std::vector<std::string> subResult;
    ICDMetaData::ruleMap::const_iterator it = d->q_rule.begin();
    for (; it != d->q_rule.end(); ++it) {
        const ICDMetaData::smtMeta &meta = it->second;
        if (IcdDefine::icdComplex == meta->metaType()) {
            ICDComplexData::smtComplex complex
                = std::dynamic_pointer_cast<ICDComplexData>(meta);
            subResult = complex->tableNames();
            result.insert(result.end(), subResult.begin(), subResult.end());
        }
    }
    return result;
}

/**
* @brief 获取所有ICD子表
* @return 子表数据集
*/
TableNode::tableVector TableNode::subTables() const
{
    TableNode::tableVector result;
    if (!isComplex()) {
        return result;
    }
    TableNode::tableVector subResult;
    ICDMetaData::ruleMap::const_iterator it = d->q_rule.begin();
    for (; it != d->q_rule.end(); ++it) {
        const ICDMetaData::smtMeta &meta = it->second;
        if (IcdDefine::icdComplex == meta->metaType()) {
            ICDComplexData::smtComplex complex
                = SMT_CONVERT(ICDComplexData, meta);
            subResult = complex->subTables();
            result.insert(result.end(), subResult.begin(), subResult.end());
        }
    }
    return result;
}

/**
* @brief 查询子表
* @param [in] name : 子表名（规则序号1/表名1/……/规则序号n/目标表）
* @return 子表名集
*/
TableNode::smtTable TableNode::subTable(const std::string &name) const
{
    TableNode::smtTable result;
    if (name.empty() || !isComplex()) {
        return result;
    }
    std::size_t pos = name.find("/");
    std::string subName = name.substr(0, pos);
    ICDMetaData::ruleMap::const_iterator it 
        = d->q_rule.find(atoi(subName.c_str()));
    ICDComplexData::smtComplex complex = 0;
    if (it != d->q_rule.end()) {
        if (complex = SMT_CONVERT(ICDComplexData, it->second)) {
            result = complex->table(name.substr(pos + strlen("/")));
        }
    }

    return result;
}

/**
* @brief 更新子表长度
*/
void TableNode::updateLength(const std::string &name)
{
    if (!isComplex()) {
        return;
    }
    if (!name.empty()) {
        std::size_t pos = name.find("/");
        std::string subName = name.substr(0, pos);
        ICDMetaData::ruleMap::const_iterator it
            = d->q_rule.find(atoi(subName.c_str()));
        ICDComplexData::smtComplex complex = 0;
        if (it != d->q_rule.end()) {
            if (complex = SMT_CONVERT(ICDComplexData, it->second)) {
                subName = (-1 == pos) ? "" : name.substr(pos + strlen("/"));
                complex->updateLength(subName);
                // 更新字节序号
                bool reorder = true;
                int offset = complex->index() + complex->byteLength();
                for (++it; it != d->q_rule.end(); ++it) {
                    const ICDMetaData::smtMeta &meta = it->second;
                    if (!meta) {
                        continue;
                    }
                    if (reorder) {
                        reorder = false;
                        offset -= meta->index();
                    }
                    if (0 == offset) {
                        break;  // 并未变更数据长度，不需要更新字节号
                    }
                    meta->setIndex(meta->index() + offset);
                }
            }
        }
    }
    calculateLength();
}

/**
 * @brief 克隆（深拷贝）
 * @return ICD表实例
 */
TableNode::smtTable TableNode::clone() const
{
    TableNode::smtTable table(new TableNode(*this));

    return table;
}

TableNode::TableNode(const TableNode &rhs)
    : ICDElement(rhs)
    , d(new TableNodePrivate())
{
    *this = rhs;
}

TableNode &TableNode::operator =(const TableNode &rhs)
{
    if (&rhs == this) {
        return *this;
    }
    *(ICDElement *)this = rhs;

    d->q_base = rhs.d->q_base;
    d->q_complex = rhs.d->q_complex;
    d->q_length = rhs.d->q_length;
    d->q_real = rhs.d->q_real;

    ICDMetaData::ruleMap cloneRules;
    std::map<int, ICDFrameCodeData::smtFrameCode> codes;    // 帧识别码
    ICDMetaData::ruleMap::const_iterator it = rhs.d->q_rule.begin();
    for (; it != rhs.d->q_rule.end(); ++it) {
        const ICDMetaData::smtMeta &meta = it->second;
        if (meta) {
            ICDMetaData::smtMeta cloneMeta = meta->clone();
            cloneRules[cloneMeta->serial()] = cloneMeta;
            if (GlobalDefine::dtFrameCode == meta->type()) {
                ICDFrameCodeData::smtFrameCode framCode
                    = std::dynamic_pointer_cast<ICDFrameCodeData>(meta);
                if (framCode) {
                    codes[atoi(framCode->defaultStr().c_str())] = framCode;
                }
            }
        }
    }
    setRule(cloneRules);
    // 将帧识别码和帧数据建立连接
    std::map<int, ICDFrameCodeData::smtFrameCode>::iterator itC;
    for (itC = codes.begin(); itC != codes.end(); ++itC) {
        ICDFrameCodeData::smtFrameCode &frameCode = itC->second;
        ICDComplexData::smtComplex complex =
            std::dynamic_pointer_cast<ICDComplexData>(rule(itC->first));
        if (frameCode && complex) {
            frameCode->bindData(complex);
        }
    }

    return *this;
}

void TableNode::calculateLength()
{
    d->q_length = 0;
    d->q_real = 0.0;

    ICDMetaData::smtMeta last = 0;
    ICDMetaData::ruleMap::const_iterator it = d->q_rule.begin();
    for (; it != d->q_rule.end(); ++it) {
        last = it->second;
        d->q_real += last->realLength();
    }
    if (last) {
        d->q_length = last->index() + last->byteLength();
    }
    if (!lengthCheck()) {
        d->q_base.nLength = d->q_length;
    }
}
