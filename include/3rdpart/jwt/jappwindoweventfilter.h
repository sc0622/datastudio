#ifndef JAPPWINDOWEVENTFILTER_H
#define JAPPWINDOWEVENTFILTER_H

#include "jwt_global.h"
#include <QObject>
#include <QAbstractNativeEventFilter>

class JAppWindowEventFilterPrivate;
class QQuickWindow;

class JWT_EXPORT JAppWindowEventFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
    Q_PROPERTY(bool resizable READ resizable WRITE setResizable NOTIFY resizableChanged)
public:
    explicit JAppWindowEventFilter(QQuickWindow *window, QObject *parent = 0);
    ~JAppWindowEventFilter();

    bool resizable() const;

signals:
    void resizableChanged(bool value);

public slots:
    void setResizable(bool value);

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(JAppWindowEventFilter)
    J_DECLARE_PRIVATE(JAppWindowEventFilter)
};

#endif // JAPPWINDOWEVENTFILTER_H
