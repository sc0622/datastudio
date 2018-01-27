#include "precomp.h"
#include "tablesel_widget.h"
#include "icdwidget/coretree_widget.h"

TableSelWidget::TableSelWidget(QWidget *parent)
    : QDialog(parent)
    , d_currentItem(0)
{
    setWindowTitle(QStringLiteral("��Э��ѡ��"));

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_icdTree = new Icd::CoreTreeWidget(this);
    d_icdTree->setLoadingDeep(Icd::TreeItemTypeTable);
    d_icdTree->setTreeMode(Icd::CoreTreeWidget::TreeModeTableSel);
    vertLayoutMain->addWidget(d_icdTree);

    //
    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    horiLayoutBottom->setContentsMargins(8, 8, 8, 8);
    vertLayoutMain->addLayout(horiLayoutBottom);

    horiLayoutBottom->addStretch();

    QPushButton *buttonOk = new QPushButton(QStringLiteral("ȷ��"), this);
    QPushButton *buttonCancel = new QPushButton(QStringLiteral("ȡ��"), this);
    buttonOk->setMinimumWidth(80);
    buttonCancel->setMinimumWidth(80);
    buttonOk->setDefault(true);
    horiLayoutBottom->addWidget(buttonOk);
    horiLayoutBottom->addWidget(buttonCancel);

    horiLayoutBottom->addSpacing(10);

    //
    connect(d_icdTree, &Icd::CoreTreeWidget::currentItemChanged, this,
            [=](QStandardItem *current){
        d_currentItem = current;
    });
    connect(d_icdTree, &Icd::CoreTreeWidget::itemDoubleClicked, this,
            [=](QStandardItem *current){
        d_currentItem = current;
        if (current && current->type() != Icd::TreeItemTypeTable) {
            return;
        }
        accept();
    });
    connect(buttonOk, &QPushButton::clicked, this, &TableSelWidget::accept);
    connect(buttonCancel, &QPushButton::clicked, this, &TableSelWidget::reject);
}

TableSelWidget::~TableSelWidget()
{

}

bool TableSelWidget::init()
{
    bool result = true;

    //
    Icd::Parser *parser = 0;
    if (jframeNotifier().sendMessage("query_icd_parser@com_icdsystem",
                                     (JWPARAM)&parser) != JFrame::ResultSuccess) {
        return false;
    }

    //
    d_icdTree->setParser(parser);

    //
    if (!d_icdTree->loadData()) {
        return false;
    }

    return result;
}

QString TableSelWidget::tableDomain() const
{
    if (d_currentItem && d_currentItem->type() == Icd::TreeItemTypeTable) {
        return d_currentItem->data(Icd::TreeItemDomainRole).toString();
    } else {
        return QString();
    }
}

void TableSelWidget::accept()
{
    d_currentItem = d_icdTree->currentItem();

    if (!d_currentItem || d_currentItem->type() != Icd::TreeItemTypeTable) {
        QMessageBox::warning(this, QStringLiteral("����"),
                             QStringLiteral("û��ѡ����Ч��ڵ�"));
        return;
    }

    QDialog::accept();
}
