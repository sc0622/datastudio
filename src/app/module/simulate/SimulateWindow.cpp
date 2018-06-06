#include "precomp.h"
#include "SimulateWindow.h"
#include "SimulateTreeView.h"
#include "SimulateSetView.h"
#include "SimulateSettingsDlg.h"

namespace Simulate {

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    splitterMain_ = new JSplitter(QList<double>() << 1 << 3, this);
    splitterMain_->setObjectName("simulate.window.splitter.main");
    splitterMain_->setOpaqueResize(true);
    vertLayoutMain->addWidget(splitterMain_);

    treeView_ = new TreeView(this);
    splitterMain_->addWidget(treeView_);

    setView_ = new SetView(this);
    splitterMain_->addWidget(setView_);

    jnotify->on("simulate.toolbar.settings", this, [=](JNEvent &){
        Simulate::SettingsDlg settingsDlg(this);
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
    result = result && setView_->init();

    return result;
}

}
