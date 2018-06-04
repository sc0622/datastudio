#ifndef JPROTOCOL_H
#define JPROTOCOL_H

#include  "../icdmeta_global.h"
#include "icdcore/icdcore_global.h"

class QJSValue;

namespace icdmeta {

J_TYPEDEF_QT_SHAREDPTR(JProtocol)

class JIcdTable;
class JProtocolPrivate;

class ICDMETA_EXPORT JProtocol : public QObject, public Json::Serializable
{
    Q_OBJECT
    Q_PROPERTY(QString identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(QString domain READ domain NOTIFY domainChanged)
    Q_PROPERTY(QString tableName READ tableName NOTIFY tableNameChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
    Q_PROPERTY(QString queryPath READ queryPath NOTIFY queryPathChanged)
    Q_PROPERTY(QString channelId READ channelId NOTIFY channelIdChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(bool checked READ isChecked NOTIFY checkedChanged)
    Q_PROPERTY(icdmeta::JIcdTable* table READ table NOTIFY tableChanged)
public:
    enum LoadError {
        LoadNoError,
        LoadFilePathError,
        LoadQueryPathError,
        LoadParseError
    };
    Q_ENUM(LoadError)

    explicit JProtocol(const QString &identity, QObject *parent = 0);
    ~JProtocol();

    static void registerQmlType();

    QString identity() const;
    QString domain() const;
    QString tableName() const;
    QString filePath() const;
    QString queryPath() const;
    QString channelId() const;
    bool isValid() const;
    bool isChecked() const;
    icdmeta::JIcdTable *table() const;

signals:
    void identityChanged(const QString &value);
    void domainChanged(const QString &value);
    void tableNameChanged(const QString &value);
    void filePathChanged(const QString &value);
    void queryPathChanged(const QString &value);
    void channelIdChanged(const QString &value);
    void validChanged(bool value);
    void checkedChanged(bool checked);
    void tableChanged(icdmeta::JIcdTable *table);
    void loaded(bool success, int error);

public slots:
    void setIdentity(const QString &value);
    void setDomain(const QString &value);
    void setTableName(const QString &value);
    void setFilePath(const QString &value);
    void setQueryPath(const QString &value);
    void setChannelId(const QString &value);
    bool load(QJSValue callback = QJSValue());
    void unload();
    void resetBuffer();
    void cancelWatcher();

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int /*deep*/ = -1) override;

protected:
    void customEvent(QEvent *event);

private:
    Q_DISABLE_COPY(JProtocol)
    J_DECLARE_PRIVATE(JProtocol)
};

}

#endif // JPROTOCOL_H
