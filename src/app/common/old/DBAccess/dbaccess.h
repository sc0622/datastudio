#ifndef DBACCESS_H
#define DBACCESS_H

#include "KernelClass/globalstruct.h"
#include "icdcore/3rdpart/jsoncpp/json_tool.h"
#include <unordered_map>

/**
* @brief 数据库表定义
*/
namespace DataBaseDefine{

    enum TableDefine {
        dbInvalid = -1,         // 无效表
        dbPlane,                // 机型表
        dbSystem,               // 系统表
        dbICDBase,              // ICD基本信息表
        dbUserPower,            // 用户权限表
        dbDataType,             // 数据类型表
        dbPowerType,            // 权限类型表
        dbCounterType,          // 帧计数类型表
        dbCheckType,            // 校验类型表
        dbTotal                 // 边界标记
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
 * @brief 数据库访问接口定义
 */
class DBACCESS_EXPORT DBAccess
{
public:
    /**
     * @brief The stDBInfo 数据库信息结构
     */
    struct stDBInfo
    {
        std::string     driver;     // 驱动
        std::string     server;     // 服务名
        std::string     database;   // 数据库名
        std::string     user;       // 用户名
        std::string     pwd;        // 密码
    };

    /**
     * @brief 构造函数
     */
    DBAccess();

    /**
    * @brief 析构函数
    */
    ~DBAccess();

    /**
    * @brief 获取数据库表名
    * @param [in] dbf : 表枚举
    * @return 表名字符串，为空失败
    */
    static std::string tableName(DataBaseDefine::TableDefine dbf);

    /**
     * @brief 初始化数据库，在调用其他数据操作接口前需要调用一次
     * @param [in] config : 包含数据库配置信息
     * @return 执行结果，true：成功；false：失败
     */
    bool init(const Json::Value &config);

    /**
    * @brief 查询数据库连接状态
    * @return 数据库连接状态，true：已连接；false：尚未连接
    */
    bool isOpen() const;

    /**
    * @brief 连接数据库
    * @return 数据库连接状态，true：连接成功；false：连接失败
    */
    bool connect();

    /**
    * @brief 断开据库连接
    */
    void disconnect();

    /**
    * @brief 获取最后一次的错误信息
    * @return 数据错误信息
    */
    std::string lastError() const;

    /**
    * @brief 获取发生错误的函数
    * @return 数据错误信息
    */
    std::string errorFunction() const;

    /**
    * @brief 获取子表上限
    * @return 子表上限
    */
    int tableLimitation() const;

    /**
    * @brief 设置子表上限
    * @param [in] top : 上限值
    */
    void setTableLimitation(int top);

    /**
     * @brief 读取数据库字典表数据
     * @param [out] dic : 所有字典数据<字典标识, <字典数据集>>
     * @return 执行结果，true：成功；false：失败
     */
    bool readDictionary(std::unordered_map<std::string, std::vector<stDictionary> >& dic);

    /**
    * @brief 读取字典表数据
    * @param [in] dics : 字典表
    * @param [out] result : 字典数据<字典标识, <字典数据集>>
    * @return 执行结果，true：成功；false：失败
    */
    bool readDictionary(const std::vector<std::string> &dics,
                        std::unordered_map<std::string, std::vector<stDictionary> >& result);

    /**
    * @brief 读取本地字典表数据（数据库无法连接时使用，注意与数据数据同步）
    * @param [out] dic : 所有字典数据<字典标识, <字典数据集>>
    * @return 执行结果，true：成功；false：失败
    */
    bool readLocalDictionary(std::unordered_map<std::string, std::vector<stDictionary> >& dic);

    /**
    * @brief 读取机型基本信息
    * @param [out] plane : 机型信息集
    * @return 执行结果，true：成功；false：失败
    */
    bool readPlane(std::vector<stPlane> &plane);

    /**
    * @brief 批量保存机型数据（完整）
    * @param [in] infrastructure : 系统结构数据<机型信息, <分系统信息集>>
    * @param [in] tableRules : 表规则数据<ICD表信息, <表规则信息集>>
    * @return 执行结果，true：成功；false：失败
    */
    bool savePlanes(const DMSpace::_vectorPS &infrastructure,
                    const DMSpace::_vectorIcdTR &tableRules);

    /**
    * @brief 保存机型
    * @param [in] plane : 机型信息
    * @return 执行结果，true：成功；false：失败
    */
    bool savePlane(const stPlane &plane);

    /**
    * @brief 保存机型
    * @param [in] plane_system : 机型-系统信息
    * @param [in] rules : 表规则数据
    * @param [in] dataLevel : 待保存数据层级（GlobalDefine::NodeType）
    * @param [in] dbSource : 数据库数据源标识
    * @return 执行结果，true：成功；false：失败
    */
    bool savePlane(const DMSpace::_vectorPS &plane_system,
                   const DMSpace::_vectorIcdTR &rules,
                   int dataLevel, bool dbSource = true);

    /**
    * @brief 清空数据库
    * @return 执行结果，true：成功；false：失败
    */
    bool clearDataBase();

    /**
    * @brief 删除机型
    * @param [in] planes : 机型标识集
    * @return 执行结果，true：成功；false：失败
    */
    bool deletePlane(const std::vector<int> &planes);

    /**
     * @brief 读取系统信息
     * @param [out] system : 所有系统信息<机型码, <分系统信息集>>
     * @return 执行结果，true：成功；false：失败
     */
    bool readSystem(std::unordered_map<int, std::vector<stSystem> > &system);

    /**
    * @brief 读取系统信息
    * @param [in] plane : 机型码
    * @param [in] system : 分系信息集
    * @return 执行结果，true：成功；false：失败
    */
    bool readSystem(int plane, std::vector<stSystem> &systems);

    /**
     * @brief 读取ICD表基本信息
     * @param [in] top : 只读取顶层表标识
     * @param [out] icdBase : ICD基本信息集<所属组, <字表信息集>>
     * @return 执行结果，true：成功；false：失败
     */
    bool readICDBase(bool top, std::unordered_map<std::string, std::vector<stICDBase> >& icdBase);

    /**
    * @brief 读取ICD表基本信息
    * @param [in] name : ICD表名
    * @param [out] icdBase : ICD基本信息
    * @return 执行结果，true：成功；false：失败
    */
    bool readICDBase(const std::string &name, stICDBase &icdBase);

    /**
    * @brief 读取ICD表基本信息
    * @param [in] name : ICD表名集
    * @param [out] icdBase : ICD基本信息集
    * @return 执行结果，true：成功；false：失败
    */
    bool readICDBase(const std::vector<std::string> &names,
                     std::vector<stICDBase> &icdBase);

    /**
     * @brief 读取特定条件的ICD表基本信息
     * @param [in] condition : 过滤条件（单独的机型码或者机型码和系统码的组合“xx/xx”）
     * @param [in] ICDBase : <父表标识, <表标识, ICD表集>>
     * @param [in] topOnly : 顶层表标识
     * @return 执行结果，true：成功；false：失败
     */
    bool readSpecificICD(const std::string& condition,
                         DMSpace::_vectorSB &ICDBase,
                         bool topOnly = false);

    /**
     * @brief 读取用户权限信息
     * @param [in] user : 用户名
     * @param [out] dic : 该用户的所有表权限信息
     * @return 执行结果，true：成功；false：失败
     */
    bool readUserTables(const std::string& user, std::vector<stTablePrivilege>);

    /**
     * @brief 读取表结构数据
     * @param [in] table : 表名
     * @param [out] rules : 表结构数据<规则名, 规则数据>
     * @return 执行结果，true：成功；false：失败
     */
    bool readTableRules(const std::string& table, DMSpace::srMap& rules);

    /**
     * @brief 读取表结构数据
     * @param [in] tables : 表名集
     * @param [out] rules : 表结构数据集<表名, <规则数据集>>
     * @return 执行结果，true：成功；false：失败
     */
    bool readTableRules(const std::vector<std::string> &tables,
                        DMSpace::svrMap &rules);

    /**
    * @brief 保存表规则数据
    * @param [in] icdBase : 表基本信息
    * @param [in] rules : 表规则数据
    * @param [in] subTables : 子表基本信息
    * @return 执行结果，true：成功；false：失败
    */
    bool saveTableRules(const stICDBase& icdBase,
                        const DMSpace::srMap&rules,
                        const std::vector<stICDBase> &subTables
                        = std::vector<stICDBase>());

    /**
    * @brief 批量保存表规则数据
    * @param [in] rules : 表规则数据
    * @return 执行结果，true：成功；false：失败
    */
    bool saveTableRules(const std::vector<std::pair<stICDBase, DMSpace::srMap>> &rules);

    /**
    * @brief 删除表规则数据
    * @param [in] table : 表名
    * @param [in] rules : 待更新表规则数据（删除之后的待更新数据）
    * @param [in] subTables : 待删除子表名
    * @return 执行结果，true：成功；false：失败
    */
    bool deleteTableRules(const std::string &table,
                          const DMSpace::srMap&rules,
                          const std::vector<std::string> &subTable);

    /**
    * @brief 保存一条规则数据
    * @param [in] table : 表名
    * @param [in] rule : 规则数据
    * @return 执行结果，true：成功；false：失败
    */
    bool saveRule(const std::string &table, const stTableRules &rule);

    /**
    * @brief 保存一条规则数据(复合数据)
    * @param [in] table : 表名
    * @param [in] rule : 规则数据
    * @param [in] subTables : 子表数据
    * @return 执行结果，true：成功；false：失败
    */
    bool saveRule(const std::string &table,
                  const stTableRules &rule,
                  const std::vector <stICDBase> &subTables);

    /**
    * @brief 删除表规则数据
    * @param [in] table : 表名
    * @param [in] rule : 规则数据<规则序号, <子表集>>
    * @return 执行结果，true：成功；false：失败
    */
    bool deleteRules(const std::string &table, const std::unordered_map < int,
                     std::vector < std::string >> &rule);

    /**
    * @brief 查询指定表的所有子表
    * @param [in] table : 表名
    * @return 子表集
    */
    std::vector<std::string> querySubTables(const std::string& table) const;

    // end
    /**特殊查询**/

protected:
    /**
     * @brief 读取数据库配置信息
     * @param [int] config : 包含数据库配置信息的xml文件全路径
     * @param [out] dbInfo : 数据配置信息
     * @return 执行结果， true：成功；false：失败
     */
    bool loadDBInfo(const Json::Value &config, stDBInfo& dbInfo);

    /**
     * @brief 创建ICD规则表语句
     * @param [in] table : 表名
     * @return 创建表命令语句
     */
    std::string createICDTableCmd(const std::string& table);

private:
    DBAccessPrivate     *d;
};

#endif // DBACCESS_H
