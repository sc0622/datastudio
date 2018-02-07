#include "precomp.h"
#include "DetailTable.h"

namespace Edit {

DetailTable::DetailTable(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    d_tableView = new JTableView(this);
    vertLayoutMain->addWidget(d_tableView);

    //

    d_tableView->setHorizontalHeaderLabels({"item1", "item2"});
}

}
