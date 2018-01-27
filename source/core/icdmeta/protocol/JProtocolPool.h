#ifndef JPROTOCOLPOOL_H
#define JPROTOCOLPOOL_H

#include "../icdmeta_global.h"

namespace icdmeta {

J_TYPEDEF_QT_SHAREDPTR(JProtocolPool)

class JProtocolPoolPrivate;
class JProtocol;

class ICDMETA_EXPORT JProtocolPool : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<icdmeta::JProtocol> protocols READ protocols NOTIFY protocolsChanged)
public:
    static void registerQmlType();

    QQmlListProperty<icdmeta::JProtocol> protocols();

    Q_INVOKABLE bool loadConfig(const QString &filePath, const QString &nodePath);
    Q_INVOKABLE bool saveConfig(const QString &filePath, const QString &nodePath);

signals:
    void protocolsChanged();

public slots:
    icdmeta::JProtocol *identityOf(const QString &identity) const;
    icdmeta::JProtocol *tableIdOf(const QString &identity) const;
    void reset();

private:
    explicit JProtocolPool(QObject *parent = nullptr);
    ~JProtocolPool();

private:
    Q_DISABLE_COPY(JProtocolPool)
    J_DECLARE_PRIVATE(JProtocolPool)
    J_DECLARE_SINGLE_INSTANCE(JProtocolPool)
};

}

#endif // JPROTOCOLPOOL_H
