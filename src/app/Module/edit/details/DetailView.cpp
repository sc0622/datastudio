#include "precomp.h"
#include "DetailView.h"
#include "DetailTable.h"
#include "DetailItemEdit.h"

namespace Edit {

DetailView::DetailView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    d_splitterMain = new JSplitter({3, 1}, Qt::Vertical, this);
    d_splitterMain->setObjectName("Edit::detail::splitterMain");
    vertLayoutMain->addWidget(d_splitterMain);

    d_detailTable = new DetailTable(this);
    d_splitterMain->addWidget(d_detailTable);

    d_detailItemEdit = new DetailItemEdit(this);
    d_splitterMain->addWidget(d_detailItemEdit);
}

DetailView::~DetailView()
{
    JMain::saveWidgetState(d_splitterMain);
}

bool DetailView::init()
{
    bool result = true;

    JMain::restoreWidgetState(d_splitterMain);

    return result;
}

bool DetailView::updateView(const Icd::ObjectPtr &object)
{
    return true;
}

}
