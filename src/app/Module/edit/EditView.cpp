#include "precomp.h"
#include "EditView.h"
#include "EditTreeView.h"
#include "EditSetView.h"
#include "EditSettingsDlg.h"

namespace Edit {

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
    d_splitterMain->setObjectName("Edit::splitterMain");
    vertLayoutMain->addWidget(d_splitterMain);

    d_treeView = new TreeView(this);
    d_splitterMain->addWidget(d_treeView);

    d_setView = new SetView(this);
    d_splitterMain->addWidget(d_setView);

    jnotify->on("main.edit.toolbar.show", this, [=](JNEvent &event){
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
    d_treeView->init();
    d_setView->init();

    return result;
}

void View::updateToolBar()
{
    d_toolBar->clear();

    const Json::Value option = JMain::instance()->option("edit", "option");

    // database
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/database.png"),
                         tr("Database"), this, [=](){
        QVariantList args;
        args.append("edit");
        args.append(qVariantFromValue((void*)d_treeView));
        jnotify->send("database.config", args);
    });
    d_toolBar->addSeparator();
    // view
    addViewAction(option);
    d_toolBar->addSeparator();
    // copy
    QAction *actionCopy = d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/copy.png"),
                                               tr("Copy"), this, [=](){
        jnotify->send("edit.toolbar.tree.copy", d_treeView);
    });
    Q_UNUSED(actionCopy);
    // save
    QAction *actionSave = d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/save.png"),
                                               tr("Save"), this, [=](){
        jnotify->send("edit.toolbar.tree.save", d_treeView);
    });
    Q_UNUSED(actionSave);
    actionSave->setEnabled(false);
    // saveAs
    QAction *actionSaveAs = d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/save-as.png"),
                                                 tr("Save as"), this, [=](){
        jnotify->send("edit.toolbar.tree.saveas", d_treeView);
    });
    Q_UNUSED(actionSaveAs);
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
        jnotify->send("edit.toolbar.tree.showOffset", d_treeView, checked);
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
        jnotify->send("edit.toolbar.tree.showType", d_treeView, checked);
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

    //
    connect(buttonView, &QToolButton::clicked, this, [=](bool checked){
        actionOffset->setChecked(checked);
        actionType->setChecked(checked);
    });
}

QAction *View::addOrigValueRadixAction(QAction *action)
{
    if (!action) {
        Q_ASSERT(false);
        return nullptr;
    }

    QMenu *menu = new QMenu();
    action->setMenu(menu);

    QActionGroup *actionGroupView = new QActionGroup(menu);

    QAction *actionNoOrigValue = menu->addAction(tr("Hide"), this, [=](bool checked){
        Q_UNUSED(checked);
    });
    actionNoOrigValue->setCheckable(true);
    menu->addSeparator();
    QAction *actionDecimal = menu->addAction(tr("Decimal integer"), this, [=](bool checked){
        Q_UNUSED(checked);
    });
    actionDecimal->setCheckable(true);
    actionDecimal->setActionGroup(actionGroupView);
    QAction *actionHexadecimal = menu->addAction(tr("Hexadecimal integer"), this, [=](bool checked){
        Q_UNUSED(checked);
    });
    actionHexadecimal->setCheckable(true);
    actionHexadecimal->setActionGroup(actionGroupView);
    QAction *actionBinary = menu->addAction(tr("Binary integer"), this, [=](bool checked){
        Q_UNUSED(checked);
    });
    actionBinary->setCheckable(true);
    actionBinary->setActionGroup(actionGroupView);
    QAction *actionOctal = menu->addAction(tr("Octal integer"), this, [=](bool checked){
        Q_UNUSED(checked);
    });
    actionOctal->setCheckable(true);
    actionOctal->setActionGroup(actionGroupView);

    return actionNoOrigValue;
}

void View::addSettingsAction()
{
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/settings.png"),
                         tr("Settings"), this, [=](){
        Edit::SettingsDlg settingsDlg(this);
        if (settingsDlg.exec() != QDialog::Accepted) {
            return;
        }
    });
}

}
