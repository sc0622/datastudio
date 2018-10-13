#include "precomp.h"
#include "dataitem_widget.h"
#include "icdcore/icdcore_inc.h"
#include "icdworker/icdworker.h"
#include "icdtab_widget.h"
#include "datatable_widget.h"
#include "jutraledit/jutraledit_view.h"
#include <float.h>

namespace Simulate {

// class DataItemWidget

DataItemWidget::DataItemWidget(ItemType itemType, QWidget *parent)
    : QFrame(parent)
    , itemType_(itemType)
    , item_(nullptr)
    , buttonSend_(nullptr)
    , itemLayout_(nullptr)
    , realtimeUpdate_(false)
{
    setObjectName("DataItemWidget");
    setFrameShape(QFrame::NoFrame);

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(6, 3, 8, 0);
    vertLayoutMain->setSpacing(0);

    QHBoxLayout *layoutTop = new QHBoxLayout();
    vertLayoutMain->addLayout(layoutTop);

    labelTitle_ = new QLabel(this);
    labelTitle_->setObjectName("labelTitle");
    labelTitle_->setContentsMargins(8, 0, 8, 0);
    labelTitle_->setFixedHeight(25);
    QFont font = labelTitle_->font();
    font.setBold(true);
    labelTitle_->setFont(font);
    layoutTop->addWidget(labelTitle_, 0, Qt::AlignTop);

    layoutTop->addStretch();

    clientLayout_ = new QHBoxLayout();
    clientLayout_->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->addLayout(clientLayout_);

    switch (itemType_) {
    case ItemTypeComplex:
    {
        const Icd::ComplexItemPtr complex = JHandlePtrCast<Icd::ComplexItem>(dataItem());
        if (!complex) {
            break;
        }
        if (complex->childCount() > 0) {
            break;
        }
        break;
    }
    case ItemTypeFrame:
        break;
    default:
    {
        QFrame *formButton = new QFrame(this);
        formButton->setObjectName("frameButton");
        formButton->setFixedWidth(100);
        layoutTop->addWidget(formButton);

        buttonSend_ = new QPushButton(tr("Send"), this);
        buttonSend_->setFixedSize(60, 25);
        layoutTop->addWidget(buttonSend_);

        layoutTop->addSpacing(10);

        QPushButton *buttonRestore = new QPushButton(tr("Reset"), this);
        buttonRestore->setFixedSize(60, 25);
        layoutTop->addWidget(buttonRestore);

        layoutTop->addSpacing(10);

        QPushButton *buttonRemove = new QPushButton(tr("Remove"), this);
        buttonRemove->setFixedSize(60, 25);
        layoutTop->addWidget(buttonRemove);

        QWidget *widgetClient = new QWidget(this);
        widgetClient->setObjectName("widgetClient");
        clientLayout_->addWidget(widgetClient);

        QHBoxLayout *horiLayoutClient = new QHBoxLayout(widgetClient);
        horiLayoutClient->setContentsMargins(0, 6, 0, 6);

        itemLayout_ = new QVBoxLayout();
        horiLayoutClient->addLayout(itemLayout_);
        itemLayout_->setContentsMargins(0, 0, 0, 0);
        itemLayout_->setSpacing(6);
        itemLayout_->addStretch();

        connect(buttonRemove, SIGNAL(clicked(bool)), this, SIGNAL(remove()));
        connect(buttonSend_, SIGNAL(clicked(bool)), this, SIGNAL(send()));
        connect(buttonRestore, &QPushButton::clicked, this, [=](){
            restoreUi(defaultData_);
        });

        break;
    }}

    //
    const Json::Value option = JMain::instance()->option("simulate", "option.view");
    if (option.isMember("realtimeUpdate")) {
        realtimeUpdate_ = option["realtimeUpdate"].asBool();
    }

    jnotify->on("simulate.view.realtimeUpdate.changed", this, [=](JNEvent &event){
        realtimeUpdate_ = event.argument().toBool();
    });
}

DataItemWidget::~DataItemWidget()
{
    //
    if (item_) {
        item_->setData(QVariant::Invalid, Icd::TreeBoundRole);
    }
    //
    if (worker_) {
        worker_->disconnect(this);
        worker_->workerSend()->disconnect(this);
    }
}

DataItemWidget *DataItemWidget::createWidget(const Icd::ItemPtr &dataItem, QWidget *parent)
{
    DataItemWidget *itemWidget = nullptr;
    switch (dataItem->type()) {
    case Icd::ItemHeader:
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
    return itemType_;
}

const Icd::WorkerPtr &DataItemWidget::worker() const
{
    return worker_;
}

void DataItemWidget::setWorker(const Icd::WorkerPtr &worker)
{
    // the same
    if (worker && (worker_ == worker)) {
        return;
    }
    //
    if (worker_) {
        worker_->disconnect(this);
        worker_->workerSend()->disconnect(this);
    }
    //
    if (worker) {
        auto setButtonSendEnabled = [=](Icd::WorkerTrans::TimeEvent event){
            if (buttonSend_) {
                switch (event) {
                case Icd::WorkerTrans::TimeOneShot:
                    buttonSend_->setEnabled(worker->isOpen());
                    break;
                case Icd::WorkerTrans::TimePeriodic:
                    buttonSend_->setEnabled(false);
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

    worker_ = worker;
}

Icd::ItemPtr DataItemWidget::dataItem() const
{
    return dataItem_;
}

bool DataItemWidget::setDataItem(const Icd::ItemPtr &dataItem)
{
    //
    if (!dataItem) {
        return false;
    }

    defaultData_ = JHandlePtrCast<Icd::Item>(dataItem->clone());
    dataItem_ = dataItem;

    return true;
}

QStandardItem *DataItemWidget::item() const
{
    return item_;
}

void DataItemWidget::setItem(QStandardItem *item)
{
    item_ = item;
}

void DataItemWidget::updateUi()
{
    updateUi(dataItem_);
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
    if (item_) {
        item_->setData(QVariant::Invalid, Icd::TreeBoundRole);
    }
}

bool DataItemWidget::realtimeUpdate() const
{
    return realtimeUpdate_;
}

bool DataItemWidget::autoSend() const
{
    if (!worker_) {
        return false;
    }

    const Icd::ChannelPtr channel = worker_->channel();
    if (!channel) {
        return false;
    }

    return channel->autoSend();
}

bool DataItemWidget::autoUpdate() const
{
    return realtimeUpdate_ || !autoSend();
}

QHBoxLayout *DataItemWidget::clientLayout()
{
    return clientLayout_;
}

QVBoxLayout *DataItemWidget::itemLayout()
{
    return itemLayout_;
}

int DataItemWidget::titleBarHeight() const
{
    return labelTitle_->height();
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
    if (item_) {
        const QString path = item_->data(Icd::TreeItemPathRole).toString();
        const QString name = path.section('@', 0, 0);
        labelTitle_->setText("<font color=#7777bb>" + name + "</font><font color=#807777aa size=2>@"
                              + path.section('@', 1) + "</font>");
    } else {
        labelTitle_->setText(QString::fromStdString(data->name()));
    }

    if (worker_) {

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

    spinValue_ = new JSpinBox(this);
    spinValue_->setFillChar(QChar('0'));
    spinValue_->setRadix(16);
    spinValue_->setFixedWidth(300);
    formLayout->addRow(tr("Current value:"), spinValue_);

    auto updateValue = [=](int value) {
        if (!dataItem()) {
            return;
        }
        dataItem()->setData(value);
    };
    connect(spinValue_, static_cast<void(JSpinBox::*)(int)>
            (&JSpinBox::valueChanged), this, [=](int value){
        if (!autoUpdate()) {
            return;
        }
        updateValue(value);
    });
    connect(spinValue_, &JSpinBox::editingFinished, this, [=](){
        if (!autoUpdate()) {
            updateValue(spinValue_->value());
        }
        if (!autoSend()) {
            emit send();
        }
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
    const Icd::HeaderItemPtr itemHead = JHandlePtrCast<Icd::HeaderItem>(data);
    if (!itemHead) {
        return;
    }
    //
    spinValue_->setValue(int(itemHead->defaultValue()));
}

bool ItemWidgetHead::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }

    const Icd::HeaderItemPtr itemHead = JHandlePtrCast<Icd::HeaderItem>(data);
    if (!itemHead) {
        return false;
    }

    spinValue_->setRange(0, int((0x1U << (int(itemHead->bufferSize()) * 8)) - 1));
    spinValue_->setValue(int(itemHead->data()));

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

    spinValue_ = new QSpinBox(this);
    spinValue_->setFixedWidth(300);
    formLayout->addRow(tr("Current value:"), spinValue_);

    auto updateValue = [=](int value) {
        if (!dataItem()) {
            return;
        }
        dataItem()->setData(value);
    };
    connect(spinValue_, static_cast<void(QSpinBox::*)(int)>
            (&QSpinBox::valueChanged), this, [=](int value){
        if (!autoUpdate()) {
            return;
        }
        updateValue(value);
    });
    connect(spinValue_, &JSpinBox::editingFinished, this, [=](){
        if (!autoUpdate()) {
            updateValue(spinValue_->value());
        }
        if (!autoSend()) {
            emit send();
        }
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
    const Icd::CounterItemPtr itemCounter = JHandlePtrCast<Icd::CounterItem>(data);
    if (!itemCounter) {
        return;
    }
    //
    spinValue_->setValue(int(itemCounter->defaultValue()));
}

bool ItemWidgetCounter::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }

    const Icd::CounterItemPtr itemCounter = JHandlePtrCast<Icd::CounterItem>(data);
    if (!itemCounter) {
        return false;
    }
    //
    spinValue_->setRange(0, int((0x1U << (int(itemCounter->bufferSize()) * 8)) - 1));
    spinValue_->setValue(int(itemCounter->data()));

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

    spinValue_ = new QSpinBox(this);
    spinValue_->setFixedWidth(300);
    formLayout->addRow(tr("Current value:"), spinValue_);

    comboBoxCheckType_ = new QComboBox(this);
    comboBoxCheckType_->setFixedWidth(300);
    formLayout->addRow(tr("Parity:"), comboBoxCheckType_);

    spinStartPos_ = new QSpinBox(this);
    spinStartPos_->setFixedWidth(300);
    spinStartPos_->setRange(0, 65535);
    spinStartPos_->setSuffix(tr(" B"));
    formLayout->addRow(tr("Start offset:"), spinStartPos_);

    spinEndPos_ = new QSpinBox(this);
    spinEndPos_->setFixedWidth(300);
    spinEndPos_->setRange(0, 65535);
    spinEndPos_->setSuffix(tr(" B"));
    formLayout->addRow(tr("Stop offset:"), spinEndPos_);

    labelCheckLength_ = new QLabel(this);
    formLayout->addRow(tr("Length:"), labelCheckLength_);
    //
    comboBoxCheckType_->addItem(tr("No parity"), Icd::CheckNone);
    comboBoxCheckType_->addItem(tr("8 bits sum-parity"), Icd::CheckSum8);
    comboBoxCheckType_->addItem(tr("16 bits sum-parity"), Icd::CheckSum16);
    comboBoxCheckType_->addItem(tr("8 bits xor-parity"), Icd::CheckXor8);
    comboBoxCheckType_->addItem(tr("16 bits xor-parity"), Icd::CheckXor16);
    comboBoxCheckType_->addItem(tr("CRC8 parity"), Icd::CheckCrc8);
    comboBoxCheckType_->addItem(tr("CRC16 parity"), Icd::CheckCrc16);

    auto updateValue = [=](int value) {
        if (!dataItem()) {
            return;
        }
        dataItem()->setData(value);
    };
    connect(spinValue_, static_cast<void(QSpinBox::*)(int)>
            (&QSpinBox::valueChanged), this, [=](int value){
        if (!autoUpdate()) {
            return;
        }
        updateValue(value);
    });
    connect(spinValue_, &JSpinBox::editingFinished, this, [=](){
        if (!autoUpdate()) {
            updateValue(spinValue_->value());
        }
        if (!autoSend()) {
            emit send();
        }
    });
    connect(comboBoxCheckType_, static_cast<void(QComboBox::*)(int)>
            (&QComboBox::currentIndexChanged), this, [=](int index){
        Q_UNUSED(index);
        const Icd::CheckItemPtr itemCheck = JHandlePtrCast<Icd::CheckItem>(dataItem());
        if (!itemCheck) {
            return;
        }
        itemCheck->setCheckType(Icd::CheckType(comboBoxCheckType_->currentData().toInt()));
    });
    const QString sByteUnit = tr(" B");
    connect(spinStartPos_, static_cast<void(QSpinBox::*)(int)>
            (&QSpinBox::valueChanged), this, [=](int value){
        //
        const Icd::CheckItemPtr itemCheck = JHandlePtrCast<Icd::CheckItem>(dataItem());
        if (!itemCheck) {
            return;
        }
        itemCheck->setStartPos(value);
        // CheckLength
        labelCheckLength_->setText(QString::number(itemCheck->checkLength()) + sByteUnit);
        //
        if (value > spinEndPos_->value()) {
            spinStartPos_->setValue(spinEndPos_->value());
        }
    });
    connect(spinEndPos_, static_cast<void(QSpinBox::*)(int)>
            (&QSpinBox::valueChanged), this, [=](int value){
        //
        const Icd::CheckItemPtr itemCheck = JHandlePtrCast<Icd::CheckItem>(dataItem());
        if (!itemCheck) {
            return;
        }
        itemCheck->setEndPos(value);
        // CheckLength
        labelCheckLength_->setText(QString::number(itemCheck->checkLength()) + sByteUnit);
        //
        if (value < spinStartPos_->value()) {
            spinEndPos_->setValue(spinStartPos_->value());
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
    const Icd::CheckItemPtr itemCheck = JHandlePtrCast<Icd::CheckItem>(data);
    if (!itemCheck) {
        return;
    }
    //
    spinValue_->setValue(int(itemCheck->defaultValue()));
}

bool ItemWidgetCheck::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }
    //
    spinValue_->setValue(0);
    comboBoxCheckType_->setCurrentIndex(0);
    spinStartPos_->setValue(0);
    spinEndPos_->setValue(0);
    labelCheckLength_->setText("");
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
    spinStartPos_->setRange(0, qCeil(tableSend->bufferSize()) - 1);
    spinEndPos_->setRange(0, qCeil(tableSend->bufferSize()) - 1);
    //
    const Icd::CheckItemPtr checkItem = JHandlePtrCast<Icd::CheckItem>(data);
    if (!checkItem) {
        return false;
    }
    // value
    spinValue_->setValue(int(checkItem->data()));
    // CheckType
    switch (checkItem->checkType()) {
    case Icd::CheckNone:
    default:
        comboBoxCheckType_->setCurrentIndex(0);
        break;
    case Icd::CheckSum8:
        comboBoxCheckType_->setCurrentIndex(1);
        break;
    case Icd::CheckSum16:
        comboBoxCheckType_->setCurrentIndex(2);
        break;
    case Icd::CheckCrc8:
        comboBoxCheckType_->setCurrentIndex(3);
        break;
    case Icd::CheckCrc16:
        comboBoxCheckType_->setCurrentIndex(4);
        break;
    }
    // EndPos
    spinEndPos_->setValue(checkItem->endPos());
    // StartPos
    spinStartPos_->setValue(checkItem->startPos());
    // CheckLength
    labelCheckLength_->setText(QString::number(checkItem->checkLength())
                                + tr(" B"));

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

    comboBoxCode_ = new QComboBox(this);
    comboBoxCode_->setFixedWidth(300);
    formLayout->addRow(tr("Current frame code:"), comboBoxCode_);
    //
    connect(comboBoxCode_, static_cast<void(QComboBox::*)
            (int)>(&QComboBox::currentIndexChanged), this, [=](int index){
        if (!dataItem()) {
            return;
        }
        dataItem()->setData(comboBoxCode_->itemData(index).toInt());
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
                comboBoxCode_->setEnabled(true);
                break;
            case Icd::WorkerTrans::TimePeriodic:
                comboBoxCode_->setEnabled(false);
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
    const Icd::FrameCodeItemPtr itemFrameCode = JHandlePtrCast<Icd::FrameCodeItem>(data);
    if (!itemFrameCode) {
        return;
    }
    //
    comboBoxCode_->setCurrentText(QString("0x%1").arg(int(itemFrameCode->defaultValue()),
                                                       2, 16, QChar('0')).toUpper());
}

bool ItemWidgetFrameCode::updateUi(const Icd::ItemPtr &data)
{
    //
    comboBoxCode_->clear();

    if (!DataItemWidget::updateUi(data)) {
        return false;
    }
    //
    const Icd::FrameCodeItemPtr itemFrameCode = JHandlePtrCast<Icd::FrameCodeItem>(data);
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
        comboBoxCode_->addItem(QString::fromStdString(citer->second->name()), citer->first);
    }
    //
    comboBoxCode_->setCurrentText(QString("0x%1").arg(int(itemFrameCode->data()),
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

    checkBoxLink_ = new QCheckBox(tr("(Linkage original and value)"), this);
    checkBoxLink_->setFixedWidth(300);
    checkBoxLink_->setCheckable(false);
    checkBoxLink_->setChecked(true);   //FIXME
    formLayout->addRow(tr("Relative data:"), checkBoxLink_);

    sliderValue_ = new QSlider(this);
    sliderValue_->setFixedWidth(300);
    sliderValue_->setOrientation(Qt::Horizontal);
    formLayout->addRow(tr("Original data:"), sliderValue_);

    spinValue_ = new QDoubleSpinBox(this);
    spinValue_->setFixedWidth(300);
    spinValue_->setDecimals(DBL_DIG);
    formLayout->addRow(tr("Original data:"), spinValue_);

    spinData_ = new QDoubleSpinBox(this);
    spinData_->setFixedWidth(300);
    spinData_->setDecimals(DBL_DIG);
    spinData_->setEnabled(false);
    formLayout->addRow(tr("Output data:"), spinData_);

    labelDesc_ = new QLabel(this);
    formLayout->addRow(tr("Describe:"), labelDesc_);
    //
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
            iData &= (1ll << (int(itemNumeric->bufferSize()) << 3)) - 1;
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
            uData &= (1ull << (int(itemNumeric->bufferSize()) << 3)) - 1;
            suffix = QString(" (%1)").arg(uData, int(itemNumeric->bufferSize() * 2),
                                          16, QChar('0')).toUpper();
            break;
        }
        case Icd::NumericF32:
        {
            float _data = float(data);
            qint32 iData;
            memcpy(&iData, &_data, 4);
            suffix = QString(" (%1)").arg(iData, int(itemNumeric->bufferSize() * 2),
                                          16, QChar('0')).toUpper();
            break;
        }
        case Icd::NumericF64:
        {
            qint64 iData;
            memcpy(&iData, &data, 8);
            suffix = QString(" (%1)").arg(iData, int(itemNumeric->bufferSize() * 2),
                                          16, QChar('0')).toUpper();
            break;
        }
        default:
        {
            qulonglong uData = qulonglong(data);
            uData &= (1ull << (int(itemNumeric->bufferSize()) << 3)) - 1;
            suffix = QString(" (%1)").arg(uData, int(itemNumeric->bufferSize() * 2),
                                          16, QChar('0')).toUpper();
            break;
        }}
        spinData_->setSuffix(suffix);
    };
    auto updateValue = [=](double value) {
        if (!dataItem()) {
            return;
        }
        //
        const Icd::NumericItemPtr itemNumeric = JHandlePtrCast<Icd::NumericItem>(dataItem());
        if (!itemNumeric) {
            return;
        }
        itemNumeric->setData(value);
        const double dataValue = (value - itemNumeric->offset()) / itemNumeric->scale();
        spinData_->setValue(dataValue);
        setDataSuffix(itemNumeric);
    };
    auto editFinished = [=](){
        if (!autoUpdate()) {
            updateValue(spinValue_->value());
        }
        if (!autoSend()) {
            emit send();
        }
    };
    connect(sliderValue_, &QSlider::valueChanged, [=](int value){
        spinValue_->setValue(value);
        editFinished();
    });
    connect(spinValue_, static_cast<void(QDoubleSpinBox::*)(double)>
            (&QDoubleSpinBox::valueChanged), this, [=](double value){
        //
        sliderValue_->setValue(int(value));
        //
        if (!autoUpdate()) {
            return;
        }
        updateValue(value);
    });
    connect(spinValue_, &QDoubleSpinBox::editingFinished, this, [=](){
        editFinished();
    });
    connect(spinData_, static_cast<void(QDoubleSpinBox::*)(double)>
            (&QDoubleSpinBox::valueChanged), this, [=](double value){
        const Icd::NumericItemPtr itemNumeric = JHandlePtrCast<Icd::NumericItem>(dataItem());
        if (!itemNumeric) {
            return;
        }
        const double realValue = value * itemNumeric->scale() + itemNumeric->offset();
        itemNumeric->setData(realValue);
        setDataSuffix(itemNumeric);
        if (!checkBoxLink_->isChecked()) {
            return;
        }
        spinValue_->setValue(realValue);
    });
    connect(checkBoxLink_, &QCheckBox::stateChanged, this, [=](int state){
        if (state == Qt::Checked) {
            spinData_->setEnabled(true);
        } else {
            spinData_->setEnabled(false);
        }
    });

    //
    checkBoxLink_->setChecked(false);
}

ItemWidgetNumeric::~ItemWidgetNumeric()
{

}

void ItemWidgetNumeric::restoreUi(const Icd::ItemPtr &data)
{
    //
    DataItemWidget::restoreUi(data);
    //
    const Icd::NumericItemPtr itemNumeric = JHandlePtrCast<Icd::NumericItem>(data);
    if (!itemNumeric) {
        return;
    }
    //
    spinValue_->setValue(itemNumeric->defaultValue());
}

bool ItemWidgetNumeric::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }
    //
    const Icd::NumericItemPtr itemNumeric = JHandlePtrCast<Icd::NumericItem>(data);
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
        spinValue_->setDecimals(FLT_DIG);
        spinData_->setDecimals(FLT_DIG);
        break;
    default:
        spinData_->setDecimals(DBL_DIG);
        spinValue_->setDecimals(DBL_DIG);
        break;
    }
    // unit
    const QString unit = QString::fromStdString(itemNumeric->unit()).trimmed();
    if (!unit.isEmpty()) {
        spinValue_->setSuffix(" " + unit);
    }
    // range
    std::pair<double, double> range = itemNumeric->valueRange();
    //
    spinValue_->setRange(range.first, range.second);
    spinValue_->setSingleStep(itemNumeric->scale());
    //
    if (range.first < INT_MIN) {
        range.first = INT_MIN;
    }
    if (range.second > INT_MAX) {
        range.second = INT_MAX;
    }
    sliderValue_->setRange(qFloor(range.first), qCeil(range.second));
    //
    range = itemNumeric->dataRange();
    spinData_->setRange(range.first, range.second);
    //
    spinValue_->setValue(itemNumeric->data());
    //
    labelDesc_->setText(tr("Scale: %1, Offset: %2, Range: %3, Unit: %4, size: %5.")
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

    labelDesc_ = new QLabel(this);
    formLayout->addRow(tr("Describe:"), labelDesc_);

    connect(dataView_, &JUtralEdit::JView::textChanged, this, [=](){
        const Icd::ArrayItemPtr arrayItem = JHandlePtrCast<Icd::ArrayItem>(dataItem());
        if (!arrayItem) {
            return;
        }
        //
        switch (dataView_->showType()) {
        case JUtralEdit::JView::ShowAsHexData:
        {
            const QByteArray hexData = dataView_->hexData();
            arrayItem->setValue(hexData.constData(), hexData.size());
            break;
        }
        case JUtralEdit::JView::ShowAsString:
        {
            const QByteArray text = dataView_->text().toLocal8Bit();
            arrayItem->setValue(text.constData(), text.size());
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
    const Icd::ArrayItemPtr arrayItem = JHandlePtrCast<Icd::ArrayItem>(data);
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
    const Icd::ArrayItemPtr arrayItem = JHandlePtrCast<Icd::ArrayItem>(data);
    if (!arrayItem) {
        return false;
    }
    //
    QString text(tr("Size: "));
    text.append(QString::number(int(arrayItem->bufferSize())));
    const QString desc = QString::fromStdString(arrayItem->desc()).trimmed();
    if (!desc.isEmpty()) {
        text.append(tr(", Describe: ")).append(desc);
    }

    return true;
}

// class ItemWidgetBitMap

ItemWidgetBitMap::ItemWidgetBitMap(QWidget *parent)
    : DataItemWidget(ItemTypeBitMap, parent)
{
    formLayout_ = new QFormLayout();
    formLayout_->setLabelAlignment(Qt::AlignRight);
    itemLayout()->addLayout(formLayout_);
    itemLayout()->addStretch();

    spinData_ = new QDoubleSpinBox(this);
    spinData_->setFixedWidth(300);
    spinData_->setDecimals(0);
    formLayout_->addRow(tr("Output data:"), spinData_);

    labelDesc_ = new QLabel(this);
    formLayout_->addRow(tr("Describe:"), labelDesc_);
    //
    auto updateValue = [=](int value) {
        if (!dataItem()) {
            return;
        }
        const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem>(dataItem());
        if (!itemBit) {
            return;
        }
        //
        const unsigned int data = static_cast<unsigned int>(value) & (itemBit->mask() >> itemBit->bitStart());
        if (data != static_cast<unsigned int>(value)) {
            spinData_->setValue(data);
            return;
        }
        //
        QMapIterator<int, QCheckBox *> citer(checkBoxes_);
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
                    .arg(ushort(static_cast<unsigned char>(data)), 2, 16, QChar('0'))
                    .arg(ushort(static_cast<unsigned char>(data)), itemBit->bitCount(), 2, QChar('0'));
        } else if (itemBit->bitCount() <= 16) {
            suffix = QString(" (%1, %2)")
                    .arg(ushort(data), 4, 16, QChar('0'))
                    .arg(ushort(data), itemBit->bitCount(), 2, QChar('0'));
        } else if (itemBit->bitCount() <= 32) {
            suffix = QString(" (%1, %2)")
                    .arg(uint(value), 8, 16, QChar('0'))
                    .arg(uint(value), itemBit->bitCount(), 2, QChar('0'));
        } else {
            suffix = QString(" (%1, %2)")
                    .arg(qulonglong(data), 16, 16, QChar('0'))
                    .arg(qulonglong(data), itemBit->bitCount(), 2, QChar('0'));
        }
        spinData_->setSuffix(suffix.toUpper());
    };
    connect(spinData_, static_cast<void(QDoubleSpinBox::*)(double)>
            (&QDoubleSpinBox::valueChanged), this, [=](double value){
        updateValue(int(value));
    });
    connect(spinData_, &QDoubleSpinBox::editingFinished, this, [=](){
        if (!autoUpdate()) {
            updateValue(int(spinData_->value()));
        }
        if (!autoSend()) {
            emit send();
        }
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
    const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem>(data);
    if (!itemBit) {
        return;
    }
    //
    const quint32 defaultValue = quint32(itemBit->defaultValue());
    QMapIterator<int, QCheckBox *> citer(checkBoxes_);
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
    QMapIterator<int, QCheckBox *> citer(checkBoxes_);
    while (citer.hasNext()) {
        citer.next();
        QCheckBox *checkBox = citer.value();
        QWidget *label = formLayout_->labelForField(checkBox);
        formLayout_->removeWidget(label);
        formLayout_->removeWidget(checkBox);
        label->deleteLater();
        checkBox->deleteLater();
    }
    //
    const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem>(data);
    if (!itemBit) {
        return false;
    }
    //
    spinData_->setRange(0, (0x1ULL << itemBit->bitCount()) - 1);
    //
    for (int i = 0; i < itemBit->bitCount(); ++i) {
        //
        const QString name = QString::fromStdString(itemBit->nameAt(i + itemBit->bitStart()));
        if (name.isEmpty()) {
            continue;
        }
        QCheckBox *checkBox = new QCheckBox((name.isEmpty() ? "<?>" : name), this);
        formLayout_->insertRow(1, tr("BIT%1: ")
                                .arg(i + itemBit->bitStart(), 2, 10, QChar('0')), checkBox);
        checkBox->setChecked(itemBit->testBit(i));
        checkBoxes_[i] = checkBox;
        connect(checkBox, &QCheckBox::toggled, this, [=](bool checked){
            quint32 value = quint32(spinData_->value());
            if (checked) {
                value |= (0x1UL << i);
            } else {
                value &= ~(0x1UL << i);
            }
            spinData_->setValue(value & (itemBit->mask() >> itemBit->bitStart()));
        });
    }
    //
    spinData_->setValue(itemBit->data());
    //
    labelDesc_->setText(tr("Start bit: %1, stop bit: %2, bits: %3, size: %4.")
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

    spinData_ = new QDoubleSpinBox(this);
    spinData_->setFixedWidth(300);
    spinData_->setDecimals(0);
    formLayout->addRow(tr("Output data:"), spinData_);

    labelDesc_ = new QLabel(this);
    formLayout->addRow(tr("Describe:"), labelDesc_);
    //
    auto updateValue = [=](int value) {
        if (!dataItem()) {
            return;
        }
        const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem>(dataItem());
        if (!itemBit) {
            return;
        }
        //
        itemBit->setData(value);
        //
        QString suffix;
        if (itemBit->bitCount() <= 8) {
            suffix = QString(" (%1, %2)")
                    .arg(ushort(static_cast<unsigned char>(value)), 2, 16, QChar('0'))
                    .arg(ushort(static_cast<unsigned char>(value)), itemBit->bitCount(), 2, QChar('0'));
        } else if (itemBit->bitCount() <= 16) {
            suffix = QString(" (%1, %2)")
                    .arg(ushort(value), 4, 16, QChar('0'))
                    .arg(ushort(value), itemBit->bitCount(), 2, QChar('0'));
        } else if (itemBit->bitCount() <= 32) {
            suffix = QString(" (%1, %2)")
                    .arg(uint(value), 8, 16, QChar('0'))
                    .arg(uint(value), itemBit->bitCount(), 2, QChar('0'));
        } else {
            suffix = QString(" (%1, %2)")
                    .arg(qulonglong(value), 16, 16, QChar('0'))
                    .arg(qulonglong(value), itemBit->bitCount(), 2, QChar('0'));
        }
        spinData_->setSuffix(suffix.toUpper());
    };
    connect(spinData_, static_cast<void(QDoubleSpinBox::*)(double)>
            (&QDoubleSpinBox::valueChanged), this, [=](double value){
        if (!autoUpdate()) {
            return;
        }
        updateValue(int(value));
    });
    connect(spinData_, &JSpinBox::editingFinished, this, [=](){
        if (!autoUpdate()) {
            updateValue(int(spinData_->value()));
        }
        if (!autoSend()) {
            emit send();
        }
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
    const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem>(data);
    if (!itemBit) {
        return;
    }

    //
    spinData_->setValue(itemBit->defaultValue());
}

bool ItemWidgetBitValue::updateUi(const Icd::ItemPtr &data)
{
    if (!DataItemWidget::updateUi(data)) {
        return false;
    }

    const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem>(data);
    if (!itemBit) {
        return false;
    }

    //
    spinData_->setRange(0, (0x1UL << itemBit->bitCount()) - 1);

    //
    spinData_->setValue(itemBit->data());

    //
    labelDesc_->setText(tr("Start bit: %1, stop bit: %2, bits: %3, size: %4.")
                         .arg(itemBit->bitStart())
                         .arg(itemBit->bitStart() + itemBit->bitCount() - 1)
                         .arg(itemBit->bitCount())
                         .arg(itemBit->bufferSize()));

    return true;
}

// class ItemWidgetComplex

ItemWidgetComplex::ItemWidgetComplex(QWidget *parent)
    : DataItemWidget(ItemTypeComplex, parent)
    , tableWidget_(nullptr)
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

    const Icd::ComplexItemPtr complex = JHandlePtrCast<Icd::ComplexItem>(dataItem());
    if (!complex) {
        return false;
    }

    //
    const Icd::TablePtr &table = complex->table();
    if (!table) {
        return true;
    }

    //
    if (!tableWidget_) {
        //
        tableWidget_ = new DataTableWidget(this);
        tableWidget_->layout()->setContentsMargins(0, 1, 0, 0);
        clientLayout()->addWidget(tableWidget_);
        tableWidget_->setItemTable(item());
        //
        connect(tableWidget_, &DataTableWidget::heightChanged, this, [=](){
            fixHeight();
        });
        connect(tableWidget_, &DataTableWidget::itemRemoved, this, [=](){
            fixHeight();
        });
    }

    if (!tableWidget_->addDataItem(domain, worker(), table)) {
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
    if (tableWidget_) {
        int offset = 0;
        if (!QMetaObject::invokeMethod(
                    tableWidget_, "focusItem", Q_RETURN_ARG(int, offset),
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
    if (tableWidget_) {
        if (!QMetaObject::invokeMethod(tableWidget_, "removeItem",
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
    const Icd::ComplexItemPtr complex = JHandlePtrCast<Icd::ComplexItem>(data);
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

    const Icd::ComplexItemPtr complex = JHandlePtrCast<Icd::ComplexItem>(data);
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
    if (tableWidget_->rowCount() > 0) {
        height = tableWidget_->contentsHeight();
    } else {
        height = 30;
    }

    setFixedHeight(height + titleBarHeight() + 1);
}

// class ItemWidgetFrame

ItemWidgetFrame::ItemWidgetFrame(QWidget *parent)
    : DataItemWidget(ItemTypeFrame, parent)
{
    tabWidget_ = new IcdTabWidget(this);
    tabWidget_->setContentsMargins(0, 0, 0, 0);
    clientLayout()->addWidget(tabWidget_);

    //
    connect(tabWidget_, &IcdTabWidget::currentChanged, this, [=](){
        fixHeight(tabWidget_->currentIndex());
    });
    connect(tabWidget_, &IcdTabWidget::heightChanged, this, [=](){
        fixHeight(tabWidget_->currentIndex());
    });
    connect(tabWidget_, &IcdTabWidget::itemRemoved, this, [=](){
        fixHeight(tabWidget_->currentIndex());
    });
}

ItemWidgetFrame::~ItemWidgetFrame()
{

}

bool ItemWidgetFrame::addDataItem(const QString &domain)
{
    //
    const Icd::FrameItemPtr frame = JHandlePtrCast<Icd::FrameItem>(dataItem());
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
            if (!tabWidget_->addDataItem(domain, worker(), table, itemTable)) {
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
                if (!tabWidget_->addDataItem(domain, worker(), table, itemTable)) {
                    return false;
                }
                //
                QApplication::processEvents(QEventLoop::EventLoopExec);
                break;
            }
        }
    }

    tabWidget_->setCurrentIndex(0);

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
                tabWidget_, "focusItem", Q_RETURN_ARG(int, offset),
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
    if (!QMetaObject::invokeMethod(tabWidget_, "removeItem", Q_ARG(QString, domain))) {
        return;
    }

    //
    fixHeight(tabWidget_->currentIndex());
}

void ItemWidgetFrame::restoreUi(const Icd::ItemPtr &data)
{
    //
    DataItemWidget::restoreUi(data);

    //
    const Icd::FrameItemPtr itemFrame = JHandlePtrCast<Icd::FrameItem>(data);
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

    const Icd::FrameItemPtr itemFrame = JHandlePtrCast<Icd::FrameItem>(data);
    if (!itemFrame) {
        return false;
    }

    //
    fixHeight(tabWidget_->currentIndex());

    return true;
}

void ItemWidgetFrame::fixHeight(int index)
{
    //
    DataTableWidget *tableWidget = qobject_cast<DataTableWidget *>(tabWidget_->widget(index));
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

    setFixedHeight(height + tabWidget_->tabBar()->height()
                   + titleBarHeight() + 2);
}

}
