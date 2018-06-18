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

    groupType_ = new QGroupBox(QStringLiteral("������Ϣ"), this);
    groupType_->setVisible(false);
    layoutMain->addWidget(groupType_);

    QFormLayout *formLayout = new QFormLayout(groupType_);
    formLayout->setContentsMargins(6, 3, 0, 3);
    boxType_ = new QComboBox(this);
    formLayout->addRow(QStringLiteral("��   �ͣ�"), boxType_);

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
        initTypeInfo(data);
    }

    currentMetaUi_->setUIData(data);

    switch (metaUiType) {
    case MetaUI::wdHeader:
    case MetaUI::wdCounter:
    case MetaUI::wdCheck:
    case MetaUI::wdFrameCode:
    case MetaUI::wdCommon:
    case MetaUI::wdBitMap:
    case MetaUI::wdBitValue:
    case MetaUI::wdDiscern:
    case MetaUI::wdComplex:
    case MetaUI::wdBuffer:
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

    // ����ǹ������ݣ������³�ʼ����������������
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
    // ԭʼ����
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
    // ��ȡԭʼ����
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
    case GlobalDefine::dtF64:
        metaUiType = MetaUI::wdCommon;
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

    // ����ˢ�½���
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
        case GlobalDefine::dtF32:
        case GlobalDefine::dtF64:
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
        default:break;
        }
    }
    enableConnection(false);

    // ��ʼ��������
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
