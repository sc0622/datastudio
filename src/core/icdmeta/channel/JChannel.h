#ifndef JCHANNEL_H
#define JCHANNEL_H

#include "../icdmeta_global.h"
#include "icdcore/icdcore_global.h"

namespace Icd {
template<typename T> class std::shared_ptr;
class Channel;
typedef std::shared_ptr<Channel> ChannelPtr;
}

namespace icdmeta {

J_TYPEDEF_QT_SHAREDPTR(JChannel)

class JChannelPrivate;
class JSuperChannel;

class ICDMETA_EXPORT JChannel : public QObject, public Json::Serializable
{
    Q_OBJECT
    Q_PROPERTY(QString identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(QString domain READ domain NOTIFY domainChanged)
    Q_PROPERTY(icdmeta::JSuperChannel* channel READ channel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(bool isOpen READ isOpen NOTIFY isOpenChanged)
public:
    explicit JChannel(const QString &identity, QObject *parent = nullptr);
    ~JChannel();

    static void registerQmlType();

    QString identity() const;
    QString domain() const;
    icdmeta::JSuperChannel *channel() const;
    Icd::ChannelPtr nativeChannel() const;
    bool isChecked() const;
    bool isValid() const;
    bool isOpen() const;

signals:
    void identityChanged();
    void domainChanged();
    void channelChanged(icdmeta::JSuperChannel *channel);
    void checkedChanged(bool active);
    void validChanged(bool valid);
    void isOpenChanged(bool open);

public slots:
    void setChannel(icdmeta::JSuperChannel *channel);
    void setChecked(bool checked);
    QByteArray readData(int size) const;
    int writeData(const QByteArray &data);
    bool open();
    void close();

    bool saveConfig(const QString &filePath, const QString &pathPrefix = QString());

    // Serializable interface
public:
    Json::Value save() const;
    bool restore(const Json::Value &json, int /*deep*/ = -1);

private:
    Q_DISABLE_COPY(JChannel)
    J_DECLARE_PRIVATE(JChannel)
};

}

#endif // JCHANNEL_H
