#include "precomp.h"
#include "MonitorView.h"
#include "MonitorTreeView.h"
#include "MonitorDataView.h"
#include "MonitorChartView.h"
#include "MonitorSettingsDlg.h"

namespace Monitor {

View::View(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    d_toolBar = new QToolBar(this);
    d_toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    vertLayoutMain->addWidget(d_toolBar);

    d_splitterMain = new JSplitter({1, 3}, this);
    d_splitterMain->setObjectName("Monitor::splitterMain");
    vertLayoutMain->addWidget(d_splitterMain);

    d_splitterLeft = new JSplitter({1.5, 1}, Qt::Vertical, this);
    d_splitterLeft->setObjectName("Monitor::splitterLeft");
    d_splitterMain->addWidget(d_splitterLeft);

    d_treeView = new TreeView(this);
    d_splitterLeft->addWidget(d_treeView);

    d_dataView = new DataView(this);
    d_splitterLeft->addWidget(d_dataView);

    d_chartView = new ChartView(this);
    d_splitterMain->addWidget(d_chartView);

    jnotify->on("main.monitor.toolbar.show", this, [=](JNEvent &event){
        d_toolBar->setVisible(event.argument().toBool());
    });
    jnotify->on("main.toolbar.show", this, [=](JNEvent &event){
        d_toolBar->setVisible(event.argument().toBool());
    });
}

View::~View()
{
    JMain::saveWidgetState(d_splitterMain);
    JMain::saveWidgetState(d_splitterLeft);
}

bool View::init()
{
    bool result = true;

    JMain::restoreWidgetState(d_splitterMain);
    JMain::restoreWidgetState(d_splitterLeft);
    result = result && d_treeView->init();
    result = result && d_dataView->init();
    result = result && d_chartView->init();
    updateToolBar();

    return result;
}

void View::updateToolBar()
{
    d_toolBar->clear();

    Json::Value option = JMain::instance()->option("monitor", "option/tree");
    if (option.isNull()) {
        Q_ASSERT(false);
        return;
    }

    // database
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/database.png"),
                         tr("Database"), this, [=](){
        jnotify->send("monitor.toolbar.database.config", d_treeView);
    });
    d_toolBar->addSeparator();
    // view
    addViewAction(option);
    // flush-switch
    addFlushSwitchAction(option);
    d_toolBar->addSeparator();
    // binding channel
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/connect.png"),
                         tr("Binding channel"), this, [=](){
        jnotify->send("monitor.toolbar.tree.channel.binding", d_treeView);
    });
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/disconnect.png"),
                         tr("Unbinding channel"), this, [=](){
        jnotify->send("monitor.toolbar.tree.channel.unbinding", d_treeView);
    });
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/export.png"),
                         tr("Export status"), this, [=](){
        jnotify->send("monitor.toolbar.tree.channel.export", d_treeView);
    });
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/run_all.png"),
                         tr("Start all"), this, [=](){
        jnotify->send("monitor.toolbar.tree.channel.runAll", d_treeView);
    });
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/stop_all.png"),
                         tr("Stop all"), this, [=](){
        jnotify->send("monitor.toolbar.tree.channel.stopAll", d_treeView);
    });
    d_toolBar->addSeparator();
    // clean
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                         tr("Clear chart"), this, [=](){
        jnotify->send("monitor.toolbar.chart.clean", d_chartView);
    });
    // column-width-same
    QAction *actionColumnWidthSame = d_toolBar->addAction(
                QIcon(":/datastudio/image/toolbar/colwidth-same.png"),
                tr("Same column width"), this, [=](){
        jnotify->send("monitor.toolbar.chart.columnWidthSame", d_chartView);
    });
    actionColumnWidthSame->trigger();
    // x-label-sync
    QAction *actionXAxisSync = d_toolBar->addAction(
                QIcon(":/datastudio/image/toolbar/xlabel_sync.png"),
                tr(" Synchronize x-label"));
    actionXAxisSync->setCheckable(true);
    connect(actionXAxisSync, &QAction::toggled, this, [=](bool checked){
        jnotify->send("monitor.toolbar.chart.xAxisSync", d_chartView, checked);
    });
    if (option.isMember("xAxisSync")) {
        const bool xAxisSync = option["xAxisSync"].asBool();
        if (xAxisSync) {
            actionXAxisSync->setChecked(xAxisSync);
        } else {
            d_chartView->setXAxisSync(false);
        }
    } else {
        actionXAxisSync->setChecked(true);
    }
    // show y-label
    QAction *actionYLabel = d_toolBar->addAction(
                QIcon(":/datastudio/image/toolbar/ylabel.png"),
                tr(" Show y-label"));
    actionYLabel->setCheckable(true);
    connect(actionYLabel, &QAction::toggled, this, [=](bool checked){
        jnotify->send("monitor.toolbar.chart.showYLabel", d_chartView, checked);
    });
    if (option.isMember("showYLabel")) {
        const bool showYLabel = option["showYLabel"].asBool();
        if (showYLabel) {
            actionYLabel->setChecked(showYLabel);
        } else {
            d_chartView->showYLabel(false);
        }
    } else {
        actionYLabel->setChecked(true);
    }
    // show y-align
    QAction *actionYAlign = d_toolBar->addAction(
                QIcon(":/datastudio/image/toolbar/align-left.png"),
                tr(" Show y-align"));
    actionYAlign->setCheckable(true);
    connect(actionYAlign, &QAction::toggled, this, [=](bool checked){
        jnotify->send("monitor.toolbar.chart.showYAlign", d_chartView, checked);
    });
    if (option.isMember("showYAlign")) {
        const bool showYAlign = option["showYAlign"].asBool();
        if (showYAlign) {
            actionYAlign->setChecked(showYAlign);
        } else {
            d_chartView->showYAlign(false);
        }
    } else {
        actionYAlign->setChecked(true);
    }
    // sync-track
    QAction *actionSyncTrack = d_toolBar->addAction(
                QIcon(":/datastudio/image/toolbar/sync-track.png"),
                tr(" Synchronize track"));
    actionSyncTrack->setCheckable(true);
    connect(actionSyncTrack, &QAction::toggled, this, [=](bool checked){
        jnotify->send("monitor.toolbar.chart.syncTrack", d_chartView, checked);
    });
    if (option.isMember("syncTrack")) {
        const bool syncTrack = option["syncTrack"].asBool();
        if (syncTrack) {
            actionSyncTrack->setChecked(syncTrack);
        } else {
            d_chartView->setSyncTrack(false);
        }
    } else {
        actionSyncTrack->setChecked(true);
    }

    d_toolBar->addSeparator();
    addSettingsAction();
}

void View::addViewAction(const Json::Value &option)
{
    QToolButton *buttonView = new QToolButton(this);
    buttonView->setCheckable(true);
    buttonView->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    buttonView->setIcon(QIcon(":/datastudio/image/toolbar/view.png"));
    buttonView->setText(tr("Show data"));
    buttonView->setPopupMode(QToolButton::MenuButtonPopup);
    d_toolBar->addWidget(buttonView);

    QMenu *menuView = new QMenu(buttonView);
    buttonView->setMenu(menuView);

    // offset
    QAction *actionOffset = menuView->addAction(tr("Show offset"));
    actionOffset->setCheckable(true);
    connect(actionOffset, &QAction::toggled, this, [=](bool checked){
        jnotify->send("monitor.toolbar.tree.showOffset", d_treeView, checked);
    });
    if (option.isMember("showOffset")) {
        const bool showOffset = option["showOffset"].asBool();
        if (showOffset) {
            actionOffset->setChecked(showOffset);
        } else {
            d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowOffset, false);
        }
    } else {
        actionOffset->setChecked(true);
    }
    // type
    QAction *actionType = menuView->addAction(tr("Show type"));
    actionType->setCheckable(true);
    connect(actionType, &QAction::toggled, this, [=](bool checked){
        jnotify->send("monitor.toolbar.tree.showType", d_treeView, checked);
    });
    if (option.isMember("showType")) {
        const bool showType = option["showType"].asBool();
        if (showType) {
            actionType->setChecked(showType);
        } else {
            d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowType, false);
        }
    } else {
        actionType->setChecked(true);
    }
    // orignal value
    QAction *actionOrignal = menuView->addAction(tr("Orignal value"));
    actionOrignal->setCheckable(true);
    QAction *actionNoOrigValue = addOrigValueRadixAction(actionOrignal, option);
    // real value
    QAction *actionReal = menuView->addAction(tr("Real value"));
    actionReal->setCheckable(true);
    connect(actionReal, &QAction::toggled, this, [=](bool checked){
        jnotify->send("monitor.toolbar.tree.showReal", d_treeView, checked);
    });
    if (option.isMember("showValue")) {
        const bool showValue = option["showValue"].asBool();
        if (showValue) {
            actionReal->setChecked(showValue);
        } else {
            d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowValue, false);
        }
    } else {
        actionReal->setChecked(true);
    }
    // desc
    QAction *actionDesc = menuView->addAction(tr("Description"));
    actionDesc->setCheckable(true);
    connect(actionDesc, &QAction::toggled, this, [=](bool checked){
        jnotify->send("monitor.toolbar.tree.showDesc", d_treeView, checked);
    });
    if (option.isMember("showDesc")) {
        const bool showDesc = option["showDesc"].asBool();
        if (showDesc) {
            actionDesc->setChecked(showDesc);
        } else {
            d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowSpec, false);
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

QAction *View::addOrigValueRadixAction(QAction *action, const Json::Value &option)
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
            jnotify->send("monitor.toolbar.tree.showOrignal", d_treeView, QVariantList{false});
        } else {
            QAction *action = actionGroupView->checkedAction();
            if (action) {
                jnotify->send("monitor.toolbar.tree.showOrignal", d_treeView,
                              QVariantList{true, action->data().toInt()});
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
            jnotify->send("monitor.toolbar.tree.showOrignal", d_treeView, QVariantList{true, 10});
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
            jnotify->send("monitor.toolbar.tree.showOrignal", d_treeView, QVariantList{true, 16});
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
            jnotify->send("monitor.toolbar.tree.showOrignal", d_treeView, QVariantList{true, 2});
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
            jnotify->send("monitor.toolbar.tree.showOrignal", d_treeView, QVariantList{true, 8});
        }
    });
    //
    if (option.isMember("showOrignal")) {
        const int radix = option["showOrignal"].asInt();
        if (radix <= 1 || radix > 36) {
            actionNoOrigValue->setChecked(true);
        } else {
            actionNoOrigValue->setChecked(false);
            d_treeView->setShowAttribute(Icd::CoreTreeWidget::ShowData, false);
            switch (radix) {
            case 16: actionHexadecimal->setChecked(true); break;
            case 2: actionBinary->setChecked(true); break;
            case 8: actionOctal->setChecked(true); break;
            case 10:
            default: actionDecimal->setChecked(true); break;
            }
            if (radix != 16) {
                d_treeView->setOrigValueRadix(radix);
            }
        }
    } else {
        actionNoOrigValue->setChecked(true);
    }

    return actionNoOrigValue;
}

void View::addFlushSwitchAction(const Json::Value &option)
{
    QToolButton *buttonFlush = new QToolButton(this);
    buttonFlush->setCheckable(true);
    buttonFlush->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    buttonFlush->setPopupMode(QToolButton::MenuButtonPopup);
    d_toolBar->addWidget(buttonFlush);

    QMenu *menuFlush = new QMenu(buttonFlush);
    buttonFlush->setMenu(menuFlush);

    // period of flush
    menuFlush->addAction(tr("Period of flush"), this, [=](){
        jnotify->send("monitor.toolbar.tree.flushPeriod");
    });

    auto setFlushStatus = [=](bool checked){
        buttonFlush->setIcon(QIcon(QString(":/datastudio/image/toolbar/%1.png")
                                   .arg(checked ? "stop" : "run")));
        buttonFlush->setText(tr("%1 flush").arg(checked ? tr("Stop") : tr("Start")));
        buttonFlush->setChecked(checked);
    };
    connect(buttonFlush, &QToolButton::toggled, this, [=](bool checked){
        setFlushStatus(checked);
        jnotify->send("monitor.toolbar.tree.flushToggle", checked);
    });

    if (option.isMember("flushEnabled")) {
        const bool checked = option["flushEnabled"].asBool();
        setFlushStatus(checked);
        if (!checked) {
            jnotify->send("monitor.toolbar.tree.flushToggle", false);
        }
    } else {
        setFlushStatus(true);
    }
}

void View::addSettingsAction()
{
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/settings.png"),
                         tr("Settings"), this, [=](){
        Monitor::SettingsDlg settingsDlg(this);
        if (settingsDlg.exec() != QDialog::Accepted) {
            return;
        }
    });
}

}
