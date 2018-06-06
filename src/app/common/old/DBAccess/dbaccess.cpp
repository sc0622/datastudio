#include "dbaccess.h"
#include <QtSql>
#include "icdcore/3rdpart/jsoncpp/json_tool.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

// 最小子表数量
#ifndef MINCOUNT
#define MINCOUNT    10
#endif

// 最大子表数量
#ifndef MAXCOUNT
#define MAXCOUNT    1000
#endif

// 默认子表数量
#ifndef DEFAULTCOUNT
#define DEFAULTCOUNT    40
#endif

#ifndef TABLELENGTH
// 单张表名长度 ICDTable_32位UID
#define TABLELENGTH    42
#endif

class DBAccessPrivate
{
public:
    DBAccessPrivate()
        : q_tableLimitation(DEFAULTCOUNT)
        , q_icd(DBAccess::tableName(DataBaseDefine::dbICDBase))
        , q_userPower(DBAccess::tableName(DataBaseDefine::dbUserPower))
    {
        q_dic.push_back(DBAccess::tableName(DataBaseDefine::dbDataType)); // 数据类型
        q_dic.push_back(DBAccess::tableName(DataBaseDefine::dbPowerType));// 权限类型
        q_dic.push_back(DBAccess::tableName(DataBaseDefine::dbCounterType));// 帧计数类型
        q_dic.push_back(DBAccess::tableName(DataBaseDefine::dbCheckType));// 校验类型
    }

public:
    QSqlDatabase        q_db;
    DBAccess::stDBInfo  q_dbInfo;
    QString             q_error;
    QString             q_errorFunc;
    int                 q_tableLimitation;  // 子表限制
    std::vector<std::string> q_dic; // 数据库字典

    const std::string q_icd;        // ICD集表
    const std::string q_userPower;  // 用户权限
};

DBAccess::DBAccess()
    :d(new DBAccessPrivate())
{
}

DBAccess::~DBAccess()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

/**
* @brief 获取数据库表名
* @param [in] dbf : 表枚举
* @return 表名字符串，为空失败
*/
std::string DBAccess::tableName(DataBaseDefine::TableDefine dbf)
{
    std::string result;
    switch (dbf) {
    case DataBaseDefine::dbInvalid:
        break;
    case DataBaseDefine::dbPlane:
        result = "ICDPlane";
        break;
    case DataBaseDefine::dbSystem:
        result = "ICDSystem";
        break;
    case DataBaseDefine::dbICDBase:
        result = "ICDLeaf";
        break;
    case DataBaseDefine::dbUserPower:
        result = "UserPower";
        break;
    case DataBaseDefine::dbDataType:
        result = "Dic_DataType";
        break;
    case DataBaseDefine::dbPowerType:
        result = "Dic_PowerType";
        break;
    case DataBaseDefine::dbCounterType:
        result = "Dic_CounterType";
        break;
    case DataBaseDefine::dbCheckType:
        result = "Dic_CheckType";
        break;
    default:
        break;
    }

    return result;
}

/**
 * @brief 初始化数据库，在调用其他数据操作接口前需要调用一次
 * @param [in] file : 包含数据库配置信息的xml文件全路径
 * @return 执行结果，true：成功；false：失败
 */
bool DBAccess::init(const Json::Value &config)
{
    if (!loadDBInfo(config, d->q_dbInfo)) {
        return false;
    }

    // construct database
    const QString connetionName("com.smartsoft.datastudio.dbaccess.sql");
    if (QSqlDatabase::contains(connetionName)) {    // exist
        d->q_db = QSqlDatabase::database(connetionName, true);
    } else {    // new
        d->q_db = QSqlDatabase::addDatabase("QODBC", connetionName);
        if (!d->q_db.isValid()) {
            d->q_error = QString("failed to connect database!\r\n"
                                 "add database failed!");
            d->q_errorFunc = __FUNCTION__;
            qWarning() << d->q_error;
            return false;
        }
    }

    // set
    QString dsn;
    QString driver = QString::fromStdString(d->q_dbInfo.driver);
    driver = driver.toUpper();
    if ("SQL SERVER" == driver) {
        dsn = QString("DRIVER={%1};SERVER=%2;DATABASE=%3;UID=%4;PWD=%5;")
                .arg(d->q_dbInfo.driver.c_str())
                .arg(d->q_dbInfo.server.c_str())
                .arg(d->q_dbInfo.database.c_str())
                .arg(d->q_dbInfo.user.c_str())
                .arg(d->q_dbInfo.pwd.c_str());

    } else if ("ORACLE" == driver) {
        dsn = QString("DRIVER={%1};SERVER=%2;UID=%3;PWD=%4;")
                .arg("microsoft odbc for oracle")
                .arg("oraclesever.world")
                .arg(d->q_dbInfo.user.c_str())
                .arg(d->q_dbInfo.pwd.c_str());
    }
    d->q_db.setDatabaseName(dsn);
    d->q_db.setUserName(d->q_dbInfo.user.c_str());
    d->q_db.setPassword(d->q_dbInfo.pwd.c_str());
    // set timeout 10s
    d->q_db.setConnectOptions("SQL_ATTR_CONNECTION_TIMEOUT=10;"
                              "SQL_ATTR_LOGIN_TIMEOUT=10");

    // connect
    if (!d->q_db.open()) {
        QSqlError sqlError = d->q_db.lastError();
        d->q_error = QString("failed to connect database!\r\n error type:"
                             "%1;\r\n error text:%2;"
                             "\r\n nativeErrorCode:%3;")
                .arg(sqlError.type()).arg(sqlError.text())
                .arg(sqlError.nativeErrorCode());
        d->q_errorFunc = __FUNCTION__;
        qWarning() << d->q_error;

        return false;
    }

    return true;
}

/**
* @brief 查询数据库连接状态
* @return 数据库连接状态，true：已连接；false：尚未连接
*/
bool DBAccess::isOpen() const
{
    if (!d->q_db.isValid()) {
        d->q_error = QString("database invalid!");
        d->q_errorFunc = __FUNCTION__;
        return false;
    }
    QSqlQuery query(d->q_db);
    // 验证数据库是否连接
    if (!query.exec("select 1")) {
        d->q_error = QString("database disconnected!");
        d->q_errorFunc = __FUNCTION__;
        return false;
    } else {
        return d->q_db.isOpen();
    }
}

/**
* @brief 连接数据库
* @return 数据库连接状态，true：连接成功；false：连接失败
*/
bool DBAccess::connect()
{
    if (!d->q_db.open()) {
        QSqlError sqlError = d->q_db.lastError();
        d->q_error = QString("failed to connect database!\r\n error type:"
                             "%1\r\n error text:%2\r\n nativeErrorCode:%3")
                .arg(sqlError.type()).arg(sqlError.text())
                .arg(sqlError.nativeErrorCode());
        d->q_errorFunc = __FUNCTION__;
        qWarning() << d->q_error;

        return false;
    }

    return true;
}

/**
* @brief 断开据库连接
*/
void DBAccess::disconnect()
{
    if (d->q_db.isValid()) {
        if (d->q_db.isOpen()) {
            d->q_db.close();
        }
    }
}

/**
* @brief 获取最后一次的错误信息
* @return 数据错误信息
*/
std::string DBAccess::lastError() const
{
    return d->q_error.toStdString();
}

/**
* @brief 获取发生错误的函数
* @return 数据错误信息
*/
std::string DBAccess::errorFunction() const
{
    return d->q_errorFunc.toStdString();
}

/**
* @brief 获取子表上限
* @return 子表上限
*/
int DBAccess::tableLimitation() const
{
    return d->q_tableLimitation;
}

/**
* @brief 设置子表上限
* @param [in] top : 上限值
*/
void DBAccess::setTableLimitation(int top)
{
    if (top > MINCOUNT && top < MAXCOUNT) {
        d->q_tableLimitation = top;
    }
}

/**
 * @brief 读取数据库字典表数据
 * @param [out] dic : 所有字典数据<字典标识, <字典数据集>>
 * @return 执行结果，true：成功；false：失败
 */
bool DBAccess::readDictionary(std::map<std::string, std::vector<stDictionary> > &dic)
{
    if (!isOpen()) {
        return false;
    }
    const int dicCount = d->q_dic.size();
    if (dicCount < 1) {
        return true;
    }
    stDictionary stDic;
    std::vector<stDictionary> table;
    for (int i = 0; i < dicCount; ++i) {
        QString sql = QString("select * from %1").arg(d->q_dic[i].c_str());
        QSqlQuery query(d->q_db);
        // 无效数据
        if (!query.exec(sql)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            continue;
        }
        while (query.next()) {
            stDic.nCode = query.value("code").toInt();
            stDic.sDec = query.value("describe").toString().toStdString();
            //stDic.sSign = query.value("sign").toString().toStdString();
            stDic.sRemark = query.value("remark").toString().toStdString();

            table.push_back(stDic);
        }
        dic[d->q_dic[i]] = table;
    }

    return true;
}

/**
* @brief 读取字典表数据
* @param [in] dics : 字典表
* @param [out] result : 字典数据<字典标识, <字典数据集>>
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::readDictionary(const std::vector<std::string> &dics, 
                              std::map<std::string, std::vector<stDictionary> >& result)
{
    if (!isOpen()) {
        return false;
    }
    if (dics.empty()) {
        return true;
    }
    const int dicCount = dics.size();
    stDictionary stDic;
    std::vector<stDictionary> table;
    for (int i = 0; i < dicCount; ++i) {
        QString sql = QString("select * from %1").arg(dics[i].c_str());
        QSqlQuery query(d->q_db);
        // 无效数据
        if (!query.exec(sql)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            continue;
        }
        while (query.next()) {
            stDic.nCode = query.value("code").toInt();
            stDic.sDec = query.value("describe").toString().toStdString();
            stDic.sSign = query.value("sign").toString().toStdString();
            stDic.sRemark = query.value("remark").toString().toStdString();

            table.push_back(stDic);
        }
        result[dics[i]] = table;
        table.clear();
    }

    return true;
}

/**
* @brief 读取本地字典表数据（数据库无法连接时使用，注意与数据数据同步）
* @param [out] dic : 所有字典数据<字典标识, <字典数据集>>
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::readLocalDictionary(std::map<std::string, std::vector<stDictionary> >& dic)
{
    stDictionary stDic;
    std::vector<stDictionary> table;
    // 校验类型
    {
        stDic.nCode = GlobalDefine::ctInvalid;
        stDic.sDec = QStringLiteral("无校验").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ct8And;
        stDic.sDec = QStringLiteral("8位和校验").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ct16And;
        stDic.sDec = QStringLiteral("16位和校验").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ctCRC;
        stDic.sDec = QStringLiteral("CRC校验").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ctXor8;
        stDic.sDec = QStringLiteral("8位异或和校验").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ctXor16;
        stDic.sDec = QStringLiteral("16位异或和校验").toStdString();
        table.push_back(stDic);

        dic[tableName(DataBaseDefine::dbCheckType)] = table;
        table.clear();
    }

    // 帧计数类型
    {
        stDic.nCode = GlobalDefine::counterU8;
        stDic.sDec = QStringLiteral("8位计数").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::counterU16;
        stDic.sDec = QStringLiteral("16位计数").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::counterU32;
        stDic.sDec = QStringLiteral("32位计数").toStdString();
        table.push_back(stDic);
        dic[tableName(DataBaseDefine::dbCounterType)] = table;
        table.clear();
    }

    // 数据类型
    {
        stDic.nCode = GlobalDefine::dtHead;
        stDic.sDec = QStringLiteral("包头").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtCounter;
        stDic.sDec = QStringLiteral("帧计数").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtCheck;
        stDic.sDec = QStringLiteral("校验").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtFrameCode;
        stDic.sDec = QStringLiteral("帧识别码").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtU8;
        stDic.sDec = QStringLiteral("无符号8位").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dt8;
        stDic.sDec = QStringLiteral("有符号8位").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtU16;
        stDic.sDec = QStringLiteral("无符号16位").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dt16;
        stDic.sDec = QStringLiteral("有符号16位").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtU32;
        stDic.sDec = QStringLiteral("无符号32位").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dt32;
        stDic.sDec = QStringLiteral("有符号32位").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtF32;
        stDic.sDec = QStringLiteral("32位浮点数").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtF64;
        stDic.sDec = QStringLiteral("64位浮点数").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtBitMap;
        stDic.sDec = QStringLiteral("比特映射").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtBitValue;
        stDic.sDec = QStringLiteral("比特值").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtComplex;
        stDic.sDec = QStringLiteral("复合数据").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtDiscern;
        stDic.sDec = QStringLiteral("帧数据").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtBuffer;
        stDic.sDec = QStringLiteral("数据预留区").toStdString();
        table.push_back(stDic);
        dic[tableName(DataBaseDefine::dbDataType)] = table;
        table.clear();
    }

    // 权限类型
    {
    }

    return true;
}

/**
* @brief 读取机型基本信息
* @param [out] plane : 机型信息集
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::readPlane(std::vector<stPlane> &plane)
{
    if (!isOpen()) {
        return false;
    }
    QString sql = QString("select * from %1 order by describe")
            .arg(tableName(DataBaseDefine::dbPlane).c_str());
    QSqlQuery query(d->q_db);
    // 无效数据
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    stPlane stBase;
    while (query.next()) {

        stBase.nCode = query.value("code").toInt();
        stBase.sName = query.value("describe").toString().toStdString();
        stBase.sSign = query.value("sign").toString().toStdString();
        stBase.sDescribe = query.value("remark").toString().toStdString();
        plane.push_back(stBase);
    }

    return true;
}

/**
* @brief 批量保存机型数据（完整）
* @param [in] infrastructure : 系统结构数据<机型信息, <分系统信息集>>
* @param [in] tableRules : 表规则数据<ICD表信息, <表规则信息集>>
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::savePlanes(const DMSpace::_vectorPS &infrastructure,
                          const DMSpace::_vectorIcdTR &tableRules)
{
    if (!isOpen()) {
        return false;
    }
    int count = 0;;
    QString sql;
    QString cdtPlane;
    QString cdtSystem;
    // plane
    DMSpace::_vectorPS_Cit itPS = infrastructure.begin();
    for (; itPS != infrastructure.end(); ++itPS) {
        const stPlane &plane = itPS->first;
        cdtPlane.append(QString("select '%1' code,'%2' describe,'%3' sign,"
                                "'%4' remark union ").arg(plane.nCode)
                        .arg(plane.sName.c_str()).arg(plane.sSign.c_str())
                        .arg(plane.sDescribe.c_str()));
        // system
        const std::vector<stSystem> &systems = itPS->second;
        count = systems.size();
        for (int i = 0; i < count; ++i) {
            const stSystem &system = systems[i];
            cdtSystem.append(QString("select '%1' typeCode,'%2' systemCode,'%3'"
                                     " describe,'%4' sign,'%5' remark union ")
                             .arg(plane.nCode).arg(system.nCode)
                             .arg(system.sName.c_str()).arg(system.sSign.c_str())
                             .arg(system.sDescribe.c_str()));
        }
    }
    cdtPlane.chop(QString(" union ").size());
    cdtSystem.chop(QString(" union ").size());
    if (!cdtPlane.isEmpty()) {
        // plane sql
        sql.append(QString("merge %1 a using (%2) b on (a.code=b.code) when"
                           " matched then update set a.describe=b.describe,"
                           "a.sign=b.sign,a.remark=b.remark when not matched"
                           " then insert(code,describe,sign,remark) values"
                           "(b.code,b.describe,b.sign,b.remark);\r\n")
                   .arg(tableName(DataBaseDefine::dbPlane).c_str()).arg(cdtPlane));
    }
    if (!cdtSystem.isEmpty()) {
        // system sql
        sql.append(QString("merge %1 a using (%2) b on (a.typeCode=b.typeCode"
                           " and a.systemCode=b.systemCode) when matched then"
                           " update set a.describe=b.describe,a.sign=b.sign,"
                           "a.remark=b.remark when not matched then insert"
                           "(typeCode,systemCode,describe,sign,remark) values"
                           "(b.code,b.describe,b.sign,b.remark);\r\n")
                   .arg(tableName(DataBaseDefine::dbSystem).c_str()).arg(cdtSystem));
    }

    QString cdtICD;
    QString cdtRule;
    QString dbSubTables;
    QString cdtDeleteICD;
    QStringList lstRule;
    std::vector<std::string> subTables;
    // icdbase
    DMSpace::_vectorIcdTR_Cit itBR = tableRules.begin();
    for (; itBR != tableRules.end(); ++itBR) {
        QString rulCmd;
        const stICDBase &base = itBR->first;
        cdtICD.append(QString("select '%1' tableName,'%2' ptName,'%3' id,"
                              "'%4' code,'%5' length,'%6' bGroup,'%7' describe,"
                              "'%8' remark union ").arg(base.sName.c_str())
                      .arg(base.sParentName.c_str()).arg(base.sID.c_str())
                      .arg(base.sCode.c_str()).arg(base.nLength)
                      .arg(base.sGroup.c_str()).arg(base.sDescribe.c_str())
                      .arg(base.sRemark.c_str()));
        // exist tables
        if (!dbSubTables.contains(QString::fromStdString(base.sName))) {
            subTables = querySubTables(base.sName);
            const int bound = subTables.size();
            for (int i = 0; i < bound; ++i) {
                dbSubTables.append(
                            QString("select '%1' tableName union ")
                            .arg(subTables[i].c_str()));
            }
        }
        // rules
        const std::vector<stTableRules> &rules = itBR->second;
        count = rules.size();
        for (int i = 0; i < count; ++i) {
            const stTableRules &tableRule = rules[i];
            rulCmd.append(QString("('%1','%2','%3','%4','%5','%6',"
                                  "'%7','%8','%9','%10','%11','%12'),")
                          .arg(tableRule.nCode).arg(tableRule.nSerial)
                          .arg(tableRule.sName.c_str())
                          .arg(tableRule.sPrgCode.c_str()).arg(tableRule.uType)
                          .arg(tableRule.sDefault.c_str())
                          .arg(tableRule.sRange.c_str())
                          .arg(tableRule.dOffset, 0, 'g', 16)
                          .arg(tableRule.sUnit.c_str())
                          .arg(tableRule.dScale, 0, 'g', 16)
                          .arg(tableRule.sRule.c_str())
                          .arg(tableRule.sRemark.c_str()));
        }
        if (!rulCmd.isEmpty()) {
            rulCmd.chop(QString(",").size());
            rulCmd = QString("delete from %1;insert into %1 (number,serial"
                             ",name,code,type,defaultValue,range,offset,"
                             "unit,scale,regulation,remark) values %2;")
                    .arg(base.sName.c_str()).arg(rulCmd);
            cdtRule.append(rulCmd);
        }
        if (cdtRule.size() > 1000) {    // 限制单条语句长度为1000
            lstRule << cdtRule;
            cdtRule.clear();
        }
    }
    if (!cdtRule.isEmpty()) {
        lstRule << cdtRule;
    }
    cdtICD.chop(QString(" union ").size());

    QSqlQuery query(d->q_db);
    // 删除ICD表
    if (!dbSubTables.isEmpty() && !cdtICD.isEmpty()) {
        dbSubTables.chop(QString(" union ").size());
        QString cmd = QString("select a.tableName from (%1) a except"
                              " select b.tableName from (%2) b")
                .arg(dbSubTables).arg(cdtICD);
        if (!query.exec(cmd)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            return false;
        }
        QString name;
        while (query.next()) {
            name = query.value(0).toString();
            cdtDeleteICD.append(QString("drop table %1;").arg(name));
            cdtDeleteICD.append(
                        QString("delete from %1 where tableName='%2';")
                        .arg(d->q_icd.c_str()).arg(name));
        }
    }

    // 事务处理
    d->q_db.transaction();
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }

    if (!cdtICD.isEmpty()) {
        // 先查询表是否存在，如果不存在则先创建
        QString cdtCreate = QString("select a.tableName from (%1) a except"
                                    " select name from sysobjects where"
                                    " xtype='u'").arg(cdtICD);
        // 无效数据
        if (!query.exec(cdtCreate)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            return false;
        }
        cdtCreate.clear();
        while (query.next()) {
            cdtCreate += createICDTableCmd(query.value(0).toString()
                                           .toStdString()).c_str();
        }
        // ICD sql
        cdtCreate = QString("merge %1 a using (%2) b on (a.tableName"
                            "=b.tableName) when matched then update set"
                            " a.ptName=b.ptName,a.id=b.id,a.code=b.code,"
                            "a.length=b.length,a.bGroup=b.bGroup,"
                            "a.describe=b.describe,a.remark=b.remark when"
                            " not matched then insert(tableName,ptName,id,"
                            "code,length,bGroup,describe,remark) values"
                            "(b.tableName,b.ptName,b.id,b.code,b.length,"
                            "b.bGroup,b.describe,b.remark);")
                .arg(d->q_icd.c_str()).arg(cdtICD);
        if (!query.exec(cdtCreate)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            d->q_db.rollback();
            return false;
        }
    }
    // 删除表
    if (!cdtDeleteICD.isEmpty()) {
        qDebug() << cdtDeleteICD.size() << " " << cdtDeleteICD << "\r\n";
        if (!query.exec(cdtDeleteICD)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            d->q_db.rollback();
            return false;
        }
    }
    // rule sql
    for (int i = 0; i < lstRule.size(); ++i) {
        const QString &cmd = lstRule[i];
        if (!query.exec(cmd)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            d->q_db.rollback();
            return false;
        }
    }
    if (!d->q_db.commit()) {   // 批量提交
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief 保存机型
* @param [in] plane : 机型信息
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::savePlane(const stPlane& plane)
{
    if (!isOpen()) {
        return false;
    }
    QString sql = QString("merge %1 a using (select '%2' code,'%3'"
                          "describe,'%4' sign,'%5' remark) b on "
                          "(a.code=b.code) when matched then update"
                          " set a.describe=b.describe,a.sign=b.sign,"
                          "a.remark=b.remark when not matched then"
                          " insert(code,describe,sign,remark) values"
                          "(b.code,b.describe,b.sign,b.remark)"
                          "output $action,inserted.*,deleted.*;")
            .arg(tableName(DataBaseDefine::dbPlane).c_str())
            .arg(plane.nCode)
            .arg(plane.sName.c_str())
            .arg(plane.sSign.c_str())
            .arg(plane.sDescribe.c_str());
    QSqlQuery query(d->q_db);

    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }

    return true;
}

/**
* @brief 保存机型
* @param [in] plane_system : 机型-系统信息
* @param [in] rules : 表规则数据
* @param [in] dataLevel : 待保存数据层级（GlobalDefine::NodeType）
* @param [in] dbSource : 数据库数据源标识
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::savePlane(const DMSpace::_vectorPS &plane_system,
                         const DMSpace::_vectorIcdTR &rules,
                         int dataLevel, bool dbSource)
{
    if (!isOpen()) {
        return false;
    }
    if (plane_system.empty()) {
        // 清空数据库
        return clearDataBase();
    }
    int count = 0;
    QString sql;
    QString cdtPlane;
    QString cdtDeletePlane;
    QString cdtSystem;
    QString cdtDeleteSystem;
    QString cdtDeleteTable;
    QStringList lstDeleteSystem;
    QStringList lstDeleteICD;
    QStringList lstGroup;
    DMSpace::_vectorPS_Cit itPS = plane_system.begin();
    for (; itPS != plane_system.end(); ++itPS) {
        // 机型sql
        const stPlane &plane = itPS->first;
        cdtPlane.append(QString("select '%1' code,'%2' describe,'%3' sign,"
                                "'%4' remark union ").arg(plane.nCode).arg(plane.sName.c_str())
                        .arg(plane.sSign.c_str()).arg(plane.sDescribe.c_str()));
        // 系统信息sql
        const std::vector<stSystem> &systems = itPS->second;
        count = systems.size();
        for (int i = 0; i < count; ++i) {
            const stSystem &system = systems[i];
            sql.append(QString("select '%1' typeCode,'%2' systemCode,'%3'"
                               " describe,'%4' sign,'%5' remark union ").arg(plane.nCode)
                       .arg(system.nCode).arg(system.sName.c_str())
                       .arg(system.sSign.c_str()).arg(system.sDescribe.c_str()));
            lstGroup << QString("%1/%2").arg(plane.nCode).arg(system.nCode);
        }
        cdtSystem.append(sql);
        // 删除系统sql
        if (sql.isEmpty()) {
            cdtDeleteSystem.append(QString("delete from %1 where typeCode"
                                           "='%2';")
                                   .arg(tableName(DataBaseDefine::dbSystem).c_str())
                                   .arg(plane.nCode));
            cdtDeleteTable.append(QString("select RTRIM(LTRIM"
                                          "(str(typeCode)))+'/'+ RTRIM"
                                          "(LTRIM(str(systemCode))) from %1"
                                          " where typeCode ='%3' union ")
                                  .arg(tableName(DataBaseDefine::dbSystem).c_str())
                                  .arg(plane.nCode));
        } else {
            sql.chop(QString(" union ").size());
            cdtDeleteSystem.append(QString("delete from %1 where typeCode"
                                           "='%2' and systemCode not in"
                                           "(select b.systemCode from(%3)"
                                           " b);")
                                   .arg(tableName(DataBaseDefine::dbSystem).c_str())
                                   .arg(plane.nCode).arg(sql));
            cdtDeleteTable.append(QString("select RTRIM(LTRIM"
                                          "(str(typeCode)))+'/'+ RTRIM"
                                          "(LTRIM(str(systemCode))) from %1"
                                          " where typeCode ='%2' and"
                                          " systemCode not in(select"
                                          " a.systemCode from(%4)a) union ")
                                  .arg(tableName(DataBaseDefine::dbSystem).c_str())
                                  .arg(plane.nCode).arg(sql));
        }
        sql.clear();
        if (cdtDeleteSystem.size() > 1000) {    // 限制单条语句长度为1000
            lstDeleteSystem << cdtDeleteSystem;
            cdtDeleteSystem.clear();
        }
    }
    if (!cdtDeleteSystem.isEmpty()) {
        lstDeleteSystem << cdtDeleteSystem;
    }
    cdtPlane.chop(QString(" union ").size());
    cdtSystem.chop(QString(" union ").size());
    cdtDeleteTable.chop(QString(" union ").size());
    // 表规则
    QString cdtICD;
    QString cdtRule;
    QStringList lstRule;
    DMSpace::_vectorIcdTR_Cit itR = rules.begin();
    for (; itR != rules.end(); ++itR) {
        const stICDBase &icdBase = itR->first;
        const std::vector<stTableRules> &tableRules = itR->second;
        // 基本信息表
        cdtICD.append(QString("select '%1' tableName,'%2' ptName,'%3' id,"
                              "'%4' code,'%5' length,'%6' bGroup,'%7' describe,"
                              "'%8' remark union ").arg(icdBase.sName.c_str())
                      .arg(icdBase.sParentName.c_str())
                      .arg(icdBase.sID.c_str()).arg(icdBase.sCode.c_str())
                      .arg(icdBase.nLength).arg(icdBase.sGroup.c_str())
                      .arg(icdBase.sDescribe.c_str()).arg(icdBase.sRemark.c_str()));
        // 规则数据
        count = tableRules.size();
        QString rulCmd;
        for (int i = 0; i < count; ++i) {
            const stTableRules &tableRule = tableRules[i];
            rulCmd.append(QString("('%1','%2','%3','%4','%5','%6','%7',"
                                  "'%8','%9','%10','%11','%12'),").arg(tableRule.nCode)
                          .arg(tableRule.nSerial).arg(tableRule.sName.c_str())
                          .arg(tableRule.sPrgCode.c_str()).arg(tableRule.uType)
                          .arg(tableRule.sDefault.c_str())
                          .arg(tableRule.sRange.c_str())
                          .arg(tableRule.dOffset, 0, 'g', 16)
                          .arg(tableRule.sUnit.c_str())
                          .arg(tableRule.dScale, 0, 'g', 16)
                          .arg(tableRule.sRule.c_str())
                          .arg(tableRule.sRemark.c_str()));
        }
        if (!rulCmd.isEmpty()) {
            rulCmd.chop(QString(",").size());
            rulCmd = QString("insert into %1 (number,serial,name,"
                             "code,type,defaultValue,range,offset,unit,"
                             "scale,regulation,remark) values %2;")
                    .arg(icdBase.sName.c_str()).arg(rulCmd);
        }
        rulCmd.prepend(QString("delete from %1;")
                       .arg(icdBase.sName.c_str()));
        cdtRule.append(rulCmd);
        if (cdtRule.size() > 1000) {    // 限制单条语句长度为1000
            lstRule << cdtRule;
            cdtRule.clear();
        }
    }
    if (!cdtRule.isEmpty()) {
        lstRule << cdtRule;
    }
    cdtICD.chop(QString(" union ").size());
    QString tableId;
    QStringList lstNewICD;
    QSqlQuery query(d->q_db);
    if (!cdtICD.isEmpty()) {
        QString cdtCreate;
        // 先查询表是否存在，如果不存在则先创建
        sql = QString("select a.tableName from (%1) a except select name"
                      " from sysobjects where xtype='u'").arg(cdtICD);
        // 无效数据
        if (!query.exec(sql)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            return false;
        }
        // 创建ICD表
        while (query.next()) {
            tableId += query.value(0).toString() + " ";
            cdtCreate += createICDTableCmd(query.value(0).toString()
                                           .toStdString()).c_str();
            if (cdtCreate.size() > 1000) {    // 限制单条语句长度为1000
                lstNewICD << cdtCreate;
                cdtCreate.clear();
            }
        }
        if (!cdtCreate.isEmpty()) {
            lstNewICD << cdtCreate;
        }
    }
    sql.clear();
    if (dbSource) { // 数据源为数据库
        if (GlobalDefine::ntTable > dataLevel) {
            if (GlobalDefine::ntSystem == dataLevel) {
                lstDeleteSystem.clear();    // 保存层次为机型时，不删除机型数据
                cdtDeleteTable.clear();
            } else if (GlobalDefine::ntPlane > dataLevel) {    // 所有机型
                cdtDeletePlane.append(QString("delete from %1 where code"
                                              " not in(select a.code from (%2) a)")
                                      .arg(tableName(DataBaseDefine::dbPlane).c_str())
                                      .arg(cdtPlane));
                lstDeleteSystem << QString("delete from %1 where typeCode in"
                                           "(select code from %2 where code"
                                           " not in(select a.code from (%3) a))")
                                   .arg(tableName(DataBaseDefine::dbSystem).c_str())
                                   .arg(tableName(DataBaseDefine::dbPlane).c_str())
                                   .arg(cdtPlane);
                if (!cdtDeleteTable.isEmpty()) {
                    cdtDeleteTable.append(" union ");
                }
                cdtDeleteTable.append(QString("select RTRIM(LTRIM"
                                              "(str(typeCode)))+'/'+ RTRIM"
                                              "(LTRIM(str(systemCode)))"
                                              " from %1 where typeCode in"
                                              "(select code from %2 where"
                                              " code not in(select code"
                                              " from (%3)b))")
                                      .arg(tableName(DataBaseDefine::dbSystem).c_str())
                                      .arg(tableName(DataBaseDefine::dbPlane).c_str())
                                      .arg(cdtPlane));
            }
            // 保存层次在ICD表之上
            count = lstGroup.size();
            for (int i = 0; i < count; ++i) {
                sql.append("'").append(lstGroup[i]).append("',");
            }
            if (!sql.isEmpty()) {
                sql.chop(QString(",").size());
                sql = QString("select a.tableName from %1 a where a.bGroup"
                              " in(%2)").arg(d->q_icd.c_str()).arg(sql);
                if (!cdtICD.isEmpty()) {
                    sql.append(QString(" except select b.tableName from"
                                       " (%3) b").arg(cdtICD));
                }
            }
        } else if (GlobalDefine::ntTable == dataLevel) {
            lstDeleteSystem.clear();    // 保存层次为表时，不删除机型数据
            cdtDeleteTable.clear();
            // 保存层次为ICD表
            // 第一条数据为顶层表，查询数据库中顶层表下的所有子表，
            // 如果在待保存数据中没有，则删除该表
            if (!rules.empty()) {
                QString dbSubTables;
                std::vector<std::string> subTables;
                for (itR = rules.begin(); itR != rules.end(); ++itR) {
                    const std::string &name = itR->first.sName;
                    if (dbSubTables.contains(QString::fromStdString(name))) {
                        continue;
                    }
                    subTables = querySubTables(name);
                    const int bound = subTables.size();
                    for (int i = 0; i < bound; ++i) {
                        dbSubTables.append(
                                    QString("select '%1' tableName union ")
                                    .arg(subTables[i].c_str()));
                    }
                }
                if (!dbSubTables.isEmpty() && !cdtICD.isEmpty()) {
                    dbSubTables.chop(QString(" union ").size());
                    sql = QString("select a.tableName from (%1) a except"
                                  " select b.tableName from (%2) b")
                            .arg(dbSubTables).arg(cdtICD);
                }
            }
        }
    } else {    // 数据源不为数据库，只删除已存在表的子表
        lstDeleteSystem.clear();    // 不删除机型数据
        cdtDeleteTable.clear();
        if (!rules.empty()) {
            //if (GlobalDefine::ntTable > dataLevel) {
            // 整理到表一级
            QString dbSubTables;
            std::vector<std::string> subTables;
            for (itR = rules.begin(); itR != rules.end(); ++itR) {
                const std::string &name = itR->first.sName;
                if (dbSubTables.contains(QString::fromStdString(name))) {
                    continue;
                }
                subTables = querySubTables(name);
                const int bound = subTables.size();
                for (int i = 0; i < bound; ++i) {
                    dbSubTables.append(
                                QString("select '%1' tableName union ")
                                .arg(subTables[i].c_str()));
                }
            }
            if (!dbSubTables.isEmpty() && !cdtICD.isEmpty()) {
                dbSubTables.chop(QString(" union ").size());
                sql = QString("select a.tableName from (%1) a except"
                              " select b.tableName from (%2) b")
                        .arg(dbSubTables).arg(cdtICD);
            }
        }
        //}
    }
    if (!cdtDeleteTable.isEmpty()) {
        cdtDeleteTable = QString("select tableName from %1 where bGroup"
                                 " in(%2)")
                .arg(tableName(DataBaseDefine::dbICDBase).c_str())
                .arg(cdtDeleteTable);
    }
    // 删除ICD表
    if (!sql.isEmpty() && !cdtDeleteTable.isEmpty()) {
        sql.append(" union ");
    }
    sql.append(cdtDeleteTable);
    if (!sql.isEmpty()) {
        if (!query.exec(sql)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            return false;
        }
        QString name;
        QString cdtDeleteICD;
        while (query.next()) {
            name = query.value(0).toString();
            cdtDeleteICD.append(QString("drop table %1;").arg(name));
            cdtDeleteICD.append(
                        QString("delete from %1 where tableName='%2';")
                        .arg(d->q_icd.c_str()).arg(name));
            if (cdtDeleteICD.size() > 1000) {    // 限制单条语句长度为1000
                lstDeleteICD << cdtDeleteICD;
                cdtDeleteICD.clear();
            }
        }
        if (!cdtDeleteICD.isEmpty()) {
            lstDeleteICD << cdtDeleteICD;
        }
    }
    // 事务
    d->q_db.transaction();  // sql语句太长，分批执行
    // 删除系统
    count = lstDeleteSystem.size();
    for (int i = 0; i < count; ++i) {
        const QString &cmd = lstDeleteSystem[i];
        if (!query.exec(cmd)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            d->q_db.rollback();
            return false;
        }
    }
    sql = cdtDeletePlane; // 删除机型
    // 保存机型信息
    sql.append(QString("merge %1 a using (%2) b on (a.code=b.code) when"
                       " matched then update set a.describe=b.describe,a.sign=b.sign,"
                       "a.remark=b.remark when not matched then insert(code,describe,"
                       "sign,remark) values(b.code,b.describe,b.sign,b.remark);")
               .arg(tableName(DataBaseDefine::dbPlane).c_str())
               .arg(cdtPlane));
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }
    // 保存系统信息
    if (!cdtSystem.isEmpty()) {
        sql = QString("merge %1 a using (%2) b on (a.typeCode"
                      "=b.typeCode and a.systemCode=b.systemCode) when"
                      " matched then update set a.describe=b.describe,"
                      "a.sign=b.sign,a.remark=b.remark when not matched"
                      " then insert(typeCode,systemCode,describe,sign,"
                      "remark) values(b.typeCode,b.systemCode,b.describe,"
                      "b.sign,b.remark) output $action,inserted.*,"
                      "deleted.*;")
                .arg(tableName(DataBaseDefine::dbSystem).c_str())
                .arg(cdtSystem);
        if (!query.exec(sql)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            d->q_db.rollback();
            return false;
        }
    }
    // 新增表
    count = lstNewICD.size();
    for (int i = 0; i < count; ++i) {
        const QString &cmd = lstNewICD[i];
        if (!query.exec(cmd)) {
            d->q_error = QString("create table<%1> failed!\r\n").arg(tableId);
            d->q_error += query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            qDebug() << cmd;
            d->q_db.rollback();
            return false;
        }
    }
    // 删除表
    count = lstDeleteICD.size();
    for (int i = 0; i < count; ++i) {
        const QString &cmd = lstDeleteICD[i];
        if (!query.exec(cmd)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            d->q_db.rollback();
            return false;
        }
    }
    // 更新ICD表
    if (!cdtICD.isEmpty()) {
        sql = QString("merge %1 a using (%2) b on (a.tableName"
                      "=b.tableName) when matched then update set a.ptName"
                      "=b.ptName,a.id=b.id,a.code=b.code,a.length=b.length"
                      ",a.bGroup=b.bGroup,a.describe=b.describe,a.remark"
                      "=b.remark when not matched then insert(tableName,"
                      "ptName,id,code,length,bGroup,describe,remark)"
                      " values(b.tableName,b.ptName,b.id,b.code,b.length,"
                      "b.bGroup,b.describe,b.remark);")
                .arg(d->q_icd.c_str()).arg(cdtICD);
        if (!query.exec(sql)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            d->q_db.rollback();
            return false;
        }
    }
    // 更新规则数据
    for (int i = 0; i < lstRule.size(); ++i) {
        const QString &cmd = lstRule[i];
        if (!query.exec(cmd)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            d->q_db.rollback();
            return false;
        }
    }
    if (!d->q_db.commit()) {   // 批量提交
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief 清空数据库
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::clearDataBase()
{
    if (!isOpen()) {
        return false;
    }
    QString sql = QString("use %1;").arg(d->q_dbInfo.database.c_str());
    // 删除系统
    sql.append(QString("delete from %1;")
               .arg(tableName(DataBaseDefine::dbSystem).c_str()));
    // 删除机型
    sql.append(QString("delete from %1;")
               .arg(tableName(DataBaseDefine::dbPlane).c_str()));
    // 删除ICD基本信息表
    sql.append(QString("delete from %1;")
               .arg(tableName(DataBaseDefine::dbICDBase).c_str()));
    // 删除规则信息
    QString cdt = QString("use %1;select 'drop table '+name+';'"
                          " from sysobjects where xtype='u' and"
                          " name like 'ICDTable_%'")
            .arg(d->q_dbInfo.database.c_str());
    QSqlQuery query(d->q_db);
    if (!query.exec(cdt)) {
        // 无效数据
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    QStringList lstCmd;
    while (query.next()) {
        sql.append(query.value(0).toString());
        if (sql.size() > 1000) {    // 限制单条语句长度为1000
            lstCmd << sql;
            sql.clear();
        }
    }
    if (!sql.isEmpty()) {
        lstCmd << sql;
    }
    // 事务
    d->q_db.transaction();
    const int count = lstCmd.size();
    for (int i = 0; i < count; ++i) {
        const QString &cmd = lstCmd[i];
        if (!query.exec(cmd)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            d->q_db.rollback();
            return false;
        }
    }
    if (!d->q_db.commit()) {   // 批量提交
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief 删除机型
* @param [in] planes : 机型标识集
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::deletePlane(const std::vector<int> &planes)
{
    if (!isOpen()) {
        return false;
    }
    if (planes.empty()) {
        return true;
    }
    QString cdtEqual;
    QString cdtLike;
    cdtEqual.append("(");
    const int count = planes.size();
    for (int i = 0; i < count; ++i) {
        cdtEqual.append(QString("'%1',").arg(planes[i]));
        cdtLike.append(QString("bGroup like '%1/%' or ").arg(planes[i]));
    }
    cdtEqual.chop(QString(",").size());
    cdtEqual.append(")");
    cdtLike.chop(QString(" or ").size());

    // 删除分系统
    QString sql = QString("delete from %1 where typeCode in%2;")
            .arg(tableName(DataBaseDefine::dbSystem).c_str())
            .arg(cdtEqual);

    // 删除基本信息表
    sql += QString("delete from %1 where %2;")
            .arg(d->q_icd.c_str())
            .arg(cdtLike);

    // 删除规则表
    QString subSql = QString("select 'drop table '+tableName+';'"
                             " from %1 where %2")
            .arg(d->q_icd.c_str())
            .arg(cdtLike);
    QSqlQuery query(d->q_db);
    // 无效数据
    if (!query.exec(subSql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    while (query.next()) {
        sql.append(query.value(0).toString());
    }
    // 删除机型
    sql = QString("delete from %1 where code in%2;")
            .arg(tableName(DataBaseDefine::dbPlane).c_str())
            .arg(cdtEqual);

    // 执行删除所有相关数据命令
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }

    return true;
}

/**
 * @brief 读取系统信息（机型和分系统）
 * @param [out] system : 所有系统信息<机型码, <分系统信息集>>
 * @return 执行结果，true：成功；false：失败
 */
bool DBAccess::readSystem(std::map<int, std::vector<stSystem> > &system)
{
    if (!isOpen()) {
        return false;
    }
    QString sql = QString("select * from %1 order by typeCode,describe")
            .arg(tableName(DataBaseDefine::dbSystem).c_str());
    QSqlQuery query(d->q_db);
    // 无效数据
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    int typeCode = 0;
    stSystem stBase;
    while (query.next()) {

        typeCode = query.value("typeCode").toInt();
        stBase.nCode = query.value("systemCode").toInt();
        stBase.sName = query.value("describe").toString().toStdString();
        stBase.sSign = query.value("sign").toString().toStdString();
        stBase.sDescribe = query.value("remark").toString().toStdString();
        system[typeCode].push_back(stBase);
    }

    return true;
}

/**
* @brief 读取系统信息
* @param [in] plane : 机型码
* @param [in] system : 分系信息集
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::readSystem(int plane, std::vector<stSystem> &systems)
{
    if (!isOpen()) {
        return false;
    }
    QString sql = QString("select * from %1 where typeCode='%2' order by describe")
            .arg(tableName(DataBaseDefine::dbSystem).c_str()).arg(plane);
    QSqlQuery query(d->q_db);
    // 无效数据
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    int typeCode = 0;
    stSystem stBase;
    while (query.next()) {

        typeCode = query.value("typeCode").toInt();
        stBase.nCode = query.value("systemCode").toInt();
        stBase.sName = query.value("describe").toString().toStdString();
        stBase.sSign = query.value("sign").toString().toStdString();
        stBase.sDescribe = query.value("remark").toString().toStdString();
        systems.push_back(stBase);
    }

    return true;
}

/**
 * @brief 读取ICD表基本信息
 * @param [in] top : 只读取顶层表标识
 * @param [out] icdBase : ICD基本信息集<所属组, <字表信息集>>
 * @return 执行结果，true：成功；false：失败
 */
bool DBAccess::readICDBase(bool top,
                           std::map<std::string, std::vector<stICDBase> > &icdBase)
{
    if (!isOpen()) {
        return false;
    }
    QString sql;
    if (top) {
        sql = QString("select * from %1 where ptName=''"
                      " order by bGroup,describe").arg(d->q_icd.c_str());
    } else {
        sql = QString("select * from %1 order by bGroup,describe")
                .arg(d->q_icd.c_str());
    }
    QSqlQuery query(d->q_db);
    // 无效数据
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    int index = 0;
    while (query.next()) {
        stICDBase base;

        base.sName = query.value(index++).toString().toStdString();
        base.sParentName = query.value(index++).toString().toStdString();
        base.sID = query.value(index++).toString().toStdString();
        base.sCode = query.value(index++).toString().toStdString();
        base.nLength = query.value(index++).toInt();
        base.sGroup = query.value(index++).toString().toStdString();
        base.sDescribe = query.value(index++).toString().toStdString();
        base.sRemark = query.value(index).toString().toStdString();

        icdBase[base.sGroup].push_back(base);
        index = 0;
    }
    return true;
}

/**
* @brief 读取ICD表基本信息
* @param [in] name : ICD表名
* @param [out] icdBase : ICD基本信息
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::readICDBase(const std::string &name, stICDBase &icdBase)
{
    if (!isOpen()) {
        return false;
    }
    QString sql = QString("select * from %1 where tableName = '%2'")
            .arg(d->q_icd.c_str()).arg(name.c_str());
    QSqlQuery query(d->q_db);
    // 无效数据
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    while (query.next()) {
        icdBase.sName = query.value("tableName").toString().toStdString();
        icdBase.sParentName = query.value("ptName").toString().toStdString();
        icdBase.sID = query.value("id").toString().toStdString();
        icdBase.sCode = query.value("code").toString().toStdString();
        icdBase.nLength = query.value("length").toInt();
        icdBase.sGroup = query.value("bGroup").toString().toStdString();
        icdBase.sDescribe = query.value("describe").toString().toStdString();
        icdBase.sRemark = query.value("remark").toString().toStdString();

        break;  // 只有一条数据
    }

    return true;
}

/**
* @brief 读取ICD表基本信息
* @param [in] name : ICD表名集
* @param [out] icdBase : ICD基本信息集
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::readICDBase(const std::vector<std::string> &names,
                           std::vector<stICDBase> &icdBase)
{
    if (!isOpen()) {
        return false;
    }
    if (names.empty()) {
        return true;
    }
    QString cdtNames;
    const int count = names.size();
    for (int i = 0; i < count; ++i) {
        cdtNames.append("'")
                .append(QString::fromStdString(names[i])).append("',");
    }
    cdtNames.chop(1);
    QString sql = QString("select * from %1 where tableName in(%2)")
            .arg(d->q_icd.c_str()).arg(cdtNames);
    QSqlQuery query(d->q_db);
    if (!query.exec(sql)) {
        // 无效数据
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    int index = 0;
    stICDBase base;
    while (query.next()) {
        base.sName = query.value(index++).toString().toStdString();
        base.sParentName = query.value(index++).toString().toStdString();
        base.sID = query.value(index++).toString().toStdString();
        base.sCode = query.value(index++).toString().toStdString();
        base.nLength = query.value(index++).toInt();
        base.sGroup = query.value(index++).toString().toStdString();
        base.sDescribe = query.value(index++).toString().toStdString();
        base.sRemark = query.value(index).toString().toStdString();

        icdBase.push_back(base);
        index = 0;
    }

    return true;
}

/**
* @brief 读取特定条件的ICD表基本信息
* @param [in] condition : 过滤条件（单独的机型码或者机型码和系统码的组合“xx/xx”）
* @param [in] ICDBase : <父表标识, <表标识, ICD表集>>
* @param [in] topOnly : 顶层表标识
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::readSpecificICD(const std::string &condition,
                               DMSpace::_vectorSB &ICDBase, bool topOnly)
{
    if (!isOpen()) {
        return false;
    }
    QString sql;
    if (topOnly) {
        sql = QString("select * from %1 where bGroup like '%2%' and"
                      " ptName='' order by bGroup,describe").arg(d->q_icd.c_str())
                .arg(condition.c_str());
    } else {
        sql = QString("select * from %1 where bGroup like '%2%'"
                      " order by bGroup,describe").arg(d->q_icd.c_str())
                .arg(condition.c_str());
    }
    QSqlQuery query(d->q_db);
    // 无效数据
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    int index = 0;
    std::string preName("top"); // 顶层表为空，先随意负值
    std::vector<stICDBase> icdBases;
    while (query.next()) {
        stICDBase base;

        base.sName = query.value(index++).toString().toStdString();
        base.sParentName = query.value(index++).toString().toStdString();
        base.sID = query.value(index++).toString().toStdString();
        base.sCode = query.value(index++).toString().toStdString();
        base.nLength = query.value(index++).toInt();
        base.sGroup = query.value(index++).toString().toStdString();
        base.sDescribe = query.value(index++).toString().toStdString();
        base.sRemark = query.value(index++).toString().toStdString();

        if (preName != base.sParentName) {
            ICDBase.push_back(std::make_pair(preName, icdBases));
            icdBases.clear();  // 清空
            preName = base.sParentName;
        }
        icdBases.push_back(base);
        index = 0;
    }
    // 最后一张表数据
    ICDBase.push_back(std::make_pair(preName, icdBases));

    return true;
}

/**
 * @brief 读取用户权限信息
 * @param [in] user : 用户名
 * @param [out] dic : 该用户的所有表权限信息
 * @return 执行结果，true：成功；false：失败
 */
bool DBAccess::readUserTables(const std::string &/*user*/, std::vector<stTablePrivilege>)
{
    // TODO
    return true;
}

/**
 * @brief 读取表结构数据
 * @param [in] table : 表名
 * @param [out] rules : 表结构数据<规则名, 规则数据>
 * @return 执行结果，true：成功；false：失败
 */
bool DBAccess::readTableRules(const std::string &table, DMSpace::srMap &rules)
{
    if (!isOpen()) {
        return false;
    }
    QString sql = QString("select * from %1 order by serial")
            .arg(table.c_str());
    QSqlQuery query(d->q_db);
    // 无效数据
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    int index = 0;
    while (query.next()) {
        stTableRules rule;

        rule.nCode = query.value(index++).toInt();
        rule.nSerial = query.value(index++).toInt();
        rule.sName = query.value(index++).toString().toStdString();
        rule.sPrgCode = query.value(index++).toString().toStdString();
        rule.uType = (unsigned short)query.value(index++).toInt();
        rule.sDefault = query.value(index++).toString().toStdString();
        rule.sRange = query.value(index++).toString().toStdString();
        rule.dOffset = query.value(index++).toDouble();
        rule.sUnit = query.value(index++).toString().toStdString();
        rule.dScale = query.value(index++).toDouble();
        rule.sRule = query.value(index++).toString().toStdString();
        rule.sRemark = query.value(index++).toString().toStdString();

        rules[rule.nSerial] = rule;
        index = 0;
    }

    return true;
}

/**
 * @brief 读取表结构数据
 * @param [in] tables : 表名集
 * @param [out] rules : 表结构数据集<表名, <规则数据集>>
 * @return 执行结果，true：成功；false：失败
 */
bool DBAccess::readTableRules(const std::vector<std::string> &tables,
                              DMSpace::svrMap &rules)
{
    if (!isOpen()) {
        return false;
    }
    if (tables.empty()) {
        return true;
    }
    QString sql;
    const int count(tables.size());
    for (int i = 0; i < count; ++i) {
        sql.append(QString("select '%1' tableName,* from %1 union ")
                   .arg(tables[i].c_str()));
    }
    sql.chop(QString("union ").size());
    sql.append("order by tableName,serial;");
    QSqlQuery query(d->q_db);
    // 无效数据
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    int index = 0;
    std::string name;
    std::string preName;
    std::vector<stTableRules> tableRules;
    while (query.next()) {
        stTableRules rule;

        name = query.value(index++).toString().toStdString();
        rule.nCode = query.value(index++).toInt();
        rule.nSerial = query.value(index++).toInt();
        rule.sName = query.value(index++).toString().toStdString();
        rule.sPrgCode = query.value(index++).toString().toStdString();
        rule.uType = (unsigned short)query.value(index++).toInt();
        rule.sDefault = query.value(index++).toString().toStdString();
        rule.sRange = query.value(index++).toString().toStdString();
        rule.dOffset = query.value(index++).toDouble();
        rule.sUnit = query.value(index++).toString().toStdString();
        rule.dScale = query.value(index++).toDouble();
        rule.sRule = query.value(index++).toString().toStdString();
        rule.sRemark = query.value(index++).toString().toStdString();

        if (preName != name) {
            if (!preName.empty()) {
                rules[preName] = tableRules;
                tableRules.clear();  // 清空
            }
            preName = name;
        }
        tableRules.push_back(rule);
        index = 0;
    }
    // 最后一张表数据
    rules[preName] = tableRules;

    return true;
}

/**
 * @brief 保存表规则数据
 * @param [in] icdBase : 表基本信息
 * @param [in] rules : 表规则数据
 * @param [in] subTables : 子表基本信息
 * @return 执行结果，true：成功；false：失败
 */
bool DBAccess::saveTableRules(const stICDBase &icdBase,
                              const DMSpace::srMap &rules,
                              const std::vector<stICDBase> &subTables)
{
    if (!isOpen()) {
        return false;
    }
    //
    QString cdtRule;
    DMSpace::srMap::const_iterator it = rules.begin();
    // 规则数据
    for (; it != rules.end(); ++it) {
        const stTableRules& tableRule(it->second);
        cdtRule.append(QString("('%1','%2','%3','%4','%5','%6','%7',"
                               "'%8','%9','%10','%11','%12'),").arg(tableRule.nCode)
                       .arg(tableRule.nSerial).arg(tableRule.sName.c_str())
                       .arg(tableRule.sPrgCode.c_str()).arg(tableRule.uType)
                       .arg(tableRule.sDefault.c_str())
                       .arg(tableRule.sRange.c_str())
                       .arg(tableRule.dOffset, 0, 'g', 16)
                       .arg(tableRule.sUnit.c_str())
                       .arg(tableRule.dScale, 0, 'g', 16)
                       .arg(tableRule.sRule.c_str())
                       .arg(tableRule.sRemark.c_str()));
    }
    if (!cdtRule.isEmpty()) {
        cdtRule.chop(QString(",").size());
        cdtRule = QString("insert into %1 (number,serial,name,code"
                          ",type,defaultValue,range,offset,unit,scale,"
                          "regulation,remark) values %2;")
                .arg(icdBase.sName.c_str()).arg(cdtRule);
    }
    cdtRule.prepend(QString("delete from %1;").arg(icdBase.sName.c_str()));

    QString sql;
    QSqlQuery query(d->q_db);
    // 先查询表是否存在，如果不存在则先创建
    sql = QString("select name from sysobjects where xtype='u'"
                  " and name='%1'").arg(icdBase.sName.c_str());
    // 无效数据
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    // 规则表操作
    if (0 == query.numRowsAffected()) { // 创建表
        sql = QString::fromStdString(createICDTableCmd(icdBase.sName));
    }
    QString cdtICD;
    // 基本信息表数据更新
    cdtICD = QString("select '%1' tableName,'%2' ptName,'%3' id,"
                     "'%4' code,'%5' length,'%6' bGroup,'%7' describe,"
                     "'%8' remark").arg(icdBase.sName.c_str())
            .arg(icdBase.sParentName.c_str())
            .arg(icdBase.sID.c_str()).arg(icdBase.sCode.c_str())
            .arg(icdBase.nLength).arg(icdBase.sGroup.c_str())
            .arg(icdBase.sDescribe.c_str())
            .arg(icdBase.sRemark.c_str());

    sql += QString("merge %1 a using (%2) b on (a.tableName"
                   "=b.tableName) when matched then update set"
                   " a.ptName=b.ptName,a.id=b.id,a.code=b.code,"
                   "a.length=b.length,a.bGroup=b.bGroup,"
                   "a.describe=b.describe,a.remark=b.remark"
                   " when not matched then insert(tableName,"
                   "ptName,id,code,length,bGroup,describe,remark)"
                   " values(b.tableName,b.ptName,b.id,b.code,"
                   "b.length,b.bGroup,b.describe,b.remark)"
                   "output $action,inserted.*,deleted.*;")
            .arg(d->q_icd.c_str()).arg(cdtICD);

    // 具体规则数据
    sql.append(cdtRule);
    // 子表数据
    QString cdtSubTable;
    const int count = subTables.size();
    for (int i = 0; i < count; ++i) {
        const stICDBase &base = subTables[i];
        cdtSubTable.append(QString("select '%1' tableName,'%2' ptName,"
                                   "'%3' id,'%4' code,'%5' length,'%6' bGroup,"
                                   "'%7' describe,'%8' remark")
                           .arg(base.sName.c_str())
                           .arg(base.sParentName.c_str()).arg(base.sID.c_str())
                           .arg(base.sCode.c_str()).arg(base.nLength)
                           .arg(base.sGroup.c_str()).arg(base.sDescribe.c_str())
                           .arg(base.sRemark.c_str())).append(" union ");
    }
    cdtSubTable.chop(QString(" union ").size());
    if (!cdtSubTable.isEmpty()) {
        // 先查询表是否存在，如果不存在则先创建
        QString cdtCreate
                = QString("select a.tableName from (%1) a except select name"
                          " from sysobjects where xtype='u'").arg(cdtSubTable);
        // 无效数据
        if (!query.exec(cdtCreate)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            return false;
        }
        while (query.next()) {
            sql += createICDTableCmd(query.value(0).toString()
                                     .toStdString()).c_str();
        }
        // 构造sql
        sql += QString("merge %1 a using (%2) b on (a.tableName"
                       "=b.tableName) when matched then update set"
                       " a.ptName=b.ptName,a.id=b.id,a.code=b.code,"
                       "a.length=b.length,a.bGroup=b.bGroup,"
                       "a.describe=b.describe,a.remark=b.remark"
                       " when not matched then insert(tableName,"
                       "ptName,id,code,length,bGroup,describe,remark)"
                       " values(b.tableName,b.ptName,b.id,b.code,"
                       "b.length,b.bGroup,b.describe,b.remark)"
                       "output $action,inserted.*,deleted.*;")
                .arg(d->q_icd.c_str()).arg(cdtSubTable);
    }
    // 事务处理
    d->q_db.transaction();
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }
    if (!d->q_db.commit()) {   // 批量提交
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief 批量保存表规则数据
* @param [in] rules : 表规则数据
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::saveTableRules(const std::vector<std::pair<stICDBase, DMSpace::srMap>> &rules)
{
    if (!isOpen()) {
        return false;
    }
    if (rules.empty()) {
        return true;
    }
    // 第一条数据为顶层表，查询数据库中顶层表下的所有子表，如果在待保存数据中
    // 没有，则删除该表
    std::vector<std::string> subTables = querySubTables(rules[0].first.sName);
    QString dbSubTables;
    int count = subTables.size();
    for (int i = 0; i < count; ++i) {
        dbSubTables.append(QString("select '%1' tableName union ")
                           .arg(subTables[i].c_str()));
    }
    dbSubTables.chop(QString(" union ").size());

    QString cdtICD;
    QString cdtRule;
    QStringList lstRule;
    count = rules.size();
    for (int i = 0; i < count; ++i) {
        const std::pair<stICDBase, DMSpace::srMap> &data = rules[i];
        const stICDBase &icdBase = data.first;
        const DMSpace::srMap &mapRules = data.second;
        // 基本信息表数据更新
        cdtICD.append(QString("select '%1' tableName,'%2' ptName,'%3' id,"
                              "'%4' code,'%5' length,'%6' bGroup,'%7' describe,"
                              "'%8' remark union ").arg(icdBase.sName.c_str())
                      .arg(icdBase.sParentName.c_str())
                      .arg(icdBase.sID.c_str()).arg(icdBase.sCode.c_str())
                      .arg(icdBase.nLength).arg(icdBase.sGroup.c_str())
                      .arg(icdBase.sDescribe.c_str()).arg(icdBase.sRemark.c_str()));
        QString rulCmd;
        DMSpace::srMap::const_iterator it = mapRules.begin();
        // 规则数据
        for (; it != mapRules.end(); ++it) {
            const stTableRules& tableRule(it->second);
            rulCmd.append(QString("('%1','%2','%3','%4','%5','%6','%7',"
                                  "'%8','%9','%10','%11','%12'),")
                          .arg(tableRule.nCode).arg(tableRule.nSerial)
                          .arg(tableRule.sName.c_str())
                          .arg(tableRule.sPrgCode.c_str()).arg(tableRule.uType)
                          .arg(tableRule.sDefault.c_str())
                          .arg(tableRule.sRange.c_str())
                          .arg(tableRule.dOffset, 0, 'g', 16)
                          .arg(tableRule.sUnit.c_str())
                          .arg(tableRule.dScale, 0, 'g', 16)
                          .arg(tableRule.sRule.c_str())
                          .arg(tableRule.sRemark.c_str()));
        }
        if (!rulCmd.isEmpty()) {
            rulCmd.chop(QString(",").size());
            rulCmd = QString("insert into %1 (number,serial,name,"
                             "code,type,defaultValue,range,offset,unit,"
                             "scale,regulation,remark) values %2;")
                    .arg(icdBase.sName.c_str()).arg(rulCmd);
            cdtRule.append(rulCmd);
        }
        cdtRule.prepend(QString("delete from %1;")
                        .arg(icdBase.sName.c_str()));

        if (cdtRule.size() > 1000) {    // 限制单条语句长度为1000
            lstRule << cdtRule;
            cdtRule.clear();
        }
    }
    if (!cdtRule.isEmpty()) {
        lstRule << cdtRule;
    }
    cdtICD.chop(QString(" union ").size());

    // 先查询表是否存在，如果不存在则先创建
    QString sql = QString("select a.tableName from (%1) a except select name"
                          " from sysobjects where xtype='u'").arg(cdtICD);
    // 无效数据
    QSqlQuery query(d->q_db);
    QString cdtCreate;
    QString tableId;
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    // 创建ICD表
    while (query.next()) {
        tableId += query.value(0).toString() + " ";
        cdtCreate += createICDTableCmd(query.value(0).toString()
                                       .toStdString()).c_str();
    }
    // 删除ICD表
    QString cdtDeleteICD;
    if (!dbSubTables.isEmpty()) {
        sql = QString("select a.tableName from (%1) a except select"
                      " b.tableName from (%2) b")
                .arg(dbSubTables).arg(cdtICD);
        if (!query.exec(sql)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            return false;
        }
        QString name;
        while (query.next()) {
            name = query.value(0).toString();
            cdtDeleteICD.append(QString("drop table %1;").arg(name));
            cdtDeleteICD.append(QString("delete from %1 where tableName"
                                        "='%2';").arg(d->q_icd.c_str()).arg(name));
        }
    }
    // 事务
    d->q_db.transaction();  // sql语句太长，分批执行
    sql = cdtCreate;
    sql.append(cdtDeleteICD);
    // 更新ICD表
    sql.append(QString("merge %1 a using (%2) b on (a.tableName=b.tableName)"
                       " when matched then update set a.ptName=b.ptName,a.id"
                       "=b.id,a.code=b.code,a.length=b.length,a.bGroup"
                       "=b.bGroup,a.describe=b.describe,a.remark=b.remark when"
                       " not matched then insert(tableName,ptName,id,code,"
                       "length,bGroup,describe,remark) values(b.tableName,"
                       "b.ptName,b.id,b.code,b.length,b.bGroup,b.describe,"
                       "b.remark) output $action,inserted.*,deleted.*;")
               .arg(d->q_icd.c_str()).arg(cdtICD));
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }
    // 更新规则数据
    for (int i = 0; i < lstRule.size(); ++i) {
        const QString &cmd = lstRule[i];
        if (!query.exec(cmd)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            d->q_db.rollback();
            return false;
        }
    }
    if (!d->q_db.commit()) {   // 批量提交
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief 删除表规则数据
* @param [in] table : 表名
* @param [in] rules : 待更新表规则数据（删除之后的待更新数据）
* @param [in] subTables : 待删除子表名
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::deleteTableRules(const std::string &table, 
                                const DMSpace::srMap&rules,
                                const std::vector<std::string> &subTable)
{
    if (!isOpen()) {
        return false;
    }
    //
    int maxIndex = -1;
    QString cdtRule;
    DMSpace::srMap::const_iterator it = rules.begin();
    // 规则数据
    for (; it != rules.end(); ++it) {
        const stTableRules& tableRule(it->second);
        maxIndex = max(maxIndex, tableRule.nSerial);
        cdtRule
                += QString("select '%1' number,'%2' serial,'%3' name,"
                           "'%4' code,'%5' type,'%6' defaultValue,'%7' range,'%8' offset,"
                           "'%9' unit,'%10' scale,'%11' regulation,'%12' remark union ")
                .arg(tableRule.nCode).arg(tableRule.nSerial)
                .arg(tableRule.sName.c_str()).arg(tableRule.sPrgCode.c_str())
                .arg(tableRule.uType).arg(tableRule.sDefault.c_str())
                .arg(tableRule.sRange.c_str())
                .arg(tableRule.dOffset, 0, 'g', 16)
                .arg(tableRule.sUnit.c_str())
                .arg(tableRule.dScale, 0, 'g', 16)
                .arg(tableRule.sRule.c_str()).arg(tableRule.sRemark.c_str());
    }
    cdtRule.chop(QString(" union ").size());
    QString sql;
    // 更新具体规则数据
    if (!cdtRule.isEmpty()) {
        sql = QString("merge %1 a using (%2) b on (a.serial=b.serial)"
                      "when matched then update set a.number=b.number,"
                      "a.name=b.name,a.code=b.code,a.type=b.type,"
                      "a.defaultValue=b.defaultValue,a.range=b.range,"
                      "a.offset=b.offset,a.unit=b.unit,a.scale=b.scale,"
                      "a.regulation=b.regulation,a.remark=b.remark when"
                      " not matched then insert(number,serial,name"
                      ",code,type,defaultValue,range,offset,unit,scale,"
                      "regulation,remark) values(b.number,b.serial,"
                      "b.name,b.code,b.type,b.defaultValue,"
                      "b.range,b.offset,b.unit,b.scale,b.regulation,"
                      "b.remark) output $action,inserted.*,deleted.*;")
                .arg(table.c_str()).arg(cdtRule);
        // 清空本次存储的最大序号之后的数据
        sql.append(QString("delete from %1 where serial > %2;")
                   .arg(table.c_str()).arg(maxIndex));
    }
    QString cdtSubTable;
    const int count = subTable.size();
    for (int i = 0; i < count; ++i) {
        const std::string &name = subTable[i];
        cdtSubTable.append(QString("drop table %1;").arg(name.c_str()));
        cdtSubTable.append(QString("delete from %1 where tableName='%2';")
                           .arg(d->q_icd.c_str()).arg(name.c_str()));
    }
    cdtSubTable.chop(QString(",").size());
    sql.append(cdtSubTable);

    QSqlQuery query(d->q_db);
    // 事务处理
    d->q_db.transaction();
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }
    if (!d->q_db.commit()) {   // 批量提交
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief 保存一条规则数据
* @param [in] table : 表名
* @param [in] rule : 规则数据
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::saveRule(const std::string &table, const stTableRules &rule)
{
    if (!isOpen()) {
        return false;
    }
    QString condition
            = QString("select '%1' number,'%2' serial,'%3' name,'%4' code,"
                      "'%5' type,'%6' defaultValue,'%7' range,'%8' offset,'%9' unit,"
                      "'%10' scale,'%11' regulation,'%12' remark")
            .arg(rule.nCode).arg(rule.nSerial)
            .arg(rule.sName.c_str()).arg(rule.sPrgCode.c_str())
            .arg(rule.uType).arg(rule.sDefault.c_str())
            .arg(rule.sRange.c_str()).arg(rule.dOffset, 0, 'g', 16)
            .arg(rule.sUnit.c_str()).arg(rule.dScale, 0, 'g', 16)
            .arg(rule.sRule.c_str()).arg(rule.sRemark.c_str());

    QString sql = QString("merge %1 a using (%2) b on (a.serial=b.serial)"
                          "when matched then update set a.number=b.number,"
                          "a.name=b.name,a.code=b.code,a.type=b.type,"
                          "a.defaultValue=b.defaultValue,a.range=b.range,"
                          "a.offset=b.offset,a.unit=b.unit,a.scale=b.scale,"
                          "a.regulation=b.regulation,a.remark=b.remark when not"
                          " matched then insert(number,serial,name,code,type,"
                          "defaultValue,range,offset,unit,scale,regulation,"
                          "remark) values(b.number,b.serial,b.name,b.code,"
                          "b.type,b.defaultValue,b.range,b.offset,b.unit,"
                          "b.scale,b.regulation,b.remark) output $action,"
                          "inserted.*,deleted.*;")
            .arg(table.c_str()).arg(condition);

    QSqlQuery query(d->q_db);
    // 事务处理
    d->q_db.transaction();
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }
    if (!d->q_db.commit()) {   // 批量提交
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief 保存一条规则数据(复合数据)
* @param [in] table : 表名
* @param [in] rule : 规则数据
* @param [in] subTables : 子表数据
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::saveRule(const std::string &table,
                        const stTableRules &rule,
                        const std::vector <stICDBase> &subTables)
{
    if (!isOpen()) {
        return false;
    }
    // 保存规则数据
    QString condition
            = QString("select '%1' number,'%2' serial,'%3' name,'%4' code,"
                      "'%5' type,'%6' defaultValue,'%7' range,'%8' offset,'%9' unit,"
                      "'%10' scale,'%11' regulation,'%12' remark")
            .arg(rule.nCode).arg(rule.nSerial)
            .arg(rule.sName.c_str()).arg(rule.sPrgCode.c_str())
            .arg(rule.uType).arg(rule.sDefault.c_str())
            .arg(rule.sRange.c_str()).arg(rule.dOffset, 0, 'g', 16)
            .arg(rule.sUnit.c_str()).arg(rule.dScale, 0, 'g', 16)
            .arg(rule.sRule.c_str()).arg(rule.sRemark.c_str());

    QString sql = QString("merge %1 a using (%2) b on (a.serial=b.serial)"
                          "when matched then update set a.number=b.number,a.name"
                          "=b.name,a.code=b.code,a.type=b.type,a.defaultValue"
                          "=b.defaultValue,a.range=b.range,a.offset=b.offset,"
                          "a.unit=b.unit,a.scale=b.scale,a.regulation=b.regulation,"
                          "a.remark=b.remark when not matched then insert(number,"
                          "serial,name,code,type,defaultValue,range,offset,unit,"
                          "scale,regulation,remark) values(b.number,b.serial,"
                          "b.name,b.code,b.type,b.defaultValue,b.range,b.offset,"
                          "b.unit,b.scale,b.regulation,b.remark) output $action,"
                          "inserted.*,deleted.*;")
            .arg(table.c_str()).arg(condition);

    // 保存子表数据
    QString cdtSubTable;
    const int count = subTables.size();
    for (int i = 0; i < count; ++i) {
        const stICDBase &base = subTables[i];
        cdtSubTable.append(QString("select '%1' tableName,'%2' ptName,"
                                   "'%3' id,'%4' code,'%5' length,'%6' bGroup,'%7' describe,"
                                   "'%8' remark").arg(base.sName.c_str())
                           .arg(base.sParentName.c_str()).arg(base.sID.c_str())
                           .arg(base.sCode.c_str()).arg(base.nLength)
                           .arg(base.sGroup.c_str()).arg(base.sDescribe.c_str())
                           .arg(base.sRemark.c_str())).append(" union ");
    }
    cdtSubTable.chop(QString(" union ").size());

    QSqlQuery query(d->q_db);
    if (!cdtSubTable.isEmpty()) {
        // 先查询表是否存在，如果不存在则先创建
        QString cdtCreate
                = QString("select a.tableName from (%1) a except select name"
                          " from sysobjects where xtype='u'").arg(cdtSubTable);
        // 无效数据
        if (!query.exec(cdtCreate)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            return false;
        }
        while (query.next()) {
            sql.append(createICDTableCmd(query.value(0).toString()
                                         .toStdString()).c_str());
        }
        // 构造sql
        sql += QString("merge %1 a using (%2) b on (a.tableName"
                       "=b.tableName) when matched then update set"
                       " a.ptName=b.ptName,a.id=b.id,a.code=b.code,"
                       "a.length=b.length,a.bGroup=b.bGroup,"
                       "a.describe=b.describe,a.remark=b.remark"
                       " when not matched then insert(tableName,"
                       "ptName,id,code,length,bGroup,describe,remark)"
                       " values(b.tableName,b.ptName,b.id,b.code,"
                       "b.length,b.bGroup,b.describe,b.remark)"
                       "output $action,inserted.*,deleted.*;")
                .arg(d->q_icd.c_str()).arg(cdtSubTable);
    }
    // 事务
    d->q_db.transaction();
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }
    if (!d->q_db.commit()) {   // 批量提交
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief 删除表规则数据
* @param [in] table : 表名
* @param [in] rule : 规则数据<规则序号, <子表集>>
* @return 执行结果，true：成功；false：失败
*/
bool DBAccess::deleteRules(const std::string &table,
                           const std::map < int,
                           std::vector < std::string >> &rule)
{
    if (!isOpen()) {
        return false;
    }
    if (rule.empty()) {
        return true;
    }
    QString cdtRule;
    QString cdtSubTable;
    int count = 0;
    std::map < int,
            std::vector < std::string >> ::const_iterator it = rule.begin();
    for (; it != rule.end(); ++it) {
        cdtRule.append(QString("'%1',").arg(it->first));
        count = it->second.size();
        for (int i = 0; i < count; ++i) {
            const std::string &name = it->second[i];
            cdtSubTable.append(QString("drop table %1;").arg(name.c_str()));
            cdtSubTable.append(QString("delete from %1 where tableName='%2';")
                               .arg(d->q_icd.c_str()).arg(name.c_str()));
        }
    }
    cdtRule.chop(QString(",").size());

    QString sql = QString("delete from %1 where serial in(%2);")
            .arg(table.c_str()).arg(cdtRule).append(cdtSubTable);
    QSqlQuery query(d->q_db);
    // 事务处理
    d->q_db.transaction();
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }
    if (!d->q_db.commit()) {   // 批量提交
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
 * @brief 读取数据库配置信息
 * @param [int] config : 包含数据库配置信息的xml文件全路径
 * @param [out] dbInfo : 数据配置信息
 * @return 执行结果， true：成功；false：失败
 */
bool DBAccess::loadDBInfo(const Json::Value &config, stDBInfo& dbInfo)
{
    if (config.isNull()) {
        d->q_error = QStringLiteral("Invalid config information!");
        d->q_errorFunc = __FUNCTION__;
        return false;
    }

    // driver
    if (!config.isMember("driver")) {
        d->q_error = QStringLiteral("can not find database config section<driver>！");
        d->q_errorFunc = __FUNCTION__;
        return false;
    }
    dbInfo.driver = config["driver"].asString();
    // serverName
    if (!config.isMember("serverName")) {
        d->q_error = QStringLiteral("can not find database config section<serverName>！");
        d->q_errorFunc = __FUNCTION__;
        return false;
    }
    dbInfo.server = config["serverName"].asString();
    // database
    if (!config.isMember("database")) {
        d->q_error = QStringLiteral("can not find database config section<database>！");
        d->q_errorFunc = __FUNCTION__;
        return false;
    }
    dbInfo.database = config["database"].asString();
    // userName
    if (!config.isMember("userName")) {
        d->q_error = QStringLiteral("can not find database config section<userName>！");
        d->q_errorFunc = __FUNCTION__;
        return false;
    }
    dbInfo.user = config["userName"].asString();
    // password
    if (!config.isMember("password")) {
        d->q_error = QStringLiteral("can not find database config section<password>！");
        d->q_errorFunc = __FUNCTION__;
        return false;
    }
    dbInfo.pwd = config["password"].asString();
    // tableLimitation
    if (config.isMember("tableLimitation")) {
        d->q_tableLimitation = config["tableLimitation"].asInt();
    }

    return true;
}

/**
 * @brief 创建ICD规则表语句
 * @param [in] table : 表名
 * @return 创建表命令语句
 */
std::string DBAccess::createICDTableCmd(const std::string &table)
{
    // 默认regulation字段长度是2000，可根据配置文件子表数据限制进行增长
    QString name = QString::fromStdString(table);
    QString cmd = QString("create table %1 (\r\n"
                          "number int not null,"
                          "serial int not null,"
                          "name varchar(40) not null,"
                          "code varchar(20) null,"
                          "type smallint null,"
                          "defaultValue varchar(30) null,"
                          "range varchar(30) null,"
                          "offset double precision null,"
                          "unit varchar(20) null,"
                          "scale double precision null,"
                          "regulation varchar(%2) null,"
                          "remark varchar(200) null,"
                          "constraint PK_%3 primary key (serial)\r\n"
                          ");").arg(name)
            .arg(max(2000, d->q_tableLimitation * 42))
            .arg(name.toUpper());

#if 0
    QString alter = QString("alter table %1 alter column "
                            "regulation varchar(%2) null;")
            .arg(name)
            .arg(max(2000, d->q_tableLimitation * 42));
#endif

    return cmd.toStdString();
}

/**
* @brief 查询指定表的所有子表
* @param [in] table : 表名
* @return 子表集
*/
std::vector<std::string> DBAccess::querySubTables(const std::string& table) const
{
    std::vector<std::string> result;
    if (!isOpen()) {
        return result;
    }
    QString sql;
    QString name;
    QStringList tableList = QStringList() <<table.c_str();
    QStringListIterator it = tableList;
    QSqlQuery query(d->q_db);
    do {
        while (it.hasNext()) {
            name.append("'").append(it.next()).append("'").append(",");
        }
        name.chop(QString(",").size());
        tableList.clear();  // 清空数据
        sql = QString("select tableName from %1 where ptName in(%2)")
                .arg(d->q_icd.c_str()).arg(name);
        if (!query.exec(sql)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc <<" " << d->q_error;
            return result;
        }
        while (query.next()) {
            name = query.value("tableName").toString();
            result.push_back(name.toStdString());
            tableList.append(name);
        }
        // 清空数据
        name.clear();
        it = tableList;
    } while (!tableList.empty());

    return result;
}
