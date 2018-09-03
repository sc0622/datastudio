#include "precomp.h"
#include "icdparser_global.h"

// class IcdParserPrivate

class IcdParserPrivate
{
public:
    IcdParserPrivate()
        : translator(nullptr)
    {

    }

private:
    friend class IcdParser;
    QTranslator *translator;
    QList<SingletonReleaseCallback> callbacks;
};

// class IcdParser

J_IMPLEMENT_SINGLE_INSTANCE(IcdParser, IcdParser)

bool IcdParser::init()
{
    if (qApp) {
        d->translator = new QTranslator();
        bool result = d->translator->load(":/icdparser/lang/zh_CN.qm");
        if (result) {
            qApp->installTranslator(d->translator);
        }
    }

    return true;
}

void IcdParser::registerSingletonRelease(SingletonReleaseCallback callback)
{
    if (!callback) {
        return;
    }

    if (d->callbacks.contains(callback)) {
        return;
    }

    d->callbacks.append(callback);
}

IcdParser::IcdParser()
    : d(new IcdParserPrivate())
{
    Q_INIT_RESOURCE(resource);
}

IcdParser::~IcdParser()
{
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
