#include "precomp.h"
#include "icdparser_global.h"

// class IcdParserPrivate

class IcdParserPrivate
{
public:
    IcdParserPrivate()
#ifndef J_NO_QT
        : translator(nullptr)
    #endif
    {

    }

private:
    friend class IcdParser;
#ifndef J_NO_QT
    QTranslator *translator;
#endif
    std::list<SingletonReleaseCallback> callbacks;
};

// class IcdParser

J_IMPLEMENT_SINGLE_INSTANCE(IcdParser, IcdParser)

bool IcdParser::init()
{
#ifndef J_NO_QT
    if (qApp) {
        d->translator = new QTranslator();
        bool result = d->translator->load(":/icdparser/lang/zh_CN.qm");
        if (result) {
            qApp->installTranslator(d->translator);
        }
    }
#endif
    return true;
}

void IcdParser::registerSingletonRelease(SingletonReleaseCallback callback)
{
    if (!callback) {
        return;
    }

    for (std::list<SingletonReleaseCallback>::const_iterator citer = d->callbacks.cbegin();
         citer != d->callbacks.cend(); ++citer) {
        if (*citer == callback) {
            return;
        }
    }

    d->callbacks.push_back(callback);
}

IcdParser::IcdParser()
    : d(new IcdParserPrivate())
{
#ifndef J_NO_QT
    Q_INIT_RESOURCE(resource);
#endif
}

IcdParser::~IcdParser()
{
#ifndef J_NO_QT
    if (d->translator) {
        qApp->removeTranslator(d->translator);
        delete d->translator;
    }

    foreach (auto &callback, d->callbacks) {
        if (callback) {
            callback();
        }
    }

    Q_CLEANUP_RESOURCE(resource);
#endif

    delete d;
}
