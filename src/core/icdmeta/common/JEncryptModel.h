#ifndef JENCRYPTMODEL_H
#define JENCRYPTMODEL_H

#include "../icdmeta_global.h"

namespace icdmeta {

class JEncryptModelPrivate;

class ICDMETA_EXPORT JEncryptModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cpuId READ cpuId NOTIFY cpuIdChanged)
    Q_PROPERTY(QString hdSeries READ hdSeries NOTIFY hdSeriesChanged)
    Q_PROPERTY(QString macAddress READ macAddress NOTIFY macAddressChanged)
    Q_PROPERTY(QString serialNumber READ serialNumber WRITE setSerialNumber NOTIFY serialNumberChanged)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
public:
    explicit JEncryptModel(QObject *parent = 0);
    ~JEncryptModel();

    static void registerQmlType();

    QString cpuId() const;
    QString hdSeries() const;
    QString macAddress() const;
    QString serialNumber() const;
    bool isActive() const;

    Q_INVOKABLE bool activate();
    Q_INVOKABLE bool deactivate();

signals:
    void cpuIdChanged();
    void hdSeriesChanged();
    void macAddressChanged();
    void serialNumberChanged(const QString &value);
    void activeChanged(bool value);

public slots:
    void setSerialNumber(const QString &value);
    void setActive(bool value);
    void reset();

private:
    Q_DISABLE_COPY(JEncryptModel)
    J_DECLARE_PRIVATE(JEncryptModel)
    J_DECLARE_SINGLE_INSTANCE(JEncryptModel)
};

}

#endif // JENCRYPTMODEL_H
