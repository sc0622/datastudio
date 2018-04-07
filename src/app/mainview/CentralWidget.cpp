#include "precomp.h"
#include "CentralWidget.h"
#include "MainMenuBar.h"
#include "../module/preference/PreferenceView.h"
#include "../module/edit/EditView.h"
#include "../module/monitor/MonitorView.h"
#include "../module/simulate/SimulateView.h"
#include "../module/analyse/AnalyseView.h"
#include "../module/common/DataSourceConfigDlg.h"

CentralWidget::CentralWidget(QWidget *parent)
    : QTabWidget(parent)
{
    setIconSize(QSize(20, 20));

    d_menuBar = new Main::MenuBar(this);
    setCornerWidget(d_menuBar, Qt::TopRightCorner);

    d_preferenceView = new Preference::View(this);
    d_editView = new Edit::View(this);
    d_monitorView = new Monitor::View(this);
    d_simulateView = new Simulate::View(this);
    d_analyseView = new Analyse::View(this);

    connect(this, &CentralWidget::currentChanged, this, [=](int index){
        jnotify->send("main.tab.changed", index);
    });

    addTab(d_preferenceView, QIcon(":/datastudio/image/global/setting.png"), tr("Preference"));
    addTab(d_editView, QIcon(":/datastudio/image/global/dataedit.png"), tr("Edit"));
    addTab(d_monitorView, QIcon(":/datastudio/image/global/monitor.png"), tr("Monitor"));
    addTab(d_simulateView, QIcon(":/datastudio/image/global/simulate.png"), tr("Simulate"));
    addTab(d_analyseView, QIcon(":/datastudio/image/global/analyse.png"), tr("Analyse"));

    connect(this, &CentralWidget::tabBarDoubleClicked, this, [=](){
        jnotify->send("main.toolbar.show.toggle", d_menuBar);
    });

    //
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
        DataSourceConfigDlg dialog(module, receiver, this);
        if (dialog.exec() != QDialog::Accepted) {
            return;
        }
        //
    });
}

CentralWidget::~CentralWidget()
{
    JMain::saveWidgetState(this);
}

bool CentralWidget::init()
{
    bool result = true;

    JMain::restoreWidgetState(this);

    d_menuBar->init();
    d_preferenceView->init();
    d_editView->init();
    d_monitorView->init();
    d_simulateView->init();
    d_analyseView->init();

    return result;
}
