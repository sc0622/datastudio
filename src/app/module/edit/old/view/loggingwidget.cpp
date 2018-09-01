#include "precomp.h"
#include "loggingwidget.h"
#include "KernelClass/icdfactory.h"
#include "KernelClass/icdbitdata.h"
#include "BitMapEdit.h"
#include "BitValueEdit.h"
#include "CounterEdit.h"
#include "CheckEdit.h"
#include "HeaderEdit.h"
#include "VehicleEdit.h"
#include "SystemEdit.h"
#include "TableEdit.h"
#include "SubTableEdit.h"
#include "ComplexEdit.h"
#include "FrameEdit.h"
#include "FrameCodeEdit.h"
#include "BufferEdit.h"

LoggingWidget::LoggingWidget(QWidget *parent)
    : QWidget(parent)
    , currentEdit_(nullptr)
{
    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setContentsMargins(0, 0, 0, 0);
    layoutMain->setSpacing(1);

    groupType_ = new QGroupBox(tr("Type information"), this);
    groupType_->setVisible(false);
    layoutMain->addWidget(groupType_);

    QFormLayout *formLayout = new QFormLayout(groupType_);
    formLayout->setContentsMargins(3, 3, 3, 3);
    boxType_ = new QComboBox(this);
    formLayout->addRow(tr("Type:"), boxType_);

    stackedWidget_ = new QStackedWidget(this);
    layoutMain->addWidget(stackedWidget_);

    enableConnection(true);
}

bool LoggingWidget::initUIData(int windowType, const _UIData &data)
{
    if (currentEdit_) {
        if (windowType != currentEdit_->windowType()) {
            stackedWidget_->removeWidget(stackedWidget_);
            delete currentEdit_;
            currentEdit_ = nullptr;
        }
    }

    if (!currentEdit_) {
        currentEdit_ = ObjectEdit::create(windowType);
        if (!currentEdit_) {
            return false;
        }
        stackedWidget_->addWidget(currentEdit_);
        connect(currentEdit_, &ObjectEdit::confirmed, this, &LoggingWidget::slotConfirm);
        connect(currentEdit_, &ObjectEdit::canceled, this, &LoggingWidget::slotCancel);
    }

    if (!currentEdit_->setData(data)) {
        stackedWidget_->removeWidget(stackedWidget_);
        delete currentEdit_;
        currentEdit_ = nullptr;
        return false;
    }

    switch (windowType) {
    case ObjectEdit::wdItem:
    case ObjectEdit::wdHeader:
    case ObjectEdit::wdCounter:
    case ObjectEdit::wdCheck:
    case ObjectEdit::wdFrameCode:
    case ObjectEdit::wdArray:
    case ObjectEdit::wdNumeric:
    case ObjectEdit::wdBitMap:
    case ObjectEdit::wdBitValue:
    case ObjectEdit::wdFrame:
    case ObjectEdit::wdComplex:
    case ObjectEdit::wdBuffer:
        initTypeInfo(data);
        groupType_->setVisible(true);
        break;
    default:
        groupType_->setVisible(false);
        break;
    }

    return true;
}

void *LoggingWidget::uiData()
{
    if (currentEdit_) {
        return currentEdit_->nonData();
    } else {
        return nullptr;
    }
}

void LoggingWidget::enableCommit(bool enable)
{
    if (currentEdit_) {
        currentEdit_->enableCommit(enable);
    }
}

void LoggingWidget::slotConfirm(bool &result)
{
    if (!currentEdit_) {
        return;
    }

    void *data = currentEdit_->nonData();
    if (!data) {
        return;
    }

    emit dataSaved(data, result);
    if (!result) {
        return;
    }

    // 如果是规则数据，则重新初始化数据类型下拉框
    if (!currentEdit_) {
        return;
    }

    data = currentEdit_->nonData();
    if (!data) {
        return;
    }

    const int windowType = currentEdit_->windowType();
    if (windowType >= ObjectEdit::wdHeader && windowType <= ObjectEdit::wdComplex) {
        _UIData _data;
        _data.data = data;
        _data.type = GlobalDefine::optEdit;
        initTypeInfo(_data);
    }
}

void LoggingWidget::slotCancel()
{
    if (currentEdit_) {
        if (boxType_->isVisible()) {
            enableConnection(false);
            boxType_->setCurrentIndex(boxType_->findData(currentEdit_->originalType()));
            enableConnection(true);
        }
    }

    emit canceled();
}

void LoggingWidget::slotTypeChanged(int index)
{
    Q_UNUSED(index);
    // 原始数据
    if (!currentEdit_) {
        return;
    }

    ICDCommonData::smtCommon &smtData = *static_cast<ICDCommonData::smtCommon*>(currentEdit_->nonData());
    if (!smtData) {
        return;
    }

    bool newData = ((GlobalDefine::dtTotal - GlobalDefine::dtHead) == boxType_->count());
    stTableRules rule = ICDFactory::instance().convert2Rule(smtData);
    int windowType = ObjectEdit::wdItem;
    // 获取原始数据
    rule.uType = static_cast<unsigned short>(boxType_->currentData().toInt());
    switch (rule.uType) {
    case GlobalDefine::dtHead:
        windowType = ObjectEdit::wdHeader;
        break;
    case GlobalDefine::dtCounter:
        rule.sDefault = QString::number(GlobalDefine::counterU8).toStdString();
        windowType = ObjectEdit::wdCounter;
        break;
    case GlobalDefine::dtCheck:
        rule.sDefault = QString::number(GlobalDefine::ctInvalid).toStdString();
        windowType = ObjectEdit::wdCheck;
        break;
    case GlobalDefine::dtFrameCode:
        rule.sDefault = QString::number(GlobalDefine::frame8).toStdString();
        windowType = ObjectEdit::wdFrameCode;
        break;
    case GlobalDefine::dtArray:
        windowType = ObjectEdit::wdArray;
        break;
    case GlobalDefine::dtNumeric:
        windowType = ObjectEdit::wdNumeric;
        break;
    case GlobalDefine::dtBitMap:
        windowType = ObjectEdit::wdBitMap;
        break;
    case GlobalDefine::dtBitValue:
        windowType = ObjectEdit::wdBitValue;
        break;
    case GlobalDefine::dtComplex:
        windowType = ObjectEdit::wdComplex;
        break;
    case GlobalDefine::dtFrame:
        windowType = ObjectEdit::wdFrame;
        rule.dOffset = 1.0;
        break;
    case GlobalDefine::dtBuffer:
        windowType = ObjectEdit::wdBuffer;
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
    initUIData(windowType, data);
    if (!newData && currentEdit_) {
        currentEdit_->changeType(rule.uType);
    }
}

void LoggingWidget::initTypeInfo(const _UIData &data)
{
    if (!boxType_ || !data.data) {
        return;
    }

    std::vector<stDictionary> dics;
    QVariantList args;
    args.append(qVariantFromValue(static_cast<void*>(&dics)));
    args.append(int(GlobalDefine::dicDataType));
    jnotify->send("edit.queryDictionaryTable", args);

    const size_t count = dics.size();
    int lowerBound = GlobalDefine::dtHead;
    int upperBound = GlobalDefine::dtBuffer;
    const ICDMetaData::smtMeta &meta = *static_cast<ICDMetaData::smtMeta*>(data.data);
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
        case GlobalDefine::dtArray:
        case GlobalDefine::dtNumeric:
        case GlobalDefine::dtBitMap:
        case GlobalDefine::dtBitValue:
            upperBound = GlobalDefine::dtBitValue;
            break;
        case GlobalDefine::dtComplex:
            lowerBound = GlobalDefine::dtComplex;
            upperBound = GlobalDefine::dtComplex;
            break;
        case GlobalDefine::dtFrame:
            lowerBound = GlobalDefine::dtFrame;
            upperBound = GlobalDefine::dtFrame;
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
    for (size_t i = 0; i < count; ++i) {
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
