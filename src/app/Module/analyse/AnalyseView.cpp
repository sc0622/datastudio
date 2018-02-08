#include "precomp.h"
#include "AnalyseView.h"
#include "AnalyseTreeView.h"
#include "AnalyseChartView.h"
#include "AnalyseSettingsDlg.h"
#include "private/convertdatadlg.h"

namespace Analyse {

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
    d_splitterMain->setObjectName("Analyse::splitterMain");
    vertLayoutMain->addWidget(d_splitterMain);

    d_treeView = new TreeView(this);
    d_splitterMain->addWidget(d_treeView);

    d_chartView = new ChartView(this);
    d_splitterMain->addWidget(d_chartView);

    jnotify->on("main.analyse.toolbar.show", this, [=](JNEvent &event){
        d_toolBar->setVisible(event.argument().toBool());
    });
    jnotify->on("main.toolbar.show", this, [=](JNEvent &event){
        d_toolBar->setVisible(event.argument().toBool());
    });
}

View::~View()
{
    JMain::saveWidgetState(d_splitterMain);
}

bool View::init()
{
    bool result = true;

    JMain::restoreWidgetState(d_splitterMain);
    updateToolBar();
    result = result && d_treeView->init();
    result = result && d_chartView->init();
    updateToolBar();

    return result;
}

void View::updateToolBar()
{
    d_toolBar->clear();

    const Json::Value option = JMain::instance()->option("analyse", "option");

    // database
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/database.png"),
                         tr("Database"), this, [=](){
        QVariantList args{"analyse", qVariantFromValue((void*)d_treeView)};
        jnotify->send("database.config", args);
    });
    d_toolBar->addSeparator();
    // view
    addViewAction(option);
    d_toolBar->addSeparator();
    // tree
    addTreeAction(option["tree"]);
    d_toolBar->addSeparator();
    // chart
    addChartAction(option["chart"]);
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
        jnotify->send("analyse.toolbar.tree.showOffset", d_treeView, checked);
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
        jnotify->send("analyse.toolbar.tree.showType", d_treeView, checked);
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

    connect(buttonView, &QToolButton::clicked, this, [=](bool checked){
        actionOffset->setChecked(checked);
        actionType->setChecked(checked);
    });
}

void View::addTreeAction(const Json::Value &option)
{
    Q_UNUSED(option);
    // load data
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/load.png"),
                         tr("Load data"), this, [=](){
        jnotify->send("analyse.toolbar.tree.loadData", d_treeView);
    });
    // unload data
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/unload.png"),
                         tr("Unload data"), this, [=](){
        jnotify->send("analyse.toolbar.tree.unloadData", d_treeView);
    });
    // convert data
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/convert.png"),
                         tr("Convert data"), this, [=](){
        convertRecordData();
    });
}

void View::addChartAction(const Json::Value &option)
{
    // set coordinate
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/coordinate.png"),
                         tr("Set coordinate"), this, [=](){
        jnotify->send("analyse.toolbar.chart.setCoordinate");
    });
    // update data
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/update.png"),
                         tr("Update data"), this, [=](){
        jnotify->send("analyse.toolbar.chart.update");
    });
    // clear chart
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                         tr("Clear chart"), this, [=](){
        jnotify->send("analyse.toolbar.chart.clean");
    });
    // column-width-same
    QAction *actionColumnWidthSame = d_toolBar->addAction(
                QIcon(":/datastudio/image/toolbar/colwidth-same.png"),
                tr("Same column width"), this, [=](){
        jnotify->send("analyse.toolbar.chart.columnWidthSame", d_chartView);
    });
    actionColumnWidthSame->trigger();
    // x-label-sync
    QAction *actionXAxisSync = d_toolBar->addAction(
                QIcon(":/datastudio/image/toolbar/xlabel_sync.png"),
                tr(" Synchronize x-label"));
    actionXAxisSync->setCheckable(true);
    connect(actionXAxisSync, &QAction::toggled, this, [=](bool checked){
        jnotify->send("analyse.toolbar.chart.xAxisSync", d_chartView, checked);
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
        jnotify->send("analyse.toolbar.chart.showYLabel", d_chartView, checked);
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
        jnotify->send("analyse.toolbar.chart.showYAlign", d_chartView, checked);
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
        jnotify->send("analyse.toolbar.chart.syncTrack", d_chartView, checked);
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
}

void View::addSettingsAction()
{
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/settings.png"),
                         tr("Settings"), this, [=](){
        Analyse::SettingsDlg settingsDlg(this);
        if (settingsDlg.exec() != QDialog::Accepted) {
            return;
        }
    });
}

void View::convertRecordData()
{
    ConvertDataDlg convertDlg(this);
    convertDlg.exec();
}

}
