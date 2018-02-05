#include "precomp.h"
#include "MainSettingsDlg.h"
#include "settings/GeneralWidget.h"

namespace Main {

SettingsDlg::SettingsDlg(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Global Settings"));
    resize(800, 500);

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_splitter = new JSplitter(this);
    d_splitter->setObjectName("main.settings.splitter");
    d_splitter->setHandleWidth(5);
    d_splitter->setScales(QList<double>() << 1 << 2.5);
    vertLayoutMain->addWidget(d_splitter);

    d_treeView = new JTreeView(this);
    d_treeView->setFrameShape(QFrame::NoFrame);
    d_treeView->header()->hide();
    d_treeView->setAnimated(true);
    d_splitter->addWidget(d_treeView);

    d_scrollArea = new QScrollArea(this);
    d_scrollArea->setFrameShape(QFrame::NoFrame);
    d_scrollArea->setWidgetResizable(true);
    d_splitter->addWidget(d_scrollArea);

    connect(d_treeView, &JTreeView::currentItemChanged,
            this, &SettingsDlg::onCurrentItemChanged);

    init();
}

SettingsDlg::~SettingsDlg()
{
    JMain::saveWidgetState(d_splitter);
}

bool SettingsDlg::init()
{
    bool result = true;

    if (result) {
        JMain::restoreWidgetState(d_splitter);
    }

    d_treeView->clearContents();

    auto rootItem = d_treeView->invisibleRootItem();

    // environment
    auto environmentItem = new JTreeViewItem(tr("Environment"));
    environmentItem->setData("environment", Icd::TreeItemDomainRole);
    rootItem->appendRow(environmentItem);
    d_treeView->expandItem(environmentItem);

    // environment - general
    auto generalItem = new JTreeViewItem(tr("General"));
    generalItem->setData("environment/general", Icd::TreeItemDomainRole);
    environmentItem->appendRow(generalItem);
    d_treeView->setCurrentItem(generalItem);

    return result;
}

void SettingsDlg::onCurrentItemChanged(QStandardItem *current, QStandardItem *previous)
{
    Q_UNUSED(previous);
    QWidget *widget = d_scrollArea->takeWidget();
    if (widget) {
        widget->deleteLater();
    }

    if (!current) {
        return;
    }

    const QVariant varDomain = current->data(Icd::TreeItemDomainRole);
    if (varDomain.type() != QVariant::String) {
        return;
    }

    const QString domain = varDomain.toString();
    if (domain.isEmpty()) {
        return;
    }

    if (domain == "environment/general") {
        auto generalWidget = new GeneralWidget(d_scrollArea);
        d_scrollArea->setWidget(generalWidget);
    } else {
        //
    }
}

}
