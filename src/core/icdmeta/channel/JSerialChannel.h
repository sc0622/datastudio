#ifndef JSERIALCHANNEL_H
#define JSERIALCHANNEL_H

#include "JSuperChannel.h"

namespace icdmeta {

J_TYPEDEF_QT_SHAREDPTR(JSerialChannel)

class JSerialChannelPrivate;

class ICDMETA_EXPORT JSerialChannel : public JSuperChannel
{
    Q_OBJECT
    Q_PROPERTY(QStringList portNames READ portNames NOTIFY portNamesChanged)
    Q_PROPERTY(QStringList baudRates READ baudRates NOTIFY baudRatesChanged)
    Q_PROPERTY(QStringList dataBitss READ dataBitss NOTIFY dataBitssChanged)
    Q_PROPERTY(QStringList stopBitss READ stopBitss NOTIFY stopBitssChanged)
    Q_PROPERTY(QStringList parities READ parities NOTIFY paritiesChanged)
    Q_PROPERTY(QString portName READ portName WRITE setPortName NOTIFY portNameChanged)
    Q_PROPERTY(int baudRate READ baudRate WRITE setBaudRate NOTIFY baudRateChanged)
    Q_PROPERTY(DataBits dataBits READ dataBits WRITE setDataBits NOTIFY dataBitsChanged)
    Q_PROPERTY(StopBits stopBits READ stopBits WRITE setStopBits NOTIFY stopBitsChanged)
    Q_PROPERTY(Parity parity READ parity WRITE setParity NOTIFY parityChanged)
public:
    /**
     * @brief The DataBits enum
     */
    enum DataBits {
        Data5 = 5,
        Data6 = 6,
        Data7 = 7,
        Data8 = 8,
        UnknownDataBits = -1
    };
    Q_ENUM(DataBits)

    /**
     * @brief The StopBits enum
     */
    enum StopBits {
        OneStop = 1,
        OneAndHalfStop = 3,
        TwoStop = 2,
        UnknownStopBits = -1
    };
    Q_ENUM(StopBits)

    /**
     * @brief The Parity enum
     */
    enum Parity {
        NoParity = 0,
        EvenParity = 2,
        OddParity = 3,
        SpaceParity = 4,
        MarkParity = 5,
        UnknownParity = -1
    };
    Q_ENUM(Parity)

    explicit JSerialChannel(const QString &identity, QObject *parent = nullptr);
    ~JSerialChannel();

    /**
     * @brief registerQmlType
     */
    static void registerQmlType();

    /**
     * @brief channelType
     * @return
     */
    virtual ChannelType channelType() const;

    /**
     * @brief portNames
     * @return
     */
    QStringList portNames() const;

    /**
     * @brief baudRates
     * @return
     */
    QStringList baudRates() const;

    /**
     * @brief dataBitss
     * @return
     */
    QStringList dataBitss() const;

    /**
     * @brief stopBitss
     * @return
     */
    QStringList stopBitss() const;

    /**
     * @brief parities
     * @return
     */
    QStringList parities() const;

    /**
     * @brief portName
     * @return
     */
    QString portName() const;

    /**
     * @brief baudRate
     * @return
     */
    int baudRate() const;

    /**
     * @brief dataBits
     * @return
     */
    DataBits dataBits() const;

    /**
     * @brief stopBits
     * @return
     */
    StopBits stopBits() const;

    /**
     * @brief parity
     * @return
     */
    Parity parity() const;

signals:
    void portNamesChanged();
    void baudRatesChanged();
    void dataBitssChanged();
    void stopBitssChanged();
    void paritiesChanged();
    void portNameChanged(const QString &portName);
    void baudRateChanged(int baudRate);
    void dataBitsChanged(DataBits dataBits);
    void stopBitsChanged(StopBits stopBits);
    int parityChanged(Parity parity);

public slots:
    void setPortName(const QString &portName);
    void setBaudRate(int baudRate);
    void setDataBits(DataBits dataBits);
    void setStopBits(StopBits stopBits);
    void setParity(Parity parity);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int deep = -1) override;

private:
    Q_DISABLE_COPY(JSerialChannel)
    J_DECLARE_PRIVATE(JSerialChannel)
};

}

#endif // JSERIALCHANNEL_H
