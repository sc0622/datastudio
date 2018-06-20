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
    // Ĭ���ļ�·��
    q_defaultPath = readXmlFile();

    q_dbaccess = new DBAccess();
    if (GlobalDefine::dsDatabase == q_dataSource) {
        // ��ʼ�����ݿ�
        const Json::Value option = JMain::instance()->option("edit", "parser");
        if (!option.isNull()) {
            if (!q_dbaccess->init(option)) {
                QMessageBox::warning(parentWidget_, QStringLiteral("���ݿ����"),
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

// ��Ϣ��Ӧ�ص�
JLRESULT DataManegement::notifyRespond(const Icd::JNEvent &event)
{
    QVariantList args = event.argument().toList();
    const QString id = event.channel().section("edit.", 1);

    if ("loadInfrastructure" == id) { // ��ѯ���еĻ�������
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
                // ���ػ����ṹ����
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
                // �����ļ�����
                Icd::VehiclePtrArray vehicles;
                // ��������
                if (!parser->parse(vehicles, deep)) {
                    *error = QStringLiteral("�����ļ�ʧ�ܣ�");
                    break;
                }
                // ת������
                Json::Value config2(Json::objectValue);
                Icd::SqlParser sqlParser(config2);
                DMSpace::_vectorPS infrastructure;
                DMSpace::_vectorIcdTR tableRules;
                std::vector<int> params;
                params.push_back((int)&infrastructure);
                params.push_back((int)&tableRules);

                if (!sqlParser.convert2Other(vehicles, params)) {
                    *error = QStringLiteral("�����ļ�ʧ�ܣ�");
                    break;
                }
                if (!loadXmlBaseData(infrastructure, tableRules)) {
                    *error = QStringLiteral("�����ļ�ʧ�ܣ�");
                }
                break;
            }
            default:
                break;
            }
        }
    } else if("queryAllPlane" == id) { // ��ѯ���еĻ�������
        if (args.count() > 0) {
            PlaneNode::planeVector *data = jVariantFromVoid<PlaneNode::planeVector>(args[0]);
            if (!data) {
                return -1;
            }
            *data = q_planes;
        }
    } else if ("querySinglePlane" == id) { // ��ѯһ����������
        if (args.count() >= 2) {
            PlaneNode::smtPlane *data = jVariantFromVoid<PlaneNode::smtPlane>(args[0]);
            if (!data) {
                return -1;
            }
            *data = planeNode(args[1].toInt());
        }
    } else if ("queryDictionary" == id) { // ��ѯ�ֵ��
        if (args.count() > 0) {
            stQueryDic *data = jVariantFromVoid<stQueryDic>(args[0]);
            if (!data) {
                return -1;
            }

            queryDictionary(*data);
        }
    } else if ("queryDictionaryTable" == id) { // ��ѯһ���ֵ��
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
        case GlobalDefine::dicDataType:    // ��������
            dicName = DBAccess::tableName(DataBaseDefine::dbDataType);
            break;
        case GlobalDefine::dicPowerType:   // Ȩ������
            dicName = DBAccess::tableName(DataBaseDefine::dbPowerType);
            break;
        case GlobalDefine::dicCounterType:  // ֡��������
            dicName = DBAccess::tableName(DataBaseDefine::dbCounterType);
            break;
        case GlobalDefine::dicCheckType:   // У������
            dicName = DBAccess::tableName(DataBaseDefine::dbCheckType);
            break;
        case GlobalDefine::dicArrayType:   // ��������
            dicName = DBAccess::tableName(DataBaseDefine::dbArrayType);
            break;
        default:
            break;
        }
        *dics = CDictionary::instance().singleDic(dicName);
    } else if ("loadRules" == id) {    // ���ع�������
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
    } else if ("unLoadRules" == id) {    // ж�ع�������
        if (args.count() < 1) {
            return -1;
        }
        const QString *condition = jVariantFromVoid<QString>(args[0]);
        if (!condition) {
            return -1;
        }
        unLoadRules(*condition);
    } else if ("modifyAllPlane" == id) { // �޸����л�������
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
        // �������л�������
        *result = modifyAllKC(planes);
    } else if ("savePlane" == id) { // �����������
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
        // ��������
        *result = savePlane(*plane);
    } else if ("modifyPlane" == id) {   // �޸Ļ�������
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
        // �޸�����
        *result = modifyOneKC(SMT_CONVERT(PlaneNode, *element));
    } else if ("deletePlane" == id) {   // ɾ����������
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
        // ɾ������
        *result = deletePlane(*planeID);
    } else if ("saveSystem" == id) {    // ����ϵͳ����
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
        // ��������
        *result = saveSystem(planeID.toInt(), systems);
    } else if ("modifySystem" == id) {    // �޸�ϵͳ����
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
        // �޸�����
        *result = modifySystem(planeID, SMT_CONVERT(SystemNode, element));
    } else if ("deleteSystem" == id) {  // ɾ��ϵͳ����
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
        // ɾ������
        *result = deleteSystem(planeID.toInt(), systemID);
    } else if ("saveTable" == id) { // ����ICD������
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
        // ��������
        *result = saveTable(keys, tables);
    } else if ("modifyTable" == id) { // �޸�ICD������
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
        // �޸�����
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
        // ɾ������
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
        // �������ݿ�
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
        // �������ݿ�
        *result = insertRule(keys, meta);
    } else if ("deleteRule" == id) {    // ɾ����������
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
        // ɾ�����ݿ�
        *result = deleteRule(keys, rules);
    } else if ("deleteSubTable" == id) {    // ɾ��֡ʶ�����ӱ�
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
        // ɾ�����ݿ�
        *result = deleteSubTable(keys, rules);
    }

    return 0;
}

/**
 * @brief ���ػ�������
     * @param [in] deep
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DataManegement::loadBaseData(std::string &error, int deep)
{
    // �������
    clearKC();

    // ��ʼ���ֵ������
    if (!loadDic()) {
        return false;
    }

    // ��ʼ��ϵͳ��Ϣ
    if (!loadInfrastructure(deep)) {
        error = q_dbaccess->lastError().c_str();
        qDebug() << error.c_str();
        return false;
    }

    return true;
}

/**
* @brief ���ػ�������
* @param [in] loadXmlBaseData : ����ϵͳ��Ϣ
* @param [in] tableRules : �������Ϣ
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DataManegement::loadXmlBaseData(const DMSpace::_vectorPS &infrastructure,
                                     const DMSpace::_vectorIcdTR &tableRules)
{
    // �������
    clearKC();

    // ��ʼ���ֵ������
    if (!loadDic()) {
        return false;
    }
    TableNode::tableVector tables;  // ��¼�����
    // ���춥�������
    int count = tableRules.size();
    //    int tableLevel = 1; // ������1��ʼ
    for (int i = 0; i < count; ++i) {
        const DMSpace::pairIcdTR &icdTR = tableRules[i];
        stICDBase icdBase = icdTR.first;
        if (!icdBase.sParentName.empty()) {
            continue;
        }
        // ��д����ȼ�
        //         if (icdBase.sLevel.empty()) {
        //             icdBase.sLevel = QString("%1_%2").arg(tableLevel)
        //                 .arg(i + 1).toStdString();
        //         }
        // û�и���֤���Ƕ����
        TableNode::smtTable tableNode(new TableNode(icdBase));
        tables.push_back(tableNode);
    }

    // ������������з���
    count = tables.size();
    TableNode::smtTable table = 0;
    std::unordered_map<std::string, TableNode::tableVector> classified;
    for (int i = 0; i < count; ++i) {
        if (table = tables[i]) {
            stICDBase icdBase = table->icdBase();
            classified[icdBase.sGroup].push_back(table);
        }
    }

    // �������ϵͳ����
    std::string group;
    std::unordered_map<std::string, TableNode::tableVector>::iterator
            itTable = classified.end();
    DMSpace::_vectorPS_Cit itPS = infrastructure.begin();
    for (; itPS != infrastructure.end(); ++itPS) {
        const stPlane &plane = itPS->first;
        // ����
        PlaneNode::smtPlane planeNode(new PlaneNode(plane));
        const std::vector<stSystem>& department = itPS->second;
        count = department.size();
        // ϵͳ
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
 * @brief �����ֵ������
 * @return ִ�н����true���ɹ���false��ʧ��
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
* @brief ���¼����ֵ������
* @param [in] dics : �ֵ��
* @return ִ�н����true���ɹ���false��ʧ��
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
 * @brief ���ػ����ܹ����ݣ����ͺ�ϵͳ��
     * @param [in] deep
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DataManegement::loadInfrastructure(int deep)
{
    if (!q_dbaccess) {
        return false;
    }
    // ��ȡ����
    std::vector<stPlane> planeBase;
    if (!q_dbaccess->readPlane(planeBase)) {
        qDebug() << "readPlane failed!";
        return false;
    }
    // ��ȡϵͳ
    std::unordered_map<int, std::vector<stSystem> > systemBase;
    if (deep >= Icd::ObjectSystem) {
        if (!q_dbaccess->readSystem(systemBase)) {
            qDebug() <<"readSystem failed!";
            return false;
        }
    }
    // ��ѯ�������Ϣ
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
    //    int tableLevel = 1; // ������1��ʼ
    for (int i = 0; i < count; ++i) {
        // ����
        stPlane plane = planeBase.at(i);
        PlaneNode::smtPlane planeNode(new PlaneNode(plane));
        if ((it = systemBase.find(plane.nCode)) != systemBase.end()) {
            const std::vector<stSystem>& department = it->second;
            const int bound = department.size();
            // ��ϵͳ
            for (int j = 0; j < bound; ++j) {
                stSystem system = department.at(j);
                SystemNode::smtSystem systemNode(new SystemNode(system));
                std::string group = QString("%1/%2")
                        .arg(plane.nCode).arg(system.nCode).toStdString();
                // �����
                if ((itBase = icdBase.find(group)) != icdBase.end()) {
                    std::vector<stICDBase>& collBase = itBase->second;
                    const int bound(collBase.size());
                    for (int j = 0; j < bound; ++j) {
                        stICDBase& base = collBase.at(j);
                        // ֻ����������ϵͳ�ڵ�Ĺ����
                        if (base.sParentName.empty()) {
                            // ��д����ȼ�
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
 * @brief ����ָ�����͵�����ICD��
 * @param [in] planeType : �ɻ��ͺ���
 * @param [in] increment : ������ʶ
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DataManegement::loadTypeRule(int planeType, bool increment)
{
    if (NULL == q_dbaccess) {
        return false;
    }
    // ��ȡ��������Ϣ
    DMSpace::_vectorSB ICDBase;
    QString condition = QString::number(planeType);
    if (!q_dbaccess->readSpecificICD(condition.toStdString(), ICDBase)) {
        return false;
    }
    return loadRuleData(planeType, ICDBase, increment);
}

/**
* @brief ����ָ�����͵�����ICD��
* @param [in] planeType : �ɻ��ͺ���
* @param [in] tableRules : �������Ϣ
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DataManegement::loadXmlTypeRule(int planeType,
                                     DMSpace::_vectorIcdTR &tableRules)
{
    // ��ѯ�ڴ����Ƿ��Ѿ����ػ����ܹ�����
    PlaneNode::smtPlane plane = planeNode(planeType);
    if (NULL == plane) {
        return false;
    }
    SystemNode::smtSystem system;
    std::unordered_map<std::string, TableNode::tableVector> rules = loadXmlRuleData(tableRules);
    std::unordered_map<std::string, TableNode::tableVector>::iterator it = rules.begin();
    // �����ݱ��浽�ڴ���
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
* @brief ж��ָ�����͵�����ICD��
* @param [in] planeType : �ɻ��ͺ���
* @return ִ�н����true���ɹ���false��ʧ��
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
 * @brief ����ָ�������·�ϵͳ������ICD��
 * @param [in] planeType : �ɻ��ͺ���
 * @param [in] system : ϵͳ��
 * @param [in] increment : ������ʶ
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DataManegement::loadSystemRule(int planeType, int system,
                                    bool increment)
{
    if (NULL == q_dbaccess) {
        return false;
    }
    // ��ȡ��������Ϣ
    DMSpace::_vectorSB ICDBase;
    QString condition = QString("%1/%2").arg(planeType).arg(system);
    if (!q_dbaccess->readSpecificICD(condition.toStdString(), ICDBase)) {
        return false;
    }
    return loadRuleData(planeType, ICDBase, increment);
}

/**
* @brief ����ָ�������·�ϵͳ������ICD��
* @param [in] planeType : �ɻ��ͺ���
* @param [in] system : ϵͳ��
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DataManegement::loadXmlSystemRule(int planeType, int system,
                                       DMSpace::_vectorIcdTR &tableRules)
{
    // ��ѯ�ڴ����Ƿ��Ѿ����ػ����ܹ�����
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
    // �����ݱ��浽�ڴ���
    if (it != rules.end()) {
        singleSystem->setTable(it->second);
    }

    return true;
}

/**
* @brief ж��ָ�������·�ϵͳ������ICD��
* @param [in] planeType : �ɻ��ͺ���
* @param [in] system : ϵͳ��
* @return ִ�н����true���ɹ���false��ʧ��
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
 * @brief ���ص���ICD��
 * @param [in] plane : �ɻ��ͺ���
 * @param [in] system : ϵͳ��
 * @param [in] table : ICD����
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DataManegement::loadTableRule(int plane,
                                   int system,
                                   const std::string &table)
{
    if (NULL == q_dbaccess) {
        return false;
    }
    // ��ѯ�ڴ����Ƿ��Ѿ����ػ����ܹ�����
    PlaneNode::smtPlane data = planeNode(plane);
    if (NULL == data) {
        return false;
    }
    // ��ѯϵͳ��
    SystemNode::smtSystem sysNode = data->system(system);
    if (NULL == sysNode) {
        return false;
    }
    // ��ȡ�ӱ�
    std::vector<std::string> tables = q_dbaccess->querySubTables(table);
    tables.insert(tables.begin(), table);
    //
    std::vector<stICDBase> icdBases;
    if (!q_dbaccess->readICDBase(tables, icdBases)) {
        return false;
    }
    // ��ȡ��������
    DMSpace::svrMap rules;
    if (!q_dbaccess->readTableRules(tables, rules)) {
        return false;
    }
    // �����
    TableNode::tableMap tableNodes;    // ��¼��ڵ�
    const int count = icdBases.size();
    for (int j = 0; j < count; ++j) {
        const stICDBase &icdData = icdBases[j];

        TableNode::smtTable table(new TableNode(icdData));
        tableNodes[icdData.sName] = table;
    }
    // ������
    fillRules(tableNodes, rules);

    // ��ѯ��ϵ���������ݱ��浽�ڴ���
    // ��ѯ���ϱ���ӱ����ݣ�����֮��������
    TableNode::smtTable _table = recursiveLinkTable(tableNodes, table);
    if (!_table) {
        return false;
    }
    _table->setLengthCheck(_table->length() == (int)_table->realLength()
                           && _table->length() > 0);
    // �����ڴ�����
    sysNode->addTable(_table);

    return true;
}

/**
* @brief ���ص���ICD��
* @param [in] plane : �ɻ��ͺ���
* @param [in] system : ϵͳ��
* @param [in] table : ICD����
* @param [in] tableRules : �������Ϣ
* @return ִ�н����true���ɹ���false��ʧ��
*/
bool DataManegement::loadXmlTablemRule(int plane, int system,
                                       const std::string &table,
                                       DMSpace::_vectorIcdTR &tableRules)
{
    // ��ѯ�ڴ����Ƿ��Ѿ����ػ����ܹ�����
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
    // �����ݱ��浽�ڴ���
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
* @brief ж�ص���ICD��
* @param [in] planeType : �ɻ��ͺ���
* @param [in] system : ϵͳ��
* @param [in] table : ICD����
* @return ִ�н����true���ɹ���false��ʧ��
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
 * @brief �ݹ���ر��������
 * @param [in] name : ICD����
 * @param [in] table : ��������
 * @param [in] rules : ���������
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DataManegement::recursiveLoadRule(const std::string &name,
                                       TableNode::smtTable &table,
                                       DMSpace::_vectorIcdTR &rules)
{
    if (!table) {
        return false;
    }
    // ��ѯ����
    DMSpace::pairIcdTR icdTR = singleIcdTR(name, rules, true);
    if (icdTR.first.sName.empty()) {
        return false;
    }
    int size = 0;
    table->setICDBase(icdTR.first);
    const std::vector<stTableRules> &tableRules = icdTR.second;
    std::unordered_map<int, ICDFrameCodeData::smtFrameCode> codes;    // ֡ʶ����
    const int count = tableRules.size();
    ICDMetaData::smtMeta metaData = 0;
    for (int i = 0; i < count; ++i) {
        const stTableRules &ruleData = tableRules[i];
        // ���ݹ������ݴ�����Ӧ��������ʵ��
        if (!(metaData = ICDFactory::instance().CreatObject(ruleData))) {
            continue;
        }
        if (GlobalDefine::dtFrameCode == ruleData.uType) {
            codes[atoi(ruleData.sDefault.c_str())]
                    = SMT_CONVERT(ICDFrameCodeData, metaData);
        }
        // �����������Ϊ�������ݣ����ٴζ�ȡ�ñ�Ĺ�������
        if (IcdDefine::icdComplex == metaData->metaType()) {
            ICDComplexData::smtComplex complex
                    = std::dynamic_pointer_cast<ICDComplexData>(metaData);
            // ѭ����ȡ�ӱ�����
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
        // ��¼
        table->addRule(metaData);
    }
    // ��֡ʶ�����֡���ݽ�������
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
 * @brief ����һ����������
 * @param [in] data : ��������
 * @return ִ�н����true���ɹ���false��ʧ��
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
 * @brief �޸�һ����������
 * @param [in] data : ��������
 * @return ִ�н����true���ɹ���false��ʧ��
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
 * @brief ɾ��һ����������
 * @param [in] code : ������
 * @return ִ�н����true���ɹ���false��ʧ��
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
* @brief ��ջ�������
*/
void DataManegement::clearKC()
{
    q_planes.clear();
}

/**
 * @brief ��ѯ��������
 * @param [in] code : ������
 * @return ��������
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
* @brief ��ѯ�������������
* @param [in] name : ICD����
* @param [in] rules : ICD�����ݼ�
* @param [in] remove : ɾ����ʶ
* @return ���������
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
 * @brief ���ع�������
 * @param [in] planeType : ������
 * @param [in] ICDBase : ICD����������
 * @param [in] increment : ������ʶ
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DataManegement::loadRuleData(int planeType,
                                  const DMSpace::_vectorSB &ICDBase,
                                  bool increment)
{
    // �������ݿ�
    if (NULL == q_dbaccess) {
        return false;
    }
    // ��ѯ�ڴ����Ƿ��Ѿ����ػ����ܹ�����
    PlaneNode::smtPlane data = planeNode(planeType);
    if (NULL == data) {
        return false;
    }
    TableNode::tableMap tableNodes;    // ��¼��ڵ�
    std::vector<std::string> topTables;     // ��¼�����
    // �����ϵ
    std::vector<std::string> tables;    // ��¼���б�
    int count = ICDBase.size();
    int size = 0;
    for (int i = 1; i < count; ++i) {
        const std::vector<stICDBase> &base = ICDBase[i].second;
        size = base.size();
        for (int j = 0; j < size; ++j) {
            const stICDBase &icdData = base[j];
            tables.push_back(icdData.sName);
            // ��ѡ�����
            if (icdData.sParentName.empty()) {
                topTables.push_back(icdData.sName);
            }
            TableNode::smtTable table(new TableNode(icdData));
            tableNodes[icdData.sName] = table;
        }
    }
    // ��ȡ��������
    DMSpace::svrMap rules;
    if (!q_dbaccess->readTableRules(tables, rules)) {
        return false;
    }
    // ������
    fillRules(tableNodes, rules);

    // ��ѯ��ϵ���������ݱ��浽�ڴ���
    // ��ѯ���ϱ���ӱ����ݣ�����֮��������
    // ������������з��࣬����������
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
    // ��������
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
    // �����ݱ��浽�ڴ���
    if (increment) {    // �������������ֻ����
        std::unordered_map<int, TableNode::tableVector>::iterator itIS
                = system_table.begin();
        SystemNode::smtSystem sysNode;
        for (; itIS != system_table.end(); ++itIS) {
            // ��ѯϵͳ��
            if (!(sysNode = data->system(itIS->first))) {
                continue;
            }
            const TableNode::tableVector &items = itIS->second;
            count = items.size();
            for (int i = 0; i < count; ++i) {
                sysNode->addTable(items[i]);
            }
        }
    } else {    // ������������
        std::unordered_map<int, TableNode::tableVector>::iterator itIS
                = system_table.begin();
        SystemNode::smtSystem sysNode;
        for (; itIS != system_table.end(); ++itIS) {
            // ��ѯϵͳ��
            if (!(sysNode = data->system(itIS->first))) {
                continue;
            }
            sysNode->setTable(itIS->second);
        }
    }

    return true;
}

/**
* @brief ����������
* @param [in] tables : ��
* @param [in] rules : �������ݼ�<����, �������ݼ�>
*/
void DataManegement::fillRules(TableNode::tableMap &tables,
                               const DMSpace::svrMap &rules)
{
    ICDMetaData::smtMeta metaData = 0;
    ICDMetaData::ruleMap::iterator itR;
    std::unordered_map<int, ICDFrameCodeData::smtFrameCode> codes;    // ֡ʶ����
    std::unordered_map<int, ICDFrameCodeData::smtFrameCode>::iterator itC = codes.begin();
    TableNode::tableMap::iterator itTM = tables.end();
    DMSpace::svrMap::const_iterator itSSR = rules.begin();
    for (; itSSR != rules.end(); ++itSSR) {
        // ��ѯ��
        if ((itTM = tables.find(itSSR->first)) != tables.end()) {
            TableNode::smtTable &table = itTM->second;
            if (!table) {
                continue;
            }
            const std::vector<stTableRules> &srRules = itSSR->second;
            ICDMetaData::ruleMap tableRules;
            // ������������
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
            // ��֡ʶ�����֡���ݽ�������
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
* @brief ���ع�������
* @param [in] tableRules : �������Ϣ
* @return �������Ϣ<�����飬<�����>>
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
    TableNode::tableMap tables;  // ��¼�����
    std::vector<std::string> topTables;  // ��¼�����
    // ������������
    int count = tableRules.size();
    for (int i = 0; i < count; ++i) {
        const DMSpace::pairIcdTR &icdTR = tableRules[i];
        stICDBase icdBase = icdTR.first;
        const std::vector<stTableRules> &rules = icdTR.second;
        // û�и���֤���Ƕ����
        if (icdBase.sParentName.empty()) {
            topTables.push_back(icdBase.sName);
        }
        TableNode::smtTable tableNode(new TableNode(icdBase));
        mapRules = convert2IcdRules(rules);
        tableNode->setRule(mapRules);
        tables[icdBase.sName] = tableNode;
        mapRules.clear();
    }
    // ��ѯ���ϱ���ӱ����ݣ�����֮��������
    // ������������з��࣬����������
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
 * @brief �ݹ齨�����ϱ��ϵ
 * @param [in] allTable : Դ��
 * @param [in] name : ����
 * @return �ӱ�����
 */
TableNode::smtTable DataManegement::recursiveLinkTable(TableNode::tableMap &allTable,
                                                       const std::string &name)
{
    TableNode::tableMap::iterator itTM = allTable.find(name);
    if (itTM == allTable.end()) {
        return TableNode::smtTable();
    }
    int count = 0;
    // ���ϱ�
    TableNode::smtTable tableNode = itTM->second;
    if (tableNode->isComplex()) {
        ICDMetaData::ruleMap tableRules = tableNode->allRule();
        ICDMetaData::ruleMap::iterator itTR = tableRules.begin();
        ICDComplexData::smtComplex complex = 0;
        TableNode::smtTable subTable = 0;
        for (; itTR != tableRules.end(); ++itTR) {
            ICDMetaData::smtMeta& metaData = itTR->second;
            // �Ǹ���������
            if (NULL == metaData
                    || IcdDefine::icdComplex != metaData->metaType()) {
                continue;
            }
            if (!(complex = SMT_CONVERT(ICDComplexData, metaData))) {
                continue;
            }
            // ���ܴ��ڶ����ӱ�
            QStringList lstName = QString::fromStdString(complex->rule())
                    .split("@", QString::SkipEmptyParts);
            count = lstName.size();
            // ��ȡ�ӱ�����
            for (int i = 0; i < count; ++i) {
                subTable = recursiveLinkTable(allTable,
                                              lstName[i].toStdString());
                if (NULL != subTable) {
                    // ��������
                    complex->addTable(subTable);
                    // ���ܱ���ɾ���ӱ�
                    if (!subTable->icdBase().sParentName.empty()) {
                        if ((itTM = allTable.find(subTable->key())) != allTable.end())
                            allTable.erase(itTM);
                    }
                }
            }
            // ���±���
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
    case GlobalDefine::dictCode:   // ����
        CDictionary::instance().convert2Code(dicName(data.dic),
                                             data.condition,
                                             code);
        data.result = QString::number(code).toStdString();
        break;
    case GlobalDefine::dictDec:    // ����
        CDictionary::instance().convert2Dec(dicName(data.dic),
                                            atoi(data.condition.c_str()),
                                            data.result);
        break;
    case GlobalDefine::dictSign:   // ��ʶ
        single = CDictionary::instance()
                .singleItem(dicName(data.dic), atoi(data.condition.c_str()));
        data.result = single.sSign;
        break;
    case GlobalDefine::dictRemark: // ��ע
        single = CDictionary::instance()
                .singleItem(dicName(data.dic), atoi(data.condition.c_str()));
        data.result = single.sRemark;
        break;
    default:
        break;
    }

    return true;
}

// ���ع�������
bool DataManegement::loadRules(PlaneNode::smtPlane &plane,
                               const QString &condition,
                               bool increment)
{
    bool result = false;
    if (GlobalDefine::dsNone == q_dataSource) {
        return result;
    }

    QStringList items = condition.split("/");
    // ��������Դ�Ĳ�ͬ�����¶�ȡ����
    if (GlobalDefine::dsDatabase == q_dataSource) {
        // ����ԴΪ���ݿ�
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
        // ����ԴΪ�ļ�   // �����ļ�����
        Json::Value config;
        config["sourceType"] = "file";
        config["filePath"] = q_defaultPath;
        Icd::ParserPtr parser = Icd::Parser::create(config);
        if (!parser) {
            return false;
        }
        Icd::VehiclePtr vehicle;
        Icd::VehiclePtrArray vehicles;
        // ��������
        switch (items.size()) {
        case 1: // ����
        {
            if (!parser->parse(items.first().toStdString(), vehicle, Icd::ObjectItem)) {
                break;
            }
            break;
        }
        case 2: // ϵͳ
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
        case 3: // ��
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
        // ת������
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

// ж�ع�������
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

// �޸����л�������
bool DataManegement::modifyAllKC(const std::vector<PlaneNode::smtPlane> &planes)
{
    q_planes = planes;
    return true;
}

// �����������
bool DataManegement::savePlane(const stPlane &plane)
{
    // �����ڴ�
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
    // ɾ���ڴ�
    const int count = keys.size();
    for (int i = 0; i < count; ++i) {
        deleteOneKC(keys.at(i));
    }

    return true;
}

// ����ϵͳ����
bool DataManegement::saveSystem(int plane, const std::vector<stSystem>& system)
{
    // �����ڴ�
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

// �޸�ϵͳ����
bool DataManegement::modifySystem(int plane, const SystemNode::smtSystem &system)
{
    PlaneNode::smtPlane smtPlane = planeNode(plane);
    if (!smtPlane) {
        return false;
    }
    smtPlane->addSystem(system);

    return true;
}

// ɾ��ϵͳ����
bool DataManegement::deleteSystem(int plane, const std::vector<int>& system)
{
    // ɾ���ڴ�
    PlaneNode::smtPlane smtPlane = planeNode(plane);
    if (!smtPlane) {
        return false;
    }
    smtPlane->deleteSystem(system);

    return true;
}

// ����ICD������
bool DataManegement::saveTable(const QString &keys, const std::vector<stICDBase> &tables)
{
    // �����ڴ�
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

// �޸�ICD������
bool DataManegement::modifyTable(const QString &keys,
                                 const TableNode::smtTable &table)
{
    // �����ڴ�
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
    if (keyLst.empty()) {   // �����
        system->addTable(table);
    } else {    // �ӱ�
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

// ɾ��ICD������
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
    // ��ѯ�ӱ�
    if (!plane) {
        return false;
    }
    if (!(system = plane->system(systemID))) {
        return false;
    }
    // ɾ���ڴ�
    system->deleteTable(tables);

    return true;
}

// �����������
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
    if (keyLst.size() > 0) {    // �ӱ�����
        if (!(table = table->subTable(keyLst.join("/").toStdString()))) {
            return result;
        }
    }
    // �����֡ʶ���룬�Ȱ�֡����
    ICDFrameCodeData::smtFrameCode frameCode = std::dynamic_pointer_cast<ICDFrameCodeData>(meta);
    if (frameCode) {    // ����֡����
        std::string code = frameCode->defaultStr();
        if (code.empty()) {
            frameCode->bindData(0);
        } else {
            ICDComplexData::smtComplex complex = std::dynamic_pointer_cast<ICDComplexData>(
                        table->rule(atoi(code.c_str())));
            frameCode->bindData(complex);
        }
    }
    bool reorder = false;   // �����������ݱ�־
    // ����޸����ݳ��ȣ����Ҹ����ݲ������һ�����ݣ�������ֽ���Ŵ�λ�����⣬
    // �����Ƚ�ԭʼ���ݿ������Ӵ�����������ſ�ʼ�����������ֽ���ţ��ٱ���
    ICDMetaData::smtMeta oldMeta = table->rule(meta->serial());
    if (oldMeta) {  // �޸�
        if (meta->serial() != table->allRule().rbegin()->first) {
            // �޸��м�����
            if (meta->byteLength() != oldMeta->byteLength()
                    || meta->length() != oldMeta->length()
                    || meta->metaType() != oldMeta->metaType()) {
                // ���ݳ��ȱ���������������ͱ��
                reorder = true;
            } else {
                ICDBitData::smtBit oldBit = SMT_CONVERT(ICDBitData, oldMeta);
                ICDBitData::smtBit bit = SMT_CONVERT(ICDBitData, meta);
                // �ж��Ƿ�����Ӱ���ֽ���ŵ�����
                if (oldBit) {
                    if (oldBit->start() != bit->start()) {
                        reorder = true;
                    }
                }
            }
        }
    }
    if (reorder) {
        // ������������
        ICDMetaData::ruleMap rules = table->allRule();
        ICDMetaData::ruleMap::iterator itR = rules.find(meta->serial());
        int index = 0;
        ICDBitData::smtBit bit = 0;
        ICDMetaData::smtMeta metaPre = meta;
        ICDBitData::smtBit bitPre = SMT_CONVERT(ICDBitData, metaPre);
        // ��Ŀ������֮��ʼ�������ֽں�
        for (++itR; itR != rules.end(); ++itR) {
            ICDMetaData::smtMeta &smtMeta = itR->second;
            if (!smtMeta) {
                continue;
            }
            bit = SMT_CONVERT(ICDBitData, smtMeta);
            // �����ֽ����
            if (bitPre && bit
                    && bitPre->end() < bit->start()) {
                index = bitPre->index() - bitPre->start() / 8 + bit->start() / 8;
            } else {
                if (metaPre) {
                    index = metaPre->index() + metaPre->byteLength();
                }
                if (bit) {
                    index += bit->start() / 8;  // ƫ���ֽ�
                }
            }
            smtMeta->setIndex(index);
            metaPre = smtMeta;
            bitPre = bit;
        }
        // ����Ŀ������
        table->addRule(meta);
        result = GlobalDefine::special;
    } else {    // �������߲�δ������ݳ���
        // �����ڴ�
        table->addRule(meta);
        result = GlobalDefine::success;
    }
    // ���±���
    if (keyLst.size() > 0) {    // �ӱ�����
        topTable->updateLength(keyLst.join("/").toStdString());
    }

    return result;
}

// �����������
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
    if (keyLst.size() > 0) {    // �ӱ�����
        table = table->subTable(keyLst.join("/").toStdString());
        if (!table) {
            return false;
        }
    }
    // �����֡ʶ���룬�Ȱ�֡����
    ICDFrameCodeData::smtFrameCode frameCode = SMT_CONVERT(ICDFrameCodeData, meta);
    if (frameCode) {    // ����֡����
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
    // �������ݻ�����ֽ���Ŵ�λ�����⣬���ԴӲ���λ�ÿ�ʼ��
    // ���������ֽ���ţ��ٱ���
    ICDMetaData::ruleMap rules = table->allRule();
    // ��¼������
    ICDMetaData::smtMeta newData = meta;
    ICDMetaData::smtMeta oldData = 0;
    ICDMetaData::ruleMap::iterator itR = rules.find(meta->serial());
    if (itR == rules.end()) {
        return false;
    }
    // ��ѯ�Ƿ���bit�ϲ�����
    if (!(oldData = itR->second)) {
        return false;
    }
    int index = 0;
    ICDBitData::smtBit bit = 0;
    ICDBitData::smtBit bitPre = SMT_CONVERT(ICDBitData, newData);
    // ��Ŀ�����ݿ�ʼ�������ֽ����
    for (; itR != rules.end(); ++itR) {
        if (!(oldData = itR->second)) {
            continue;
        }
        bit = SMT_CONVERT(ICDBitData, oldData);
        if (bitPre && bit
                && bitPre->end() < bit->start()) {
            // ��ǰ���ܹ���ǰһ��ϲ���������ƫ����
            index = bitPre->index() - bitPre->start() / 8 + bit->start() / 8;
        } else {
            index = newData->index() + newData->byteLength();
            if (bit) {
                index += bit->start() / 8;  // ƫ���ֽ�
            }
        }
        // �����ֽ����
        oldData->setIndex(index);
        // ����˳���
        oldData->setSerial(oldData->serial() + 1);
        // ����map
        itR->second = newData;
        newData = oldData;
        bitPre = bit;
    }
    // ����ԭʼ�����һ������
    rules[newData->serial()] = newData;
    // �����ڴ�
    table->setRule(rules);
    // ���±���
    if (keyLst.size() > 0) {    // �ӱ�����
        topTable->updateLength(keyLst.join("/").toStdString());
    }

    return true;
}

// ɾ����������
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
    // ����ɾ�����ݺ󣬻�����ֽ���Ų����������⣬�����Ƚ�ԭʼ���ݿ�����
    // ɾ��Ŀ�����ݺ����������ֽ���ţ��ٱ���
    TableNode::smtTable topTable = rootTable(planeID, systemID, rootParent);
    TableNode::smtTable table = topTable;
    if (!table) {
        return false;
    }
    if (keyLst.size() > 0) {    // �ӱ�����
        if (!(table = table->subTable(keyLst.join("/").toStdString()))) {
            return false;
        }
    }
    // ɾ������
    table->deleteRule(rules);
    // ����ɾ������ɾ�������ݲ������һ������ʱ��������������
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
                // ��ǰ���ܹ���ǰһ��ϲ���������ƫ����
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
    // ���±���
    if (keyLst.size() > 0) {    // �ӱ�����
        topTable->updateLength(keyLst.join("/").toStdString());
    }

    return true;
}

// ɾ��֡ʶ�����ӱ�
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
    // ��ѯ�ӱ�
    TableNode::smtTable topTable = rootTable(planeID, systemID, rootParent);
    TableNode::smtTable table = topTable;
    if (!table) {
        return false;
    }
    if (keyLst.size() > 0) {    // �ӱ�����
        if (!(table = table->subTable(keyLst.join("/").toStdString()))) {
            return false;
        }
    }
    // ��ѯ֡����
    ICDComplexData::smtComplex complex
            = std::dynamic_pointer_cast<ICDComplexData>(table->rule(serial));
    if (!complex) {
        return false;
    }
    // ɾ���ڴ�
    complex->deleteTable(subTables);
    // ���±���
    if (keyLst.size() > 0) {    // �ӱ�����
        topTable->updateLength(keyLst.join("/").toStdString());
    }

    return true;
}

// ��ѯ�ڴ�����
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
    } else if (1 == keyList.size()) {   // ����
        *element = planeNode(keyList.first().toInt());
    } else if (2 == keyList.size()) {   // ϵͳ
        PlaneNode::smtPlane plane = planeNode(keyList.takeFirst().toInt());
        if (!plane) {
            return -1;
        }
        *element = plane->system(keyList.takeFirst().toInt());
    } else if (3 == keyList.size()) {  // ��
        PlaneNode::smtPlane plane = planeNode(keyList.takeFirst().toInt());
        if (!plane) {
            return -1;
        }
        SystemNode::smtSystem system = plane->system(keyList.takeFirst().toInt());
        if (!system) {
            return -1;;
        }
        *element = system->table(keyList.takeFirst().toStdString());
    } else {    // ����
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
            // �ӱ�
            *element = table->subTable(keyList.join("/").toStdString());
        } else {
            // ��������
            keyList.takeLast();
            if (!keyList.isEmpty()) {
                table = table->subTable(keyList.join("/").toStdString());
                if (!table) {
                    return -1;;
                }
            }
            // ��������
            *element = table->rule(lastItem.toInt());
        }
    }

    return 0;
}

// ��ѯ��������Դ
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

// ��ѯ�ӱ�����
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

// ���濽������
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
    // ��ѯԴ����
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
        // Դ����Ϊ��������ʱ���п������ӱ�Ҳ�п����ǹ���
        lastItem = dest.mid(dest.lastIndexOf("/") + strlen("/"));
        if (lastItem.contains("ICDTable_")) { // �ӱ�
            lastItem.clear();
        } else {    // ����
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
    // ��������
    if (GlobalDefine::ntPlane == element->objectType()) {
        QString id;
        // ����
        const stPlane &base = *reinterpret_cast<stPlane *>(params->at(1));
        id = jnotify->send("edit.queryId", int(GlobalDefine::ntPlane)).toString();
        // ��¡����
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
        // ϵͳ
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
        // ��
        const stICDBase &base = *reinterpret_cast<stICDBase *>(params->at(1));
        TableNode::smtTable table = SMT_CONVERT(TableNode, element)->clone();
        if (!table) {
            return -1;;
        }
        if (lastItem.isEmpty()) {   // �����
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
        } else {    // �ӱ�
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
            // ���±���
            if (topTable) {
                topTable->updateLength(subKey);
            }
        }
        *result = table;
    } else if (GlobalDefine::ntRule == element->objectType()) {
        // ����
        ICDCommonData::smtCommon data =
                SMT_CONVERT(ICDCommonData, *reinterpret_cast<ICDMetaData::smtMeta *>(params->at(1)));
        ICDMetaData::smtMeta meta = SMT_CONVERT(ICDMetaData, element);
        TableNode::smtTable table = SMT_CONVERT(TableNode, desElement);
        if (!data || !meta || !table) {
            return -1;;
        }
        ICDCommonData::smtCommon common;
        // ����û��ı��������ͣ���ʹ�ý������ݣ�����ʹ��Դ����
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
        // ���±���
        if (topTable) {
            topTable->updateLength(subKey);
        }
    }

    return 0;
}

// ���ڴ����ݱ��浽���ݿ�
JLRESULT DataManegement::saveDatabase(const Icd::JNEvent &event)
{
    // ��ѯ����Դ��������ļ�����������һ����
    // ������������㼶��ɾ���ڴ��в����ڵ����ݣ�
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
    } else if (1 == keyList.size()) {  // �������
        dataLevel = GlobalDefine::ntPlane;
    } else if (2 == keyList.size()) {   // ����ϵͳ
        dataLevel = GlobalDefine::ntSystem;
    } else {    // �����
        dataLevel = GlobalDefine::ntTable;
    }
    // ��������
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
        QString error = QStringLiteral("�������ݿ�ʧ�ܣ�[ԭ��]��%1").arg(*err);
        qDebug() << error;
    }

    return 0;
}

// ���ڴ����ݱ��浽xml�ļ�
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

    // ��������
    if (!generateSavedData(keyList, plane_system, tableRules, *err)) {
        if (err->isEmpty()) {
            *err = QStringLiteral("�����ڴ�����ʧ�ܣ�");
        }
        return -1;;
    }

    std::vector<int> datas;
    Icd::VehiclePtrArray vehicles;
    Json::Value configSql(Json::objectValue);
    Icd::SqlParser sqlParser(configSql);
    // ת������
    datas.push_back((int)&plane_system);
    datas.push_back((int)&tableRules);
    if (!sqlParser.convert2Core(datas, vehicles)) {
        *err = QStringLiteral("�����ڴ�����ʧ�ܣ�");
        return -1;;
    }
    // �����ļ�����
    Json::Value config;
    config["sourceType"] = "file";
    config["filePath"] = file.toStdString();
    Icd::ParserPtr parser = Icd::Parser::create(config);
    if (!parser) {
        return -1;
    }
    // ��������
    if (!parser->save(vehicles)) {
        *err = QStringLiteral("�ļ�д��ʧ�ܣ�");
    }

    return 0;
}

// �������ݿ�������Ϣ
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
        // ���³�ʼ�����ݿ�
        if (!q_dbaccess) {
            q_dbaccess = new DBAccess();
        }
        if (!q_dbaccess->init(option)) {
            QMessageBox::warning(parentWidget_, QStringLiteral("���ݿ����"),
                                 q_dbaccess->lastError().c_str());
            return -1;
        }
        // �������ԴΪ���ݿ⣬���ػ�������
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

// ��ѯ���ݿ������Ϣ
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

// ��ѯ�����
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

// ��ѯ�ֵ���
std::string DataManegement::dicName(GlobalDefine::DictionaryIndex dic) const
{
    std::string dicName;
    switch (dic) {
    case GlobalDefine::dicDataType:    // ��������
        dicName = DBAccess::tableName(DataBaseDefine::dbDataType);
        break;
    case GlobalDefine::dicPowerType:   // Ȩ������
        dicName = DBAccess::tableName(DataBaseDefine::dbPowerType);
        break;
    case GlobalDefine::dicArrayType:   // ��������
        dicName = DBAccess::tableName(DataBaseDefine::dbArrayType);
        break;
    case GlobalDefine::dicCheckType:   // У������
        dicName = DBAccess::tableName(DataBaseDefine::dbCheckType);
        break;
    default:break;
    }

    return dicName;
}

// ������Ҫ���������
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
    } else if (1 == keyList.size()) {  // �������
        if (!generatePlane(keyList.first().toInt(),
                           plane_system, tables, err)) {
            return false;
        }
    } else if (2 == keyList.size()) {   // ����ϵͳ
        int planeID = keyList.first().toInt();
        int systemID = keyList.last().toInt();
        if (!generateSystem(planeID, systemID,
                            plane_system, tables, err)) {
            return false;
        }
    } else {    // �����
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
            // ϵͳ
            if (!(system = systemVector[j])) {
                continue;
            }
            systems.push_back(system->system());
            tableVector = system->allTable();
            // ��
            const int size = tableVector.size();
            for (int k = 0; k < size; ++k) {
                if (!(table = tableVector[k])) {
                    continue;
                }
                if (table->isEmpty()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                    err = QStringLiteral("��������ڿձ�<%1>�������ƺ����ԣ�").arg(table->name().c_str());
                    return false;
#endif
                }
                if (table->lengthCheck()
                        && table->length() != (int)table->realLength()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                    err = QStringLiteral("��<%1>������岻�����������ƺ����ԣ�").arg(table->name().c_str());
                    return false;
#endif
                }
                if (table->hasEmptySubTable(nameTip)) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                    err = QStringLiteral("��<%1>�´��ڿ��ӱ������ƺ����ԣ�").arg(nameTip.c_str());
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
        // ϵͳ
        if (!(system = systemVector[i])) {
            continue;
        }
        systems.push_back(system->system());
        tableVector = system->allTable();
        // ��
        const int count = tableVector.size();
        for (int j = 0; j < count; ++j) {
            if (!(table = tableVector[j])) {
                continue;
            }
            if (table->isEmpty()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                err = QStringLiteral("��������ڿձ�<%1>�������ƺ����ԣ�").arg(table->name().c_str());
                return false;
#endif
            }
            if (table->lengthCheck() && table->length() != (int)table->realLength()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                err = QStringLiteral("��<%1>������岻�����������ƺ����ԣ�").arg(table->name().c_str());
                return false;
#endif
            }
            if (table->hasEmptySubTable(nameTip)) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                err = QStringLiteral("��<%1>�´��ڿ��ӱ������ƺ����ԣ�").arg(nameTip.c_str());
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
    // ��
    TableNode::smtTable table = 0;
    const int count = tableVector.size();
    std::string nameTip;
    for (int i = 0; i < count; ++i) {
        if (!(table = tableVector[i])) {
            continue;
        }
        if (table->isEmpty()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
            err = QStringLiteral("��������ڿձ�<%1>�������ƺ����ԣ�").arg(table->name().c_str());
            return false;
#endif
        }
        if (table->lengthCheck() && table->length() != (int)table->realLength()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
            err = QStringLiteral("��<%1>������岻�����������ƺ����ԣ�").arg(table->name().c_str());
            return false;
#endif
        }
        if (table->hasEmptySubTable(nameTip)) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
            err = QStringLiteral("��<%1>�´��ڿ��ӱ������ƺ����ԣ�").arg(nameTip.c_str());
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
        err = QStringLiteral("��������ڿձ�<%1>�������ƺ����ԣ�")
                .arg(table->name().c_str());
        return false;
#endif
    }
    if (table->lengthCheck()
            && table->length() != (int)table->realLength()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
        err = QStringLiteral("��<%1>������岻�����������ƺ����ԣ�").arg(table->name().c_str());
        return false;
#endif
    }
    if (table->hasEmptySubTable(nameTip)) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
        err = QStringLiteral("��<%1>�´��ڿ��ӱ������ƺ����ԣ�").arg(nameTip.c_str());
        return false;
#endif
    }
    if (!subId.empty()) {    // �ӱ�����
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

// �������ڴ�����ת����ͨ�ýṹ
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

// ��ͨ�ýṹת���ɹ����ڴ�����
ICDMetaData::ruleMap DataManegement::convert2IcdRules(
        const std::vector<stTableRules> &rules)
{
    QMap<int, ICDFrameCodeData::smtFrameCode> codes;    // ֡ʶ����
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
    // ��֡ʶ�����֡���ݽ�������
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

// �����������ݣ����¶������й�����ʶ��
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

// ����ϵͳ���ݣ����¶������й�����ʶ��
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

// ���������ݣ����¶����ӹ�����ʶ��
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
    std::unordered_map<int, ICDFrameCodeData::smtFrameCode> codes;    // ֡ʶ����
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
    // ��֡ʶ�����֡���ݽ�������
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

// �����������ݣ����¶����ӹ�����ʶ��
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

// ����xml�ļ���Ϣ
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
