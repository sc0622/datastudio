#ifndef ICDCOMPLEXDATA_H
#define ICDCOMPLEXDATA_H

#include "icdcommondata.h"
#include "tablenode.h"


class ICDComplexDataPrivate;
/**
 * @brief ICD����������
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
    * @brief ��֤�Ƿ��пձ�
    * @param [in] names : �ձ����֣������ӱ�
    * @return true���У�false��û��
    */
    bool hasEmptyTable(std::string &names) const;

    /**
     * @brief ���ñ�����
     * @param [in] table : ������
     */
    void setTable(const TableNode::tableVector& table);

    /**
     * @brief ���ӱ�����
     * @param [in] table : ������
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    void addTable(const TableNode::smtTable& table);

    /**
     * @brief ��ȡ���б�����
     * @return �����ݼ�
     */
    TableNode::tableVector allTable() const;

    /**
    * @brief ��ȡ���б����������ӱ�
    * @return ������
    */
    std::vector<std::string> tableNames() const;

    /**
    * @brief ��ȡ�����ӱ�����
    * @return �����ݼ�
    */
    TableNode::tableVector subTables() const;

    /**
    * @brief ��ѯ��
    * @param [in] name : ����������1/����2/����/Ŀ���
    * @return �ӱ�����
    */
    TableNode::smtTable table(const std ::string &name) const;

    /**
    * @brief ���³��ȣ������ӱ�
    */
    void updateLength(const std ::string &name);

    /**
     * @brief ɾ��������
     * @param [in] table : ���ʶ��
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool deleteTable(const std::vector<std::string> &table);

    /**
     * @brief ɾ�����е�ĳһ������
     * @param [in] name : ���ʶ
     * @param [in] serial : �����
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool deleteTableItem(const std::string& name, int serial);

    /**
     * @brief ɾ�����б�����
     */
    void clearTable();

protected:
    // ���ӱ�����ת��Ϊ�ַ���(�ӱ�1@����@�ӱ�n)
    virtual std::string stringValue() const;
    void calculateLength();

private:
    ICDComplexDataPrivate   *d;
};

#endif // ICDCOMPLEXDATA_H
