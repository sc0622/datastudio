#ifndef JSUPERCHANNEL_H
#define JSUPERCHANNEL_H

#include "../icdmeta_global.h"
#include "icdcore/icdcore_global.h"
#include <QObject>

namespace Icd {
class Channel;
typedef std::shared_ptr<Channel> ChannelPtr;
}

namespace icdmeta {

J_TYPEDEF_QT_SHAREDPTR(JSuperChannel)

class JSuperChannelPrivate;

class ICDMETA_EXPORT JSuperChannel : public QObject, public Json::Serializable
{
    Q_OBJECT
    Q_PROPERTY(QStringList channelTypes READ channelTypes NOTIFY channelTypesChanged)
    Q_PROPERTY(ChannelType channelType READ channelType NOTIFY channelTypeChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString mark READ mark WRITE setMark NOTIFY markChanged)
    Q_PROPERTY(QString desc READ desc WRITE setDesc NOTIFY descChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(bool isOpen READ isOpen NOTIFY isOpenChanged)
public:
    enum ChannelType {
        ChannelInvalid = -1,
        ChannelSerial,
        ChannelUdp,
        ChannelFile
    };
    Q_ENUM(ChannelType)

    explicit JSuperChannel(QObject *parent = nullptr);
    virtual ~JSuperChannel() override;

    static void registerQmlType();

    Icd::ChannelPtr nativeChannel() const;
    QStringList channelTypes() const;
    virtual ChannelType channelType() const;
    QString name() const;
    QString mark() const;
    QString desc() const;
    bool isValid() const;
    bool isOpen() const;

    Q_INVOKABLE icdmeta::JSuperChannel *create(ChannelType channelType);
    Q_INVOKABLE void release(QObject *object);

protected:
    void setChannel(const Icd::ChannelPtr &nativeChannel);

signals:
    void channelTypesChanged();
    void channelTypeChanged();
    void nameChanged(const QString &name);
    void markChanged(const QString &mark);
    void descChanged(const QString &desc);
    void checkedChanged(bool checked);
    void validChanged(bool valid);
    void isOpenChanged(bool open);

public slots:
    void setName(const QString &name);
    void setMark(const QString &mark);
    void setDesc(const QString &desc);

    bool open();
    void close();

    QByteArray readData(int size);
    int writeData(const QByteArray &data);

    // Serializable interface
public:
    virtual Json::Value save() const override;
    virtual bool restore(const Json::Value &json, int /*deep*/ = -1) override;

private:
    Q_DISABLE_COPY(JSuperChannel)
    J_DECLARE_PRIVATE(JSuperChannel)
};

}

#endif // JSUPERCHANNEL_H
