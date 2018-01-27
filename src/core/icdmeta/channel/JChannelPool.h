#ifndef JCHANNELPOOL_H
#define JCHANNELPOOL_H

#include <QObject>
#include "../icdmeta_global.h"

namespace icdmeta {

J_TYPEDEF_QT_SHAREDPTR(JChannel)

class JChannelPoolPrivate;

class ICDMETA_EXPORT JChannelPool : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<icdmeta::JChannel> channels READ channels NOTIFY channelsChanged)
public:
    explicit JChannelPool(QObject *parent = 0);
    ~JChannelPool();

    static void registerQmlType();

    QQmlListProperty<icdmeta::JChannel> channels();

    Q_INVOKABLE bool loadConfig(const QString &filePath, const QString &nodePath);
    Q_INVOKABLE bool saveConfig(const QString &filePath, const QString &nodePath);

signals:
    void channelsChanged();

public slots:
    icdmeta::JChannel *identityOf(const QString &identity) const;
    QList<icdmeta::JChannel *> identityOf(const QString &identity, bool fuzzy) const;
    icdmeta::JChannel *domainOf(const QString &domain) const;
    void reset();

private:
    Q_DISABLE_COPY(JChannelPool)
    J_DECLARE_PRIVATE(JChannelPool)
    J_DECLARE_SINGLE_INSTANCE(JChannelPool)

};

}

#endif // JCHANNELPOOL_H
