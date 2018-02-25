#include "precomp.h"
#include "MainMenuBar.h"
#include "MainSettingsDlg.h"

namespace Main {

MenuBar::MenuBar(QWidget *parent)
    : QMenuBar(parent)
{

}

bool MenuBar::init()
{
    bool result = true;

    Json::Value config = JMain::instance()->config("global.menubar");
    if (config.isNull()) {
        //return false;
    }

    if (result) {
        updateMenuBar(config);
    }

    return result;
}

void MenuBar::updateMenuBar(const Json::Value &config)
{
    clear();

    addSettingsMenu(config);
    addAnalyseToolAction(config);
    addScreenshotAction(config);
    addPaletteAction(config);
    addCalcToolAction(config);
    addShowToolbarAction(config);
}

void MenuBar::addSettingsMenu(const Json::Value &config)
{
    Q_UNUSED(config);
    QMenu *menu = addMenu(QIcon(":/datastudio/image/toolbar/list.png"),
                          tr("Main menu"));
    // global settings
    menu->addAction(QIcon(":/datastudio/image/global/setting.png"),
                    tr("Settings"), this, [=](){
        Main::SettingsDlg settingsDlg(this);
        if (settingsDlg.exec() != QDialog::Accepted) {
            return;
        }
    });
}

void MenuBar::addAnalyseToolAction(const Json::Value &config)
{
    Q_UNUSED(config);
#ifdef _MSC_VER
    QAction *action = addAction(tr("Data analyse tool"));
    action->setIcon(QIcon(":/datastudio/image/toolbar/ruler.png"));
    connect(action, &QAction::triggered, this, [=](){
        QProcess::startDetached("\"" + QCoreApplication::applicationDirPath()
                                .append("/../tools/DataAnalyse.exe\""));
    });
#endif
}

void MenuBar::addCalcToolAction(const Json::Value &config)
{
    Q_UNUSED(config);
#ifdef _MSC_VER
    QAction *action = addAction(tr("Calculate"));
    action->setIcon(QIcon(":/datastudio/image/toolbar/calc.png"));
    connect(action, &QAction::triggered, this, [=](){
        QProcess::startDetached("calc.exe");
    });
#endif
}

void MenuBar::addScreenshotAction(const Json::Value &config)
{
    Q_UNUSED(config);
#ifdef _MSC_VER
    QAction *action = addAction(tr("Screenshot"));
    action->setIcon(QIcon(":/datastudio/image/toolbar/screenshot.png"));
    connect(action, &QAction::triggered, this, [=](){
        QProcess::startDetached("\"" + QCoreApplication::applicationDirPath()
                                .append("/../tools/WinSnap/WinSnap.exe\""));
    });
#endif
}

void MenuBar::addPaletteAction(const Json::Value &config)
{
    Q_UNUSED(config);
#ifdef _MSC_VER
    QAction *action = addAction(tr("Palette"));
    action->setIcon(QIcon(":/datastudio/image/toolbar/palette.png"));
    connect(action, &QAction::triggered, this, [=](){
        QProcess::startDetached("mspaint.exe");
    });
#endif
}

void MenuBar::addShowToolbarAction(const Json::Value &config)
{
    QAction *actionShowToolBar = addAction(tr("Show toolbar"));
    actionShowToolBar->setCheckable(true);
    actionShowToolBar->setChecked(true);
    auto setIcon = [=](bool checked){
        actionShowToolBar->setIcon(QIcon(QString(":/datastudio/image/toolbar/arrow-%1.png")
                                         .arg(checked ? "up" : "down")));
    };
    setIcon(true);
    connect(actionShowToolBar, &QAction::toggled, this, [=](bool checked){
        setIcon(checked);
        jnotify->send("main.toolbar.show", checked);
        //
        JMain::instance()->setConfig("global.menubar.toolBarVisible", checked);
    });
    jnotify->on("main.toolbar.show.toggle", this, [=](JNEvent &){
        actionShowToolBar->toggle();
    });

    if (config.isMember("toolBarVisible")) {
        actionShowToolBar->setChecked(config["toolBarVisible"].asBool());
    }
}

}

