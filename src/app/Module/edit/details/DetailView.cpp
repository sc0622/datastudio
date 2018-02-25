#include "precomp.h"
#include "DetailView.h"
#include "DetailTable.h"
#include "DetailItemEdit.h"

namespace Edit {

DetailView::DetailView(QWidget *parent)
    : QWidget(parent)
    , d_currentData({nullptr, nullptr})
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

    connect(d_detailTable, &DetailTable::currentRowChanged,
            this, &DetailView::onCurrentRowChanged);

    d_detailItemEdit->hide();
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

void DetailView::updateView(QStandardItem *item)
{
    d_detailTable->resetView();
    d_detailItemEdit->resetView();

    d_currentData.item = item;
    d_currentData.object = nullptr;

    if (!item) {
        return;
    }

    const Icd::ParserPtr parser = JMain::instance()->parser("edit");
    if (!parser) {
        return;
    }

    int objectType = Icd::ObjectInvalid;
    switch (item->type()) {
    case Icd::TreeItemTypeRoot: objectType = Icd::ObjectRoot; break;
    case Icd::TreeItemTypeVehicle: objectType = Icd::ObjectVehicle; break;
    case Icd::TreeItemTypeSystem: objectType = Icd::ObjectSystem; break;
    case Icd::TreeItemTypeTable: objectType = Icd::ObjectTable; break;
    case Icd::TreeItemTypeDataItem: objectType = Icd::ObjectItem; break;
    case Icd::TreeItemTypeItemTable: objectType = Icd::ObjectItem; break;
    //case Icd::TreeItemTypeItemBitMap: objectType = Icd::ObjectItem; break;
    default:
        break;
    }

    if (objectType == Icd::ObjectInvalid) {
        return;
    }

    const QString &domain = item->data(Icd::TreeItemDomainRole).toString();

    Icd::ObjectPtr object = parser->parse(domain.toStdString(), objectType, 1);
    if (!object) {
        return;
    }

    d_detailTable->updateView(object);
    d_detailItemEdit->updateView(object);
}

void DetailView::onCurrentRowChanged(int currentRow)
{
    Q_UNUSED(currentRow);
}

}
