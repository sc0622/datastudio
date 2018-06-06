#include "precomp.h"
#include "MainTabWidget.h"
#include "MainMenuBar.h"

namespace Main {

TabWidget::TabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    setIconSize(QSize(20, 20));
    setStyleSheet("QTabWidget::tab-bar{alignment:center;}");

    menuBar_ = new MenuBar(this);
    setCornerWidget(menuBar_, Qt::TopRightCorner);

    addTab(new QWidget(this), QIcon(":/datastudio/image/global/setting.png"), tr("Prefer"));
    addTab(new QWidget(this), QIcon(":/datastudio/image/global/dataedit.png"), tr("Edit"));
    addTab(new QWidget(this), QIcon(":/datastudio/image/global/monitor.png"), tr("Monitor"));
    addTab(new QWidget(this), QIcon(":/datastudio/image/global/simulate.png"), tr("Simulate"));
    addTab(new QWidget(this), QIcon(":/datastudio/image/global/analyse.png"), tr("Analyse"));

    connect(this, &TabWidget::currentChanged, this, [=](int index){
        jnotify->send("main.tab.changed", index);
    });
    connect(this, &TabWidget::tabBarDoubleClicked, this, [=](){
        jnotify->send("main.toolbar.show.toggle", menuBar_);
    });

    jnotify->on("main.tab.currentIndex", this, [=](JNEvent &event){
        event.setReturnValue(currentIndex());
    });

    setFixedHeight(tabBar()->contentsRect().height());
}

TabWidget::~TabWidget()
{
    JMain::saveWidgetState(this);
}

bool TabWidget::init()
{
    bool result = true;

    JMain::restoreWidgetState(this);

    menuBar_->init();

    return result;
}

}
