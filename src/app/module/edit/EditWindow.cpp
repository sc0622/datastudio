#include "precomp.h"
#include "EditWindow.h"
#ifdef EDIT_OLD
#include "old/nav/icdnavigation.h"
#include "old/view/EditMainView.h"
#else
#include "EditTreeView.h"
#include "EditSetView.h"
#endif
#include "EditSettingsDlg.h"

namespace Edit {

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    splitterMain_ = new JSplitter(QList<double>() << 1 << 3, this);
    splitterMain_->setObjectName("edit.window.splitter.main");
    splitterMain_->setOpaqueResize(true);
    vertLayoutMain->addWidget(splitterMain_);

    treeView_ = new TreeView(this);
    splitterMain_->addWidget(treeView_);

    setView_ = new SetView(this);
    splitterMain_->addWidget(setView_);

    jnotify->on("edit.toolbar.settings", this, [=](JNEvent &){
        Edit::SettingsDlg settingsDlg(this);
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

    treeView_->init();
    setView_->init();

    return result;
}

}
