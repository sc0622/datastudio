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

    return d->callbacks.append(callback);
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
