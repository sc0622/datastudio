#include "precomp.h"
#include "dataenginewidget.h"
#include "loggingwidget.h"
#include "jwt/jxmltable.h"
#include "jwt/jsplitter.h"
#include "metaui.h"
#include "KernelClass/icdfactory.h"
#include "KernelClass/icdcomplexdata.h"
#include "KernelClass/icdcheckdata.h"
#include "KernelClass/icdbitdata.h"
#include "KernelClass/icdframecodedata.h"
#include "KernelClass/icdcounterdata.h"

DataEngineWidget::DataEngineWidget(QWidget *parent)
    : QWidget(parent)
    , q_planeName("PlaneTable")
    , q_systemName("SystemTable")
    , q_tableName("IcdTable")
    , q_ruleName("RuleTable")
    , q_discertName("DiscertTable")
    , q_metaName("CommonTable")
    , q_dataType(-1)
    , q_subType(-1)
    , q_newIndex(-1)
    , q_changePos(-1)
    , q_data(0)
{
    q_defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    QVBoxLayout* veriLayoutMain = new QVBoxLayout(this);
    veriLayoutMain->setContentsMargins(0, 0, 0, 0);
    veriLayoutMain->setSpacing(1);

    splitterMain_ = new JSplitter(this);
    splitterMain_->setObjectName("edit.view.splitter.main");
    splitterMain_->setScales(QList<double>() << 2 << 1);
    splitterMain_->setHandleWidth(3);
    veriLayoutMain->addWidget(splitterMain_);

    q_table = new JXmlTable(this);
    q_table->loadConfig(JMain::instance()->configDir().append("/old/xmltable.xml"),
                        q_planeName);
    q_table->setSortingEnabled(false);
    q_table->setContextMenuPolicy(Qt::CustomContextMenu);
    q_paste = new QAction(QStringLiteral("粘贴"), q_table);
    q_table->addAction(q_paste);
    splitterMain_->addWidget(q_table);

    q_loggingWidget = new LoggingWidget(this);
    splitterMain_->addWidget(q_loggingWidget);

    q_edtStatus = new QLabel(this);
    q_edtStatus->setObjectName("__status__");
    q_edtStatus->setFixedHeight(22);
    q_edtStatus->setTextFormat(Qt::RichText);
    q_edtStatus->setAlignment(Qt::AlignCenter);
    q_edtStatus->hide();
    veriLayoutMain->addWidget(q_edtStatus);

    connect(q_loggingWidget, &LoggingWidget::dataSaved, this, &DataEngineWidget::slotSave2Memory);
    connect(q_loggingWidget, &LoggingWidget::canceled, this, &DataEngineWidget::slotCanceled);
    connect(q_paste, &QAction::triggered, this, &DataEngineWidget::slotPaste);

    enableConnection(true);

    // 默认隐藏
    q_loggingWidget->setVisible(false);

    //
    jnotify->on("edit.toolbar.item.add", this, [=](JNEvent &){
        slotNew(GlobalDefine::dtU8);
    });
    jnotify->on("edit.toolbar.item.insert", this, [=](JNEvent &){
        slotInsert();
    });
    jnotify->on("edit.toolbar.item.up", this, [=](JNEvent &){
        slotMoveUp();
    });
    jnotify->on("edit.toolbar.item.down", this, [=](JNEvent &){
        slotMoveDown();
    });
    jnotify->on("edit.toolbar.item.remove", this, [=](JNEvent &){
        slotDelete();
    });
    jnotify->on("edit.toolbar.item.clean", this, [=](JNEvent &){
        slotClear();
    });
    jnotify->on("edit.toolbar.export.db", this, [=](JNEvent &){
        slotSave2Source(GlobalDefine::dsDatabase);
    });
    jnotify->on("edit.toolbar.export.file", this, [=](JNEvent &){
        slotSave2Source(GlobalDefine::dsFile);
    });
    jnotify->on("edit.toolbar.tool.genguid", this, [=](JNEvent &){
        const QString guid = QUuid::createUuid().toString().remove(QRegExp("[{}-]"));
        QClipboard *clipboard = QApplication::clipboard();
        if (clipboard) {
            clipboard->setText("ICDTable_" + guid);
        }
    });
}

DataEngineWidget::~DataEngineWidget()
{
    JMain::saveWidgetState(splitterMain_);
}

bool DataEngineWidget::init()
{
    JMain::restoreWidgetState(splitterMain_);

    return true;
}

// 初始化界面
void DataEngineWidget::initUI(int type, void *data)
{
    if (!data) {
        return;
    }

    // 删除上次数据
    if (GlobalDefine::ntUnknown == q_dataType) {
        q_data.reset();
    } else {
        q_vData.clear();
    }
    // 重置变更项
    q_changePos = -1;

    q_dataType = type;
    q_subType = -1;
    q_newIndex = -1;
    q_loggingWidget->setVisible(false);
    setActionEnabled("add", true);
    setActionEnabled("insert", false);
    setActionEnabled("remove", false);
    setActionEnabled("up", false);
    setActionEnabled("down", false);
    setActionEnabled("file", true);
    // 查询数据是否有变更
    int dataState = 0;
    QVariantList args;
    args.append(qVariantFromValue((void*)&dataState));
    QString command("nodeState");
    args.append(qVariantFromValue((void*)&command));
    jnotify->send("edit.queryNodeFlag", args);

    setActionEnabled("db", dataState);

    q_table->verticalHeader()->setSectionsMovable(true);    // 默认可以交换数据项位置
    //q_table->setSortingEnabled(true);   // 默认启用排序功能
    q_table->setProperty("copyData", -1);   // 重置拷贝数据属性

    // 初始化数据过程中断开表信号槽
    enableConnection(false);
    if (GlobalDefine::ntUnknown == type) {
        q_vData = *reinterpret_cast<std::vector<ICDElement::smtElement> *>(data);
        const int count = q_vData.size();
        std::vector<PlaneNode::smtPlane> planes;
        for (int i = 0; i < count; ++i) {
            planes.push_back(std::dynamic_pointer_cast<PlaneNode>(q_vData[i])->clone());
        }
        showData(planes);
        setActionEnabled("file", !planes.empty());
    } else if (type == GlobalDefine::ntPlane) {
        ICDElement::smtElement element = *reinterpret_cast<ICDElement::smtElement *>(data);
        PlaneNode::smtPlane plane = std::dynamic_pointer_cast<PlaneNode>(element);
        q_data = plane->clone();
        showData(plane->allSystem());
    } else if (type == GlobalDefine::ntSystem) {
        ICDElement::smtElement element = *reinterpret_cast<ICDElement::smtElement *>(data);
        SystemNode::smtSystem system = std::dynamic_pointer_cast<SystemNode>(element);
        q_data = system->clone();
        showData(system->allTable());
    } else if (type == GlobalDefine::ntTable) {
        ICDElement::smtElement element = *reinterpret_cast<ICDElement::smtElement *>(data);
        TableNode::smtTable table = std::dynamic_pointer_cast<TableNode>(element);
        q_data = table->clone();
        if (table) {
            showData(table->allRule());
        }
    } else if (type == GlobalDefine::ntRule) {
        std::vector<int> &rule = *reinterpret_cast<std::vector<int> *>(data);
        if (2 == rule.size()) {
            q_subType = rule.at(0);
            ICDElement::smtElement element = *reinterpret_cast<ICDElement::smtElement *>(rule.at(1));
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
        // 单独的规则界面，禁用保存
        setActionEnabled("db", false);
        setActionEnabled("file", false);
    }
    // 初始完成重新启用表信号槽
    enableConnection(true);
    // 初始化完成，更新界面提示信息
    if (type == GlobalDefine::ntTable) {
        int loaded = 0;
        args.clear();
        args.append(qVariantFromValue((void*)&loaded));
        command = "loadedState";
        args.append(qVariantFromValue((void*)&command));
        jnotify->send("edit.queryNodeFlag", args);

        if (Qt::Unchecked == loaded) {
            q_edtStatus->setText(QStringLiteral("尚未加载数据，不能进行操作"));
            q_edtStatus->setHidden(loaded);
            // 更新按钮状态
            setActionEnabled("add", loaded);
            setActionEnabled("insert", loaded);
        } else {
            setTipsVisible(q_ruleName == q_table->tableName());
        }
    } else {
        setTipsVisible(q_ruleName == q_table->tableName());
    }
}

// 主动初始化
void DataEngineWidget::reInit()
{
    void *data(0);
    std::vector<int> rule;
    ICDElement::smtElement element;
    std::vector<ICDElement::smtElement> elements;
    // 查询原始数据重新初始化表
    if (GlobalDefine::ntUnknown == q_dataType) {
        PlaneNode::planeVector planes;
        QVariantList args;
        args.append(qVariantFromValue((void*)&planes));
        jnotify->send("edit.queryAllPlane", args);

        const int count = planes.size();
        for (int i = 0; i < count; ++i) {
            elements.push_back(planes[i]);
        }
        data = &elements;
    } else {
        QVariantList args;
        args.append(qVariantFromValue((void*)&element));
        QString lParam;
        args.append(qVariantFromValue((void*)&lParam));
        jnotify->send("edit.queryNodeData", args);
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

// 查询当前显示表数据信息
int DataEngineWidget::queryTableInformation(const QString &type) const
{
    int result = 0;
    ICDElement::smtElement element;
    // 查询顶层表
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
    QVariantList args;
    args.append(qVariantFromValue((void*)&element));
    args.append(qVariantFromValue((void*)&key));
    jnotify->send("edit." + cmd, args);

    TableNode::smtTable table = SMT_CONVERT(TableNode, element);
    if (!table) {
        return result;
    }

    if (type == "total") {  // 总长度
        if (table->lengthCheck()) {
            result = table->icdBase().nLength;
        } else {
            result = table->length();
        }
    } else if (type == "used") {    // 已使用
        result = table->length();
    } else if (type == "remains") { // 剩余
        if (table->lengthCheck()) {
            result = table->icdBase().nLength;
            result -= table->length();
        }
    } else if (type == "lengthCheck") { // 剩余
        result = table->lengthCheck();
    } else {
    }

    return result;
}

// 查询已存在表中的数据
QMap<QString, QString> DataEngineWidget::queryExistedData(const QString &section) const
{
    QMap<QString, QString> result;
    if (GlobalDefine::ntRule == q_dataType
        && !(GlobalDefine::dtComplex == q_subType
        || GlobalDefine::dtDiscern == q_subType)) {
        // 非复合/帧规则数据，查询父表
        QString keys;
        ICDElement::smtElement element;
        QVariantList args;
        args.append(qVariantFromValue((void*)&keys));
        jnotify->send("edit.queryNodeKeys", args);
        // 关键码集中包含该当前规则数据id，查询父表需将其删除
        keys.truncate(keys.lastIndexOf("/"));

        args.clear();
        args.append(qVariantFromValue((void*)&element));
        args.append(qVariantFromValue((void*)&keys));
        jnotify->send("edit.querySingleElement", args);

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

// 查询帧数据
QMap<int, QString> DataEngineWidget::queryFrameData(const QString &type) const
{
    QMap<int, QString> result;
    TableNode::smtTable table = parentTable();
    if (!table) {
        return result;
    }
    QVector<int> used;  // 已经使用
    ICDMetaData::smtMeta meta = 0;
    ICDFrameCodeData::smtFrameCode frameCode = 0;
    ICDMetaData::ruleMap rules = table->allRule();
    ICDMetaData::ruleMap::const_iterator it = rules.begin();
    for (; it != rules.end(); ++it) {
        if (!(meta = it->second)) {
            continue;
        }
        if (GlobalDefine::dtDiscern == meta->type()) {
            // 帧数据
            result[meta->serial()] = QString::fromStdString(meta->name());
        } else if (GlobalDefine::dtFrameCode == meta->type()) {
            // 帧识别码
            if (frameCode = SMT_CONVERT(ICDFrameCodeData, meta)) {
                used.push_back(frameCode->bindingSerial());
            }
        }
    }

    if (type == "idle") {   // 查询剩余可用的，删除已使用的
        QVectorIterator<int> it = used;
        while (it.hasNext()) {
            result.remove(it.next());
        }
    }

    return result;
}

// 填充bit类型数据的起始字节号
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
        if (!bitOriginal) { // 由bit改为非bit型数据
            // 查询规则表，确定字节序号
            index = metaPre->index() + metaPre->byteLength();
        } else {    // 判定是否能够合并
            // 查询前一项数据
            ICDBitData::smtBit bitPre = SMT_CONVERT(ICDBitData, metaPre);
            if (bitPre) {   // 判定能否向前合并
                // 当前数据的起始位在前项数据的结束位之后
                if (bitPre->end() < bitOriginal->start()) {
                    // 可以向前合并
                    index = bitPre->index() - bitPre->start() / 8 + bitOriginal->start() / 8;
                } else {    // 不能合并
                    index = bitPre->index() + bitPre->byteLength();
                    index += bitOriginal->start() / 8;  // 偏移字节
                }
            } else {    // 不能合并
                index = metaPre->index() + metaPre->byteLength();
                index += bitOriginal->start() / 8;  // 偏移字节
            }
        }
    } else {    // 第一项数据
        if (bitOriginal) {
            index = bitOriginal->start() / 8;   // 偏移字节
        }
    }
    meta->setIndex(index);
}

// 查询数据长度偏移量
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
    // 计算原始长度和编辑数据长度差
    if (!canMerged(meta)) {
        if (-1 != q_newIndex && q_table->currentRow() == q_newIndex) {
            // 新增
            ICDBitData::smtBit bit = SMT_CONVERT(ICDBitData, meta);
            if (bit) {
                result = bit->rangeLength();
            } else {
                result = meta->byteLength();
            }
        } else {    // 编辑
            result = meta->index() + meta->byteLength();
            ICDMetaData::smtMeta next = table->rule(meta->serial() + 1);
            if (next) { // 中间数据
                // 计算到下一数据的差
                result -= next->index();
            } else { // 最后一项数据
                ICDMetaData::smtMeta old = table->rule(meta->serial());
                if (old) {
                    // 计算数据长度差
                    result -= (old->index() + old->byteLength());
                }
            }
        }
    } else {
        if (-1 == q_newIndex) { // 编辑
            result = meta->index();
            ICDMetaData::smtMeta old = table->rule(meta->serial());
            ICDMetaData::smtMeta next = table->rule(meta->serial() + 1);
            if (old) {
                if (next) { // 中间数据
                    // 计算到下一数据的差
                    result -= next->index();
                } else { // 最后一项数据
                    result -= old->index();
                }
            }
        } else {    // 新增
            result = meta->byteLength() - 1;
        }
    }

    return result;
}

// 查询界面控件状态
bool DataEngineWidget::queryWidgetState(const QString &name) const
{
    bool result = false;
    if (name == "add") {
        result = jnotify->send("edit.toolbar.action.querystate", name).toBool();
    } else if (name == "clean") {
        result = jnotify->send("edit.toolbar.action.querystate", name).toBool();
    }

    return result;
}

// 处理来自其他模块的命令
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

// 加载显示机型
void DataEngineWidget::showData(const PlaneNode::planeVector &planes)
{
    if (NULL == q_table) {
        return;
    }
    if (q_table->tableName() != q_planeName) {
        q_table->setTableName(q_planeName);
    }
    q_table->clearContents();
    const int count = planes.size();
    for (int row = 0; row < count; ++row) {
        updateOne(row, planes[row], optNew);
    }
    setActionEnabled("clean", count > 0);
}

// 加载显示系统
void DataEngineWidget::showData(const SystemNode::systemVector &systems)
{
    if (NULL == q_table) {
        return;
    }
    if (q_table->tableName() != q_systemName) {
        q_table->setTableName(q_systemName);
    }
    q_table->clearContents();
    const int count = systems.size();
    for (int i = 0; i < count; ++i) {
        updateOne(i, systems[i], optNew);
    }
    setActionEnabled("clean", count > 0);
}

// 加载显示ICD表
void DataEngineWidget::showData(const TableNode::tableVector &tables)
{
    if (NULL == q_table) {
        return;
    }
    if (q_table->tableName() != q_tableName) {
        q_table->setTableName(q_tableName);
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
    setActionEnabled("clean", tables.size() > 0);
}

// 加载显示规则
void DataEngineWidget::showData(const ICDMetaData::ruleMap &rules)
{
    if (NULL == q_table) {
        return;
    }
    if (q_table->tableName() != q_ruleName) {
        q_table->setTableName(q_ruleName);
    }
    q_table->clearContents();
    // JXmlTable内部尚未实现mapToSource功能，暂时禁用排序
    //q_table->setSortingEnabled(false);
    // 清空数据时，隐藏提示
    setTipsVisible(false);
    ICDMetaData::ruleMap::const_iterator it = rules.begin();
    for (int row = 0; it != rules.end(); ++it) {
        const ICDMetaData::smtMeta &item = it->second;
        if (!item) {
            continue;
        }
        updateOne(row++, item, optNew);
    }
    setActionEnabled("clean", rules.size() > 0);
    // 显示提示信息
    if (rules.empty()) {
        setTipsVisible(true);
    }
}

// 规则数据项
void DataEngineWidget::showData(const ICDMetaData::smtMeta &data)
{
    setTipsVisible(false);
    if (GlobalDefine::dtDiscern == data->type()) {  // 帧数据
        if (q_table->tableName() != q_discertName) {
            q_table->setTableName(q_discertName);
        }
        q_table->clearContents();
        ICDComplexData::smtComplex complex
            = std::dynamic_pointer_cast<ICDComplexData>(data);
        // 按码排序
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
        setActionEnabled("clean", subTable.size() > 0);
    } else {
        // 具体规则项不允许拖拽
        q_table->verticalHeader()->setSectionsMovable(false);
        if (q_table->tableName() != q_metaName) {
            q_table->setTableName(q_metaName);
        }
        updateMetaOne(data);
        // 隐藏操作按钮
        setActionEnabled("add", false);
        setActionEnabled("insert", false);
        setActionEnabled("up", false);
        setActionEnabled("down", false);
        setActionEnabled("remove", false);
        setActionEnabled("clean", false);
        setActionEnabled("db", false);
        setActionEnabled("file", false);
    }
}

// 子表
void DataEngineWidget::updateSubOne(int index,
                                    const stICDBase &data,
                                    optType type)
{
    int column(0);
    switch (type) {
    case optNew:
        // 插入空白行
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
    // 插入数据
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

// 规则属性
void DataEngineWidget::updateMetaOne(const ICDMetaData::smtMeta &data)
{
    ICDCommonData::smtCommon common = SMT_CONVERT(ICDCommonData, data);
    if (!common) {
        return;
    }
    int resizeRow = -1;
    stQueryDic dic;
    QList<QPair<QString, QString>> datas;   // 显示要素
    QPair<QString, QString> pairData;
    // 查询字典数据
    dic.dic = GlobalDefine::dicDataType;
    dic.dicType = GlobalDefine::dictDec;
    dic.condition = QString::number(common->type()).toStdString();

    QVariantList args;
    args.append(qVariantFromValue((void*)&dic));
    jnotify->send("edit.queryDictionary", args);

    // 构造显示数据
    pairData.first = QStringLiteral("数据类型");
    pairData.second = dic.result.empty()
        ? QStringLiteral("无效") : QString(dic.result.c_str());
    datas.append(pairData);
    pairData.first = QStringLiteral("数据名称");
    pairData.second = QString(common->name().c_str());
    datas.append(pairData);
    pairData.first = QStringLiteral("数据标识");
    pairData.second = QString(common->proCode().c_str());
    datas.append(pairData);
    pairData.first = QStringLiteral("起始字号");
    pairData.second = QString::number(common->index());
    datas.append(pairData);
    pairData.first = QStringLiteral("数据长度");
    pairData.second = QStringLiteral("%1 字节").arg(common->length());
    datas.append(pairData);
    pairData.first = QStringLiteral("默认值");
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

            args.clear();
            args.append(qVariantFromValue((void*)&dic));
            jnotify->send("edit.queryDictionary", args);

            pairData.first = QStringLiteral("帧类型");
            pairData.second = QString(dic.result.c_str());
            datas.insert(1, pairData);
        } else if (data->type() == GlobalDefine::dtCheck) {
            ICDCheckData::smtCheck check
                = std::dynamic_pointer_cast<ICDCheckData>(data);
            QPair<QString, QString> &ref = datas[5];
            ref.first = QStringLiteral("校验起始位");
            ref.second = QString::number(check->start());
            pairData.first = QStringLiteral("校验终止位");
            pairData.second = QString::number(check->end());
            datas.insert(6, pairData);
            dic.dic = GlobalDefine::dicCheckType;
            dic.dicType = GlobalDefine::dictDec;
            dic.condition = QString::number(check->checkType())
                .toStdString();

            args.clear();
            args.append(qVariantFromValue((void*)&dic));
            jnotify->send("edit.queryDictionary", args);

            pairData.first = QStringLiteral("校验类型");
            pairData.second = QString(dic.result.c_str());
            datas.insert(1, pairData);
        } else if (data->type() == GlobalDefine::dtFrameCode) {
            ICDFrameCodeData::smtFrameCode frameCode
                = std::dynamic_pointer_cast<ICDFrameCodeData>(data);
            pairData.first = QStringLiteral("帧数据");
            ICDComplexData::smtComplex complex = frameCode->data();
            if (!complex) {
                pairData.second = QStringLiteral("不绑定");
            } else {
                pairData.second = QStringLiteral("绑定<%1>")
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
            pairData.first = QStringLiteral("数据范围");
            pairData.second = QString().append(lstRange.first())
                .append(" ~ ").append(lstRange.last());
            datas.insert(5, pairData);
            pairData.first = QStringLiteral("比例尺");
            pairData.second = QString::number(common->scale(), 'g', 16);
            datas.insert(7, pairData);
            pairData.first = QStringLiteral("单位");
            pairData.second = QString(common->unit().c_str());
            datas.insert(7, pairData);
            pairData.first = QStringLiteral("偏置");
            pairData.second = QString::number(common->offset(), 'g', 16);
            datas.insert(7, pairData);
            pairData.first = QStringLiteral("特征值描述");
            pairData.second.clear();
            std::unordered_map<double, std::string> items = common->values();
            std::unordered_map<double, std::string>::iterator it = items.begin();
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
        ref.second = QStringLiteral("%1 字节")
            .arg(bit->lengthOfByte().c_str());
        QString value;
        QString content;
        std::unordered_map<double, std::string> items = bit->values();
        std::unordered_map<double, std::string>::iterator it = items.begin();
        pairData.second.clear();
        if (data->type() == GlobalDefine::dtBitMap) {
            for (; it != items.end(); ++it) {
                if (it->second.empty()) {
                    continue;   // 隐藏描述为空的数据
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
            pairData.first = QStringLiteral("比例尺");
            pairData.second = QString::number(bit->scale(), 'g', 16);
            datas.insert(6, pairData);
            pairData.first = QStringLiteral("单位");
            pairData.second = QString(bit->unit().c_str());
            datas.insert(6, pairData);
            pairData.first = QStringLiteral("偏置");
            pairData.second = QString::number(bit->offset(), 'g', 16);
            datas.insert(6, pairData);
            QString fill;
            pairData.second.clear();
            for (; it != items.end(); ++it) {
                if (it->second.empty()) {
                    continue;   // 隐藏描述为空的数据
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
        pairData.first = QStringLiteral("特征值描述");
        pairData.second = pairData.second.trimmed();
        resizeRow = 7;
        datas.insert(6, pairData);
        pairData.first = QStringLiteral("比特位");
        pairData.second = QString("%1~%2")
            .arg(bit->start()).arg(bit->start() + bit->length() - 1);
        datas.insert(5, pairData);
    }
    pairData.first = QStringLiteral("描述");
    pairData.second = QString(common->remark().c_str());
    datas.append(pairData);
    // 更新表
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

// 机型
void DataEngineWidget::updateOne(int index,
                                 const PlaneNode::smtPlane &data,
                                 optType type)
{
    int column(0);
    switch (type) {
    case optNew:
        // 插入空白行
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
    // 插入数据
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
        // 插入空白行
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
    // 插入数据
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
        // 插入空白行
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
    // 插入数据
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
    // 查询字典数据
    dic.dic = GlobalDefine::dicDataType;
    dic.dicType = GlobalDefine::dictDec;
    switch (type) {
    case optNew:
        // 插入空白行
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
        // 复合数据记录子表ID
        q_table->setItemData(index, column,
                             common->rule().c_str(), ComplexTable);
    }
    // 插入数据
    q_table->setItemData(index, column, indentify, Qt::UserRole);
    q_table->setItemValue(index, column, common->name().c_str());
    q_table->setItemValue(index, ++column, common->proCode().c_str());
    if (frameCode) {
        q_table->setItemData(index, column,
                             frameCode->bindingSerial(), Qt::UserRole);
    }
    if (optCopy == type) {
        // 复制的数据，根据前一项的数据重新设置起始字号
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

    QVariantList args;
    args.append(qVariantFromValue((void*)&dic));
    jnotify->send("edit.queryDictionary", args);

    q_table->setItemValue(index, column,
                          dic.result.empty()
                          ? QStringLiteral("无效") : dic.result.c_str());
    q_table->setItemValue(index, ++column, data->remark().c_str());
    q_table->setItemData(index, column, data->byteLength(), Qt::UserRole);
}

// 获取当前选中项数据
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
    if (row == q_newIndex) {    // 新增数据
        if (q_table->property("copyData").toInt() == q_newIndex) {
            // 如果当前选中数据是拷贝数据，直接从内存获取
            QString keys = q_table->property("source").toString();
            keys = keys.mid(keys.indexOf("##") + strlen("##"));
            if (keys.isEmpty()) {
                return result;
            }
            QVariantList args;
            args.append(qVariantFromValue((void*)&result));
            args.append(qVariantFromValue((void*)&keys));
            jnotify->send("edit.querySingleElement", args);
        } else {    // 插入或者新增
        }
    } else {    // 编辑数据
        QString key = q_table->itemData(row, column, Qt::UserRole).toString();
        // 查询拷贝数据
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

// 启/停用信号槽
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

// 新增机型
void DataEngineWidget::newPlane()
{
    if (!q_table) {
        return;
    }
    QString id;
    stPlane plane;
    QVariantList args;
    args.append(qVariantFromValue((void*)&id));
    args.append(int(GlobalDefine::ntPlane));
    jnotify->send("edit.queryId", args);

    plane.nCode = id.toInt();
    plane.sName = QStringLiteral("机型%1").arg(plane.nCode).toStdString();
    plane.sSign = QString("plane_%1").arg(plane.nCode).toStdString();
    PlaneNode::smtPlane smtData(new PlaneNode(plane));
    // 更新表
    q_newIndex = q_table->rowCount();
    updateOne(q_newIndex, smtData, optNew);
    q_table->selectRow(q_newIndex);
}

// 更新机型编辑数据界面
void DataEngineWidget::updatePlaneUI(GlobalDefine::OptionType option)
{
    stPlane base;
    PlaneNode::smtPlane plane
        = std::dynamic_pointer_cast<PlaneNode>(currentData());
    // 查询当前选中数据
    if (!plane) {
        int column = 0;
        int row = q_table->currentRow();
        if (row < 0) {
            return;
        }
        QString id;
        QVariantList args;
        args.append(qVariantFromValue((void*)&id));
        args.append(int(GlobalDefine::ntPlane));
        jnotify->send("edit.queryId", args);

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
    q_loggingWidget->initUIData(MetaUI::wdPlane, data);
    q_loggingWidget->setVisible(true);
}

// 新增系统
void DataEngineWidget::newSystem()
{
    if (!q_table) {
        return;
    }
    QString id;
    stSystem system;
    QVariantList args;
    args.append(qVariantFromValue((void*)&id));
    args.append(int(GlobalDefine::ntSystem));
    jnotify->send("edit.queryId", args);

    system.nCode = id.toInt();
    system.sName = QStringLiteral("系统%1").arg(system.nCode).toStdString();
    system.sSign = QString("system_%1").arg(system.nCode).toStdString();
    SystemNode::smtSystem smtData(new SystemNode(system));
    // 更新表
    q_newIndex = q_table->rowCount();
    updateOne(q_newIndex, smtData, optNew);
    q_table->selectRow(q_newIndex);
}

// 更新系统编辑数据界面
void DataEngineWidget::updateSystemUI(GlobalDefine::OptionType option)
{
    stSystem base;
    // 查询当前选中数据
    SystemNode::smtSystem system
        = std::dynamic_pointer_cast<SystemNode>(currentData());
    if (!system) {
        int column = 0;
        int row = q_table->currentRow();
        if (row < 0) {
            return;
        }
        QString id;
        QVariantList args;
        args.append(qVariantFromValue((void*)&id));
        args.append(int(GlobalDefine::ntSystem));
        jnotify->send("edit.queryId", args);

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
    q_loggingWidget->initUIData(MetaUI::wdSystem, data);
    q_loggingWidget->setVisible(true);
}

// 新增ICD表
void DataEngineWidget::newICDTable()
{
    if (!q_table) {
        return;
    }
    QString id;
    QString keys;
    stICDBase table;

    QVariantList args;
    args.append(qVariantFromValue((void*)&keys));
    jnotify->send("edit.queryNodeKeys", args);

    args.clear();
    args.append(qVariantFromValue((void*)&id));
    args.append(int(GlobalDefine::ntTable));
    jnotify->send("edit.queryId", args);

    table.check = false;
    table.sName = id.toStdString();
    table.sID = id.mid(id.lastIndexOf("_") + strlen("_")).toStdString();
    table.sDescribe = QStringLiteral("表%1")
        .arg(q_table->rowCount() + 1).toStdString();
    table.sCode = QString("table_%1")
        .arg(q_table->rowCount() + 1).toStdString();
    table.nLength = 0;
    table.sGroup = keys.toStdString();
    TableNode::smtTable smtData(new TableNode(table));
    // 更新表
    q_newIndex = q_table->rowCount();
    updateOne(q_newIndex, smtData, optNew);
    q_table->selectRow(q_newIndex);
}

// 更新ICD表编辑数据界面
void DataEngineWidget::updateICDTableUI(GlobalDefine::OptionType option)
{
    stICDBase base;
    // 查询当前选中数据
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
    q_loggingWidget->initUIData(MetaUI::wdTable, data);
    q_loggingWidget->setVisible(true);
}

// 新增规则
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
    // 更新表
    updateOne(q_newIndex, smtData, optNew);
    q_table->selectRow(q_newIndex);

    // 将还未保存的数据长度设为0，使长度校验正确提示
    q_table->setItemData(q_newIndex, q_table->columnCount() - 1,
                         0, Qt::UserRole);
}

// 更新规则编辑数据界面
void DataEngineWidget::updateICDRuleUI(GlobalDefine::OptionType option)
{
    ICDMetaData::smtMeta meta
        = std::dynamic_pointer_cast<ICDMetaData>(currentData());
    if (GlobalDefine::optNew == option) {    // 新增
        if (!meta) {
            int column = 0;
            stTableRules rule;
            // 查询界面数据
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
            // 更新字节序号和顺序号
            meta = meta->clone();
            meta->setIndex(q_table->itemValue(q_newIndex, 2).toInt());
            meta->setSerial(q_table->rowCount());
            // 帧识别码，重置绑定信息
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

// 更新数据录入界面
void DataEngineWidget::updateDetailUI(const _UIData &data)
{
    ICDMetaData::smtMeta meta
        = *reinterpret_cast<ICDMetaData::smtMeta *>(data.data);
    if (!meta) {
        return;
    }
    if (IcdDefine::icdCommon == meta->metaType()) {
        if (GlobalDefine::dtHead == meta->type()) {
            q_loggingWidget->initUIData(MetaUI::wdHeader, data);
        } else if (GlobalDefine::dtCounter == meta->type()) {
            q_loggingWidget->initUIData(MetaUI::wdCounter, data);
        } else if (GlobalDefine::dtCheck == meta->type()) {
            q_loggingWidget->initUIData(MetaUI::wdCheck, data);
        } else if (GlobalDefine::dtFrameCode == meta->type()) {
            q_loggingWidget->initUIData(MetaUI::wdFrameCode, data);
        } else {
            q_loggingWidget->initUIData(MetaUI::wdCommon, data);
        }
    } else if (IcdDefine::icdCustom == meta->metaType()) {
        if (GlobalDefine::dtBuffer == meta->type()) {
            q_loggingWidget->initUIData(MetaUI::wdBuffer, data);
        }
    } else if (IcdDefine::icdBit == meta->metaType()) {
        if (GlobalDefine::dtBitMap == meta->type()) {
            q_loggingWidget->initUIData(MetaUI::wdBitMap, data);
        } else if (GlobalDefine::dtBitValue == meta->type()) {
            q_loggingWidget->initUIData(MetaUI::wdBitValue, data);
        }
    } else if (IcdDefine::icdComplex == meta->metaType()) {
        if (GlobalDefine::dtComplex == meta->type()) {
            q_loggingWidget->initUIData(MetaUI::wdComplex, data);
        } else if (GlobalDefine::dtDiscern == meta->type()) {
            q_loggingWidget->initUIData(MetaUI::wdDiscern, data);
        }
    }
    q_loggingWidget->setVisible(true);
}

// 新增子表
void DataEngineWidget::newSubTable()
{
    if (!q_table) {
        return;
    }
    int maxCount = 0;
    QVariantList args;
    args.append(qVariantFromValue((void*)&maxCount));
    jnotify->send("edit.queryMaxTableCount", args);
    if (q_table->rowCount() >= maxCount) {

        QMessageBox::information(this, QStringLiteral("新增"),
                                 QStringLiteral("子表已达上限，无法新增！如需提升上限，请联系管理员！"));
        return;
    }

    QString id;
    QString keys;
    stICDBase base;
    args.clear();
    args.append(qVariantFromValue((void*)&keys));
    jnotify->send("edit.queryNodeKeys", args);

    QStringList keyLst = keys.split("/", QString::SkipEmptyParts);
    if (keyLst.size() < 2) {
        return;
    }
    args.clear();
    args.append(qVariantFromValue((void*)&id));
    args.append(int(GlobalDefine::ntTable));
    jnotify->send("edit.queryId", args);

    base.sName = id.toStdString();
    base.sID = id.mid(id.lastIndexOf("_") + strlen("_")).toStdString();
    base.sDescribe = QStringLiteral("表%1")
        .arg(q_table->rowCount() + 1).toUpper().toStdString();
    base.sCode = QString("%1").arg(q_table->rowCount() + 1,
                                   2, 16, QChar('0')).toStdString();
    base.sGroup = QString("%1/%2").arg(keyLst.at(0))
        .arg(keyLst.at(1)).toStdString();
    base.sRemark = "##1";
    // 更新表
    q_newIndex = q_table->rowCount();
    updateSubOne(q_newIndex, base, optNew);
    q_table->selectRow(q_newIndex);
}

// 更新子表编辑数据界面
void DataEngineWidget::updateSubTableUI(GlobalDefine::OptionType option)
{
    stICDBase base;
    // 查询当前选中数据
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
        base.sRemark.insert(0, q_table->itemValue(row, ++column).toString().toStdString() + "##");
    } else {
        base = table->icdBase();
    }
    _UIData data;
    data.data = &base;
    data.type = option;
    q_loggingWidget->initUIData(MetaUI::wdSubTable, data);
    q_loggingWidget->setVisible(true);
}

// 显/隐状态提示
void DataEngineWidget::setTipsVisible(bool visible)
{
    if (!q_edtStatus) {
        return;
    }
    q_edtStatus->setVisible(visible);
    if (visible) {
        TableNode::smtTable table = SMT_CONVERT(TableNode, q_data);
        if (!table) {
            return;
        }
        int total = table->icdBase().nLength;
        int use = table->length();
        QString tips = QStringLiteral("数据项总长：%1 字节，").arg(total)
                + QStringLiteral("已定义： %1 字节，").arg(table->length())
                + QStringLiteral("实际规划： %1 字节")
                .arg(table->realLength());
        q_edtStatus->setText(tips);
        if (-1 == q_newIndex) {
            setActionEnabled("add", true);
            setActionEnabled("insert", true);
        }
        if (table->lengthCheck()) {
            // 需要进行长度检查时才变更操作按钮状态
            if (use > total) {
                setActionEnabled("add", false);
                setActionEnabled("insert", false);
            }
        }
    }
}

// 查询当前选中行所处bit位的第一项数据序号
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
    // 查询
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

// 查询能否将当前数据合并到前一项或者后一项数据
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
        // 非复合/帧规则数据，当前数据为已有数据
        metaNext = table->rule(meta->serial() + 1);
    } else {
        if (q_table->currentRow() == q_newIndex && -1 != q_newIndex) {
            // 当前数据为未保存的新增数据
            metaNext = table->rule(meta->serial());
        } else {
            // 当前数据为已有数据
            metaNext = table->rule(meta->serial() + 1);
        }
    }
    ICDBitData::smtBit bitPre = SMT_CONVERT(ICDBitData, metaPre);
    ICDBitData::smtBit bitNext = SMT_CONVERT(ICDBitData, metaNext);
    if (bitPre) {
        // 判断能否向前合并
        if (bitPre->end() < bitOriginal->start()) {
            result = true;  // 可以合并
        }
        // 后一项为bit型，分两种情况
        if (bitNext) {
            // 1、两个同一类型的bit之间
            if (bitPre->index() == bitNext->index()) {
                if (result) { // 可以向前合并
                    if (bitOriginal->end() >= bitNext->start()) {
                        result = false; // 无法合并
                    }
                }
            } else {    // 2、不同类型的bit之间
                if (!result) {  // 不能向前合并
                    if (bitOriginal->end() < bitNext->start()) {
                        result = true;  // 可以向后合并
                    }
                }
            }
        }
    } else {
        // 判断能否向后合并
        if (bitNext && bitOriginal->end() < bitNext->start()) {
            result = true;  // 可以合并
        }
    }

    return result;
}

// 显示右键菜单
void DataEngineWidget::showMenu()
{
    QMenu menu(q_table);
    if (q_newIndex != q_table->currentRow()) {
        menu.addAction(QStringLiteral("复制"), this, SLOT(slotCopy()));
    }
    QAction *act = menu.addAction(QStringLiteral("粘贴"), this,
                                  SLOT(slotPaste()));
    // 根据源数据和目的数据层次，决定粘贴选项可用状态
    act->setEnabled(canPasted());
    menu.exec(QCursor::pos());
}

// 当前表是否允许粘贴复制的数据
bool DataEngineWidget::canPasted() const
{
    bool result = true;
    QString keys = q_table->property("source").toString();
    if (keys.isEmpty() || -1 != q_newIndex) {
        // 没有源数据或者表中有未保存数据
        result = false;
    } else {    //
        QStringList typeList = keys.mid(0, keys.indexOf("##")).split("_");
        const int level = typeList.first().toInt();
        if (level == q_dataType) {  // 源数据和目标数据层级相同
            if (GlobalDefine::ntTable < q_dataType) {
                // 规则数据层
                const int subType = typeList.last().toInt();
                if (GlobalDefine::dtComplex == q_subType) {
                    // 目标子类型为复合数据，则允许粘贴除子表数据之外的所有数据
                    result = (GlobalDefine::dtDiscern != subType);
                } else if (GlobalDefine::dtDiscern == q_subType) {
                    // 目标子类型为子表数据，则只允许粘贴子表数据
                    result = (GlobalDefine::dtDiscern == subType);
                } else {
                    // 目标层级不允许粘贴源数据
                    result = false;
                }
            }
        } else {    // 源数据和目标数据层级不同
            const int subType = typeList.last().toInt();
            if (GlobalDefine::ntTable > q_dataType) {
                // 目标层级在表层次以上，不允许粘贴源数据
                result = false;
            } else {
                if (GlobalDefine::ntTable == q_dataType) {
                    // 表层次                // 查询当前节点是否加载数据
                    int loadState = 0;
                    QVariantList args;
                    args.append(qVariantFromValue((void*)&loadState));
                    QString command("loadedState");
                    args.append(qVariantFromValue((void*)&command));
                    jnotify->send("edit.queryNodeFlag", args);

                    if (Qt::Unchecked == loadState) {
                        result = false; // 未加载规则数据，不允许复制
                    } else {
                        if (GlobalDefine::ntRule == level) {
                            // 源数据为规则数据层，不允许粘贴子表数据
                            result = (GlobalDefine::dtDiscern != subType);
                        } else {
                            // 目标层级不允许粘贴源数据
                            result = false;
                        }
                    }
                } else {
                    // 规则数据层
                    if (GlobalDefine::dtComplex == q_subType) {
                        // 目标子类型为复合数据，则允许粘贴规则数据
                        result = (GlobalDefine::ntTable == level);
                    } else {
                        // 目标层级不允许粘贴源数据
                        result = false;
                    }
                }
            }
        }
    }

    return result;
}

// 删除数据
void DataEngineWidget::deleteRule(int pos)
{
    TableNode::smtTable table = SMT_CONVERT(TableNode, q_data);
    ICDMetaData::ruleMap rules = table->allRule();
    if (rules.empty()) {
        return;
    }
    // 将后一项数据往前挪，最后一项数据即为需要删除的项
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
            index = bitPre->index() - bitPre->start() / 8 + bit->start() / 8; // 偏移字节
        }
        else {
            if (metaPre) {
                index = metaPre->index() + metaPre->byteLength();
            }
            if (bit) {
                index += bit->start() / 8;   // 偏移字节
            }
        }
        meta->setSerial(meta->serial() - 1);
        meta->setIndex(index);
        it->second = meta;
        metaPre = meta;
        bitPre = bit;
        it = itNext;
    }
    // 删除最后一项数据
    if (it != rules.end()) {
        rules.erase(it);
    }
    // 更新内存
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
    // 设置数据新序号
    meta->setSerial(pos);
    ICDBitData::smtBit bitOriginal = SMT_CONVERT(ICDBitData, meta);
    // 1、确定数据字节序号和偏移量
    bool mergerd = false;
    ICDMetaData::ruleMap rules = table->allRule();
    ICDMetaData::smtMeta metaPre = table->rule(pos - 1);
    ICDBitData::smtBit bitPre = SMT_CONVERT(ICDBitData, metaPre);
    if (bitOriginal) {
        if (bitPre) {
            // 判断能否向前合并
            if (bitPre->end() < bitOriginal->start()) {
                mergerd = true;  // 可以合并
                meta->setIndex(bitPre->index() - bitPre->start() / 8 + bitOriginal->start() / 8);
            }
        }
    }
    if (!mergerd) {
        // 不能合并
        if (metaPre) {
            meta->setIndex(metaPre->index() + metaPre->byteLength());
        } else {
            meta->setIndex(0);
        }
        if (bitOriginal) {
            // 偏移字节
            meta->setIndex(meta->index() + bitOriginal->start() / 8);
        }
    }

    // 2、整理规则数据
    // 从目标数据开始，调整字节序号
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
            // 当前项能够向前一项合并
            index = bitPre->index() - bitPre->start() / 8 + bit->start() / 8;
        } else {
            index = newData->index() + newData->byteLength();
            if (bit) {
                index += bit->start() / 8;  // 偏移字节
            }
        }
        next->setSerial(newData->serial() + 1);
        next->setIndex(index);
        bitPre = bit;
        newData = next;
    }
    // 保存原始的最后一项数据
    rules[newData->serial()] = newData;
    // 保存内存
    table->setRule(rules);
}

// 变更机型位置
void DataEngineWidget::movePlane(int from, int to)
{
    const int count = q_vData.size();
    if (from < 0 || to >= count) {
        return;
    }
    ICDElement::smtElement element = q_vData[from];
    if (from < to) {    // 从前向后移
        for (int i = from; i < to; ++i) {
            q_vData[i] = q_vData[i + 1];
        }
    } else {    // 从后向前移
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

// 变更系统位置
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
    if (from < to) {    // 从前向后移
        for (int i = from; i < to; ++i) {
            systems[i] = systems[i + 1];
        }
    } else {    // 从后向前移
        for (int i = from; i > to; --i) {
            systems[i] = systems[i - 1];
        }
    }
    systems[to] = system;
    plane->setSystem(systems);
    showData(systems);
    q_table->selectRow(to);
}

// 变更表位置
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
    if (from < to) {    // 从前向后移
        for (int i = from; i < to; ++i) {
            tables[i] = tables[i + 1];
        }
    } else {    // 从后向前移
        for (int i = from; i > to; --i) {
            tables[i] = tables[i - 1];
        }
    }
    tables[to] = table;
    system->setTable(tables);
    showData(tables);
    q_table->selectRow(to);
}

// 变更规则位置
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
    // 将制定位置的规则数据移动到目标位置，分删除和插入两步完成
    deleteRule(from);
    insertRule(meta, to);
    showData(table->allRule());
    q_table->selectRow(to - 1);
}

// 变更子表位置
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
    if (from < to) {    // 从前向后移
        for (int i = from; i < to; ++i) {
            tables[i] = tables[i + 1];
        }
    } else {    // 从后向前移
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
        // 如果数据长度变更，则重新计算表数据项字节号
        bool reorder = false;   // 重新整理数据标志
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
            // 如果数据长度变更，则重新计算表数据项字节号
            bool reorder = false;   // 重新整理数据标志
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

// 重新计算规则数据字节序号
void DataEngineWidget::reorderTable(TableNode::smtTable &table, int from)
{
    ICDMetaData::ruleMap rules = table->allRule();
    if (rules.empty()) {
        return;
    }
    // 从目标位置开始，重新计算字节序号
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
                index += bit->start() / 8;  // 偏移字节
            }
        }
        meta->setIndex(index);
        metaPre = meta;
        bitPre = bit;
    }
    // 更新内存
    table->setRule(rules);
}

// 查询当前规则数据父表
TableNode::smtTable DataEngineWidget::parentTable() const
{
    TableNode::smtTable table = 0;
    // 分表下面的节点和规则节点两类，表下面的节点，查询q_data副本数据
    if (GlobalDefine::ntRule == q_dataType
        && !(GlobalDefine::dtComplex == q_subType
        || GlobalDefine::dtDiscern == q_subType)) {
        // 非复合/帧规则数据，查询父表
        QString keys;
        ICDElement::smtElement element;
        QVariantList args;
        args.append(qVariantFromValue((void*)&keys));
        jnotify->send("edit.queryNodeKeys", args);

        // 关键码集中包含该当前规则数据id，查询父表需将其删除
        keys.truncate(keys.lastIndexOf("/"));
        args.clear();
        args.append(qVariantFromValue((void*)&element));
        args.append(qVariantFromValue((void*)&keys));
        jnotify->send("edit.querySingleElement", args);

        table = SMT_CONVERT(TableNode, element);
    } else {
        // 规则节点，在拷贝数据中查询父表
        table = SMT_CONVERT(TableNode, q_data);
    }

    return table;
}

// 更新编辑数据界面
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
                QVariantList args;
                args.append(qVariantFromValue((void*)&loaded));
                args.append(qVariantFromValue((void*)&condition));
                jnotify->send("edit.queryNodeFlag", args);
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
    // 根据是否有位置变动，判定是否启用保存按钮
    if (q_changePos >= 0) {
        q_loggingWidget->enableOptionButton(true);
    }
}

// 表当前选中项变更
void DataEngineWidget::slotCurrentItemChanged(QStandardItem *current,
                                              QStandardItem *previous)
{
    if (!current) {
        // 隐藏数据录入窗口
        q_loggingWidget->setVisible(false);
        setActionEnabled("up", false);
        setActionEnabled("down", false);
        setActionEnabled("remove", false);
    } else {
        if (previous && current->row() == previous->row()) {
            // 未变更选中行
            return;
        } else {
            if (q_newIndex < 0 || q_newIndex != current->row()) {
                updateLoggingUI(GlobalDefine::optEdit);
            } else {     // 新增数据并未保存用户就切换选中表中其他数据
                updateLoggingUI(GlobalDefine::optNew);
            }
        }
        int count = q_table->rowCount();
        // 除了具体规则节点，其他节点均允许变更位置
        if (count > 1 && GlobalDefine::ntRule != q_dataType
            || GlobalDefine::dtDiscern == q_subType
            || GlobalDefine::dtComplex == q_subType) {
            setActionEnabled("up", 0 != current->row());
            setActionEnabled("down", count - 1 != current->row());
            // 变更数据顺序后未保存，不允许删除
            setActionEnabled("remove", q_changePos < 0 ? true :false);
        } else if (1 == count) {
            setActionEnabled("remove", true);
        }
    }
    // 切换节点后，根据列表中是否有新增数据，决定新增按钮是否可用
    if (-1 != q_newIndex) {
        setActionEnabled("add", false);
        setActionEnabled("insert", false);
        setActionEnabled("up", false);
        setActionEnabled("down", false);
    } else {
        // 变更数据位置后，未保存数据，不允许新增、插入
        if (q_changePos >= 0) {
            setActionEnabled("add", false);
            setActionEnabled("insert", false);
        }
    }
}

// 点击表数据项
void DataEngineWidget::slotItemPressed(QStandardItem *item)
{
    Q_UNUSED(item);
    if (Qt::RightButton == QApplication::mouseButtons()) {
        // 具体规则定义时，不显示右键菜单
        if (q_metaName != q_table->tableName()) {
            showMenu();
        }
    }
}

// 表自定义菜单
void DataEngineWidget::slotCustomContextMenu(const QPoint &pos)
{
    // 具体规则定义时，不显示右键菜单
    if (q_metaName != q_table->tableName()) {
        QMenu menu(q_table);
        int row = q_table->currentRow();
        if (row > -1 && q_newIndex != row) {
            menu.addAction(QStringLiteral("复制"), this, SLOT(slotCopy()));
        }
        QAction *act = menu.addAction(QStringLiteral("粘贴"), this,
                                      SLOT(slotPaste()));
        // 根据源数据和目的数据层次，决定粘贴选项可用状态
        act->setEnabled(canPasted());
        menu.exec(q_table->viewport()->mapToGlobal(pos));
    }
}

// 表数据变更
void DataEngineWidget::slotContentChanged()
{
    // 仅在规则表数据时显示提示
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
        // 规则序号从1开始
        moveRule(oldIndex + 1, newIndex + 1);
    } else if (GlobalDefine::ntRule == q_dataType) {
        if (GlobalDefine::dtComplex == q_subType) {
            // 规则序号从1开始
            moveRule(oldIndex + 1, newIndex + 1);
        } else if (GlobalDefine::dtDiscern == q_subType) {
            moveSubTable(oldIndex, newIndex);
        }
    }
    q_loggingWidget->enableOptionButton(true);
}

// 保存数据
void DataEngineWidget::slotSave2Memory(void *data, bool &result)
{
    if (!data) {
        return;
    }

    // 保存数据到内存，分拷贝、插入、新增和编辑，拷贝、插入、新增直接保存到内存
    // 编辑由于涉及到数据的位置更换，所有先保存到副本数据，再一次性更新内存数据
    bool reInitNeed = true;
    if (q_table->property("copyData").toInt() == q_newIndex
        && -1 != q_newIndex) {  // 保存拷贝数据
        QVector<int> params;
        ICDElement::smtElement element;
        QString keys = q_table->property("source").toString();
        keys = keys.mid(keys.indexOf("##") + strlen("##"));
        params << (int)&keys << (int)data;

        QVariantList args;
        args.append(qVariantFromValue((void*)&params));
        args.append(qVariantFromValue((void*)&element));
        jnotify->send("edit.savePastedData", args);

        result = true;
    } else if (-1 != q_newIndex
        && (q_table->rowCount() - 1) != q_newIndex
        && q_table->currentRow() == q_newIndex) {   // 插入数据
        std::vector<int> params;
        params.push_back(q_table->currentRow());
        params.push_back((int)data);

        QVariantList args;
        args.append(qVariantFromValue((void*)&params));
        args.append(qVariantFromValue((void*)&result));
        jnotify->send("edit.insertNodeData", args);

    } else {    // 新增（插入数据到最后一项算作新增）/编辑
        if (-1 != q_changePos) {    // 改变了数据项的位置
            // 保存数据到副本
            saveEditData(data);
            std::vector<int> params;
            if (GlobalDefine::ntUnknown == q_dataType) {
                params.push_back((int)&q_vData);
            } else {
                params.push_back((int)&q_data);
            }
            params.push_back(q_changePos);

            QVariantList args;
            args.append(qVariantFromValue((void*)&params));
            args.append(qVariantFromValue((void*)&result));
            jnotify->send("edit.reorderNodeData", args);

            // 保存到内存
        } else { // 新增或者仅编辑了数据
            QVariantList args;
            args.append(qVariantFromValue((void*)data));
            args.append(qVariantFromValue((void*)&result));
            jnotify->send("edit.updateNodeData", args);
            reInitNeed = false;
        }
    }
    if (result) {
        // 重新初始化界面
        int row = q_table->currentRow();
        if (reInitNeed) {
            reInit();
        }
        q_table->selectRow(row);
    } else {
        setActionEnabled("add", result);
        setActionEnabled("insert", result);
    }
}

void DataEngineWidget::slotCanceled()
{
    if (-1 != q_changePos) {
        return;  // 只有拖动数据位置时才重新初始化
    }

    // 重新初始化界面
    int row = q_table->currentRow();
    reInit();
    q_table->selectRow(row);

    //q_loggingWidget->setVisible(false);
}

// 新增
void DataEngineWidget::slotNew(const QVariant &param)
{
    if (GlobalDefine::ntUnknown == q_dataType) {   // 机型
        newPlane();
    } else if (GlobalDefine::ntPlane == q_dataType) {  // 系统
        newSystem();
    } else if (GlobalDefine::ntSystem == q_dataType) { // 表
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
    // 变更数据项以后，必须保存数据之后才能继续移动
    if (-1 != q_newIndex) {
        q_table->verticalHeader()->setSectionsMovable(false);
    }
}

// 插入
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
        if (index < 0) {    // 未选中数据，插入到末尾，做新增处理
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
//                 // 如果插入位置在两个同字节的bit之间，
//                 // 则将插入位置移动到bit字节之前
//                 rule.nSerial = queryFirstBitSerial(rule.nCode);
//             }
            rule.sName = QString("item_%1").arg(index).toStdString();
            rule.uType = GlobalDefine::dtU8;
            rule.sPrgCode = QString("code_%1").arg(index).toStdString();
            ICDMetaData::smtMeta smtData
                = ICDFactory::instance().CreatObject(rule);
            // 更新表
            updateOne(q_newIndex, smtData, optNew);
            q_table->selectRow(q_newIndex);
        }
        // 变更数据项以后，必须保存数据之后才能继续移动
        if (-1 != q_newIndex) {
            q_table->verticalHeader()->setSectionsMovable(false);
        }
    }
}

// 上移
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
        // 规则序号从1开始
        moveRule(row + 1, row);
    } else if (GlobalDefine::ntRule == q_dataType) {
        if (GlobalDefine::dtComplex == q_subType) {
            // 规则序号从1开始
            moveRule(row + 1, row);
        } else if (GlobalDefine::dtDiscern == q_subType) {
            moveTable(row, row - 1);
        }
    }
    q_loggingWidget->enableOptionButton(true);
}

// 下移
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
        // 规则序号从1开始
        moveRule(row + 1, row + 2);
    } else if (GlobalDefine::ntRule == q_dataType) {
        if (GlobalDefine::dtComplex == q_subType) {
            // 规则序号从1开始
            moveRule(row + 1, row + 2);
        } else if (GlobalDefine::dtDiscern == q_subType) {
            moveTable(row, row + 1);
        }
    }
    q_loggingWidget->enableOptionButton(true);
}

// 删除
void DataEngineWidget::slotDelete()
{
    bool result = false;
    bool showTip = false;
    if (GlobalDefine::ntUnknown == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("确认提示"),
            QStringLiteral("即将删除机型及所有下属数据\r\n确认删除？"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
            return;
        }
    } else if (GlobalDefine::ntPlane == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("确认提示"),
            QStringLiteral("即将删除分系统及所有下属数据\r\n确认删除？"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
            return;
        }
    } else if (GlobalDefine::ntSystem == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("确认提示"),
            QStringLiteral("即将删除ICD表及所有下属数据")
            + QStringLiteral("\r\n确认删除该ICD表数据？"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
            return;
        }
    } else if (GlobalDefine::ntTable == q_dataType
               || GlobalDefine::ntRule == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("确认提示"),
            QStringLiteral("即将删除数据定义及所有下属数据")
            + QStringLiteral("\r\n确认删除该项数据？"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
            return;
        }
        showTip = true;
    } else {
        return;
    }
    q_table->setProperty("copyData", -1);   // 重置拷贝数据属性
    // 删除
    int current = q_table->currentRow();
    if (current == q_newIndex && -1 != q_newIndex) {    // 删除尚未保存的数据
        q_newIndex = -1;
        q_table->removeRow(current);
        setActionEnabled("add", true);
        setActionEnabled("insert", q_dataType > GlobalDefine::ntSystem);
        q_table->verticalHeader()->setSectionsMovable(true);
    } else {    // 删除内存数据
        QVariant data = q_table->itemData(current, 0, Qt::UserRole);
        QVariant subTable = q_table->itemData(current, 0, ComplexTable);
        QString key = data.toString();
        // 复合表需要将子表加入规则id，与导航树同步
        if (subTable.isValid()) {
            key.append("/").append(subTable.toString());
        }
        std::vector<QString> keys(1, key);
        QVariantList args;
        args.append(qVariantFromValue((void*)&keys));
        args.append(qVariantFromValue((void*)&result));
        jnotify->send("edit.deleteNode", args);
        if (result) {
            if (q_table->currentRow() == q_newIndex) {
                q_newIndex = -1;
            }
            q_table->removeRow(q_table->currentRow());
            setTipsVisible(showTip);
            // 重新初始化
            int row = q_table->currentRow();
            reInit();
            q_table->selectRow(row);
        }
    }
}

// 清空
void DataEngineWidget::slotClear()
{
    bool result = false;
    if (GlobalDefine::ntUnknown == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("确认提示"),
            QStringLiteral("即将删除所有机型，包括所有下属数据")
            + QStringLiteral("\r\n确认删除？"))) {
            return;
        }
    } else if (GlobalDefine::ntPlane == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("确认提示"),
            QStringLiteral("即将删除当前机型的所有系统，包括所有下属数据")
            + QStringLiteral("\r\n确认删除？"))) {
            return;
        }
    } else if (GlobalDefine::ntSystem == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("确认提示"),
            QStringLiteral("即将删除当前系统的所有协议表，包括所有下属数据")
            + QStringLiteral("\r\n确认删除？"))) {
            return;
        }
    } else if (GlobalDefine::ntTable == q_dataType
               || GlobalDefine::ntRule == q_dataType) {
        if (QMessageBox::No == QMessageBox::question(this,
            QStringLiteral("确认提示"),
            QStringLiteral("即将删除当前表的所有数据定义，包括所有下属数据")
            + QStringLiteral("\r\n确认删除？"))) {
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
    QVariantList args;
    args.append(qVariantFromValue((void*)&keys));
    args.append(qVariantFromValue((void*)&result));
    jnotify->send("edit.deleteNode", args);

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

// 保存
void DataEngineWidget::slotSave2Source(GlobalDefine::DataSource type)
{
    int loaded = 0;
    // 查询是否有未加载数据
    QVariantList args;
    args.append(qVariantFromValue((void*)&loaded));
    QString command("loadedState");
    args.append(qVariantFromValue((void*)&command));
    jnotify->send("edit.queryNodeFlag", args);

    if (GlobalDefine::wholeState != loaded) {
        if (QMessageBox::No == QMessageBox::warning(this,
            QStringLiteral("保存提示"),
            QStringLiteral("当前选中项有未加载数据，\r\n")
            + QStringLiteral("继续保存将丢失这部分数据，是否继续？"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {

            return;
        }
    }
    QString tip;
    QString err;
    QString actionName;
    QVector<int> params;
    if (GlobalDefine::dsDatabase == type) {   // 保存数据库
        actionName = "db";
        setActionEnabled(actionName, false);
        params << (int)&err << (int)&q_defaultPath;
        args.clear();
        args.append(qVariantFromValue((void*)&params));
        args.append(int(GlobalDefine::dsDatabase));
        jnotify->send("edit.saveMemoryData", args);

        tip = QStringLiteral("保存数据到数据库");
    } else if (GlobalDefine::dsFile == type) {  // 保存文件
        actionName = "file";
        setActionEnabled(actionName, false);
        QString file = QFileDialog::getSaveFileName(this, QStringLiteral("保存协议数据"), q_defaultPath,
                                                    "JSON files (*.json);;"
                                                    "XML files (*.xml)");
        if (file.isEmpty()) {
            setActionEnabled(actionName, true);
            return;
        }
        q_defaultPath = file;
        params << (int)&err << (int)&q_defaultPath;
        args.clear();
        args.append(qVariantFromValue((void*)&params));
        args.append(int(GlobalDefine::dsFile));
        jnotify->send("edit.saveMemoryData", args);

        tip = QStringLiteral("保存数据到文件[%1]").arg(q_defaultPath);
    }

    if (!err.isEmpty()) {
        tip.append(QStringLiteral("失败！\n[%1]").arg(err));
        setActionEnabled(actionName, true);
    } else {
        tip.append(QStringLiteral("成功！"));
        if ("file" == actionName) {
            setActionEnabled(actionName, true);
        }
    }

    QMessageBox::information(this, QStringLiteral("保存数据"), tip);
}

// 复制
void DataEngineWidget::slotCopy()
{
    QString keys;
    QString id = q_table->itemData(q_table->currentRow(),
                                   0, Qt::UserRole).toString();
    QVariantList args;
    args.append(qVariantFromValue((void*)&keys));
    jnotify->send("edit.queryNodeKeys", args);

    // 将源数据信息存储起来[数据层级-子类型层级##id]
    if (keys.isEmpty()) {
        keys = id;
    } else {
        keys.append("/").append(id);
    }
    keys.prepend(QString("%1_%2##").arg(q_dataType).arg(q_subType));
    q_table->setProperty("source", keys);
}

// 粘贴
void DataEngineWidget::slotPaste()
{
    QString keys = q_table->property("source").toString();
    keys = keys.mid(keys.indexOf("##") + strlen("##"));
    if (keys.isEmpty()) {
        return;
    }
    // 查询源数据
    ICDElement::smtElement element;
    QVariantList args;
    args.append(qVariantFromValue((void*)&element));
    args.append(qVariantFromValue((void*)&keys));
    jnotify->send("edit.querySingleElement", args);
    if (!element) {
        QMessageBox::information(this, QStringLiteral("粘贴"),
                                 QStringLiteral("粘贴失败！，数据源无效！"));
        return;
    }
    if (GlobalDefine::ntUnknown == q_dataType) {   // 根节点
        PlaneNode::smtPlane plane = SMT_CONVERT(PlaneNode, element);
        // 更新机型
        if (plane) {
            q_newIndex = q_table->rowCount();
            updateOne(q_newIndex, plane, optCopy);
            q_table->selectRow(q_newIndex);
        }
    } else if (GlobalDefine::ntPlane == q_dataType) {   // 机型
        SystemNode::smtSystem system = SMT_CONVERT(SystemNode, element);
        // 更新系统
        if (system) {
            q_newIndex = q_table->rowCount();
            updateOne(q_newIndex, system, optCopy);
            q_table->selectRow(q_newIndex);
        }
    } else if (GlobalDefine::ntSystem == q_dataType) {  // 系统
        // 更新表
        q_newIndex = q_table->rowCount();
        updateOne(q_newIndex, SMT_CONVERT(TableNode, element), optCopy);
        q_table->selectRow(q_newIndex);
    } else if (GlobalDefine::ntTable == q_dataType) {   // 表
        // 更新规则
        q_newIndex = q_table->rowCount();
        updateOne(q_newIndex, SMT_CONVERT(ICDMetaData, element), optCopy);
        q_table->selectRow(q_newIndex);
        // 将还未保存的数据长度设为0，使长度校验正确提示
        q_table->setItemData(q_newIndex, q_table->columnCount() - 1,
                             0, Qt::UserRole);
    } else if (GlobalDefine::ntRule == q_dataType) {    // 规则
        if (GlobalDefine::dtComplex == q_subType) {
            // 更新表
            q_newIndex = q_table->rowCount();
            updateOne(q_newIndex,
                      SMT_CONVERT(ICDMetaData, element),
                      optCopy);
            q_table->selectRow(q_newIndex);
            // 将还未保存的数据长度设为0，使长度校验正确提示
            q_table->setItemData(q_newIndex, q_table->columnCount() - 1,
                                 0, Qt::UserRole);
        } else if (GlobalDefine::dtDiscern == q_subType) {
            int maxCount = 0;
            args.clear();
            args.append(qVariantFromValue((void*)&maxCount));
            jnotify->send("edit.queryMaxTableCount", args);
            if (q_table->rowCount() >= maxCount) {

                QMessageBox::information(this, QStringLiteral("粘贴"),
                                         QStringLiteral("子表已达上限，无法粘贴！如需提升上限，请联系管理员！"));
                return;
            }
            TableNode::smtTable table = SMT_CONVERT(TableNode, element);
            if (table) {
                // 更新表
                q_newIndex = q_table->rowCount();
                updateSubOne(q_newIndex, table->icdBase(), optCopy);
                q_table->selectRow(q_newIndex);
            }
        }
    }
    // 变更数据项以后，必须保存数据之后才能继续移动
    if (-1 != q_newIndex) {
        q_table->verticalHeader()->setSectionsMovable(false);
    }
}

void DataEngineWidget::setActionEnabled(const QString &name, bool enabled)
{
    QVariantList args;
    args.append(name);
    args.append(enabled);
    jnotify->send("edit.toolbar.action.enabled", args);
}
