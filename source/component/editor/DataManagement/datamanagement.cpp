#include "datamanagement.h"
#include "Dictionary.h"
#include "icdfactory.h"
#include "icdframecodedata.h"
#include "icdbitdata.h"
#include "icdwidget\icdwidget_global.h"

#if 0
#define _DEBUG_PRINT_D
#endif

//
J_EXTERN_C J_ATTR_EXPORT void *CreateComponent(void* attemper)
{
    return static_cast<IJComponent *>
            (new DataManegement(reinterpret_cast<IJAttempter *>(attemper)));
}

DataManegement::DataManegement(IJAttempter *attemper)
    : q_attempter(attemper)
    , q_dbaccess(NULL)
    , q_dataSource(GlobalDefine::dsNone)
{
    QTextCodec* code = QTextCodec::codecForName("System");
    if (NULL != code) {
        QTextCodec::setCodecForLocale(code);
    }
    // Ĭ���ļ�·��
    //     q_defaultPath = QStandardPaths::writableLocation(
    //         QStandardPaths::DesktopLocation).toStdString();
    q_defaultPath = readXmlFile();

    q_dbaccess = new DBAccess();
    if (GlobalDefine::dsDatabase == q_dataSource)
    {
        // ��ʼ�����ݿ�
        std::string file = jframeFacade()->configDirPath()
                .append("/jframe_global.xml");
        if (!q_dbaccess->init(file)) {
            QMessageBox::warning(jframeLayout()->mainWindow(),
                                 QStringLiteral("���ݿ����"),
                                 q_dbaccess->lastError().c_str());
        }
    }
    // ���ػ�������
    //loadBaseData();
}

DataManegement::~DataManegement()
{
}

bool DataManegement::loadInterface()
{
    bool result = true;

    // ������Ϣ
    q_attempter->notifier().beginGroup(this)
            .append("dataSourceChanged", &DataManegement::dataSourceChanged)
            .append("loadInfrastructure", &DataManegement::notifyRespond)
            .append("queryAllPlane", &DataManegement::notifyRespond)
            .append("querySinglePlane", &DataManegement::notifyRespond)
            .append("querySingleElement", &DataManegement::querySingleElement)
            .append("queryDictionary", &DataManegement::notifyRespond)
            .append("queryDictionaryTable", &DataManegement::notifyRespond)
            .append("queryDataSource", &DataManegement::queryDataSource)
            .append("queryMaxTableCount", &DataManegement::queryMaxTableCount)
            .endGroup();

    return result;
}

bool DataManegement::updateInterface()
{
    bool result = true;

    //TODO: Add your updating statements.

    return result;
}

void DataManegement::releaseInterface()
{
    // ȡ��������Ϣ
    q_attempter->notifier().beginGroup(this)
            .remove("dataSourceChanged")
            .remove("loadInfrastructure")
            .remove("queryAllPlane")
            .remove("querySinglePlane")
            .remove("querySingleElement")
            .remove("queryDictionary")
            .remove("queryDictionaryTable")
            .remove("queryDataSource")
            .remove("queryMaxTableCount")
            .endGroup();

    //
}

void* DataManegement::queryInterface(const std::string &iid, unsigned int ver)
{
    J_QUERY_INTERFACE(IJCommandSink, iid, ver);
    J_QUERY_INTERFACE(IJMessageSink, iid, ver);
    Q_UNUSED(iid);
    Q_UNUSED(ver);

    return 0;
}

std::string DataManegement::componentName() const
{
    return "DataManegement";
}

std::string DataManegement::componentDesc() const
{
    return "DataManegement";
}

void DataManegement::attach()
{
    // ������Ϣ
    q_attempter->notifier().beginGroup(this)
            .append("loadRules", &DataManegement::notifyRespond)
            .append("unLoadRules", &DataManegement::notifyRespond)
            .append("modifyAllPlane", &DataManegement::notifyRespond)
            .append("savePlane", &DataManegement::notifyRespond)
            .append("modifyPlane", &DataManegement::notifyRespond)
            .append("deletePlane", &DataManegement::notifyRespond)
            .append("saveSystem", &DataManegement::notifyRespond)
            .append("modifySystem", &DataManegement::notifyRespond)
            .append("deleteSystem", &DataManegement::notifyRespond)
            .append("saveTable", &DataManegement::notifyRespond)
            .append("modifyTable", &DataManegement::notifyRespond)
            .append("deleteTable", &DataManegement::notifyRespond)
            .append("saveRule", &DataManegement::notifyRespond)
            .append("insertRule", &DataManegement::notifyRespond)
            .append("deleteRule", &DataManegement::notifyRespond)
            .append("deleteSubTable", &DataManegement::notifyRespond)
            .append("copyMemoryData", &DataManegement::saveCopyData)
            .append("saveDatabase", &DataManegement::saveDatabase)
            .append("saveFile", &DataManegement::saveFile)
            .append("queryDBError", &DataManegement::queryDBError)
            .endGroup();

    //TODO: Add your statements.
}

void DataManegement::detach()
{
    //TODO: Add your statements.

    // ȡ��������Ϣ
    q_attempter->notifier().beginGroup(this)
            .remove("loadRules")
            .remove("unLoadRules")
            .remove("modifyAllPlane")
            .remove("savePlane")
            .remove("modifyPlane")
            .remove("deletePlane")
            .remove("saveSystem")
            .remove("modifySystem")
            .remove("deleteSystem")
            .remove("saveTable")
            .remove("modifyTable")
            .remove("deleteTable")
            .remove("saveRule")
            .remove("insertRule")
            .remove("deleteRule")
            .remove("deleteSubTable")
            .remove("copyMemoryData")
            .remove("saveDatabase")
            .remove("saveFile")
            .remove("queryDBError")
            .endGroup();
}

bool DataManegement::commandSink(void *sender, const std::string &domain, const std::string &objectName,
                                 const std::string &eventType, void *data)
{
    Q_UNUSED(domain);
    Q_UNUSED(objectName);
    Q_UNUSED(eventType);
    Q_UNUSED(data);

    QObject *objectSender = reinterpret_cast<QObject *>(sender);
    if (!objectSender) {
        return false;
    }

    //TODO: Add your statements.

    return false;
}

bool DataManegement::messageSink(IJComponent *sender, const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(sender);
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    //TODO: Add your statements.

    return false;
}

std::string DataManegement::observerId() const
{
    return componentName();
}

// ��Ϣ��Ӧ�ص�
JLRESULT DataManegement::notifyRespond(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    if ("loadInfrastructure" == id) { // ��ѯ���еĻ�������
        if (lParam) {
            QString &error = *reinterpret_cast<QString *>(lParam);
            switch (wParam) {
            case GlobalDefine::dsDatabase: {
                // ���ػ����ṹ����
                std::string err;
                if (!loadBaseData(err)) {
                    error = err.c_str();
                }
            }
                break;
            case GlobalDefine::dsFile: {
//                 QString file = QFileDialog::getOpenFileName(
//                     jframeLayout()->mainWindow(), QStringLiteral("��"),
//                     q_defaultPath.c_str(), "XML files (*.xml);;JSON files (*.json)");
//                 if (file.isEmpty()) {
//                     break;
//                 }
//                 q_defaultPath = file.toStdString();
                // �����ļ�����
                Icd::VehiclePtrArray vehicles;
                Icd::FileParser fileParser(q_defaultPath);
                // ��������
                if (!fileParser.parseVehicle(vehicles, Icd::ObjectTable)) {
                    error = QStringLiteral("�����ļ�ʧ�ܣ�");
                    break;
                }
                // ת������
                Icd::SqlParser sqlParser(std::string(""));
                DMSpace::_vectorPS infrastructure;
                DMSpace::_vectorIcdTR tableRules;
                std::vector<int> params;
                params.push_back((int)&infrastructure);
                params.push_back((int)&tableRules);

                if (!sqlParser.convert2Other(vehicles, params)) {
                    error = QStringLiteral("�����ļ�ʧ�ܣ�");
                    break;
                }
                if (!loadXmlBaseData(infrastructure, tableRules)) {
                    error = QStringLiteral("�����ļ�ʧ�ܣ�");
                }
            }
                break;
            default:break;
            }
        }
    } else if("queryAllPlane" == id) { // ��ѯ���еĻ�������
        if (wParam) {
            PlaneNode::planeVector& data
                    = *reinterpret_cast<PlaneNode::planeVector*>(wParam);

            data = q_planes;
        }
    } else if ("querySinglePlane" == id) { // ��ѯһ����������
        if (wParam) {
            PlaneNode::smtPlane& data = *reinterpret_cast<PlaneNode::smtPlane*>(wParam);

            data = planeNode(lParam);
        }
    } else if ("queryDictionary" == id) { // ��ѯ�ֵ��
        if (wParam) {
            stQueryDic &data = *reinterpret_cast<stQueryDic*>(wParam);

            queryDictionary(data);
        }
    } else if ("queryDictionaryTable" == id) { // ��ѯһ���ֵ��
        if (!wParam
                || lParam > GlobalDefine::dicCheckType
                || lParam < GlobalDefine::dicDataType) {
            return 0;
        }
        std::vector<stDictionary> &dics
                = *reinterpret_cast<std::vector<stDictionary> *>(wParam);
        std::string dicName;
        switch (lParam) {
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
        default:break;
        }

        dics = CDictionary::instance().singleDic(dicName);
    } else if ("loadRules" == id) {    // ���ع�������
        if (wParam && lParam) {
            PlaneNode::smtPlane &data
                    = *reinterpret_cast<PlaneNode::smtPlane*>(wParam);
            const std::vector<int> &params
                    = *reinterpret_cast<std::vector<int> *>(lParam);
            if (2 != params.size()) {
                return 0;
            }
            const QString &condition = *reinterpret_cast<QString *>(params[0]);
            bool increment = params[1];
            loadRules(data, condition, increment);
        }
    } else if ("unLoadRules" == id) {    // ж�ع�������
        if (wParam) {
            const QString &condition
                    = *reinterpret_cast<QString*>(wParam);

            unLoadRules(condition);
        }
    } else if ("modifyAllPlane" == id) { // �޸����л�������
        if (wParam && lParam) {
            const std::vector<ICDElement::smtElement> elements
                    = *reinterpret_cast<std::vector<ICDElement::smtElement> *>(wParam);
            bool &result = *reinterpret_cast<bool *>(lParam);
            const int count = elements.size();
            std::vector<PlaneNode::smtPlane> planes;
            for (int i = 0; i < count; ++i) {
                planes.push_back(std::dynamic_pointer_cast<PlaneNode>(elements[i]));
            }
            // �������л�������
            result = modifyAllKC(planes);
        }
    } else if ("savePlane" == id) { // �����������
        if (wParam && lParam) {
            stPlane &plane = *reinterpret_cast<stPlane *>(wParam);
            bool &result = *reinterpret_cast<bool *>(lParam);
            // ��������
            result = savePlane(plane);
        }
    } else if ("modifyPlane" == id) {   // �޸Ļ�������
        if (wParam && lParam) {
            ICDElement::smtElement element
                    = *reinterpret_cast<ICDElement::smtElement *>(wParam);
            bool &result = *reinterpret_cast<bool *>(lParam);
            // �޸�����
            result = modifyOneKC(SMT_CONVERT(PlaneNode, element));
        }
    } else if ("deletePlane" == id) {   // ɾ����������
        if (wParam && lParam) {
            std::vector<int> &planeID
                    = *reinterpret_cast<std::vector<int> *>(wParam);
            bool &result = *reinterpret_cast<bool *>(lParam);
            // ɾ������
            result = deletePlane(planeID);
        }
    } else if ("saveSystem" == id) {    // ����ϵͳ����
        if (wParam && lParam) {
            std::vector<void*> &params
                    = *reinterpret_cast<std::vector<void*> *>(wParam);
            if (params.size() != 2) {
                return 0;
            }
            QString &planeID = *reinterpret_cast<QString *>(params.at(0));
            std::vector<stSystem> &systems
                    = *reinterpret_cast<std::vector<stSystem> *>(params.at(1));
            bool &result = *reinterpret_cast<bool *>(lParam);
            // ��������
            result = saveSystem(planeID.toInt(), systems);
        }
    } else if ("modifySystem" == id) {    // �޸�ϵͳ����
        if (wParam && lParam) {
            std::vector<void*> &params
                    = *reinterpret_cast<std::vector<void*> *>(wParam);
            if (params.size() != 2) {
                return 0;
            }
            int &planeID = *reinterpret_cast<int *>(params.at(0));
            ICDElement::smtElement element
                    = *reinterpret_cast<ICDElement::smtElement *>(params.at(1));
            bool &result = *reinterpret_cast<bool *>(lParam);
            // �޸�����
            result = modifySystem(planeID, SMT_CONVERT(SystemNode, element));
        }
    } else if ("deleteSystem" == id) {  // ɾ��ϵͳ����
        if (wParam && lParam) {
            std::vector<void*> &params
                    = *reinterpret_cast<std::vector<void*> *>(wParam);
            if (params.size() != 2) {
                return 0;
            }
            QString &planeID = *reinterpret_cast<QString *>(params.at(0));
            std::vector<int> &systemID
                    = *reinterpret_cast<std::vector<int> *>(params.at(1));
            bool &result = *reinterpret_cast<bool *>(lParam);
            // ɾ������
            result = deleteSystem(planeID.toInt(), systemID);
        }
    } else if ("saveTable" == id) { // ����ICD������
        if (wParam && lParam) {
            std::vector<void*> &params
                    = *reinterpret_cast<std::vector<void*> *>(wParam);
            if (params.size() != 2) {
                return 0;
            }
            QString &keys = *reinterpret_cast<QString *>(params.at(0));
            std::vector<stICDBase> &tables
                    = *reinterpret_cast<std::vector<stICDBase> *>(params.at(1));
            bool &result = *reinterpret_cast<bool *>(lParam);
            // ��������
            result = saveTable(keys, tables);
        }
    } else if ("modifyTable" == id) { // �޸�ICD������
        if (wParam && lParam) {
            std::vector<void*> &params
                    = *reinterpret_cast<std::vector<void*> *>(wParam);
            if (params.size() != 2) {
                return 0;
            }
            QString &keys = *reinterpret_cast<QString *>(params.at(0));
            ICDElement::smtElement element
                    = *reinterpret_cast<ICDElement::smtElement *>(params.at(1));
            bool &result = *reinterpret_cast<bool *>(lParam);
            // �޸�����
            result = modifyTable(keys, SMT_CONVERT(TableNode, element));
        }
    } else if ("deleteTable" == id) {
        if (wParam && lParam) { // ɾ��ICD������
            std::vector<void*> &params
                    = *reinterpret_cast<std::vector<void*> *>(wParam);
            if (params.size() != 2) {
                return 0;
            }
            QString &keys = *reinterpret_cast<QString *>(params.at(0));
            std::vector<std::string> tables
                    = *reinterpret_cast<std::vector<std::string> *>(params.at(1));
            bool &result = *reinterpret_cast<bool *>(lParam);
            // ɾ������
            result = deleteTable(keys, tables);
        }
    } else if ("saveRule" == id) {
        if (wParam && lParam) {
            std::vector<void*> &params
                    = *reinterpret_cast<std::vector<void*> *>(wParam);
            if (params.size() != 2) {
                return 0;
            }
            QString &keys = *reinterpret_cast<QString *>(params.at(0));
            ICDMetaData::smtMeta &meta
                    = *reinterpret_cast<ICDMetaData::smtMeta *>(params.at(1));
            bool &result = *reinterpret_cast<bool *>(lParam);
            // �������ݿ�
            result = saveRule(keys, meta);
        }
    } else if ("insertRule" == id) {
        if (wParam && lParam) {
            std::vector<void*> &params
                    = *reinterpret_cast<std::vector<void*> *>(wParam);
            if (params.size() != 2) {
                return 0;
            }
            QString &keys = *reinterpret_cast<QString *>(params.at(0));
            ICDMetaData::smtMeta &meta
                    = *reinterpret_cast<ICDMetaData::smtMeta *>(params.at(1));
            bool &result = *reinterpret_cast<bool *>(lParam);
            // �������ݿ�
            result = insertRule(keys, meta);
        }
    } else if ("deleteRule" == id) {    // ɾ����������
        if (wParam && lParam) {
            std::vector<void*> &params
                    = *reinterpret_cast<std::vector<void*> *>(wParam);
            if (params.size() != 2) {
                return 0;
            }
            QString &keys = *reinterpret_cast<QString *>(params.at(0));
            std::vector<int> &rules
                    = *reinterpret_cast <std::vector<int> *>(params.at(1));
            bool &result = *reinterpret_cast<bool *>(lParam);
            // ɾ�����ݿ�
            result = deleteRule(keys, rules);
        }
    } else if ("deleteSubTable" == id) {    // ɾ��֡ʶ�����ӱ�
        if (wParam && lParam) {
            std::vector<void*> &params
                    = *reinterpret_cast<std::vector<void*> *>(wParam);
            if (params.size() != 2) {
                return 0;
            }
            QString &keys = *reinterpret_cast<QString *>(params.at(0));
            std::vector<std::string> &rules
                    = *reinterpret_cast <std::vector<std::string> *>(params.at(1));
            bool &result = *reinterpret_cast<bool *>(lParam);
            // ɾ�����ݿ�
            result = deleteSubTable(keys, rules);
        }
    }

    return 0;
}

/**
 * @brief ���ػ�������
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DataManegement::loadBaseData(std::string &error)
{
    // �������
    clearKC();

    // ��ʼ���ֵ������
    if (!loadDic()) {
        return false;
    }

    // ��ʼ��ϵͳ��Ϣ
    if (!loadInfrastructure()) {
        error = q_dbaccess->lastError().c_str();
        jframeLogWarning(error.c_str());
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
    std::map<std::string, TableNode::tableVector> classified;
    for (int i = 0; i < count; ++i) {
        if (table = tables[i]) {
            stICDBase icdBase = table->icdBase();
            classified[icdBase.sGroup].push_back(table);
        }
    }

    // �������ϵͳ����
    std::string group;
    std::map<std::string, TableNode::tableVector>::iterator
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
    std::map<std::string, std::vector<stDictionary> > dic;
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
    std::map<std::string, std::vector<stDictionary> > data;
    
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
 * @return ִ�н����true���ɹ���false��ʧ��
 */
bool DataManegement::loadInfrastructure()
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
    std::map<int, std::vector<stSystem> > systemBase;
    if (!q_dbaccess->readSystem(systemBase)) {
        qDebug() <<"readSystem failed!";
        return false;
    }
    // ��ѯ�������Ϣ
    std::map<std::string, std::vector<stICDBase>> icdBase;
    if (!q_dbaccess->readICDBase(true, icdBase)) {
        qDebug() <<"readICDBase failed!";
        return false;
    }
    std::map<std::string, std::vector<stICDBase>>::iterator itBase
            = icdBase.end();
    std::map<int, std::vector<stSystem> >::iterator it
            = systemBase.end();
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
    std::map<std::string, TableNode::tableVector> rules
            = loadXmlRuleData(tableRules);
    std::map<std::string, TableNode::tableVector>::iterator it = rules.begin();
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
    std::map<std::string, TableNode::tableVector> rules
            = loadXmlRuleData(tableRules);
    const std::string group = QString("%1/%2")
            .arg(planeType).arg(system).toStdString();
    std::map<std::string, TableNode::tableVector>::iterator it
            = rules.find(group);
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
    std::map<std::string, TableNode::tableVector> rules
            = loadXmlRuleData(tableRules);
    const std::string group = QString("%1/%2")
            .arg(plane).arg(system).toStdString();
    std::map<std::string, TableNode::tableVector>::iterator it
            = rules.find(group);
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
    std::map<int, ICDFrameCodeData::smtFrameCode> codes;    // ֡ʶ����
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
    std::map<int, ICDFrameCodeData::smtFrameCode>::iterator itC;
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
    std::map<int, TableNode::tableVector> system_table;
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
        std::map<int, TableNode::tableVector>::iterator itIS
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
        std::map<int, TableNode::tableVector>::iterator itIS
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
    std::map<int, ICDFrameCodeData::smtFrameCode> codes;    // ֡ʶ����
    std::map<int, ICDFrameCodeData::smtFrameCode>::iterator itC = codes.begin();
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
std::map<std::string, TableNode::tableVector> DataManegement::loadXmlRuleData(
        const DMSpace::_vectorIcdTR &tableRules)
{
    //
    std::map<std::string, TableNode::tableVector> result;
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
            result = loadSystemRule(items.at(0).toInt(),
                                    items.at(1).toInt(), increment);
            break;
        case 3:
            result = loadTableRule(items.at(0).toInt(),
                                   items.at(1).toInt(),
                                   items.at(2).toStdString());
            break;
        default:
            break;
        }
    } else if (GlobalDefine::dsFile == q_dataSource) {
        // ����ԴΪxml�ļ�// �����ļ�����
        Icd::VehiclePtr vehicle;
        Icd::VehiclePtrArray vehicles;
        Icd::FileParser fileParser(q_defaultPath);
        // ��������
        switch (items.size()) {
        case 1: // ����
            if (!fileParser.parseVehicle(items.first().toStdString(),
                                         vehicle, Icd::ObjectItem)) {
                break;
            }
            break;
        case 2: // ϵͳ
        {
            Icd::SystemPtr system(new Icd::System());
            if (!fileParser.parseSystem(items[0].toStdString(),
                                        items[1].toStdString(), system, Icd::ObjectItem)) {
                break;
            }
            vehicle = Icd::VehiclePtr(new Icd::Vehicle());
            vehicle->setId(items[0].toStdString());
            vehicle->appendSystem(system);
        }
            break;
        case 3: // ��
        {
            Icd::TablePtr table(new Icd::Table());
            if (!fileParser.parseTable(items[0].toStdString(),
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
        }
            break;
        default:
            break;
        }
        if (!vehicle) {
            return result;
        }

        vehicles.push_back(vehicle);
        // ת������
        Icd::SqlParser sqlParser(std::string(""));
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
            result = loadXmlSystemRule(items[0].toInt(),
                    items[1].toInt(), tableRules);
            break;
        case 3:
            result = loadXmlTablemRule(items[0].toInt(), items[1].toInt(),
                    items[2].toStdString(), tableRules);
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
    case 1:
        unLoadTypeRule(items.at(0).toInt());
        break;
    case 2:
        unLoadSystemRule(items.at(0).toInt(), items.at(1).toInt());
        break;
    case 3:
        unLloadTableRule(items.at(0).toInt(),
                         items.at(1).toInt(),
                         items.at(2).toStdString());
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
bool DataManegement::saveSystem(int plane,
                                const std::vector<stSystem>& system)
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
            smtPlane->addSystem(SystemNode::smtSystem(
                                    new SystemNode(stData)));
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
bool DataManegement::saveTable(const QString &keys,
                               const std::vector<stICDBase> &tables)
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
bool DataManegement::deleteTable(const QString &keys,
                                 const std::vector<std::string>& tables)
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
bool DataManegement::saveRule(const QString &keys,
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
        if (!(table = table->subTable(keyLst.join("/").toStdString()))) {
            return false;
        }
    }
    // �����֡ʶ���룬�Ȱ�֡����
    ICDFrameCodeData::smtFrameCode frameCode
            = std::dynamic_pointer_cast<ICDFrameCodeData>(meta);
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
    } else {    // �������߲�δ������ݳ���
        // �����ڴ�
        table->addRule(meta);
    }
    // ���±���
    if (keyLst.size() > 0) {    // �ӱ�����
        topTable->updateLength(keyLst.join("/").toStdString());
    }

    return true;
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
JLRESULT DataManegement::querySingleElement(const std::string &id,
                                            JWPARAM wParam,
                                            JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!wParam || !lParam) {
        return JFrame::ResultError;
    }
    ICDElement::smtElement &element
            = *reinterpret_cast<ICDElement::smtElement *>(wParam);
    const QString &condition = *reinterpret_cast<QString *>(lParam);
    QStringList keyList = condition.split("/", QString::SkipEmptyParts);
    if (keyList.empty()) {
        return JFrame::ResultError;
    } else if (1 == keyList.size()) {   // ����
        element = planeNode(keyList.first().toInt());
    } else if (2 == keyList.size()) {   // ϵͳ
        PlaneNode::smtPlane plane = planeNode(keyList.takeFirst().toInt());
        if (!plane) {
            return JFrame::ResultError;
        }
        element = plane->system(keyList.takeFirst().toInt());
    } else if (3 == keyList.size()) {  // ��
        PlaneNode::smtPlane plane = planeNode(keyList.takeFirst().toInt());
        if (!plane) {
            return JFrame::ResultError;
        }
        SystemNode::smtSystem system
                = plane->system(keyList.takeFirst().toInt());
        if (!system) {
            return JFrame::ResultError;
        }
        element = system->table(keyList.takeFirst().toStdString());
    } else {    // ����
        PlaneNode::smtPlane plane = planeNode(keyList.takeFirst().toInt());
        if (!plane) {
            return JFrame::ResultError;
        }
        SystemNode::smtSystem system
                = plane->system(keyList.takeFirst().toInt());
        if (!system) {
            return JFrame::ResultError;
        }
        TableNode::smtTable table
                = system->table(keyList.takeFirst().toStdString());
        if (!table) {
            return JFrame::ResultError;
        }
        QString lastItem = keyList.last();
        if (lastItem.contains("ICDTable_")) {
            // �ӱ�
            element = table->subTable(keyList.join("/").toStdString());
        } else {
            // ��������
            keyList.takeLast();
            if (!keyList.isEmpty()) {
                table = table->subTable(keyList.join("/").toStdString());
                if (!table) {
                    return JFrame::ResultError;
                }
            }
            // ��������
            element = table->rule(lastItem.toInt());
        }
    }

    return JFrame::ResultSuccess;
}

// ��ѯ��������Դ
JLRESULT DataManegement::queryDataSource(const std::string &id,
                                         JWPARAM wParam,
                                         JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);
    if (!wParam) {
        return JFrame::ResultError;
    }
    int &result = *reinterpret_cast<int *>(wParam);

    result = q_dataSource;

    return JFrame::ResultSuccess;
}

// ��ѯ�ӱ�����
JLRESULT DataManegement::queryMaxTableCount(const std::string &id,
                                            JWPARAM wParam,
                                            JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);
    if (!wParam) {
        return JFrame::ResultError;
    }
    int &result = *reinterpret_cast<int *>(wParam);

    result = q_dbaccess->tableLimitation();

    return JFrame::ResultSuccess;
}

// ���濽������
JLRESULT DataManegement::saveCopyData(const std::string &id,
                                      JWPARAM wParam,
                                      JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!wParam || !lParam) {
        return JFrame::ResultError;
    }
    QVector<int> &params = *reinterpret_cast<QVector<int> *>(wParam);
    ICDElement::smtElement &result
            = *reinterpret_cast<ICDElement::smtElement *>(lParam);
    if (params.size() < 3) {
        return JFrame::ResultError;
    }
    // ��ѯԴ����
    ICDElement::smtElement element;
    querySingleElement("querySingleElement", (JWPARAM)&element, params[0]);
    if (!element) {
        return JFrame::ResultError;
    }
    QString lastItem;
    TableNode::smtTable topTable = 0;
    ICDElement::smtElement desElement;
    QString dest = *reinterpret_cast<QString *>(params[2]);
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
        querySingleElement("querySingleElement",
                           (JWPARAM)&desElement, (JLPARAM)&dest);
        if (!desElement) {
            return JFrame::ResultError;
        }
    }
    // ��������
    if (GlobalDefine::ntPlane == element->objectType()) {
        QString id;
        // ����
        const stPlane &base = *reinterpret_cast<stPlane *>(params[1]);
        q_attempter->notifier().sendMessage("queryID@ICDNavigation",
                                            (WPARAM)&id,
                                            GlobalDefine::ntPlane);
        // ��¡����
        PlaneNode::smtPlane plane = SMT_CONVERT(PlaneNode, element)->clone();
        if (!plane) {
            return JFrame::ResultError;
        }
        plane->setPlane(base);
        copyPlaneData(plane, id.toInt());
        addOneKC(plane);
        result = plane;
    } else if (GlobalDefine::ntSystem == element->objectType()) {
        QString id;
        q_attempter->notifier().sendMessage("queryID@ICDNavigation",
                                            (WPARAM)&id,
                                            GlobalDefine::ntSystem);
        // ϵͳ
        const stSystem &base = *reinterpret_cast<stSystem *>(params[1]);
        SystemNode::smtSystem system = SMT_CONVERT(SystemNode, element)->clone();
        PlaneNode::smtPlane plane = SMT_CONVERT(PlaneNode, desElement);
        if (!system || !plane) {
            return JFrame::ResultError;
        }
        stICDBase icdBase;
        icdBase.sGroup = dest.toStdString();
        system->setSystem(base);
        copySystemData(system, plane->numeralId(), id.toInt());
        plane->addSystem(system);
        result = system;
    } else if (GlobalDefine::ntTable == element->objectType()) {
        // ��
        const stICDBase &base = *reinterpret_cast<stICDBase *>(params[1]);
        TableNode::smtTable table = SMT_CONVERT(TableNode, element)->clone();
        if (!table) {
            return JFrame::ResultError;
        }
        if (lastItem.isEmpty()) {   // �����
            SystemNode::smtSystem system = SMT_CONVERT(SystemNode, desElement);
            if (!system) {
                return JFrame::ResultError;
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
                return JFrame::ResultError;
            }
            ICDComplexData::smtComplex complex
                    = SMT_CONVERT(ICDComplexData, destTable->rule(lastItem.toInt()));
            if (!complex) {
                return JFrame::ResultError;
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
        result = table;
    } else if (GlobalDefine::ntRule == element->objectType()) {
        // ����
        ICDCommonData::smtCommon data
                = SMT_CONVERT(ICDCommonData,
                              *reinterpret_cast<ICDMetaData::smtMeta *>(params[1]));
        ICDMetaData::smtMeta meta = SMT_CONVERT(ICDMetaData, element);
        TableNode::smtTable table = SMT_CONVERT(TableNode, desElement);
        if (!data || !meta || !table) {
            return JFrame::ResultError;
        }
        ICDCommonData::smtCommon common;
        // ����û��ı��������ͣ���ʹ�ý������ݣ�����ʹ��Դ����
        if (data->type() == meta->type()) {
            if (!(common = SMT_CONVERT(ICDCommonData, meta->clone()))) {
                return JFrame::ResultError;
            }
            ICDFactory::instance().copyBaseData(data, common);
            copyRuleData(SMT_CONVERT(ICDMetaData, common), table->icdBase());
        } else {
            common = SMT_CONVERT(ICDCommonData, data->clone());
        }
        table->addRule(common);
        result = common;
        // ���±���
        if (topTable) {
            topTable->updateLength(subKey);
        }
    }

    return JFrame::ResultSuccess;
}

// ���ڴ����ݱ��浽���ݿ�
JLRESULT DataManegement::saveDatabase(const std::string &id,
                                      JWPARAM wParam,
                                      JLPARAM lParam)
{
    Q_UNUSED(id);
    // ��ѯ����Դ��������ļ�����������һ����
    // ������������㼶��ɾ���ڴ��в����ڵ����ݣ�
    if (!wParam || !lParam) {
        return JFrame::ResultError;
    }
    QString &keys = *reinterpret_cast<QString *>(wParam);
    QString &err = *reinterpret_cast<QString *>(lParam);
    if (GlobalDefine::dsNone == q_dataSource) {
        return JFrame::ResultSuccess;
    }

    DMSpace::_vectorPS plane_system;
    DMSpace::_vectorIcdTR tableRules;
    QStringList keyList = keys.split("/", QString::SkipEmptyParts);
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
    if (!generateSavedData(keyList, plane_system, tableRules, err)) {
        return JFrame::ResultError;
    }

    if (!q_dbaccess->isOpen()) {
        std::string file = jframeFacade()->configDirPath()
                .append("/jframe_global.xml");
        q_dbaccess->init(file);
    }

    if (!q_dbaccess->savePlane(plane_system,
                               tableRules,
                               dataLevel,
                               GlobalDefine::dsDatabase == q_dataSource)) {
        err = q_dbaccess->lastError().c_str();
        QString error = QStringLiteral("�������ݿ�ʧ�ܣ�[ԭ��]��%1").arg(err);
        jframeLogWarning(error.toLocal8Bit().data());
    }

    return JFrame::ResultSuccess;
}

// ���ڴ����ݱ��浽xml�ļ�
JLRESULT DataManegement::saveFile(const std::string &id,
                                  JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!wParam || !lParam) {
        return JFrame::ResultError;
    }
    QVector<QString> &params = *reinterpret_cast<QVector<QString> *>(wParam);
    if (params.size() != 2) {
        return JFrame::ResultError;
    }
    const QString &keys = params.at(0);
    const QString &file = params.at(1);
    QString &err = *reinterpret_cast<QString *>(lParam);
    DMSpace::_vectorPS plane_system;
    DMSpace::_vectorIcdTR tableRules;
    QStringList keyList = keys.split("/", QString::SkipEmptyParts);

    // ��������
    if (!generateSavedData(keyList, plane_system, tableRules, err)) {
        if (err.isEmpty()) {
            err = QStringLiteral("�����ڴ�����ʧ�ܣ�");
        }
        return JFrame::ResultError;
    }

    std::vector<int> datas;
    Icd::VehiclePtrArray vehicles;
    Icd::SqlParser sqlParser(std::string(""));
    // ת������
    datas.push_back((int)&plane_system);
    datas.push_back((int)&tableRules);
    if (!sqlParser.convert2Core(datas, vehicles)) {
        err = QStringLiteral("�����ڴ�����ʧ�ܣ�");
        return JFrame::ResultError;
    }
    // �����ļ�����
    Icd::FileParser fileParser(file.toStdString());
    // ��������
    if (!fileParser.save(vehicles)) {
        err = QStringLiteral("�ļ�д��ʧ�ܣ�");
    }

    return JFrame::ResultSuccess;
}

// �������ݿ�������Ϣ
JLRESULT DataManegement::dataSourceChanged(const std::string &id,
                                           JWPARAM wParam,
                                           JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!wParam) {
        return JFrame::ResultError;
    }
    const QString &type = *reinterpret_cast<QString *>(wParam);
    if ("database" == type) {
        q_dataSource = GlobalDefine::dsDatabase;
        // ���³�ʼ�����ݿ�
        std::string file = jframeFacade()->configDirPath()
                .append("/jframe_global.xml");
        if (!q_dbaccess) {
            q_dbaccess = new DBAccess();
        }
        if (!q_dbaccess->init(file)) {
            QMessageBox::warning(jframeLayout()->mainWindow(),
                                 QStringLiteral("���ݿ����"),
                                 q_dbaccess->lastError().c_str());
            return JFrame::ResultError;
        }
        // �������ԴΪ���ݿ⣬���ػ�������
        //     if (GlobalDefine::dsDatabase == q_dataSource) {
        loadBaseData();
        //    }
    } else if (QStringLiteral("�ļ�") == type) {
        q_dataSource = GlobalDefine::dsFile;
        if (!lParam) {
            return JFrame::ResultError;
        }
        QString error;
        const QString &file = *reinterpret_cast<QString *>(lParam);
        q_defaultPath = file.toStdString();

        return notifyRespond("loadInfrastructure",
                             GlobalDefine::dsFile, (JLPARAM)&error);
    } else {
        q_dataSource = GlobalDefine::dsNone;
    }

    return JFrame::ResultSuccess;
}

// ��ѯ���ݿ������Ϣ
JLRESULT DataManegement::queryDBError(const std::string &id,
                                      JWPARAM wParam,
                                      JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);
    if (!wParam) {
        return JFrame::ResultError;
    }
    QString &error = *reinterpret_cast<QString *>(wParam);

    error = q_dbaccess->lastError().c_str();

    return JFrame::ResultSuccess;
}

// ��ѯ�����
TableNode::smtTable DataManegement::rootTable(int plane, 
                                              int system,
                                              const std::string &table)
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
                    err = QStringLiteral("��������ڿձ�<%1>�������ƺ����ԣ�")
                        .arg(table->name().c_str());
                    return false;
#endif
                }
                if (table->lengthCheck()
                        && table->length() != (int)table->realLength()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                    err = QStringLiteral("��<%1>������岻�����������ƺ����ԣ�")
                            .arg(table->name().c_str());
                    return false;
#endif
                }
                if (table->hasEmptySubTable(nameTip)) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                    err = QStringLiteral("��<%1>�´��ڿ��ӱ������ƺ����ԣ�")
                        .arg(nameTip.c_str());
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
                err = QStringLiteral("��������ڿձ�<%1>�������ƺ����ԣ�")
                    .arg(table->name().c_str());
                return false;
#endif
            }
            if (table->lengthCheck()
                    && table->length() != (int)table->realLength()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                err = QStringLiteral("��<%1>������岻�����������ƺ����ԣ�")
                        .arg(table->name().c_str());
                return false;
#endif
            }
            if (table->hasEmptySubTable(nameTip)) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
                err = QStringLiteral("��<%1>�´��ڿ��ӱ������ƺ����ԣ�")
                    .arg(nameTip.c_str());
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

bool DataManegement::generateSystem(int planeId,
                                    int systemId,
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
            err = QStringLiteral("��������ڿձ�<%1>�������ƺ����ԣ�")
                .arg(table->name().c_str());
            return false;
#endif
        }
        if (table->lengthCheck()
                && table->length() != (int)table->realLength()) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
            err = QStringLiteral("��<%1>������岻�����������ƺ����ԣ�")
                    .arg(table->name().c_str());
            return false;
#endif
        }
        if (table->hasEmptySubTable(nameTip)) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
            err = QStringLiteral("��<%1>�´��ڿ��ӱ������ƺ����ԣ�")
                .arg(nameTip.c_str());
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
        err = QStringLiteral("��<%1>������岻�����������ƺ����ԣ�")
                .arg(table->name().c_str());
        return false;
#endif
    }
    if (table->hasEmptySubTable(nameTip)) {
#if (DEF_IGNORE_EMPTY_TABLE != 1)
        err = QStringLiteral("��<%1>�´��ڿ��ӱ������ƺ����ԣ�")
            .arg(nameTip.c_str());
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
    for (int l = 0; l < number; l++) {
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
        ICDMetaData::smtMeta metaData
                = ICDFactory::instance().CreatObject(ruleData);
        if (NULL != metaData) {
            result[ruleData.nSerial] = metaData;
            if (GlobalDefine::dtFrameCode == ruleData.uType) {
                codes[atoi(ruleData.sDefault.c_str())]
                        = SMT_CONVERT(ICDFrameCodeData, metaData);
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
            ICDComplexData::smtComplex complex =
                    SMT_CONVERT(ICDComplexData, it->second);
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
    icdBase.sGroup = QString("%1/%2").arg(planeId)
            .arg(systemId).toStdString();
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
bool DataManegement::copyTableData(TableNode::smtTable &table,
                                   const stICDBase &icdBase)
{
    if (!table) {
        return false;
    }

    stICDBase base = table->icdBase();
    base.sID = QUuid::createUuid()
            .toString().remove(QRegExp("[{}-]")).toStdString();
    base.sName = std::string("ICDTable_").append(base.sID);
    base.sParentName = icdBase.sName;
    base.sGroup = icdBase.sGroup;
    table->setICDBase(base);
    ICDMetaData::ruleMap rules = table->allRule();
    ICDMetaData::ruleMap::iterator it = rules.begin();
    std::map<int, ICDFrameCodeData::smtFrameCode> codes;    // ֡ʶ����
    for (; it != rules.end(); ++it) {
        ICDCommonData::smtCommon common
                = SMT_CONVERT(ICDCommonData, it->second);
        if (common) {
            if (GlobalDefine::dtFrameCode == common->type()) {
                codes[atoi(common->defaultStr().c_str())]
                        = SMT_CONVERT(ICDFrameCodeData, common);
            }
        }
        if (!copyRuleData(it->second, base)) {
            return false;
        }
    }
    // ��֡ʶ�����֡���ݽ�������
    std::map<int, ICDFrameCodeData::smtFrameCode>::iterator itC;
    for (itC = codes.begin(); itC != codes.end(); ++itC) {
        ICDFrameCodeData::smtFrameCode &frameCode = itC->second;
        ICDComplexData::smtComplex complex =
                SMT_CONVERT(ICDComplexData, table->rule(itC->first));
        if (frameCode && complex) {
            frameCode->bindData(complex);
        }
    }

    return true;
}

// �����������ݣ����¶����ӹ�����ʶ��
bool DataManegement::copyRuleData(ICDMetaData::smtMeta &meta,
                                  const stICDBase &icdBase)
{
    if (!meta) {
        return false;
    }
    if (meta->metaType() != IcdDefine::icdComplex) {
        return true;
    }
    ICDComplexData::smtComplex complex
            = SMT_CONVERT(ICDComplexData, meta);
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
    std::string file = jframeFacade()->configDirPath()
            .append("/jframe_global.xml");
    TiXmlDocument doc(file);
    std::string xmlFile;

    if (!doc.LoadFile()) {
        return xmlFile;
    }
    TiXmlNode* root = doc.FirstChild("jframe");
    if (!root) {
        return xmlFile;
    }
    if (!(root = root->FirstChild("app"))) {
        return xmlFile;
    }

    TiXmlElement *element = root->FirstChildElement("icddata");
    if (!element) {
        return xmlFile;
    }
    std::string sourceName = element->Attribute("name");
    std::string itemName;
    std::string itemValue;
    element = element->FirstChildElement();
    while (element) {
        itemName = element->Value();
        itemValue = element->Attribute("name");
        if ("file" == itemName) {
            if (sourceName == itemValue) {
                q_dataSource = GlobalDefine::dsFile;
            }
            TiXmlElement *childElement = element->FirstChildElement("path");
            if (childElement) {
                xmlFile = jframeFacade()->parsePath(QString(childElement->GetText())
                                                    .trimmed().toStdString());
            }
        } else if ("database" == itemName) {
            if (sourceName == itemValue) {
                q_dataSource = GlobalDefine::dsDatabase;
            }
        }
        element = element->NextSiblingElement();
    }

    return xmlFile;
}
