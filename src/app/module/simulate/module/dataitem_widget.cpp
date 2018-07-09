#include "precomp.h"
#include "dataitem_widget.h"
#include "icdcore/icdcore_inc.h"
#include "icdworker/icdworker.h"
#include "icdtab_widget.h"
#include "datatable_widget.h"
#include "jutraledit/jutraledit_view.h"

namespace Simulate {

// class DataItemWidget

DataItemWidget::DataItemWidget(ItemType itemType, QWidget *parent)
    : QFrame(parent)
    , d_itemType(itemType)
    , d_item(nullptr)
    , d_itemLayout(nullptr)
    , d_buttonSend(nullptr)
{
    setObjectName("DataItemWidget");
    setFrameShape(QFrame::NoFrame);

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(6, 3, 8, 0);
    vertLayoutMain->setSpacing(0);

    QHBoxLayout *layoutTop = new QHBoxLayout();
    vertLayoutMain->addLayout(layoutTop);

    d_labelTitle = new QLabel(this);
    d_labelTitle->setObjectName("labelTitle");
    d_labelTitle->setContentsMargins(8, 0, 8, 0);
    d_labelTitle->setFixedHeight(25);
    QFont font = d_labelTitle->font();
    font.setBold(true);
    d_labelTitle->setFont(font);
    layoutTop->addWidget(d_labelTitle, 0, Qt::AlignTop);

    layoutTop->addStretch();

    d_clientLayout = new QHBoxLayout();
    d_clientLayout->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->addLayout(d_clientLayout);

    switch (d_itemType) {
    case ItemTypeComplex:
    {
        const Icd::ComplexItemPtr complex = JHandlePtrCast<Icd::ComplexItem, Icd::Item>(dataItem());
        if (!complex) {
            break;
        }
        if (complex->childCount() > 0) {
            break;
        }
    }
    case ItemTypeFrame:
        break;
    default:
    {
        QFrame *formButton = new QFrame(this);
        formButton->setObjectName("frameButton");
        formButton->setFixedWidth(100);
        layoutTop->addWidget(formButton);

        d_buttonSend = new QPushButton(QStringLiteral("发送"), this);
        d_buttonSend->setFixedSize(60, 25);
        layoutTop->addWidget(d_buttonSend);

        layoutTop->addSpacing(10);

        QPushButton *buttonRestore = new QPushButton(QStringLiteral("复位"), this);
        buttonRestore->setFixedSize(60, 25);
        layoutTop->addWidget(buttonRestore);

        layoutTop->addSpacing(10);

        QPushButton *buttonRemove = new QPushButton(QStringLiteral("删除"), this);
        buttonRemove->setFixedSize(60, 25);
        layoutTop->addWidget(buttonRemove);

        QWidget *widgetClient = new QWidget(this);
        widgetClient->setObjectName("widgetClient");
        d_clientLayout->addWidget(widgetClient);

        QHBoxLayout *horiLayoutClient = new QHBoxLayout(widgetClient);
        horiLayoutClient->setContentsMargins(0, 6, 0, 6);

        d_itemLayout = new QVBoxLayout();
        horiLayoutClient->addLayout(d_itemLayout);
        d_itemLayout->setContentsMargins(0, 0, 0, 0);
        d_itemLayout->setSpacing(6);
        d_itemLayout->addStretch();

        horiLayoutClient->addStretch();

        connect(buttonRemove, SIGNAL(clicked(bool)), this, SIGNAL(remove()));
        connect(d_buttonSend, SIGNAL(clicked(bool)), this, SIGNAL(send()));
        connect(buttonRestore, &QPushButton::clicked, this, [=](){
            restoreUi(d_defaultData);
        });

        break;
    }}
}

DataItemWidget::~DataItemWidget()
{
    //
    if (d_item) {
        d_item->setData(QVariant::Invalid, Icd::TreeBoundRole);
    }
    //
    if (d_worker) {
        d_worker->disconnect(this);
        d_worker->workerSend()->disconnect(this);
    }
}

DataItemWidget *DataItemWidget::createWidget(const Icd::ItemPtr &dataItem, QWidget *parent)
{
    DataItemWidget *itemWidget = nullptr;
    switch (dataItem->type()) {
    case Icd::ItemHead:
        itemWidget = new ItemWidgetHead(parent);
        break;
    case Icd::ItemCounter:
        itemWidget = new ItemWidgetCounter(parent);
        break;
    case Icd::ItemCheck:
        itemWidget = new ItemWidgetCheck(parent);
        break;
    case Icd::ItemFrameCode:
        itemWidget = new ItemWidgetFrameCode(parent);
        break;
    case Icd::ItemNumeric:
        itemWidget = new ItemWidgetNumeric(parent);
        break;
    case Icd::ItemArray:
        itemWidget = new ItemWidgetArray(parent);
        break;
    case Icd::ItemBitMap:
        itemWidget = new ItemWidgetBitMap(parent);
        break;
    case Icd::ItemBitValue:
        itemWidget = new ItemWidgetBitValue(parent);
        break;
    case Icd::ItemComplex:
        itemWidget = new ItemWidgetComplex(parent);
        break;
    case Icd::ItemFrame:
        itemWidget = new ItemWidgetFrame(parent);
        break;
    default:
        break;
    }
    //
    if (itemWidget) {
        if (!itemWidget->setDataItem(dataItem)) {
            itemWidget->deleteLater();
            return nullptr;
        }
    }

    return itemWidget;
}

DataItemWidget::ItemType DataItemWidget::itemType() const
{
    return d_itemType;
}

const Icd::WorkerPtr &DataItemWidget::worker() const
{
    return d_worker;
}

void DataItemWidget::setWorker(const Icd::WorkerPtr &worker)
{
    // the same
    if (worker && (d_worker == worker)) {
        return;
    }
    //
    if (d_worker) {
        d_worker->disconnect(this);
        d_worker->workerSend()->disconnect(this);
    }
    //
    if (worker) {
        auto setButtonSendEnabled = [=](Icd::WorkerTrans::TimeEvent event){
            if (d_buttonSend) {
                switch (event) {
                case Icd::WorkerTrans::TimeOneShot:
                    d_buttonSend->setEnabled(worker->isOpen());
                    break;
                case Icd::WorkerTrans::TimePeriodic:
                    d_buttonSend->setEnabled(false);
                    break;
                default:
                    break;
                }
            }
        };

        setButtonSendEnabled(worker->workerSend()->timeEvent());

        connect(worker.get(), &Icd::Worker::toggled, this,
                [=](){
            setButtonSendEnabled(worker->workerSend()->timeEvent());
        });
        connect(worker->workerSend().get(), &Icd::WorkerSend::timeEventChanged, this,
                [=](Icd::WorkerTrans::TimeEvent event){
            setButtonSendEnabled(event);
        });
    }

    d_worker = worker;
}

Icd::ItemPtr DataItemWidget::dataItem() const
{
    return d_dataItem;
}

bool DataItemWidget::setDataItem(const Icd::ItemPtr &dataItem)
{
    //
    if (!dataItem) {
        return false;
    }

    d_defaultData = Icd::ItemPtr(dynamic_cast<Icd::Item *>(dataItem->clone()));
    d_dataItem = dataItem;

    return true;
}

QStandardItem *DataItemWidget::item() const
{
    return d_item;
}

void DataItemWidget::setItem(QStandardItem *item)
{
    d_item = item;
}

void DataItemWidget::updateUi()
{
    updateUi(d_dataItem);
}

bool DataItemWidget::addDataItem(const QString &domain)
{
    Q_UNUSED(domain);
    return true;
}

int DataItemWidget::focusItem(const QString &domain)
{
    Q_UNUSED(domain);
    return 0;
}

void DataItemWidget::removeItem(const QString &domain)
{
    Q_UNUSED(domain);
    //
    if (d_item) {
        d_item->setData(QVariant::Invalid, Icd::TreeBoundRole);
    }
}

QHBoxLayout *DataItemWidget::clientLayout()
{
    return d_clientLayout;
}

QVBoxLayout *DataItemWidget::itemLayout()
{
    return d_itemLayout;
}

int DataItemWidget::titleBarHeight() const
{
    return d_labelTitle->height();
}

void DataItemWidget::restoreUi(const Icd::ItemPtr &data)
{
    if (!data) {
        return;
    }
    //
}

bool DataItemWidget::updateUi(const Icd::ItemPtr &data)
{
    if (!data) {
        return false;
    }
    //
    if (d_item) {
        const QString path = d_item->data(Icd::TreeItemPathRole).toString();
        const QString name = path.section('@', 0, 0);
        d_labelTitle->setText("<font color=#7777bb>" + name + "</font><font color=#807777aa size=2>@"
                              + path.section('@', 1) + "</font>");
    } else {
        d_labelTitle->setText(QString::fromStdString(data->name()));
    }

    if (d_worker) {

    }

    return true;
}

void DataItemWidget::resizeEvent(QResizeEvent *event)
{
    if (event->oldSize().height() != event->size().height()) {
        emit heightChanged(event->size().height());
    }

    QFrame::resizeEvent(event);
}

// class ItemWidgetHead

ItemWidgetHead::ItemWidgetHead(QWidget *parent)
    : DataItemWidget(ItemTypeHead, parent)
{
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    itemLayout()->addLayout(formLayout);
    itemLayout()->addStretch();

    d_spinValue = new JSpinBox(this);
    d_spinValue->setFillChar(QChar('0'));
    d_spinValue->setRadix(16);
    d_spinValue->setFixedWidth(300);
    formLayout->addRow(QStringLiteral("当前数值："), d_spinValue);
    //
    connect(d_spinValue, static_cast<void(JSpinBox::*)(int)>
            (&JSpinBox::valueChanged), this, [=](int value){
        if (!dataItem()) {
            return;
        }
        dataItem()->setData(value);
    });
}

ItemWidgetHead::~ItemWidgetHead()
{

}

void ItemWidgetHead::restoreUi(const Icd::ItemPtr &data)
{
    //
    DataItemWidget::restoreUi(data);
    //
    const Icd::HeaderItemPtr itemHead = JHandlePtrCast<Icd::HeaderItem, Icd::Item>(data);
    if (!itemHead) {
        return;
    }
    //
    d_spinValue->setValue((int)itemHead->defaultValue());
}

bool ItemWidgetHead::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }

    const Icd::HeaderItemPtr itemHead = JHandlePtrCast<Icd::HeaderItem, Icd::Item>(data);
    if (!itemHead) {
        return false;
    }

    d_spinValue->setRange(0, (0x1U << (int)itemHead->bufferSize() * 8) - 1);
    d_spinValue->setValue((int)itemHead->data());

    return true;
}

// class ItemWidgetCounter

ItemWidgetCounter::ItemWidgetCounter(QWidget *parent)
    : DataItemWidget(ItemTypeCounter, parent)
{
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    itemLayout()->addLayout(formLayout);
    itemLayout()->addStretch();

    d_spinValue = new QSpinBox(this);
    d_spinValue->setFixedWidth(300);
    formLayout->addRow(QStringLiteral("当前数值："), d_spinValue);
    //
    connect(d_spinValue, static_cast<void(QSpinBox::*)(int)>
            (&QSpinBox::valueChanged), this, [=](int value){
        if (!dataItem()) {
            return;
        }
        dataItem()->setData(value);
    });
}

ItemWidgetCounter::~ItemWidgetCounter()
{

}

void ItemWidgetCounter::restoreUi(const Icd::ItemPtr &data)
{
    //
    DataItemWidget::restoreUi(data);
    //
    const Icd::CounterItemPtr itemCounter = JHandlePtrCast<Icd::CounterItem, Icd::Item>(data);
    if (!itemCounter) {
        return;
    }
    //
    d_spinValue->setValue((int)itemCounter->defaultValue());
}

bool ItemWidgetCounter::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }

    const Icd::CounterItemPtr itemCounter = JHandlePtrCast<Icd::CounterItem, Icd::Item>(data);
    if (!itemCounter) {
        return false;
    }
    //
    d_spinValue->setRange(0, (0x1U << int(itemCounter->bufferSize() * 8)) - 1);
    d_spinValue->setValue((int)itemCounter->data());

    return true;
}

// class ItemWidgetCheck

ItemWidgetCheck::ItemWidgetCheck(QWidget *parent)
    : DataItemWidget(ItemTypeCheck, parent)
{
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    itemLayout()->addLayout(formLayout);
    itemLayout()->addStretch();

    d_spinValue = new QSpinBox(this);
    d_spinValue->setFixedWidth(300);
    formLayout->addRow(QStringLiteral("当前数值："), d_spinValue);

    d_comboBoxCheckType = new QComboBox(this);
    d_comboBoxCheckType->setMinimumWidth(220);
    formLayout->addRow(QStringLiteral("校验类型："), d_comboBoxCheckType);

    d_spinStartPos = new QSpinBox(this);
    d_spinStartPos->setFixedWidth(300);
    d_spinStartPos->setRange(0, 65535);
    d_spinStartPos->setSuffix(QStringLiteral(" 字节"));
    formLayout->addRow(QStringLiteral("起始位置："), d_spinStartPos);

    d_spinEndPos = new QSpinBox(this);
    d_spinEndPos->setFixedWidth(300);
    d_spinEndPos->setRange(0, 65535);
    d_spinEndPos->setSuffix(QStringLiteral(" 字节"));
    formLayout->addRow(QStringLiteral("终止位置："), d_spinEndPos);

    d_labelCheckLength = new QLabel(this);
    formLayout->addRow(QStringLiteral("校验长度："), d_labelCheckLength);
    //
    d_comboBoxCheckType->addItem(QStringLiteral("无校验"));
    d_comboBoxCheckType->addItem(QStringLiteral("8位和校验"));
    d_comboBoxCheckType->addItem(QStringLiteral("16位和校验"));
    d_comboBoxCheckType->addItem(QStringLiteral("CRC8校验"));
    d_comboBoxCheckType->addItem(QStringLiteral("CRC16校验"));
    //
    connect(d_spinValue, static_cast<void(QSpinBox::*)(int)>
            (&QSpinBox::valueChanged), this, [=](int value){
        if (!dataItem()) {
            return;
        }
        dataItem()->setData(value);
    });
    connect(d_comboBoxCheckType, static_cast<void(QComboBox::*)(int)>
            (&QComboBox::currentIndexChanged), this, [=](int index){
        //
        const Icd::CheckItemPtr itemCheck = JHandlePtrCast<Icd::CheckItem, Icd::Item>(dataItem());
        if (!itemCheck) {
            return;
        }
        //
        switch (index) {
        case 0:
        default:
            itemCheck->setCheckType(Icd::CheckNone);
            break;
        case 1:
            itemCheck->setCheckType(Icd::CheckSum8);
            break;
        case 2:
            itemCheck->setCheckType(Icd::CheckSum16);
            break;
        case 3:
            itemCheck->setCheckType(Icd::CheckCrc8);
            break;
        case 4:
            itemCheck->setCheckType(Icd::CheckCrc16);
            break;
        }
    });
    const QString sByteUnit = QStringLiteral(" 字节");
    connect(d_spinStartPos, static_cast<void(QSpinBox::*)(int)>
            (&QSpinBox::valueChanged), this, [=](int value){
        //
        const Icd::CheckItemPtr itemCheck = JHandlePtrCast<Icd::CheckItem, Icd::Item>(dataItem());
        if (!itemCheck) {
            return;
        }
        itemCheck->setStartPos(value);
        // CheckLength
        d_labelCheckLength->setText(QString::number(itemCheck->checkLength()) + sByteUnit);
        //
        if (value > d_spinEndPos->value()) {
            d_spinStartPos->setValue(d_spinEndPos->value());
        }
    });
    connect(d_spinEndPos, static_cast<void(QSpinBox::*)(int)>
            (&QSpinBox::valueChanged), this, [=](int value){
        //
        const Icd::CheckItemPtr itemCheck = JHandlePtrCast<Icd::CheckItem, Icd::Item>(dataItem());
        if (!itemCheck) {
            return;
        }
        itemCheck->setEndPos(value);
        // CheckLength
        d_labelCheckLength->setText(QString::number(itemCheck->checkLength()) + sByteUnit);
        //
        if (value < d_spinStartPos->value()) {
            d_spinEndPos->setValue(d_spinStartPos->value());
        }
    });
}

ItemWidgetCheck::~ItemWidgetCheck()
{

}

void ItemWidgetCheck::restoreUi(const Icd::ItemPtr &data)
{
    //
    DataItemWidget::restoreUi(data);
    //
    const Icd::CheckItemPtr itemCheck = JHandlePtrCast<Icd::CheckItem, Icd::Item>(data);
    if (!itemCheck) {
        return;
    }
    //
    d_spinValue->setValue(itemCheck->defaultValue());
}

bool ItemWidgetCheck::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }
    //
    d_spinValue->setValue(0);
    d_comboBoxCheckType->setCurrentIndex(0);
    d_spinStartPos->setValue(0);
    d_spinEndPos->setValue(0);
    d_labelCheckLength->setText("");
    //
    if (!worker()) {
        return false;
    }
    //
    const Icd::TablePtr tableSend = worker()->workerSend()->table();
    if (!tableSend) {
        return false;
    }
    //
    d_spinStartPos->setRange(0, tableSend->bufferSize() - 1);
    d_spinEndPos->setRange(0, tableSend->bufferSize() - 1);
    //
    const Icd::CheckItemPtr checkItem = JHandlePtrCast<Icd::CheckItem, Icd::Item>(data);
    if (!checkItem) {
        return false;
    }
    // value
    d_spinValue->setValue(checkItem->data());
    // CheckType
    switch (checkItem->checkType()) {
    case Icd::CheckNone:
    default:
        d_comboBoxCheckType->setCurrentIndex(0);
        break;
    case Icd::CheckSum8:
        d_comboBoxCheckType->setCurrentIndex(1);
        break;
    case Icd::CheckSum16:
        d_comboBoxCheckType->setCurrentIndex(2);
        break;
    case Icd::CheckCrc8:
        d_comboBoxCheckType->setCurrentIndex(3);
        break;
    case Icd::CheckCrc16:
        d_comboBoxCheckType->setCurrentIndex(4);
        break;
    }
    // EndPos
    d_spinEndPos->setValue(checkItem->endPos());
    // StartPos
    d_spinStartPos->setValue(checkItem->startPos());
    // CheckLength
    d_labelCheckLength->setText(QString::number(checkItem->checkLength())
                                + QStringLiteral(" 字节"));

    return true;
}

// class ItemWidgetFrameCode

ItemWidgetFrameCode::ItemWidgetFrameCode(QWidget *parent)
    : DataItemWidget(ItemTypeFrameCode, parent)
{
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    itemLayout()->addLayout(formLayout);
    itemLayout()->addStretch();

    d_comboBoxCode = new QComboBox(this);
    d_comboBoxCode->setFixedWidth(300);
    formLayout->addRow(QStringLiteral("当前帧码："), d_comboBoxCode);
    //
    connect(d_comboBoxCode, static_cast<void(QComboBox::*)
            (int)>(&QComboBox::currentIndexChanged), this, [=](int index){
        if (!dataItem()) {
            return;
        }
        dataItem()->setData(d_comboBoxCode->itemData(index).toInt());
    });
}

ItemWidgetFrameCode::~ItemWidgetFrameCode()
{

}

void ItemWidgetFrameCode::setWorker(const Icd::WorkerPtr &worker)
{
    if (worker && (this->worker() == worker)) {
        return;
    }

    if (this->worker()) {
        this->worker()->workerSend()->disconnect(this);
    }

    DataItemWidget::setWorker(worker);

    if (worker) {
        connect(worker->workerSend().get(), &Icd::WorkerSend::timeEventChanged, this,
                [=](Icd::WorkerTrans::TimeEvent event){
            switch (event) {
            case Icd::WorkerTrans::TimeOneShot:
                d_comboBoxCode->setEnabled(true);
                break;
            case Icd::WorkerTrans::TimePeriodic:
                d_comboBoxCode->setEnabled(false);
                break;
            default:
                break;
            }
        });
    }
}

void ItemWidgetFrameCode::restoreUi(const Icd::ItemPtr &data)
{
    //
    DataItemWidget::restoreUi(data);
    //
    const Icd::FrameCodeItemPtr itemFrameCode = JHandlePtrCast<Icd::FrameCodeItem, Icd::Item>(data);
    if (!itemFrameCode) {
        return;
    }
    //
    d_comboBoxCode->setCurrentText(QString("0x%1").arg((int)itemFrameCode->defaultValue(),
                                                       2, 16, QChar('0')).toUpper());
}

bool ItemWidgetFrameCode::updateUi(const Icd::ItemPtr &data)
{
    //
    d_comboBoxCode->clear();

    if (!DataItemWidget::updateUi(data)) {
        return false;
    }
    //
    const Icd::FrameCodeItemPtr itemFrameCode = JHandlePtrCast<Icd::FrameCodeItem, Icd::Item>(data);
    if (!itemFrameCode) {
        return false;
    }
    //
    const Icd::FrameItemPtr frame = itemFrameCode->frame();
    if (!frame) {
        return true;
    }
    //
    const Icd::TablePtrMap &tables = frame->allTable();
    for (Icd::TablePtrMap::const_iterator citer = tables.cbegin();
         citer != tables.cend(); ++citer) {
        d_comboBoxCode->addItem(QString::fromStdString(citer->second->name()), citer->first);
    }
    //
    d_comboBoxCode->setCurrentText(QString("0x%1").arg((int)itemFrameCode->data(),
                                                       2, 16, QChar('0')).toUpper());

    return true;
}

// class ItemWidgetNumeric

ItemWidgetNumeric::ItemWidgetNumeric(QWidget *parent)
    : DataItemWidget(ItemTypeNumeric, parent)
{
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    itemLayout()->addLayout(formLayout);
    itemLayout()->addStretch();

    d_checkBoxLink = new QCheckBox(QStringLiteral("（原始数值与输出数值联动）"), this);
    d_checkBoxLink->setMinimumWidth(300);
    d_checkBoxLink->setCheckable(false);
    d_checkBoxLink->setChecked(true);   //FIXME
    formLayout->addRow(QStringLiteral("数值关联："), d_checkBoxLink);

    d_sliderValue = new QSlider(this);
    d_sliderValue->setMinimumWidth(300);
    d_sliderValue->setOrientation(Qt::Horizontal);
    formLayout->addRow(QStringLiteral("原始数值："), d_sliderValue);

    d_spinValue = new QDoubleSpinBox(this);
    d_spinValue->setMinimumWidth(300);
    d_spinValue->setDecimals(DBL_DIG);
    formLayout->addRow(QStringLiteral("原始数值："), d_spinValue);

    d_spinData = new QDoubleSpinBox(this);
    d_spinData->setMinimumWidth(300);
    d_spinData->setDecimals(DBL_DIG);
    d_spinData->setEnabled(false);
    formLayout->addRow(QStringLiteral("输出数值："), d_spinData);

    d_labelDesc = new QLabel(this);
    formLayout->addRow(QStringLiteral("描述信息："), d_labelDesc);
    //
    connect(d_sliderValue, &QSlider::valueChanged, [=](int value){
        d_spinValue->setValue(value);
    });
    connect(d_spinValue, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double value){
        d_sliderValue->setValue((int)value);
    });
    auto setDataSuffix = [=](const Icd::NumericItemPtr &itemNumeric){
        QString suffix;
        double data = (itemNumeric->data() - itemNumeric->offset()) / itemNumeric->scale();
        switch (itemNumeric->numericType()) {
        case Icd::NumericI8:
        case Icd::NumericI16:
        case Icd::NumericI32:
        case Icd::NumericI64:
        {
            qlonglong iData = qlonglong(data);
            iData &= (1ll << ((int)itemNumeric->bufferSize() << 3)) - 1;
            suffix = QString(" (%1)").arg(iData, int(itemNumeric->bufferSize() * 2),
                                          16, QChar('0')).toUpper();
            break;
        }
        case Icd::NumericU8:
        case Icd::NumericU16:
        case Icd::NumericU32:
        case Icd::NumericU64:
        {
            qulonglong uData = qulonglong(data);
            uData &= (1ull << ((int)itemNumeric->bufferSize() << 3)) - 1;
            suffix = QString(" (%1)").arg(uData, int(itemNumeric->bufferSize() * 2),
                                          16, QChar('0')).toUpper();
            break;
        }
        case Icd::NumericF32:
        {
            float _data = float(data);
            qint32 iData = *(qint32*)&_data;
            suffix = QString(" (%1)").arg(iData, int(itemNumeric->bufferSize() * 2),
                                          16, QChar('0')).toUpper();
            break;
        }
        case Icd::NumericF64:
        {
            qlonglong iData = *(qlonglong*)&data;
            suffix = QString(" (%1)").arg(iData, int(itemNumeric->bufferSize() * 2),
                                          16, QChar('0')).toUpper();
            break;
        }
        default:
        {
            qulonglong uData = qulonglong(data);
            uData &= (1ui64 << ((int)itemNumeric->bufferSize() << 3)) - 1;
            suffix = QString(" (%1)").arg(uData, int(itemNumeric->bufferSize() * 2),
                                          16, QChar('0')).toUpper();
            break;
        }}
        d_spinData->setSuffix(suffix);
    };
    connect(d_spinValue, static_cast<void(QDoubleSpinBox::*)(double)>
            (&QDoubleSpinBox::valueChanged), this, [=](double value){
        const Icd::NumericItemPtr itemNumeric = JHandlePtrCast<Icd::NumericItem, Icd::Item>(dataItem());
        if (!itemNumeric) {
            return;
        }
        itemNumeric->setData(value);
        const double dataValue = (value - itemNumeric->offset()) / itemNumeric->scale();
        d_spinData->setValue(dataValue);
        setDataSuffix(itemNumeric);
    });
    connect(d_spinData, static_cast<void(QDoubleSpinBox::*)(double)>
            (&QDoubleSpinBox::valueChanged), this, [=](double value){
        const Icd::NumericItemPtr itemNumeric = JHandlePtrCast<Icd::NumericItem, Icd::Item>(dataItem());
        if (!itemNumeric) {
            return;
        }
        const double realValue = value * itemNumeric->scale() + itemNumeric->offset();
        itemNumeric->setData(realValue);
        setDataSuffix(itemNumeric);
        if (!d_checkBoxLink->isChecked()) {
            return;
        }
        d_spinValue->setValue(realValue);
    });
    connect(d_checkBoxLink, &QCheckBox::stateChanged, this, [=](int state){
        if (state == Qt::Checked) {
            d_spinData->setEnabled(true);
        } else {
            d_spinData->setEnabled(false);
        }
    });

    //
    d_checkBoxLink->setChecked(false);
}

ItemWidgetNumeric::~ItemWidgetNumeric()
{

}

void ItemWidgetNumeric::restoreUi(const Icd::ItemPtr &data)
{
    //
    DataItemWidget::restoreUi(data);
    //
    const Icd::NumericItemPtr itemNumeric = JHandlePtrCast<Icd::NumericItem, Icd::Item>(data);
    if (!itemNumeric) {
        return;
    }
    //
    d_spinValue->setValue(itemNumeric->defaultValue());
}

bool ItemWidgetNumeric::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }
    //
    const Icd::NumericItemPtr itemNumeric = JHandlePtrCast<Icd::NumericItem, Icd::Item>(data);
    if (!itemNumeric) {
        return false;
    }
    //
    const Icd::LimitItemPtr &limit = itemNumeric->limit();
    if (!limit) {
        return false;
    }
    //
    switch (itemNumeric->numericType()) {
    case Icd::NumericF32:
        d_spinValue->setDecimals(FLT_DIG);
        d_spinData->setDecimals(FLT_DIG);
        break;
    default:
        d_spinData->setDecimals(DBL_DIG);
        d_spinValue->setDecimals(DBL_DIG);
        break;
    }
    // unit
    const QString unit = QString::fromStdString(itemNumeric->unit()).trimmed();
    if (!unit.isEmpty()) {
        d_spinValue->setSuffix(" " + unit);
    }
    // range
    std::pair<double, double> range = itemNumeric->valueRange();
    //
    d_spinValue->setRange(range.first, range.second);
    d_spinValue->setSingleStep(itemNumeric->scale());
    //
    if (range.first < INT_MIN) {
        range.first = INT_MIN;
    }
    if (range.second > INT_MAX) {
        range.second = INT_MAX;
    }
    d_sliderValue->setRange(qFloor(range.first), qCeil(range.second));
    //
    range = itemNumeric->dataRange();
    d_spinData->setRange(range.first, range.second);
    //
    d_spinValue->setValue(itemNumeric->data());
    //
    d_labelDesc->setText(QStringLiteral("比例：%1，偏移：%2，范围：%3，单位：%4，占用字节数：%5。")
                         .arg(itemNumeric->scale(), 0, 'g', 20)
                         .arg(itemNumeric->offset(), 0, 'g', 20)
                         .arg(QString::fromStdString(limit->toString()))
                         .arg(unit.isEmpty() ? "<?>" : unit)
                         .arg(itemNumeric->bufferSize()));

    return true;
}

// class ItemWidgetArray

ItemWidgetArray::ItemWidgetArray(QWidget *parent)
    : DataItemWidget(ItemTypeArray, parent)
{
    dataView_ = new JUtralEdit::JView(this);
    dataView_->setEditable(true);
    dataView_->setShowType(JUtralEdit::JView::ShowAsString);
    itemLayout()->addWidget(dataView_);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    itemLayout()->addLayout(formLayout);

    clientLayout()->setStretch(0, 0);

    d_labelDesc = new QLabel(this);
    formLayout->addRow(QStringLiteral("描述信息："), d_labelDesc);

    connect(dataView_, &JUtralEdit::JView::textChanged, this, [=](){
        const Icd::ArrayItemPtr arrayItem = JHandlePtrCast<Icd::ArrayItem, Icd::Item>(dataItem());
        if (!arrayItem) {
            return;
        }
        //
        switch (dataView_->showType()) {
        case JUtralEdit::JView::ShowAsHexData:
        {
            const QByteArray hexData = dataView_->hexData();
            arrayItem->setData(hexData.constData(), hexData.size());
            break;
        }
        case JUtralEdit::JView::ShowAsString:
        {
            const QByteArray text = dataView_->text().toLocal8Bit();
            arrayItem->setData(text.constData(), text.size());
            break;
        }
        default:
            break;
        }
    });
}

ItemWidgetArray::~ItemWidgetArray()
{

}

void ItemWidgetArray::restoreUi(const Icd::ItemPtr &data)
{
    //
    DataItemWidget::restoreUi(data);
    //
    const Icd::ArrayItemPtr arrayItem = JHandlePtrCast<Icd::ArrayItem, Icd::Item>(data);
    if (!arrayItem) {
        return;
    }
}

bool ItemWidgetArray::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }
    //
    const Icd::ArrayItemPtr arrayItem = JHandlePtrCast<Icd::ArrayItem, Icd::Item>(data);
    if (!arrayItem) {
        return false;
    }
    //
    QString text(QStringLiteral("占用字节数："));
    text.append(QString::number(int(arrayItem->bufferSize())));
    const QString desc = QString::fromStdString(arrayItem->desc()).trimmed();
    if (!desc.isEmpty()) {
        text.append(QStringLiteral("，描述：")).append(desc);
    }

    return true;
}

// class ItemWidgetBitMap

ItemWidgetBitMap::ItemWidgetBitMap(QWidget *parent)
    : DataItemWidget(ItemTypeBitMap, parent)
{
    d_formLayout = new QFormLayout();
    d_formLayout->setLabelAlignment(Qt::AlignRight);
    itemLayout()->addLayout(d_formLayout);
    itemLayout()->addStretch();

    d_spinData = new QDoubleSpinBox(this);
    d_spinData->setFixedWidth(300);
    d_spinData->setDecimals(DBL_DIG);
    d_formLayout->addRow(QStringLiteral("输出数值："), d_spinData);

    d_labelDesc = new QLabel(this);
    d_formLayout->addRow(QStringLiteral("描述信息："), d_labelDesc);
    //
    connect(d_spinData, static_cast<void(QDoubleSpinBox::*)(double)>
            (&QDoubleSpinBox::valueChanged), this, [=](double value){
        //
        const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem, Icd::Item>(dataItem());
        if (!itemBit) {
            return;
        }
        //
        const unsigned int data = (unsigned int)value & (itemBit->mask() >> itemBit->bitStart());
        if (data != (unsigned int)value) {
            d_spinData->setValue(data);
            return;
        }
        //
        QMapIterator<int, QCheckBox *> citer(d_checkBoxes);
        while (citer.hasNext()) {
            citer.next();
            QCheckBox *checkBox = citer.value();
            checkBox->setChecked(data & (0x1UL << citer.key()));
        }
        //
        itemBit->setData(data);
        //
        QString suffix;
        if (itemBit->bitCount() <= 8) {
            suffix = QString(" (%1, %2)")
                    .arg((ushort)(unsigned char)data, 2, 16, QChar('0'))
                    .arg((ushort)(unsigned char)data, itemBit->bitCount(), 2, QChar('0'));
        } else if (itemBit->bitCount() <= 16) {
            suffix = QString(" (%1, %2)")
                    .arg((ushort)data, 4, 16, QChar('0'))
                    .arg((ushort)data, itemBit->bitCount(), 2, QChar('0'));
        } else if (itemBit->bitCount() <= 32) {
            suffix = QString(" (%1, %2)")
                    .arg((uint)value, 8, 16, QChar('0'))
                    .arg((uint)value, itemBit->bitCount(), 2, QChar('0'));
        } else {
            suffix = QString(" (%1, %2)")
                    .arg((qulonglong)data, 16, 16, QChar('0'))
                    .arg((qulonglong)data, itemBit->bitCount(), 2, QChar('0'));
        }
        d_spinData->setSuffix(suffix.toUpper());
    });
}

ItemWidgetBitMap::~ItemWidgetBitMap()
{

}

void ItemWidgetBitMap::restoreUi(const Icd::ItemPtr &data)
{
    //
    DataItemWidget::restoreUi(data);
    //
    const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem, Icd::Item>(data);
    if (!itemBit) {
        return;
    }
    //
    const quint32 defaultValue = (quint32)itemBit->defaultValue();
    QMapIterator<int, QCheckBox *> citer(d_checkBoxes);
    while (citer.hasNext()) {
        citer.next();
        QCheckBox *checkBox = citer.value();
        checkBox->setChecked(defaultValue & (0x1UL << citer.key()));
    }
}

bool ItemWidgetBitMap::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }
    //
    QMapIterator<int, QCheckBox *> citer(d_checkBoxes);
    while (citer.hasNext()) {
        citer.next();
        QCheckBox *checkBox = citer.value();
        QWidget *label = d_formLayout->labelForField(checkBox);
        d_formLayout->removeWidget(label);
        d_formLayout->removeWidget(checkBox);
        label->deleteLater();
        checkBox->deleteLater();
    }
    //
    const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem, Icd::Item>(data);
    if (!itemBit) {
        return false;
    }
    //
    d_spinData->setRange(0, (0x1UL << itemBit->bitCount()) - 1);
    //
    for (int i = 0; i < itemBit->bitCount(); ++i) {
        //
        const QString name = QString::fromStdString(itemBit->nameAt(i + itemBit->bitStart()));
        if (name.isEmpty()) {
            continue;
        }
        QCheckBox *checkBox = new QCheckBox((name.isEmpty() ? "<?>" : name), this);
        d_formLayout->insertRow(1, QStringLiteral("BIT%1：")
                                .arg(i + itemBit->bitStart(), 2, 10, QChar('0')), checkBox);
        checkBox->setChecked(itemBit->testBit(i));
        d_checkBoxes[i] = checkBox;
        connect(checkBox, &QCheckBox::toggled, this, [=](bool checked){
            unsigned int value = (uint)d_spinData->value();
            if (checked) {
                value |= (0x1UL << i);
            } else {
                value &= ~(0x1UL << i);
            }
            d_spinData->setValue(value & (itemBit->mask() >> itemBit->bitStart()));
        });
    }
    //
    d_spinData->setValue(itemBit->data());
    //
    d_labelDesc->setText(QStringLiteral("起始位偏移：%1，终止位偏移：%2，位数：%3，占用字节数：%4。")
                         .arg(itemBit->bitStart())
                         .arg(itemBit->bitStart() + itemBit->bitCount() - 1)
                         .arg(itemBit->bitCount())
                         .arg(itemBit->bufferSize()));

    return true;
}

// class ItemWidgetBitValue

ItemWidgetBitValue::ItemWidgetBitValue(QWidget *parent)
    : DataItemWidget(ItemTypeBitValue, parent)
{
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    itemLayout()->addLayout(formLayout);
    itemLayout()->addStretch();

    d_spinData = new QDoubleSpinBox(this);
    d_spinData->setFixedWidth(300);
    d_spinData->setDecimals(0);
    formLayout->addRow(QStringLiteral("输出数值："), d_spinData);

    d_labelDesc = new QLabel(this);
    formLayout->addRow(QStringLiteral("描述信息："), d_labelDesc);
    //
    connect(d_spinData, static_cast<void(QDoubleSpinBox::*)(double)>
            (&QDoubleSpinBox::valueChanged), this, [=](double value){
        //
        const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem, Icd::Item>(dataItem());
        if (!itemBit) {
            return;
        }
        //
        itemBit->setData(value);
        //
        QString suffix;
        if (itemBit->bitCount() <= 8) {
            suffix = QString(" (%1, %2)")
                    .arg((ushort)(unsigned char)value, 2, 16, QChar('0'))
                    .arg((ushort)(unsigned char)value, itemBit->bitCount(), 2, QChar('0'));
        } else if (itemBit->bitCount() <= 16) {
            suffix = QString(" (%1, %2)")
                    .arg((ushort)value, 4, 16, QChar('0'))
                    .arg((ushort)value, itemBit->bitCount(), 2, QChar('0'));
        } else if (itemBit->bitCount() <= 32) {
            suffix = QString(" (%1, %2)")
                    .arg((uint)value, 8, 16, QChar('0'))
                    .arg((uint)value, itemBit->bitCount(), 2, QChar('0'));
        } else {
            suffix = QString(" (%1, %2)")
                    .arg((qulonglong)value, 16, 16, QChar('0'))
                    .arg((qulonglong)value, itemBit->bitCount(), 2, QChar('0'));
        }
        d_spinData->setSuffix(suffix.toUpper());
    });
}

ItemWidgetBitValue::~ItemWidgetBitValue()
{

}

void ItemWidgetBitValue::restoreUi(const Icd::ItemPtr &data)
{
    //
    DataItemWidget::restoreUi(data);

    //
    const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem, Icd::Item>(data);
    if (!itemBit) {
        return;
    }

    //
    d_spinData->setValue(itemBit->defaultValue());
}

bool ItemWidgetBitValue::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }

    const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem, Icd::Item>(data);
    if (!itemBit) {
        return false;
    }

    //
    d_spinData->setRange(0, (0x1UL << itemBit->bitCount()) - 1);

    //
    d_spinData->setValue(itemBit->data());

    //
    d_labelDesc->setText(QStringLiteral("起始位偏移：%1，终止位偏移：%2，位数：%3，占用字节数：%4。")
                         .arg(itemBit->bitStart())
                         .arg(itemBit->bitStart() + itemBit->bitCount() - 1)
                         .arg(itemBit->bitCount())
                         .arg(itemBit->bufferSize()));

    return true;
}

// class ItemWidgetComplex

ItemWidgetComplex::ItemWidgetComplex(QWidget *parent)
    : DataItemWidget(ItemTypeComplex, parent)
    , d_tableWidget(0)
{

}

ItemWidgetComplex::~ItemWidgetComplex()
{

}

bool ItemWidgetComplex::addDataItem(const QString &domain)
{
    if (!DataItemWidget::addDataItem(domain)) {
        return false;
    }

    const Icd::ComplexItemPtr complex = JHandlePtrCast<Icd::ComplexItem, Icd::Item>(dataItem());
    if (!complex) {
        return false;
    }

    //
    const Icd::TablePtr &table = complex->table();
    if (!table) {
        return true;
    }

    //
    if (!d_tableWidget) {
        //
        d_tableWidget = new DataTableWidget(this);
        d_tableWidget->layout()->setContentsMargins(0, 1, 0, 0);
        clientLayout()->addWidget(d_tableWidget);
        d_tableWidget->setItemTable(item());
        //
        connect(d_tableWidget, &DataTableWidget::heightChanged, this, [=](){
            fixHeight();
        });
        connect(d_tableWidget, &DataTableWidget::itemRemoved, this, [=](){
            fixHeight();
        });
    }

    if (!d_tableWidget->addDataItem(domain, worker(), table)) {
        return false;
    }

    //
    fixHeight();

    return true;
}

int ItemWidgetComplex::focusItem(const QString &domain)
{
    int pos = DataItemWidget::focusItem(domain);

    //
    if (d_tableWidget) {
        int offset = 0;
        if (!QMetaObject::invokeMethod(
                    d_tableWidget, "focusItem", Q_RETURN_ARG(int, offset),
                    Q_ARG(QString, domain))) {
            offset = 0;
            //return pos;
        }

        //
        pos += offset + titleBarHeight() + 1;
    }

    return pos;
}

void ItemWidgetComplex::removeItem(const QString &domain)
{
    //
    if (d_tableWidget) {
        if (!QMetaObject::invokeMethod(d_tableWidget, "removeItem",
                                       Q_ARG(QString, domain))) {
            //
        }

        //
        fixHeight();
    }

    //
    DataItemWidget::removeItem(domain);
}

void ItemWidgetComplex::restoreUi(const Icd::ItemPtr &data)
{
    //
    DataItemWidget::restoreUi(data);

    //
    const Icd::ComplexItemPtr complex = JHandlePtrCast<Icd::ComplexItem, Icd::Item>(data);
    if (!complex) {
        return;
    }

    //
}

bool ItemWidgetComplex::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }

    const Icd::ComplexItemPtr complex = JHandlePtrCast<Icd::ComplexItem, Icd::Item>(data);
    if (!complex) {
        return false;
    }

    //

    return true;
}

void ItemWidgetComplex::fixHeight()
{
    //
    int height = 0;
    if (d_tableWidget->rowCount() > 0) {
        height = d_tableWidget->contentsHeight();
    } else {
        height = 30;
    }

    setFixedHeight(height + titleBarHeight() + 1);
}

// class ItemWidgetFrame

ItemWidgetFrame::ItemWidgetFrame(QWidget *parent)
    : DataItemWidget(ItemTypeFrame, parent)
{
    d_tabWidget = new IcdTabWidget(this);
    d_tabWidget->setContentsMargins(0, 0, 0, 0);
    clientLayout()->addWidget(d_tabWidget);

    //
    connect(d_tabWidget, &IcdTabWidget::currentChanged, this, [=](){
        fixHeight(d_tabWidget->currentIndex());
    });
    connect(d_tabWidget, &IcdTabWidget::heightChanged, this, [=](){
        fixHeight(d_tabWidget->currentIndex());
    });
    connect(d_tabWidget, &IcdTabWidget::itemRemoved, this, [=](){
        fixHeight(d_tabWidget->currentIndex());
    });
}

ItemWidgetFrame::~ItemWidgetFrame()
{

}

bool ItemWidgetFrame::addDataItem(const QString &domain)
{
    //
    const Icd::FrameItemPtr frame = JHandlePtrCast<Icd::FrameItem, Icd::Item>(dataItem());
    if (!frame) {
        return false;
    }

    //
    const QString section = domain.section('/', 1);

    //
    int i = 0;
    const std::string tableId = section.section('/', 0, 0).toStdString();
    int childCount = item()->rowCount();
    const Icd::TablePtrMap &tables = frame->allTable();
    Icd::TablePtrMap::const_iterator citer = tables.cbegin();
    if (tableId.empty()) {
        for (; citer != tables.cend() && i < childCount; ++citer, ++i) {
            const Icd::TablePtr &table = citer->second;
            QStandardItem *itemTable = item()->child(i);
            if (!itemTable) {
                continue;
            }
            //
            if (!d_tabWidget->addDataItem(domain, worker(), table, itemTable)) {
                continue;
            }
        }
        //
        QApplication::processEvents(QEventLoop::EventLoopExec);
    } else {
        for (; citer != tables.cend() && i < childCount; ++citer, ++i) {
            const Icd::TablePtr &table = citer->second;
            QStandardItem *itemTable = item()->child(i);
            if (table && itemTable && table->id() == tableId) {
                if (!d_tabWidget->addDataItem(domain, worker(), table, itemTable)) {
                    return false;
                }
                //
                QApplication::processEvents(QEventLoop::EventLoopExec);
                break;
            }
        }
    }

    d_tabWidget->setCurrentIndex(0);

    //
    fixHeight(0);

    return true;
}

int ItemWidgetFrame::focusItem(const QString &domain)
{
    int pos = DataItemWidget::focusItem(domain);

    //
    int offset = 0;
    if (!QMetaObject::invokeMethod(
                d_tabWidget, "focusItem", Q_RETURN_ARG(int, offset),
                Q_ARG(QString, domain))) {
        return pos;
    }

    //
    pos += offset + titleBarHeight() + 1;

    return pos;
}

void ItemWidgetFrame::removeItem(const QString &domain)
{
    DataItemWidget::removeItem(domain);

    //
    if (!QMetaObject::invokeMethod(d_tabWidget, "removeItem", Q_ARG(QString, domain))) {
        return;
    }

    //
    fixHeight(d_tabWidget->currentIndex());
}

void ItemWidgetFrame::restoreUi(const Icd::ItemPtr &data)
{
    //
    DataItemWidget::restoreUi(data);

    //
    const Icd::FrameItemPtr itemFrame = JHandlePtrCast<Icd::FrameItem, Icd::Item>(data);
    if (!itemFrame) {
        return;
    }

    //
}

bool ItemWidgetFrame::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }

    const Icd::FrameItemPtr itemFrame = JHandlePtrCast<Icd::FrameItem, Icd::Item>(data);
    if (!itemFrame) {
        return false;
    }

    //
    fixHeight(d_tabWidget->currentIndex());

    return true;
}

void ItemWidgetFrame::fixHeight(int index)
{
    //
    DataTableWidget *tableWidget = qobject_cast<DataTableWidget *>(d_tabWidget->widget(index));
    if (!tableWidget) {
        return;
    }

    //
    int height = 0;
    if (tableWidget->rowCount() > 0) {
        height = tableWidget->contentsHeight();
    } else {
        height = 30;
    }

    setFixedHeight(height + d_tabWidget->tabBar()->height()
                   + titleBarHeight() + 2);
}

}
