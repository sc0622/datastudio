#include "precomp.h"
#include "MainToolBar.h"
#include "../module/prefer/DataSourceDlg.h"

namespace Main {

ToolBar::ToolBar(QWidget *parent)
    : QToolBar(parent)
{
    setObjectName(metaObject()->className());

    bindingNotify();
}

bool ToolBar::init()
{
    return true;
}

void ToolBar::update(int index)
{
    clear();
    bindingNotify();

    switch (index) {
    case 0: updatePrefer(); break;
    case 1: updateEdit(); break;
    case 2: updateMonitor(); break;
    case 3: updateSimulate(); break;
    case 4: updateAnalyse(); break;
    default:
        break;
    }
}

void ToolBar::bindingNotify()
{
    jnotify->un(this);

    jnotify->on("main.toolbar.show", this, [=](JNEvent &event){
        const int currentTab = jnotify->send("main.tab.currentIndex").toInt();
        if (currentTab == 0) {
            return;
        }
        setVisible(event.argument().toBool());
    });
    jnotify->on("main.tab.changed", this, [=](JNEvent &event){
        update(event.argument().toInt());
    });
    jnotify->on("database.config", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() < 2) {
            return;
        }
        const QString module = args.at(0).toString();
        QObject *receiver = jVariantFromVoid<QObject>(args.at(1));
        if (!receiver) {
            return;
        }
        DataSourceDlg dialog(module, receiver, this);
        if (dialog.exec() != QDialog::Accepted) {
            return;
        }
    });
}

// prefer

void ToolBar::updatePrefer()
{
    setVisible(false);
}

// edit

void ToolBar::updateEdit()
{
    setVisible(JMain::instance()->config("global.menubar.toolBarVisible").asBool());

    const Json::Value option = JMain::instance()->option("edit", "option");
    // database
    QAction *actionDatabase = addAction(QIcon(":/datastudio/image/toolbar/database.png"),
                                        tr("Database"));
    connect(actionDatabase, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.database.config");
    });
#ifdef EDIT_OLD
    // data [edit]
    addSeparator();
    addEditItemAction(option["item"]);
    // export
    addSeparator();
    addEditExportAction(option["export"]);
    // tool
    addSeparator();
    addEditToolAction(option["tool"]);
#else
    // view
    addSeparator();
    addEditViewAction(option["tree"]);
    // copy
    QAction *actionCopy = addAction(QIcon(":/datastudio/image/toolbar/copy.png"),
                                    tr("Copy"));
    connect(actionCopy, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.tree.copy");
    });
    // save
    QAction *actionSave = addAction(QIcon(":/datastudio/image/toolbar/save.png"),
                                    tr("Save"));
    connect(actionSave, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.tree.save");
    });
    actionSave->setEnabled(false);
    // saveAs
    QAction *actionSaveAs = addAction(QIcon(":/datastudio/image/toolbar/save-as.png"),
                                      tr("Save as"));
    connect(actionSaveAs, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.tree.saveas");
    });
#endif
    // window
    addSeparator();
    addEditWindowAction(option["window"]);
    addSeparator();
    addEditSettingsAction();
}

void ToolBar::addEditViewAction(const Json::Value &option)
{
    QToolButton *buttonView = new QToolButton(this);
    buttonView->setCheckable(true);
    buttonView->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    buttonView->setIcon(QIcon(":/datastudio/image/toolbar/view.png"));
    buttonView->setText(tr("Show data"));
    buttonView->setPopupMode(QToolButton::MenuButtonPopup);
    addWidget(buttonView);

    QMenu *menuView = new QMenu(buttonView);
    buttonView->setMenu(menuView);

    // offset
    QAction *actionOffset = menuView->addAction(tr("Show offset"));
    actionOffset->setCheckable(true);
    connect(actionOffset, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("edit", "option.tree.showOffset", checked);
        jnotify->send("edit.toolbar.tree.showOffset", checked);
    });
    if (option.isMember("showOffset")) {
        const bool showOffset = option["showOffset"].asBool();
        if (showOffset) {
            actionOffset->setChecked(showOffset);
        } else {
            jnotify->send("edit.toolbar.tree.showOffset", QVariant(false));
        }
    } else {
        actionOffset->setChecked(true);
    }
    // type
    QAction *actionType = menuView->addAction(tr("Show type"));
    actionType->setCheckable(true);
    connect(actionType, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("edit", "option.tree.showType", checked);
        jnotify->send("edit.toolbar.tree.showType", checked);
    });
    if (option.isMember("showType")) {
        const bool showType = option["showType"].asBool();
        if (showType) {
            actionType->setChecked(showType);
        } else {
            jnotify->send("edit.toolbar.tree.showType", QVariant(false));
        }
    } else {
        actionType->setChecked(true);
    }

    //
    connect(buttonView, &QToolButton::clicked, this, [=](bool checked){
        actionOffset->setChecked(checked);
        actionType->setChecked(checked);
    });
}
#ifdef EDIT_OLD
void ToolBar::addEditItemAction(const Json::Value &option)
{
    Q_UNUSED(option);
    // add item
    QAction *actionAddItem = addAction(QIcon(":/datastudio/image/global/add.png"),
                                       tr("Add item"));
    connect(actionAddItem, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.item.add");
    });
    // insert item
    QAction *actionInsertItem = addAction(QIcon(":/datastudio/image/global/insert.png"),
                                          tr("Insert item"));
    connect(actionInsertItem, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.item.insert");
    });
    // up item
    QAction *actionUpItem = addAction(QIcon(":/datastudio/image/global/up.png"),
                                      tr("Up item"));
    connect(actionUpItem, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.item.up");
    });
    // down item
    QAction *actionDownItem = addAction(QIcon(":/datastudio/image/global/down.png"),
                                        tr("Down item"));
    connect(actionDownItem, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.item.down");
    });
    // remove item
    QAction *actionRemoveItem = addAction(QIcon(":/datastudio/image/global/remove.png"),
                                          tr("Remove item"));
    connect(actionRemoveItem, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.item.remove");
    });
    // clean item
    QAction *actionCleanItem = addAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                                         tr("Clean item"));
    connect(actionCleanItem, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.item.clean");
    });
    //
    jnotify->on("edit.toolbar.action.enabled", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() != 2) {
            return;
        }
        const QString name = args[0].toString();
        const bool enabled = args[1].toBool();
        if (name == "add") {
            actionAddItem->setEnabled(enabled);
        } else if (name == "insert") {
            actionInsertItem->setEnabled(enabled);
        } else if (name == "up") {
            actionUpItem->setEnabled(enabled);
        } else if (name == "down") {
            actionDownItem->setEnabled(enabled);
        } else if (name == "remove") {
            actionRemoveItem->setEnabled(enabled);
        } else if (name == "clean") {
            actionCleanItem->setEnabled(enabled);
        }
    });
    jnotify->on("edit.toolbar.action.querystate", this, [=](JNEvent &event){
        const QString name = event.argument().toString();
        if (name == "add") {
            event.setReturnValue(actionAddItem->isEnabled());
        } else if (name == "insert") {
            event.setReturnValue(actionInsertItem->isEnabled());
        } else if (name == "up") {
            event.setReturnValue(actionUpItem->isEnabled());
        } else if (name == "down") {
            event.setReturnValue(actionDownItem->isEnabled());
        } else if (name == "remove") {
            event.setReturnValue(actionRemoveItem->isEnabled());
        } else if (name == "clean") {
            event.setReturnValue(actionCleanItem->isEnabled());
        }
    });
}

void ToolBar::addEditExportAction(const Json::Value &option)
{
    Q_UNUSED(option);
    // export to database
    QAction *actionExportToDB = addAction(QIcon(":/datastudio/image/global/export-db.png"),
                                          tr("Export to DB"));
    connect(actionExportToDB, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.export.db");
    });
    // export to file
    QAction *actionExportToFile = addAction(QIcon(":/datastudio/image/global/export-file.png"),
                                            tr("Export to file"));
    connect(actionExportToFile, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.export.file");
    });
    // save
    QAction *actionSave = addAction(QIcon(":/datastudio/image/toolbar/save.png"),
                                            tr("Save data"));
    connect(actionSave, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.save");
    });
    //
    jnotify->on("edit.toolbar.action.enabled", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() != 2) {
            return;
        }
        const QString name = args[0].toString();
        const bool enabled = args[1].toBool();
        if (name == "db") {
            actionExportToDB->setEnabled(enabled);
        } else if (name == "file") {
            actionExportToFile->setEnabled(enabled);
        } else if (name == "save") {
            actionSave->setEnabled(enabled);
        }
    });
#if 1
    jnotify->on("edit.toolbar.action.querystate", this, [=](JNEvent &event){
        const QString name = event.argument().toString();
        if (name == "db") {
            event.setReturnValue(actionExportToDB->isEnabled());
        } else if (name == "file") {
            event.setReturnValue(actionExportToFile->isEnabled());
        }
    });
#endif
}

void ToolBar::addEditToolAction(const Json::Value &option)
{
    Q_UNUSED(option);
    // generate GUID
    QAction *actionGenGuid = addAction(QIcon(":/datastudio/image/global/id.png"),
                                       tr("Generate GUID"));
    connect(actionGenGuid, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.tool.genguid");
    });
}
#endif
void ToolBar::addEditWindowAction(const Json::Value &option)
{
    Q_UNUSED(option);
    // tree window
    QAction *actionTreeWin = addAction(QIcon(":/datastudio/image/global/tree-node.png"),
                                       tr("Tree window"));
    actionTreeWin->setCheckable(true);
    if (option.isMember("tree")) {
        actionTreeWin->setChecked(option["tree"].asBool());
    } else {
        actionTreeWin->setChecked(true);
    }
    auto notifyTree = [=](bool checked){
        JMain::instance()->setOption("edit", "option.window.tree", checked);
        jnotify->send("edit.toolbar.window.tree", checked);
    };
    connect(actionTreeWin, &QAction::toggled, this, [=](bool checked){
        notifyTree(checked);
    });
    notifyTree(actionTreeWin->isChecked());
    // Settings window
    QAction *actionSettingsWin = addAction(QIcon(":/datastudio/image/global/settings.png"),
                                           tr("Settings window"));
    actionSettingsWin->setCheckable(true);
    if (option.isMember("settings")) {
        actionSettingsWin->setChecked(option["settings"].asBool());
    } else {
        actionSettingsWin->setChecked(true);
    }
    auto notifySettings = [=](bool checked){
        JMain::instance()->setOption("edit", "option.window.settings", checked);
        jnotify->send("edit.toolbar.window.settings", checked);
    };
    connect(actionSettingsWin, &QAction::toggled, this, [=](bool checked){
        notifySettings(checked);
    });
    notifySettings(actionSettingsWin->isChecked());
}

void ToolBar::addEditSettingsAction()
{
    QAction *actinSettings = addAction(QIcon(":/datastudio/image/toolbar/settings.png"),
                                       tr("Settings"));
    connect(actinSettings, &QAction::triggered, this, [=](){
        jnotify->send("edit.toolbar.settings");
    });
}

// monitor

void ToolBar::updateMonitor()
{
    setVisible(JMain::instance()->config("global.menubar.toolBarVisible").asBool());

    const Json::Value option = JMain::instance()->option("monitor", "option");
    // database
    QAction *actionDatabase = addAction(QIcon(":/datastudio/image/toolbar/database.png"),
                                        tr("Database"));
    connect(actionDatabase, &QAction::triggered, this, [=](){
        jnotify->send("monitor.toolbar.database.config");
    });
    addSeparator();
    // view
    addMonitorViewAction(option["tree"]);
    // flush-switch
    addMonitorFlushSwitchAction(option);
    addSeparator();
    // tree
    addMonitorTreeAction(option["tree"]);
    addSeparator();
    // chart
    addMonitorChartAction(option["chart"]);
    addSeparator();
    // window
    addMonitorWindowAction(option["window"]);
    addSeparator();
    addMonitorSettingsAction();
}

void ToolBar::addMonitorViewAction(const Json::Value &option)
{
    QToolButton *buttonView = new QToolButton(this);
    buttonView->setCheckable(true);
    buttonView->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    buttonView->setIcon(QIcon(":/datastudio/image/toolbar/view.png"));
    buttonView->setText(tr("Show data"));
    buttonView->setPopupMode(QToolButton::MenuButtonPopup);
    addWidget(buttonView);

    QMenu *menuView = new QMenu(buttonView);
    buttonView->setMenu(menuView);

    // offset
    QAction *actionOffset = menuView->addAction(tr("Show offset"));
    actionOffset->setCheckable(true);
    connect(actionOffset, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("monitor", "option.tree.showOffset", checked);
        jnotify->send("monitor.toolbar.tree.showOffset", checked);
    });
    if (option.isMember("showOffset")) {
        const bool showOffset = option["showOffset"].asBool();
        if (showOffset) {
            actionOffset->setChecked(showOffset);
        } else {
            jnotify->send("monitor.toolbar.tree.showOffset", QVariant(false));
        }
    } else {
        actionOffset->setChecked(true);
    }
    // type
    QAction *actionType = menuView->addAction(tr("Show type"));
    actionType->setCheckable(true);
    connect(actionType, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("monitor", "option.tree.showType", checked);
        jnotify->send("monitor.toolbar.tree.showType", checked);
    });
    if (option.isMember("showType")) {
        const bool showType = option["showType"].asBool();
        if (showType) {
            actionType->setChecked(showType);
        } else {
            jnotify->send("monitor.toolbar.tree.showType", QVariant(false));
        }
    } else {
        actionType->setChecked(true);
    }
    // orignal value
    QAction *actionOrignal = menuView->addAction(tr("Orignal value"));
    actionOrignal->setCheckable(true);
    QAction *actionNoOrigValue = addMonitorOrigValueRadixAction(actionOrignal, option);
    // real value
    QAction *actionReal = menuView->addAction(tr("Real value"));
    actionReal->setCheckable(true);
    connect(actionReal, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("monitor", "option.tree.showReal", checked);
        jnotify->send("monitor.toolbar.tree.showReal", checked);
    });
    if (option.isMember("showReal")) {
        const bool showReal = option["showReal"].asBool();
        if (showReal) {
            actionReal->setChecked(showReal);
        } else {
            jnotify->send("monitor.toolbar.tree.showReal", QVariant(false));
        }
    } else {
        actionReal->setChecked(true);
    }
    // desc
    QAction *actionDesc = menuView->addAction(tr("Description"));
    actionDesc->setCheckable(true);
    connect(actionDesc, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("monitor", "option.tree.showDesc", checked);
        jnotify->send("monitor.toolbar.tree.showDesc", checked);
    });
    if (option.isMember("showDesc")) {
        const bool showDesc = option["showDesc"].asBool();
        if (showDesc) {
            actionDesc->setChecked(showDesc);
        } else {
            jnotify->send("monitor.toolbar.tree.showDesc", QVariant(false));
        }
    } else {
        actionDesc->setChecked(true);
    }
    //
    connect(buttonView, &QToolButton::toggled, this, [=](bool checked){
        actionOffset->setChecked(checked);
        actionType->setChecked(checked);
        actionNoOrigValue->setChecked(!checked);
        actionReal->setChecked(checked);
        actionDesc->setChecked(checked);
    });
}

QAction *ToolBar::addMonitorOrigValueRadixAction(QAction *action, const Json::Value &option)
{
    if (!action) {
        Q_ASSERT(false);
        return nullptr;
    }

    QMenu *menu = new QMenu(this);
    action->setMenu(menu);

    QActionGroup *actionGroupView = new QActionGroup(menu);
    // hide action
    QAction *actionNoOrigValue = menu->addAction(tr("Hide"));
    actionNoOrigValue->setCheckable(true);
    connect(actionNoOrigValue, &QAction::toggled, this, [=](bool checked){
        if (checked) {
            JMain::instance()->setOption("monitor", "option.tree.showOrignal", 0);
            jnotify->send("monitor.toolbar.tree.showOrignal", QVariantList() << false);
        } else {
            QAction *action = actionGroupView->checkedAction();
            if (action) {
                const int radix = action->data().toInt();
                JMain::instance()->setOption("monitor", "option.tree.showOrignal", radix);
                jnotify->send("monitor.toolbar.tree.showOrignal", QVariantList() << true << radix);
            }
        }
    });
    menu->addSeparator();
    // decimal action
    QAction *actionDecimal = menu->addAction(tr("Decimal integer"));
    actionDecimal->setCheckable(true);
    actionDecimal->setData(10);
    actionDecimal->setActionGroup(actionGroupView);
    connect(actionDecimal, &QAction::toggled, this, [=](bool checked){
        if (checked) {
            actionNoOrigValue->setChecked(false);
            JMain::instance()->setOption("monitor", "option.tree.showOrignal", 10);
            jnotify->send("monitor.toolbar.tree.showOrignal", QVariantList() << true << 10);
        }
    });
    // hexadecimal action
    QAction *actionHexadecimal = menu->addAction(tr("Hexadecimal integer"));
    actionHexadecimal->setCheckable(true);
    actionHexadecimal->setData(16);
    actionHexadecimal->setActionGroup(actionGroupView);
    connect(actionHexadecimal, &QAction::toggled, this, [=](bool checked){
        if (checked) {
            actionNoOrigValue->setChecked(false);
            JMain::instance()->setOption("monitor", "option.tree.showOrignal", 16);
            jnotify->send("monitor.toolbar.tree.showOrignal", QVariantList() << true << 16);
        }
    });
    // binary action
    QAction *actionBinary = menu->addAction(tr("Binary integer"));
    actionBinary->setCheckable(true);
    actionBinary->setData(2);
    actionBinary->setActionGroup(actionGroupView);
    connect(actionBinary, &QAction::toggled, this, [=](bool checked){
        if (checked) {
            actionNoOrigValue->setChecked(false);
            JMain::instance()->setOption("monitor", "option.tree.showOrignal", 2);
            jnotify->send("monitor.toolbar.tree.showOrignal", QVariantList() << true << 2);
        }
    });
    // octal action
    QAction *actionOctal = menu->addAction(tr("Octal integer"));
    actionOctal->setCheckable(true);
    actionOctal->setData(8);
    actionOctal->setActionGroup(actionGroupView);
    connect(actionOctal, &QAction::toggled, this, [=](bool checked){
        if (checked) {
            actionNoOrigValue->setChecked(false);
            JMain::instance()->setOption("monitor", "option.tree.showOrignal", 8);
            jnotify->send("monitor.toolbar.tree.showOrignal", QVariantList() << true << 8);
        }
    });
    //
    if (option.isMember("showOrignal")) {
        const int radix = option["showOrignal"].asInt();
        if (radix <= 1 || radix > 36) {
            actionNoOrigValue->setChecked(true);
        } else {
            actionNoOrigValue->setChecked(false);
            jnotify->send("monitor.toolbar.tree.showData", QVariant(false));
            switch (radix) {
            case 16: actionHexadecimal->setChecked(true); break;
            case 2: actionBinary->setChecked(true); break;
            case 8: actionOctal->setChecked(true); break;
            case 10:
            default: actionDecimal->setChecked(true); break;
            }
            if (radix != 16) {
                jnotify->send("monitor.toolbar.tree.showOrignal", QVariantList() << true << radix);
            }
        }
    } else {
        actionNoOrigValue->setChecked(true);
    }

    return actionNoOrigValue;
}

void ToolBar::addMonitorFlushSwitchAction(const Json::Value &option)
{
    QToolButton *buttonFlush = new QToolButton(this);
    buttonFlush->setCheckable(true);
    buttonFlush->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    buttonFlush->setPopupMode(QToolButton::MenuButtonPopup);
    addWidget(buttonFlush);

    QMenu *menuFlush = new QMenu(buttonFlush);
    buttonFlush->setMenu(menuFlush);

    // period of flush
    QAction *actionPeriodOfFlush = menuFlush->addAction(tr("Period of flush"));
    connect(actionPeriodOfFlush, &QAction::triggered, this, [=](){
        jnotify->send("monitor.toolbar.flushPeriod");
    });

    auto setFlushStatus = [=](bool checked){
        buttonFlush->setIcon(QIcon(QString(":/datastudio/image/toolbar/%1.png")
                                   .arg(checked ? "stop" : "run")));
        buttonFlush->setText(tr("%1 flush").arg(checked ? tr("Stop") : tr("Start")));
        buttonFlush->setChecked(checked);
    };
    connect(buttonFlush, &QToolButton::toggled, this, [=](bool checked){
        setFlushStatus(checked);
        JMain::instance()->setOption("monitor", "option.flushEnabled", checked);
        jnotify->send("monitor.toolbar.flushToggle", checked);
    });

    if (option.isMember("flushEnabled")) {
        const bool checked = option["flushEnabled"].asBool();
        setFlushStatus(checked);
        if (!checked) {
            jnotify->send("monitor.toolbar.flushToggle", QVariant(false));
        }
    } else {
        setFlushStatus(true);
    }
}

void ToolBar::addMonitorTreeAction(const Json::Value &option)
{
    Q_UNUSED(option);
    QAction *actionBindingChannel = addAction(QIcon(":/datastudio/image/toolbar/connect.png"),
                                              tr("Binding channel"));
    connect(actionBindingChannel, &QAction::triggered, this, [=](){
        jnotify->send("monitor.toolbar.tree.channel.binding");
    });
    QAction *actionUnbindingChannel = addAction(QIcon(":/datastudio/image/toolbar/disconnect.png"),
                                                tr("Unbinding channel"));
    connect(actionUnbindingChannel, &QAction::triggered, this, [=](){
        jnotify->send("monitor.toolbar.tree.channel.unbinding");
    });
    QAction *actionExprtStatus = addAction(QIcon(":/datastudio/image/toolbar/export.png"),
                                           tr("Export status"));
    connect(actionExprtStatus, &QAction::triggered, this, [=](){
        jnotify->send("monitor.toolbar.tree.channel.export");
    });
    QAction *actionStartAll = addAction(QIcon(":/datastudio/image/toolbar/run_all.png"),
                                        tr("Start all"));
    connect(actionStartAll, &QAction::triggered, this, [=](){
        jnotify->send("monitor.toolbar.tree.channel.runAll");
    });
    QAction *actionStopAll = addAction(QIcon(":/datastudio/image/toolbar/stop_all.png"),
                                       tr("Stop all"));
    connect(actionStopAll, &QAction::triggered, this, [=](){
        jnotify->send("monitor.toolbar.tree.channel.stopAll");
    });
}

void ToolBar::addMonitorChartAction(const Json::Value &option)
{
    // clean
    QAction *actionClearChart = addAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                                          tr("Clear chart"));
    connect(actionClearChart, &QAction::triggered, this, [=](){
        jnotify->send("monitor.toolbar.chart.clean");
    });
    // column-width-same
    QAction *actionColumnWidthSame = addAction(
                QIcon(":/datastudio/image/toolbar/colwidth-same.png"),
                tr("Same column width"));
    connect(actionColumnWidthSame, &QAction::triggered, this, [=](){
        jnotify->send("monitor.toolbar.chart.columnWidthSame");
    });
    actionColumnWidthSame->trigger();
    // x-label-sync
    QAction *actionXAxisSync = addAction(
                QIcon(":/datastudio/image/toolbar/xlabel_sync.png"),
                tr(" Synchronize x-label"));
    actionXAxisSync->setCheckable(true);
    connect(actionXAxisSync, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("monitor", "option.chart.xAxisSync", checked);
        jnotify->send("monitor.toolbar.chart.xAxisSync", checked);
    });
    if (option.isMember("xAxisSync")) {
        const bool xAxisSync = option["xAxisSync"].asBool();
        if (xAxisSync) {
            actionXAxisSync->setChecked(xAxisSync);
        } else {
            jnotify->send("monitor.toolbar.chart.xAxisSync", QVariant(false));
        }
    } else {
        actionXAxisSync->setChecked(true);
    }
    // show y-label
    QAction *actionYLabel = addAction(
                QIcon(":/datastudio/image/toolbar/ylabel.png"),
                tr(" Show y-label"));
    actionYLabel->setCheckable(true);
    connect(actionYLabel, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("monitor", "option.chart.showYLabel", checked);
        jnotify->send("monitor.toolbar.chart.showYLabel", checked);
    });
    if (option.isMember("showYLabel")) {
        const bool showYLabel = option["showYLabel"].asBool();
        if (showYLabel) {
            actionYLabel->setChecked(showYLabel);
        } else {
            jnotify->send("monitor.toolbar.chart.showYLabel", QVariant(false));
        }
    } else {
        actionYLabel->setChecked(true);
    }
    // show y-align
    QAction *actionYAlign = addAction(
                QIcon(":/datastudio/image/toolbar/align-left.png"),
                tr(" Show y-align"));
    actionYAlign->setCheckable(true);
    connect(actionYAlign, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("monitor", "option.chart.showYAlign", checked);
        jnotify->send("monitor.toolbar.chart.showYAlign", checked);
    });
    if (option.isMember("showYAlign")) {
        const bool showYAlign = option["showYAlign"].asBool();
        if (showYAlign) {
            actionYAlign->setChecked(showYAlign);
        } else {
            jnotify->send("monitor.toolbar.chart.showYAlign", QVariant(false));
        }
    } else {
        actionYAlign->setChecked(true);
    }
    // sync-track
    QAction *actionSyncTrack = addAction(
                QIcon(":/datastudio/image/toolbar/sync-track.png"),
                tr(" Synchronize track"));
    actionSyncTrack->setCheckable(true);
    connect(actionSyncTrack, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("monitor", "option.chart.syncTrack", checked);
        jnotify->send("monitor.toolbar.chart.syncTrack", checked);
    });
    if (option.isMember("syncTrack")) {
        const bool syncTrack = option["syncTrack"].asBool();
        if (syncTrack) {
            actionSyncTrack->setChecked(syncTrack);
        } else {
            jnotify->send("monitor.toolbar.chart.syncTrack", QVariant(false));
        }
    } else {
        actionSyncTrack->setChecked(true);
    }
}

void ToolBar::addMonitorWindowAction(const Json::Value &option)
{
    Q_UNUSED(option);
    // tree window
    QAction *actionTreeWin = addAction(QIcon(":/datastudio/image/global/tree-node.png"),
                                       tr("Tree window"));
    actionTreeWin->setCheckable(true);
    if (option.isMember("tree")) {
        actionTreeWin->setChecked(option["tree"].asBool());
    } else {
        actionTreeWin->setChecked(true);
    }
    auto notifyTree = [=](bool checked){
        JMain::instance()->setOption("monitor", "option.window.tree", checked);
        jnotify->send("monitor.toolbar.window.tree", checked);
    };
    connect(actionTreeWin, &QAction::toggled, this, [=](bool checked){
        notifyTree(checked);
    });
    notifyTree(actionTreeWin->isChecked());
    // Buffer window
    QAction *actionBufferWin = addAction(QIcon(":/datastudio/image/global/buffer-view.png"),
                                         tr("Buffer window"));
    actionBufferWin->setCheckable(true);
    if (option.isMember("buffer")) {
        actionBufferWin->setChecked(option["buffer"].asBool());
    } else {
        actionBufferWin->setChecked(true);
    }
    auto notifyBuffer = [=](bool checked){
        JMain::instance()->setOption("monitor", "option.window.buffer", checked);
        jnotify->send("monitor.toolbar.window.buffer", checked);
    };
    connect(actionBufferWin, &QAction::toggled, this, [=](bool checked){
        notifyBuffer(checked);
    });
    notifyBuffer(actionBufferWin->isChecked());
    // Chart window
    QAction *actionChartWin = addAction(QIcon(":/datastudio/image/global/chart.png"),
                                        tr("Chart window"));
    actionChartWin->setCheckable(true);
    if (option.isMember("chart")) {
        actionChartWin->setChecked(option["chart"].asBool());
    } else {
        actionChartWin->setChecked(true);
    }
    auto notifySettings = [=](bool checked){
        JMain::instance()->setOption("monitor", "option.window.chart", checked);
        jnotify->send("monitor.toolbar.window.chart", checked);
    };
    connect(actionChartWin, &QAction::toggled, this, [=](bool checked){
        notifySettings(checked);
    });
    notifySettings(actionChartWin->isChecked());
}

void ToolBar::addMonitorSettingsAction()
{
    QAction *actinSettings = addAction(QIcon(":/datastudio/image/toolbar/settings.png"),
                                       tr("Settings"));
    connect(actinSettings, &QAction::triggered, this, [=](){
        jnotify->send("monitor.toolbar.settings");
    });
}

// simulate

void ToolBar::updateSimulate()
{
    setVisible(JMain::instance()->config("global.menubar.toolBarVisible").asBool());

    const Json::Value option = JMain::instance()->option("simulate", "option");
    // database
    QAction *actionDatabase = addAction(QIcon(":/datastudio/image/toolbar/database.png"),
                                        tr("Database"));
    connect(actionDatabase, &QAction::triggered, this, [=](){
        jnotify->send("simulate.toolbar.database.config");
    });
    addSeparator();
    // view
    addSimulateViewAction(option["tree"]);
    // flush-switch
    addSimulateFlushSwitchAction(option);
    addSeparator();
    // tree
    addSimulateTreeAction(option["tree"]);
    addSeparator();
    // set
    addSimulateSetAction(option["set"]);
    addSeparator();
    // window
    addSimulateWindowAction(option["window"]);
    addSeparator();
    addSimulateSettingsAction();
}

void ToolBar::addSimulateViewAction(const Json::Value &option)
{
    QToolButton *buttonView = new QToolButton(this);
    buttonView->setCheckable(true);
    buttonView->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    buttonView->setIcon(QIcon(":/datastudio/image/toolbar/view.png"));
    buttonView->setText(tr("Show data"));
    buttonView->setPopupMode(QToolButton::MenuButtonPopup);
    addWidget(buttonView);

    QMenu *menuView = new QMenu(buttonView);
    buttonView->setMenu(menuView);

    // offset
    QAction *actionOffset = menuView->addAction(tr("Show offset"));
    actionOffset->setCheckable(true);
    connect(actionOffset, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("simulate", "option.tree.showOffset", checked);
        jnotify->send("simulate.toolbar.tree.showOffset", checked);
    });
    if (option.isMember("showOffset")) {
        const bool showOffset = option["showOffset"].asBool();
        if (showOffset) {
            actionOffset->setChecked(showOffset);
        } else {
            jnotify->send("simulate.toolbar.tree.showOffset", QVariant(false));
        }
    } else {
        actionOffset->setChecked(true);
    }
    // type
    QAction *actionType = menuView->addAction(tr("Show type"));
    actionType->setCheckable(true);
    connect(actionType, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("simulate", "option.tree.showType", checked);
        jnotify->send("simulate.toolbar.tree.showType", checked);
    });
    if (option.isMember("showType")) {
        const bool showType = option["showType"].asBool();
        if (showType) {
            actionType->setChecked(showType);
        } else {
            jnotify->send("simulate.toolbar.tree.showType", QVariant(false));
        }
    } else {
        actionType->setChecked(true);
    }
    // orignal value
    QAction *actionOrignal = menuView->addAction(tr("Orignal value"));
    actionOrignal->setCheckable(true);
    QAction *actionNoOrigValue = addSimulateOrigValueRadixAction(actionOrignal, option);
    // real value
    QAction *actionReal = menuView->addAction(tr("Real value"));
    actionReal->setCheckable(true);
    connect(actionReal, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("simulate", "option.tree.showReal", checked);
        jnotify->send("simulate.toolbar.tree.showReal", checked);
    });
    if (option.isMember("showReal")) {
        const bool showReal = option["showReal"].asBool();
        if (showReal) {
            actionReal->setChecked(showReal);
        } else {
            jnotify->send("simulate.toolbar.tree.showReal", QVariant(false));
        }
    } else {
        actionReal->setChecked(true);
    }
    // desc
    QAction *actionDesc = menuView->addAction(tr("Description"));
    actionDesc->setCheckable(true);
    connect(actionDesc, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("simulate", "option.tree.showDesc", checked);
        jnotify->send("simulate.toolbar.tree.showDesc", checked);
    });
    if (option.isMember("showDesc")) {
        const bool showDesc = option["showDesc"].asBool();
        if (showDesc) {
            actionDesc->setChecked(showDesc);
        } else {
            jnotify->send("simulate.toolbar.tree.showDesc", QVariant(false));
        }
    } else {
        actionDesc->setChecked(true);
    }
    //
    connect(buttonView, &QToolButton::toggled, this, [=](bool checked){
        actionOffset->setChecked(checked);
        actionType->setChecked(checked);
        actionNoOrigValue->setChecked(!checked);
        actionReal->setChecked(checked);
        actionDesc->setChecked(checked);
    });
}

QAction *ToolBar::addSimulateOrigValueRadixAction(QAction *action, const Json::Value &option)
{
    if (!action) {
        Q_ASSERT(false);
        return nullptr;
    }

    QMenu *menu = new QMenu(this);
    action->setMenu(menu);

    QActionGroup *actionGroupView = new QActionGroup(menu);
    // hide action
    QAction *actionNoOrigValue = menu->addAction(tr("Hide"));
    actionNoOrigValue->setCheckable(true);
    connect(actionNoOrigValue, &QAction::toggled, this, [=](bool checked){
        if (checked) {
            JMain::instance()->setOption("simulate", "option.tree.showOrignal", 0);
            jnotify->send("simulate.toolbar.tree.showOrignal", QVariantList() << false);
        } else {
            QAction *action = actionGroupView->checkedAction();
            if (action) {
                const int radix = action->data().toInt();
                JMain::instance()->setOption("simulate", "option.tree.showOrignal", radix);
                jnotify->send("simulate.toolbar.tree.showOrignal", QVariantList() << true << radix);
            }
        }
    });
    menu->addSeparator();
    // decimal action
    QAction *actionDecimal = menu->addAction(tr("Decimal integer"));
    actionDecimal->setCheckable(true);
    actionDecimal->setData(10);
    actionDecimal->setActionGroup(actionGroupView);
    connect(actionDecimal, &QAction::toggled, this, [=](bool checked){
        if (checked) {
            actionNoOrigValue->setChecked(false);
            JMain::instance()->setOption("simulate", "option.tree.showOrignal", 10);
            jnotify->send("simulate.toolbar.tree.showOrignal", QVariantList() << true << 10);
        }
    });
    // hexadecimal action
    QAction *actionHexadecimal = menu->addAction(tr("Hexadecimal integer"));
    actionHexadecimal->setCheckable(true);
    actionHexadecimal->setData(16);
    actionHexadecimal->setActionGroup(actionGroupView);
    connect(actionHexadecimal, &QAction::toggled, this, [=](bool checked){
        if (checked) {
            actionNoOrigValue->setChecked(false);
            JMain::instance()->setOption("simulate", "option.tree.showOrignal", 16);
            jnotify->send("simulate.toolbar.tree.showOrignal", QVariantList() << true << 16);
        }
    });
    // binary action
    QAction *actionBinary = menu->addAction(tr("Binary integer"));
    actionBinary->setCheckable(true);
    actionBinary->setData(2);
    actionBinary->setActionGroup(actionGroupView);
    connect(actionBinary, &QAction::toggled, this, [=](bool checked){
        if (checked) {
            actionNoOrigValue->setChecked(false);
            JMain::instance()->setOption("simulate", "option.tree.showOrignal", 2);
            jnotify->send("simulate.toolbar.tree.showOrignal", QVariantList() << true << 2);
        }
    });
    // octal action
    QAction *actionOctal = menu->addAction(tr("Octal integer"));
    actionOctal->setCheckable(true);
    actionOctal->setData(8);
    actionOctal->setActionGroup(actionGroupView);
    connect(actionOctal, &QAction::toggled, this, [=](bool checked){
        if (checked) {
            actionNoOrigValue->setChecked(false);
            JMain::instance()->setOption("simulate", "option.tree.showOrignal", 8);
            jnotify->send("simulate.toolbar.tree.showOrignal", QVariantList() << true << 8);
        }
    });
    //
    if (option.isMember("showOrignal")) {
        const int radix = option["showOrignal"].asInt();
        if (radix <= 1 || radix > 36) {
            actionNoOrigValue->setChecked(true);
        } else {
            actionNoOrigValue->setChecked(false);
            jnotify->send("simulate.toolbar.tree.showData", QVariant(false));
            switch (radix) {
            case 16: actionHexadecimal->setChecked(true); break;
            case 2: actionBinary->setChecked(true); break;
            case 8: actionOctal->setChecked(true); break;
            case 10:
            default: actionDecimal->setChecked(true); break;
            }
            if (radix != 16) {
                JMain::instance()->setOption("simulate", "option.tree.showOrignal", radix);
                jnotify->send("simulate.toolbar.tree.showOrignal", QVariantList() << true << radix);
            }
        }
    } else {
        actionNoOrigValue->setChecked(true);
    }

    return actionNoOrigValue;
}

void ToolBar::addSimulateFlushSwitchAction(const Json::Value &option)
{
    QToolButton *buttonFlush = new QToolButton(this);
    buttonFlush->setCheckable(true);
    buttonFlush->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    buttonFlush->setPopupMode(QToolButton::MenuButtonPopup);
    addWidget(buttonFlush);

    QMenu *menuFlush = new QMenu(buttonFlush);
    buttonFlush->setMenu(menuFlush);

    // period of flush
    QAction *actionPeriodOfFlush = menuFlush->addAction(tr("Period of flush"));
    connect(actionPeriodOfFlush, &QAction::triggered, this, [=](){
        jnotify->send("monitor.toolbar.flushPeriod");
    });

    auto setFlushStatus = [=](bool checked){
        buttonFlush->setIcon(QIcon(QString(":/datastudio/image/toolbar/%1.png")
                                   .arg(checked ? "stop" : "run")));
        buttonFlush->setText(tr("%1 flush").arg(checked ? tr("Stop") : tr("Start")));
        buttonFlush->setChecked(checked);
    };
    connect(buttonFlush, &QToolButton::toggled, this, [=](bool checked){
        setFlushStatus(checked);
        JMain::instance()->setOption("simulate", "option.flushEnabled", checked);
        jnotify->send("simulate.toolbar.flushToggle", checked);
    });

    if (option.isMember("flushEnabled")) {
        const bool checked = option["flushEnabled"].asBool();
        setFlushStatus(checked);
        if (!checked) {
            jnotify->send("simulate.toolbar.flushToggle", QVariant(false));
        }
    } else {
        setFlushStatus(true);
    }
}

void ToolBar::addSimulateTreeAction(const Json::Value &option)
{
    Q_UNUSED(option);
    QAction *actionBindingChannel = addAction(QIcon(":/datastudio/image/toolbar/connect.png"),
                                              tr("Binding channel"));
    connect(actionBindingChannel, &QAction::triggered, this, [=](){
        jnotify->send("simulate.toolbar.tree.channel.binding");
    });
    QAction *actionUnbindingChannel = addAction(QIcon(":/datastudio/image/toolbar/disconnect.png"),
                                                tr("Unbinding channel"));
    connect(actionUnbindingChannel, &QAction::triggered, this, [=](){
        jnotify->send("simulate.toolbar.tree.channel.unbinding");
    });
    QAction *actionExprtStatus = addAction(QIcon(":/datastudio/image/toolbar/export.png"),
                                           tr("Export status"));
    connect(actionExprtStatus, &QAction::triggered, this, [=](){
        jnotify->send("simulate.toolbar.tree.channel.export");
    });
    QAction *actionStartAll = addAction(QIcon(":/datastudio/image/toolbar/run_all.png"),
                                        tr("Start all"));
    connect(actionStartAll, &QAction::triggered, this, [=](){
        jnotify->send("simulate.toolbar.tree.channel.runAll");
    });
    QAction *actionStopAll = addAction(QIcon(":/datastudio/image/toolbar/stop_all.png"),
                                       tr("Stop all"));
    connect(actionStopAll, &QAction::triggered, this, [=](){
        jnotify->send("simulate.toolbar.tree.channel.stopAll");
    });
}

void ToolBar::addSimulateSetAction(const Json::Value &option)
{
    Q_UNUSED(option);
    // clean
    QAction *actionClearView = addAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                                         tr("Clear view"));
    connect(actionClearView, &QAction::triggered, this, [=](){
        jnotify->send("simulate.toolbar.set.clean");
    });
}

void ToolBar::addSimulateWindowAction(const Json::Value &option)
{
    Q_UNUSED(option);
    // tree window
    QAction *actionTreeWin = addAction(QIcon(":/datastudio/image/global/tree-node.png"),
                                       tr("Tree window"));
    actionTreeWin->setCheckable(true);
    if (option.isMember("tree")) {
        actionTreeWin->setChecked(option["tree"].asBool());
    } else {
        actionTreeWin->setChecked(true);
    }
    auto notifyTree = [=](bool checked){
        JMain::instance()->setOption("simulate", "option.window.tree", checked);
        jnotify->send("simulate.toolbar.window.tree", checked);
    };
    connect(actionTreeWin, &QAction::toggled, this, [=](bool checked){
        notifyTree(checked);
    });
    notifyTree(actionTreeWin->isChecked());
    // Modify window
    QAction *actionModifyWin = addAction(QIcon(":/datastudio/image/global/edit.png"),
                                         tr("Modify window"));
    actionModifyWin->setCheckable(true);
    if (option.isMember("modify")) {
        actionModifyWin->setChecked(option["modify"].asBool());
    } else {
        actionModifyWin->setChecked(true);
    }
    auto notifyModifyWin = [=](bool checked){
        JMain::instance()->setOption("simulate", "option.window.modify", checked);
        jnotify->send("simulate.toolbar.window.modify", checked);
    };
    connect(actionModifyWin, &QAction::toggled, this, [=](bool checked){
        notifyModifyWin(checked);
    });
    notifyModifyWin(actionModifyWin->isChecked());
}

void ToolBar::addSimulateSettingsAction()
{
    QAction *actionSettings = addAction(QIcon(":/datastudio/image/toolbar/settings.png"),
                                        tr("Settings"));
    connect(actionSettings, &QAction::triggered, this, [=](){
        jnotify->send("simulate.toolbar.settings");
    });
}

// analyse

void ToolBar::updateAnalyse()
{
    setVisible(JMain::instance()->config("global.menubar.toolBarVisible").asBool());

    const Json::Value option = JMain::instance()->option("analyse", "option");

    // database
    QAction *actionDatabase = addAction(QIcon(":/datastudio/image/toolbar/database.png"),
                                        tr("Database"));
    connect(actionDatabase, &QAction::triggered, this, [=](){
        jnotify->send("analyse.toolbar.database.config");
    });
    addSeparator();
    // view
    addAnalyseViewAction(option["tree"]);
    addSeparator();
    // tree
    addAnalyseTreeAction(option["tree"]);
    addSeparator();
    // chart
    addAnalyseChartAction(option["chart"]);
    addSeparator();
    // window
    addAnalyseWindowAction(option["window"]);
    addSeparator();
    addAnalyseSettingsAction();
}

void ToolBar::addAnalyseViewAction(const Json::Value &option)
{
    QToolButton *buttonView = new QToolButton(this);
    buttonView->setCheckable(true);
    buttonView->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    buttonView->setIcon(QIcon(":/datastudio/image/toolbar/view.png"));
    buttonView->setText(tr("Show data"));
    buttonView->setPopupMode(QToolButton::MenuButtonPopup);
    addWidget(buttonView);

    QMenu *menuView = new QMenu(buttonView);
    buttonView->setMenu(menuView);

    // offset
    QAction *actionOffset = menuView->addAction(tr("Show offset"));
    actionOffset->setCheckable(true);
    connect(actionOffset, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("analyse", "option.tree.showOffset", checked);
        jnotify->send("analyse.toolbar.tree.showOffset", checked);
    });
    if (option.isMember("showOffset")) {
        const bool showOffset = option["showOffset"].asBool();
        if (showOffset) {
            actionOffset->setChecked(showOffset);
        } else {
            jnotify->send("analyse.toolbar.tree.showOffset", QVariant(false));
        }
    } else {
        actionOffset->setChecked(true);
    }
    // type
    QAction *actionType = menuView->addAction(tr("Show type"));
    actionType->setCheckable(true);
    connect(actionType, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("analyse", "option.tree.showType", checked);
        jnotify->send("analyse.toolbar.tree.showType", checked);
    });
    if (option.isMember("showType")) {
        const bool showType = option["showType"].asBool();
        if (showType) {
            actionType->setChecked(showType);
        } else {
            jnotify->send("analyse.toolbar.tree.showType", QVariant(false));
        }
    } else {
        actionType->setChecked(true);
    }

    connect(buttonView, &QToolButton::clicked, this, [=](bool checked){
        actionOffset->setChecked(checked);
        actionType->setChecked(checked);
    });
}

void ToolBar::addAnalyseTreeAction(const Json::Value &option)
{
    Q_UNUSED(option);
    // load data
    QAction *actionLoadData = addAction(QIcon(":/datastudio/image/toolbar/load.png"),
                                        tr("Load data"));
    connect(actionLoadData, &QAction::triggered, this, [=](){
        jnotify->send("analyse.toolbar.tree.loadData");
    });
    // unload data
    QAction *actionUnloadData = addAction(QIcon(":/datastudio/image/toolbar/unload.png"),
                                          tr("Unload data"));
    connect(actionUnloadData, &QAction::triggered, this, [=](){
        jnotify->send("analyse.toolbar.tree.unloadData");
    });
    // convert data
    QAction *actionConvertData = addAction(QIcon(":/datastudio/image/toolbar/convert.png"),
                                           tr("Convert data"));
    connect(actionConvertData, &QAction::triggered, this, [=](){
        jnotify->send("analyse.toolbar.convertdata");
    });
}

void ToolBar::addAnalyseChartAction(const Json::Value &option)
{
    // set coordinate
    QAction *actionSetCoordinate = addAction(QIcon(":/datastudio/image/toolbar/coordinate.png"),
                                             tr("Set coordinate"));
    connect(actionSetCoordinate, &QAction::triggered, this, [=](){
        jnotify->send("analyse.toolbar.chart.setCoordinate");
    });
    // update data
    QAction *actionUpdateData = addAction(QIcon(":/datastudio/image/toolbar/update.png"),
                                          tr("Update data"));
    connect(actionUpdateData, &QAction::triggered, this, [=](){
        jnotify->send("analyse.toolbar.chart.update");
    });
    // clear chart
    QAction *actionClearChart = addAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                                          tr("Clear chart"));
    connect(actionClearChart, &QAction::triggered, this, [=](){
        jnotify->send("analyse.toolbar.chart.clean");
    });
    // column-width-same
    QAction *actionColumnWidthSame = addAction(
                QIcon(":/datastudio/image/toolbar/colwidth-same.png"),
                tr("Same column width"));
    connect(actionColumnWidthSame, &QAction::triggered, this, [=](){
        jnotify->send("analyse.toolbar.chart.columnWidthSame");
    });
    actionColumnWidthSame->trigger();
    // x-label-sync
    QAction *actionXAxisSync = addAction(
                QIcon(":/datastudio/image/toolbar/xlabel_sync.png"),
                tr(" Synchronize x-label"));
    actionXAxisSync->setCheckable(true);
    connect(actionXAxisSync, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("analyse", "option.chart.xAxisSync", checked);
        jnotify->send("analyse.toolbar.chart.xAxisSync", checked);
    });
    if (option.isMember("xAxisSync")) {
        const bool xAxisSync = option["xAxisSync"].asBool();
        if (xAxisSync) {
            actionXAxisSync->setChecked(xAxisSync);
        } else {
            jnotify->send("analyse.toolbar.chart.xAxisSync", QVariant(false));
        }
    } else {
        actionXAxisSync->setChecked(true);
    }
    // show y-label
    QAction *actionYLabel = addAction(
                QIcon(":/datastudio/image/toolbar/ylabel.png"),
                tr(" Show y-label"));
    actionYLabel->setCheckable(true);
    connect(actionYLabel, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("analyse", "option.chart.showYLabel", checked);
        jnotify->send("analyse.toolbar.chart.showYLabel", checked);
    });
    if (option.isMember("showYLabel")) {
        const bool showYLabel = option["showYLabel"].asBool();
        if (showYLabel) {
            actionYLabel->setChecked(showYLabel);
        } else {
            jnotify->send("analyse.toolbar.chart.showYLabel", QVariant(false));
        }
    } else {
        actionYLabel->setChecked(true);
    }
    // show y-align
    QAction *actionYAlign = addAction(
                QIcon(":/datastudio/image/toolbar/align-left.png"),
                tr(" Show y-align"));
    actionYAlign->setCheckable(true);
    connect(actionYAlign, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("analyse", "option.chart.showYAlign", checked);
        jnotify->send("analyse.toolbar.chart.showYAlign", checked);
    });
    if (option.isMember("showYAlign")) {
        const bool showYAlign = option["showYAlign"].asBool();
        if (showYAlign) {
            actionYAlign->setChecked(showYAlign);
        } else {
            jnotify->send("analyse.toolbar.chart.showYAlign", QVariant(false));
        }
    } else {
        actionYAlign->setChecked(true);
    }
    // sync-track
    QAction *actionSyncTrack = addAction(
                QIcon(":/datastudio/image/toolbar/sync-track.png"),
                tr(" Synchronize track"));
    actionSyncTrack->setCheckable(true);
    connect(actionSyncTrack, &QAction::toggled, this, [=](bool checked){
        JMain::instance()->setOption("analyse", "option.chart.syncTrack", checked);
        jnotify->send("analyse.toolbar.chart.syncTrack", checked);
    });
    if (option.isMember("syncTrack")) {
        const bool syncTrack = option["syncTrack"].asBool();
        if (syncTrack) {
            actionSyncTrack->setChecked(syncTrack);
        } else {
            jnotify->send("analyse.toolbar.chart.syncTrack", QVariant(false));
        }
    } else {
        actionSyncTrack->setChecked(true);
    }
}

void ToolBar::addAnalyseWindowAction(const Json::Value &option)
{
    Q_UNUSED(option);
    // tree window
    QAction *actionTreeWin = addAction(QIcon(":/datastudio/image/global/tree-node.png"),
                                       tr("Tree window"));
    actionTreeWin->setCheckable(true);
    if (option.isMember("tree")) {
        actionTreeWin->setChecked(option["tree"].asBool());
    } else {
        actionTreeWin->setChecked(true);
    }
    auto notifyTree = [=](bool checked){
        JMain::instance()->setOption("analyse", "option.window.tree", checked);
        jnotify->send("analyse.toolbar.window.tree", checked);
    };
    connect(actionTreeWin, &QAction::toggled, this, [=](bool checked){
        notifyTree(checked);
    });
    notifyTree(actionTreeWin->isChecked());
    // Chart window
    QAction *actionChartWin = addAction(QIcon(":/datastudio/image/global/chart.png"),
                                        tr("Chart window"));
    actionChartWin->setCheckable(true);
    if (option.isMember("chart")) {
        actionChartWin->setChecked(option["chart"].asBool());
    } else {
        actionChartWin->setChecked(true);
    }
    auto notifySettings = [=](bool checked){
        JMain::instance()->setOption("analyse", "option.window.chart", checked);
        jnotify->send("analyse.toolbar.window.chart", checked);
    };
    connect(actionChartWin, &QAction::toggled, this, [=](bool checked){
        notifySettings(checked);
    });
    notifySettings(actionChartWin->isChecked());
}

void ToolBar::addAnalyseSettingsAction()
{
    QAction *actionSettings = addAction(QIcon(":/datastudio/image/toolbar/settings.png"),
                                        tr("Settings"));
    connect(actionSettings, &QAction::triggered, this, [=](){
        jnotify->send("analyse.toolbar.settings");
    });
}

// simulate

// analyse

}
