#include "precomp.h"
#include "CentralWidget.h"
#include "../module/prefer/PreferWindow.h"
#include "../module/edit/EditWindow.h"
#include "../module/monitor/MonitorWindow.h"
#include "../module/simulate/SimulateWindow.h"
#include "../module/analyse/AnalyseWindow.h"

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setContentsMargins(0, 0, 0, 0);

    stackedWidget_ = new QStackedWidget;
    layoutMain->addWidget(stackedWidget_);

    preferWindow_ = new Prefer::Window(this);
    editWindow_ = new Edit::Window(this);
    monitorWindow_ = new Monitor::Window(this);
    simulateWindow_ = new Simulate::Window(this);
    analyseWindow_ = new Analyse::Window(this);
    stackedWidget_->addWidget(preferWindow_);
    stackedWidget_->addWidget(editWindow_);
    stackedWidget_->addWidget(monitorWindow_);
    stackedWidget_->addWidget(simulateWindow_);
    stackedWidget_->addWidget(analyseWindow_);

    jnotify->on("main.tab.changed", this, [=](JNEvent &event){
        const int index = event.argument().toInt();
        stackedWidget_->setCurrentIndex(index);
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

    preferWindow_->init();
    editWindow_->init();
    monitorWindow_->init();
    simulateWindow_->init();
    analyseWindow_->init();

    return result;
}
