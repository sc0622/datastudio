#ifndef DBACCESS_H
#define DBACCESS_H

#include "globalstruct.h"
#include <map>

/**
* @brief ���ݿ����
*/
namespace DataBaseDefine{

    enum TableDefine {
        dbInvalid = -1,         // ��Ч��
        dbPlane,                // ���ͱ�
        dbSystem,               // ϵͳ��
        dbICDBase,              // ICD������Ϣ��
        dbUserPower,            // �û�Ȩ�ޱ�
        dbDataType,             // �������ͱ�
        dbPowerType,            // Ȩ�����ͱ�
        dbCounterType,          // ֡�������ͱ�
        dbCheckType,            // У�����ͱ�
        dbTotal                 // �߽���
    };
}

#ifdef DBACCESS_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef DBACCESS_BUILD
#       define DBACCESS_EXPORT __declspec(dllexport)
#   else
#       define DBACCESS_EXPORT __declspec(dllimport)
#       if defined(DEBUG) || defined(_DEBUG)
#           pragma comment(lib, "DBAccessd.lib")
#       else
#           pragma comment(lib, "DBAccess.lib")
#       endif
#   endif // !DBACCESS_BUILD
#endif // _MSC_VER || ...
#endif // DBACCESS_LIB

#ifndef DBACCESS_EXPORT
#define DBACCESS_EXPORT
#endif

class DBAccessPrivate;

/**
 * @brief ���ݿ���ʽӿڶ���
 */
class DBACCESS_EXPORT DBAccess
{
public:
    /**
     * @brief The stDBInfo ���ݿ���Ϣ�ṹ
     */
    struct stDBInfo
    {
        std::string     driver;     // ����
        std::string     server;     // ������
        std::string     database;   // ���ݿ���
        std::string     user;       // �û���
        std::string     pwd;        // ����
    };

    /**
     * @brief ���캯��
     */
    DBAccess();

    /**
    * @brief ��������
    */
    ~DBAccess();

    /**
    * @brief ��ȡ���ݿ����
    * @param [in] dbf : ��ö��
    * @return �����ַ�����Ϊ��ʧ��
    */
    static std::string tableName(DataBaseDefine::TableDefine dbf);

    /**
     * @brief ��ʼ�����ݿ⣬�ڵ����������ݲ����ӿ�ǰ��Ҫ����һ��
     * @param [in] file : �������ݿ�������Ϣ��xml�ļ�ȫ·��
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool init(const std::string& file);

    /**
    * @brief ��ѯ���ݿ�����״̬
    * @return ���ݿ�����״̬��true�������ӣ�false����δ����
    */
    bool isOpen() const;

    /**
    * @brief �������ݿ�
    * @return ���ݿ�����״̬��true�����ӳɹ���false������ʧ��
    */
    bool connect();

    /**
    * @brief �Ͽ��ݿ�����
    */
    void disconnect();

    /**
    * @brief ��ȡ���һ�εĴ�����Ϣ
    * @return ���ݴ�����Ϣ
    */
    std::string lastError() const;

    /**
    * @brief ��ȡ��������ĺ���
    * @return ���ݴ�����Ϣ
    */
    std::string errorFunction() const;

    /**
    * @brief ��ȡ�ӱ�����
    * @return �ӱ�����
    */
    int tableLimitation() const;

    /**
    * @brief �����ӱ�����
    * @param [in] top : ����ֵ
    */
    void setTableLimitation(int top);

    /**
     * @brief ��ȡ���ݿ��ֵ������
     * @param [out] dic : �����ֵ�����<�ֵ��ʶ, <�ֵ����ݼ�>>
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool readDictionary(std::map<std::string, std::vector<stDictionary> >& dic);

    /**
    * @brief ��ȡ�ֵ������
    * @param [in] dics : �ֵ��
    * @param [out] result : �ֵ�����<�ֵ��ʶ, <�ֵ����ݼ�>>
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool readDictionary(const std::vector<std::string> &dics,
                        std::map<std::string, std::vector<stDictionary> >& result);

    /**
    * @brief ��ȡ�����ֵ�����ݣ����ݿ��޷�����ʱʹ�ã�ע������������ͬ����
    * @param [out] dic : �����ֵ�����<�ֵ��ʶ, <�ֵ����ݼ�>>
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool readLocalDictionary(std::map<std::string, std::vector<stDictionary> >& dic);

    /**
    * @brief ��ȡ���ͻ�����Ϣ
    * @param [out] plane : ������Ϣ��
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool readPlane(std::vector<stPlane> &plane);

    /**
    * @brief ��������������ݣ�������
    * @param [in] infrastructure : ϵͳ�ṹ����<������Ϣ, <��ϵͳ��Ϣ��>>
    * @param [in] tableRules : ���������<ICD����Ϣ, <�������Ϣ��>>
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool savePlanes(const DMSpace::_vectorPS &infrastructure,
                    const DMSpace::_vectorIcdTR &tableRules);

    /**
    * @brief �������
    * @param [in] plane : ������Ϣ
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool savePlane(const stPlane &plane);

    /**
    * @brief �������
    * @param [in] plane_system : ����-ϵͳ��Ϣ
    * @param [in] rules : ���������
    * @param [in] dataLevel : ���������ݲ㼶��GlobalDefine::NodeType��
    * @param [in] dbSource : ���ݿ�����Դ��ʶ
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool savePlane(const DMSpace::_vectorPS &plane_system,
                   const DMSpace::_vectorIcdTR &rules,
                   int dataLevel, bool dbSource = true);

    /**
    * @brief ������ݿ�
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool clearDataBase();

    /**
    * @brief ɾ������
    * @param [in] planes : ���ͱ�ʶ��
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool deletePlane(const std::vector<int> &planes);

    /**
     * @brief ��ȡϵͳ��Ϣ
     * @param [out] system : ����ϵͳ��Ϣ<������, <��ϵͳ��Ϣ��>>
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool readSystem(std::map<int, std::vector<stSystem> > &system);

    /**
    * @brief ��ȡϵͳ��Ϣ
    * @param [in] plane : ������
    * @param [in] system : ��ϵ��Ϣ��
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool readSystem(int plane, std::vector<stSystem> &systems);

    /**
     * @brief ��ȡICD�������Ϣ
     * @param [in] top : ֻ��ȡ������ʶ
     * @param [out] icdBase : ICD������Ϣ��<������, <�ֱ���Ϣ��>>
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool readICDBase(bool top,
                     std::map<std::string, std::vector<stICDBase> >& icdBase);

    /**
    * @brief ��ȡICD�������Ϣ
    * @param [in] name : ICD����
    * @param [out] icdBase : ICD������Ϣ
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool readICDBase(const std::string &name, stICDBase &icdBase);

    /**
    * @brief ��ȡICD�������Ϣ
    * @param [in] name : ICD������
    * @param [out] icdBase : ICD������Ϣ��
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool readICDBase(const std::vector<std::string> &names,
                     std::vector<stICDBase> &icdBase);

    /**
     * @brief ��ȡ�ض�������ICD�������Ϣ
     * @param [in] condition : ���������������Ļ�������߻������ϵͳ�����ϡ�xx/xx����
     * @param [in] ICDBase : <�����ʶ, <���ʶ, ICD��>>
     * @param [in] topOnly : ������ʶ
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool readSpecificICD(const std::string& condition,
                         DMSpace::_vectorSB &ICDBase,
                         bool topOnly = false);

    /**
     * @brief ��ȡ�û�Ȩ����Ϣ
     * @param [in] user : �û���
     * @param [out] dic : ���û������б�Ȩ����Ϣ
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool readUserTables(const std::string& user, std::vector<stTablePrivilege>);

    /**
     * @brief ��ȡ��ṹ����
     * @param [in] table : ����
     * @param [out] rules : ��ṹ����<������, ��������>
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool readTableRules(const std::string& table, DMSpace::srMap& rules);

    /**
     * @brief ��ȡ��ṹ����
     * @param [in] tables : ������
     * @param [out] rules : ��ṹ���ݼ�<����, <�������ݼ�>>
     * @return ִ�н����true���ɹ���false��ʧ��
     */
    bool readTableRules(const std::vector<std::string> &tables,
                        DMSpace::svrMap &rules);

    /**
    * @brief ������������
    * @param [in] icdBase : �������Ϣ
    * @param [in] rules : ���������
    * @param [in] subTables : �ӱ������Ϣ
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool saveTableRules(const stICDBase& icdBase,
                        const DMSpace::srMap&rules,
                        const std::vector<stICDBase> &subTables
                        = std::vector<stICDBase>());

    /**
    * @brief ����������������
    * @param [in] rules : ���������
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool saveTableRules(const std::vector<std::pair<stICDBase, DMSpace::srMap>> &rules);

    /**
    * @brief ɾ�����������
    * @param [in] table : ����
    * @param [in] rules : �����±�������ݣ�ɾ��֮��Ĵ��������ݣ�
    * @param [in] subTables : ��ɾ���ӱ���
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool deleteTableRules(const std::string &table,
                          const DMSpace::srMap&rules,
                          const std::vector<std::string> &subTable);

    /**
    * @brief ����һ����������
    * @param [in] table : ����
    * @param [in] rule : ��������
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool saveRule(const std::string &table, const stTableRules &rule);

    /**
    * @brief ����һ����������(��������)
    * @param [in] table : ����
    * @param [in] rule : ��������
    * @param [in] subTables : �ӱ�����
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool saveRule(const std::string &table,
                  const stTableRules &rule,
                  const std::vector <stICDBase> &subTables);

    /**
    * @brief ɾ�����������
    * @param [in] table : ����
    * @param [in] rule : ��������<�������, <�ӱ�>>
    * @return ִ�н����true���ɹ���false��ʧ��
    */
    bool deleteRules(const std::string &table, const std::map < int,
                     std::vector < std::string >> &rule);

    /**
    * @brief ��ѯָ����������ӱ�
    * @param [in] table : ����
    * @return �ӱ�
    */
    std::vector<std::string> querySubTables(const std::string& table) const;

    // end
    /**�����ѯ**/

protected:
    /**
     * @brief ��ȡ���ݿ�������Ϣ
     * @param [int] file : �������ݿ�������Ϣ��xml�ļ�ȫ·��
     * @param [out] dbInfo : ����������Ϣ
     * @return ִ�н���� true���ɹ���false��ʧ��
     */
    bool loadDBInfo(const std::string& file, stDBInfo& dbInfo);

    /**
     * @brief ����ICD��������
     * @param [in] table : ����
     * @return �������������
     */
    std::string createICDTableCmd(const std::string& table);

private:
    DBAccessPrivate     *d;
};

#endif // DBACCESS_H
