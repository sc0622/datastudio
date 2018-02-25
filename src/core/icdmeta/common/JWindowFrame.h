#ifndef JWINDOWFRAME_H
#define JWINDOWFRAME_H

#include "./../icdmeta_global.h"
#include <QObject>
#include <QAbstractNativeEventFilter>

class QQuickWindow;

namespace icdmeta {

class JWindowFramePrivate;

class ICDMETA_EXPORT JWindowFrame
        : public QObject
        , public QAbstractNativeEventFilter
{
    Q_OBJECT
    Q_PROPERTY(bool resizable READ resizable WRITE setResizable NOTIFY resizableChanged)
public:
    explicit JWindowFrame(QQuickWindow *window, QObject *parent = nullptr);
    ~JWindowFrame();

    bool resizable() const;

signals:
    void resizableChanged(bool value);

public slots:
    void setResizable(bool value);

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(JWindowFrame)
    J_DECLARE_PRIVATE(JWindowFrame)
};

}

#endif // JWINDOWFRAME_H
