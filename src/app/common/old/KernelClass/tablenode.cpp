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
    // �������Ϣ
    stICDBase       q_base;
    // ���ϱ��ʶ
    bool            q_complex;
    // ����(bit�ͳ���ȡ��)
    int             q_length;
    // ����ʵ����
    double          q_real;
    // ��������� <�����, ��������>
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
    // �ӱ����г���У��
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
 * @brief �����¹ҹ�����Ϣ
 * @param [in] rule : ������Ϣ��<�����ֺ�, ������Ϣ>
 */
void TableNode::setRule(const ICDMetaData::ruleMap &rule)
{
    // �����ø��ϱ��־
    setComplexity(false);

    d->q_rule = rule;
    ICDFrameCodeData::smtFrameCode frameCode;
    ICDMetaData::ruleMap::const_iterator it = d->q_rule.begin();
    for (; it != d->q_rule.end(); ++it) {
        // ֻҪ��һ���������Ϊ�������ݣ������ø��ϱ��ʶ
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
 * @brief ��ȡ�¹ҹ�����Ϣ
 * @param [in] serial : �������
 * @return �¹ҹ�����Ϣ
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
 * @brief �����¹ҹ�����Ϣ
 * @param [in] rule : ������Ϣ
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool TableNode::addRule(const ICDMetaData::smtMeta &rule)
{
    if (!rule) {
        return false;
    }

    d->q_rule[rule->serial()] = rule;
    // ��������Ϊ�������ݣ������ø��ϱ��ʶ
    if (IcdDefine::icdComplex == rule->metaType()) {
        setComplexity(true);
    }
    ICDFrameCodeData::smtFrameCode frameCode = std::dynamic_pointer_cast<ICDFrameCodeData>(rule);
    if (frameCode) {
        frameCode->setDefaultStr(frameCode->bindingStringSerial());
    }
    /*
    ICDMetaData::ruleMap::iterator it = d->q_rule.find(rule->serial());
    if (it == d->q_rule.end()) {
        d->q_rule[rule->serial()] = rule;
        //d->q_rule.insert(make_pair(rule->serial(), rule));
        // ��������Ϊ�������ݣ������ø��ϱ��ʶ
        if (IcdDefine::icdComplex == rule->metaType()) {
            setComplexity(true);
        }
    } else {
        switch (rule->type())
        {
        case GlobalDefine::dtCounter:   // ֡����
            *std::dynamic_pointer_cast<ICDCounterData>(it->second)
                = *std::dynamic_pointer_cast<ICDCounterData>(rule);
            break;
        case GlobalDefine::dtCheck:     // У��
            *std::dynamic_pointer_cast<ICDCheckData>(it->second)
                = *std::dynamic_pointer_cast<ICDCheckData>(rule);
            break;
        case GlobalDefine::dtFrameCode: // ֡ʶ����
            *std::dynamic_pointer_cast<ICDFrameCodeData>(it->second)
                = *std::dynamic_pointer_cast<ICDFrameCodeData>(rule);
            break;
        case GlobalDefine::dtHead:      // ��ͷ
        case GlobalDefine::dtU8:        // �޷���8λ
        case GlobalDefine::dt8:         // �з���8λ
        case GlobalDefine::dtU16:       // �޷���16λ
        case GlobalDefine::dt16:        // �з���16λ
        case GlobalDefine::dtU32:       // �޷���32λ
        case GlobalDefine::dt32:        // �з���32λ
        case GlobalDefine::dtU64:       // �޷���64λ
        case GlobalDefine::dt64:        // �з���64λ
        case GlobalDefine::dtF32:       // 32λ������
        case GlobalDefine::dtF64:       // 64λ������
            *std::dynamic_pointer_cast<ICDCommonData>(it->second)
                = *std::dynamic_pointer_cast<ICDCommonData>(rule);
            break;
        case GlobalDefine::dtBitMap:   // BITMAP
        case GlobalDefine::dtBitValue: // BITVALUE
            *std::dynamic_pointer_cast<ICDBitData>(it->second)
                = *std::dynamic_pointer_cast<ICDBitData>(rule);
            break;
        case GlobalDefine::dtComplex:  // ��������
        case GlobalDefine::dtDiscern:  // ֡����
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
 * @brief ɾ���¹�����Ϣ
 * @param [in] rule : �������
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool TableNode::deleteRule(const std::vector<int> &rule)
{
    bool dirty = false; // ɾ��֡���ݺ���Ҫ����֡ʶ��������
    const int count(rule.size());
    ICDMetaData::ruleMap::iterator it = d->q_rule.end();
    for (int i = 0; i < count; ++i) {
        it = d->q_rule.find(rule.at(i));
        if (it != d->q_rule.end()) {
            ICDMetaData::smtMeta meta = it->second;
            // ֡����
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
            // ֡ʶ����
            ICDFrameCodeData::smtFrameCode frameCode
                = std::dynamic_pointer_cast<ICDFrameCodeData>(it->second);
            if (frameCode) {
                // ��ѯ�������ݣ�֤���󶨵�֡�����Ѿ���ɾ��
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
 * @brief ɾ���¹�����ICD��
 */
void TableNode::clearRule()
{
    d->q_rule.clear();
    // ���ø��ϱ��־
    setComplexity(false);
    calculateLength();
}

/**
* @brief ��ȡ����ICD�ӱ�
* @return �ӱ�����
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
* @brief ��ȡ����ICD�ӱ�
* @return �ӱ����ݼ�
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
* @brief ��ѯ�ӱ�
* @param [in] name : �ӱ������������1/����1/����/�������n/Ŀ���
* @return �ӱ�����
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
* @brief �����ӱ���
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
                // �����ֽ����
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
                        break;  // ��δ������ݳ��ȣ�����Ҫ�����ֽں�
                    }
                    meta->setIndex(meta->index() + offset);
                }
            }
        }
    }
    calculateLength();
}

/**
 * @brief ��¡�������
 * @return ICD��ʵ��
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
    std::unordered_map<int, ICDFrameCodeData::smtFrameCode> codes;    // ֡ʶ����
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
    // ��֡ʶ�����֡���ݽ�������
    std::unordered_map<int, ICDFrameCodeData::smtFrameCode>::iterator itC;
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
