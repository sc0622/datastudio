#include "precomp.h"
#include "icdnavigationui.h"
#include "jwt/jtreeview.h"
#include "KernelClass/icdcomplexdata.h"
#include "KernelClass/icdbitdata.h"
#include "icdwidget/icd_searchedit.h"

ICDNavigationUi::ICDNavigationUi(QWidget *parent)
    : QWidget(parent)
    , q_ruleMenu(0)
{
    QVBoxLayout* verMainLayout = new QVBoxLayout(this);
    verMainLayout->setContentsMargins(0, 0, 0, 0);
    verMainLayout->setSpacing(0);

    searchEdit_ = new Icd::SearchEdit(this);
    verMainLayout->addWidget(searchEdit_);

    q_treeView = new JTreeView(this);
    q_treeView->setAnimated(true);
    q_treeView->header()->hide();
    q_treeView->header()->setDefaultAlignment(Qt::AlignCenter);
    q_treeView->setHeaderLabel(QStringLiteral("协议"));
    q_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    q_treeView->setAcceptDrops(true);
    //q_treeView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    QStandardItem *root = new QStandardItem();
    root->setText(QStringLiteral("协议"));
    root->setData(GlobalDefine::ntUnknown, LevelIndex);
    root->setData(GlobalDefine::dsNone, UserKey);
    root->setData(GlobalDefine::noneState, DataChanged);
    root->setData(GlobalDefine::dsNone, DataSource);
    q_treeView->addTopLevelItem(root);
    q_treeView->setItemDelegateForColumn(0, new JTreeItemDelegate(q_treeView));
    //q_treeView->setFilterModel(new JTreeSortFilterModel(q_treeView));

    verMainLayout->addWidget(q_treeView);

    //
    //JTreeSortFilterModel *filterModel = new JTreeSortFilterModel(this);
    //q_treeView->setFilterModel(filterModel);

    //
    //connect(searchEdit_, &Icd::SearchEdit::textChanged, this, [=](const QString &text){
    //    filterModel->setFilterPattern(text);
    //});
    connect(q_treeView, SIGNAL(itemPressed(QStandardItem *)),
            this, SLOT(slotItemPressed(QStandardItem *)));
    connect(q_treeView,
            SIGNAL(currentItemChanged(QStandardItem *, QStandardItem *)),
            this,
            SLOT(slotCurrentChanged(QStandardItem *, QStandardItem *)));
    connect(q_treeView, &JTreeView::expanded, this, [=](){
        q_treeView->resizeColumnToContents(0);
    });
    connect(q_treeView, &JTreeView::collapsed, this, [=](){
        q_treeView->resizeColumnToContents(0);
    });

    jnotify->on("edit.tree.loadVehicle", this, [=](JNEvent &){
        loadVehicle();
    });
}

ICDNavigationUi::~ICDNavigationUi()
{

}

bool ICDNavigationUi::init()
{
    return loadVehicle();
}

// 点击树节点
void ICDNavigationUi::slotItemPressed(QStandardItem *item)
{
    Qt::MouseButtons buttons = QApplication::mouseButtons();
    switch (buttons) {
    case Qt::LeftButton:
    {
        if (item->hasChildren()) {
            break;
        }

        const QVariant level = item->data(LevelIndex);
        if (!level.isValid()) {
            break;
        }

        // 查询数据源
        int dataSource = GlobalDefine::dsNone;
        QVariantList args;
        args.append(qVariantFromValue((void*)&dataSource));
        jnotify->send("edit.queryDataSource", args);

        switch (level.toInt()) {
        case GlobalDefine::ntUnknown:   // 根节点
            if (GlobalDefine::dsDatabase == dataSource) {
                loadData("database");
            } else if (GlobalDefine::dsFile == dataSource) {
                loadData("file");
            }
            break;
        case GlobalDefine::ntPlane:     // 机型
        case GlobalDefine::ntSystem:    // 分系统
        {
            if (hasRuleTable(item)) {
                int state = dataLoaded(item);
                if (GlobalDefine::wholeState != state) {
                    slotLoadRule();
                }
            }
            break;
        }
        case GlobalDefine::ntTable:     // 规则表
        {
            int state = dataLoaded(item);
            if (GlobalDefine::wholeState != state) {
                slotLoadRule();
            }
            break;
        }
        default:
            break;
        }

        break;
    }
    case Qt::RightButton:
    {
        showMenu(item);
        break;
    }
    default:
        break;
    }
}

// 切换选中节点
void ICDNavigationUi::slotCurrentChanged(QStandardItem* current,
                                         QStandardItem* previous)
{
    if (current == previous) {
        // 同一节点
        return;
    }
    if (current && previous) {
        if (current->row() == previous->row()
                && current->parent() == previous->parent()) {
            // 同一行的不同节点
            return;
        }
    }
    updateMainView(current);
}

// 加载规则表数据
void ICDNavigationUi::slotLoadRule()
{
    // 数据变更未保存，二次提示
    if (queryNodeFlag("nodeState") != GlobalDefine::noneState) {
        if (QMessageBox::question(this,
                                  QStringLiteral("加载"),
                                  QStringLiteral("有数据尚未保存，")
                                  + QStringLiteral("继续操作将丢失这部分数据！是否继续？"))
                == QMessageBox::No) {
            return;
        }
    }

    QStandardItem* current = q_treeView->currentItem();
    if (!current) {
        return;
    }
    std::vector<int> params;
    QString condition = queryNodeKeys(current);
    PlaneNode::smtPlane plane;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    // 加载规则数据
    params.push_back((int)&condition);
    params.push_back(1);
    QVariantList args;
    args.append(qVariantFromValue((void*)&plane));
    args.append(qVariantFromValue((void*)&params));
    jnotify->send("edit.loadRules", args);

    QApplication::restoreOverrideCursor();
    // 更新导航树
    updateRuleItems(plane);
    // 更新数据变更状态
    updateDataState(current, StateReset);
    // 更新主视图表
    updateMainView(current);
}

// 卸载规则表数据
void ICDNavigationUi::slotUnloadRule()
{
    // 数据变更未保存，二次提示
    if (queryNodeFlag("nodeState") != GlobalDefine::noneState) {
        if (QMessageBox::question(this,
                                  QStringLiteral("卸载"),
                                  QStringLiteral("有数据尚未保存，")
                                  + QStringLiteral("继续操作将丢失这部分数据！是否继续？"))
                == QMessageBox::No) {
            return;
        }
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QStandardItem *current = q_treeView->currentItem();
    QString condition = queryNodeKeys(current);
    QVariantList args;
    args.append(qVariantFromValue((void*)&condition));
    jnotify->send("edit.unLoadRules", args);

    QApplication::restoreOverrideCursor();
    // 更新树节点
    updateLoadedFlag(current, optUnload);
    // 更新数据状态标识
    updateDataState(current, StateReset);
    // 删除规则节点
    unLoadRuleNode(current);
    // 更新主视图表
    updateMainView(current);
}

// 更新规则表数据
void ICDNavigationUi::slotUpdate()
{
    QStandardItem* current = q_treeView->currentItem();
    if (!current) {
        return;
    }
    // 数据变更未保存，二次提示
    if (queryNodeFlag("nodeState") != GlobalDefine::noneState) {
        if (QMessageBox::question(this,
                                  QStringLiteral("更新"),
                                  QStringLiteral("有数据尚未保存，")
                                  + QStringLiteral("继续操作将丢失这部分数据！是否继续？"))
                == QMessageBox::No) {
            return;
        }
    }

    std::vector<int> params;
    QString condition = queryNodeKeys(current);
    PlaneNode::smtPlane plane;
    // 加载规则数据
    params.push_back((int)&condition);
    params.push_back(0);
    QVariantList args;
    args.append(qVariantFromValue((void*)&plane));
    args.append(qVariantFromValue((void*)&params));
    jnotify->send("edit.loadRules", args);

    // 更新数据状态标识
    updateDataState(current, StateReset);
    // 更新导航树
    updateRuleItems(plane);
    // 更新主视图表
    updateMainView(current);
}

// 新增数据
void ICDNavigationUi::slotNew()
{
    QAction *act = qobject_cast<QAction *>(sender());
    QVariantList args;
    args.append(int(GlobalDefine::optNew));
    QVariant data = act->data();
    args.append(qVariantFromValue((void*)&data));
    jnotify->send("edit.dealNavigationCommand", args);
}

// 清除表数据
void ICDNavigationUi::slotClear()
{
    QVariantList args;
    args.append(int(GlobalDefine::optClear));
    QVariant data;
    args.append(qVariantFromValue((void*)&data));
    jnotify->send("edit.dealNavigationCommand", args);
}

// 删除数据
void ICDNavigationUi::slotDelete()
{
    if (!q_treeView) {
        return;
    }
    QStandardItem *current = q_treeView->currentItem();
    if (!current) {
        return;
    }
    if (QMessageBox::question(this,
                              QStringLiteral("确认提示"),
                              QStringLiteral("即将删除当前选中数据及所有下属数据\r\n")
                              + QStringLiteral("确认删除该项数据？"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
            == QMessageBox::No) {
        return;
    }
    bool result = false;
    int level = current->data(LevelIndex).toInt();
    QString currentKey = current->data(UserKey).toString();
    // 先删除数据库
    if (GlobalDefine::ntPlane == level) {   // 机型
        std::vector<int> planeID(1, currentKey.toInt());
        QVariantList args;
        args.append(qVariantFromValue((void*)&planeID));
        args.append(qVariantFromValue((void*)&result));
        jnotify->send("edit.deletePlane", args);
    } else if (GlobalDefine::ntSystem == level) {   // 系统
        std::vector<void*> params;
        std::vector<int> systemID(1, currentKey.toInt());
        QString planeID = parentKey(GlobalDefine::ntPlane);

        params.push_back(&planeID);
        params.push_back(&systemID);

        QVariantList args;
        args.append(qVariantFromValue((void*)&params));
        args.append(qVariantFromValue((void*)&result));
        jnotify->send("edit.deleteSystem", args);
    } else if (GlobalDefine::ntTable == level) {    // 表
        std::vector<void*> params;
        QString keys = queryNodeKeys(current);
        std::vector<std::string> tableID(1, currentKey.toStdString());

        keys = keys.left(keys.lastIndexOf("/"));
        params.push_back(&keys);
        params.push_back(&tableID);

        QVariantList args;
        args.append(qVariantFromValue((void*)&params));
        args.append(qVariantFromValue((void*)&result));
        jnotify->send("edit.deleteTable", args);
    } else if (GlobalDefine::ntRule == level) {
        std::vector<void*> params;
        QString keys = queryNodeKeys(current);
        keys = keys.left(keys.lastIndexOf(currentKey) - strlen("/"));
        if (GlobalDefine::dtComplex == current->data(RuleDefine).toInt()
                && GlobalDefine::dtDiscern
                == current->parent()->data(RuleDefine).toInt()) {    // 帧数据
            std::vector<std::string> rules;
            // 查询子表
            rules.push_back(currentKey.split("/").last().toStdString());
            params.push_back(&keys);
            params.push_back(&rules);

            QVariantList args;
            args.append(qVariantFromValue((void*)&params));
            args.append(qVariantFromValue((void*)&result));
            jnotify->send("edit.deleteSubTable", args);
        } else {    // 其他规则数据
            std::vector<int> rules;
            // 查询子表
            rules.push_back(currentKey.split("/").first().toInt());
            params.push_back(&keys);
            params.push_back(&rules);

            QVariantList args;
            args.append(qVariantFromValue((void*)&params));
            args.append(qVariantFromValue((void*)&result));
            jnotify->send("edit.deleteRule", args);
        }
    }
    if (result) {
        QStandardItem *parentItem = current->parent();
        if (parentItem) {
            if (level < GlobalDefine::ntRule) {
                // 更新父节点状态
                parentItem->removeRow(current->row());
                updateParentFlag(parentItem);
            } else {
                // 重新加载树
                reLoadRuleNode(parentItem);
            }
            // 更新数据状态标识
            updateDataState(parentItem, StateChanged);
            // 重新刷新界面数据
            q_treeView->setCurrentItem(parentItem);
            if (parentItem->data(SubTable).isValid()) {
                // 更新规则提示
                updateTopTableTip();
            }
        }
    }

    // 再更新界面
}

void ICDNavigationUi::slotLoadData()
{
    if (!sender()) {
        Q_ASSERT(false);
        return;
    }

    loadData(sender()->objectName());
}

// 加载数据
void ICDNavigationUi::loadData(const QString &name)
{
    // 数据变更未保存，二次提示
    if (queryNodeFlag("nodeState") != GlobalDefine::noneState) {
        if (QMessageBox::question(this,
                                  QStringLiteral("加载"),
                                  QStringLiteral("有数据尚未保存，")
                                  + QStringLiteral("继续操作将丢失这部分数据！是否继续？"))
                == QMessageBox::No) {
            return;
        }
    }

    int dataSource = GlobalDefine::dsNone;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    // 加载数据
    QString error;
    if (name == "database") {
        dataSource = GlobalDefine::dsDatabase;

        QVariantList args;
        args.append(int(dataSource));
        args.append(qVariantFromValue((void*)&error));
        args.append(int(Icd::ObjectTable));
        jnotify->send("edit.loadInfrastructure", args);
    } else if (name == "file") {
        dataSource = GlobalDefine::dsFile;

        QVariantList args;
        args.append(int(dataSource));
        args.append(qVariantFromValue((void*)&error));
        args.append(int(Icd::ObjectTable));
        jnotify->send("edit.loadInfrastructure", args);
    }
    if (!error.isEmpty()) {
        switch (dataSource)
        {
        case GlobalDefine::dsDatabase:
            break;
        case GlobalDefine::dsFile:
            error.prepend(QStringLiteral("加载数据失败！"));
            break;
        default:
            error.prepend(QStringLiteral("加载数据失败！"));
            break;
        }
        QMessageBox::warning(this, QStringLiteral("加载数据"), error);
        // 加载失败，直接返回
        return;
    }

    // 初始化
    PlaneNode::planeVector data;

    QVariantList args;
    args.append(qVariantFromValue((void*)&data));
    jnotify->send("edit.queryAllPlane", args);

    loadData(data, dataSource);

    QApplication::restoreOverrideCursor();
}

bool ICDNavigationUi::loadVehicle()
{
    int dataSource = GlobalDefine::dsNone;
    QVariantList args;
    args.append(qVariantFromValue((void*)&dataSource));
    jnotify->send("edit.queryDataSource", args);

    switch (dataSource) {
    case GlobalDefine::dsDatabase: loadData("database"); break;
    case GlobalDefine::dsFile: loadData("file"); break;
    default: break;
    }

    return true;
}

// 加载显示机型系统结构数据
void ICDNavigationUi::loadData(const PlaneNode::planeVector &data, int source)
{
    if (NULL == q_treeView) {
        return;
    }
    QStandardItem *root = q_treeView->topLevelItem(0);
    if (NULL == root) {
        return;
    }
    // 清除历史数据
    if (root->hasChildren()) {
        root->removeRows(0, root->rowCount());
    }
    root->setData(source, DataSource);
    root->setData(GlobalDefine::noneState, DataChanged);
    root->setData(GlobalDefine::noneState, ItemLoaded);

    const int count = data.size();
    for (int i = 0; i < count; ++i) {
        if (!formPlaneNode(root, data[i])) {
            return;
        }
    }
    q_treeView->expandItem(root);
    if (q_treeView->currentItem() != root) {
        q_treeView->setCurrentItem(root);
    } else {
        slotCurrentChanged(root, NULL);
    }
}

// 更新树
void ICDNavigationUi::updateRuleItems(const PlaneNode::smtPlane& plane)
{
    if (!plane || !q_treeView) {
        return;
    }
    QStandardItem *current = q_treeView->currentItem();
    if (!current) {
        return;
    }
    QVariant level = current->data(LevelIndex);
    if (!level.isValid()) {
        return;
    }
    if (GlobalDefine::ntPlane == level.toInt()) { // 机型下所有规则表
        SystemNode::systemVector systems = plane->allSystem();
        const int count = systems.size();
        QStandardItem *item = NULL;
        for (int i = 0; i < count; ++i) {
            SystemNode::smtSystem system = systems[i];
            TableNode::tableVector tables = system->allTable();
            const int bound = tables.size();
            for (int j = 0; j < bound; ++j) {
                TableNode::smtTable table = tables[j];
                item = findItem(UserKey, table->key().c_str(), current, false);
                if (item) {
                    loadRuleNode(item, table);
                    q_treeView->expandItem(item);
                }
            }
            q_treeView->expandItem(findItem(UserKey, system->numeralId(), current, false));
        }
        q_treeView->expandItem(current);
        // 更新节点状态
        updateLoadedFlag(current, optLoad);
    } else if (GlobalDefine::ntSystem == level.toInt()) { // 系统下所有规则表
        QString systemID = parentKey(GlobalDefine::ntSystem);
        SystemNode::smtSystem system = plane->system(systemID.toInt());
        if (system) {
            QStandardItem *item = NULL;
            TableNode::smtTable table = 0;
            int count = current->rowCount();
            for (int i = 0; i < count;) {
                item = current->child(i);
                Q_ASSERT(0 != item);
                if (table = system->table(item->data(UserKey)
                                          .toString().toStdString())) {
                    loadRuleNode(item, table);
                    q_treeView->expandItem(item);
                    i++;
                } else {    // 不存在节点，删除
                    current->removeRow(i);
                    count--;
                }
            }
        }
        q_treeView->expandItem(current);
        // 更新节点状态
        updateLoadedFlag(current, optLoad);
    } else if (GlobalDefine::ntTable == level.toInt()) { // 单张规则表
        QString systemID = parentKey(GlobalDefine::ntSystem);
        QString tableID = parentKey(GlobalDefine::ntTable);
        TableNode::smtTable data = 0;
        SystemNode::smtSystem system = plane->system(systemID.toInt());
        if (system) {
            data = system->table(tableID.toStdString());
            loadRuleNode(current, data);
            q_treeView->expandItem(current);
            current->setIcon(QIcon(":/datastudio/image/global/circle-green.png"));
        }
        // 更新节点状态
        updateLoadedFlag(current, optLoad);
    }
}

// 查询节点数据
void ICDNavigationUi::queryNodeData(const QString &condition,
                                    ICDElement::smtElement &element)
{
    QString keys = queryCurrentKeys();
    if (keys.isEmpty()) {
        keys = condition;
    } else {
        if (!condition.isEmpty()) {
            if ("parentTable" == condition) {
                keys = keys.left(keys.lastIndexOf("/"));
            } else if ("topTable" == condition) {
            } else {
                keys.append("/").append(condition);
            }
        }
    }

    QVariantList args;
    args.append(qVariantFromValue((void*)&element));
    args.append(qVariantFromValue((void*)&keys));
    jnotify->send("edit.querySingleElement", args);
}

// 查询节点状态
int ICDNavigationUi::queryNodeFlag(const QString &type) const
{
    int result = 0;
    QStandardItem *item = q_treeView->currentItem();
    QString condition(type);
    if (condition.startsWith("child")) {
        item = findItem(UserKey, condition.split("@").last(), item);
        condition.truncate(condition.indexOf("@"));
    }
    if (!item) {
        return result;
    }
    QVariant var;
    if (condition == "dataState") {
        var = q_treeView->topLevelItem(0)->data(DataChanged);
        if (var.isValid()) {
            result = var.toInt();
        }
    } else if (condition == "nodeState") {
        var = item->data(DataChanged);
        if (var.isValid()) {
            result = var.toInt();
        }
    } else if (condition == "loadedState"
               || condition == "childloadedState") {
        result = GlobalDefine::wholeState;
        if (hasRuleTable(item)) {
            result = item->data(ItemLoaded).toInt();
        }
    }

    return result;
}

// 查询当前节点顶层表
void ICDNavigationUi::queryTopTable(ICDElement::smtElement &element)
{
    if (!q_treeView) {
        return;
    }
    QStandardItem *item = q_treeView->currentItem();
    if (!item) {
        return;
    }
    QString keys = queryNodeKeys(item);
    QStringList lst = keys.split("/", QString::SkipEmptyParts);
    if (lst.size() < 3) {
        return; // 当前节点层级在表一级之上
    }
    while (lst.size() > 3) {
        lst.removeLast();
    }
    keys = lst.join("/");

    QVariantList args;
    args.append(qVariantFromValue((void*)&element));
    args.append(qVariantFromValue((void*)&keys));
    jnotify->send("edit.querySingleElement", args);
}

// 查询当前节点及直属父节点的关键码集
QString ICDNavigationUi::queryCurrentKeys() const
{
    if (!q_treeView) {
        return QString();
    }

    return queryNodeKeys(q_treeView->currentItem());
}

// 查询节点及直属父节点的关键码集
QString ICDNavigationUi::queryNodeKeys(QStandardItem *item) const
{
    QString result;
    if (!item) {
        return result;
    }
    while (item) {
        QVariant level = item->data(LevelIndex);
        if (!level.isValid() || GlobalDefine::ntUnknown == level.toInt()) {
            break;
        }
        result.insert(0, item->data(UserKey).toString());
        result.insert(0, "/");
        item = item->parent();
    }
    result.right(result.size() - result.indexOf("/"));
    result.remove(result.indexOf("/"), strlen("/"));

    return result;
}

void ICDNavigationUi::collapseItem(QStandardItem *item)
{
    if (!item) {
        return;
    }
    if (item->hasChildren()) {
        int count = item->rowCount();
        for (int i = 0; i < count; ++i) {
            collapseItem(item->child(i));
        }
    }
    q_treeView->collapseItem(item);
}

void ICDNavigationUi::createRuleMenu()
{
    if (!q_ruleMenu) {
        q_ruleMenu = new QMenu(QStringLiteral("新增"), q_treeView);
        q_ruleMenu->setIcon(QIcon(":/datastudio/image/global/add.png"));
        std::vector<stDictionary> dics;

        QVariantList args;
        args.append(qVariantFromValue((void*)&dics));
        args.append(int(GlobalDefine::dicDataType));
        jnotify->send("edit.queryDictionaryTable", args);

        QAction *action = nullptr;
        const int count = dics.size();
        for (int i = 0; i < count; ++i) {
            const stDictionary &dic = dics.at(i);
            action = q_ruleMenu->addAction(QString::fromStdString(dic.sDec));
            action->setData(dic.nCode);
            connect(action, &QAction::triggered, this, &ICDNavigationUi::slotNew);
        }
    }
}

// 更新表节点下的所有规则节点提示信息
void ICDNavigationUi::updateTopTableTip()
{
    ICDElement::smtElement table = 0;

    queryTopTable(table);
    QStandardItem *item = q_treeView->currentItem();
    if (!item || item->data(LevelIndex).toInt() < GlobalDefine::ntTable) {
        return; // 当前节点层级不需要更新
    }
    // 获取顶层表节点
    while (item->data(LevelIndex).toInt() > GlobalDefine::ntTable) {
        item = item->parent();
    }
    updateRuleTip(SMT_CONVERT(TableNode, table), item);
}

// 更新规则节点提示信息
void ICDNavigationUi::updateRuleTip(const TableNode::smtTable &table, QStandardItem *parent)
{
    if (!table || table->isEmpty()) {
        return;
    }
    int offset = -1;
    QString identify;
    QStandardItem *ruleItem = 0;
    TableNode::smtTable subData = 0;
    ICDComplexData::smtComplex complex = 0;
    ICDMetaData::ruleMap dataMap(table->allRule());
    ICDMetaData::ruleMap::iterator it(dataMap.begin());
    for (; it != dataMap.end(); ++it) {
        ICDMetaData::smtMeta &meta = it->second;
        if (!meta) {
            continue;
        }
        complex = 0;
        identify = QString("%1").arg(meta->serial());
        if (GlobalDefine::dtComplex == meta->type()) {
            complex = SMT_CONVERT(ICDComplexData, meta);
            identify.append("/").append(complex->rule().c_str());
        }
        if (ruleItem = findItem(UserKey, identify, parent)) {
            ruleItem->setText(offsetString(meta, offset));
        }
        if (complex) {
            TableNode::tableVector subTable = complex->allTable();
            const int count = subTable.size();
            for (int i = 0; i < count; ++i) {
                if (!(subData = subTable[i])) {
                    continue;
                }
                if (GlobalDefine::dtDiscern == meta->type()) { // 帧数据
                    ruleItem = findItem(UserKey, subData->key().c_str(), ruleItem);
                } else if (GlobalDefine::dtComplex == meta->type()) {
                }
                updateRuleTip(subData, ruleItem);
            }
        }
    }
}

// 查询数据源
int ICDNavigationUi::queryDataSource() const
{
    int result = GlobalDefine::dsNone;
    if (!q_treeView) {
        return result;
    }
    QStandardItem *root = q_treeView->topLevelItem(0);
    if (!root) {
        return result;
    }

    return root->data(DataSource).toInt();
}

// 更新节点数据
bool ICDNavigationUi::updateNodeData(int dataAddr)
{
    bool result = false;
    if (0 == dataAddr || !q_treeView) {
        return result;
    }
    QStandardItem *current = q_treeView->currentItem();
    if (!current) {
        return result;
    }
    QVariant level = current->data(LevelIndex);
    if (!level.isValid()) {
        return result;
    }
    if (GlobalDefine::ntUnknown == level.toInt()) {
        result = updatePalneData(*reinterpret_cast<stPlane *>(dataAddr));
    } else if (GlobalDefine::ntPlane == level.toInt()) {
        result = updateSystemData(*reinterpret_cast<stSystem *>(dataAddr));
    } else if (GlobalDefine::ntSystem == level.toInt()) {
        result = updateICDData(*reinterpret_cast<stICDBase *>(dataAddr));
    } else if (GlobalDefine::ntTable == level.toInt()) {
        result = updateRuleData(*reinterpret_cast<ICDMetaData::smtMeta *>(dataAddr));
    } else if (GlobalDefine::ntRule == level.toInt()) {
        QVariant variant = current->data(SubTable);
        if (variant.isValid()) {    // 复合数据
            result = updateRuleData(*reinterpret_cast<ICDMetaData::smtMeta *>(dataAddr), false);
            // 更新规则提示
            updateTopTableTip();
        } else {
            variant = current->data(RuleDefine);
            if (GlobalDefine::dtDiscern == variant.toInt()) {
                result = updateSubTableData(*reinterpret_cast<stICDBase *>(dataAddr));
            } else {    // 编辑具体规则数据
                result = updateDetailRuleData(*reinterpret_cast<ICDMetaData::smtMeta *>(dataAddr));
            }
        }
    }

    return result;
}

// 获取不重复的分组ID
QString ICDNavigationUi::nextID(int type)
{
    QString result;
    if (!q_treeView) {
        return result;
    }
    QStandardItem *current = q_treeView->currentItem();
    if (!current) {
        return result;
    }
    int code = -1;
    if (GlobalDefine::ntPlane == type) { // 机型
        maxID(current, type, result);
        if (result.isEmpty()) {
            result = QString("1001");
        } else {
            code = result.toInt() + 1;
            result = QString("%1").arg(code, 4, 10, QChar('0'));
        }
    } else if (GlobalDefine::ntSystem == type) { // 系统
        maxID(current, type, result);
        if (result.isEmpty()) {
            result = QString("2001");
        } else {
            code = result.toInt() + 1;
            result = QString("%1").arg(code, 4, 10, QChar('0'));
        }
    } else if (GlobalDefine::ntTable == type) { // 表
#if 0
        QString key;
        QStringList keyList;
        int itemCount = current->rowCount();
        while (current->data(0, LevelIndex) > GlobalDefine::ntSystem) {
            current = current->parent();
        }
        keyList = queryNodeKeys(current).split("/");
        maxID(current, type, result);
        if (result.isEmpty()) {
            result = QString("ICD_%1_%2_0001")
                    .arg(keyList.at(0)).arg(keyList.at(1));
        } else {
            keyList = result.split("_");
            code = keyList.takeLast().toInt() + 1;
            result = keyList.join("_")
                    .append(QString("_%1").arg(code, 4, 10, QChar('0')));
        }
#else
        result = QUuid::createUuid().toString()
                .remove(QRegExp("[{}-]")).prepend("ICDTable_");
#endif
    }

    return result;
}

// 插入节点数据
bool ICDNavigationUi::insertNodeData(const std::vector<int> &params)
{
    bool result = false;
    if (2 != params.size() || !q_treeView) {
        return result;
    }
    QStandardItem *current = q_treeView->currentItem();
    if (!current) {
        return result;
    }
    ICDMetaData::smtMeta meta
            = *reinterpret_cast<ICDMetaData::smtMeta *>(params.at(1));
    QString keys = queryNodeKeys(current);
    QStringList keyLst = keys.split("/", QString::SkipEmptyParts);
    int planeID = keyLst.takeFirst().toInt();
    int systemID = keyLst.takeFirst().toInt();
    ICDComplexData::smtComplex complex = SMT_CONVERT(ICDComplexData, meta);
    if (GlobalDefine::dtComplex == meta->type()) {
        stICDBase base;
        base.sID = QUuid::createUuid()
                .toString().remove(QRegExp("[{}-]")).toStdString();
        base.sName.append("ICDTable_").append(base.sID);
        base.sCode = complex->proCode();
        base.nLength = complex->length();
        base.sParentName = keyLst.last().toStdString();
        base.sGroup = QString("%1/%2").arg(planeID)
                .arg(systemID).toStdString();
        base.sDescribe = complex->name();
        base.sRemark = complex->remark();
        TableNode::smtTable table(new TableNode(base));
        complex->addTable(table);
    }
    std::vector<void*> arguments;

    arguments.push_back(&keys);
    arguments.push_back(&meta);

    QVariantList args;
    args.append(qVariantFromValue((void*)&arguments));
    args.append(qVariantFromValue((void*)&result));
    jnotify->send("edit.insertRule", args);

    if (result) {
        // 更新数据状态标识
        updateDataState(current, StateChanged);
        // 重新加载树
        reLoadRuleNode(current);
        // 重新刷新界面数据
        //slotCurrentChanged(current, NULL);
        if (current->data(SubTable).isValid()) {
            // 更新规则提示
            updateTopTableTip();
        }
    }

    return result;
}

// 删除节点
bool ICDNavigationUi::deleteNode(const std::vector<QString> &nodeID)
{
    bool result = false;
    QStandardItem *current = q_treeView->currentItem();
    if (!current) {
        return result;
    }
    int level = current->data(LevelIndex).toInt();
    const int count = nodeID.size();
    // 删除数据库
    if (GlobalDefine::ntUnknown == level) {
        std::vector<int> planeID;
        for (int i = 0; i < count; ++i) {
            planeID.push_back(nodeID.at(i).toInt());
        }
        QVariantList args;
        args.append(qVariantFromValue((void*)&planeID));
        args.append(qVariantFromValue((void*)&result));
        jnotify->send("edit.deletePlane", args);
    } else if (GlobalDefine::ntPlane == level) {
        std::vector<int> systemID;
        std::vector<void*> params;
        QString planeID = parentKey(GlobalDefine::ntPlane);

        for (int i = 0; i < count; ++i) {
            systemID.push_back(nodeID.at(i).toInt());
        }
        params.push_back(&planeID);
        params.push_back(&systemID);

        QVariantList args;
        args.append(qVariantFromValue((void*)&params));
        args.append(qVariantFromValue((void*)&result));
        jnotify->send("edit.deleteSystem", args);
    } else if (GlobalDefine::ntSystem == level) {
        std::vector<std::string> tableID;
        std::vector<void*> params;
        QString keys = queryNodeKeys(current);

        for (int i = 0; i < count; ++i) {
            tableID.push_back(nodeID.at(i).toStdString());
        }
        params.push_back(&keys);
        params.push_back(&tableID);

        QVariantList args;
        args.append(qVariantFromValue((void*)&params));
        args.append(qVariantFromValue((void*)&result));
        jnotify->send("edit.deleteTable", args);
    } else if (GlobalDefine::ntTable == level
               || GlobalDefine::ntRule == level) {
        std::vector<void*> params;
        QString keys = queryNodeKeys(current);
        int ruleType = current->data(RuleDefine).toInt();
        if (GlobalDefine::dtDiscern == ruleType) {
            // 帧数据
            std::vector<std::string> rules;
            for (int i = 0; i < count; ++i) {
                rules.push_back(nodeID.at(i).toStdString());
            }
            params.push_back(&keys);
            params.push_back(&rules);

            QVariantList args;
            args.append(qVariantFromValue((void*)&params));
            args.append(qVariantFromValue((void*)&result));
            jnotify->send("edit.deleteSubTable", args);
        } else {    // 其他规则数据
            std::vector<int> rules;
            // 查询子表
            for (int i = 0; i < count; ++i) {
                rules.push_back(nodeID.at(i).split("/").first().toInt());
            }
            params.push_back(&keys);
            params.push_back(&rules);

            QVariantList args;
            args.append(qVariantFromValue((void*)&params));
            args.append(qVariantFromValue((void*)&result));
            jnotify->send("edit.deleteRule", args);
        }
    }
    if (result) {
        std::map<int, int> items;
        QStandardItem *item = 0;
        // 记录需要删除的数据行号
        const int count = nodeID.size();
        for (int i = 0; i < count; ++i) {
            if (item = findItem(UserKey, nodeID.at(i), current)) {
                items[item->row()] = item->row();
            }
        }
        // 从后往前删除
        std::map<int, int>::reverse_iterator it = items.rbegin();
        for (; it != items.rend(); ++it) {
            current->removeRow(it->first);
        }
        // 更新父节点加载状态
        if (GlobalDefine::ntTable > level) {
            updateParentFlag(current);
        }
        if (level > GlobalDefine::ntSystem) {
            // 重新加载树
            reLoadRuleNode(current);
        }
        // 更新数据状态标识
        updateDataState(current, StateChanged);
        // 重新刷新界面数据
        //slotCurrentChanged(current, NULL);
        //q_treeView->setCurrentItem(current);
        if (current->data(SubTable).isValid()) {
            // 更新规则提示
            updateTopTableTip();
        }
    }

    return result;
}

// 重新调整节点顺序
bool ICDNavigationUi::reorderNodeData(const std::vector<int> &params)
{
    bool result = false;
    if (2 != params.size() || !q_treeView) {
        return result;
    }
    QStandardItem *current = q_treeView->currentItem();
    if (!current) {
        return result;
    }
    ICDElement::smtElement element;
    std::vector<ICDElement::smtElement> elements;
    int pos = params.at(1);
    QString keys = queryNodeKeys(current);
    QStringList keyLst = keys.split("/", QString::SkipEmptyParts);
    int level = current->data(LevelIndex).toInt();
    if (GlobalDefine::ntUnknown == level) {
        elements = *reinterpret_cast<std::vector<ICDElement::smtElement> *>(params.at(0));

        QVariantList args;
        args.append(qVariantFromValue((void*)&elements));
        args.append(qVariantFromValue((void*)&result));
        jnotify->send("edit.modifyAllPlane", args);
    } else {
        int planeID = keyLst.takeFirst().toInt();
        element = *reinterpret_cast<ICDElement::smtElement *>(params.at(0));
        if (GlobalDefine::ntPlane == level) {
            QVariantList args;
            args.append(qVariantFromValue((void*)&element));
            args.append(qVariantFromValue((void*)&result));
            jnotify->send("edit.modifyPlane", args);
        } else if (GlobalDefine::ntSystem == level) {
            std::vector<void *> params;
            params.push_back(&planeID);
            params.push_back(&element);

            QVariantList args;
            args.append(qVariantFromValue((void*)&params));
            args.append(qVariantFromValue((void*)&result));
            jnotify->send("edit.modifySystem", args);
        } else if (GlobalDefine::ntTable == level) {
            std::vector<void*> params;
            params.push_back(&keys);
            params.push_back(&element);

            QVariantList args;
            args.append(qVariantFromValue((void*)&params));
            args.append(qVariantFromValue((void*)&result));
            jnotify->send("edit.modifyTable", args);
        } else if (GlobalDefine::ntRule == level) {
            QVariant variant = current->data(SubTable);
            std::vector<void*> params;
            if (variant.isValid()) {    // 复合数据，更新表
                keys = keys.left(keys.lastIndexOf("/"));
                params.push_back(&keys);
                params.push_back(&element);

                QVariantList args;
                args.append(qVariantFromValue((void*)&params));
                args.append(qVariantFromValue((void*)&result));
                jnotify->send("edit.modifyTable", args);
            } else {
                variant = current->data(RuleDefine);
                if (GlobalDefine::dtDiscern == variant.toInt()) {
                    int ret = 0;
                    keys = keys.left(keys.lastIndexOf("/"));
                    params.push_back(&keys);
                    params.push_back(&element);

                    QVariantList args;
                    args.append(qVariantFromValue((void*)&params));
                    args.append(qVariantFromValue((void*)&ret));
                    jnotify->send("edit.saveRule", args);
                    result = (result != 0);
                }
            }
        }
    }

    if (result) {
        if (GlobalDefine::ntUnknown == level) {
            reorderPlaneNode(elements, pos);
        } else if (GlobalDefine::ntPlane == level) {
            PlaneNode::smtPlane plane = SMT_CONVERT(PlaneNode, element);
            reorderSystemNode(plane, pos);
        } else if (GlobalDefine::ntSystem == level) {
            SystemNode::smtSystem system = SMT_CONVERT(SystemNode, element);
            reorderTableNode(system, pos);
        } else if (GlobalDefine::ntTable == level) {
            TableNode::smtTable table = SMT_CONVERT(TableNode, element);
            reorderRuleNode(table, pos);
        } else if (GlobalDefine::ntRule == level) {
            if (GlobalDefine::dtDiscern == current->data(RuleDefine).toInt()) {    // 帧数据
                ICDMetaData::smtMeta meta = SMT_CONVERT(ICDMetaData, element);
                reorderSubTableNode(meta, pos);
            } else {
                TableNode::smtTable table = SMT_CONVERT(TableNode, element);
                reorderRuleNode(table, pos);
            }
        }
    }

    return result;
}

// 保存粘贴数据
ICDElement::smtElement ICDNavigationUi::savePastedData(const QVector<int> &params)
{
    if (params.size() < 2) {
        return false;
    }
    QVector<int> datas(params);
    QString keys = queryCurrentKeys();
    datas << (int)&keys;
    ICDElement::smtElement copyData;
    // 保存数据
    QVariantList args;
    args.append(qVariantFromValue((void*)&datas));
    args.append(qVariantFromValue((void*)&copyData));
    jnotify->send("edit.copyMemoryData", args);

    if (!copyData) {
        return copyData;
    }
    // 更新数据
    QStandardItem *current = q_treeView->currentItem();
    // 更新界面
    QStandardItem *item = 0;
    ICDElement::smtElement element;
    ICDMetaData::smtMeta meta;
    switch (current->data(LevelIndex).toInt()) {
    case GlobalDefine::ntUnknown:   // 根节点
        // 插入机型数据
        item = formPlaneNode(current, SMT_CONVERT(PlaneNode, copyData));
        break;
    case GlobalDefine::ntPlane:     // 机型节点
        // 插入系统数据
        item = formSystemNode(current, SMT_CONVERT(SystemNode, copyData));
        break;
    case GlobalDefine::ntSystem:    // 系统节点
        // 插入表数据
        item = formTableNode(current, SMT_CONVERT(TableNode, copyData));
        break;
    case GlobalDefine::ntTable:     // 表节点
        // 插入规则数据
        item = formRuleNode(current, SMT_CONVERT(ICDMetaData, copyData));
        break;
    case GlobalDefine::ntRule:      // 规则节点
        queryNodeData(QString(), element);
        meta = SMT_CONVERT(ICDMetaData, element);
        if (GlobalDefine::dtDiscern == current->data(RuleDefine).toInt()) {
            // 插入表数据
            item = formSubTableNode(current, SMT_CONVERT(TableNode, copyData),
                                    current->row(), meta ? meta->index() : -1);
        } else {
            // 插入规则数据
            item = formRuleNode(current, SMT_CONVERT(ICDMetaData, copyData),
                                meta ? meta->index() : -1);
        }
        break;
    default:break;
    }
    if (item) {
        // 更新数据状态
        updateDataState(item, StateChanged);
        // 重新刷新界面数据
        //slotCurrentChanged(current, NULL);
        q_treeView->expandItem(current);
    }

    return copyData;
}

// 保存内存数据到数据源
bool ICDNavigationUi::saveMemoryData(int type, const QString &file, QString &err)
{
    QStandardItem *current = q_treeView->currentItem();
    if (!current) {
        return false;
    }
    QString keys = queryNodeKeys(current);
    // 保存数据到数据源
    if (GlobalDefine::dsDatabase == type) {
        // 保存根节点时二次确认
        if (keys.isEmpty() && !current->hasChildren()) {
            if (QMessageBox::warning(this,
                                     QStringLiteral("确认提示"),
                                     QStringLiteral("即将删除所有机型数据，包括下属数据")
                                     + QStringLiteral("\r\n确认删除？"),
                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
                    == QMessageBox::No) {
                return false;
            }
        }
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        QVariantList args;
        args.append(qVariantFromValue((void*)&keys));
        args.append(qVariantFromValue((void*)&err));
        jnotify->send("edit.saveDatabase", args);
    } else if (GlobalDefine::dsFile == type) {
        QVector<QString> params;

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        params << keys << file;

        QVariantList args;
        args.append(qVariantFromValue((void*)&params));
        args.append(qVariantFromValue((void*)&err));
        jnotify->send("edit.saveFile", args);
    }
    QApplication::restoreOverrideCursor();

    if (err.isEmpty()) {
        // 更新数据状态标识
        updateDataState(current, StateReset);
        // 重新刷新界面数据
        //slotCurrentChanged(current, NULL);
    }

    return true;
}

// 构造单个机型节点（包括下挂分系统）
QStandardItem* ICDNavigationUi::formPlaneNode(QStandardItem *parent,
                                              const PlaneNode::smtPlane &plane)
{
    if (!parent || !plane) {
        return nullptr;
    }
    SystemNode::systemVector subSystem = plane->allSystem();
    QStandardItem* result = new QStandardItem(QIcon(":/icdwidget/image/tree/icon-vehicle.png"),
                                              QString::fromStdString(plane->name())
                                              + "<font color=green size=2>[VEHICLE]</font>");
    result->setData(plane->id().c_str(), UserKey);
    if (subSystem.empty()) {
        result->setData(GlobalDefine::wholeState, ItemLoaded);
    } else {
        result->setData(GlobalDefine::noneState, ItemLoaded);
    }
    result->setData(GlobalDefine::noneState, DataChanged);
    result->setData(GlobalDefine::ntPlane, LevelIndex);
    parent->appendRow(result);

    const int count = subSystem.size();
    for (int i = 0; i < count; ++i) {
        if (!formSystemNode(result, subSystem[i])) {
            parent->removeRow(result->row());
            return nullptr;
        }
    }
    q_treeView->expandItem(parent, true, 3);

    return result;
}

// 重新调整机型节点顺序
void ICDNavigationUi::reorderPlaneNode(const std::vector<ICDElement::smtElement> &planes,
                                       int pos)
{
    QStandardItem *current = q_treeView->currentItem();
    if (!current) {
        return;
    }
    QStandardItem *item = 0;
    QHash<QString, QStandardItem *> items;
    int index = current->rowCount();
    // 先将子节点从当前节点移除
    while (index > pos) {
        item = current->takeChild(--index);
        items[item->data(UserKey).toString()] = item;
    }
    // 根据新的顺序重新挂接到父节点上
    const int count = planes.size();
    for (int i = pos; i < count; ++i) {
        if (item = items.value(planes[i]->id().c_str())) {
            item->setText(planes[i]->name().c_str());
            item->setData(GlobalDefine::wholeState, DataChanged);
            current->insertRow(i, item);
        }
    }
    // 删除多余结点
    current->removeRows(current->rowCount() - items.size(), items.size());
    // 更新节点状态变更标识
    updateDataState(current, StateChanged);
}

// 构造单个分系统节点
QStandardItem* ICDNavigationUi::formSystemNode(QStandardItem *parent,
                                               const SystemNode::smtSystem &system)
{
    if (NULL == parent || NULL == system) {
        return NULL;
    }
    TableNode::tableVector icdTable = system->allTable();
    const int count = icdTable.size();
    QStandardItem* result = new QStandardItem(QIcon(":/icdwidget/image/tree/icon-system.png"),
                                              QString::fromStdString(system->name())
                                              + "<font color=green size=2>[SYSTEM]</font>");
    result->setData(system->numeralId(), UserKey);
    if (0 == count) {
        result->setData(GlobalDefine::wholeState, ItemLoaded);
    } else {
        result->setData(GlobalDefine::noneState, ItemLoaded);
    }
    result->setData(GlobalDefine::noneState, DataChanged);
    result->setData(GlobalDefine::ntSystem, LevelIndex);
    parent->appendRow(result);

    TableNode::smtTable table = 0;
    for (int i = 0; i < count; ++i) {
        if (!(table = icdTable[i])) {
            continue;
        }
        if (!formTableNode(result, table)) {
            parent->removeRow(result->row());
            return NULL;
        }
    }
    q_treeView->expandItem(parent);

    return result;
}

// 重新调整系统节点顺序
void ICDNavigationUi::reorderSystemNode(const PlaneNode::smtPlane &plane,
                                        int pos)
{
    QStandardItem *current = q_treeView->currentItem();
    if (!current || !plane) {
        return;
    }
    QStandardItem *item = 0;
    QHash<QString, QStandardItem *> items;
    int index = current->rowCount();
    // 先将子节点从当前节点移除
    while (index > pos) {
        item = current->takeChild(--index);
        items[item->data(UserKey).toString()] = item;
    }
    // 根据新的顺序重新挂接到父节点上
    SystemNode::systemVector systems = plane->allSystem();
    const int count = systems.size();
    SystemNode::smtSystem system = 0;
    for (int i = pos; i < count; ++i) {
        if (!(system = systems[i])) {
            continue;
        }
        if (item = items.value(system->id().c_str())) {
            item->setText(system->name().c_str());
            item->setData(GlobalDefine::wholeState, DataChanged);
            current->insertRow(i, item);
        }
    }
    // 删除多余结点
    current->removeRows(current->rowCount() - items.size(), items.size());
    // 更新节点状态变更标识
    updateDataState(current, StateChanged);
}

// 构造单个规则表节点
QStandardItem* ICDNavigationUi::formTableNode(QStandardItem *parent,
                                              const TableNode::smtTable &table)
{
    if (NULL == system) {
        return NULL;
    }
    QStandardItem* result = new QStandardItem(QIcon(":/datastudio/image/global/circle-gray.png"),
                                              QString::fromStdString(table->icdBase().sDescribe)
                                              + "<font color=green size=2>[TABLE]</font>");
    result->setToolTip(result->text());
    result->setData(table->key().c_str(), UserKey);
    result->setData(GlobalDefine::noneState, ItemLoaded);
    result->setData(GlobalDefine::noneState, DataChanged);
    result->setData(GlobalDefine::ntTable, LevelIndex);
    parent->appendRow(result);

    if (!table->allRule().empty()) {
        loadRuleNode(result, table);
        // 更新节点状态
        updateLoadedFlag(result, optLoad);
    }

    return result;
}

// 重新调整规则表节点顺序
void ICDNavigationUi::reorderTableNode(const SystemNode::smtSystem &system,
                                       int pos)
{
    QStandardItem *current = q_treeView->currentItem();
    if (!current || !system) {
        return;
    }
    QStandardItem *item = 0;
    QHash<QString, QStandardItem *> items;
    // 先将子节点从当前节点移除
    int index = current->rowCount();
    while (index > pos) {
        item = current->takeChild(--index);
        items[item->data(UserKey).toString()] = item;
    }
    // 根据新的顺序重新挂接到父节点上
    TableNode::tableVector tables = system->allTable();
    const int count = tables.size();
    TableNode::smtTable table = 0;
    for (int i = pos; i < count; ++i) {
        if (!(table = tables[i])) {
            continue;
        }
        if (item = items.value(table->id().c_str())) {
            item->setText(table->name().c_str());
            item->setData(GlobalDefine::wholeState, DataChanged);
            current->insertRow(i, item);
        }
    }
    // 删除多余节点
    current->removeRows(current->rowCount() - items.size(), items.size());
    // 更新节点状态变更标识
    updateDataState(current, StateChanged);
}

// 构造单个规则子表节点
QStandardItem* ICDNavigationUi::formSubTableNode(QStandardItem *parent,
                                                 const TableNode::smtTable& table,
                                                 int index, int offset)
{
    stICDBase base = table->icdBase();
    QStandardItem *result = new QStandardItem(QIcon(":/icdwidget/image/tree/icon-table.png"),
                                              QString("<font color=darkgreen size=2>[%1:%2]</font>"
                                                      "%3<font color=darkgreen size=2>[TABLE]</font>")
                                              .arg(index, 4, 10, QChar('0')).arg(offset, 4, 10, QChar('0'))
                                              .arg(table->name().c_str()));
    result->setData(GlobalDefine::ntRule, LevelIndex);
    result->setData(GlobalDefine::dtComplex, RuleDefine);
    result->setData(base.sName.c_str(), SubTable);
    result->setData(base.sName.c_str(), UserKey);
    result->setData(GlobalDefine::noneState, DataChanged);
    parent->appendRow(result);

    loadRuleNode(result, table, offset);

    return result;
}

// 重新调整规则子表节点顺序
void ICDNavigationUi::reorderSubTableNode(const ICDMetaData::smtMeta &meta,
                                          int pos)
{
    ICDComplexData::smtComplex complex
            = std::dynamic_pointer_cast<ICDComplexData>(meta);
    QStandardItem *current = q_treeView->currentItem();
    if (!current || !complex) {
        return;
    }
    QStandardItem *item = 0;
    QHash<QString, QStandardItem *> items;
    // 先将子节点从当前节点移除
    int index = current->rowCount();
    while (index > pos) {
        item = current->takeChild(--index);
        items[item->data(UserKey).toString()] = item;
    }
    // 根据新的顺序重新挂接到父节点上
    TableNode::tableVector tables = complex->allTable();
    const int count = tables.size();
    TableNode::smtTable table = 0;
    for (int i = pos; i < count; ++i) {
        if (!(table = tables[i])) {
            continue;
        }
        if (item = items.value(table->id().c_str())) {
            item->setText(table->name().c_str());
            current->insertRow(i, item);
        }
    }
    // 删除多余结点
    current->removeRows(current->rowCount() - items.size(), items.size());
    // 更新节点状态变更标识
    updateDataState(current, StateChanged);
}

// 构造规则节点
QStandardItem* ICDNavigationUi::formRuleNode(QStandardItem *parent,
                                             const ICDMetaData::smtMeta& meta,
                                             int offset)
{
    if (NULL == meta) {
        return NULL;
    }
    QStandardItem* result = new QStandardItem(offsetString(meta, offset));
    result->setData(meta->serial(), UserKey);
    result->setData(GlobalDefine::noneState, DataChanged);
    result->setData(GlobalDefine::ntRule, LevelIndex);
    result->setData(meta->type(), RuleDefine);
    parent->appendRow(result);
    // 如果是复合数据，则递归加载子表
    if (IcdDefine::icdComplex == meta->metaType()) {
        ICDComplexData::smtComplex complex = SMT_CONVERT(ICDComplexData, meta);
        if (complex) {
            TableNode::tableVector subTable = complex->allTable();
            const int count = subTable.size();
            TableNode::smtTable subData = 0;
            for (int i = 0; i < count; ++i) {
                if (!(subData = subTable[i])) {
                    continue;
                }
                if (GlobalDefine::dtDiscern == meta->type()) { // 帧数据
                    formSubTableNode(result, subData, i + 1, meta->index());
                } else if (GlobalDefine::dtComplex == meta->type()) {
                    loadRuleNode(result, subData, meta->index());
                }
            }
        }
        if (GlobalDefine::dtComplex == meta->type()) {
            result->setData(QString("%1/%2").arg(complex->serial())
                            .arg(complex->rule().c_str()), UserKey);
            ICDComplexData::smtComplex complex = SMT_CONVERT(ICDComplexData, meta);
            result->setData(complex->rule().c_str(), SubTable);
            result->setIcon(QIcon(":/icdwidget/image/tree/icon-complex.png"));
        } else {
            result->setIcon(QIcon(":/icdwidget/image/tree/icon-frame.png"));
        }
    } else {
        result->setIcon(QIcon(":/icdwidget/image/tree/icon-item.png"));
    }

    return result;
}

// 重新调整规则节点顺序
void ICDNavigationUi::reorderRuleNode(const TableNode::smtTable& table,
                                      int pos)
{
    Q_UNUSED(pos)
    QStandardItem *current = q_treeView->currentItem();
    if (!current || !table) {
        return;
    }
    // 重新加载树
    loadRuleNode(current, table);

    // 更新节点状态变更标识
    updateDataState(current, StateChanged);
}

// 加载规则节点
void ICDNavigationUi::loadRuleNode(QStandardItem *parent, const TableNode::smtTable &table, int offset)
{
    if (!parent || !table) {
        return;
    }
    // 清空原始数据
    parent->removeRows(0, parent->rowCount());

    ICDMetaData::ruleMap dataMap(table->allRule());
    ICDMetaData::ruleMap::iterator it(dataMap.begin());
    for (; it != dataMap.end(); ++it) {
        ICDMetaData::smtMeta &meta = it->second;
        if (!formRuleNode(parent, meta, offset)) {
            parent->removeRows(0, parent->rowCount());
            return;
        }
    }
    if (GlobalDefine::ntTable == parent->data(LevelIndex).toInt()
            && dataMap.size() > 0) {
        parent->setIcon(QIcon(":/datastudio/image/global/circle-green.png"));
    }
}

// 卸载规则数据
void ICDNavigationUi::unLoadRuleNode(QStandardItem *item)
{
    if (!q_treeView || !item) {
        return;
    }
    QVariant level = item->data(LevelIndex);
    if (!level.isValid()) {
        return;
    }
    if (level.toInt() < GlobalDefine::ntTable) {
        int index(0);
        QStandardItem *childItem = item->child(index);
        while (childItem) {
            unLoadRuleNode(childItem);
            childItem = item->child(++index);
        }
    } else {
        item->removeRows(0, item->rowCount());
    }
}

// 重新加载规则节点
void ICDNavigationUi::reLoadRuleNode(QStandardItem *item)
{
    if (!item) {
        return;
    }
    PlaneNode::smtPlane plane;
    // 查询内存数据
    int planeID = parentKey(GlobalDefine::ntPlane).toInt();

    QVariantList args;
    args.append(qVariantFromValue((void*)&plane));
    args.append(planeID);
    jnotify->send("edit.querySinglePlane", args);
    // 查询规则表
    int offset = -1;
    TableNode::smtTable table;
    int level = item->data(LevelIndex).toInt();
    if (GlobalDefine::ntTable == level) {
        table = ruleNodeMap(item, plane);
    } else if (GlobalDefine::ntRule == level) {
        ICDMetaData::smtMeta meta = ruleNodeData(item, plane);
        if (meta) {
            offset = meta->index();
        }
        table = subRuleNodeMap(item, plane);
    }
    if (table) {
        item->removeRows(0, item->rowCount());
        loadRuleNode(item, table, offset);
    }
}

// 显示右键菜单
void ICDNavigationUi::showMenu(QStandardItem* item)
{
    if (NULL == item) {
        return;
    }
    QVariant level = item->data(LevelIndex);
    if (!level.isValid()) {
        return;
    }
    QMenu menu(this);
    QMenu *rule = 0;
    QAction *act = 0;
    QAction *actNew = 0;
    QAction *actClear = 0;
    int state = GlobalDefine::noneState;
    // 如果节点下所有的规则表均未加载，则显示“加载”菜单
    // 如果节点下所有的规则表均卸载，则不显示“卸载”菜单
    switch (level.toInt()) {
    case GlobalDefine::ntUnknown:   // 根节点
        if (GlobalDefine::dsNone == item->data(UserKey).toInt()) {
            // 查询数据源
            int dataSource = GlobalDefine::dsNone;

            QVariantList args;
            args.append(qVariantFromValue((void*)&dataSource));
            jnotify->send("edit.queryDataSource", args);

            if (GlobalDefine::dsDatabase == dataSource) {
                act = menu.addAction(QIcon(":/datastudio/image/global/load-db.png"),
                                     QStringLiteral("加载数据库"),
                                     this, SLOT(slotLoadData()));
                act->setObjectName("database");
            } else if (GlobalDefine::dsFile == dataSource) {
                act = menu.addAction(QIcon(":/datastudio/image/global/load-file.png"),
                                     QStringLiteral("加载文件"),
                                     this, SLOT(slotLoadData()));
                act->setObjectName(QStringLiteral("file"));
            }
        } else {
            actNew = menu.addAction(QIcon(":/datastudio/image/global/add.png"),
                                    QStringLiteral("新增"),
                                    this, SLOT(slotNew()));
            actClear = menu.addAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                                      QStringLiteral("清空"),
                                      this, SLOT(slotClear()));
        }
        break;
    case GlobalDefine::ntPlane:     // 机型
    case GlobalDefine::ntSystem:    // 分系统
        if (hasRuleTable(item)) {
            state = dataLoaded(item);
            if (GlobalDefine::wholeState != state) {
                menu.addAction(QIcon(":/icdwidget/image/tree/load.png"),
                               QStringLiteral("加载"),
                               this, SLOT(slotLoadRule()));
            }
            if (GlobalDefine::noneState != state) {
                menu.addAction(QIcon(":/icdwidget/image/tree/unload.png"),
                               QStringLiteral("卸载"),
                               this, SLOT(slotUnloadRule()));
            }
        }
        actNew = menu.addAction(QIcon(":/datastudio/image/global/add.png"),
                                QStringLiteral("新增"),
                                this, SLOT(slotNew()));
        menu.addAction(QIcon(":/datastudio/image/global/remove.png"),
                       QStringLiteral("删除"),
                       this, SLOT(slotDelete()));
        actClear = menu.addAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                                  QStringLiteral("清空"),
                                  this, SLOT(slotClear()));
        break;
    case GlobalDefine::ntTable:     // 规则表
        createRuleMenu();
        state = dataLoaded(item);
        if (GlobalDefine::wholeState == state) {
            menu.addAction(QIcon(":/icdwidget/image/tree/unload.png"),
                           QStringLiteral("卸载"),
                           this, SLOT(slotUnloadRule()));
            menu.addAction(QIcon(":/icdwidget/image/tree/update.png"),
                           QStringLiteral("更新"),
                           this, SLOT(slotUpdate()));
            menu.addMenu(q_ruleMenu);
            rule = q_ruleMenu;
            menu.addAction(QIcon(":/datastudio/image/global/remove.png"),
                           QStringLiteral("删除"),
                           this, SLOT(slotDelete()));
            actClear = menu.addAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                                      QStringLiteral("清空"),
                                      this, SLOT(slotClear()));
        } else {
            menu.addAction(QIcon(":/icdwidget/image/tree/load.png"),
                           QStringLiteral("加载"),
                           this, SLOT(slotLoadRule()));
            menu.addAction(QIcon(":/datastudio/image/global/remove.png"),
                           QStringLiteral("删除"),
                           this, SLOT(slotDelete()));
        }
        break;
    case GlobalDefine::ntRule:     // 具体规则
        switch (item->data(RuleDefine).toInt()) {
        case GlobalDefine::dtComplex:   // 复合数据，增加规则菜单
            createRuleMenu();
            menu.addMenu(q_ruleMenu);
            rule = q_ruleMenu;
            menu.addAction(QIcon(":/datastudio/image/global/remove.png"),
                           QStringLiteral("删除"),
                           this, SLOT(slotDelete()));
            actClear = menu.addAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                                      QStringLiteral("清空"),
                                      this, SLOT(slotClear()));
            break;
        case GlobalDefine::dtDiscern:   // 帧数据，增加新增菜单
            actNew = menu.addAction(QIcon(":/datastudio/image/global/add.png"),
                                    QStringLiteral("新增"),
                                    this, SLOT(slotNew()));
            menu.addAction(QIcon(":/datastudio/image/global/remove.png"),
                           QStringLiteral("删除"),
                           this, SLOT(slotDelete()));
            actClear = menu.addAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                                      QStringLiteral("清空"),
                                      this, SLOT(slotClear()));
            break;
        default:
            menu.addAction(QIcon(":/datastudio/image/global/remove.png"),
                           QStringLiteral("删除"),
                           this, SLOT(slotDelete()));
            break;
        }
        break;
    default:
        break;
    }
    // 有子节点增加展开、收起菜单
    if (item->hasChildren()) {
        if (q_treeView->isItemExpanded(item)) {
            QAction *actionCollapse = menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                                                     QStringLiteral("收起"));
            connect(actionCollapse, &QAction::triggered, this, [=](){
                collapseItem(item);
            });
        } else {
            QAction *actionExpand = menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                                                   QStringLiteral("展开"));
            connect(actionExpand, &QAction::triggered, this, [=](){
                q_treeView->expandItem(item);
            });
        }
    }
    // 查询界面按钮状态
    if (actNew) {
        bool newFlag = false;

        QVariantList args;
        QString command("add");
        args.append(qVariantFromValue((void*)&command));
        args.append(qVariantFromValue((void*)&newFlag));
        jnotify->send("edit.queryMainViewState", args);
        actNew->setEnabled(newFlag);
    }
    if (rule) {
        bool newFlag = false;

        QVariantList args;
        QString command("add");
        args.append(qVariantFromValue((void*)&command));
        args.append(qVariantFromValue((void*)&newFlag));
        jnotify->send("edit.queryMainViewState", args);

        rule->setEnabled(newFlag);
    }
    if (actClear) {
        bool clearFlag = false;

        QVariantList args;
        QString command("clean");
        args.append(qVariantFromValue((void*)&command));
        args.append(qVariantFromValue((void*)&clearFlag));
        jnotify->send("edit.queryMainViewState", args);

        actClear->setEnabled(clearFlag);
    }

    if (!menu.isEmpty()) {
        menu.exec(QCursor::pos());
    }
}

/**
 * @brief 查询指定节点是否尚未加载数据
 * @param [in] item : 节点指针
 * @param [in] level : 节点类型
 * @return 数据加载状态
 */
int ICDNavigationUi::dataLoaded(QStandardItem *item) const
{
    int result = GlobalDefine::noneState;
    if (!item ) {
        return result;
    }
    return result = item->data(ItemLoaded).toInt();
}

// 获取当前节点的直属子节点数据
QVector<QString> ICDNavigationUi::directChidData(QStandardItem* item) const
{
    QVector<QString> result;
    if (NULL == item) {
        return result;
    }
    QVariant level = item->data(LevelIndex);
    if (!level.isValid()) {
        return result;
    }
    // 子节点关键码集
    int index = 0;
    QStandardItem* childItem = 0;
    while (childItem = item->child(index++)) {
        result.append(childItem->data(UserKey).toString());
    }

    return result;
}

// 获取指定节点的子表
std::vector<std::string> ICDNavigationUi::subTables(QStandardItem *item) const
{
    std::vector<std::string> result;
    if (NULL == item) {
        return result;
    }
    QVariant value = item->data(SubTable);
    if (value.isValid()) {
        result.push_back(value.toString().toStdString());
    }

    int index = 0;
    std::vector<std::string> tables;
    QStandardItem *childItem = item->child(index);
    while (childItem) {
        tables = subTables(childItem);
        if (!tables.empty()) {
            result.insert(result.end(), tables.begin(), tables.end());
        }
        childItem = item->child(++index);
    }

    return result;
}

// 查询节点是否有下挂规则节点
bool ICDNavigationUi::hasRuleTable(QStandardItem *item) const
{
    if (!item) {
        return false;
    }

    if (item->data(LevelIndex).toInt() > GlobalDefine::ntSystem) {
        return true;
    }
    int index = 0;
    QStandardItem *childItem = NULL;
    while (childItem = item->child(index++)) {
        if (hasRuleTable(childItem)) {
            return true;
        }
    }

    return false;
}

// 查询节点下挂顶层规则表数量
int ICDNavigationUi::topRuleTable(QStandardItem *item) const
{
    int result = 0;
    if (!item) {
        return result;
    }
    int level = item->data(LevelIndex).toInt();
    if (level < GlobalDefine::ntTable) {    // 机型、系统
        int index = 0;
        QStandardItem *childItem = NULL;
        while (childItem = item->child(index++)) {
            result += topRuleTable(childItem);
        }
    } else if (level == GlobalDefine::ntTable) {    // 顶层规则表
        result = 1;
    } else {    // 具体规则项
    }

    return result;
}

// 获取当前选中节点的直属父节点关键码（机型码，系统码，表名）
QString ICDNavigationUi::parentKey(int level)
{
    QString result;
    QStandardItem* item = q_treeView->currentItem();
    if (!item) {
        return result;  // 无效
    }
    QVariant varLevel;
    QVariant varKey;
    while (NULL != item) {
        varLevel = item->data(LevelIndex);
        if (varLevel.toInt() < level) { // 目标节点不在当前选中节点的父节点序列
            break;
        } else if (varLevel.toInt() == level) {
            varKey = item->data(UserKey);
            break;
        } else {
            item = item->parent();
        }
    }
    if (!varKey.isValid()) {
        return result;
    }

    return result = varKey.toString();
}

// 所有机型数据
PlaneNode::planeVector ICDNavigationUi::planeNodeMap()
{
    PlaneNode::planeVector result;

    QVariantList args;
    args.append(qVariantFromValue((void*)&result));
    jnotify->send("edit.queryAllPlane", args);

    return result;
}

// 指定机型下的所有系统数据
SystemNode::systemMap ICDNavigationUi::systemNodeMap(QStandardItem* item,
                                                     PlaneNode::smtPlane plane)
{
    SystemNode::systemMap result;
    if (!item || !plane) {
        return result;
    }
    QVector<QString> keys = directChidData(item);
    QVectorIterator<QString> it(keys);
    while (it.hasNext()) {
        SystemNode::smtSystem system = plane->system(it.next().toInt());
        if (system) {
            result[system->numeralId()] = system;
        }
    }

    return result;
}

// 指定系统下的所有表数据
TableNode::tableMap ICDNavigationUi::tableNodeMap(QStandardItem* item,
                                                  PlaneNode::smtPlane plane)
{
    TableNode::tableMap result;
    if (!item || !plane) {
        return result;
    }
    QVector<QString> keys = directChidData(item);
    QVectorIterator<QString> it(keys);
    QString sysCode = parentKey(GlobalDefine::ntSystem);
    SystemNode::smtSystem system = plane->system(sysCode.toInt());
    if (system) {
        while (it.hasNext()) {
            TableNode::smtTable icdTable = system->table(it.next().toStdString());
            if (icdTable) {
                result[icdTable->key()] = icdTable;
            }
        }
    }

    return result;
}

// 指定表的所有规则数据
TableNode::smtTable ICDNavigationUi::ruleNodeMap(QStandardItem* item,
                                                 PlaneNode::smtPlane plane)
{
    TableNode::smtTable result;
    if (!item || !plane) {
        return result;
    }
    QString sysCode = parentKey(GlobalDefine::ntSystem);
    if (!sysCode.isEmpty()) {
        SystemNode::smtSystem system = plane->system(sysCode.toInt());
        if (system) {
            QString tableCode = parentKey(GlobalDefine::ntTable);
            result = system->table(tableCode.toStdString());
        }
    }
    return result;
}

// 复合数据下的具体规则
TableNode::smtTable ICDNavigationUi::subRuleNodeMap(QStandardItem* item,
                                                    PlaneNode::smtPlane plane)
{
    TableNode::smtTable result;
    if (!item || !plane) {
        return result;
    }
    QString keys = queryNodeKeys(item);
    QStringList keyLst = keys.split("/");
    keyLst.takeFirst(); // 机型
    QString sysCode = keyLst.takeFirst();   // 系统
    if (!sysCode.isEmpty()) {
        SystemNode::smtSystem system = plane->system(sysCode.toInt());
        if (system) {
            QString tableCode = keyLst.takeFirst();
            if (result = system->table(tableCode.toStdString())) {
                result = result->subTable(keyLst.join("/").toStdString());
            }
        }
    }
    return result;
}

// 单条规则数据
ICDMetaData::smtMeta ICDNavigationUi::ruleNodeData(QStandardItem *item,
                                                   PlaneNode::smtPlane plane)
{
    ICDMetaData::smtMeta result;
    if (!item || !plane) {
        return result;
    }
    QString keys = queryNodeKeys(item);
    QStringList keyLst = keys.split("/");
    keyLst.takeFirst(); // 机型
    QString sysCode = keyLst.takeFirst();   // 系统
    if (!sysCode.isEmpty()) {
        SystemNode::smtSystem system = plane->system(sysCode.toInt());
        if (system) {
            QString tableCode = keyLst.takeFirst();
            TableNode::smtTable table
                    = system->table(tableCode.toStdString());
            if (table) {
                QString lastKey = keyLst.takeLast();
                int serial = lastKey.toInt();
                if (lastKey.startsWith("ICDTable_")) {
                    serial = keyLst.takeLast().toInt();
                }
                if (keyLst.size() > 1) {
                    table = table->subTable(keyLst.join("/").toStdString());
                }
                if (table) {
                    result = table->rule(serial);
                }
            }
        }
    }
    return result;
}

// 查询节点
QStandardItem *ICDNavigationUi::findItem(int role,
                                         const QVariant& value,
                                         QStandardItem* parent,
                                         bool direct) const
{
    if (!q_treeView) {
        return NULL;
    }
    QStandardItem *root = parent;
    if (!root) {
        root = q_treeView->invisibleRootItem();
    }
    int index(0);
    QStandardItem *item = root->child(index);
    while (item) {
        QVariant key = item->data(role);
        if (key == value) {
            break;
        }
        if (!direct) {
            if (item->rowCount() > 0) {
                if (item = findItem(role, value, item)) {
                    break;
                }
            }
        }
        item = root->child(++index);
    }
    return item;
}

// 更新已加载规则表状态
void ICDNavigationUi::updateLoadedFlag(QStandardItem *item, int state)
{
    if (!item) {
        return;
    }
    // 表节点单独处理
    if (item->data(LevelIndex).toInt() == GlobalDefine::ntTable) {
        if (optUnload == state) {
            item->setData(GlobalDefine::noneState, ItemLoaded);
            item->setIcon(QIcon(":/datastudio/image/global/circle-gray.png"));
        } else if (optLoad == state) {
            item->setData(GlobalDefine::wholeState, ItemLoaded);
            item->setIcon(QIcon(":/datastudio/image/global/circle-green.png"));
        }
        updateParentFlag(item->parent());
    } else {
        if (optUnload == state) {
            item->setData(GlobalDefine::noneState, ItemLoaded);
        } else if (optLoad == state) {
            item->setData(GlobalDefine::wholeState, ItemLoaded);
        }
        updateParentFlag(item->parent());
        updateChildFlag(item, state);
    }
}

// 更新父节点已加载规则状态
void ICDNavigationUi::updateParentFlag(QStandardItem *item)
{
    if (!item) {
        return;
    }
    int index = 0;
    int state = 0;
    int loaded = 0;     // 已加载
    int unloaded = 0;   // 未加载
    int newState = GlobalDefine::noneState;
    int oldState = item->data(ItemLoaded).toInt();
    QStandardItem *childItem = 0;
    // 遍历子节点，判定状态
    while (childItem = item->child(index++)) {
        state = childItem->data(ItemLoaded).toInt();
        if (GlobalDefine::partiallyState == state) {
            newState = GlobalDefine::partiallyState;
            break;
        } else if (GlobalDefine::wholeState == state) {
            loaded += 1;
        } else {
            unloaded += 1;
        }
    }
    // 如果新状态与原始状态一致，则不必再更新父节点
    if (GlobalDefine::partiallyState != newState) {
        if (0 == loaded) {
            newState = GlobalDefine::noneState;
        } else {
            if (0 == unloaded) {
                newState = GlobalDefine::wholeState;
            } else {
                newState = GlobalDefine::partiallyState;
            }
        }
    }
    // 更新本节点状态
    item->setData(newState, ItemLoaded);
    // 新状态与原始状态不一致，更新父节点
    if (oldState != newState) {
        updateParentFlag(item->parent());
    }
}

// 更新子节点已加载规则状态
void ICDNavigationUi::updateChildFlag(QStandardItem *item, int state)
{
    if (!item) {
        return;
    }
    int index = 0;
    int level = 0;
    QStandardItem *childItem = 0;
    while (childItem = item->child(index++)) {
        level = childItem->data(LevelIndex).toInt();
        if (level > GlobalDefine::ntTable) {
            break;
        } else if (level == GlobalDefine::ntTable) {
            // 表节点单独处理
            if (optUnload == state) {
                childItem->setData(GlobalDefine::noneState, ItemLoaded);
                childItem->setIcon(QIcon(":/datastudio/image/global/circle-gray.png"));
            } else if (optLoad == state) {
                childItem->setData(GlobalDefine::wholeState, ItemLoaded);
                childItem->setIcon(QIcon(":/datastudio/image/global/circle-green.png"));
            }
            else {
                break;
            }
        } else {
            if (optUnload == state) {
                // 更新子节点
                updateChildFlag(childItem, state);
                // 更新本节点
                childItem->setData(GlobalDefine::noneState, ItemLoaded);
            } else if (optLoad == state) {
                // 更新子节点
                updateChildFlag(childItem, state);
                // 更新本节点
                childItem->setData(GlobalDefine::wholeState, ItemLoaded);
            } else {
                break;
            }
        }
    }
}

// 更新节点数据更新状态
void ICDNavigationUi::updateDataState(QStandardItem *item, int state)
{
    if (!item) {
        return;
    }
    if (StateReset == state) {
        if (item->data(DataChanged).toInt() != GlobalDefine::noneState) {
            item->setData(GlobalDefine::noneState, DataChanged);
            // 父节点
            updateParentState(item->parent(), StateChanged);
            // 子节点到表一级
            updateChildState(item, state);
        }
    } else if (StateChanged == state) {
        // 到表一级
        while (item) {
            if (item->data(LevelIndex).toInt() >= GlobalDefine::ntRule) {
                item = item->parent();
            } else {
                break;
            }
        }
        item->setData(GlobalDefine::wholeState, DataChanged);
        // 更新父节点状态
        updateParentState(item->parent(), state);
    }
}

// 更新父节点数据更新状态
void ICDNavigationUi::updateParentState(QStandardItem *item, int state)
{
    if (!item) {
        return;
    }
    if (StateReset == state) {
        if (item->data(DataChanged).toInt() != GlobalDefine::noneState) {
            item->setData(GlobalDefine::noneState, DataChanged);
            updateParentState(item->parent(), state);
        }
    } else if (StateChanged == state) {
        int index = 0;
        int tmpState = 0;
        int changed = 0;     // 已变更
        int unchanged = 0;   // 未变更
        int newState = GlobalDefine::noneState;
        int oldState = item->data(DataChanged).toInt();
        QStandardItem *childItem = 0;
        // 遍历子节点，判定状态
        while (childItem = item->child(index++)) {
            tmpState = childItem->data(DataChanged).toInt();
            if (GlobalDefine::partiallyState == tmpState) {
                newState = GlobalDefine::partiallyState;
                break;
            } else if (GlobalDefine::wholeState == tmpState) {
                changed += 1;
            } else {
                unchanged += 1;
            }
        }
        // 如果新状态与原始状态一致，则不必再更新父节点
        if (GlobalDefine::partiallyState != newState) {
            if (0 == changed) {
                newState = GlobalDefine::noneState;
            } else {
                if (0 == unchanged) {
                    newState = GlobalDefine::wholeState;
                } else {
                    newState = GlobalDefine::partiallyState;
                }
            }
        }
        // 更新本节点状态
        item->setData(newState, DataChanged);
        // 新状态与原始状态不一致，更新父节点
        if (oldState != newState) {
            updateParentState(item->parent(), state);
        }
    }
}

// 更新子节点数据更新状态
void ICDNavigationUi::updateChildState(QStandardItem *item, int state)
{
    // 到表一级
    if (!item || item->data(LevelIndex).toInt() > GlobalDefine::ntTable) {
        return;
    }
    if (StateReset == state) {
        int index = 0;
        QStandardItem *childItem = 0;
        // 子节点
        while (childItem = item->child(index++)) {
            if (childItem->data(DataChanged).toInt() != GlobalDefine::noneState) {
                childItem->setData(GlobalDefine::noneState, DataChanged);
                //
                updateChildState(childItem, state);
            }
        }
    }
}

// 更新主视图数据
void ICDNavigationUi::updateMainView(QStandardItem *current)
{
    if (!current) {
        return;
    }
    PlaneNode::smtPlane plane;
    // 查询内存数据
    int planeID = queryNodeKeys(current).split("/").first().toInt();

    QVariantList args;
    args.append(qVariantFromValue((void*)&plane));
    args.append(planeID);
    jnotify->send("edit.querySinglePlane", args);

    void *data(0);
    std::vector<int> rule;
    ICDElement::smtElement element;
    std::vector<ICDElement::smtElement> elements;
    // 获取当前节点的直属子节点数据
    int level = current->data(LevelIndex).toInt();
    switch (level) {
    case GlobalDefine::ntUnknown: { // 根节点
        // 获取机型基本信息
        PlaneNode::planeVector planes = planeNodeMap();
        const int count = planes.size();
        for (int i = 0; i < count; ++i) {
            elements.push_back(planes[i]);
        }
        data = &elements;
    }
        break;
    case GlobalDefine::ntPlane: {   // 机型节点
        queryNodeData("", element);
        data = &element;
    }
        break;
    case GlobalDefine::ntSystem: {  // 系统节点
        // 查询节点数据
        queryNodeData("", element);
        data = &element;
    }
        break;
    case GlobalDefine::ntTable: {   // ICD表节点
        queryNodeData("", element);
        data = &element;
    }
        break;
    case GlobalDefine::ntRule: {   // 规则节点
        QVariant variant = current->data(SubTable);
        queryNodeData("", element);
        if (variant.isValid()) {    // 复合数据
            rule.push_back(GlobalDefine::dtComplex);
        } else {
            rule.push_back(current->data(RuleDefine).toInt());
        }
        rule.push_back((int)&element);
        data = &rule;
    }
        break;
    default:
        break;
    }
    if (!data) {
        return;
    }

    // 更新主视图显示
    args.clear();
    args.append(qVariantFromValue((void*)data));
    args.append(level);
    jnotify->send("edit.showInfrastructure", args);
}

// 保存机型数据
bool ICDNavigationUi::updatePalneData(stPlane &data)
{
    bool result = false;
    QStandardItem *current = q_treeView->currentItem();
    // 保存内存
    QVariantList args;
    args.append(qVariantFromValue((void*)&data));
    args.append(qVariantFromValue((void*)&result));
    jnotify->send("edit.savePlane", args);

    if (result) {
        // 更新树节点
        QStandardItem *item = findItem(UserKey, data.nCode, current);
        if (item) { // 更新节点
            item->setText(QString(data.sName.c_str()));
        } else { // 插入节点
            item = new QStandardItem();
            item->setText(QString(data.sName.c_str()));
            item->setData(data.nCode, UserKey);
            item->setData(GlobalDefine::noneState, ItemLoaded);
            item->setData(GlobalDefine::ntPlane, LevelIndex);
            current->appendRow(item);
            // 更新节点状态
            updateLoadedFlag(item, optLoad);
        }
        // 更新数据状态标识
        updateDataState(item, StateChanged);
        // 重新刷新界面数据
        //slotCurrentChanged(current, NULL);
    }

    return result;
}

// 保存系统数据
bool ICDNavigationUi::updateSystemData(stSystem &data)
{
    bool result = false;
    QStandardItem *current = q_treeView->currentItem();
    // 保存内存
    QString planeID = parentKey(GlobalDefine::ntPlane);
    std::vector<void*> params;
    std::vector<stSystem> systems(1, data);
    params.push_back(&planeID);
    params.push_back(&systems);

    QVariantList args;
    args.append(qVariantFromValue((void*)&params));
    args.append(qVariantFromValue((void*)&result));
    jnotify->send("edit.saveSystem", args);
    if (result) {
        // 更新树节点
        QStandardItem *item = findItem(UserKey, data.nCode, current);
        if (item) { // 更新节点
            item->setText(QString(data.sName.c_str()));
        } else { // 插入节点
            item = new QStandardItem();
            item->setText(QString(data.sName.c_str()));
            item->setData(data.nCode, UserKey);
            item->setData(GlobalDefine::noneState, ItemLoaded);
            item->setData(GlobalDefine::ntSystem, LevelIndex);
            current->appendRow(item);
            q_treeView->expandItem(current);
            // 更新节点状态
            updateLoadedFlag(item, optLoad);
        }
        // 更新数据状态标识
        updateDataState(item, StateChanged);
        // 重新刷新界面数据
        //slotCurrentChanged(current, NULL);
    }

    return result;
}

// 保存ICD表数据
bool ICDNavigationUi::updateICDData(stICDBase &data)
{
    bool result = false;
    QStandardItem *current = q_treeView->currentItem();
    QString keys = queryNodeKeys(current);
    if (data.sGroup.empty()) {
        data.sGroup = keys.toStdString();
    }
    if (data.sID.empty()) {
        data.sID = data.sName.substr(data.sName.find("_") + strlen("_"));
    }
    std::vector<void*> params;
    std::vector<stICDBase> tables(1, data);
    params.push_back(&keys);
    params.push_back(&tables);
    // 保存内存
    QVariantList args;
    args.append(qVariantFromValue((void*)&params));
    args.append(qVariantFromValue((void*)&result));
    jnotify->send("edit.saveTable", args);

    if (result) {
        // 更新树节点
        QStandardItem *item = findItem(UserKey,
                                       data.sName.c_str(), current);
        if (item) { // 更新节点
            item->setText(QString(data.sDescribe.c_str()));
        } else { // 插入节点
            item = new QStandardItem();
            item->setText(QString(data.sDescribe.c_str()));
            item->setData(data.sName.c_str(), UserKey);
            item->setData(GlobalDefine::wholeState, ItemLoaded);
            item->setData(GlobalDefine::ntTable, LevelIndex);
            current->appendRow(item);
            q_treeView->expandItem(current);
            // 更新节点状态
            updateLoadedFlag(item, optLoad);
        }
        // 更新数据状态标识
        updateDataState(item, StateChanged);
        // 重新刷新界面
        //slotCurrentChanged(current, NULL);
    }

    return result;
}

// 保存ICD规则数据
bool ICDNavigationUi::updateRuleData(ICDMetaData::smtMeta &data, bool top)
{
    int result = 0;
    if (!data) {
        return false;
    }
    QStandardItem *current = q_treeView->currentItem();
    ICDMetaData::smtMeta meta = data;
    QString keys = queryNodeKeys(current);

    int offset = -1;
    if (!top) {
        ICDElement::smtElement element;
        QString pID = keys.left(keys.lastIndexOf("/"));

        QVariantList args;
        args.append(qVariantFromValue((void*)&element));
        args.append(qVariantFromValue((void*)&pID));
        jnotify->send("edit.querySingleElement", args);

        ICDMetaData::smtMeta metaParent = SMT_CONVERT(ICDMetaData, element);
        if (metaParent) {
            offset = metaParent->index();
        }
    }
    std::vector<void*> params;
    ICDComplexData::smtComplex complex = SMT_CONVERT(ICDComplexData, meta);
    if (IcdDefine::icdComplex == meta->metaType()) {
        QStringList keyLst = keys.split("/", QString::SkipEmptyParts);
        int planeID = keyLst.takeFirst().toInt();
        int systemID = keyLst.takeFirst().toInt();
        std::string parentName = keyLst.last().toStdString();
        // 查询内存数据
        PlaneNode::smtPlane plane;
        QStandardItem *current = q_treeView->currentItem();

        QVariantList args;
        args.append(qVariantFromValue((void*)&plane));
        args.append(planeID);
        jnotify->send("edit.querySinglePlane", args);

        TableNode::smtTable oldTable = ruleNodeMap(current, plane);
        if (!oldTable) {
            return false;
        }
        keyLst.takeFirst(); // root parent
        if (keyLst.size() > 0) {    // 位于复合数据层级
            oldTable = oldTable->subTable(keyLst.join("/").toStdString());
            if (!oldTable) {
                return false;
            }
        }
        ICDComplexData::smtComplex oldData
                = SMT_CONVERT(ICDComplexData, oldTable->rule(complex->serial()));
        if (oldData) {
            ICDComplexData::smtComplex newData
                    = SMT_CONVERT(ICDComplexData, oldData->clone());
            if (GlobalDefine::dtComplex == meta->type()) {
                // 更新子表数据
                stICDBase base;
                TableNode::smtTable table = newData->table(newData->rule());
                if (!table) {
                    base.sID = QUuid::createUuid()
                            .toString().remove(QRegExp("[{}-]")).toStdString();
                    base.sName.append("ICDTable_").append(base.sID);
                    base.sCode = complex->proCode();
                    base.nLength = complex->length();
                    base.sParentName = parentName;
                    base.sGroup = QString("%1/%2").arg(planeID)
                            .arg(systemID).toStdString();
                    base.sDescribe = complex->name();
                    base.sRemark = complex->remark();
                    TableNode::smtTable table(new TableNode(base));
                    newData->addTable(table);
                } else {
                    base = table->icdBase();
                    base.sCode = complex->proCode();
                    base.nLength = complex->length();
                    base.sDescribe = complex->name();
                    base.sRemark = complex->remark();
                    table->setICDBase(base);
                }
            } else if (GlobalDefine::dtDiscern == meta->type()) {
                // 更新子表数据
                TableNode::tableVector tables = newData->allTable();
                const int count = tables.size();
                TableNode::smtTable table = 0;
                for (int i = 0; i < count; ++i) {
                    if (table = tables[i]) {
                        stICDBase base = table->icdBase();
                        base.nLength = complex->length();
                        table->setICDBase(base);
                    }
                }
            }
            newData->setName(complex->name());
            newData->setProCode(complex->proCode());
            newData->setOffset(complex->offset());
            newData->setRemark(complex->remark());
            meta = newData;
        } else {
            if (GlobalDefine::dtComplex == meta->type()) {  // 复合数据
                stICDBase base;
                base.sID = QUuid::createUuid()
                        .toString().remove(QRegExp("[{}-]")).toStdString();
                base.sName.append("ICDTable_").append(base.sID);
                base.sCode = complex->proCode();
                base.nLength = complex->length();
                base.sParentName = parentName;
                base.sGroup = QString("%1/%2").arg(planeID)
                        .arg(systemID).toStdString();
                base.sDescribe = complex->name();
                base.sRemark = complex->remark();
                TableNode::smtTable table(new TableNode(base));
                complex->addTable(table);
            }
        }
    }
    params.push_back(&keys);
    params.push_back(&meta);

    QVariantList args;
    args.append(qVariantFromValue((void*)&params));
    args.append(qVariantFromValue((void*)&result));
    jnotify->send("edit.saveRule", args);

    if (result != 0) {
        // 更新树节点
        QString identify = QString("%1").arg(meta->serial());
        if (GlobalDefine::dtComplex == meta->type()) {
            identify.append("/").append(complex->rule().c_str());
        }
        QStandardItem *item = findItem(UserKey, identify, current);
        //        QString text = QString("%1[<font color=darkgreen>%2</font>]")
        //                .arg(meta->name().c_str()).arg(stringDataType(meta->type()));
        QString text = offsetString(meta, offset);
        if (item) { // 更新节点
            item->setText(text);
        } else { // 插入节点
            item = new QStandardItem(text);
            item->setData(identify, UserKey);
            item->setData(GlobalDefine::ntRule, LevelIndex);
            item->setData(meta->type(), RuleDefine);
            if (GlobalDefine::dtComplex == meta->type()) {
                item->setData(complex->rule().c_str(), SubTable);
                item->setIcon(QIcon(":/icdwidget/image/tree/icon-complex.png"));
            } else if (GlobalDefine::dtDiscern == meta->type()) {
                item->setIcon(QIcon(":/icdwidget/image/tree/icon-frame.png"));
            } else {
                item->setIcon(QIcon(":/icdwidget/image/tree/icon-item.png"));
            }
            current->appendRow(item);
            q_treeView->expandItem(current);
        }
        // 更新数据状态标识
        updateDataState(item, StateChanged);
        // 重新刷新界面数据
        //slotCurrentChanged(current, NULL);
        if (GlobalDefine::special == result) {
            updateTopTableTip();
        }
    }

    return true;
}

// 保存ICD规则数据
bool ICDNavigationUi::updateDetailRuleData(ICDMetaData::smtMeta &data)
{
    int result = 0;
    QStandardItem *current = q_treeView->currentItem();
    if (!data || !current) {
        return result;
    }
    std::vector<void*> params;
    QString keys = queryNodeKeys(current);
    ICDMetaData::smtMeta meta = data;
    // 从ID集中去掉本节点id
    keys.truncate(keys.lastIndexOf("/"));
    params.push_back(&keys);
    params.push_back(&meta);

    QVariantList args;
    args.append(qVariantFromValue((void*)&params));
    args.append(qVariantFromValue((void*)&result));
    jnotify->send("edit.saveRule", args);

    if (result != 0) {
        // 更新树节点
        //        QString text = QString("%1[<font color=darkgreen>%2</font>]")
        //                .arg(meta->name().c_str()).arg(stringDataType(meta->type()));
        QString text;
        ICDElement::smtElement element;
        QString pID = keys.left(keys.lastIndexOf("/"));

        args.clear();
        args.append(qVariantFromValue((void*)&element));
        args.append(qVariantFromValue((void*)&pID));
        jnotify->send("edit.querySingleElement", args);

        ICDMetaData::smtMeta metaParent = SMT_CONVERT(ICDMetaData, element);
        if (metaParent) {
            text = offsetString(meta, metaParent->index());
        } else {
            text = offsetString(meta);
        }
        // 更新节点
        current->setText(text);
        // 更新数据状态标识
        updateDataState(current, StateChanged);
        if (GlobalDefine::special == result) {
            updateTopTableTip();
        }
    }

    return result;
}

// 保存子表数据
bool ICDNavigationUi::updateSubTableData(stICDBase &data)
{
    int result = 0;
    QStandardItem *current = q_treeView->currentItem();
    QString keys = queryNodeKeys(current);
    QStringList keyLst = keys.split("/", QString::SkipEmptyParts);
    int planeID = keyLst.takeFirst().toInt();
    int systemID = keyLst.takeFirst().toInt();
    keyLst.takeLast();
    if (data.sGroup.empty()) {
        data.sGroup = QString("%1/%2")
                .arg(planeID).arg(systemID).toStdString();
    }
    if (data.sID.empty()) {
        data.sID = QUuid::createUuid()
                .toString().remove(QRegExp("[{}-]")).toStdString();
    }
    if (data.sParentName.empty()) {
        data.sParentName = keyLst.last().toStdString();
    }
    // 查询内存数据
    PlaneNode::smtPlane plane;

    QVariantList args;
    args.append(qVariantFromValue((void*)&plane));
    args.append(planeID);
    jnotify->send("edit.querySinglePlane", args);

    ICDComplexData::smtComplex complex
            = SMT_CONVERT(ICDComplexData, ruleNodeData(current, plane));
    if (!complex) {
        return result;
    }
    ICDComplexData::smtComplex smtData
            = SMT_CONVERT(ICDComplexData, complex->clone());
    TableNode::smtTable table;
    data.nLength = complex->length();
    if (table = smtData->table(data.sName)) {
        table->setICDBase(data);
    } else {
        smtData->addTable(TableNode::smtTable(new TableNode(data)));
    }
    keys = keys.left(keys.lastIndexOf("/"));
    //keys.append("/").append(value.sName.c_str());

    std::vector<void*> params;
    params.push_back(&keys);
    params.push_back(&smtData);

    args.clear();
    args.append(qVariantFromValue((void*)&params));
    args.append(qVariantFromValue((void*)&result));
    jnotify->send("edit.saveRule", args);

    if (result != 0) {
        // 更新树节点
        QStandardItem *item = findItem(UserKey, data.sName.c_str(), current);
        if (item) { // 更新节点
            item->setText(data.sDescribe.c_str());
        } else { // 插入节点
            item = new QStandardItem();
            item->setText(QString(data.sDescribe.c_str()));
            item->setData(data.sName.c_str(), UserKey);
            item->setData(GlobalDefine::ntRule, LevelIndex);
            item->setData(GlobalDefine::dtComplex, RuleDefine);
            item->setData(data.sName.c_str(), SubTable);
            current->appendRow(item);
            q_treeView->expandItem(current);
        }
        // 更新数据状态标识
        updateDataState(item, StateChanged);
    }

    return result;
}

// 将数据类型转换成对应的英文字符串
QString ICDNavigationUi::stringDataType(int type) const
{
    QString result;
    switch (type) {
    case GlobalDefine::dtHead:      // 包头
        result = "HEAD";
        break;
    case GlobalDefine::dtCounter:   // 帧计数
        result = "COUNTER";
        break;
    case GlobalDefine::dtCheck:    // 校验
        result = "CHECK";
        break;
    case GlobalDefine::dtFrameCode: // 帧识别码
        result = "FRAMECODE";
        break;
    case GlobalDefine::dtU8:        // 无符号8位
        result = "U8";
        break;
    case GlobalDefine::dt8:         // 有符号8位
        result = "I8";
        break;
    case GlobalDefine::dtU16:       // 无符号16位
        result = "U16";
        break;
    case GlobalDefine::dt16:        // 有符号16位
        result = "I16";
        break;
    case GlobalDefine::dtU32:       // 无符号32位
        result = "U32";
        break;
    case GlobalDefine::dt32:        // 有符号32位
        result = "I32";
        break;
    case GlobalDefine::dtF32:       // 32位浮点数
        result = "F32";
        break;
    case GlobalDefine::dtF64:       // 64位浮点数
        result = "F64";
        break;
    case GlobalDefine::dtBitMap:    // 比特映射
        result = "BITMAP";
        break;
    case GlobalDefine::dtBitValue:  // 比特值
        result = "BITVALUE";
        break;
    case GlobalDefine::dtComplex:   // 复合数据
        result = "COMPLEX";
        break;
    case GlobalDefine::dtDiscern:   // 帧数据
        result = "FRAME";
        break;
    case GlobalDefine::dtBuffer:    // 数据预留区
        result = "BUFFER";
        break;
    default:
        result = "INVALID";
        break;
    }

    return result;
}

// 构造数据偏移量字符串
QString ICDNavigationUi::offsetString(const ICDElement::smtElement &element, int offset) const
{
    QString result;
    if (!element) {
        return result;
    }

    switch (element->objectType()) {
    case GlobalDefine::ntTable:
    {
        TableNode::smtTable table = SMT_CONVERT(TableNode, element);
        if (!table) {
            break;
        }
        break;
    }
    case GlobalDefine::ntRule:
    {
        ICDMetaData::smtMeta meta = SMT_CONVERT(ICDMetaData, element);
        if (!meta) {
            break;
        }

        if (meta->metaType() == IcdDefine::icdBit) {
            ICDBitData::smtBit bit = SMT_CONVERT(ICDBitData, meta);
            if (!bit) {
                break;
            }
            if (offset >= 0) {
                result = QString("<font color=darkgreen size=2>[%1:%2:%3(%4)]</font>"
                                 "%5<font color=darkgreen size=2>[%6]</font>")
                        .arg(meta->serial(), 4, 10, QChar('0'))
                        .arg(offset + meta->index(), 4, 10, QChar('0'))
                        .arg(meta->index(), 4, 10, QChar('0'))
                        .arg(bit->start(), 2, 10, QChar('0'))
                        .arg(meta->name().c_str()).arg(stringDataType(meta->type()));
            } else {
                result = QString("<font color=darkgreen size=2>[%1:%2(%3)]</font>"
                                 "%4<font color=darkgreen size=2>[%5</font>")
                        .arg(meta->serial(), 4, 10, QChar('0'))
                        .arg(meta->index(), 4, 10, QChar('0'))
                        .arg(bit->start(), 2, 10, QChar('0'))
                        .arg(meta->name().c_str()).arg(stringDataType(meta->type()));
            }
        } else {
            if (offset >= 0) {
                result = QString("<font color=darkgreen size=2>[%1:%2:%3]</font>"
                                 "%4<font color=darkgreen size=2>[%5]</font>")
                        .arg(meta->serial(), 4, 10, QChar('0'))
                        .arg(offset + meta->index(), 4, 10, QChar('0'))
                        .arg(meta->index(), 4, 10, QChar('0'))
                        .arg(meta->name().c_str()).arg(stringDataType(meta->type()));
            } else {
                result = QString("<font color=darkgreen size=2>[%1:%2]</font>"
                                 "%3<font color=darkgreen size=2>[%4]</font>")
                        .arg(meta->serial(), 4, 10, QChar('0'))
                        .arg(meta->index(), 4, 10, QChar('0'))
                        .arg(meta->name().c_str()).arg(stringDataType(meta->type()));
            }
        }
        break;
    }
    default:
        break;
    }

    return result;
}

// 查询树中指定节点类型的最大值
void ICDNavigationUi::maxID(QStandardItem *parent, int level, QString &id)
{
    if (!parent) {
        return;
    }
    //
    int index = 0;
    int rank = GlobalDefine::ntUnknown;
    QStandardItem *child = NULL;
    QString tmpId;
    while (child = parent->child(index++)) {
        rank = child->data(LevelIndex).toInt();
        if (rank == level) {     // 目标层
            tmpId = child->data(UserKey).toString();
            if (tmpId > id) {
                id = tmpId;
            }
        } else if (rank > level) {
            if (GlobalDefine::ntTable != level) {
                break;  // 只有查询表时才需要遍历规则节点
            } else {
                tmpId = child->data(SubTable).toString();
                if (tmpId > id) {
                    id = tmpId;
                }
            }
        }
        maxID(child, level, id);
    }
}
