#include "precomp.h"
#include "axissel_widget.h"
#include "icdwidget/icdwidget_global.h"

// class AxisXDropLineEdit

AxisXDropLineEdit::AxisXDropLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    setAcceptDrops(true);
    setReadOnly(true);

    setMinimumHeight(40);
}

void AxisXDropLineEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("icd/table-drag/analyse")) {
        event->accept();
    } else {
        event->ignore();
    }
}

void AxisXDropLineEdit::dropEvent(QDropEvent *event)
{
    //
    if (!event->mimeData()->hasFormat("icd/table-drag/analyse")) {
        return;
    }

    //
    QStandardItem *item = reinterpret_cast<QStandardItem *>
            (event->mimeData()->property("item").value<void *>());
    if (!item) {
        return;
    }

    //
    const int itemType = item->type();
    if (itemType != Icd::TreeItemTypeDataItem) {
        return;
    }

    setText(item->text().remove(QRegExp("<([^>]*)>")));
    setProperty("domain", item->data(Icd::TreeItemDomainRole));
}

// class AxisYDropListWidget

AxisYDropListWidget::AxisYDropListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DropOnly);
}

void AxisYDropListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("icd/table-drag/analyse")) {
        if (count() >= 20) {    //
            event->ignore();
        } else {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
    } else {
        event->ignore();
    }
}

void AxisYDropListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("icd/table-drag/analyse")) {
        if (count() >= 20) {    //
            event->ignore();
        } else {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
    } else {
        event->ignore();
    }
}

void AxisYDropListWidget::dropEvent(QDropEvent *event)
{
    //
    if (!event->mimeData()->hasFormat("icd/table-drag/analyse")) {
        return;
    }

    //
    QStandardItem *item = reinterpret_cast<QStandardItem *>
            (event->mimeData()->property("item").value<void *>());
    if (!item) {
        return;
    }

    //
    const int itemType = item->type();
    if (itemType != Icd::TreeItemTypeDataItem) {
        return;
    }

    const QString text = item->text().remove(QRegExp("<([^>]*)>"));
    const QList<QListWidgetItem*> listItems = findItems(text, Qt::MatchFixedString);
    if (!listItems.isEmpty()) {
        return;
    }
    QListWidgetItem *listItem = new QListWidgetItem(text);
    listItem->setData(Icd::TreeItemDomainRole, item->data(Icd::TreeItemDomainRole));
    addItem(listItem);
}

// class AxisSelWidget

AxisSelWidget::AxisSelWidget(QWidget *parent)
    : QDialog(parent)
    , d_hasTimeFormat(false)
{
    setWindowTitle(tr("Axis settings"));

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    vertLayoutMain->addLayout(formLayout);

    QHBoxLayout *horiLayoutAxisX = new QHBoxLayout();
    formLayout->addRow(tr("Horizontal coordinate (X):"), horiLayoutAxisX);

    d_editAxisX = new AxisXDropLineEdit(this);
    d_editAxisX->setPlaceholderText(tr("Drag data item to here"));
    horiLayoutAxisX->addWidget(d_editAxisX);

    d_comboBoxAxisX = new QComboBox(this);
    d_comboBoxAxisX->setMinimumWidth(80);
    d_comboBoxAxisX->addItem(tr("Time"));
    d_comboBoxAxisX->addItem(tr("Custom"));
    horiLayoutAxisX->addWidget(d_comboBoxAxisX);

    QGroupBox *groupBoxAxisY = new QGroupBox(tr("Vertical coordinate(Y)"), this);
    vertLayoutMain->addWidget(groupBoxAxisY);

    QHBoxLayout *horiLayoutAxisY = new QHBoxLayout(groupBoxAxisY);

    d_listWidgetAxisY = new AxisYDropListWidget(this);
    horiLayoutAxisY->addWidget(d_listWidgetAxisY);

    QHBoxLayout *horiLayoutButtons = new QHBoxLayout();
    vertLayoutMain->addLayout(horiLayoutButtons);

    horiLayoutButtons->addStretch();

    QPushButton *buttonOk = new QPushButton(tr("Ok"), this);
    QPushButton *buttonCancel = new QPushButton(tr("Cancel"), this);
    buttonOk->setFixedSize(120, 30);
    buttonCancel->setFixedSize(120, 30);
    horiLayoutButtons->addWidget(buttonOk);
    horiLayoutButtons->addWidget(buttonCancel);

    //
    connect(buttonOk, &QPushButton::clicked, this, &AxisSelWidget::accept);
    connect(buttonCancel, &QPushButton::clicked, this, &AxisSelWidget::reject);

    //
    /*QFile file(":/icd/qss/default.qss");
    if (file.open(QFile::ReadOnly)) {
        setStyleSheet(file.readAll());
    }*/
}

AxisSelWidget::~AxisSelWidget()
{

}

void AxisSelWidget::setHasTimeFormat(bool valid)
{
    d_hasTimeFormat = valid;
}

QVariant::Type AxisSelWidget::axisXType() const
{
    if (d_comboBoxAxisX->currentIndex() == 0) {
        return QVariant::DateTime;
    } else {
        return QVariant::Double;
    }
}

QString AxisSelWidget::axisXDomain() const
{
    return d_editAxisX->property("domain").toString();
}

QStringList AxisSelWidget::axisYDomain() const
{
    QStringList domains;
    int count = d_listWidgetAxisY->count();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem *item = d_listWidgetAxisY->item(i);
        domains.append(item->data(Icd::TreeItemDomainRole).toString());
    }
    return domains;
}

void AxisSelWidget::accept()
{
    if (!d_hasTimeFormat && d_editAxisX->text().isEmpty()) {
        d_editAxisX->setFocus();
        return;
    }

    if (d_listWidgetAxisY->count() == 0) {
        d_listWidgetAxisY->setFocus();
        return;
    }

    QDialog::accept();
}
