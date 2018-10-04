#include "precomp.h"
#include "icdwidget_global.h"

// class IcdWidgetPrivate

class IcdWidgetPrivate
{
public:
    IcdWidgetPrivate(IcdWidget *q)
        : J_QPTR(q)
        , translator(nullptr)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(IcdWidget)
    QTranslator *translator;
    QList<SingletonReleaseCallback> callbacks;
};

void IcdWidgetPrivate::init()
{
    if (qApp) {
        translator = new QTranslator();
        bool result = translator->load(":/icdwidget/lang/zh_CN.qm");
        if (result) {
            qApp->installTranslator(translator);
        }
    }
}

// class IcdCore

J_IMPLEMENT_SINGLE_INSTANCE(IcdWidget, IcdWidget)

bool IcdWidget::init()
{
    return true;
}

void IcdWidget::registerSingletonRelease(SingletonReleaseCallback callback)
{
    if (!callback) {
        return;
    }

    Q_D(IcdWidget);
    if (d->callbacks.contains(callback)) {
        return;
    }

    d->callbacks.append(callback);
}

QStringList IcdWidget::protoItemTypes()
{
    return protoItemMapTypes().values();
}

QMap<int, QString> IcdWidget::protoItemMapTypes()
{
    QMap<int, QString> items;
    items[Icd::ItemHeader] = tr("Header");
    items[Icd::ItemCounter] = tr("Counter");
    items[Icd::ItemCheck] = tr("Check");
    items[Icd::ItemFrameCode] = tr("FrameCode");
    items[Icd::ItemNumeric] = tr("Numeric");
    items[Icd::ItemArray] = tr("Array");
    items[Icd::ItemBitMap] = tr("BitMap");
    items[Icd::ItemBitValue] = tr("BitValue");
    items[Icd::ItemComplex] = tr("Complex");
    items[Icd::ItemFrame] = tr("Frame");
    return items;
}

QString IcdWidget::typeString(const Icd::ItemPtr &item)
{
    if (!item) {
        return QString();
    }

    switch (item->type()) {
    case Icd::ItemHeader: return tr("Header");
    case Icd::ItemCounter:
    {
        const Icd::CounterItemPtr counterItem = JHandlePtrCast<Icd::CounterItem>(item);
        if (!counterItem) {
            break;
        }
        return counterTypeString(counterItem->counterType());
    }
    case Icd::ItemCheck:
    {
        const Icd::CheckItemPtr checkItem = JHandlePtrCast<Icd::CheckItem>(item);
        if (!checkItem) {
            break;
        }
        return checkTypeString(checkItem->checkType());
    }
    case Icd::ItemFrameCode:
    {
        const Icd::FrameCodeItemPtr frameCodeItem = JHandlePtrCast<Icd::FrameCodeItem>(item);
        if (!frameCodeItem) {
            break;
        }
        return frameCodeTypeString(frameCodeItem->frameCodeType());
    }
    case Icd::ItemNumeric:
    {
        const Icd::NumericItemPtr numericItem = JHandlePtrCast<Icd::NumericItem>(item);
        if (!numericItem) {
            break;
        }
        return numericTypeString(numericItem->numericType());
    }
    case Icd::ItemBitMap: return tr("BitMap");
    case Icd::ItemBitValue: return tr("BitValue");
    case Icd::ItemComplex: return tr("Complex");
    case Icd::ItemFrame: return tr("Frame");
    default:
        break;
    }

    return tr("Unknown");
}

QString IcdWidget::counterTypeString(int counterType)
{
    switch (counterType) {
    case Icd::CounterU8: return tr("Counter U8");
    case Icd::CounterU16: return tr("Counter U16");
    case Icd::CounterU32: return tr("Counter U32");
    case Icd::CounterU64: return tr("Counter U64");
    default:
        break;
    }

    return tr("Unknown");
}

QString IcdWidget::checkTypeString(int checkType)
{
    switch (checkType) {
    case Icd::CheckNone: return tr("No check");
    case Icd::CheckSum8: return tr("Check sum8");
    case Icd::CheckSum16: return tr("Check sum16");
    case Icd::CheckCrc8: return tr("Check crc8");
    case Icd::CheckCrc16: return tr("Check crc16");
    case Icd::CheckXor8: return tr("Check xor8");
    case Icd::CheckXor16: return tr("Check xor16");
    default:
        break;
    }

    return tr("Unknown");
}

QString IcdWidget::numericTypeString(int numericType)
{
    switch (numericType) {
    case Icd::NumericI8: return tr("Int8");
    case Icd::NumericU8: return tr("UInt8");
    case Icd::NumericI16: return tr("Int16");
    case Icd::NumericU16: return tr("UInt16");
    case Icd::NumericI32: return tr("Int32");
    case Icd::NumericU32: return tr("UInt32");
    case Icd::NumericI64: return tr("Int64");
    case Icd::NumericU64: return tr("UInt64");
    case Icd::NumericF32: return tr("Float32");
    case Icd::NumericF64: return tr("Float64");
    default:
        break;
    }

    return tr("Unknown");
}

QString IcdWidget::arrayTypeString(int arrayType)
{
    switch (arrayType) {
    case Icd::ArrayI8: return tr("Int8");
    case Icd::ArrayU8: return tr("UInt8");
    case Icd::ArrayI16: return tr("Int16");
    case Icd::ArrayU16: return tr("UInt16");
    case Icd::ArrayI32: return tr("Int32");
    case Icd::ArrayU32: return tr("UInt32");
    case Icd::ArrayI64: return tr("Int64");
    case Icd::ArrayU64: return tr("UInt64");
    case Icd::ArrayF32: return tr("Float32");
    case Icd::ArrayF64: return tr("Float64");
    default:
        break;
    }

    return tr("Unknown");
}

QString IcdWidget::frameCodeTypeString(int frameCodeType)
{
    switch (frameCodeType) {
    case Icd::FrameCodeU8: return tr("FrameCode U8");
    case Icd::FrameCodeU16: return tr("FrameCode U16");
    case Icd::FrameCodeU32: return tr("FrameCode U32");
    case Icd::FrameCodeU64: return tr("FrameCode U64");
    default:
        break;
    }

    return tr("Unknown");
}

QString IcdWidget::prettyValue(double value, bool isFloat)
{
    return QString::fromStdString(Icd::NumericItem::prettyValue(value, isFloat
                                                                ? Icd::NumericF32 : Icd::NumericF64));
}

IcdWidget::IcdWidget(QObject *parent)
    : QObject(parent)
    , J_DPTR(new IcdWidgetPrivate(this))
{
    Q_INIT_RESOURCE(resource);
    Q_D(IcdWidget);
    d->init();
}

IcdWidget::~IcdWidget()
{
    Q_D(IcdWidget);

    if (d->translator) {
        qApp->removeTranslator(d->translator);
        delete d->translator;
    }

    foreach (auto &callback, d->callbacks) {
        if (callback) {
            callback();
        }
    }

    delete d;

    Q_CLEANUP_RESOURCE(resource);
}
