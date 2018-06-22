#include "dbaccess.h"
#include <QtSql>
#include "icdcore/3rdpart/jsoncpp/json_tool.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

// ��С�ӱ�����
#ifndef MINCOUNT
#define MINCOUNT    10
#endif

// ����ӱ�����
#ifndef MAXCOUNT
#define MAXCOUNT    1000
#endif

// Ĭ���ӱ�����
#ifndef DEFAULTCOUNT
#define DEFAULTCOUNT    40
#endif

#ifndef TABLELENGTH
// ���ű������� ICDTable_32λUID
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
        q_dic.push_back(DBAccess::tableName(DataBaseDefine::dbDataType));       // ��������
        q_dic.push_back(DBAccess::tableName(DataBaseDefine::dbPowerType));      // Ȩ������
        q_dic.push_back(DBAccess::tableName(DataBaseDefine::dbCounterType));    // ֡��������
        q_dic.push_back(DBAccess::tableName(DataBaseDefine::dbCheckType));      // У������
        q_dic.push_back(DBAccess::tableName(DataBaseDefine::dbArrayType));      // ��������
    }

public:
    QSqlDatabase        q_db;
    DBAccess::stDBInfo  q_dbInfo;
    QString             q_error;
    QString             q_errorFunc;
    int                 q_tableLimitation;  // �ӱ�����
    std::vector<std::string> q_dic; // ���ݿ��ֵ�

    const std::string q_icd;        // ICD����
    const std::string q_userPower;  // �û�Ȩ��
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
* @brief ��ȡ���ݿ����
* @param [in] dbf : ��ö��
* @return �����ַ�����Ϊ��ʧ��
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
    case DataBaseDefine::dbArrayType:
        result = "Dic_ArrayType";
        break;
    default:
        break;
    }

    return result;
}

/**
 * @brief ��ʼ�����ݿ⣬�ڵ����������ݲ����ӿ�ǰ��Ҫ����һ��
 * @param [in] file : �������ݿ�������Ϣ��xml�ļ�ȫ·��
 * @return ִ�н����true���ɹ���false��ʧ��
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
* @brief ��ѯ���ݿ�����״̬
* @return ���ݿ�����״̬��true�������ӣ�false����δ����
*/
bool DBAccess::isOpen() const
{
    if (!d->q_db.isValid()) {
        d->q_error = QString("database invalid!");
        d->q_errorFunc = __FUNCTION__;
        return false;
    }
    QSqlQuery query(d->q_db);
    // ��֤���ݿ��Ƿ�����
    if (!query.exec("select 1")) {
        d->q_error = QString("database disconnected!");
        d->q_errorFunc = __FUNCTION__;
        return false;
    } else {
        return d->q_db.isOpen();
    }
}

/**
* @brief �������ݿ�
* @return ���ݿ�����״̬��true�����ӳɹ���false������ʧ��
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
* @brief �Ͽ��ݿ�����
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
* @brief ��ȡ���һ�εĴ�����Ϣ
* @return ���ݴ�����Ϣ
*/
std::string DBAccess::lastError() const
{
    return d->q_error.toStdString();
}

/**
* @brief ��ȡ��������ĺ���
* @return ���ݴ�����Ϣ
*/
std::string DBAccess::errorFunction() const
{
    return d->q_errorFunc.toStdString();
}

/**
* @brief ��ȡ�ӱ�����
* @return �ӱ�����
*/
int DBAccess::tableLimitation() const
{
    return d->q_tableLimitation;
}

/**
* @brief �����ӱ�����
* @param [in] top : ����ֵ
*/
void DBAccess::setTableLimitation(int top)
{
    if (top > MINCOUNT && top < MAXCOUNT) {
        d->q_tableLimitation = top;
    }
}

/**
 * @brief ��ȡ���ݿ��ֵ������
 * @param [out] dic : �����ֵ�����<�ֵ��ʶ, <�ֵ����ݼ�>>
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DBAccess::readDictionary(std::unordered_map<std::string, std::vector<stDictionary> > &dic)
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
        // ��Ч����
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
* @brief ��ȡ�ֵ������
* @param [in] dics : �ֵ��
* @param [out] result : �ֵ�����<�ֵ��ʶ, <�ֵ����ݼ�>>
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DBAccess::readDictionary(const std::vector<std::string> &dics,
                              std::unordered_map<std::string, std::vector<stDictionary> >& result)
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
        // ��Ч����
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
* @brief ��ȡ�����ֵ�����ݣ����ݿ��޷�����ʱʹ�ã�ע������������ͬ����
* @param [out] dic : �����ֵ�����<�ֵ��ʶ, <�ֵ����ݼ�>>
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DBAccess::readLocalDictionary(std::unordered_map<std::string, std::vector<stDictionary> >& dic)
{
    stDictionary stDic;
    std::vector<stDictionary> table;
    // У������
    {
        stDic.nCode = GlobalDefine::ctInvalid;
        stDic.sDec = QStringLiteral("��У��").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ct8And;
        stDic.sDec = QStringLiteral("8λ��У��").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ct16And;
        stDic.sDec = QStringLiteral("16λ��У��").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ctCRC8;
        stDic.sDec = QStringLiteral("CRC8У��").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ctCRC16;
        stDic.sDec = QStringLiteral("CRC16У��").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ctXor8;
        stDic.sDec = QStringLiteral("8λ����У��").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ctXor16;
        stDic.sDec = QStringLiteral("16λ����У��").toStdString();
        table.push_back(stDic);

        dic[tableName(DataBaseDefine::dbCheckType)] = table;
        table.clear();
    }

    // ֡��������
    {
        stDic.nCode = GlobalDefine::counterU8;
        stDic.sDec = QStringLiteral("8λ����").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::counterU16;
        stDic.sDec = QStringLiteral("16λ����").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::counterU32;
        stDic.sDec = QStringLiteral("32λ����").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::counterU64;
        stDic.sDec = QStringLiteral("64λ����").toStdString();
        table.push_back(stDic);
        dic[tableName(DataBaseDefine::dbCounterType)] = table;
        table.clear();
    }
    // ��������
    {
        stDic.nCode = GlobalDefine::dtHead;
        stDic.sDec = QStringLiteral("��ͷ").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtCounter;
        stDic.sDec = QStringLiteral("֡����").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtCheck;
        stDic.sDec = QStringLiteral("У��").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtFrameCode;
        stDic.sDec = QStringLiteral("֡ʶ����").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtNumeric;
        stDic.sDec = QStringLiteral("��ֵ").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtArray;
        stDic.sDec = QStringLiteral("����").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtBitMap;
        stDic.sDec = QStringLiteral("����ӳ��").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtBitValue;
        stDic.sDec = QStringLiteral("����ֵ").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtComplex;
        stDic.sDec = QStringLiteral("��������").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtFrame;
        stDic.sDec = QStringLiteral("֡����").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::dtBuffer;
        stDic.sDec = QStringLiteral("����Ԥ����").toStdString();
        table.push_back(stDic);
        dic[tableName(DataBaseDefine::dbDataType)] = table;
        table.clear();
    }
    // ��ֵ����
    {
        stDic.nCode = GlobalDefine::NumericI8;
        stDic.sDec = QStringLiteral("8λ�з���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::NumericU8;
        stDic.sDec = QStringLiteral("8λ�޷���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::NumericI16;
        stDic.sDec = QStringLiteral("16λ�з���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::NumericU16;
        stDic.sDec = QStringLiteral("16λ�޷���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::NumericI32;
        stDic.sDec = QStringLiteral("32λ�з���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::NumericU32;
        stDic.sDec = QStringLiteral("32λ�޷���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::NumericI64;
        stDic.sDec = QStringLiteral("64λ�з���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::NumericU64;
        stDic.sDec = QStringLiteral("64λ�޷���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::NumericF32;
        stDic.sDec = QStringLiteral("32λ������").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::NumericF64;
        stDic.sDec = QStringLiteral("64λ������").toStdString();
        table.push_back(stDic);

        dic[tableName(DataBaseDefine::dbNumericType)] = table;
        table.clear();
    }
    // ��������
    {
        stDic.nCode = GlobalDefine::ArrayI8;
        stDic.sDec = QStringLiteral("8λ�з���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ArrayU8;
        stDic.sDec = QStringLiteral("8λ�޷���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ArrayI16;
        stDic.sDec = QStringLiteral("16λ�з���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ArrayU16;
        stDic.sDec = QStringLiteral("16λ�޷���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ArrayI32;
        stDic.sDec = QStringLiteral("32λ�з���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ArrayU32;
        stDic.sDec = QStringLiteral("32λ�޷���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ArrayI64;
        stDic.sDec = QStringLiteral("64λ�з���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ArrayU64;
        stDic.sDec = QStringLiteral("64λ�޷���").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ArrayF32;
        stDic.sDec = QStringLiteral("32λ������").toStdString();
        table.push_back(stDic);
        stDic.nCode = GlobalDefine::ArrayF64;
        stDic.sDec = QStringLiteral("64λ������").toStdString();
        table.push_back(stDic);

        dic[tableName(DataBaseDefine::dbArrayType)] = table;
        table.clear();
    }

    // Ȩ������
    {
    }

    return true;
}

/**
* @brief ��ȡ���ͻ�����Ϣ
* @param [out] plane : ������Ϣ��
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DBAccess::readPlane(std::vector<stPlane> &plane)
{
    if (!isOpen()) {
        return false;
    }
    QString sql = QString("select * from %1 order by describe")
            .arg(tableName(DataBaseDefine::dbPlane).c_str());
    QSqlQuery query(d->q_db);
    // ��Ч����
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
* @brief ��������������ݣ�������
* @param [in] infrastructure : ϵͳ�ṹ����<������Ϣ, <��ϵͳ��Ϣ��>>
* @param [in] tableRules : ���������<ICD����Ϣ, <�������Ϣ��>>
* @return ִ�н����true���ɹ���false��ʧ��
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
                                  "'%7','%8','%9','%10','%11','%12','%13'),")
                          .arg(tableRule.nCode).arg(tableRule.nSerial)
                          .arg(tableRule.sName.c_str())
                          .arg(tableRule.sPrgCode.c_str())
                          .arg(tableRule.uType)
                          .arg(tableRule.sDefault.c_str())
                          .arg(tableRule.sRange.c_str())
                          .arg(tableRule.dOffset, 0, 'g', 16)
                          .arg(tableRule.sUnit.c_str())
                          .arg(tableRule.dScale, 0, 'g', 16)
                          .arg(tableRule.sRule.c_str())
                          .arg(tableRule.sRemark.c_str())
                          .arg(tableRule.subType));
        }
        if (!rulCmd.isEmpty()) {
            rulCmd.chop(QString(",").size());
            rulCmd = QString("delete from %1;insert into %1 (number,serial"
                             ",name,code,type,defaultValue,range,offset,"
                             "unit,scale,regulation,remark,subType) values %2;")
                    .arg(base.sName.c_str()).arg(rulCmd);
            cdtRule.append(rulCmd);
        }
        if (cdtRule.size() > 1000) {    // ���Ƶ�����䳤��Ϊ1000
            lstRule << cdtRule;
            cdtRule.clear();
        }
    }
    if (!cdtRule.isEmpty()) {
        lstRule << cdtRule;
    }
    cdtICD.chop(QString(" union ").size());

    QSqlQuery query(d->q_db);
    // ɾ��ICD��
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

    // ������
    d->q_db.transaction();
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }

    if (!cdtICD.isEmpty()) {
        // �Ȳ�ѯ���Ƿ���ڣ�������������ȴ���
        QString cdtCreate = QString("select a.tableName from (%1) a except"
                                    " select name from sysobjects where"
                                    " xtype='u'").arg(cdtICD);
        // ��Ч����
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
    // ɾ����
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
    if (!d->q_db.commit()) {   // �����ύ
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief �������
* @param [in] plane : ������Ϣ
* @return ִ�н����true���ɹ���false��ʧ��
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
* @brief �������
* @param [in] plane_system : ����-ϵͳ��Ϣ
* @param [in] rules : ���������
* @param [in] dataLevel : ���������ݲ㼶��GlobalDefine::NodeType��
* @param [in] dbSource : ���ݿ�����Դ��ʶ
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DBAccess::savePlane(const DMSpace::_vectorPS &plane_system,
                         const DMSpace::_vectorIcdTR &rules,
                         int dataLevel, bool dbSource)
{
    if (!isOpen()) {
        return false;
    }
    if (plane_system.empty()) {
        // ������ݿ�
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
        // ����sql
        const stPlane &plane = itPS->first;
        cdtPlane.append(QString("select '%1' code,'%2' describe,'%3' sign,"
                                "'%4' remark union ").arg(plane.nCode).arg(plane.sName.c_str())
                        .arg(plane.sSign.c_str()).arg(plane.sDescribe.c_str()));
        // ϵͳ��Ϣsql
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
        // ɾ��ϵͳsql
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
        if (cdtDeleteSystem.size() > 1000) {    // ���Ƶ�����䳤��Ϊ1000
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
    // �����
    QString cdtICD;
    QString cdtRule;
    QStringList lstRule;
    DMSpace::_vectorIcdTR_Cit itR = rules.begin();
    for (; itR != rules.end(); ++itR) {
        const stICDBase &icdBase = itR->first;
        const std::vector<stTableRules> &tableRules = itR->second;
        // ������Ϣ��
        cdtICD.append(QString("select '%1' tableName,'%2' ptName,'%3' id,"
                              "'%4' code,'%5' length,'%6' bGroup,'%7' describe,"
                              "'%8' remark union ").arg(icdBase.sName.c_str())
                      .arg(icdBase.sParentName.c_str())
                      .arg(icdBase.sID.c_str()).arg(icdBase.sCode.c_str())
                      .arg(icdBase.nLength).arg(icdBase.sGroup.c_str())
                      .arg(icdBase.sDescribe.c_str()).arg(icdBase.sRemark.c_str()));
        // ��������
        count = tableRules.size();
        QString rulCmd;
        for (int i = 0; i < count; ++i) {
            const stTableRules &tableRule = tableRules[i];
            rulCmd.append(QString("('%1','%2','%3','%4','%5','%6','%7',"
                                  "'%8','%9','%10','%11','%12','%13'),").arg(tableRule.nCode)
                          .arg(tableRule.nSerial).arg(tableRule.sName.c_str())
                          .arg(tableRule.sPrgCode.c_str())
                          .arg(tableRule.uType)
                          .arg(tableRule.sDefault.c_str())
                          .arg(tableRule.sRange.c_str())
                          .arg(tableRule.dOffset, 0, 'g', 16)
                          .arg(tableRule.sUnit.c_str())
                          .arg(tableRule.dScale, 0, 'g', 16)
                          .arg(tableRule.sRule.c_str())
                          .arg(tableRule.sRemark.c_str())
                          .arg(tableRule.subType));
        }
        if (!rulCmd.isEmpty()) {
            rulCmd.chop(QString(",").size());
            rulCmd = QString("insert into %1 (number,serial,name,"
                             "code,type,defaultValue,range,offset,unit,"
                             "scale,regulation,remark,subType) values %2;")
                    .arg(icdBase.sName.c_str()).arg(rulCmd);
        }
        rulCmd.prepend(QString("delete from %1;")
                       .arg(icdBase.sName.c_str()));
        cdtRule.append(rulCmd);
        if (cdtRule.size() > 1000) {    // ���Ƶ�����䳤��Ϊ1000
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
        // �Ȳ�ѯ���Ƿ���ڣ�������������ȴ���
        sql = QString("select a.tableName from (%1) a except select name"
                      " from sysobjects where xtype='u'").arg(cdtICD);
        // ��Ч����
        if (!query.exec(sql)) {
            d->q_error = query.lastError().text();
            d->q_errorFunc = __FUNCTION__;
            qDebug() << d->q_errorFunc << " " << d->q_error;
            return false;
        }
        // ����ICD��
        while (query.next()) {
            tableId += query.value(0).toString() + " ";
            cdtCreate += createICDTableCmd(query.value(0).toString()
                                           .toStdString()).c_str();
            if (cdtCreate.size() > 1000) {    // ���Ƶ�����䳤��Ϊ1000
                lstNewICD << cdtCreate;
                cdtCreate.clear();
            }
        }
        if (!cdtCreate.isEmpty()) {
            lstNewICD << cdtCreate;
        }
    }
    sql.clear();
    if (dbSource) { // ����ԴΪ���ݿ�
        if (GlobalDefine::ntTable > dataLevel) {
            if (GlobalDefine::ntSystem == dataLevel) {
                lstDeleteSystem.clear();    // ������Ϊ����ʱ����ɾ����������
                cdtDeleteTable.clear();
            } else if (GlobalDefine::ntPlane > dataLevel) {    // ���л���
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
            // ��������ICD��֮��
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
            lstDeleteSystem.clear();    // ������Ϊ��ʱ����ɾ����������
            cdtDeleteTable.clear();
            // ������ΪICD��
            // ��һ������Ϊ�������ѯ���ݿ��ж�����µ������ӱ�
            // ����ڴ�����������û�У���ɾ���ñ�
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
    } else {    // ����Դ��Ϊ���ݿ⣬ֻɾ���Ѵ��ڱ���ӱ�
        lstDeleteSystem.clear();    // ��ɾ����������
        cdtDeleteTable.clear();
        if (!rules.empty()) {
            //if (GlobalDefine::ntTable > dataLevel) {
            // ������һ��
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
    // ɾ��ICD��
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
            if (cdtDeleteICD.size() > 1000) {    // ���Ƶ�����䳤��Ϊ1000
                lstDeleteICD << cdtDeleteICD;
                cdtDeleteICD.clear();
            }
        }
        if (!cdtDeleteICD.isEmpty()) {
            lstDeleteICD << cdtDeleteICD;
        }
    }
    // ����
    d->q_db.transaction();  // sql���̫��������ִ��
    // ɾ��ϵͳ
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
    sql = cdtDeletePlane; // ɾ������
    // ���������Ϣ
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
    // ����ϵͳ��Ϣ
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
    // ������
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
    // ɾ����
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
    // ����ICD��
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
    // ���¹�������
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
    if (!d->q_db.commit()) {   // �����ύ
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief ������ݿ�
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DBAccess::clearDataBase()
{
    if (!isOpen()) {
        return false;
    }
    QString sql = QString("use %1;").arg(d->q_dbInfo.database.c_str());
    // ɾ��ϵͳ
    sql.append(QString("delete from %1;")
               .arg(tableName(DataBaseDefine::dbSystem).c_str()));
    // ɾ������
    sql.append(QString("delete from %1;")
               .arg(tableName(DataBaseDefine::dbPlane).c_str()));
    // ɾ��ICD������Ϣ��
    sql.append(QString("delete from %1;")
               .arg(tableName(DataBaseDefine::dbICDBase).c_str()));
    // ɾ��������Ϣ
    QString cdt = QString("use %1;select 'drop table '+name+';'"
                          " from sysobjects where xtype='u' and"
                          " name like 'ICDTable_%'")
            .arg(d->q_dbInfo.database.c_str());
    QSqlQuery query(d->q_db);
    if (!query.exec(cdt)) {
        // ��Ч����
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    QStringList lstCmd;
    while (query.next()) {
        sql.append(query.value(0).toString());
        if (sql.size() > 1000) {    // ���Ƶ�����䳤��Ϊ1000
            lstCmd << sql;
            sql.clear();
        }
    }
    if (!sql.isEmpty()) {
        lstCmd << sql;
    }
    // ����
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
    if (!d->q_db.commit()) {   // �����ύ
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief ɾ������
* @param [in] planes : ���ͱ�ʶ��
* @return ִ�н����true���ɹ���false��ʧ��
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

    // ɾ����ϵͳ
    QString sql = QString("delete from %1 where typeCode in%2;")
            .arg(tableName(DataBaseDefine::dbSystem).c_str())
            .arg(cdtEqual);

    // ɾ��������Ϣ��
    sql += QString("delete from %1 where %2;")
            .arg(d->q_icd.c_str())
            .arg(cdtLike);

    // ɾ�������
    QString subSql = QString("select 'drop table '+tableName+';'"
                             " from %1 where %2")
            .arg(d->q_icd.c_str())
            .arg(cdtLike);
    QSqlQuery query(d->q_db);
    // ��Ч����
    if (!query.exec(subSql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    while (query.next()) {
        sql.append(query.value(0).toString());
    }
    // ɾ������
    sql = QString("delete from %1 where code in%2;")
            .arg(tableName(DataBaseDefine::dbPlane).c_str())
            .arg(cdtEqual);

    // ִ��ɾ�����������������
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }

    return true;
}

/**
 * @brief ��ȡϵͳ��Ϣ�����ͺͷ�ϵͳ��
 * @param [out] system : ����ϵͳ��Ϣ<������, <��ϵͳ��Ϣ��>>
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DBAccess::readSystem(std::unordered_map<int, std::vector<stSystem> > &system)
{
    if (!isOpen()) {
        return false;
    }
    QString sql = QString("select * from %1 order by typeCode,describe")
            .arg(tableName(DataBaseDefine::dbSystem).c_str());
    QSqlQuery query(d->q_db);
    // ��Ч����
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
* @brief ��ȡϵͳ��Ϣ
* @param [in] plane : ������
* @param [in] system : ��ϵ��Ϣ��
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DBAccess::readSystem(int plane, std::vector<stSystem> &systems)
{
    if (!isOpen()) {
        return false;
    }
    QString sql = QString("select * from %1 where typeCode='%2' order by describe")
            .arg(tableName(DataBaseDefine::dbSystem).c_str()).arg(plane);
    QSqlQuery query(d->q_db);
    // ��Ч����
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
 * @brief ��ȡICD�������Ϣ
 * @param [in] top : ֻ��ȡ������ʶ
 * @param [out] icdBase : ICD������Ϣ��<������, <�ֱ���Ϣ��>>
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DBAccess::readICDBase(bool top, std::unordered_map<std::string, std::vector<stICDBase> > &icdBase)
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
    // ��Ч����
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
* @brief ��ȡICD�������Ϣ
* @param [in] name : ICD����
* @param [out] icdBase : ICD������Ϣ
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DBAccess::readICDBase(const std::string &name, stICDBase &icdBase)
{
    if (!isOpen()) {
        return false;
    }
    QString sql = QString("select * from %1 where tableName = '%2'")
            .arg(d->q_icd.c_str()).arg(name.c_str());
    QSqlQuery query(d->q_db);
    // ��Ч����
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

        break;  // ֻ��һ������
    }

    return true;
}

/**
* @brief ��ȡICD�������Ϣ
* @param [in] name : ICD������
* @param [out] icdBase : ICD������Ϣ��
* @return ִ�н����true���ɹ���false��ʧ��
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
        // ��Ч����
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
* @brief ��ȡ�ض�������ICD�������Ϣ
* @param [in] condition : ���������������Ļ�������߻������ϵͳ�����ϡ�xx/xx����
* @param [in] ICDBase : <�����ʶ, <���ʶ, ICD��>>
* @param [in] topOnly : ������ʶ
* @return ִ�н����true���ɹ���false��ʧ��
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
    // ��Ч����
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    int index = 0;
    std::string preName("top"); // �����Ϊ�գ������⸺ֵ
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
            icdBases.clear();  // ���
            preName = base.sParentName;
        }
        icdBases.push_back(base);
        index = 0;
    }
    // ���һ�ű�����
    ICDBase.push_back(std::make_pair(preName, icdBases));

    return true;
}

/**
 * @brief ��ȡ�û�Ȩ����Ϣ
 * @param [in] user : �û���
 * @param [out] dic : ���û������б�Ȩ����Ϣ
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DBAccess::readUserTables(const std::string &/*user*/, std::vector<stTablePrivilege>)
{
    // TODO
    return true;
}

/**
 * @brief ��ȡ��ṹ����
 * @param [in] table : ����
 * @param [out] rules : ��ṹ����<������, ��������>
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DBAccess::readTableRules(const std::string &table, DMSpace::srMap &rules)
{
    if (!isOpen()) {
        return false;
    }
    QString sql = QString("select * from %1 order by serial")
            .arg(table.c_str());
    QSqlQuery query(d->q_db);
    // ��Ч����
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
        rule.subType = (unsigned short)query.value(index++).toInt();

        rules[rule.nSerial] = rule;
        index = 0;
    }

    return true;
}

/**
 * @brief ��ȡ��ṹ����
 * @param [in] tables : ������
 * @param [out] rules : ��ṹ���ݼ�<����, <�������ݼ�>>
 * @return ִ�н����true���ɹ���false��ʧ��
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
    // ��Ч����
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
        rule.subType = (unsigned short)query.value(index++).toInt();

        if (preName != name) {
            if (!preName.empty()) {
                rules[preName] = tableRules;
                tableRules.clear();  // ���
            }
            preName = name;
        }
        tableRules.push_back(rule);
        index = 0;
    }
    // ���һ�ű�����
    rules[preName] = tableRules;

    return true;
}

/**
 * @brief ������������
 * @param [in] icdBase : �������Ϣ
 * @param [in] rules : ���������
 * @param [in] subTables : �ӱ������Ϣ
 * @return ִ�н����true���ɹ���false��ʧ��
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
    // ��������
    for (; it != rules.end(); ++it) {
        const stTableRules& tableRule(it->second);
        cdtRule.append(QString("('%1','%2','%3','%4','%5','%6','%7',"
                               "'%8','%9','%10','%11','%12','%13'),").arg(tableRule.nCode)
                       .arg(tableRule.nSerial).arg(tableRule.sName.c_str())
                       .arg(tableRule.sPrgCode.c_str())
                       .arg(tableRule.uType)
                       .arg(tableRule.sDefault.c_str())
                       .arg(tableRule.sRange.c_str())
                       .arg(tableRule.dOffset, 0, 'g', 16)
                       .arg(tableRule.sUnit.c_str())
                       .arg(tableRule.dScale, 0, 'g', 16)
                       .arg(tableRule.sRule.c_str())
                       .arg(tableRule.sRemark.c_str())
                       .arg(tableRule.subType));
    }
    if (!cdtRule.isEmpty()) {
        cdtRule.chop(QString(",").size());
        cdtRule = QString("insert into %1 (number,serial,name,code"
                          ",type,defaultValue,range,offset,unit,scale,"
                          "regulation,remark,subType) values %2;")
                .arg(icdBase.sName.c_str()).arg(cdtRule);
    }
    cdtRule.prepend(QString("delete from %1;").arg(icdBase.sName.c_str()));

    QString sql;
    QSqlQuery query(d->q_db);
    // �Ȳ�ѯ���Ƿ���ڣ�������������ȴ���
    sql = QString("select name from sysobjects where xtype='u'"
                  " and name='%1'").arg(icdBase.sName.c_str());
    // ��Ч����
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    // ��������
    if (0 == query.numRowsAffected()) { // ������
        sql = QString::fromStdString(createICDTableCmd(icdBase.sName));
    }
    QString cdtICD;
    // ������Ϣ�����ݸ���
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
                   "a.describe=b.describe,a.remark=b.remark,a.subType=b.subType"
                   " when not matched then insert(tableName,"
                   "ptName,id,code,length,bGroup,describe,remark)"
                   " values(b.tableName,b.ptName,b.id,b.code,"
                   "b.length,b.bGroup,b.describe,b.remark,b.subType)"
                   "output $action,inserted.*,deleted.*;")
            .arg(d->q_icd.c_str()).arg(cdtICD);

    // �����������
    sql.append(cdtRule);
    // �ӱ�����
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
        // �Ȳ�ѯ���Ƿ���ڣ�������������ȴ���
        QString cdtCreate
                = QString("select a.tableName from (%1) a except select name"
                          " from sysobjects where xtype='u'").arg(cdtSubTable);
        // ��Ч����
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
        // ����sql
        sql += QString("merge %1 a using (%2) b on (a.tableName"
                       "=b.tableName) when matched then update set"
                       " a.ptName=b.ptName,a.id=b.id,a.code=b.code,"
                       "a.length=b.length,a.bGroup=b.bGroup,"
                       "a.describe=b.describe,a.remark=b.remark,a.subType=b.subType"
                       " when not matched then insert(tableName,"
                       "ptName,id,code,length,bGroup,describe,remark)"
                       " values(b.tableName,b.ptName,b.id,b.code,"
                       "b.length,b.bGroup,b.describe,b.remark,b.subType)"
                       "output $action,inserted.*,deleted.*;")
                .arg(d->q_icd.c_str()).arg(cdtSubTable);
    }
    // ������
    d->q_db.transaction();
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }
    if (!d->q_db.commit()) {   // �����ύ
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief ����������������
* @param [in] rules : ���������
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DBAccess::saveTableRules(const std::vector<std::pair<stICDBase, DMSpace::srMap>> &rules)
{
    if (!isOpen()) {
        return false;
    }
    if (rules.empty()) {
        return true;
    }
    // ��һ������Ϊ�������ѯ���ݿ��ж�����µ������ӱ�����ڴ�����������
    // û�У���ɾ���ñ�
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
        // ������Ϣ�����ݸ���
        cdtICD.append(QString("select '%1' tableName,'%2' ptName,'%3' id,"
                              "'%4' code,'%5' length,'%6' bGroup,'%7' describe,"
                              "'%8' remark union ").arg(icdBase.sName.c_str())
                      .arg(icdBase.sParentName.c_str())
                      .arg(icdBase.sID.c_str()).arg(icdBase.sCode.c_str())
                      .arg(icdBase.nLength).arg(icdBase.sGroup.c_str())
                      .arg(icdBase.sDescribe.c_str()).arg(icdBase.sRemark.c_str()));
        QString rulCmd;
        DMSpace::srMap::const_iterator it = mapRules.begin();
        // ��������
        for (; it != mapRules.end(); ++it) {
            const stTableRules& tableRule(it->second);
            rulCmd.append(QString("('%1','%2','%3','%4','%5','%6','%7',"
                                  "'%8','%9','%10','%11','%12','%13'),")
                          .arg(tableRule.nCode).arg(tableRule.nSerial)
                          .arg(tableRule.sName.c_str())
                          .arg(tableRule.sPrgCode.c_str())
                          .arg(tableRule.uType)
                          .arg(tableRule.sDefault.c_str())
                          .arg(tableRule.sRange.c_str())
                          .arg(tableRule.dOffset, 0, 'g', 16)
                          .arg(tableRule.sUnit.c_str())
                          .arg(tableRule.dScale, 0, 'g', 16)
                          .arg(tableRule.sRule.c_str())
                          .arg(tableRule.sRemark.c_str())
                          .arg(tableRule.subType));
        }
        if (!rulCmd.isEmpty()) {
            rulCmd.chop(QString(",").size());
            rulCmd = QString("insert into %1 (number,serial,name,"
                             "code,type,defaultValue,range,offset,unit,"
                             "scale,regulation,remark,subType) values %2;")
                    .arg(icdBase.sName.c_str()).arg(rulCmd);
            cdtRule.append(rulCmd);
        }
        cdtRule.prepend(QString("delete from %1;")
                        .arg(icdBase.sName.c_str()));

        if (cdtRule.size() > 1000) {    // ���Ƶ�����䳤��Ϊ1000
            lstRule << cdtRule;
            cdtRule.clear();
        }
    }
    if (!cdtRule.isEmpty()) {
        lstRule << cdtRule;
    }
    cdtICD.chop(QString(" union ").size());

    // �Ȳ�ѯ���Ƿ���ڣ�������������ȴ���
    QString sql = QString("select a.tableName from (%1) a except select name"
                          " from sysobjects where xtype='u'").arg(cdtICD);
    // ��Ч����
    QSqlQuery query(d->q_db);
    QString cdtCreate;
    QString tableId;
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        return false;
    }
    // ����ICD��
    while (query.next()) {
        tableId += query.value(0).toString() + " ";
        cdtCreate += createICDTableCmd(query.value(0).toString()
                                       .toStdString()).c_str();
    }
    // ɾ��ICD��
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
    // ����
    d->q_db.transaction();  // sql���̫��������ִ��
    sql = cdtCreate;
    sql.append(cdtDeleteICD);
    // ����ICD��
    sql.append(QString("merge %1 a using (%2) b on (a.tableName=b.tableName)"
                       " when matched then update set a.ptName=b.ptName,a.id"
                       "=b.id,a.code=b.code,a.length=b.length,a.bGroup"
                       "=b.bGroup,a.describe=b.describe,a.remark=b.remark,a.subType=b.subType when"
                       " not matched then insert(tableName,ptName,id,code,"
                       "length,bGroup,describe,remark) values(b.tableName,"
                       "b.ptName,b.id,b.code,b.length,b.bGroup,b.describe,"
                       "b.remarkb.subType) output $action,inserted.*,deleted.*;")
               .arg(d->q_icd.c_str()).arg(cdtICD));
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }
    // ���¹�������
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
    if (!d->q_db.commit()) {   // �����ύ
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief ɾ�����������
* @param [in] table : ����
* @param [in] rules : �����±�������ݣ�ɾ��֮��Ĵ��������ݣ�
* @param [in] subTables : ��ɾ���ӱ���
* @return ִ�н����true���ɹ���false��ʧ��
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
    // ��������
    for (; it != rules.end(); ++it) {
        const stTableRules& tableRule(it->second);
        maxIndex = max(maxIndex, tableRule.nSerial);
        cdtRule
                += QString("select '%1' number,'%2' serial,'%3' name,"
                           "'%4' code,'%5' type,'%6' defaultValue,'%7' range,'%8' offset,"
                           "'%9' unit,'%10' scale,'%11' regulation,'%12' remark,'%13' subType union ")
                .arg(tableRule.nCode).arg(tableRule.nSerial)
                .arg(tableRule.sName.c_str()).arg(tableRule.sPrgCode.c_str())
                .arg(tableRule.uType).arg(tableRule.sDefault.c_str())
                .arg(tableRule.sRange.c_str())
                .arg(tableRule.dOffset, 0, 'g', 16)
                .arg(tableRule.sUnit.c_str())
                .arg(tableRule.dScale, 0, 'g', 16)
                .arg(tableRule.sRule.c_str()).arg(tableRule.sRemark.c_str())
                .arg(tableRule.subType);
    }
    cdtRule.chop(QString(" union ").size());
    QString sql;
    // ���¾����������
    if (!cdtRule.isEmpty()) {
        sql = QString("merge %1 a using (%2) b on (a.serial=b.serial)"
                      "when matched then update set a.number=b.number,"
                      "a.name=b.name,a.code=b.code,a.type=b.type,"
                      "a.defaultValue=b.defaultValue,a.range=b.range,"
                      "a.offset=b.offset,a.unit=b.unit,a.scale=b.scale,"
                      "a.regulation=b.regulation,a.remark=b.remark,a.subType=b.subType when"
                      " not matched then insert(number,serial,name"
                      ",code,type,defaultValue,range,offset,unit,scale,"
                      "regulation,remark) values(b.number,b.serial,"
                      "b.name,b.code,b.type,b.defaultValue,"
                      "b.range,b.offset,b.unit,b.scale,b.regulation,"
                      "b.remark,b.subType) output $action,inserted.*,deleted.*;")
                .arg(table.c_str()).arg(cdtRule);
        // ��ձ��δ洢��������֮�������
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
    // ������
    d->q_db.transaction();
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }
    if (!d->q_db.commit()) {   // �����ύ
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief ����һ����������
* @param [in] table : ����
* @param [in] rule : ��������
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DBAccess::saveRule(const std::string &table, const stTableRules &rule)
{
    if (!isOpen()) {
        return false;
    }
    QString condition
            = QString("select '%1' number,'%2' serial,'%3' name,'%4' code,"
                      "'%5' type,'%6' defaultValue,'%7' range,'%8' offset,'%9' unit,"
                      "'%10' scale,'%11' regulation,'%12' remark,'%13' subType")
            .arg(rule.nCode).arg(rule.nSerial)
            .arg(rule.sName.c_str()).arg(rule.sPrgCode.c_str())
            .arg(rule.uType).arg(rule.sDefault.c_str())
            .arg(rule.sRange.c_str()).arg(rule.dOffset, 0, 'g', 16)
            .arg(rule.sUnit.c_str()).arg(rule.dScale, 0, 'g', 16)
            .arg(rule.sRule.c_str()).arg(rule.sRemark.c_str())
            .arg(rule.subType);

    QString sql = QString("merge %1 a using (%2) b on (a.serial=b.serial)"
                          "when matched then update set a.number=b.number,"
                          "a.name=b.name,a.code=b.code,a.type=b.type,"
                          "a.defaultValue=b.defaultValue,a.range=b.range,"
                          "a.offset=b.offset,a.unit=b.unit,a.scale=b.scale,"
                          "a.regulation=b.regulation,a.remark=b.remark,a.subType=b.subType when not"
                          " matched then insert(number,serial,name,code,type,"
                          "defaultValue,range,offset,unit,scale,regulation,"
                          "remark) values(b.number,b.serial,b.name,b.code,"
                          "b.type,b.defaultValue,b.range,b.offset,b.unit,"
                          "b.scale,b.regulation,b.remark,b.subType) output $action,"
                          "inserted.*,deleted.*;")
            .arg(table.c_str()).arg(condition);

    QSqlQuery query(d->q_db);
    // ������
    d->q_db.transaction();
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }
    if (!d->q_db.commit()) {   // �����ύ
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief ����һ����������(��������)
* @param [in] table : ����
* @param [in] rule : ��������
* @param [in] subTables : �ӱ�����
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DBAccess::saveRule(const std::string &table,
                        const stTableRules &rule,
                        const std::vector <stICDBase> &subTables)
{
    if (!isOpen()) {
        return false;
    }
    // �����������
    QString condition
            = QString("select '%1' number,'%2' serial,'%3' name,'%4' code,"
                      "'%5' type,'%6' defaultValue,'%7' range,'%8' offset,'%9' unit,"
                      "'%10' scale,'%11' regulation,'%12' remark, '%13' subType")
            .arg(rule.nCode).arg(rule.nSerial)
            .arg(rule.sName.c_str()).arg(rule.sPrgCode.c_str())
            .arg(rule.uType).arg(rule.sDefault.c_str())
            .arg(rule.sRange.c_str()).arg(rule.dOffset, 0, 'g', 16)
            .arg(rule.sUnit.c_str()).arg(rule.dScale, 0, 'g', 16)
            .arg(rule.sRule.c_str()).arg(rule.sRemark.c_str())
            .arg(rule.subType);

    QString sql = QString("merge %1 a using (%2) b on (a.serial=b.serial)"
                          "when matched then update set a.number=b.number,a.name"
                          "=b.name,a.code=b.code,a.type=b.type,a.defaultValue"
                          "=b.defaultValue,a.range=b.range,a.offset=b.offset,"
                          "a.unit=b.unit,a.scale=b.scale,a.regulation=b.regulation,"
                          "a.remark=b.remark,a.subType=b.subType when not matched "
                          "then insert(number,serial,name,code,type,defaultValue,range,offset,unit,"
                          "scale,regulation,remark) values(b.number,b.serial,"
                          "b.name,b.code,b.type,b.defaultValue,b.range,b.offset,"
                          "b.unit,b.scale,b.regulation,b.remark,b.subType) output $action,"
                          "inserted.*,deleted.*;")
            .arg(table.c_str()).arg(condition);

    // �����ӱ�����
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
        // �Ȳ�ѯ���Ƿ���ڣ�������������ȴ���
        QString cdtCreate
                = QString("select a.tableName from (%1) a except select name"
                          " from sysobjects where xtype='u'").arg(cdtSubTable);
        // ��Ч����
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
        // ����sql
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
    // ����
    d->q_db.transaction();
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }
    if (!d->q_db.commit()) {   // �����ύ
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
* @brief ɾ�����������
* @param [in] table : ����
* @param [in] rule : ��������<�������, <�ӱ�>>
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DBAccess::deleteRules(const std::string &table,
                           const std::unordered_map <int, std::vector < std::string >> &rule)
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
    std::unordered_map<int, std::vector < std::string >> ::const_iterator it = rule.begin();
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
    // ������
    d->q_db.transaction();
    if (!query.exec(sql)) {
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();
        return false;
    }
    if (!d->q_db.commit()) {   // �����ύ
        d->q_error = query.lastError().text();
        d->q_errorFunc = __FUNCTION__;
        qDebug() << d->q_errorFunc << " " << d->q_error;
        d->q_db.rollback();

        return false;
    }

    return true;
}

/**
 * @brief ��ȡ���ݿ�������Ϣ
 * @param [int] config : �������ݿ�������Ϣ��xml�ļ�ȫ·��
 * @param [out] dbInfo : ����������Ϣ
 * @return ִ�н���� true���ɹ���false��ʧ��
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
        d->q_error = QStringLiteral("can not find database config section<driver>��");
        d->q_errorFunc = __FUNCTION__;
        return false;
    }
    dbInfo.driver = config["driver"].asString();
    // serverName
    if (!config.isMember("serverName")) {
        d->q_error = QStringLiteral("can not find database config section<serverName>��");
        d->q_errorFunc = __FUNCTION__;
        return false;
    }
    dbInfo.server = config["serverName"].asString();
    // database
    if (!config.isMember("database")) {
        d->q_error = QStringLiteral("can not find database config section<database>��");
        d->q_errorFunc = __FUNCTION__;
        return false;
    }
    dbInfo.database = config["database"].asString();
    // userName
    if (!config.isMember("userName")) {
        d->q_error = QStringLiteral("can not find database config section<userName>��");
        d->q_errorFunc = __FUNCTION__;
        return false;
    }
    dbInfo.user = config["userName"].asString();
    // password
    if (!config.isMember("password")) {
        d->q_error = QStringLiteral("can not find database config section<password>��");
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
 * @brief ����ICD��������
 * @param [in] table : ����
 * @return �������������
 */
std::string DBAccess::createICDTableCmd(const std::string &table)
{
    // Ĭ��regulation�ֶγ�����2000���ɸ��������ļ��ӱ��������ƽ�������
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
                          "subType smallint null,"
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
* @brief ��ѯָ����������ӱ�
* @param [in] table : ����
* @return �ӱ�
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
        tableList.clear();  // �������
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
        // �������
        name.clear();
        it = tableList;
    } while (!tableList.empty());

    return result;
}
