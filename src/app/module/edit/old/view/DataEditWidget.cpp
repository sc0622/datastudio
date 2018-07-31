#include "precomp.h"
#include "DataEditWidget.h"
#include "loggingwidget.h"
#include "jwt/jxmltable.h"
#include "jwt/jsplitter.h"
#include "ObjectEdit.h"
#include "KernelClass/icdfactory.h"
#include "KernelClass/icdcomplexdata.h"
#include "KernelClass/icdcheckdata.h"
#include "KernelClass/icdbitdata.h"
#include "KernelClass/icdframecodedata.h"
#include "KernelClass/icdcounterdata.h"
#include "KernelClass/icdarraydata.h"
#include "KernelClass/icdnumericdata.h"

DataEditWidget::DataEditWidget(QWidget *parent)
    : QWidget(parent)
    , vehicleName_("VehicleTable")
    , systemName_("SystemTable")
    , tableName_("Table")
    , ruleName_("RuleTable")
    , frameName_("FrameTable")
    , objectName_("ObjectTable")
    , dataType_(-1)
    , subType_(-1)
    , newIndex_(-1)
    , changedPos(-1)
    , data_(nullptr)
{
    defaultPath_ = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    QVBoxLayout* veriLayoutMain = new QVBoxLayout(this);
    veriLayoutMain->setContentsMargins(0, 0, 0, 0);
    veriLayoutMain->setSpacing(1);

    splitterMain_ = new JSplitter(this);
    splitterMain_->setObjectName("edit.view.splitter.main");
    splitterMain_->setScales(QList<double>() << 2 << 1);
    splitterMain_->setHandleWidth(3);
    veriLayoutMain->addWidget(splitterMain_);

    tableView_ = new JXmlTable(this);
    tableView_->loadConfig(JMain::instance()->configDir().append("/old/xmltable.xml"),
                           vehicleName_);
    tableView_->setSortingEnabled(false);
    tableView_->setContextMenuPolicy(Qt::CustomContextMenu);
    actionPast_ = new QAction(QStringLiteral("ճ��"), tableView_);
    tableView_->addAction(actionPast_);
    splitterMain_->addWidget(tableView_);

    loggingWidget_ = new LoggingWidget(this);
    splitterMain_->addWidget(loggingWidget_);

    editStatus_ = new QLabel(this);
    editStatus_->setObjectName("__status__");
    editStatus_->setFixedHeight(22);
    editStatus_->setTextFormat(Qt::RichText);
    editStatus_->setAlignment(Qt::AlignCenter);
    editStatus_->hide();
    veriLayoutMain->addWidget(editStatus_);

    connect(loggingWidget_, &LoggingWidget::dataSaved, this, &DataEditWidget::slotSave2Memory);
    connect(loggingWidget_, &LoggingWidget::canceled, this, &DataEditWidget::slotCanceled);
    connect(actionPast_, &QAction::triggered, this, &DataEditWidget::slotPaste);

    enableConnection(true);

    // Ĭ������
    loggingWidget_->setVisible(false);

    //
    jnotify->on("edit.toolbar.item.add", this, [=](JNEvent &){
        slotNew(GlobalDefine::dtNumeric);
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

DataEditWidget::~DataEditWidget()
{
    JMain::saveWidgetState(splitterMain_);
}

bool DataEditWidget::init()
{
    JMain::restoreWidgetState(splitterMain_);

    return true;
}

// ��ʼ������
void DataEditWidget::initUI(int type, void *data)
{
    if (!data) {
        return;
    }

    // ɾ���ϴ�����
    if (GlobalDefine::ntUnknown == dataType_) {
        data_.reset();
    } else {
        vData_.clear();
    }
    // ���ñ����
    changedPos = -1;

    dataType_ = type;
    subType_ = -1;
    newIndex_ = -1;
    loggingWidget_->setVisible(false);
    setActionEnabled("add", true);
    setActionEnabled("insert", false);
    setActionEnabled("remove", false);
    setActionEnabled("up", false);
    setActionEnabled("down", false);
    setActionEnabled("file", true);
    // ��ѯ�����Ƿ��б��
    int dataState = 0;
    QVariantList args;
    args.append(qVariantFromValue((void*)&dataState));
    QString command("nodeState");
    args.append(qVariantFromValue((void*)&command));
    jnotify->send("edit.queryNodeFlag", args);

    setActionEnabled("db", dataState);

    tableView_->verticalHeader()->setSectionsMovable(true);    // Ĭ�Ͽ��Խ���������λ��
    //q_table->setSortingEnabled(true);   // Ĭ������������
    tableView_->setProperty("copyData", -1);   // ���ÿ�����������

    // ��ʼ�����ݹ����жϿ����źŲ�
    enableConnection(false);
    if (GlobalDefine::ntUnknown == type) {
        vData_ = *reinterpret_cast<std::vector<ICDElement::smtElement> *>(data);
        const int count = vData_.size();
        std::vector<PlaneNode::smtPlane> planes;
        for (int i = 0; i < count; ++i) {
            planes.push_back(std::dynamic_pointer_cast<PlaneNode>(vData_[i])->clone());
        }
        showData(planes);
        setActionEnabled("file", !planes.empty());
    } else if (type == GlobalDefine::ntVehicle) {
        ICDElement::smtElement element = *reinterpret_cast<ICDElement::smtElement *>(data);
        PlaneNode::smtPlane plane = std::dynamic_pointer_cast<PlaneNode>(element);
        auto &systems = plane->allSystem();
        foreach (const auto &system, systems) {
            vData_.push_back(system);
        }
        data_ = plane->clone();
        showData(systems);
    } else if (type == GlobalDefine::ntSystem) {
        ICDElement::smtElement element = *reinterpret_cast<ICDElement::smtElement *>(data);
        SystemNode::smtSystem system = std::dynamic_pointer_cast<SystemNode>(element);
        data_ = system->clone();
        showData(system->allTable());
    } else if (type == GlobalDefine::ntTable) {
        ICDElement::smtElement element = *reinterpret_cast<ICDElement::smtElement *>(data);
        TableNode::smtTable table = std::dynamic_pointer_cast<TableNode>(element);
        data_ = table->clone();
        if (table) {
            showData(table->allRule());
        }
    } else if (type == GlobalDefine::ntRule) {
        std::vector<int> &rule = *reinterpret_cast<std::vector<int> *>(data);
        if (2 == rule.size()) {
            subType_ = rule.at(0);
            ICDElement::smtElement element = *reinterpret_cast<ICDElement::smtElement *>(rule.at(1));
            //element = *reinterpret_cast<ICDElement::smtElement *>(rule.at(1));
            if (GlobalDefine::dtComplex == subType_) {
                TableNode::smtTable table = SMT_CONVERT(TableNode, element);
                if (table) {
                    data_ = table->clone();
                    showData(table->allRule());
                } else {
                    showData(ICDMetaData::ruleMap());
                }
            } else {
                ICDMetaData::smtMeta meta = SMT_CONVERT(ICDMetaData, element);
                if (meta) {
                    data_ = meta->clone();
                    showData(meta);
                    if (GlobalDefine::dtFrame != meta->type()) {
                        _UIData data;
                        data.data = &meta;
                        data.type = GlobalDefine::optEdit;
                        updateDetailUI(data);
                    }
                }
            }
        }
        // �����Ĺ�����棬���ñ���
        setActionEnabled("db", false);
        setActionEnabled("file", false);
    }
    // ��ʼ����������ñ��źŲ�
    enableConnection(true);
    // ��ʼ����ɣ����½�����ʾ��Ϣ
    if (type == GlobalDefine::ntTable) {
        int loaded = 0;
        args.clear();
        args.append(qVariantFromValue((void*)&loaded));
        command = "loadedState";
        args.append(qVariantFromValue((void*)&command));
        jnotify->send("edit.queryNodeFlag", args);

        if (Qt::Unchecked == loaded) {
            editStatus_->setText(QStringLiteral("��δ�������ݣ����ܽ��в���"));
            editStatus_->setHidden(loaded);
            // ���°�ť״̬
            setActionEnabled("add", loaded);
            setActionEnabled("insert", loaded);
        } else {
            setTipsVisible(ruleName_ == tableView_->tableName());
        }
    } else {
        setTipsVisible(ruleName_ == tableView_->tableName());
    }
}

// ������ʼ��
void DataEditWidget::reInit()
{
    void *data = nullptr;
    std::vector<int> rule;
    ICDElement::smtElement element;
    std::vector<ICDElement::smtElement> elements;
    // ��ѯԭʼ�������³�ʼ����
    if (GlobalDefine::ntUnknown == dataType_) {
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
        if (GlobalDefine::ntRule == dataType_) {
            rule.push_back(subType_);
            rule.push_back((int)&element);
            data = &rule;
        } else {
            data = &element;
        }
    }
    initUI(dataType_, data);
}

// ��ѯ��ǰ��ʾ��������Ϣ
int DataEditWidget::queryTableInformation(const QString &type) const
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
    if (GlobalDefine::ntRule == dataType_) {
        if (GlobalDefine::dtComplex != subType_) {
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
QMap<QString, QString> DataEditWidget::queryExistedData(const QString &section) const
{
    QMap<QString, QString> result;
    if (GlobalDefine::ntRule == dataType_
            && !(GlobalDefine::dtComplex == subType_
                 || GlobalDefine::dtFrame == subType_)) {
        // �Ǹ���/֡�������ݣ���ѯ����
        QString keys;
        ICDElement::smtElement element;
        QVariantList args;
        args.append(qVariantFromValue((void*)&keys));
        jnotify->send("edit.queryNodeKeys", args);
        // �ؼ��뼯�а����õ�ǰ��������id����ѯ�����轫��ɾ��
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
            if (!(meta = it->second) || !data_
                    || atoi(data_->id().c_str()) == meta->serial()) {
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
        if (!tableView_ || column < 0 || column >(tableView_->colorCount() - 1)) {
            return result;
        }
        QString value;
        const int count = tableView_->rowCount();
        for (int i = 0; i < count; ++i) {
            if (tableView_->currentRow() == i) {
                continue;
            }
            value = tableView_->itemValue(i, column).toString();
            result[value] = value;
        }
    }

    return result;
}

// ��ѯ֡����
QMap<int, QString> DataEditWidget::queryFrameData(const QString &type) const
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
        if (GlobalDefine::dtFrame == meta->type()) {
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
void DataEditWidget::fillBitIndex(ICDMetaData::smtMeta &meta)
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
int DataEditWidget::queryOffset(const ICDMetaData::smtMeta &meta) const
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
        if (-1 != newIndex_ && tableView_->currentRow() == newIndex_) {
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
        if (-1 == newIndex_) { // �༭
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
bool DataEditWidget::queryWidgetState(const QString &name) const
{
    bool result = false;
    if (name == "add") {
        result = jnotify->send("edit.toolbar.action.querystate", name).toBool();
    } else if (name == "clean") {
        result = jnotify->send("edit.toolbar.action.querystate", name).toBool();
    }

    return result;
}

// ������������ģ�������
void DataEditWidget::dealCommand(int command, const QVariant &param)
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

// ������ʾ����
void DataEditWidget::showData(const PlaneNode::planeVector &planes)
{
    if (NULL == tableView_) {
        return;
    }
    if (tableView_->tableName() != vehicleName_) {
        tableView_->setTableName(vehicleName_);
    }
    tableView_->clearContents();
    const int count = planes.size();
    for (int row = 0; row < count; ++row) {
        updateOne(row, planes[row], optNew);
    }
    setActionEnabled("clean", count > 0);
}

// ������ʾϵͳ
void DataEditWidget::showData(const SystemNode::systemVector &systems)
{
    if (NULL == tableView_) {
        return;
    }
    if (tableView_->tableName() != systemName_) {
        tableView_->setTableName(systemName_);
    }
    tableView_->clearContents();
    const int count = systems.size();
    for (int i = 0; i < count; ++i) {
        updateOne(i, systems[i], optNew);
    }
    setActionEnabled("clean", count > 0);
}

// ������ʾICD��
void DataEditWidget::showData(const TableNode::tableVector &tables)
{
    if (NULL == tableView_) {
        return;
    }
    if (tableView_->tableName() != tableName_) {
        tableView_->setTableName(tableName_);
    }
    tableView_->clearContents();
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

// ������ʾ����
void DataEditWidget::showData(const ICDMetaData::ruleMap &rules)
{
    if (NULL == tableView_) {
        return;
    }
    if (tableView_->tableName() != ruleName_) {
        tableView_->setTableName(ruleName_);
    }
    tableView_->clearContents();
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
    setActionEnabled("clean", rules.size() > 0);
    // ��ʾ��ʾ��Ϣ
    if (rules.empty()) {
        setTipsVisible(true);
    }
}

// ����������
void DataEditWidget::showData(const ICDMetaData::smtMeta &data)
{
    setTipsVisible(false);
    if (GlobalDefine::dtFrame == data->type()) {  // ֡����
        if (tableView_->tableName() != frameName_) {
            tableView_->setTableName(frameName_);
        }
        tableView_->clearContents();
        ICDComplexData::smtComplex complex = std::dynamic_pointer_cast<ICDComplexData>(data);
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
        setActionEnabled("clean", subTable.size() > 0);
    } else {
        // ��������������ק
        tableView_->verticalHeader()->setSectionsMovable(false);
        if (tableView_->tableName() != objectName_) {
            tableView_->setTableName(objectName_);
        }
        updateMetaOne(data);
        // ���ز�����ť
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

// �ӱ�
void DataEditWidget::updateSubOne(int index, const stICDBase &data, optType type)
{
    int column(0);
    switch (type) {
    case optNew:
        // ����հ���
        tableView_->insertRow(index);
        break;
    case optEdit:
        break;
    case optDelete:
        tableView_->removeRow(index);
        break;
    case optCopy:
        tableView_->insertRow(index);
        tableView_->setProperty("copyData", index);
    default:break;
    }
    // ��������
    tableView_->setItemData(index, column, data.sID.c_str(), Qt::UserRole);
    tableView_->setItemData(index, column, data.sGroup.c_str(), Qt::UserRole + 1);
    tableView_->setItemValue(index, column, data.sName.c_str());
    tableView_->setItemValue(index, ++column, "0x" + QString::fromStdString(data.sCode));
    tableView_->setItemValue(index, ++column, data.sRemark.c_str());
    tableView_->setItemValue(index, ++column, data.sDescribe.c_str());
}

// ��������
void DataEditWidget::updateMetaOne(const ICDMetaData::smtMeta &data)
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

    QVariantList args;
    args.append(qVariantFromValue((void*)&dic));
    jnotify->send("edit.queryDictionary", args);

    // ������ʾ����
    pairData.first = QStringLiteral("��������");
    pairData.second = dic.result.empty() ? QStringLiteral("��Ч") : QString(dic.result.c_str());
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
            //
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

            pairData.first = QStringLiteral("����������");
            pairData.second = QString(dic.result.c_str());
            datas.insert(1, pairData);
        } else if (data->type() == GlobalDefine::dtArray) {
            ICDArrayData::smtArray array
                    = std::dynamic_pointer_cast<ICDArrayData>(data);
            dic.dic = GlobalDefine::dicArrayType;
            dic.dicType = GlobalDefine::dictDec;
            dic.condition = QString::number(array->arrayType()).toStdString();

            args.clear();
            args.append(qVariantFromValue((void*)&dic));
            jnotify->send("edit.queryDictionary", args);

            pairData.first = QStringLiteral("��������");
            pairData.second = QString(dic.result.c_str());
            datas.insert(1, pairData);
        } else if (data->type() == GlobalDefine::dtCheck) {
            ICDCheckData::smtCheck check = std::dynamic_pointer_cast<ICDCheckData>(data);
            QPair<QString, QString> &ref = datas[5];
            ref.first = QStringLiteral("��ʼƫ��");
            ref.second = QString::number(check->start());
            pairData.first = QStringLiteral("��ֹƫ��");
            pairData.second = QString::number(check->end());
            datas.insert(6, pairData);
            dic.dic = GlobalDefine::dicCheckType;
            dic.dicType = GlobalDefine::dictDec;
            dic.condition = QString::number(check->checkType())
                    .toStdString();

            args.clear();
            args.append(qVariantFromValue((void*)&dic));
            jnotify->send("edit.queryDictionary", args);

            pairData.first = QStringLiteral("У������");
            pairData.second = QString(dic.result.c_str());
            datas.insert(1, pairData);
        } else if (data->type() == GlobalDefine::dtFrameCode) {
            ICDFrameCodeData::smtFrameCode frameCode = std::dynamic_pointer_cast<ICDFrameCodeData>(data);
            pairData.first = QStringLiteral("֡������");
            ICDComplexData::smtComplex complex = frameCode->data();
            if (!complex) {
                pairData.second = QStringLiteral("����");
            } else {
                pairData.second = QStringLiteral("��<%1>").arg(complex->name().c_str());
            }
            datas.insert(3, pairData);
        } else {
            int rowOffset = 5;
            if (data->type() == GlobalDefine::dtNumeric) {
                ICDNumericData::smtNumeric numeric = std::dynamic_pointer_cast<ICDNumericData>(data);
                dic.dic = GlobalDefine::dicNumericType;
                dic.dicType = GlobalDefine::dictDec;
                dic.condition = QString::number(numeric->numericType()).toStdString();

                args.clear();
                args.append(qVariantFromValue((void*)&dic));
                jnotify->send("edit.queryDictionary", args);

                pairData.first = QStringLiteral("��ֵ����");
                pairData.second = QString(dic.result.c_str());
                datas.insert(1, pairData);
                ++rowOffset;
            }

            QStringList lstRange = QString(common->range().c_str()).split("~");
            if (lstRange.size() < 2) {
                lstRange.append("");
            }
            if (lstRange.first().isEmpty()) {
                lstRange.first() = "-Inf";
            }
            if (lstRange.last().isEmpty()) {
                lstRange.last() = "+Inf";
            }

            // specs
            pairData.first = QStringLiteral("����ֵ����");
            pairData.second.clear();
            std::map<double, std::string> items = common->values();
            std::map<double, std::string>::iterator it = items.begin();
            for (; it != items.end(); ++it) {
                pairData.second += QString("%1: %2\r\n").arg(it->first).arg(it->second.c_str());
            }
            pairData.second = pairData.second.trimmed();
            datas.insert(rowOffset + 2, pairData);
            // range
            pairData.first = QStringLiteral("���ݷ�Χ");
            pairData.second = QString().append(lstRange.first())
                    .append(" ~ ").append(lstRange.last());
            datas.insert(rowOffset, pairData);
            // unit
            pairData.first = QStringLiteral("��λ");
            pairData.second = QString(common->unit().c_str());
            datas.insert(rowOffset + 2, pairData);
            // offset
            pairData.first = QStringLiteral("ƫ��");
            pairData.second = QString::number(common->offset(), 'g', 16);
            datas.insert(rowOffset + 2, pairData);
            // scale
            pairData.first = QStringLiteral("������");
            pairData.second = QString::number(common->scale(), 'g', 16);
            datas.insert(rowOffset + 2, pairData);
            resizeRow = rowOffset + 5;
        }
    } else if (data->metaType() == IcdDefine::icdBit) {
        ICDBitData::smtBit bit = SMT_CONVERT(ICDBitData, data);
        QPair<QString, QString> &ref = datas[4];
        ref.second = QStringLiteral("%1 �ֽ�").arg(bit->lengthOfByte().c_str());
        std::map<double, std::string> items = bit->values();
        std::map<double, std::string>::iterator it = items.begin();
        pairData.second.clear();
        if (data->type() == GlobalDefine::dtBitMap) {
            for (; it != items.end(); ++it) {
                if (it->second.empty()) {
                    continue;   // ��������Ϊ�յ�����
                }
                QString content;
                QStringList lst = QString(it->second.c_str()).split(":");
                QStringList lstValue = lst.last().split(";");
                if (lstValue.size() > 1) {
                    if (!lstValue.first().isEmpty()) {
                        content.append("0: ").append(lstValue.first());
                    }
                    if (!lstValue.last().isEmpty()) {
                        content.append(", 1: ").append(lstValue.last());
                    }
                } else {
                    content.clear();
                }
                QString(it->second.c_str()).split(":").last();
                QString value = QString::number((int)it->first);
                if (content.isEmpty()) {
                    pairData.second += QString("%1: %2\r\n").arg(value, 2, QChar('0')).arg(lst.first());
                } else {
                    pairData.second += QString("%1: %2 [%3]\r\n")
                            .arg(value, 2, QChar('0')).arg(lst.first()).arg(content);
                }
            }
        } else if (data->type() == GlobalDefine::dtBitValue) {
            // unit
            pairData.first = QStringLiteral("��λ");
            pairData.second = QString(bit->unit().c_str());
            datas.insert(6, pairData);
            // scale
            pairData.first = QStringLiteral("������");
            pairData.second = QString::number(bit->scale(), 'g', 16);
            datas.insert(6, pairData);
            // offset
            pairData.first = QStringLiteral("ƫ��");
            pairData.second = QString::number(bit->offset(), 'g', 16);
            datas.insert(6, pairData);
            pairData.second.clear();
            for (; it != items.end(); ++it) {
                if (it->second.empty()) {
                    continue;   // ��������Ϊ�յ�����
                }
                pairData.second += QString("%1: %2\r\n").arg((int)it->first) .arg(it->second.c_str());
            }
        }
        pairData.first = QStringLiteral("����ֵ����");
        pairData.second = pairData.second.trimmed();
        resizeRow = 7;
        datas.insert(6, pairData);
        pairData.first = QStringLiteral("����λ");
        pairData.second = QString("%1~%2").arg(bit->start()).arg(bit->start() + bit->length() - 1);
        datas.insert(5, pairData);
    }

    pairData.first = QStringLiteral("����");
    pairData.second = QString(common->remark().c_str());
    datas.append(pairData);
    // ���±�
    QListIterator<QPair<QString, QString>> it = datas;
    int row = 0;
    tableView_->setRowCount(datas.size());
    while (it.hasNext()) {
        pairData = it.next();
        tableView_->setItemValue(row, 0, pairData.first);
        tableView_->setItemValue(row++, 1, pairData.second);
    }
    if (resizeRow > -1) {
        tableView_->resizeRowToContents(resizeRow);
    }
}

// ����
void DataEditWidget::updateOne(int index, const PlaneNode::smtPlane &data, optType type)
{
    int column(0);
    switch (type) {
    case optNew:
        // ����հ���
        tableView_->insertRow(index);
        break;
    case optEdit:
        break;
    case optDelete:
        tableView_->removeRow(index);
        break;
    case optCopy:
        tableView_->insertRow(index);
        tableView_->setProperty("copyData", index);
    default:break;
    }
    if (!data) {
        return;
    }
    // ��������
    tableView_->setItemData(index, column, data->id().c_str(), Qt::UserRole);
    tableView_->setItemValue(index, column, data->name().c_str());
    tableView_->setItemValue(index, ++column, data->sign().c_str());
    tableView_->setItemValue(index, ++column, data->describe().c_str());
}

void DataEditWidget::updateOne(int index, const SystemNode::smtSystem &data, optType type)
{
    int column(0);
    switch (type) {
    case optNew:
        // ����հ���
        tableView_->insertRow(index);
        break;
    case optEdit:
        break;
    case optDelete:
        tableView_->removeRow(index);
        break;
    case optCopy:
        tableView_->insertRow(index);
        tableView_->setProperty("copyData", index);
    default:break;
    }
    if (!data) {
        return;
    }
    // ��������
    tableView_->setItemData(index, column, data->numeralId(), Qt::UserRole);
    tableView_->setItemValue(index, column, data->name().c_str());
    tableView_->setItemValue(index, ++column, data->sign().c_str());
    tableView_->setItemValue(index, ++column, data->describe().c_str());
}

void DataEditWidget::updateOne(int index, const TableNode::smtTable &data, optType type)
{
    stICDBase base = data->icdBase();
    int column(0);
    switch (type) {
    case optNew:
        // ����հ���
        tableView_->insertRow(index);
        break;
    case optEdit:
        break;
    case optDelete:
        tableView_->removeRow(index);
        break;
    case optCopy:
        tableView_->insertRow(index);
        tableView_->setProperty("copyData", index);
    default:
        break;
    }
    if (!data) {
        return;
    }
    // ��������
    tableView_->setItemData(index, column, data->key().c_str(), Qt::UserRole);
    tableView_->setItemData(index, column, base.sGroup.c_str(), Qt::UserRole + 1);
    tableView_->setItemValue(index, column, base.sName.c_str());
    tableView_->setItemValue(index, ++column, QString::fromStdString(base.sCode));
    tableView_->setItemValue(index, ++column, base.sRemark.c_str());
    tableView_->setItemValue(index, ++column, base.sDescribe.c_str());
}

void DataEditWidget::updateOne(int index, const ICDMetaData::smtMeta &data, optType type)
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
        tableView_->insertRow(index);
        break;
    case optEdit:
        break;
    case optDelete:
        tableView_->removeRow(index);
        break;
    case optCopy:
        tableView_->insertRow(index);
        tableView_->setProperty("copyData", index);
    default:break;
    }
    const ICDCommonData::smtCommon common = std::dynamic_pointer_cast<ICDCommonData>(data);
    const ICDFrameCodeData::smtFrameCode frameCode = std::dynamic_pointer_cast<ICDFrameCodeData>(data);
    QString indentify = QString::number(common->serial());
    if (GlobalDefine::dtComplex == common->type()) {
        // �������ݼ�¼�ӱ�ID
        tableView_->setItemData(index, column, common->rule().c_str(), ComplexTable);
    }
    // ��������
    tableView_->setItemData(index, column, indentify, Qt::UserRole);
    tableView_->setItemValue(index, column, common->name().c_str());
    tableView_->setItemValue(index, ++column, common->proCode().c_str());
    if (frameCode) {
        tableView_->setItemData(index, column, frameCode->bindingSerial(), Qt::UserRole);
    }
    if (optCopy == type) {
        // ���Ƶ����ݣ�����ǰһ�����������������ʼ�ֺ�
        int code = 0;
        if (index > 0) {
            code = tableView_->itemValue(index - 1, 2).toInt();
            code += tableView_->itemData(index - 1, tableView_->columnCount() - 1,
                                         Qt::UserRole).toInt();
        }
        tableView_->setItemValue(index, ++column, code);
    } else {
        tableView_->setItemValue(index, ++column, common->index());
    }
    tableView_->setItemValue(index, ++column, common->lengthOfByte().c_str());
    dic.condition = QString::number(common->type()).toStdString();
    tableView_->setItemData(index, ++column, common->type(), Qt::UserRole);
    tableView_->setItemData(index, column, common->subType(), Qt::UserRole + 1);

    QVariantList args;
    args.append(qVariantFromValue((void*)&dic));
    jnotify->send("edit.queryDictionary", args);

    tableView_->setItemValue(index, column,
                             dic.result.empty()
                             ? QStringLiteral("��Ч") : dic.result.c_str());
    tableView_->setItemValue(index, ++column, data->remark().c_str());
    tableView_->setItemData(index, column, data->byteLength(), Qt::UserRole);
}

// ��ȡ��ǰѡ��������
ICDElement::smtElement DataEditWidget::currentData() const
{
    ICDElement::smtElement result;
    if (!tableView_) {
        return result;
    }
    int column = 0;
    int row = tableView_->currentRow();
    if (row < 0) {
        return result;
    }
    if (row == newIndex_) {    // ��������
        if (tableView_->property("copyData").toInt() == newIndex_) {
            // �����ǰѡ�������ǿ������ݣ�ֱ�Ӵ��ڴ��ȡ
            QString keys = tableView_->property("source").toString();
            keys = keys.mid(keys.indexOf("##") + strlen("##"));
            if (keys.isEmpty()) {
                return result;
            }
            QVariantList args;
            args.append(qVariantFromValue((void*)&result));
            args.append(qVariantFromValue((void*)&keys));
            jnotify->send("edit.querySingleElement", args);
        } else {    // �����������
        }
    } else {    // �༭����
        QString key = tableView_->itemData(row, column, Qt::UserRole).toString();
        // ��ѯ��������
        if (GlobalDefine::ntUnknown == dataType_) {
            const int count = vData_.size();
            for (int i = 0; i < count; ++i) {
                if (vData_[i]->id() == key.toStdString()) {
                    result = vData_[i];
                }
            }
        } else if (GlobalDefine::ntVehicle == dataType_) {
            const int count = vData_.size();
            for (int i = 0; i < count; ++i) {
                if (vData_[i]->id() == key.toStdString()) {
                    result = vData_[i];
                }
            }
        } else if (GlobalDefine::ntSystem == dataType_) {
            SystemNode::smtSystem system = std::dynamic_pointer_cast<SystemNode>(data_);
            if (system) {
                result = system->table(key.toStdString());
            }
        } else if (GlobalDefine::ntTable == dataType_) {
            TableNode::smtTable table = std::dynamic_pointer_cast<TableNode>(data_);
            if (table) {
                result = table->rule(key.toInt());
            }
        } else if (GlobalDefine::ntRule == dataType_) {
            if (GlobalDefine::dtComplex == subType_) {
                TableNode::smtTable table = std::dynamic_pointer_cast<TableNode>(data_);
                if (table) {
                    result = table->rule(key.toInt());
                }
            } else if (GlobalDefine::dtFrame == subType_) {
                ICDComplexData::smtComplex complex = std::dynamic_pointer_cast<ICDComplexData>(data_);
                if (complex) {
                    result = complex->table(key.toStdString());
                }
            }
        }
    }

    return result;
}

// ��/ͣ���źŲ�
void DataEditWidget::enableConnection(bool enable)
{
    disconnect(tableView_, SIGNAL(currentItemChanged(QStandardItem *, QStandardItem *)),
               this, SLOT(slotCurrentItemChanged(QStandardItem *, QStandardItem *)));
    disconnect(tableView_, SIGNAL(contentChanged()),
               this, SLOT(slotContentChanged()));
    disconnect(tableView_, SIGNAL(itemPressed(QStandardItem *)),
               this, SLOT(slotItemPressed(QStandardItem *)));
    disconnect(tableView_, SIGNAL(customContextMenuRequested(const QPoint &)),
               this, SLOT(slotCustomContextMenu(const QPoint &)));
    disconnect(tableView_->verticalHeader(), SIGNAL(sectionMoved(int, int, int)),
               this, SLOT(slotDataPosChanged(int, int, int)));
    if (enable) {
        connect(tableView_, SIGNAL(currentItemChanged(QStandardItem *, QStandardItem *)),
                this, SLOT(slotCurrentItemChanged(QStandardItem *, QStandardItem *)));
        connect(tableView_, SIGNAL(contentChanged()),
                this, SLOT(slotContentChanged()));
        connect(tableView_, SIGNAL(itemPressed(QStandardItem *)),
                this, SLOT(slotItemPressed(QStandardItem *)));
        connect(tableView_, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(slotCustomContextMenu(const QPoint &)));
        connect(tableView_->verticalHeader(), SIGNAL(sectionMoved(int, int, int)),
                this, SLOT(slotDataPosChanged(int, int, int)));
    }
}

// ��������
void DataEditWidget::newPlane()
{
    if (!tableView_) {
        return;
    }
    QString id;
    stPlane plane;
    QVariantList args;
    args.append(qVariantFromValue((void*)&id));
    args.append(int(GlobalDefine::ntVehicle));
    jnotify->send("edit.queryId", args);

    plane.nCode = id.toInt();
    plane.sName = QStringLiteral("����%1").arg(plane.nCode).toStdString();
    plane.sSign = QString("plane_%1").arg(plane.nCode).toStdString();
    PlaneNode::smtPlane smtData(new PlaneNode(plane));
    // ���±�
    newIndex_ = tableView_->rowCount();
    updateOne(newIndex_, smtData, optNew);
    tableView_->selectRow(newIndex_);
}

// ���»��ͱ༭���ݽ���
void DataEditWidget::updatePlaneUI(GlobalDefine::OptionType option)
{
    stPlane base;
    PlaneNode::smtPlane plane
            = std::dynamic_pointer_cast<PlaneNode>(currentData());
    // ��ѯ��ǰѡ������
    if (!plane) {
        int column = 0;
        int row = tableView_->currentRow();
        if (row < 0) {
            return;
        }
        QString id;
        QVariantList args;
        args.append(qVariantFromValue((void*)&id));
        args.append(int(GlobalDefine::ntVehicle));
        jnotify->send("edit.queryId", args);

        base.nCode = id.toInt();
        base.sName = tableView_->itemValue(row, column).toString().toStdString();
        base.sSign = tableView_->itemValue(row, ++column).toString().toStdString();
        base.sDescribe = tableView_->itemValue(row, ++column).toString().toStdString();
    } else {
        base = plane->plane();
    }
    _UIData data;
    data.data = &base;
    data.type = option;
    loggingWidget_->initUIData(ObjectEdit::wdVehicle, data);
    loggingWidget_->setVisible(true);
}

// ����ϵͳ
void DataEditWidget::newSystem()
{
    if (!tableView_) {
        return;
    }
    QString id;
    stSystem system;
    QVariantList args;
    args.append(qVariantFromValue((void*)&id));
    args.append(int(GlobalDefine::ntSystem));
    jnotify->send("edit.queryId", args);

    system.nCode = id.toInt();
    system.sName = QStringLiteral("ϵͳ%1").arg(system.nCode).toStdString();
    system.sSign = QString("system_%1").arg(system.nCode).toStdString();
    SystemNode::smtSystem smtData(new SystemNode(system));
    // ���±�
    newIndex_ = tableView_->rowCount();
    updateOne(newIndex_, smtData, optNew);
    tableView_->selectRow(newIndex_);
}

// ����ϵͳ�༭���ݽ���
void DataEditWidget::updateSystemUI(GlobalDefine::OptionType option)
{
    stSystem base;
    // ��ѯ��ǰѡ������
    SystemNode::smtSystem system = std::dynamic_pointer_cast<SystemNode>(currentData());
    if (!system) {
        int column = 0;
        int row = tableView_->currentRow();
        if (row < 0) {
            return;
        }
        QString id;
        QVariantList args;
        args.append(qVariantFromValue((void*)&id));
        args.append(int(GlobalDefine::ntSystem));
        jnotify->send("edit.queryId", args);
        base.nCode = id.toInt();
        base.sName = tableView_->itemValue(row, column).toString().toStdString();
        base.sSign = tableView_->itemValue(row, ++column).toString().toStdString();
        base.sDescribe = tableView_->itemValue(row, ++column).toString().toStdString();
    } else {
        base = system->system();
    }
    _UIData data;
    data.data = &base;
    data.type = option;
    loggingWidget_->initUIData(ObjectEdit::wdSystem, data);
    loggingWidget_->setVisible(true);
}

// ����ICD��
void DataEditWidget::newICDTable()
{
    if (!tableView_) {
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
    table.sID = id.toStdString();
    table.sName = QStringLiteral("��%1").arg(tableView_->rowCount() + 1).toStdString();
    table.sCode = QString("table_%1").arg(tableView_->rowCount() + 1).toStdString();
    table.nLength = 0;
    table.sGroup = keys.toStdString();
    TableNode::smtTable smtData(new TableNode(table));
    // ���±�
    newIndex_ = tableView_->rowCount();
    updateOne(newIndex_, smtData, optNew);
    tableView_->selectRow(newIndex_);
}

// ����ICD��༭���ݽ���
void DataEditWidget::updateICDTableUI(GlobalDefine::OptionType option)
{
    stICDBase base;
    // ��ѯ��ǰѡ������
    TableNode::smtTable table = SMT_CONVERT(TableNode, currentData());
    if (!table) {
        int column = 0;
        int row = tableView_->currentRow();
        if (row < 0) {
            return;
        }
        base.check = false;
        base.sID = tableView_->itemData(row, column, Qt::UserRole).toString().toStdString();
        base.sGroup = tableView_->itemData(row, column, Qt::UserRole + 1).toString().toStdString();
        base.sName = tableView_->itemValue(row, column).toString().toStdString();
        base.sCode = tableView_->itemValue(row, ++column).toString().remove(0, 2).toStdString();
        base.nLength = tableView_->itemValue(row, ++column).toInt();
        base.sRemark = tableView_->itemValue(row, ++column).toString().toStdString();
    } else {
        base = table->icdBase();
    }
    _UIData data;
    data.data = &base;
    data.type = option;
    loggingWidget_->initUIData(ObjectEdit::wdTable, data);
    loggingWidget_->setVisible(true);
}

// ��������
void DataEditWidget::newICDRule(int type)
{
    if (!tableView_) {
        return;
    }
    newIndex_ = tableView_->rowCount();
    stTableRules rule;
    int index = tableView_->rowCount() - 1;
    if (index > -1) {
        rule.nSerial = tableView_->itemData(index, 0, Qt::UserRole).toInt() + 1;
        rule.nCode = tableView_->itemValue(index, 2).toInt();
        rule.nCode += tableView_->itemData(index, tableView_->columnCount() - 1, Qt::UserRole).toInt();
    } else {
        rule.nCode = 0;
        rule.nSerial = 1;
    }
    rule.sName = QString("item_%1").arg(rule.nSerial).toStdString();
    if (type > GlobalDefine::dtBuffer || type < GlobalDefine::dtHead) {
        rule.uType = GlobalDefine::dtNumeric;
    } else {
        rule.uType = type;
    }
    // subType
    switch (rule.uType) {
    case GlobalDefine::dtNumeric: rule.subType = GlobalDefine::NumericU8; break;
    default:
        break;
    }
    //
    rule.sPrgCode = QString("code_%1").arg(newIndex_).toStdString();
    ICDMetaData::smtMeta smtData = ICDFactory::instance().CreatObject(rule);
    // ���±�
    updateOne(newIndex_, smtData, optNew);
    tableView_->selectRow(newIndex_);

    // ����δ��������ݳ�����Ϊ0��ʹ����У����ȷ��ʾ
    tableView_->setItemData(newIndex_, tableView_->columnCount() - 1, 0, Qt::UserRole);
}

// ���¹���༭���ݽ���
void DataEditWidget::updateICDRuleUI(GlobalDefine::OptionType option)
{
    ICDMetaData::smtMeta meta = std::dynamic_pointer_cast<ICDMetaData>(currentData());
    if (GlobalDefine::optNew == option) {    // ����
        if (!meta) {
            int column = 0;
            stTableRules rule;
            // ��ѯ��������
            rule.sName = tableView_->itemValue(newIndex_, column).toString().toStdString();
            rule.nSerial = tableView_->itemData(newIndex_, column, Qt::UserRole).toInt();
            rule.sPrgCode = tableView_->itemValue(newIndex_, ++column).toString().toStdString();
            rule.nCode = tableView_->itemValue(newIndex_, ++column).toInt();
            rule.uLength = tableView_->itemValue(newIndex_, ++column).toInt();
            rule.uType = tableView_->itemData(newIndex_, ++column, Qt::UserRole).toInt();
            rule.subType = tableView_->itemData(newIndex_, column, Qt::UserRole + 1).toInt();
            rule.sRemark = tableView_->itemValue(newIndex_, ++column).toString().toStdString();
            meta = ICDFactory::instance().CreatObject(rule);
        } else {
            // �����ֽ���ź�˳���
            meta = meta->clone();
            meta->setIndex(tableView_->itemValue(newIndex_, 2).toInt());
            meta->setSerial(tableView_->rowCount());
            // ֡ʶ���룬���ð���Ϣ
            ICDFrameCodeData::smtFrameCode frameCode = std::dynamic_pointer_cast<ICDFrameCodeData>(meta);
            if (frameCode) {
                frameCode->bindData(nullptr);
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
void DataEditWidget::updateDetailUI(const _UIData &data)
{
    ICDMetaData::smtMeta meta
            = *reinterpret_cast<ICDMetaData::smtMeta *>(data.data);
    if (!meta) {
        return;
    }
    if (IcdDefine::icdCommon == meta->metaType()) {
        if (GlobalDefine::dtHead == meta->type()) {
            loggingWidget_->initUIData(ObjectEdit::wdHeader, data);
        } else if (GlobalDefine::dtCounter == meta->type()) {
            loggingWidget_->initUIData(ObjectEdit::wdCounter, data);
        } else if (GlobalDefine::dtCheck == meta->type()) {
            loggingWidget_->initUIData(ObjectEdit::wdCheck, data);
        } else if (GlobalDefine::dtArray == meta->type()) {
            loggingWidget_->initUIData(ObjectEdit::wdArray, data);
        } else if (GlobalDefine::dtNumeric == meta->type()) {
            loggingWidget_->initUIData(ObjectEdit::wdNumeric, data);
        } else if (GlobalDefine::dtFrameCode == meta->type()) {
            loggingWidget_->initUIData(ObjectEdit::wdFrameCode, data);
        } else {
            loggingWidget_->initUIData(ObjectEdit::wdItem, data);
        }
    } else if (IcdDefine::icdCustom == meta->metaType()) {
        if (GlobalDefine::dtBuffer == meta->type()) {
            loggingWidget_->initUIData(ObjectEdit::wdBuffer, data);
        }
    } else if (IcdDefine::icdBit == meta->metaType()) {
        if (GlobalDefine::dtBitMap == meta->type()) {
            loggingWidget_->initUIData(ObjectEdit::wdBitMap, data);
        } else if (GlobalDefine::dtBitValue == meta->type()) {
            loggingWidget_->initUIData(ObjectEdit::wdBitValue, data);
        }
    } else if (IcdDefine::icdComplex == meta->metaType()) {
        if (GlobalDefine::dtComplex == meta->type()) {
            loggingWidget_->initUIData(ObjectEdit::wdComplex, data);
        } else if (GlobalDefine::dtFrame == meta->type()) {
            loggingWidget_->initUIData(ObjectEdit::wdFrame, data);
        }
    }
    loggingWidget_->setVisible(true);
}

// �����ӱ�
void DataEditWidget::newSubTable()
{
    if (!tableView_) {
        return;
    }
    int maxCount = 0;
    QVariantList args;
    args.append(qVariantFromValue((void*)&maxCount));
    jnotify->send("edit.queryMaxTableCount", args);
    if (tableView_->rowCount() >= maxCount) {

        QMessageBox::information(this, QStringLiteral("����"),
                                 QStringLiteral("�ӱ��Ѵ����ޣ��޷������������������ޣ�����ϵ����Ա��"));
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

    base.sID = id.toStdString();
    base.sName = QStringLiteral("��%1").arg(tableView_->rowCount() + 1).toUpper().toStdString();
    base.sCode = QString("0x%1").arg(tableView_->rowCount() + 1, 2, 16, QChar('0')).toStdString();
    base.sGroup = QString("%1/%2").arg(keyLst.at(0)).arg(keyLst.at(1)).toStdString();
    base.sRemark = "1";
    // ���±�
    newIndex_ = tableView_->rowCount();
    updateSubOne(newIndex_, base, optNew);
    tableView_->selectRow(newIndex_);
}

// �����ӱ�༭���ݽ���
void DataEditWidget::updateSubTableUI(GlobalDefine::OptionType option)
{
    stICDBase base;
    // ��ѯ��ǰѡ������
    TableNode::smtTable table
            = std::dynamic_pointer_cast<TableNode>(currentData());
    if (!table) {
        int column = 0;
        int row = tableView_->currentRow();
        if (row < 0) {
            return;
        }
        base.sID = tableView_->itemData(row, column, Qt::UserRole).toString().toStdString();
        base.sGroup = tableView_->itemData(row, column, Qt::UserRole + 1).toString().toStdString();
        base.sName = tableView_->itemValue(row, column).toString().toStdString();
        base.sCode = tableView_->itemValue(row, ++column).toString().remove(0, 2).toStdString();
        base.sRemark = tableView_->itemValue(row, ++column).toString().toStdString();
        base.sDescribe = tableView_->itemValue(row, ++column).toString().toStdString();
    } else {
        base = table->icdBase();
    }
    _UIData data;
    data.data = &base;
    data.type = option;
    loggingWidget_->initUIData(ObjectEdit::wdSubTable, data);
    loggingWidget_->setVisible(true);
}

// ��/��״̬��ʾ
void DataEditWidget::setTipsVisible(bool visible)
{
    if (!editStatus_) {
        return;
    }
    editStatus_->setVisible(visible);
    if (visible) {
        TableNode::smtTable table = SMT_CONVERT(TableNode, data_);
        if (!table) {
            return;
        }
        int total = table->icdBase().nLength;
        int use = table->length();
        QString tips = QStringLiteral("�������ܳ���%1 �ֽڣ�").arg(total)
                + QStringLiteral("�Ѷ��壺 %1 �ֽڣ�").arg(table->length())
                + QStringLiteral("ʵ�ʹ滮�� %1 �ֽ�")
                .arg(table->realLength());
        editStatus_->setText(tips);
        if (-1 == newIndex_) {
            setActionEnabled("add", true);
            setActionEnabled("insert", true);
        }
        if (table->lengthCheck()) {
            // ��Ҫ���г��ȼ��ʱ�ű��������ť״̬
            if (use > total) {
                setActionEnabled("add", false);
                setActionEnabled("insert", false);
            }
        }
    }
}

// ��ѯ��ǰѡ��������bitλ�ĵ�һ���������
int DataEditWidget::queryFirstBitSerial(int bitIndex)
{
    int result = -1;
    if (!tableView_) {
        return result;
    }
    int row = tableView_->currentRow();
    if (row < 0) {
        return result;
    }
    int column = 2; // index
    int value = 0;
    // ��ѯ
    for (int i = row - 1; i > -1; --i) {
        value = tableView_->itemValue(i, column).toInt();
        if (bitIndex == value) {
            result = tableView_->itemData(i, 0, Qt::UserRole).toInt();
        } else if (value < bitIndex) {
            break;  //
        }
    }

    return result;
}

// ��ѯ�ܷ񽫵�ǰ���ݺϲ���ǰһ����ߺ�һ������
bool DataEditWidget::canMerged(const ICDMetaData::smtMeta &meta) const
{
    bool result = false;
    TableNode::smtTable table = parentTable();
    ICDBitData::smtBit bitOriginal = SMT_CONVERT(ICDBitData, meta);
    if (!table || !bitOriginal) {
        return result;
    }
    ICDMetaData::smtMeta metaPre = table->rule(meta->serial() - 1);
    ICDMetaData::smtMeta metaNext = 0;

    if (GlobalDefine::ntRule == dataType_
            && !(GlobalDefine::dtComplex == subType_
                 || GlobalDefine::dtFrame == subType_)) {
        // �Ǹ���/֡�������ݣ���ǰ����Ϊ��������
        metaNext = table->rule(meta->serial() + 1);
    } else {
        if (tableView_->currentRow() == newIndex_ && -1 != newIndex_) {
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
void DataEditWidget::showMenu()
{
    QMenu menu(tableView_);
    if (newIndex_ != tableView_->currentRow()) {
        menu.addAction(QStringLiteral("����"), this, SLOT(slotCopy()));
    }
    QAction *act = menu.addAction(QStringLiteral("ճ��"), this,
                                  SLOT(slotPaste()));
    // ����Դ���ݺ�Ŀ�����ݲ�Σ�����ճ��ѡ�����״̬
    act->setEnabled(canPasted());
    menu.exec(QCursor::pos());
}

// ��ǰ���Ƿ�����ճ�����Ƶ�����
bool DataEditWidget::canPasted() const
{
    bool result = true;
    QString keys = tableView_->property("source").toString();
    if (keys.isEmpty() || -1 != newIndex_) {
        // û��Դ���ݻ��߱�����δ��������
        result = false;
    } else {    //
        QStringList typeList = keys.mid(0, keys.indexOf("##")).split("_");
        const int level = typeList.first().toInt();
        if (level == dataType_) {  // Դ���ݺ�Ŀ�����ݲ㼶��ͬ
            if (GlobalDefine::ntTable < dataType_) {
                // �������ݲ�
                const int subType = typeList.last().toInt();
                if (GlobalDefine::dtComplex == subType_) {
                    // Ŀ��������Ϊ�������ݣ�������ճ�����ӱ�����֮�����������
                    result = (GlobalDefine::dtFrame != subType);
                } else if (GlobalDefine::dtFrame == subType_) {
                    // Ŀ��������Ϊ�ӱ����ݣ���ֻ����ճ���ӱ�����
                    result = (GlobalDefine::dtFrame == subType);
                } else {
                    // Ŀ��㼶������ճ��Դ����
                    result = false;
                }
            }
        } else {    // Դ���ݺ�Ŀ�����ݲ㼶��ͬ
            const int subType = typeList.last().toInt();
            if (GlobalDefine::ntTable > dataType_) {
                // Ŀ��㼶�ڱ������ϣ�������ճ��Դ����
                result = false;
            } else {
                if (GlobalDefine::ntTable == dataType_) {
                    // ����                // ��ѯ��ǰ�ڵ��Ƿ��������
                    int loadState = 0;
                    QVariantList args;
                    args.append(qVariantFromValue((void*)&loadState));
                    QString command("loadedState");
                    args.append(qVariantFromValue((void*)&command));
                    jnotify->send("edit.queryNodeFlag", args);

                    if (Qt::Unchecked == loadState) {
                        result = false; // δ���ع������ݣ���������
                    } else {
                        if (GlobalDefine::ntRule == level) {
                            // Դ����Ϊ�������ݲ㣬������ճ���ӱ�����
                            result = (GlobalDefine::dtFrame != subType);
                        } else {
                            // Ŀ��㼶������ճ��Դ����
                            result = false;
                        }
                    }
                } else {
                    // �������ݲ�
                    if (GlobalDefine::dtComplex == subType_) {
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
void DataEditWidget::deleteRule(int pos)
{
    TableNode::smtTable table = SMT_CONVERT(TableNode, data_);
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

void DataEditWidget::insertRule(const ICDMetaData::smtMeta &meta, int pos)
{
    if (!meta) {
        return;
    }
    TableNode::smtTable table = std::dynamic_pointer_cast<TableNode>(data_);
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
void DataEditWidget::movePlane(int from, int to)
{
    const int count = vData_.size();
    if (from < 0 || to >= count) {
        return;
    }
    ICDElement::smtElement element = vData_[from];
    if (from < to) {    // ��ǰ�����
        for (int i = from; i < to; ++i) {
            vData_[i] = vData_[i + 1];
        }
    } else {    // �Ӻ���ǰ��
        for (int i = from; i > to; --i) {
            vData_[i] = vData_[i - 1];
        }
    }
    vData_[to] = element;
    std::vector<PlaneNode::smtPlane> planes;
    for (int i = 0; i < count; ++i) {
        planes.push_back(SMT_CONVERT(PlaneNode, vData_[i])->clone());
    }
    showData(planes);
    tableView_->selectRow(to);
}

// ���ϵͳλ��
void DataEditWidget::moveSystem(int from, int to)
{
    PlaneNode::smtPlane plane = SMT_CONVERT(PlaneNode, data_);
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
    tableView_->selectRow(to);
}

// �����λ��
void DataEditWidget::moveTable(int from, int to)
{
    SystemNode::smtSystem system = SMT_CONVERT(SystemNode, data_);
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
    tableView_->selectRow(to);
}

// �������λ��
void DataEditWidget::moveRule(int from, int to)
{
    TableNode::smtTable table = SMT_CONVERT(TableNode, data_);
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
    tableView_->selectRow(to - 1);
}

// ����ӱ�λ��
void DataEditWidget::moveSubTable(int from, int to)
{
    ICDComplexData::smtComplex complex = SMT_CONVERT(ICDComplexData, data_);
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
    tableView_->selectRow(to);
}

void DataEditWidget::saveEditData(void *data)
{
    if (GlobalDefine::ntUnknown == dataType_) {
        PlaneNode::smtPlane plane;
        const stPlane &_plane = *reinterpret_cast<stPlane *>(data);
        const int count = vData_.size();
        for (int i = 0; i < count; ++i) {
            if (atoi(vData_[i]->id().c_str()) == _plane.nCode) {
                plane = std::dynamic_pointer_cast<PlaneNode>(vData_[i]);
                plane->setPlane(_plane);
                break;
            }
        }
    } else if (GlobalDefine::ntVehicle == dataType_) {
        const stSystem &_system = *reinterpret_cast<stSystem *>(data);
        PlaneNode::smtPlane plane = std::dynamic_pointer_cast<PlaneNode>(data_);
        SystemNode::smtSystem system = plane->system(_system.nCode);
        if (system) {
            system->setSystem(_system);
        }
    } else if (GlobalDefine::ntSystem == dataType_) {
        const stICDBase &_icdBase = *reinterpret_cast<stICDBase *>(data);
        SystemNode::smtSystem system = std::dynamic_pointer_cast<SystemNode>(data_);
        TableNode::smtTable table = system->table(_icdBase.sID);
        if (table) {
            table->setICDBase(_icdBase);
        }
    } else if (GlobalDefine::ntTable == dataType_) {
        ICDMetaData::smtMeta meta = *reinterpret_cast<ICDMetaData::smtMeta *>(data);
        TableNode::smtTable table = SMT_CONVERT(TableNode, data_);
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
    } else if (GlobalDefine::ntRule == dataType_) {
        if (GlobalDefine::dtComplex == subType_) {
            ICDMetaData::smtMeta meta = *reinterpret_cast<ICDMetaData::smtMeta *>(data);
            TableNode::smtTable table = SMT_CONVERT(TableNode, data_);
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
        } else if (GlobalDefine::dtFrame == subType_) {
            const stICDBase &_icdBase = *reinterpret_cast<stICDBase *>(data);
            ICDComplexData::smtComplex complex = std::dynamic_pointer_cast<ICDComplexData>(data_);
            TableNode::smtTable table = complex->table(_icdBase.sName);
            table->setICDBase(_icdBase);
        }
    }
}

// ���¼�����������ֽ����
void DataEditWidget::reorderTable(TableNode::smtTable &table, int from)
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
TableNode::smtTable DataEditWidget::parentTable() const
{
    TableNode::smtTable table = 0;
    // �ֱ�����Ľڵ�͹���ڵ����࣬������Ľڵ㣬��ѯq_data��������
    if (GlobalDefine::ntRule == dataType_
            && !(GlobalDefine::dtComplex == subType_
                 || GlobalDefine::dtFrame == subType_)) {
        // �Ǹ���/֡�������ݣ���ѯ����
        QString keys;
        ICDElement::smtElement element;
        QVariantList args;
        args.append(qVariantFromValue((void*)&keys));
        jnotify->send("edit.queryNodeKeys", args);

        // �ؼ��뼯�а����õ�ǰ��������id����ѯ�����轫��ɾ��
        keys.truncate(keys.lastIndexOf("/"));
        args.clear();
        args.append(qVariantFromValue((void*)&element));
        args.append(qVariantFromValue((void*)&keys));
        jnotify->send("edit.querySingleElement", args);

        table = SMT_CONVERT(TableNode, element);
    } else {
        // ����ڵ㣬�ڿ��������в�ѯ����
        table = SMT_CONVERT(TableNode, data_);
    }

    return table;
}

// ���±༭���ݽ���
void DataEditWidget::updateLoggingUI(GlobalDefine::OptionType option)
{
    if (GlobalDefine::ntUnknown == dataType_) {
        updatePlaneUI(option);
    } else if (GlobalDefine::ntVehicle == dataType_) {
        updateSystemUI(option);
    } else if (GlobalDefine::ntSystem == dataType_) {
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
                    loggingWidget_->setVisible(false);
                }
            }
        }
    } else if (GlobalDefine::ntTable == dataType_) {
        updateICDRuleUI(option);
    } else if (GlobalDefine::ntRule == dataType_) {
        if (GlobalDefine::dtComplex == subType_) {
            updateICDRuleUI(option);
        } else if (GlobalDefine::dtFrame == subType_) {
            updateSubTableUI(option);
        }
    }
    // �����Ƿ���λ�ñ䶯���ж��Ƿ����ñ��水ť
    if (changedPos >= 0) {
        loggingWidget_->enableCommit(true);
    }
}

// ��ǰѡ������
void DataEditWidget::slotCurrentItemChanged(QStandardItem *current,
                                            QStandardItem *previous)
{
    if (!current) {
        // ��������¼�봰��
        loggingWidget_->setVisible(false);
        setActionEnabled("up", false);
        setActionEnabled("down", false);
        setActionEnabled("remove", false);
    } else {
        if (previous && current->row() == previous->row()) {
            // δ���ѡ����
            return;
        } else {
            if (newIndex_ < 0 || newIndex_ != current->row()) {
                updateLoggingUI(GlobalDefine::optEdit);
            } else {     // �������ݲ�δ�����û����л�ѡ�б�����������
                updateLoggingUI(GlobalDefine::optNew);
            }
        }
        int count = tableView_->rowCount();
        // ���˾������ڵ㣬�����ڵ��������λ��
        if (count > 1 && GlobalDefine::ntRule != dataType_
                || GlobalDefine::dtFrame == subType_
                || GlobalDefine::dtComplex == subType_) {
            setActionEnabled("up", 0 != current->row());
            setActionEnabled("down", count - 1 != current->row());
            // �������˳���δ���棬������ɾ��
            setActionEnabled("remove", changedPos < 0 ? true :false);
        } else if (1 == count) {
            setActionEnabled("remove", true);
        }
    }
    // �л��ڵ�󣬸����б����Ƿ����������ݣ�����������ť�Ƿ����
    if (-1 != newIndex_) {
        setActionEnabled("add", false);
        setActionEnabled("insert", false);
        setActionEnabled("up", false);
        setActionEnabled("down", false);
    } else {
        // �������λ�ú�δ�������ݣ�����������������
        if (changedPos >= 0) {
            setActionEnabled("add", false);
            setActionEnabled("insert", false);
        }
    }
}

// �����������
void DataEditWidget::slotItemPressed(QStandardItem *item)
{
    Q_UNUSED(item);
    if (Qt::RightButton == QApplication::mouseButtons()) {
        // ���������ʱ������ʾ�Ҽ��˵�
        if (objectName_ != tableView_->tableName()) {
            showMenu();
        }
    }
}

// ���Զ���˵�
void DataEditWidget::slotCustomContextMenu(const QPoint &pos)
{
    // ���������ʱ������ʾ�Ҽ��˵�
    if (objectName_ != tableView_->tableName()) {
        QMenu menu(tableView_);
        int row = tableView_->currentRow();
        if (row > -1 && newIndex_ != row) {
            menu.addAction(QStringLiteral("����"), this, SLOT(slotCopy()));
        }
        QAction *act = menu.addAction(QStringLiteral("ճ��"), this,
                                      SLOT(slotPaste()));
        // ����Դ���ݺ�Ŀ�����ݲ�Σ�����ճ��ѡ�����״̬
        act->setEnabled(canPasted());
        menu.exec(tableView_->viewport()->mapToGlobal(pos));
    }
}

// �����ݱ��
void DataEditWidget::slotContentChanged()
{
    // ���ڹ��������ʱ��ʾ��ʾ
    setTipsVisible(ruleName_ == tableView_->tableName());
}

void DataEditWidget::slotDataPosChanged(int logical,
                                        int oldIndex,
                                        int newIndex)
{
    Q_UNUSED(logical);
    if (changedPos < 0) {
        changedPos = qMin(oldIndex, newIndex);
    } else if (changedPos > 0) {
        changedPos = qMin(changedPos, qMin(oldIndex, newIndex));
    }
    if (GlobalDefine::ntUnknown == dataType_) {
        movePlane(oldIndex, newIndex);
    } else if (GlobalDefine::ntVehicle == dataType_) {
        moveSystem(oldIndex, newIndex);
    } else if (GlobalDefine::ntSystem == dataType_) {
        moveTable(oldIndex, newIndex);
    } else if (GlobalDefine::ntTable == dataType_) {
        // ������Ŵ�1��ʼ
        moveRule(oldIndex + 1, newIndex + 1);
    } else if (GlobalDefine::ntRule == dataType_) {
        if (GlobalDefine::dtComplex == subType_) {
            // ������Ŵ�1��ʼ
            moveRule(oldIndex + 1, newIndex + 1);
        } else if (GlobalDefine::dtFrame == subType_) {
            moveSubTable(oldIndex, newIndex);
        }
    }
    loggingWidget_->enableCommit(true);
}

// ��������
void DataEditWidget::slotSave2Memory(void *data, bool &result)
{
    if (!data) {
        return;
    }

    // �������ݵ��ڴ棬�ֿ��������롢�����ͱ༭�����������롢����ֱ�ӱ��浽�ڴ�
    // �༭�����漰�����ݵ�λ�ø����������ȱ��浽�������ݣ���һ���Ը����ڴ�����
    if (tableView_->property("copyData").toInt() == newIndex_
            && -1 != newIndex_) {  // ���濽������
        QVector<int> params;
        ICDElement::smtElement element;
        QString keys = tableView_->property("source").toString();
        keys = keys.mid(keys.indexOf("##") + strlen("##"));
        params << (int)&keys << (int)data;

        QVariantList args;
        args.append(qVariantFromValue((void*)&params));
        args.append(qVariantFromValue((void*)&element));
        jnotify->send("edit.savePastedData", args);

        result = true;
    } else if (-1 != newIndex_
               && (tableView_->rowCount() - 1) != newIndex_
               && tableView_->currentRow() == newIndex_) {   // ��������
        std::vector<int> params;
        params.push_back(tableView_->currentRow());
        params.push_back((int)data);

        QVariantList args;
        args.append(qVariantFromValue((void*)&params));
        args.append(qVariantFromValue((void*)&result));
        jnotify->send("edit.insertNodeData", args);

    } else {    // �������������ݵ����һ������������/�༭
        if (-1 != changedPos) {    // �ı����������λ��
            // �������ݵ�����
            saveEditData(data);
            std::vector<int> params;
            if (GlobalDefine::ntUnknown == dataType_) {
                params.push_back((int)&vData_);
            } else {
                params.push_back((int)&data_);
            }
            params.push_back(changedPos);

            QVariantList args;
            args.append(qVariantFromValue((void*)&params));
            args.append(qVariantFromValue((void*)&result));
            jnotify->send("edit.reorderNodeData", args);
            // ���浽�ڴ�
        } else { // �������߽��༭������
            QVariantList args;
            args.append(qVariantFromValue((void*)data));
            args.append(qVariantFromValue((void*)&result));
            jnotify->send("edit.updateNodeData", args);
        }
    }

    if (result) {
        // ���³�ʼ������
        int row = tableView_->currentRow();
        reInit();
        tableView_->selectRow(row);
    } else {
        setActionEnabled("add", result);
        setActionEnabled("insert", result);
    }
}

void DataEditWidget::slotCanceled()
{
    if (-1 != changedPos) {
        return;  // ֻ���϶�����λ��ʱ�����³�ʼ��
    }

    // ���³�ʼ������
    int row = tableView_->currentRow();
    reInit();
    tableView_->selectRow(row);

    //q_loggingWidget->setVisible(false);
}

// ����
void DataEditWidget::slotNew(const QVariant &param)
{
    if (GlobalDefine::ntUnknown == dataType_) {   // ����
        newPlane();
    } else if (GlobalDefine::ntVehicle == dataType_) {  // ϵͳ
        newSystem();
    } else if (GlobalDefine::ntSystem == dataType_) { // ��
        newICDTable();
    } else if (GlobalDefine::ntTable == dataType_) {
        newICDRule(param.toInt());
    } else if (GlobalDefine::ntRule == dataType_) {
        if (GlobalDefine::dtComplex == subType_) {
            newICDRule(param.toInt());
        } else if (GlobalDefine::dtFrame == subType_) {
            newSubTable();
        }
    }
    // ����������Ժ󣬱��뱣������֮����ܼ����ƶ�
    if (-1 != newIndex_) {
        tableView_->verticalHeader()->setSectionsMovable(false);
    }
}

// ����
void DataEditWidget::slotInsert()
{
    if (GlobalDefine::ntTable == dataType_
            || (GlobalDefine::ntRule == dataType_
                && GlobalDefine::dtComplex == subType_)) {
        if (!tableView_) {
            return;
        }
        stTableRules rule;
        int index = tableView_->currentRow();
        if (index < 0) {    // δѡ�����ݣ����뵽ĩβ������������
            newICDRule(GlobalDefine::dtNumeric);
        } else {
            newIndex_ = index;
            rule.nSerial = tableView_->itemData(index, 0, Qt::UserRole).toInt();
            rule.nCode = tableView_->itemValue(index - 1, 2).toInt();
            rule.nCode += tableView_->itemData(index - 1, tableView_->columnCount() - 1,
                                               Qt::UserRole).toInt();
            rule.sName = QString("item_%1").arg(index).toStdString();
            rule.uType = GlobalDefine::dtNumeric;
            // subType
            switch (rule.uType) {
            case GlobalDefine::dtNumeric: rule.subType = GlobalDefine::NumericU8; break;
            default:
                break;
            }
            //
            rule.sPrgCode = QString("code_%1").arg(index).toStdString();
            ICDMetaData::smtMeta smtData
                    = ICDFactory::instance().CreatObject(rule);
            // ���±�
            updateOne(newIndex_, smtData, optNew);
            tableView_->selectRow(newIndex_);
        }
        // ����������Ժ󣬱��뱣������֮����ܼ����ƶ�
        if (-1 != newIndex_) {
            tableView_->verticalHeader()->setSectionsMovable(false);
        }
    }
}

// ����
void DataEditWidget::slotMoveUp()
{
    int row = tableView_->currentRow();
    if (changedPos < 0) {
        changedPos = row - 1;
    } else if (changedPos > 0) {
        changedPos = qMin(changedPos, row - 1);
    }
    if (GlobalDefine::ntUnknown == dataType_) {
        movePlane(row, row - 1);
    } else if (GlobalDefine::ntVehicle == dataType_) {
        moveSystem(row, row - 1);
    } else if (GlobalDefine::ntSystem == dataType_) {
        moveTable(row, row - 1);
    } else if (GlobalDefine::ntTable == dataType_) {
        // ������Ŵ�1��ʼ
        moveRule(row + 1, row);
    } else if (GlobalDefine::ntRule == dataType_) {
        if (GlobalDefine::dtComplex == subType_) {
            // ������Ŵ�1��ʼ
            moveRule(row + 1, row);
        } else if (GlobalDefine::dtFrame == subType_) {
            moveTable(row, row - 1);
        }
    }
    loggingWidget_->enableCommit(true);
}

// ����
void DataEditWidget::slotMoveDown()
{
    int row = tableView_->currentRow();
    if (changedPos < 0) {
        changedPos = row;
    } else if (changedPos > 0) {
        changedPos = qMin(changedPos, row);
    }
    if (GlobalDefine::ntUnknown == dataType_) {
        movePlane(row, row + 1);
    } else if (GlobalDefine::ntVehicle == dataType_) {
        moveSystem(row, row + 1);
    } else if (GlobalDefine::ntSystem == dataType_) {
        moveTable(row, row + 1);
    } else if (GlobalDefine::ntTable == dataType_) {
        // ������Ŵ�1��ʼ
        moveRule(row + 1, row + 2);
    } else if (GlobalDefine::ntRule == dataType_) {
        if (GlobalDefine::dtComplex == subType_) {
            // ������Ŵ�1��ʼ
            moveRule(row + 1, row + 2);
        } else if (GlobalDefine::dtFrame == subType_) {
            moveTable(row, row + 1);
        }
    }
    loggingWidget_->enableCommit(true);
}

// ɾ��
void DataEditWidget::slotDelete()
{
    bool result = false;
    bool showTip = false;
    if (GlobalDefine::ntUnknown == dataType_) {
        if (QMessageBox::No == QMessageBox::question(this,
                                                     QStringLiteral("ȷ����ʾ"),
                                                     QStringLiteral("����ɾ�����ͼ�������������\r\nȷ��ɾ����"),
                                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
            return;
        }
    } else if (GlobalDefine::ntVehicle == dataType_) {
        if (QMessageBox::No == QMessageBox::question(this,
                                                     QStringLiteral("ȷ����ʾ"),
                                                     QStringLiteral("����ɾ����ϵͳ��������������\r\nȷ��ɾ����"),
                                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
            return;
        }
    } else if (GlobalDefine::ntSystem == dataType_) {
        if (QMessageBox::No == QMessageBox::question(this,
                                                     QStringLiteral("ȷ����ʾ"),
                                                     QStringLiteral("����ɾ��ICD��������������")
                                                     + QStringLiteral("\r\nȷ��ɾ����ICD�����ݣ�"),
                                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
            return;
        }
    } else if (GlobalDefine::ntTable == dataType_
               || GlobalDefine::ntRule == dataType_) {
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
    tableView_->setProperty("copyData", -1);   // ���ÿ�����������
    // ɾ��
    int current = tableView_->currentRow();
    if (current == newIndex_ && -1 != newIndex_) {    // ɾ����δ���������
        newIndex_ = -1;
        tableView_->removeRow(current);
        setActionEnabled("add", true);
        setActionEnabled("insert", dataType_ > GlobalDefine::ntSystem);
        tableView_->verticalHeader()->setSectionsMovable(true);
    } else {    // ɾ���ڴ�����
        QVariant data = tableView_->itemData(current, 0, Qt::UserRole);
        QVariant subTable = tableView_->itemData(current, 0, ComplexTable);
        QString key = data.toString();
        // ���ϱ���Ҫ���ӱ�������id���뵼����ͬ��
        if (subTable.isValid()) {
            key.append("/").append(subTable.toString());
        }
        std::vector<QString> keys(1, key);
        QVariantList args;
        args.append(qVariantFromValue((void*)&keys));
        args.append(qVariantFromValue((void*)&result));
        jnotify->send("edit.deleteNode", args);
        if (result) {
            if (tableView_->currentRow() == newIndex_) {
                newIndex_ = -1;
            }
            tableView_->removeRow(tableView_->currentRow());
            setTipsVisible(showTip);
            // ���³�ʼ��
            int row = tableView_->currentRow();
            reInit();
            tableView_->selectRow(row);
        }
    }
}

// ���
void DataEditWidget::slotClear()
{
    bool result = false;
    if (GlobalDefine::ntUnknown == dataType_) {
        if (QMessageBox::No == QMessageBox::question(this,
                                                     QStringLiteral("ȷ����ʾ"),
                                                     QStringLiteral("����ɾ�����л��ͣ�����������������")
                                                     + QStringLiteral("\r\nȷ��ɾ����"))) {
            return;
        }
    } else if (GlobalDefine::ntVehicle == dataType_) {
        if (QMessageBox::No == QMessageBox::question(this,
                                                     QStringLiteral("ȷ����ʾ"),
                                                     QStringLiteral("����ɾ����ǰ���͵�����ϵͳ������������������")
                                                     + QStringLiteral("\r\nȷ��ɾ����"))) {
            return;
        }
    } else if (GlobalDefine::ntSystem == dataType_) {
        if (QMessageBox::No == QMessageBox::question(this,
                                                     QStringLiteral("ȷ����ʾ"),
                                                     QStringLiteral("����ɾ����ǰϵͳ������Э�������������������")
                                                     + QStringLiteral("\r\nȷ��ɾ����"))) {
            return;
        }
    } else if (GlobalDefine::ntTable == dataType_
               || GlobalDefine::ntRule == dataType_) {
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

    const int count = tableView_->rowCount();
    for (int i = 0; i < count; ++i) {
        QVariant data = tableView_->itemData(i, 0, Qt::UserRole);
        keys.push_back(data.toString());
    }
    QVariantList args;
    args.append(qVariantFromValue((void*)&keys));
    args.append(qVariantFromValue((void*)&result));
    jnotify->send("edit.deleteNode", args);

    if (result) {
        reInit();
    }
}

// ����
void DataEditWidget::slotSave2Source(GlobalDefine::DataSource type)
{
    int loaded = 0;
    // ��ѯ�Ƿ���δ��������
    QVariantList args;
    args.append(qVariantFromValue((void*)&loaded));
    QString command("loadedState");
    args.append(qVariantFromValue((void*)&command));
    jnotify->send("edit.queryNodeFlag", args);

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
    QString actionName;
    QVector<int> params;
    if (GlobalDefine::dsDatabase == type) {   // �������ݿ�
        actionName = "db";
        setActionEnabled(actionName, false);
        params << (int)&err << (int)&defaultPath_;
        args.clear();
        args.append(qVariantFromValue((void*)&params));
        args.append(int(GlobalDefine::dsDatabase));
        jnotify->send("edit.saveMemoryData", args);

        tip = QStringLiteral("�������ݵ����ݿ�");
    } else if (GlobalDefine::dsFile == type) {  // �����ļ�
        actionName = "file";
        setActionEnabled(actionName, false);
        QString file = QFileDialog::getSaveFileName(this, QStringLiteral("����Э������"), defaultPath_,
                                                    "JSON files (*.json);;"
                                                    "XML files (*.xml)");
        if (file.isEmpty()) {
            setActionEnabled(actionName, true);
            return;
        }
        defaultPath_ = file;
        params << (int)&err << (int)&defaultPath_;
        args.clear();
        args.append(qVariantFromValue((void*)&params));
        args.append(int(GlobalDefine::dsFile));
        jnotify->send("edit.saveMemoryData", args);

        tip = QStringLiteral("�������ݵ��ļ�[%1]").arg(defaultPath_);
    }

    if (!err.isEmpty()) {
        tip.append(QStringLiteral("ʧ�ܣ�\n[%1]").arg(err));
        setActionEnabled(actionName, true);
    } else {
        tip.append(QStringLiteral("�ɹ���"));
        if ("file" == actionName) {
            setActionEnabled(actionName, true);
        }
    }

    QMessageBox::information(this, QStringLiteral("��������"), tip);
}

// ����
void DataEditWidget::slotCopy()
{
    QString keys;
    QString id = tableView_->itemData(tableView_->currentRow(),
                                      0, Qt::UserRole).toString();
    QVariantList args;
    args.append(qVariantFromValue((void*)&keys));
    jnotify->send("edit.queryNodeKeys", args);

    // ��Դ������Ϣ�洢����[���ݲ㼶-�����Ͳ㼶##id]
    if (keys.isEmpty()) {
        keys = id;
    } else {
        keys.append("/").append(id);
    }
    keys.prepend(QString("%1_%2##").arg(dataType_).arg(subType_));
    tableView_->setProperty("source", keys);
}

// ճ��
void DataEditWidget::slotPaste()
{
    QString keys = tableView_->property("source").toString();
    keys = keys.mid(keys.indexOf("##") + strlen("##"));
    if (keys.isEmpty()) {
        return;
    }
    // ��ѯԴ����
    ICDElement::smtElement element;
    QVariantList args;
    args.append(qVariantFromValue((void*)&element));
    args.append(qVariantFromValue((void*)&keys));
    jnotify->send("edit.querySingleElement", args);
    if (!element) {
        QMessageBox::information(this, QStringLiteral("ճ��"),
                                 QStringLiteral("ճ��ʧ�ܣ�������Դ��Ч��"));
        return;
    }
    if (GlobalDefine::ntUnknown == dataType_) {   // ���ڵ�
        PlaneNode::smtPlane plane = SMT_CONVERT(PlaneNode, element);
        // ���»���
        if (plane) {
            newIndex_ = tableView_->rowCount();
            updateOne(newIndex_, plane, optCopy);
            tableView_->selectRow(newIndex_);
        }
    } else if (GlobalDefine::ntVehicle == dataType_) {   // ����
        SystemNode::smtSystem system = SMT_CONVERT(SystemNode, element);
        // ����ϵͳ
        if (system) {
            newIndex_ = tableView_->rowCount();
            updateOne(newIndex_, system, optCopy);
            tableView_->selectRow(newIndex_);
        }
    } else if (GlobalDefine::ntSystem == dataType_) {  // ϵͳ
        // ���±�
        newIndex_ = tableView_->rowCount();
        updateOne(newIndex_, SMT_CONVERT(TableNode, element), optCopy);
        tableView_->selectRow(newIndex_);
    } else if (GlobalDefine::ntTable == dataType_) {   // ��
        // ���¹���
        newIndex_ = tableView_->rowCount();
        updateOne(newIndex_, SMT_CONVERT(ICDMetaData, element), optCopy);
        tableView_->selectRow(newIndex_);
        // ����δ��������ݳ�����Ϊ0��ʹ����У����ȷ��ʾ
        tableView_->setItemData(newIndex_, tableView_->columnCount() - 1,
                                0, Qt::UserRole);
    } else if (GlobalDefine::ntRule == dataType_) {    // ����
        if (GlobalDefine::dtComplex == subType_) {
            // ���±�
            newIndex_ = tableView_->rowCount();
            updateOne(newIndex_,
                      SMT_CONVERT(ICDMetaData, element),
                      optCopy);
            tableView_->selectRow(newIndex_);
            // ����δ��������ݳ�����Ϊ0��ʹ����У����ȷ��ʾ
            tableView_->setItemData(newIndex_, tableView_->columnCount() - 1,
                                    0, Qt::UserRole);
        } else if (GlobalDefine::dtFrame == subType_) {
            int maxCount = 0;
            args.clear();
            args.append(qVariantFromValue((void*)&maxCount));
            jnotify->send("edit.queryMaxTableCount", args);
            if (tableView_->rowCount() >= maxCount) {

                QMessageBox::information(this, QStringLiteral("ճ��"),
                                         QStringLiteral("�ӱ��Ѵ����ޣ��޷�ճ���������������ޣ�����ϵ����Ա��"));
                return;
            }
            TableNode::smtTable table = SMT_CONVERT(TableNode, element);
            if (table) {
                // ���±�
                newIndex_ = tableView_->rowCount();
                updateSubOne(newIndex_, table->icdBase(), optCopy);
                tableView_->selectRow(newIndex_);
            }
        }
    }
    // ����������Ժ󣬱��뱣������֮����ܼ����ƶ�
    if (-1 != newIndex_) {
        tableView_->verticalHeader()->setSectionsMovable(false);
    }
}

void DataEditWidget::setActionEnabled(const QString &name, bool enabled)
{
    QVariantList args;
    args.append(name);
    args.append(enabled);
    jnotify->send("edit.toolbar.action.enabled", args);
}
