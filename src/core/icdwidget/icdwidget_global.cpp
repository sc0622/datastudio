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
    QStringList items;
    items << tr("Header");
    items << tr("Counter");
    items << tr("Check");
    items << tr("FrameCode");
    items << tr("Numeric");
    items << tr("Array");
    items << tr("BitMap");
    items << tr("BitValue");
    items << tr("Complex");
    items << tr("Frame");
    return items;
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
