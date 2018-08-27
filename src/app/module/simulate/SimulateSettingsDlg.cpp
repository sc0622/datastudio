#include "precomp.h"
#include "SimulateSettingsDlg.h"

namespace Simulate {

// class TreeViewSettings

TreeViewSettings::TreeViewSettings(QWidget *parent)
    : QWidget(parent)
    , modifyFlags_(1)
{
    QFormLayout *formLayoutMain = new QFormLayout(this);
    formLayoutMain->setLabelAlignment(Qt::AlignRight);

    comboBoxDeep_ = new QComboBox(this);
    comboBoxDeep_->addItem(tr("Vehicle"), Icd::ObjectVehicle);
    comboBoxDeep_->addItem(tr("System"), Icd::ObjectSystem);
    comboBoxDeep_->addItem(tr("Table"), Icd::ObjectTable);
    comboBoxDeep_->addItem(tr("Item"), Icd::ObjectItem);
    comboBoxDeep_->setCurrentIndex(1);
    formLayoutMain->addRow(tr("Loading deep:"), comboBoxDeep_);

    if (!init()) {
        //
    }

    connect(comboBoxDeep_, static_cast<void(QComboBox::*)(int)>
            (&QComboBox::currentIndexChanged), this, [=](){
        modifyFlags_.setBit(0, true);
        emit contentChanged();
    });
}

TreeViewSettings::~TreeViewSettings()
{

}

bool TreeViewSettings::tryAccept()
{
    if (modifyFlags_.testBit(0)) {
        const int deep = comboBoxDeep_->currentData().toInt();
        if (deep == -1) {
            QMessageBox::warning(this, tr("Warning"), tr("Deep cannot be empty!"));
            comboBoxDeep_->setFocus();
            return false;
        }

        JMain::instance()->setOption("simulate", "option.tree.loadDeep", deep);
        jnotify->send("simulate.toolbar.tree.loadDeep", deep);
    }

    return true;
}

bool TreeViewSettings::init()
{
    const Json::Value option = JMain::instance()->option("simulate", "option.tree");
    if (option.isNull()) {
        return false;
    }

    if (option.isMember("loadDeep")) {
        const int deep = qBound(0, option["loadDeep"].asInt() - Icd::ObjectVehicle,
                comboBoxDeep_->count() - 1);
        comboBoxDeep_->setCurrentIndex(deep);
    } else {
        comboBoxDeep_->setCurrentIndex(1);
    }

    return true;
}

// class SetViewSettings

SetViewSettings::SetViewSettings(QWidget *parent)
    : QWidget(parent)
    , modifyFlags_(1)
{
    QFormLayout *formLayoutMain = new QFormLayout(this);
    formLayoutMain->setLabelAlignment(Qt::AlignRight);

    checkBoxImmUpdate_ = new QCheckBox(tr("Update memory data once value changed"), this);
    formLayoutMain->addRow(tr("Realtime update:"), checkBoxImmUpdate_);

    if (!init()) {
        //
    }

    connect(checkBoxImmUpdate_, &QCheckBox::stateChanged, this, [=](int state){
        modifyFlags_.setBit(0, true);
        emit contentChanged();
    });
}

SetViewSettings::~SetViewSettings()
{

}

bool SetViewSettings::tryAccept()
{
    if (modifyFlags_.testBit(0)) {
        const bool realtimeUpdate = checkBoxImmUpdate_->isChecked();
        JMain::instance()->setOption("simulate", "option.view.realtimeUpdate", realtimeUpdate);
        jnotify->post("simulate.view.realtimeUpdate.changed", QVariant(realtimeUpdate));
    }

    return true;
}

bool SetViewSettings::init()
{
    const Json::Value option = JMain::instance()->option("simulate", "option.view");
    if (option.isNull()) {
        return false;
    }

    // realtime update
    if (option.isMember("realtimeUpdate")) {
        checkBoxImmUpdate_->setChecked(option["realtimeUpdate"].asBool());
    }

    return true;
}

// class SettingsDlg

SettingsDlg::SettingsDlg(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Other settings for simulate module"));

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    QTabWidget *tabWidget = new QTabWidget(this);
    vertLayoutMain->addWidget(tabWidget);

    treeViewSettings_ = new TreeViewSettings(this);
    tabWidget->addTab(treeViewSettings_,
                      QIcon(":/datastudio/image/global/tree-node.png"), tr("Tree view"));

    setViewSettings_ = new SetViewSettings(this);
    tabWidget->addTab(setViewSettings_,
                      QIcon(":/datastudio/image/global/edit.png"), tr("Set view"));

    vertLayoutMain->addSpacing(20);

    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    vertLayoutMain->addLayout(horiLayoutBottom);

    horiLayoutBottom->addStretch();

    QPushButton *buttonOk = new QPushButton(tr("Ok"), this);
    buttonOk->setMinimumWidth(80);
    horiLayoutBottom->addWidget(buttonOk);

    QPushButton *buttonCancel = new QPushButton(tr("Cancel"), this);
    buttonCancel->setMinimumWidth(80);
    horiLayoutBottom->addWidget(buttonCancel);

    horiLayoutBottom->addSpacing(10);

    vertLayoutMain->addSpacing(10);

    resize(500, sizeHint().height());

    connect(treeViewSettings_, &TreeViewSettings::contentChanged, this, [=](){
        buttonOk->setEnabled(true);
    });
    connect(setViewSettings_, &SetViewSettings::contentChanged, this, [=](){
        buttonOk->setEnabled(true);
    });
    connect(buttonOk, &QPushButton::clicked, this, [=](){
        if (!saveConfig()) {
            return;
        }
        this->accept();
    });
    connect(buttonCancel, &QPushButton::clicked, this, [=](){
        this->reject();
    });

    buttonOk->setDisabled(true);
}

SettingsDlg::~SettingsDlg()
{

}

bool SettingsDlg::saveConfig()
{
    if (!treeViewSettings_->tryAccept()) {
        return false;
    }
    if (!setViewSettings_->tryAccept()) {
        return false;
    }

    return true;
}

}
