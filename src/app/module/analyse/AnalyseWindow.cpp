#include "precomp.h"
#include "AnalyseWindow.h"
#include "AnalyseTreeView.h"
#include "AnalyseChartView.h"
#include "AnalyseSettingsDlg.h"
#include "private/convertdatadlg.h"

namespace Analyse {

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    splitterMain_ = new JSplitter(QList<double>() << 1 << 3, this);
    splitterMain_->setObjectName("analyse.window.splitter.main");
    splitterMain_->setOpaqueResize(true);
    vertLayoutMain->addWidget(splitterMain_);

    treeView_ = new TreeView(this);
    splitterMain_->addWidget(treeView_);

    chartView_ = new ChartView(this);
    splitterMain_->addWidget(chartView_);

    jnotify->on("analyse.toolbar.convertdata", this, [=](JNEvent &){
        convertRecordData();
    });
    jnotify->on("analyse.toolbar.settings", this, [=](JNEvent &){
        Analyse::SettingsDlg settingsDlg(this);
        if (settingsDlg.exec() != QDialog::Accepted) {
            return;
        }
    });
}

Window::~Window()
{
    JMain::saveWidgetState(splitterMain_);
}

bool Window::init()
{
    bool result = true;

    JMain::restoreWidgetState(splitterMain_);
    result = result && treeView_->init();
    result = result && chartView_->init();

    return result;
}

void Window::convertRecordData()
{
    ConvertDataDlg convertDlg(this);
    convertDlg.exec();
}

}
