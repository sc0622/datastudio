#ifndef TABLENODE_H
#define TABLENODE_H
#include "icdmetadata.h"
#include <vector>

class TableNodePrivate;
/**
 * @brief ICD����
 */
class KERNELCLASS_EXPORT TableNode : public ICDElement
{
public:
    typedef std::shared_ptr<TableNode> smtTable;
    typedef std::unordered_map<std::string, TableNode::smtTable> tableMap;
    typedef std::vector<TableNode::smtTable> tableVector;

public:

    /**
     * @brief ���캯��
     * @param [in] icdBase : �������Ϣ
     */
    TableNode(const stICDBase& icdBase, bool complex = false);
    ~TableNode();

    /**
     * @brief ���ñ������Ϣ
     * @param [in] icdBase : �������Ϣ
     */
    void setICDBase(const stICDBase& icdBase);

    /**
     * @brief ��ȡ�������Ϣ
     * @return �������Ϣ
     */
    stICDBase icdBase() const;

    /**
     * @brief ���ø��ϱ�״̬
     * @param [in] complex : ����״̬
     */
    void setComplexity(bool complex);

    /**
     * @brief ���ó���У��
     * @param [in] check : У��״̬
     */
    void setLengthCheck(bool check) const;

    /**
     * @brief ��֤������Ϣ�Ƿ�Ϊ��
     * @return ������ϢΪ�ձ�ʶ
     */
    bool isEmpty() const;

    /**
     * @brief ��֤�����Ƿ񸴺ϱ�
     * @return ���ϱ�״̬��true���ǣ�false������
     */
    bool isComplex() const;

    /**
    * @brief ��֤�����Ƿ��п��ӱ�
    * @param [in] names : �ձ����֣������ӱ�
    * @return true���У�false��û��
    */
    bool hasEmptySubTable(std::string &names) const;

    /**
     * @brief У�鳤��
     * @return ��������ʵ����һ��ʱ����true������false
     */
    bool lengthCheck() const;

    /**
     * @brief �����ֽڳ���
     * @return
     */
    int length() const;

    /**
     * @brief ������ʵ����
     * @return
     */
    double realLength() const;

    /**
     * @brief ��ȡ���ʶ
     * @return ���ʶ
     */
    std::string key() const;

    /**
     * @brief �����¹ҹ�����Ϣ
     * @param [in] rule : ������Ϣ��<�����ֺ�, ������Ϣ>
     */
    void setRule(const ICDMetaData::ruleMap& rule);

    /**
     * @brief ��ȡ�����¹ҹ�����Ϣ
     * @return �����¹ҹ�����Ϣ
     */
    ICDMetaData::ruleMap allRule() const;

    /**
     * @brief ��ȡ�¹ҹ�����Ϣ
     * @param [in] serial : �������
     * @return �¹ҹ�����Ϣ
     */
    ICDMetaData::smtMeta rule(int serial) const;

    /**
     * @brief �����¹ҹ�����Ϣ
     * @param [in] rule : ������Ϣ
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool addRule(const ICDMetaData::smtMeta& rule);

    /**
     * @brief ɾ���¹�����Ϣ
     * @param [in] rule : �������
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool deleteRule(const std::vector<int> &rule);

    /**
     * @brief ɾ���¹�����ICD��
     */
    void clearRule();

    /**
    * @brief ��ȡ����ICD�ӱ�
    * @return �ӱ�����
    */
    std::vector<std::string> subTableIds() const;

    /**
    * @brief ��ȡ����ICD�ӱ�
    * @return �ӱ����ݼ�
    */
    tableVector subTables() const;

    /**
    * @brief ��ѯ�ӱ�
    * @param [in] name : �ӱ������������1/����1/����/�������n/Ŀ���
    * @return �ӱ�����
    */
    TableNode::smtTable subTable(const std::string &name) const;

    /**
    * @brief �����ӱ���
    */
    void updateLength(const std::string &name);

    /**
     * @brief ��¡�������
     * @return ICD��ʵ��
     */
    TableNode::smtTable clone() const;

    // ����/��ȡID
    void setId(std::string id) override;
    std::string id() const override;

    // ����/��ȡ����
    void setName(const std::string& name) override;
    std::string name() const override;

    void setProCode(const std::string& proCode) override;
    std::string proCode() const override;

    // ����/��ȡ��ʶ
    void setSign(const std::string& sign) override;
    std::string sign() const override;

    // ����/��ȡ����
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
