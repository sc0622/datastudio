#ifndef JSEARCHWATCHER_H
#define JSEARCHWATCHER_H

#include "../icdmeta_global.h"
#include <QFutureWatcher>
#include "./JWatcher.h"

namespace icdmeta {

// class JSearchWatcher

class ICDMETA_EXPORT JSearchWatcher : public JWatcher<QString>
{
    Q_OBJECT
public:
    JSearchWatcher(QJSValue callback, QObject *target, QObject *parent = nullptr);
    virtual ~JSearchWatcher();

signals:

public slots:
    void cancel();

protected:
    void customEvent(QEvent *event);

private:
};

}

#endif // JSEARCHWATCHER_H
