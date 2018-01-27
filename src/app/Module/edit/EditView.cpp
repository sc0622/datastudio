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
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/database.png"),
                         tr("Database"), this, [=](){
        QVariantList args;
        args.append("edit");
        args.append(qVariantFromValue((void*)d_treeView));
        jnotify->send("database.config", args);
    });
    d_toolBar->addSeparator();
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/copy.png"),
                         tr("Copy"), this, [=](){
        //
    });
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/save.png"),
                         tr("Save"), this, [=](){
        //
    });
    d_toolBar->addAction(QIcon(":/datastudio/image/toolbar/save-as.png"),
                         tr("Save as"), this, [=](){
        //
    });
    d_toolBar->addSeparator();
    addViewAction();
    d_toolBar->addSeparator();
    addSettingsAction();
}

void View::addViewAction()
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
    QAction *actionOffset = menuView->addAction(tr("Show offset"),
                                                this, [=](bool checked){
        Q_UNUSED(checked);
    });
    actionOffset->setCheckable(true);
    // type
    QAction *actionType = menuView->addAction(tr("Show type"),
                                              this, [=](bool checked){
        Q_UNUSED(checked);
    });
    actionType->setCheckable(true);
    // real value
    QAction *actionReal = menuView->addAction(tr("Real value"),
                                              this, [=](bool checked){
        Q_UNUSED(checked);
    });
    actionReal->setCheckable(true);
    // orignal value
    QAction *actionOrignal = menuView->addAction(tr("Orignal value"),
                                                 this, [=](bool checked){
        Q_UNUSED(checked);
    });
    actionOrignal->setCheckable(true);
    QAction *actionNoOrigValue = addOrigValueRadixAction(actionOrignal);
    // desc
    QAction *actionDesc = menuView->addAction(tr("Description"),
                                              this, [=](bool checked){
        Q_UNUSED(checked);
    });
    actionDesc->setCheckable(true);
    //
    connect(buttonView, &QToolButton::clicked, this, [=](bool checked){
        actionOffset->setChecked(checked);
        actionType->setChecked(checked);
        actionReal->setChecked(checked);
        actionNoOrigValue->setChecked(!checked);
        actionDesc->setChecked(checked);
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
