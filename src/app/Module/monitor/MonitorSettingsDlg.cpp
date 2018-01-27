#include "precomp.h"
#include "MonitorSettingsDlg.h"

namespace Monitor {

// class TreeViewSettings

TreeViewSettings::TreeViewSettings(QWidget *parent)
    : QWidget(parent)
    , d_modifyFlags(1)
{
    QFormLayout *formLayoutMain = new QFormLayout(this);
    formLayoutMain->setLabelAlignment(Qt::AlignRight);

    d_comboBoxDeep = new QComboBox(this);
    d_comboBoxDeep->addItem(tr("Vehicle"), 0);
    d_comboBoxDeep->addItem(tr("System"), 1);
    d_comboBoxDeep->addItem(tr("Table"), 2);
    d_comboBoxDeep->addItem(tr("Item"), 3);
    d_comboBoxDeep->setCurrentIndex(1);
    formLayoutMain->addRow(tr("Loading deep:"), d_comboBoxDeep);

    if (!init()) {
        //
    }

    connect(d_comboBoxDeep, static_cast<void(QComboBox::*)(int)>
            (&QComboBox::currentIndexChanged), this, [=](){
        d_modifyFlags.setBit(0, true);
        emit contentChanged();
    });
}

TreeViewSettings::~TreeViewSettings()
{

}

bool TreeViewSettings::tryAccept()
{
    if (d_modifyFlags.testBit(0)) {
        const int deep = d_comboBoxDeep->currentIndex();
        if (deep == -1) {
            QMessageBox::warning(this, tr("Warning"), tr("Deep cannot be empty!"));
            d_comboBoxDeep->setFocus();
            return false;
        }

        jnotify->send("monitor.toolbar.tree.loadDeep", deep);
    }

    return true;
}

bool TreeViewSettings::init()
{
    const Json::Value option = JMain::instance()->option("monitor", "option/tree");
    if (option.isNull()) {
        return false;
    }

    if (option.isMember("loadDeep")) {
        const int deep = qBound(0, option["loadDeep"].asInt(), d_comboBoxDeep->count() - 1);
        d_comboBoxDeep->setCurrentIndex(deep);
    } else {
        d_comboBoxDeep->setCurrentIndex(1);
    }

    return true;
}

// class ChartViewSettings

ChartViewSettings::ChartViewSettings(QWidget *parent)
    : QWidget(parent)
    , d_modifyFlags(2)
{
    QFormLayout *formLayoutMain = new QFormLayout(this);
    formLayoutMain->setLabelAlignment(Qt::AlignRight);

    d_spinBoxColumnCount = new QSpinBox(this);
    d_spinBoxColumnCount->setRange(1, 10);
    d_spinBoxColumnCount->setValue(1);
    formLayoutMain->addRow(tr("Column count:"), d_spinBoxColumnCount);

    d_spinBoxYLabelWidth = new QSpinBox(this);
    d_spinBoxYLabelWidth->setRange(1, 20);
    d_spinBoxYLabelWidth->setValue(10);
    formLayoutMain->addRow(tr("Y-label width:"), d_spinBoxYLabelWidth);

    if (!init()) {
        //
    }

    connect(d_spinBoxColumnCount, static_cast<void(QSpinBox::*)(int)>
            (&QSpinBox::valueChanged), this, [=](){
        d_modifyFlags.setBit(0, true);
        emit contentChanged();
    });
    connect(d_spinBoxYLabelWidth, static_cast<void(QSpinBox::*)(int)>
            (&QSpinBox::valueChanged), this, [=](){
        d_modifyFlags.setBit(1, true);
        emit contentChanged();
    });
}

ChartViewSettings::~ChartViewSettings()
{

}

bool ChartViewSettings::tryAccept()
{
    if (d_modifyFlags.testBit(0)) {
        jnotify->send("monitor.toolbar.chart.columnCount", d_spinBoxColumnCount->value());
    }
    if (d_modifyFlags.testBit(1)) {
        jnotify->send("monitor.toolbar.chart.yLabelWidth", d_spinBoxYLabelWidth->value());
    }

    return true;
}

bool ChartViewSettings::init()
{
    const Json::Value option = JMain::instance()->option("monitor", "option/chart");
    if (option.isNull()) {
        return false;
    }

    // columnCount
    if (option.isMember("columnCount")) {
        const int columnCount = qBound(1, option["columnCount"].asInt(), 10);
        d_spinBoxColumnCount->setValue(columnCount);
    }
    // yLabelWidth
    if (option.isMember("yLabelWidth")) {
        const int yLabelWidth = qBound(1, option["yLabelWidth"].asInt(), 20);
        d_spinBoxYLabelWidth->setValue(yLabelWidth);
    }

    return true;
}

// class SettingsDlg

SettingsDlg::SettingsDlg(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Other settings for monitor module"));

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    QTabWidget *tabWidget = new QTabWidget(this);
    vertLayoutMain->addWidget(tabWidget);

    d_treeViewSettings = new TreeViewSettings(this);
    tabWidget->addTab(d_treeViewSettings,
                      QIcon(":/datastudio/image/global/tree-node.png"), tr("Tree view"));

    d_chartViewSettings = new ChartViewSettings(this);
    tabWidget->addTab(d_chartViewSettings,
                      QIcon(":/datastudio/image/global/linechart.png"), tr("Chart view"));

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

    connect(d_treeViewSettings, &TreeViewSettings::contentChanged, this, [=](){
        buttonOk->setEnabled(true);
    });
    connect(d_chartViewSettings, &ChartViewSettings::contentChanged, this, [=](){
        buttonOk->setEnabled(true);
    });
    connect(buttonOk, &QPushButton::clicked, this, [=](){
        if (!saveConfig()) {
            return;
        }
        accept();
    });
    connect(buttonCancel, &QPushButton::clicked, this, [=](){
        reject();
    });

    buttonOk->setDisabled(true);
}

SettingsDlg::~SettingsDlg()
{

}

bool SettingsDlg::saveConfig()
{
    if (!d_treeViewSettings->tryAccept()) {
        return false;
    }
    if (!d_chartViewSettings->tryAccept()) {
        return false;
    }

    return true;
}

}
