#ifndef DATAMANEGEMENT_H
#define DATAMANEGEMENT_H

#include <QWidget>
#include "DBAccess/dbaccess.h"
#include "KernelClass/planenode.h"
#include "icdwidget/jnotify.h"

#define DEF_IGNORE_EMPTY_TABLE 0    // add by iclosue [2017-11-11]

//
#ifdef _MSC_VER
typedef unsigned __int64 JWPARAM;
typedef __int64 JLPARAM;
typedef __int64 JLRESULT;
typedef bool jbool;
typedef char jchar;
typedef unsigned char juchar;
typedef short jshort;
typedef unsigned short jushort;
typedef int jint;
typedef unsigned juint;
typedef __int64 jlonglong;
typedef unsigned __int64 julonglong;
#else
typedef unsigned long long JWPARAM;
typedef long long JLPARAM;
typedef long long JLRESULT;
typedef bool jbool;
typedef char jchar;
typedef unsigned char juchar;
typedef short jshort;
typedef unsigned short jushort;
typedef int jint;
typedef unsigned juint;
typedef long long jlonglong;
typedef unsigned long long julonglong;
#endif


/**
 * @brief �ڴ����ݹ����࣬���������ݿ�ͽ������ݵĽ���
 */
class DataManegement : public QWidget
{
    Q_OBJECT
public:
    explicit DataManegement(QWidget *parent = nullptr);
    ~DataManegement();

signals:

public slots:

protected:
    // ��Ϣ��Ӧ�ص�
    JLRESULT notifyRespond(const Icd::JNEvent &event);

protected:

private:
    /**
     * @brief ���ػ�������
     * @param [out] error : ������Ϣ
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool loadBaseData(std::string &error = std::string());

    /**
    * @brief ���ػ�������
    * @param [in] loadXmlBaseData : ����ϵͳ��Ϣ
    * @param [in] tableRules : �������Ϣ
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool loadXmlBaseData(const DMSpace::_vectorPS &infrastructure,
                         const DMSpace::_vectorIcdTR &tableRules);

    /**
     * @brief �����ֵ������
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool loadDic();

    /**
    * @brief ���¼����ֵ������
    * @param [in] dics : �ֵ��
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool reloadDic(const std::vector<std::string> &dics);

    /**
     * @brief ���ػ����ܹ����ݣ����ͺ�ϵͳ��
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool loadInfrastructure();

    /**
     * @brief ����ָ�����͵�����ICD��
     * @param [in] planeType : �ɻ��ͺ���
     * @param [in] increment : ������ʶ
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool loadTypeRule(int planeType, bool increment);

    /**
    * @brief ����ָ�����͵�����ICD��
    * @param [in] planeType : �ɻ��ͺ���
    * @param [in] tableRules : �������Ϣ
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool loadXmlTypeRule(int planeType,
                         DMSpace::_vectorIcdTR &tableRules);
    /**
    * @brief ж��ָ�����͵�����ICD��
    * @param [in] planeType : �ɻ��ͺ���
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    void unLoadTypeRule(int planeType);

    /**
     * @brief ����ָ�������·�ϵͳ������ICD��
     * @param [in] planeType : �ɻ��ͺ���
     * @param [in] system : ϵͳ��
     * @param [in] increment : ������ʶ
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool loadSystemRule(int planeType, int system, bool increment);

    /**
    * @brief ����ָ�������·�ϵͳ������ICD��
    * @param [in] planeType : �ɻ��ͺ���
    * @param [in] system : ϵͳ��
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool loadXmlSystemRule(int planeType, int system,
                           DMSpace::_vectorIcdTR &tableRules);
    /**
    * @brief ж��ָ�������·�ϵͳ������ICD��
    * @param [in] planeType : �ɻ��ͺ���
    * @param [in] system : ϵͳ��
    * @param [in] tableRules : �������Ϣ
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    void unLoadSystemRule(int planeType, int system);

    /**
     * @brief ���ص���ICD��
     * @param [in] plane : �ɻ��ͺ���
     * @param [in] system : ϵͳ��
     * @param [in] table : ICD����
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool loadTableRule(int plane, int system, const std::string &table);

    /**
    * @brief ���ص���ICD��
    * @param [in] plane : �ɻ��ͺ���
    * @param [in] system : ϵͳ��
    * @param [in] table : ICD����
    * @param [in] tableRules : �������Ϣ
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool loadXmlTablemRule(int plane, int system, const std::string &table,
                           DMSpace::_vectorIcdTR &tableRules);

    /**
    * @brief ж�ص���ICD��
    * @param [in] planeType : �ɻ��ͺ���
    * @param [in] system : ϵͳ��
    * @param [in] table : ICD����
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    void unLloadTableRule(int plane, int system, const std::string &table);

    /**
     * @brief �ݹ���ر��������
     * @param [in] name : ICD����
     * @param [in] table : ��ʵ��
     * @param [in] rules : ���������
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool recursiveLoadRule(const std::string& name,
                           TableNode::smtTable &table,
                           DMSpace::_vectorIcdTR &rules);

    /**
     * @brief ɾ����������
     * @param [in] planeType : �ɻ��ͺ���
     * @param [in] system : ϵͳ��
     * @param [in] table : ����
     * @param [in] number : ���
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool deleteOneRule(int planeType, int system, const std::string& table, int number);

    // �������ݹ���
    /**
     * @brief ����һ����������
     * @param [in] data : ��������
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool addOneKC(const PlaneNode::smtPlane& data);

    /**
     * @brief �޸�һ����������
     * @param [in] data : ��������
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool modifyOneKC(const PlaneNode::smtPlane& data);

    /**
     * @brief ɾ��һ����������
     * @param [in] code : ������
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool deleteOneKC(int code);

    /**
    * @brief ��ջ�������
    */
    void clearKC();

    /**
     * @brief ��ѯ��������
     * @param [in] code : ������
     * @return ��������
     */
    PlaneNode::smtPlane planeNode(int code);

    /**
    * @brief ��ѯ�������������
    * @param [in] name : ICD����
    * @param [in] rules : ICD�����ݼ�
    * @param [in] remove : ɾ����ʶ
    * @return ���������
    */
    DMSpace::pairIcdTR singleIcdTR(const std::string &name,
                                   DMSpace::_vectorIcdTR &rules,
                                   bool remove = false) const;

protected:
    /**
     * @brief ���ع�������
     * @param [in] planeType : ������
     * @param [in] ICDBase : ICD����������<������, �ӱ�>
     * @param [in] increment : ������ʶ
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool loadRuleData(int planeType,
                      const DMSpace::_vectorSB &ICDBase,
                      bool increment = false);
    /**
    * @brief ����������
    * @param [in] tables : ��
    * @param [in] rules : �������ݼ�<����, �������ݼ�>
    */
    void fillRules(TableNode::tableMap &tables,
                   const DMSpace::svrMap &rules);

    /**
    * @brief ���ع�������
    * @param [in] tableRules : �������Ϣ
    * @return �������Ϣ<�����飬<�����>>
    */
    std::map<std::string, TableNode::tableVector> loadXmlRuleData(
            const DMSpace::_vectorIcdTR &tableRules);

    /**
     * @brief �ݹ齨�����ϱ��ϵ
     * @param [in] allTable : Դ��
     * @param [in] name : ����
     * @return �ӱ�����
     */
    TableNode::smtTable recursiveLinkTable(TableNode::tableMap& allTable,
                                           const std::string& name);

    // ��ѯ�ֵ��
    bool queryDictionary(stQueryDic &data);
    // ���ع�������
    bool loadRules(PlaneNode::smtPlane &plane,
                   const QString &condition,
                   bool increment);
    // ж�ع�������
    void unLoadRules(const QString &condition);
    // �޸����л�������
    bool modifyAllKC(const std::vector<PlaneNode::smtPlane> &planes);
    // �����������
    bool savePlane(const stPlane &plane);
    // ɾ����������
    bool deletePlane(const std::vector<int> &keys);
    // ����ϵͳ����
    bool saveSystem(int plane, const std::vector<stSystem>& system);
    // �޸�ϵͳ����
    bool modifySystem(int plane, const SystemNode::smtSystem &system);
    // ɾ��ϵͳ����
    bool deleteSystem(int plane, const std::vector<int>& system);
    // ����ICD������
    bool saveTable(const QString &keys,
                   const std::vector<stICDBase> &tables);
    // �޸�ICD������
    bool modifyTable(const QString &keys, const TableNode::smtTable &table);
    // ɾ��ICD������
    bool deleteTable(const QString &keys,
                     const std::vector<std::string>& tables);
    // �����������
    int saveRule(const QString &keys,
                 const ICDMetaData::smtMeta &meta);
    // �����������
    bool insertRule(const QString &keys,
                    const ICDMetaData::smtMeta &meta);
    // ɾ����������
    bool deleteRule(const QString &keys,
                    const std::vector<int> &rules);
    // ɾ��֡ʶ�����ӱ�
    bool deleteSubTable(const QString &keys,
                        const std::vector<std::string> &subTables);
    // ��ѯ�ڴ�����
    JLRESULT querySingleElement(const Icd::JNEvent &event);
    // ��ѯ��������Դ
    JLRESULT queryDataSource(const Icd::JNEvent &event);
    // ��ѯ�ӱ�����
    JLRESULT queryMaxTableCount(const Icd::JNEvent &event);
    // ���濽������
    JLRESULT saveCopyData(const Icd::JNEvent &event);
    // ���ڴ����ݱ��浽���ݿ�
    JLRESULT saveDatabase(const Icd::JNEvent &event);
    // ���ڴ����ݱ��浽�ļ�
    JLRESULT saveFile(const Icd::JNEvent &event);
    // �������ݿ�������Ϣ
    JLRESULT dataSourceChanged(const Icd::JNEvent &event);
    // ��ѯ���ݿ������Ϣ
    JLRESULT queryDBError(const Icd::JNEvent &event);
    // ��ѯ�����
    TableNode::smtTable rootTable(int plane,
                                  int system,
                                  const std::string &table);
    // ��ѯ�ֵ���
    std::string dicName(GlobalDefine::DictionaryIndex dic) const;

    // ������Ҫ���������
    bool generateSavedData(QStringList &keyList,
                           DMSpace::_vectorPS &plane_system,
                           DMSpace::_vectorIcdTR &tableRules,
                           QString &err);
    bool generateAll(DMSpace::_vectorPS &plane_system,
                     TableNode::tableVector &tables,
                     QString &err);
    bool generatePlane(int planeId,
                       DMSpace::_vectorPS &plane_system,
                       TableNode::tableVector &tables,
                       QString &err);
    bool generateSystem(int planeId,
                        int systemId,
                        DMSpace::_vectorPS &plane_system,
                        TableNode::tableVector &tables,
                        QString &err);
    bool generateTable(int planeId,
                       int systemId,
                       const std::string &tableId,
                       const std::string &subId,
                       DMSpace::_vectorPS &plane_system,
                       TableNode::tableVector &tables,
                       QString &err);

    // �������ڴ�����ת����ͨ�ýṹ
    DMSpace::_vectorIcdTR convert2TableRules(
            const std::vector<TableNode::smtTable> &tables) const;
    // ��ͨ�ýṹת���ɹ����ڴ�����
    ICDMetaData::ruleMap convert2IcdRules(
            const std::vector<stTableRules> &rules);

    // �����������ݣ����¶������й�����ʶ��
    bool copyPlaneData(PlaneNode::smtPlane &plane, int planeId);
    // ����ϵͳ���ݣ����¶������й�����ʶ��
    bool copySystemData(SystemNode::smtSystem &system, int planeId, int systemId);
    // ���������ݣ����¶����ӹ�����ʶ��
    bool copyTableData(TableNode::smtTable &table,
                       const stICDBase &icdBase);
    // �����������ݣ����¶����ӹ�����ʶ��
    bool copyRuleData(ICDMetaData::smtMeta &meta,
                      const stICDBase &icdBase);
    // ����xml�ļ���Ϣ
    std::string readXmlFile();

private:
    DBAccess*        q_dbaccess;     // ���ݿ����ʵ��
    // ICD�ڴ�����<�����������>
    PlaneNode::planeVector  q_planes;

    int             q_dataSource;   // ����Դ
    std::string     q_defaultPath;  // �����ļ�Ĭ��·��
};

#endif // DATAMANEGEMENT_H
