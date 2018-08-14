#include "precomp.h"
#include "tablesel_widget.h"
#include "icdwidget/coretree_widget.h"

TableSelWidget::TableSelWidget(QWidget *parent)
    : QDialog(parent)
    , d_currentItem(nullptr)
{
    setWindowTitle(tr("Select table protocol"));

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_treeView = new Icd::CoreTreeWidget(this);
    d_treeView->setLoadingDeep(Icd::ObjectTable);
    d_treeView->setTreeMode(Icd::CoreTreeWidget::TreeModeTableSel);
    vertLayoutMain->addWidget(d_treeView);

    //
    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    horiLayoutBottom->setContentsMargins(8, 8, 8, 8);
    vertLayoutMain->addLayout(horiLayoutBottom);

    horiLayoutBottom->addStretch();

    QPushButton *buttonOk = new QPushButton(tr("Ok"), this);
    QPushButton *buttonCancel = new QPushButton(tr("Cancel"), this);
    buttonOk->setMinimumWidth(80);
    buttonCancel->setMinimumWidth(80);
    buttonOk->setDefault(true);
    horiLayoutBottom->addWidget(buttonOk);
    horiLayoutBottom->addWidget(buttonCancel);

    horiLayoutBottom->addSpacing(10);

    //
    connect(d_treeView, &Icd::CoreTreeWidget::currentItemChanged, this,
            [=](QStandardItem *current){
        d_currentItem = current;
    });
    connect(d_treeView, &Icd::CoreTreeWidget::itemDoubleClicked, this,
            [=](QStandardItem *current){
        d_currentItem = current;
        if (current && current->type() != Icd::TreeItemTypeTable) {
            return;
        }
        this->accept();
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

    const Json::Value config = JMain::instance()->option("analyse", "parser");
    if (config.isNull()) {
        return false;
    }

    const Icd::ParserPtr parser = Icd::Parser::create(config);
    if (!parser) {
        return false;
    }

    d_treeView->setParser(parser);

    //
    if (!d_treeView->loadData()) {
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
    d_currentItem = d_treeView->currentItem();

    if (!d_currentItem || d_currentItem->type() != Icd::TreeItemTypeTable) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Not select any table"));
        return;
    }

    QDialog::accept();
}
