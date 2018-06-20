#include "precomp.h"
#include "datamanagement.h"
#include "Dictionary.h"
#include "KernelClass/icdfactory.h"
#include "KernelClass/icdframecodedata.h"
#include "KernelClass/icdbitdata.h"
#include "icdwidget/icdwidget_global.h"
#include "icdparser_sql.h"
#include "main_global.h"

#if 0
#define _DEBUG_PRINT_D
#endif

DataManegement::DataManegement(QWidget *parent)
    : QObject(parent)
    , parentWidget_(parent)
    , q_dbaccess(nullptr)
    , q_dataSource(GlobalDefine::dsNone)
{
    // 默认文件路径
    q_defaultPath = readXmlFile();

    q_dbaccess = new DBAccess();
    if (GlobalDefine::dsDatabase == q_dataSource) {
        // 初始化数据库
        const Json::Value option = JMain::instance()->option("edit", "parser");
        if (!option.isNull()) {
            if (!q_dbaccess->init(option)) {
                QMessageBox::warning(parentWidget_, QStringLiteral("数据库错误"),
                                     q_dbaccess->lastError().c_str());
            }
        }
    }

    jnotify->on("edit.toolbar.database.config", this, [=](JNEvent &){
        QVariantList args;
        args << "edit" << qVariantFromValue((void*)this);
        jnotify->send("database.config", args);
    });
    jnotify->on("edit.parser.changed", this, [=](Icd::JNEvent &event){
        dataSourceChanged(event);
    });
    jnotify->on("edit.loadInfrastructure", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.queryAllPlane", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.querySinglePlane", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.querySingleElement", this, [=](Icd::JNEvent &event){
        querySingleElement(event);
    });
    jnotify->on("edit.queryDictionary", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.queryDictionaryTable", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.queryDataSource", this, [=](Icd::JNEvent &event){
        queryDataSource(event);
    });
    jnotify->on("edit.queryMaxTableCount", this, [=](Icd::JNEvent &event){
        queryMaxTableCount(event);
    });
    jnotify->on("edit.loadRules", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.unLoadRules", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.modifyAllPlane", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.savePlane", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.modifyPlane", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.deletePlane", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.saveSystem", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.modifySystem", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.deleteSystem", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.saveTable", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.modifyTable", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.deleteTable", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.saveRule", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.insertRule", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.deleteRule", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.deleteSubTable", this, [=](Icd::JNEvent &event){
        notifyRespond(event);
    });
    jnotify->on("edit.copyMemoryData", this, [=](Icd::JNEvent &event){
        saveCopyData(event);
    });
    jnotify->on("edit.saveDatabase", this, [=](Icd::JNEvent &event){
        saveDatabase(event);
    });
    jnotify->on("edit.saveFile", this, [=](Icd::JNEvent &event){
        saveFile(event);
    });
    jnotify->on("edit.queryDBError", this, [=](Icd::JNEvent &event){
        queryDBError(event);
    });
}

DataManegement::~DataManegement()
{
}

bool DataManegement::init()
{
    return true;
}

// 消息响应回调
JLRESULT DataManegement::notifyRespond(const Icd::JNEvent &event)
{
    QVariantList args = event.argument().toList();
    const QString id = event.channel().section("edit.", 1);

    if ("loadInfrastructure" == id) { // 查询所有的机型数据
        if (args.count() >= 2) {
            QString *error = jVariantFromVoid<QString>(args[1]);
            if (!error) {
                return -1;
            }
            switch (args[0].toInt()) {
            case GlobalDefine::dsDatabase:
            {
                // deep
                int deep = Icd::ObjectTable;
                if (args.count() >= 3) {
                    deep = args[2].toInt();
                }
                // 加载基本结构数据
                std::string err;
                if (!loadBaseData(err, deep)) {
                    *error = err.c_str();
                }
                break;
            }
            case GlobalDefine::dsFile:
            {
                Json::Value config;
                config["sourceType"] = "file";
                config["filePath"] = q_defaultPath;
                Icd::ParserPtr parser = Icd::Parser::create(config);
                if (!parser) {
                    return -1;
                }
                // deep
                int deep = Icd::ObjectTable;
                if (args.count() >= 3) {
                    deep = args[2].toInt();
                }
                // 加载文件数据
                Icd::VehiclePtrArray vehicles;
                // 解析数据
                if (!parser->parse(vehicles, deep)) {
                    *error = QStringLiteral("解析文件失败！");
                    break;
                }
                // 转换数据
                Json::Value config2(Json::objectValue);
                Icd::SqlParser sqlParser(config2);
                DMSpace::_vectorPS infrastructure;
                DMSpace::_vectorIcdTR tableRules;
                std::vector<int> params;
                params.push_back((int)&infrastructure);
                params.push_back((int)&tableRules);

                if (!sqlParser.convert2Other(vehicles, params)) {
                    *error = QStringLiteral("解析文件失败！");
                    break;
                }
                if (!loadXmlBaseData(infrastructure, tableRules)) {
                    *error = QStringLiteral("解析文件失败！");
                }
                break;
            }
            default:
                break;
            }
        }
    } else if("queryAllPlane" == id) { // 查询所有的机型数据
        if (args.count() > 0) {
            PlaneNode::planeVector *data = jVariantFromVoid<PlaneNode::planeVector>(args[0]);
            if (!data) {
                return -1;
            }
            *data = q_planes;
        }
    } else if ("querySinglePlane" == id) { // 查询一条机型数据
        if (args.count() >= 2) {
            PlaneNode::smtPlane *data = jVariantFromVoid<PlaneNode::smtPlane>(args[0]);
            if (!data) {
                return -1;
            }
            *data = planeNode(args[1].toInt());
        }
    } else if ("queryDictionary" == id) { // 查询字典表
        if (args.count() > 0) {
            stQueryDic *data = jVariantFromVoid<stQueryDic>(args[0]);
            if (!data) {
                return -1;
            }

            queryDictionary(*data);
        }
    } else if ("queryDictionaryTable" == id) { // 查询一张字典表
        if (args.count() < 2) {
            return -1;
        }
        const int type = args[1].toInt();
        if (type > GlobalDefine::dicCheckType || type < GlobalDefine::dicDataType) {
            return 0;
        }
        std::vector<stDictionary> *dics = jVariantFromVoid<std::vector<stDictionary>>(args[0]);
        if (!dics) {
            return -1;
        }
        std::string dicName;
        switch (type) {
        case GlobalDefine::dicDataType:    // 数据类型
            dicName = DBAccess::tableName(DataBaseDefine::dbDataType);
            break;
        case GlobalDefine::dicPowerType:   // 权限类型
            dicName = DBAccess::tableName(DataBaseDefine::dbPowerType);
            break;
        case GlobalDefine::dicCounterType: // 帧计数类型
            dicName = DBAccess::tableName(DataBaseDefine::dbCounterType);
            break;
        case GlobalDefine::dicCheckType:   // 校验类型
            dicName = DBAccess::tableName(DataBaseDefine::dbCheckType);
            break;
        case GlobalDefine::dicArrayType:   // 数组类型
            dicName = DBAccess::tableName(DataBaseDefine::dbArrayType);
            break;
        case GlobalDefine::dicNumericType:  // 数值类型
            dicName = DBAccess::tableName(DataBaseDefine::dbNumericType);
            break;
        default:
            break;
        }
        *dics = CDictionary::instance().singleDic(dicName);
    } else if ("loadRules" == id) {    // 加载规则数据
        if (args.count() < 2) {
            return -1;
        }
        PlaneNode::smtPlane *data = jVariantFromVoid<PlaneNode::smtPlane>(args[0]);
        if (!data) {
            return -1;
        }
        const std::vector<int> *params = jVariantFromVoid<std::vector<int>>(args[1]);
        if (!params || 2 != params->size()) {
            return 0;
        }
        const QString &condition = *reinterpret_cast<QString *>(params->at(0));
        bool increment = params->at(1);
        loadRules(*data, condition, increment);
    } else if ("unLoadRules" == id) {    // 卸载规则数据
        if (args.count() < 1) {
            return -1;
        }
        const QString *condition = jVariantFromVoid<QString>(args[0]);
        if (!condition) {
            return -1;
        }
        unLoadRules(*condition);
    } else if ("modifyAllPlane" == id) { // 修改所有机型数据
        if (args.count() < 2) {
            return -1;
        }
        const std::vector<ICDElement::smtElement> *elements
                = jVariantFromVoid<std::vector<ICDElement::smtElement> >(args[0]);
        if (!elements) {
            return -1;
        }
        bool *result = jVariantFromVoid<bool>(args[1]);
        if (!result) {
            return -1;
        }
        const int count = elements->size();
        std::vector<PlaneNode::smtPlane> planes;
        for (int i = 0; i < count; ++i) {
            planes.push_back(std::dynamic_pointer_cast<PlaneNode>(elements->at(i)));
        }
        // 更新所有机型数据
        *result = modifyAllKC(planes);
    } else if ("savePlane" == id) { // 保存机型数据
        if (args.count() < 2) {
            return -1;
        }
        stPlane *plane = jVariantFromVoid<stPlane>(args[0]);
        if (!plane) {
            return -1;
        }
        bool *result = jVariantFromVoid<bool>(args[1]);
        if (!result) {
            return -1;
        }
        // 保存数据
        *result = savePlane(*plane);
    } else if ("modifyPlane" == id) {   // 修改机型数据
        if (args.count() < 2) {
            return -1;
        }
        ICDElement::smtElement *element = jVariantFromVoid<ICDElement::smtElement>(args[0]);
        if (!element) {
            return -1;
        }
        bool *result = jVariantFromVoid<bool>(args[1]);
        if (!result) {
            return -1;
        }
        // 修改数据
        *result = modifyOneKC(SMT_CONVERT(PlaneNode, *element));
    } else if ("deletePlane" == id) {   // 删除机型数据
        if (args.count() < 2) {
            return -1;
        }
        std::vector<int> *planeID = jVariantFromVoid<std::vector<int> >(args[0]);
        if (!planeID) {
            return -1;
        }
        bool *result = jVariantFromVoid<bool>(args[1]);
        if (!result) {
            return -1;
        }
        // 删除数据
        *result = deletePlane(*planeID);
    } else if ("saveSystem" == id) {    // 保存系统数据
        if (args.count() < 2) {
            return -1;
        }
        std::vector<void*> *params = jVariantFromVoid<std::vector<void*> >(args[0]);
        if (!params || params->size() != 2) {
            return 0;
        }
        QString &planeID = *reinterpret_cast<QString *>(params->at(0));
        std::vector<stSystem> &systems
                = *reinterpret_cast<std::vector<stSystem> *>(params->at(1));
        bool *result = jVariantFromVoid<bool>(args[1]);
        // 保存数据
        *result = saveSystem(planeID.toInt(), systems);
    } else if ("modifySystem" == id) {    // 修改系统数据
        if (args.count() < 2) {
            return -1;
        }
        std::vector<void*> *params = jVariantFromVoid<std::vector<void*> >(args[0]);
        if (!params || params->size() != 2) {
            return 0;
        }
        int &planeID = *reinterpret_cast<int *>(params->at(0));
        ICDElement::smtElement &element
                = *reinterpret_cast<ICDElement::smtElement *>(params->at(1));
        bool *result = jVariantFromVoid<bool>(args[1]);
        // 修改数据
        *result = modifySystem(planeID, SMT_CONVERT(SystemNode, element));
    } else if ("deleteSystem" == id) {  // 删除系统数据
        if (args.count() < 2) {
            return -1;
        }
        std::vector<void*> *params = jVariantFromVoid<std::vector<void*> >(args[0]);
        if (!params || params->size() != 2) {
            return 0;
        }
        QString &planeID = *reinterpret_cast<QString *>(params->at(0));
        std::vector<int> &systemID
                = *reinterpret_cast<std::vector<int> *>(params->at(1));
        bool *result = jVariantFromVoid<bool>(args[1]);
        // 删除数据
        *result = deleteSystem(planeID.toInt(), systemID);
    } else if ("saveTable" == id) { // 保存ICD表数据
        if (args.count() < 2) {
            return -1;
        }
        std::vector<void*> *params = jVariantFromVoid<std::vector<void*> >(args[0]);
        if (!params || params->size() != 2) {
            return 0;
        }
        QString &keys = *reinterpret_cast<QString *>(params->at(0));
        std::vector<stICDBase> &tables
                = *reinterpret_cast<std::vector<stICDBase> *>(params->at(1));
        bool *result = jVariantFromVoid<bool>(args[1]);
        // 保存数据
        *result = saveTable(keys, tables);
    } else if ("modifyTable" == id) { // 修改ICD表数据
        if (args.count() < 2) {
            return -1;
        }
        std::vector<void*> *params = jVariantFromVoid<std::vector<void*> >(args[0]);
        if (!params || params->size() != 2) {
            return 0;
        }
        QString &keys = *reinterpret_cast<QString *>(params->at(0));
        ICDElement::smtElement element
                = *reinterpret_cast<ICDElement::smtElement *>(params->at(1));
        bool *result = jVariantFromVoid<bool>(args[1]);
        // 修改数据
        *result = modifyTable(keys, SMT_CONVERT(TableNode, element));
    } else if ("deleteTable" == id) {
        if (args.count() < 2) {
            return -1;
        }
        std::vector<void*> *params = jVariantFromVoid<std::vector<void*> >(args[0]);
        if (!params || params->size() != 2) {
            return 0;
        }
        QString &keys = *reinterpret_cast<QString *>(params->at(0));
        std::vector<std::string> tables
                = *reinterpret_cast<std::vector<std::string> *>(params->at(1));
        bool *result = jVariantFromVoid<bool>(args[1]);
        // 删除数据
        *result = deleteTable(keys, tables);
    } else if ("saveRule" == id) {
        if (args.count() < 2) {
            return -1;
        }
        std::vector<void*> *params = jVariantFromVoid<std::vector<void*> >(args[0]);
        if (!params || params->size() != 2) {
            return 0;
        }
        QString &keys = *reinterpret_cast<QString *>(params->at(0));
        ICDMetaData::smtMeta &meta
                = *reinterpret_cast<ICDMetaData::smtMeta *>(params->at(1));
        int *result = jVariantFromVoid<int>(args[1]);
        // 保存数据库
        *result = saveRule(keys, meta);
    } else if ("insertRule" == id) {
        if (args.count() < 2) {
            return -1;
        }
        std::vector<void*> *params = jVariantFromVoid<std::vector<void*> >(args[0]);
        if (!params || params->size() != 2) {
            return 0;
        }
        QString &keys = *reinterpret_cast<QString *>(params->at(0));
        ICDMetaData::smtMeta &meta
                = *reinterpret_cast<ICDMetaData::smtMeta *>(params->at(1));
        bool *result = jVariantFromVoid<bool>(args[1]);
        // 保存数据库
        *result = insertRule(keys, meta);
    } else if ("deleteRule" == id) {    // 删除规则数据
        if (args.count() < 2) {
            return -1;
        }
        std::vector<void*> *params = jVariantFromVoid<std::vector<void*> >(args[0]);
        if (!params || params->size() != 2) {
            return 0;
        }
        QString &keys = *reinterpret_cast<QString *>(params->at(0));
        std::vector<int> &rules
                = *reinterpret_cast <std::vector<int> *>(params->at(1));
        bool *result = jVariantFromVoid<bool>(args[1]);
        // 删除数据库
        *result = deleteRule(keys, rules);
    } else if ("deleteSubTable" == id) {    // 删除帧识别码子表
        if (args.count() < 2) {
            return -1;
        }
        std::vector<void*> *params = jVariantFromVoid<std::vector<void*> >(args[0]);
        if (!params || params->size() != 2) {
            return 0;
        }
        QString &keys = *reinterpret_cast<QString *>(params->at(0));
        std::vector<std::string> &rules
                = *reinterpret_cast <std::vector<std::string> *>(params->at(1));
        bool *result = jVariantFromVoid<bool>(args[1]);
        // 删除数据库
        *result = deleteSubTable(keys, rules);
    }

    return 0;
}

/**
 * @brief 加载基础数据
     * @param [in] deep
 * @return 执行结果，true：成功；false：失败
 */
bool DataManegement::loadBaseData(std::string &error, int deep)
{
    // 清空数据
    clearKC();

    // 初始化字典表数据
    if (!loadDic()) {
        return false;
    }

    // 初始化系统信息
    if (!loadInfrastructure(deep)) {
        error = q_dbaccess->lastError().c_str();
        qDebug() << error.c_str();
        return false;
    }

    return true;
}

/**
* @brief 加载基础数据
* @param [in] loadXmlBaseData : 机型系统信息
* @param [in] tableRules : 表规则信息
* @return 执行结果，true：成功；false：失败
*/
bool DataManegement::loadXmlBaseData(const DMSpace::_vectorPS &infrastructure,
                                     const DMSpace::_vectorIcdTR &tableRules)
{
    // 清空数据
    clearKC();

    // 初始化字典表数据
    if (!loadDic()) {
        return false;
    }
    TableNode::tableVector tables;  // 记录表规则
    // 构造顶层表数据
    int count = tableRules.size();
    //    int tableLevel = 1; // 顶层表从1开始
    for (int i = 0; i < count; ++i) {
        const DMSpace::pairIcdTR &icdTR = tableRules[i];
        stICDBase icdBase = icdTR.first;
        if (!icdBase.sParentName.empty()) {
            continue;
        }
        // 填写界面等级
        //         if (icdBase.sLevel.empty()) {
        //             icdBase.sLevel = QString("%1_%2").arg(tableLevel)
        //                 .arg(i + 1).toStdString();
        //         }
        // 没有父表，证明是顶层表
        TableNode::smtTable tableNode(new TableNode(icdBase));
        tables.push_back(tableNode);
    }

    // 根据所属组进行分类
    count = tables.size();
    TableNode::smtTable table = 0;
    std::unordered_map<std::string, TableNode::tableVector> classified;
    for (int i = 0; i < count; ++i) {
        if (table = tables[i]) {
            stICDBase icdBase = table->icdBase();
            classified[icdBase.sGroup].push_back(table);
        }
    }

    // 构造机型系统数据
    std::string group;
    std::unordered_map<std::string, TableNode::tableVector>::iterator
            itTable = classified.end();
    DMSpace::_vectorPS_Cit itPS = infrastructure.begin();
    for (; itPS != infrastructure.end(); ++itPS) {
        const stPlane &plane = itPS->first;
        // 机型
        PlaneNode::smtPlane planeNode(new PlaneNode(plane));
        const std::vector<stSystem>& department = itPS->second;
        count = department.size();
        // 系统
        for (int i = 0; i < count; ++i) {
            const stSystem &system = department.at(i);
            SystemNode::smtSystem systemNode(new SystemNode(system));
            group = QString("%1/%2")
                    .arg(plane.nCode).arg(system.nCode).toStdString();
            if ((itTable = classified.find(group)) != classified.end()) {
                systemNode->setTable(itTable->second);
            }
            planeNode->addSystem(systemNode);
        }
        addOneKC(planeNode);
    }

    return true;
}

/**
 * @brief 加载字典表数据
 * @return 执行结果，true：成功；false：失败
 */
bool DataManegement::loadDic()
{
    if (NULL == q_dbaccess) {
        return false;
    }
    std::unordered_map<std::string, std::vector<stDictionary> > dic;
    /*
    if (!q_dbaccess->readDictionary(dic)) {
        qDebug() <<"readDictionary failed!";
        return false;
        }*/
    if (!q_dbaccess->readLocalDictionary(dic)) {
        qDebug() << "readDictionary failed!";
        return false;
    }
    CDictionary::instance().initDictionary(dic);

    return true;
}

/**
* @brief 重新加载字典表数据
* @param [in] dics : 字典表集
* @return 执行结果，true：成功；false：失败
*/
bool DataManegement::reloadDic(const std::vector<std::string> &dics)
{
    Q_UNUSED(dics);
    if (NULL == q_dbaccess) {
        return false;
    }
    std::unordered_map<std::string, std::vector<stDictionary> > data;

    /*if (!q_dbaccess->readDictionary(dics, data)) {
        qDebug() <<"readDictionary failed!";
        return false;
        }*/
    if (!q_dbaccess->readLocalDictionary(data)) {
        qDebug() << "readDictionary failed!";
        return false;
    }

    return CDictionary::instance().updateDictionary(data);
}

/**
 * @brief 加载基础架构数据（机型和系统）
     * @param [in] deep
 * @return 执行结果，true：成功；false：失败
 */
bool DataManegement::loadInfrastructure(int deep)
{
    if (!q_dbaccess) {
        return false;
    }
    // 读取机型
    std::vector<stPlane> planeBase;
    if (!q_dbaccess->readPlane(planeBase)) {
        qDebug() << "readPlane failed!";
        return false;
    }
    // 读取系统
    std::unordered_map<int, std::vector<stSystem> > systemBase;
    if (deep >= Icd::ObjectSystem) {
        if (!q_dbaccess->readSystem(systemBase)) {
            qDebug() <<"readSystem failed!";
            return false;
        }
    }
    // 查询表基本信息
    std::unordered_map<std::string, std::vector<stICDBase>> icdBase;
    if (deep >= Icd::ObjectTable) {
        if (!q_dbaccess->readICDBase(true, icdBase)) {
            qDebug() <<"readICDBase failed!";
            return false;
        }
    }
    //
    std::unordered_map<std::string, std::vector<stICDBase>>::iterator itBase = icdBase.end();
    std::unordered_map<int, std::vector<stSystem> >::iterator it = systemBase.end();
    const int count = planeBase.size();
    //    int tableLevel = 1; // 顶层表从1开始
    for (int i = 0; i < count; ++i) {
        // 机型
        stPlane plane = planeBase.at(i);
        PlaneNode::smtPlane planeNode(new PlaneNode(plane));
        if ((it = systemBase.find(plane.nCode)) != systemBase.end()) {
            const std::vector<stSystem>& department = it->second;
            const int bound = department.size();
            // 分系统
            for (int j = 0; j < bound; ++j) {
                stSystem system = department.at(j);
                SystemNode::smtSystem systemNode(new SystemNode(system));
                std::string group = QString("%1/%2")
                        .arg(plane.nCode).arg(system.nCode).toStdString();
                // 规则表
                if ((itBase = icdBase.find(group)) != icdBase.end()) {
                    std::vector<stICDBase>& collBase = itBase->second;
                    const int bound(collBase.size());
                    for (int j = 0; j < bound; ++j) {
                        stICDBase& base = collBase.at(j);
                        // 只加入隶属于系统节点的规则表
                        if (base.sParentName.empty()) {
                            // 填写界面等级
                            //                             if (base.sLevel.empty()) {
                            //                                 base.sLevel = QString("%1_%2")
                            //                                     .arg(tableLevel).arg(j + 1)
                            //                                     .toStdString();
                            //                             }
                            TableNode::smtTable tableNode(new TableNode(base));
                            systemNode->addTable(tableNode);
                        }
                    }
                }
                planeNode->addSystem(systemNode);
            }
        }
        addOneKC(planeNode);
    }

    return true;
}

/**
 * @brief 加载指定机型的所有ICD表
 * @param [in] planeType : 飞机型号码
 * @param [in] increment : 增量标识
 * @return 执行结果，true：成功；false：失败
 */
bool DataManegement::loadTypeRule(int planeType, bool increment)
{
    if (NULL == q_dbaccess) {
        return false;
    }
    // 读取基本表信息
    DMSpace::_vectorSB ICDBase;
    QString condition = QString::number(planeType);
    if (!q_dbaccess->readSpecificICD(condition.toStdString(), ICDBase)) {
        return false;
    }
    return loadRuleData(planeType, ICDBase, increment);
}

/**
* @brief 加载指定机型的所有ICD表
* @param [in] planeType : 飞机型号码
* @param [in] tableRules : 表规则信息
* @return 执行结果，true：成功；false：失败
*/
bool DataManegement::loadXmlTypeRule(int planeType,
                                     DMSpace::_vectorIcdTR &tableRules)
{
    // 查询内存中是否已经加载基础架构数据
    PlaneNode::smtPlane plane = planeNode(planeType);
    if (NULL == plane) {
        return false;
    }
    SystemNode::smtSystem system;
    std::unordered_map<std::string, TableNode::tableVector> rules = loadXmlRuleData(tableRules);
    std::unordered_map<std::string, TableNode::tableVector>::iterator it = rules.begin();
    // 将数据保存到内存中
    for (; it != rules.end(); ++it) {
        const std::string &group = it->first;
        system = plane->system(QString(group.c_str())
                               .split("/").last().toInt());
        if (system) {
            system->setTable(it->second);
        }
    }

    return true;
}

/**
* @brief 卸载指定机型的所有ICD表
* @param [in] planeType : 飞机型号码
* @return 执行结果，true：成功；false：失败
*/
void DataManegement::unLoadTypeRule(int planeType)
{
    PlaneNode::smtPlane smtPlane = planeNode(planeType);
    if (smtPlane) {
        SystemNode::systemVector systems = smtPlane->allSystem();
        const int count = systems.size();
        SystemNode::smtSystem smtSystem = 0;
        TableNode::tableVector tables;
        TableNode::smtTable smtTable = 0;
        for (int i = 0; i < count; ++i) {
            if (!(smtSystem = systems[i])) {
                continue;
            }
            tables = smtSystem->allTable();
            const int bound = tables.size();
            for (int j = 0; j < bound; ++j) {
                if (!(smtTable = tables[j])) {
                    continue;
                }
                smtTable->clearRule();
                smtTable->setLengthCheck(false);
            }
        }
    }
}

/**
 * @brief 加载指定机型下分系统的所有ICD表
 * @param [in] planeType : 飞机型号码
 * @param [in] system : 系统码
 * @param [in] increment : 增量标识
 * @return 执行结果，true：成功；false：失败
 */
bool DataManegement::loadSystemRule(int planeType, int system,
                                    bool increment)
{
    if (NULL == q_dbaccess) {
        return false;
    }
    // 读取基本表信息
    DMSpace::_vectorSB ICDBase;
    QString condition = QString("%1/%2").arg(planeType).arg(system);
    if (!q_dbaccess->readSpecificICD(condition.toStdString(), ICDBase)) {
        return false;
    }
    return loadRuleData(planeType, ICDBase, increment);
}

/**
* @brief 加载指定机型下分系统的所有ICD表
* @param [in] planeType : 飞机型号码
* @param [in] system : 系统码
* @return 执行结果，true：成功；false：失败
*/
bool DataManegement::loadXmlSystemRule(int planeType, int system,
                                       DMSpace::_vectorIcdTR &tableRules)
{
    // 查询内存中是否已经加载基础架构数据
    PlaneNode::smtPlane plane = planeNode(planeType);
    if (!plane) {
        return false;
    }
    SystemNode::smtSystem singleSystem = plane->system(system);
    if (!system) {
        return false;
    }
    std::unordered_map<std::string, TableNode::tableVector> rules = loadXmlRuleData(tableRules);
    const std::string group = QString("%1/%2").arg(planeType).arg(system).toStdString();
    std::unordered_map<std::string, TableNode::tableVector>::iterator it = rules.find(group);
    // 将数据保存到内存中
    if (it != rules.end()) {
        singleSystem->setTable(it->second);
    }

    return true;
}

/**
* @brief 卸载指定机型下分系统的所有ICD表
* @param [in] planeType : 飞机型号码
* @param [in] system : 系统码
* @return 执行结果，true：成功；false：失败
*/
void DataManegement::unLoadSystemRule(int planeType, int system)
{
    PlaneNode::smtPlane smtPlane = planeNode(planeType);
    if (!smtPlane) {
        return;
    }
    SystemNode::smtSystem smtSystem = smtPlane->system(system);
    if (!smtSystem) {
        return;
    }
    TableNode::tableVector tables = smtSystem->allTable();
    const int count = tables.size();
    TableNode::smtTable smtTable = 0;
    for (int i = 0; i < count; ++i) {
        if (!(smtTable = tables[i])) {
            continue;
        }
        smtTable->clearRule();
        smtTable->setLengthCheck(false);
    }
}

/**
 * @brief 加载单张ICD表
 * @param [in] plane : 飞机型号码
 * @param [in] system : 系统码
 * @param [in] table : ICD表名
 * @return 执行结果，true：成功；false：失败
 */
bool DataManegement::loadTableRule(int plane,
                                   int system,
                                   const std::string &table)
{
    if (NULL == q_dbaccess) {
        return false;
    }
    // 查询内存中是否已经加载基础架构数据
    PlaneNode::smtPlane data = planeNode(plane);
    if (NULL == data) {
        return false;
    }
    // 查询系统表
    SystemNode::smtSystem sysNode = data->system(system);
    if (NULL == sysNode) {
        return false;
    }
    // 读取子表
    std::vector<std::string> tables = q_dbaccess->querySubTables(table);
    tables.insert(tables.begin(), table);
    //
    std::vector<stICDBase> icdBases;
    if (!q_dbaccess->readICDBase(tables, icdBases)) {
        return false;
    }
    // 读取规则数据
    DMSpace::svrMap rules;
    if (!q_dbaccess->readTableRules(tables, rules)) {
        return false;
    }
    // 构造表
    TableNode::tableMap tableNodes;    // 记录表节点
    const int count = icdBases.size();
    for (int j = 0; j < count; ++j) {
        const stICDBase &icdData = icdBases[j];

        TableNode::smtTable table(new TableNode(icdData));
        tableNodes[icdData.sName] = table;
    }
    // 填充规则
    fillRules(tableNodes, rules);

    // 查询关系，将表数据保存到内存中
    // 查询复合表的子表数据，并将之关联起来
    TableNode::smtTable _table = recursiveLinkTable(tableNodes, table);
    if (!_table) {
        return false;
    }
    _table->setLengthCheck(_table->length() == (int)_table->realLength()
                           && _table->length() > 0);
    // 更新内存数据
    sysNode->addTable(_table);

    return true;
}

/**
* @brief 加载单张ICD表
* @param [in] plane : 飞机型号码
* @param [in] system : 系统码
* @param [in] table : ICD表名
* @param [in] tableRules : 表规则信息
* @return 执行结果，true：成功；false：失败
*/
bool DataManegement::loadXmlTablemRule(int plane, int system,
                                       const std::string &table,
                                       DMSpace::_vectorIcdTR &tableRules)
{
    // 查询内存中是否已经加载基础架构数据
    PlaneNode::smtPlane singlePlane = planeNode(plane);
    if (!singlePlane) {
        return false;
    }
    SystemNode::smtSystem singleSystem = singlePlane->system(system);
    if (!system) {
        return false;
    }
    std::unordered_map<std::string, TableNode::tableVector> rules = loadXmlRuleData(tableRules);
    const std::string group = QString("%1/%2").arg(plane).arg(system).toStdString();
    std::unordered_map<std::string, TableNode::tableVector>::iterator it = rules.find(group);
    // 将数据保存到内存中
    if (it != rules.end()) {
        const TableNode::tableVector &tables = it->second;
        const int count = tables.size();
        TableNode::smtTable singleTable = 0;
        for (int i = 0; i < count; ++i) {
            if ((singleTable = tables[i]) && singleTable->id() == table) {
                singleSystem->addTable(singleTable);
            }
        }
    }

    return true;
}

/**
* @brief 卸载单张ICD表
* @param [in] planeType : 飞机型号码
* @param [in] system : 系统码
* @param [in] table : ICD表名
* @return 执行结果，true：成功；false：失败
*/
void DataManegement::unLloadTableRule(int plane,
                                      int system,
                                      const std::string &table)
{
    PlaneNode::smtPlane smtPlane = planeNode(plane);
    if (!smtPlane) {
        return;
    }
    SystemNode::smtSystem smtSystem = smtPlane->system(system);
    if (!smtSystem) {
        return;
    }
    TableNode::smtTable smtTable = smtSystem->table(table);
    if (!smtTable) {
        return;
    }
    smtTable->clearRule();
    smtTable->setLengthCheck(false);
}

/**
 * @brief 递归加载表规则数据
 * @param [in] name : ICD表名
 * @param [in] table : 规则数据
 * @param [in] rules : 表规则数据
 * @return 执行结果，true：成功；false：失败
 */
bool DataManegement::recursiveLoadRule(const std::string &name,
                                       TableNode::smtTable &table,
                                       DMSpace::_vectorIcdTR &rules)
{
    if (!table) {
        return false;
    }
    // 查询数据
    DMSpace::pairIcdTR icdTR = singleIcdTR(name, rules, true);
    if (icdTR.first.sName.empty()) {
        return false;
    }
    int size = 0;
    table->setICDBase(icdTR.first);
    const std::vector<stTableRules> &tableRules = icdTR.second;
    std::unordered_map<int, ICDFrameCodeData::smtFrameCode> codes;    // 帧识别码
    const int count = tableRules.size();
    ICDMetaData::smtMeta metaData = 0;
    for (int i = 0; i < count; ++i) {
        const stTableRules &ruleData = tableRules[i];
        // 根据规则数据创建对应的数据类实例
        if (!(metaData = ICDFactory::instance().CreatObject(ruleData))) {
            continue;
        }
        if (GlobalDefine::dtFrameCode == ruleData.uType) {
            codes[atoi(ruleData.sDefault.c_str())]
                    = SMT_CONVERT(ICDFrameCodeData, metaData);
        }
        // 如果规则数据为复合数据，则再次读取该表的规则数据
        if (IcdDefine::icdComplex == metaData->metaType()) {
            ICDComplexData::smtComplex complex
                    = std::dynamic_pointer_cast<ICDComplexData>(metaData);
            // 循环读取子表数据
            QStringList nameLst = QString::fromStdString(complex->rule())
                    .split("@", QString::SkipEmptyParts);
            TableNode::tableVector subTables;
            size = nameLst.size();
            for (int j = 0; j < size; ++j) {
                TableNode::smtTable subTable(new TableNode(stICDBase()));
                if (recursiveLoadRule(nameLst[j].toStdString(),
                                      subTable, rules)) {
                    subTables.push_back(subTable);
                }
            }
            complex->setTable(subTables);
        }
        // 记录
        table->addRule(metaData);
    }
    // 将帧识别码和帧数据建立连接
    std::unordered_map<int, ICDFrameCodeData::smtFrameCode>::iterator itC;
    for (itC = codes.begin(); itC != codes.end(); ++itC) {
        ICDFrameCodeData::smtFrameCode &frameCode = itC->second;
        ICDComplexData::smtComplex complex =
                std::dynamic_pointer_cast<ICDComplexData>(
                    table->rule(itC->first));
        if (frameCode && complex) {
            frameCode->bindData(complex);
        }
    }

    return true;
}

/**
 * @brief 新增一条机型数据
 * @param [in] data : 机型数据
 * @return 执行结果，true：成功；false：失败
 */
bool DataManegement::addOneKC(const PlaneNode::smtPlane &data)
{
    if (!data) {
        return false;
    }
    const int count = q_planes.size();
    PlaneNode::smtPlane plane = 0;
    for (int i = 0; i < count; ++i) {
        if ((plane = q_planes[i]) && plane->id() == data->id()) {
            return false;
        }
    }
    q_planes.push_back(data);

    return true;
}

/**
 * @brief 修改一条机型数据
 * @param [in] data : 机型数据
 * @return 执行结果，true：成功；false：失败
 */
bool DataManegement::modifyOneKC(const PlaneNode::smtPlane &data)
{
    if (!data) {
        return false;
    }
    const int count = q_planes.size();
    PlaneNode::smtPlane plane = 0;
    for (int i = 0; i < count; ++i) {
        if ((plane = q_planes[i]) && plane->id() == data->id()) {
            q_planes[i] = data;
            break;
        } else {
            plane = 0;
        }
    }
    if (!plane) {
        q_planes.push_back(data);
    }

    return true;
}

/**
 * @brief 删除一条机型数据
 * @param [in] code : 机型码
 * @return 执行结果，true：成功；false：失败
 */
bool DataManegement::deleteOneKC(int code)
{
    bool result = false;
    PlaneNode::smtPlane plane = 0;
    PlaneNode::planeVector::iterator it = q_planes.begin();
    for (; it != q_planes.end(); ++it) {
        if ((plane = *it) && plane->numeralId() == code) {
            q_planes.erase(it);
            result = true;
            break;
        }
    }

    return true;
}

/**
* @brief 清空机型数据
*/
void DataManegement::clearKC()
{
    q_planes.clear();
}

/**
 * @brief 查询机型数据
 * @param [in] code : 机型码
 * @return 机型数据
 */
PlaneNode::smtPlane DataManegement::planeNode(int code)
{
    PlaneNode::smtPlane plane = 0;
    const int count = q_planes.size();
    for (int i = 0; i < count; ++i) {
        if ((plane = q_planes[i]) && plane->numeralId() == code) {
            break;
        } else {
            plane = 0;
        }
    }

    return plane;
}

/**
* @brief 查询当个规则表数据
* @param [in] name : ICD表名
* @param [in] rules : ICD表数据集
* @param [in] remove : 删除标识
* @return 规则表数据
*/
DMSpace::pairIcdTR DataManegement::singleIcdTR(const std::string &name,
                                               DMSpace::_vectorIcdTR &rules,
                                               bool remove /*= false*/) const
{
    DMSpace::pairIcdTR result;
    DMSpace::_vectorIcdTR_It it = rules.begin();
    for (; it != rules.end(); ++it) {
        if (name == it->first.sName) {
            result = *it;
            break;
        }
    }
    if (remove && it != rules.end()) {
        rules.erase(it);
    }

    return result;
}

/**
 * @brief 加载规则数据
 * @param [in] planeType : 机型码
 * @param [in] ICDBase : ICD基本表数据
 * @param [in] increment : 增量标识
 * @return 执行结果，true：成功；false：失败
 */
bool DataManegement::loadRuleData(int planeType,
                                  const DMSpace::_vectorSB &ICDBase,
                                  bool increment)
{
    // 访问数据库
    if (NULL == q_dbaccess) {
        return false;
    }
    // 查询内存中是否已经加载基础架构数据
    PlaneNode::smtPlane data = planeNode(planeType);
    if (NULL == data) {
        return false;
    }
    TableNode::tableMap tableNodes;    // 记录表节点
    std::vector<std::string> topTables;     // 记录顶层表
    // 构造关系
    std::vector<std::string> tables;    // 记录所有表
    int count = ICDBase.size();
    int size = 0;
    for (int i = 1; i < count; ++i) {
        const std::vector<stICDBase> &base = ICDBase[i].second;
        size = base.size();
        for (int j = 0; j < size; ++j) {
            const stICDBase &icdData = base[j];
            tables.push_back(icdData.sName);
            // 挑选顶层表
            if (icdData.sParentName.empty()) {
                topTables.push_back(icdData.sName);
            }
            TableNode::smtTable table(new TableNode(icdData));
            tableNodes[icdData.sName] = table;
        }
    }
    // 读取规则数据
    DMSpace::svrMap rules;
    if (!q_dbaccess->readTableRules(tables, rules)) {
        return false;
    }
    // 填充规则
    fillRules(tableNodes, rules);

    // 查询关系，将表数据保存到内存中
    // 查询复合表的子表数据，并将之关联起来
    // 根据所属组进行分类，按描述排序
    std::unordered_map<int, TableNode::tableVector> system_table;
    count = topTables.size();
    TableNode::smtTable _table = 0;
    for (int i = 0; i < count; ++i) {
        _table = recursiveLinkTable(tableNodes, topTables.at(i));
        if (_table) {
            const stICDBase &icdBase = _table->icdBase();
            system_table[QString(icdBase.sGroup.c_str())
                    .split("/").last().toInt()].push_back(_table);
            _table->setLengthCheck(_table->length() == (int)_table->realLength()
                                   && _table->length() > 0);
        }
    }
    // 冗余数据
#ifdef _DEBUG_PRINT_D
    if (tableNodes.size() > topTables.size()) {
        count = topTables.size();
        TableNode::tableMap::iterator it;
        for (int i = 0; i < count; ++i) {
            if ((it = tableNodes.find(topTables[i])) != tableNodes.end()) {
                tableNodes.erase(it);
            }
        }
        for (it = tableNodes.begin(); it != tableNodes.end(); ++it) {
            qDebug() << it->first.c_str();
        }
    }
#endif
    // 将数据保存到内存中
    if (increment) {    // 如果是增量，则只更新
        std::unordered_map<int, TableNode::tableVector>::iterator itIS
                = system_table.begin();
        SystemNode::smtSystem sysNode;
        for (; itIS != system_table.end(); ++itIS) {
            // 查询系统表
            if (!(sysNode = data->system(itIS->first))) {
                continue;
            }
            const TableNode::tableVector &items = itIS->second;
            count = items.size();
            for (int i = 0; i < count; ++i) {
                sysNode->addTable(items[i]);
            }
        }
    } else {    // 重新设置数据
        std::unordered_map<int, TableNode::tableVector>::iterator itIS
                = system_table.begin();
        SystemNode::smtSystem sysNode;
        for (; itIS != system_table.end(); ++itIS) {
            // 查询系统表
            if (!(sysNode = data->system(itIS->first))) {
                continue;
            }
            sysNode->setTable(itIS->second);
        }
    }

    return true;
}

/**
* @brief 填充规则数据
* @param [in] tables : 表集
* @param [in] rules : 规则数据集<表名, 规则数据集>
*/
void DataManegement::fillRules(TableNode::tableMap &tables,
                               const DMSpace::svrMap &rules)
{
    ICDMetaData::smtMeta metaData = 0;
    ICDMetaData::ruleMap::iterator itR;
    std::unordered_map<int, ICDFrameCodeData::smtFrameCode> codes;    // 帧识别码
    std::unordered_map<int, ICDFrameCodeData::smtFrameCode>::iterator itC = codes.begin();
    TableNode::tableMap::iterator itTM = tables.end();
    DMSpace::svrMap::const_iterator itSSR = rules.begin();
    for (; itSSR != rules.end(); ++itSSR) {
        // 查询表
        if ((itTM = tables.find(itSSR->first)) != tables.end()) {
            TableNode::smtTable &table = itTM->second;
            if (!table) {
                continue;
            }
            const std::vector<stTableRules> &srRules = itSSR->second;
            ICDMetaData::ruleMap tableRules;
            // 构造表规则数据
            const int count = srRules.size();
            for (int i = 0; i < count; ++i) {
                const stTableRules &ruleData = srRules[i];
                if (metaData = ICDFactory::instance().CreatObject(ruleData)) {
                    tableRules[ruleData.nSerial] = metaData;
                    if (GlobalDefine::dtFrameCode == ruleData.uType) {
                        codes[atoi(ruleData.sDefault.c_str())]
                                = SMT_CONVERT(ICDFrameCodeData, metaData);
                    }
                }
            }
            // 将帧识别码和帧数据建立连接
            for (itC = codes.begin(); itC != codes.end(); ++itC) {
                if ((itR = tableRules.find(itC->first)) != tableRules.end()) {
                    ICDFrameCodeData::smtFrameCode &frameCode
                            = itC->second;
                    ICDComplexData::smtComplex complex
                            = SMT_CONVERT(ICDComplexData, itR->second);
                    if (frameCode && complex) {
                        frameCode->bindData(complex);
                    }
                }
            }
            table->setRule(tableRules);
            codes.clear();
        }
    }
}

/**
* @brief 加载规则数据
* @param [in] tableRules : 表规则信息
* @return 表规则信息<所属组，<表规则>>
*/
std::unordered_map<std::string, TableNode::tableVector> DataManegement::loadXmlRuleData(
        const DMSpace::_vectorIcdTR &tableRules)
{
    //
    std::unordered_map<std::string, TableNode::tableVector> result;
    if (tableRules.empty()) {
        return result;
    }
    ICDMetaData::ruleMap mapRules;
    TableNode::tableMap tables;  // 记录表规则
    std::vector<std::string> topTables;  // 记录顶层表
    // 构造表规则数据
    int count = tableRules.size();
    for (int i = 0; i < count; ++i) {
        const DMSpace::pairIcdTR &icdTR = tableRules[i];
        stICDBase icdBase = icdTR.first;
        const std::vector<stTableRules> &rules = icdTR.second;
        // 没有父表，证明是顶层表
        if (icdBase.sParentName.empty()) {
            topTables.push_back(icdBase.sName);
        }
        TableNode::smtTable tableNode(new TableNode(icdBase));
        mapRules = convert2IcdRules(rules);
        tableNode->setRule(mapRules);
        tables[icdBase.sName] = tableNode;
        mapRules.clear();
    }
    // 查询复合表的子表数据，并将之关联起来
    // 根据所属组进行分类，按描述排序
    count = topTables.size();
    TableNode::smtTable _table = 0;
    for (int i = 0; i < count; ++i) {
        _table = recursiveLinkTable(tables, topTables.at(i));
        if (_table) {
            const stICDBase &icdBase = _table->icdBase();
            _table->setLengthCheck(_table->length() == (int)_table->realLength()
                                   && _table->length() > 0);
            result[icdBase.sGroup].push_back(_table);
        }
    }

    return result;
}

/**
 * @brief 递归建立复合表关系
 * @param [in] allTable : 源表集
 * @param [in] name : 表名
 * @return 子表数据
 */
TableNode::smtTable DataManegement::recursiveLinkTable(TableNode::tableMap &allTable,
                                                       const std::string &name)
{
    TableNode::tableMap::iterator itTM = allTable.find(name);
    if (itTM == allTable.end()) {
        return TableNode::smtTable();
    }
    int count = 0;
    // 复合表
    TableNode::smtTable tableNode = itTM->second;
    if (tableNode->isComplex()) {
        ICDMetaData::ruleMap tableRules = tableNode->allRule();
        ICDMetaData::ruleMap::iterator itTR = tableRules.begin();
        ICDComplexData::smtComplex complex = 0;
        TableNode::smtTable subTable = 0;
        for (; itTR != tableRules.end(); ++itTR) {
            ICDMetaData::smtMeta& metaData = itTR->second;
            // 非复合类数据
            if (NULL == metaData
                    || IcdDefine::icdComplex != metaData->metaType()) {
                continue;
            }
            if (!(complex = SMT_CONVERT(ICDComplexData, metaData))) {
                continue;
            }
            // 可能存在多张子表
            QStringList lstName = QString::fromStdString(complex->rule())
                    .split("@", QString::SkipEmptyParts);
            count = lstName.size();
            // 读取子表数据
            for (int i = 0; i < count; ++i) {
                subTable = recursiveLinkTable(allTable,
                                              lstName[i].toStdString());
                if (NULL != subTable) {
                    // 建立连接
                    complex->addTable(subTable);
                    // 从总表中删除子表
                    if (!subTable->icdBase().sParentName.empty()) {
                        if ((itTM = allTable.find(subTable->key())) != allTable.end())
                            allTable.erase(itTM);
                    }
                }
            }
            // 更新表长度
            if (count > 0) {
                tableNode->updateLength("");
            }
        }
    }

    return tableNode;
}

bool DataManegement::queryDictionary(stQueryDic &data)
{
    //    bool result = false;
    if (data.dic < GlobalDefine::dicDataType
            || data.dic > GlobalDefine::dicCheckType) {
        return false;
    }
    int code = -1;
    stDictionary single;
    switch (data.dicType) {
    case GlobalDefine::dictCode:   // 编码
        CDictionary::instance().convert2Code(dicName(data.dic),
                                             data.condition,
                                             code);
        data.result = QString::number(code).toStdString();
        break;
    case GlobalDefine::dictDec:    // 描述
        CDictionary::instance().convert2Dec(dicName(data.dic),
                                            atoi(data.condition.c_str()),
                                            data.result);
        break;
    case GlobalDefine::dictSign:   // 标识
        single = CDictionary::instance()
                .singleItem(dicName(data.dic), atoi(data.condition.c_str()));
        data.result = single.sSign;
        break;
    case GlobalDefine::dictRemark: // 备注
        single = CDictionary::instance()
                .singleItem(dicName(data.dic), atoi(data.condition.c_str()));
        data.result = single.sRemark;
        break;
    default:
        break;
    }

    return true;
}

// 加载规则数据
bool DataManegement::loadRules(PlaneNode::smtPlane &plane,
                               const QString &condition,
                               bool increment)
{
    bool result = false;
    if (GlobalDefine::dsNone == q_dataSource) {
        return result;
    }

    QStringList items = condition.split("/");
    // 根据数据源的不同来重新读取数据
    if (GlobalDefine::dsDatabase == q_dataSource) {
        // 数据源为数据库
        switch (items.size()) {
        case 1:
            result = loadTypeRule(items.at(0).toInt(), increment);
            break;
        case 2:
            result = loadSystemRule(items.at(0).toInt(), items.at(1).toInt(), increment);
            break;
        case 3:
            result = loadTableRule(items.at(0).toInt(), items.at(1).toInt(), items.at(2).toStdString());
            break;
        default:
            break;
        }
    } else if (GlobalDefine::dsFile == q_dataSource) {
        // 数据源为文件   // 加载文件数据
        Json::Value config;
        config["sourceType"] = "file";
        config["filePath"] = q_defaultPath;
        Icd::ParserPtr parser = Icd::Parser::create(config);
        if (!parser) {
            return false;
        }
        Icd::VehiclePtr vehicle;
        Icd::VehiclePtrArray vehicles;
        // 解析数据
        switch (items.size()) {
        case 1: // 机型
        {
            if (!parser->parse(items.first().toStdString(), vehicle, Icd::ObjectItem)) {
                break;
            }
            break;
        }
        case 2: // 系统
        {
            Icd::SystemPtr system(new Icd::System());
            if (!parser->parse(items[0].toStdString(),
                               items[1].toStdString(), system, Icd::ObjectItem)) {
                break;
            }
            vehicle = Icd::VehiclePtr(new Icd::Vehicle());
            vehicle->setId(items[0].toStdString());
            vehicle->appendSystem(system);
            break;
        }
        case 3: // 表
        {
            Icd::TablePtr table(new Icd::Table());
            if (!parser->parse(items[0].toStdString(),
                               items[1].toStdString(), items[2].toStdString(),
                               table, Icd::ObjectItem)) {
                break;
            }
            Icd::SystemPtr system(new Icd::System());
            system->setId(items[1].toStdString());
            system->appendTable(table);
            vehicle = Icd::VehiclePtr(new Icd::Vehicle());
            vehicle->setId(items[0].toStdString());
            vehicle->appendSystem(system);
            break;
        }
        default:
            break;
        }

        if (!vehicle) {
            return result;
        }

        vehicles.push_back(vehicle);
        // 转换数据
        Json::Value config2(Json::objectValue);
        Icd::SqlParser sqlParser(config2);
        DMSpace::_vectorPS infrastructure;
        DMSpace::_vectorIcdTR tableRules;
        std::vector<int> params;
        params.push_back((int)&infrastructure);
        params.push_back((int)&tableRules);
        if (!sqlParser.convert2Other(vehicles, params)) {
            return result;
        }
        switch (items.size()) {
        case 1:
            result = loadXmlTypeRule(items[0].toInt(), tableRules);
            break;
        case 2:
            result = loadXmlSystemRule(items[0].toInt(), items[1].toInt(), tableRules);
            break;
        case 3:
            result = loadXmlTablemRule(items[0].toInt(), items[1].toInt(), items[2].toStdString(),
                    tableRules);
            break;
        default:
            break;
        }
    }
    if (items.size() > 0) {
        plane = planeNode(items.at(0).toInt());
    }

    return result;
}

// 卸载规则数据
void DataManegement::unLoadRules(const QString &condition)
{
    QStringList items = condition.split("/");
    switch (items.size()) {
    case 1: unLoadTypeRule(items.at(0).toInt()); break;
    case 2: unLoadSystemRule(items.at(0).toInt(), items.at(1).toInt()); break;
    case 3:
        unLloadTableRule(items.at(0).toInt(), items.at(1).toInt(), items.at(2).toStdString());
        break;
    default:
        break;
    }
}

// 修改所有机型数据
bool DataManegement::modifyAllKC(const std::vector<PlaneNode::smtPlane> &planes)
{
    q_planes = planes;
    return true;
}

// 保存机型数据
bool DataManegement::savePlane(const stPlane &plane)
{
    // 保存内存
    PlaneNode::smtPlane data = planeNode(plane.nCode);
    if (data) {
        data->setPlane(plane);
    } else {
        addOneKC(PlaneNode::smtPlane(new PlaneNode(plane)));
    }

    return true;
}

bool DataManegement::deletePlane(const std::vector<int> &keys)
{
    // 删除内存
    const int count = keys.size();
    for (int i = 0; i < count; ++i) {
        deleteOneKC(keys.at(i));
    }

    return true;
}

// 保存系统数据
bool DataManegement::saveSystem(int plane, const std::vector<stSystem>& system)
{
    // 保存内存
    const int count = system.size();
    PlaneNode::smtPlane smtPlane = planeNode(plane);
    if (!smtPlane) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        const stSystem &stData = system.at(i);
        SystemNode::smtSystem data = smtPlane->system(stData.nCode);
        if (data) {
            data->setSystem(stData);
        } else {
            smtPlane->addSystem(SystemNode::smtSystem(new SystemNode(stData)));
        }
    }

    return true;
}

// 修改系统数据
bool DataManegement::modifySystem(int plane, const SystemNode::smtSystem &system)
{
    PlaneNode::smtPlane smtPlane = planeNode(plane);
    if (!smtPlane) {
        return false;
    }
    smtPlane->addSystem(system);

    return true;
}

// 删除系统数据
bool DataManegement::deleteSystem(int plane, const std::vector<int>& system)
{
    // 删除内存
    PlaneNode::smtPlane smtPlane = planeNode(plane);
    if (!smtPlane) {
        return false;
    }
    smtPlane->deleteSystem(system);

    return true;
}

// 保存ICD表数据
bool DataManegement::saveTable(const QString &keys, const std::vector<stICDBase> &tables)
{
    // 保存内存
    const int count = tables.size();
    QStringList keyLst = keys.split("/");
    int planeID = keyLst.first().toInt();
    int systemID = keyLst.last().toInt();
    PlaneNode::smtPlane plane = planeNode(planeID);
    if (!plane) {
        return false;
    }
    SystemNode::smtSystem system = plane->system(systemID);
    if (!system) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        const stICDBase &table = tables[i];
        TableNode::smtTable data = system->table(table.sName);
        if (data) {
            data->setICDBase(table);
        } else {
            system->addTable(TableNode::smtTable(new TableNode(table)));
        }
    }

    return true;
}

// 修改ICD表数据
bool DataManegement::modifyTable(const QString &keys,
                                 const TableNode::smtTable &table)
{
    // 保存内存
    QStringList keyLst = keys.split("/");
    int planeID = keyLst.takeFirst().toInt();
    int systemID = keyLst.takeFirst().toInt();
    std::string rootParent = keyLst.takeFirst().toStdString();
    PlaneNode::smtPlane plane = planeNode(planeID);
    if (!plane) {
        return false;
    }
    SystemNode::smtSystem system = plane->system(systemID);
    if (!system) {
        return false;
    }
    TableNode::smtTable smtTable = system->table(rootParent);
    if (!smtTable) {
        return false;
    }
    if (keyLst.empty()) {   // 顶层表
        system->addTable(table);
    } else {    // 子表
        int id = keyLst.takeLast().toInt();
        if (!keyLst.empty()) {
            if (!(smtTable = smtTable->subTable(keyLst.join("/").toStdString()))) {
                return false;
            }
        }
        ICDComplexData::smtComplex complex
                = std::dynamic_pointer_cast<ICDComplexData>(smtTable->rule(id));
        if (!complex) {
            return false;
        }
        complex->addTable(table);
    }

    return true;
}

// 删除ICD表数据
bool DataManegement::deleteTable(const QString &keys, const std::vector<std::string>& tables)
{
    bool result = false;
    QStringList keyLst = keys.split("/");
    if (keyLst.size() != 2) {
        return result;
    }
    int planeID = keyLst.first().toInt();
    int systemID = keyLst.last().toInt();
    PlaneNode::smtPlane plane = planeNode(planeID);
    SystemNode::smtSystem system;
    std::vector<std::string> deleteTables;
    // 查询子表
    if (!plane) {
        return false;
    }
    if (!(system = plane->system(systemID))) {
        return false;
    }
    // 删除内存
    system->deleteTable(tables);

    return true;
}

// 保存规则数据
int DataManegement::saveRule(const QString &keys, const ICDMetaData::smtMeta &meta)
{
    int result = 0;
    if (!meta) {
        return result;
    }
    QStringList keyLst = keys.split("/");
    if (keyLst.size() < 3) {
        return result;
    }
    int planeID = keyLst.takeFirst().toInt();
    int systemID = keyLst.takeFirst().toInt();
    std::string rootParent = keyLst.takeFirst().toStdString();
    TableNode::smtTable topTable = rootTable(planeID, systemID, rootParent);
    TableNode::smtTable table = topTable;
    if (!table) {
        return result;
    }
    if (keyLst.size() > 0) {    // 子表数据
        if (!(table = table->subTable(keyLst.join("/").toStdString()))) {
            return result;
        }
    }
    // 如果是帧识别码，先绑定帧数据
    ICDFrameCodeData::smtFrameCode frameCode = std::dynamic_pointer_cast<ICDFrameCodeData>(meta);
    if (frameCode) {    // 重置帧数据
        std::string code = frameCode->defaultStr();
        if (code.empty()) {
            frameCode->bindData(0);
        } else {
            ICDComplexData::smtComplex complex = std::dynamic_pointer_cast<ICDComplexData>(
                        table->rule(atoi(code.c_str())));
            frameCode->bindData(complex);
        }
    }
    bool reorder = false;   // 重新整理数据标志
    // 如果修改数据长度，并且该数据不是最后一项数据，会出现字节序号错位的问题，
    // 所以先将原始数据拷贝，从待保存数据序号开始，重新整理字节序号，再保存
    ICDMetaData::smtMeta oldMeta = table->rule(meta->serial());
    if (oldMeta) {  // 修改
        if (meta->serial() != table->allRule().rbegin()->first) {
            // 修改中间数据
            if (meta->byteLength() != oldMeta->byteLength()
                    || meta->length() != oldMeta->length()
                    || meta->metaType() != oldMeta->metaType()) {
                // 数据长度变更，或则数据类型变更
                reorder = true;
            } else {
                ICDBitData::smtBit oldBit = SMT_CONVERT(ICDBitData, oldMeta);
                ICDBitData::smtBit bit = SMT_CONVERT(ICDBitData, meta);
                // 判定是否变更了影响字节序号的属性
                if (oldBit) {
                    if (oldBit->start() != bit->start()) {
                        reorder = true;
                    }
                }
            }
        }
    }
    if (reorder) {
        // 重新整理数据
        ICDMetaData::ruleMap rules = table->allRule();
        ICDMetaData::ruleMap::iterator itR = rules.find(meta->serial());
        int index = 0;
        ICDBitData::smtBit bit = 0;
        ICDMetaData::smtMeta metaPre = meta;
        ICDBitData::smtBit bitPre = SMT_CONVERT(ICDBitData, metaPre);
        // 从目标数据之后开始，调整字节号
        for (++itR; itR != rules.end(); ++itR) {
            ICDMetaData::smtMeta &smtMeta = itR->second;
            if (!smtMeta) {
                continue;
            }
            bit = SMT_CONVERT(ICDBitData, smtMeta);
            // 更新字节序号
            if (bitPre && bit
                    && bitPre->end() < bit->start()) {
                index = bitPre->index() - bitPre->start() / 8 + bit->start() / 8;
            } else {
                if (metaPre) {
                    index = metaPre->index() + metaPre->byteLength();
                }
                if (bit) {
                    index += bit->start() / 8;  // 偏移字节
                }
            }
            smtMeta->setIndex(index);
            metaPre = smtMeta;
            bitPre = bit;
        }
        // 更新目标数据
        table->addRule(meta);
        result = GlobalDefine::special;
    } else {    // 新增或者并未变更数据长度
        // 保存内存
        table->addRule(meta);
        result = GlobalDefine::success;
    }
    // 更新表长度
    if (keyLst.size() > 0) {    // 子表数据
        topTable->updateLength(keyLst.join("/").toStdString());
    }

    return result;
}

// 保存规则数据
bool DataManegement::insertRule(const QString &keys,
                                const ICDMetaData::smtMeta &meta)
{
    if (!meta) {
        return false;
    }
    QStringList keyLst = keys.split("/");
    if (keyLst.size() < 3) {
        return false;
    }
    int planeID = keyLst.takeFirst().toInt();
    int systemID = keyLst.takeFirst().toInt();
    std::string rootParent = keyLst.takeFirst().toStdString();
    TableNode::smtTable topTable = rootTable(planeID, systemID, rootParent);
    TableNode::smtTable table = topTable;
    if (!table) {
        return false;
    }
    if (keyLst.size() > 0) {    // 子表数据
        table = table->subTable(keyLst.join("/").toStdString());
        if (!table) {
            return false;
        }
    }
    // 如果是帧识别码，先绑定帧数据
    ICDFrameCodeData::smtFrameCode frameCode = SMT_CONVERT(ICDFrameCodeData, meta);
    if (frameCode) {    // 重置帧数据
        std::string code = frameCode->defaultStr();
        if (code.empty()) {
            frameCode->bindData(0);
        } else {
            ICDComplexData::smtComplex complex
                    = std::dynamic_pointer_cast<ICDComplexData>(
                        table->rule(atoi(code.c_str())));
            frameCode->bindData(complex);
        }
    }
    // 插入数据会出现字节序号错位的问题，所以从插入位置开始，
    // 重新整理字节序号，再保存
    ICDMetaData::ruleMap rules = table->allRule();
    // 记录新数据
    ICDMetaData::smtMeta newData = meta;
    ICDMetaData::smtMeta oldData = 0;
    ICDMetaData::ruleMap::iterator itR = rules.find(meta->serial());
    if (itR == rules.end()) {
        return false;
    }
    // 查询是否是bit合并数据
    if (!(oldData = itR->second)) {
        return false;
    }
    int index = 0;
    ICDBitData::smtBit bit = 0;
    ICDBitData::smtBit bitPre = SMT_CONVERT(ICDBitData, newData);
    // 从目标数据开始，调整字节序号
    for (; itR != rules.end(); ++itR) {
        if (!(oldData = itR->second)) {
            continue;
        }
        bit = SMT_CONVERT(ICDBitData, oldData);
        if (bitPre && bit
                && bitPre->end() < bit->start()) {
            // 当前项能够向前一项合并，不增加偏移量
            index = bitPre->index() - bitPre->start() / 8 + bit->start() / 8;
        } else {
            index = newData->index() + newData->byteLength();
            if (bit) {
                index += bit->start() / 8;  // 偏移字节
            }
        }
        // 更新字节序号
        oldData->setIndex(index);
        // 更新顺序号
        oldData->setSerial(oldData->serial() + 1);
        // 更新map
        itR->second = newData;
        newData = oldData;
        bitPre = bit;
    }
    // 保存原始的最后一项数据
    rules[newData->serial()] = newData;
    // 保存内存
    table->setRule(rules);
    // 更新表长度
    if (keyLst.size() > 0) {    // 子表数据
        topTable->updateLength(keyLst.join("/").toStdString());
    }

    return true;
}

// 删除规则数据
bool DataManegement::deleteRule(const QString &keys,
                                const std::vector<int> &rules)
{
    if (rules.empty()) {
        return true;
    }
    QStringList keyLst = keys.split("/");
    if (keyLst.size() < 3) {
        return false;
    }
    int planeID = keyLst.takeFirst().toInt();
    int systemID = keyLst.takeFirst().toInt();
    std::string rootParent = keyLst.takeFirst().toStdString();
    // 由于删除数据后，会出现字节序号不连续的问题，所以先将原始数据拷贝，
    // 删除目标数据后，重新整理字节序号，再保存
    TableNode::smtTable topTable = rootTable(planeID, systemID, rootParent);
    TableNode::smtTable table = topTable;
    if (!table) {
        return false;
    }
    if (keyLst.size() > 0) {    // 子表数据
        if (!(table = table->subTable(keyLst.join("/").toStdString()))) {
            return false;
        }
    }
    // 删除规则
    table->deleteRule(rules);
    // 批量删除或者删除的数据不是最后一条数据时，进行重新整理
    if (rules.size() > 1
            || (!table->isEmpty()
                && table->allRule().rbegin()->first != rules[0])) {
        ICDMetaData::ruleMap newRules;
        ICDMetaData::ruleMap tableRules = table->allRule();
        ICDMetaData::ruleMap::iterator itR = tableRules.begin();
        int offset = 0;
        ICDBitData::smtBit bit = 0;
        ICDBitData::smtBit bitPre = 0;
        for (int i = 1; itR != tableRules.end(); ++itR) {
            ICDMetaData::smtMeta meta = itR->second;
            if (!meta) {
                continue;
            }
            bit = SMT_CONVERT(ICDBitData, meta);
            if (bitPre && bit
                    && bitPre->end() < bit->start()) {
                // 当前项能够向前一项合并，不增加偏移量
                offset += bit->end() / 8 - bitPre->end() / 8;
            } else {
                offset += meta->byteLength();
            }
            meta->setSerial(i);
            meta->setIndex(offset - meta->byteLength());
            newRules[i++] = meta;
            bitPre = bit;
        }
        table->setRule(newRules);
    }
    // 更新表长度
    if (keyLst.size() > 0) {    // 子表数据
        topTable->updateLength(keyLst.join("/").toStdString());
    }

    return true;
}

// 删除帧识别码子表
bool DataManegement::deleteSubTable(const QString &keys,
                                    const std::vector<std::string> &subTables)
{
    QStringList keyLst = keys.split("/");
    if (keyLst.size() < 4) {
        return false;
    }
    int planeID = keyLst.takeFirst().toInt();
    int systemID = keyLst.takeFirst().toInt();
    int serial = keyLst.takeLast().toInt();
    std::string rootParent = keyLst.takeFirst().toStdString();
    // 查询子表
    TableNode::smtTable topTable = rootTable(planeID, systemID, rootParent);
    TableNode::smtTable table = topTable;
    if (!table) {
        return false;
    }
    if (keyLst.size() > 0) {    // 子表数据
        if (!(table = table->subTable(keyLst.join("/").toStdString()))) {
            return false;
        }
    }
    // 查询帧数据
    ICDComplexData::smtComplex complex
            = std::dynamic_pointer_cast<ICDComplexData>(table->rule(serial));
    if (!complex) {
        return false;
    }
    // 删除内存
    complex->deleteTable(subTables);
    // 更新表长度
    if (keyLst.size() > 0) {    // 子表数据
        topTable->updateLength(keyLst.join("/").toStdString());
    }

    return true;
}

// 查询内存数据
JLRESULT DataManegement::querySingleElement(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    ICDElement::smtElement *element = jVariantFromVoid<ICDElement::smtElement>(args[0]);
    if (!element) {
        return -1;
    }
    const QString *keys = jVariantFromVoid<QString>(args[1]);
    if (!keys) {
        return -1;
    }
    QStringList keyList = keys->split("/", QString::SkipEmptyParts);
    if (keyList.empty()) {
        return -1;
    } else if (1 == keyList.size()) {   // 机型
        *element = planeNode(keyList.first().toInt());
    } else if (2 == keyList.size()) {   // 系统
        PlaneNode::smtPlane plane = planeNode(keyList.takeFirst().toInt());
        if (!plane) {
            return -1;
        }
        *element = plane->system(keyList.takeFirst().toInt());
    } else if (3 == keyList.size()) {  // 表
        PlaneNode::smtPlane plane = planeNode(keyList.takeFirst().toInt());
        if (!plane) {
            return -1;
        }
        SystemNode::smtSystem system = plane->system(keyList.takeFirst().toInt());
        if (!system) {
            return -1;;
        }
        *element = system->table(keyList.takeFirst().toStdString());
    } else {    // 规则
        PlaneNode::smtPlane plane = planeNode(keyList.takeFirst().toInt());
        if (!plane) {
            return -1;;
        }
        SystemNode::smtSystem system = plane->system(keyList.takeFirst().toInt());
        if (!system) {
            return -1;;
        }
        TableNode::smtTable table = system->table(keyList.takeFirst().toStdString());
        if (!table) {
            return -1;;
        }
        QString lastItem = keyList.last();
        if (lastItem.contains("ICDTable_")) {
            // 子表
            *element = table->subTable(keyList.join("/").toStdString());
        } else {
            // 规则数据
            keyList.takeLast();
            if (!keyList.isEmpty()) {
                table = table->subTable(keyList.join("/").toStdString());
                if (!table) {
                    return -1;;
                }
            }
            // 规则数据
            *element = table->rule(lastItem.toInt());
        }
    }

    return 0;
}

// 查询加载数据源
JLRESULT DataManegement::queryDataSource(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 1) {
        return -1;
    }
    int *result = jVariantFromVoid<int>(args[0]);

    *result = int(q_dataSource);

    return 0;
}

// 查询子表上限
JLRESULT DataManegement::queryMaxTableCount(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 1) {
        return -1;
    }
    int *result = jVariantFromVoid<int>(args[0]);

    *result = q_dbaccess->tableLimitation();

    return 0;
}

// 保存拷贝数据
JLRESULT DataManegement::saveCopyData(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    QVector<int> *params = jVariantFromVoid<QVector<int> >(args[0]);
    if (!params || params->size() < 3) {
        return -1;
    }
    ICDElement::smtElement *result = jVariantFromVoid<ICDElement::smtElement>(args[1]);
    if (!result) {
        return -1;;
    }
    // 查询源数据
    ICDElement::smtElement element;
    QVariantList newArgs;
    newArgs.append(qVariantFromValue((void*)&element));
    newArgs.append(params->at(0));
    Icd::JNEvent newEvent("edit.querySingleElement", newArgs);
    querySingleElement(newEvent);
    if (!element) {
        return -1;
    }
    QString lastItem;
    TableNode::smtTable topTable = 0;
    ICDElement::smtElement desElement;
    QString dest = *reinterpret_cast<QString *>(params->at(2));
    QStringList lst = dest.split("/", QString::SkipEmptyParts);
    std::string subKey;
    if (lst.size() > 2) {
        int planeId = lst.takeFirst().toInt();
        int systemId = lst.takeFirst().toInt();
        topTable = rootTable(planeId, systemId, lst.takeFirst().toStdString());
        if (!lst.empty()) {
            subKey = lst.join("/").toStdString();
        }
        // 源数据为规则类型时，有可能是子表，也有可能是规则
        lastItem = dest.mid(dest.lastIndexOf("/") + strlen("/"));
        if (lastItem.contains("ICDTable_")) { // 子表
            lastItem.clear();
        } else {    // 规则
            dest.truncate(dest.lastIndexOf("/"));
        }
    }
    if (!dest.isEmpty()) {
        newArgs.clear();
        newArgs.append(qVariantFromValue((void*)&desElement));
        newArgs.append(qVariantFromValue((void*)&dest));
        Icd::JNEvent newEvent("edit.querySingleElement", newArgs);
        querySingleElement(newEvent);
        if (!desElement) {
            return -1;;
        }
    }
    // 更新数据
    if (GlobalDefine::ntPlane == element->objectType()) {
        QString id;
        // 机型
        const stPlane &base = *reinterpret_cast<stPlane *>(params->at(1));
        id = jnotify->send("edit.queryId", int(GlobalDefine::ntPlane)).toString();
        // 克隆数据
        PlaneNode::smtPlane plane = SMT_CONVERT(PlaneNode, element)->clone();
        if (!plane) {
            return -1;;
        }
        plane->setPlane(base);
        copyPlaneData(plane, id.toInt());
        addOneKC(plane);
        *result = plane;
    } else if (GlobalDefine::ntSystem == element->objectType()) {
        QString id = jnotify->send("edit.queryId", int(GlobalDefine::ntSystem)).toString();
        // 系统
        const stSystem &base = *reinterpret_cast<stSystem *>(params->at(1));
        SystemNode::smtSystem system = SMT_CONVERT(SystemNode, element)->clone();
        PlaneNode::smtPlane plane = SMT_CONVERT(PlaneNode, desElement);
        if (!system || !plane) {
            return -1;;
        }
        stICDBase icdBase;
        icdBase.sGroup = dest.toStdString();
        system->setSystem(base);
        copySystemData(system, plane->numeralId(), id.toInt());
        plane->addSystem(system);
        *result = system;
    } else if (GlobalDefine::ntTable == element->objectType()) {
        // 表
        const stICDBase &base = *reinterpret_cast<stICDBase *>(params->at(1));
        TableNode::smtTable table = SMT_CONVERT(TableNode, element)->clone();
        if (!table) {
            return -1;;
        }
        if (lastItem.isEmpty()) {   // 顶层表
            SystemNode::smtSystem system = SMT_CONVERT(SystemNode, desElement);
            if (!system) {
                return -1;;
            }
            stICDBase icdBase;
            icdBase.sDescribe = base.sDescribe;
            icdBase.sCode = base.sCode;
            icdBase.nLength = base.nLength;
            icdBase.sGroup
                    = dest.mid(dest.indexOf("/ICDTable_")).toStdString();
            icdBase.sRemark = base.sRemark;
            table->setICDBase(icdBase);
            copyTableData(table, icdBase);
            system->addTable(table);
        } else {    // 子表
            TableNode::smtTable destTable = SMT_CONVERT(TableNode, desElement);
            if (!destTable) {
                return -1;;
            }
            ICDComplexData::smtComplex complex
                    = SMT_CONVERT(ICDComplexData, destTable->rule(lastItem.toInt()));
            if (!complex) {
                return -1;;
            }
            stICDBase icdBase = destTable->icdBase();
            icdBase.sDescribe = base.sDescribe;
            icdBase.sCode = base.sCode;
            icdBase.sRemark = base.sRemark;
            table->setICDBase(icdBase);
            copyTableData(table, icdBase);
            complex->addTable(table);
            // 更新表长度
            if (topTable) {
                topTable->updateLength(subKey);
            }
        }
        *result = table;
    } else if (GlobalDefine::ntRule == element->objectType()) {
        // 规则
        ICDCommonData::smtCommon data =
                SMT_CONVERT(ICDCommonData, *reinterpret_cast<ICDMetaData::smtMeta *>(params->at(1)));
        ICDMetaData::smtMeta meta = SMT_CONVERT(ICDMetaData, element);
        TableNode::smtTable table = SMT_CONVERT(TableNode, desElement);
        if (!data || !meta || !table) {
            return -1;;
        }
        ICDCommonData::smtCommon common;
        // 如果用户改变数据类型，则使用界面数据，否则使用源数据
        if (data->type() == meta->type()) {
            if (!(common = SMT_CONVERT(ICDCommonData, meta->clone()))) {
                return -1;;
            }
            ICDFactory::instance().copyBaseData(data, common);
            copyRuleData(SMT_CONVERT(ICDMetaData, common), table->icdBase());
        } else {
            common = SMT_CONVERT(ICDCommonData, data->clone());
        }
        table->addRule(common);
        *result = common;
        // 更新表长度
        if (topTable) {
            topTable->updateLength(subKey);
        }
    }

    return 0;
}

// 将内存数据保存到数据库
JLRESULT DataManegement::saveDatabase(const Icd::JNEvent &event)
{
    // 查询数据源，如果是文件，则整理到表一级，
    // 否则，整理到保存层级（删除内存中不存在的数据）
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;;
    }
    QString *keys = jVariantFromVoid<QString>(args[0]);
    if (!keys) {
        return -1;
    }
    QString *err = jVariantFromVoid<QString>(args[1]);
    if (!err) {
        return -1;
    }

    if (GlobalDefine::dsNone == q_dataSource) {
        return 0;
    }

    DMSpace::_vectorPS plane_system;
    DMSpace::_vectorIcdTR tableRules;
    QStringList keyList = keys->split("/", QString::SkipEmptyParts);
    int dataLevel = GlobalDefine::ntUnknown;

    if (0 == keyList.size()) {
    } else if (1 == keyList.size()) {  // 保存机型
        dataLevel = GlobalDefine::ntPlane;
    } else if (2 == keyList.size()) {   // 保存系统
        dataLevel = GlobalDefine::ntSystem;
    } else {    // 保存表
        dataLevel = GlobalDefine::ntTable;
    }
    // 构造数据
    if (!generateSavedData(keyList, plane_system, tableRules, *err)) {
        return -1;;
    }

    if (!q_dbaccess->isOpen()) {
        const Json::Value option = JMain::instance()->option("edit", "parser");
        if (!option.isNull()) {
            q_dbaccess->init(option);
        }
    }

    if (!q_dbaccess->savePlane(plane_system,
                               tableRules,
                               dataLevel,
                               GlobalDefine::dsDatabase == q_dataSource)) {
        *err = q_dbaccess->lastError().c_str();
        QString error = QStringLiteral("保存数据库失败！[原因]：%1").arg(*err);
        qDebug() << error;
    }

    return 0;
}

// 将内存数据保存到xml文件
JLRESULT DataManegement::saveFile(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;;
    }
    QVector<QString> *params = jVariantFromVoid<QVector<QString> >(args[0]);
    if (!params || params->size() != 2) {
        return -1;;
    }
    const QString &keys = params->at(0);
    const QString &file = params->at(1);
    QString *err = jVariantFromVoid<QString>(args[1]);
    if (!err) {
        return -1;
    }
    DMSpace::_vectorPS plane_system;
    DMSpace::_vectorIcdTR tableRules;
    QStringList keyList = keys.split("/", QString::SkipEmptyParts);

    // 构造数据
    if (!generateSavedData(keyList, plane_system, tableRules, *err)) {
        if (err->isEmpty()) {
            *err = QStringLiteral("解析内存数据失败！");
        }
        return -1;;
    }

    std::vector<int> datas;
    Icd::VehiclePtrArray vehicles;
    Json::Value configSql(Json::objectValue);
    Icd::SqlParser sqlParser(configSql);
    // 转换数据
    datas.push_back((int)&plane_system);
    datas.push_back((int)&tableRules);
    if (!sqlParser.convert2Core(datas, vehicles)) {
        *err = QStringLiteral("解析内存数据失败！");
        return -1;;
    }
    // 加载文件数据
    Json::Value config;
    config["sourceType"] = "file";
    config["filePath"] = file.toStdString();
    Icd::ParserPtr parser = Icd::Parser::create(config);
    if (!parser) {
        return -1;
    }
    // 保存数据
    if (!parser->save(vehicles)) {
        *err = QStringLiteral("文件写入失败！");
    }

    return 0;
}

// 更新数据库配置信息
JLRESULT DataManegement::dataSourceChanged(const Icd::JNEvent &event)
{
    Q_UNUSED(event);
    const Json::Value option = JMain::instance()->option("edit", "parser");
    if (!option.isObject()) {
        return -1;
    }

    const std::string sourceType = option["sourceType"].asString();

    if (sourceType == "sql") {
        q_dataSource = GlobalDefine::dsDatabase;
        // 重新初始化数据库
        if (!q_dbaccess) {
            q_dbaccess = new DBAccess();
        }
        if (!q_dbaccess->init(option)) {
            QMessageBox::warning(parentWidget_, QStringLiteral("数据库错误"),
                                 q_dbaccess->lastError().c_str());
            return -1;
        }
        // 如果数据源为数据库，加载基础数据
        loadBaseData();
    } else if (sourceType == "file") {
        q_dataSource = GlobalDefine::dsFile;
        q_defaultPath = option["filePath"].asString();
#if 0
        QString error;
        QVariantList newArgs;
        newArgs.append(GlobalDefine::dsFile);
        newArgs.append(qVariantFromValue((void*)&error));
        newArgs.append(int(Icd::ObjectTable));
        Icd::JNEvent newEvent("edit.loadInfrastructure", newArgs);
        return notifyRespond(newEvent);
#else
        jnotify->send("edit.tree.loadVehicle");
#endif
    } else {
        q_dataSource = GlobalDefine::dsNone;
    }

    return 0;
}

// 查询数据库错误信息
JLRESULT DataManegement::queryDBError(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.isEmpty()) {
        return -1;;
    }
    QString *error = jVariantFromVoid<QString>(args[0]);
    if (!error) {
        return -1;
    }
    *error = q_dbaccess->lastError().c_str();

    return 0;
}

// 查询顶层表
TableNode::smtTable DataManegement::rootTable(int plane, int system, const std::string &table)
{
    TableNode::smtTable result;
    PlaneNode::smtPlane smtPlane = planeNode(plane);
    if (!smtPlane) {
        return result;
    }
    SystemNode::smtSystem smtSystem = smtPlane->system(system);
    if (!smtSystem) {
        return result;
    }

    return (result = smtSystem->table(table));
}

// 查询字典名
std::string DataManegement::dicName(GlobalDefine::DictionaryIndex dic) const
{
    std::string dicName;
    switch (dic) {
    case GlobalDefine::dicDataType:    // 数据类型
        dicName = DBAccess::tableName(DataBaseDefine::dbDataType);
        break;
    case GlobalDefine::dicPowerType:   // 权限类型
        dicName = DBAccess::tableName(DataBaseDefine::dbPowerType);
        break;
    case GlobalDefine::dicArrayType:   // 数组类型
        dicName = DBAccess::tableName(DataBaseDefine::dbArrayType);
        break;
    case GlobalDefine::dicCheckType:   // 校验类型
        dicName = DBAccess::tableName(DataBaseDefine::dbCheckType);
        break;
    default:break;
    }

    return dicName;
}

// 构造需要保存的数据
bool DataManegement::generateSavedData(QStringList &keyList,
                                       DMSpace::_vectorPS &plane_system,
                                       DMSpace::_vectorIcdTR &tableRules,
                                       QString &err)
{
    TableNode::tableVector tables;
    if (0 == keyList.size()) {
        if (!generateAll(plane_system, tables, err)) {
            return false;
        }
    } else if (1 == keyList.size()) {  // 保存机型
        if (!generatePlane(keyList.first().toInt(),
                           plane_system, tables, err)) {
            return false;
        }
    } else if (2 == keyList.size()) {   // 保存系统
        int planeID = keyList.first().toInt();
        int systemID = keyList.last().toInt();
        if (!generateSystem(planeID, systemID,
                            plane_system, tables, err)) {
            return false;
        }
    } else {    // 保存表
        int planeID = keyList.takeFirst().toInt();
        int systemID = keyList.takeFirst().toInt();
        std::string rootParent = keyList.takeFirst().toStdString();
        if (!generateTable(planeID, systemID, rootParent,
                           keyList.join("/").toStdString(),
                           plane_system, tables, err)) {
            return false;
        }
    }
    tableRules = convert2TableRules(tables);

    return true;
}

bool DataManegement::generateAll(DMSpace::_vectorPS &plane_system,
                                 TableNode::tableVector &tables,
                                 QString &err)
{
    PlaneNode::smtPlane smtPlane = 0;
    SystemNode::smtSystem system = 0;
    SystemNode::systemVector systemVector;
    std::vector<stSystem> systems;
    TableNode::tableVector tableVector;
    TableNode::tableVector subTables;
    TableNode::smtTable table = 0;
    const int count = q_planes.size();
    std::string nameTip;
    for (int i = 0; i < count; ++i) {
        if (!(smtPlane = q_planes[i])) {
            continue;
        }
        systemVector = smtPlane->allSystem();
        const int bound = systemVector.size();
        for (int j = 0; j < bound; ++j) {
            // 系统
            if (!(system = systemVector[j])) {
                continue;
            }
            systems.push_back(system->system());
            tableVector = system->allTable();
            // 表
            const int size = tableVector.size();
            for (int k = 0; k < size; ++k) {
                if (!(table = tableVector[k])) {
                    continue;
                }
                if (table->isEmpty()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                    err = QStringLiteral("不允许存在空表<%1>，请完善后重试！").arg(table->name().c_str());
                    return false;
#endif
                }
                if (table->lengthCheck()
                        && table->length() != (int)table->realLength()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                    err = QStringLiteral("表<%1>数据项定义不完整，请完善后重试！").arg(table->name().c_str());
                    return false;
#endif
                }
                if (table->hasEmptySubTable(nameTip)) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                    err = QStringLiteral("表<%1>下存在空子表，请完善后重试！").arg(nameTip.c_str());
                    return false;
#endif
                }
                tables.push_back(table);
                subTables = table->subTables();
                if (!subTables.empty()) {
                    const int number = subTables.size();
                    for (int l = 0; l < number; l++) {
                        if (!(table = subTables[l])) {
                            continue;
                        }
                        tables.push_back(table);
                    }
                }
            }
        }
        plane_system.push_back(std::make_pair(smtPlane->plane(), systems));
        systems.clear();
    }

    return true;
}

bool DataManegement::generatePlane(int planeId,
                                   DMSpace::_vectorPS &plane_system,
                                   TableNode::tableVector &tables,
                                   QString &err)
{
    PlaneNode::smtPlane smtPlane = planeNode(planeId);
    if (!smtPlane) {
        return false;
    }
    std::vector<stSystem> systems;
    TableNode::tableVector tableVector;
    TableNode::tableVector subTables;
    SystemNode::smtSystem system = 0;
    SystemNode::systemVector systemVector = smtPlane->allSystem();
    TableNode::smtTable table = 0;
    const int bound = systemVector.size();
    std::string nameTip;
    for (int i = 0; i < bound; ++i) {
        // 系统
        if (!(system = systemVector[i])) {
            continue;
        }
        systems.push_back(system->system());
        tableVector = system->allTable();
        // 表
        const int count = tableVector.size();
        for (int j = 0; j < count; ++j) {
            if (!(table = tableVector[j])) {
                continue;
            }
            if (table->isEmpty()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                err = QStringLiteral("不允许存在空表<%1>，请完善后重试！").arg(table->name().c_str());
                return false;
#endif
            }
            if (table->lengthCheck() && table->length() != (int)table->realLength()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                err = QStringLiteral("表<%1>数据项定义不完整，请完善后重试！").arg(table->name().c_str());
                return false;
#endif
            }
            if (table->hasEmptySubTable(nameTip)) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                err = QStringLiteral("表<%1>下存在空子表，请完善后重试！").arg(nameTip.c_str());
                return false;
#endif
            }
            tables.push_back(table);
            subTables = table->subTables();
            if (!subTables.empty()) {
                const int number = subTables.size();
                for (int l = 0; l < number; l++) {
                    if (!(table = subTables[l])) {
                        continue;
                    }
                    tables.push_back(table);
                }
            }
        }
    }
    plane_system.push_back(std::make_pair(smtPlane->plane(), systems));

    return true;
}

bool DataManegement::generateSystem(int planeId, int systemId, DMSpace::_vectorPS &plane_system,
                                    TableNode::tableVector &tables, QString &err)
{
    PlaneNode::smtPlane smtPlane = planeNode(planeId);
    if (!smtPlane) {
        return false;
    }
    SystemNode::smtSystem smtSystem = smtPlane->system(systemId);
    if (!smtSystem) {
        return false;
    }
    std::vector<stSystem> systems;
    TableNode::tableVector subTables;
    TableNode::tableVector tableVector = smtSystem->allTable();
    systems.push_back(smtSystem->system());
    // 表
    TableNode::smtTable table = 0;
    const int count = tableVector.size();
    std::string nameTip;
    for (int i = 0; i < count; ++i) {
        if (!(table = tableVector[i])) {
            continue;
        }
        if (table->isEmpty()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
            err = QStringLiteral("不允许存在空表<%1>，请完善后重试！").arg(table->name().c_str());
            return false;
#endif
        }
        if (table->lengthCheck() && table->length() != (int)table->realLength()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
            err = QStringLiteral("表<%1>数据项定义不完整，请完善后重试！").arg(table->name().c_str());
            return false;
#endif
        }
        if (table->hasEmptySubTable(nameTip)) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
            err = QStringLiteral("表<%1>下存在空子表，请完善后重试！").arg(nameTip.c_str());
            return false;
#endif
        }
        tables.push_back(table);
        subTables = table->subTables();
        if (!subTables.empty()) {
            const int number = subTables.size();
            for (int l = 0; l < number; l++) {
                if (!(table = subTables[l])) {
                    continue;
                }
                tables.push_back(table);
            }
        }
    }
    plane_system.push_back(std::make_pair(smtPlane->plane(), systems));

    return true;
}

bool DataManegement::generateTable(int planeId,
                                   int systemId,
                                   const std::string &tableId,
                                   const std::string &subId,
                                   DMSpace::_vectorPS &plane_system,
                                   TableNode::tableVector &tables,
                                   QString &err)
{
    PlaneNode::smtPlane smtPlane = planeNode(planeId);
    if (!smtPlane) {
        return false;
    }
    SystemNode::smtSystem smtSystem = smtPlane->system(systemId);
    if (!smtSystem) {
        return false;
    }
    TableNode::smtTable table = smtSystem->table(tableId);
    if (!table) {
        return false;
    }
    std::string nameTip;
    if (table->isEmpty()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
        err = QStringLiteral("不允许存在空表<%1>，请完善后重试！")
                .arg(table->name().c_str());
        return false;
#endif
    }
    if (table->lengthCheck()
            && table->length() != (int)table->realLength()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
        err = QStringLiteral("表<%1>数据项定义不完整，请完善后重试！").arg(table->name().c_str());
        return false;
#endif
    }
    if (table->hasEmptySubTable(nameTip)) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
        err = QStringLiteral("表<%1>下存在空子表，请完善后重试！").arg(nameTip.c_str());
        return false;
#endif
    }
    if (!subId.empty()) {    // 子表数据
        if (!(table = table->subTable(subId))) {
            return false;
        }
    }
    std::vector<stSystem> systems;
    tables.push_back(table);
    systems.push_back(smtSystem->system());
    TableNode::tableVector subTables = table->subTables();
    const int number = subTables.size();
    for (int l = 0; l < number; ++l) {
        if (!(table = subTables[l])) {
            continue;
        }
        tables.push_back(table);
    }
    plane_system.push_back(std::make_pair(smtPlane->plane(), systems));

    return true;
}

// 将规则内存数据转换成通用结构
DMSpace::_vectorIcdTR DataManegement::convert2TableRules(
        const std::vector<TableNode::smtTable> &tables) const
{
    std::vector<stTableRules> rules;
    stTableRules singleRule;
    TableNode::smtTable table;
    DMSpace::_vectorIcdTR tableRules;
    ICDMetaData::ruleMap icdRules;
    ICDMetaData::ruleMap::iterator it = icdRules.end();
    const int count = tables.size();
    for (int i = 0; i < count; ++i) {
        if (table = tables.at(i)) {
            rules.clear();
            icdRules = table->allRule();
            for (it = icdRules.begin(); it != icdRules.end(); ++it) {
                singleRule = ICDFactory::instance().convert2Rule(it->second);
                rules.push_back(singleRule);
            }
            tableRules.push_back(std::make_pair(table->icdBase(), rules));
        }
    }

    return tableRules;
}

// 将通用结构转换成规则内存数据
ICDMetaData::ruleMap DataManegement::convert2IcdRules(
        const std::vector<stTableRules> &rules)
{
    QMap<int, ICDFrameCodeData::smtFrameCode> codes;    // 帧识别码
    ICDMetaData::ruleMap result;
    const int count = rules.size();
    for (int i = 0; i < count; ++i) {
        const stTableRules &ruleData = rules.at(i);
        ICDMetaData::smtMeta metaData = ICDFactory::instance().CreatObject(ruleData);
        if (NULL != metaData) {
            result[ruleData.nSerial] = metaData;
            if (GlobalDefine::dtFrameCode == ruleData.uType) {
                codes[atoi(ruleData.sDefault.c_str())] = SMT_CONVERT(ICDFrameCodeData, metaData);
            }
        }
    }
    // 将帧识别码和帧数据建立连接
    QMapIterator<int, ICDFrameCodeData::smtFrameCode> itCode = codes;
    ICDMetaData::ruleMap::iterator it = result.end();
    while (itCode.hasNext()) {
        itCode.next();
        if ((it = result.find(itCode.key())) != result.end()) {
            ICDFrameCodeData::smtFrameCode frameCode = itCode.value();
            ICDComplexData::smtComplex complex = SMT_CONVERT(ICDComplexData, it->second);
            if (frameCode && complex) {
                frameCode->bindData(complex);
            }
        }
    }

    return result;
}

// 拷贝机型数据（重新定义所有规则表标识）
bool DataManegement::copyPlaneData(PlaneNode::smtPlane &plane, int planeId)
{
    if (!plane) {
        return false;
    }
    plane->setNumeralId(planeId);
    SystemNode::systemVector systems = plane->allSystem();
    const int count = systems.size();
    SystemNode::smtSystem system = 0;
    for (int i = 0; i < count; ++i) {
        if (!(system = systems[i])) {
            continue;
        }
        if (!copySystemData(system, planeId, system->numeralId())) {
            return false;
        }
    }

    return true;
}

// 拷贝系统数据（重新定义所有规则表标识）
bool DataManegement::copySystemData(SystemNode::smtSystem &system,
                                    int planeId, int systemId)
{
    if (!system) {
        return false;
    }
    system->setNumeralId(systemId);
    stICDBase icdBase;
    icdBase.sGroup = QString("%1/%2").arg(planeId).arg(systemId).toStdString();
    TableNode::smtTable table = 0;
    TableNode::tableVector newTable;
    TableNode::tableVector tables = system->allTable();
    const int count = tables.size();
    for (int i = 0; i < count; ++i) {
        if (!(table = tables[i])) {
            continue;
        }
        if (!copyTableData(table, icdBase)) {
            return false;
        }
        newTable.push_back(table);
    }
    system->setTable(newTable);

    return true;
}

// 拷贝表数据（重新定义子规则表标识）
bool DataManegement::copyTableData(TableNode::smtTable &table, const stICDBase &icdBase)
{
    if (!table) {
        return false;
    }

    stICDBase base = table->icdBase();
    base.sID = QUuid::createUuid().toString().remove(QRegExp("[{}-]")).toStdString();
    base.sName = std::string("ICDTable_").append(base.sID);
    base.sParentName = icdBase.sName;
    base.sGroup = icdBase.sGroup;
    table->setICDBase(base);
    ICDMetaData::ruleMap rules = table->allRule();
    ICDMetaData::ruleMap::iterator it = rules.begin();
    std::unordered_map<int, ICDFrameCodeData::smtFrameCode> codes;    // 帧识别码
    for (; it != rules.end(); ++it) {
        ICDCommonData::smtCommon common = SMT_CONVERT(ICDCommonData, it->second);
        if (common) {
            if (GlobalDefine::dtFrameCode == common->type()) {
                codes[atoi(common->defaultStr().c_str())] = SMT_CONVERT(ICDFrameCodeData, common);
            }
        }
        if (!copyRuleData(it->second, base)) {
            return false;
        }
    }
    // 将帧识别码和帧数据建立连接
    std::unordered_map<int, ICDFrameCodeData::smtFrameCode>::iterator itC;
    for (itC = codes.begin(); itC != codes.end(); ++itC) {
        ICDFrameCodeData::smtFrameCode &frameCode = itC->second;
        ICDComplexData::smtComplex complex = SMT_CONVERT(ICDComplexData, table->rule(itC->first));
        if (frameCode && complex) {
            frameCode->bindData(complex);
        }
    }

    return true;
}

// 拷贝规则数据（重新定义子规则表标识）
bool DataManegement::copyRuleData(ICDMetaData::smtMeta &meta, const stICDBase &icdBase)
{
    if (!meta) {
        return false;
    }
    if (meta->metaType() != IcdDefine::icdComplex) {
        return true;
    }
    ICDComplexData::smtComplex complex = SMT_CONVERT(ICDComplexData, meta);
    if (!complex) {
        return false;
    }
    TableNode::tableVector newTables;
    TableNode::tableVector tables = complex->allTable();
    const int count = tables.size();
    TableNode::smtTable table = 0;
    for (int i = 0; i < count; ++i) {
        if (!(table = tables[i])) {
            continue;
        }
        if (!copyTableData(table, icdBase)) {
            return false;
        }
        newTables.push_back(table);
    }
    complex->setTable(newTables);

    return true;
}

// 加载xml文件信息
std::string DataManegement::readXmlFile()
{
    const Json::Value option = JMain::instance()->option("edit", "parser");
    if (option.isNull()) {
        return std::string();
    }

    std::string sourceName = option["sourceType"].asString();
    if ("file" == sourceName) {
        q_dataSource = GlobalDefine::dsFile;
        return option["filePath"].asString();
    } else if ("database" == sourceName) {
        q_dataSource = GlobalDefine::dsDatabase;
    }

    return std::string();
}
