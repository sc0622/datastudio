#include "dataenginewidget.h"
#include "loggingwidget.h"
#include "jxmltable.h"
#include "jsplitter.h"
#include "metaui.h"
#include "icdfactory.h"
#include "icdcomplexdata.h"
#include "icdcheckdata.h"
#include "icdbitdata.h"
#include "icdframecodedata.h"
#include "icdcounterdata.h"

DataEngineWidget::DataEngineWidget(IJAttempter &attempter, QWidget *parent)
    : q_attempter(attempter)
    , QWidget(parent)
    , q_actNew(0)
    , q_actInsert(0)
    , q_actUp(0)
    , q_actDown(0)
    , q_actDelete(0)
    , q_actClear(0)
    , q_actSaveDB(0)
    , q_actSaveFile(0)
    , q_planeName("PlaneTable")
    , q_systemName("SystemTable")
    , q_tableName("ICDTable")
    , q_ruleName("RuleTable")
    , q_discertName("DiscertTable")
    , q_metaName("CommonTable")
    , q_dataType(-1)
    , q_subType(-1)
    , q_newIndex(-1)
    , q_changePos(-1)
    , q_data(0)
{
    q_defaultPath = QStandardPaths::writableLocation(
        QStandardPaths::DesktopLocation);

    q_edtStatus = new QLabel(this);
    //q_edtStatus->setMaximumHeight(45);
    q_edtStatus->setEnabled(false);
    QPalette palette = q_edtStatus->palette();
    palette.setColor(QPalette::WindowText, Qt::red);
    palette.setColor(QPalette::Base, Qt::transparent);
    q_edtStatus->setPalette(palette);

    q_table = new JXmlTable(this);
    q_table->loadConfig(QString::fromStdString(jframeFacade()
        ->thisDirPath()).append("/config/editor/icd_infrastructure.xml"),
        q_planeName);
    q_table->setSortingEnabled(false);
    q_table->setContextMenuPolicy(Qt::CustomContextMenu);
    q_paste = new QAction(QStringLiteral("ճ��"), q_table);
    q_table->addAction(q_paste);
    q_tableBox = new QGroupBox(QStringLiteral("���ͣ�"), this);
    QVBoxLayout* veriLayoutTable = new QVBoxLayout(q_tableBox);
    veriLayoutTable->setContentsMargins(2, 2, 2, 2);
    veriLayoutTable->addWidget(q_table);

    q_tipWidget = new QWidget(this);
    q_tipWidget->setMaximumHeight(20);
    QHBoxLayout *horiLayoutTop = new QHBoxLayout(q_tipWidget);
    horiLayoutTop->setContentsMargins(0, 0, 4, 0);
    horiLayoutTop->addStretch();
    horiLayoutTop->addWidget(q_edtStatus, 0, Qt::AlignCenter);

    q_loggingWidget = new LoggingWidget(attempter, this);
    JSplitter* splitter = new JSplitter(this);
    splitter->setOrientation(Qt::Horizontal);
    splitter->setScales(QList<double>() << 2.5 << 1.0);
    splitter->setHandleColor(QColor(32, 32, 32, 10));
    splitter->setHandleWidth(4);
    splitter->addWidget(q_tableBox);
    splitter->addWidget(q_loggingWidget);

    QVBoxLayout* veriLayoutMain = new QVBoxLayout(this);
    veriLayoutMain->setContentsMargins(0, 2, 0, 0);
    veriLayoutMain->addWidget(q_tipWidget);
    veriLayoutMain->addWidget(splitter);

    //
    connect(q_loggingWidget, SIGNAL(dataSaved(void *, bool &)),
            this, SLOT(slotSave2Memory(void *, bool &)));
    connect(q_loggingWidget, SIGNAL(canceled()), this, SLOT(slotCanceled()));

    connect(q_paste, SIGNAL(triggered()), this, SLOT(slotPaste()));
    enableConnection(true);

    // Ĭ������
    q_loggingWidget->setVisible(false);
}

DataEngineWidget::~DataEngineWidget()
{
}

// ��ʼ������
void DataEngineWidget::initUI(int type, void *data)
{
    // ɾ���ϴ�����
    if (GlobalDefine::ntUnknown == q_dataType) {
        q_data.reset();
    } else {
        q_vData.clear();
    }
    // ���ñ����
    q_changePos = -1;

    q_dataType = type;
    q_subType = -1;
    q_newIndex = -1;
    q_loggingWidget->setVisible(false);
    setActionState(q_actNew, true);
    setActionState(q_actInsert, false);
    setActionState(q_actDelete, false);
    setActionState(q_actUp, false);
    setActionState(q_actDown, false);
    setActionState(q_actSaveFile, true);
    // ��ѯ�����Ƿ��б��
    int dataState = 0;
    q_attempter.notifier().sendMessage("queryNodeFlag@ICDNavigation",
                                       (JWPARAM)&dataState,
                                       (JLPARAM)&QString("nodeState"));
    setActionState(q_actSaveDB, dataState);

    q_table->verticalHeader()->setSectionsMovable(true);    // Ĭ�Ͽ��Խ���������λ��
    //q_table->setSortingEnabled(true);   // Ĭ������������
    q_table->setProperty("copyData", -1);   // ���ÿ�����������

    // ��ʼ�����ݹ����жϿ����źŲ�
    enableConnection(false);
    if (GlobalDefine::ntUnknown == type) {
        q_vData
            = *reinterpret_cast<std::vector<ICDElement::smtElement> *>(data);
        const int count = q_vData.size();
        std::vector<PlaneNode::smtPlane> planes;
        for (int i = 0; i < count; ++i) {
            planes.push_back(std::dynamic_pointer_cast<PlaneNode>(q_vData[i])->clone());
        }
        showData(planes);
        setActionState(q_actSaveFile, !planes.empty());
    } else if (type == GlobalDefine::ntPlane) {
        ICDElement::smtElement element
            = *reinterpret_cast<ICDElement::smtElement *>(data);
        PlaneNode::smtPlane plane
            = std::dynamic_pointer_cast<PlaneNode>(element);
        q_data = plane->clone();
        showData(plane->allSystem());
    } else if (type == GlobalDefine::ntSystem) {
        ICDElement::smtElement element
            = *reinterpret_cast<ICDElement::smtElement *>(data);
        SystemNode::smtSystem system
            = std::dynamic_pointer_cast<SystemNode>(element);
        q_data = system->clone();
        showData(system->allTable());
    } else if (type == GlobalDefine::ntTable) {
        ICDElement::smtElement element
            = *reinterpret_cast<ICDElement::smtElement *>(data);
        TableNode::smtTable table
            = std::dynamic_pointer_cast<TableNode>(element);
        q_data = table->clone();
        if (table) {
            showData(table->allRule());
        }
    } else if (type == GlobalDefine::ntRule) {
        std::vector<int> &rule
            = *reinterpret_cast<std::vector<int> *>(data);
        if (2 == rule.size()) {
            q_subType = rule.at(0);
            ICDElement::smtElement element
                = *reinterpret_cast<ICDElement::smtElement *>(rule.at(1));
            //element = *reinterpret_cast<ICDElement::smtElement *>(rule.at(1));
            if (GlobalDefine::dtComplex == q_subType) {
                TableNode::smtTable table = SMT_CONVERT(TableNode, element);
                if (table) {
                    q_data = table->clone();
                    showData(table->allRule());
                } else {
                    showData(ICDMetaData::ruleMap());
                }
            } else {
                ICDMetaData::smtMeta meta = SMT_CONVERT(ICDMetaData, element);
                if (meta) {
                    q_data = meta->clone();
                    showData(meta);
                    if (GlobalDefine::dtDiscern != meta->type()) {
                        _UIData data;
                        data.data = &meta;
                        data.type = GlobalDefine::optEdit;
                        updateDetailUI(data);
                    }
                }
            }
        }
        // �����Ĺ�����棬���ñ���
        setActionState(q_actSaveDB, false);
        setActionState(q_actSaveFile, false);
    }
    // ��ʼ����������ñ��źŲ�
    enableConnection(true);
    // ��ʼ����ɣ����½�����ʾ��Ϣ
    if (type == GlobalDefine::ntTable) {
        int loaded = 0;
        q_attempter.notifier().sendMessage("queryNodeFlag@ICDNavigation",
                                           (JWPARAM)&loaded,
                                           (JLPARAM)&QString("loadedState"));
        if (Qt::Unchecked == loaded) {
            q_edtStatus->setText(QStringLiteral("��δ�������ݣ����ܽ��в���"));
            q_tipWidget->setHidden(loaded);
            // ���°�ť״̬
            setActionState(q_actNew, loaded);
            setActionState(q_actInsert, loaded);
        } else {
            setTipsVisible(q_ruleName == q_table->tableName());
        }
    } else {
        setTipsVisible(q_ruleName == q_table->tableName());
    }
}

// ������ʼ��
void DataEngineWidget::reInit()
{
    void *data(0);
    std::vector<int> rule;
    ICDElement::smtElement element;
    std::vector<ICDElement::smtElement> elements;
    // ��ѯԭʼ�������³�ʼ����
    if (GlobalDefine::ntUnknown == q_dataType) {
        PlaneNode::planeVector planes;
        q_attempter.notifier().sendMessage("queryAllPlane@DataManegement",
                                           (JWPARAM)&planes);
        const int count = planes.size();
        for (int i = 0; i < count; ++i) {
            elements.push_back(planes[i]);
        }
        data = &elements;
    } else {
        q_attempter.notifier().sendMessage("queryNodeData@ICDNavigation",
                                           (JWPARAM)&element,
                                           (JLPARAM)&QString());
        if (GlobalDefine::ntRule == q_dataType) {
            rule.push_back(q_subType);
            rule.push_back((int)&element);
            data = &rule;
        } else {
            data = &element;
        }
    }
    initUI(q_dataType, data);
}

// ��ѯ��ǰ��ʾ��������Ϣ
int DataEngineWidget::queryTableInformation(const QString &type) const
{
    int result = 0;
    ICDElement::smtElement element;
    // ��ѯ�����
    QString key;
    QString cmd = "queryNodeData@ICDNavigation";
//    if (GlobalDefine::ntTable == q_dataType
//        || (GlobalDefine::ntRule == q_dataType
//            && GlobalDefine::dtComplex == q_subType)) {
//        key = "parentTable";
//    }
    if (GlobalDefine::ntRule == q_dataType) {
        if (GlobalDefine::dtComplex != q_subType) {
            key = "parentTable";
        }
    }
    q_attempter.notifier().sendMessage(cmd.toStdString(),
                                       (JWPARAM)&element,
                                       (JLPARAM)&key);
    TableNode::smtTable table = SMT_CONVERT(TableNode, element);
    if (!table) {
        return result;
    }

    if (type == "total") {  // �ܳ���
        if (table->lengthCheck()) {
            result = table->icdBase().nLength;
        } else {
            result = table->length();
        }
    } else if (type == "used") {    // ��ʹ��
        result = table->length();
    } else if (type == "remains") { // ʣ��
        if (table->lengthCheck()) {
            result = table->icdBase().nLength;
            result -= table->length();
        }
    } else if (type == "lengthCheck") { // ʣ��
        result = table->lengthCheck();
    } else {
    }

    return result;
}

// ��ѯ�Ѵ��ڱ��е�����
QMap<QString, QString> DataEngineWidget::queryExistedData(const QString &section) const
{
    QMap<QString, QString> result;
    if (GlobalDefine::ntRule == q_dataType
        && !(GlobalDefine::dtComplex == q_subType
        || GlobalDefine::dtDiscern == q_subType)) {
        // �Ǹ���/֡�������ݣ���ѯ����
        QString keys;
        ICDElement::smtElement element;
        q_attempter.notifier().sendMessage("queryNodeKeys@ICDNavigation",
                                           (WPARAM)&keys);
        // �ؼ��뼯�а����õ�ǰ��������id����ѯ�����轫��ɾ��
        keys.truncate(keys.lastIndexOf("/"));
        q_attempter.notifier().sendMessage("querySingleElement",
                                           (JWPARAM)&element,
                                           (JLPARAM)&keys);
        TableNode::smtTable table = SMT_CONVERT(TableNode, element);
        if (!table) {
            return result;
        }
        QString value;
        ICDMetaData::smtMeta meta = 0;
        ICDMetaData::ruleMap rules = table->allRule();
        ICDMetaData::ruleMap::const_iterator it = rules.begin();
        for (; it != rules.end(); ++it) {
            if (!(meta = it->second) || !q_data
                || atoi(q_data->id().c_str()) == meta->serial()) {
                continue;
            }
            if ("name" == section) {
                value = QString::fromStdString(meta->name());
                result[value] = value;
            } else if ("code" == section) {
                value = QString::fromStdString(meta->sign());
                result[value] = value;
            }
        }
    } else {
        int column = -1;
        if ("name" == section) {
            column = 0;
        } else if ("code" == section) {
            column = 1;
        }
        if (!q_table || column < 0 || column >(q_table->colorCount() - 1)) {
            return result;
        }
        QString value;
        const int count = q_table->rowCount();
        for (int i = 0; i < count; ++i) {
            if (q_table->currentRow() == i) {
                continue;
            }
            value = q_table->itemValue(i, column).toString();
            result[value] = value;
        }
    }

    return result;
}

// ��ѯ֡����
QMap<int, QString> DataEngineWidget::queryFrameData(const QString &type) const
{
    QMap<int, QString> result;
    TableNode::smtTable table = parentTable();
    if (!table) {
        return result;
    }
    QVector<int> used;  // �Ѿ�ʹ��
    ICDMetaData::smtMeta meta = 0;
    ICDFrameCodeData::smtFrameCode frameCode = 0;
    ICDMetaData::ruleMap rules = table->allRule();
    ICDMetaData::ruleMap::const_iterator it = rules.begin();
    for (; it != rules.end(); ++it) {
        if (!(meta = it->second)) {
            continue;
        }
        if (GlobalDefine::dtDiscern == meta->type()) {
            // ֡����
            result[meta->serial()] = QString::fromStdString(meta->name());
        } else if (GlobalDefine::dtFrameCode == meta->type()) {
            // ֡ʶ����
            if (frameCode = SMT_CONVERT(ICDFrameCodeData, meta)) {
                used.push_back(frameCode->bindingSerial());
            }
        }
    }

    if (type == "idle") {   // ��ѯʣ����õģ�ɾ����ʹ�õ�
        QVectorIterator<int> it = used;
        while (it.hasNext()) {
            result.remove(it.next());
        }
    }

    return result;
}

// ���bit�������ݵ���ʼ�ֽں�
void DataEngineWidget::fillBitIndex(ICDMetaData::smtMeta &meta)
{
    TableNode::smtTable table = parentTable();
    if (!table) {
        return;
    }
    int index = 0;
    ICDMetaData::smtMeta metaPre = table->rule(meta->serial() - 1);
    ICDBitData::smtBit bitOriginal = SMT_CONVERT(ICDBitData, meta);
    if (metaPre) {
        if (!bitOriginal) { // ��bit��Ϊ��bit������
            // ��ѯ�����ȷ���ֽ����
            index = metaPre->index() + metaPre->byteLength();
        } else {    // �ж��Ƿ��ܹ��ϲ�
            // ��ѯǰһ������
            ICDBitData::smtBit bitPre = SMT_CONVERT(ICDBitData, metaPre);
            if (bitPre) {   // �ж��ܷ���ǰ�ϲ�
                // ��ǰ���ݵ���ʼλ��ǰ�����ݵĽ���λ֮��
                if (bitPre->end() < bitOriginal->start()) {
                    // ������ǰ�ϲ�
                    index = bitPre->index() - bitPre->start() / 8 + bitOriginal->start() / 8;
                } else {    // ���ܺϲ�
                    index = bitPre->index() + bitPre->byteLength();
                    index += bitOriginal->start() / 8;  // ƫ���ֽ�
                }
            } else {    // ���ܺϲ�
                index = metaPre->index() + metaPre->byteLength();
                index += bitOriginal->start() / 8;  // ƫ���ֽ�
            }
        }
    } else {    // ��һ������
        if (bitOriginal) {
            index = bitOriginal->start() / 8;   // ƫ���ֽ�
        }
    }
    meta->setIndex(index);
}

// ��ѯ���ݳ���ƫ����
int DataEngineWidget::queryOffset(const ICDMetaData::smtMeta &meta) const
{
    int result = 0;
    if (!meta) {
        return result;
    }
    TableNode::smtTable table = parentTable();
    if (!table) {
        return result;
    }
    // ����ԭʼ���Ⱥͱ༭���ݳ��Ȳ�
    if (!canMerged(meta)) {
        if (-1 != q_newIndex && q_table->currentRow() == q_newIndex) {
            // ����
            ICDBitData::smtBit bit = SMT_CONVERT(ICDBitData, meta);
            if (bit) {
                result = bit->rangeLength();
            } else {
                result = meta->byteLength();
            }
        } else {    // �༭
            result = meta->index() + meta->byteLength();
            ICDMetaData::smtMeta next = table->rule(meta->serial() + 1);
            if (next) { // �м�����
                // ���㵽��һ���ݵĲ�
                result -= next->index();
            } else { // ���һ������
                ICDMetaData::smtMeta old = table->rule(meta->serial());
                if (old) {
                    // �������ݳ��Ȳ�
                    result -= (old->index() + old->byteLength());
                }
            }
        }
    } else {
        if (-1 == q_newIndex) { // �༭
            result = meta->index();
            ICDMetaData::smtMeta old = table->rule(meta->serial());
            ICDMetaData::smtMeta next = table->rule(meta->serial() + 1);
            if (old) {
                if (next) { // �м�����
                    // ���㵽��һ���ݵĲ�
                    result -= next->index();
                } else { // ���һ������
                    result -= old->index();
                }
            }
        } else {    // ����
            result = meta->byteLength() - 1;
        }
    }

    return result;
}

// ��ѯ����ؼ�״̬
bool DataEngineWidget::queryWidgetState(const QString &name) const
{
    bool result = false;
    if (name == "new") {
        if (q_actNew) {
            result = q_actNew->isEnabled();
        }
    } else if (name == "clear") {
        if (q_actClear) {
            result = q_actClear->isEnabled();
        }
    }

    return result;
}

// ������������ģ�������
void DataEngineWidget::dealCommand(int command, const QVariant &param)
{
    switch (command) {
    case GlobalDefine::optNew:
        slotNew(param);
        break;
    case GlobalDefine::optClear:
        slotClear();
        break;
    default:
        break;
    }
}

void DataEngineWidget::setMenuPtr(const std::string &name, QAction *action)
{
    if (!action) {
        return;
    }
    action->setEnabled(false);
    if ("action_add" == name) {
        q_actNew = action;
    } else if ("action_insert" == name) {
        q_actInsert = action;
    } else if ("action_up" == name) {
        q_actUp = action;
    } else if ("action_down" == name) {
        q_actDown = action;
    } else if ("action_delete" == name) {
        q_actDelete = action;
    } else if ("action_clear" == name) {
        q_actClear = action;
    } else if ("action_database" == name) {
        q_actSaveDB = action;
    } else if ("action_file" == name) {
        q_actSaveFile = action;
    } else if ("action_table_guid" == name) {
        q_genTableGuid = action;
    }
}

bool DataEngineWidget::dealMenuCmd(const std::string &name)
{
    if ("action_add" == name) {
        slotNew(GlobalDefine::dtU8);
    } else if ("action_insert" == name) {
        slotInsert();
    } else if ("action_up" == name) {
        slotMoveUp();
    } else if ("action_down" == name) {
        slotMoveDown();
    } else if ("action_delete" == name) {
        slotDelete();
    } else if ("action_clear" == name) {
        slotClear();
    } else if ("action_database" == name) {
        slotSave2Source(GlobalDefine::dsDatabase);
    } else if ("action_file" == name) {
        slotSave2Source(GlobalDefine::dsFile);
    } else if ("action_table_guid" == name) {
        const QString guid = QUuid::createUuid().toString().remove(QRegExp("[{}-]"));
        QClipboard *clipboard = QApplication::clipboard();
        if (clipboard) {
            clipboard->setText("ICDTable_" + guid);
            return true;
        }
    } else {
        return true;
    }

    return false;
}

// ������ʾ����
void DataEngineWidget::showData(const PlaneNode::planeVector &planes)
{
    if (NULL == q_table) {
        return;
    }
    if (q_table->tableName() != q_planeName) {
        q_table->setTableName(q_planeName);
        q_tableBox->setTitle(QStringLiteral("���ͣ�"));
    }
    q_table->clearContents();
    const int count = planes.size();
    for (int row = 0; row < count; ++row) {
        updateOne(row, planes[row], optNew);
    }
    setActionState(q_actClear, count > 0);
}

// ������ʾϵͳ
void DataEngineWidget::showData(const SystemNode::systemVector &systems)
{
    if (NULL == q_table) {
        return;
    }
    if (q_table->tableName() != q_systemName) {
        q_table->setTableName(q_systemName);
        q_tableBox->setTitle(QStringLiteral("ϵͳ��"));
    }
    q_table->clearContents();
    const int count = systems.size();
    for (int i = 0; i < count; ++i) {
        updateOne(i, systems[i], optNew);
    }
    setActionState(q_actClear, count > 0);
}

// ������ʾICD��
void DataEngineWidget::showData(const TableNode::tableVector &tables)
{
    if (NULL == q_table) {
        return;
    }
    if (q_table->tableName() != q_tableName) {
        q_table->setTableName(q_tableName);
        q_tableBox->setTitle(QStringLiteral("ICD��"));
    }
    q_table->clearContents();
    const int count = tables.size();
    TableNode::smtTable item = 0;
    for (int i = 0, row = 0; i < count; ++i) {
        if (!(item = tables[i])) {
            continue;
        }
        updateOne(row++, item, optNew);
    }
    setActionState(q_actClear, tables.size() > 0);
}

// ������ʾ����
void DataEngineWidget::showData(const ICDMetaData::ruleMap &rules)
{
    if (NULL == q_table) {
        return;
    }
    if (q_table->tableName() != q_ruleName) {
        q_table->setTableName(q_ruleName);
        q_tableBox->setTitle(QStringLiteral("ICD���ݶ��壺"));
    }
    q_table->clearContents();
    // JXmlTable�ڲ���δʵ��mapToSource���ܣ���ʱ��������
    //q_table->setSortingEnabled(false);
    // �������ʱ��������ʾ
    setTipsVisible(false);
    ICDMetaData::ruleMap::const_iterator it = rules.begin();
    for (int row = 0; it != rules.end(); ++it) {
        const ICDMetaData::smtMeta &item = it->second;
        if (!item) {
            continue;
        }
        updateOne(row++, item, optNew);
    }
    setActionState(q_actClear, rules.size() > 0);
    // ��ʾ��ʾ��Ϣ
    if (rules.empty()) {
        setTipsVisible(true);
    }
}

// ����������
void DataEngineWidget::showData(const ICDMetaData::smtMeta &data)
{
    setTipsVisible(false);
    if (GlobalDefine::dtDiscern == data->type()) {  // ֡����
        if (q_table->tableName() != q_discertName) {
            q_table->setTableName(q_discertName);
            q_tableBox->setTitle(QStringLiteral("֡���ݶ��壺"));
        }
        q_table->clearContents();
        ICDComplexData::smtComplex complex
            = std::dynamic_pointer_cast<ICDComplexData>(data);
        // ��������
        TableNode::tableVector subTable = complex->allTable();
        const int count = subTable.size();
        TableNode::smtTable subData = 0;
        for (int i = 0; i < count; ++i) {
            if (!(subData = subTable[i])) {
                continue;
            }
            stICDBase base = subData->icdBase();
            updateSubOne(i, base, optNew);
        }
        setActionState(q_actClear, subTable.size() > 0);
    } else {
        // ��������������ק
        q_table->verticalHeader()->setSectionsMovable(false);
        if (q_table->tableName() != q_metaName) {
            q_table->setTableName(q_metaName);
            q_tableBox->setTitle(QStringLiteral("ICD���ݶ��壺"));
        }
        updateMetaOne(data);
        // ���ز�����ť
        setActionState(q_actNew, false);
        setActionState(q_actInsert, false);
        setActionState(q_actUp, false);
        setActionState(q_actDown, false);
        setActionState(q_actDelete, false);
        setActionState(q_actClear, false);
        setActionState(q_actSaveDB, false);
        setActionState(q_actSaveFile, false);
    }
}

// �ӱ�
void DataEngineWidget::updateSubOne(int index,
                                    const stICDBase &data,
                                    optType type)
{
    int column(0);
    switch (type) {
    case optNew:
        // ����հ���
        q_table->insertRow(index);
        break;
    case optEdit:
        break;
    case optDelete:
        q_table->removeRow(index);
        break;
    case optCopy:
        q_table->insertRow(index);
        q_table->setProperty("copyData", index);
    default:break;
    }
    // ��������
    QStringList lst = QString::fromStdString(data.sRemark).split("##");
    q_table->setItemData(index, column, data.sName.c_str(), Qt::UserRole);
    q_table->setItemValue(index, column, data.sDescribe.c_str());
    q_table->setItemValue(index, ++column, data.sCode.c_str());
    q_table->setItemData(index, column, data.sGroup.c_str(), Qt::UserRole);
    if (lst.size() > 1) {
        q_table->setItemValue(index, ++column, lst.last());
    }
    q_table->setItemValue(index, ++column, lst.first());
}

// ��������
void DataEngineWidget::updateMetaOne(const ICDMetaData::smtMeta &data)
{
    ICDCommonData::smtCommon common = SMT_CONVERT(ICDCommonData, data);
    if (!common) {
        return;
    }
    int resizeRow = -1;
    stQueryDic dic;
    QList<QPair<QString, QString>> datas;   // ��ʾҪ��
    QPair<QString, QString> pairData;
    // ��ѯ�ֵ�����
    dic.dic = GlobalDefine::dicDataType;
    dic.dicType = GlobalDefine::dictDec;
    dic.condition = QString::number(common->type()).toStdString();
    q_attempter.notifier().sendMessage("queryDictionary@DataManegement",
                                       (JWPARAM)&dic);
    // ������ʾ����
    pairData.first = QStringLiteral("��������");
    pairData.second = dic.result.empty()
        ? QStringLiteral("��Ч") : QString(dic.result.c_str());
    datas.append(pairData);
    pairData.first = QStringLiteral("��������");
    pairData.second = QString(common->name().c_str());
    datas.append(pairData);
    pairData.first = QStringLiteral("���ݱ�ʶ");
    pairData.second = QString(common->proCode().c_str());
    datas.append(pairData);
    pairData.first = QStringLiteral("��ʼ�ֺ�");
    pairData.second = QString::number(common->index());
    datas.append(pairData);
    pairData.first = QStringLiteral("���ݳ���");
    pairData.second = QStringLiteral("%1 �ֽ�").arg(common->length());
    datas.append(pairData);
    pairData.first = QStringLiteral("Ĭ��ֵ");
    pairData.second = QString(common->defaultStr().c_str());
    datas.append(pairData);

    if (data->metaType() == IcdDefine::icdCommon) {
        if (data->type() == GlobalDefine::dtHead) {
        } else if (data->type() == GlobalDefine::dtCounter) {
            ICDCounterData::smtCounter counter
                = std::dynamic_pointer_cast<ICDCounterData>(data);
            dic.dic = GlobalDefine::dicCounterType;
            dic.dicType = GlobalDefine::dictDec;
            dic.condition = QString::number(counter->counterType())
                .toStdString();
            q_attempter.notifier()
                .sendMessage("queryDictionary@DataManegement",
                (JWPARAM)&dic);
            pairData.first = QStringLiteral("֡����");
            pairData.second = QString(dic.result.c_str());
            datas.insert(1, pairData);
        } else if (data->type() == GlobalDefine::dtCheck) {
            ICDCheckData::smtCheck check
                = std::dynamic_pointer_cast<ICDCheckData>(data);
            QPair<QString, QString> &ref = datas[5];
            ref.first = QStringLiteral("У����ʼλ");
            ref.second = QString::number(check->start());
            pairData.first = QStringLiteral("У����ֹλ");
            pairData.second = QString::number(check->end());
            datas.insert(6, pairData);
            dic.dic = GlobalDefine::dicCheckType;
            dic.dicType = GlobalDefine::dictDec;
            dic.condition = QString::number(check->checkType())
                .toStdString();
            q_attempter.notifier()
                .sendMessage("queryDictionary@DataManegement",
                (JWPARAM)&dic);
            pairData.first = QStringLiteral("У������");
            pairData.second = QString(dic.result.c_str());
            datas.insert(1, pairData);
        } else if (data->type() == GlobalDefine::dtFrameCode) {
            ICDFrameCodeData::smtFrameCode frameCode
                = std::dynamic_pointer_cast<ICDFrameCodeData>(data);
            pairData.first = QStringLiteral("֡����");
            ICDComplexData::smtComplex complex = frameCode->data();
            if (!complex) {
                pairData.second = QStringLiteral("����");
            } else {
                pairData.second = QStringLiteral("��<%1>")
                    .arg(complex->name().c_str());
            }
            datas.insert(3, pairData);
        } else {
            QStringList lstRange
                = QString(common->range().c_str()).split("~");
            if (lstRange.size() < 2) {
                lstRange.append("");
            }
            if (lstRange.first().isEmpty()) {
                lstRange.first() = "-Inf";
            }
            if (lstRange.last().isEmpty()) {
                lstRange.last() = "+Inf";
            }
            pairData.first = QStringLiteral("���ݷ�Χ");
            pairData.second = QString().append(lstRange.first())
                .append(" ~ ").append(lstRange.last());
            datas.insert(5, pairData);
            pairData.first = QStringLiteral("������");
            pairData.second = QString::number(common->scale(), 'g', 16);
            datas.insert(7, pairData);
            pairData.first = QStringLiteral("��λ");
            pairData.second = QString(common->unit().c_str());
            datas.insert(7, pairData);
            pairData.first = QStringLiteral("ƫ��");
            pairData.second = QString::number(common->offset(), 'g', 16);
            datas.insert(7, pairData);
            pairData.first = QStringLiteral("����ֵ����");
            pairData.second.clear();
            std::map<double, std::string> items = common->values();
            std::map<double, std::string>::iterator it = items.begin();
            for (; it != items.end(); ++it) {
                pairData.second += QString("%1 : <%2>\r\n")
                    .arg(it->first).arg(it->second.c_str());
            }
            pairData.second = pairData.second.trimmed();
            datas.insert(7, pairData);
            resizeRow = 7;
        }
    } else if (data->metaType() == IcdDefine::icdBit) {
        ICDBitData::smtBit bit = SMT_CONVERT(ICDBitData, data);
        QPair<QString, QString> &ref = datas[4];
        ref.second = QStringLiteral("%1 �ֽ�")
            .arg(bit->lengthOfByte().c_str());
        QString value;
        QString content;
        std::map<double, std::string> items = bit->values();
        std::map<double, std::string>::iterator it = items.begin();
        pairData.second.clear();
        if (data->type() == GlobalDefine::dtBitMap) {
            for (; it != items.end(); ++it) {
                if (it->second.empty()) {
                    continue;   // ��������Ϊ�յ�����
                }
                content = " ";
                QStringList lst = QString(it->second.c_str()).split(":");
                QStringList lstValue = lst.last().split(";");
                if (lstValue.size() > 1) {
                    if (!lstValue.first().isEmpty()) {
                        content.append("0:").append(lstValue.first());
                    }
                    if (!lstValue.last().isEmpty()) {
                        content.append(", 1:").append(lstValue.last());
                    }
                } else {
                    content.clear();
                }
                QString(it->second.c_str()).split(":").last();
                value = QString::number((int)it->first);
                pairData.second += QString("%1 : <%2%3>\r\n")
                    .arg(value).arg(lst.first()).arg(content);
            }
        } else if (data->type() == GlobalDefine::dtBitValue) {
            pairData.first = QStringLiteral("������");
            pairData.second = QString::number(bit->scale(), 'g', 16);
            datas.insert(6, pairData);
            pairData.first = QStringLiteral("��λ");
            pairData.second = QString(bit->unit().c_str());
            datas.insert(6, pairData);
            pairData.first = QStringLiteral("ƫ��");
            pairData.second = QString::number(bit->offset(), 'g', 16);
            datas.insert(6, pairData);
            QString fill;
            pairData.second.clear();
            for (; it != items.end(); ++it) {
                if (it->second.empty()) {
                    continue;   // ��������Ϊ�յ�����
                }
                value = QString::number((int)it->first, 2)
                    .right(bit->length());
                if (value.length() < bit->length()) {
                    fill = QString(bit->length() - value.length(), '0');
                    value.insert(0, fill);
                }
                pairData.second += QString("%1 : <%2>\r\n")
                    .arg(value).arg(it->second.c_str());
            }
        }
        pairData.first = QStringLiteral("����ֵ����");
        pairData.second = pairData.second.trimmed();
        resizeRow = 7;
        datas.insert(6, pairData);
        pairData.first = QStringLiteral("����λ");
        pairData.second = QString("%1~%2")
            .arg(bit->start()).arg(bit->start() + bit->length() - 1);
        datas.insert(5, pairData);
    }
    pairData.first = QStringLiteral("����");
    pairData.second = QString(common->remark().c_str());
    datas.append(pairData);
    // ���±�
    QListIterator<QPair<QString, QString>> it = datas;
    int row = 0;
    q_table->setRowCount(datas.size());
    while (it.hasNext()) {
        pairData = it.next();
        q_table->setItemValue(row, 0, pairData.first);
        q_table->setItemValue(row++, 1, pairData.second);
    }
    if (resizeRow > -1) {
        q_table->resizeRowToContents(resizeRow);
    }
}

// ����
void DataEngineWidget::updateOne(int index,
                                 const PlaneNode::smtPlane &data,
                                 optType type)
{
    int column(0);
    switch (type) {
    case optNew:
        // ����հ���
        q_table->insertRow(index);
        break;
    case optEdit:
        break;
    case optDelete:
        q_table->removeRow(index);
        break;
    case optCopy:
        q_table->insertRow(index);
        q_table->setProperty("copyData", index);
    default:break;
    }
    if (!data) {
        return;
    }
    // ��������
    q_table->setItemData(index, column, data->id().c_str(), Qt::UserRole);
    q_table->setItemValue(index, column, data->name().c_str());
    q_table->setItemValue(index, ++column, data->sign().c_str());
    q_table->setItemValue(index, ++column, data->describe().c_str());
}

void DataEngineWidget::updateOne(int index,
                                 const SystemNode::smtSystem &data,
                                 optType type)
{
    int column(0);
    switch (type) {
    case optNew:
        // ����հ���
        q_table->insertRow(index);
        break;
    case optEdit:
        break;
    case optDelete:
        q_table->removeRow(index);
        break;
    case optCopy:
        q_table->insertRow(index);
        q_table->setProperty("copyData", index);
    default:break;
    }
    if (!data) {
        return;
    }
    // ��������
    q_table->setItemData(index, column, data->numeralId(), Qt::UserRole);
    q_table->setItemValue(index, column, data->name().c_str());
    q_table->setItemValue(index, ++column, data->sign().c_str());
    q_table->setItemValue(index, ++column, data->describe().c_str());
}

void DataEngineWidget::updateOne(int index,
                                 const TableNode::smtTable &data,
                                 optType type)
{
    stICDBase base = data->icdBase();
    int column(0);
    switch (type) {
    case optNew:
        // ����հ���
        q_table->insertRow(index);
        break;
    case optEdit:
        break;
    case optDelete:
        q_table->removeRow(index);
        break;
    case optCopy:
        q_table->insertRow(index);
        q_table->setProperty("copyData", index);
    default:break;
    }
    if (!data) {
        return;
    }
    // ��������
    q_table->setItemData(index, column, data->key().c_str(), Qt::UserRole);
    q_table->setItemValue(index, column, base.sDescribe.c_str());
    q_table->setItemData(index, ++column, base.sGroup.c_str(), Qt::UserRole);
    q_table->setItemValue(index, column, base.sCode.c_str());
    q_table->setItemData(index, ++column, base.sID.c_str(), Qt::UserRole);
    q_table->setItemValue(index, column, base.nLength);
    q_table->setItemValue(index, ++column, QString(base.sRemark.c_str())
                          .split("##").first());
}

void DataEngineWidget::updateOne(int index,
                                 const ICDMetaData::smtMeta &data,
                                 optType type)
{
    if (!data || index < 0) {
        return;
    }
    stQueryDic dic;
    int column = 0;
    // ��ѯ�ֵ�����
    dic.dic = GlobalDefine::dicDataType;
    dic.dicType = GlobalDefine::dictDec;
    switch (type) {
    case optNew:
        // ����հ���
        q_table->insertRow(index);
        break;
    case optEdit:
        break;
    case optDelete:
        q_table->removeRow(index);
        break;
    case optCopy:
        q_table->insertRow(index);
        q_table->setProperty("copyData", index);
    default:break;
    }
    const ICDCommonData::smtCommon common
        = std::dynamic_pointer_cast<ICDCommonData>(data);
    const ICDFrameCodeData::smtFrameCode frameCode
        = std::dynamic_pointer_cast<ICDFrameCodeData>(data);
    QString indentify = QString::number(common->serial());
    if (GlobalDefine::dtComplex == common->type()) {
        // �������ݼ�¼�ӱ�ID
        q_table->setItemData(index, column, 
                             common->rule().c_str(), ComplexTable);
    }
    // ��������
    q_table->setItemData(index, column, indentify, Qt::UserRole);
    q_table->setItemValue(index, column, common->name().c_str());
    q_table->setItemValue(index, ++column, common->proCode().c_str());
    if (frameCode) {
        q_table->setItemData(index, column,
                             frameCode->bindingSerial(), Qt::UserRole);
    }
    if (optCopy == type) {
        // ���Ƶ����ݣ�����ǰһ�����������������ʼ�ֺ�
        int code = 0;
        if (index > 0) {
            code = q_table->itemValue(index - 1, 2).toInt();
            code += q_table->itemData(index - 1, q_table->columnCount() - 1,
                                      Qt::UserRole).toInt();
        }
        q_table->setItemValue(index, ++column, code);
    } else {
        q_table->setItemValue(index, ++column, common->index());
    }
    q_table->setItemValue(index, ++column, common->lengthOfByte().c_str());
    dic.condition = QString::number(common->type()).toStdString();
    q_table->setItemData(index, ++column, common->type(), Qt::UserRole);
    q_attempter.notifier().sendMessage("queryDictionary@DataManegement",
                                       (JWPARAM)&dic);
    q_table->setItemValue(index, column,
                          dic.result.empty()
                          ? QStringLiteral("��Ч") : dic.result.c_str());
    q_table->setItemValue(index, ++column, data->remark().c_str());
    q_table->setItemData(index, column, data->byteLength(), Qt::UserRole);
}

// ��ȡ��ǰѡ��������
ICDElement::smtElement DataEngineWidget::currentData() const
{
    ICDElement::smtElement result;
    if (!q_table) {
        return result;
    }
    int column = 0;
    int row = q_table->currentRow();
    if (row < 0) {
        return result;
    }
    if (row == q_newIndex) {    // ��������
        if (q_table->property("copyData").toInt() == q_newIndex) {
            // �����ǰѡ�������ǿ������ݣ�ֱ�Ӵ��ڴ��ȡ
            QString keys = q_table->property("source").toString();
            keys = keys.mid(keys.indexOf("##") + strlen("##"));
            if (keys.isEmpty()) {
                return result;
            }
            q_attempter.notifier().sendMessage("querySingleElement",
                                               (JWPARAM)&result,
                                               (JLPARAM)&keys);
        } else {    // �����������
        }
    } else {    // �༭����
        QString key = q_table->itemData(row, column, Qt::UserRole).toString();
        // ��ѯ��������
        if (GlobalDefine::ntUnknown == q_dataType) {
            const int count = q_vData.size();
            for (int i = 0; i < count; ++i) {
                if (q_vData[i]->id() == key.toStdString()) {
                    result = q_vData[i];
                }
            }
        } else if (GlobalDefine::ntPlane == q_dataType) {
            const int count = q_vData.size();
            for (int i = 0; i < count; ++i) {
                if (q_vData[i]->id() == key.toStdString()) {
                    result = q_vData[i];
                }
            }
        } else if (GlobalDefine::ntSystem == q_dataType) {
            SystemNode::smtSystem system
                = std::dynamic_pointer_cast<SystemNode>(q_data);
            if (system) {
                result = system->table(key.toStdString());
            }
        } else if (GlobalDefine::ntTable == q_dataType) {
            TableNode::smtTable table
                = std::dynamic_pointer_cast<TableNode>(q_data);
            if (table) {
                result = table->rule(key.toInt());
            }
        } else if (GlobalDefine::ntRule == q_dataType) {
            if (GlobalDefine::dtComplex == q_subType) {
                TableNode::smtTable table
                    = std::dynamic_pointer_cast<TableNode>(q_data);
                if (table) {
                    result = table->rule(key.toInt());
                }
            } else if (GlobalDefine::dtDiscern == q_subType) {
                ICDComplexData::smtComplex complex
                    = std::dynamic_pointer_cast<ICDComplexData>(q_data);
                if (complex) {
                    result = complex->table(key.toStdString());
                }
            }
        }
    }

    return result;
}

// ��/ͣ���źŲ�
void DataEngineWidget::enableConnection(bool enable)
{
    disconnect(q_table,
               SIGNAL(currentItemChanged(QStandardItem *, QStandardItem *)),
               this,
               SLOT(slotCurrentItemChanged(QStandardItem *, QStandardItem *)));
    disconnect(q_table, SIGNAL(contentChanged()),
               this, SLOT(slotContentChanged()));
    disconnect(q_table, SIGNAL(itemPressed(QStandardItem *)),
               this, SLOT(slotItemPressed(QStandardItem *)));
    disconnect(q_table, SIGNAL(customContextMenuRequested(const QPoint &)),
               this, SLOT(slotCustomContextMenu(const QPoint &)));
    disconnect(q_table->verticalHeader(), SIGNAL(sectionMoved(int, int, int)),
               this, SLOT(slotDataPosChanged(int, int, int)));
    if (enable) {
        connect(q_table,
                SIGNAL(currentItemChanged(QStandardItem *, QStandardItem *)),
                this,
                SLOT(slotCurrentItemChanged(QStandardItem *, QStandardItem *)));
        connect(q_table, SIGNAL(contentChanged()),
                this, SLOT(slotContentChanged()));
        connect(q_table, SIGNAL(itemPressed(QStandardItem *)),
                this, SLOT(slotItemPressed(QStandardItem *)));
        connect(q_table, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(slotCustomContextMenu(const QPoint &)));
        connect(q_table->verticalHeader(), SIGNAL(sectionMoved(int, int, int)),
                this, SLOT(slotDataPosChanged(int, int, int)));
    }
}

// ��������
void DataEngineWidget::newPlane()
{
    if (!q_table) {
        return;
    }
    QString id;
    stPlane plane;
    q_attempter.notifier().sendMessage("queryID@ICDNavigation",
                                       (WPARAM)&id,
                                       GlobalDefine::ntPlane);
    plane.nCode = id.toInt();
    plane.sName = QStringLiteral("����%1").arg(plane.nCode).toStdString();
    plane.sSign = QString("plane_%1").arg(plane.nCode).toStdString();
    PlaneNode::smtPlane smtData(new PlaneNode(plane));
    // ���±�
    q_newIndex = q_table->rowCount();
    updateOne(q_newIndex, smtData, optNew);
    q_table->selectRow(q_newIndex);
}

// ���»��ͱ༭���ݽ���
void DataEngineWidget::updatePlaneUI(GlobalDefine::OptionType option)
{
    stPlane base;
    PlaneNode::smtPlane plane
        = std::dynamic_pointer_cast<PlaneNode>(currentData());
    // ��ѯ��ǰѡ������
    if (!plane) {
        int column = 0;
        int row = q_table->currentRow();
        if (row < 0) {
            return;
        }
        QString id;
        q_attempter.notifier().sendMessage("queryID@ICDNavigation",
                                           (WPARAM)&id,
                                           GlobalDefine::ntPlane);
        base.nCode = id.toInt();
        base.sName = q_table->itemValue(row, column)
            .toString().toStdString();
        base.sSign = q_table->itemValue(row, ++column)
            .toString().toStdString();
        base.sDescribe = q_table->itemValue(row, ++column)
            .toString().toStdString();
    } else {
        base = plane->plane();
    }
    _UIData data;
    data.data = &base;
    data.type = option;
    q_loggingWidget->initUIData(LoggingWidget::wdPlane, data);
    q_loggingWidget->setVisible(true);
}

// ����ϵͳ
void DataEngineWidget::newSystem()
{
    if (!q_table) {
        return;
    }
    QString id;
    stSystem system;
    q_attempter.notifier().sendMessage("queryID@ICDNavigation",
                                       (WPARAM)&id,
                                       GlobalDefine::ntSystem);
    system.nCode = id.toInt();
    system.sName = QStringLiteral("ϵͳ%1").arg(system.nCode).toStdString();
    system.sSign = QString("system_%1").arg(system.nCode).toStdString();
    SystemNode::smtSystem smtData(new SystemNode(system));
    // ���±�
    q_newIndex = q_table->rowCount();
    updateOne(q_newIndex, smtData, optNew);
    q_table->selectRow(q_newIndex);
}

// ����ϵͳ�༭���ݽ���
void DataEngineWidget::updateSystemUI(GlobalDefine::OptionType option)
{
    stSystem base;
    // ��ѯ��ǰѡ������
    SystemNode::smtSystem system
        = std::dynamic_pointer_cast<SystemNode>(currentData());
    if (!system) {
        int column = 0;
        int row = q_table->currentRow();
        if (row < 0) {
            return;
        }
        QString id;
        q_attempter.notifier().sendMessage("queryID@ICDNavigation",
                                           (WPARAM)&id,
                                           GlobalDefine::ntSystem);
        base.nCode = id.toInt();
        base.sName = q_table->itemValue(row, column)
            .toString().toStdString();
        base.sSign = q_table->itemValue(row, ++column)
            .toString().toStdString();
        base.sDescribe = q_table->itemValue(row, ++column)
            .toString().toStdString();
    } else {
        base = system->system();
    }
    _UIData data;
    data.data = &base;
    data.type = option;
    q_loggingWidget->initUIData(LoggingWidget::wdSystem, data);
    q_loggingWidget->setVisible(true);
}

// ����ICD��
void DataEngineWidget::newICDTable()
{
    if (!q_table) {
        return;
    }
    QString id;
    QString keys;
    stICDBase table;
    q_attempter.notifier().sendMessage("queryNodeKeys@ICDNavigation",
                                       (WPARAM)&keys);
    q_attempter.notifier().sendMessage("queryID@ICDNavigation",
                                       (WPARAM)&id,
                                       GlobalDefine::ntTable);
    table.check = false;
    table.sName = id.toStdString();
    table.sID = id.mid(id.lastIndexOf("_") + strlen("_")).toStdString();
    table.sDescribe = QStringLiteral("��%1")
        .arg(q_table->rowCount() + 1).toStdString();
    table.sCode = QString("table_%1")
        .arg(q_table->rowCount() + 1).toStdString();
    table.nLength = 0;
    table.sGroup = keys.toStdString();
    TableNode::smtTable smtData(new TableNode(table));
    // ���±�
    q_newIndex = q_table->rowCount();
    updateOne(q_newIndex, smtData, optNew);
    q_table->selectRow(q_newIndex);
}

// ����ICD��༭���ݽ���
void DataEngineWidget::updateICDTableUI(GlobalDefine::OptionType option)
{
    stICDBase base;
    // ��ѯ��ǰѡ������
    TableNode::smtTable table = SMT_CONVERT(TableNode, currentData());
    if (!table) {
        int column = 0;
        int row = q_table->currentRow();
        if (row < 0) {
            return;
        }
        base.check = false;
        base.sDescribe = q_table->itemValue(row, column)
            .toString().toStdString();
        base.sName = q_table->itemData(row, column, Qt::UserRole)
            .toString().toStdString();
        base.sCode = q_table->itemValue(row, ++column)
            .toString().toStdString();
        base.sGroup = q_table->itemData(row, column, Qt::UserRole)
            .toString().toStdString();
        base.nLength = q_table->itemValue(row, ++column)
            .toInt();
        base.sID = q_table->itemData(row, column, Qt::UserRole)
            .toString().toStdString();
        base.sRemark = q_table->itemValue(row, ++column)
            .toString().toStdString();
    } else {
        base = table->icdBase();
    }
    _UIData data;
    data.data = &base;
    data.type = option;
    q_loggingWidget->initUIData(LoggingWidget::wdTable, data);
    q_loggingWidget->setVisible(true);
}

// ��������
void DataEngineWidget::newICDRule(int type)
{
    if (!q_table) {
        return;
    }
    q_newIndex = q_table->rowCount();
    stTableRules rule;
    int index = q_table->rowCount() - 1;
    if (index > -1) {
        rule.nSerial = q_table->itemData(index, 0, Qt::UserRole).toInt() + 1;

        rule.nCode = q_table->itemValue(index, 2).toInt();
        rule.nCode += q_table->itemData(index, q_table->columnCount() - 1,
                                        Qt::UserRole).toInt();
    } else {
        rule.nCode = 0;
        rule.nSerial = 1;
    }
    rule.sName = QString("item_%1").arg(rule.nSerial).toStdString();
    if (type > GlobalDefine::dtDiscern || type < GlobalDefine::dtHead) {
        rule.uType = GlobalDefine::dtU8;
    } else {
        rule.uType = type;
    }
    rule.sPrgCode = QString("code_%1").arg(q_newIndex).toStdString();
    ICDMetaData::smtMeta smtData
        = ICDFactory::instance().CreatObject(rule);
    // ���±�
    updateOne(q_newIndex, smtData, optNew);
    q_table->selectRow(q_newIndex);

    // ����δ��������ݳ�����Ϊ0��ʹ����У����ȷ��ʾ
    q_table->setItemData(q_newIndex, q_table->columnCount() - 1,
                         0, Qt::UserRole);
}

// ���¹���༭���ݽ���
void DataEngineWidget::updateICDRuleUI(GlobalDefine::OptionType option)
{
    ICDMetaData::smtMeta meta
        = std::dynamic_pointer_cast<ICDMetaData>(currentData());
    if (GlobalDefine::optNew == option) {    // ����
        if (!meta) {
            int column = 0;
            stTableRules rule;
            // ��ѯ��������
            rule.sName = q_table->itemValue(q_newIndex, column)
                .toString().toStdString();
            rule.nSerial = q_table->itemData(q_newIndex, column,
                                             Qt::UserRole).toInt();
            rule.sPrgCode = q_table->itemValue(q_newIndex, ++column)
                .toString().toStdString();
            rule.nCode = q_table->itemValue(q_newIndex, ++column).toInt();
            rule.uLength = q_table->itemValue(q_newIndex, ++column).toInt();
            rule.uType = q_table->itemData(q_newIndex, ++column,
                                           Qt::UserRole).toInt();
            rule.sRemark = q_table->itemValue(q_newIndex, ++column)
                .toString().toStdString();
            meta = ICDFactory::instance().CreatObject(rule);
        } else {
            // �����ֽ���ź�˳���
            meta = meta->clone();
            meta->setIndex(q_table->itemValue(q_newIndex, 2).toInt());
            meta->setSerial(q_table->rowCount());
            // ֡ʶ���룬���ð���Ϣ
            ICDFrameCodeData::smtFrameCode frameCode =
                std::dynamic_pointer_cast<ICDFrameCodeData>(meta);
            if (frameCode) {
                frameCode->bindData(0);
            }
        }
    }
    if (!meta) {
        return;
    }
    _UIData data;

    data.data = &meta;
    data.type = option;
    updateDetailUI(data);
}

// ��������¼�����
void DataEngineWidget::updateDetailUI(const _UIData &data)
{
    ICDMetaData::smtMeta meta
        = *reinterpret_cast<ICDMetaData::smtMeta *>(data.data);
    if (!meta) {
        return;
    }
    if (IcdDefine::icdCommon == meta->metaType()) {
        if (GlobalDefine::dtHead == meta->type()) {
            q_loggingWidget->initUIData(LoggingWidget::wdHeader, data);
        } else if (GlobalDefine::dtCounter == meta->type()) {
            q_loggingWidget->initUIData(LoggingWidget::wdCounter, data);
        } else if (GlobalDefine::dtCheck == meta->type()) {
            q_loggingWidget->initUIData(LoggingWidget::wdCheck, data);
        } else if (GlobalDefine::dtFrameCode == meta->type()) {
            q_loggingWidget->initUIData(LoggingWidget::wdFrameCode, data);
        } else {
            q_loggingWidget->initUIData(LoggingWidget::wdCommon, data);
        }
    } else if (IcdDefine::icdCustom == meta->metaType()) {
        if (GlobalDefine::dtBuffer == meta->type()) {
            q_loggingWidget->initUIData(LoggingWidget::wdBuffer, data);
        }
    } else if (IcdDefine::icdBit == meta->metaType()) {
        if (GlobalDefine::dtBitMap == meta->type()) {
            q_loggingWidget->initUIData(LoggingWidget::wdBitMap, data);
        } else if (GlobalDefine::dtBitValue == meta->type()) {
            q_loggingWidget->initUIData(LoggingWidget::wdBitValue, data);
        }
    } else if (IcdDefine::icdComplex == meta->metaType()) {
        if (GlobalDefine::dtComplex == meta->type()) {
            q_loggingWidget->initUIData(LoggingWidget::wdComplex, data);
        } else if (GlobalDefine::dtDiscern == meta->type()) {
            q_loggingWidget->initUIData(LoggingWidget::wdDiscern, data);
        }
    }
    q_loggingWidget->setVisible(true);
}

// �����ӱ�
void DataEngineWidget::newSubTable()
{
    if (!q_table) {
        return;
    }
    int maxCount = 0;
    q_attempter.notifier().sendMessage("queryMaxTableCount@DataManegement",
                                       (JWPARAM)&maxCount);
    if (q_table->rowCount() >= maxCount) {

        QMessageBox::information(this, QStringLiteral("����"),
                                 QStringLiteral("�ӱ��Ѵ����ޣ��޷������������������ޣ�����ϵ����Ա��"));
        return;
    }

    QString id;
    QString keys;
    stICDBase base;
    q_attempter.notifier().sendMessage("queryNodeKeys@ICDNavigation",
                                       (WPARAM)&keys);
    QStringList keyLst = keys.split("/", QString::SkipEmptyParts);
    if (keyLst.size() < 2) {
        return;
    }
    q_attempter.notifier().sendMessage("queryID@ICDNavigation",
                                       (WPARAM)&id,
                                       GlobalDefine::ntTable);
    base.sName = id.toStdString();
    base.sID = id.mid(id.lastIndexOf("_") + strlen("_")).toStdString();
    base.sDescribe = QStringLiteral("��%1")
        .arg(q_table->rowCount() + 1).toUpper().toStdString();
    base.sCode = QString("%1").arg(q_table->rowCount() + 1,
                                   2, 16, QChar('0')).toStdString();
    base.sGroup = QString("%1/%2").arg(keyLst.at(0))
        .arg(keyLst.at(1)).toStdString();
    base.sRemark = "##1";
    // ���±�
    q_newIndex = q_table->rowCount();
    updateSubOne(q_newIndex, base, optNew);
    q_table->selectRow(q_newIndex);
}

// �����ӱ�༭���ݽ���
void DataEngineWidget::updateSubTableUI(GlobalDefine::OptionType option)
{
    stICDBase base;
    // ��ѯ��ǰѡ������
    TableNode::smtTable table
        = std::dynamic_pointer_cast<TableNode>(currentData());
    if (!table) {
        int column = 0;
        int row = q_table->currentRow();
        if (row < 0) {
            return;
        }
        base.sDescribe = q_table->itemValue(row, column)
            .toString().toStdString();
        base.sName = q_table->itemData(row, column, Qt::UserRole)
            .toString().toStdString();
        base.sCode = q_table->itemValue(row, ++column)
            .toString().toStdString();
        base.sGroup = q_table->itemData(row, column, Qt::UserRole)
            .toString().toStdString();
        base.sRemark = q_table->itemValue(row, ++column)
            .toString().toStdString();
        base.sRemark.insert(0, q_table->itemValue(row, ++column)
                            .toString().toStdString() + "##");
    } else {
        base = table->icdBase();
    }
    _UIData data;
    data.data = &base;
    data.type = option;
    q_loggingWidget->initUIData(LoggingWidget::wdSubTable, data);
    q_loggingWidget->setVisible(true);
}

// ��/��״̬��ʾ
void DataEngineWidget::setTipsVisible(bool visible)
{
    if (!q_edtStatus) {
        return;
    }
    q_tipWidget->setVisible(visible);
    if (visible) {
        TableNode::smtTable table = SMT_CONVERT(TableNode, q_data);
        if (!table) {
            return;
        }
        int total = table->icdBase().nLength;
        int use = table->length();
        QString tips = QStringLiteral("�������ܳ���%1 �ֽڣ�").arg(total)
                + QStringLiteral("�Ѷ��壺 %1 �ֽڣ�").arg(table->length())
                + QStringLiteral("ʵ�ʹ滮�� %1 �ֽ�")
                .arg(table->realLength());
        q_edtStatus->setText(tips);
        if (-1 == q_newIndex) {
            setActionState(q_actNew, true);
            setActionState(q_actInsert, true);
        }
        if (table->lengthCheck()) {
            // ��Ҫ���г��ȼ��ʱ�ű��������ť״̬
            if (use > total) {
                setActionState(q_actNew, false);
                setActionState(q_actInsert, false);
            }
        }
    }
}

// ��ѯ��ǰѡ��������bitλ�ĵ�һ���������
int DataEngineWidget::queryFirstBitSerial(int bitIndex)
{
    int result = -1;
    if (!q_table) {
        return result;
    }
    int row = q_table->currentRow();
    if (row < 0) {
        return result;
    }
    int column = 2; // index
    int value = 0;
    // ��ѯ
    for (int i = row - 1; i > -1; --i) {
        value = q_table->itemValue(i, column).toInt();
        if (bitIndex == value) {
            result = q_table->itemData(i, 0, Qt::UserRole).toInt();
        } else if (value < bitIndex) {
            break;  // 
        }
    }

    return result;
}

// ��ѯ�ܷ񽫵�ǰ���ݺϲ���ǰһ����ߺ�һ������
bool DataEngineWidget::canMerged(const ICDMetaData::smtMeta &meta) const
{
    bool result = false;
    TableNode::smtTable table = parentTable();
    ICDBitData::smtBit bitOriginal = SMT_CONVERT(ICDBitData, meta);
    if (!table || !bitOriginal) {
        return result;
    }
    ICDMetaData::smtMeta metaPre = table->rule(meta->serial() - 1);
    ICDMetaData::smtMeta metaNext = 0;

    if (GlobalDefine::ntRule == q_dataType
        && !(GlobalDefine::dtComplex == q_subType
        || GlobalDefine::dtDiscern == q_subType)) {
        // �Ǹ���/֡�������ݣ���ǰ����Ϊ��������
        metaNext = table->rule(meta->serial() + 1);
    } else {
        if (q_table->currentRow() == q_newIndex && -1 != q_newIndex) {
            // ��ǰ����Ϊδ�������������
            metaNext = table->rule(meta->serial());
        } else {
            // ��ǰ����Ϊ��������
            metaNext = table->rule(meta->serial() + 1);
        }
    }
    ICDBitData::smtBit bitPre = SMT_CONVERT(ICDBitData, metaPre);
    ICDBitData::smtBit bitNext = SMT_CONVERT(ICDBitData, metaNext);
    if (bitPre) {
        // �ж��ܷ���ǰ�ϲ�
        if (bitPre->end() < bitOriginal->start()) {
            result = true;  // ���Ժϲ�
        }
        // ��һ��Ϊbit�ͣ����������
        if (bitNext) {
            // 1������ͬһ���͵�bit֮��
            if (bitPre->index() == bitNext->index()) {
                if (result) { // ������ǰ�ϲ�
                    if (bitOriginal->end() >= bitNext->start()) {
                        result = false; // �޷��ϲ�
                    }
                }
            } else {    // 2����ͬ���͵�bit֮��
                if (!result) {  // ������ǰ�ϲ�
                    if (bitOriginal->end() < bitNext->start()) {
                        result = true;  // �������ϲ�
                    }
                }
            }
        }
    } else {
        // �ж��ܷ����ϲ�
        if (bitNext && bitOriginal->end() < bitNext->start()) {
            result = true;  // ���Ժϲ�
        }
    }

    return result;
}

// ��ʾ�Ҽ��˵�
void DataEngineWidget::showMenu()
{
    QMenu menu(q_table);
    if (q_newIndex != q_table->currentRow()) {
        menu.addAction(QStringLiteral("����"), this, SLOT(slotCopy()));
    }
    QAction *act = menu.addAction(QStringLiteral("ճ��"), this,
                                  SLOT(slotPaste()));
    // ����Դ���ݺ�Ŀ�����ݲ�Σ�����ճ��ѡ�����״̬
    act->setEnabled(canPasted());
    menu.exec(QCursor::pos());
}

// ��ǰ���Ƿ�����ճ�����Ƶ�����
bool DataEngineWidget::canPasted() const
{
    bool result = true;
    QString keys = q_table->property("source").toString();
    if (keys.isEmpty() || -1 != q_newIndex) {
        // û��Դ���ݻ��߱�����δ��������
        result = false;
    } else {    // 
        QStringList typeList = keys.mid(0, keys.indexOf("##")).split("_");
        const int level = typeList.first().toInt();
        if (level == q_dataType) {  // Դ���ݺ�Ŀ�����ݲ㼶��ͬ
            if (GlobalDefine::ntTable < q_dataType) {
                // �������ݲ�
                const int subType = typeList.last().toInt();
                if (GlobalDefine::dtComplex == q_subType) {
                    // Ŀ��������Ϊ�������ݣ�������ճ�����ӱ�����֮�����������
                    result = (GlobalDefine::dtDiscern != subType);
                } else if (GlobalDefine::dtDiscern == q_subType) {
                    // Ŀ��������Ϊ�ӱ����ݣ���ֻ����ճ���ӱ�����
                    result = (GlobalDefine::dtDiscern == subType);
                } else {
                    // Ŀ��㼶������ճ��Դ����
                    result = false;
                }
            }
        } else {    // Դ���ݺ�Ŀ�����ݲ㼶��ͬ
            const int subType = typeList.last().toInt();
            if (GlobalDefine::ntTable > q_dataType) {
                // Ŀ��㼶�ڱ������ϣ�������ճ��Դ����
                result = false;
            } else {
                if (GlobalDefine::ntTable == q_dataType) {
                    // ����                // ��ѯ��ǰ�ڵ��Ƿ��������
                    int loadState = 0;
                    q_attempter.notifier()
                        .sendMessage("queryNodeFlag@ICDNavigation",
                        (JWPARAM)&loadState, (JLPARAM)&QString("loadedState"));
                    if (Qt::Unchecked == loadState) {
                        result = false; // δ���ع������ݣ���������
                    } else {
                        if (GlobalDefine::ntRule == level) {
                            // Դ����Ϊ�������ݲ㣬������ճ���ӱ�����
                            result = (GlobalDefine::dtDiscern != subType);
                        } else {
                            // Ŀ��㼶������ճ��Դ����
                            result = false;
                        }
                    }
                } else {
                    // �������ݲ�
                    if (GlobalDefine::dtComplex == q_subType) {
                        // Ŀ��������Ϊ�������ݣ�������ճ����������
                        result = (GlobalDefine::ntTable == level);
                    } else {
                        // Ŀ��㼶������ճ��Դ����
                        result = false;
                    }
                }
            }
        }
    }

    return result;
}

// ɾ������
void DataEngineWidget::deleteRule(int pos)
{
    TableNode::smtTable table = SMT_CONVERT(TableNode, q_data);
    ICDMetaData::ruleMap rules = table->allRule();
    if (rules.empty()) {
        return;
    }
    // ����һ��������ǰŲ�����һ�����ݼ�Ϊ��Ҫɾ������
    int index = 0;
    ICDBitData::smtBit bit = 0;
    ICDMetaData::smtMeta metaPre = table->rule(pos - 1);
    ICDBitData::smtBit bitPre = SMT_CONVERT(ICDBitData, metaPre);
    ICDMetaData::ruleMap::iterator it = rules.find(pos);
    ICDMetaData::ruleMap::iterator itNext = it;
    for (itNext++; itNext != rules.end(); ++itNext) {
        ICDMetaData::smtMeta meta = itNext->second;
        if (!meta) {
            continue;
        }
        bit = SMT_CONVERT(ICDBitData, meta);
        if (bitPre && bit
            && bitPre->end() < bit->start()) {
            index = bitPre->index() - bitPre->start() / 8 + bit->start() / 8; // ƫ���ֽ�
        }
        else {
            if (metaPre) {
                index = metaPre->index() + metaPre->byteLength();
            }
            if (bit) {
                index += bit->start() / 8;   // ƫ���ֽ�
            }
        }
        meta->setSerial(meta->serial() - 1);
        meta->setIndex(index);
        it->second = meta;
        metaPre = meta;
        bitPre = bit;
        it = itNext;
    }
    // ɾ�����һ������
    if (it != rules.end()) {
        rules.erase(it);
    }
    // �����ڴ�
    table->setRule(rules);
}

void DataEngineWidget::insertRule(const ICDMetaData::smtMeta &meta, int pos)
{
    if (!meta) {
        return;
    }
    TableNode::smtTable table = std::dynamic_pointer_cast<TableNode>(q_data);
    if (!table) {
        return;
    }
    // �������������
    meta->setSerial(pos);
    ICDBitData::smtBit bitOriginal = SMT_CONVERT(ICDBitData, meta);
    // 1��ȷ�������ֽ���ź�ƫ����
    bool mergerd = false;
    ICDMetaData::ruleMap rules = table->allRule();
    ICDMetaData::smtMeta metaPre = table->rule(pos - 1);
    ICDBitData::smtBit bitPre = SMT_CONVERT(ICDBitData, metaPre);
    if (bitOriginal) {
        if (bitPre) {
            // �ж��ܷ���ǰ�ϲ�
            if (bitPre->end() < bitOriginal->start()) {
                mergerd = true;  // ���Ժϲ�
                meta->setIndex(bitPre->index() - bitPre->start() / 8 + bitOriginal->start() / 8);
            }
        }
    }
    if (!mergerd) {
        // ���ܺϲ�
        if (metaPre) {
            meta->setIndex(metaPre->index() + metaPre->byteLength());
        } else {
            meta->setIndex(0);
        }
        if (bitOriginal) {
            // ƫ���ֽ�
            meta->setIndex(meta->index() + bitOriginal->start() / 8);
        }
    }

    // 2�������������
    // ��Ŀ�����ݿ�ʼ�������ֽ����
    int index = 0;
    ICDBitData::smtBit bit = 0;
    ICDMetaData::smtMeta newData = meta;
    bitPre = SMT_CONVERT(ICDBitData, meta);
    ICDMetaData::ruleMap::iterator it = rules.find(pos);
    for (; it != rules.end(); ++it) {
        ICDMetaData::smtMeta next = it->second;
        it->second = newData;
        if (!next) {
            continue;
        }
        bit = SMT_CONVERT(ICDBitData, next);
        if (bitPre && bit
            && bitPre->end() < bit->start()) {
            // ��ǰ���ܹ���ǰһ��ϲ�
            index = bitPre->index() - bitPre->start() / 8 + bit->start() / 8;
        } else {
            index = newData->index() + newData->byteLength();
            if (bit) {
                index += bit->start() / 8;  // ƫ���ֽ�
            }
        }
        next->setSerial(newData->serial() + 1);
        next->setIndex(index);
        bitPre = bit;
        newData = next;
    }
    // ����ԭʼ�����һ������
    rules[newData->serial()] = newData;
    // �����ڴ�
    table->setRule(rules);
}

// �������λ��
void DataEngineWidget::movePlane(int from, int to)
{
    const int count = q_vData.size();
    if (from < 0 || to >= count) {
        return;
    }
    ICDElement::smtElement element = q_vData[from];
    if (from < to) {    // ��ǰ�����
        for (int i = from; i < to; ++i) {
            q_vData[i] = q_vData[i + 1];
        }
    } else {    // �Ӻ���ǰ��
        for (int i = from; i > to; --i) {
            q_vData[i] = q_vData[i - 1];
        }
    }
    q_vData[to] = element;
    std::vector<PlaneNode::smtPlane> planes;
    for (int i = 0; i < count; ++i) {
        planes.push_back(SMT_CONVERT(PlaneNode, q_vData[i])->clone());
    }
    showData(planes);
    q_table->selectRow(to);
}

// ���ϵͳλ��
void DataEngineWidget::moveSystem(int from, int to)
{
    PlaneNode::smtPlane plane = SMT_CONVERT(PlaneNode, q_data);
    if (!plane) {
        return;
    }
    SystemNode::systemVector systems = plane->allSystem();
    const int count = systems.size();
    if (from < 0 || to >= count) {
        return;
    }
    SystemNode::smtSystem system = systems[from];
    if (from < to) {    // ��ǰ�����
        for (int i = from; i < to; ++i) {
            systems[i] = systems[i + 1];
        }
    } else {    // �Ӻ���ǰ��
        for (int i = from; i > to; --i) {
            systems[i] = systems[i - 1];
        }
    }
    systems[to] = system;
    plane->setSystem(systems);
    showData(systems);
    q_table->selectRow(to);
}

// �����λ��
void DataEngineWidget::moveTable(int from, int to)
{
    SystemNode::smtSystem system = SMT_CONVERT(SystemNode, q_data);
    if (!system) {
        return;
    }
    TableNode::tableVector tables = system->allTable();
    const int count = tables.size();
    if (from < 0 || to >= count) {
        return;
    }
    TableNode::smtTable table = tables[from];
    if (from < to) {    // ��ǰ�����
        for (int i = from; i < to; ++i) {
            tables[i] = tables[i + 1];
        }
    } else {    // �Ӻ���ǰ��
        for (int i = from; i > to; --i) {
            tables[i] = tables[i - 1];
        }
    }
    tables[to] = table;
    system->setTable(tables);
    showData(tables);
    q_table->selectRow(to);
}

// �������λ��
void DataEngineWidget::moveRule(int from, int to)
{
    TableNode::smtTable table = SMT_CONVERT(TableNode, q_data);
    if (!table) {
        return;
    }
    ICDMetaData::smtMeta meta = table->rule(from);
    if (!meta) {
        return;
    }
    // ���ƶ�λ�õĹ��������ƶ���Ŀ��λ�ã���ɾ���Ͳ����������
    deleteRule(from);
    insertRule(meta, to);
    showData(table->allRule());
    q_table->selectRow(to - 1);
}

// ����ӱ�λ��
void DataEngineWidget::moveSubTable(int from, int to)
{
    ICDComplexData::smtComplex complex = SMT_CONVERT(ICDComplexData, q_data);
    if (!complex) {
        return;
    }
    TableNode::tableVector tables = complex->allTable();
    const int count = tables.size();
    if (from < 0 || to >= count) {
        return;
    }
    TableNode::smtTable table = tables[from];
    if (from < to) {    // ��ǰ�����
        for (int i = from; i < to; ++i) {
            tables[i] = tables[i + 1];
        }
    } else {    // �Ӻ���ǰ��
        for (int i = from; i > to; --i) {
            tables[i] = tables[i - 1];
        }
    }
    tables[to] = table;
    complex->setTable(tables);
    showData(complex);
    q_table->selectRow(to);
}

void DataEngineWidget::saveEditData(void *data)
{
    if (GlobalDefine::ntUnknown == q_dataType) {
        PlaneNode::smtPlane plane;
        const stPlane &_plane = *reinterpret_cast<stPlane *>(data);
        const int count = q_vData.size();
        for (int i = 0; i < count; ++i) {
            if (atoi(q_vData[i]->id().c_str()) == _plane.nCode) {
                plane = std::dynamic_pointer_cast<PlaneNode>(q_vData[i]);
                plane->setPlane(_plane);
                break;
            }
        }
    } else if (GlobalDefine::ntPlane == q_dataType) {
        const stSystem &_system = *reinterpret_cast<stSystem *>(data);
        PlaneNode::smtPlane plane
            = std::dynamic_pointer_cast<PlaneNode>(q_data);
        SystemNode::smtSystem system = plane->system(_system.nCode);
        if (system) {
            system->setSystem(_system);
        }
    } else if (GlobalDefine::ntSystem == q_dataType) {
        const stICDBase &_icdBase = *reinterpret_cast<stICDBase *>(data);
        SystemNode::smtSystem system
            = std::dynamic_pointer_cast<SystemNode>(q_data);
        TableNode::smtTable table = system->table(_icdBase.sName);
        if (table) {
            table->setICDBase(_icdBase);
        }
    } else if (GlobalDefine::ntTable == q_dataType) {
        ICDMetaData::smtMeta meta
            = *reinterpret_cast<ICDMetaData::smtMeta *>(data);
        TableNode::smtTable table = SMT_CONVERT(TableNode, q_data);
        ICDMetaData::smtMeta old = table->rule(meta->serial());
        // ������ݳ��ȱ���������¼�����������ֽں�
        bool reorder = false;   // �����������ݱ�־
        if (meta->byteLength() != old->byteLength()
            || meta->length() != old->length()
            || meta->metaType() != old->metaType()) {
            reorder = true;
        } else {
            ICDBitData::smtBit oldBit = SMT_CONVERT(ICDBitData, old);
            ICDBitData::smtBit bit = SMT_CONVERT(ICDBitData, meta);
            if (oldBit) {
                if (oldBit->start() != bit->start()) {
                    reorder = true;
                }
            }
        }
        table->addRule(meta);
        if (reorder) {
            reorderTable(table, meta->serial() + 1);
        }
    } else if (GlobalDefine::ntRule == q_dataType) {
        if (GlobalDefine::dtComplex == q_subType) {
            ICDMetaData::smtMeta meta
                = *reinterpret_cast<ICDMetaData::smtMeta *>(data);
            TableNode::smtTable table = SMT_CONVERT(TableNode, q_data);
            ICDMetaData::smtMeta old = table->rule(meta->serial());
            // ������ݳ��ȱ���������¼�����������ֽں�
            bool reorder = false;   // �����������ݱ�־
            if (meta->byteLength() != old->byteLength()
                || meta->length() != old->length()
                || meta->metaType() != old->metaType()) {
                reorder = true;
            } else {
                ICDBitData::smtBit oldBit = SMT_CONVERT(ICDBitData, old);
                ICDBitData::smtBit bit = SMT_CONVERT(ICDBitData, meta);
                if (oldBit) {
                    if (oldBit->start() != bit->start()) {
                        reorder = true;
                    }
                }
            }
            table->addRule(meta);
            if (reorder) {
                reorderTable(table, meta->serial() + 1);
            }
        } else if (GlobalDefine::dtDiscern == q_subType) {
            const stICDBase &_icdBase = *reinterpret_cast<stICDBase *>(data);
            ICDComplexData::smtComplex complex
                = std::dynamic_pointer_cast<ICDComplexData>(q_data);
            TableNode::smtTable table = complex->table(_icdBase.sName);
            table->setICDBase(_icdBase);
        }
    }
}

// ���¼�����������ֽ����
void DataEngineWidget::reorderTable(TableNode::smtTable &table, int from)
{
    ICDMetaData::ruleMap rules = table->allRule();
    if (rules.empty()) {
        return;
    }
    // ��Ŀ��λ�ÿ�ʼ�����¼����ֽ����
    int index = 0;
    ICDBitData::smtBit bit = 0;
    ICDMetaData::smtMeta metaPre = table->rule(from - 1);
    ICDBitData::smtBit bitPre = SMT_CONVERT(ICDBitData, metaPre);
    ICDMetaData::ruleMap::iterator it = rules.find(from);
    for (; it != rules.end(); ++it) {
        ICDMetaData::smtMeta &meta = it->second;
        if (!meta) {
            continue;
        }
        bit = SMT_CONVERT(ICDBitData, meta);
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
        meta->setIndex(index);
        metaPre = meta;
        bitPre = bit;
    }
    // �����ڴ�
    table->setRule(rules);
}

// ��ѯ��ǰ�������ݸ���
TableNode::smtTable DataEngineWidget::parentTable() const
{
    TableNode::smtTable table = 0;
    // �ֱ�����Ľڵ�͹���ڵ����࣬������Ľڵ㣬��ѯq_data��������
    if (GlobalDefine::ntRule == q_dataType
        && !(GlobalDefine::dtComplex == q_subType
        || GlobalDefine::dtDiscern == q_subType)) {
        // �Ǹ���/֡�������ݣ���ѯ����
        QString keys;
        ICDElement::smtElement element;
        q_attempter.notifier().sendMessage("queryNodeKeys@ICDNavigation",
                                           (WPARAM)&keys);
        // �ؼ��뼯�а����õ�ǰ��������id����ѯ�����轫��ɾ��
        keys.truncate(keys.lastIndexOf("/"));
        q_attempter.notifier().sendMessage("querySingleElement",
                                           (JWPARAM)&element,
                                           (JLPARAM)&keys);
        table = SMT_CONVERT(TableNode, element);
    } else {
        // ����ڵ㣬�ڿ��������в�ѯ����
        table = SMT_CONVERT(TableNode, q_data);
    }

    return table;
}

void DataEngineWidget::setActionState(QAction *act, bool enabled)
{
    if (act) {
        act->setEnabled(enabled);
    }
}

// ���±༭���ݽ���
void DataEngineWidget::updateLoggingUI(GlobalDefine::OptionType option)
{
    if (GlobalDefine::ntUnknown == q_dataType) {
        updatePlaneUI(option);
    } else if (GlobalDefine::ntPlane == q_dataType) {
        updateSystemUI(option);
    } else if (GlobalDefine::ntSystem == q_dataType) {
        if (GlobalDefine::optNew == option) {
            updateICDTableUI(option);
        } else {
            ICDElement::smtElement element = currentData();
            if (element) {
                int loaded = 0;
                QString condition = QString("childloadedState@%1")
                        .arg(element->id().c_str());
                q_attempter.notifier().sendMessage("queryNodeFlag@ICDNavigation",
                                                   (JWPARAM)&loaded,
                                                   (JLPARAM)&condition);
                if (loaded > 0) {
                    updateICDTableUI(option);
                } else {
                    q_loggingWidget->setVisible(false);
                }
            }
        }
    } else if (GlobalDefine::ntTable == q_dataType) {
        updateICDRuleUI(option);
    } else if (GlobalDefine::ntRule == q_dataType) {
        if (GlobalDefine::dtComplex == q_subType) {
            updateICDRuleUI(option);
        } else if (GlobalDefine::dtDiscern == q_subType) {
            updateSubTableUI(option);
        } else {
            //q_loggingWidget->setVisible(false);
        }
    }
    // �����Ƿ���λ�ñ䶯���ж��Ƿ����ñ��水ť
    if (q_changePos >= 0) {
        q_loggingWidget->enableOptionButton(true);
    }
}

// ��ǰѡ������
void DataEngineWidget::slotCurrentItemChanged(QStandardItem *current,
                                              QStandardItem *previous)
{
    if (!current) {
        // ��������¼�봰��
        q_loggingWidget->setVisible(false);
        setActionState(q_actUp, false);
        setActionState(q_actDown, false);
        setActionState(q_actDelete, false);
    } else {
        if (previous && current->row() == previous->row()) {
            // δ���ѡ����
            return;
        } else {
            if (q_newIndex < 0 || q_newIndex != current->row()) {
                updateLoggingUI(GlobalDefine::optEdit);
            } else {     // �������ݲ�δ�����û����л�ѡ�б�����������
                updateLoggingUI(GlobalDefine::optNew);
            }
        }
        int count = q_table->rowCount();
        // ���˾������ڵ㣬�����ڵ��������λ��
        if (count > 1 && GlobalDefine::ntRule != q_dataType
            || GlobalDefine::dtDiscern == q_subType
            || GlobalDefine::dtComplex == q_subType) {
            setActionState(q_actUp, 0 != current->row());
            setActionState(q_actDown, count - 1 != current->row());
            // �������˳���δ���棬������ɾ��
            setActionState(q_actDelete, q_changePos < 0 ? true :false);
        } else if (1 == count) {
            setActionState(q_actDelete, true);
        }
    }
    // �л��ڵ�󣬸����б����Ƿ����������ݣ�����������ť�Ƿ����
    if (-1 != q_newIndex) {
        setActionState(q_actNew, false);
        setActionState(q_actInsert, false);
        setActionState(q_actUp, false);
        setActionState(q_actDown, false);
    } else {
        // �������λ�ú�δ�������ݣ�����������������
        if (q_changePos >= 0) {
            setActionState(q_actNew, false);
            setActionState(q_actInsert, false);
        }
    }
}

// �����������
void DataEngineWidget::slotItemPressed(QStandardItem *item)
{
    Q_UNUSED(item);
    if (Qt::RightButton == QApplication::mouseButtons()) {
        // ���������ʱ������ʾ�Ҽ��˵�
        if (q_metaName != q_table->tableName()) {
            showMenu();
        }
    }
}

// ���Զ���˵�
void DataEngineWidget::slotCustomContextMenu(const QPoint &pos)
{
    // ���������ʱ������ʾ�Ҽ��˵�
    if (q_metaName != q_table->tableName()) {
        QMenu menu(q_table);
        int row = q_table->currentRow();
        if (row > -1 && q_newIndex != row) {
            menu.addAction(QStringLiteral("����"), this, SLOT(slotCopy()));
        }
        QAction *act = menu.addAction(QStringLiteral("ճ��"), this,
                                      SLOT(slotPaste()));
        // ����Դ���ݺ�Ŀ�����ݲ�Σ�����ճ��ѡ�����״̬
        act->setEnabled(canPasted());
        menu.exec(q_table->viewport()->mapToGlobal(pos));
    }
}

// �����ݱ��
void DataEngineWidget::slotContentChanged()
{
    // ���ڹ��������ʱ��ʾ��ʾ
    setTipsVisible(q_ruleName == q_table->tableName());
}

void DataEngineWidget::slotDataPosChanged(int logical,
                                          int oldIndex,
                                          int newIndex)
{
    Q_UNUSED(logical);
    if (q_changePos < 0) {
        q_changePos = qMin(oldIndex, newIndex);
    } else if (q_changePos > 0) {
        q_changePos = qMin(q_changePos, qMin(oldIndex, newIndex));
    }
    if (GlobalDefine::ntUnknown == q_dataType) {
        movePlane(oldIndex, newIndex);
    } else if (GlobalDefine::ntPlane == q_dataType) {
        moveSystem(oldIndex, newIndex);
    } else if (GlobalDefine::ntSystem == q_dataType) {
        moveTable(oldIndex, newIndex);
    } else if (GlobalDefine::ntTable == q_dataType) {
        // ������Ŵ�1��ʼ
        moveRule(oldIndex + 1, newIndex + 1);
    } else if (GlobalDefine::ntRule == q_dataType) {
        if (GlobalDefine::dtComplex == q_subType) {
            // ������Ŵ�1��ʼ
            moveRule(oldIndex + 1, newIndex + 1);
        } else if (GlobalDefine::dtDiscern == q_subType) {
            moveSubTable(oldIndex, newIndex);
        }
    }
    q_loggingWidget->enableOptionButton(true);
}

// ��������
void DataEngineWidget::slotSave2Memory(void *data, bool &result)
{
    if (!data) {
        return;
    }
    // �������ݵ��ڴ棬�ֿ��������롢�����ͱ༭�����������롢����ֱ�ӱ��浽�ڴ�
    // �༭�����漰�����ݵ�λ�ø����������ȱ��浽�������ݣ���һ���Ը����ڴ�����
    if (q_table->property("copyData").toInt() == q_newIndex
        && -1 != q_newIndex) {  // ���濽������
        QVector<int> params;
        ICDElement::smtElement element;
        QString keys = q_table->property("source").toString();
        keys = keys.mid(keys.indexOf("##") + strlen("##"));
        params << (int)&keys << (int)data;
        q_attempter.notifier().sendMessage("savePastedData@ICDNavigation",
                                           (JWPARAM)&params,
                                           (JLPARAM)&element);
        result = true;
    } else if (-1 != q_newIndex
        && (q_table->rowCount() - 1) != q_newIndex
        && q_table->currentRow() == q_newIndex) {   // ��������
        std::vector<int> params;
        params.push_back(q_table->currentRow());
        params.push_back((int)data);
        q_attempter.notifier().sendMessage("insertNodeData@ICDNavigation",
                                           (JWPARAM)&params,
                                           (JLPARAM)&result);
    } else {    // �������������ݵ����һ������������/�༭
        if (-1 != q_changePos) {    // �ı����������λ��
            // �������ݵ�����
            saveEditData(data);
            std::vector<int> params;
            if (GlobalDefine::ntUnknown == q_dataType) {
                params.push_back((int)&q_vData);
            } else {
                params.push_back((int)&q_data);
            }
            params.push_back(q_changePos);
            q_attempter.notifier().sendMessage("reorderNodeData@ICDNavigation",
                                               (JWPARAM)&params,
                                               (JLPARAM)&result);
            // ���浽�ڴ�
        } else { // �������߽��༭������
            q_attempter.notifier().sendMessage("updateNodeData@ICDNavigation",
                                               (JWPARAM)data,
                                               (JLPARAM)&result);
        }
    }
    if (result) {
        // ���³�ʼ������
        int row = q_table->currentRow();
        reInit();
        q_table->selectRow(row);
    } else {
        setActionState(q_actNew, result);
        setActionState(q_actInsert, result);
    }
}

void DataEngineWidget::slotCanceled()
{
    if (-1 != q_changePos) {
        return;  // ֻ���϶�����λ��ʱ�����³�ʼ��
    }
    
    // ���³�ʼ������
    int row = q_table->currentRow();
    reInit();
    q_table->selectRow(row);

    //q_loggingWidget->setVisible(false);
}

// ����
void DataEngineWidget::slotNew(const QVariant &param)
{
    if (GlobalDefine::ntUnknown == q_dataType) {   // ����
        newPlane();
    } else if (GlobalDefine::ntPlane == q_dataType) {  // ϵͳ
        newSystem();
    } else if (GlobalDefine::ntSystem == q_dataType) { // ��
        newICDTable();
    } else if (GlobalDefine::ntTable == q_dataType) {
        newICDRule(param.toInt());
    } else if (GlobalDefine::ntRule == q_dataType) {
        if (GlobalDefine::dtComplex == q_subType) {
            newICDRule(param.toInt());
        } else if (GlobalDefine::dtDiscern == q_subType) {
            newSubTable();
        }
    }
    // ����������Ժ󣬱��뱣������֮����ܼ����ƶ�
    if (-1 != q_newIndex) {
        q_table->verticalHeader()->setSectionsMovable(false);
    }
}

// ����
void DataEngineWidget::slotInsert()
{
    if (GlobalDefine::ntTable == q_dataType
        || (GlobalDefine::ntRule == q_dataType
        && GlobalDefine::dtComplex == q_subType)) {
        if (!q_table) {
            return;
        }
        stTableRules rule;
        int index = q_table->currentRow();
        if (index < 0) {    // δѡ�����ݣ����뵽ĩβ������������
            newICDRule(GlobalDefine::dtU8);
        } else {
            q_newIndex = index;
            rule.nSerial = q_table->itemData(index, 0, Qt::UserRole).toInt();
            rule.nCode = q_table->itemValue(index - 1, 2).toInt();
            rule.nCode += q_table->itemData(index - 1, 
                                            q_table->columnCount() - 1,
                                            Qt::UserRole).toInt();
//             if (index > 0
//                 && q_table->itemValue(index - 1, 2).toInt() == rule.nCode) {
//                 // �������λ��������ͬ�ֽڵ�bit֮�䣬
//                 // �򽫲���λ���ƶ���bit�ֽ�֮ǰ
//                 rule.nSerial = queryFirstBitSerial(rule.nCode);
//             }
            rule.sName = QString("item_%1").arg(index).toStdString();
            rule.uType = GlobalDefine::dtU8;
            rule.sPrgCode = QString("code_%1").arg(index).toStdString();
            ICDMetaData::smtMeta smtData
                = ICDFactory::instance().CreatObject(rule);
            // ���±�
            updateOne(q_newIndex, smtData, optNew);
            q_table->selectRow(q_newIndex);
        }
        // ����������Ժ󣬱��뱣������֮����ܼ����ƶ�
        if (-1 != q_newIndex) {
            q_table->verticalHeader()->setSectionsMovable(false);
        }
    }
}

// ����
void DataEngineWidget::slotMoveUp()
{
    int row = q_table->currentRow();
    if (q_changePos < 0) {
        q_changePos = row - 1;
    } else if (q_changePos > 0) {
        q_changePos = qMin(q_changePos, row - 1);
    }
    if (GlobalDefine::ntUnknown == q_dataType) {
        movePlane(row, row - 1);
    } else if (GlobalDefine::ntPlane == q_dataType) {
        moveSystem(row, row - 1);
    } else if (GlobalDefine::ntSystem == q_dataType) {
        moveTable(row, row - 1);
    } else if (GlobalDefine::ntTable == q_dataType) {
        // ������Ŵ�1��ʼ
        moveRule(row + 1, row);
    } else if (GlobalDefine::ntRule == q_dataType) {
        if (GlobalDefine::dtComplex == q_subType) {
            // ������Ŵ�1��ʼ
            moveRule(row + 1, row);
        } else if (GlobalDefine::dtDiscern == q_subType) {
            moveTable(row, row - 1);
        }
    }
    q_loggingWidget->enableOptionButton(true);
}

// ����
void DataEngineWidget::slotMoveDown()
{
    int row = q_table->currentRow();
    if (q_changePos < 0) {
        q_changePos = row;
    } else if (q_changePos > 0) {
        q_changePos = qMin(q_changePos, row);
    }
    if (GlobalDefine::ntUnknown == q_dataType) {
        movePlane(row, row + 1);
    } else if (GlobalDefine::ntPlane == q_dataType) {
        moveSystem(row, row + 1);
    } else if (GlobalDefine::ntSystem == q_dataType) {
        moveTable(row, row + 1);
    } else if (GlobalDefine::ntTable == q_dataType) {
        // ������Ŵ�1��ʼ
        moveRule(row + 1, row + 2);
    } else if (GlobalDefine::ntRule == q_dataType) {
        if (GlobalDefine::dtComplex == q_subType) {
            // ������Ŵ�1��ʼ
            moveRule(row + 1, row + 2);
        } else if (GlobalDefine::dtDiscern == q_subType) {
            moveTable(row, row + 1);
        }
    }
    q_loggingWidget->enableOptionButton(true);
}

// ɾ��
void DataEngineWidget::slotDelete()
{
    bool result = false;
    bool showTip = false;
    if (GlobalDefine::ntUnknown == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("ȷ����ʾ"),
            QStringLiteral("����ɾ�����ͼ�������������\r\nȷ��ɾ����"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
            return;
        }
    } else if (GlobalDefine::ntPlane == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("ȷ����ʾ"),
            QStringLiteral("����ɾ����ϵͳ��������������\r\nȷ��ɾ����"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
            return;
        }
    } else if (GlobalDefine::ntSystem == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("ȷ����ʾ"),
            QStringLiteral("����ɾ��ICD��������������")
            + QStringLiteral("\r\nȷ��ɾ����ICD�����ݣ�"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
            return;
        }
    } else if (GlobalDefine::ntTable == q_dataType
               || GlobalDefine::ntRule == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("ȷ����ʾ"),
            QStringLiteral("����ɾ�����ݶ��弰������������")
            + QStringLiteral("\r\nȷ��ɾ���������ݣ�"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
            return;
        }
        showTip = true;
    } else {
        return;
    }
    q_table->setProperty("copyData", -1);   // ���ÿ�����������
    // ɾ��
    int current = q_table->currentRow();
    if (current == q_newIndex && -1 != q_newIndex) {    // ɾ����δ���������
        q_newIndex = -1;
        q_table->removeRow(current);
        setActionState(q_actNew, true);
        setActionState(q_actInsert, q_dataType > GlobalDefine::ntSystem);
        q_table->verticalHeader()->setSectionsMovable(true);
    } else {    // ɾ���ڴ�����
        QVariant data = q_table->itemData(current, 0, Qt::UserRole);
        QVariant subTable = q_table->itemData(current, 0, ComplexTable);
        QString key = data.toString();
        // ���ϱ���Ҫ���ӱ�������id���뵼����ͬ��
        if (subTable.isValid()) {
            key.append("/").append(subTable.toString());
        }
        std::vector<QString> keys(1, key);
        q_attempter.notifier().sendMessage("deleteNode@ICDNavigation",
                                           (JWPARAM)&keys,
                                           (JLPARAM)&result);
        if (result) {
            if (q_table->currentRow() == q_newIndex) {
                q_newIndex = -1;
            }
            q_table->removeRow(q_table->currentRow());
            setTipsVisible(showTip);
            // ���³�ʼ��
            int row = q_table->currentRow();
            reInit();
            q_table->selectRow(row);
        }
    }
}

// ���
void DataEngineWidget::slotClear()
{
    bool result = false;
    if (GlobalDefine::ntUnknown == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("ȷ����ʾ"),
            QStringLiteral("����ɾ�����л��ͣ�����������������")
            + QStringLiteral("\r\nȷ��ɾ����"))) {
            return;
        }
    } else if (GlobalDefine::ntPlane == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("ȷ����ʾ"),
            QStringLiteral("����ɾ����ǰ���͵�����ϵͳ������������������")
            + QStringLiteral("\r\nȷ��ɾ����"))) {
            return;
        }
    } else if (GlobalDefine::ntSystem == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("ȷ����ʾ"),
            QStringLiteral("����ɾ����ǰϵͳ������ICD������������������")
            + QStringLiteral("\r\nȷ��ɾ����"))) {
            return;
        }
    } else if (GlobalDefine::ntTable == q_dataType
               || GlobalDefine::ntRule == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("ȷ����ʾ"),
            QStringLiteral("����ɾ����ǰ����������ݶ��壬����������������")
            + QStringLiteral("\r\nȷ��ɾ����"))) {
            return;
        }
    } else {
        return;
    }
    std::vector<QString> keys;

    const int count = q_table->rowCount();
    for (int i = 0; i < count; ++i) {
        QVariant data = q_table->itemData(i, 0, Qt::UserRole);

        keys.push_back(data.toString());
    }
    q_attempter.notifier().sendMessage("deleteNode@ICDNavigation",
                                       (JWPARAM)&keys,
                                       (JLPARAM)&result);
    if (result) {
        reInit();
//         q_table->clearContents();
//         q_table->setRowCount(0);
//         if (GlobalDefine::ntUnknown == q_dataType) {
//             q_btnSaveFile->setEnabled(false);
//             q_btnSaveDB->setEnabled(true);
//         }
    }
}

// ����
void DataEngineWidget::slotSave2Source(GlobalDefine::DataSource type)
{
    int loaded = 0;
    // ��ѯ�Ƿ���δ��������
    q_attempter.notifier().sendMessage("queryNodeFlag@ICDNavigation",
                                       (JWPARAM)&loaded,
                                       (JLPARAM)&QString("loadedState"));
    if (GlobalDefine::wholeState != loaded) {
        if (QMessageBox::No == QMessageBox::warning(this,
            QStringLiteral("������ʾ"),
            QStringLiteral("��ǰѡ������δ�������ݣ�\r\n")
            + QStringLiteral("�������潫��ʧ�ⲿ�����ݣ��Ƿ������"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {

            return;
        }
    }
    QString tip;
    QString err;
    QAction *act = 0;
    QVector<int> params;
    if (GlobalDefine::dsDatabase == type) {   // �������ݿ�
        setActionState(act = q_actSaveDB, false);
        params << (int)&err << (int)&q_defaultPath;
        q_attempter.notifier().sendMessage("saveMemoryData@ICDNavigation",
                                           (JWPARAM)&params,
                                           GlobalDefine::dsDatabase);
        tip = QStringLiteral("�������ݵ����ݿ�");
    } else if (GlobalDefine::dsFile == type) {  // �����ļ�
        setActionState(act = q_actSaveFile, false);
        QString file = QFileDialog::getSaveFileName(this, QStringLiteral("����ICD����"), q_defaultPath,
                                                    "XML files (*.xml);;"
                                                    "JSON files (*.json)");
        if (file.isEmpty()) {
            act->setEnabled(true);
            return;
        }
        q_defaultPath = file;
        params << (int)&err << (int)&q_defaultPath;
        q_attempter.notifier().sendMessage("saveMemoryData@ICDNavigation",
                                           (JWPARAM)&params,
                                           GlobalDefine::dsFile);
        tip = QStringLiteral("�������ݵ��ļ�\r\n<%1>\r\n").arg(q_defaultPath);
    }

    if (!err.isEmpty()) {
        tip.append(QStringLiteral("ʧ�ܣ�\r\n[%1]").arg(err));
        setActionState(act, true);
    } else {
        tip.append(QStringLiteral("�ɹ���"));
        if (q_actSaveFile == act) {
            setActionState(act, true);
        }
    }

    QMessageBox::information(this, QStringLiteral("��������"), tip);
}

// ����
void DataEngineWidget::slotCopy()
{
    QString keys;
    QString id = q_table->itemData(q_table->currentRow(),
                                   0, Qt::UserRole).toString();
    q_attempter.notifier().sendMessage("queryNodeKeys@ICDNavigation",
                                       (WPARAM)&keys);
    // ��Դ������Ϣ�洢����[���ݲ㼶-�����Ͳ㼶##id]
    if (keys.isEmpty()) {
        keys = id;
    } else {
        keys.append("/").append(id);
    }
    keys.prepend(QString("%1_%2##").arg(q_dataType).arg(q_subType));
    q_table->setProperty("source", keys);
}

// ճ��
void DataEngineWidget::slotPaste()
{
    QString keys = q_table->property("source").toString();
    keys = keys.mid(keys.indexOf("##") + strlen("##"));
    if (keys.isEmpty()) {
        return;
    }
    // ��ѯԴ����
    ICDElement::smtElement element;
    q_attempter.notifier().sendMessage("querySingleElement@DataManegement",
                                       (JWPARAM)&element, (JLPARAM)&keys);
    if (!element) {
        QMessageBox::information(this, QStringLiteral("ճ��"),
                                 QStringLiteral("ճ��ʧ�ܣ�������Դ��Ч��"));
        return;
    }
    if (GlobalDefine::ntUnknown == q_dataType) {   // ���ڵ�
        PlaneNode::smtPlane plane = SMT_CONVERT(PlaneNode, element);
        // ���»���
        if (plane) {
            q_newIndex = q_table->rowCount();
            updateOne(q_newIndex, plane, optCopy);
            q_table->selectRow(q_newIndex);
        }
    } else if (GlobalDefine::ntPlane == q_dataType) {   // ����
        SystemNode::smtSystem system = SMT_CONVERT(SystemNode, element);
        // ����ϵͳ
        if (system) {
            q_newIndex = q_table->rowCount();
            updateOne(q_newIndex, system, optCopy);
            q_table->selectRow(q_newIndex);
        }
    } else if (GlobalDefine::ntSystem == q_dataType) {  // ϵͳ
        // ���±�
        q_newIndex = q_table->rowCount();
        updateOne(q_newIndex, SMT_CONVERT(TableNode, element), optCopy);
        q_table->selectRow(q_newIndex);
    } else if (GlobalDefine::ntTable == q_dataType) {   // ��
        // ���¹���
        q_newIndex = q_table->rowCount();
        updateOne(q_newIndex, SMT_CONVERT(ICDMetaData, element), optCopy);
        q_table->selectRow(q_newIndex);
        // ����δ��������ݳ�����Ϊ0��ʹ����У����ȷ��ʾ
        q_table->setItemData(q_newIndex, q_table->columnCount() - 1,
                             0, Qt::UserRole);
    } else if (GlobalDefine::ntRule == q_dataType) {    // ����
        if (GlobalDefine::dtComplex == q_subType) {
            // ���±�
            q_newIndex = q_table->rowCount();
            updateOne(q_newIndex,
                      SMT_CONVERT(ICDMetaData, element),
                      optCopy);
            q_table->selectRow(q_newIndex);
            // ����δ��������ݳ�����Ϊ0��ʹ����У����ȷ��ʾ
            q_table->setItemData(q_newIndex, q_table->columnCount() - 1,
                                 0, Qt::UserRole);
        } else if (GlobalDefine::dtDiscern == q_subType) {
            int maxCount = 0;
            q_attempter.notifier()
                .sendMessage("queryMaxTableCount@DataManegement",
                             (JWPARAM)&maxCount);
            if (q_table->rowCount() >= maxCount) {

                QMessageBox::information(this, QStringLiteral("ճ��"),
                                         QStringLiteral("�ӱ��Ѵ����ޣ��޷�ճ���������������ޣ�����ϵ����Ա��"));
                return;
            }
            TableNode::smtTable table = SMT_CONVERT(TableNode, element);
            if (table) {
                // ���±�
                q_newIndex = q_table->rowCount();
                updateSubOne(q_newIndex, table->icdBase(), optCopy);
                q_table->selectRow(q_newIndex);
            }
        }
    }
    // ����������Ժ󣬱��뱣������֮����ܼ����ƶ�
    if (-1 != q_newIndex) {
        q_table->verticalHeader()->setSectionsMovable(false);
    }
}
