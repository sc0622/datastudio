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
 * @brief 内存数据管理类，负责与数据库和界面数据的交互
 */
class DataManegement : public QObject
{
    Q_OBJECT
public:
    explicit DataManegement(QWidget *parent = nullptr);
    ~DataManegement();

    bool init();

private:
    // 消息响应回调
    JLRESULT notifyRespond(const Icd::JNEvent &event);

    /**
     * @brief 加载基础数据
     * @param [out] error : 错误信息
     * @param [in] deep
     * @return 执行结果，true：成功；false：失败
     */
    bool loadBaseData(std::string &error = std::string(), int deep = Icd::ObjectTable);

    /**
    * @brief 加载基础数据
    * @param [in] loadXmlBaseData : 机型系统信息
    * @param [in] tableRules : 表规则信息
    * @return 执行结果，true：成功；false：失败
    */
    bool loadXmlBaseData(const DMSpace::_vectorPS &infrastructure, const DMSpace::_vectorIcdTR &tableRules);

    /**
     * @brief 加载字典表数据
     * @return 执行结果，true：成功；false：失败
     */
    bool loadDic();

    /**
    * @brief 重新加载字典表数据
    * @param [in] dics : 字典表集
    * @return 执行结果，true：成功；false：失败
    */
    bool reloadDic(const std::vector<std::string> &dics);

    /**
     * @brief 加载基础架构数据（机型和系统）
     * @param [in] deep
     * @return 执行结果，true：成功；false：失败
     */
    bool loadInfrastructure(int deep);

    /**
     * @brief 加载指定机型的所有ICD表
     * @param [in] planeType : 飞机型号码
     * @param [in] increment : 增量标识
     * @return 执行结果，true：成功；false：失败
     */
    bool loadTypeRule(int planeType, bool increment);

    /**
    * @brief 加载指定机型的所有ICD表
    * @param [in] planeType : 飞机型号码
    * @param [in] tableRules : 表规则信息
    * @return 执行结果，true：成功；false：失败
    */
    bool loadXmlTypeRule(int planeType, DMSpace::_vectorIcdTR &tableRules);
    /**
    * @brief 卸载指定机型的所有ICD表
    * @param [in] planeType : 飞机型号码
    * @return 执行结果，true：成功；false：失败
    */
    void unLoadTypeRule(int planeType);

    /**
     * @brief 加载指定机型下分系统的所有ICD表
     * @param [in] planeType : 飞机型号码
     * @param [in] system : 系统码
     * @param [in] increment : 增量标识
     * @return 执行结果，true：成功；false：失败
     */
    bool loadSystemRule(int planeType, int system, bool increment);

    /**
    * @brief 加载指定机型下分系统的所有ICD表
    * @param [in] planeType : 飞机型号码
    * @param [in] system : 系统码
    * @return 执行结果，true：成功；false：失败
    */
    bool loadXmlSystemRule(int planeType, int system, DMSpace::_vectorIcdTR &tableRules);
    /**
    * @brief 卸载指定机型下分系统的所有ICD表
    * @param [in] planeType : 飞机型号码
    * @param [in] system : 系统码
    * @param [in] tableRules : 表规则信息
    * @return 执行结果，true：成功；false：失败
    */
    void unLoadSystemRule(int planeType, int system);

    /**
     * @brief 加载单张ICD表
     * @param [in] plane : 飞机型号码
     * @param [in] system : 系统码
     * @param [in] table : ICD表名
     * @return 执行结果，true：成功；false：失败
     */
    bool loadTableRule(int plane, int system, const std::string &table);

    /**
    * @brief 加载单张ICD表
    * @param [in] plane : 飞机型号码
    * @param [in] system : 系统码
    * @param [in] table : ICD表名
    * @param [in] tableRules : 表规则信息
    * @return 执行结果，true：成功；false：失败
    */
    bool loadXmlTablemRule(int plane, int system, const std::string &table, DMSpace::_vectorIcdTR &tableRules);

    /**
    * @brief 卸载单张ICD表
    * @param [in] planeType : 飞机型号码
    * @param [in] system : 系统码
    * @param [in] table : ICD表名
    * @return 执行结果，true：成功；false：失败
    */
    void unLloadTableRule(int plane, int system, const std::string &table);

    /**
     * @brief 递归加载表规则数据
     * @param [in] name : ICD表名
     * @param [in] table : 表实例
     * @param [in] rules : 表规则数据
     * @return 执行结果，true：成功；false：失败
     */
    bool recursiveLoadRule(const std::string& name, TableNode::smtTable &table, DMSpace::_vectorIcdTR &rules);

    /**
     * @brief 删除单条规则
     * @param [in] planeType : 飞机型号码
     * @param [in] system : 系统码
     * @param [in] table : 表名
     * @param [in] number : 序号
     * @return 执行结果，true：成功；false：失败
     */
    bool deleteOneRule(int planeType, int system, const std::string& table, int number);

    // 核心数据管理
    /**
     * @brief 新增一条机型数据
     * @param [in] data : 机型数据
     * @return 执行结果，true：成功；false：失败
     */
    bool addOneKC(const PlaneNode::smtPlane& data);

    /**
     * @brief 修改一条机型数据
     * @param [in] data : 机型数据
     * @return 执行结果，true：成功；false：失败
     */
    bool modifyOneKC(const PlaneNode::smtPlane& data);

    /**
     * @brief 删除一条机型数据
     * @param [in] code : 机型码
     * @return 执行结果，true：成功；false：失败
     */
    bool deleteOneKC(int code);

    /**
    * @brief 清空机型数据
    */
    void clearKC();

    /**
     * @brief 查询机型数据
     * @param [in] code : 机型码
     * @return 机型数据
     */
    PlaneNode::smtPlane planeNode(int code);

    /**
    * @brief 查询当个规则表数据
    * @param [in] name : ICD表名
    * @param [in] rules : ICD表数据集
    * @param [in] remove : 删除标识
    * @return 规则表数据
    */
    DMSpace::pairIcdTR singleIcdTR(const std::string &name, DMSpace::_vectorIcdTR &rules,
                                   bool remove = false) const;

protected:
    /**
     * @brief 加载规则数据
     * @param [in] planeType : 机型码
     * @param [in] ICDBase : ICD基本表数据<父表名, 子表集>
     * @param [in] increment : 增量标识
     * @return 执行结果，true：成功；false：失败
     */
    bool loadRuleData(int planeType, const DMSpace::_vectorSB &ICDBase, bool increment = false);
    /**
    * @brief 填充规则数据
    * @param [in] tables : 表集
    * @param [in] rules : 规则数据集<表名, 规则数据集>
    */
    void fillRules(TableNode::tableMap &tables, const DMSpace::svrMap &rules);

    /**
    * @brief 加载规则数据
    * @param [in] tableRules : 表规则信息
    * @return 表规则信息<所属组，<表规则>>
    */
    std::unordered_map<std::string, TableNode::tableVector> loadXmlRuleData(
            const DMSpace::_vectorIcdTR &tableRules);

    /**
     * @brief 递归建立复合表关系
     * @param [in] allTable : 源表集
     * @param [in] name : 表名
     * @return 子表数据
     */
    TableNode::smtTable recursiveLinkTable(TableNode::tableMap& allTable, const std::string& name);

    // 查询字典表
    bool queryDictionary(stQueryDic &data);
    // 加载规则数据
    bool loadRules(PlaneNode::smtPlane &plane, const QString &condition, bool increment);
    // 卸载规则数据
    void unLoadRules(const QString &condition);
    // 修改所有机型数据
    bool modifyAllKC(const std::vector<PlaneNode::smtPlane> &planes);
    // 保存机型数据
    bool savePlane(const stPlane &plane);
    // 删除机型数据
    bool deletePlane(const std::vector<int> &keys);
    // 保存系统数据
    bool saveSystem(int plane, const std::vector<stSystem>& system);
    // 修改系统数据
    bool modifySystem(int plane, const SystemNode::smtSystem &system);
    // 删除系统数据
    bool deleteSystem(int plane, const std::vector<int>& system);
    // 保存ICD表数据
    bool saveTable(const QString &keys, const std::vector<stICDBase> &tables);
    // 修改ICD表数据
    bool modifyTable(const QString &keys, const TableNode::smtTable &table);
    // 删除ICD表数据
    bool deleteTable(const QString &keys, const std::vector<std::string>& tables);
    // 保存规则数据
    int saveRule(const QString &keys, const ICDMetaData::smtMeta &meta);
    // 保存规则数据
    bool insertRule(const QString &keys, const ICDMetaData::smtMeta &meta);
    // 删除规则数据
    bool deleteRule(const QString &keys, const std::vector<int> &rules);
    // 删除帧识别码子表
    bool deleteSubTable(const QString &keys, const std::vector<std::string> &subTables);
    // 查询内存数据
    JLRESULT querySingleElement(const Icd::JNEvent &event);
    // 查询加载数据源
    JLRESULT queryDataSource(const Icd::JNEvent &event);
    // 查询子表上限
    JLRESULT queryMaxTableCount(const Icd::JNEvent &event);
    // 保存拷贝数据
    JLRESULT saveCopyData(const Icd::JNEvent &event);
    // 将内存数据保存到数据库
    JLRESULT saveDatabase(const Icd::JNEvent &event);
    // 将内存数据保存到文件
    JLRESULT saveFile(const Icd::JNEvent &event);
    // 更新数据库配置信息
    JLRESULT dataSourceChanged(const Icd::JNEvent &event);
    // 查询数据库错误信息
    JLRESULT queryDBError(const Icd::JNEvent &event);
    // 查询顶层表
    TableNode::smtTable rootTable(int plane, int system, const std::string &table);
    // 查询字典名
    std::string dicName(GlobalDefine::DictionaryIndex dic) const;

    // 构造需要保存的数据
    bool generateSavedData(QStringList &keyList, DMSpace::_vectorPS &plane_system,
                           DMSpace::_vectorIcdTR &tableRules, QString &err);
    bool generateAll(DMSpace::_vectorPS &plane_system, TableNode::tableVector &tables, QString &err);
    bool generatePlane(int planeId, DMSpace::_vectorPS &plane_system, TableNode::tableVector &tables,
                       QString &err);
    bool generateSystem(int planeId, int systemId, DMSpace::_vectorPS &plane_system,
                        TableNode::tableVector &tables, QString &err);
    bool generateTable(int planeId, int systemId, const std::string &tableId, const std::string &subId,
                       DMSpace::_vectorPS &plane_system, TableNode::tableVector &tables, QString &err);

    // 将规则内存数据转换成通用结构
    DMSpace::_vectorIcdTR convert2TableRules(const std::vector<TableNode::smtTable> &tables) const;
    // 将通用结构转换成规则内存数据
    ICDMetaData::ruleMap convert2IcdRules(const std::vector<stTableRules> &rules);

    // 拷贝机型数据（重新定义所有规则表标识）
    bool copyPlaneData(PlaneNode::smtPlane &plane, int planeId);
    // 拷贝系统数据（重新定义所有规则表标识）
    bool copySystemData(SystemNode::smtSystem &system, int planeId, int systemId);
    // 拷贝机数据（重新定义子规则表标识）
    bool copyTableData(TableNode::smtTable &table, const stICDBase &icdBase);
    // 拷贝规则数据（重新定义子规则表标识）
    bool copyRuleData(ICDMetaData::smtMeta &meta, const stICDBase &icdBase);
    // 加载xml文件信息
    std::string readXmlFile();

private:
    QWidget *parentWidget_;
    DBAccess *q_dbaccess;               // 数据库访问实例
    PlaneNode::planeVector q_planes;    // ICD内存数据<具体机型数据>
    int q_dataSource;                   // 数据源
    std::string q_defaultPath;          // 加载文件默认路径
};

#endif // DATAMANEGEMENT_H
