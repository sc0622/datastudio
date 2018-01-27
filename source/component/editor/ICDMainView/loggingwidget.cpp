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
#include "icdfactory.h"
#include "icdbitdata.h"
#include "complexui.h"
#include "discernui.h"
#include "framecodeui.h"
#include "bufferui.h"

LoggingWidget::LoggingWidget(IJAttempter &attempter, QWidget *parent)
    : q_attempter(attempter)
    , QWidget(parent)
{
    q_groupType = new QGroupBox(QStringLiteral("������Ϣ"), this);
    q_boxType = new QComboBox(this);
    QFormLayout *formLayout = new QFormLayout(q_groupType);
    formLayout->addRow(QStringLiteral("���ͣ�"), q_boxType);

    MetaUI *metaUI = new MetaUI(this);
    metaUI->setObjectName("meta");
    HeaderUI *headerUI = new HeaderUI(attempter, this);
    headerUI->setObjectName("header");
    CounterUI *counterUI = new CounterUI(attempter, this);
    counterUI->setObjectName("counter");
    CheckUI *checkUI = new CheckUI(attempter, this);
    checkUI->setObjectName("check");
    FrameCodeUI *frameCodeUI = new FrameCodeUI(attempter, this);
    frameCodeUI->setObjectName("frameCode");
    CommonUI *commonUI = new CommonUI(attempter, this);
    commonUI->setObjectName("common");
    BitMapUI *bitMapUI = new BitMapUI(attempter, this);
    bitMapUI->setObjectName("bitMap");
    BitValueUI *bitValueUI = new BitValueUI(attempter, this);
    bitValueUI->setObjectName("bitValue");
    DiscernUI *discernUI = new DiscernUI(attempter, this);
    discernUI->setObjectName("discern");
    ComplexUI *complexUI = new ComplexUI(attempter, this);
    complexUI->setObjectName("complex");
    BufferUI *bufferUI = new BufferUI(attempter, this);
    bufferUI->setObjectName("buffer");
    SubTableUI *subTableUI = new SubTableUI(attempter, this);
    subTableUI->setObjectName("subTable");
    TableUI *tableUI = new TableUI(attempter, this);
    tableUI->setObjectName("table");
    SystemUI *systemUI = new SystemUI(attempter, this);
    systemUI->setObjectName("system");
    PlaneUI *planeUI = new PlaneUI(attempter, this);
    planeUI->setObjectName("plane");

    q_stack = new QStackedWidget(this);
    q_relation[q_stack->addWidget(metaUI)] = wdUnknown;
    q_relation[q_stack->addWidget(headerUI)] = wdHeader;
    q_relation[q_stack->addWidget(counterUI)] = wdCounter;
    q_relation[q_stack->addWidget(checkUI)] = wdCheck;
    q_relation[q_stack->addWidget(frameCodeUI)] = wdFrameCode;
    q_relation[q_stack->addWidget(commonUI)] = wdCommon;
    q_relation[q_stack->addWidget(bitMapUI)] = wdBitMap;
    q_relation[q_stack->addWidget(bitValueUI)] = wdBitValue;
    q_relation[q_stack->addWidget(discernUI)] = wdDiscern;
    q_relation[q_stack->addWidget(complexUI)] = wdComplex;
    q_relation[q_stack->addWidget(bufferUI)] = wdBuffer;
    q_relation[q_stack->addWidget(subTableUI)] = wdSubTable;
    q_relation[q_stack->addWidget(tableUI)] = wdTable;
    q_relation[q_stack->addWidget(systemUI)] = wdSystem;
    q_relation[q_stack->addWidget(planeUI)] = wdPlane;
    q_stack->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* veriLayouMain = new QVBoxLayout(this);
    veriLayouMain->setContentsMargins(5, 0, 2, 10);
    veriLayouMain->addWidget(q_groupType);
    //veriLayouMain->addSpacing(10);
    veriLayouMain->addWidget(q_stack);

    //
    //connect(metaUI, SIGNAL(confirm(bool &)),
    //        this, SLOT(slotConfirm(bool &)));
    connect(headerUI, SIGNAL(confirm(bool &)), 
            this, SLOT(slotConfirm(bool &)));
    connect(counterUI, SIGNAL(confirm(bool &)),
            this, SLOT(slotConfirm(bool &)));
    connect(checkUI, SIGNAL(confirm(bool &)), 
            this, SLOT(slotConfirm(bool &)));
    connect(frameCodeUI, SIGNAL(confirm(bool &)),
            this, SLOT(slotConfirm(bool &)));
    connect(commonUI, SIGNAL(confirm(bool &)), 
            this, SLOT(slotConfirm(bool &)));
    connect(bitMapUI, SIGNAL(confirm(bool &)),
            this, SLOT(slotConfirm(bool &)));
    connect(bitValueUI, SIGNAL(confirm(bool &)),
            this, SLOT(slotConfirm(bool &)));
    connect(discernUI, SIGNAL(confirm(bool &)),
            this, SLOT(slotConfirm(bool &)));
    connect(complexUI, SIGNAL(confirm(bool &)),
            this, SLOT(slotConfirm(bool &)));
    connect(bufferUI, SIGNAL(confirm(bool &)),
            this, SLOT(slotConfirm(bool &)));
    connect(subTableUI, SIGNAL(confirm(bool &)),
            this, SLOT(slotConfirm(bool &)));
    connect(tableUI, SIGNAL(confirm(bool &)), 
            this, SLOT(slotConfirm(bool &)));
    connect(systemUI, SIGNAL(confirm(bool &)), 
            this, SLOT(slotConfirm(bool &)));
    connect(planeUI, SIGNAL(confirm(bool &)),
            this, SLOT(slotConfirm(bool &)));

    //connect(metaUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(headerUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(counterUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(checkUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(frameCodeUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(commonUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(bitMapUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(bitValueUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(discernUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(complexUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(bufferUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(subTableUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(tableUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(systemUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    connect(planeUI, SIGNAL(canceled()), this, SLOT(slotCancel()));
    enableConnection(true);

    // ��¼
    q_widget[wdUnknown] = metaUI;
    q_widget[wdHeader] = headerUI;
    q_widget[wdCounter] = counterUI;
    q_widget[wdCheck] = checkUI;
    q_widget[wdFrameCode] = frameCodeUI;
    q_widget[wdCommon] = commonUI;
    q_widget[wdBitMap] = bitMapUI;
    q_widget[wdBitValue] = bitValueUI;
    q_widget[wdDiscern] = discernUI;
    q_widget[wdComplex] = complexUI;
    q_widget[wdBuffer] = bufferUI;
    q_widget[wdSubTable] = subTableUI;
    q_widget[wdTable] = tableUI;
    q_widget[wdSystem] = systemUI;
    q_widget[wdPlane] = planeUI;
    // 
    q_groupType->setVisible(false);
}

// ���õ�ǰ��ʾ����
void LoggingWidget::setCurrentWidget(LoggingWidget::WIDGEGTYPE type)
{
    if (!q_stack) {
        return;
    }
    QHash<WIDGEGTYPE, MetaUI*>::iterator it = q_widget.find(type);
    if (it != q_widget.end()) {
        q_stack->setCurrentWidget(it.value());
    } else {
        q_stack->setCurrentWidget(q_widget[wdUnknown]);
    }
}

// ��ʼ����������
void LoggingWidget::initUIData(LoggingWidget::WIDGEGTYPE type, 
                               const _UIData &data)
{
    if (!q_stack) {
        return;
    }
    // ��ʼ��֮ǰ���ȶϿ���һ�����źŲ�����
    MetaUI *preWidget = q_widget[q_relation[q_stack->currentIndex()]];
    if (preWidget) {
        preWidget->enableConnection(false);
    }
    setCurrentWidget(type);

    QHash<WIDGEGTYPE, MetaUI*>::iterator it = q_widget.find(type);
    if (it != q_widget.end()) {
        it.value()->setUIData(data);
    }
    switch (type)
    {
    case wdHeader:
    case wdCounter:
    case wdCheck:
    case wdFrameCode:
    case wdCommon:
    case wdBitMap:
    case wdBitValue:
    case wdDiscern:
    case wdComplex:
    case wdBuffer:
        q_groupType->setVisible(true);
        initTypeInfo(data);
        break;
    default:
        q_groupType->setVisible(false);
        break;
    }
}

// ��ȡ��������
void *LoggingWidget::uiData()
{
    MetaUI *widget = qobject_cast<MetaUI*>(q_stack->currentWidget());
    if (widget) {
        return widget->uiData();
    }
    return widget;
}

void LoggingWidget::enableOptionButton(bool enable)
{
    MetaUI *widget = qobject_cast<MetaUI*>(q_stack->currentWidget());
    if (widget) {
        widget->enableOptionButton(enable);
    }
}

// ��������
void LoggingWidget::slotConfirm(bool &result)
{
    void *data = NULL;
    WIDGEGTYPE type = q_relation[q_stack->currentIndex()];
    MetaUI *widget = q_widget[type];

    if (widget) {
        data = widget->uiData();
        if (data) {
            emit dataSaved(data, result);
            if (result) {   // ����ǹ������ݣ������³�ʼ����������������
                if (type >= wdHeader && type <= wdComplex) {
                    _UIData _data;

                    _data.data = data;
                    _data.type = GlobalDefine::optEdit;
                    initTypeInfo(_data);
                }
            }
        }
    }
}

// ȡ��
void LoggingWidget::slotCancel()
{
    MetaUI *widget = q_widget[q_relation[q_stack->currentIndex()]];
    if (widget) {
        if (q_boxType->isVisible()) {
            enableConnection(false);

            q_boxType->setCurrentIndex(
                q_boxType->findData(widget->originalType()));

            enableConnection(true);
        }
    }
    emit canceled();
}

// �������ͱ��
void LoggingWidget::slotTypeChanged(int index)
{
    Q_UNUSED(index);
    // ԭʼ����
    MetaUI *widget = q_widget[q_relation[q_stack->currentIndex()]];
    if (!widget) {
        return;
    }
    ICDCommonData::smtCommon &smtData
        = *reinterpret_cast<ICDCommonData::smtCommon*>(widget->uiData());
    if (!smtData) {
        return;
    }
    bool newData = ((GlobalDefine::dtTotal - GlobalDefine::dtHead)
                    == q_boxType->count());
    stTableRules rule = ICDFactory::instance().convert2Rule(smtData);
    WIDGEGTYPE type = wdCommon;
    // ��ȡԭʼ����
    switch (rule.uType = q_boxType->currentData().toInt()) {
    case GlobalDefine::dtHead:
        type = wdHeader;
        break;
    case GlobalDefine::dtCounter:
        rule.sDefault = QString::number(GlobalDefine::counterU8).toStdString();
        type = wdCounter;
        break;
    case GlobalDefine::dtCheck:
        rule.sDefault = QString::number(GlobalDefine::ctInvalid).toStdString();
        type = wdCheck;
        break;
    case GlobalDefine::dtFrameCode:
        rule.sDefault = QString::number(GlobalDefine::frame8).toStdString();
        type = wdFrameCode;
        break;
    case GlobalDefine::dtU8:
    case GlobalDefine::dt8:
        type = wdCommon;
        break;
    case GlobalDefine::dtU16:
    case GlobalDefine::dt16:
        type = wdCommon;
        break;
    case GlobalDefine::dtU32:
    case GlobalDefine::dt32:
    case GlobalDefine::dtF32:
        type = wdCommon;
        break;
    case GlobalDefine::dtF64:
        type = wdCommon;
        break;
    case GlobalDefine::dtBitMap:
        type = wdBitMap;
        break;
    case GlobalDefine::dtBitValue:
        type = wdBitValue;
        break;
    case GlobalDefine::dtComplex:
        type = wdComplex;
        break;
    case GlobalDefine::dtDiscern:
        type = wdDiscern;
        rule.dOffset = 1.0;
        break;
    case GlobalDefine::dtBuffer:
        type = wdBuffer;
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
    initUIData(type, data);
    if (!newData) {
        if (widget = q_widget[q_relation[q_stack->currentIndex()]]) {
            widget->changeDataType(rule.uType);
        }
    }
}

// ��ʼ������������Ϣ
void LoggingWidget::initTypeInfo(const _UIData &data)
{
    if (!q_boxType || !data.data) {
        return;
    }
    std::vector<stDictionary> dics;
    q_attempter.notifier().sendMessage("queryDictionaryTable@DataManegement",
                                       (JWPARAM)&dics,
                                       GlobalDefine::dicDataType);
    const int count = dics.size();
    int lowerBound = GlobalDefine::dtHead;
    int upperBound = GlobalDefine::dtBuffer;
    const ICDMetaData::smtMeta &meta
        = *reinterpret_cast<ICDMetaData::smtMeta*>(data.data);
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
    q_boxType->clear();
    for (int i = 0; i < count; ++i) {
        const stDictionary &dic = dics.at(i);
        if (dic.nCode < lowerBound) {
            continue;
        } else if (dic.nCode > upperBound) {
            break;
        } else {
            q_boxType->addItem(dic.sDec.c_str(), dic.nCode);
        }
    }
    q_boxType->setCurrentIndex(q_boxType->findData(meta->type()));

    enableConnection(true);
}

void LoggingWidget::enableConnection(bool enable)
{
    disconnect(q_boxType, SIGNAL(currentIndexChanged(int)),
               this, SLOT(slotTypeChanged(int)));
    if (enable) {
        connect(q_boxType, SIGNAL(currentIndexChanged(int)),
                this, SLOT(slotTypeChanged(int)));
    }
}
