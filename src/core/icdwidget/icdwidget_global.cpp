#include "precomp.h"
#include "icdwidget_global.h"

// class IcdWidgetPrivate

class IcdWidgetPrivate
{
public:
    IcdWidgetPrivate(IcdWidget *q)
        : J_QPTR(q)
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(IcdWidget)
    QList<SingletonReleaseCallback> callbacks;
};

void IcdWidgetPrivate::init()
{

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

    foreach (auto &callback, d->callbacks) {
        if (callback) {
            callback();
        }
    }

    delete d;

    Q_CLEANUP_RESOURCE(resource);
}
