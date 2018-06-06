#include "precomp.h"
#include "MonitorWindow.h"
#include "MonitorTreeView.h"
#include "MonitorDataView.h"
#include "MonitorChartView.h"
#include "MonitorSettingsDlg.h"

namespace Monitor {

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    splitterMain_ = new JSplitter(QList<double>() << 1 << 3, this);
    splitterMain_->setObjectName("monitor.window.splitter.main");
    splitterMain_->setOpaqueResize(true);
    vertLayoutMain->addWidget(splitterMain_);

    splitterLeft_ = new JSplitter(QList<double>() << 1.5 << 1, Qt::Vertical, this);
    splitterLeft_->setObjectName("mointor.window.splitter.left");
    splitterLeft_->setOpaqueResize(true);
    splitterMain_->addWidget(splitterLeft_);

    treeView_ = new TreeView(this);
    splitterLeft_->addWidget(treeView_);

    dataView_ = new DataView(this);
    splitterLeft_->addWidget(dataView_);

    chartView_ = new ChartView(this);
    splitterMain_->addWidget(chartView_);

    jnotify->on("monitor.toolbar.settings", this, [=](JNEvent &){
        Monitor::SettingsDlg settingsDlg(this);
        if (settingsDlg.exec() != QDialog::Accepted) {
            return;
        }
    });
}

Window::~Window()
{
    JMain::saveWidgetState(splitterMain_);
    JMain::saveWidgetState(splitterLeft_);
}

bool Window::init()
{
    bool result = true;

    JMain::restoreWidgetState(splitterMain_);
    JMain::restoreWidgetState(splitterLeft_);

    result = result && treeView_->init();
    result = result && dataView_->init();
    result = result && chartView_->init();

    return result;
}

}
