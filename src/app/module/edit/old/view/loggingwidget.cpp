#include "precomp.h"
#include "loggingwidget.h"
#include "bitmapui.h"
#include "bitvalueui.h"
#include "counterui.h"
#include "checkui.h"
#include "commonui.h"
#include "headerui.h"
#include "planeui.h"
#include "systemui.h"
#include "tableui.h"
#include "subtableui.h"
#include "KernelClass/icdfactory.h"
#include "KernelClass/icdbitdata.h"
#include "complexui.h"
#include "discernui.h"
#include "framecodeui.h"
#include "bufferui.h"

LoggingWidget::LoggingWidget(QWidget *parent)
    : QWidget(parent)
    , currentMetaUi_(nullptr)
{
    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setContentsMargins(0, 0, 0, 0);
    layoutMain->setSpacing(1);

    groupType_ = new QGroupBox(QStringLiteral("类型信息"), this);
    groupType_->setVisible(false);
    layoutMain->addWidget(groupType_);

    QFormLayout *formLayout = new QFormLayout(groupType_);
    formLayout->setContentsMargins(6, 3, 0, 3);
    boxType_ = new QComboBox(this);
    formLayout->addRow(QStringLiteral("类   型："), boxType_);

    stackedWidget_ = new QStackedWidget(this);
    layoutMain->addWidget(stackedWidget_);

    enableConnection(true);
}

void LoggingWidget::initUIData(int metaUiType, const _UIData &data)
{
    if (currentMetaUi_) {
        if (metaUiType != currentMetaUi_->uiType()) {
            stackedWidget_->removeWidget(stackedWidget_);
            delete currentMetaUi_;
            currentMetaUi_ = nullptr;
        }
    }

    if (!currentMetaUi_) {
        currentMetaUi_ = MetaUI::create(metaUiType);
        if (!currentMetaUi_) {
            return;
        }
        stackedWidget_->addWidget(currentMetaUi_);
        connect(currentMetaUi_, &MetaUI::confirmed, this, &LoggingWidget::slotConfirm);
        connect(currentMetaUi_, &MetaUI::canceled, this, &LoggingWidget::slotCancel);
    }

    currentMetaUi_->setUIData(data);

    switch (metaUiType) {
    case MetaUI::wdHeader:
    case MetaUI::wdCounter:
    case MetaUI::wdCheck:
    case MetaUI::wdFrameCode:
    case MetaUI::wdCommon:
    case MetaUI::wdArray:
    case MetaUI::wdBitMap:
    case MetaUI::wdBitValue:
    case MetaUI::wdDiscern:
    case MetaUI::wdComplex:
    case MetaUI::wdBuffer:
        initTypeInfo(data);
        groupType_->setVisible(true);
        break;
    default:
        groupType_->setVisible(false);
        break;
    }
}

void *LoggingWidget::uiData()
{
    if (currentMetaUi_) {
        return currentMetaUi_->uiData();
    } else {
        return nullptr;
    }
}

void LoggingWidget::enableOptionButton(bool enable)
{
    if (currentMetaUi_) {
        currentMetaUi_->enableOptionButton(enable);
    }
}

void LoggingWidget::slotConfirm(bool &result)
{
    if (!currentMetaUi_) {
        return;
    }

    void *data = currentMetaUi_->uiData();
    if (!data) {
        return;
    }

    emit dataSaved(data, result);
    if (!result) {
        return;
    }

    // 如果是规则数据，则重新初始化数据类型下拉框
    if (!currentMetaUi_) {
        return;
    }

    data = currentMetaUi_->uiData();
    if (!data) {
        return;
    }

    const int uiType = currentMetaUi_->uiType();
    if (uiType >= MetaUI::wdHeader && uiType <= MetaUI::wdComplex) {
        _UIData _data;
        _data.data = data;
        _data.type = GlobalDefine::optEdit;
        initTypeInfo(_data);
    }
}

void LoggingWidget::slotCancel()
{
    if (currentMetaUi_) {
        if (boxType_->isVisible()) {
            enableConnection(false);
            boxType_->setCurrentIndex(boxType_->findData(currentMetaUi_->originalType()));
            enableConnection(true);
        }
    }

    emit canceled();
}

void LoggingWidget::slotTypeChanged(int index)
{
    Q_UNUSED(index);
    // 原始数据
    if (!currentMetaUi_) {
        return;
    }

    ICDCommonData::smtCommon &smtData =
            *reinterpret_cast<ICDCommonData::smtCommon*>(currentMetaUi_->uiData());
    if (!smtData) {
        return;
    }

    bool newData = ((GlobalDefine::dtTotal - GlobalDefine::dtHead) == boxType_->count());
    stTableRules rule = ICDFactory::instance().convert2Rule(smtData);
    int metaUiType = MetaUI::wdCommon;
    // 获取原始数据
    switch (rule.uType = boxType_->currentData().toInt()) {
    case GlobalDefine::dtHead:
        metaUiType = MetaUI::wdHeader;
        break;
    case GlobalDefine::dtCounter:
        rule.sDefault = QString::number(GlobalDefine::counterU8).toStdString();
        metaUiType = MetaUI::wdCounter;
        break;
    case GlobalDefine::dtCheck:
        rule.sDefault = QString::number(GlobalDefine::ctInvalid).toStdString();
        metaUiType = MetaUI::wdCheck;
        break;
    case GlobalDefine::dtFrameCode:
        rule.sDefault = QString::number(GlobalDefine::frame8).toStdString();
        metaUiType = MetaUI::wdFrameCode;
        break;
    case GlobalDefine::dtU8:
    case GlobalDefine::dt8:
        metaUiType = MetaUI::wdCommon;
        break;
    case GlobalDefine::dtU16:
    case GlobalDefine::dt16:
        metaUiType = MetaUI::wdCommon;
        break;
    case GlobalDefine::dtU32:
    case GlobalDefine::dt32:
    case GlobalDefine::dtF32:
        metaUiType = MetaUI::wdCommon;
        break;
    case GlobalDefine::dtU64:
    case GlobalDefine::dt64:
    case GlobalDefine::dtF64:
        metaUiType = MetaUI::wdCommon;
        break;
    case GlobalDefine::dtArray:
        metaUiType = MetaUI::wdArray;
        break;
    case GlobalDefine::dtBitMap:
        metaUiType = MetaUI::wdBitMap;
        break;
    case GlobalDefine::dtBitValue:
        metaUiType = MetaUI::wdBitValue;
        break;
    case GlobalDefine::dtComplex:
        metaUiType = MetaUI::wdComplex;
        break;
    case GlobalDefine::dtDiscern:
        metaUiType = MetaUI::wdDiscern;
        rule.dOffset = 1.0;
        break;
    case GlobalDefine::dtBuffer:
        metaUiType = MetaUI::wdBuffer;
		break;
    default:
        break;
    }

    _UIData data;
    ICDMetaData::smtMeta meta = ICDFactory::instance().CreatObject(rule);
    data.data = &meta;
    if (newData) {
        data.type = GlobalDefine::optNew;
    }

    // 重新刷新界面
    initUIData(metaUiType, data);
    if (!newData && currentMetaUi_) {
        currentMetaUi_->changeDataType(rule.uType);
    }
}

void LoggingWidget::initTypeInfo(const _UIData &data)
{
    if (!boxType_ || !data.data) {
        return;
    }

    std::vector<stDictionary> dics;
    QVariantList args;
    args.append(qVariantFromValue((void*)&dics));
    args.append(int(GlobalDefine::dicDataType));
    jnotify->send("edit.queryDictionaryTable", args);

    const int count = dics.size();
    int lowerBound = GlobalDefine::dtHead;
    int upperBound = GlobalDefine::dtBuffer;
    const ICDMetaData::smtMeta &meta = *reinterpret_cast<ICDMetaData::smtMeta*>(data.data);
    if (!meta) {
        Q_ASSERT(false);
        return;
    }
    if (GlobalDefine::optNew != data.type) {
        switch (meta->type()) {
        case GlobalDefine::dtHead:
        case GlobalDefine::dtCounter:
        case GlobalDefine::dtCheck:
        case GlobalDefine::dtFrameCode:
        case GlobalDefine::dtU8:
        case GlobalDefine::dt8:
        case GlobalDefine::dtU16:
        case GlobalDefine::dt16:
        case GlobalDefine::dtU32:
        case GlobalDefine::dt32:
        case GlobalDefine::dtU64:
        case GlobalDefine::dt64:
        case GlobalDefine::dtF32:
        case GlobalDefine::dtF64:
        case GlobalDefine::dtArray:
        case GlobalDefine::dtBitMap:
        case GlobalDefine::dtBitValue:
            upperBound = GlobalDefine::dtBitValue;
            break;
        case GlobalDefine::dtComplex:
            lowerBound = GlobalDefine::dtComplex;
            upperBound = GlobalDefine::dtComplex;
            break;
        case GlobalDefine::dtDiscern:
            lowerBound = GlobalDefine::dtDiscern;
            upperBound = GlobalDefine::dtDiscern;
            break;
        case GlobalDefine::dtBuffer:
            lowerBound = GlobalDefine::dtBuffer;
            upperBound = GlobalDefine::dtBuffer;
            break;
        default:
            break;
        }
    }
    enableConnection(false);

    // 初始化下拉框
    boxType_->clear();
    for (int i = 0; i < count; ++i) {
        const stDictionary &dic = dics.at(i);
        if (dic.nCode < lowerBound) {
            continue;
        } else if (dic.nCode > upperBound) {
            break;
        } else {
            boxType_->addItem(dic.sDec.c_str(), dic.nCode);
        }
    }
    boxType_->setCurrentIndex(boxType_->findData(meta->type()));

    enableConnection(true);
}

void LoggingWidget::enableConnection(bool enable)
{
    disconnect(boxType_, SIGNAL(currentIndexChanged(int)), this, SLOT(slotTypeChanged(int)));
    if (enable) {
        connect(boxType_, SIGNAL(currentIndexChanged(int)), this, SLOT(slotTypeChanged(int)));
    }
}
